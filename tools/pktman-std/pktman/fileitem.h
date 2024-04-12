// =============================================================================
//	fileitem.h
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#include "CaptureFile/WPTypes.h"
#include "CaptureFile/CaptureFile.h"
#include <filesystem>

using std::filesystem::path;


// =============================================================================
//		CFileItem
// =============================================================================

class CFileItem
{
protected:
	CaptureFile::CCaptureFile	m_File;
	bool						m_bEOF;
	UInt64						m_nStartTime;
	CaptureFile::CFilePacket	m_Packet;

public:
	;	CFileItem( CaptureFile::CCaptureFile& inFile )
		:	m_File( inFile )
		,	m_bEOF( false )
		,	m_nStartTime( 0 )
	{}
	;	CFileItem( const CFileItem& inOther )
		:	m_File( inOther.m_File )
		,	m_bEOF( inOther.m_bEOF )
		,	m_nStartTime( inOther.m_nStartTime )
		,	m_Packet( inOther.m_Packet )
	{}
	;	CFileItem( const path& inPath, const CaptureFile::tFileMode& inMode )
		:	m_File( inPath, inMode )
		,	m_bEOF( false )
		,	m_nStartTime( 0 )
	{}
	;	CFileItem( const path& inPath, CaptureFile::CF_FILE_TYPE inType )
		:	m_File( inPath, inType )
		,	m_bEOF( false )
		,	m_nStartTime( 0 )
	{}
	virtual	~CFileItem() {
		m_File.Close();
	}

	operator const CaptureFile::CCaptureFile*() const { return &m_File; }

	void	Close() {
		m_File.Close();
		m_bEOF = true;
	}

	bool	IsEOF() const { return m_bEOF; }
	bool	IsValid() const { return m_File.IsOpen(); }

	UInt64	GetDeltaTime() const {
		return m_Packet.GetTimeStamp() - m_nStartTime;
	}
	const CaptureFile::CFilePacket&	GetPacket() const { return m_Packet; }

	bool	ReadFirst() {
		UInt32	nError( 0 );
		if ( m_File.ReadFilePacket( m_Packet, nError ) ) {
			m_bEOF = false;
			m_nStartTime = m_Packet.GetTimeStamp();
			return true;
		}
		m_bEOF = true;
		return false;
	}
	bool	ReadNext() {
		if ( !m_bEOF ) {
			UInt32	nError( 0 );
			if ( m_File.ReadFilePacket( m_Packet, nError ) ) {
				return true;
			}
			m_bEOF = true;
		}
		return false;
	}
};
