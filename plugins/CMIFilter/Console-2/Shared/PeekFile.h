// ============================================================================
//	PeekFile.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1996-2000. All rights reserved.

#ifndef PEEKFILE_H
#define PEEKFILE_H

#include "AGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

// The header is always in big-endian format and always
// starts the first 50 bytes of the file.
//
// TimeDate, TimeStart, and TimeStop
//   Values in Mac OS timebase: seconds since midnight Jan 1 1904.
// MediaType
//   0 for Ethernet
//   1 for Token Ring (or assume Token Ring for .tpc files)
// MediaSubType
//   0 for "native" format
//   1 for 802.11b
//   2 for 802.11a

struct PeekFileHeader
{
	UInt8		fVersion;		// Packet file version.
	UInt8		fStatus;		// Packet file status.
	UInt32		fLength;		// File length in bytes.
	UInt32		fPacketCount;	// Number of packets.
	UInt32		fTimeDate;		// Capture session start date.
	UInt32		fTimeStart;		// Capture session start time.
	UInt32		fTimeStop;		// Capture session stop time.
	UInt32		fMediaType;		// Media type (Ethernet=0, Token Ring=1).
	UInt32		fMediaSubType;	// SubType (native=0, 802.11b=1, 802.11a=2).
	UInt32		fAppVers;		// Application version number: Maj.Min.Bug.Build.
	UInt32		fLinkSpeed;		// Link speed in bits/second.
	UInt32		fReserved[3];	// Reserved. Should be zero.
};
typedef struct PeekFileHeader PeekFileHeader;

#define kPacketFile_Version5			5
#define kPacketFile_Version6			6
#define kPacketFile_Version7			7
#define kPacketFile_Version8			8
#define kPacketFile_VersionTagged		9	// Only used internally.

#define kFileType_EtherPeekPacketFile	_T(".pkt")
#define kFileExt_EtherPeekPacketFile	_T("pkt")
#define kFileType_GigaPeekPacketFile	_T(".pkt")
#define kFileExt_GigaPeekPacketFile		_T("pkt")
#define kFileType_OmniPeekPacketFile	_T(".pkt")
#define kFileExt_OmniPeekPacketFile		_T("pkt")
#define kFileType_TokenPeekPacketFile	_T(".tpc")
#define kFileExt_TokenPeekPacketFile	_T("tpc")
#define kFileType_AiroPeekPacketFile	_T(".apc")
#define kFileExt_AiroPeekPacketFile		_T("apc")
#define kFileType_WanPeekPacketFile		_T(".wpc")
#define kFileExt_WanPeekPacketFile		_T("wpc")

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif
