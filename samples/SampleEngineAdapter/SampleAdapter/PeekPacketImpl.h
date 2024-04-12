// =============================================================================
//	peekpacketimpl.h
// =============================================================================
//	Copyright (c) 2001-2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "MediaSpec.h"
#include "Protospecs.h"
#include "icloneable.h"

//#define OPT_THREADSAFEPACKET
#ifdef OPT_THREADSAFEPACKET
typedef CHeMultiThreadModel PacketThreadModel;
#else
typedef CHeMultiThreadModelNoCS PacketThreadModel;
#endif

/// \class Packet
/// \ingroup Packet
/// \brief coclass PeekCore.Packet.
/// \see IPacketInitialize, IPacket, IPacketLayers, IPacketSpecs
class HE_NO_VTABLE Packet :
	public CHeObjRoot<PacketThreadModel>,
	public CHeClass<Packet>,
	public IPacket,
	public IPacketLayers,
	public IPacketSpecs,
	public IFullDuplexPacket,
	public IWirelessPacket,
	public IWanPacket,
	public ITCPPacket
{
private:
	IPacketBuffer*				m_pPacketBuffer;
	UInt32						m_PacketBufferIndex;
	UInt64						m_PacketNumber;
	PeekPacket*					m_pPacketHeader;
	const UInt8*				m_pPacketData;
	const UInt8*				m_pPacketDataEnd;
	TMediaType					m_MediaType;
	TMediaSubType				m_MediaSubType;
	UInt16						m_ActualLength;
	bool						m_bPacketNumber;
	bool						m_bProtocol;
	bool						m_bCalcPhysical;
	bool						m_bCalcLogical;
	bool						m_bCalcPorts;
	bool						m_bBSSID;
	bool						m_bReceiver;
	bool						m_bTransmitter;
	bool						m_bAddress1;
	bool						m_bAddress2;
	bool						m_bAddress3;
	bool						m_bAddress4;
	bool						m_bTransmitterImplied;
	bool						m_bTransmitterAuthenticationType;
	bool						m_bTransmitterEncryptionType;
	bool						m_bReceiverEncryptionType;
	bool						m_bWEPKeyIndexChecked;
	bool						m_bWEPKeyIndexValid;
	bool						m_bIPHeader;
	bool						m_bTCPHeader;
	bool						m_bTCPOptions;
	bool						m_bProtoSpecArray;
	int							m_nProtoSpecCount;
	ProtoSpecs::SProtospecInfo	m_ProtoSpecArray[MAX_PROTOSPEC_DEPTH];
	UInt16						m_ProtoSpecIDArray[MAX_PROTOSPEC_DEPTH];
	CMediaSpec					m_Protocol;
	CMediaSpec					m_SrcPhysical;
	CMediaSpec					m_DestPhysical;
	CMediaSpec					m_SrcLogical;
	CMediaSpec					m_DestLogical;
	CMediaSpec					m_SrcPort;
	CMediaSpec					m_DestPort;
	CMediaSpec					m_BSSID;
	CMediaSpec					m_Receiver;
	CMediaSpec					m_Transmitter;
	CMediaSpec*					m_pAddress1;
	CMediaSpec*					m_pAddress2;
	CMediaSpec*					m_pAddress3;
	CMediaSpec*					m_pAddress4;
	PeekEncryptionType			m_TransmitterEncryptionType;
	PeekEncryptionType			m_ReceiverEncryptionType;
	PeekAuthenticationType		m_TransmitterAuthenticationType;
	SInt32						m_WEPKeyIndex;

	bool						m_bIPv4;
	UInt16						m_IPTotalLength;
	UInt16						m_IPID;
	UInt16						m_IPFragmentBits;

	bool						m_bTCP;
	UInt32						m_TCPSequence;
	UInt32						m_TCPAck;
	UInt8						m_TCPFlags;
	UInt16						m_TCPWindow;
	UInt16						m_TCPChecksum;
	UInt16						m_TCPUrgentPointer;
	const UInt8*				m_pTCPOptionBegin;
	const UInt8*				m_pTCPOptionEnd;

	bool						m_bTCPOptionMSS;
	bool						m_bTCPOptionWindowScale;
	bool						m_bTCPOptionSACK;
	bool						m_bTCPOptionTimestamp;
	UInt16						m_TCPOptionMSS;
	UInt8						m_TCPOptionWindowScale;
	UInt32						m_TCPOptionSACKBegin;
	UInt32						m_TCPOptionSACKEnd;
	UInt32						m_TCPOptionTimestampValue;
	UInt32						m_TCPOptionTimestampEcho;

	CHePtr<IHeUnknown>			m_spAppLayer;
	UInt32						m_FlowID;
	UInt32						m_VoIPCallFlowIndex;
	UInt64						m_VoIPCallID;

public:
	HE_INTERFACE_MAP_BEGIN(Packet)
        HE_INTERFACE_ENTRY_IID(IPacketSpecs_IID,IPacketSpecs)
        HE_INTERFACE_ENTRY_IID(IPacketLayers_IID,IPacketLayers)
        HE_INTERFACE_ENTRY_IID(ITCPPacket_IID,ITCPPacket)
        HE_INTERFACE_ENTRY_IID(IPacket_IID,IPacket)
        HE_INTERFACE_ENTRY_IID(IFullDuplexPacket_IID,IFullDuplexPacket)
        HE_INTERFACE_ENTRY_IID(IWirelessPacket_IID,IWirelessPacket)
        HE_INTERFACE_ENTRY_IID(IWanPacket_IID,IWanPacket)
	HE_INTERFACE_MAP_END()

	;			Packet();
	virtual		~Packet() {}

	HeResult	FinalConstruct();
	void		FinalRelease();

	// IPacket
public:
	HE_IMETHOD Initialize(IPacketBuffer* pPacketBuffer, UInt32 nPacketBufferIndex,
		PeekPacket* pPacketHeader, const UInt8* pPacketData,
		TMediaType mt, TMediaSubType mst);
	HE_IMETHOD GetPacketHeader(const PeekPacket** ppPeekPacket);
	HE_IMETHOD GetPacketData(const UInt8** ppPacketData);
	HE_IMETHOD GetPacketDataEnd(const UInt8** ppPacketData);
	HE_IMETHOD GetPacketMediaSpecificInfo(const UInt8** ppMediaSpecificInfo,
		UInt32 *pulMediaSpecificInfoLength);
	HE_IMETHOD GetPacketBuffer(IPacketBuffer** ppPacketBuffer);
	HE_IMETHOD GetPacketBufferIndex(UInt32* pIndex);
	HE_IMETHOD GetPacketNumber(UInt64* pPacketNumber);
	HE_IMETHOD GetMediaType(TMediaType* pMediaType);
	HE_IMETHOD GetMediaSubType(TMediaSubType* pMediaSubType);
	HE_IMETHOD GetPacketLength(UInt16* pcbPacketLength);
	HE_IMETHOD GetSliceLength(UInt16* pcbSliceLength);
	HE_IMETHOD GetActualLength(UInt16* pcbActualLength);
	HE_IMETHOD GetFlags(UInt32* pFlags);
	HE_IMETHOD GetStatus(UInt32* pStatus);
	HE_IMETHOD GetTimeStamp(UInt64* pnTimeStamp);
	HE_IMETHOD GetProtoSpec(UInt32* pProtoSpec);
	HE_IMETHOD GetProtoSpecID(UInt16* pProtoSpecID);
	HE_IMETHOD SetFlag(UInt32 f);
	HE_IMETHOD ClearFlag(UInt32 f);
	HE_IMETHOD TestFlag(UInt32 f, BOOL* pbResult);
	HE_IMETHOD SetStatus(UInt32 s);
	HE_IMETHOD ClearStatus(UInt32 s);
	HE_IMETHOD TestStatus(UInt32 s, BOOL* pbResult);
	HE_IMETHOD IsWirelessPacket(BOOL* pbWirelessPacket);
	HE_IMETHOD IsFullDuplexPacket(BOOL* pbFullDuplexPacket);
	HE_IMETHOD IsWanPacket(BOOL* pbWanPacket);
	HE_IMETHOD IsError(BOOL* pbError);
	HE_IMETHOD IsMulticast(BOOL* pbMulticast);
	HE_IMETHOD IsBroadcast(BOOL* pbBroadcast);
	HE_IMETHOD IsDescendentOf(const UInt16 pParentArray[], SInt32 lCount,
		UInt32* pulMatchInstanceID, SInt32* plMatchIndex, BOOL* pbResult);
	HE_IMETHOD IsFragment(BOOL* pbFragment);
	HE_IMETHOD GetIPFragmentOffset(UInt16* pFragmentOffset);
	HE_IMETHOD GetIPFragmentBits(UInt16* pFragmentBits);
	HE_IMETHOD GetIPID(UInt16* pIPID);
	HE_IMETHOD GetIPLength(UInt16* pIPLength);
	HE_IMETHOD GetAppLayer(IHeUnknown** ppUnkAppLayer);
	HE_IMETHOD SetAppLayer(IHeUnknown* pUnkAppLayer);
	HE_IMETHOD GetFlowID(UInt32* pFlowID);
	HE_IMETHOD SetFlowID(UInt32 nFlowID);
	HE_IMETHOD GetVoIPCallID(UInt64* pCallID);
	HE_IMETHOD SetVoIPCallID(UInt64 nCallID);
	HE_IMETHOD GetVoIPCallFlowIndex(UInt32* pCallFlowIndex);
	HE_IMETHOD SetVoIPCallFlowIndex(UInt32 nCallFlowIndex);

	// IPacketLayers
public:
	HE_IMETHOD GetPacketLayers(UInt32* pulLayerCount, PacketLayerInfo pLayerInfo[]);
	HE_IMETHOD GetHeaderLayer(UInt16 usProtoSpecID, UInt32* pulSourceProtoSpec,
		const UInt8** ppData, UInt16* pusBytesRemaining);
	HE_IMETHOD GetDataLayer(UInt16 usProtoSpecID, UInt32* pulSourceProtoSpec,
		const UInt8** ppData, UInt16* pusBytesRemaining);

private:
	HE_IMETHOD GetPhysicalLayer(const UInt8** ppData, UInt16* pusBytesRemaining);
	HE_IMETHOD GetLinkLayer(const UInt8** ppData, UInt16* pusBytesRemaining);
	HE_IMETHOD GetNetworkLayer(const UInt8** ppData, UInt16* pusBytesRemaining);

	// IPacketSpecs
public:
	HE_IMETHOD GetProtocol(TMediaSpec* pSpec);
	HE_IMETHOD GetRawProtocol(TMediaSpec* pSpec);
	HE_IMETHOD GetSrcPhysical(TMediaSpec* pSpec);
	HE_IMETHOD GetDestPhysical(TMediaSpec* pSpec);
	HE_IMETHOD GetSrcLogical(TMediaSpec* pSpec);
	HE_IMETHOD GetDestLogical(TMediaSpec* pSpec);
	HE_IMETHOD GetSrcPort(TMediaSpec* pSpec);
	HE_IMETHOD GetDestPort(TMediaSpec* pSpec);
	HE_IMETHOD GetBSSID(TMediaSpec* pSpec);
	HE_IMETHOD GetReceiver(TMediaSpec* pSpec);
	HE_IMETHOD GetTransmitter(TMediaSpec* pSpec);
	HE_IMETHOD GetAddress1(TMediaSpec* pSpec);
	HE_IMETHOD GetAddress2(TMediaSpec* pSpec);
	HE_IMETHOD GetAddress3(TMediaSpec* pSpec);
	HE_IMETHOD GetAddress4(TMediaSpec* pSpec);
	HE_IMETHOD IsTransmitterImplied(BOOL* pbImplied);

	// IFullDuplexPacket
public:
	HE_IMETHOD GetFullDuplexChannel(UInt32* pChannel);

	// IWirelessPacket
public:
	HE_IMETHOD GetWirelessChannel(WirelessChannel* pChannel);
	HE_IMETHOD GetDataRate(UInt16* pDataRate);
	HE_IMETHOD GetFlagsN(UInt32* pFlags);
	HE_IMETHOD GetSignalLevel(UInt8* pSignalLevel);
	HE_IMETHOD GetSignaldBm(SInt8* pSignaldBm);
	HE_IMETHOD GetSignaldBmN(UInt8 nStream, SInt8* pSignaldBm);
	HE_IMETHOD GetNoiseLevel(UInt8* pNoiseLevel);
	HE_IMETHOD GetNoisedBm(SInt8* pNoisedBm);
	HE_IMETHOD GetNoisedBmN(UInt8 nStream, SInt8* pNoisedBm);
	HE_IMETHOD GetTransmitterEncryptionType(PeekEncryptionType* pEncryptionType);
	HE_IMETHOD GetReceiverEncryptionType(PeekEncryptionType* pEncryptionType);
	HE_IMETHOD GetTransmitterAuthenticationType(PeekAuthenticationType* pAuthenticationType);
	HE_IMETHOD GetWEPKeyIndex(SInt32* pWEPKeyIndex);
	HE_IMETHOD GetWirelessControlFlags(UInt8* pFlags);
	HE_IMETHOD GetWirelessFragmentNumber(UInt8* pFragmentNumber);

	// IWanPacket
public:
	HE_IMETHOD GetWanDirection(UInt8* pWanDirection);
	HE_IMETHOD GetWanProtocol(UInt8* pWanProtocol);

	// ITCPPacket
public:
	HE_IMETHOD GetTCPSequence(UInt32* pVal);
	HE_IMETHOD GetTCPAck(UInt32* pVal);
	HE_IMETHOD GetTCPFlags(UInt8* pVal);
	HE_IMETHOD GetTCPWindow(UInt16* pVal);
	HE_IMETHOD GetTCPChecksum(UInt16* pVal);
	HE_IMETHOD GetTCPUrgentPointer(UInt16* pVal);
	HE_IMETHOD GetTCPOption(UInt8 optionType, const UInt8** ppData, UInt32* pcbData);
	HE_IMETHOD GetTCPOptionMSS(UInt16* pVal);
	HE_IMETHOD GetTCPOptionWindowScale(UInt8* pVal);
	HE_IMETHOD GetTCPOptionSACK(UInt32* pBegin, UInt32* pEnd);
	HE_IMETHOD GetTCPOptionTimestamp(UInt32* pVal, UInt32* pEcho);

private:
	UInt32	GetProtoSpec();
	bool	CalcProtoSpecArray();
	bool	CalcProtocol( CMediaSpec& outSpec );
	bool	CalcRawProtocol( CMediaSpec& outSpec );
	void	CalcPhysical();
	void	CalcLogical();
	void	CalcPorts();
	bool	CalcBSSID( CMediaSpec& outSpec );
	bool	CalcReceiver( CMediaSpec& outSpec );
	bool	CalcTransmitter( CMediaSpec& outSpec );
	bool	CalcIPHeader();
	bool	CalcTCPHeader();
	void	CalcTCPOptions();

private:
	inline MediaSpecificPrivateHeader* GetMediaSpecHeader() const {
		if ( m_pPacketHeader != NULL ) {
			return reinterpret_cast<MediaSpecificPrivateHeader*>(m_pPacketHeader->fMediaSpecInfoBlock);
		}
		return NULL;
	}

	inline FullDuplexPrivateHeader* GetFullDuplexHeader() const {
		MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
		if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_FullDuplex) ) {
			return reinterpret_cast<FullDuplexPrivateHeader*>
				(((UInt8*)pHdr) + sizeof(MediaSpecificPrivateHeader));
		}
		return NULL;
	}

	inline Wireless80211PrivateHeader* GetWirelessHeader() const {
		MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
		if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_Wireless3) ) {
			return reinterpret_cast<Wireless80211PrivateHeader*>
				(((UInt8*)pHdr) + sizeof(MediaSpecificPrivateHeader));
		}
		return NULL;
	}

	inline WideAreaNetworkHeader* GetWanHeader() const {
		const MediaSpecificPrivateHeader*	pHdr = GetMediaSpecHeader();
		if ( (pHdr != NULL) && (pHdr->nType == kMediaSpecificHdrType_Wan) ) {
			return reinterpret_cast<WideAreaNetworkHeader*>
				(((UInt8*)pHdr) + sizeof(MediaSpecificPrivateHeader));
		}
		return NULL;
	}
};
