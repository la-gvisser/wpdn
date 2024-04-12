// =============================================================================
//	PeekTaggedFile.cpp
// =============================================================================
//	Copyright (c) 2003-2008 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include <sys/stat.h>
#include <limits.h>
#include "CatchHR.h"
#include "XmlUtil.h"
#include "PeekFile.h"
#include "TimeStamp.h"
#include "OutputFileStream.h"
#include "PeekTaggedFile.h"
#include "AlignUtil.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

using namespace TaggedFile;

enum Format
{
	kFormat_Milliseconds,
	kFormat_Microseconds,
	kFormat_Nanoseconds,
	kFormat_Seconds
};

const UInt64	kTimeStamp_Invalid = 0xFFFFFFFFFFFFFFFFull;	// ULLONG_MAX


// ----------------------------------------------------------------------------
//		ISO8601StringToTimeStamp
// ----------------------------------------------------------------------------

HRESULT
ISO8601StringToTimeStamp(
	LPCTSTR	lpszStr,
	UInt64*	ts )
{
	// Sanity checks.
	if ( lpszStr == NULL ) return E_POINTER;
	if ( ts == NULL ) return E_POINTER;

	// Initialize OUT params.
	*ts = 0;

	// ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssssssssZ
	int	year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	int	n = _stscanf_s( lpszStr, _T("%d-%d-%dT%d:%d:%d"),
				&year, &month, &day, &hour, &minute, &second );
	if ( n != 6 ) return E_FAIL;

	// Convert to filetime.
	SYSTEMTIME	st;
	memset( &st, 0, sizeof(st) );
	st.wYear = (WORD) year;
	st.wMonth = (WORD) month;
	st.wDay = (WORD) day;
	st.wHour = (WORD) hour;
	st.wMinute = (WORD) minute;
	st.wSecond = (WORD) second;
	st.wMilliseconds = 0;
	if ( !::SystemTimeToFileTime( &st, (FILETIME*) ts ) )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	// Convert to PEEK timestamp format.
	*ts = TimeConv::FileToPeek( *ts );

	// Look for subseconds.
	LPCTSTR	p = lpszStr;
	while ( *p )
	{
		if ( *p == _T('.') )
		{
			p++;	// Skip the period.

			TCHAR*	pp = NULL;
			UInt32	nNanoseconds = _tcstoul( p, &pp, 10 );

			if ( pp > p )
			{
				// Convert to nanoseconds.
				size_t	len = pp - p;	// No need for divide by sizeof(TCHAR).
				if ( len > 9 )
				{
					for ( size_t i = (len - 9); i != 0; i-- )
					{
						nNanoseconds /= 10;
					}
				}
				else if ( len < 9 )
				{
					for ( size_t i = (9 - len); i != 0; i-- )
					{
						nNanoseconds *= 10;
					}
				}

				*ts += nNanoseconds;	// Add the nanoseconds.
			}

			break;
		}

		p++;
	}

	// TODO: look for 'Z' and convert from local time if not found.

	return S_OK;
}


// ----------------------------------------------------------------------------
//		TimeStampToISO8601String
// ----------------------------------------------------------------------------

HRESULT
TimeStampToISO8601String(
	UInt64	ts,
	Format  fmt,
	LPTSTR  lpszStr,
	int     cchStr )
{
	// Sanity checks.
	if ( lpszStr == NULL ) return E_POINTER;

	// Clear the string.
	lpszStr[0] = 0;

	// Get the time in system time format.
	UInt64		ft = TimeConv::PeekToFile( ts );
	SYSTEMTIME	st;
	if ( ::FileTimeToSystemTime( (FILETIME*)&ft, &st ) == 0 )
	{
		DWORD dwResult = ::GetLastError();
		return HRESULT_FROM_WIN32(dwResult);
	}

	switch ( fmt )
	{
		case kFormat_Milliseconds:
		{
			UInt32	nMilliseconds = (UInt32)((ts % RESOLUTION_PER_SECOND) / 1000000);

			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf_s( lpszStr, cchStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%3.3uZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nMilliseconds );
		}
		break;

		case kFormat_Microseconds:
		{
			UInt32	nMicroseconds = (UInt32)((ts % RESOLUTION_PER_SECOND) / 1000);

			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.ssssssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf_s( lpszStr, cchStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6uZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nMicroseconds );
		}
		break;

		case kFormat_Nanoseconds:
		{
			UInt32	nNanoseconds = (UInt32)(ts % RESOLUTION_PER_SECOND);

			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssssssssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf_s( lpszStr, cchStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9uZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, nNanoseconds );
		}
		break;

		case kFormat_Seconds:
		{
			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			_stprintf_s( lpszStr, cchStr, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		}
		break;
	}

	return S_OK;
}


// =============================================================================
//	CCaptureFileWriter
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFileWriter::CCaptureFileWriter(
	LPCTSTR		lpszFile,
	bool		bCompressed ) :
#ifdef OPT_USE_ZLIB
	m_hCompressedFile( NULL ),
#endif
#ifdef OPT_OUTPUTFILESTREAM
	m_pOutputFileStream( NULL ),
#else
	m_hUncompressedFile( INVALID_HANDLE_VALUE ),
#endif
	m_nResult( 0 )
{
	if ( lpszFile != NULL )
	{
		Open( lpszFile, bCompressed );
	}
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFileWriter::~CCaptureFileWriter()
{
	Close();
}


// -----------------------------------------------------------------------------
//		Open
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::Open(
	LPCTSTR	lpszFile,
	bool	bCompressed )
{
#ifndef OPT_USE_ZLIB
	bCompressed;	// UNUSED
#endif

	if ( IsOpen() )
	{
		return false;
	}

	m_strFileName = lpszFile;

#ifdef OPT_USE_ZLIB
	if ( bCompressed )
	{
		m_hCompressedFile = gzopen( CT2A(lpszFile), "wb" );
	}
	else
#endif
	{
#ifdef OPT_OUTPUTFILESTREAM
		m_pOutputFileStream = new COutputFileStream( lpszFile, 64*1024 );	// 64K is a good default
#else
		m_hUncompressedFile = ::CreateFile( lpszFile, 
			GENERIC_WRITE, 0, NULL,	CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
#endif // OPT_OUTPUTFILESTREAM
	}

	m_nResult = ::GetLastError();

	return IsOpen();
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::Close()
{
	if ( !IsOpen() )
	{
		return false;
	}

	bool bRet;
	
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		bRet = (gzclose( m_hCompressedFile ) == 0);
		m_nResult = ::GetLastError();
		m_hCompressedFile = NULL;
	}
	else
#endif
	{
#ifdef OPT_OUTPUTFILESTREAM
		bRet = false;
		if ( m_pOutputFileStream != NULL )
		{
			m_pOutputFileStream->Flush( true );
			m_nResult = ::GetLastError();
			delete m_pOutputFileStream;
			m_pOutputFileStream = NULL;
			bRet = true;
		}
#else
		bRet = (::CloseHandle( m_hUncompressedFile ) != FALSE);
		m_nResult = ::GetLastError();
		m_hUncompressedFile = INVALID_HANDLE_VALUE;
#endif // #ifdef OPT_OUTPUTFILESTREAM
	}

	m_strFileName.Empty();

	return bRet;
}


// -----------------------------------------------------------------------------
//		Delete
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::Delete()
{
	// Save the filename (Close() resets it).
	const CString	strFileName = m_strFileName;

	// Close the file - can't delete with an open handle.
	Close();

	// Delete the file.
	return (::DeleteFile( strFileName ) != FALSE);
}


// -----------------------------------------------------------------------------
//		IsOpen
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::IsOpen() const
{
	return 
#ifdef OPT_USE_ZLIB
		(m_hCompressedFile != NULL) || 
#endif // #ifdef OPT_USE_ZLIB
#ifdef OPT_OUTPUTFILESTREAM
		(m_pOutputFileStream && m_pOutputFileStream->IsOpen())
#else
		(m_hUncompressedFile != INVALID_HANDLE_VALUE)
#endif // #ifdef OPT_OUTPUTFILESTREAM
		;
}


// -----------------------------------------------------------------------------
//		GetSize
// -----------------------------------------------------------------------------

UInt32
CCaptureFileWriter::GetSize() const
{
	struct _stat theStatus;
	_tstat( m_strFileName, &theStatus );
	return (UInt32) theStatus.st_size;
}


// -----------------------------------------------------------------------------
//		GetFileName
// -----------------------------------------------------------------------------

CString
CCaptureFileWriter::GetFileName() const
{
	return m_strFileName;
}


// -----------------------------------------------------------------------------
//		GetFileHandle
// -----------------------------------------------------------------------------

HANDLE
CCaptureFileWriter::GetFileHandle() const
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		// TODO: get the real file handle somehow
		return NULL;
	}
#endif /* OPT_USE_ZLIB */

#ifdef OPT_OUTPUTFILESTREAM
	if ( (m_pOutputFileStream != NULL) && m_pOutputFileStream->IsOpen() )
	{
		return m_pOutputFileStream->GetFileRef();
	}
#else
	if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
	{
		return m_hUncompressedFile;
	}
#endif /* OPT_OUTPUTFILESTREAM */

	return NULL;
}


// -----------------------------------------------------------------------------
//		WriteHeader
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::WriteHeader(
	UInt32	nType,
	UInt32	nLength,
	UInt32	nFlags )
{
	BLOCK_HEADER header;
	header.nType   = nType;
	header.nLength = nLength;
	header.nFlags  = nFlags;

	return WriteHeader( header );
}


// -----------------------------------------------------------------------------
//		WriteHeader
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::WriteHeader(
	const BLOCK_HEADER&	blockHeader )
{
	return WriteBytes( (void*) &blockHeader, sizeof(BLOCK_HEADER) );
}


// -----------------------------------------------------------------------------
//		WriteBytes
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::WriteBytes(
	const void*	pData,
	UInt32		nLength )
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{	
		if ( gzwrite( m_hCompressedFile, (const voidp) pData, nLength ) != (int) nLength )
		{
			m_nResult = GetLastError();
			return false;
		}
	}
	else
#endif
	{
#ifdef OPT_OUTPUTFILESTREAM
		if ( m_pOutputFileStream != NULL )
		{
			if ( !m_pOutputFileStream->WriteBytes( pData, nLength ) )
			{
				m_nResult = ::GetLastError();
				return false;
			}
		}
#else
		DWORD dwWritten;
		if ( !::WriteFile( m_hUncompressedFile, pData, nLength, &dwWritten, NULL ) )
		{
			m_nResult = GetLastError();
			return false;
		}
#endif // #ifdef OPT_OUTPUTFILESTREAM
	}

	return true;
}


// -----------------------------------------------------------------------------
//		GetFilePosition
// -----------------------------------------------------------------------------

DWORD
CCaptureFileWriter::GetFilePosition() const
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		return gztell( m_hCompressedFile );
	}
	else
#endif
	{
#ifdef OPT_OUTPUTFILESTREAM
		if ( m_pOutputFileStream != NULL )
		{
			return (DWORD) m_pOutputFileStream->GetFile().GetPosition();
		}
#else
		if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
		{
			return ::SetFilePointer( m_hUncompressedFile, 0, NULL, FILE_CURRENT );
		}
#endif // #ifdef OPT_OUTPUTFILESTREAM
	}

	ASSERT(0);
	return 0;
}


// -----------------------------------------------------------------------------
//		SetFilePosition
// -----------------------------------------------------------------------------

bool
CCaptureFileWriter::SetFilePosition(
	DWORD	dwPos,
	DWORD	dwPositionFlags ) const
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		return (gzseek( m_hCompressedFile, dwPos, dwPositionFlags ) != -1);
	}
	else
#endif
	{
#ifdef OPT_OUTPUTFILESTREAM
		if ( m_pOutputFileStream != NULL )
		{
			switch ( dwPositionFlags )
			{
				case SEEK_SET:
					return m_pOutputFileStream->GetFile().SetPosition( dwPos, kFromStart );
				case SEEK_END:
					return m_pOutputFileStream->GetFile().SetPosition( dwPos, kFromEnd );
				case SEEK_CUR:
					return m_pOutputFileStream->GetFile().SetPosition( dwPos, kFromCurrent );
				default:
					// WTF?
					break;
			}
		}
#else
		if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
		{
			if ( ::SetFilePointer( m_hUncompressedFile, dwPos, NULL, dwPositionFlags ) != INVALID_SET_FILE_POINTER )
			{
				return true;
			}
		}
#endif // #ifdef OPT_OUTPUTFILESTREAM
	}

	ASSERT(0);
	return false;
}


// -----------------------------------------------------------------------------
//		Flush
// -----------------------------------------------------------------------------

bool 
CCaptureFileWriter::Flush()
{
	m_nResult = 0;

#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		// Probably not necessary...
//		gzflush( m_hCompressedFile, Z_FULL_FLUSH );
	}
	else
#endif
	{
#ifdef OPT_OUTPUTFILESTREAM
		if ( m_pOutputFileStream != NULL )
		{
			if ( !m_pOutputFileStream->Flush( true ) )
			{
				m_nResult = ::GetLastError();
				return false;
			}
		}
#else
		if ( m_hUncompressedFile != NULL )
		{	// do something?
		}
#endif	// #ifdef OPT_OUTPUTFILESTREAM
	}

	return true;
}


// =============================================================================
//	CCaptureFileReader
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFileReader::CCaptureFileReader(
	LPCTSTR	lpszFile,
	bool	bIsCompressed ) :
		m_nResult(0),
		m_nLength(0),
#ifdef OPT_USE_ZLIB
		m_hCompressedFile(NULL),
#endif // #ifdef OPT_USE_ZLIB
		m_hUncompressedFile(INVALID_HANDLE_VALUE),
		m_pMemMappedFile(NULL),
		m_nMemMappedLength(0),
		m_bOwnFile(false),
		m_bTaggedFile(false),
		m_nCurBlockPosition(0),
		m_nCurFilePosition(0)
{
	if ( lpszFile != NULL )
	{
		Open( lpszFile, bIsCompressed );
	}
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFileReader::~CCaptureFileReader()
{
	Close();
}


// -----------------------------------------------------------------------------
//		Open
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::Open(
	LPCTSTR	lpszFile,
	bool	bIsCompressed )
{
	m_bOwnFile = true;

#ifdef OPT_USE_ZLIB
	if ( bIsCompressed )
	{
		m_hCompressedFile = gzopen( CT2A(lpszFile), "rb" );
		if ( m_hCompressedFile == NULL )
		{
			m_nResult = ::GetLastError();
			return false;
		}
		gzseek( m_hCompressedFile, 0, SEEK_END );	// TODO: this doesn't work - can't SEEK_END
		m_nLength = gztell( m_hCompressedFile );
		gzrewind( m_hCompressedFile );
	}
	else
#else
	bIsCompressed;	// unused
#endif
	{
		m_hUncompressedFile = ::CreateFile( lpszFile, GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if ( m_hUncompressedFile == INVALID_HANDLE_VALUE )
		{
			m_nResult = ::GetLastError();
			return false;
		}

		m_nLength = ::GetFileSize( m_hUncompressedFile, NULL );
	}

	// read out the first block type, it should be the version block
	if ( ReadHeader( m_curBlock ) )
	{
		m_bTaggedFile = (m_curBlock.nType == CCaptureFileVersionBlock::GetType());
	}

	return m_bTaggedFile;
}


// -----------------------------------------------------------------------------
//		AttachCompressed
// -----------------------------------------------------------------------------

#ifdef OPT_USE_ZLIB
bool
CCaptureFileReader::AttachCompressed(
	gzFile	hCompressedFile,
	bool	bOwn )
{
	m_hCompressedFile = hCompressedFile;
	m_bOwnFile = bOwn;
	// figure out the size
	gzseek( m_hCompressedFile, 0, SEEK_END );	// TODO: this doesn't work - can't SEEK_END
	m_nLength = gztell( m_hCompressedFile );
	gzrewind( m_hCompressedFile );
	// read out the first block type, it should be the version block
	if ( ReadHeader( m_curBlock ) )
	{
		m_bTaggedFile = m_curBlock.nType == CCaptureFileVersionBlock::GetType();
	}

	return m_bTaggedFile;
}
#endif


// -----------------------------------------------------------------------------
//		AttachFile
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::AttachFile(
	HANDLE	hUncompressedFile,
	bool	bOwn )
{
	m_hUncompressedFile = hUncompressedFile;
	m_bOwnFile = bOwn;
	// figure out the size
	m_nLength = ::GetFileSize( m_hUncompressedFile, NULL );
	// read out the first block type, it should be the version block
	m_bTaggedFile = false;
	if ( ReadHeader( m_curBlock ) )
	{
		m_bTaggedFile = m_curBlock.nType == CCaptureFileVersionBlock::GetType();
	}

	return m_bTaggedFile;
}


// -----------------------------------------------------------------------------
//		AttachMemMap
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::AttachMemMap(
	const UInt8*	pMemMappedFile,
	UInt32			nLength,
	bool			bOwn )
{
	m_pMemMappedFile = pMemMappedFile;
	m_nMemMappedLength = nLength;
	m_bOwnFile = bOwn;
	m_nLength = nLength;
	// read out the first block type, it should be the version block
	if ( ReadHeader( m_curBlock ) )
	{
		m_bTaggedFile = m_curBlock.nType == CCaptureFileVersionBlock::GetType();
	}

	return m_bTaggedFile;
}


// -----------------------------------------------------------------------------
//		OwnFile
// -----------------------------------------------------------------------------

void
CCaptureFileReader::OwnFile(
	bool	bOwn )
{
	m_bOwnFile = bOwn;
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::Close()
{
	if ( m_bOwnFile )
	{
#ifdef OPT_USE_ZLIB
		if ( m_hCompressedFile )
		{
			gzclose( m_hCompressedFile );
			m_hCompressedFile = NULL;
		}
		else 
#endif // #ifdef OPT_USE_ZLIB
		if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( m_hUncompressedFile );
			m_hUncompressedFile = INVALID_HANDLE_VALUE;
		}
		else if ( m_pMemMappedFile )
		{
			m_pMemMappedFile = NULL;
			m_nMemMappedLength = 0;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		IsTaggedFile
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::IsTaggedFile() const
{
	return m_bTaggedFile;
}


// -----------------------------------------------------------------------------
//		IsOpen
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::IsOpen() const
{
	return (
#ifdef OPT_USE_ZLIB
			(m_hCompressedFile != NULL) ||
#endif
		    (m_hUncompressedFile != INVALID_HANDLE_VALUE) ||
			(m_pMemMappedFile != NULL));
}


// -----------------------------------------------------------------------------
//		IsEndOfFile
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::IsEndOfFile() const
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		return (m_nCurFilePosition == (UInt32) -1 || 
		        gzeof(m_hCompressedFile) != 0);
	}
	else 
#endif // #ifdef OPT_USE_ZLIB
	if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
	{
		if ( m_nCurFilePosition == (UInt32) -1 )
		{
			return true;
		}

		DWORD	dwCur = ::SetFilePointer( m_hUncompressedFile, 0, NULL, FILE_CURRENT );
		DWORD	dwLen = ::GetFileSize( m_hUncompressedFile, NULL );
		return (dwCur == dwLen);
	}
	else if ( m_pMemMappedFile )
	{
		return (m_nCurFilePosition == (UInt32) -1) || 
				(m_nCurFilePosition >= m_nMemMappedLength);
	}
	else
	{
		return false;
	}
}


// -----------------------------------------------------------------------------
//		IsEndOfBlock
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::IsEndOfBlock() const
{
	// check if the current block + block length == current file position
	// if the block length is 0 then just check for end of file
	if ( m_curBlock.nLength == 0 )
	{
		return IsEndOfFile();
	}

	return (m_curBlock.nLength + sizeof(BLOCK_HEADER) + m_nCurBlockPosition == m_nCurFilePosition);
}


// -----------------------------------------------------------------------------
//		Rewind
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::Rewind()
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		if ( gzrewind( m_hCompressedFile ) != 0 )
		{
			m_nResult = ::GetLastError();
			return false;
		}
	}
	else 
#endif
	if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
	{
		if ( ::SetFilePointer( m_hUncompressedFile, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
		{
			m_nResult = ::GetLastError();
			return false;
		}
	}
	else if ( m_pMemMappedFile != NULL )
	{
		m_nMemMappedLength = 0;
	}
	
	m_nCurBlockPosition = 0;
	m_nCurFilePosition = 0;

	return true;
}


// -----------------------------------------------------------------------------
//		GetCurBlockPosition
// -----------------------------------------------------------------------------

UInt32
CCaptureFileReader::GetCurBlockPosition() const
{
	return m_nCurBlockPosition;
}


// -----------------------------------------------------------------------------
//		GetCurBlockType
// -----------------------------------------------------------------------------

BlockType
CCaptureFileReader::GetCurBlockType() const
{
	return m_curBlock.nType;
}


// -----------------------------------------------------------------------------
//		GetCurBlockLength
// -----------------------------------------------------------------------------

UInt32
CCaptureFileReader::GetCurBlockLength() const
{
	return m_curBlock.nLength;
}


// -----------------------------------------------------------------------------
//		GetCurBlockFlags
// -----------------------------------------------------------------------------

UInt32
CCaptureFileReader::GetCurBlockFlags() const
{
	return m_curBlock.nFlags;
}


// -----------------------------------------------------------------------------
//		NextBlock
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::NextBlock()
{
	// set the position of the file to the last block's position + the block's length
	// set the position of the block to the same place

	// if the length is 0, set to the end of the file
	if ( m_curBlock.nLength == 0 )
	{
#ifdef OPT_USE_ZLIB
		if ( m_hCompressedFile != NULL )
		{
			gzseek( m_hCompressedFile, 0, SEEK_END );	// TODO: this doesn't work - can't SEEK_END
			m_nCurFilePosition = gzseek( m_hCompressedFile, 0, SEEK_END );
		}
		else 
#endif // #ifdef OPT_USE_ZLIB
		if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
		{
			::SetFilePointer( m_hUncompressedFile, 0, NULL, FILE_END );
			m_nCurFilePosition = ::GetFileSize( m_hUncompressedFile, NULL );
		}
		else if ( m_pMemMappedFile != NULL )
		{
			m_nCurFilePosition = m_nMemMappedLength;
		}

		m_nCurBlockPosition = m_nCurFilePosition;
		memset( &m_curBlock, 0, sizeof(m_curBlock) );
		return false;
	}
		
	UInt32 nToMove = (m_curBlock.nLength) ? m_curBlock.nLength + sizeof(BLOCK_HEADER) : 0;

#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		int nResult = gzseek( m_hCompressedFile, m_nCurBlockPosition + nToMove, SEEK_SET );
		if ( nResult < 0 )
		{
			m_nResult = ::GetLastError();
			return false;
		}
		
		m_nCurFilePosition = gztell( m_hCompressedFile );
	}
	else 
#endif
	if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
	{
		if ( ::SetFilePointer( m_hUncompressedFile,
			m_nCurBlockPosition + nToMove, NULL, FILE_BEGIN ) ==
			INVALID_SET_FILE_POINTER )
		{
			m_nResult = ::GetLastError();
			return false;
		}

		m_nCurFilePosition = m_nCurBlockPosition + nToMove;
	}
	else if ( m_pMemMappedFile != NULL )
	{
		if ( (m_nCurBlockPosition + nToMove) > m_nMemMappedLength )
		{
			return false;
		}

		m_nCurFilePosition = m_nCurBlockPosition + nToMove;
	}

	m_nCurBlockPosition = m_nCurFilePosition;
	return ReadHeader( m_curBlock );
}


// -----------------------------------------------------------------------------
//		ReadHeader
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::ReadHeader(
	BLOCK_HEADER&	blockHeader )
{
	return ReadBytes( &blockHeader, sizeof(blockHeader) );
}


// -----------------------------------------------------------------------------
//		GetCurBlockHeader
// -----------------------------------------------------------------------------

const BLOCK_HEADER&
CCaptureFileReader::GetCurBlockHeader()
{
	return m_curBlock;
}


// -----------------------------------------------------------------------------
//		ReadBytes
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::ReadBytes(
	void*	pData,
	int		nLength )
{
	// if the block's bytes are compressed, but the file isn't compressed
	// we need to do a special read
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile )
	{
		int	len = gzread( m_hCompressedFile, pData, nLength );
		if ( len != nLength )
		{
			m_nResult = ::GetLastError();
			return false;
		}
		
		m_nCurFilePosition = gztell( m_hCompressedFile ); 
	}
	else 
#endif // #ifdef OPT_USE_ZLIB
	if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
	{
		DWORD	dwRead = 0;
		if ( !::ReadFile( m_hUncompressedFile, pData, nLength, &dwRead, NULL ) ||
			 dwRead != (DWORD) nLength )
		{
			m_nResult = ::GetLastError();
			return false;
		}

		m_nCurFilePosition += nLength;
	}
	else if ( m_pMemMappedFile != NULL )
	{
		if ( (m_nCurFilePosition + nLength) > m_nMemMappedLength)
		{
			return false;
		}

		memcpy( pData, m_pMemMappedFile + m_nCurFilePosition, nLength );
		m_nCurFilePosition += nLength;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		FindBlock
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::FindBlock(
	TaggedFile::BlockType	inBlockType )
{
	Rewind();

	if ( !ReadHeader( m_curBlock ) )
	{
		return false;
	}

	bool	bDone = false;
	while ( !bDone )
	{
		if ( GetCurBlockType() == inBlockType )
		{
			// we're here, so set the position to the beginning of this block
			return true;
		}

		bDone = !NextBlock();
	};

	return false;
}


// -----------------------------------------------------------------------------
//		GetFilePosition
// -----------------------------------------------------------------------------

DWORD
CCaptureFileReader::GetFilePosition() const
{
	return m_nCurFilePosition;
}


// -----------------------------------------------------------------------------
//		SetFilePosition
// -----------------------------------------------------------------------------

bool
CCaptureFileReader::SetFilePosition(
	DWORD	dwPos,
	DWORD	positionFlags,
	bool	bReadHeader )
{
#ifdef OPT_USE_ZLIB
	if ( m_hCompressedFile != NULL )
	{
		m_nCurFilePosition = gzseek( m_hCompressedFile, dwPos, positionFlags );
		if ( bReadHeader )
		{
			m_nCurBlockPosition = m_nCurFilePosition;
			return ReadHeader( m_curBlock );
		}
		return true;
	}
	else
#endif
	if ( m_hUncompressedFile != INVALID_HANDLE_VALUE )
	{
		m_nCurFilePosition = ::SetFilePointer( m_hUncompressedFile, dwPos, NULL, positionFlags );
		if ( m_nCurFilePosition != INVALID_SET_FILE_POINTER )
		{
			if ( bReadHeader )
			{
				m_nCurBlockPosition = m_nCurFilePosition;
				return ReadHeader( m_curBlock );
			}
			else
			{
				return true;
			}
		}

		return false;
	}
	else if ( m_pMemMappedFile != NULL )
	{
		switch ( positionFlags )
		{
			case SEEK_END:	// pos is offset from the end of the memory map
				m_nCurFilePosition = m_nMemMappedLength - dwPos;
				break;
			case SEEK_SET:	// pos is offset from the start of the file
				m_nCurFilePosition = dwPos;
				break;
			case FILE_CURRENT:	// pos is offset from the current position
				m_nCurFilePosition += dwPos;
				break;
			default:
				ASSERT(0);
				return false;
				break;
		}

		if ( m_nCurFilePosition <= m_nMemMappedLength )
		{
			if ( bReadHeader )
			{
				m_nCurBlockPosition = m_nCurFilePosition;
				return ReadHeader( m_curBlock );
			}
			else
			{
				return true;
			}
		}

		return false;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		ReadMSXMLDoc
// -----------------------------------------------------------------------------

HRESULT
CCaptureFileReader::ReadMSXMLDoc(
	IXMLDOMDocument**	ppXMLDoc )
{
	if ( ppXMLDoc == NULL ) return E_POINTER;

	HRESULT hr = S_OK;

	try
	{
		CComPtr<IXMLDOMDocument>	spXMLDoc;
		hr = spXMLDoc.CoCreateInstance( __uuidof(DOMDocument) );
		if ( FAILED(hr) ) AtlThrow( hr );
		hr = spXMLDoc->put_async( VARIANT_FALSE );
		if ( FAILED(hr) ) AtlThrow( hr );

		const UInt32	cbBlock = GetCurBlockLength();

		CStreamOnCByteStream	stream;
		ULARGE_INTEGER			cbStream;
		cbStream.QuadPart = cbBlock;
		hr = stream.SetSize( cbStream );
		if ( FAILED(hr) ) AtlThrow( hr );

		if ( !ReadBytes( stream.GetRawDataPtr(), cbBlock ) )
		{
			AtlThrow( E_FAIL );
		}

		// Load the xml.
		CComVariant		varXml;
		V_VT(&varXml) = VT_UNKNOWN;
		V_UNKNOWN(&varXml) = (IStream*) &stream;
		VARIANT_BOOL	bSuccess = VARIANT_FALSE;
		hr = spXMLDoc->load( varXml, &bSuccess );
		if ( FAILED(hr) || (bSuccess != VARIANT_TRUE) )
		{
			TRACE( _T("Cannot read XML stream\n") );
			AtlThrow( E_FAIL );
		}

		hr = spXMLDoc.CopyTo( ppXMLDoc );
	}
	CATCH_HR(hr)

	return hr;
}


// =============================================================================
//	CCaptureFileVersionBlock
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFileVersionBlock::CCaptureFileVersionBlock()
{
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFileVersionBlock::~CCaptureFileVersionBlock()
{
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CCaptureFileVersionBlock::Write(
	CCaptureFileWriter&	theFile,
	DWORD*				outBytesWritten /* = NULL */ )
{
	HRESULT	hr = S_OK;

	try
	{
		CComPtr<IXMLDOMDocument>	spDoc;
		hr = XmlUtil::CreateXmlDocument( &spDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		CComPtr<IXMLDOMElement>	spRoot;
		XmlUtil::AddChildElement( spDoc, spDoc,
			kCaptureFileVersionBlock_Root, &spRoot );

		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileVersionBlock_FileVersion, m_strFileVers );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileVersionBlock_AppVersion, m_strAppVers );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileVersionBlock_ProductVersion, m_strProductVers );

		CStreamOnCByteStream	stream;
		CComPtr<IPersistStream>	spPersist;
		hr = spDoc->QueryInterface( &spPersist );
		if ( FAILED(hr) ) AtlThrow( hr );
		hr = spPersist->Save( &stream, TRUE );
		if ( FAILED(hr) ) AtlThrow( hr );

		// Pad the block out to 2 KB.
		const UInt32	cbPaddedLength = 2048 - sizeof(BLOCK_HEADER);
		if ( stream.GetLength() < cbPaddedLength )
		{
			UInt8	fill[cbPaddedLength];
			memset( fill, 0x20, cbPaddedLength );
			UInt32	cbFill = cbPaddedLength - stream.GetLength();
			stream.Write( fill, cbFill, NULL );
		}

		const UInt32	cbLength = stream.GetLength();
		const UInt32	nFlags = MAKE_BLOCK_FLAGS( Uncompressed, XMLFormat );

		if ( !theFile.WriteHeader( GetType(), cbLength, nFlags ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !theFile.WriteBytes( stream.GetRawDataPtr(), cbLength ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( outBytesWritten != NULL )
		{
			*outBytesWritten = sizeof(BLOCK_HEADER) + cbLength;
		}
	}
	CATCH_HR(hr)

	return (hr == S_OK);
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

bool
CCaptureFileVersionBlock::Read(
	CCaptureFileReader&	theFile )
{
	HRESULT hr = S_OK;

	try
	{
		CComPtr<IXMLDOMDocument>	spDoc;
		hr = theFile.ReadMSXMLDoc( &spDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		// Get the root node.
		CComPtr<IXMLDOMElement>	spRoot;
		hr = spDoc->get_documentElement( &spRoot );
		if ( FAILED(hr) ) AtlThrow( hr );

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileVersionBlock_FileVersion, m_strFileVers ) )
		{
			AtlThrow( E_FAIL );
		}
		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileVersionBlock_AppVersion, m_strAppVers ) )
		{
			AtlThrow( E_FAIL );
		}
		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileVersionBlock_ProductVersion, m_strProductVers ) )
		{
			AtlThrow( E_FAIL );
		}

		hr = S_OK;
	}
	CATCH_HR(hr)

	return (hr == S_OK);
}


// -----------------------------------------------------------------------------
//		SetCurrentVersion
// -----------------------------------------------------------------------------

void
CCaptureFileVersionBlock::SetCurrentVersion( 
	ENV_VERSION	FileVersion, 
	ENV_VERSION	ProductVersion)
{
	m_strAppVers.Format( _T("%u.%u.%u.%u"), 
		FileVersion.MajorVer, FileVersion.MinorVer,
		FileVersion.MajorRev, FileVersion.MinorRev );
	m_strProductVers.Format( _T("%u.%u.%u.%u"), 
		ProductVersion.MajorVer, ProductVersion.MinorVer,
		ProductVersion.MajorRev, ProductVersion.MinorRev );
	m_strFileVers.Format( _T("%u"), kPacketFile_VersionTagged );
}


// -----------------------------------------------------------------------------
//		ConvertVersion
// -----------------------------------------------------------------------------

ENV_VERSION
ConvertVersion(
	const CString&	strVers,
	UInt32*			pOutVers )
{
	ENV_VERSION vers;
	UInt16* ayNums[] = { 
		&vers.MajorVer,
		&vers.MinorVer,
		&vers.MajorRev,
		&vers.MinorRev,
	};

	memset(&vers, 0, sizeof(vers));

	int iDot = 0, iCount = 0;
	const int nMaxCount = 3;	// max version is 4 bytes
	do 
	{
		int iOldDot = iDot;
		iDot = strVers.Find(_T('.'), iDot);
		if (iDot > 0)
		{
			*ayNums[iCount] = (UInt16) _ttol(strVers.Mid( iOldDot, iDot - iOldDot ));
			if (pOutVers)
				((UInt8*) pOutVers)[nMaxCount - iCount] = (UInt8) (*ayNums[iCount]);
			iCount++;
			iDot++;
		}
		else
		{	// convert the rest
			*ayNums[iCount] = (UInt16) _ttol(strVers.Mid( iOldDot, strVers.GetLength() - iOldDot ));
			if (pOutVers)
				((UInt8*) pOutVers)[nMaxCount - iCount] = (UInt8) *ayNums[iCount];
		}
	} while(iDot != -1 && iCount <= nMaxCount);

	return vers;
}


// -----------------------------------------------------------------------------
//		GetAppVersion
// -----------------------------------------------------------------------------

ENV_VERSION
CCaptureFileVersionBlock::GetAppVersion(
	UInt32*	pOutVers ) const
{
	return ConvertVersion( m_strAppVers, pOutVers );
}


// -----------------------------------------------------------------------------
//		GetProductVersion
// -----------------------------------------------------------------------------

ENV_VERSION
CCaptureFileVersionBlock::GetProductVersion(
	UInt32*	pOutVers ) const
{
	return ConvertVersion( m_strProductVers, pOutVers );
}


// -----------------------------------------------------------------------------
//		GetFileVerison
// -----------------------------------------------------------------------------

UInt32
CCaptureFileVersionBlock::GetFileVersion() const
{
	return (UInt32) _ttol( m_strFileVers );
}


// =============================================================================
//	CCaptureFileSessionBlock
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFileSessionBlock::CCaptureFileSessionBlock() :
	m_bIncludeWirelessInfo( false ),
	m_bWroteBlock( false ),
	m_nBlockDataPos( 0 ),
	m_nBlockDataLen( 0 )
{
	// Keep data rates sorted.
	m_ayDataRates.SetKeepSorted( true );
	m_ayDataRates.SetComparator( CLongComparator::GetComparator(), false );
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFileSessionBlock::~CCaptureFileSessionBlock()
{
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void 
CCaptureFileSessionBlock::Reset()
{
	m_bWroteBlock = false;
	m_nBlockDataPos = 0;	
	m_nBlockDataLen = 0;
}

// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CCaptureFileSessionBlock::Write(
	CCaptureFileWriter&	theFile,
	DWORD*				outBytesWritten )
{
	HRESULT	hr = S_OK;

	try
	{
		CComPtr<IXMLDOMDocument>	spDoc;
		hr = XmlUtil::CreateXmlDocument( &spDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		CComPtr<IXMLDOMElement>	spRoot;
		XmlUtil::AddChildElement( spDoc, spDoc,
			kCaptureFileSessionBlock_Root, &spRoot );

		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_RawTime, m_strTimeCount );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_Time, m_strTime );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_MediaType, m_strMediaType );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_MediaSubType, m_strMediaSubType );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_TimeZoneBias, m_strTimezoneBias );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_LinkSpeed, m_strLinkSpeed );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileSessionBlock_PacketCount, m_strPacketCount );

		if ( !m_strComment.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_Comment, m_strComment );
		}

		if ( !m_strSessionStartTime.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_SessionStartTime, m_strSessionStartTime );
		}

		if ( !m_strSessionEndTime.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_SessionEndTime, m_strSessionEndTime );
		}

		if ( !m_strAdapterName.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_AdapterName, m_strAdapterName );
		}

		if ( !m_strAdapterAddr.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_AdapterAddr, m_strAdapterAddr );
		}

		if ( !m_strCaptureName.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_CaptureName, m_strCaptureName );
		}

		if ( !m_strCaptureID.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_CaptureID, m_strCaptureID );
		}

		if ( !m_strOwner.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_Owner, m_strOwner );
		}

		if ( !m_strFileIndex.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_FileIndex, m_strFileIndex );
		}

		if ( !m_strHost.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_Host, m_strHost );
		}

		if ( !m_strEngineName.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_EngineName, m_strEngineName );
		}

		if ( !m_strEngineAddr.IsEmpty() )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_EngineAddr, m_strEngineAddr );
		}

		if ( m_bIncludeWirelessInfo )
		{
			XmlUtil::AddChildElement( spDoc, spRoot,
				kCaptureFileSessionBlock_MediaDomain, m_strDomain );

			if ( m_ayDataRates.GetCount() )
			{
				CComPtr<IXMLDOMElement>	spDataRates;
				XmlUtil::AddChildElement( spDoc, spRoot,
					kCaptureFileSessionBlock_DataRates, &spDataRates );
				for (UInt32 n = 0; n < m_ayDataRates.GetCount(); n++)
				{
					XmlUtil::AddChildElement( spDoc, spDataRates,
						kCaptureFileSessionBlock_Rate, m_ayDataRates[n] );
				}
			}
		}

		if ( m_channels.GetCount() > 0 )
		{
			switch ( m_channels.GetChannelType() )
			{
				case CChannelManager::kWirelessChannels:
				{
					CComPtr<IXMLDOMElement>	spChannelList;
					XmlUtil::AddChildElement( spDoc, spRoot,
						kCaptureFileSessionBlock_ChannelList, &spChannelList );

					SWirelessChannel*	pChannels;
					SInt32				nCount = 0;
					m_channels.GetChannels( (void**) &pChannels, &nCount );
					for ( SInt32 n = 0; n < nCount; n++ )
					{	
						// AiroPeek 5.1.x can read these files, but won't see
						// the channel info properly we can fix by adding
						// <Channel>n</Channel> elements, or ignore since the
						// app just has to discover the channels for the
						// signal/channel stats windows.
						CComPtr<IXMLDOMElement>	spChannel;
						XmlUtil::AddChildElement( spDoc, spChannelList,
							kCaptureFileSessionBlock_Channel, &spChannel );

						// Add number, frequency, band to channel.
						XmlUtil::AddChildElement( spDoc, spChannel,
							kCaptureFileSessionBlock_Channel_Number, pChannels[n].Channel );
						XmlUtil::AddChildElement( spDoc, spChannel,
							kCaptureFileSessionBlock_Channel_Freq, pChannels[n].Frequency );
						XmlUtil::AddChildElement( spDoc, spChannel,
							kCaptureFileSessionBlock_Channel_Band, pChannels[n].Band );
					}
				}
				break;

				case CChannelManager::kEnumChannels:
				{
					CComPtr<IXMLDOMElement>	spChannelList;
					XmlUtil::AddChildElement( spDoc, spRoot,
						kCaptureFileSessionBlock_ChannelList, &spChannelList );

					UInt32*	pChannels;
					SInt32	nCount = 0;
					m_channels.GetChannels( (void**) &pChannels, &nCount );
					for ( SInt32 n = 0; n < nCount; n++ )
					{
						XmlUtil::AddChildElement( spDoc, spChannelList,
							kCaptureFileSessionBlock_Channel, pChannels[n] );
					}
				}
				break;

				default:
				{
					ASSERT( 0 );
				}
				break;
			}
		}

		CStreamOnCByteStream	stream;
		CComPtr<IPersistStream>	spPersist;
		hr = spDoc->QueryInterface( &spPersist );
		if ( FAILED(hr) ) AtlThrow( hr );
		hr = spPersist->Save( &stream, TRUE );
		if ( FAILED(hr) ) AtlThrow( hr );

		ULONG cbStream = stream.GetLength();
		{
			size_t cb;
			if (!m_bWroteBlock)
			{
				// Pad the block out to next 2 KB (first write only)
				// Safest for now to always add a block of at least 2KB...
				cb = AlignUtil::AlignUp( cbStream, 2048 ) + 2048 - cbStream;
			}
			else
			{	
				// fill the rest of the stream with padding, just in case the doc got smaller
				ASSERT(m_nBlockDataLen >= cbStream);
				if (m_nBlockDataLen < cbStream)
					return false;		// TODO: move the block?
				cb = m_nBlockDataLen - cbStream;
			}

			// we know the amount to pad, so do it
			CAtlArray<UInt8> fill;
			fill.SetCount(cb);
			memset( fill.GetData(), 0x20, cb );
			stream.Write( fill.GetData(), (ULONG) cb, NULL );
		}

		const UInt32	cbLength = stream.GetLength();
		const UInt32	nFlags = MAKE_BLOCK_FLAGS( Uncompressed, XMLFormat );

		bool bRestorePos = false;

		if (!m_bWroteBlock)
		{
			if ( !theFile.WriteHeader( GetType(), cbLength, nFlags ) )
			{
				AtlThrow( E_FAIL );
			}

			m_nBlockDataLen = cbLength;
		}

		// flush the file first so that we get an accurate file position
		theFile.Flush();
		DWORD dwPos = theFile.GetFilePosition();

		if (!m_bWroteBlock)
		{
			m_nBlockDataPos = dwPos;
		}
		else
		{
			ASSERT( cbLength == m_nBlockDataLen );

			// validate the block length
			if (cbLength > m_nBlockDataLen)
			{
				return false;	// TODO: move the block?
			}

			// seek and restore
			bRestorePos = true;
			theFile.SetFilePosition( m_nBlockDataPos, SEEK_SET );
		}

		if ( !theFile.WriteBytes( stream.GetRawDataPtr(), cbLength ) )
		{
			AtlThrow( E_FAIL );
		}

		m_bWroteBlock = true;

		if (bRestorePos)
		{
			// flush again so that the actual file gets updated
			theFile.Flush();
			theFile.SetFilePosition( dwPos, SEEK_SET );
		}

		if ( outBytesWritten != NULL )
		{
			*outBytesWritten = sizeof(BLOCK_HEADER) + cbLength;
		}
	}
	CATCH_HR(hr)

	return (hr == S_OK);
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

bool
CCaptureFileSessionBlock::Read(
	CCaptureFileReader&	theFile )
{
	HRESULT hr = S_OK;

	try
	{
		CComPtr<IXMLDOMDocument>	spDoc;
		hr = theFile.ReadMSXMLDoc( &spDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		// Get the root node.
		CComPtr<IXMLDOMElement>	spRoot;
		hr = spDoc->get_documentElement( &spRoot );
		if ( FAILED(hr) ) AtlThrow( hr );

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_RawTime, m_strTimeCount ) )
		{
			m_strTimeCount.Empty();
//			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_Time, m_strTime ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_MediaType, m_strMediaType ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_MediaSubType, m_strMediaSubType ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_TimeZoneBias, m_strTimezoneBias ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_LinkSpeed, m_strLinkSpeed ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_PacketCount, m_strPacketCount ) )
		{
			m_strPacketCount.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_Comment, m_strComment ) )
		{
			m_strComment.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_SessionStartTime, m_strSessionStartTime ) )
		{
			m_strSessionStartTime.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_SessionEndTime, m_strSessionEndTime ) )
		{
			m_strSessionEndTime.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_AdapterName, m_strAdapterName ) )
		{
			m_strAdapterName.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_AdapterAddr, m_strAdapterAddr ) )
		{
			m_strAdapterAddr.Empty();
		}
		
		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_CaptureName, m_strCaptureName ) )
		{
			m_strCaptureName.Empty();
		}
		
		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_CaptureID, m_strCaptureID ) )
		{
			m_strCaptureID.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_Owner, m_strOwner ) )
		{
			m_strOwner.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_FileIndex, m_strFileIndex ) )
		{
			m_strFileIndex.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_Host, m_strHost ) )
		{
			m_strHost.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_EngineName, m_strEngineName ) )
		{
			m_strEngineName.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_EngineAddr, m_strEngineAddr ) )
		{
			m_strEngineAddr.Empty();
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileSessionBlock_MediaDomain, m_strDomain ) )
		{
			m_strDomain.Empty();
		}

		CComPtr<IXMLDOMNodeList>	spDataRates;
		hr = spRoot->selectNodes(
			CComBSTR( kCaptureFileSessionBlock_DataRates _T("/") kCaptureFileSessionBlock_Rate ), &spDataRates );
		if ( hr == S_OK )
		{
			long	cNodes;
			hr = spDataRates->get_length( &cNodes );
			if ( FAILED(hr) ) AtlThrow( hr );
			for ( long i = 0; i < cNodes; i++)
			{
				CComPtr<IXMLDOMNode>	spNode;
				hr = spDataRates->get_item( i, &spNode );
				if ( FAILED(hr) ) AtlThrow( hr );
				UInt32	nRate;
				XmlUtil::GetElement( CComQIPtr<IXMLDOMElement>( spNode ), nRate );
				if ( m_ayDataRates.GetIndexOf( nRate ) < 0 )
				{
					m_ayDataRates.AddItem( nRate );
				}
			}
		}

		CComPtr<IXMLDOMNodeList>	spChannelNodes;
		hr = spRoot->selectNodes(
			CComBSTR( kCaptureFileSessionBlock_ChannelList _T("/") kCaptureFileSessionBlock_Channel ), &spChannelNodes );
		if ( hr == S_OK )
		{
			m_channels.SetChannelType( IS_WIRELESS( GetMediaSubType() ) ?
				CChannelManager::kWirelessChannels : CChannelManager::kEnumChannels );

			long	cNodes;
			hr = spChannelNodes->get_length( &cNodes );
			if ( FAILED(hr) ) AtlThrow( hr );
			for ( long i = 0; i < cNodes; i++)
			{
				CComPtr<IXMLDOMNode>	spNode;
				hr = spChannelNodes->get_item( i, &spNode );
				if ( FAILED(hr) ) AtlThrow( hr );

				// This either has a single value (enumerated channel/old wireless), 
				// or has children (detailed channel).
				// Try and get the "number" child.
				SWirelessChannel	channel = { 0 };
				if ( XmlUtil::GetChildElement( spNode,
					kCaptureFileSessionBlock_Channel_Number, channel.Channel ) )
				{
					if ( !XmlUtil::GetChildElement( spNode, 
						kCaptureFileSessionBlock_Channel_Freq, channel.Frequency ) )
					{
						AtlThrow( E_FAIL );
					}
					if ( !XmlUtil::GetChildElement( spNode, 
						kCaptureFileSessionBlock_Channel_Band, channel.Band ) )
					{
						AtlThrow( E_FAIL );
					}
					m_channels.AddChannel( &channel );
				}
				else
				{
					SInt32	nChannel;
					XmlUtil::GetElement( CComQIPtr<IXMLDOMElement>( spNode ), nChannel );

					if ( IS_WIRELESS( GetMediaSubType() ) )
					{
						SWirelessChannel	wc = CChannelManager::SetupWirelessChannel( 
													GetMediaType(), GetMediaSubType(), nChannel );
						m_channels.AddChannel( &wc );
					}
					else
					{
						m_channels.AddChannel( &nChannel );
					}
				}
			}
		}

		hr = S_OK;
	}
	CATCH_HR(hr)

	return (hr == S_OK);
}


// -----------------------------------------------------------------------------
//		SetWriteTime
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetWriteTime(
	time_t	t )
{
	m_strTimeCount.Format( _T("%u"), t );

	struct tm Tm;
	errno_t	err = gmtime_s( &Tm, &t );
	if ( err == 0 ) {
		_tcsftime( m_strTime.GetBuffer( 256 ), 256, _T( "%Y-%m-%dT%H:%M:%SZ" ), &Tm );
		m_strTime.ReleaseBuffer();
	}
}


// -----------------------------------------------------------------------------
//		GetWriteTime
// -----------------------------------------------------------------------------

time_t
CCaptureFileSessionBlock::GetWriteTime() const
{
	if ( m_strTimeCount.IsEmpty() ) return 0;
	return (time_t) _ttol( m_strTimeCount );
}


// -----------------------------------------------------------------------------
//		SetTimeZoneBias
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetTimeZoneBias(
	SInt32	nBias )
{
	m_strTimezoneBias.Format( _T("%d"), nBias );
}


// -----------------------------------------------------------------------------
//		GetTimezoneBias
// -----------------------------------------------------------------------------

SInt32
CCaptureFileSessionBlock::GetTimeZoneBias() const
{
	if ( m_strTimezoneBias.IsEmpty() ) return 0;
	return (SInt32) _ttol( m_strTimezoneBias );
}


// -----------------------------------------------------------------------------
//		SetMediaType
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetMediaType(
	TMediaType	inMediaType )
{
	m_strMediaType.Format( _T("%u"), inMediaType );
}


// -----------------------------------------------------------------------------
//		GetMediaType
// -----------------------------------------------------------------------------

TMediaType
CCaptureFileSessionBlock::GetMediaType() const
{
	if ( m_strMediaType.IsEmpty() ) return kMediaType_802_3;
	return (TMediaType) _ttol( m_strMediaType );
}


// -----------------------------------------------------------------------------
//		SetMediaSubType
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetMediaSubType(
	TMediaSubType	inSubType )
{
	m_strMediaSubType.Format( _T("%u"), inSubType );
}


// -----------------------------------------------------------------------------
//		GetMediaSubType
// -----------------------------------------------------------------------------

TMediaSubType
CCaptureFileSessionBlock::GetMediaSubType() const
{
	if ( m_strMediaSubType.IsEmpty() ) return kMediaSubType_Native;
	return (TMediaSubType) _ttol( m_strMediaSubType );
}


// -----------------------------------------------------------------------------
//		SetLinkSpeed
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetLinkSpeed(
	UInt64	inLinkSpeed )
{
	m_strLinkSpeed.Format( _T("%I64u"), inLinkSpeed );
}


// -----------------------------------------------------------------------------
//		GetLinkSpeed
// -----------------------------------------------------------------------------

UInt64
CCaptureFileSessionBlock::GetLinkSpeed() const
{
	if ( m_strLinkSpeed.IsEmpty() ) return 0;
	return (UInt64) _tcstoui64( m_strLinkSpeed, NULL, 10 );
}


// -----------------------------------------------------------------------------
//		SetPacketCount
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetPacketCount(
	UInt32	inPacketCount )
{
	m_strPacketCount.Format( _T("%u"), inPacketCount );
}


// -----------------------------------------------------------------------------
//		GetPacketCount
// -----------------------------------------------------------------------------

UInt32
CCaptureFileSessionBlock::GetPacketCount() const
{
	if ( m_strPacketCount.IsEmpty() ) return 0;
	return (UInt32) _ttol( m_strPacketCount );
}


// -----------------------------------------------------------------------------
//		SetComment
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetComment(
	const CString&	strComment )
{
	m_strComment = strComment;
}


// -----------------------------------------------------------------------------
//		GetComment
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetComment() const
{
	return m_strComment;
}


// -----------------------------------------------------------------------------
//		SetSessionStartTime
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetSessionStartTime(
	UInt64	t )
{
	if ( (t == 0) || (t == kTimeStamp_Invalid) )
	{
		m_strSessionStartTime.Empty();
	}
	else
	{
		TimeStampToISO8601String(
			t,
			kFormat_Nanoseconds,
			m_strSessionStartTime.GetBuffer( 256 ),
			256 );
		m_strSessionStartTime.ReleaseBuffer();
	}
}


// -----------------------------------------------------------------------------
//		GetSessionStartTime
// -----------------------------------------------------------------------------

UInt64
CCaptureFileSessionBlock::GetSessionStartTime() const
{
	UInt64	ts = 0;
	ISO8601StringToTimeStamp( m_strSessionStartTime, &ts );
	return ts;
}


// -----------------------------------------------------------------------------
//		SetSessionEndTime
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetSessionEndTime(
	UInt64	t )
{
	if ( (t == 0) || (t == kTimeStamp_Invalid) )
	{
		m_strSessionEndTime.Empty();
	}
	else
	{
		TimeStampToISO8601String(
			t,
			kFormat_Nanoseconds,
			m_strSessionEndTime.GetBuffer( 256 ),
			256 );
		m_strSessionEndTime.ReleaseBuffer();
	}
}


// -----------------------------------------------------------------------------
//		GetSessionEndTime
// -----------------------------------------------------------------------------

UInt64
CCaptureFileSessionBlock::GetSessionEndTime() const
{
	UInt64	ts = 0;
	ISO8601StringToTimeStamp( m_strSessionEndTime, &ts );
	return ts;
}


// -----------------------------------------------------------------------------
//		SetAdapterName
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetAdapterName(
	const CString&	strAdapterName )
{
	m_strAdapterName = strAdapterName;
}


// -----------------------------------------------------------------------------
//		GetAdapterName
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetAdapterName() const
{
	return m_strAdapterName;
}


// -----------------------------------------------------------------------------
//		SetAdapterAddr
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetAdapterAddr(
	const CString&	strAdapterAddr )
{
	m_strAdapterAddr = strAdapterAddr;
}


// -----------------------------------------------------------------------------
//		GetAdapterAddr
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetAdapterAddr() const
{
	return m_strAdapterAddr;
}


// -----------------------------------------------------------------------------
//		SetCaptureName
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetCaptureName(
	const CString&	strCaptureName )
{
	m_strCaptureName = strCaptureName;
}


// -----------------------------------------------------------------------------
//		GetCaptureName
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetCaptureName() const
{
	return m_strCaptureName;
}


// -----------------------------------------------------------------------------
//		SetCaptureID
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetCaptureID(
	const GUID&	id )
{
	OLECHAR	szID[256];
	if ( ::StringFromGUID2( id, szID, 256 ) )
	{
		m_strCaptureID = szID;
	}
}


// -----------------------------------------------------------------------------
//		GetCaptureID
// -----------------------------------------------------------------------------

GUID
CCaptureFileSessionBlock::GetCaptureID() const
{
	HRESULT	hr;
	GUID	id;
	hr = ::CLSIDFromString( CT2OLE( m_strCaptureID ), &id );
	if ( FAILED(hr) ) return GUID_NULL;
	return id;
}


// -----------------------------------------------------------------------------
//		SetOwner
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetOwner(
	const CString&	strOwner )
{
	m_strOwner = strOwner;
}


// -----------------------------------------------------------------------------
//		GetOwner
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetOwner() const
{
	return m_strOwner;
}


// -----------------------------------------------------------------------------
//		SetFileIndex
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetFileIndex(
	UInt32	nIndex )
{
	m_strFileIndex.Format( _T("%u"), nIndex );
}


// -----------------------------------------------------------------------------
//		GetFileIndex
// -----------------------------------------------------------------------------

UInt32
CCaptureFileSessionBlock::GetFileIndex() const
{
	return (UInt32) _ttol( m_strFileIndex );
}


// -----------------------------------------------------------------------------
//		SetHost
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetHost(
	const CString&	strHost )
{
	m_strHost = strHost;
}


// -----------------------------------------------------------------------------
//		GetHost
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetHost() const
{
	return m_strHost;
}


// -----------------------------------------------------------------------------
//		SetEngineName
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetEngineName(
	const CString&	strName )
{
	m_strEngineName = strName;
}


// -----------------------------------------------------------------------------
//		GetEngineName
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetEngineName() const
{
	return m_strEngineName;
}


// -----------------------------------------------------------------------------
//		SetEngineAddr
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetEngineAddr(
	const CString&	strAddr )
{
	m_strEngineAddr = strAddr;
}


// -----------------------------------------------------------------------------
//		GetEngineAddr
// -----------------------------------------------------------------------------

CString
CCaptureFileSessionBlock::GetEngineAddr() const
{
	return m_strEngineAddr;
}


// -----------------------------------------------------------------------------
//		SetDomain
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetDomain(
	TMediaDomain	inDomain )
{
	m_strDomain.Format(_T("%u"), inDomain );
	m_bIncludeWirelessInfo = true;
}


// -----------------------------------------------------------------------------
//		GetDomain
// -----------------------------------------------------------------------------

TMediaDomain
CCaptureFileSessionBlock::GetDomain() const
{
	return (TMediaDomain) _ttol(m_strDomain);
}


// -----------------------------------------------------------------------------
//		AddDataRate
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::AddDataRate(
	UInt32	inDataRate )
{
	if (m_ayDataRates.GetIndexOf(inDataRate) < 0)
		m_ayDataRates.AddItem( inDataRate );
	m_bIncludeWirelessInfo = true;
}


// -----------------------------------------------------------------------------
//		SetDataRates
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::SetDataRates(
	const UInt32*	inDataRates,
	SInt32			nCount )
{
	m_ayDataRates.RemoveAllItems();
	for (SInt32 nIdx = 0; nIdx < nCount; nIdx++)
		m_ayDataRates.AddItem( inDataRates[nIdx] );
	m_bIncludeWirelessInfo = true;
}


// -----------------------------------------------------------------------------
//		GetDataRates
// -----------------------------------------------------------------------------

void
CCaptureFileSessionBlock::GetDataRates(
	UInt32*	payRates,
	UInt32&	nCount ) const
{
	ASSERT(payRates != NULL);
	nCount = min( nCount, m_ayDataRates.GetCount() );
	memcpy( payRates, m_ayDataRates.GetData(), nCount*sizeof(UInt32) );
}


// -----------------------------------------------------------------------------
//		GetDataRateCount
// -----------------------------------------------------------------------------

UInt32
CCaptureFileSessionBlock::GetDataRateCount() const
{
	return m_ayDataRates.GetCount();
}


// =============================================================================
//	CCaptureFilePacketBlock
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFilePacketBlock::CCaptureFilePacketBlock() :
	m_bUseMemoryPointer( false ),
	m_bWroteHeader( false ),
	m_pPacket( NULL ),
	m_nDataSize( 0 ),
	m_nPacketCount( 0 ),
	m_packetData( NULL ),
	m_MediaSpec( NULL ),
	m_Wireless( NULL ),
	m_FullDuplex( NULL ),
	m_Wan( NULL ),
	m_nBlockHeaderPos( 0 )

{
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFilePacketBlock::~CCaptureFilePacketBlock()
{
}


// -----------------------------------------------------------------------------
//		UpdateHeader
// -----------------------------------------------------------------------------

bool
CCaptureFilePacketBlock::UpdateHeader(
	CCaptureFileWriter&	theFile,
	bool				bRestoreFilePos )
{
	theFile.Flush();

	// get the current position and subtract the start position (and the header length)
	// if the WriteHeader function has not been called, there isn't really anything to 
	// update.  Also, executing the code below will overwrite the version block
	// since the block header pos it at 0.  So I guess just don't update the header
	// if it's never been written?  Maybe someday write the header if it hasn't been
	// written yet
	if (m_nBlockHeaderPos == 0) return false;

	const UInt32	nFilePos = theFile.GetFilePosition();
	const UInt32	cbLength = nFilePos - m_nBlockHeaderPos - sizeof(BLOCK_HEADER);
	const UInt32	nFlags = MAKE_BLOCK_FLAGS( Uncompressed, BinaryFormat );

	if ( !theFile.SetFilePosition( m_nBlockHeaderPos, SEEK_SET ) )
	{
		return false;
	}

	// now update
	if ( !theFile.WriteHeader( GetType(), cbLength, nFlags ) )
	{
		return false;
	}

	theFile.Flush();

	if ( bRestoreFilePos )
	{
		// move position back to normal
		if ( !theFile.SetFilePosition( nFilePos, SEEK_SET ) )
		{
			return false;
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		WriteHeader
// -----------------------------------------------------------------------------

bool
CCaptureFilePacketBlock::WriteHeader(
	CCaptureFileWriter&	theFile,
	DWORD*				outBytesWritten )
{
	if ( !m_bWroteHeader )
	{
		m_bWroteHeader = true;

		theFile.Flush();
		m_nBlockHeaderPos = theFile.GetFilePosition();

		const UInt32	nFlags = MAKE_BLOCK_FLAGS( Uncompressed, BinaryFormat );

		if ( !theFile.WriteHeader( GetType(), 0, nFlags ) )
		{
			return false;
		}

		if ( outBytesWritten != NULL )
		{
			*outBytesWritten = sizeof(BLOCK_HEADER);
		}

		m_header.actualLength.fieldType = Attrib_ActualLength;
		m_header.timestampLo.fieldType = Attrib_TimeStampLo;
		m_header.timestampHi.fieldType = Attrib_TimeStampHi;
		m_header.flagsStatus.fieldType = Attrib_FlagsStatus;
		m_header.mediaSpecType.fieldType = Attrib_MediaSpecType;

		m_packetDataStart.packetDataLength.fieldType = Attrib_PacketDataLength;

		// init the wireless header
		// channel, data rate, dbm and % strength/noise
		m_wirelessHeader.channelNumber.fieldType = Attrib_ChannelNumber; 
		m_wirelessHeader.channelFreq.fieldType = Attrib_ChannelFreq; 
		m_wirelessHeader.channelBand.fieldType = Attrib_ChannelBand; 
		m_wirelessHeader.dataRate.fieldType = Attrib_DataRate;
		m_wirelessHeader.signalStrength.fieldType = Attrib_SignalStrength;
		m_wirelessHeader.signaldBm.fieldType = Attrib_SignaldBm;
		m_wirelessHeader.signaldBm1.fieldType = Attrib_SignaldBm1;
		m_wirelessHeader.signaldBm2.fieldType = Attrib_SignaldBm2;
		m_wirelessHeader.signaldBm3.fieldType = Attrib_SignaldBm3;
		m_wirelessHeader.noiseStrength.fieldType = Attrib_NoiseStrength;
		m_wirelessHeader.noisedBm.fieldType = Attrib_NoisedBm;
		m_wirelessHeader.noisedBm1.fieldType = Attrib_NoisedBm1;
		m_wirelessHeader.noisedBm2.fieldType = Attrib_NoisedBm2;
		m_wirelessHeader.noisedBm3.fieldType = Attrib_NoisedBm3;
		m_wirelessHeader.flagsN.fieldType = Attrib_FlagsN;

		// init full duplex header
		m_fullDuplexHeader.channel.fieldType = Attrib_ChannelNumber;

		// init wan header
		m_wanHeader.Direction.fieldType = Attrib_Direction;
		m_wanHeader.Protocol.fieldType = Attrib_Protocol;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CCaptureFilePacketBlock::Write(
	CCaptureFileWriter&	theFile,
	DWORD*				outBytesWritten )
{
	if ( !m_bWroteHeader )
	{
		WriteHeader( theFile, outBytesWritten );	// write header and init structs
	}

	// now write the packet
	// just one packet at a time
	// init this packet header
	const PeekPacket* pPacket = GetPacket();
	m_header.actualLength.fieldValue = pPacket->fPacketLength;
	m_header.timestampLo.fieldValue = (UInt32) (pPacket->fTimeStamp & 0xFFFFFFFF);
	m_header.timestampHi.fieldValue = (UInt32) ((pPacket->fTimeStamp >> 32) & 0xFFFFFFFF);
	ASSERT( (pPacket->fFlags & 0xFFFF0000) == 0 );
	ASSERT( (pPacket->fStatus & 0xFFFF0000) == 0 );
	m_header.flagsStatus.fieldValue = MAKELONG(pPacket->fFlags, pPacket->fStatus);

	// get the media specific info header type
	// init the media info stuff
	const MediaSpecificPrivateHeader* pHdr = 
		reinterpret_cast<const MediaSpecificPrivateHeader*>(pPacket->fMediaSpecInfoBlock);

	if (pHdr)
	{
		m_header.mediaSpecType.fieldValue = pHdr->nType;

		// change the wireless3 type to wireless2, or previous versions kinda 
		// screw it up.  any media spec header changes should maintain
		// the oldest "type"; the header is attributed, so no worry about format
		// just allow older versions to read what they can
		switch (m_header.mediaSpecType.fieldValue)
		{
		case kMediaSpecificHdrType_Wireless3:
			m_header.mediaSpecType.fieldValue = kMediaSpecificHdrType_Wireless2;
			break;
		default:
			break;
		}

		// write the main header - must be before the media specific info
		if ( outBytesWritten )
			*outBytesWritten += sizeof(m_header);
		if ( !theFile.WriteBytes( &m_header, sizeof(m_header) ))
			return false;

		switch (pHdr->nType)
		{
			case kMediaSpecificHdrType_Wireless3:
			{	
				const Wireless80211PrivateHeader* pWireless = 
					reinterpret_cast<const Wireless80211PrivateHeader*>
						(pPacket->fMediaSpecInfoBlock + sizeof(MediaSpecificPrivateHeader));
				m_wirelessHeader.channelNumber.fieldValue = pWireless->Channel.Channel;
				m_wirelessHeader.channelFreq.fieldValue = pWireless->Channel.Frequency;
				m_wirelessHeader.channelBand.fieldValue = pWireless->Channel.Band;
				m_wirelessHeader.dataRate.fieldValue = pWireless->DataRate;
				m_wirelessHeader.signalStrength.fieldValue = pWireless->SignalPercent;
				m_wirelessHeader.signaldBm.fieldValue = pWireless->SignaldBm;
				m_wirelessHeader.signaldBm1.fieldValue = pWireless->SignaldBm1;
				m_wirelessHeader.signaldBm2.fieldValue = pWireless->SignaldBm2;
				m_wirelessHeader.signaldBm3.fieldValue = pWireless->SignaldBm3;
				m_wirelessHeader.noiseStrength.fieldValue = pWireless->NoisePercent;
				m_wirelessHeader.noisedBm.fieldValue = pWireless->NoisedBm;
				m_wirelessHeader.noisedBm1.fieldValue = pWireless->NoisedBm1;
				m_wirelessHeader.noisedBm2.fieldValue = pWireless->NoisedBm2;
				m_wirelessHeader.noisedBm3.fieldValue = pWireless->NoisedBm3;
				m_wirelessHeader.flagsN.fieldValue = pWireless->FlagsN;
				if ( outBytesWritten )
					*outBytesWritten += sizeof(m_wirelessHeader);
				if (!theFile.WriteBytes(&m_wirelessHeader, sizeof(m_wirelessHeader)))
					return false;
				break;
			}
			case kMediaSpecificHdrType_FullDuplex:
			{
				const FullDuplexPrivateHeader* pDuplex = 
					reinterpret_cast<const FullDuplexPrivateHeader*>
						(pPacket->fMediaSpecInfoBlock + sizeof(MediaSpecificPrivateHeader));
				m_fullDuplexHeader.channel.fieldValue = pDuplex->Channel;
				if ( outBytesWritten )
					*outBytesWritten += sizeof(m_fullDuplexHeader);
				if (!theFile.WriteBytes(&m_fullDuplexHeader, sizeof(m_fullDuplexHeader)))
					return false;
				break;
			}
			case kMediaSpecificHdrType_Wan:
			{
				const WideAreaNetworkHeader* pWan = 
					reinterpret_cast<const WideAreaNetworkHeader*>
						(pPacket->fMediaSpecInfoBlock + sizeof(MediaSpecificPrivateHeader));
				m_wanHeader.Direction.fieldValue = pWan->Direction;
				m_wanHeader.Protocol.fieldValue  = pWan->Protocol;
				if ( outBytesWritten )
					*outBytesWritten += sizeof(m_wanHeader);
				if (!theFile.WriteBytes(&m_wanHeader, sizeof(m_wanHeader)))
					return false;
				break;
			}
			default:
				break;
		}
	}
	else
	{
		m_header.mediaSpecType.fieldValue = kMediaSpecificHdrType_Invalid;

		// write the main header
		if ( outBytesWritten )
			*outBytesWritten += sizeof(m_header);
		if ( !theFile.WriteBytes( &m_header, sizeof(m_header) ))
			return false;
	}

	// write the packet data start header
	UInt32 nPacketDataLength = m_nDataSize;
	if (nPacketDataLength == 0)
		nPacketDataLength = (pPacket->fSliceLength == 0) ? 
							pPacket->fPacketLength : min(pPacket->fSliceLength, pPacket->fPacketLength);
	m_packetDataStart.packetDataLength.fieldValue = nPacketDataLength;
	if ( outBytesWritten )
		*outBytesWritten += sizeof(m_packetDataStart);
	if (!theFile.WriteBytes( &m_packetDataStart, sizeof(m_packetDataStart)))
		return false;

	// write the packet data 
	if ( outBytesWritten )
		*outBytesWritten += nPacketDataLength;
	if (!theFile.WriteBytes( m_packetData, nPacketDataLength ))
		return false;
	m_nPacketCount++;

	return true;
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

bool
CCaptureFilePacketBlock::Read(
	CCaptureFileReader&	theFile )
{
	// read in the next packet
	// loop reading attributes until we get the packet data length
	if (theFile.IsEndOfBlock())
		return false;

	PacketAttribute theAttrib;
	memset( &m_Packet, 0, sizeof(m_Packet) );
	memset( &m_mediaInfo, 0, sizeof(m_mediaInfo) );

	// this happens each read, but should be cheap since the size is exactly the same
	m_ayReadPacketData.SetCount( USHRT_MAX );
	m_packetData = (const UInt8*) m_ayReadPacketData.GetData();

	do 
	{
		if ( !theFile.ReadBytes( &theAttrib, sizeof(theAttrib) ) )
		{	// end of the block or read error?
			return false;
		}

		switch (theAttrib.fieldType)
		{
			case Attrib_ActualLength:
				m_Packet.fPacketLength = (UInt16) theAttrib.fieldValue;
				break;
			case Attrib_TimeStampLo:
				m_Packet.fTimeStamp |= theAttrib.fieldValue;
				break;
			case Attrib_TimeStampHi:
				m_Packet.fTimeStamp |= (((UInt64)theAttrib.fieldValue) << 32);
				break;
			case Attrib_FlagsStatus:
				// flags in loword
				m_Packet.fFlags  = (UInt32) LOWORD(theAttrib.fieldValue);
				m_Packet.fStatus = (UInt32) HIWORD(theAttrib.fieldValue);
				break;
			case Attrib_MediaSpecType:
				m_mediaInfo.StdHeader.nType = theAttrib.fieldValue;
				switch ( m_mediaInfo.StdHeader.nType )
				{
					case kMediaSpecificHdrType_WirelessEx:	// never used in production
					case kMediaSpecificHdrType_Wireless2:
					case kMediaSpecificHdrType_Wireless3:
						// make sure it's properly set to the latest version
						m_mediaInfo.StdHeader.nType = kMediaSpecificHdrType_Wireless3;
						// set the size
						m_mediaInfo.StdHeader.nSize = sizeof(MediaSpecificPrivateHeader) + sizeof(Wireless80211PrivateHeader);
						// init the wireless header
						WirelessHeaderUtil::InitMediaSpecInfo( &m_mediaInfo.MediaInfo.wireless );
						break;
					case kMediaSpecificHdrType_FullDuplex:
						// set the size
						m_mediaInfo.StdHeader.nSize = sizeof(MediaSpecificPrivateHeader) + sizeof(FullDuplexPrivateHeader);
						break;
					case kMediaSpecificHdrType_Wan:
						// set the size
						m_mediaInfo.StdHeader.nSize = sizeof(MediaSpecificPrivateHeader) + sizeof(WideAreaNetworkHeader);
						break;
					case kMediaSpecificHdrType_Invalid:
						// none present; do nothing
						break;
					default:	// WTF?
						ASSERT(0);
						break;
				}
				if (m_mediaInfo.StdHeader.nSize != 0)
				{
					// assign the pointer to the media spec info to m_Packet's pointer
					m_Packet.fMediaSpecInfoBlock = (UInt8*) &m_mediaInfo;
				}
				break;
			case Attrib_ChannelNumber:
				// the first tagged file didn't have media spec types (and was a wireless file)
				// so the easy way to do this is map 0 (kMediaSpecificHdrType_Invalid) to Wireless
				switch ( m_mediaInfo.StdHeader.nType )
				{
					case kMediaSpecificHdrType_Invalid:
						// set the type and size
						m_mediaInfo.StdHeader.nType = kMediaSpecificHdrType_Wireless3;
						m_mediaInfo.StdHeader.nSize = sizeof(MediaSpecificPrivateHeader) + sizeof(Wireless80211PrivateHeader);
						// assign the pointer to the media spec info to m_Packet's pointer
						m_Packet.fMediaSpecInfoBlock = (UInt8*) &m_mediaInfo;
						// fill in the missing data for pre-5.6.0 versions
						m_mediaInfo.MediaInfo.wireless.Channel = CChannelManager::SetupWirelessChannel( kMediaType_802_3,
							kMediaSubType_802_11_gen, theAttrib.fieldValue );
						break;
					case kMediaSpecificHdrType_Wireless3:
					case kMediaSpecificHdrType_Wireless2:
						// this is in v5.6.0+, so we can be assured that the band/freq info is here
						m_mediaInfo.MediaInfo.wireless.Channel.Channel = (SInt16) theAttrib.fieldValue;
						break;
					case kMediaSpecificHdrType_FullDuplex:
						m_mediaInfo.MediaInfo.fullDuplex.Channel = (UInt32) theAttrib.fieldValue;
						break;
					default:	// WTF?
						ASSERT(0);
						break;
				}
				break;
			case Attrib_ChannelFreq:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.Channel.Frequency = (UInt32) theAttrib.fieldValue;
				break;
			case Attrib_ChannelBand:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.Channel.Band = (UInt32) theAttrib.fieldValue;
				break;
			case Attrib_DataRate:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.DataRate = (UInt16) theAttrib.fieldValue;
				break;
			case Attrib_SignalStrength:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.SignalPercent= (UInt8) theAttrib.fieldValue;
				break;
			case Attrib_SignaldBm:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.SignaldBm = 
					WirelessHeaderUtil::dBmConvert( (SInt16) theAttrib.fieldValue );
				break;
			case Attrib_SignaldBm1:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.SignaldBm1 = (SInt8) theAttrib.fieldValue;
				break;
			case Attrib_SignaldBm2:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.SignaldBm2 = (SInt8) theAttrib.fieldValue;
				break;
			case Attrib_SignaldBm3:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.SignaldBm3 = (SInt8) theAttrib.fieldValue;
				break;
			case Attrib_NoiseStrength:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.NoisePercent = (UInt8) theAttrib.fieldValue;
				break;
			case Attrib_NoisedBm:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.NoisedBm = 
					WirelessHeaderUtil::dBmConvert( (SInt16) theAttrib.fieldValue );
				break;
			case Attrib_NoisedBm1:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.NoisedBm1 = (SInt8) theAttrib.fieldValue;
				break;
			case Attrib_NoisedBm2:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.NoisedBm2 = (SInt8) theAttrib.fieldValue;
				break;
			case Attrib_NoisedBm3:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.NoisedBm3 = (SInt8) theAttrib.fieldValue;
				break;
			case Attrib_FlagsN:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wireless3);
				m_mediaInfo.MediaInfo.wireless.FlagsN = theAttrib.fieldValue;
				break;
			case Attrib_Direction:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wan);
				m_mediaInfo.MediaInfo.wan.Direction = (UInt8) theAttrib.fieldValue;
				break;
			case Attrib_Protocol:
				ASSERT(m_mediaInfo.StdHeader.nType == kMediaSpecificHdrType_Wan);
				m_mediaInfo.MediaInfo.wan.Protocol = (UInt8) theAttrib.fieldValue;
				break;
			case Attrib_PacketDataLength:	
				// set the slice length
				if (theAttrib.fieldValue < m_Packet.fPacketLength)
					m_Packet.fSliceLength = (UInt16) theAttrib.fieldValue;
				// after this we are done
				if ( m_bUseMemoryPointer )
				{	// get the pointer to the memory
					SetPacket( &m_Packet, theFile.GetFilePosition() + theFile.GetMemoryMap() );
					// advance the data pointer and return
					return theFile.SetFilePosition( theAttrib.fieldValue, FILE_CURRENT );
				}
				else
				{	// read them into a buffer
					if ( theFile.ReadBytes( m_ayReadPacketData.GetData(), theAttrib.fieldValue ) )
					{	// set our pointers up
						SetPacket( &m_Packet, (UInt8*) m_ayReadPacketData.GetData() );
						return true;
					}
				}
				return false;	// failed to read the packet data
			default:
				ASSERT(0);
				break;
		}
	}
	while ( !theFile.IsEndOfBlock() );

	return false;
}


// -----------------------------------------------------------------------------
//		SetPacket
// -----------------------------------------------------------------------------

void
CCaptureFilePacketBlock::SetPacket( 
	const PeekPacket*		thePacket, 
	const UInt8*			inPacketData,
	UInt16					inDataSize /* = 0 */ )
{
	m_pPacket = thePacket; 
	m_packetData = inPacketData;
	m_nDataSize = inDataSize;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void 
CCaptureFilePacketBlock::Reset()
{
	m_bWroteHeader = false;
	m_nPacketCount = 0;
	m_nBlockHeaderPos = 0;	
}


// -----------------------------------------------------------------------------
//		SetUseMemoryPointer
// -----------------------------------------------------------------------------

void 
CCaptureFilePacketBlock::SetUseMemoryPointer(
	bool	bUseMemoryPointer )
{
	m_bUseMemoryPointer = bUseMemoryPointer;
}


// =============================================================================
//	CCaptureFileIdBlock
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFileIdBlock::CCaptureFileIdBlock() :
	m_bEnabled( false ),
	m_nIndex( 0 )
{
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFileIdBlock::~CCaptureFileIdBlock()
{
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CCaptureFileIdBlock::Write(
	CCaptureFileWriter&	theFile,
	DWORD*				outBytesWritten /* = NULL */ )
{
	if ( !m_bEnabled ) return true;

	HRESULT	hr = S_OK;

	try
	{
		// Increment the index.
		m_nIndex++;

		CComPtr<IXMLDOMDocument>	spDoc;
		hr = XmlUtil::CreateXmlDocument( &spDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		CComPtr<IXMLDOMElement>	spRoot;
		XmlUtil::AddChildElement( spDoc, spDoc,
			kCaptureFileIdBlock_Root, &spRoot );

		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileIdBlock_Id, m_strId );
		XmlUtil::AddChildElement( spDoc, spRoot,
			kCaptureFileIdBlock_Index, m_nIndex );

		CStreamOnCByteStream	stream;
		CComPtr<IPersistStream>	spPersist;
		hr = spDoc->QueryInterface( &spPersist );
		if ( FAILED(hr) ) AtlThrow( hr );
		hr = spPersist->Save( &stream, TRUE );
		if ( FAILED(hr) ) AtlThrow( hr );

		// Pad the block out to 2 KB.
		const UInt32	cbPaddedLength = 2048 - sizeof(BLOCK_HEADER);
		if ( stream.GetLength() < cbPaddedLength )
		{
			UInt8	fill[cbPaddedLength];
			memset( fill, 0x20, cbPaddedLength );
			UInt32	cbFill = cbPaddedLength - stream.GetLength();
			stream.Write( fill, cbFill, NULL );
		}

		const UInt32	cbLength = stream.GetLength();
		const UInt32	nFlags =  MAKE_BLOCK_FLAGS( Uncompressed, XMLFormat );

		if ( !theFile.WriteHeader( GetType(), cbLength, nFlags ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !theFile.WriteBytes( stream.GetRawDataPtr(), cbLength ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( outBytesWritten != NULL )
		{
			*outBytesWritten = sizeof(BLOCK_HEADER) + cbLength;
		}
	}
	CATCH_HR(hr)

	return (hr == S_OK);
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

bool
CCaptureFileIdBlock::Read(
	CCaptureFileReader&	theFile )
{
	HRESULT hr = S_OK;

	try
	{
		CComPtr<IXMLDOMDocument>	spDoc;
		hr = theFile.ReadMSXMLDoc( &spDoc );
		if ( FAILED(hr) ) AtlThrow( hr );

		// Get the root node.
		CComPtr<IXMLDOMElement>	spRoot;
		hr = spDoc->get_documentElement( &spRoot );
		if ( FAILED(hr) ) AtlThrow( hr );

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileIdBlock_Id, m_strId ) )
		{
			AtlThrow( E_FAIL );
		}

		if ( !XmlUtil::GetChildElement( spRoot,
			kCaptureFileIdBlock_Index, m_nIndex ) )
		{
			m_nIndex = 0;
		}

		hr = S_OK;
	}
	CATCH_HR(hr)

	return (hr == S_OK);
}


// -----------------------------------------------------------------------------
//		SetId
// -----------------------------------------------------------------------------

void
CCaptureFileIdBlock::SetId()
{
	GUID		id;
	HRESULT		hr = ::CoCreateGuid( &id );
	if ( FAILED(hr) )
	{
		id = GUID_NULL;
	}
	SetId( id );
}


// -----------------------------------------------------------------------------
//		SetId
// -----------------------------------------------------------------------------

void
CCaptureFileIdBlock::SetId(
	const GUID&	id )
{
	OLECHAR		szId[256];
	VERIFY( ::StringFromGUID2( id, szId, 256 ) > 0 );
	m_strId = szId;
	m_nIndex = 0;
}


// -----------------------------------------------------------------------------
//		GetId
// -----------------------------------------------------------------------------

GUID
CCaptureFileIdBlock::GetId() const
{
	GUID	id;
	HRESULT	hr = ::CLSIDFromString( CT2OLE( m_strId ), &id );
	if ( FAILED(hr) )
	{
		id = GUID_NULL;
	}

	return id;
}


// =============================================================================
//	CCaptureFileGpsBlock
// =============================================================================

// -----------------------------------------------------------------------------
//		constructor
// -----------------------------------------------------------------------------

CCaptureFileGpsBlock::CCaptureFileGpsBlock() :
	m_pByteStream( NULL )
{
}


// -----------------------------------------------------------------------------
//		destructor
// -----------------------------------------------------------------------------

CCaptureFileGpsBlock::~CCaptureFileGpsBlock()
{
	delete m_pByteStream;
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CCaptureFileGpsBlock::Write(
	CCaptureFileWriter&	theFile,
	DWORD*				/*outBytesWritten*/ )
{
	ASSERT( m_pByteStream );
	if ( m_pByteStream == NULL ) return false;

	const UInt32	cbLength = m_pByteStream->GetLength();
	const UInt32	nFlags = MAKE_BLOCK_FLAGS( Uncompressed, BinaryFormat );

	if ( !theFile.WriteHeader( GetType(), cbLength, nFlags ) )
	{
		return false;
	}

	if ( !theFile.WriteBytes( m_pByteStream->GetData(), cbLength ) )
	{
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

bool
CCaptureFileGpsBlock::Read(
	CCaptureFileReader&	theFile )
{
	delete m_pByteStream;
	m_pByteStream = new CByteStream;
	ASSERT( m_pByteStream );
	if ( m_pByteStream == NULL ) return false;

	UInt32	nBlockSize = theFile.GetCurBlockLength();
	m_pByteStream->SetLength( nBlockSize );
	bool bResult = theFile.ReadBytes( m_pByteStream->GetData(), nBlockSize );

	return bResult;
}
