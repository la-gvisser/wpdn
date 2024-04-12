// =============================================================================
//	ByteStream.cpp
// =============================================================================
//	Copyright (c) 2000-2007 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1998-2000 AG Group, Inc. All rights reserved.

#include "stdafx.h"
#include "ByteStream.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// better throws for out of memory...

#if defined(_AFX)
#define THROW_MEMORY_EXCEPTION throw new CMemoryException
#elif defined(_ATL)
#define THROW_MEMORY_EXCEPTION AtlThrow(E_OUTOFMEMORY)
#else
#define THROW_MEMORY_EXCEPTION throw -1
#endif

// -----------------------------------------------------------------------------
//		CByteStream
// -----------------------------------------------------------------------------

CByteStream::CByteStream()
	: m_nMarker( 0 ),
		m_nChunkSize( 1024 ),
		m_nDataStored( 0 ),
		m_nDataAllocated( 0 ),
		m_pData( NULL ),
		m_bOwnsData( true )
{
}


// -----------------------------------------------------------------------------
//		CByteStream(void*,UInt32)
// -----------------------------------------------------------------------------

CByteStream::CByteStream(
	void*	inData,
	UInt32	inSize,
	bool	inOwnsData )
		: m_nMarker( 0 ),
			m_nChunkSize( 1024 ),
			m_nDataStored( inSize ),
			m_nDataAllocated( inSize ),
			m_pData( inData ),
			m_bOwnsData( inOwnsData )
{
}


// -----------------------------------------------------------------------------
//		CByteStream(const CByteStream&)
// -----------------------------------------------------------------------------

CByteStream::CByteStream(
	const CByteStream&	inOriginal )
		: m_nMarker( 0 ),
			m_nChunkSize( 1024 ),
			m_nDataStored( 0 ),
			m_nDataAllocated( 0 ),
			m_pData( NULL ),
			m_bOwnsData( true )
{
	// Copy the chunk size from the original.
	ASSERT( inOriginal.GetChunkSize() > 0 );
	SetChunkSize( inOriginal.GetChunkSize() );

	// Get the data length of the original.
	UInt32	nDataLength = inOriginal.GetLength();

	if ( nDataLength > 0 )
	{
		// Make a copy of the data.
		const void*	pData = inOriginal.GetData();
		WriteBytes( pData, nDataLength );

		// Copy the marker position from the original.
		SetMarker( inOriginal.GetMarker(), kMarker_Begin );
	}
}


// -----------------------------------------------------------------------------
//		~CByteStream
// -----------------------------------------------------------------------------

CByteStream::~CByteStream()
{
	if ( (m_pData != NULL) && m_bOwnsData )
	{
		// Free the data storage.
		FreeData( m_pData );
		m_pData = NULL;
	}
}


// -----------------------------------------------------------------------------
//		operator=
// -----------------------------------------------------------------------------

CByteStream&
CByteStream::operator=( 
	const CByteStream& inOriginal )
{
	if ( this != &inOriginal )
	{
		if ( m_pData == inOriginal.GetData() )
		{
			// Just copy the marker position.
			SetMarker( inOriginal.GetMarker(), kMarker_Begin );
		}
		else
		{
			if ( !m_bOwnsData )
			{
				// Release the data this object refers to.
				m_nMarker = 0;
				m_nDataStored = 0;
				m_nDataAllocated = 0;
				m_pData = NULL;
				m_bOwnsData = true;
			}

			// Copy the chunk size from the original.
			SetChunkSize( inOriginal.GetChunkSize() );

			// Get the data length of the original.
			UInt32	nDataLength = inOriginal.GetLength();

			if ( nDataLength > 0 )
			{
				// Make a copy of the data.
				SetMarker( 0, kMarker_Begin );
				WriteBytes( inOriginal.GetData(), nDataLength );

				// Copy the marker position from the original.
				SetMarker( inOriginal.GetMarker(), kMarker_Begin );
			}
		}
	}
	
	return *this;
}


// -----------------------------------------------------------------------------
//		SetMarker
// -----------------------------------------------------------------------------

void
CByteStream::SetMarker(
	UInt32		inOffset,
	TMarkerPos	inMarkerPos ) const
{
	switch ( inMarkerPos )
	{
		case kMarker_Begin:
		{
			m_nMarker = inOffset;
			if ( m_nMarker > m_nDataStored )
			{
				m_nMarker = m_nDataStored;
			}
		}
		break;

		case kMarker_Current:
		{
			m_nMarker += inOffset;
			if ( m_nMarker > m_nDataStored )
			{
				m_nMarker = m_nDataStored;
			}
		}
		break;

		case kMarker_End:
		{
			if ( inOffset < m_nDataStored )
			{
				m_nMarker = m_nDataStored - inOffset;
			}
			else
			{
				m_nMarker = 0;
			}
		}
		break;
	}
}


// -----------------------------------------------------------------------------
//		AcquireData
// -----------------------------------------------------------------------------

void*
CByteStream::AcquireData()
{
	void*	pData = m_pData;

	m_nMarker = 0;
	m_nDataStored = 0;
	m_nDataAllocated = 0;
	m_pData = NULL;
	m_bOwnsData = true;

	return pData;
}


// -----------------------------------------------------------------------------
//		FreeData [static]
// -----------------------------------------------------------------------------

void
CByteStream::FreeData(
	void*	pData )
{
	if ( pData != NULL )
	{
		free( pData );
	}
}

// -----------------------------------------------------------------------------
//		FreeExtra
// -----------------------------------------------------------------------------

UInt32			
CByteStream::FreeExtra()
{
	// calculate the actual bytes that we need based on the write pointer
	SetLength( m_nDataStored );
	return m_nDataStored;
}

// -----------------------------------------------------------------------------
//		SetLength
// -----------------------------------------------------------------------------

void
CByteStream::SetLength(
	UInt32	inNewLength )
{
	if ( !m_bOwnsData )
	{
		// Can't change the size of
		// data not allocated here.
		return;
	}

	if ( inNewLength == 0 )
	{
		m_nDataStored = 0;
		m_nDataAllocated = 0;
		m_nMarker = 0;

		if ( m_pData != NULL )
		{
			FreeData( m_pData );
			m_pData = NULL;
		}
	}
	else if ( inNewLength > m_nDataStored )
	{
		// Growing.
		CheckAllocation( inNewLength - m_nDataStored );
		m_nDataStored = inNewLength;
	}
	else if ( inNewLength < m_nDataStored )
	{
		// Shrinking.
		if ( inNewLength > (m_nDataAllocated - m_nChunkSize) )
		{
			// No need to realloc, just change the
			// size of data stored.
			m_nDataStored = inNewLength;
		}
		else
		{
			UInt32	nAllocationSize = inNewLength;

			if ( (nAllocationSize % m_nChunkSize) != 0 )
			{
				// Round up to the nearest chunk size.
				nAllocationSize += m_nChunkSize - (nAllocationSize % m_nChunkSize);
			}

			void*	pNewData;
			if ( m_pData == NULL )
			{
				// First time allocation.
				pNewData = malloc( nAllocationSize );
			}
			else
			{
				// Resize existing storage.
				pNewData = realloc( m_pData, nAllocationSize );
			}

			// Throw if memory error.
			if ( pNewData == NULL ) THROW_MEMORY_EXCEPTION;

			// Update the storage.
			m_pData = pNewData;
			m_nDataStored = inNewLength;
			m_nDataAllocated = nAllocationSize;
		}

		// Move the marker if the marker is no longer valid.
		if ( m_nMarker > m_nDataStored )
		{
			m_nMarker = m_nDataStored;
		}
	}
}


// -----------------------------------------------------------------------------
//		WriteBytes
// -----------------------------------------------------------------------------

bool
CByteStream::WriteBytes(
	const void*	inData,
	UInt32		inSize )
{
	// Sanity check.
	ASSERT( inSize != 0 );
	if ( inSize == 0 ) return false;

	if ( m_bOwnsData )
	{
		// Resize if necessary.
		if ( !CheckAllocation( inSize ) )
		{
			return false;
		}
	}

	// Check the size remaining.
	if ( m_nDataAllocated - m_nMarker < inSize )
	{
		// Not enough room.
		return false;
	}

	if ( inData != NULL )
	{
		void*	pData = GetPtrAtMarker();

		if ( inSize == sizeof(UInt8) )
		{
			*(UInt8*)pData = *(UInt8*)inData;
		}
		else if ( inSize == sizeof(UInt16) )
		{
			*(UInt16*)pData = *(UInt16*)inData;
		}
		else if ( inSize == sizeof(UInt32) )
		{
			*(UInt32*)pData = *(UInt32*)inData;
		}
		else
		{
			memcpy( pData, inData, inSize );
		}
	}

	// Move the marker.
	m_nMarker += inSize;
	
	// If we're writing beyond the previous amount of data
	// stored, add to the amount of data stored.
	if ( m_nMarker > m_nDataStored )
	{
		m_nDataStored = m_nMarker;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		ReadBytes
// -----------------------------------------------------------------------------

bool
CByteStream::ReadBytes(
	void*	outData,
	UInt32	inSize ) const
{
	// Sanity check.
	ASSERT( outData != NULL );
	if ( outData == NULL ) return false;
	ASSERT( inSize != 0 );
	if ( inSize == 0 ) return false;

	if ( (m_nMarker + inSize) <= m_nDataStored )
	{
		// Get a pointer to data at the marker.
		void*	pData = GetPtrAtMarker();

		// Handle common size requests.
		if ( inSize == sizeof(UInt32) )
		{
			*(UInt32*)outData = *(UInt32*)pData;
		}
		else if ( inSize == sizeof(UInt16) )
		{
			*(UInt16*)outData = *(UInt16*)pData;
		}
		else if ( inSize == sizeof(UInt8) )
		{
			*(UInt8*)outData = *(UInt8*)pData;
		}
		else
		{
			// Fall back on a full copy.
			memcpy( outData, pData, inSize );
		}

		// Move the marker.
		m_nMarker += inSize;

		return true;
	}
	else
	{
		// Not enough bytes left.
		m_nMarker = m_nDataStored;
		return false;
	}
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CByteStream::Write(
	const wchar_t*	psz )
{
	// Sanity check.
	ASSERT( psz != NULL );
	if ( psz == NULL ) return false;

	// Write it, not including the zero terminator.
	return WriteBytes( psz, (UInt32)(wcslen( psz ) * sizeof(wchar_t)) );
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CByteStream::Write(
	const char*	psz )
{
	// Sanity check.
	ASSERT( psz != NULL );
	if ( psz == NULL ) return false;

	// Write it, not including the zero terminator.
	return WriteBytes( psz, (UInt32)(strlen( psz ) * sizeof(char)) );
}


// -----------------------------------------------------------------------------
//		ReadStringW
// -----------------------------------------------------------------------------

const wchar_t*
CByteStream::ReadStringW() const
{
	const wchar_t*	pString = (const wchar_t*) GetPtrAtMarker();

	// Determine the string length.
	int		nStringLen;
	for ( nStringLen = 0; ((nStringLen*sizeof(wchar_t) + m_nMarker) < m_nDataStored) &&
		(pString[nStringLen] != 0); nStringLen++ )
	{
	}

	if ( (nStringLen*sizeof(wchar_t) + m_nMarker) < m_nDataStored )
	{
		// Move the marker past the end of the string.
		m_nMarker += (nStringLen + 1)*sizeof(wchar_t);

		return pString;
	}
	else
	{
		return NULL;
	}
}


// -----------------------------------------------------------------------------
//		ReadStringA
// -----------------------------------------------------------------------------

const char*
CByteStream::ReadStringA() const
{
	const char*	pString = (const char*) GetPtrAtMarker();

	// Determine the string length.
	int		nStringLen;
	for ( nStringLen = 0; ((nStringLen*sizeof(char) + m_nMarker) < m_nDataStored) &&
		(pString[nStringLen] != 0); nStringLen++ )
	{
	}

	if ( (nStringLen*sizeof(char) + m_nMarker) < m_nDataStored )
	{
		// Move the marker past the end of the string.
		m_nMarker += (nStringLen + 1)*sizeof(char);

		return pString;
	}
	else
	{
		return NULL;
	}
}


// -----------------------------------------------------------------------------
//		WriteString
// -----------------------------------------------------------------------------

bool
CByteStream::WriteString(
	const wchar_t*	psz )
{
	// Sanity check.
	ASSERT( psz != NULL );
	if ( psz == NULL ) return false;

	// Write it, INCLUDING the zero terminator.
	return WriteBytes( psz, (UInt32)(wcslen( psz ) + 1) * sizeof(wchar_t) );
}


// -----------------------------------------------------------------------------
//		WriteString
// -----------------------------------------------------------------------------

bool
CByteStream::WriteString(
	const char*	psz )
{
	// Sanity check.
	ASSERT( psz != NULL );
	if ( psz == NULL ) return false;

	// Write it, INCLUDING the zero terminator.
	return WriteBytes( psz, (UInt32)(strlen( psz ) + 1) * sizeof(char) );
}


#ifdef _WIN32
// -----------------------------------------------------------------------------
//		WriteToFile
// -----------------------------------------------------------------------------

bool
CByteStream::WriteToFile( 
	const wchar_t*	pszFullPath ) const
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	if ( GetLength() > 0 )
	{
		// Create and open the file.
		HANDLE	hFile = ::CreateFileW( pszFullPath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			return false;
		}
		
		DWORD	nBytesToWrite = (DWORD) GetLength();
		DWORD	nBytesWritten = 0;
		if ( !::WriteFile( hFile, GetData(),
			nBytesToWrite, &nBytesWritten, NULL ) )
		{
			// Close the file.
			::CloseHandle( hFile );
			return false;
		}
		
		// Close the file.
		::CloseHandle( hFile );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		WriteToFile
// -----------------------------------------------------------------------------

bool
CByteStream::WriteToFile( 
	const char*	pszFullPath ) const
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	if ( GetLength() > 0 )
	{
		// Create and open the file.
		HANDLE	hFile = ::CreateFileA( pszFullPath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			return false;
		}
		
		DWORD	nBytesToWrite = (DWORD) GetLength();
		DWORD	nBytesWritten = 0;
		if ( !::WriteFile( hFile, GetData(),
			nBytesToWrite, &nBytesWritten, NULL ) )
		{
			// Close the file.
			::CloseHandle( hFile );
			return false;
		}
		
		// Close the file.
		::CloseHandle( hFile );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		ReadFromFile
// -----------------------------------------------------------------------------

bool
CByteStream::ReadFromFile( 
	const wchar_t*	pszFullPath )
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	// Open the file.
	HANDLE	hFile = ::CreateFileW( pszFullPath, GENERIC_READ, FILE_SHARE_READ,
						NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	// Get the file size.
	UInt32	nFileSize = ::GetFileSize( hFile, NULL );

	// Setup the stream.
	SetLength( nFileSize );

	if ( nFileSize > 0 )
	{
		DWORD	nByteCount;
		if ( !::ReadFile( hFile, GetData(), nFileSize, &nByteCount, NULL ) )
		{
			// Close the file.
			::CloseHandle( hFile );
			
			// Return the result.
			return false;
		}
	}

	// Close the file.
	::CloseHandle( hFile );

	return true;
}


// -----------------------------------------------------------------------------
//		ReadFromFile
// -----------------------------------------------------------------------------

bool
CByteStream::ReadFromFile( 
	const char*	pszFullPath )
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	// Open the file.
	HANDLE	hFile = ::CreateFileA( pszFullPath, GENERIC_READ, FILE_SHARE_READ,
						NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	// Get the file size.
	UInt32	nFileSize = ::GetFileSize( hFile, NULL );

	// Setup the stream.
	SetLength( nFileSize );

	if ( nFileSize > 0 )
	{
		DWORD	nByteCount;
		if ( !::ReadFile( hFile, GetData(), nFileSize, &nByteCount, NULL ) )
		{
			// Close the file.
			::CloseHandle( hFile );
			
			// Return the result.
			return false;
		}
	}

	// Close the file.
	::CloseHandle( hFile );

	return true;
}

#else

// -----------------------------------------------------------------------------
//		WriteToFile
// -----------------------------------------------------------------------------

bool
CByteStream::WriteToFile( 
	const wchar_t*	pszFullPath ) const
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	const UInt32	cbData = GetLength();
	if ( cbData > 0 )
	{
		// Convert to multi-byte string. MSVC has _wfopen, but not others.
		const size_t	cchW = wcslen( pszFullPath );
		const size_t	cchA = (cchW + 1) * 4;
		const size_t	cbA  = cchA * sizeof(char);
		char*			psz  = static_cast<char*>(malloc( cbA ));
		if ( psz == NULL ) return false;
		if ( wcstombs( psz, pszFullPath, cbA ) == (size_t) -1 )
		{
			free( psz );
			return false;
		}

		// Create and open the file.
		// Note use of binary-mode - no text translations.
		FILE*	fp = fopen( psz, "wb" );
		free( psz );
		if ( fp == NULL )
		{
			return false;
		}
		
		if ( fwrite( GetData(), cbData, 1, fp ) != 1 )
		{
			// Close the file.
			fclose( fp );
			return false;
		}
		
		// Close the file.
		fclose( fp );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		WriteToFile
// -----------------------------------------------------------------------------

bool
CByteStream::WriteToFile( 
	const char*	pszFullPath ) const
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	const UInt32	cbData = GetLength();
	if ( cbData > 0 )
	{
		// Create and open the file.
		// Note use of binary-mode - no text translations.
		FILE*	fp = fopen( pszFullPath, "wb" );
		if ( fp == NULL )
		{
			return false;
		}
		
		if ( fwrite( GetData(), cbData, 1, fp ) != 1 )
		{
			// Close the file.
			fclose( fp );
			return false;
		}
		
		// Close the file.
		fclose( fp );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		ReadFromFile
// -----------------------------------------------------------------------------

bool
CByteStream::ReadFromFile( 
	const wchar_t*	pszFullPath )
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	// Convert to multi-byte string. MSVC has _wstat and _wfopen, but not others.
	const size_t	cchW = wcslen( pszFullPath );
	const size_t	cchA = (cchW + 1) * 4;
	const size_t	cbA  = cchA * sizeof(char);
	char*			psz  = static_cast<char*>(malloc( cbA ));
	if ( psz == NULL ) return false;
	if ( wcstombs( psz, pszFullPath, cbA ) == (size_t) -1 )
	{
		free( psz );
		return false;
	}

	// Get the file size.
	struct stat	st;
	memset( &st, 0, sizeof(st) );
	if ( stat( psz, &st ) != 0 )
	{
		free( psz );
		return false;
	}
	const UInt32	cbData = static_cast<UInt32>(st.st_size);

	// Open the file.
	// Note use of binary-mode - no text translations.
	FILE*	fp = fopen( psz, "rb" );
	free( psz );
	if ( fp == NULL )
	{
		return false;
	}

	// Set the stream length.
	SetLength( cbData );

	if ( cbData > 0 )
	{
		if ( fread( GetData(), cbData, 1, fp ) != 1 )
		{
			// Close the file.
			fclose( fp );
			return false;
		}
	}

	// Close the file.
	fclose( fp );

	return true;
}


// -----------------------------------------------------------------------------
//		ReadFromFile
// -----------------------------------------------------------------------------

bool
CByteStream::ReadFromFile( 
	const char*	pszFullPath )
{
	ASSERT( pszFullPath != NULL );
	if ( pszFullPath == NULL ) return false;
	ASSERT( pszFullPath[0] != 0 );
	if ( pszFullPath[0] == 0 ) return false;

	// Get the file size.
	struct stat	st;
	memset( &st, 0, sizeof(st) );
	if ( stat( pszFullPath, &st ) != 0 )
	{
		return false;
	}
	const UInt32	cbData = static_cast<UInt32>(st.st_size);

	// Open the file.
	// Note use of binary-mode - no text translations.
	FILE*	fp = fopen( pszFullPath, "rb" );
	if ( fp == NULL )
	{
		return false;
	}

	// Set the stream length.
	SetLength( cbData );

	if ( cbData > 0 )
	{
		if ( fread( GetData(), cbData, 1, fp ) != 1 )
		{
			// Close the file.
			fclose( fp );
			return false;
		}
	}

	// Close the file.
	fclose( fp );

	return true;
}

#endif


// -----------------------------------------------------------------------------
//		CheckAllocation
// -----------------------------------------------------------------------------

bool
CByteStream::CheckAllocation(
	UInt32	inBytesNeeded )
{
	ASSERT( m_bOwnsData );

	// Calculate the amount available.
	UInt32	nBytesAvailable = m_nDataAllocated - m_nDataStored;

	// Check against the amount required.
	if ( nBytesAvailable >= inBytesNeeded ) return true;

	// Calculate the allocation size.
	UInt32	nAllocationSize = m_nDataAllocated + m_nChunkSize;
	while ( nAllocationSize < (inBytesNeeded + m_nDataStored) )
	{
		nAllocationSize += m_nChunkSize;
	}

	// Do the allocation.
	void*	pNewData = NULL;
	if ( m_pData == NULL )
	{
		// First time allocation.
		// Note: realloc should be able to handle first-time
		// allocation, but this may be safer for cross-platform code.
		pNewData = malloc( nAllocationSize );
	}
	else
	{
		// Resize existing storage.
		pNewData = realloc( m_pData, nAllocationSize );
	}

	// Check allocation success. Throw if memory error.
	if ( pNewData == NULL ) THROW_MEMORY_EXCEPTION;	// ENOMEM

	// Set the new data.
	m_pData = pNewData;

#if defined(_DEBUG) && defined(_AFX)
//		TRACE1( "CByteStream malloc/realloc, size = %d\n", nAllocationSize );
#endif

	// Update the amount of storage allocated.
	m_nDataAllocated = nAllocationSize;

	return true;
}
