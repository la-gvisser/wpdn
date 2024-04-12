// =============================================================================
//	tagfile.h
// =============================================================================
//	Copyright (c) 2003-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"

#include "wppushpack.h"

namespace TaggedFile
{
	// each section of the file is a block that start with a block type (4 bytes)
	typedef UInt32	BlockType;

	// the entire block header for each block
	struct BLOCK_HEADER {	
		BlockType	nType;		// 4 byte type field
		UInt32		nLength;	// length of block, 0 => entire rest of file
		UInt32		nFlags;		// 1st byte is CompressionType, 2nd byte is FormatType, 3 & 4 are reserved
	} WP_PACK_STRUCT;
	typedef struct BLOCK_HEADER	BLOCK_HEADER;

	typedef enum {
		Uncompressed,			// not compressed
		ZLIB					// zlib style compression
	} CompressionType;

	typedef enum {
		BinaryFormat,			// special binary format
		TextFormat,				// UNICODE text block
		XMLFormat,				// XML encoded block
		HTMLFormat				// HTML encoded block
	} FormatType;

	// defined block types
	enum { 
		kCaptureFileVersionBlock	= 0x7f766572,	// 0x7f + 'ver' (XML format)
		kCaptureFileSessionBlock	= 0x73657373,	// capture session block (XML format)
		kCaptureFilePacketsBlock	= 0x706B7473,	// packet data block (binary format)
		kCaptureFileGpsBlock		= 0x67707369,	// gps information (binary format)
		kCaptureFileIdBlock			= 0x63706964	// packet capture id block (XML format)
	};

	// Packet attributes for the packet block
	typedef enum {
		Attrib_ActualLength,					// length of the packet on the wire
		Attrib_TimeStampLo,						// low 32 bits of the UTC timestamp, in nanoseconds from 1/1/1601
		Attrib_TimeStampHi,						// high 32 bits of timestamp
		Attrib_FlagsStatus,						// high 16 bits are PEEK status bits, low 16 are PEEK flags
		Attrib_ChannelNumber,					// channel number
		Attrib_DataRate,						// wireless data rate in 0.5Mbs
		Attrib_SignalStrength,					// signal strength, 0-100%
		Attrib_SignaldBm,						// signal dBm, (signed)
		Attrib_NoiseStrength,					// noise strength, 0-100%
		Attrib_NoisedBm,						// noise dBm (signed)
		Attrib_MediaSpecType,					// type of media specific info that follows
		Attrib_Protocol,						// WAN Protocol (X.25, Frame Relay, PPP,....)
		Attrib_Direction,						// WAN Direction (to DTE or to DCE)
		Attrib_ChannelFreq,						// channel frequency (wireless)
		Attrib_ChannelBand,						// channel band (a, b, turbo, etc.)
		Attrib_SignaldBm1,						// signal dBm, (signed)
		Attrib_SignaldBm2,						// signal dBm, (signed)
		Attrib_SignaldBm3,						// signal dBm, (signed)
		Attrib_NoisedBm1,						// noise dBm (signed)
		Attrib_NoisedBm2,						// noise dBm (signed)
		Attrib_NoisedBm3,						// noise dBm (signed)
		Attrib_FlagsN,							// 802.11N flags
		Attrib_PacketDataLength = 0xffff		// length of the packet data (always follows this attribute)
	} PacketAttrib;

	struct PacketAttribute	{
		UInt16	fieldType;						// PacketAttrib 
		UInt32	fieldValue;						// data for corresponding attribute
	} WP_PACK_STRUCT;
	typedef struct PacketAttribute	PacketAttribute;
}

// these are some useful XML attributes that are used in the version and session blocks

#define kCaptureFileVersionBlock_Root				L"VersionInfo"
#define kCaptureFileVersionBlock_AppVersion			L"AppVersion"
#define kCaptureFileVersionBlock_ProductVersion		L"ProdVersion"
#define kCaptureFileVersionBlock_FileVersion		L"FileVersion"

#define kCaptureFileSessionBlock_Root				L"Session"
#define kCaptureFileSessionBlock_RawTime			L"RawTime"		// time_t units
#define kCaptureFileSessionBlock_Time				L"Time"
#define kCaptureFileSessionBlock_TimeZoneBias		L"TimeZoneBias"
#define kCaptureFileSessionBlock_MediaType			L"MediaType"
#define kCaptureFileSessionBlock_MediaSubType		L"MediaSubType"
#define kCaptureFileSessionBlock_LinkSpeed			L"LinkSpeed"		// bps
#define kCaptureFileSessionBlock_PacketCount		L"PacketCount"
#define kCaptureFileSessionBlock_Comment			L"Comment"
#define kCaptureFileSessionBlock_SessionStartTime	L"SessionStartTime"
#define kCaptureFileSessionBlock_SessionEndTime		L"SessionEndTime"
#define kCaptureFileSessionBlock_AdapterName		L"AdapterName"
#define kCaptureFileSessionBlock_AdapterAddr		L"AdapterAddr"
#define kCaptureFileSessionBlock_Adapter			L"Adapter"
#define kCaptureFileSessionBlock_CaptureName		L"CaptureName"
#define kCaptureFileSessionBlock_CaptureID			L"CaptureID"
#define kCaptureFileSessionBlock_Owner				L"Owner"
#define kCaptureFileSessionBlock_FileIndex			L"FileIndex"
#define kCaptureFileSessionBlock_Host				L"Host"
#define kCaptureFileSessionBlock_EngineName			L"EngineName"
#define kCaptureFileSessionBlock_EngineAddr			L"EngineAddr"
#define kCaptureFileSessionBlock_MediaDomain		L"Domain"
#define kCaptureFileSessionBlock_DataRates			L"DataRates"
#define kCaptureFileSessionBlock_Rate				L"Rate"
#define kCaptureFileSessionBlock_ChannelList		L"ChannelList"
#define kCaptureFileSessionBlock_Channel			L"Channel"		// for enumerated channels (non-wireless), this is the channel number
#define kCaptureFileSessionBlock_Channel_Freq		L"Frequency"		// for wireless channels, these 3 things indicate the channel details
#define kCaptureFileSessionBlock_Channel_Band		L"Band"
#define kCaptureFileSessionBlock_Channel_Number		L"Number"

#define kCaptureFileIdBlock_Root					L"CaptureId"
#define kCaptureFileIdBlock_Id						L"Id"
#define kCaptureFileIdBlock_Index					L"Index"

#include "wppoppack.h"
