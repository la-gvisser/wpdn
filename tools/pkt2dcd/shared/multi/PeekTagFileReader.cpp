// ============================================================================
//  PeekTagFileReader.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.

#include <stdafx.h>
#include <time.h>
#include <memutil.h>
#include "PeekTagFileReader.h"

// ============================================================================
//  CPeekTagFileReader
// ============================================================================

// ----------------------------------------------------------------------------
//		constructor
// ----------------------------------------------------------------------------

CPeekTaggedFileReader::CPeekTaggedFileReader( FILE* pFile )
{	// initialize first block header
	// rewind
	m_pFile = pFile;
	m_blockPosition = 0;	// the current block starts at 0
	m_bTaggedFile = false;
	m_blockPtr = 0;

	if (m_pFile)
	{
		fseek( m_pFile, 0, SEEK_SET );
		// read the block header
		if ( fread( &m_blockHeader, sizeof(m_blockHeader), 1, m_pFile ) == 1 )
		{
			m_blockHeader.nType = MemUtil::Swap32(m_blockHeader.nType);
			m_bTaggedFile = (m_blockHeader.nType == TaggedFile::kCaptureFileVersionBlock);
		}
	}
}

// ----------------------------------------------------------------------------
//		destructor
// ----------------------------------------------------------------------------

CPeekTaggedFileReader::~CPeekTaggedFileReader()
{
	if (m_pFile)
		fclose( m_pFile );
}


// ----------------------------------------------------------------------------
//		Attach
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::Attach( FILE* pFile )
{
	m_pFile = pFile;
	m_blockPosition = ftell(pFile);	// the current block starts at 0
	m_bTaggedFile = true;
	m_blockPtr = 0;
	return true;
}


// ----------------------------------------------------------------------------
//		Attach
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::GetBlockHeader( )
{
	// read the block header
	if ( fread( &m_blockHeader, sizeof(m_blockHeader), 1, m_pFile ) == 1 )
	{
		m_blockHeader.nType = MemUtil::Swap32(m_blockHeader.nType);
		m_bTaggedFile = (m_blockHeader.nType == TaggedFile::kCaptureFileVersionBlock);
	}

	return true;
}

// ----------------------------------------------------------------------------
//		Detach
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::Detach()
{
	m_pFile = NULL;
	return true;
}

// ----------------------------------------------------------------------------
//		IsTaggedFile
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::IsTaggedFile() const
{
	return m_bTaggedFile;
}

// ----------------------------------------------------------------------------
//		IsEOF
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::IsEOF() const
{
	return (feof(m_pFile) != 0);
}

// ----------------------------------------------------------------------------
//		IsEOB
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::IsEOB() const
{
	if (m_blockHeader.nLength == 0)
		return IsEOF();
	return (m_blockPtr >= m_blockHeader.nLength);
}

// ----------------------------------------------------------------------------
//		GetBlockType
// ----------------------------------------------------------------------------

TaggedFile::BlockType CPeekTaggedFileReader::GetBlockType() const
{
	return m_blockHeader.nType;
}

// ----------------------------------------------------------------------------
//		GetFormat
// ----------------------------------------------------------------------------

TaggedFile::FormatType CPeekTaggedFileReader::GetFormat() const
{
	return (TaggedFile::FormatType) ((m_blockHeader.nFlags & 0xff00) >> 8);
}

// ----------------------------------------------------------------------------
//		GetBlockLength
// ----------------------------------------------------------------------------

UInt32 CPeekTaggedFileReader::GetBlockLength() const
{
	return m_blockHeader.nLength;
}

// ----------------------------------------------------------------------------
//		GetBlockData
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::GetBlockData( UInt8* pBuffer, UInt32 nBuffSize )
{	// read some bytes
	if ( m_blockHeader.nLength != 0 && 
		 nBuffSize > m_blockHeader.nLength + m_blockPtr )
		return false;

	if ( fread( pBuffer, sizeof(UInt8), nBuffSize, m_pFile ) == nBuffSize )
	{	// update position
		m_blockPtr += nBuffSize;
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
//		NextBlock
// ----------------------------------------------------------------------------

bool CPeekTaggedFileReader::NextBlock()
{
	if (m_blockHeader.nLength == 0)	// this is the last block
	{
		fseek(m_pFile, 0, SEEK_END);
		m_blockPtr = 0;
		return (feof(m_pFile) == 0);
	}

	fseek( m_pFile, m_blockHeader.nLength + m_blockPosition + sizeof(m_blockHeader), SEEK_SET );
	m_blockPosition += m_blockHeader.nLength + sizeof(m_blockHeader);

	if (fread(&m_blockHeader, sizeof(m_blockHeader), 1, m_pFile) == 1)
	{	// byte swap the version number
		m_blockHeader.nType = MemUtil::Swap32(m_blockHeader.nType);
		m_blockPtr = 0;
		if (m_blockHeader.nLength == 0)
		{	// calculate based on the remaining file length
			UInt32 nCur = ftell(m_pFile);
			fseek(m_pFile, 0, SEEK_END);
			m_blockHeader.nLength = ftell(m_pFile) - nCur;
			fseek(m_pFile, nCur, SEEK_SET);
		}
		return (!IsEOF());
	}

	return false;
}




// -----------------------------------------------------------------------------
//		GetFilePosition
// -----------------------------------------------------------------------------

int
CPeekTaggedFileReader::GetFilePosition() const
{
	return ftell( m_pFile );
}


// -----------------------------------------------------------------------------
//		SetFilePosition
// -----------------------------------------------------------------------------

int
CPeekTaggedFileReader::SetFilePosition( int	nPos )
{
	m_blockPosition = nPos;
	m_blockPtr = 0;
	return fseek( m_pFile, nPos, SEEK_SET );
}
