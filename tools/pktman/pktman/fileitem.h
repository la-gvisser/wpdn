// =============================================================================
//	fileitem.h
// =============================================================================
//	Copyright (c) 2013 WildPackets, Inc. All rights reserved.

#pragma once

#include ".\CaptureFile\WPTypes.h"
#include ".\CaptureFile\CaptureFile.h"


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
		,	m_nStartTime( 0 )
		,	m_bEOF( false )
	{}
	;	CFileItem( CFileItem& inOther )
		:	m_File( inOther.m_File )
		,	m_bEOF( inOther.m_bEOF )
		,	m_nStartTime( inOther.m_nStartTime )
		,	m_Packet( inOther.m_Packet )
	{}
	;	CFileItem( const std::wstring& inFileName, const CaptureFile::_tFileMode& inMode )
		:	m_File( inFileName, inMode )
		,	m_nStartTime( 0 )
		,	m_bEOF( false )
	{}
	;	~CFileItem() {}

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
		if ( m_File.ReadFilePacket( m_Packet, 0 ) ) {
			m_bEOF = false;
			m_nStartTime = m_Packet.GetTimeStamp();
			return true;
		}
		m_bEOF = true;
		return false;
	}
	bool	ReadNext() {
		if ( !m_bEOF ) {
			if ( m_File.ReadFilePacket( m_Packet ) ) {
				return true;
			}
			m_bEOF = true;
		}
		return false;
	}
};
