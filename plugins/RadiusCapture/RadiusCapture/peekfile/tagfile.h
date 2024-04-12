// =============================================================================
//	tagfile.h
// =============================================================================
//	Copyright (c) 2003-2008 WildPackets, Inc. All rights reserved.

#ifndef TAGFILE_H
#define TAGFILE_H

#include "WPTypes.h"

#include "wppushpack.h"

namespace TaggedFile
{
	// each section of the file is a block that start with a block type (4 bytes)
	typedef UInt32	BlockType;

	// the entire block header for each block
	typedef struct 
	{	
		BlockType	nType;		// 4 byte type field
		UInt32		nLength;	// length of block, 0 => entire rest of file
		UInt32		nFlags;		// 1st byte is CompressionType, 2nd byte is FormatType, 3 & 4 are reserved
	} WP_PACK_STRUCT BLOCK_HEADER;

	typedef enum
	{
		Uncompressed,			// not compressed
		ZLIB					// zlib style compression
	} CompressionType;

	typedef enum
	{
		BinaryFormat,			// special binary format
		TextFormat,				// UNICODE text block
		XMLFormat,				// XML encoded block
		HTMLFormat				// HTML encoded block
	} FormatType;

	// defined block types
	enum
	{ 
		kCaptureFileVersionBlock	= 0x7f766572,	// 0x7f + 'ver' (XML format)
		kCaptureFileSessionBlock	= 0x73657373,	// capture session block (XML format)
		kCaptureFilePacketsBlock	= 0x706B7473,	// packet data block (binary format)
		kCaptureFileGpsBlock		= 0x67707369,	// gps information (binary format)
		kCaptureFileIdBlock			= 0x63706964	// packet capture id block (XML format)
	};

	// Packet attributes for the packet block
	typedef enum
	{
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

	typedef struct
	{
		UInt16	fieldType;						// PacketAttrib 
		UInt32	fieldValue;						// data for corresponding attribute
	} WP_PACK_STRUCT PacketAttribute;
}

// these are some useful XML attributes that are used in the version and session blocks

#define kCaptureFileVersionBlock_Root				_T("VersionInfo")
#define kCaptureFileVersionBlock_AppVersion			_T("AppVersion")
#define kCaptureFileVersionBlock_ProductVersion		_T("ProdVersion")
#define kCaptureFileVersionBlock_FileVersion		_T("FileVersion")

#define kCaptureFileSessionBlock_Root				_T("Session")
#define kCaptureFileSessionBlock_RawTime			_T("RawTime")		// time_t units
#define kCaptureFileSessionBlock_Time				_T("Time")
#define kCaptureFileSessionBlock_TimeZoneBias		_T("TimeZoneBias")
#define kCaptureFileSessionBlock_MediaType			_T("MediaType")
#define kCaptureFileSessionBlock_MediaSubType		_T("MediaSubType")
#define kCaptureFileSessionBlock_LinkSpeed			_T("LinkSpeed")		// bps
#define kCaptureFileSessionBlock_PacketCount		_T("PacketCount")
#define kCaptureFileSessionBlock_Comment			_T("Comment")
#define kCaptureFileSessionBlock_SessionStartTime	_T("SessionStartTime")
#define kCaptureFileSessionBlock_SessionEndTime		_T("SessionEndTime")
#define kCaptureFileSessionBlock_AdapterName		_T("AdapterName")
#define kCaptureFileSessionBlock_AdapterAddr		_T("AdapterAddr")
#define kCaptureFileSessionBlock_CaptureName		_T("CaptureName")
#define kCaptureFileSessionBlock_CaptureID			_T("CaptureID")
#define kCaptureFileSessionBlock_Owner				_T("Owner")
#define kCaptureFileSessionBlock_FileIndex			_T("FileIndex")
#define kCaptureFileSessionBlock_Host				_T("Host")
#define kCaptureFileSessionBlock_EngineName			_T("EngineName")
#define kCaptureFileSessionBlock_EngineAddr			_T("EngineAddr")
#define kCaptureFileSessionBlock_MediaDomain		_T("Domain")
#define kCaptureFileSessionBlock_DataRates			_T("DataRates")
#define kCaptureFileSessionBlock_Rate				_T("Rate")
#define kCaptureFileSessionBlock_ChannelList		_T("ChannelList")
#define kCaptureFileSessionBlock_Channel			_T("Channel")		// for enumerated channels (non-wireless), this is the channel number
#define kCaptureFileSessionBlock_Channel_Freq		_T("Frequency")		// for wireless channels, these 3 things indicate the channel details
#define kCaptureFileSessionBlock_Channel_Band		_T("Band")
#define kCaptureFileSessionBlock_Channel_Number		_T("Number")

#define kCaptureFileIdBlock_Root					_T("CaptureId")
#define kCaptureFileIdBlock_Id						_T("Id")
#define kCaptureFileIdBlock_Index					_T("Index")

#include "wppoppack.h"

#endif /* TAGFILE_H */
