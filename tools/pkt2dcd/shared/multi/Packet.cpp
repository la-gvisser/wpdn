// =============================================================================
//	Packet.cpp
// =============================================================================
//	Copyright (c) 1996-2014 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include "Packet.h"
#include "CRC16.h"
#include "CRC32.h"
#include "MediaSpecUtil.h"
#include "MemUtil.h"
#include "PSIDs.h"
#include "PacketBuffer.h"
#include "PacketHeaders.h"
#include <algorithm>
#include <string.h>

#ifdef PRODUCT	// defined in Peek.h
	// Peek gets a mighty and potent application-layer decoding object.
	#include "AppLayer.h"	// peek/source/AppLayer.h + cpp
#else  // !PRODUCT
	// Non-peek projects get a pathetic and useless stub.
	class AppLayer
	{
	public :
		AppLayer( const CPacket * const /*inPacket*/ = NULL ) { }
		AppLayer( const AppLayer & /*inOriginal*/ ) { }
		~AppLayer() { }
		AppLayer & operator = ( const AppLayer& /*inOriginal*/ ) { return *this; }
	};
#endif // PRODUCT

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

namespace
{
	// Special values for testing DECnet address.
#if __BYTE_ORDER == __BIG_ENDIAN
	const UInt32	kDECEthernetAddrValue  = 0xAA000400;
	const UInt32	kDECTokenRingAddrValue = 0x55000200;
#else
	const UInt32	kDECEthernetAddrValue  = 0x000400AA;
	const UInt32	kDECTokenRingAddrValue = 0x00200055;
#endif

	const size_t VLAN_ID_OFFSET		        = 0;
	const UInt16 VLAN_ID_MASK				= 0x0FFF;
	const size_t VLAN_ID_SIZE				= sizeof(UInt16);
	const size_t VLAN_ID_LAYER_SIZE			= sizeof(UInt16) + sizeof(UInt16);

	const size_t MPLS_LABEL_OFFSET		    = 0;
	const UInt32 MPLS_LABEL_SHIFT		    = 12;
	const size_t MPLS_LABEL_SIZE			= sizeof(UInt32);
	const size_t MPLS_LABEL_LAYER_SIZE	    = sizeof(UInt32);

	inline bool PreferRawProtocol( UInt32 id )
	{
		if ( IS_ROOT_PSID( id ) ) return true;
		const UInt16	nPSID = GET_PSID( id );
		return (nPSID == ProtoSpecDefs::SNAP) || 
				(nPSID == ProtoSpecDefs::LSAP) || 
				(nPSID == ProtoSpecDefs::IEEE_802_11_Data);
	}
}


// =============================================================================
//	Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
//		CPacket
// -----------------------------------------------------------------------------

CPacket::CPacket() :
	m_MediaType( kMediaType_802_3 ),
	m_MediaSubType( kMediaSubType_Native ),
	m_pPeekPacket( NULL ),
	m_pPacketData( NULL ),
	m_bOwnsPacket( false ),
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
	m_bTCPOption( false ),
	m_bTCPOptionWindowScaleCached( false ),
	m_bTCPOptionWindowScaleExists( false ),
	m_bTCPOptionSACK( false ),
	m_bTCPOptionMSS( false ),
	m_pAddress1( NULL ),
	m_pAddress2( NULL ),
	m_pAddress3( NULL ),
	m_pAddress4( NULL ),
	m_ProtocolIndex( static_cast<UInt32>(-1) ),
	m_SrcPhysicalIndex( static_cast<UInt32>(-1) ),
	m_DestPhysicalIndex( static_cast<UInt32>(-1) ),
	m_SrcLogicalIndex( static_cast<UInt32>(-1) ),
	m_DestLogicalIndex( static_cast<UInt32>(-1) ),
	m_pSourceBuffer( NULL),
	m_nPacketIndex( 0 ),
	m_bProtospecArray( false ),
	m_pAppLayer(),
	m_bFlowID( false ),
	m_bVoIPCallID( false ),
	m_bVoIPCallFlowIndex( false ),
	m_bVLANID( false ),
	m_bMPLSLabel( false ),
	m_FrameCheckSequence( 0 ),
	m_pWirelessHeaderFacade( 0 )
{}


// -----------------------------------------------------------------------------
//		CPacket(const CPacket&)
// -----------------------------------------------------------------------------

CPacket::CPacket(
	const CPacket&	inOriginal ) :
		m_bOwnsPacket( false ),
		m_pSourceBuffer( NULL ),
		m_pWirelessHeaderFacade( 0 )
{
	// SetPacket will initialize all members.
	SetPacket(
		inOriginal.m_MediaType,
		inOriginal.m_MediaSubType,
		inOriginal.m_pPeekPacket,
		inOriginal.m_pPacketData,
		inOriginal.m_bOwnsPacket,
		inOriginal.m_nPacketIndex,
		inOriginal.m_pSourceBuffer );
}


// -----------------------------------------------------------------------------
//		CPacket(TMediaType,TMediaSubType,PeekPacket*,UInt8*,bool)
// -----------------------------------------------------------------------------

CPacket::CPacket(
	TMediaType				inMediaType,
	TMediaSubType			inSubType,
	PeekPacket*				inPacket,
	UInt8*					inData,
	bool					inCopy,
	UInt32					inPacketIndex,
	const CPacketBuffer*	inPacketBuffer) :
		m_bOwnsPacket( false ),
		m_pSourceBuffer( NULL ),
		m_pWirelessHeaderFacade( 0 )
{
	// SetPacket will initialize all members.
	SetPacket( inMediaType, inSubType, inPacket, inData, inCopy, inPacketIndex, inPacketBuffer);
}


// -----------------------------------------------------------------------------
//		~CPacket
// -----------------------------------------------------------------------------

CPacket::~CPacket()
{
	if ( m_bOwnsPacket )
	{
		// Free the packet.
		UInt8*	pData = (UInt8*) m_pPeekPacket;
		delete [] pData;
	}

	if ( m_pSourceBuffer != NULL )
	{
		m_pSourceBuffer->ReleasePacketLock( m_nPacketIndex );
	}
}


// =============================================================================
//	Public Interface
// =============================================================================

// -----------------------------------------------------------------------------
//		operator=
// -----------------------------------------------------------------------------

CPacket&
CPacket::operator=(
	const CPacket&	inOriginal )
{
	if ( this != &inOriginal )
	{
		SetPacket(
			inOriginal.m_MediaType,
			inOriginal.m_MediaSubType,
			inOriginal.m_pPeekPacket,
			inOriginal.m_pPacketData,
			inOriginal.m_bOwnsPacket,
			inOriginal.m_nPacketIndex,
			inOriginal.m_pSourceBuffer);
	}

	return *this;
}


// -----------------------------------------------------------------------------
//		SetPacket
// -----------------------------------------------------------------------------

void
CPacket::SetPacket(
	TMediaType				inMediaType,
	TMediaSubType			inSubType,
	PeekPacket*				inPacket,
	UInt8*					inData,
	bool					inCopy,
	UInt32					inPacketIndex,
	const CPacketBuffer*	inPacketBuffer )
{
	// Cleanup previously owned data.
	if ( m_bOwnsPacket )
	{
		delete [] m_pPeekPacket;
		m_pPeekPacket = NULL;
	}	

	if ( m_pSourceBuffer != NULL )
	{
		m_pSourceBuffer->ReleasePacketLock( m_nPacketIndex );
		m_pSourceBuffer = NULL;
		m_nPacketIndex = 0;
	}

	// Set the media type.
	m_MediaType = inMediaType;

	// Set the physical medium.
	m_MediaSubType = inSubType;

	if ( inPacket != NULL )
	{
		if ( inCopy )
		{
			// Get the data length.
			UInt16	nPacketBytes = inPacket->fSliceLength;
			if ( nPacketBytes == 0 ) nPacketBytes = inPacket->fPacketLength;

			// Allocate a contiguous buffer for the header, media spec info, and data.
			UInt8*	pData = NULL;
			UInt32  nInfoBlockSize = 0;
			try
			{
				UInt32 nLength = sizeof(PeekPacket) + nPacketBytes;
				const MediaSpecificPrivateHeader* pHdr = 
					reinterpret_cast<const MediaSpecificPrivateHeader*>(inPacket->fMediaSpecInfoBlock);
				if (pHdr != NULL)
				{
					nInfoBlockSize = pHdr->nSize;
					nLength += nInfoBlockSize;
				}
				pData = new UInt8[nLength];
			}
			catch(...)
			{
				m_pPeekPacket = NULL;
				m_pPacketData = NULL;
				pData = NULL;
			}

			if ( pData != NULL )
			{
				m_pPeekPacket = reinterpret_cast<PeekPacket*>(pData);
				m_pPacketData = pData + nInfoBlockSize + sizeof(PeekPacket);

				// Copy the header.
				memmove( m_pPeekPacket, inPacket, sizeof(PeekPacket) );
				// if there is an info block, copy that too
				if (nInfoBlockSize != 0)
				{
					// copy the info
					memmove( pData + sizeof(PeekPacket), inPacket->fMediaSpecInfoBlock,
								nInfoBlockSize );
					m_pPeekPacket->fMediaSpecInfoBlock = pData + sizeof(PeekPacket);
				}

				// Copy the data.
				if ( inData != NULL )
				{
					memmove( m_pPacketData, inData, nPacketBytes );
				}
				else
				{
					memmove( m_pPacketData, ((UInt8*)inPacket) + nInfoBlockSize + sizeof(PeekPacket), nPacketBytes );
				}

				m_nPacketIndex = inPacketIndex;

				// Now owns the packet.
				m_bOwnsPacket = true;
			}
		}
		else
		{
			// Use the packet header given.
			m_pPeekPacket = inPacket;

			if ( inPacketBuffer != NULL )
			{
				m_pSourceBuffer = inPacketBuffer;
				m_nPacketIndex  = inPacketIndex;
				m_pSourceBuffer->AddPacketLock( inPacketIndex );
			}
			// If inData is NULL, the data is contiguous.
			// Use the packet data following the PeekPacket and info block.
			if ( inData != NULL )
			{
				m_pPacketData = inData;
			}
			else
			{
				UInt32 nInfoLength = 0;
				const MediaSpecificPrivateHeader* pHdr = 
					reinterpret_cast<const MediaSpecificPrivateHeader*>(inPacket->fMediaSpecInfoBlock);
				if ( pHdr != NULL )
				{
					nInfoLength = pHdr->nSize;
				}
				
				m_pPacketData = ((UInt8*)inPacket) + nInfoLength + sizeof(PeekPacket);
			}

			// Doesn't own the packet.
			m_bOwnsPacket = false;
		}

		{
			const MediaSpecificHeaderAll* pHdr =
				reinterpret_cast<const MediaSpecificHeaderAll*>(m_pPeekPacket->fMediaSpecInfoBlock);

			ConstructWirelessHeaderFacade(pHdr);
		}
	}
	else
	{
		m_pPeekPacket = NULL;
		m_pPacketData = NULL;
		m_bOwnsPacket = false;

		ConstructWirelessHeaderFacade(0);
	}

	// Reset the cache flags.
	m_bProtocol							= false;
	m_bCalcPhysical						= false;
	m_bCalcLogical						= false;
	m_bCalcPorts						= false;
	m_bBSSID							= false;
	m_bReceiver							= false;
	m_bTransmitter						= false;
	m_bAddress1							= false;
	m_bAddress2							= false;
	m_bAddress3							= false;
	m_bAddress4							= false;
	m_bTransmitterImplied				= false;
	m_bTransmitterAuthenticationType	= false;
	m_bTransmitterEncryptionType		= false;
	m_bReceiverEncryptionType			= false;
	m_bWEPKeyIndexChecked				= false;
	m_bWEPKeyIndexValid					= false;
	m_bTCPOption						= false;
	m_bTCPOptionWindowScaleCached		= false;
	m_bTCPOptionWindowScaleExists		= false;
	m_bTCPOptionSACK					= false;
	m_bTCPOptionMSS						= false;
	m_bFlowID							= false;
	m_bVoIPCallID						= false;
	m_bVoIPCallFlowIndex				= false;
	m_bVLANID							= false;
	m_bMPLSLabel						= false;
	m_pAddress1							= NULL;
	m_pAddress2							= NULL;
	m_pAddress3							= NULL;
	m_pAddress4							= NULL;
	m_ProtocolIndex						= static_cast<UInt32>(-1);
	m_SrcPhysicalIndex					= static_cast<UInt32>(-1);
	m_DestPhysicalIndex					= static_cast<UInt32>(-1);
	m_SrcLogicalIndex					= static_cast<UInt32>(-1);
	m_DestLogicalIndex					= static_cast<UInt32>(-1);
	m_bProtospecArray					= false;
	m_FrameCheckSequence				= 0;
}


// -----------------------------------------------------------------------------
//		GetPacketNumber
// -----------------------------------------------------------------------------

UInt64
CPacket::GetPacketNumber() const
{
	if ( NULL == m_pSourceBuffer ) return (UInt64) m_nPacketIndex;
	return m_pSourceBuffer->PacketIndexToPacketNumber( m_nPacketIndex );
}


// -----------------------------------------------------------------------------
//		GetProtoSpec
// -----------------------------------------------------------------------------

PROTOSPEC_INSTID
CPacket::GetProtoSpec() const
{
	ASSERT( m_pPeekPacket != NULL );

	if ( (m_pPeekPacket->fProtoSpec == 0) || !m_bProtospecArray )
	{
		PROTOSPEC_INSTID	psid = m_pPeekPacket->fProtoSpec;
		int					nPSInfo = MAX_PROTOSPEC_DEPTH;
		if ( SUCCEEDED(ProtoSpecs::GetPacketProtoSpecCombined(
			m_MediaType, m_MediaSubType, m_pPeekPacket->fFlags,
			m_pPacketData, this->GetActualLength(), psid, m_ProtospecArray, nPSInfo )) )
		{
			m_pPeekPacket->fProtoSpec = static_cast<UInt32>(psid);
			m_nProtospecCount = nPSInfo;
		}
		else
		{
			m_nProtospecCount = 0;
		}

		m_bProtospecArray = true;
	}

	return m_pPeekPacket->fProtoSpec;
}


// -----------------------------------------------------------------------------
//		GetProtoSpecID
// -----------------------------------------------------------------------------

PROTOSPEC_ID
CPacket::GetProtoSpecID() const
{
	return GET_PSID( this->GetProtoSpec() );
}


// -----------------------------------------------------------------------------
//		IsMulticast
// -----------------------------------------------------------------------------

bool
CPacket::IsMulticast() const
{
	CMediaSpec	theSpec;
	if ( GetDestPhysical( theSpec ) )
	{
		return theSpec.IsMulticast();
	}

	return false;
}


// -----------------------------------------------------------------------------
//		IsBroadcast
// -----------------------------------------------------------------------------

bool
CPacket::IsBroadcast() const
{
	CMediaSpec	theSpec;
	if ( GetDestPhysical( theSpec ) )
	{
		return theSpec.IsBroadcast();
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetPhysicalLayer
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetPhysicalLayer(
	UInt16*	outBytesRemaining ) const
{
	// Get the packet data.
	const UInt8*	pData = GetPacketData();

	switch ( GetMediaType() )
	{
		case kMediaType_802_3:
		{
			switch ( GetMediaSubType() )
			{
				case kMediaSubType_Native:
				{
					// The entire packet is available.
					if ( pData != NULL )
					{
						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = GetActualLength();
						}
					}
				}
				break;

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:		// gen for GENERIC (any bands, including multibands
				{
					// The entire packet is available.
					if ( pData != NULL )
					{
						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = GetActualLength();
						}
					}
				}
				break;

				default:
					break;
			}
		}
		break;

		case kMediaType_CoWan:
		case kMediaType_Wan:
		{
			// get the real media type from the protocol field
			TMediaSubType theSubType;
			VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol( GetMediaType(), GetWanProtocol(), &theSubType ) == ERROR_SUCCESS );

			switch ( theSubType )
			{
				case kMediaSubType_wan_ppp:			// PPP
				case kMediaSubType_wan_frameRelay:	// Frame Relay
				case kMediaSubType_wan_x25:			// X.25
				case kMediaSubType_wan_x25e:		// X.25 modulo 128
				case kMediaSubType_wan_ipars:		// IPARS
				case kMediaSubType_wan_u200:		// U200
				case kMediaSubType_wan_Q931:		// BRI Q.931
				{
					// The entire packet is available.
					if ( pData != NULL )
					{
						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = GetActualLength();
						}
					}
				}
				break;

				default:
				{
					ASSERT( false );
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

	return pData;
}


// -----------------------------------------------------------------------------
//		GetLinkLayer
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetLinkLayer(
	UInt16*	outBytesRemaining ) const
{
	// TODO: This should use ProtoSpecs to get the layer

	// Get the physical layer.
	const UInt8*	pData;
	UInt16			cbData;
	pData = GetPhysicalLayer( &cbData );
	if ( pData == NULL ) return NULL;

	switch ( GetMediaType() )
	{
		case kMediaType_802_3:
		{
			switch ( GetMediaSubType() )
			{
				case kMediaSubType_Native:
				{
					// TODO: Add support for VLAN tagging here?
					// Skip the Ethernet header.
					if ( cbData > 14 )
					{
						// Increment the data ptr.
						pData += 14;

						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = (UInt16)(cbData - 14);
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
					if ( cbData >= 2 )
					{
						// Get the 802.11 MAC header.
						const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

						switch ( pHdr->FrameControlType & 0x0C )
						{
							case 0x00:	// Management
							{
								if ( cbData > 24 )
								{
									pData += 24;

									if ( outBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*outBytesRemaining = (UInt16)(cbData - 24);
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
								UInt16	nMACLen = (UInt16)(((pHdr->FrameControlFlags & 0x03) == 0x03) ? 30 : 24);
								if( (pHdr->FrameControlType & 0xF0) == 0x80 )
								{
									nMACLen += 2;	// Frame has QoS, adjust.
								}
								if ( cbData > nMACLen )
								{
									pData += nMACLen;

									if ( outBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*outBytesRemaining = (UInt16)(cbData - nMACLen);
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
					ASSERT( 0 );
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
			if ( cbData >= 15 )
			{
				// Get the source routing info length.
				UInt16	nSourceRouteInfoLength = 0;
				if ( TestFlag( kPacketFlag_RouteInfo ) )
				{
					nSourceRouteInfoLength = (UInt16)(pData[14] & 0x1F);
				}

				// Calculate the total number of bytes to skip.
				const UInt16	nBytesToSkip = (UInt16)(14 + nSourceRouteInfoLength);

				if ( cbData > nBytesToSkip )
				{
					// Increment the data ptr.
					pData += nBytesToSkip;

					if ( outBytesRemaining != NULL )
					{
						// Update bytes remaining.
						*outBytesRemaining = (UInt16)(cbData - nBytesToSkip);
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
			TMediaSubType	mst;
			VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				GetMediaType(), GetWanProtocol(), &mst ) == ERROR_SUCCESS );

			switch ( mst )
			{
				case kMediaSubType_wan_ppp:			// WAN PPP
				{
					// Skip the WAN PPP header.
					if ( cbData > 2 )
					{
						// Increment the data ptr.
						pData += 2;

						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = (UInt16)(cbData - 2);
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
					if ( cbData > 2 )
					{
						// Increment the data ptr.
						pData += 2;

						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = (UInt16)(cbData - 2);
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
			// Don't know how to handle this media type.
			ASSERT( false );
			pData = NULL;
		}
		break;
	}

	return pData;
}


// -----------------------------------------------------------------------------
//		GetNetworkLayer
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetNetworkLayer(
	UInt16*	outBytesRemaining ) const
{
	// Get the link layer.
	const UInt8*	pData;
	UInt16			cbData;
	pData = GetLinkLayer( &cbData );
	if ( pData == NULL ) return NULL;

	switch ( GetMediaType() )
	{
		case kMediaType_802_3:
		{
			switch ( GetMediaSubType() )
			{
				case kMediaSubType_Native:
				{
					// Test for and skip 802.2 LLC header.
					// Otherwise, it's already there.
					if ( TestFlag( kPacketFlag_SNAP ) )
					{
						if ( cbData >= 2 )
						{
							if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
							{
								// Skip 802.2 LLC header with SNAP.
								if ( cbData > 8 )
								{
									// Increment the data ptr.
									pData += 8;

									if ( outBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*outBytesRemaining = (UInt16)(cbData - 8);
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
								if ( outBytesRemaining != NULL )
								{
									// Update bytes remaining.
									*outBytesRemaining = cbData;
								}
							}
							else
							{
								// Skip 802.2 LLC header.
								if ( cbData > 3 )
								{
									// Skip 3 bytes for unnumbered LSAP, otherwise 4 bytes.
									const UInt16	nSkip = ( (pData[2] & 0x03) == 0x03 ) ? 3 : 4;

									if ( cbData > nSkip )
									{
										pData += nSkip;

										if ( outBytesRemaining != NULL )
										{
											// Update bytes remaining.
											*outBytesRemaining = (UInt16)(cbData - nSkip);
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
						if ( outBytesRemaining != NULL )
						{
							// Update bytes remaining.
							*outBytesRemaining = cbData;
						}
					}
				}
				break;

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:
				{
					// Everything in 802.11 is SNAP/LSAP
					if ( cbData >= 2 )
					{
						if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
						{
							// Skip 802.2 LLC header with SNAP.
							if ( cbData > 8 )
							{
								// Increment the data ptr.
								pData += 8;

								if ( outBytesRemaining != NULL )
								{
									// Update bytes remaining.
									*outBytesRemaining = (UInt16)(cbData - 8);
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
							if ( outBytesRemaining != NULL )
							{
								// Update bytes remaining.
								*outBytesRemaining = cbData;
							}
						}
						else
						{
							// Skip 802.2 LLC header.
							if ( cbData > 3 )
							{
								// Skip 3 bytes for unnumbered LSAP, otherwise 4 bytes.
								const UInt16	nSkip = ( (pData[2] & 0x03) == 0x03 ) ? 3 : 4;

								if ( cbData > nSkip )
								{
									pData += nSkip;

									if ( outBytesRemaining != NULL )
									{
										// Update bytes remaining.
										*outBytesRemaining = (UInt16)(cbData - nSkip);
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

				default:
				{
					ASSERT( 0 );
					pData = NULL;
				}
				break;
			}
		}
		break;

		case kMediaType_802_5:
		{
			// TODO: Handle MAC Ctl packets differently?
			// Skip 802.2 LLC header.
			if ( cbData > 8 )
			{
				// Increment the data ptr.
				pData += 8;

				if ( outBytesRemaining != NULL )
				{
					// Update bytes remaining.
					*outBytesRemaining = (UInt16)(cbData - 8);
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
			TMediaSubType	mst;
			VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				GetMediaType(), GetWanProtocol(), &mst ) == ERROR_SUCCESS );

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
					if ( outBytesRemaining != NULL )
					{
						// Update bytes remaining.
						*outBytesRemaining = cbData;
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

	return pData;
}


// -----------------------------------------------------------------------------
//		GetIPHeaderStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetIPHeaderStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::IP, kHeader, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetIPDataStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetIPDataStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::IP, kData, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetIPv6HeaderStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetIPv6HeaderStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::IPv6, kHeader, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetIPv6DataStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetIPv6DataStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::IPv6, kData, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetTCPHeaderStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetTCPHeaderStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::TCP, kHeader, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetTCPDataStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetTCPDataStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::TCP, kData, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetUDPHeaderStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetUDPHeaderStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::UDP, kHeader, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetUDPDataStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetUDPDataStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::UDP, kData, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetICMPHeaderStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetICMPHeaderStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::ICMP, kHeader, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		GetICMPDataStart
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetICMPDataStart(
	UInt16*	outBytesRemaining ) const
{
	return this->GetPacketData( ProtoSpecDefs::ICMP, kData, outBytesRemaining );
}


// -----------------------------------------------------------------------------
//		CopyToBuffer
// -----------------------------------------------------------------------------

UInt16
CPacket::CopyToBuffer(
	UInt8*	inBuffer,
	UInt16	inSlicing ) const
{
	UInt16	length;

	ASSERT( m_pPeekPacket != NULL );
	ASSERT( m_pPacketData != NULL );

	// Copy the header.
	memmove( inBuffer, m_pPeekPacket, sizeof(PeekPacket) );

	// adjust for new slicing
	UInt16	nPacketBytes = GetActualLength();
	if ( inSlicing != 0 && nPacketBytes > inSlicing )
	{
		nPacketBytes = inSlicing;
		((PeekPacket*)inBuffer)->fSliceLength = inSlicing;
		((PeekPacket*)inBuffer)->fStatus |= kPacketStatus_Sliced;
	}

	// copy the info block
	UInt32 nInfoBlock = 0;
	const MediaSpecificPrivateHeader* pHdr = GetMediaSpecHeader();
	if (pHdr != NULL)
	{
		nInfoBlock = pHdr->nSize;
		memmove( inBuffer + nInfoBlock, pHdr, nInfoBlock );
	}
	// Copy the data.
	memmove( inBuffer + nInfoBlock + sizeof(PeekPacket), m_pPacketData, nPacketBytes );

	length = (UInt16)(sizeof(PeekPacket) + nInfoBlock + nPacketBytes);
	length = (UInt16)(length + (length % 2));

	return length;
}


// -----------------------------------------------------------------------------
//		GetProtocol
// -----------------------------------------------------------------------------

bool
CPacket::GetProtocol(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_Protocol;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetRawProtocol
// -----------------------------------------------------------------------------

bool
CPacket::GetRawProtocol(
	CMediaSpec& outSpec ) const
{
	// Check the cached protocol.
	if ( m_Protocol.IsValid() &&
		(m_Protocol.GetType() != kMediaSpecType_ProtoSpec) )
	{
		// Copy the cached protocol.
		outSpec = m_Protocol;
		return true;
	}

	// Get the raw protocol (non-ProtoSpec).
	return CalcRawProtocol( outSpec );
}


// -----------------------------------------------------------------------------
//		GetApplication
// -----------------------------------------------------------------------------

bool
CPacket::GetApplication(
	CMediaSpec& outSpec ) const
{
	static const char s_NullApplicationID[8] = {};
	if ( memcmp( m_pPeekPacket->fApplication, s_NullApplicationID, 8 ) != 0 )
	{
		outSpec.SetClass( kMediaSpecClass_Protocol );
		outSpec.SetType( kMediaSpecType_ApplicationID );
		outSpec.fMask = 0xFF000000;
		memcpy( outSpec.fData, m_pPeekPacket->fApplication, 8 );
		return true;
	}

	outSpec.Invalidate();

	return false;
}


// -----------------------------------------------------------------------------
//		GetSrcPhysical
// -----------------------------------------------------------------------------

bool
CPacket::GetSrcPhysical(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_SrcPhysical;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetDestPhysical
// -----------------------------------------------------------------------------

bool
CPacket::GetDestPhysical(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_DestPhysical;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetSrcLogical
// -----------------------------------------------------------------------------

bool
CPacket::GetSrcLogical(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_SrcLogical;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetDestLogical
// -----------------------------------------------------------------------------

bool
CPacket::GetDestLogical(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_DestLogical;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetSrcPort
// -----------------------------------------------------------------------------

bool
CPacket::GetSrcPort(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_SrcPort;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetDestPort
// -----------------------------------------------------------------------------

bool
CPacket::GetDestPort(
	CMediaSpec& outSpec ) const
{
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
		outSpec = m_DestPort;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetBSSID
// -----------------------------------------------------------------------------

bool
CPacket::GetBSSID(
	CMediaSpec& outSpec ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType ) );
	if ( !IS_WIRELESS( m_MediaSubType ) ) return false;

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
		outSpec = m_BSSID;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetReceiver
// -----------------------------------------------------------------------------

bool
CPacket::GetReceiver(
	CMediaSpec& outSpec ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType ) );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

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
		outSpec = m_Receiver;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetTransmitter
// -----------------------------------------------------------------------------

bool
CPacket::GetTransmitter(
	CMediaSpec& outSpec ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

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
		outSpec = m_Transmitter;
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetAddress1
// -----------------------------------------------------------------------------

bool
CPacket::GetAddress1(
	CMediaSpec& outSpec ) const
{
	// TODO: This should use ProtoSpecs to get the layer
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

	if ( !m_bAddress1 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress1 = true;

		// Reset address 1.
		m_pAddress1 = NULL;

		// Get the physical layer header.
		const UInt8*	pData;
		UInt16			cbData;
		pData = GetPhysicalLayer( &cbData );

		if ( (pData != NULL) && (cbData >= 2) )
		{
			// Get the 802.11 MAC header.
			const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x00:	// "To DS"=0, "From DS"=0.
						{
							if ( GetDestPhysical( outSpec ) )
							{
								m_pAddress1 = &m_DestPhysical;
							}
						}
						break;

						case 0x01:	// "To DS"=1, "From DS"=0.
						{
							if ( GetBSSID( outSpec ) )
							{
								m_pAddress1 = &m_BSSID;
							}
						}
						break;

						case 0x02:	// "To DS"=0, "From DS"=1.
						{
							if ( GetDestPhysical( outSpec ) )
							{
								m_pAddress1 = &m_DestPhysical;
							}
						}
						break;

						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							if ( GetReceiver( outSpec ) )
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
							if ( GetBSSID( outSpec ) )
							{
								m_pAddress1 = &m_BSSID;
							}
						}
						break;

						case 0x40:	// Beamforming Report Poll
						case 0x50:	// VHT NDP Announcement
						case 0x80:	// BlockAckReq
						case 0x90:	// BlockAck
						case 0xB0:	// RTS
						case 0xC0:	// CTS
						case 0xD0:	// Ack
						case 0xE0:	// CF-End
						case 0xF0:	// CF-End + CF-Ack
						{
							if ( GetReceiver( outSpec ) )
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
		outSpec = *m_pAddress1;
		outSpec.SetType( kMediaSpecType_WirelessAddr );
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetAddress2
// -----------------------------------------------------------------------------

bool
CPacket::GetAddress2(
	CMediaSpec& outSpec ) const
{
	// TODO: This should use ProtoSpecs to get the layer
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

	if ( !m_bAddress2 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress2 = true;

		// Reset address 2.
		m_pAddress2 = NULL;

		// Get the physical layer header.
		const UInt8*	pData;
		UInt16			cbData;
		pData = GetPhysicalLayer( &cbData );

		if ( (pData != NULL) && (cbData >= 2) )
		{
			// Get the 802.11 MAC header.
			const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x00:	// "To DS"=0, "From DS"=0.
						{
							if ( GetSrcPhysical( outSpec ) )
							{
								m_pAddress2 = &m_SrcPhysical;
							}
						}
						break;

						case 0x01:	// "To DS"=1, "From DS"=0.
						{
							if ( GetSrcPhysical( outSpec ) )
							{
								m_pAddress2 = &m_SrcPhysical;
							}
						}
						break;

						case 0x02:	// "To DS"=0, "From DS"=1.
						{
							if ( GetBSSID( outSpec ) )
							{
								m_pAddress2 = &m_BSSID;
							}
						}
						break;

						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							if ( GetTransmitter( outSpec ) )
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
						case 0x40:	// Beamforming Report Poll
						case 0x50:	// VHT NDP Announcement
						case 0x80:	// BlockAckReq
						case 0x90:	// BlockAck
						case 0xA0:	// PS-Poll
						case 0xB0:	// RTS
						{
							if ( GetTransmitter( outSpec ) )
							{
								m_pAddress2 = &m_Transmitter;
							}
						}
						break;

						case 0xE0:	// CF-End
						case 0xF0:	// CF-End + CF-Ack
						{
							if ( GetBSSID( outSpec ) )
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
		outSpec = *m_pAddress2;
		outSpec.SetType( kMediaSpecType_WirelessAddr );
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetAddress3
// -----------------------------------------------------------------------------

bool
CPacket::GetAddress3(
	CMediaSpec& outSpec ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

	if ( !m_bAddress3 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress3 = true;

		// Reset address 3.
		m_pAddress3 = NULL;

		// Get the physical layer header.
		const UInt8*	pData;
		UInt16			cbData;
		pData = GetPhysicalLayer( &cbData );

		if ( (pData != NULL) && (cbData >= 2) )
		{
			// Get the 802.11 MAC header.
			const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x00:	// "To DS"=0, "From DS"=0.
						{
							if ( GetBSSID( outSpec ) )
							{
								m_pAddress3 = &m_BSSID;
							}
						}
						break;

						case 0x01:	// "To DS"=1, "From DS"=0.
						{
							if ( GetDestPhysical( outSpec ) )
							{
								m_pAddress3 = &m_DestPhysical;
							}
						}
						break;

						case 0x02:	// "To DS"=0, "From DS"=1.
						{
							if ( GetSrcPhysical( outSpec ) )
							{
								m_pAddress3 = &m_SrcPhysical;
							}
						}
						break;

						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							if ( GetDestPhysical( outSpec ) )
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
		outSpec = *m_pAddress3;
		outSpec.SetType( kMediaSpecType_WirelessAddr );
	}

	return bValidSpec;
}


// -----------------------------------------------------------------------------
//		GetAddress4
// -----------------------------------------------------------------------------

bool
CPacket::GetAddress4(
	CMediaSpec& outSpec ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

	if ( !m_bAddress4 )
	{
		// Set the flag to tell that the spec has been checked.
		m_bAddress4 = true;

		// Reset address 4.
		m_pAddress4 = NULL;

		// Get the physical layer header.
		const UInt8*	pData;
		UInt16			cbData;
		pData = GetPhysicalLayer( &cbData );

		if ( (pData != NULL) && (cbData >= 2) )
		{
			// Get the 802.11 MAC header.
			const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

			switch ( pHdr->FrameControlType & 0x0C )
			{
				case 0x00:	// Management
				case 0x08:	// Data
				case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
				{
					// Get the address to use from the FrameControl fields "To DS" and "From DS".
					switch ( pHdr->FrameControlFlags & 0x03 )
					{
						case 0x03:	// "To DS"=1, "From DS"=1.
						{
							if ( GetSrcPhysical( outSpec ) )
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
		outSpec = *m_pAddress4;
		outSpec.SetType( kMediaSpecType_WirelessAddr );
	}

	return bValidSpec;
}


bool
CPacket::GetAddressN( const int inIndex, CMediaSpec& outSpec ) const
{
	switch ( inIndex )
	{
		case 1 :	return GetAddress1( outSpec );
		case 2 :	return GetAddress2( outSpec );
		case 3 :	return GetAddress3( outSpec );
		case 4 :	return GetAddress4( outSpec );
	}
	return false;
}

// -----------------------------------------------------------------------------
//		GetCapablities
// -----------------------------------------------------------------------------

bool
CPacket::GetCapablities(
	UInt8&	outCap ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

	outCap = 0;

	// Get the physical layer header.
	UInt16			nDataBytes = 0;
	const UInt8*	pData = GetPhysicalLayer( &nDataBytes );

	// Size of Management MAC header + 12 bytes of Frame data.
	if ( (pData != NULL) && (nDataBytes >= (24 + 12)) )
	{
		// Get the 802.11 MAC header.
		const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

		UInt8	nType = pHdr->FrameControlType & 0x0C;
		if ( nType == 0x00 )
		{
			// Management
			UInt8	nSubType = pHdr->FrameControlType & 0xF0;
			if ( (nSubType == 0x80) || (nSubType == 0x50) )
			{
				// Beacon or Probe Response
				// Three address Wireless MAC header: 24 bytes.
				const UInt8* pMan = pData + 24;
				outCap = pMan[10];
				return true;
			}
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetWirelessControlFlags
// -----------------------------------------------------------------------------

bool
CPacket::GetWirelessControlFlags( UInt8 & outFlags ) const
{
	// This address is only present in 802.11 frames.
	ASSERT( IS_WIRELESS( m_MediaSubType )  );
	if ( !IS_WIRELESS( m_MediaSubType )  ) return false;

	// Get the physical layer header.
	UInt16			nDataBytes = 0;
	const UInt8*	pData = GetPhysicalLayer( &nDataBytes );

	// flags is second byte of 802.11 header
	const bool bGotIt = (( NULL != pData ) && ( 1 < nDataBytes ));
	if ( bGotIt )
	{
		outFlags = pData[ 1 ];
	}
	return bGotIt;
}


// -----------------------------------------------------------------------------
//		GetESSID
// -----------------------------------------------------------------------------

const UInt8*
CPacket::GetESSID( UInt8& outESSIDLength ) const
{
	const UInt8 * pResult = NULL;
	outESSIDLength = 0;

	// This ESSID is only present in 802.11 frames.
	ASSERT( IS_WIRELESS(m_MediaSubType) );
	if ( IS_WIRELESS(m_MediaSubType) )
	{
		// Ignore anything that isn't a beacon, probe response, or probe request
		const PROTOSPEC_INSTID	pspecInstID		= GetProtoSpec();
		const PROTOSPEC_ID		pspecID			= GET_PSID( pspecInstID );
		const bool	bBeaconOrProbeRsp	=  (pspecID == ProtoSpecDefs::IEEE_802_11_Beacon)
										|| (pspecID == ProtoSpecDefs::IEEE_802_11_Probe_Rsp);
		const bool	bRequest			= ( ProtoSpecDefs::IEEE_802_11_Probe_Req == pspecID );
		if ( bBeaconOrProbeRsp || bRequest )
		{
			// Get the physical layer header.
			UInt16			nDataBytes = 0;
			const UInt8*	pData = GetPhysicalLayer( &nDataBytes );

			// Ignore sliced packets.
			if ( GetSliceLength() == 0 )
			{
				// MAC(24) + Fixed Fields(12 or 0) + FCS(4)
				UInt16	nFixedFieldByteCount = bBeaconOrProbeRsp ? 12 : 0;
				UInt16	nSkipHeaderByteCount = 24 + nFixedFieldByteCount;
				UInt16	nFCSByteCount		 = 4;

				// Ignore packets without enough data for an option list.
				if ( (pData != NULL) && (nDataBytes >= (nSkipHeaderByteCount + nFCSByteCount)) )
				{
					// Skip past the MAC header, fixed fields, and adjust for FCS bytes.
					UInt16			nElementBytes = (UInt16)(nDataBytes - (nSkipHeaderByteCount + nFCSByteCount));
					const UInt8*	pElementData = pData + nSkipHeaderByteCount;

					// Parse the element list.
					UInt16	nOffset = 0;
					for ( int nElem = 0; nElem < 3; )
					{
						// Need at least the element id and length, and assume the length isn't zero.
						if ( (nOffset + 2) >= nElementBytes ) break;

						// Get the ID and length.
						UInt8	nElementId     = pElementData[nOffset++];
						UInt8	nElementLength = pElementData[nOffset++];

						if ( (nOffset + nElementLength) <= nElementBytes )
						{
							// SSID
							if ( nElementId == 0 )
							{
								if ( nElementLength > 0 )
								{
									// This is the ESSID.
									pResult			= (pElementData + nOffset);
									outESSIDLength	= nElementLength;
									break;
								}

								++nElem;
							}
							// DS Parameter Set
							else if ( nElementId == 3 )
							{
								++nElem;
							}
						}

						// Increment the offset.
						nOffset = (UInt16)(nOffset + nElementLength);
					}
				}
			}
		}
	}

	return pResult;
}


// -----------------------------------------------------------------------------
//		GetVLANIDs
// -----------------------------------------------------------------------------

bool
CPacket::GetVLANIDs( TArrayEx<UInt16>& outArray ) const
{
	if ( !m_bVLANID )
	{
		// Get the VLANID.
		CalcVLANIDs( m_ayVLANID );

		// Set the flag to tell that the array has been checked.
		m_bVLANID = true;
	}

	outArray = m_ayVLANID;

	return true;
}


// -----------------------------------------------------------------------------
//		GetMPLSLabels
// -----------------------------------------------------------------------------

bool
CPacket::GetMPLSLabels( TArrayEx<UInt32>& outArray ) const
{
	if ( !m_bMPLSLabel )
	{
		// Get the MPLSLabel.
		CalcMPLSLabels( m_ayMPLSLabel );

		// Set the flag to tell that the array has been checked.
		m_bMPLSLabel = true;
	}

	outArray = m_ayMPLSLabel;

	return true;
}


// =============================================================================
//	Private Interface
// =============================================================================

// -----------------------------------------------------------------------------
//		CalcProtocol
// -----------------------------------------------------------------------------

bool
CPacket::CalcProtocol(
	CMediaSpec& outSpec ) const
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPeekPacket != NULL );
	if ( m_pPeekPacket == NULL ) return false;

	const UInt32	ps = m_pPeekPacket->fProtoSpec;
	if ( PreferRawProtocol( ps ) && CalcRawProtocol( outSpec ) )
	{
		return true;
	}

	// ProtoSpec defined protocol.
	outSpec.SetClass( kMediaSpecClass_Protocol );
	outSpec.SetType( kMediaSpecType_ProtoSpec );
	static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ProtoSpec );
	outSpec.fMask = nMask;
	*(UInt32*) &outSpec.fData[0] = ps;

	return true;
}


// -----------------------------------------------------------------------------
//		CalcRawProtocol
// -----------------------------------------------------------------------------

bool
CPacket::CalcRawProtocol(
	CMediaSpec& outSpec ) const
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPeekPacket != NULL );
	if ( m_pPeekPacket == NULL ) return false;

	switch ( GetMediaType() )
	{
		case kMediaType_802_3:
		{
			switch ( GetMediaSubType() )
			{
				case kMediaSubType_Native:
				{
					if ( TestFlag( kPacketFlag_SNAP ) )
					{
						// Get the start of the link layer.
						const UInt8*	pData;
						UInt16			cbData;
						pData = GetLinkLayer( &cbData );

						if ( (pData != NULL) && (cbData >= 2) )
						{
							if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
							{
								if ( cbData >= 8 )
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
						const UInt8*	pData;
						UInt16			cbData;
						pData = GetPhysicalLayer( &cbData );

						if ( (pData != NULL) && (cbData >= 14) )
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
					if ( !TestStatus( kPacketStatus_Encrypted ) )
					{
						// Get the start of the physical layer.
						const UInt8*	pData;
						UInt16			cbData;
						pData = GetPhysicalLayer( &cbData );

						if ( (pData != NULL) && (cbData >= 2) )
						{
							const UInt8	nFrameControlType = pData[0];
							const UInt8	nType = nFrameControlType & 0x0C;
							if ( nType == 0x08 )
							{
								const UInt8	nSubType = nFrameControlType & 0xF0;
								switch ( nSubType )
								{
									case 0x00:	// Data
									case 0x10:	// Data + CF-Ack
									case 0x20:	// Data + CF-Poll
									case 0x30:	// Data + CF-Ack + CF-Poll
									{
										// Get the MAC header length.
										const UInt8	nFrameControlFlags = pData[1];
										UInt16		nMACLen = static_cast<UInt16>(((nFrameControlFlags & 0x03) == 0x03) ? 30 : 24);
										if( (nFrameControlFlags & 0xF0) == 0x80 )
										{
											nMACLen += 2;	// Frame has QoS, adjust.
										}
										if ( cbData > nMACLen )
										{
											pData  += nMACLen;
											cbData  = static_cast<UInt16>(cbData - nMACLen);

											if ( cbData >= 2 )
											{
												if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
												{
													if ( cbData >= 8 )
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
									}
									break;
								}
							}
						}
					}

					if ( !outSpec.IsValid() )
					{
						// Just say that it's the ProtoSpec (or 802.11 data if it's an LLC ProtoSpec).
						outSpec.SetClass( kMediaSpecClass_Protocol );
						outSpec.SetType( kMediaSpecType_ProtoSpec );
						outSpec.fMask = outSpec.GetTypeMask();

						const PROTOSPEC_INSTID	ps = GetProtoSpec();
						if ( IS_LLC_PSID( ps ) )
						{
							// TODO: work with native protospecs
							PROTOSPEC_INSTID	nParent = 0;
							ProtoSpecs::GetParentID( ps, nParent );
							if ( nParent != 0 )
							{
								*(PROTOSPEC_INSTID*) &outSpec.fData[0] = nParent;
							}
							else
							{
								*(PROTOSPEC_INSTID*) &outSpec.fData[0] = ps;
							}
						}
						else
						{
							*(PROTOSPEC_INSTID*) &outSpec.fData[0] = ps;
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
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetLinkLayer( &cbData );

			if ( (pData != NULL) && (cbData >= 2) )
			{
				if ( (pData[0] == 0xAA) && (pData[1] == 0xAA) )
				{
					if ( cbData >= 8 )
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
			TMediaSubType	mst;
			VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				GetMediaType(), GetWanProtocol(), &mst ) == ERROR_SUCCESS );

			switch ( mst )
			{
				case kMediaSubType_wan_ppp:			// WAN PPP
				{
					// protocol is the 3/4 bytes
					const UInt8*	pData;
					UInt16			cbData;
					pData = GetPhysicalLayer( &cbData );
					if ( (pData != NULL) && (cbData >= 4) )
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
					UInt16			cbData;
					pData = GetPhysicalLayer( &cbData );
					if ( (pData != NULL) && (cbData >= 4) )
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
					*((PROTOSPEC_INSTID*) outSpec.fData) = m_pPeekPacket->fProtoSpec;
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
CPacket::CalcPhysical() const
{
	// Invalidate the physical addresses.
	m_SrcPhysical.Invalidate();
	m_DestPhysical.Invalidate();

	// Sanity check.
	ASSERT( IsValid() );
	if ( !IsValid() ) return;

	// Get the physical layer.
	const UInt8*	pData;
	UInt16			cbData;
	pData = GetPhysicalLayer( &cbData );

	switch ( GetMediaType() )
	{
		case kMediaType_802_3:
		{
			switch ( GetMediaSubType() )
			{
				case kMediaSubType_Native:
				{
					if ( pData != NULL )
					{
						static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_EthernetAddr );

						if ( cbData >= 12 )
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

						if ( cbData >= 6 )
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

				case kMediaSubType_802_11_b:
				case kMediaSubType_802_11_a:
				case kMediaSubType_802_11_gen:
				{
					if ( (pData != NULL) && (cbData >= 2) )
					{
						// Get the 802.11 MAC header.
						const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

						const UInt8*	pSrcAddr  = NULL;
						const UInt8*	pDestAddr = NULL;
						switch ( pHdr->FrameControlType & 0x0C )
						{
							case 0x00:	// Management, Addr1 = DA, Addr2 = SA, Addr3 = BSSID
							{
								if ( cbData >= 22 )
								{
									pSrcAddr = &pHdr->Addr2[0];
								}
								if ( cbData >= 10 )
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
										if ( cbData >= 16 )
										{
											pSrcAddr = &pHdr->Addr2[0];
										}
										if ( cbData >= 10 )
										{
											pDestAddr = &pHdr->Addr1[0];
										}
									}
									break;

									case 0x01:	// "To DS"=1, "From DS"=0.
									{
										if ( cbData >= 16 )
										{
											pSrcAddr = &pHdr->Addr2[0];
										}
										if ( cbData >= 22 )
										{
											pDestAddr = &pHdr->Addr3[0];
										}
									}
									break;

									case 0x02:	// "To DS"=0, "From DS"=1.
									{
										if ( cbData >= 22 )
										{
											pSrcAddr = &pHdr->Addr3[0];
										}
										if ( cbData >= 10 )
										{
											pDestAddr = &pHdr->Addr1[0];
										}
									}
									break;

									case 0x03:	// "To DS"=1, "From DS"=1.
									{
										if ( cbData >= 30 )
										{
											pSrcAddr = &pHdr->Addr4[0];
										}
										if ( cbData >= 22 )
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
									case 0x40:	// Beamforming Report Poll
									case 0x50:	// VHT NDP Announcement
									case 0x80:	// BlockAckReq
									case 0x90:	// BlockAck
									case 0xA0:	// PS-Poll
									case 0xB0:	// RTS
									case 0xE0:	// CF-End
									case 0xF0:	// CF-End + CF-Ack
									{
										if ( cbData >= 16 )
										{
											pSrcAddr = &pHdr->Addr2[0];
										}
										if ( cbData >= 10 )
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
										// immediately preceeding RTS frame.

										// Get the packet buffer.
										const CPacketBuffer*	pPacketBuffer = GetPacketBuffer();
										//ASSERT( pPacketBuffer != NULL );

										// Get the packet's receiver.
										CMediaSpec				specReceiver;
										if ( (pPacketBuffer != NULL) && GetAddress1( specReceiver ) )
										{
											// Get this packet's buffer index.
											const UInt32	nIndex = GetPacketBufferIndex();

											for ( UInt32 nOffset = 1; nOffset <= 5; nOffset++ )
											{
												// Sanity check the buffer index and offset.
												if ( nOffset > nIndex ) break;

												// Get the next previous packet.
												CPacket	thePrevPacket = pPacketBuffer->GetPacket( nIndex - nOffset );
												if ( !thePrevPacket.IsValid() ) break;

												// Don't trust it if it's a CRC error packet.
												if ( thePrevPacket.TestFlag( kPacketFlag_CRC ) ) break;

												// Get the previous packet's transmitter.
												CMediaSpec	specPrevTransmitter;
												if ( thePrevPacket.GetAddress2( specPrevTransmitter ) )
												{
													// Compare this packet's receiver with
													// the previous packet's transmitter.
													if ( specReceiver == specPrevTransmitter )
													{
														// Get the receiver from the previous packet, 
														// make sure it's not broadcast/multicast (TT23771).
														CMediaSpec prevReceiver;
														if( thePrevPacket.GetAddress1( prevReceiver ) && 
															!prevReceiver.IsBroadcastOrMulticast() )
														{
															// The receiver from the previous packet is the
															// implied transmitter of this packet.
															m_SrcPhysical = prevReceiver;

															// Force the media spec type to be Ethernet.
															m_SrcPhysical.SetType( kMediaSpecType_EthernetAddr );

															// Note that we've gotten this from another packet.
															m_bTransmitterImplied = true;

															// Stop looking.
															break;
														}
													}
												}
											}
										}

										if ( cbData >= 10 )
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

				if ( cbData >= 14 )
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

				if ( cbData >= 8 )
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
			TMediaSubType	mst;
			VERIFY( UMediaSpecUtils::GetSubTypeFromProtocol(
				GetMediaType(), GetWanProtocol(), &mst ) == ERROR_SUCCESS );

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
					if ( (pData != NULL) && (cbData >= 2) )
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
CPacket::CalcLogical() const
{
	// Invalidate the logical addresses.
	m_SrcLogical.Invalidate();
	m_DestLogical.Invalidate();

	// Sanity check.
	ASSERT( IsValid() );
	if ( !IsValid() ) return;

	// Check for a type we know how to format.
	static const PROTOSPEC_ID s_ParentArray[] =
	{
		ProtoSpecDefs::IP,
		ProtoSpecDefs::IPv6,
		ProtoSpecDefs::AppleTalk_Ph2,
		ProtoSpecDefs::AARP_Response,
		ProtoSpecDefs::AARP,
		ProtoSpecDefs::Long_DDP,
		ProtoSpecDefs::Short_DDP,
		ProtoSpecDefs::DECnet,
		ProtoSpecDefs::IPX
	};
	HRESULT				hr;
	PROTOSPEC_INSTID	matchID;
	SInt32				lParentIndex;
	BOOL				bResult;
	hr = IsDescendentOf( s_ParentArray, COUNTOF(s_ParentArray),
		&matchID, &lParentIndex, &bResult );
	if ( FAILED(hr) || !bResult ) return;
	const UInt32		psid = s_ParentArray[lParentIndex];

	switch ( psid )
	{
		// IP
		case ProtoSpecDefs::IP:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetPacketData( GET_PSID( matchID ), kHeader, &cbData );
			if ( (pData != NULL) && (cbData >= 20) ) 
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_IPAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt32*) &m_SrcLogical.fData[0] = *(UInt32*) &pData[12];

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_IPAddr );
				m_DestLogical.fMask = nMask;
				*(UInt32*) &m_DestLogical.fData[0] = *(UInt32*) &pData[16];
			}
		}
		break;

		// EType2-IPv6
		// 802.3-IPv6
		case ProtoSpecDefs::IPv6:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetPacketData( GET_PSID( matchID ), kHeader, &cbData );
			if ( (pData != NULL) && (cbData >= 40) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPv6Addr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_IPv6Addr );
				m_SrcLogical.fMask = nMask;
				*(UInt32*) &m_SrcLogical.fData[0]  = *(UInt32*) &pData[8];
				*(UInt32*) &m_SrcLogical.fData[4]  = *(UInt32*) &pData[12];
				*(UInt32*) &m_SrcLogical.fData[8]  = *(UInt32*) &pData[16];
				*(UInt32*) &m_SrcLogical.fData[12] = *(UInt32*) &pData[20];

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
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 10) )
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

		// AARP Response
		case ProtoSpecDefs::AARP_Response:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 28) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_AppleTalkAddr );

				m_DestLogical.SetClass( kMediaSpecClass_Address );
				m_DestLogical.SetType( kMediaSpecType_AppleTalkAddr );
				m_DestLogical.fMask = nMask;
				*(UInt16*) &m_DestLogical.fData[0] = *(UInt16*) &pData[25];
				m_DestLogical.fData[2] = pData[27];
			}
		}
		break;

		// AARP
		case ProtoSpecDefs::AARP:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetPacketData( GET_PSID( matchID ), kHeader, &cbData );
			if ( (pData != NULL) && (cbData >= 18) )
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
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 13) )
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
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 2) )
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

		// EType2-DECnet
		case ProtoSpecDefs::DECnet:
		{
			if ( GetMediaType() == kMediaType_802_3 )
			{
				const UInt8*	pData;
				UInt16			cbData;
				pData = GetPhysicalLayer( &cbData );
				if ( (pData != NULL) && (cbData >= 14) )
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
			else if ( GetMediaType() == kMediaType_802_5 )
			{
				const UInt8*	pData;
				UInt16			cbData;
				pData = GetPhysicalLayer( &cbData );
				if ( (pData != NULL) && (cbData >= 14) )
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
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 28) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPXAddr );

				m_SrcLogical.SetClass( kMediaSpecClass_Address );
				m_SrcLogical.SetType( kMediaSpecType_IPXAddr );
				m_SrcLogical.fMask = nMask;
				*(UInt32*) &m_SrcLogical.fData[0] = *(UInt32*) &pData[18];
				*(UInt32*) &m_SrcLogical.fData[4] = *(UInt32*) &pData[22];
				*(UInt16*) &m_SrcLogical.fData[8] = *(UInt16*) &pData[26];

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
CPacket::CalcPorts() const
{
	// Invalidate the ports.
	m_SrcPort.Invalidate();
	m_DestPort.Invalidate();

	// Sanity check.
	ASSERT( IsValid() );
	if ( !IsValid() ) return;

	// Check for a type we know how to find the source port for.
	static const PROTOSPEC_ID s_ParentArray[] =
	{
		ProtoSpecDefs::TCP,
		ProtoSpecDefs::UDP,
		ProtoSpecDefs::AppleTalk_Ph2,
		ProtoSpecDefs::Long_DDP,
		ProtoSpecDefs::Short_DDP,
		ProtoSpecDefs::IPX,
		ProtoSpecDefs::XNS
	};
	HRESULT				hr;
	PROTOSPEC_INSTID	matchID;
	SInt32				lParentIndex;
	BOOL				bResult;
	hr = IsDescendentOf( s_ParentArray, COUNTOF(s_ParentArray),
		&matchID, &lParentIndex, &bResult );
	if ( FAILED(hr) || !bResult ) return;
	const UInt32		psid = s_ParentArray[lParentIndex];

	switch ( psid )
	{
		case ProtoSpecDefs::TCP:
		case ProtoSpecDefs::UDP:
		{	
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetPacketData( GET_PSID( matchID ), kHeader, &cbData );
			if ( (pData != NULL) && (cbData >= 20) ) 
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_IPPort );

				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_IPPort );
				m_SrcPort.fMask = nMask;
				*(UInt16*) &m_SrcPort.fData[0] = *(UInt16*) &pData[0];

				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_IPPort );
				m_DestPort.fMask = nMask;
				*(UInt16*) &m_DestPort.fData[0] = *(UInt16*) &pData[2];
			}
		}
		break;

		case ProtoSpecDefs::AppleTalk_Ph2:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 12) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ATPort );

				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_ATPort );
				m_SrcPort.fMask    = nMask;
				m_SrcPort.fData[0] = pData[11];

				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_ATPort );
				m_DestPort.fMask    = nMask;
				m_DestPort.fData[0] = pData[10];
			}
		}
		break;

		case ProtoSpecDefs::Long_DDP:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 15) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ATPort );

				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_ATPort );
				m_SrcPort.fMask    = nMask;
				m_SrcPort.fData[0] = pData[14];

				m_DestPort.SetClass( kMediaSpecClass_Port );
				m_DestPort.SetType( kMediaSpecType_ATPort );
				m_DestPort.fMask    = nMask;
				m_DestPort.fData[0] = pData[5];
			}
		}
		break;

		case ProtoSpecDefs::Short_DDP:
		{
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 7) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_ATPort );

				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_ATPort );
				m_SrcPort.fMask    = nMask;
				m_SrcPort.fData[0] = pData[6];

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
			const UInt8*	pData;
			UInt16			cbData;
			pData = GetNetworkLayer( &cbData );
			if ( (pData != NULL) && (cbData >= 30) )
			{
				static const UInt32	nMask = CMediaSpec::GetTypeMask( kMediaSpecType_NWPort );

				m_SrcPort.SetClass( kMediaSpecClass_Port );
				m_SrcPort.SetType( kMediaSpecType_NWPort );
				m_SrcPort.fMask = nMask;
				*(UInt16*) m_SrcPort.fData = *(UInt16*) &pData[28];

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
CPacket::CalcBSSID(
	CMediaSpec& outSpec ) const
{
	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPeekPacket != NULL );
	if ( m_pPeekPacket == NULL ) return false;

	const UInt8*	pData;
	UInt16			cbData;
	pData = GetPhysicalLayer( &cbData );

	switch ( GetMediaSubType() )
	{
		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_gen:
		{
			if ( (pData != NULL) && (cbData >= 2) )
			{
				// Get the 802.11 MAC header.
				const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

				const UInt8*	pAddr = NULL;
				switch ( pHdr->FrameControlType & 0x0C )
				{
					case 0x00:	// Management, Addr1 = DA, Addr2 = SA, Addr3 = BSSID
					{
						if ( cbData >= 22 )
						{
							pAddr = &pHdr->Addr3[0];
						}
					}
					break;

					case 0x08:	// Data
					case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
					{
						// Get the address to use from the FrameControl fields "To DS" and "From DS".
						switch ( pHdr->FrameControlFlags & 0x03 )
						{
							case 0x00:	// "To DS"=0, "From DS"=0.
							{
								if ( cbData >= 22 )
								{
									pAddr = &pHdr->Addr3[0];
								}
							}
							break;

							case 0x01:	// "To DS"=1, "From DS"=0.
							{
								if ( cbData >= 10 )
								{
									pAddr = &pHdr->Addr1[0];
								}
							}
							break;

							case 0x02:	// "To DS"=0, "From DS"=1.
							{
								if ( cbData >= 16 )
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
								if ( cbData >= 10 )
								{
									pAddr = &pHdr->Addr1[0];
								}
							}
							break;

							case 0xE0:	// CF-End
							case 0xF0:	// CF-End + CF-Ack
							{
								if ( cbData >= 16 )
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
CPacket::CalcReceiver(
	CMediaSpec& outSpec ) const
{
	// TODO: This should use ProtoSpecs to get the layer

	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPeekPacket != NULL );
	if ( m_pPeekPacket == NULL ) return false;

	// Get the physical layer.
	const UInt8*	pData;
	UInt16			cbData;
	pData = GetPhysicalLayer( &cbData );

	switch ( GetMediaSubType() )
	{
		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_gen:
		{
			if ( (pData != NULL) && (cbData >= 2) )
			{
				// Get the 802.11 MAC header.
				const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

				const UInt8*	pAddr = NULL;
				switch ( pHdr->FrameControlType & 0x0C )
				{
					case 0x00:	// Management
					case 0x08:	// Data
					case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
					{
						// The receiver is always Address 1.
						if ( cbData >= 10 )
						{
							pAddr = &pHdr->Addr1[0];
						}
					}
					break;

					case 0x04:	// Control
					{
						switch ( pHdr->FrameControlType & 0xF0 )
						{
							case 0x40:	// Beamforming Report Poll
							case 0x50:	// VHT NDP Announcement
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
								if ( cbData >= 10 )
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
CPacket::CalcTransmitter(
	CMediaSpec& outSpec ) const
{
	// TODO: This should use ProtoSpecs to get the layer

	// Invalidate the spec.
	outSpec.Invalidate();

	// Sanity check.
	ASSERT( m_pPeekPacket != NULL );
	if ( m_pPeekPacket == NULL ) return false;

	const UInt8*	pData;
	UInt16			cbData;
	pData = GetPhysicalLayer( &cbData );

	switch ( GetMediaSubType() )
	{
		case kMediaSubType_802_11_b:
		case kMediaSubType_802_11_a:
		case kMediaSubType_802_11_gen:
		{
			if ( (pData != NULL) && (cbData >= 2) )
			{
				// Get the 802.11 MAC header.
				const Wireless80211MacHeader*	pHdr = (Wireless80211MacHeader*) pData;

				const UInt8*	pAddr = NULL;
				switch ( pHdr->FrameControlType & 0x0C )
				{
					case 0x00:	// Management
					case 0x08:	// Data
					case 0x0C:	// Siemens proprietary (see TT12938) (after AP3)
					{
						// The transmitter is always Address 2.
						if ( cbData >= 16 )
						{
							pAddr = &pHdr->Addr2[0];
						}
					}
					break;

					case 0x04:	// Control
					{
						switch ( pHdr->FrameControlType & 0xF0 )
						{
							case 0x40:	// Beamforming Report Poll
							case 0x50:	// VHT NDP Announcement
							case 0x80:	// BlockAckReq
							case 0x90:	// BlockAck
							case 0xA0:	// PS-Poll
							case 0xB0:	// RTS
							case 0xE0:	// CF-End
							case 0xF0:	// CF-End + CF-Ack
							{
								if ( cbData >= 16 )
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
								// immediately preceeding RTS frame.

								// Get the packet buffer.
								const CPacketBuffer*	pPacketBuffer = GetPacketBuffer();

								// Get the packet's receiver.
								CMediaSpec				specReceiver;
								if ( (pPacketBuffer != NULL) && GetAddress1( specReceiver ) )
								{
									// Get this packet's buffer index.
									const UInt32	nIndex = GetPacketBufferIndex();

									for ( UInt32 nOffset = 1; nOffset <= 5; nOffset++ )
									{
										// Sanity check the buffer index and offset.
										if ( nOffset > nIndex ) break;

										// Get the next previous packet.
										CPacket	thePrevPacket = pPacketBuffer->GetPacket( nIndex - nOffset );
										//ASSERT( thePrevPacket.IsValid() );
										if ( !thePrevPacket.IsValid() ) break;

										// Don't trust it if it's a CRC error packet.
										if ( thePrevPacket.TestFlag( kPacketFlag_CRC ) ) break;

										// Get the previous packet's transmitter.
										CMediaSpec	specPrevTransmitter;
										if ( thePrevPacket.GetAddress2( specPrevTransmitter ) )
										{
											// Compare this packet's receiver with
											// the previous packet's transmitter.
											if ( specReceiver == specPrevTransmitter )
											{
												// Get the receiver from the previous packet, 
												// make sure it's not broadcast/multicast (TT23771).
												CMediaSpec prevReceiver;
												if( thePrevPacket.GetAddress1( prevReceiver ) && 
													!prevReceiver.IsBroadcastOrMulticast() )
												{
													// The receiver from the previous packet is the
													// implied transmitter of this packet.
													m_SrcPhysical = prevReceiver;

													// Force the media spec type to be Ethernet.
													m_SrcPhysical.SetType( kMediaSpecType_EthernetAddr );

													// Note that we've gotten this from another packet.
													m_bTransmitterImplied = true;

													// Stop looking.
													break;
												}
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
//		GetProtospecArray
// -----------------------------------------------------------------------------

const ProtoSpecs::SProtospecInfo *
CPacket::GetProtospecArray(
	int &	outInfoCount ) const
{
	if ( !m_bProtospecArray )
	{
		if ( !this->CalcProtospecArray() ) return NULL;
	}
	outInfoCount = m_nProtospecCount;
	return m_ProtospecArray;
}


// -----------------------------------------------------------------------------
//		CalcFrameCheckSequence
// -----------------------------------------------------------------------------

bool
CPacket::CalcFrameCheckSequence() const
{
	static const UInt16	FCS_BYTE_COUNT	= 4;

	const UInt16	nPacketByteCount	= this->GetActualLength();
	if( nPacketByteCount < FCS_BYTE_COUNT ) return false;
	if( nPacketByteCount < this->GetPacketLength() ) return false;

	UInt32 nFCS = BIGTOHOST32( *(UInt32*) (m_pPacketData + (nPacketByteCount-FCS_BYTE_COUNT)) );
	if( nFCS == 0 )
	{
		// Calculate it ourselves.
		if( m_MediaType == kMediaType_Wan  || m_MediaType == kMediaType_CoWan )
		{
			const UInt16 FCS_BYTE_COUNT_WAN = 2;
			nFCS = static_cast<UInt32>( CRC16( m_pPacketData, nPacketByteCount-FCS_BYTE_COUNT_WAN ) );
			nFCS = BIGTOHOST16( static_cast<UInt16>(nFCS) );
		}
		else
		{
			nFCS = CRC32( m_pPacketData, nPacketByteCount-FCS_BYTE_COUNT );
			nFCS = BIGTOHOST32( nFCS );
		}
	}

	m_FrameCheckSequence = nFCS;
	return true;
}


// -----------------------------------------------------------------------------
//		CalcProtospecArray
// -----------------------------------------------------------------------------

bool
CPacket::CalcProtospecArray() const
{
	this->GetProtoSpec();
	return true;
}


// -----------------------------------------------------------------------------
//		CalcVLANIDs
// -----------------------------------------------------------------------------

bool
CPacket::CalcVLANIDs(
	TArrayEx<UInt16>& outArray ) const
{
	// Invalidate the array.
	outArray.RemoveAllItems();

	int									nLayerInfo;
	const ProtoSpecs::SProtospecInfo*	pLayerInfo = GetProtospecArray( nLayerInfo );
	if ( (pLayerInfo != NULL) && (nLayerInfo > 0) )
	{
		// Iterate backwards to go deeper into the packet and get the 
		// VLAN IDs in the correct order.
		for ( int i = nLayerInfo - 1; i >= 0; --i )
		{
			const ProtoSpecs::SProtospecInfo&	info = pLayerInfo[i];

			UInt32	nProtoSpec = GET_PSID( info.nInstID );
			if ( nProtoSpec == ProtoSpecDefs::VLAN )
			{
				// Extract all VLAN IDs in the stack.
				const UInt8*	pVlanID    = info.pHeader + VLAN_ID_OFFSET;
				const UInt8*	pVlanIDEnd = info.pPayload;
				while ( pVlanID < pVlanIDEnd )
				{
					const UInt16	nVlanID = MemUtil::GetShort( pVlanID, __BIG_ENDIAN ) & VLAN_ID_MASK;
					outArray.AddItem( nVlanID );
					pVlanID += VLAN_ID_LAYER_SIZE;
				}
			}
		}
	}

	return !outArray.IsEmpty();
}


// -----------------------------------------------------------------------------
//		CalcMPLSLabels
// -----------------------------------------------------------------------------

bool
CPacket::CalcMPLSLabels(
	TArrayEx<UInt32>& outArray ) const
{
	// Invalidate the array.
	outArray.RemoveAllItems();

	int									nLayerInfo;
	const ProtoSpecs::SProtospecInfo*	pLayerInfo = GetProtospecArray( nLayerInfo );
	if ( (pLayerInfo != NULL) && (nLayerInfo > 0) )
	{
		// Iterate backwards to go deeper into the packet and get the 
		// MPLS Labels in the reverse order.
		for ( int i = nLayerInfo - 1; i >= 0; --i )
		{
			const ProtoSpecs::SProtospecInfo&	info = pLayerInfo[i];

			UInt32	nProtoSpec = GET_PSID( info.nInstID );
			if ( nProtoSpec == ProtoSpecDefs::MPLS )
			{
				// Extract all VLAN IDs in the stack.
				const UInt8*	pVlanID    = info.pHeader + MPLS_LABEL_OFFSET;
				const UInt8*	pVlanIDEnd = info.pPayload;
				while ( pVlanID < pVlanIDEnd )
				{
					const UInt32	nMplsLabel = MemUtil::GetLong( pVlanID, __BIG_ENDIAN ) >> MPLS_LABEL_SHIFT;
					outArray.AddItem( nMplsLabel );
					pVlanID += MPLS_LABEL_LAYER_SIZE;
				}
			}
		}
	}

	return !outArray.IsEmpty();
}


// -----------------------------------------------------------------------------
//		GetPacketData
// -----------------------------------------------------------------------------
//	@return NULL if anything goes wrong

const UInt8 *
CPacket::GetPacketData(
	const PROTOSPEC_ID	inProtocol,
	const HeaderOrData	inHeaderOrData,
	UInt16 *			outBytesRemaining ) const
{
	// get the array
	int		nProtospecCount	= 0;
	const ProtoSpecs::SProtospecInfo *	ayProtospecInfo = this->GetProtospecArray( nProtospecCount );
	if (( NULL == ayProtospecInfo ) || ( nProtospecCount <= 0 )) return NULL;
	
	// find the desired protospec
	int			nIndex	= -1;
	HRESULT hr = ProtoSpecs::GetIndexFromInfo(
		const_cast< ProtoSpecs::SProtospecInfo * >( ayProtospecInfo ),
		nProtospecCount, inProtocol, nIndex );
	if ( hr != S_OK ) return NULL;
	if (( nIndex < 0 ) || ( nProtospecCount <= nIndex )) return NULL;
	
	// use its pointers
	const ProtoSpecs::SProtospecInfo &	info	= ayProtospecInfo[ nIndex ];
	const UInt8 *	pResult	= NULL;
	if ( kHeader == inHeaderOrData )
	{
		pResult = info.pHeader;
	}
	else
	{
		pResult = info.pPayload;
	}
	if (( NULL != outBytesRemaining ) && ( NULL != m_pPacketData ) && ( NULL != pResult ))
	{
		const UInt16	nTotalByteCount	= this->GetActualLength();
		const UInt16	nConsumedByteCount	= (UInt16) ( pResult - m_pPacketData );
		*outBytesRemaining = nTotalByteCount - nConsumedByteCount;
	}
	return pResult;
}


// -----------------------------------------------------------------------------
//		GetPacketDataEnd()
// -----------------------------------------------------------------------------
///	Return a pointer to the byte just past the end of valid payload data.
/// For unsliced packets, this is the first byte of the FCS. For sliced packets,
/// this is the next byte after the slice.
const UInt8*
CPacket::GetPacketDataEnd() const
{
	static const UInt16	FCS_BYTE_COUNT	= 4;
	
	ASSERT( m_pPeekPacket != NULL );
	ASSERT( FCS_BYTE_COUNT <= m_pPeekPacket->fPacketLength );

	// Not slicing? Return actual end, minus those 4 FCS bytes.
	const UInt8		*pBegin				= this->GetPacketData();
	const UInt16	nUnslicedByteCount	= m_pPeekPacket->fPacketLength - FCS_BYTE_COUNT;
	const UInt16	nSlicedByteCount	= m_pPeekPacket->fSliceLength;
	UInt8*			pEnd				= NULL;
	if ( 0 == nSlicedByteCount )
	{
		pEnd = const_cast<UInt8*>( pBegin + nUnslicedByteCount );
	}
	else
	{
		// Slicing? Return the byte after the slice, unless the slice actually lands after the FCS starts.
		const UInt16		nByteCount			= ( (nUnslicedByteCount < nSlicedByteCount) ? nUnslicedByteCount : nSlicedByteCount );
		pEnd = const_cast<UInt8*>( pBegin + nByteCount );
	}

	// If small IPv4 packet, honor its total length value (dodge extra
	// padding bytes for some less-than-64-byte-long packets).
	if ( nUnslicedByteCount <= 70 )
	{
		// Get IPv4 header.
		UInt16			cbHeader	= 0;
		const UInt8*	pHeader		= GetIPHeaderStart( &cbHeader );
		if ( pHeader != NULL && cbHeader >= 4 )
		{
			// Find IP data end.
			const UInt8*	pIPDataEnd	= pHeader + MemUtil::GetShort( pHeader + 2, __BIG_ENDIAN );

			// If IP data ends before our slice/packet-length guess, use IP data end.
			// TODO: this could be incorrect for IPv4 tunneled over some other 
			// protocol that includes data after the tunneled payload
			if ( pIPDataEnd < pEnd )
			{
				ASSERT( m_pPacketData < pIPDataEnd );
				pEnd = const_cast<UInt8*>( pIPDataEnd );
			}
		}
	}

	return pEnd;
}

// -----------------------------------------------------------------------------
//		IsDescendentOf
// -----------------------------------------------------------------------------

HRESULT 
CPacket::IsDescendentOf(
	PROTOSPEC_INSTID		/*inInstanceID*/, 
	PROTOSPEC_INSTID*		outMatchParentInstID, 
	const PROTOSPEC_INSTID*	inParentIDs, 
	SInt32					inParentCount,
	SInt32*					outMatchParentIndex ) const
{	
	int nCount = 0;
	const ProtoSpecs::SProtospecInfo* pArray = GetProtospecArray( nCount );
	if ( m_bProtospecArray )	
	{
		// use the protocol array if we have it
		// now walk the array in reverse and check against each parent ID
		// the protospec array is already in reverse
		for (int iProtocol = 0; iProtocol < nCount; iProtocol++)
		{
			const UInt16	nPSID = GET_PSID(pArray[iProtocol].nInstID);
			for (int iParent = 0; iParent < inParentCount; iParent++)
			{
				if (GET_PSID(inParentIDs[iParent]) == nPSID) 
				{
					if (outMatchParentInstID) 
						*outMatchParentInstID = pArray[iProtocol].nInstID;
					if (outMatchParentIndex)
						*outMatchParentIndex = iParent;
					return S_OK;
				}
			}
		}
	}

	if (outMatchParentInstID) 
		*outMatchParentInstID = 0;
	if (outMatchParentIndex)
		*outMatchParentIndex = -1;
	return S_FALSE;
}


// -----------------------------------------------------------------------------
//		IsDescendentOf
// -----------------------------------------------------------------------------

HRESULT 
CPacket::IsDescendentOf(
	const PROTOSPEC_ID			pParentArray[],
	SInt32						nParents,
	PROTOSPEC_INSTID*			pMatchInstanceID,
	SInt32*						pMatchIndex,
	BOOL*						pbResult ) const
{
	// Sanity checks.
	ASSERT( pParentArray != NULL );
	if ( pParentArray == NULL ) return E_POINTER;
	ASSERT( nParents > 0 );
	if ( nParents <= 0 ) return E_POINTER;
	// pMatchInstanceID may be NULL.
	// pMatchIndex may be NULL.
	ASSERT( pbResult != NULL );
	if ( pbResult == NULL ) return E_POINTER;

	int									nProtoSpecCount;
	const ProtoSpecs::SProtospecInfo*	pProtoSpecInfoArray = GetProtospecArray( nProtoSpecCount );
	if ( pProtoSpecInfoArray == NULL ) return E_FAIL;

	// Walk the array and check against each parent ID.
	for ( int i = 0; i < nProtoSpecCount; ++i )
	{
		const UInt16	nPSID = GET_PSID( pProtoSpecInfoArray[i].nInstID );
		for ( int j = 0; j < nParents; ++j )
		{
			if ( pParentArray[j] == nPSID )
			{
				// Got a match.
				if ( pMatchInstanceID != NULL )
				{
					*pMatchInstanceID = pProtoSpecInfoArray[i].nInstID;
				}
				if ( pMatchIndex != NULL )
				{
					*pMatchIndex = j;
				}
				*pbResult = TRUE;

				return S_OK;
			}
		}
	}

	// No match found.
	if ( pMatchInstanceID != NULL )
	{
		*pMatchInstanceID = 0;
	}
	if ( pMatchIndex != NULL )
	{
		*pMatchIndex = -1;
	}
	*pbResult = FALSE;

	return S_OK;
}


	// Predicate for find_if() in FindProtospecInfo() below.
	static bool
	MatchesProtspecID(
		const ProtoSpecs::SProtospecInfo &	inInfo,
		const PROTOSPEC_ID					inPSID )
	{
		return inPSID == GET_PSID( inInfo.nInstID );
	}

	class MatchesProtspecIDX
	{
		PROTOSPEC_ID	m_nPSID;
	public :
		explicit MatchesProtspecIDX( const PROTOSPEC_ID inPSID ) :	m_nPSID( inPSID ) { }
		MatchesProtspecIDX( const MatchesProtspecIDX & inOriginal ) :	m_nPSID( inOriginal.m_nPSID ) { }
		bool
		operator()( const ProtoSpecs::SProtospecInfo & inInfo ) const
		{
			return ( m_nPSID == GET_PSID( inInfo.nInstID ) );
		}
	};
	
// -----------------------------------------------------------------------------
//		FindProtospecInfo()
//
///	If any of the protospecs in this packet's protospec array match the given id, return the info for that protospec.
/// If not, return NULL.
// -----------------------------------------------------------------------------
const ProtoSpecs::SProtospecInfo *
CPacket::FindProtospecInfo( const PROTOSPEC_ID inProtospecID ) const
{
	// Get the protospec array so we have something to search.
	using namespace ProtoSpecs;
	int						nProtospecCount	= 0;
	const SProtospecInfo *	pProtospecArray	= this->GetProtospecArray( nProtospecCount );
	if (( NULL == pProtospecArray ) || ( nProtospecCount <= 0 )) return NULL;

	// Search it.
	const SProtospecInfo *	end			= pProtospecArray + nProtospecCount;
	const SProtospecInfo *	findIter	= std::find_if( pProtospecArray, end, MatchesProtspecIDX( inProtospecID ) );
	if ( findIter == end ) return NULL;
	return findIter;
}


// -----------------------------------------------------------------------------
//		GetTransmitterEncryption()
//
/// If this packet contains some clue to a wireless encryption in use by
/// the transmitter, output it.
///
/// Also calculates WEP key index as a side effect, if WEPpish.
///
/// Leaves parameters unchanged if we don't have anything better to say.
///
///	Currently detects only:
///		None
///		WEP
///		TKIP
///
///	(If you're looking for CKIP detection, @see GetReceiverEncryption())
///
/// @param ioEncryption: if you know the encryption type, pass that in. Because
///			a lot of encryption types just look like WEP at the single-packet
///			level, we'll just set this to WEP unless you have something better
///			that you picked out of some earlier packet.
// -----------------------------------------------------------------------------

bool
CPacket::GetTransmitterEncryptionType(
	EncryptionType & 	 ioEncryptionType ) const
{
	// If we already calculated this once, don't do it again.
	if ( !m_bTransmitterEncryptionType )
	{
		// Okay, so we have to calculate it, just this once.
		m_bTransmitterEncryptionType	= true;
		// Assume no encryption unless we calculate otherwise.
		m_nTransmitterEncryptionType	= kEncryptionType_None;
	
		// WEP, even if decrypted WEP
		if ( this->TestStatus( kPacketStatus_Encrypted ) )
		{
			m_nTransmitterEncryptionType	= kEncryptionType_WEP;
		}
	
		// Can we dig deeper into WEP data for other encryption data?
		const PROTOSPEC_INSTID	pspecInstID	= this->GetProtoSpec();
		const PROTOSPEC_ID		pspecID		= GET_PSID( pspecInstID );

		if (( ProtoSpecDefs::IEEE_802_11_WEP_Data == pspecID )
			|| ( ProtoSpecDefs::IEEE_802_11_Encrypted_Data == pspecID ))
		{
			// Get the MAC header.
			UInt16			nWEPBytes = 0;
			const UInt8*	pWEPHdr = this->GetLinkLayer( &nWEPBytes );
			if ( (pWEPHdr != NULL) && (nWEPBytes >= 4) )
			{
				// is TKIP?
				//Note: with the advent of WPA2 this could actually be tkip or ccmp 
				//but we dont have any way of knowing which it is without the eapol keys
				const UInt8	kTKIPFlag	= 0x20;
				if ( kTKIPFlag & pWEPHdr[ 3 ] )
				{
					m_nTransmitterEncryptionType	= kEncryptionType_TKIP;
				}
				else	// not TKIP
				{
					m_nTransmitterEncryptionType	= kEncryptionType_WEP;
				}

				// Either way, the key index is in the uppermost 2 bits of offset [3], and displayed as "1-4" not "0-3"
				m_nWEPKeyIndex 			= (UInt8)(((pWEPHdr[3] & 0xC0) >> 6) + 1);
				m_bWEPKeyIndexChecked	= true;
				m_bWEPKeyIndexValid		= true;
			
			}
		}
	}

	// Don't clobber better encryption with WEP guesses
	if ( (int) ioEncryptionType < (int) m_nTransmitterEncryptionType )
	{
		ioEncryptionType = m_nTransmitterEncryptionType;
	}
	return m_bTransmitterEncryptionType;
}


// -----------------------------------------------------------------------------
//		GetTransmitterAuthenticationType()
//
///	If this packet contains some authentication protocol info, output that.
///
/// If this packet contains no authentication protocol info (which is most 
/// packets), leave the output argument untouched.
///
/// Currently detects:
///	-- None
///	-- LEAP
///	-- PEAP
/// -- EAPTLS
/// Strangely, does not detect EAP, nothing does.
// -----------------------------------------------------------------------------

bool
CPacket::GetTransmitterAuthenticationType(
	AuthenticationType &	ioAuthenticationType ) const
{
	// If already calculated this once, don't do it again.
	if ( !m_bTransmitterAuthenticationType )
	{
		// Do it just this once.
		m_bTransmitterAuthenticationType = true;
		// Assuming the weakest.
		m_nTransmitterAuthenticationType = kAuthenticationType_None;

		switch ( this->GetProtoSpecID() )
		{
			case ProtoSpecDefs::EAP_Response:
			{
				// get the EAP response data
				const UInt8 *		pEAPData      = NULL;
				UInt16				nEAPByteCount = 0;
				pEAPData = GetPacketData( ProtoSpecDefs::EAP_Response, kData, &nEAPByteCount );
				if ( NULL == pEAPData ) break;
				const size_t	kOffset_Code	= 0;
				const UInt8		kCode_Response	= 2;
				const size_t	kOffset_Type	= 4;
				const UInt8		kType_EAPTLS	= 13;
				const UInt8		kType_LEAP		= 17;
				const UInt8		kType_PEAP		= 25;
				if ( nEAPByteCount <= kOffset_Type ) break;
				if ( pEAPData[ kOffset_Code ] != kCode_Response ) break;
				switch ( pEAPData[ kOffset_Type ] )
				{
					case kType_EAPTLS:
						m_nTransmitterAuthenticationType = kAuthenticationType_EAPTLS;
						break;
						
					case kType_LEAP:
						m_nTransmitterAuthenticationType = kAuthenticationType_LEAP;
						break;
						
					case kType_PEAP:
						m_nTransmitterAuthenticationType = kAuthenticationType_PEAP;
						break;
				}
			}
			break;
		}
	}

	// Don't clobber better authentication with None
	if ( (int) ioAuthenticationType < (int) m_nTransmitterAuthenticationType )
	{
		ioAuthenticationType = m_nTransmitterAuthenticationType;
	}
	return m_bTransmitterAuthenticationType;
}


// -----------------------------------------------------------------------------
//		GetReceiverEncryptionType
//
/// If this packet contains some indication of CKIP, output that. Otherwise,
/// leave the parameter untouched.
// -----------------------------------------------------------------------------

bool
CPacket::GetReceiverEncryptionType( EncryptionType & ioEncryptionType ) const
{
	// Calculate only once.
	if ( !m_bReceiverEncryptionType )
	{
		// Okay, we'll do it just this once.
		m_bReceiverEncryptionType = true;
		// Assuming the wimpiest encryption possible.
		m_nReceiverEncryptionType = kEncryptionType_None;
		
		// association responses are how we detect CKIP flags
		switch ( this->GetProtoSpecID() )
		{
			case ProtoSpecDefs::IEEE_802_11_Assoc_Rsp:
			{
				const UInt8 *	pResponseData      = NULL;
				UInt16			nRepsonseByteCount = 0;
				pResponseData = GetPacketData( ProtoSpecDefs::IEEE_802_11_Assoc_Rsp, kData, &nRepsonseByteCount );
				if ( NULL == pResponseData ) break;
				const size_t	kOffset_Code		= 0;
				const size_t	kOffset_Length		= 1;
				const UInt8		kCode_CKIP			= 0x85;
					
				if ( kCode_CKIP == pResponseData[ kOffset_Code ] )
				{
					const UInt8		kMinLength			= 27;
					const size_t	kOffset_CKIPFlags	= 10;
					const UInt8		kCKIPFlagKeyPermute	= 0x10;
					const UInt8		kCKIPFlagMIC		= 0x08;
					if ( nRepsonseByteCount <= kOffset_CKIPFlags ) break;
					if ( pResponseData[ kOffset_Code ] != kCode_CKIP ) break;
					if ( pResponseData[ kOffset_Length ] < kMinLength ) break;
					const UInt8	nCKIPFlags = pResponseData[ kOffset_CKIPFlags ];
					if ( ( kCKIPFlagKeyPermute | kCKIPFlagMIC ) & nCKIPFlags )
					{
						m_nReceiverEncryptionType = kEncryptionType_CKIP;
					}
				}
			}
			break;
		}
	}

	// Don't clobber better encryption with WEP guesses
	if ( (int) ioEncryptionType < (int) m_nReceiverEncryptionType )
	{
		ioEncryptionType = m_nReceiverEncryptionType;
	}
	return m_bReceiverEncryptionType;

}

// -----------------------------------------------------------------------------
//		GetWEPKeyIndex
//
///	If this packet looks WEPpish and we can determine which WEP key it uses,
/// output that.
///
/// If we couldn't calculate a WEP key index, leave the output parameter 
/// untouched, and return false.
// -----------------------------------------------------------------------------

bool
CPacket::GetWEPKeyIndex( UInt8 & outWEPKeyIndex ) const
{
	// Only calculate this once.
	if ( !m_bWEPKeyIndexChecked )
	{
		// And never do this again. Ever.
		m_bWEPKeyIndexChecked	= true;
		
		// Let GetTransmitterEncryptionType() do all the work. It already 
		// understands TKIP and WEP and where the key index is in those kind of packets.
		EncryptionType nDontCare	= kEncryptionType_None;
		this->GetTransmitterEncryptionType( nDontCare );
	}

	// If we have a valid key, output it.
	if ( m_bWEPKeyIndexValid )
	{
		outWEPKeyIndex = m_nWEPKeyIndex;
	}

	return m_bWEPKeyIndexValid;
}

// ### move somewhere with TCP flags and such
static const UInt8	TCP_OPTION_END				= 0x00;
static const UInt8	TCP_OPTION_NOOP				= 0x01;
static const UInt8	TCP_OPTION_MSS				= 0x02;
static const UInt8	TCP_OPTION_WINDOW_SCALE		= 0x03;
static const UInt8	TCP_OPTION_SELECTIVE_ACK	= 0x05;
static const UInt8	TCP_OPTION_TIMESTAMP		= 0x08;

//---------------------------------------------------------------------
//		FindTCPOption()
//---------------------------------------------------------------------
///	If this packet is a TCP packet, and has TCP options (usually only SYN packets might have options),
/// and those options contain the given option ID, return true and output a pointer to the option's data.
/// Otherwise return false and leave output parameter untouched.
bool
CPacket::FindTCPOption(
	const UInt8					inTCPOptionID,
	const UInt8 *&				outOptionData,
	UInt16 &					outOptionByteCount ) const
{
	static const bool NOT_FOUND = false;
	
	// Get TCP Option pointers, if any
	if ( !m_bTCPOption )
	{
		UInt16	nBytesRemaining	= 0;
		m_pTCPOptionBegin	= this->GetTCPHeaderStart( &nBytesRemaining );
		if ( NULL != m_pTCPOptionBegin ) 
		{
			m_pTCPOptionBegin += 20;	// skip fixed-sized portion of TCP header
		}
		m_pTCPOptionEnd		= this->GetTCPDataStart(   &nBytesRemaining );
		m_bTCPOption = true;
	}
	// Not TCP or no options?
	if ( !m_pTCPOptionBegin || ( m_pTCPOptionEnd <= m_pTCPOptionBegin ) ) return NOT_FOUND;

	// Scan the option list for the requested option ID.
	UInt8	nOptionByteCount	= 1;
	for ( const UInt8 * pOption = m_pTCPOptionBegin; pOption < m_pTCPOptionEnd; pOption += nOptionByteCount )
	{
		const UInt8	nOptionID	= *pOption;
		if ( TCP_OPTION_NOOP == nOptionID ) 
		{
			nOptionByteCount	= 1;
			continue;
		}
		else if ( TCP_OPTION_END == nOptionID )
		{
			return NOT_FOUND;
		}

		// How big is it (if size byte is valid).
		if ( m_pTCPOptionEnd <= ( pOption + 1 ) ) return NOT_FOUND;
		nOptionByteCount	= ( (2 > *( pOption + 1 )) ? 2 : *( pOption + 1 ) );	//TT13816 AiroPeek freezes when opening file. Malformed packets can have zero-length options, which would cause an infinite loop. Always skip AT LEAST the type and length bytes and move forward.
		// Do we have all the option data?
		if ( m_pTCPOptionEnd < pOption + nOptionByteCount ) return NOT_FOUND;
		
		// If this is our option, we're done.
		if ( inTCPOptionID == nOptionID )
		{
			static const UInt8	ID_AND_SIZE_BYTES	= 2;
			outOptionData		= pOption          + ID_AND_SIZE_BYTES;
			outOptionByteCount	= nOptionByteCount - ID_AND_SIZE_BYTES;
			static const bool	FOUND	= true;
			return FOUND;
		}
	}
	return NOT_FOUND;
}

//---------------------------------------------------------------------
//		GetTCPWindowScale()
//---------------------------------------------------------------------
/// If this packet has a TCP Window Scale option, return true and output the scale value (even if that value is 0). 
/// Otherwise return false and leave output parameter untouched.
bool		
CPacket::GetTCPWindowScale( UInt8 & outScale ) const
{
	// First time requested? Fetch it now.
	if ( !m_bTCPOptionWindowScaleCached )
	{
		m_bTCPOptionWindowScaleCached	= true;

		const UInt8 *	pOptionBegin		= NULL;
		UInt16			nOptionByteCount	= 0;
		m_bTCPOptionWindowScaleExists
			= this->FindTCPOption( TCP_OPTION_WINDOW_SCALE, pOptionBegin, nOptionByteCount );
		if ( m_bTCPOptionWindowScaleExists )
		{
			ASSERT( sizeof( m_nTCPOptionWindowScale ) == nOptionByteCount );
			m_nTCPOptionWindowScale = *pOptionBegin;
		}
	}

	// Only touch output if we have something to say.
	if ( m_bTCPOptionWindowScaleExists )
	{
		outScale = m_nTCPOptionWindowScale;
	}
	return m_bTCPOptionWindowScaleExists;
}

//---------------------------------------------------------------------
//		GetTCPSack()
//---------------------------------------------------------------------
/// If this is a TCP packet with a selective ACK (SACK) option, return true and output the begin and end of the first SACK range.
bool
CPacket::GetTCPSACK( UInt32 & outSACKBegin, UInt32 & outSACKEnd ) const
{
	if ( !m_bTCPOptionSACK )
	{
		m_bTCPOptionSACK	= true;

		const UInt8 *	pData		= NULL;
		UInt16			nByteCount	= 0;
		const bool	bGotIt	= this->FindTCPOption( TCP_OPTION_SELECTIVE_ACK, pData, nByteCount );
		if ( bGotIt && ( 2*sizeof( UInt32 )  <= nByteCount ) )
		{
			const UInt32 * const pSACK	= reinterpret_cast< const UInt32 *>( pData );
			m_nTCPOptionSACKBegin	= NETWORKTOHOST32( *pSACK );
			m_nTCPOptionSACKEnd		= NETWORKTOHOST32( *( pSACK + 1 ) );
		}
		else 
		{
			m_nTCPOptionSACKBegin	= 0;
			m_nTCPOptionSACKEnd		= 0;
		}
	}

	// If no SACK, we're done.
	if (( 0 == m_nTCPOptionSACKBegin ) && ( 0 == m_nTCPOptionSACKEnd )) return false;

	outSACKBegin	= m_nTCPOptionSACKBegin;
	outSACKEnd		= m_nTCPOptionSACKEnd;
	return (( 0 != m_nTCPOptionSACKBegin ) || ( 0 != m_nTCPOptionSACKEnd ));
};


//---------------------------------------------------------------------
//		GetTCPMSS()
//---------------------------------------------------------------------
/// If this packet has a TCP MSS option, return true and output the MSS value. 
/// Otherwise return false and leave output parameter untouched.
bool		
CPacket::GetTCPMSS( UInt16 & outMSS ) const
{
	// First time requested? Fetch it now.
	if ( !m_bTCPOptionMSS )
	{
		m_bTCPOptionMSS	= true;

		m_nTCPOptionMSS						= 0;
		const UInt8 *	pOptionBegin		= NULL;
		UInt16			nOptionByteCount	= 0;
		const bool		bGotIt
			= this->FindTCPOption( TCP_OPTION_MSS, pOptionBegin, nOptionByteCount );
		if ( !bGotIt ) return false;

		ASSERT( sizeof( m_nTCPOptionMSS ) == nOptionByteCount );
		m_nTCPOptionMSS = NETWORKTOHOST16( *reinterpret_cast< const UInt16 * >( pOptionBegin ) );
	}

	// Only touch output if we have something to say.
	if ( m_bTCPOptionMSS  && ( 0 < m_nTCPOptionMSS ) )
	{
		outMSS = m_nTCPOptionMSS;
		return true;
	}
	return false;
}


//---------------------------------------------------------------------
//		GetFrameCheckSequence
//---------------------------------------------------------------------

bool
CPacket::GetFrameCheckSequence( UInt32& outFCS ) const
{
	bool bValidFCS = true;

	if( m_FrameCheckSequence == 0 ) 
	{
		bValidFCS = CalcFrameCheckSequence();
	}
	
	outFCS = m_FrameCheckSequence;
	return bValidFCS;
}


// -----------------------------------------------------------------------------
//		App()
// -----------------------------------------------------------------------------
/// Return an object that knows how to parse application-layer stuff.
///
/// This class is empty for non-Peek projects.

const AppLayer &
CPacket::App() const
{
	if ( NULL == m_pAppLayer.get() )
	{
		m_pAppLayer.reset( new AppLayer( this ) );
	}
	return *m_pAppLayer;
}


// -----------------------------------------------------------------------------
//		FlowID
// -----------------------------------------------------------------------------
/// Externally set FlowTracker flow ID.
/// This property is here so that external objects can cache it once calculated
/// for this packet. Value is forgotten as soon as this packet is done processing.
void
CPacket::SetFlowID( const UInt32 inFlowID ) const
{
	m_bFlowID	= true;
	m_FlowID	= inFlowID;
}

bool
CPacket::GetFlowID( UInt32 & outFlowID ) const
{
	if ( !m_bFlowID ) return false;
	outFlowID = m_FlowID;
	return true;
}


// -----------------------------------------------------------------------------
//		VoIPCallID
// -----------------------------------------------------------------------------
/// Externally controlled CallManager call ID.
/// This property is here so that external objects can cache it once calculated
/// for this packet. Value is forgotten as soon as this packet is done processing.
void
CPacket::SetVoIPCallID( const UInt64 inCallID ) const
{
	m_bVoIPCallID	= true;
	m_VoIPCallID	= inCallID;
}

bool
CPacket::GetVoIPCallID( UInt64 & outCallID ) const
{
	if ( !m_bVoIPCallID ) return false;
	outCallID = m_VoIPCallID;
	return true;
}

const WirelessHeaderFacade*	
CPacket::GetWirelessHeaderFacade() const
{
	return m_pWirelessHeaderFacade.get();
}

void 
CPacket::ConstructWirelessHeaderFacade(const MediaSpecificHeaderAll*  pHeader)
{
	WirelessHeaderFacade* pNewFacade = 0;

	if (IS_WIRELESS(m_MediaSubType) && pHeader)
	{	
		//gcc doesn't allow taking the address of a member in a packed struct, forcing us to perform this little hack.
		//Treat the header as a byte array and offset to the location of the member, then cast that pointer to the appropriate data type
		const UInt8* pSubHeader = reinterpret_cast<const UInt8*>(pHeader)+sizeof(MediaSpecificPrivateHeader);

		//construct the appropriate facade using placement new to avoid memory allocations
		switch (pHeader->StdHeader.nType)
		{
		case kMediaSpecificHdrType_Wireless3:
		{
			pNewFacade = new Wireless80211PrivateHeaderFacade(*reinterpret_cast<const Wireless80211PrivateHeader*>(pSubHeader));
		} break;
		case kMediaSpecificHdrType_TunneledWireless:
		{
			pNewFacade = new TunneledWirelessHeaderFacade(*reinterpret_cast<const TunneledWirelessHeader*>(pSubHeader));
		} break;
		default: break;
		}
	}

	m_pWirelessHeaderFacade.reset(pNewFacade);
}

// -----------------------------------------------------------------------------
//		VoIPCallFlowIndex
// -----------------------------------------------------------------------------
/// Externally controlled CallManager index into a call.
/// This property is here so that external objects can cache it once calculated
/// for this packet. Value is forgotten as soon as this packet is done processing.
void
CPacket::SetVoIPCallFlowIndex( const UInt32 inCallFlowIndex ) const
{
	m_bVoIPCallFlowIndex	= true;
	m_VoIPCallFlowIndex		= inCallFlowIndex;
}
bool
CPacket::GetVoIPCallFlowIndex( UInt32 & outCallFlowIndex ) const
{
	if ( !m_bVoIPCallFlowIndex ) return false;
	outCallFlowIndex = m_VoIPCallFlowIndex;
	return true;
}
