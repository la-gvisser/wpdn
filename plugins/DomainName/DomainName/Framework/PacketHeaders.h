// =============================================================================
//	PacketHeaders.h
// =============================================================================
//	Copyright (c) 1996-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "wppushpack.h"

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Various networking objects.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// -----------------------------------------------------------------------------
//		Ethernet
// -----------------------------------------------------------------------------

struct EthernetPacketHeader
{
	UInt8		DestAddr[6];		// Destination address.
	UInt8		SrcAddr[6];			// Source address.
	UInt16		Protocol;			// Protocol.
} WP_PACK_STRUCT;
typedef struct EthernetPacketHeader EthernetPacketHeader;


// -----------------------------------------------------------------------------
//		TokenRing
// -----------------------------------------------------------------------------

struct TokenRingPacketHeader
{
	UInt8		AccessControl;		// MAC header.
	UInt8		FrameControl;		// MAC header.
	UInt8		DestAddr[6];		// Destination address.
	UInt8		SrcAddr[6];			// Source address.
} WP_PACK_STRUCT;
typedef struct TokenRingPacketHeader TokenRingPacketHeader;


// -----------------------------------------------------------------------------
//		Wireless
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
} WP_PACK_STRUCT;
typedef struct Wireless80211MacHeader Wireless80211MacHeader;


// -----------------------------------------------------------------------------
//		Media Specific Info - things not shared between media types
//			IMPORTANT - all of these structures must exactly
//				match the definitions in PeekInterface.h
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//		Media Specific Header - starts off every media specific info block
// -----------------------------------------------------------------------------

struct MediaSpecificPrivateHeader
{
	UInt32		nSize;	// size of the whole info block, which is this struct and one of the ones below
	UInt32		nType;
} WP_PACK_STRUCT;
typedef struct MediaSpecificPrivateHeader MediaSpecificPrivateHeader;

// Type definitions
#define	kMediaSpecificHdrType_Invalid		0
#define	kMediaSpecificHdrType_Wireless		1	//	obsolete, these should get upgraded to WirelessEx
#define	kMediaSpecificHdrType_WirelessEx	2	//  obsolete, internal use by drivers only
#define	kMediaSpecificHdrType_FullDuplex	3
#define	kMediaSpecificHdrType_Wan			4
#define	kMediaSpecificHdrType_Wireless2		5
#define	kMediaSpecificHdrType_Wireless3		6

// -----------------------------------------------------------------------------
//		Wireless
// -----------------------------------------------------------------------------

// Original wireless header - exactly the same as PEEK_802_11_HDR.
struct Wireless80211PrivateHeaderObs
{
	UInt8		DataRate;			// Data rate in 500 Kbps units.
	UInt8		Channel;			// 802.11b channel 1-14.
	UInt8		SignalStrength;		// Signal strength 0-100%.
	UInt8		Padding;
} WP_PACK_STRUCT;
typedef struct Wireless80211PrivateHeaderObs Wireless80211PrivateHeaderObs;

// Wireless header for v5.1.x - exactly the same as PEEK_802_11_HDR_EX.
struct Wireless80211PrivateHeaderExObs
{
	UInt8		DataRate;			// Data rate in 500 Kbps units.
	UInt8		Channel;			// 802.11b channel 1-14.
	UInt8		SignalStrength;		// Signal strength 0-100%.
	UInt8		NoiseStrength;		// Noise strength 0-100%.
	SInt16		SignaldBm;			// signal strength dBm.
	SInt16		NoisedBm;			// Noise dBm.
} WP_PACK_STRUCT;
typedef struct Wireless80211PrivateHeaderExObs Wireless80211PrivateHeaderExObs;

#define PEEK_802_11_ALL_BANDS				  0	 // All
#define PEEK_802_11_BAND                      1  // vanilla 802.11 band (2MBit, 2.4GHz)
#define PEEK_802_11_B_BAND                    2  // 802.11b band (11MBit 2.4GHz)
#define PEEK_802_11_A_BAND                    3  // 802.11a band (54MBit 5GHz)
#define PEEK_802_11_G_BAND                    4  // 802.11g band (54MBit 2.4GHz)
#define PEEK_802_11_BG_BAND  PEEK_802_11_G_BAND  // (802.11g band is also 802.11b at the same time)
#define PEEK_802_11_N_BAND                    6  // 802.11n beam forming band 100MBit+ (still not released)
#define PEEK_802_11_A_TURBO_BAND              7  // 802.11a doubled speed (108 Mbit 5GHz)
#define PEEK_802_11_G_TURBO_BAND              8  // 802.11g doubled speed (108Mbit 5Ghz)
#define PEEK_802_11_SUPER_G_BAND              9  // 802.11g dynamic turbo band
#define PEEK_802_11_A_LICENSED_1MHZ_BAND    100  // [La1]"802.11a 1MHz licensed band" (FCC only for now)
#define PEEK_802_11_A_LICENSED_5MHZ_BAND    101  // [La5]"802.11a 5MHz licensed band" (FCC only for now)
#define PEEK_802_11_A_LICENSED_10MHZ_BAND   102  // [La10]"802.11a 10MHz licensed band" (FCC only for now)
#define PEEK_802_11_A_LICENSED_15MHZ_BAND   103  // [La15]"802.11a 15MHz licensed band" (FCC only for now)
#define PEEK_802_11_A_LICENSED_20MHZ_BAND   104  // [La20]"802.11a 20MHz licensed band" (FCC only for now)
#define PEEK_802_11_UNKNOWN_1_BAND          201  // [u1]"Unknown 1"
#define PEEK_802_11_UNKNOWN_2_BAND          202  // [u2]"Unknown 2"
#define PEEK_802_11_UNKNOWN_3_BAND          203  // [u3]"Unknown 3"
#define PEEK_802_11_UNKNOWN_4_BAND          204  // [u4]"Unknown 4"
#define PEEK_802_11_UNKNOWN_5_BAND          205  // [u5]"Unknown 5"
#define PEEK_802_11_UNKNOWN_6_BAND          206  // [u6]"Unknown 6"
#define PEEK_802_11_UNKNOWN_7_BAND          207  // [u7]"Unknown 7"
#define PEEK_802_11_UNKNOWN_8_BAND          208  // [u8]"Unknown 8"
#define PEEK_802_11_UNKNOWN_9_BAND          209  // [u9]"Unknown 9"
#define PEEK_802_11_N_20MHZ_BAND            300  // [n20] "802.11n 20MHz"
#define PEEK_802_11_N_40MHZ_BAND            301  // [n40] "802.11n 40MHz"
#define PEEK_802_11_N_40MHZ_LOW_BAND        302  // [n40l] "802.11n 40MHz low"
#define PEEK_802_11_N_40MHZ_HIGH_BAND       303  // [n40h] "802.11n 40MHz high"

// Check for the 4.9GHz broadband public safety band
#define IS_49GHZ_BROADBAND_PUBLIC_SAFETY_BAND(band) \
	( ((band) == PEEK_802_11_A_LICENSED_1MHZ_BAND) || \
	  ((band) == PEEK_802_11_A_LICENSED_5MHZ_BAND) || \
	  ((band) == PEEK_802_11_A_LICENSED_10MHZ_BAND) || \
	  ((band) == PEEK_802_11_A_LICENSED_15MHZ_BAND) || \
	  ((band) == PEEK_802_11_A_LICENSED_20MHZ_BAND) )

// First the detailed channel struct, same as PEEK_802_11_DETAILED_CHANNEL
struct SWirelessChannel
{
    SInt16      Channel;       // Channel number, usually between 0-199
	UInt32		Frequency;     // Frequency in MHz i.e. 2346 for 2346MHz
	UInt32		Band;          // Using the 802_11_SPEC...
} WP_PACK_STRUCT;
typedef struct SWirelessChannel SWirelessChannel;

// Pre-802.11n wireless header - exactly the same as PEEK_802_11_HDR_2.
struct Wireless80211PrivateHeader2Obs
{
	UInt8				DataRate;			// Data rate in 500 Kbps units.
	SWirelessChannel	Channel;			// channel, freq, flags to indicate spec
	UInt8				SignalStrength;		// Signal strength 0-100%.
	UInt8				NoiseStrength;		// Noise strength 0-100%
	SInt16				SignaldBm;			// signal strength dBm
	SInt16				NoisedBm;			// Noise dBm
} WP_PACK_STRUCT;
typedef struct Wireless80211PrivateHeader2Obs Wireless80211PrivateHeader2Obs;

// 2007 (Omni5) wireless header - with 802.11n support
struct Wireless80211PrivateHeader
{
	UInt16				DataRate;
	SWirelessChannel	Channel;
	UInt32				FlagsN;
	UInt8				SignalPercent;
	UInt8				NoisePercent;
	SInt8				SignaldBm;
	SInt8				NoisedBm;
	SInt8				SignaldBm1;
	SInt8				SignaldBm2;
	SInt8				SignaldBm3;
	SInt8				NoisedBm1;
	SInt8				NoisedBm2;
	SInt8				NoisedBm3;
} WP_PACK_STRUCT;
typedef struct Wireless80211PrivateHeader Wireless80211PrivateHeader;

#ifndef PEEK_NULL_DBM_SHORT
#define PEEK_NULL_DBM_SHORT	-32767		// -32767 dBm is the smallest we can ever have in a SHORT
#endif
#ifndef PEEK_NULL_DBM_CHAR
#define PEEK_NULL_DBM_CHAR	-127		// -127 dBm is the smallest we can ever have in a CHAR
#endif
#ifndef PEEK_NULL_DBM
#define PEEK_NULL_DBM		PEEK_NULL_DBM_CHAR
#endif
#ifndef PEEK_MIN_DBM
#define PEEK_MIN_DBM		-120		// we've seen devices report values of under -110
#endif
#ifndef PEEK_MAX_DBM
#define PEEK_MAX_DBM		-5			// -5dBm is 320uW the biggest I ever saw was 1uW
#endif


// -----------------------------------------------------------------------------
//		Full Duplex header - see also PEEK_FULLDUPLEX_HDR
// -----------------------------------------------------------------------------

struct FullDuplexPrivateHeader
{
	UInt32		Channel;			// Channel number
} WP_PACK_STRUCT;
typedef struct FullDuplexPrivateHeader FullDuplexPrivateHeader;

#ifndef	PEEK_INVALID_DUPLEX_CHANNEL
#define	PEEK_INVALID_DUPLEX_CHANNEL		0
#endif


// -----------------------------------------------------------------------------
//		WAN header - see also PEEK_WAN_HDR
// -----------------------------------------------------------------------------

struct WideAreaNetworkHeader
{
	UInt8		Direction;		// "To DCE" or "To DTE"
	UInt8		Protocol;		// FrameRelay, PPP, ...
} WP_PACK_STRUCT;
typedef struct WideAreaNetworkHeader WideAreaNetworkHeader;

// WAN traffic direction.
#define PEEK_WAN_DIRECTION_INVALID	0
#define PEEK_WAN_DIRECTION_DTE		1
#define PEEK_WAN_DIRECTION_DCE		2

// WAN protocol definitions.
#define PEEK_WAN_PROT_INVALID		0x00	// (Not a valid protocol)
#define PEEK_WAN_PROT_0800			0x08	// TCP/IP
#define PEEK_WAN_PROT_FRLY			0x23	// Frame Relay
#define PEEK_WAN_PROT_PPP			0x10	// PPP/LCP (also used for HDLC)
#define PEEK_WAN_PROT_X25			0x25	// X.25
#define PEEK_WAN_PROT_X25E			0x29	// X.25 mod 128
#define PEEK_WAN_PROT_Q931			0x31	// Q.931
#define PEEK_WAN_PROT_SNA			0x40	// SNA
#define PEEK_WAN_PROT_SS7			0x70	// SS7
#define PEEK_WAN_PROT_IPARS			0x50	// Ipars
#define PEEK_WAN_PROT_U200			0x60	// U200

// X.25 sub protocol definitions.
#define PEEK_WAN_SUB_PROT_CIDIN		0x27	//6927 zsdlc.com
#define PEEK_WAN_SUB_PROT_AX25		0x2A	//692A zsdlc.com


// All possible combination of media specific info.
typedef struct MediaSpecificHeaderAll
{
	MediaSpecificPrivateHeader  StdHeader;
	union
	{
		Wireless80211PrivateHeader	wireless;
		FullDuplexPrivateHeader		fullDuplex;
		WideAreaNetworkHeader		wan;
	} WP_PACK_STRUCT MediaInfo;
} WP_PACK_STRUCT MediaSpecificHeaderAll;

#define MAX_MEDIASPECIFICHEADER	sizeof(MediaSpecificHeaderAll)

#include "wppoppack.h"

#ifdef __cplusplus
}
#endif