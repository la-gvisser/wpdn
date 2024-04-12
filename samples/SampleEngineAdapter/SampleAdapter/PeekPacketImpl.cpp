// =============================================================================
//	peekpacketimpl.cpp
// =============================================================================
//	Copyright (c) 2001-2008 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include "peekpacketimpl.h"
#include "PSIDs.h"
#include "MediaSpecUtil.h"


namespace
{
	const UInt32 FLOW_ID_INVALID			= 0;
	const UInt32 CALL_FLOW_INDEX_INVALID	= static_cast<UInt32>(-1);
	const UInt64 CALL_ID_INVALID			= 0;

	const UInt16 MORE_FRAGMENTS				= 0x2000;
	const UInt16 FRAGMENT_OFFSET_MASK		= 0x1FFF;

	const UInt8 TCP_OPTION_END				= 0x00;
	const UInt8 TCP_OPTION_NOOP				= 0x01;
	const UInt8 TCP_OPTION_MSS				= 0x02;
	const UInt8 TCP_OPTION_WINDOW_SCALE		= 0x03;
	const UInt8 TCP_OPTION_SELECTIVE_ACK	= 0x05;
	const UInt8 TCP_OPTION_TIMESTAMP		= 0x08;

	inline bool IsFragmentBits( const UInt16 fragbits )
	{
		// If no more fragments (MORE_FRAGMENTS == 0) and no previous
		// fragment (FRAGMENT_OFFSET_MASK == ) then we're not a fragment,
		// we're a normal, unfragmented, packet.
		return ((fragbits & (MORE_FRAGMENTS | FRAGMENT_OFFSET_MASK)) != 0);
	}
}


// -----------------------------------------------------------------------------
//		Packet
// -----------------------------------------------------------------------------

Packet::Packet() :
	m_pPacketBuffer( NULL ),
	m_PacketBufferIndex( 0 ),
	m_PacketNumber( 0 ),
	m_pPacketHeader(  NULL ),
	m_pPacketData( NULL ),
	m_pPacketDataEnd( NULL ),
	m_MediaType( kMediaType_802_3 ),
	m_MediaSubType( kMediaSubType_802_11_gen ),
	m_ActualLength( 0 ),
	m_bPacketNumber( false ),
	m_bProtocol( false ),
	m_bCalcPhysical( false ),
	m_bCalcLogical( false ),
	m_bCalcPorts( false ),
	m_bBSSID( false ),
	m_bReceiver( false ),
	m_bTransmitter( false ),
	m_bAddress1( false ),
	m_bAddress2( false ),
	m_bAddress3( false ),
	m_bAddress4( false ),
	m_bTransmitterImplied( false ),
	m_bTransmitterAuthenticationType( false ),
	m_bTransmitterEncryptionType( false ),
	m_bReceiverEncryptionType( false ),
	m_bWEPKeyIndexChecked( false ),
	m_bWEPKeyIndexValid( false ),
	m_bIPHeader( false ),
	m_bTCPHeader( false ),
	m_bTCPOptions( false ),
	m_bProtoSpecArray( false ),
	m_nProtoSpecCount( 0 ),
	m_pAddress1( NULL ),
	m_pAddress2( NULL ),
	m_pAddress3( NULL ),
	m_pAddress4( NULL ),
	m_TransmitterEncryptionType( peekEncryptionTypeNone ),
	m_ReceiverEncryptionType( peekEncryptionTypeNone ),
	m_TransmitterAuthenticationType( peekAuthenticationTypeNone ),
	m_WEPKeyIndex( -1 ),
	m_FlowID( FLOW_ID_INVALID ),
	m_VoIPCallFlowIndex( CALL_FLOW_INDEX_INVALID ),
	m_VoIPCallID( CALL_ID_INVALID )
{
}


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
Packet::FinalConstruct()
{
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
Packet::FinalRelease()
{
	m_spAppLayer.Release();
}


// -----------------------------------------------------------------------------
//		Initialize
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::Initialize(
	IPacketBuffer*	pPacketBuffer,
	UInt32			nPacketBufferIndex,
	PeekPacket*		pPacketHeader,
	const UInt8*	pPacketData,
	TMediaType		mt,
	TMediaSubType	mst )
{
	ObjectLock	lock( this );

	m_pPacketBuffer = pPacketBuffer;
	m_PacketBufferIndex = nPacketBufferIndex;
	m_pPacketHeader = pPacketHeader;
	m_pPacketData = pPacketData;
	m_pPacketDataEnd = NULL;
	m_MediaType = mt;
	m_MediaSubType = mst;
	m_ActualLength = 0;
	m_bPacketNumber = 0;
	m_bProtocol = false;
	m_bCalcPhysical = false;
	m_bCalcLogical = false;
	m_bCalcPorts = false;
	m_bBSSID = false;
	m_bReceiver = false;
	m_bTransmitter = false;
	m_bAddress1 = false;
	m_bAddress2 = false;
	m_bAddress3 = false;
	m_bAddress4 = false;
	m_bTransmitterImplied = false;
	m_bTransmitterAuthenticationType = false;
	m_bTransmitterEncryptionType = false;
	m_bReceiverEncryptionType = false;
	m_bWEPKeyIndexChecked = false;
	m_bWEPKeyIndexValid = false;
	m_bIPHeader = false;
	m_bTCPHeader = false;
	m_bTCPOptions = false;
	m_bProtoSpecArray = false;
	m_nProtoSpecCount = 0;
	m_pAddress1 = NULL;
	m_pAddress2 = NULL;
	m_pAddress3 = NULL;
	m_pAddress4 = NULL;
	m_spAppLayer.Release();
	m_FlowID = FLOW_ID_INVALID;
	m_VoIPCallFlowIndex = CALL_FLOW_INDEX_INVALID;
	m_VoIPCallID = CALL_ID_INVALID;
	// Leave other data as is - the flags above will control access.

	if ( pPacketHeader != NULL )
	{
		// Get the actual packet length.
		m_ActualLength = pPacketHeader->fSliceLength;
		if ( m_ActualLength == 0 )
		{
			m_ActualLength = pPacketHeader->fPacketLength;
		}
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketHeader
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketHeader(
	const PeekPacket**	ppPacketHeader )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppPacketHeader != NULL );
	if ( ppPacketHeader == NULL ) return HE_E_NULL_POINTER;

	*ppPacketHeader = m_pPacketHeader;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketData
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketData(
	const UInt8**	ppPacketData )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppPacketData != NULL );
	if ( ppPacketData == NULL ) return HE_E_NULL_POINTER;

	*ppPacketData = m_pPacketData;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketDataEnd
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketDataEnd(
	const UInt8**	ppPacketDataEnd )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppPacketDataEnd != NULL );
	if ( ppPacketDataEnd == NULL ) return HE_E_NULL_POINTER;

	*ppPacketDataEnd = m_pPacketDataEnd;

	if ( m_pPacketDataEnd != NULL )
	{
		// Use the cached value.
		return HE_S_OK;
	}

	// More sanity checks.
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;
	if ( m_pPacketData == NULL ) return HE_E_FAIL;
	if ( m_pPacketHeader->fPacketLength < 4 ) return HE_E_FAIL;

	const UInt16	nPacketLength = m_pPacketHeader->fPacketLength;
	const UInt16	nSliceLength  = m_pPacketHeader->fSliceLength;
	const UInt16	nDataLength   = nPacketLength - 4;

	if ( m_pPacketHeader->fSliceLength == 0 )
	{
		// Not slicing: use end of data minus FCS.
		m_pPacketDataEnd = m_pPacketData + nDataLength;
	}
	else
	{
		// Slicing: so use that, unless slice lands after FCS begins.
		m_pPacketDataEnd = m_pPacketData + min( nSliceLength, nDataLength );
	}

	// If small IPv4 packet, honor its total length value (dodge extra
	// padding bytes for some less-than-64-byte-long packets).
	if ( (nDataLength <= 70) && CalcIPHeader() )
	{
		// Get IPv4 header.
		const UInt8*	pHeader;
		UInt16			cbHeader;
		HeResult 		hr = GetHeaderLayer( ProtoSpecDefs::IP, NULL, &pHeader, &cbHeader );
		if ( HE_SUCCEEDED(hr) )
		{
			// Find IP data end.
			const UInt8*	pIPDataEnd	= pHeader + m_IPTotalLength;
			// If IP data ends before our slice/packet-length guess, use IP data end.
			// TODO: this could be incorrect for IPv4 tunneled over some other 
			// protocol that includes data after the tunneled payload
			if ( pIPDataEnd < m_pPacketDataEnd )
			{
				ASSERT( m_pPacketData < pIPDataEnd );
				m_pPacketDataEnd = pIPDataEnd;
			}
		}
	}

	*ppPacketDataEnd = m_pPacketDataEnd;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketMediaSpecificInfo
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketMediaSpecificInfo(
	const UInt8**	ppMediaSpecificInfo,
	UInt32*			pulMediaSpecificInfoLength )
{
	ObjectLock	lock( this );

	// Sanity checks.
	if ( ppMediaSpecificInfo == NULL ) return HE_E_NULL_POINTER;
	if ( pulMediaSpecificInfoLength == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*ppMediaSpecificInfo = m_pPacketHeader->fMediaSpecInfoBlock;

	if ( m_pPacketHeader->fMediaSpecInfoBlock != NULL )
	{
		MediaSpecificPrivateHeader*	pMediaHdr = 
			reinterpret_cast<MediaSpecificPrivateHeader*>(m_pPacketHeader->fMediaSpecInfoBlock);

		ASSERT( pMediaHdr->nSize != 0 );
		*pulMediaSpecificInfoLength = pMediaHdr->nSize;
	}
	else
	{
		*pulMediaSpecificInfoLength = 0;
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketBuffer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketBuffer(
	IPacketBuffer**	ppPacketBuffer )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppPacketBuffer != NULL );
	if ( ppPacketBuffer == NULL ) return HE_E_NULL_POINTER;

	if ( m_pPacketBuffer != NULL )
	{
		m_pPacketBuffer->AddRef();
	}

	*ppPacketBuffer = m_pPacketBuffer;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketBufferIndex
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketBufferIndex(
	UInt32*	pIndex )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pIndex != NULL );
	if ( pIndex == NULL ) return HE_E_NULL_POINTER;

	*pIndex = m_PacketBufferIndex;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketNumber
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketNumber(
	UInt64*	pPacketNumber )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pPacketNumber != NULL );
	if ( pPacketNumber == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bPacketNumber )
	{
		ASSERT( m_pPacketBuffer != NULL );
		if ( m_pPacketBuffer == NULL ) return HE_E_FAIL;

		HeResult	hr = m_pPacketBuffer->PacketIndexToPacketNumber(
									m_PacketBufferIndex, &m_PacketNumber );
		if ( HE_FAILED(hr) )
		{
			*pPacketNumber = 0;
			return hr;
		}
		m_bPacketNumber = true;
	}

	*pPacketNumber = m_PacketNumber;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetMediaType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetMediaType(
	TMediaType*	pMediaType )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pMediaType != NULL );
	if ( pMediaType == NULL ) return HE_E_NULL_POINTER;

	*pMediaType = m_MediaType;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetMediaSubType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetMediaSubType(
	TMediaSubType*	pMediaSubType )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pMediaSubType != NULL );
	if ( pMediaSubType == NULL ) return HE_E_NULL_POINTER;

	*pMediaSubType = m_MediaSubType;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketLength
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketLength(
	UInt16*	pcbPacketLength )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pcbPacketLength != NULL );
	if ( pcbPacketLength == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pcbPacketLength = m_pPacketHeader->fPacketLength;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetSliceLength
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSliceLength(
	UInt16*	pcbSliceLength )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pcbSliceLength != NULL );
	if ( pcbSliceLength == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pcbSliceLength = m_pPacketHeader->fSliceLength;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetActualLength
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetActualLength(
	UInt16*	pcbActualLength )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pcbActualLength != NULL );
	if ( pcbActualLength == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pcbActualLength = m_ActualLength;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetFlags
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetFlags(
	UInt32*	pFlags )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pFlags != NULL );
	if ( pFlags == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pFlags = m_pPacketHeader->fFlags;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetStatus
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetStatus(
	UInt32*	pStatus )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pStatus != NULL );
	if ( pStatus == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pStatus = m_pPacketHeader->fStatus;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTimeStamp
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTimeStamp(
	UInt64*	pnTimeStamp )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pnTimeStamp != NULL );
	if ( pnTimeStamp == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pnTimeStamp = m_pPacketHeader->fTimeStamp;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetProtoSpec
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetProtoSpec(
	UInt32*	pProtoSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	if ( pProtoSpec == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pProtoSpec = GetProtoSpec();

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetProtoSpecID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetProtoSpecID(
	UInt16*	pProtoSpecID )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pProtoSpecID != NULL );
	if ( pProtoSpecID == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pProtoSpecID = GET_PSID( GetProtoSpec() );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		SetFlag
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::SetFlag(
	UInt32	f )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	m_pPacketHeader->fFlags |= f;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		ClearFlag
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::ClearFlag(
	UInt32	f )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	m_pPacketHeader->fFlags &= ~f;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		TestFlag
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::TestFlag(
	UInt32	f,
	BOOL*	pbResult )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( pbResult != NULL );
	if ( pbResult == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pbResult = ((m_pPacketHeader->fFlags & f) != 0);

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		SetStatus
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::SetStatus(
	UInt32	s )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	m_pPacketHeader->fStatus |= s;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		ClearStatus
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::ClearStatus(
	UInt32	s )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	m_pPacketHeader->fStatus &= ~s;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		TestStatus
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::TestStatus(
	UInt32	s,
	BOOL*	pbResult )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( pbResult != NULL );
	if ( pbResult == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	*pbResult = ((m_pPacketHeader->fStatus & s) != 0);

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IsWirelessPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsWirelessPacket(
	BOOL*	pbWirelessPacket )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pbWirelessPacket != NULL );
	if ( pbWirelessPacket == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	// Wireless packets have a wireless media info block.
	const MediaSpecificPrivateHeader*	pMediaHdr = 
			reinterpret_cast<const MediaSpecificPrivateHeader*>(m_pPacketHeader->fMediaSpecInfoBlock);
	*pbWirelessPacket = (pMediaHdr != NULL) && (pMediaHdr->nType == kMediaSpecificHdrType_Wireless3);

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IsFullDuplexPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsFullDuplexPacket(
	BOOL*	pbFullDuplexPacket )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pbFullDuplexPacket != NULL );
	if ( pbFullDuplexPacket == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	// Full duplex packets have a full duplex media info block.
	const MediaSpecificPrivateHeader*	pMediaHdr = 
			reinterpret_cast<const MediaSpecificPrivateHeader*>(m_pPacketHeader->fMediaSpecInfoBlock);
	*pbFullDuplexPacket = (pMediaHdr != NULL) && (pMediaHdr->nType == kMediaSpecificHdrType_FullDuplex);

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IsWanPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsWanPacket(
	BOOL*	pbWanPacket )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pbWanPacket != NULL );
	if ( pbWanPacket == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	// WAN packets have a WAN media info block.
	const MediaSpecificPrivateHeader*	pMediaHdr = 
			reinterpret_cast<const MediaSpecificPrivateHeader*>(m_pPacketHeader->fMediaSpecInfoBlock);
	*pbWanPacket = (pMediaHdr != NULL) && (pMediaHdr->nType == kMediaSpecificHdrType_Wan);

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IsError
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsError(
	BOOL*	pbError )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( pbError != NULL );
	if ( pbError == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	static const UInt32	f = kPacketFlag_CRC  | kPacketFlag_Frame |
							kPacketFlag_Runt | kPacketFlag_Oversize;
	*pbError = ((m_pPacketHeader->fFlags & f) != 0);

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IsMulticast
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsMulticast(
	BOOL*	pbMulticast )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( pbMulticast != NULL );
	if ( pbMulticast == NULL ) return HE_E_NULL_POINTER;

	CMediaSpec	spec;
	HeResult	hr = GetDestPhysical( &spec );
	if ( HE_SUCCEEDED(hr) )
	{
		*pbMulticast = spec.IsMulticast();
	}
	else
	{
		*pbMulticast = FALSE;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		IsBroadcast
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsBroadcast(
	BOOL*	pbBroadcast )
{
	ObjectLock	lock( this );

	// Sanity check.
	ASSERT( pbBroadcast != NULL );
	if ( pbBroadcast == NULL ) return HE_E_NULL_POINTER;

	CMediaSpec	spec;
	HeResult	hr = GetDestPhysical( &spec );
	if ( HE_SUCCEEDED(hr) )
	{
		*pbBroadcast = spec.IsBroadcast();
	}
	else
	{
		*pbBroadcast = FALSE;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		IsDescendentOf
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsDescendentOf(
	const UInt16	pParentArray[],
	SInt32			lCount,
	UInt32*			pulMatchInstanceID,
	SInt32*			plMatchIndex,
	BOOL*			pbResult )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pParentArray != NULL );
	if ( pParentArray == NULL ) return HE_E_NULL_POINTER;
	ASSERT( lCount > 0 );
	if ( lCount <= 0 ) return HE_E_NULL_POINTER;
	// pulMatchInstanceID may be NULL.
	// plMatchIndex may be NULL.
	ASSERT( pbResult != NULL );
	if ( pbResult == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bProtoSpecArray )
	{
		// Build the layer information.
		CalcProtoSpecArray();

		// Set the flag to tell that we've cached the layer info.
		m_bProtoSpecArray = true;
	}

	// Walk the array and check against each parent ID.
	const int		nProtoSpecCount    = m_nProtoSpecCount;
	const UInt16*	ayProtoSpecIDArray = m_ProtoSpecIDArray;
	for ( int i = 0; i < nProtoSpecCount; ++i )
	{
		const UInt16	nPSID = ayProtoSpecIDArray[i];
		for ( int j = 0; j < lCount; ++j )
		{
			if ( pParentArray[j] == nPSID )
			{
				// Got a match.
				if ( pulMatchInstanceID != NULL )
				{
					*pulMatchInstanceID = m_ProtoSpecArray[i].nInstID;
				}
				if ( plMatchIndex != NULL )
				{
					*plMatchIndex = j;
				}
				*pbResult = TRUE;

				return HE_S_OK;
			}
		}
	}

	// No match found.
	if ( pulMatchInstanceID != NULL )
	{
		*pulMatchInstanceID = 0;
	}
	if ( plMatchIndex != NULL )
	{
		*plMatchIndex = -1;
	}
	*pbResult = FALSE;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IsFragment
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsFragment(
	BOOL*	pbFragment )
{
	ObjectLock	lock( this );

	ASSERT( pbFragment != NULL );
	if ( pbFragment == NULL ) return HE_E_NULL_POINTER;
	
	if ( IS_WIRELESS( m_MediaSubType ) )
	{
		// Wireless fragment?
		UInt8	nWirelessFragmentNumber;
		if ( GetWirelessFragmentNumber( &nWirelessFragmentNumber ) == HE_S_OK )
		{
			*pbFragment = TRUE;
			return HE_S_OK;
		}
	}

	// IPv4 fragment?
	UInt16	nIPFragmentOffset;
	if ( GetIPFragmentOffset( &nIPFragmentOffset ) == HE_S_OK )
	{
		*pbFragment = TRUE;
		return HE_S_OK;
	}

	// Not a fragment.
	*pbFragment = FALSE;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetIPFragmentOffset
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetIPFragmentOffset(
	UInt16*	pFragmentOffset )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pFragmentOffset != NULL );
	if ( pFragmentOffset == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcIPHeader() )
	{
		*pFragmentOffset = 0;
		return HE_E_FAIL;
	}

	*pFragmentOffset = (m_IPFragmentBits & FRAGMENT_OFFSET_MASK) << 3;

	return IsFragmentBits( m_IPFragmentBits ) ? HE_S_OK : HE_S_FALSE;
}


// -----------------------------------------------------------------------------
//		GetIPFragmentBits
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetIPFragmentBits(
	UInt16*	pFragmentBits )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pFragmentBits != NULL );
	if ( pFragmentBits == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcIPHeader() )
	{
		*pFragmentBits = 0;
		return HE_E_FAIL;
	}

	*pFragmentBits = m_IPFragmentBits;

	return IsFragmentBits( m_IPFragmentBits ) ? HE_S_OK : HE_S_FALSE;
}


// -----------------------------------------------------------------------------
//		GetIPID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetIPID(
	UInt16*	pIPID )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pIPID != NULL );
	if ( pIPID == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcIPHeader() )
	{
		*pIPID = 0;
		return HE_E_FAIL;
	}

	*pIPID = m_IPID;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetIPLength
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetIPLength(
	UInt16*	pIPLength )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pIPLength != NULL );
	if ( pIPLength == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcIPHeader() )
	{
		*pIPLength = 0;
		return HE_E_FAIL;
	}

	*pIPLength = m_IPTotalLength;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetAppLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetAppLayer(
	IHeUnknown**	ppAppLayer )
{
	ObjectLock	lock( this );

	return m_spAppLayer.CopyTo( ppAppLayer );
}


// -----------------------------------------------------------------------------
//		SetAppLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::SetAppLayer(
	IHeUnknown*	pAppLayer )
{
	ObjectLock	lock( this );

	m_spAppLayer = pAppLayer;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetFlowID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetFlowID(
	UInt32*	pFlowID )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pFlowID != NULL );
	if ( pFlowID == NULL ) return HE_E_NULL_POINTER;

	*pFlowID = m_FlowID;

	return (m_FlowID != FLOW_ID_INVALID) ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		SetFlowID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::SetFlowID(
	UInt32	nFlowID )
{
	ObjectLock	lock( this );

	m_FlowID = nFlowID;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetVoIPCallID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetVoIPCallID(
	UInt64*	pCallID )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pCallID != NULL );
	if ( pCallID == NULL ) return HE_E_NULL_POINTER;

	*pCallID = m_VoIPCallID;

	return (m_VoIPCallID != CALL_ID_INVALID) ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		SetVoIPCallID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::SetVoIPCallID(
	UInt64	nCallID )
{
	ObjectLock	lock( this );

	m_VoIPCallID = nCallID;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetVoIPCallFlowIndex
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetVoIPCallFlowIndex(
	UInt32*	pCallFlowIndex )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pCallFlowIndex != NULL );
	if ( pCallFlowIndex == NULL ) return HE_E_NULL_POINTER;

	*pCallFlowIndex = m_VoIPCallFlowIndex;

	return (m_VoIPCallFlowIndex != CALL_FLOW_INDEX_INVALID) ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		SetVoIPCallFlowIndex
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::SetVoIPCallFlowIndex(
	UInt32	nCallFlowIndex )
{
	ObjectLock	lock( this );

	m_VoIPCallFlowIndex = nCallFlowIndex;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetPacketLayers
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPacketLayers(
	UInt32*			pulLayerCount,
	PacketLayerInfo	pLayerInfo[] )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pulLayerCount != NULL );
	if ( pulLayerCount == NULL ) return HE_E_NULL_POINTER;
//	ASSERT( pLayerInfo != NULL );
//	if ( pLayerInfo == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_FAIL;

	if ( !m_bProtoSpecArray )
	{
		// Build the layer information.
		CalcProtoSpecArray();

		// Set the flag to tell that we've cached the layer info.
		m_bProtoSpecArray = true;
	}

	if ( m_nProtoSpecCount > 0 )
	{
		// Check the info count given.
		if ( *pulLayerCount < (UInt32) m_nProtoSpecCount )
		{
			// Return the count required.
			*pulLayerCount = m_nProtoSpecCount;
			return HE_E_INVALID_ARG;
		}

		if ( pLayerInfo != NULL )
		{
			// Copy the layer info.
			for ( int i = 0; i < m_nProtoSpecCount; i++ )
			{
				pLayerInfo[i].ulProtoSpec = m_ProtoSpecArray[i].nInstID;
				pLayerInfo[i].pHeader     = (UInt8*) m_ProtoSpecArray[i].pHeader;
				pLayerInfo[i].pPayload    = (UInt8*) m_ProtoSpecArray[i].pPayload;
			}
		}

		// Return the count.
		*pulLayerCount = m_nProtoSpecCount;

		hr = HE_S_OK;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetHeaderLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetHeaderLayer(
	UInt16			usProtoSpecID,
	UInt32*			pulSourceProtoSpec,
	const UInt8**	ppData,
	UInt16*			pusBytesRemaining )
{
	ObjectLock	lock( this );

	// Sanity checks.
	// Note: pulSourceProtoSpec may be NULL.
	ASSERT( ppData != NULL );
	if ( ppData == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	// Initialize "out" params.
	*ppData = NULL;
	if ( pusBytesRemaining != NULL ) *pusBytesRemaining = 0;

	HeResult	hr = HE_E_FAIL;

	if ( !m_bProtoSpecArray )
	{
		// Build the layer information.
		CalcProtoSpecArray();

		// Set the flag to tell that we've cached the layer info.
		m_bProtoSpecArray = true;
	}

	const int	nProtoSpecCount = m_nProtoSpecCount;
	if ( nProtoSpecCount > 0 )
	{
		const UInt16	nPSID = (usProtoSpecID != 0) ? usProtoSpecID : GET_PSID( GetProtoSpec() );
		const UInt16*	ayProtoSpecIDArray = m_ProtoSpecIDArray;
		for ( int i = 0; i < nProtoSpecCount; ++i )
		{
			if ( ayProtoSpecIDArray[i] == nPSID )
			{
				const ProtoSpecs::SProtospecInfo*	pLayer = &m_ProtoSpecArray[i];

				if ( pulSourceProtoSpec != NULL )
				{
					*pulSourceProtoSpec = pLayer->nInstID;
				}

				const UInt8*	pHeader = pLayer->pHeader;
				*ppData = pHeader;

				if ( pusBytesRemaining != NULL )
				{
					const UInt8*	pPacketData = m_pPacketData;
					if ( (pPacketData != NULL) && (pHeader >= pPacketData) )
					{
						// Get the packet data length.
						const UInt16	cbConsumed = static_cast<UInt16>(pHeader - pPacketData);
						const UInt16	cbTotal    = m_ActualLength;
						if ( cbConsumed < cbTotal )
						{
							*pusBytesRemaining = cbTotal - cbConsumed;
						}
						else
						{
							*pusBytesRemaining = 0;
						}
					}
					else
					{
						*pusBytesRemaining = 0;
					}
				}

				hr = HE_S_OK;
				break;
			}
		}
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetDataLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetDataLayer(
	UInt16			usProtoSpecID,
	UInt32*			pulSourceProtoSpec,
	const UInt8**	ppData,
	UInt16*			pusBytesRemaining )
{
	ObjectLock	lock( this );

	// Sanity checks.
	// Note: pulSourceProtoSpec may be NULL.
	ASSERT( ppData != NULL );
	if ( ppData == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	HeResult	hr = HE_E_FAIL;

	// Initialize "out" params.
	*ppData = NULL;
	if ( pusBytesRemaining != NULL ) *pusBytesRemaining = 0;

	if ( !m_bProtoSpecArray )
	{
		// Build the layer information.
		CalcProtoSpecArray();

		// Set the flag to tell that we've cached the layer info.
		m_bProtoSpecArray = true;
	}

	if ( m_nProtoSpecCount > 0 )
	{
		UInt16	psid = usProtoSpecID;
		if ( psid == 0 ) psid = GET_PSID( GetProtoSpec() );

		for ( int i = 0; i < m_nProtoSpecCount; ++i )
		{
			if ( m_ProtoSpecIDArray[i] == psid )
			{
				if ( pulSourceProtoSpec != NULL )
				{
					*pulSourceProtoSpec = m_ProtoSpecArray[i].nInstID;
				}

				*ppData = (UInt8*) m_ProtoSpecArray[i].pPayload;

				if ( (pusBytesRemaining != NULL) &&
					(m_pPacketData != NULL) &&
					(*ppData != NULL) )
				{
					// Get the packet data length.
					UInt16	usBytesConsumed = (UInt16)(*ppData - m_pPacketData);
//					ASSERT( usBytesConsumed <= m_ActualLength );
					if ( usBytesConsumed < m_ActualLength )
					{
						*pusBytesRemaining = m_ActualLength - usBytesConsumed;
					}
					else
					{
						*pusBytesRemaining = 0;
					}
				}

				hr = HE_S_OK;
				break;
			}
		}
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetPhysicalLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetPhysicalLayer(
	const UInt8**	ppData,
	UInt16*			pusBytesRemaining )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppData != NULL );
	if ( ppData == NULL ) return HE_E_NULL_POINTER;

	// Physical layer starts with packet data for all current media types.
	*ppData = m_pPacketData;

	if ( pusBytesRemaining != NULL )
	{
		*pusBytesRemaining = m_ActualLength;
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetLinkLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetLinkLayer(
	const UInt8**	ppData,
	UInt16*			pusBytesRemaining )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppData != NULL );
	if ( ppData == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	HeResult	hr = HE_E_FAIL;

	// Initialize "out" params.
	*ppData = NULL;
	if ( pusBytesRemaining != NULL ) *pusBytesRemaining = 0;

	// Get the physical layer.
	const UInt8*	pData = NULL;
	UInt16			usDataBytes = 0;
	hr = GetPhysicalLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return hr;

	switch ( m_MediaType )
	{
		case kMediaType_802_3:
		{
			switch ( m_MediaSubType )
			{
				case kMediaSubType_Native:
				{
					// TODO: Add support for VLAN tagging here?
					// Skip the Ethernet header.
					if ( usDataBytes > 14 )
					{
						// Increment the data ptr.
						pData += 14;

						if ( pusBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*pusBytesRemaining = (UInt16)(usDataBytes - 14);
						}
					}
					else
					{
						pData = NULL;
					}
				}
				break;

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:
				{
					if ( usDataBytes >= 2 )
					{
						// Get the 802.11 MAC header.
						Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

						switch ( pHdr->FrameControlType & 0x0C )
						{
							case 0x00:	// Management
							{
								if ( usDataBytes > 24 )
								{
									pData += 24;

									if ( pusBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*pusBytesRemaining = (UInt16)(usDataBytes - 24);
									}
								}
								else
								{
									pData = NULL;
								}
							}
							break;

							case 0x04:	// Control
							{
								// Never any link data for control packets.
								pData = NULL;
							}
							break;

							case 0x08:	// Data
							{
								// Get the MAC header length.
								UInt16	usMACLen = (UInt16)(((pHdr->FrameControlFlags & 0x03) == 0x03) ? 30 : 24);
								if( (pHdr->FrameControlType & 0xF0) == 0x80 )
								{
									usMACLen += 2;	// Frame has QoS, adjust.
								}
								if ( usDataBytes > usMACLen )
								{
									pData += usMACLen;

									if ( pusBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*pusBytesRemaining = (UInt16)(usDataBytes - usMACLen);
									}
								}
								else
								{
									pData = NULL;
								}
							}
							break;
						}
					}
				}
				break;

				default:
				{
					// Don't know how to handle this media type.
					ASSERT( false );
					pData = NULL;
				}
				break;
			}
		}
		break;

		case kMediaType_802_5:
		{
			// TODO: Handle MAC Ctl packets differently?
			// Skip Token Ring header (need 1 byte
			// to check for source routing info).
			if ( usDataBytes >= 15 )
			{
				// Get the source routing info length.
				UInt16	usSourceRouteInfoLength = 0;
				if ( (m_pPacketHeader->fFlags & kPacketFlag_RouteInfo) != 0 )
				{
					usSourceRouteInfoLength = (UInt16)(pData[14] & 0x1F);
				}

				// Calculate the total number of bytes to skip.
				UInt16	usBytesToSkip = (UInt16)(14 + usSourceRouteInfoLength);

				if ( usDataBytes > usBytesToSkip )
				{
					// Increment the data ptr.
					pData += usBytesToSkip;

					if ( pusBytesRemaining != NULL )
					{
						// Update bytes remaining.
						*pusBytesRemaining = (UInt16)(usDataBytes - usBytesToSkip);
					}
				}
				else
				{
					pData = NULL;
				}
			}
			else
			{
				pData = NULL;
			}
		}
		break;

		case kMediaType_CoWan:
		case kMediaType_Wan:
		{	
			// Get the real media type from the protocol field.
			UInt8	nWanProtocol;
			hr = GetWanProtocol( &nWanProtocol );
			if ( HE_FAILED(hr) ) return hr;
			TMediaSubType	mst;
			HE_VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol( m_MediaType, nWanProtocol, &mst ) == 0 );

			switch ( mst )
			{
				case kMediaSubType_wan_ppp:			// WAN PPP
				{
					// Skip the WAN PPP header.
					if ( usDataBytes > 2 )
					{
						// Increment the data ptr.
						pData += 2;

						if ( pusBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*pusBytesRemaining = (UInt16)(usDataBytes - 2);
						}
					}
					else
					{
						pData = NULL;
					}
				}
				break;

				case kMediaSubType_wan_frameRelay:	// Frame Relay
				{
					// Skip the WAN Frame Relay header.
					if ( usDataBytes > 2 )
					{
						// Increment the data ptr.
						pData += 2;

						if ( pusBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*pusBytesRemaining = (UInt16)(usDataBytes - 2);
						}
					}
					else
					{
						pData = NULL;
					}
				}
				break;

				case kMediaSubType_wan_x25:			// X.25
				case kMediaSubType_wan_x25e:		// X.25 modulo 128
				case kMediaSubType_wan_Q931:		// Q.931
				{
				}
				break;

				case kMediaSubType_wan_ipars:		// IPARS
				case kMediaSubType_wan_u200:		// U200
				{
					// Don't know how to handle this physical medium.
					ASSERT( false );
					pData = NULL;
				}
				break;

				default:
				{
					// Don't know how to handle this physical medium.
					ASSERT( false );
					pData = NULL;
				}
				break;
			}
		}
		break;

		default:
		{
			// Don't know how to handle this physical medium.
			ASSERT( false );
			pData = NULL;
		}
		break;
	}

	if ( pData != NULL )
	{
		*ppData = pData;
		hr = HE_S_OK;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetNetworkLayer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetNetworkLayer(
	const UInt8**	ppData,
	UInt16*			pusBytesRemaining )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppData != NULL );
	if ( ppData == NULL ) return HE_E_NULL_POINTER;
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return HE_E_FAIL;

	HeResult	hr = HE_E_FAIL;

	// Initialize "out" params.
	*ppData = NULL;
	if ( pusBytesRemaining != NULL ) *pusBytesRemaining = 0;

	// Get the link layer.
	const UInt8*	pData = NULL;
	UInt16			usDataBytes = 0;
	hr = GetLinkLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return hr;

	switch ( m_MediaType )
	{
		case kMediaType_802_3:
		{
			switch ( m_MediaSubType )
			{
				case kMediaSubType_Native:
				{
					// Test for and skip 802.2 LLC header.
					// Otherwise, it's already there.
					if ( (m_pPacketHeader->fFlags & kPacketFlag_SNAP) != 0 )
					{
						if ( usDataBytes >= 2 )
						{
							if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
							{
								// Skip 802.2 LLC header with SNAP.
								if ( usDataBytes > 8 )
								{
									// Increment the data ptr.
									pData += 8;

									if ( pusBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*pusBytesRemaining = (UInt16)(usDataBytes - 8);
									}
								}
								else
								{
									pData = NULL;
								}
							}
							else if ( (pData[0] == 0xFF) && (pData[1] == 0xFF) )
							{
								// Good 'ole NetWare raw.
								// No need to increment the data ptr.
								if ( pusBytesRemaining != NULL )
								{
									// Update bytes remaining.
									*pusBytesRemaining = usDataBytes;
								}
							}
							else
							{
								// Skip 802.2 LLC header.
								if ( usDataBytes > 3 )
								{
									// Skip 3 bytes for unnumbered LSAP, otherwise 4 bytes.
									UInt16	usSkip = ((pData[2] & 0x03) == 0x03) ? 3 : 4;

									if ( usDataBytes > usSkip )
									{
										// Increment the data ptr 
										pData += usSkip;

										if ( pusBytesRemaining != NULL )
										{
											// Update bytes remaining.
											*pusBytesRemaining = (UInt16)(usDataBytes - usSkip);
										}
									}
									else
									{
										pData = NULL;
									}
								}
								else
								{
									pData = NULL;
								}
							}
						}
						else
						{
							pData = NULL;
						}
					}
					else
					{
						// There is no 802.2 LLC header.
						if ( pusBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*pusBytesRemaining = usDataBytes;
						}
					}
				}
				break;

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:
				{
					// Skip 802.2 LLC header.
					if ( usDataBytes > 8 )
					{
						// Increment the data ptr.
						pData += 8;

						if ( pusBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*pusBytesRemaining = (UInt16)(usDataBytes - 8);
						}
					}
					else
					{
						pData = NULL;
					}
				}
				break;

				default:
				{
					ASSERT(0);
					pData = NULL;
				}	
				break;
			}
		}
		break;

		case kMediaType_802_5:
		{
			// TODO: Handle MAC Ctl packets differently?
			// Test for and skip 802.2 LLC header.
			// Otherwise, it's already there.
			if ( usDataBytes >= 2 )
			{
				if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
				{
					// Skip 802.2 LLC header with SNAP.
					if ( usDataBytes > 8 )
					{
						// Increment the data ptr.
						pData += 8;

						if ( pusBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*pusBytesRemaining = (UInt16)(usDataBytes - 8);
						}
					}
					else
					{
						pData = NULL;
					}
				}
				else if ( (pData[0] == 0xFF) && (pData[1] == 0xFF) )
				{
					// Good 'ole NetWare raw.
					// No need to increment the data ptr.
					if ( pusBytesRemaining != NULL )
					{
						// Update bytes remaining.
						*pusBytesRemaining = usDataBytes;
					}
				}
				else
				{
					// Skip 802.2 LLC header.
					if ( usDataBytes > 3 )
					{
						// Skip 3 bytes for unnumbered LSAP, otherwise 4 bytes.
						UInt16	usSkip = ((pData[2] & 0x03) == 0x03) ? 3 : 4;

						if ( usDataBytes > usSkip )
						{
							// Increment the data ptr 
							pData += usSkip;

							if ( pusBytesRemaining != NULL )
							{
								// Update bytes remaining.
								*pusBytesRemaining = (UInt16)(usDataBytes - usSkip);
							}
						}
						else
						{
							pData = NULL;
						}
					}
					else
					{
						pData = NULL;
					}
				}
			}
			else
			{
				pData = NULL;
			}
		}
		break;

		case kMediaType_CoWan:
		case kMediaType_Wan:
		{
			// get the real media type from the protocol field
			UInt8	nWanProtocol;
			hr = GetWanProtocol( &nWanProtocol );
			if ( HE_FAILED(hr) ) return hr;
			TMediaSubType	mst;
			HE_VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				m_MediaType, nWanProtocol, &mst ) == 0 );

			switch ( mst )
			{
				case kMediaSubType_wan_ppp:			// PPP
				case kMediaSubType_wan_frameRelay:	// Frame Relay
				case kMediaSubType_wan_x25:			// X.25
				case kMediaSubType_wan_x25e:		// X.25 modulo 128
				case kMediaSubType_wan_ipars:		// IPARS
				case kMediaSubType_wan_u200:		// U200
				case kMediaSubType_wan_Q931:		// BRI Q931
				{
					// There is no 802.2 LLC header.
					if ( pusBytesRemaining != NULL )
					{
						// Update bytes remaining.
						*pusBytesRemaining = (UInt16) usDataBytes;
					}
				}
				break;

				default:
				{
					ASSERT( false );
					pData = NULL;
				}
				break;
			}
		}
		break;

		default:
		{
			// Don't know how to handle this media type.
			ASSERT( false );
			pData = NULL;
		}
		break;
	}

	if ( pData != NULL )
	{
		*ppData = pData;
		hr = HE_S_OK;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetProtocol
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetProtocol(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bProtocol )
	{
		// Get the protocol.
		CalcProtocol( m_Protocol );

		// Set the flag to tell that the spec has been checked.
		m_bProtocol = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_Protocol.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_Protocol;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetRawProtocol
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetRawProtocol(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// Check the cached protocol.
	if ( m_Protocol.IsValid() &&
		(m_Protocol.GetType() != kMediaSpecType_ProtoSpec) )
	{
		// Copy the cached protocol.
		*pSpec = m_Protocol;
		return HE_S_OK;
	}

	// Get the raw protocol (non-ProtoSpec).
	CMediaSpec	specProtocol;
	if ( CalcRawProtocol( specProtocol ) )
	{
		// Copy the raw protocol.
		*pSpec = specProtocol;
		return HE_S_OK;
	}

	return HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetSrcPhysical
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSrcPhysical(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bCalcPhysical )
	{
		// Get the physical addresses.
		CalcPhysical();

		// Set the flag to tell that the spec has been checked.
		m_bCalcPhysical = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_SrcPhysical.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_SrcPhysical;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetDestPhysical
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetDestPhysical(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bCalcPhysical )
	{
		// Get the physical addresses.
		CalcPhysical();

		// Set the flag to tell that the spec has been checked.
		m_bCalcPhysical = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_DestPhysical.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_DestPhysical;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetSrcLogical
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSrcLogical(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bCalcLogical )
	{
		// Get the logical addresses.
		CalcLogical();

		// Set the flag to tell that the cached spec is valid.
		m_bCalcLogical = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_SrcLogical.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_SrcLogical;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetDestLogical
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetDestLogical(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bCalcLogical )
	{
		// Get the logical addresses.
		CalcLogical();

		// Set the flag to tell that the spec has been checked.
		m_bCalcLogical = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_DestLogical.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_DestLogical;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetSrcPort
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSrcPort(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bCalcPorts )
	{
		// Get the ports.
		CalcPorts();

		// Set the flag to tell that the cached spec is valid.
		m_bCalcPorts = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_SrcPort.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_SrcPort;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetDestPort
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetDestPort(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	if ( !m_bCalcPorts )
	{
		// Get the ports.
		CalcPorts();

		// Set the flag to tell that the cached spec is valid.
		m_bCalcPorts = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_DestPort.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_DestPort;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetBSSID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetBSSID(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType ) );
	if ( !IS_WIRELESS( m_MediaSubType ) ) return HE_E_FAIL;

	if ( !m_bBSSID )
	{
		// Get the BSSID.
		CalcBSSID( m_BSSID );

		// Set the flag to tell that the spec has been checked.
		m_bBSSID = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_BSSID.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_BSSID;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetReceiver
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetReceiver(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType ) );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	if ( !m_bReceiver )
	{
		// Get the receiver.
		CalcReceiver( m_Receiver );

		// Set the flag to tell that the spec has been checked.
		m_bReceiver = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_Receiver.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_Receiver;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetTransmitter
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTransmitter(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	if ( !m_bTransmitter )
	{
		// Get the transmitter.
		CalcTransmitter( m_Transmitter );

		// Set the flag to tell that the spec has been checked.
		m_bTransmitter = true;
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = m_Transmitter.IsValid();

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = m_Transmitter;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetAddress1
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetAddress1(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	if ( !m_bAddress1 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress1 = true;

		// Reset address 1.
		m_pAddress1 = NULL;

		// Get the physical layer.
		HeResult		hr;
		const UInt8*	pData = NULL;
		UInt16			usDataBytes = 0;
		hr = GetPhysicalLayer( &pData, &usDataBytes );

		if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
		{
			// Get the 802.11 MAC header.
			Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x00:	// "To DS"=0, "From DS"=0.
						{
							hr = GetDestPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress1 = &m_DestPhysical;
							}
						}
						break;

						case 0x01:	// "To DS"=1, "From DS"=0.
						{
							hr = GetBSSID( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress1 = &m_BSSID;
							}
						}
						break;

						case 0x02:	// "To DS"=0, "From DS"=1.
						{
							hr = GetDestPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress1 = &m_DestPhysical;
							}
						}
						break;

						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							hr = GetReceiver( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress1 = &m_Receiver;
							}
						}
						break;
					}
				}
				break;

				case 0x04:	// Control
				{
					switch ( pHdr->FrameControlType & 0xF0 )
					{
						case 0xA0:	// PS-Poll
						{
							hr = GetBSSID( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress1 = &m_BSSID;
							}
						}
						break;

						case 0x80:	// BlockAckReq
						case 0x90:	// BlockAck
						case 0xB0:	// RTS
						case 0xC0:	// CTS
						case 0xD0:	// Ack
						case 0xE0:	// CF-End
						case 0xF0:	// CF-End + CF-Ack
						{
							hr = GetReceiver( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress1 = &m_Receiver;
							}
						}
						break;
					}
				}
				break;
			}
		}
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = ((m_pAddress1 != NULL) && (m_pAddress1->IsValid()));

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = *m_pAddress1;
		pSpec->fType = kMediaSpecType_WirelessAddr;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetAddress2
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetAddress2(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	if ( !m_bAddress2 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress2 = true;

		// Reset address 2.
		m_pAddress2 = NULL;

		// Get the physical layer.
		HeResult		hr;
		const UInt8*	pData = NULL;
		UInt16			usDataBytes = 0;
		hr = GetPhysicalLayer( &pData, &usDataBytes );

		if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
		{
			// Get the 802.11 MAC header.
			Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x00:	// "To DS"=0, "From DS"=0.
						{
							hr = GetSrcPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress2 = &m_SrcPhysical;
							}
						}
						break;

						case 0x01:	// "To DS"=1, "From DS"=0.
						{
							hr = GetSrcPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress2 = &m_SrcPhysical;
							}
						}
						break;

						case 0x02:	// "To DS"=0, "From DS"=1.
						{
							hr = GetBSSID( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress2 = &m_BSSID;
							}
						}
						break;

						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							hr = GetTransmitter( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress2 = &m_Transmitter;
							}
						}
						break;
					}
				}
				break;

				case 0x04:	// Control
				{
					switch ( pHdr->FrameControlType & 0xF0 )
					{
						case 0x80:	// BlockAckReq
						case 0x90:	// BlockAck
						case 0xA0:	// PS-Poll
						case 0xB0:	// RTS
						{
							hr = GetTransmitter( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress2 = &m_Transmitter;
							}
						}
						break;

						case 0xE0:	// CF-End
						case 0xF0:	// CF-End + CF-Ack
						{
							hr = GetBSSID( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress2 = &m_BSSID;
							}
						}
						break;
					}
				}
				break;
			}
		}
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = ((m_pAddress2 != NULL) && (m_pAddress2->IsValid()));

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = *m_pAddress2;
		pSpec->fType = kMediaSpecType_WirelessAddr;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetAddress3
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetAddress3(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	if ( !m_bAddress3 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress3 = true;

		// Reset address 3.
		m_pAddress3 = NULL;

		// Get the physical layer.
		HeResult		hr;
		const UInt8*	pData = NULL;
		UInt16			usDataBytes = 0;
		hr = GetPhysicalLayer( &pData, &usDataBytes );

		if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
		{
			// Get the 802.11 MAC header.
			Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x00:	// "To DS"=0, "From DS"=0.
						{
							hr = GetBSSID( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress3 = &m_BSSID;
							}
						}
						break;

						case 0x01:	// "To DS"=1, "From DS"=0.
						{
							hr = GetDestPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress3 = &m_DestPhysical;
							}
						}
						break;

						case 0x02:	// "To DS"=0, "From DS"=1.
						{
							hr = GetSrcPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress3 = &m_SrcPhysical;
							}
						}
						break;

						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							hr = GetDestPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress3 = &m_DestPhysical;
							}
						}
						break;
					}
				}
				break;

				case 0x04:	// Control
				{
					// No 802.11 control packets with address 3.
				}
				break;
			}
		}
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = ((m_pAddress3 != NULL) && (m_pAddress3->IsValid()));

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = *m_pAddress3;
		pSpec->fType = kMediaSpecType_WirelessAddr;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetAddress4
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetAddress4(
	TMediaSpec*	pSpec )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSpec != NULL );
	if ( pSpec == NULL ) return HE_E_NULL_POINTER;

	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	if ( !m_bAddress4 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress4 = true;

		// Reset address 4.
		m_pAddress4 = NULL;

		// Get the physical layer.
		HeResult		hr;
		const UInt8*	pData = NULL;
		UInt16			usDataBytes = 0;
		hr = GetPhysicalLayer( &pData, &usDataBytes );

		if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
		{
			// Get the 802.11 MAC header.
			Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							hr = GetSrcPhysical( pSpec );
							if ( HE_SUCCEEDED(hr) )
							{
								m_pAddress4 = &m_SrcPhysical;
							}
						}
						break;
					}
				}
				break;

				case 0x04:	// Control
				{
					// No 802.11 control packets with address 4.
				}
				break;
			}
		}
	}

	// Check if we've got a valid spec.
	const bool	bValidSpec = ((m_pAddress4 != NULL) && (m_pAddress4->IsValid()));

	if ( bValidSpec )
	{
		// Copy the media spec.
		*pSpec = *m_pAddress4;
		pSpec->fType = kMediaSpecType_WirelessAddr;
	}

	return bValidSpec ? HE_S_OK : HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		IsTransmitterImplied
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::IsTransmitterImplied(
	BOOL*	pbImplied )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pbImplied != NULL );
	if ( pbImplied == NULL ) return HE_E_NULL_POINTER;

	*pbImplied = m_bTransmitterImplied;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetFullDuplexChannel
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetFullDuplexChannel(
	UInt32*	pChannel )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pChannel != NULL );
	if ( pChannel == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const FullDuplexPrivateHeader*	pHdr = GetFullDuplexHeader();
	if ( pHdr != NULL )
	{
		*pChannel = pHdr->Channel;
	}
	else
	{
		*pChannel = PEEK_INVALID_DUPLEX_CHANNEL;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetWirelessChannel
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetWirelessChannel(
	WirelessChannel*	pChannel )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pChannel != NULL );
	if ( pChannel == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		pChannel->Band      = pHdr->Channel.Band;
		pChannel->Channel   = pHdr->Channel.Channel;
		pChannel->Frequency = pHdr->Channel.Frequency;
	}
	else
	{
		const WirelessChannel	empty = { 0, 0, 0 };
		*pChannel = empty;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetDataRate
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetDataRate(
	UInt16*	pDataRate )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pDataRate != NULL );
	if ( pDataRate == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		*pDataRate = pHdr->DataRate;
	}
	else
	{
		*pDataRate = 0;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetFlagsN
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetFlagsN(
	UInt32*	pnFlagsN )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pnFlagsN != NULL );
	if ( pnFlagsN == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		*pnFlagsN = pHdr->FlagsN;
	}
	else
	{
		*pnFlagsN = 0;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetSignalLevel
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSignalLevel(
	UInt8*	pSignalLevel )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSignalLevel != NULL );
	if ( pSignalLevel == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		*pSignalLevel = pHdr->SignalPercent;
	}
	else
	{
		*pSignalLevel = 0;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetSignaldBm
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSignaldBm(
	SInt8*	pSignaldBm )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSignaldBm != NULL );
	if ( pSignaldBm == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		*pSignaldBm = pHdr->SignaldBm;
	}
	else
	{
		*pSignaldBm = PEEK_NULL_DBM;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetSignaldBmN
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetSignaldBmN(
	UInt8	nStream,
	SInt8*	pSignaldBm )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pSignaldBm != NULL );
	if ( pSignaldBm == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		switch ( nStream )
		{
			case 1:
				*pSignaldBm = pHdr->SignaldBm1;
				break;
			case 2:
				*pSignaldBm = pHdr->SignaldBm2;
				break;
			case 3:
				*pSignaldBm = pHdr->SignaldBm3;
				break;
			default:
				*pSignaldBm = PEEK_NULL_DBM;
				hr = HE_E_INVALID_ARG;
				break;
		}
	}
	else
	{
		*pSignaldBm = PEEK_NULL_DBM;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetNoiseLevel
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetNoiseLevel(
	UInt8*	pNoiseLevel )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pNoiseLevel != NULL );
	if ( pNoiseLevel == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		*pNoiseLevel = pHdr->NoisePercent;
	}
	else
	{
		*pNoiseLevel = 0;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetNoisedBm
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetNoisedBm(
	SInt8*	pNoisedBm )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pNoisedBm != NULL );
	if ( pNoisedBm == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		*pNoisedBm = pHdr->NoisedBm;
	}
	else
	{
		*pNoisedBm = PEEK_NULL_DBM;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetNoisedBmN
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetNoisedBmN(
	UInt8	nStream,
	SInt8*	pNoisedBm )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pNoisedBm != NULL );
	if ( pNoisedBm == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const Wireless80211PrivateHeader*	pHdr = GetWirelessHeader();
	if ( pHdr != NULL )
	{
		switch ( nStream )
		{
			case 1:
				*pNoisedBm = pHdr->NoisedBm1;
				break;
			case 2:
				*pNoisedBm = pHdr->NoisedBm2;
				break;
			case 3:
				*pNoisedBm = pHdr->NoisedBm3;
				break;
			default:
				*pNoisedBm = PEEK_NULL_DBM;
				hr = HE_E_INVALID_ARG;
				break;
		}
	}
	else
	{
		*pNoisedBm = PEEK_NULL_DBM;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetTransmitterEncryptionType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTransmitterEncryptionType(
	PeekEncryptionType*	pEncryptionType )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pEncryptionType != NULL );
	if ( pEncryptionType == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY
	{
		*pEncryptionType = peekEncryptionTypeNone;
		
		if ( !m_bTransmitterEncryptionType )
		{
			m_bTransmitterEncryptionType	= true;
			m_TransmitterEncryptionType		= peekEncryptionTypeNone;
			
			// WEP, even if decrypted WEP.
			if ( (m_pPacketHeader->fStatus & kPacketStatus_Encrypted) != 0 )
			{
				m_TransmitterEncryptionType = peekEncryptionTypeWEP;
			}

			// Can we dig deeper into WEP data for other encryption data?
			const PROTOSPEC_ID	psid            = GET_PSID( GetProtoSpec() );
			const PROTOSPEC_ID	PSPEC_TKIP_DATA	= 243;
			if ( (ProtoSpecDefs::IEEE_802_11_WEP_Data == psid) || (PSPEC_TKIP_DATA == psid) )
			{
				// Get data after the MAC header.
				const UInt8*	pWEPHdr;
				UInt16			cb;
				hr = GetLinkLayer( &pWEPHdr, &cb );
				if ( HE_SUCCEEDED(hr) && (cb >= 4) )
				{
					// Is it TKIP?
					const UInt8	kTKIPFlag = 0x20;
					if ( (pWEPHdr[3] & kTKIPFlag) != 0 )
					{
						m_TransmitterEncryptionType	= peekEncryptionTypeTKIP;
					}
					else	// Not TKIP
					{
						m_TransmitterEncryptionType	= peekEncryptionTypeWEP;
					}

					// Either way, the key index is in the uppermost 2 bits of offset [3], and displayed as "1-4" not "0-3".
					m_WEPKeyIndex         = (UInt8)(((pWEPHdr[3] & 0xC0) >> 6) + 1);
					m_bWEPKeyIndexChecked = true;
					m_bWEPKeyIndexValid   = true;
				}
			}
		}

		*pEncryptionType = m_TransmitterEncryptionType;
	}
	HE_CATCH(hr)
	
	return hr;
}


// -----------------------------------------------------------------------------
//		GetReceiverEncryptionType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetReceiverEncryptionType(
	PeekEncryptionType*	pEncryptionType )
{
	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;

	HE_TRY
	{
		*pEncryptionType = peekEncryptionTypeNone;

		if ( !m_bReceiverEncryptionType )
		{
			m_bReceiverEncryptionType	= true;
			m_ReceiverEncryptionType	= peekEncryptionTypeNone;

			const UInt8*	pData;
			UInt16			cbData;
			hr = GetDataLayer( ProtoSpecDefs::IEEE_802_11_Assoc_Rsp, NULL, &pData, &cbData );
			if ( HE_SUCCEEDED(hr) )
			{
				const size_t	kOffset_Code		= 0;
				const UInt8		kCode_CKIP			= 0x85;
				const size_t	kOffset_CKIPFlags	= 10;
				const UInt8		kCKIPFlagKeyPermute	= 0x10;
				const UInt8		kCKIPFlagMIC		= 0x08;

				if ( (cbData >= kOffset_CKIPFlags) &&
					(pData[kOffset_Code] == kCode_CKIP) &&
					((pData[kOffset_CKIPFlags] & (kCKIPFlagKeyPermute | kCKIPFlagMIC)) != 0) )
				{
					m_ReceiverEncryptionType = peekEncryptionTypeCKIP;
				}
			}
		}

		*pEncryptionType = m_ReceiverEncryptionType;
	}
	HE_CATCH(hr)

	return hr;
}


// -----------------------------------------------------------------------------
//		GetTransmitterAuthenticationType
// -----------------------------------------------------------------------------
// If this packet contains some authentication protocol info, output that.
//
// If this packet contains no authentication protocol info (which is most 
// packets), leave the output argument untouched.
//
// Currently detects:
// -- None
// -- LEAP
// -- PEAP
// -- EAPTLS
// Strangely, does not detect EAP, nothing does.

HE_IMETHODIMP
Packet::GetTransmitterAuthenticationType(
	PeekAuthenticationType*	pAuthenticationType )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pAuthenticationType != NULL );
	if ( pAuthenticationType == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY
	{
		*pAuthenticationType = peekAuthenticationTypeNone;

		if ( !m_bTransmitterAuthenticationType )
		{
			m_bTransmitterAuthenticationType	= true;
			m_TransmitterAuthenticationType		= peekAuthenticationTypeNone;

			const UInt8*	pData;
			UInt16			cbData;
			hr = GetDataLayer( ProtoSpecDefs::EAP_Response, NULL, &pData, &cbData );
			if ( HE_SUCCEEDED(hr) )
			{
				const size_t	kOffset_Code	= 0;
				const UInt8		kCode_Response	= 2;
				const size_t	kOffset_Type	= 4;
				const UInt8		kType_EAPTLS	= 13;
				const UInt8		kType_LEAP		= 17;
				const UInt8		kType_PEAP		= 25;

				if ( (cbData >= kOffset_Type) &&
					(pData[kOffset_Code] == kCode_Response) )
				{
					switch ( pData[kOffset_Type] )
					{
						case kType_EAPTLS:
							m_TransmitterAuthenticationType = peekAuthenticationTypeEAPTLS;
							break;
						case kType_LEAP:
							m_TransmitterAuthenticationType = peekAuthenticationTypeLEAP;
							break;
						case kType_PEAP:
							m_TransmitterAuthenticationType = peekAuthenticationTypePEAP;
							break;
					}
				}
			}
		}

		*pAuthenticationType = m_TransmitterAuthenticationType;
	}
	HE_CATCH(hr)

	return hr;
}


// -----------------------------------------------------------------------------
//		GetWEPKeyIndex
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetWEPKeyIndex(
	SInt32*	pWEPKeyIndex )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pWEPKeyIndex != NULL );
	if ( pWEPKeyIndex == NULL ) return HE_E_NULL_POINTER;

	*pWEPKeyIndex = -1;

	if ( !m_bWEPKeyIndexChecked )
	{
		m_bWEPKeyIndexChecked = true;

		// get_TransmitterEncryptionType calculates this.
		PeekEncryptionType	et;
		(void) GetTransmitterEncryptionType( &et );
	}

	if ( m_bWEPKeyIndexValid )
	{
		*pWEPKeyIndex = m_WEPKeyIndex;
	}
	else
	{
		return HE_S_FALSE;
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetWirelessControlFlags
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetWirelessControlFlags(
	UInt8*	pFlags )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pFlags != NULL );
	if ( pFlags == NULL ) return HE_E_NULL_POINTER;

	// Initialize out params.
	*pFlags = 0;

	// Are we really wireless?
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return HE_E_FAIL;

	// Fetch the wireless header (we only need the second byte).
	HeResult		hr;
	const UInt8*	pPhysHeader;
	UInt16			cb;
	hr = GetPhysicalLayer( &pPhysHeader, &cb );
	if ( HE_SUCCEEDED(hr) )
	{
		if ( cb >= 2 )
		{
			// Wireless control flags is the second byte (index 1). No point in
			// casting to a Wireless80211MacHeader just for one measly byte.
			*pFlags	= pPhysHeader[1];
		}
		else
		{
			hr = HE_E_FAIL;
		}
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetWirelessFragmentNumber
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetWirelessFragmentNumber(
	UInt8*	pnFragmentNumber )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pnFragmentNumber != NULL );
	if ( pnFragmentNumber == NULL ) return HE_E_NULL_POINTER;

	// Initialize out params.
	*pnFragmentNumber = 0;

	// Are we really wireless?
	if ( !IS_WIRELESS( m_MediaSubType )  )
	{
		return HE_E_FAIL;
	}

	// Fetch the wireless header.
	HeResult		hr;
	const UInt8*	pPhysHeader;
	UInt16			cb;
	hr = GetPhysicalLayer( &pPhysHeader, &cb );
	if ( HE_SUCCEEDED(hr) )
	{
		if ( cb >= sizeof(Wireless80211MacHeader) )
		{
			const Wireless80211MacHeader*	pWirelessHeader = reinterpret_cast<const Wireless80211MacHeader*>(pPhysHeader);

			// Extract fragment number.
			const UInt16	nSeqFrag        = LITTLETOHOST16( pWirelessHeader->Sequence );
			const UInt16	nFragmentNumber = nSeqFrag & 0x000F;

			*pnFragmentNumber = static_cast<UInt8>(nFragmentNumber);

			// Zero fragment number plus no more fragments means not a fragment.
			hr = ((nFragmentNumber == 0) && ((pWirelessHeader->FrameControlFlags & 0x04) == 0)) ? HE_S_FALSE : HE_S_OK;
		}
		else
		{
			hr = HE_E_FAIL;
		}
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetWanDirection
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetWanDirection(
	UInt8*	pWanDirection )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pWanDirection != NULL );
	if ( pWanDirection == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const WideAreaNetworkHeader*	pHdr = GetWanHeader();
	if ( pHdr != NULL )
	{
		*pWanDirection = pHdr->Direction;
	}
	else
	{
		*pWanDirection = PEEK_WAN_DIRECTION_INVALID;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetWanProtocol
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetWanProtocol(
	UInt8*	pWanProtocol )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pWanProtocol != NULL );
	if ( pWanProtocol == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	const WideAreaNetworkHeader*	pHdr = GetWanHeader();
	if ( pHdr != NULL )
	{
		*pWanProtocol = pHdr->Protocol;
	}
	else
	{
		*pWanProtocol = PEEK_WAN_PROT_INVALID;
		hr = HE_E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetTCPSequence
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPSequence(
	UInt32*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcTCPHeader() )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPSequence;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPAck
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPAck(
	UInt32*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcTCPHeader() )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPAck;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPFlags
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPFlags(
	UInt8*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcTCPHeader() )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPFlags;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPWindow
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPWindow(
	UInt16*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcTCPHeader() )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPWindow;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPChecksum
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPChecksum(
	UInt16*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcTCPHeader() )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPChecksum;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPUrgentPointer
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPUrgentPointer(
	UInt16*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	if ( !CalcTCPHeader() )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPUrgentPointer;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPOption
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPOption(
	UInt8			optionType,
	const UInt8**	ppData,
	UInt32*			pcbData )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( ppData != NULL );
	if ( ppData == NULL ) return HE_E_NULL_POINTER;
	ASSERT( pcbData != NULL );
	if ( pcbData == NULL ) return HE_E_NULL_POINTER;

	*ppData  = NULL;
	*pcbData = 0;

	// Get TCP header information.
	if ( CalcTCPHeader() )
	{
		// Note: m_pTCPOptionBegin and m_pTCPOptionEnd are NULL if there are no options.

		// Walk the option list until found or exhausted.
		for ( const UInt8* pIter = m_pTCPOptionBegin; pIter < m_pTCPOptionEnd; )
		{
			// Get the TCP option type.
			const UInt8	nType = *pIter++;
			
			// Skip 1-byte length-less NOP option.
			if ( nType == TCP_OPTION_NOOP ) continue;

			// Stop if we see an end marker.
			if ( nType == TCP_OPTION_END ) break;

			// Re-check data length.
			if ( pIter >= m_pTCPOptionEnd ) break;

			// Get option length and option data (if any).
			const UInt8		cbData = *pIter++;
			const UInt8*	pData  =  pIter;
			pIter += static_cast<size_t>(cbData);

			if ( nType == optionType )
			{
				// Found it!
				*ppData  = pData;
				*pcbData = cbData;
				return HE_S_OK;
			}
		}
	}

	return HE_E_FAIL;
}


// -----------------------------------------------------------------------------
//		GetTCPOptionMSS
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPOptionMSS(
	UInt16*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	CalcTCPOptions();

	if ( !m_bTCPOptionMSS )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPOptionMSS;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPOptionWindowScale
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPOptionWindowScale(
	UInt8*	pVal )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;

	CalcTCPOptions();

	if ( !m_bTCPOptionWindowScale )
	{
		*pVal = 0;
		return HE_E_FAIL;
	}

	*pVal = m_TCPOptionWindowScale;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPOptionSACK
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPOptionSACK(
	UInt32*	pBegin,
	UInt32*	pEnd )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pBegin != NULL );
	if ( pBegin == NULL ) return HE_E_NULL_POINTER;
	ASSERT( pEnd != NULL );
	if ( pEnd == NULL ) return HE_E_NULL_POINTER;

	CalcTCPOptions();

	if ( !m_bTCPOptionSACK )
	{
		*pBegin = 0;
		*pEnd   = 0;
		return HE_E_FAIL;
	}

	*pBegin = m_TCPOptionSACKBegin;
	*pEnd   = m_TCPOptionSACKEnd;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetTCPOptionTimestamp
// -----------------------------------------------------------------------------

HE_IMETHODIMP
Packet::GetTCPOptionTimestamp(
	UInt32*	pVal,
	UInt32*	pEcho )
{
	ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pVal != NULL );
	if ( pVal == NULL ) return HE_E_NULL_POINTER;
	ASSERT( pEcho != NULL );
	if ( pEcho == NULL ) return HE_E_NULL_POINTER;

	CalcTCPOptions();

	if ( !m_bTCPOptionTimestamp )
	{
		*pVal  = 0;
		*pEcho = 0;
		return HE_E_FAIL;
	}

	*pVal  = m_TCPOptionTimestampValue;
	*pEcho = m_TCPOptionTimestampEcho;

	return HE_S_OK;
}


// =============================================================================
//	Private Interface
// =============================================================================

// -----------------------------------------------------------------------------
//		GetProtoSpec
// -----------------------------------------------------------------------------

UInt32
Packet::GetProtoSpec()
{
	// Sanity checks.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	if ( m_pPacketHeader->fProtoSpec == 0 )
	{
		PROTOSPEC_INSTID	psid;
		ProtoSpecs::GetPacketProtoSpec(
			m_MediaType, m_MediaSubType, m_pPacketHeader->fFlags,
			m_pPacketData, m_ActualLength, psid );
		m_pPacketHeader->fProtoSpec = static_cast<UInt32>(psid);
	}

	return m_pPacketHeader->fProtoSpec;				
}


// -----------------------------------------------------------------------------
//		CalcProtoSpecArray
// -----------------------------------------------------------------------------

bool
Packet::CalcProtoSpecArray()
{
	using namespace ProtoSpecs;

	// Sanity checks.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	m_nProtoSpecCount = 0;

	// Build the ProtoSpec info array.
	SProtospecInfo	ayPSInfo[MAX_PROTOSPEC_DEPTH];
	int				nPSInfo = MAX_PROTOSPEC_DEPTH;
	if ( HE_FAILED( BuildParentInfo( m_MediaType, m_MediaSubType,
		m_pPacketData, m_ActualLength, GetProtoSpec(), ayPSInfo, nPSInfo ) ) )
	{
		return false;
	}

	// Copy the ProtoSpec info while removing kProtospecsReserved_Switch.
	int	nProtoSpecCount = 0;
	for ( int i = 0; i < nPSInfo; ++i )
	{
		const SProtospecInfo&	info = ayPSInfo[i];
		const UInt16			psid = GET_PSID( info.nInstID );
		if ( psid != kProtospecsReserved_Switch )
		{
			m_ProtoSpecArray[nProtoSpecCount]   = info;
			m_ProtoSpecIDArray[nProtoSpecCount] = psid;
			++nProtoSpecCount;
		}
	}
	m_nProtoSpecCount = nProtoSpecCount;

	return true;
}


// -----------------------------------------------------------------------------
//		CalcProtocol
// -----------------------------------------------------------------------------

bool
Packet::CalcProtocol(
	CMediaSpec& outSpec )
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	const UInt32	ps = GetProtoSpec();
	if ( GET_PSID( ps ) == ProtoSpecDefs::IEEE_802_11_Data )
	{
		// Attempt to use the "raw" protocol if 802.11 data.
		if ( CalcRawProtocol( outSpec ) )
		{
			return true;
		}
	}

	if ( !IS_LLC_PSID( ps ) )
	{
		// ProtoSpec defined protocol.
		outSpec.SetClass( kMediaSpecClass_Protocol );
		outSpec.SetType( kMediaSpecType_ProtoSpec );
		outSpec.fMask = outSpec.GetTypeMask();
		*(UInt32*) &outSpec.fData[0] = ps;
		return true;
	}

	// Use the raw protocol type.
	return CalcRawProtocol( outSpec );
}


// -----------------------------------------------------------------------------
//		CalcRawProtocol
// -----------------------------------------------------------------------------

bool
Packet::CalcRawProtocol(
	CMediaSpec& outSpec )
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	switch ( m_MediaType )
	{
		case kMediaType_802_3:
		{
			switch ( m_MediaSubType )
			{
				case kMediaSubType_Native:
				{
					if ( (m_pPacketHeader->fFlags & kPacketFlag_SNAP) != 0 )
					{
						// Get the start of the link layer.
						HeResult		hr;
						const UInt8*	pData = NULL;
						UInt16			usDataBytes = 0;
						hr = GetLinkLayer( &pData, &usDataBytes );

						if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
						{
							if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
							{
								if ( usDataBytes >= 8 )
								{
									// SNAP ID - 5 bytes. Skip the command.
									outSpec.SetClass( kMediaSpecClass_Protocol );
									outSpec.SetType( kMediaSpecType_SNAP );
									outSpec.fMask = outSpec.GetTypeMask();
									*(UInt32*) &outSpec.fData[0] = *(UInt32*) &pData[3];
									outSpec.fData[4] = pData[7];
								}
							}
							else
							{
								// Check SSAP for generic values.
								switch ( pData[0] )
								{
									case 0x00:	// Null.
									case 0xFF:	// Global.
									case 0x05:	// Group SNA.
									case 0xF5:	// Group LAN Management.
									case 0xAA:	// Almost SNAP... not quite.
									{
										// Skip to the DSAP.
										pData++;
									}
									break;
								}

								// LSAP protocol - 1 byte.
								outSpec.SetClass( kMediaSpecClass_Protocol );
								outSpec.SetType( kMediaSpecType_LSAP );
								outSpec.fMask = outSpec.GetTypeMask();
								outSpec.fData[0] = pData[0];
							}
						}
					}
					else
					{
						// Get the start of the physical layer.
						HeResult		hr;
						const UInt8*	pData = NULL;
						UInt16			usDataBytes = 0;
						hr = GetPhysicalLayer( &pData, &usDataBytes );

						if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 14) )
						{
							// Ethernet protocol - 2 bytes.
							outSpec.SetClass( kMediaSpecClass_Protocol );
							outSpec.SetType( kMediaSpecType_EthernetProto );
							outSpec.fMask = outSpec.GetTypeMask();
							*(UInt16*) &outSpec.fData[0] =
								((EthernetPacketHeader*)pData)->Protocol;
						}
					}
				}
				break;

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:
				{
					// Get the start of the link layer.
					HeResult		hr;
					const UInt8*	pData = NULL;
					UInt16			usDataBytes = 0;
					hr = GetLinkLayer( &pData, &usDataBytes );

					if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
					{
						if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
						{
							if ( usDataBytes >= 8 )
							{
								// SNAP ID - 5 bytes. Skip the command.
								outSpec.SetClass( kMediaSpecClass_Protocol );
								outSpec.SetType( kMediaSpecType_SNAP );
								outSpec.fMask = outSpec.GetTypeMask();
								*(UInt32*) &outSpec.fData[0] = *(UInt32*) &pData[3];
								outSpec.fData[4] = pData[7];
							}
						}
						else
						{
							// Check SSAP for generic values.
							switch ( pData[0] )
							{
								case 0x00:	// Null.
								case 0xFF:	// Global.
								case 0x05:	// Group SNA.
								case 0xF5:	// Group LAN Management.
								case 0xAA:	// Almost SNAP... not quite.
								{
									// Skip to the DSAP.
									pData++;
								}
								break;
							}

							// LSAP protocol - 1 byte.
							outSpec.SetClass( kMediaSpecClass_Protocol );
							outSpec.SetType( kMediaSpecType_LSAP );
							outSpec.fMask = outSpec.GetTypeMask();
							outSpec.fData[0] = pData[0];
						}
					}
					else
					{
						// Just say that it's the ProtoSpec (or 802.11 data if it's an LLC ProtoSpec).
						outSpec.SetClass( kMediaSpecClass_Protocol );
						outSpec.SetType( kMediaSpecType_ProtoSpec );
						outSpec.fMask = outSpec.GetTypeMask();

						const UInt32	ps = GetProtoSpec();
						if ( IS_LLC_PSID( ps ) )
						{
							ASSERT( ProtoSpecs::IsProtospecsLoaded() == HE_S_OK );
							PROTOSPEC_INSTID	psParent = 0;
							hr = ProtoSpecs::GetParentID( ps, psParent );
							if ( psParent != 0 )
							{
								*(UInt32*) &outSpec.fData[0] = psParent;
							}
							else
							{
								*(UInt32*) &outSpec.fData[0] = ps;
							}
						}
						else
						{
							*(UInt32*) &outSpec.fData[0] = ps;
						}
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
		break;

		case kMediaType_802_5:
		{
			// TODO: Handle MAC Ctl packets differently?
			// Get the start of the link layer.
			HeResult		hr;
			const UInt8*	pData = NULL;
			UInt16			usDataBytes = 0;
			hr = GetLinkLayer( &pData, &usDataBytes );

			if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 2) )
			{
				if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
				{
					if ( usDataBytes >= 8 )
					{
						// SNAP ID - 5 bytes. Skip the command.
						outSpec.SetClass( kMediaSpecClass_Protocol );
						outSpec.SetType( kMediaSpecType_SNAP );
						outSpec.fMask = outSpec.GetTypeMask();
						*(UInt32*) &outSpec.fData[0] = *(UInt32*) &pData[3];
						outSpec.fData[4] = pData[7];
					}
				}
				else
				{
					// Check SSAP for generic values.
					switch ( pData[0] )
					{
						case 0x00:	// Null.
						case 0xFF:	// Global.
						case 0x05:	// Group SNA.
						case 0xF5:	// Group LAN Management.
						case 0xAA:	// Almost SNAP... not quite.
						{
							// Skip to the DSAP.
							pData++;
						}
						break;
					}

					// LSAP protocol - 1 byte.
					outSpec.SetClass( kMediaSpecClass_Protocol );
					outSpec.SetType( kMediaSpecType_LSAP );
					outSpec.fMask = outSpec.GetTypeMask();
					outSpec.fData[0] = pData[0];
				}
			}
		}
		break;

		case kMediaType_Wan:
		case kMediaType_CoWan:
		{
			// Get the real media type from the protocol field.
			HeResult	hr;
			UInt8		nWanProtocol;
			hr = GetWanProtocol( &nWanProtocol );
			if ( HE_FAILED(hr) ) return false;
			TMediaSubType	mst;
			HE_VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				m_MediaType, nWanProtocol, &mst ) == 0 );

			switch ( mst )
			{
				case kMediaSubType_wan_ppp:			// WAN PPP
				{
					// protocol is the 3/4 bytes
					const UInt8*	pData;
					UInt16			nBytesRemaining;
					hr = GetPhysicalLayer( &pData, &nBytesRemaining );
					if ( HE_SUCCEEDED(hr) && (pData != NULL) && (nBytesRemaining >= 4) )
					{
						outSpec.SetClass( kMediaSpecClass_Protocol );
						outSpec.SetType( kMediaSpecType_WAN_PPP_Proto );
						outSpec.fMask = outSpec.GetTypeMask();
						*((UInt16*) outSpec.fData) = *((UInt16*) (pData + 2));
					}
				}
				break;

				case kMediaSubType_wan_frameRelay:		// WAN Frame Relay
				{
					// protocol is the 3/4 bytes
					const UInt8*	pData;
					UInt16			nBytesRemaining;
					hr = GetPhysicalLayer( &pData, &nBytesRemaining );
					if ( HE_SUCCEEDED(hr) && (pData != NULL) && (nBytesRemaining >= 4) )
					{
						outSpec.SetClass( kMediaSpecClass_Protocol );
						outSpec.SetType( kMediaSpecType_WAN_FrameRelay_Proto );
						outSpec.fMask = outSpec.GetTypeMask();
						*((UInt16*) outSpec.fData) = *((UInt16*) (pData + 2));
					}
				}
				break;

				case kMediaSubType_wan_x25:				// WAN X25
				case kMediaSubType_wan_x25e:			// WAN X25 modulo 128
				case kMediaSubType_wan_ipars:			// WAN Ipars
				case kMediaSubType_wan_u200:			// WAN U200
				case kMediaSubType_wan_Q931:			// WAN BRI Q.931
				{
					// just say that it's the root
					outSpec.SetClass( kMediaSpecClass_Protocol );
					outSpec.SetType( kMediaSpecType_ProtoSpec );
					outSpec.fMask = outSpec.GetTypeMask();
					*((PROTOSPEC_INSTID*) outSpec.fData) = GetProtoSpec();
				}
				break;

				default:
				{
					ASSERT( 0 );
				}
				break;
			}
		}
		break;

		default:
		{
			ASSERT( 0 );
		}
		break;
	}

	return (outSpec.GetClass() != kMediaSpecClass_Null);
}


// -----------------------------------------------------------------------------
//		CalcPhysical
// -----------------------------------------------------------------------------

void
Packet::CalcPhysical()
{
	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return;

	// Invalidate the physical addresses.
	m_SrcPhysical.Invalidate();
	m_DestPhysical.Invalidate();

	// Get the physical layer.
	HeResult		hr;
	const UInt8*	pData = NULL;
	UInt16			usDataBytes = 0;
	hr = GetPhysicalLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return;

	switch ( m_MediaType )
	{
		case kMediaType_802_3:
		{
			switch ( m_MediaSubType )
			{
				case kMediaSubType_Native:
				{
					if ( pData != NULL )
					{
						static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_EthernetAddr );

						if ( usDataBytes >= 12 )
						{
							// Set up the spec.
							m_SrcPhysical.SetClass( kMediaSpecClass_Address );
							m_SrcPhysical.SetType( kMediaSpecType_EthernetAddr );
							m_SrcPhysical.fMask = nMask;
							*(UInt32*) &m_SrcPhysical.fData[0] =
								*(UInt32*) &((EthernetPacketHeader*)pData)->SrcAddr[0];
							*(UInt16*) &m_SrcPhysical.fData[4] =
								*(UInt16*) &((EthernetPacketHeader*)pData)->SrcAddr[4];
						}

						if ( usDataBytes >= 6 )
						{
							// Set up the spec.
							m_DestPhysical.SetClass( kMediaSpecClass_Address );
							m_DestPhysical.SetType( kMediaSpecType_EthernetAddr );
							m_DestPhysical.fMask = nMask;
							*(UInt32*) &m_DestPhysical.fData[0] =
								*(UInt32*) &((EthernetPacketHeader*)pData)->DestAddr[0];
							*(UInt16*) &m_DestPhysical.fData[4] =
								*(UInt16*) &((EthernetPacketHeader*)pData)->DestAddr[4];
						}
					}
				}
				break;

				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_gen:
				{
					if ( (pData != NULL) && (usDataBytes >= 2) )
					{
						// Get the 802.11 MAC header.
						const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

						const UInt8*	pSrcAddr  = NULL;
						const UInt8*	pDestAddr = NULL;
						switch ( pHdr->FrameControlType & 0x0C )
						{
							case 0x00:	// Management, Addr1 = DA, Addr2 = SA, Addr3 = BSSID
							{
								if ( usDataBytes >= 22 )
								{
									pSrcAddr = &pHdr->Addr2[0];
								}
								if ( usDataBytes >= 10 )
								{
									pDestAddr = &pHdr->Addr1[0];
								}
							}
							break;

							case 0x08:	// Data
							{
								// Get the address to use from the FrameControl
								// fields "To DS" and "From DS".
								switch ( pHdr->FrameControlFlags & 0x03 )
								{
									case 0x00:	// "To DS"=0, "From DS"=0.
									{
										if ( usDataBytes >= 16 )
										{
											pSrcAddr = &pHdr->Addr2[0];
										}
										if ( usDataBytes >= 10 )
										{
											pDestAddr = &pHdr->Addr1[0];
										}
									}
									break;

									case 0x01:	// "To DS"=1, "From DS"=0.
									{
										if ( usDataBytes >= 16 )
										{
											pSrcAddr = &pHdr->Addr2[0];
										}
										if ( usDataBytes >= 22 )
										{
											pDestAddr = &pHdr->Addr3[0];
										}
									}
									break;

									case 0x02:	// "To DS"=0, "From DS"=1.
									{
										if ( usDataBytes >= 22 )
										{
											pSrcAddr = &pHdr->Addr3[0];
										}
										if ( usDataBytes >= 10 )
										{
											pDestAddr = &pHdr->Addr1[0];
										}
									}
									break;

									case 0x03:	// "To DS"=1, "From DS"=1.
									{
										if ( usDataBytes >= 30 )
										{
											pSrcAddr = &pHdr->Addr4[0];
										}
										if ( usDataBytes >= 22 )
										{
											pDestAddr = &pHdr->Addr3[0];
										}
									}
									break;
								}
							}
							break;

							case 0x04:	// Control
							{
								switch ( pHdr->FrameControlType & 0xF0 )
								{
									case 0x80:	// BlockAckReq
									case 0x90:	// BlockAck
									case 0xA0:	// PS-Poll
									case 0xB0:	// RTS
									case 0xE0:	// CF-End
									case 0xF0:	// CF-End + CF-Ack
									{
										if ( usDataBytes >= 16 )
										{
											pSrcAddr = &pHdr->Addr2[0];
										}
										if ( usDataBytes >= 10 )
										{
											pDestAddr = &pHdr->Addr1[0];
										}
									}
									break;

									case 0xC0:	// CTS
									case 0xD0:	// ACK
									{
										// 802.11 control packets of type ACK and CTS
										// only carry the receiver address.
										// For ACK packets, the transmitter is
										// implied from the "immediately previous
										// directed data, management, or PS-POLL
										// control frame".
										// For CTS packets, the transmitter is
										// implied from the receiver in the
										// immediately preceding RTS frame.

										// Get the packet's receiver.
										CMediaSpec	specReceiver;
										if ( (m_pPacketBuffer != NULL) &&
											HE_SUCCEEDED(GetAddress1( &specReceiver )) )
										{
											// Get this packet's buffer index.
											const UInt32	ulIndex = m_PacketBufferIndex;

											CHeObjStack<Packet>	thePrevPacket;

											for ( UInt32 ulOffset = 1; ulOffset <= 5; ulOffset++ )
											{
												// Sanity check the buffer index and offset.
												if ( ulOffset > ulIndex ) break;

												// Get the previous packet.
												hr = m_pPacketBuffer->GetPacket( &thePrevPacket, ulIndex - ulOffset );
												if ( HE_FAILED(hr) ) break;

												// Don't trust it if it's a CRC error packet.
												BOOL	bError;
												hr = thePrevPacket.TestFlag( kPacketFlag_CRC, &bError );
												if ( HE_FAILED(hr) || bError ) break;

												// Get the previous packet's transmitter.
												CMediaSpec	specPrevTransmitter;
												hr = thePrevPacket.GetAddress2( &specPrevTransmitter );
												if ( HE_SUCCEEDED(hr) )
												{
													// Compare this packet's receiver with
													// the previous packet's transmitter.
													if ( specReceiver == specPrevTransmitter )
													{
														// The receiver from the previous packet is the
														// implied transmitter of this packet.
														hr = thePrevPacket.GetAddress1( &m_SrcPhysical );
														if ( HE_SUCCEEDED(hr) )
														{
															// Force the media spec type to be Ethernet.
															m_SrcPhysical.SetType( kMediaSpecType_EthernetAddr );
														}

														// Note that we've gotten this from another packet.
														m_bTransmitterImplied = true;

														// Stop looking.
														break;
													}
												}
											}
										}

										if ( usDataBytes >= 10 )
										{
											pDestAddr = &pHdr->Addr1[0];
										}
									}
									break;
								}
							}
							break;
						}

						static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_EthernetAddr );

						if ( pSrcAddr != NULL )
						{
							// Set up the spec.
							m_SrcPhysical.SetClass( kMediaSpecClass_Address );
							m_SrcPhysical.SetType( kMediaSpecType_EthernetAddr );
							m_SrcPhysical.fMask = nMask;
							*(UInt32*) &m_SrcPhysical.fData[0] = *(UInt32*) &pSrcAddr[0];
							*(UInt16*) &m_SrcPhysical.fData[4] = *(UInt16*) &pSrcAddr[4];
						}

						if ( pDestAddr != NULL )
						{
							// Set up the spec.
							m_DestPhysical.SetClass( kMediaSpecClass_Address );
							m_DestPhysical.SetType( kMediaSpecType_EthernetAddr );
							m_DestPhysical.fMask = nMask;
							*(UInt32*) &m_DestPhysical.fData[0] = *(UInt32*) &pDestAddr[0];
							*(UInt16*) &m_DestPhysical.fData[4] = *(UInt16*) &pDestAddr[4];
						}
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
		break;

		case kMediaType_802_5:
		{
			if ( pData != NULL )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_TokenRingAddr );

				if ( usDataBytes >= 14 )
				{
					// Set up the spec.
					m_SrcPhysical.SetClass( kMediaSpecClass_Address );
					m_SrcPhysical.SetType( kMediaSpecType_TokenRingAddr );
					m_SrcPhysical.fMask = nMask;
					*(UInt32*) &m_SrcPhysical.fData[0] =
						*(UInt32*) &((TokenRingPacketHeader*)pData)->SrcAddr[0];
					*(UInt16*) &m_SrcPhysical.fData[4] =
						*(UInt16*) &((TokenRingPacketHeader*)pData)->SrcAddr[4];
					m_SrcPhysical.fData[0] &= 0x7F;
				}

				if ( usDataBytes >= 8 )
				{
					// Setup the spec.
					m_DestPhysical.SetClass( kMediaSpecClass_Address );
					m_DestPhysical.SetType( kMediaSpecType_TokenRingAddr );
					m_DestPhysical.fMask = nMask;
					*(UInt32*) &m_DestPhysical.fData[0] =
						*(UInt32*) &((TokenRingPacketHeader*)pData)->DestAddr[0];
					*(UInt16*) &m_DestPhysical.fData[4] =
						*(UInt16*) &((TokenRingPacketHeader*)pData)->DestAddr[4];
				}
			}
		}
		break;

		case kMediaType_CoWan:
		case kMediaType_Wan:
		{
			UInt8		nWanProtocol;
			hr = GetWanProtocol( &nWanProtocol );
			if ( HE_FAILED(hr) ) return;
			TMediaSubType	mst;
			HE_VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				m_MediaType, nWanProtocol, &mst ) == 0 );

			switch ( mst ) 
			{
				case kMediaSubType_wan_ppp:		// PPP over a T1 or an E1
				{
					// cHDLC/PPP has a 1 byte descriptor for unicast/broadcast.
					// This might be the destination address.
				}
				break;

				case kMediaSubType_wan_frameRelay: // Frame Relay
				{
					// Frame relay uses 10-bit DLCI addressing for the
					// destination the top 6 bits are the top 6 bits of byte 0,
					// the bottom 4 bits are the top 4 bits in byte 1.
					// Src is to DCE, Dest is to DTE
					if ( (pData != NULL) && (usDataBytes >= 2) )
					{
						static const UInt32	nMask     = CMediaSpec::GetTypeMask( kMediaSpecType_WAN_DLCIAddr );
						const UInt8			direction = GetWanHeader()->Direction;
						if ( direction == PEEK_WAN_DIRECTION_DCE )
						{
							m_SrcPhysical.SetClass( kMediaSpecClass_Address );
							m_SrcPhysical.SetType( kMediaSpecType_WAN_DLCIAddr );
							m_SrcPhysical.fMask = nMask;
							*(UInt16*) &m_SrcPhysical.fData[0] = 
								(((UInt16) (pData[0] & 0xfc)) << 2) + ((pData[1] & 0xf0) >> 4);
						}
						else if ( direction == PEEK_WAN_DIRECTION_DTE )
						{
							m_DestPhysical.SetClass( kMediaSpecClass_Address );
							m_DestPhysical.SetType( kMediaSpecType_WAN_DLCIAddr );
							m_DestPhysical.fMask = nMask;
							*(UInt16*) &m_DestPhysical.fData[0] = 
								(((UInt16) (pData[0] & 0xfc)) << 2) + ((pData[1] & 0xf0) >> 4);
						}
					}
				}
				break;

				case kMediaSubType_wan_x25:		// X.25
				case kMediaSubType_wan_x25e:	// X.25 modulo 128
				case kMediaSubType_wan_ipars:	// IPARS
				case kMediaSubType_wan_u200:    // U200
				case kMediaSubType_wan_Q931:	// BRI Q.931
				{
				}
				break;

				default:
				{
					ASSERT( 0 );
				}
				break;
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


// -----------------------------------------------------------------------------
//		CalcLogical
// -----------------------------------------------------------------------------

void
Packet::CalcLogical()
{
	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return;

	// Invalidate the logical addresses.
	m_SrcLogical.Invalidate();
	m_DestLogical.Invalidate();

	// Check for a type we know how to format.
	static const UInt16	s_ParentArray[] =
	{
		ProtoSpecDefs::IP,
		ProtoSpecDefs::AppleTalk_Ph2,
		ProtoSpecDefs::AARP,
		ProtoSpecDefs::Long_DDP,
		ProtoSpecDefs::Short_DDP,
		ProtoSpecDefs::IPv6,
		ProtoSpecDefs::DECnet,
		ProtoSpecDefs::IPX
	};
	HeResult			hr;
	PROTOSPEC_INSTID	matchID;
	SInt32				lParentIndex;
	BOOL				bResult;
	hr = IsDescendentOf( s_ParentArray, COUNTOF(s_ParentArray),
		&matchID, &lParentIndex, &bResult );
	if ( HE_FAILED(hr) || !bResult ) return;
	const UInt32		psid = s_ParentArray[lParentIndex];

	// Get a pointer to the start of packet data.
	const UInt8*	pData = NULL;
	UInt16			usDataBytes = 0;
	hr = GetNetworkLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return;

	switch ( psid )
	{
		// IP
		case ProtoSpecDefs::IP:
		{
			const UInt8*	pHeader;
			UInt16			cbHeader;
			hr = GetHeaderLayer( GET_PSID( matchID ), NULL, &pHeader, &cbHeader );
			if ( HE_SUCCEEDED(hr) && (cbHeader >= 20) ) 
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_IPAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt32*) &m_SrcLogical.fData[0] = *(UInt32*) &pHeader[12];

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_IPAddr );
				m_DestLogical.fMask = nMask;
				*(UInt32*) &m_DestLogical.fData[0] = *(UInt32*) &pHeader[16];
			}
		}
		break;

		// EType2-IPv6
		// 802.3-IPv6
		case ProtoSpecDefs::IPv6:
		{
			static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPv6Addr );

			if ( usDataBytes >= 24 )
			{
				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_IPv6Addr );
				m_SrcLogical.fMask = nMask;
				*(UInt32*) &m_SrcLogical.fData[0]  = *(UInt32*) &pData[8];
				*(UInt32*) &m_SrcLogical.fData[4]  = *(UInt32*) &pData[12];
				*(UInt32*) &m_SrcLogical.fData[8]  = *(UInt32*) &pData[16];
				*(UInt32*) &m_SrcLogical.fData[12] = *(UInt32*) &pData[20];
			}

			if ( usDataBytes >= 40 )
			{
				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_IPv6Addr );
				m_DestLogical.fMask = nMask;
				*(UInt32*) &m_DestLogical.fData[0]  = *(UInt32*) &pData[24];
				*(UInt32*) &m_DestLogical.fData[4]  = *(UInt32*) &pData[28];
				*(UInt32*) &m_DestLogical.fData[8]  = *(UInt32*) &pData[32];
				*(UInt32*) &m_DestLogical.fData[12] = *(UInt32*) &pData[36];
			}
		}
		break;

		// 802.3-AppleTalk
		case ProtoSpecDefs::AppleTalk_Ph2:
		{
			if ( usDataBytes >= 10 )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_AppleTalkAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt16*) &m_SrcLogical.fData[0] = *(UInt16*) &pData[6];
				m_SrcLogical.fData[2] = pData[9];

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_DestLogical.fMask = nMask;
				*(UInt16*) &m_DestLogical.fData[0] = *(UInt16*) &pData[4];
				m_DestLogical.fData[2] = pData[8];
			}
		}
		break;

		// AARP
		case ProtoSpecDefs::AARP:
		{
			if ( usDataBytes >= 18 )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_AppleTalkAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt16*) &m_SrcLogical.fData[0] = *(UInt16*) &pData[15];
				m_SrcLogical.fData[2] = pData[17];
			}
		}
		break;

		// EType2-AppleTalk-Long DDP
		case ProtoSpecDefs::Long_DDP:
		{
			if ( usDataBytes >= 13 )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_AppleTalkAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt16*) &m_SrcLogical.fData[0] = *(UInt16*) &pData[9];
				m_SrcLogical.fData[2] = pData[12];

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_DestLogical.fMask = nMask;
				*(UInt16*) &m_DestLogical.fData[0] = *(UInt16*) &pData[7];
				m_DestLogical.fData[2] = pData[11];
			}
		}
		break;

		// EType2-AppleTalk-Short DDP
		case ProtoSpecDefs::Short_DDP:
		{
			if ( usDataBytes >= 2 )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_AppleTalkAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt16*) &m_SrcLogical.fData[0] = 0;
				m_SrcLogical.fData[2] = pData[1];

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_DestLogical.fMask = nMask;
				*(UInt16*) &m_DestLogical.fData[0] = 0;
				m_DestLogical.fData[2] = pData[0];
			}
		}
		break;

		// AARP Response
		case ProtoSpecDefs::AARP_Response:
		{
			if ( usDataBytes >= 28 ) 
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_AppleTalkAddr );

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_DestLogical.fMask = nMask;
				// TODO: why not using pHeader?
				*(UInt16*) &m_DestLogical.fData[0] = *(UInt16*) &pData[25];
				m_DestLogical.fData[2] = pData[27];
			}
		}
		break;

		// EType2-DECnet
		case ProtoSpecDefs::DECnet:
		{
			if ( m_MediaType == kMediaType_802_3 )
			{
				// Get the physical layer.
				pData       = NULL;
				usDataBytes = 0;
				hr = GetPhysicalLayer( &pData, &usDataBytes );
				if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 14) )
				{
					if ( (*(UInt32*) &((EthernetPacketHeader*)pData)->SrcAddr[0]) == NETWORKTOHOST32((UInt32)0xAA000400) )
					{
						m_SrcLogical.SetClass( kMediaSpecClass_Address );
						m_SrcLogical.SetType( kMediaSpecType_DECnetAddr );
						m_SrcLogical.fMask = m_SrcLogical.GetTypeMask();
						*(UInt16*) &m_SrcLogical.fData[0] =
							*(UInt16*) &((EthernetPacketHeader*)pData)->SrcAddr[4];
					}

					if ( (*(UInt32*) &((EthernetPacketHeader*)pData)->DestAddr[0]) == NETWORKTOHOST32((UInt32)0xAA000400) )
					{
						m_DestLogical.SetClass( kMediaSpecClass_Address );
						m_DestLogical.SetType( kMediaSpecType_DECnetAddr );
						m_DestLogical.fMask = m_DestLogical.GetTypeMask();
						*(UInt16*) &m_DestLogical.fData[0] =
							*(UInt16*) &((EthernetPacketHeader*)pData)->DestAddr[4];
					}
				}
			}
			else if ( m_MediaType == kMediaType_802_5 )
			{
				// Get the physical layer.
				pData       = NULL;
				usDataBytes = 0;
				hr = GetPhysicalLayer( &pData, &usDataBytes );
				if ( HE_SUCCEEDED(hr) && (pData != NULL) && (usDataBytes >= 14) )
				{
					if ( ((*(UInt32*) &((TokenRingPacketHeader*)pData)->SrcAddr[0]) & 0xFFFFFF7F) == NETWORKTOHOST32((UInt32)0x55000200) )
					{
						m_SrcLogical.SetClass( kMediaSpecClass_Address );
						m_SrcLogical.SetType( kMediaSpecType_DECnetAddr );
						m_SrcLogical.fMask = m_SrcLogical.GetTypeMask();
						*(UInt16*) &m_SrcLogical.fData[0] =
							*(UInt16*) &((TokenRingPacketHeader*)pData)->SrcAddr[4];
						m_SrcLogical.fData[0] = MemUtil::Reverse8( m_SrcLogical.fData[0] );
						m_SrcLogical.fData[1] = MemUtil::Reverse8( m_SrcLogical.fData[1] );
					}

					if ( ((*(UInt32*) &((TokenRingPacketHeader*)pData)->DestAddr[0]) & 0xFFFFFF7F) == NETWORKTOHOST32((UInt32)0x55000200) )
					{
						m_DestLogical.SetClass( kMediaSpecClass_Address );
						m_DestLogical.SetType( kMediaSpecType_DECnetAddr );
						m_DestLogical.fMask = m_DestLogical.GetTypeMask();
						*(UInt16*) &m_DestLogical.fData[0] =
							*(UInt16*) &((TokenRingPacketHeader*)pData)->DestAddr[4];
						m_DestLogical.fData[0] = MemUtil::Reverse8( m_DestLogical.fData[0] );
						m_DestLogical.fData[1] = MemUtil::Reverse8( m_DestLogical.fData[1] );
					}
				}
			}
		}
		break;

		case ProtoSpecDefs::IPX:
		{
			static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPXAddr );

			if ( usDataBytes >= 28 )
			{
				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_IPXAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt32*) &m_SrcLogical.fData[0] = *(UInt32*) &pData[18];
				*(UInt32*) &m_SrcLogical.fData[4] = *(UInt32*) &pData[22];
				*(UInt16*) &m_SrcLogical.fData[8] = *(UInt16*) &pData[26];
			}

			if ( usDataBytes >= 18 )
			{
				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_IPXAddr );
				m_DestLogical.fMask = nMask;
				*(UInt32*) &m_DestLogical.fData[0] = *(UInt32*) &pData[6];
				*(UInt32*) &m_DestLogical.fData[4] = *(UInt32*) &pData[10];
				*(UInt16*) &m_DestLogical.fData[8] = *(UInt16*) &pData[14];
			}
		}
		break;
	}
}


// -----------------------------------------------------------------------------
//		CalcPorts
// -----------------------------------------------------------------------------

void
Packet::CalcPorts()
{
	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return;

	// Invalidate the ports.
	m_SrcPort.Invalidate();
	m_DestPort.Invalidate();

	// Check for a type we know how to find the source port for.
	static const UInt16	s_ParentArray[] =
	{
		ProtoSpecDefs::TCP,
		ProtoSpecDefs::UDP,
		ProtoSpecDefs::AppleTalk_Ph2,
		ProtoSpecDefs::Long_DDP,
		ProtoSpecDefs::Short_DDP,
		ProtoSpecDefs::IPX,
		ProtoSpecDefs::XNS
	};
	HeResult			hr;
	PROTOSPEC_INSTID	matchID;
	SInt32				lParentIndex;
	BOOL				bResult;
	hr = IsDescendentOf( s_ParentArray, COUNTOF(s_ParentArray),
		&matchID, &lParentIndex, &bResult );
	if ( HE_FAILED(hr) || !bResult ) return;
	const UInt32		psid = s_ParentArray[lParentIndex];

	// Get a pointer to the start of packet data.
	const UInt8*	pData       = NULL;
	UInt16			usDataBytes = 0;
	hr = GetNetworkLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return;

	switch ( psid )
	{
		case ProtoSpecDefs::TCP:
		case ProtoSpecDefs::UDP:
		{
			const UInt8*		pHeader;
			UInt16				cbHeader;
			hr = GetHeaderLayer( GET_PSID( matchID ), NULL, &pHeader, &cbHeader );
			if ( HE_SUCCEEDED(hr) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPPort );

				if ( cbHeader >= 2 ) 
				{
					m_SrcPort.SetClass( kMediaSpecClass_Port );
					m_SrcPort.SetType( kMediaSpecType_IPPort );
					m_SrcPort.fMask = nMask;
					*(UInt16*) &m_SrcPort.fData[0] = *(UInt16*) &pHeader[0];
				}

				if ( cbHeader >= 4 ) 
				{
					m_DestPort.SetClass( kMediaSpecClass_Port );
					m_DestPort.SetType( kMediaSpecType_IPPort );
					m_DestPort.fMask = nMask;
					*(UInt16*) &m_DestPort.fData[0] = *(UInt16*) &pHeader[2];
				}
			}
		}
		break;

		case ProtoSpecDefs::AppleTalk_Ph2:
		{
			static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ATPort );

			if ( usDataBytes >= 12 )
			{
				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_ATPort );
				m_SrcPort.fMask    = nMask;
				m_SrcPort.fData[0] = pData[11];
			}

			if ( usDataBytes >= 11 )
			{
				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_ATPort );
				m_DestPort.fMask    = nMask;
				m_DestPort.fData[0] = pData[10];
			}
		}
		break;

		case ProtoSpecDefs::Long_DDP:
		{
			static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ATPort );

			if ( usDataBytes >= 15 )
			{
				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_ATPort );
				m_SrcPort.fMask    = nMask;
				m_SrcPort.fData[0] = pData[14];
			}

			if ( usDataBytes >= 6 )
			{
				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_ATPort );
				m_DestPort.fMask    = nMask;
				m_DestPort.fData[0] = pData[5];
			}
		}
		break;

		case ProtoSpecDefs::Short_DDP:
		{
			static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ATPort );

			if ( usDataBytes >= 7 )
			{
				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_ATPort );
				m_SrcPort.fMask    = nMask;
				m_SrcPort.fData[0] = pData[6];
			}

			if ( usDataBytes >= 6 )
			{
				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_ATPort );
				m_DestPort.fMask    = nMask;
				m_DestPort.fData[0] = pData[5];
			}
		}
		break;

		case ProtoSpecDefs::IPX:
		case ProtoSpecDefs::XNS:
		{
			static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_NWPort );

			if ( usDataBytes >= 30 )
			{
				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_NWPort );
				m_SrcPort.fMask = nMask;
				*(UInt16*) m_SrcPort.fData = *(UInt16*) &pData[28];
			}

			if ( usDataBytes >= 18 )
			{
				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_NWPort );
				m_DestPort.fMask = nMask;
				*(UInt16*) m_DestPort.fData = *(UInt16*) &pData[16];
			}
		}
		break;
	}
}


// -----------------------------------------------------------------------------
//		CalcBSSID
// -----------------------------------------------------------------------------

bool
Packet::CalcBSSID(
	CMediaSpec& outSpec )
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	// Get the physical layer.
	HeResult		hr;
	const UInt8*	pData = NULL;
	UInt16			usDataBytes = 0;
	hr = GetPhysicalLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return false;

	switch ( m_MediaSubType )
	{
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_gen:
		{
			if ( (pData != NULL) && (usDataBytes >= 2) )
			{
				// Get the 802.11 MAC header.
				const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

				const UInt8*	pAddr = NULL;
				switch ( pHdr->FrameControlType & 0x0C )
				{
					case 0x00:	// Management, Addr1 = DA, Addr2 = SA, Addr3 = BSSID
					{
						if ( usDataBytes >= 22 )
						{
							pAddr = &pHdr->Addr3[0];
						}
					}
					break;

					case 0x08:	// Data
					{
						// Get the address to use from the FrameControl fields "To DS" and "From DS".
						switch ( pHdr->FrameControlFlags & 0x03 )
						{
							case 0x00:	// "To DS"=0, "From DS"=0.
							{
								if ( usDataBytes >= 22 )
								{
									pAddr = &pHdr->Addr3[0];
								}
							}
							break;

							case 0x01:	// "To DS"=1, "From DS"=0.
							{
								if ( usDataBytes >= 10 )
								{
									pAddr = &pHdr->Addr1[0];
								}
							}
							break;

							case 0x02:	// "To DS"=0, "From DS"=1.
							{
								if ( usDataBytes >= 16 )
								{
									pAddr = &pHdr->Addr2[0];
								}
							}
							break;

							case 0x03:	// "To DS"=1, "From DS"=1.
							{
								// No BSSID.
							}
							break;
						}
					}
					break;

					case 0x04:	// Control
					{
						switch ( pHdr->FrameControlType & 0xF0 )
						{
							case 0xA0:	// PS-Poll
							{
								if ( usDataBytes >= 10 )
								{
									pAddr = &pHdr->Addr1[0];
								}
							}
							break;

							case 0xE0:	// CF-End
							case 0xF0:	// CF-End + CF-Ack
							{
								if ( usDataBytes >= 16 )
								{
									pAddr = &pHdr->Addr2[0];
								}
							}
							break;
						}
					}
					break;
				}

				if ( pAddr != NULL )
				{
					// Set up the spec.
					static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_WirelessAddr );
					outSpec.SetClass( kMediaSpecClass_Address );
					outSpec.SetType( kMediaSpecType_WirelessAddr );
					outSpec.fMask = nMask;
					*(UInt32*) &outSpec.fData[0] = *(UInt32*) &pAddr[0];
					*(UInt16*) &outSpec.fData[4] = *(UInt16*) &pAddr[4];
				}
			}
		}
		break;

		default:
		{
			ASSERT( 0 );
		}
		break;
	}

	return (outSpec.GetClass() != kMediaSpecClass_Null);
}


// -----------------------------------------------------------------------------
//		CalcReceiver
// -----------------------------------------------------------------------------

bool
Packet::CalcReceiver(
	CMediaSpec& outSpec )
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	// Get the physical layer.
	HeResult		hr;
	const UInt8*	pData       = NULL;
	UInt16			usDataBytes = 0;
	hr = GetPhysicalLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return false;

	switch ( m_MediaSubType )
	{
		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_gen:
		{
			if ( (pData != NULL) && (usDataBytes >= 2) )
			{
				// Get the 802.11 MAC header.
				const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

				const UInt8*	pAddr = NULL;
				switch ( pHdr->FrameControlType & 0x0C )
				{
					case 0x00:	// Management
					case 0x08:	// Data
					{
						// The receiver is always Address 1.
						if ( usDataBytes >= 10 )
						{
							pAddr = &pHdr->Addr1[0];
						}
					}
					break;

					case 0x04:	// Control
					{
						switch ( pHdr->FrameControlType & 0xF0 )
						{
							case 0x80:	// BlockAckReq
							case 0x90:	// BlockAck
							case 0xA0:	// PS-Poll
							case 0xB0:	// RTS
							case 0xC0:	// CTS
							case 0xD0:	// Ack
							case 0xE0:	// CF-End
							case 0xF0:	// CF-End + CF-Ack
							{
								// The receiver is always Address 1.
								if ( usDataBytes >= 10 )
								{
									pAddr = &pHdr->Addr1[0];
								}
							}
							break;
						}
					}
					break;
				}

				if ( pAddr != NULL )
				{
					// Set up the spec.
					static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_WirelessAddr );
					outSpec.SetClass( kMediaSpecClass_Address );
					outSpec.SetType( kMediaSpecType_WirelessAddr );
					outSpec.fMask = nMask;
					*(UInt32*) &outSpec.fData[0] = *(UInt32*) &pAddr[0];
					*(UInt16*) &outSpec.fData[4] = *(UInt16*) &pAddr[4];
				}
			}
		}
		break;

		default:
		{
			ASSERT( 0 );
		}
		break;
	}

	return (outSpec.GetClass() != kMediaSpecClass_Null);
}


// -----------------------------------------------------------------------------
//		CalcTransmitter
// -----------------------------------------------------------------------------

bool
Packet::CalcTransmitter(
	CMediaSpec& outSpec )
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPacketHeader != NULL );
	if ( m_pPacketHeader == NULL ) return false;

	// Get the physical layer.
	HeResult		hr;
	const UInt8*	pData       = NULL;
	UInt16			usDataBytes = 0;
	hr = GetPhysicalLayer( &pData, &usDataBytes );
	if ( HE_FAILED(hr) ) return false;

	switch ( m_MediaSubType )
	{
		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_gen:
		{
			if ( (pData != NULL) && (usDataBytes >= 2) )
			{
				// Get the 802.11 MAC header.
				const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

				const UInt8*	pAddr = NULL;
				switch ( pHdr->FrameControlType & 0x0C )
				{
					case 0x00:	// Management
					case 0x08:	// Data
					{
						// The transmitter is always Address 2.
						if ( usDataBytes >= 16 )
						{
							pAddr = &pHdr->Addr2[0];
						}
					}
					break;

					case 0x04:	// Control
					{
						switch ( pHdr->FrameControlType & 0xF0 )
						{
							case 0x80:	// BlockAckReq
							case 0x90:	// BlockAck
							case 0xA0:	// PS-Poll
							case 0xB0:	// RTS
							case 0xE0:	// CF-End
							case 0xF0:	// CF-End + CF-Ack
							{
								if ( usDataBytes >= 16 )
								{
									pAddr = &pHdr->Addr2[0];
								}
							}
							break;

							case 0xC0:	// CTS
							case 0xD0:	// ACK
							{
								// 802.11 control packets of type ACK and CTS only carry the receiver address.
								// For ACK packets, the transmitter is implied from the "immediately previous
								// directed data, management, or PS-POLL control frame".
								// For CTS packets, the transmitter is implied from the receiver in the
								// immediately preceding RTS frame.

								// Get the packet's receiver.
								CMediaSpec	specReceiver;
								if ( (m_pPacketBuffer != NULL) &&
									HE_SUCCEEDED(GetAddress1( &specReceiver ) ) )
								{
									// Get this packet's buffer index.
									const UInt32	ulIndex = m_PacketBufferIndex;

									CHeObjStack<Packet>	thePrevPacket;

									for ( UInt32 ulOffset = 1; ulOffset <= 5; ulOffset++ )
									{
										// Sanity check the buffer index and offset.
										if ( ulOffset > ulIndex ) break;

										// Get the previous packet.
										hr = m_pPacketBuffer->GetPacket( &thePrevPacket, ulIndex - ulOffset );
										if ( HE_FAILED(hr) ) break;

										// Don't trust it if it's a CRC error packet.
										BOOL	bError;
										hr = thePrevPacket.TestFlag( kPacketFlag_CRC, &bError );
										if ( HE_FAILED(hr) || bError ) break;

										// Get the previous packet's transmitter.
										CMediaSpec	specPrevTransmitter;
										hr = thePrevPacket.GetAddress2( &specPrevTransmitter );
										if ( HE_SUCCEEDED(hr) )
										{
											// Compare this packet's receiver with
											// the previous packet's transmitter.
											if ( specReceiver == specPrevTransmitter )
											{
												// The receiver from the previous packet is the
												// implied transmitter of this packet.
												hr = thePrevPacket.GetAddress1( &outSpec );

												// Note that we've gotten this from another packet.
												m_bTransmitterImplied = true;

												// Stop looking.
												break;
											}
										}
									}
								}
							}
							break;
						}
					}
					break;
				}

				if ( pAddr != NULL )
				{
					// Set up the spec.
					static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_WirelessAddr );
					outSpec.SetClass( kMediaSpecClass_Address );
					outSpec.SetType( kMediaSpecType_WirelessAddr );
					outSpec.fMask = nMask;
					*(UInt32*) &outSpec.fData[0] = *(UInt32*) &pAddr[0];
					*(UInt16*) &outSpec.fData[4] = *(UInt16*) &pAddr[4];
				}
			}
		}
		break;

		default:
		{
			ASSERT( 0 );
		}
		break;
	}

	return (outSpec.GetClass() != kMediaSpecClass_Null);
}


// -----------------------------------------------------------------------------
//		CalcIPHeader
// -----------------------------------------------------------------------------

bool
Packet::CalcIPHeader()
{
	if ( !m_bIPHeader )
	{
		m_bIPHeader = true;
		m_bIPv4   = false;

		// Get IPv4 header.
		const UInt8*	pHeader;
		UInt16			cbHeader;
		HeResult		hr = GetHeaderLayer( ProtoSpecDefs::IP, NULL, &pHeader, &cbHeader );
		if ( HE_SUCCEEDED(hr) && (cbHeader >= 8) )
		{
			// Check IP version.
			const UInt8	nVersion = pHeader[0] & 0xF0;
			m_bIPv4 = (nVersion == 0x40);
			if ( m_bIPv4 )
			{
				using namespace MemUtil;

				m_IPTotalLength  = GetShort( pHeader + 2, __BIG_ENDIAN );
				m_IPID           = GetShort( pHeader + 4, __BIG_ENDIAN );
				m_IPFragmentBits = GetShort( pHeader + 6, __BIG_ENDIAN );
			}
		}
	}

	return m_bIPv4;												
}


// -----------------------------------------------------------------------------
//		CalcTCPHeader
// -----------------------------------------------------------------------------

bool	
Packet::CalcTCPHeader()
{
	if ( !m_bTCPHeader )
	{
		m_bTCPHeader = true;
		m_bTCP       = false;

		// Get the TCP header.
		const UInt8*	pHeader;
		UInt16			cbHeader;
		HeResult		hr = GetHeaderLayer( ProtoSpecDefs::TCP, NULL, &pHeader, &cbHeader );
		if ( HE_SUCCEEDED(hr) && (cbHeader >= 20) )
		{
			m_bTCP = true;

			m_TCPSequence      = MemUtil::GetLong(  pHeader +  4, __BIG_ENDIAN );
			m_TCPAck           = MemUtil::GetLong(  pHeader +  8, __BIG_ENDIAN );
			m_TCPFlags         = pHeader[13] & 0x3F;
			m_TCPWindow        = MemUtil::GetShort( pHeader + 14, __BIG_ENDIAN );
			m_TCPChecksum      = MemUtil::GetShort( pHeader + 16, __BIG_ENDIAN );
			m_TCPUrgentPointer = MemUtil::GetShort( pHeader + 18, __BIG_ENDIAN );
 
			const UInt16		cbTCPHeader = (pHeader[12] & 0xF0) >> 2;
			cbHeader = min( cbHeader, cbTCPHeader );
			if ( cbHeader > 20 )
			{
				m_pTCPOptionBegin = pHeader + 20;
				m_pTCPOptionEnd   = pHeader + cbHeader;
			}
			else
			{
				m_pTCPOptionBegin = NULL;
				m_pTCPOptionEnd   = NULL;
			}
		}
	}

	return m_bTCP;
}                      	


// -----------------------------------------------------------------------------
//		CalcTCPOptions
// -----------------------------------------------------------------------------

void
Packet::CalcTCPOptions()
{
	if ( !m_bTCPOptions )
	{
		m_bTCPOptions = true;

		// Haven't seen any of these options yet.
		m_bTCPOptionMSS         = false;
		m_bTCPOptionWindowScale = false;
		m_bTCPOptionTimestamp   = false;
		m_bTCPOptionSACK        = false;

		// Get TCP header information.
		if ( CalcTCPHeader() )
		{
			// Note: m_pTCPOptionBegin and m_pTCPOptionEnd are NULL if there are no options.

			// Walk the option list.
			for ( const UInt8* pIter = m_pTCPOptionBegin; pIter < m_pTCPOptionEnd; )
			{
				// Get the TCP option type.
				const UInt8	nType = *pIter++;
				
				// Skip 1-byte length-less NOP option.
				if ( nType == TCP_OPTION_NOOP ) continue;

				// Stop if we see an end marker.
				if ( nType == TCP_OPTION_END ) break;

				// Re-check data length.
				if ( pIter >= m_pTCPOptionEnd ) break;

				// Get option length and option data (if any).
				const UInt8		cbData = *pIter++;
				const UInt8*	pData  =  pIter;
				pIter += static_cast<size_t>(cbData);

				// Check for all option data present.
				if ( pIter > m_pTCPOptionEnd ) break;

				switch ( nType )
				{
					case TCP_OPTION_MSS:
						m_bTCPOptionMSS = true;
						m_TCPOptionMSS  = MemUtil::GetShort( pData, __BIG_ENDIAN );
						break;
						
					case TCP_OPTION_WINDOW_SCALE:
						m_bTCPOptionWindowScale = true;
						m_TCPOptionWindowScale  = *pData;
						break;
						
					case TCP_OPTION_SELECTIVE_ACK:
						m_bTCPOptionSACK     = true;
						m_TCPOptionSACKBegin = MemUtil::GetLong( pData, __BIG_ENDIAN );
						m_TCPOptionSACKEnd   = MemUtil::GetLong( pData + 4, __BIG_ENDIAN );
						break;
						
					case TCP_OPTION_TIMESTAMP:
						m_bTCPOptionTimestamp     = true;
						m_TCPOptionTimestampValue = MemUtil::GetLong( pData, __BIG_ENDIAN );
						m_TCPOptionTimestampEcho  = MemUtil::GetLong( pData + 4, __BIG_ENDIAN );
						break;
				}
			}
		}
	}
}
