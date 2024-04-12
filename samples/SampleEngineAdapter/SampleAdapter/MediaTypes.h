// =============================================================================
//	MediaTypes.h
// =============================================================================
//	Copyright (c) 1996-2006 WildPackets, Inc. All rights reserved.

/// \file
/// \brief MediaTypes header file.
///
/// This header file defines media types and related information.

#ifndef MEDIATYPES_H
#define MEDIATYPES_H

#ifdef _MANAGED
#define TYPE_ACCESS_ENUM public enum class
#else
#define TYPE_ACCESS_ENUM enum
#endif

#include "WPTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
	
// -----------------------------------------------------------------------------
//		TMediaType
// -----------------------------------------------------------------------------
/// \enum TMediaType
/// \brief Media type.
///
/// This enumeration defines the media types and which correspond to NDIS_MEDIUM.

TYPE_ACCESS_ENUM TMediaType
{
	kMediaType_802_3,			///< Ethernet (802.3)
	kMediaType_802_5,			///< Token Ring (802.5)
	kMediaType_Fddi,			///< FDDI
	kMediaType_Wan,				///< WAN
	kMediaType_LocalTalk,		///< LocalTalk
	kMediaType_Dix,				///< DEC/Intel/Xerox (DIX) Ethernet
	kMediaType_ArcnetRaw,		///< ARCNET (raw)
	kMediaType_Arcnet878_2,		///< ARCNET (878.2)
	kMediaType_Atm,				///< ATM
	kMediaType_WirelessWan,		///< Various wireless media
	kMediaType_Irda,			///< Infrared (IrDA)
	kMediaType_Bpc,				///< Broadcast Architecture
	kMediaType_CoWan,			///< Connection-oriented WAN
	kMediaType_1394,			///< IEEE 1394 (FireWire)
	kMediaType_Max				///< Not a real type, defined as an upper-bound.
};


// ----------------------------------------------------------------------------
//		TMediaSubType
// ----------------------------------------------------------------------------
/// \enum TMediaSubType
/// \brief Media sub type.
///
/// This enumeration defines the media sub types.

TYPE_ACCESS_ENUM TMediaSubType
{
	kMediaSubType_Native,			///< Native media specified by the media type.
	kMediaSubType_802_11_b,			///< Wireless LAN network using 802.11b.
	kMediaSubType_802_11_a,			///< Wireless LAN network using 802.11a.
	kMediaSubType_802_11_gen,		///< All possible 802.11 types, a, b, g, and combinations
	kMediaSubType_wan_ppp,			///< PPP over a T1 or an E1
	kMediaSubType_wan_frameRelay,	///< Frame Relay
	kMediaSubType_wan_x25,			///< X.25
	kMediaSubType_wan_x25e,			///< X.25mod128
	kMediaSubType_wan_ipars,		///< IPARS
	kMediaSubType_wan_u200,		    ///< U200
	kMediaSubType_wan_Q931,			///< Q.931
	kMediaSubType_Max				///< Not a real type, defined as an upper-bound.
};

#ifndef IS_ETHERNET
#define IS_ETHERNET(inMediaType, inMediaSubtype)	(inMediaType == kMediaType_802_3 && inMediaSubtype == kMediaSubType_Native)
#endif /*IS_ETHERNET*/

#ifndef IS_TOKENRING
#define IS_TOKENRING(inMediaType, inMediaSubtype)	(inMediaType == kMediaType_802_5 && inMediaSubtype == kMediaSubType_Native)
#endif /*IS_TOKENRING*/

#ifndef IS_WIRELESS
#define IS_WIRELESS(inSubType)	(((inSubType) >= kMediaSubType_802_11_b) && ((inSubType) <= kMediaSubType_802_11_gen))
#endif /*IS_WIRELESS*/

#ifndef IS_WAN
#define IS_WAN(inType, inSubType)	 (inType == kMediaType_CoWan || inType == kMediaType_Wan)
#endif /*IS_WAN*/

// -----------------------------------------------------------------------------
//		TMediaDomain
// -----------------------------------------------------------------------------
/// \enum TMediaDomain
/// \brief Media domain.
///
/// This enumeration defines the media domains.

enum TMediaDomain
{
	kMediaDomain_Null,			///< No domain
	kMediaDomain_FCC,			///< Federal Communications Commission (United States)
	kMediaDomain_MKK,			///< Musen Setsubi Kensa Kentei Kyokai (Japan)
	kMediaDomain_ETSI			///< European Telecommunications Standards Institute (Europe)
};


// -----------------------------------------------------------------------------
//		TMediaSpecClass
// -----------------------------------------------------------------------------
/// \enum TMediaSpecClass
/// \brief Media spec class.
///
/// This enumeration defines the media spec classes.

TYPE_ACCESS_ENUM TMediaSpecClass
{
	kMediaSpecClass_Null,		///< No class.
	kMediaSpecClass_Protocol,	///< Protocol media spec.
	kMediaSpecClass_Address,	///< Address media spec.
	kMediaSpecClass_Port		///< Port media spec.
};


// -----------------------------------------------------------------------------
//		TMediaSpecType
// -----------------------------------------------------------------------------
/// \enum TMediaSpecType
/// \brief Media spec type.
///
/// This enumeration defines the media spec types.

TYPE_ACCESS_ENUM TMediaSpecType
{
	kMediaSpecType_Null					= 0,		///< No media spec type
	kMediaSpecType_EthernetProto		= 1,		///< Ethernet protocol
	kMediaSpecType_LSAP					= 2,		///< LSAP
	kMediaSpecType_SNAP					= 3,		///< SNAP
	kMediaSpecType_LAP					= 4,		///< AppleTalk LAP
	kMediaSpecType_DDP					= 5,		///< AppleTalk DDP
	kMediaSpecType_MACCtl				= 6,		///< MAC control
	kMediaSpecType_ProtoSpec			= 9,		///< ProtoSpec
	kMediaSpecType_EthernetAddr			= 10,		///< Ethernet address
	kMediaSpecType_TokenRingAddr		= 11,		///< Token Ring address
	kMediaSpecType_LAPAddr				= 12,		///< AppleTalk LAP address
	kMediaSpecType_WirelessAddr			= 13,		///< Wireless address
	kMediaSpecType_AppleTalkAddr		= 20,		///< AppleTalk address
	kMediaSpecType_IPAddr				= 21,		///< IP address
	kMediaSpecType_DECnetAddr			= 22,		///< DECnet address
	kMediaSpecType_OtherAddr			= 23,		///< Other address
	kMediaSpecType_IPv6Addr				= 24,		///< IPv6 address
	kMediaSpecType_IPXAddr				= 25,		///< IPX address
	kMediaSpecType_Error				= 32,		///< Error
	kMediaSpecType_ATPort				= 33,		///< AppleTalk port
	kMediaSpecType_IPPort				= 34,		///< IP port
	kMediaSpecType_NWPort				= 35,		///< NetWare port
	kMediaSpecType_TCPPortPair			= 36,		///< TCP port port
	kMediaSpecType_WAN_PPP_Proto		= 37,		///< WAN PPP protocol
	kMediaSpecType_WAN_FrameRelay_Proto	= 38,		///< WAN FrameRelay protocol
	kMediaSpecType_WAN_X25_Proto		= 39,		///< WAN X.25 protocol
	kMediaSpecType_WAN_X25e_Proto		= 40,		///< WAN X.25mod128 protocol
	kMediaSpecType_WAN_Ipars_Proto		= 41,		///< WAN IPARS protocol
	kMediaSpecType_WAN_U200_Proto		= 42,		///< WAN U200 protocol
	kMediaSpecType_WAN_DLCIAddr			= 43,		///< WAN DLCI address
	kMediaSpecType_WAN_Q931_Proto		= 44		///< WAN Q931 protocol
};


// -----------------------------------------------------------------------------
//		TMediaSpec
// -----------------------------------------------------------------------------
/// \struct TMediaSpec
/// \brief Media spec data and type info.
///
/// This structure defines the media spec.

#include "wppushpack.h"
struct TMediaSpec
{
	UInt8	fClass;			///< Media spec class
	UInt8	fType;			///< Media spec type
	UInt8	fFiller1;		///< Reserved
	UInt8	fFiller2;		///< Reserved
	UInt32	fMask;			///< Mask
	UInt8	fData[24];		///< Data
} WP_PACK_STRUCT;
typedef struct TMediaSpec TMediaSpec;
#include "wppoppack.h"

#ifdef __cplusplus
}
#endif

#endif /*MEDIATYPES_H*/
