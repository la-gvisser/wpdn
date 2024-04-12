// =============================================================================
//	readerlist.h
// =============================================================================
//	Copyright (c) 2013 WildPackets, Inc. All rights reserved.

#pragma once

#include "fileitem.h"
#include ".\CaptureFile\WPTypes.h"
#include ".\CaptureFile\MediaTypes.h"
#include <vector>


// =============================================================================
//		CReaderList
// =============================================================================

class CReaderList
{
protected:
	tMediaType				m_MediaType;
	std::vector<CFileItem>	m_ayFiles;
	UInt64					m_nStartTime;
	bool					m_bInterleave;
	size_t					m_nNextFile;

	static void	CloseFile( CFileItem& inItem ) {
		inItem.Close();
	}

public:
	;	CReaderList( bool inInterleave = false )
		:	m_nStartTime( 0 )
		,	m_bInterleave( inInterleave )
		,	m_nNextFile( 0 )
	{}
	;	~CReaderList() {}

	void	Close() {
		std::for_each( m_ayFiles.begin(), m_ayFiles.end(), CReaderList::CloseFile );
	}

	tMediaType	GetMediaType() const { return m_MediaType; }
	bool		GetNextPacket( CaptureFile::CFilePacket& outPacket );

	bool		IsInterleave() const { return m_bInterleave; }

	size_t	Load( const CStringList& inFiles );
};
