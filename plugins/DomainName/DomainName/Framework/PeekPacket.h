// =============================================================================
//	PeekPacket.h
// =============================================================================
//	Copyright (c) 2000-2011 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1998-2000 AG Group, Inc. All rights reserved.

/// \file
/// \brief PeekPacket header file.
///
/// This header file is used to define PeekPacket and related information.

#ifndef PEEKPACKET_H
#define PEEKPACKET_H

#include "WPTypes.h"
#include "PacketHeaders.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "wppushpack.h"

/// \enum PeekPacketFlags
/// \brief Packet flags bits.
///
/// These are the defined values for the packet flags bits.
enum PeekPacketFlags
{
	kPacketFlag_Control				= 0x01,		///< Control vs. data packet.
	kPacketFlag_CRC					= 0x02,		///< Checksum error.
	kPacketFlag_Frame				= 0x04,		///< Frame error.
	kPacketFlag_RouteInfo			= 0x08,		///< Has routing info (Token Ring).
	kPacketFlag_Oversize			= 0x10,		///< Oversize error.
	kPacketFlag_Runt				= 0x20,		///< Runt error.
	kPacketFlag_Trigger				= 0x40,		///< Trigger packet.
	kPacketFlag_SNAP				= 0x80,		///< SNAP packet.
	kPacketFlag_DroppedPackets		= 0x100		///< One or more packets were dropped prior to this packet.
};

/// \enum PeekPacketStatus
/// \brief Packet status bits.
///
/// These are the defined values for the packet status bits.
enum PeekPacketStatus
{
	kPacketStatus_Selected			= 0x01,		///< Selected.
	kPacketStatus_Truncated			= 0x02,		///< Truncated packet.
	kPacketStatus_Encrypted			= 0x04,		///< Originally encrypted (e.g., WEP).
	kPacketStatus_DecryptionError	= 0x08,		///< Decryption error (e.g., WEP ICV).
	kPacketStatus_ContainsNote		= 0x10,		///< ApplePeek packet.
	kPacketStatus_Sliced			= 0x20,		///< Sliced packet.
	kPacketStatus_ShortPreamble		= 0x40,		///< Short preamble.
	kPacketStatus_Hidden			= 0x80		///< Hidden.
};

// This is version 5 and 6.
struct PeekPacket6
{
	UInt16		fPacketLength;	// Total length of packet.
	UInt16		fSliceLength;	// Sliced length of packet.
	UInt8		fFlags;			// CRC, frame, runt, ...
	UInt8		fStatus;		// Slicing, ...
	UInt32		fTimeStamp;		// Session relative timestamp in milliseconds.
	UInt16		fDestNum;		// Index in name table (do not use).
	UInt16		fSrcNum;		// Index in name table (do not use).
	UInt16		fProtoNum;		// Index in name table (do not use).
	SInt8		fProtoStr[8];	// String for protocol.
	UInt16		fFilterNum;		// 1-based index in filters (do not use).
} WP_PACK_STRUCT;
typedef struct PeekPacket6 PeekPacket6;

// This is the version 7.
struct PeekPacket7
{
	UInt16		fProtoSpec;		// ProtoSpec ID.
	UInt16		fPacketLength;	// Total length of packet.
	UInt16		fSliceLength;	// Sliced length of packet.
	UInt8		fFlags;			// CRC, frame, runt, ...
	UInt8		fStatus;		// Slicing, ...
	UInt64		fTimeStamp;		// Timestamp in microseconds.
} WP_PACK_STRUCT;
typedef struct PeekPacket7 PeekPacket7;

// This is the version 8 (never released as a packet file except into test).
struct PeekPacket8
{
	UInt32		fProtoSpec;		// ProtoSpec ID.
	UInt16		fPacketLength;	// Total length of packet.
	UInt16		fSliceLength;	// Sliced length of packet.
	UInt8		fFlags;			// CRC, frame, runt, ...
	UInt8		fStatus;		// Slicing, ...
	UInt64		fTimeStamp;		// Timestamp in nanoseconds UTC.
} WP_PACK_STRUCT;
typedef struct PeekPacket8 PeekPacket8;

// This is the version 9.
struct PeekPacket9
{
	UInt32		fProtoSpec;				// ProtoSpec ID.
	UInt16		fPacketLength;			// Total length of packet.
	UInt16		fSliceLength;			// Sliced length of packet.
	UInt8		fFlags;					// CRC, frame, runt, ...
	UInt8		fStatus;				// Slicing, ...
	UInt64		fTimeStamp;				// Timestamp in nanoseconds UTC.
	UInt8*		fMediaSpecInfoBlock;	// MediaSpecificPrivateHeader + (wireless or full duplex or wan or ... )
} WP_PACK_STRUCT;
typedef struct PeekPacket9 PeekPacket9;

// This is the version 10.
struct PeekPacket10
{
	UInt64		fTimeStamp;				// Timestamp in nanoseconds UTC.
	UInt32		fProtoSpec;				// ProtoSpec ID.
	UInt32		fFlags;					// CRC, frame, runt, ...
	UInt32		fStatus;				// Slicing, ...
	UInt16		fPacketLength;			// Total length of packet.
	UInt16		fSliceLength;			// Sliced length of packet.
	UInt8*		fMediaSpecInfoBlock;	// MediaSpecificPrivateHeader + (wireless or full duplex or wan or ... )
} WP_PACK_STRUCT;
typedef struct PeekPacket10 PeekPacket10;

// Define the current versions of things.
typedef struct PeekPacket10 PeekPacket;

#include "wppoppack.h"

#ifdef __cplusplus
}
#endif

#endif /* PEEKPACKET_H */