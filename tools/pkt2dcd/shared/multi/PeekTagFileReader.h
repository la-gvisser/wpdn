// ============================================================================
//  PeekTagFileReader.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.

#pragma once

#include "TagFile.h"

class CPeekTaggedFileReader 
{
public:
	CPeekTaggedFileReader( FILE* pFile = NULL );
	virtual ~CPeekTaggedFileReader();

	bool Detach( );
	bool Attach( FILE* pFile );
	bool GetBlockHeader( );
	UInt32 GetBlockHeaderLength( ) { return sizeof( TaggedFile::BLOCK_HEADER ); }

	bool IsTaggedFile() const;
	bool IsEOF() const;
	bool IsEOB() const;
	TaggedFile::BlockType GetBlockType() const;
	TaggedFile::FormatType GetFormat() const;
	UInt32 GetBlockLength() const;
	bool GetBlockData( UInt8* pBuffer, UInt32 nBuffSize );

	bool NextBlock();

	int			GetFilePosition() const;
	int			SetFilePosition( int nPos );

protected:
	FILE*	m_pFile;
	TaggedFile::BLOCK_HEADER m_blockHeader;
	UInt32  m_blockPosition;	// file pos of the current block header
	UInt32  m_blockPtr;			// position in the block (byte count)
	bool	m_bTaggedFile;
};