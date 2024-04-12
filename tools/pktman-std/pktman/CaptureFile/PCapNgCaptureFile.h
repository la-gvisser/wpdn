// ============================================================================
//	PCapNgCaptureFile.h
//		interface for CPCapNgCaptureFile class.
// ============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#include "CaptureFile.h"
#include <filesystem>

using std::filesystem::path;
using std::string;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	PCap NG Capture File
//
//	A PCap NG Capture File support reading and writing of PCap NG format files.
//	PCap files have the extension: pcapng.
//
//	PCapNg Capture File is part of the Capture File module.
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

using namespace CaptureFile;

namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

#include "wppushpack.h"

struct BlockHeader {
	UInt32	type;
	UInt32	length;
} WP_PACK_STRUCT;

struct BlockTrailer {
	UInt32	length;
} WP_PACK_STRUCT;

struct SectionBlockHeader {
	UInt32		magic;
	UInt16		major;			// Major version number.
	UInt16		minor;			// Minor version number.
	UInt64		sectionlength;
} WP_PACK_STRUCT;

struct InterfaceDescriptionBlockHeader {
	UInt16		linktype;
	UInt16		reserved;
	UInt32		snaplength;
} WP_PACK_STRUCT;

struct SimplePacketBlockHeader {
	UInt32		packetlength;
} WP_PACK_STRUCT;

struct NameResolutionBlockHeader {
	UInt16		recordtype;
	UInt16		recordvaluelength;
	UInt8		recordvalue[4];		// padded to 32-bits
} WP_PACK_STRUCT;

struct InterfaceStatisticsBlockHeader {
	UInt32		interfaceid;
	UInt32		timestamphigh;
	UInt32		timestamplow;
} WP_PACK_STRUCT;

struct EnhancedPacketBlockHeader {
	UInt32		interfaceid;
	UInt32		timestamphigh;
	UInt32		timestamplow;
	UInt32		length;
	UInt32		packetlength;
} WP_PACK_STRUCT;

struct CustomBlockHeader {
	UInt32		pen;			// Private Enterprise Number
	UInt8		data[4];
} WP_PACK_STRUCT;

struct AnyBlockHeader {
	BlockHeader	header;
	union {
		SectionBlockHeader				section;
		InterfaceDescriptionBlockHeader	description;
		SimplePacketBlockHeader			simplepacket;
		NameResolutionBlockHeader		nameresolution;
		InterfaceStatisticsBlockHeader	statistics;
		EnhancedPacketBlockHeader		packet;
		CustomBlockHeader				custom;
		UInt8							data[4];
	};
};

struct EnhancedPacketHeader {
	BlockHeader					header;
	EnhancedPacketBlockHeader	packet;
};

struct OptionHeader {
	UInt16	type;
	UInt16	length;
} WP_PACK_STRUCT;

struct CustomOptionHeader {
	UInt32	pen;
} WP_PACK_STRUCT;

struct PCapNgVersion {
	UInt16	major;
	UInt16	minor;
};

#if (0)
// See http://madwifi.org/wiki/DevDocs/RadiotapHeader.
struct ieee80211_radiotap_header {
	UInt8	it_version;	// set to 0
	UInt8	it_pad;
	UInt16	it_len;		// entire length
	UInt32	it_present;	// fields present
} WP_PACK_STRUCT;
#endif

#include "wppoppack.h"

const UInt32	PCAPNG_SECTION_BLOCK				= 0x0A0D0D0A;	// File must begin with Section Block.
const UInt32	PCAPNG_INTERFACE_DESCRIPTION_BLOCK	= 0x00000001;
const UInt32	PCAPNG_SIMPLE_PACKET_BLOCK			= 0x00000003;
const UInt32	PCAPNG_NAME_RESOLUTION_BLOCK		= 0x00000004;
const UInt32	PCAPNG_INTERFACE_STATISTICS_BLOCK	= 0x00000005;
const UInt32	PCAPNG_ENHANCED_PACKET_BLOCK		= 0x00000006;
const UInt32	PCAPNG_CUSTOM_BLOCK_COPY			= 0x00000BAD;	// Custom Blocks that may be copied to new files.
const UInt32	PCAPNG_CUSTOM_BLOCK_NO_COPY			= 0x40000BAD;	// Custom Blocks that should not be copied to new files.

const UInt32	PCAPNG_MAGIC						= 0x1A2B3C4D;
const UInt32	PCAPNG_MAGIC_SWAPPED				= 0x4D3C2B1A;

const UInt16	PCAPNG_OPTION_END					= 0x0000;
const UInt16	PCAPNG_OPTION_COMMENT				= 0x0001;
const UInt16	PCAPNG_OPTION_NAME					= 0x0002;
const UInt16	PCAPNG_OPTION_DESCRIPTION			= 0x0003;
const UInt16	PCAPNG_OPTION_IPV4ADDRESS			= 0x0004;
const UInt16	PCAPNG_OPTION_IPV6ADDRESS			= 0x0005;
const UInt16	PCAPNG_OPTION_ETHERNETADDRESS		= 0x0006;
const UInt16	PCAPNG_OPTION_EUIADDRESS			= 0x0007;
const UInt16	PCAPNG_OPTION_SPEED					= 0x0008;
const UInt16	PCAPNG_OPTION_TSRESOL				= 0x0009;
const UInt16	PCAPNG_OPTION_TZONE					= 0x000A;
const UInt16	PCAPNG_OPTION_FILTER				= 0x000B;
const UInt16	PCAPNG_OPTION_OS					= 0x000C;
const UInt16	PCAPNG_OPTION_FCSLEN				= 0x000D;
const UInt16	PCAPNG_OPTION_TSOFFSET				= 0x000E;
const UInt16	PCAPNG_OPTION_CUSTOM_0				= 0x0BAC;
const UInt16	PCAPNG_OPTION_CUSTOM_1				= 0x0BAD;
const UInt16	PCAPNG_OPTION_CUSTOM_2				= 0x4BAC;
const UInt16	PCAPNG_OPTION_CUSTOM_3				= 0x4BAD;

// Peek Defaults
#define DEFAULT_PCAPNG_MAGIC		PCAPNG_MAGIC
#define DEFAULT_PCAPNG_MAJOR		1
#define DEFAULT_PCAPNG_MINOR		0
#define DEFAULT_PCAPNG_ZONE			0
#define DEFAULT_PCAPNG_SIGFLAGS		0
#define DEFAULT_PCAPNG_SNAPLEN		65535
#define DEFAULT_PCAPNG_LINKTYPE		LINKTYPE_ETHERNET

const PCapNgVersion		kPCapNgVersion1{ DEFAULT_PCAPNG_MAJOR, DEFAULT_PCAPNG_MINOR };


// ============================================================================
//		CPCapNgCapture
// ============================================================================

class CPCapNgCapture
{
protected:
	tMediaType		m_MediaType;
	PCapNgVersion	m_Version;
	SInt32			m_nZone;
	UInt32			m_nSignificantFigures;
	UInt32			m_nSnapLength;
	UInt16			m_nLinkType;

public:
	;		CPCapNgCapture( tMediaType inType )
		:	m_MediaType( inType )
		,	m_Version( { 0, 0 } )
		,	m_nZone( DEFAULT_PCAPNG_ZONE )
		,	m_nSignificantFigures( DEFAULT_PCAPNG_SIGFLAGS )
		,	m_nSnapLength( DEFAULT_PCAPNG_SNAPLEN )
		,	m_nLinkType( DEFAULT_PCAPNG_LINKTYPE )
	{}
	virtual	~CPCapNgCapture() {}

	virtual void	CopyMetaData( const CFileMetaData& inData ) {
		(void)inData;
	}

	virtual tMediaType	GetMediaType() const { return m_MediaType; }

	static bool	IsKind( const string& inKind ) {
		if (inKind.compare( "pcapng" ) == 0) return true;
		return false;
	}
	bool		IsVersion1() const {
		return ((m_Version.major == kPCapNgVersion1.major) && (m_Version.minor == kPCapNgVersion1.minor));
	}
	bool		IsVersion1( const PCapNgVersion& inVersion ) const {
		return ((inVersion.major == kPCapNgVersion1.major) && (inVersion.minor == kPCapNgVersion1.minor));
	}

	void	SetLinkType( UInt16 inType );
	void	SetMediaType( const tMediaType& inType );
	void	SetSnapLength( UInt32 inLength ) { m_nSnapLength = inLength; }
};


// ============================================================================
//		CPCapNgCaptureReader
// ============================================================================

class CPCapNgCaptureReader
	:	public CCaptureReader
	,	public CPCapNgCapture
{
public:
	struct TPCapNgPacketHeader {
		UInt64	m_nTimeStamp;
		UInt32	m_nPacketLength;
		UInt32	m_nSliceLength;

		UInt32 GetDataLength() {
			return (m_nSliceLength == 0) ? m_nPacketLength : m_nSliceLength;
		}
		bool IsSliced() const {
			return (m_nSliceLength != 0);
		}
	};

protected:
	bool			m_bSwapped;
	UInt64			m_nTimeStampMultiplier;
	UInt64			m_nSectionLength;

protected:
	bool	BeginNewSection( const AnyBlockHeader& inSectionHeader, UInt64 inRemaining,
		UInt64& outFirstPacketOffset );
	bool	Parse();

	bool	ReadBlockHeader( AnyBlockHeader& outBlock, UInt64& outRemaining );
	bool	ReadPacketHeader( TPCapNgPacketHeader& outHeader, UInt32& outOptions,
								UInt32& outError );

public:
	;		CPCapNgCaptureReader()
			:	CCaptureReader( CaptureFile::CF_FORMAT_PCAP_NG, CaptureFile::CF_ACCESS_UNKNOWN )
			,	CPCapNgCapture( tMediaType() )
		,	m_bSwapped( false )
		,	m_nTimeStampMultiplier( 1 )
	{}
	;		CPCapNgCaptureReader( const path& inPath )
			:	CCaptureReader( inPath, CaptureFile::CF_FORMAT_PCAP_NG,
					CaptureFile::CF_ACCESS_UNKNOWN )
			,	CPCapNgCapture( tMediaType() )
		,	m_bSwapped( false )
		,	m_nTimeStampMultiplier( 1 )
	{}
	virtual	~CPCapNgCaptureReader() { Close(); }

	virtual void	CopyMetaData( const CFileMetaData& inData ) {
		CPCapNgCapture::CopyMetaData( inData );
	}
	void			Close() { CCaptureReader::Close(); }

	virtual tMediaType	GetMediaType() const {
		return CPCapNgCapture::GetMediaType();
	}
	UInt64				GetTimeStampMultiplier() const {
		return m_nTimeStampMultiplier;
	}

	static bool	IsKind( const string& inKind ) {
		return CPCapNgCapture::IsKind( inKind );
	}
	bool		IsSwapped() const { return m_bSwapped; }

	bool	Open( const path& inPath );

	virtual UInt64	MoveTo( UInt64 inPosition ) {		// Absolute
		return CCaptureReader::MoveTo( inPosition );
	}
	virtual UInt64	Seek( SInt64 inOffset ) {			// Relative
		return CCaptureReader::Seek( inOffset );
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32& outError );
	virtual bool	ReadFilePackets( UInt32 inCount, CFilePacketList& outPackets, UInt32& outError );

	void	SetSectionLength( UInt64 inLength ) { m_nSectionLength = inLength; }
	void	SetSwapped( bool inEnabled = true ) { m_bSwapped = inEnabled; }
	void	SetTimeStampMultiplier( UInt64 inMultiplier ) { m_nTimeStampMultiplier = inMultiplier; }
	void	SetVersion( const PCapNgVersion& inVersion ) { m_Version = inVersion; }
};


// ============================================================================
//		CPCapNgCaptureWriter
// ============================================================================

class CPCapNgCaptureWriter
	:	public CCaptureWriter
	,	public CPCapNgCapture
{
public:
	;		CPCapNgCaptureWriter( CF_ACCESS_TYPE inAccess, tMediaType inType )
		:	CCaptureWriter( CaptureFile::CF_FORMAT_PCAP_NG, inAccess )
		,	CPCapNgCapture( inType )
	{
		SetMediaType( inType );
	}
	virtual	~CPCapNgCaptureWriter() { CCaptureWriter::Close(); }

	virtual void	CopyMetaData( const CFileMetaData& inData ) {
		CPCapNgCapture::CopyMetaData( inData );
	}

	virtual tMediaType	GetMediaType() const {
		return CPCapNgCapture::GetMediaType();
	}

	virtual void	Init();
	static bool		IsKind( const string& inKind ) {
		return CPCapNgCapture::IsKind( inKind );
	}

	bool	Open( const path& inPath );

	virtual UInt64	MoveTo( UInt64 inPosition ) {		// Absolute
		return CCaptureWriter::MoveTo( inPosition );
	}
	virtual UInt64	Seek( SInt64 inOffset ) {			// Relative
		return CCaptureWriter::Seek( inOffset );
	}

	virtual bool	WriteFilePacket( const CFilePacket& inPacket );
};

}	// namespace CaptureFile
