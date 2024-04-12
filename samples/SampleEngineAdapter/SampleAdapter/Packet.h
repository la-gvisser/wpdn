// =============================================================================
//	Packet.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"


// =============================================================================
//		CPacketLayers
// =============================================================================

class CPacketLayers
{
	friend class CPacket;

protected:
	void*		m_pvLayers;

protected:
	void*		GetPtr() { return m_pvLayers; }
	void		ReleasePtr();
	void		SetPtr( void* p );

public:
	;			CPacketLayers( void* p = NULL );
	virtual		~CPacketLayers();

	int			GetHeaderLayer( UInt16 ProtoSpecId, UInt32* pSourceProtoSpec,
		const UInt8** ppData, UInt16* pBytesRemaining  );
};


// =============================================================================
//		CPacket
// =============================================================================

class CPacket
{
protected:
	void*		m_pvPacket;

protected:
	IPacket*	GetPtr() const;
	void		ReleasePtr();
	void		SetPtr( void* p );

public:
	;			CPacket( void* p = NULL );
	virtual		~CPacket();

	int			ClearFlag( UInt32 nFlag );
	int			ClearStatus( UInt32 nStatus );

	bool		ExtractText( const unsigned char* pData, size_t nDataLen,
		bool bTerm, unsigned char ucTerm, char cNonPrint, char* pText,
		size_t nTextLen );

	int			GetActualLength( UInt16& nLength );
	int			GetApplicationLayer( void**	pLayer );
	int			GetFlags( UInt32& nFlags );
	int			GetFlowId( UInt32& nId );
	int			GetIPFragmentBits( UInt16& nBits );
	int			GetIPFragmentOffset( UInt16& nOffset );
	int			GetIPId( UInt16& nId );
	int			GetIPLength( UInt16& nLength );
	int			GetMediaSubType( TMediaSubType& tMediaSubType );
	int			GetMediaType( TMediaType& tMediaType );
	int			GetPacketData( UInt16& nLength, const UInt8*& pData );
	int			GetPacketLayers( CPacketLayers& inLayers );
	int			GetPacketLength( UInt16& nLength );
	int			GetPacketNumber( UInt64& nPacketNumber );
	int			GetProtoSpec( UInt32& nProtoSpec );
	int			GetProtoSpecId( UInt16& nProtoSpecId );
	int			GetSliceLength( UInt16& nLength );
	int			GetStatus( UInt32& nStatus );
	int			GetTCPUDPInfo( UInt8** ppHeader, UInt16* pusHeaderBytes,
		UInt8** ppPayload, UInt16* pusPayloadBytes, UInt16* pusTransportID );
	int			GetTimeStamp( UInt64& nTimeStamp );
	int			GetVoIPCallFlowIndex( UInt32& nIndex );
	int			GetVoIPCallId( UInt64& nId );

	int			IsBroadcast( bool& bResult );
	int			IsDescendentOf( const UInt16 payParent[], SInt32 nCount,
		UInt32* pMatchInstanceId, SInt32* pMatchIndex, bool& bResult );
	int			IsError( bool& bResult );
	int			IsFragment( bool& bResult );
	int			IsFullDuplex( bool& bResult );
	int			IsMultiCaset( bool& bResult );
	int			IsWAN( bool& bResult );
	int			IsWireless( bool& bResult );

	int			SetFlag( UInt32 nFlag );
	int			SetStatus( UInt32 nStatus );

	int			TestFlag( UInt32 nFlag, bool& bResult );
	int			TestStatus( UInt32 nStatus, bool& bResult );
};
