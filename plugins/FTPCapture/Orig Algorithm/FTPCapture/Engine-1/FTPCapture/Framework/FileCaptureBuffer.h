// =============================================================================
//	FileCaptureBuffer.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "CaptureBuffer.h"
#include "RefreshFile.h"


// =============================================================================
//		CFileCaptureBuffer
// =============================================================================

class CFileCaptureBuffer
	:	public CCaptureBuffer
	,	public CRefreshFile
{
protected:
	virtual bool CheckRefresh( const CPacket& inPacket ) {
		UInt16 nPacketLength = inPacket.GetPacketDataLength();
		if ( ((nPacketLength + GetCapacityUsed()) > m_nRefreshTriggerSize) ||
			 (nPacketLength > GetCapacityAvailable()) ) {
			return Refresh();
		}
		return Monitor();
	}

public:
	enum {
		kExtraBufferSize = 9001
	};

	;		CFileCaptureBuffer() {}
	;		CFileCaptureBuffer( const CPeekString& strFilePath, const CPeekString& strFileBaseName = CPeekString(),
				 UInt32 inRefreshFileType = kRefreshFileTypeNone, bool inAppendTimestring = false );
	virtual	~CFileCaptureBuffer() { Deactivate(); }

	CFileCaptureBuffer&	operator=( const CFileCaptureBuffer& inOther ) {
		if ( this != &inOther ) {
			if ( !Copy( inOther ) ) {
				ASSERT( 0 );
			}
		}
		return *this;
	}

	bool			Copy( const CFileCaptureBuffer& inOther );

	virtual bool	Monitor( bool inWriteIfEmpty = true ) {
		if ( IsTimeExpired() ) return Refresh( inWriteIfEmpty );
		return true;
	}

	virtual bool	Refresh( bool bWriteIfEmpty = false );

	bool			Save( bool bWriteIfEmpty = false );
	bool			SaveSelected( UInt32 inRangeCount, UInt64* inRanges );
	virtual bool	SetRefreshSize( const UInt32 inRefreshSize, const UInt32 inUnits );

	// Add a packet to the buffer
	void			WritePacket( CPacket& inPacket ) {
		if ( !m_bActivated ) return;
		CheckRefresh( inPacket );
		ASSERT( inPacket.GetPacketDataLength() < GetCapacityAvailable() );
		CCaptureBuffer::WritePacket( inPacket );
	}
};
