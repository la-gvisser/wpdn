// =============================================================================
//	Packet.h
// =============================================================================
//	Copyright (c) 1996-2014 WildPackets, Inc. All rights reserved.

#ifndef PACKET_H
#define PACKET_H

#include "ArrayEx.h"
#include "AuthenticationType.h"
#include "EncryptionType.h"
#include "PacketHeaders.h"
#include "PeekPacket.h"
#include "MCSTable.h"
#include "MediaSpec.h"
#include "Protospecs.h"
#include "WirelessHeaderFacade.h"
#include <memory>	// for std::auto_ptr
#include <vector>

// Packets should always begin on a memory boundary that is evenly
// divisible by this value in order to make efficient use of CPU cycles.
#define PEEK_PACKET_ALIGNMENT	(8)

class AppLayer;
class CPacketBuffer;

class DECLSPEC CPacket
{
public:
	CPacket();
	CPacket( const CPacket& inOriginal );
	CPacket( TMediaType inMediaType, TMediaSubType inSubType,
		PeekPacket* inPacket, UInt8* inData = NULL, bool inCopy = false,
		UInt32 nPacketIdx = 0, const CPacketBuffer* pBuffer = NULL );
	~CPacket();

	CPacket&	operator=( const CPacket& inOriginal );

	void		SetPacket( TMediaType inMediaType, TMediaSubType inSubType,
					PeekPacket* inPacket, UInt8* inData = NULL, bool inCopy = false,
					UInt32 nPacketIdx = 0, const CPacketBuffer* pBuffer = NULL);

	bool		IsValid() const;

	// Packet buffer, data, headers.
	const CPacketBuffer*				GetPacketBuffer() const;
	UInt32								GetPacketBufferIndex() const;
	UInt64								GetPacketNumber() const;
	const UInt8*						GetPacketData() const;
	const UInt8*						GetPacketDataEnd() const;	// ends right before FCS
	const PeekPacket*					GetPacketHeader() const;
	const UInt8*						GetMediaSpecInfoBlock() const;
	const MediaSpecificPrivateHeader*	GetMediaSpecHeader() const;
	const Wireless80211PrivateHeader*	GetWirelessHeader() const;
	const FullDuplexPrivateHeader*		GetFullDuplexHeader() const;
	const WideAreaNetworkHeader*		GetWanHeader() const;
	WideAreaNetworkHeader*				GetWanHeader();

	// Common header information.
	TMediaType				GetMediaType() const;
	TMediaSubType			GetMediaSubType() const;
	PROTOSPEC_INSTID		GetProtoSpec() const;
	PROTOSPEC_ID			GetProtoSpecID() const;
	UInt16					GetPacketLength() const;
	UInt16					GetSliceLength() const;
	UInt16					GetActualLength() const;
	UInt32					GetFlags() const;
	UInt32					GetStatus() const;
	UInt64					GetTimeStamp() const;

	// Different types of info blocks.
	bool					IsWirelessPacket() const;
	bool					IsFullDuplexPacket() const;
	bool					IsWanPacket() const;

	// Wireless header information.
	SInt16					GetChannel() const;
	bool					GetChannel( SWirelessChannel& outChannelInfo ) const;
	UInt16					GetDataRate() const;
	double					GetCalculatedDataRate() const;
	UInt16					GetMCSIndex() const;
	UInt32					GetSpatialStreams() const;
	UInt8					GetSignalLevel() const;
	SInt8					GetSignaldBm() const;
	UInt8					GetNoiseLevel() const;
	SInt8					GetNoisedBm() const;
	SInt8					GetSignaldBmN(UInt8 nStream) const;
	SInt8					GetNoisedBmN(UInt8 nStream) const;
	UInt32					GetFlagsN() const;

	// Full-duplex header information.
	UInt32					GetFullDuplexChannel() const;

	// WAN header information.
	UInt8					GetWanDirection() const;
	UInt8					GetWanProtocol() const;

	bool					IsError() const;
	bool					IsMulticast() const;
	bool					IsBroadcast() const;

	// Set/clear flags and status bits.
	void					SetFlag( UInt32 inFlag );
	void					ClearFlag( UInt32 inFlag );
	bool					TestFlag( UInt32 inFlag ) const;
	void					SetStatus( UInt32 inStatus );
	void					ClearStatus( UInt32 inStatus );
	bool					TestStatus( UInt32 inStatus ) const;

	// Packet layers.
	const UInt8*			GetPhysicalLayer( UInt16* outBytesRemaining ) const;
	const UInt8*			GetLinkLayer( UInt16* outBytesRemaining ) const;
	const UInt8*			GetNetworkLayer( UInt16* outBytesRemaining ) const;
	const UInt8*			GetIPHeaderStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetIPDataStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetIPv6HeaderStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetIPv6DataStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetTCPHeaderStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetTCPDataStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetUDPHeaderStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetUDPDataStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetICMPHeaderStart( UInt16* outBytesRemaining ) const;
	const UInt8*			GetICMPDataStart( UInt16* outBytesRemaining ) const;

	// Packet "specs" (protocol, addresses, ports).
	bool					GetProtocol( CMediaSpec& outSpec ) const;
	bool					GetRawProtocol( CMediaSpec& outSpec ) const;
	bool					GetApplication( CMediaSpec& outSpec ) const;
	bool					GetSrcPhysical( CMediaSpec& outSpec ) const;
	bool					GetDestPhysical( CMediaSpec& outSpec ) const;
	bool					GetSrcLogical( CMediaSpec& outSpec ) const;
	bool					GetDestLogical( CMediaSpec& outSpec ) const;
	bool					GetSrcPort( CMediaSpec& outSpec ) const;
	bool					GetDestPort( CMediaSpec& outSpec ) const;

	// wireless stuff in packet data
	bool					GetBSSID( CMediaSpec& outSpec ) const;
	bool					GetReceiver( CMediaSpec& outSpec ) const;
	bool					GetTransmitter( CMediaSpec& outSpec ) const;
	bool					GetAddress1( CMediaSpec& outSpec ) const;
	bool					GetAddress2( CMediaSpec& outSpec ) const;
	bool					GetAddress3( CMediaSpec& outSpec ) const;
	bool					GetAddress4( CMediaSpec& outSpec ) const;
	bool					GetAddressN( const int inIndex, CMediaSpec& outSpec ) const;
	bool					GetCapablities( UInt8& outCap ) const;
	bool					GetWirelessControlFlags( UInt8& outCap ) const;
	bool					IsTransmitterImplied() const { return m_bTransmitterImplied; }
	const UInt8*			GetESSID( UInt8& outESSIDLength ) const;

	// VLAN and MPLS stuff in packet data
	bool					GetVLANIDs( TArrayEx<UInt16>& outArray ) const;
	bool					GetMPLSLabels( TArrayEx<UInt32>& outArray ) const;
	
	// Cached entity table indices.
	UInt32					GetProtocolIndex() const { return m_ProtocolIndex; }
	void					SetProtocolIndex( UInt32 index ) const { m_ProtocolIndex = index; }
	UInt32					GetSrcPhysicalIndex() const { return m_SrcPhysicalIndex; }
	void					SetSrcPhysicalIndex( UInt32 index ) const { m_SrcPhysicalIndex = index; }
	UInt32					GetDestPhysicalIndex() const { return m_DestPhysicalIndex; }
	void					SetDestPhysicalIndex( UInt32 index ) const { m_DestPhysicalIndex = index; }
	UInt32					GetSrcLogicalIndex() const { return m_SrcLogicalIndex; }
	void					SetSrcLogicalIndex( UInt32 index ) const { m_SrcLogicalIndex = index; }
	UInt32					GetDestLogicalIndex() const { return m_DestLogicalIndex; }
	void					SetDestLogicalIndex( UInt32 index ) const { m_DestLogicalIndex = index; }

	UInt16					CopyToBuffer( UInt8* inBuffer, UInt16 inSlice = 0 ) const;

	// protospec array
	const ProtoSpecs::SProtospecInfo * GetProtospecArray( int &	outInfoCount ) const;
	HRESULT					IsDescendentOf( PROTOSPEC_INSTID inInstanceID, 
								PROTOSPEC_INSTID* outMatchParentInstID, 
								const PROTOSPEC_INSTID* inParentIDs, 
								SInt32 inParentCount,
								SInt32* outMatchParentIndex ) const;
	HRESULT					IsDescendentOf( const PROTOSPEC_ID pParentArray[],
								SInt32 nParents, PROTOSPEC_INSTID* pMatchInstanceID,
								SInt32* pMatchIndex, BOOL* pbResult) const;
	const ProtoSpecs::SProtospecInfo *	FindProtospecInfo( const PROTOSPEC_ID inProtospecID ) const;

	bool					GetTransmitterEncryptionType( EncryptionType & ioEncryptionType ) const;
	bool					GetTransmitterAuthenticationType( AuthenticationType & ioAuthenticationType ) const;
	bool					GetReceiverEncryptionType( EncryptionType & ioEncryptionType ) const;
	bool					GetWEPKeyIndex( UInt8 & outWEPKeyIndex ) const;

	bool					FindTCPOption( const UInt8 inTCPOptionID,
								const UInt8 *& outOptionData, UInt16 & outOptionByteCount ) const;
	bool					GetTCPWindowScale( UInt8 & outScale ) const;
	bool					GetTCPSACK( UInt32 & outSACKBegin, UInt32 & outSACKEnd ) const;
	bool					GetTCPMSS(  UInt16 & outMSS ) const;

	bool					GetFrameCheckSequence( UInt32& outFCS ) const;

	// application-layer stuff
	const AppLayer &		App() const;	// packet.App().GetThingy()

	void					SetFlowID( const UInt32 inFlowID ) const;
	bool					GetFlowID( UInt32 & outFlowID ) const;
	void					SetVoIPCallID( const UInt64 inCallID ) const;
	bool					GetVoIPCallID( UInt64 & outCallID ) const;
	void					SetVoIPCallFlowIndex( const UInt32 inCallFlowIndex ) const;
	bool					GetVoIPCallFlowIndex( UInt32 & outCallFlowIndex ) const;
	
private:
	const WirelessHeaderFacade*	GetWirelessHeaderFacade() const;
	void ConstructWirelessHeaderFacade(const MediaSpecificHeaderAll*  pHeader);

protected:
	TMediaType					m_MediaType;
	TMediaSubType				m_MediaSubType;
	PeekPacket*					m_pPeekPacket;
	UInt8*						m_pPacketData;
	mutable bool				m_bOwnsPacket;
	mutable bool				m_bProtocol;
	mutable bool				m_bCalcPhysical;
	mutable bool				m_bCalcLogical;
	mutable bool				m_bCalcPorts;
	mutable bool				m_bBSSID;
	mutable bool				m_bReceiver;
	mutable bool				m_bTransmitter;
	mutable bool				m_bAddress1;
	mutable bool				m_bAddress2;
	mutable bool				m_bAddress3;
	mutable bool				m_bAddress4;
	mutable bool				m_bTransmitterImplied;
	mutable bool				m_bTransmitterAuthenticationType;
	mutable bool				m_bTransmitterEncryptionType;
	mutable bool				m_bReceiverEncryptionType;
	mutable bool				m_bWEPKeyIndexChecked;
	mutable bool				m_bWEPKeyIndexValid;
	mutable bool				m_bTCPOption;
	mutable bool				m_bTCPOptionWindowScaleCached;
	mutable bool				m_bTCPOptionWindowScaleExists;
	mutable bool				m_bTCPOptionSACK;
	mutable bool				m_bTCPOptionMSS;
	mutable bool				m_bFlowID;
	mutable bool				m_bVoIPCallID;
	mutable bool				m_bVoIPCallFlowIndex;
	mutable bool				m_bVLANID;
	mutable bool				m_bMPLSLabel;
	mutable CMediaSpec			m_Protocol;
	mutable CMediaSpec			m_SrcPhysical;
	mutable CMediaSpec			m_DestPhysical;
	mutable CMediaSpec			m_SrcLogical;
	mutable CMediaSpec			m_DestLogical;
	mutable CMediaSpec			m_SrcPort;
	mutable CMediaSpec			m_DestPort;
	mutable CMediaSpec			m_BSSID;
	mutable CMediaSpec			m_Receiver;
	mutable CMediaSpec			m_Transmitter;
	mutable CMediaSpec*			m_pAddress1;
	mutable CMediaSpec*			m_pAddress2;
	mutable CMediaSpec*			m_pAddress3;
	mutable CMediaSpec*			m_pAddress4;
	mutable AuthenticationType	m_nTransmitterAuthenticationType;
	mutable EncryptionType		m_nTransmitterEncryptionType;
	mutable EncryptionType		m_nReceiverEncryptionType;
	mutable UInt8				m_nWEPKeyIndex;
	mutable UInt32				m_ProtocolIndex;
	mutable UInt32				m_SrcPhysicalIndex;
	mutable UInt32				m_DestPhysicalIndex;
	mutable UInt32				m_SrcLogicalIndex;
	mutable UInt32				m_DestLogicalIndex;
	mutable const UInt8 *		m_pTCPOptionBegin;
	mutable const UInt8 *		m_pTCPOptionEnd;
	mutable UInt8				m_nTCPOptionWindowScale;
	mutable UInt32				m_nTCPOptionSACKBegin;
	mutable UInt32				m_nTCPOptionSACKEnd;
	mutable UInt16				m_nTCPOptionMSS;
	const CPacketBuffer*		m_pSourceBuffer;
	UInt32						m_nPacketIndex;
	mutable bool				m_bProtospecArray;
	mutable ProtoSpecs::SProtospecInfo	m_ProtospecArray[ MAX_PROTOSPEC_DEPTH ];
	mutable int					m_nProtospecCount;
	mutable std::auto_ptr< ::AppLayer >	m_pAppLayer;
	mutable UInt32				m_FlowID;
	mutable UInt64				m_VoIPCallID;
	mutable UInt32				m_VoIPCallFlowIndex;
	mutable UInt32				m_FrameCheckSequence;
	mutable TArrayEx<UInt16>	m_ayVLANID;
	mutable TArrayEx<UInt32>	m_ayMPLSLabel;

	std::auto_ptr<WirelessHeaderFacade>		m_pWirelessHeaderFacade;

	bool					CalcProtocol( CMediaSpec& outSpec ) const;
	bool					CalcRawProtocol( CMediaSpec& outSpec ) const;
	void					CalcPhysical() const;
	void					CalcLogical() const;
	void					CalcPorts() const;
	bool					CalcBSSID( CMediaSpec& outSpec ) const;
	bool					CalcReceiver( CMediaSpec& outSpec ) const;
	bool					CalcTransmitter( CMediaSpec& outSpec ) const;
	bool					CalcProtospecArray() const;
	bool					CalcFrameCheckSequence() const;
	bool					CalcVLANIDs( TArrayEx<UInt16>& outArray ) const;
	bool					CalcMPLSLabels( TArrayEx<UInt32>& outArray ) const;

	enum HeaderOrData { kHeader, kData };
	const UInt8 *
	GetPacketData(
		const PROTOSPEC_ID	inProtocol,
		const HeaderOrData	inHeaderOrData,
		UInt16 *			outBytesRemaining ) const;
	
};


// -----------------------------------------------------------------------------
//		IsValid
// -----------------------------------------------------------------------------

inline bool
CPacket::IsValid() const
{
	return (m_pPeekPacket != NULL) && (m_pPacketData != NULL);
}


// -----------------------------------------------------------------------------
//		GetPacketBuffer
// -----------------------------------------------------------------------------

inline const CPacketBuffer*
CPacket::GetPacketBuffer() const
{
	return m_pSourceBuffer;
}


// -----------------------------------------------------------------------------
//		GetPacketBufferIndex
// -----------------------------------------------------------------------------

inline UInt32
CPacket::GetPacketBufferIndex() const
{
	return m_nPacketIndex;
}


// -----------------------------------------------------------------------------
//		GetPacketData
// -----------------------------------------------------------------------------

inline const UInt8*
CPacket::GetPacketData() const
{
	return m_pPacketData;
}


// -----------------------------------------------------------------------------
//		GetPacketHeader
// -----------------------------------------------------------------------------

inline const PeekPacket*
CPacket::GetPacketHeader() const
{
	return m_pPeekPacket;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecInfoBlock
// -----------------------------------------------------------------------------

inline const UInt8*
CPacket::GetMediaSpecInfoBlock() const
{
	ASSERT( m_pPeekPacket != NULL );
	return m_pPeekPacket->fMediaSpecInfoBlock;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecHeader
// -----------------------------------------------------------------------------

inline const MediaSpecificPrivateHeader*
CPacket::GetMediaSpecHeader() const
{
	ASSERT( m_pPeekPacket != NULL );
	return reinterpret_cast<const MediaSpecificPrivateHeader*>(m_pPeekPacket->fMediaSpecInfoBlock);
}


// -----------------------------------------------------------------------------
//		GetWirelessHeader
// -----------------------------------------------------------------------------

inline const Wireless80211PrivateHeader*
CPacket::GetWirelessHeader() const
{
	const MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
	if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_Wireless3) )
	{
		return reinterpret_cast<const Wireless80211PrivateHeader*>
			(((const UInt8*) pHdr) + sizeof(MediaSpecificPrivateHeader));
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetFullDuplexHeader
// -----------------------------------------------------------------------------

inline const FullDuplexPrivateHeader*
CPacket::GetFullDuplexHeader() const
{
	const MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
	if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_FullDuplex) )
	{
		return reinterpret_cast<const FullDuplexPrivateHeader*>
			(((const UInt8*) pHdr) + sizeof(MediaSpecificPrivateHeader));
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetWanHeader
// -----------------------------------------------------------------------------

inline const WideAreaNetworkHeader*
CPacket::GetWanHeader() const
{
	const MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
	if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_Wan) )
	{
		return reinterpret_cast<const WideAreaNetworkHeader*>
			(((const UInt8*) pHdr) + sizeof(MediaSpecificPrivateHeader));
	}

	return NULL;
}

// -----------------------------------------------------------------------------
//		GetWanHeader
// -----------------------------------------------------------------------------

inline WideAreaNetworkHeader*
CPacket::GetWanHeader()
{
	ASSERT( m_pPeekPacket != NULL );
	MediaSpecificPrivateHeader* pHdr = reinterpret_cast<MediaSpecificPrivateHeader*>(m_pPeekPacket->fMediaSpecInfoBlock);
	if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_Wan) )
	{
		return reinterpret_cast<WideAreaNetworkHeader*>
			(((UInt8*) pHdr) + sizeof(MediaSpecificPrivateHeader));
	}

	return NULL;
}

// -----------------------------------------------------------------------------
//		GetMediaType
// -----------------------------------------------------------------------------

inline TMediaType
CPacket::GetMediaType() const
{
	return m_MediaType;
}


// -----------------------------------------------------------------------------
//		GetMediaSubType
// -----------------------------------------------------------------------------

inline TMediaSubType
CPacket::GetMediaSubType() const
{
	return m_MediaSubType;
}


// -----------------------------------------------------------------------------
//		GetPacketLength
// -----------------------------------------------------------------------------

inline UInt16
CPacket::GetPacketLength() const
{
	ASSERT( m_pPeekPacket != NULL );
	return m_pPeekPacket->fPacketLength;
}


// -----------------------------------------------------------------------------
//		GetSliceLength
// -----------------------------------------------------------------------------

inline UInt16
CPacket::GetSliceLength() const
{
	ASSERT( m_pPeekPacket != NULL );
	return m_pPeekPacket->fSliceLength;
}


// -----------------------------------------------------------------------------
//		GetActualLength
// -----------------------------------------------------------------------------

inline UInt16
CPacket::GetActualLength() const
{
	ASSERT( m_pPeekPacket != NULL );
	UInt16	nBytes = m_pPeekPacket->fSliceLength;
	if ( nBytes == 0 )
	{
		nBytes = m_pPeekPacket->fPacketLength;
	}
	return nBytes;
}


// -----------------------------------------------------------------------------
//		GetFlags
// -----------------------------------------------------------------------------

inline UInt32
CPacket::GetFlags() const
{
	ASSERT( m_pPeekPacket != NULL );
	return m_pPeekPacket->fFlags;
}


// -----------------------------------------------------------------------------
//		GetStatus
// -----------------------------------------------------------------------------

inline UInt32
CPacket::GetStatus() const
{
	ASSERT( m_pPeekPacket != NULL );
	return m_pPeekPacket->fStatus;
}


// -----------------------------------------------------------------------------
//		GetTimeStamp
// -----------------------------------------------------------------------------

inline UInt64
CPacket::GetTimeStamp() const
{
	ASSERT( m_pPeekPacket != NULL );
	return m_pPeekPacket->fTimeStamp;
}


// -----------------------------------------------------------------------------
//		IsWirelessPacket
// -----------------------------------------------------------------------------

inline bool
CPacket::IsWirelessPacket() const
{
	// All wireless packets have a valid WirelessHeaderFacade pointer
	const WirelessHeaderFacade* pFacade = GetWirelessHeaderFacade();
	return (pFacade != 0);
}


// -----------------------------------------------------------------------------
//		IsFullDuplexPacket
// -----------------------------------------------------------------------------

inline bool
CPacket::IsFullDuplexPacket() const
{
	const MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
	return (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_FullDuplex);
}


// -----------------------------------------------------------------------------
//		IsFullDuplexPacket
// -----------------------------------------------------------------------------

inline bool
CPacket::IsWanPacket() const
{
	const MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
	return (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_Wan);
}


// -----------------------------------------------------------------------------
//		SetFlag
// -----------------------------------------------------------------------------

inline void
CPacket::SetFlag(
	UInt32	inFlag )
{
	ASSERT( m_pPeekPacket != NULL );
	m_pPeekPacket->fFlags |= inFlag;
}


// -----------------------------------------------------------------------------
//		ClearFlag
// -----------------------------------------------------------------------------

inline void
CPacket::ClearFlag(
	UInt32	inFlag )
{
	ASSERT( m_pPeekPacket != NULL );
	m_pPeekPacket->fFlags &= ~inFlag;
}


// -----------------------------------------------------------------------------
//		TestFlag
// -----------------------------------------------------------------------------

inline bool
CPacket::TestFlag(
	UInt32	inFlag ) const
{
	ASSERT( m_pPeekPacket != NULL );
	return ((m_pPeekPacket->fFlags & inFlag) != 0);
}


// -----------------------------------------------------------------------------
//		SetStatus
// -----------------------------------------------------------------------------

inline void
CPacket::SetStatus(
	UInt32	inStatus )
{
	ASSERT( m_pPeekPacket != NULL );
	m_pPeekPacket->fStatus |= inStatus;
}


// -----------------------------------------------------------------------------
//		ClearStatus
// -----------------------------------------------------------------------------

inline void
CPacket::ClearStatus(
	UInt32	inStatus )
{
	ASSERT( m_pPeekPacket != NULL );
	m_pPeekPacket->fStatus &= ~inStatus;
}


// -----------------------------------------------------------------------------
//		TestStatus
// -----------------------------------------------------------------------------

inline bool
CPacket::TestStatus(
	UInt32	inStatus ) const
{
	ASSERT( m_pPeekPacket != NULL );
	return ((m_pPeekPacket->fStatus & inStatus) != 0);
}


// -----------------------------------------------------------------------------
//		IsError
// -----------------------------------------------------------------------------

inline bool
CPacket::IsError() const
{
	return TestFlag( kPacketFlag_CRC | kPacketFlag_Frame |
		kPacketFlag_Runt | kPacketFlag_Oversize );
}


// -----------------------------------------------------------------------------
//		GetChannel
// -----------------------------------------------------------------------------

inline SInt16
CPacket::GetChannel() const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetWirelessChannel().Channel : 0;
}


// -----------------------------------------------------------------------------
//		GetChannel
// -----------------------------------------------------------------------------

inline bool
CPacket::GetChannel(SWirelessChannel& outChannelInfo) const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	if ( pHdr == NULL )
		return false;

	outChannelInfo = pHdr->GetWirelessChannel();
	return true;
}

// -----------------------------------------------------------------------------
//		GetDataRate
// -----------------------------------------------------------------------------

inline UInt16
CPacket::GetDataRate() const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetDataRate() : 0;
}


// -----------------------------------------------------------------------------
//		GetCalculatedDataRate
// -----------------------------------------------------------------------------

inline double
CPacket::GetCalculatedDataRate() const
{
	double	dataRate = 0.0;
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	if ( pHdr != NULL )
	{
		SWirelessChannel channelInfo = pHdr->GetWirelessChannel();
		dataRate = MCSTable::GetDataRate( channelInfo.Band, pHdr->GetDataRate(), pHdr->GetFlagsN() );
	}

	return dataRate;
}


// -----------------------------------------------------------------------------
//		GetMCSIndex
// -----------------------------------------------------------------------------

inline UInt16
CPacket::GetMCSIndex() const
{
	UInt16	mcs = (UInt16)-1;
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	if ( pHdr != NULL )
	{
		SWirelessChannel channelInfo = pHdr->GetWirelessChannel();
		mcs = MCSTable::GetMCSIndex( channelInfo.Band, pHdr->GetDataRate(), pHdr->GetFlagsN() );
	}

	return mcs;
}


// -----------------------------------------------------------------------------
//		GetSpatialStreams
// -----------------------------------------------------------------------------

inline UInt32
CPacket::GetSpatialStreams() const
{
	UInt32	spatialStreamCnt = 0;
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	if ( pHdr != NULL )
	{
		SWirelessChannel channelInfo = pHdr->GetWirelessChannel();
		spatialStreamCnt = MCSTable::GetSpatialStreams( channelInfo.Band, pHdr->GetDataRate(), pHdr->GetFlagsN() );
	}

	return spatialStreamCnt;
}


// -----------------------------------------------------------------------------
//		GetSignalLevel
// -----------------------------------------------------------------------------

inline UInt8
CPacket::GetSignalLevel() const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetSignalLevel() : 0;
}


// -----------------------------------------------------------------------------
//		GetSignaldBm
// -----------------------------------------------------------------------------

inline SInt8
CPacket::GetSignaldBm() const
{
	// TODO: update for 802.11n
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetSignaldBm() : PEEK_NULL_DBM;
}


// -----------------------------------------------------------------------------
//		GetSignaldBmN
// -----------------------------------------------------------------------------

inline SInt8
CPacket::GetSignaldBmN(UInt8 nStream) const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	if (pHdr == NULL) return PEEK_NULL_DBM;
	
	return pHdr->GetSignaldBmN(nStream);
}


// -----------------------------------------------------------------------------
//		GetNoiseLevel
// -----------------------------------------------------------------------------

inline UInt8
CPacket::GetNoiseLevel( ) const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetNoiseLevel() : 0;
}


// -----------------------------------------------------------------------------
//		GetNoisedBm
// -----------------------------------------------------------------------------

inline SInt8
CPacket::GetNoisedBm() const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetNoisedBm() : PEEK_NULL_DBM;
}


// -----------------------------------------------------------------------------
//		GetNoisedBmN
// -----------------------------------------------------------------------------

inline SInt8
CPacket::GetNoisedBmN(UInt8 nStream) const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	if (pHdr == NULL) return PEEK_NULL_DBM;
	
	return pHdr->GetNoisedBmN(nStream);
}

// -----------------------------------------------------------------------------
//		GetFlagsN
// -----------------------------------------------------------------------------

inline UInt32
CPacket::GetFlagsN() const
{
	const WirelessHeaderFacade*	pHdr = GetWirelessHeaderFacade();
	ASSERT( pHdr != NULL );
	return (pHdr != NULL) ? pHdr->GetFlagsN() : 0;
}

// -----------------------------------------------------------------------------
//		GetFullDuplexChannel
// -----------------------------------------------------------------------------

inline UInt32
CPacket::GetFullDuplexChannel() const
{
	const FullDuplexPrivateHeader*	pHdr = GetFullDuplexHeader();
	return (pHdr != NULL) ? pHdr->Channel : PEEK_INVALID_DUPLEX_CHANNEL;
}


// -----------------------------------------------------------------------------
//		GetWanDirection
// -----------------------------------------------------------------------------

inline UInt8
CPacket::GetWanDirection() const
{
	const WideAreaNetworkHeader*	pHdr = GetWanHeader();
	return (pHdr != NULL) ? pHdr->Direction : PEEK_WAN_DIRECTION_INVALID;
}


// -----------------------------------------------------------------------------
//		GetWanProtocol
// -----------------------------------------------------------------------------

inline UInt8
CPacket::GetWanProtocol() const
{
	const WideAreaNetworkHeader*	pHdr = GetWanHeader();
	return (pHdr != NULL) ? pHdr->Protocol : PEEK_WAN_PROT_INVALID;
}

#endif
