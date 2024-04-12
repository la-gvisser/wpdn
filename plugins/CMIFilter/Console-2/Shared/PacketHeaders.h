// =============================================================================
//	PacketHeaders.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#ifndef PACKETHEADERS_H
#define PACKETHEADERS_H

#include "AGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

// -----------------------------------------------------------------------------
//		Ethernet Packet stuff
// -----------------------------------------------------------------------------

struct EthernetPacketHeader
{
	UInt8		DestAddr[6];		// Destination address.
	UInt8		SrcAddr[6];			// Source address.
	UInt16		Protocol;			// Protocol.
};
typedef struct EthernetPacketHeader EthernetPacketHeader;

// -----------------------------------------------------------------------------
//		TokenRing Packet stuff
// -----------------------------------------------------------------------------

struct TokenRingPacketHeader
{
	UInt8		AccessControl;		// MAC header.
	UInt8		FrameControl;		// MAC header.
	UInt8		DestAddr[6];		// Destination address.
	UInt8		SrcAddr[6];			// Source address.
};
typedef struct TokenRingPacketHeader TokenRingPacketHeader;

// -----------------------------------------------------------------------------
//		Wireless Packet stuff
// -----------------------------------------------------------------------------

struct Wireless80211MacHeader
{
	UInt8		FrameControlType;
	UInt8		FrameControlFlags;
	UInt16		Duration;
	UInt8		Addr1[6];
	UInt8		Addr2[6];
	UInt8		Addr3[6];
	UInt16		Sequence;
	UInt8		Addr4[6];
};
typedef struct Wireless80211MacHeader Wireless80211MacHeader;

// ------------------------------------------------------------
//		Media Specific Info - things not shared between media types
//			IMPORTANT - all of these structures must exactly
//				match the definitions in PeekInterface.h
// ------------------------------------------------------------

// ------------------------------------------------------------
//		Media Specific Header - starts off every media specfic info block
// ------------------------------------------------------------

struct MediaSpecificPrivateHeader
{
	UInt32		nSize;	// size of the whole info block, which is this struct and one of the ones below
	UInt32		nType;
};
typedef struct MediaSpecificPrivateHeader MediaSpecificPrivateHeader;

// type definitions
#define	kMediaSpecificHdrType_Invalid		0
#define	kMediaSpecificHdrType_Wireless		1	//	obsolete, these should get upgraded to WirelessEx
#define	kMediaSpecificHdrType_WirelessEx	2
#define	kMediaSpecificHdrType_FullDuplex	3
#define	kMediaSpecificHdrType_WAN			4

// ------------------------------------------------------------
//		Wireless Headers
// ------------------------------------------------------------

// original wireless header - exactly the same as PEEK_802_11_HDR
struct Wireless80211PrivateHeaderObs
{
	UInt8		DataRate;			// Data rate in 500 Kbps units.
	UInt8		Channel;			// 802.11b channel 1-14.
	UInt8		SignalStrength;		// Signal strength 0-100%.
	UInt8		Padding;
};
typedef struct Wireless80211PrivateHeaderObs Wireless80211PrivateHeaderObs;

// new wireless header - exactly the same as PEEK_802_11_HDR_EX
struct Wireless80211PrivateHeader
{
	UInt8		DataRate;			// Data rate in 500 Kbps units.
	UInt8		Channel;			// 802.11b channel 1-14.
	UInt8		SignalStrength;		// Signal strength 0-100%.
	UInt8		NoiseStrength;		// Noise strength 0-100%
	SInt16		SignaldBm;			// signal strength dBm
	SInt16		NoisedBm;			// Noise dBm
};
typedef struct Wireless80211PrivateHeader Wireless80211PrivateHeader;

// these are in PeekInterface.h, but it's crazy for plugins to have to include drivers/SDK
#define PEEK_NULL_DBM   -32767   // -32767 dBm is the smallest we can ever have in a SHORT
#define PEEK_MIN_DBM    -100                    // -100dBm is 0.1pW, the smallest usable frame I saw was 1pW
#define PEEK_MAX_DBM    -5                      // -5dBm is 320uW the biggest I ever saw was 1uW

// --------------------------------------------------------------------
//		Full Duplex header - exactly the same as PEEK_FULLDUPLEX_HDR
// --------------------------------------------------------------------

struct FullDuplexPrivateHeader
{
	UInt32		Channel;			// Channel number
};
typedef struct FullDuplexPrivateHeader FullDuplexPrivateHeader;

// these are in PeekInterface.h, but redefined here
#define	PEEK_INVALID_DUPLEX_CHANNEL		0

// --------------------------------------------------------------------
//		WAN header - exactly the same as PEEK_WAN_HDR
// --------------------------------------------------------------------

/*
struct WideAreaNetworkHeader
{
	UInt32		nChannel;
	...
};
typedef struct FullDuplexHeader FullDuplexHeader;
*/

// this typedef defines any possible combination of media specific info
typedef struct MediaSpecificHeaderAll
{
	MediaSpecificPrivateHeader  StdHeader;
	union
	{
		Wireless80211PrivateHeader	wireless;
		FullDuplexPrivateHeader		fullDuplex;
		// WAN, etc.
	} MediaInfo;
} MediaSpecificHeaderAll;

#define MAX_MEDIASPECIFICHEADER	sizeof(MediaSpecificHeaderAll)

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
