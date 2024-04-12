// =============================================================================
//	Packet.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "peekcore.h"
#include "Peek.h"
#include "ByteVectRef.h"
#include "Layers.h"
#include "PeekStream.h"
#include <memory>
#include <std-tr1.h>

class IPacketLayers;
class ITCPPacket;
class CPacketBuffer;
class CLayerEthernet;
class CLayerIP;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Packet
//
//	A Packet manages a single packet. The actual packet data may be attached
//	(owned or contained within a Capture Buffer) or detached (owned or contained
//	within the Packet object. The packet data may be shared by multiple Packet
//	objects, but not a Capture Buffer. A packet that is attached to a Capture
//	buffer may be Detached, this results in packet data being copied into a
//	buffer managed by the Packet.
//
//	Use the GetLayer methods to retrieve references to the Ethernet, Internet
//	Protocol (IP), Transmission Control Protocol (TCP) or User Datagram Protocol
//	(UDP) layers of the packet. A Layer contains references to the layer's
//	header and the remainder of the packet. The remainder of the packet may
//	include padding. Each specific Layer class will provide the specific Header
//	class and protocol specific attributes and payload information.
//
//	To determine if a packet contains a specific layer (protocol), call the
//	specific GetLayer method and then call the resulting object's IsValid
//	method.
//	The IP Layer class supports both IP version 4 and 6.
//
//	To further decode a Packet use the Packet Layers class to inspect all of
//	the layers (protocols) in the packet. The decoding of additional protocols
//	is plugin specific.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// =============================================================================
//		CPacketLayerInfo
// =============================================================================

class CPacketLayerInfo
{
protected:
	UInt16			m_ProtoSpec;
	CByteVectRef	m_bvHeader;
	CByteVectRef	m_bvData;

public:
	;			CPacketLayerInfo() : m_ProtoSpec( 0 ) {}

	CByteVectRef	GetData() const { return m_bvData; }
	CByteVectRef	GetHeader() const { return m_bvHeader; }
	UInt16			GetProtoSpec() const { return m_ProtoSpec; }

	void		Set( UInt16 inProtoSpec, CByteVectRef inHeader, CByteVectRef inData ) {
		m_ProtoSpec = inProtoSpec;
		m_bvHeader = inHeader;
		m_bvData = inData;
	}
};


// =============================================================================
//		CPacketLayers
// =============================================================================

class CPacketLayers
{
	friend class CPacket;

protected:
	HeLib::CHePtr<IPacketLayers>	m_spLayers;

	void		Error() {
		_ASSERTE( 0 );
		throw( E_FAIL );
	}

public:
	;			CPacketLayers( void* inPtr = NULL );
	virtual		~CPacketLayers();

	bool		GetDataLayer( UInt16 intProtoSpecId, UInt32* outSourceProtoSpec, const UInt8** outData, UInt16* outBytesRemaining );
	bool		GetHeaderLayer( UInt16 inProtoSpecId, UInt32* outSourceProtoSpec, const UInt8** outData, UInt16* outBytesRemaining );
	bool		GetLayerInfo( UInt16 inProtoSpecId, CPacketLayerInfo& outInfo );
	void		GetPacketLayers( UInt32* outLayerCount, PacketLayerInfo outLayerInfo[] );
	bool		IsNotValid() const { return (m_spLayers == NULL); }
	bool		IsValid() const { return (m_spLayers != NULL); }
	void		ReleasePtr() { m_spLayers.Release(); }
	void		SetPtr( void* inPtr ) { m_spLayers = reinterpret_cast<IPacketLayers*>( inPtr );}
};


// =============================================================================
//		CPacketData
// =============================================================================

class CPacketData
{
protected:
	const UInt8*	m_pData;

public:
	;		CPacketData( const UInt8* inData = NULL ) : m_pData( inData ) {}
	;		~CPacketData() { Dealloc( m_pData ); }

	static UInt8*	Alloc( size_t inLength ) { return reinterpret_cast<UInt8*>( malloc( inLength ) ); }
	static void		Dealloc( const UInt8* inData ) { delete inData; }

	;		operator const UInt8*() const { return m_pData; }

	size_t	GetLength() const { return (m_pData) ? _msize( (void*)m_pData ) : 0; }

	bool	IsEmpty() const { return (m_pData == NULL); }
	bool	IsNotValid() const { return (m_pData == NULL); }
	bool	IsValid() const { return (m_pData != NULL); }
};

typedef NSTR1::shared_ptr<CPacketData>	CPacketDataPtr;


// =============================================================================
//		CPacket
// =============================================================================

class CPacket
{
	friend class CBasePacketBuffer;
	friend class CCaptureBuffer;
	friend class CPacketBuffer;
	friend class CPacketHandler;

public:
	enum {
		FLOW_ID_INVALID = 0,
		CALL_ID_INVALID = 0,
		CALL_FLOW_INDEX_INVALID = -1
	};

protected:
	struct tPacketInfo {
		PeekPacket*		pPacketHeader;
		const UInt8*	pPacketData;
		TMediaType		nMediaType;
		TMediaSubType	nMediaSubType;
	};

protected:
	CHePtr<IPacket>		m_spPacket;
	CPacketLayers		m_Layers;
	CPacketDataPtr		m_spData;

	const UInt8*	AllocFrom( IPacket* inIPacket, tPacketInfo& outInfo );
	UInt8*			AllocFrom( const PeekPacket* inPacket, const UInt8* inData, tPacketInfo& outInfo );

	void		Create() {
		Reset();
		CHePtr<IPacket>	spPacket;
		Peek::ThrowIfFailed( spPacket.CreateInstance( "PeekCore.Packet" ) );
		SetPtr( spPacket );
	}

	void		Error() {
		_ASSERTE( 0 );
		throw( E_FAIL );
	}

	IPacket*	GetIPacketPtr() const { return m_spPacket; }
	bool		GetLayers() {
		if ( !HaveLayers() ) {
			m_Layers = GetPacketLayers();
		}
		return HaveLayers();
	}
	bool		HaveLayers() const { return m_Layers.IsValid(); }

	static UInt16			InternalGetActualLength( IPacket* inIPacket ) {
		UInt16	nLength = 0;
		if ( inIPacket ) {
			Peek::ThrowIfFailed( inIPacket->GetActualLength( &nLength ) );
		}
		return nLength;
	}
	static tMediaType		InternalGetMediaType( IPacket* inIPacket ) {
		tMediaType	mt( kMediaType_802_3, kMediaSubType_Native );
		if ( inIPacket ) {
			Peek::ThrowIfFailed( inIPacket->GetMediaType( &mt.fType ) );
			Peek::ThrowIfFailed( inIPacket->GetMediaSubType( &mt.fSubType ) );
		}
		return mt;
	}

private:
	CPacket&	operator=( void* ) throw() {
		_ASSERTE( 0 );
	}

public:
	static size_t	SizeOf( const CPacket& inPacket );

protected:
	void			ReleaseData() { m_spData.reset(); }
	void			ReleaseLayers() { m_Layers.ReleasePtr(); }
	void			ReleasePtr() { m_spPacket.Release(); }
	void			SetPtr( void* inPtr ) {
		Reset();
		m_spPacket = reinterpret_cast<IPacket*>( inPtr );
	}

public:
	;				CPacket( void* inPtr = NULL );
	;				CPacket( const CPacket& inPacket, bool inDetach = false ) {
		Copy( inPacket, inDetach );
	}
	;				CPacket( const PeekPacket* inPacket, const UInt8* inData,
						tMediaType inMediaType ) {
		Create( inPacket, inData, inMediaType );
	}
	virtual			~CPacket() { Reset(); }

	CPacket&		operator=( const CPacket& inOther ) throw() {
		if ( this == &inOther ) return *this;	// self assignment.
		Copy( inOther );
		return *this;
	}
	bool			operator==( CPacket& inOther ) throw() {
		return IsEqual( inOther );
	}
	bool			operator!=( CPacket& inOther ) throw() {
		return !IsEqual( inOther );
	}

	void			ClearFlag( UInt32 nFlag ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->ClearFlag( nFlag ) );
		}
	}
	void			ClearStatus( UInt32 nStatus ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->ClearStatus( nStatus ) );
		}
	}
	void			Copy( const CPacket& inOther, bool inDetach = false ) {
		Reset();
		m_spPacket = inOther.m_spPacket;
		m_spData = inOther.m_spData;
		if ( inDetach && inOther.IsAttached() ) {
			Detach();
		}
	}
	void			Create( const PeekPacket* inPacket, const UInt8* inData,
						tMediaType inMediaType );

	void			Detach();
	void			Duplicate( CPacket& outOther ) {
		outOther.Reset();
		outOther.m_spPacket = m_spPacket;
		outOther.m_spData = m_spData;
		outOther.Detach();
	}

	UInt32			GetFlags() const {
		UInt32	nFlags = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetFlags( &nFlags ) );
		}
		return nFlags;
	}
	UInt32			GetFlowId() const {
		UInt32	nFlowId = FLOW_ID_INVALID;	// No flow id.
		if ( IsValid() ) {
			// Don't throw on failure, fails if no id.
			m_spPacket->GetFlowID( &nFlowId );
		}
		return nFlowId;
	}
	bool			GetLayer( CLayerEthernet& outLayer );
	bool			GetLayer( CLayerIP& outLayer );
	bool			GetLayer( CLayerTCP& outLayer );
	bool			GetLayer( CLayerUDP& outLayer );
	tMediaType		GetMediaType() const { return InternalGetMediaType( m_spPacket ); }
	CPacketBuffer	GetPacketBuffer() const;
	UInt16			GetPacketData( const UInt8*& pData ) const;
	CPeekStream		GetPacketData() const {
		CPeekStream		psData;
		const UInt8*	pData = NULL;
		UInt16			nDataLen = GetPacketData( pData );
		if ( nDataLen > 0 ) psData.Write( nDataLen, pData );
		return psData;
	}
	CByteVectRef	GetPacketDataRef() const {
		const UInt8*	pData = NULL;
		UInt16			nDataLen = GetPacketData( pData );
		CByteVectRef	bvData( nDataLen, pData );
		return bvData;
	}
	UInt16			GetPacketDataLength() const { return InternalGetActualLength( m_spPacket ); }
	CPacketLayers	GetPacketLayers();
	UInt16			GetPacketLength() const {
		UInt16	nLength = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetPacketLength( &nLength ) );
		}
		return nLength;
	}
	UInt64			GetPacketNumber() const;
	PeekPacket		GetPeekPacket() const {
		const PeekPacket*	pHeader = NULL;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetPacketHeader( &pHeader ) );
		}
		PeekPacket	Header;
		if ( pHeader != NULL ) {
			Header = *pHeader;
		}
		else {
			memset( &Header, 0, sizeof( PeekPacket ) );
		}
		return Header;
	}
	UInt32			GetProtoSpec() const {
		UInt32	nProtoSpec = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetProtoSpec( &nProtoSpec ) );
		}
		return nProtoSpec;
	}
	UInt16			GetProtoSpecId() const {
		UInt16	nProtoSpecId = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetProtoSpecID( &nProtoSpecId ) );
		}
		return nProtoSpecId;
	}
	UInt16			GetSliceLength() const {
		UInt16	nLength = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetSliceLength( &nLength ) );
		}
		return nLength;
	}
	UInt32			GetStatus() const {
		UInt32	nStatus = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetStatus( &nStatus ) );
		}
		return nStatus;
	}
	UInt64			GetTimeStamp() const {
		UInt64	nTimeStamp = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->GetTimeStamp( &nTimeStamp ) );
		}
		return nTimeStamp;
	}
	UInt32			GetVoIPCallFlowIndex() const {
		UInt32	nIndex = static_cast<UInt32>( CALL_FLOW_INDEX_INVALID );	// No VoIP Call Flow Index.
		if ( IsValid() ){
			// Don't throw on failure, fails if no index.
			m_spPacket->GetVoIPCallFlowIndex( &nIndex );
		}
		return nIndex;
	}
	UInt64			GetVoIPCallId() const {
		UInt64	nCallId = CALL_ID_INVALID;	// No VoIP Call Id.
		if ( IsValid() ) {
			// Don't throw on failure, fails if no id.
			m_spPacket->GetVoIPCallID( &nCallId );
		}
		return nCallId;
	}

	bool			HasData() const {
		if ( IsValid() ) {
			CByteVectRef	refData = GetPacketDataRef();
			return !refData.IsEmpty();
		}
		return false;
	}

	bool			IsAttached() const { return (IsValid() && !IsDataOwner()); }
	bool			IsBroadcast() const	{
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsBroadcast( &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			IsDataOwner() const { return (m_spData != NULL); }
	bool			IsDescendentOf( const UInt16 inParents[], SInt32 inCount,
						UInt32* outMatchInstanceId, SInt32* outMatchIndex ) const {
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsDescendentOf( inParents, inCount,
				outMatchInstanceId, outMatchIndex, &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			IsDetached() const { return (IsValid() && IsDataOwner()); }
	bool			IsEmpty() const { return (GetPacketLength() == 0); }
	bool			IsEqual( const CPacket& inPacket ) const;
	bool			IsError() const {
		BOOL	bResult = TRUE;	// If not valid the it's an error packet.
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsError( &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			IsFullDuplex() const {
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsFullDuplexPacket( &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			IsMultiCast() const {
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsMulticast( &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			IsNotValid() const { return (m_spPacket == NULL); }
	bool			IsValid() const { return (m_spPacket != NULL); }
	bool			IsWAN() const {
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsWanPacket( &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			IsWireless() const {
		BOOL		bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->IsWirelessPacket( &bResult ) );
		}
		return (bResult != FALSE);
	}

	void			Reset() { ReleaseLayers(); ReleasePtr(); ReleaseData(); }

	void			SetData( const UInt8* inData ) {
		CPacketDataPtr	sp( new CPacketData( inData ) );
		m_spData = sp;
	}
	void			SetFlag( UInt32 inFlag ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->SetFlag( inFlag ) );
		}
	}
	void			SetStatus( UInt32 inStatus ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->SetStatus( inStatus ) );
		}
	}

	bool			TestFlag( UInt32 inFlag ) const {
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->TestFlag( inFlag, &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool			TestStatus( UInt32 inStatus ) const {
		BOOL	bResult = false;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacket->TestStatus( inStatus, &bResult ) );
		}
		return (bResult != FALSE);
	}
};
