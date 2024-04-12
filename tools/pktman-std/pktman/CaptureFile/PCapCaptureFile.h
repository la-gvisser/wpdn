// ============================================================================
//	PCapCaptureFile.h
//		interface for CPCapCaptureFile class.
// ============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#include "CaptureFile.h"
#include <cassert>
#include <filesystem>

using std::filesystem::path;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	PCap Capture File
//
//	A PCap Capture File support reading and writing of PCap format files.
//	PCap files have the extension: pcap or dmp.
//
//	PCap Capture File is part of the Capture File module.
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

using namespace CaptureFile;

namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

#include "wppushpack.h"

struct TcpDumpFileHeader {
	UInt32	magic;
	UInt16	major;		// Major version number.
	UInt16	minor;		// Minor version number.
	SInt32	zone;		// GMT to local correction.
	UInt32	sigfigs;	// Accuracy of timestamps.
	UInt32	snaplen;	// Max length of captured packets, in octets.
	UInt32	linktype;	// Data link type.
} WP_PACK_STRUCT;

// Note: Current version is 2.4 (major.minor)
// But Current file version is TCPDUMP_VERSION_1
struct TcpDumpPacketHeader {
	UInt32	tshi;		// Timestamp seconds.
	UInt32	tslo;		// Timestamp microseconds.
	UInt32	caplen;		// Captured packet length.
	UInt32	len;		// Original packet length.
} WP_PACK_STRUCT;

// Note: this modification is courtesy of Alexey Kuznetsov,
// see (404) <http://ftp.sunet.se/pub/os/Linux/ip-routing/lbl-tools/>.
// This is TCPDUMP_VERSION_2
struct TcpDumpPacketHeader2 {
	TcpDumpPacketHeader	hdr;
	UInt32				ifindex;	// Index, in *capturing* machine's interfaces.
	UInt16				protocol;	// Ethernet packet type.
	UInt8				pkt_type;	// Broadcast/multicast/etc. indication.
	UInt8				pad;		// Padding.
} WP_PACK_STRUCT;

// See http://madwifi.org/wiki/DevDocs/RadiotapHeader.
struct ieee80211_radiotap_header {
	UInt8	it_version;	// set to 0
	UInt8	it_pad;
	UInt16	it_len;		// entire length
	UInt32	it_present;	// fields present
} WP_PACK_STRUCT;

#include "wppoppack.h"

const UInt32 TCPDUMP_MAGIC				= 0xA1B2C3D4;
const UInt32 TCPDUMP_MAGIC_SWAPPED		= 0xD4C3B2A1;
const UInt32 TCPDUMP_MAGIC_1N			= 0xA1B23C4D;
const UInt32 TCPDUMP_MAGIC_1N_SWAPPED	= 0x4D3CB2A1;
const UInt32 TCPDUMP_MAGIC_2			= 0xA1B2CD34;
const UInt32 TCPDUMP_MAGIC_2_SWAPPED	= 0x34CDB2A1;

#define TCPDUMP_VERSION_1		1
#define TCPDUMP_VERSION_2		2

// Peek Defaults
#define DEFAULT_TCPDUMP_MAGIC		TCPDUMP_MAGIC
#define DEFAULT_TCPDUMP_MAJOR		2
#define DEFAULT_TCPDUMP_MINOR		4
#define DEFAULT_TCPDUMP_ZONE		0
#define DEFAULT_TCPDUMP_SIGFLAGS	0
#define DEFAULT_TCPDUMP_SNAPLEN		65535
#define DEFAULT_TCPDUMP_LINKTYPE	0


// ============================================================================
//		CPCapCapture
// ============================================================================

class CPCapCapture
{
protected:
	tMediaType	m_MediaType;
	UInt16		m_nMajorVersion;
	UInt16		m_nMinorVersion;
	SInt32		m_nZone;
	UInt32		m_nSignificantFigures;
	UInt32		m_nSnapLength;
	UInt32		m_nLinkType;

public:
	;		CPCapCapture( tMediaType inType = kMediaType_Ethernet )
		:	m_MediaType( inType )
		,	m_nMajorVersion( DEFAULT_TCPDUMP_MAJOR )
		,	m_nMinorVersion( DEFAULT_TCPDUMP_MINOR )
		,	m_nZone( DEFAULT_TCPDUMP_ZONE )
		,	m_nSignificantFigures( DEFAULT_TCPDUMP_SIGFLAGS )
		,	m_nSnapLength( DEFAULT_TCPDUMP_SNAPLEN )
		,	m_nLinkType( DEFAULT_TCPDUMP_LINKTYPE )
	{}
	virtual	~CPCapCapture() {}

	virtual void	CopyMetaData( const CFileMetaData& inData ) {
		(void)inData;
	}

	virtual tMediaType	GetMediaType() const { return m_MediaType; }

	static bool	IsKind( const std::string& inKind ) {
		if (inKind.compare( "pcap" ) == 0) return true;
		if (inKind.compare( "dmp" ) == 0) return true;
		return false;
	}

	void	SetLinkType( const tMediaType inType );
};


// ============================================================================
//		CPCapCaptureReader
// ============================================================================

class CPCapCaptureReader
	:	public CCaptureReader
	,	public CPCapCapture
{
public:
	struct TPCapPacketHeader {
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
	int				m_nVersion;
	bool			m_bSwapped;
	UInt64			m_nTimeStampMultiplier;
	UInt32			m_nPacketHeaderLength;

protected:
	bool	Parse();

	bool	ReadPacketHeader( TPCapPacketHeader& outHeader, UInt32& outError );

public:
	;		CPCapCaptureReader()
		:	CCaptureReader( CaptureFile::CF_FORMAT_PCAP, CaptureFile::CF_ACCESS_UNKNOWN )
		,	m_nVersion( TCPDUMP_VERSION_1 )
		,	m_bSwapped( false )
		,	m_nTimeStampMultiplier( 1000 )
		,	m_nPacketHeaderLength( sizeof( TcpDumpPacketHeader ) )
	{}
	;		CPCapCaptureReader( const path& inPath )
		:	CCaptureReader( inPath, CaptureFile::CF_FORMAT_PCAP, CaptureFile::CF_ACCESS_UNKNOWN )
		,	m_nVersion( TCPDUMP_VERSION_1 )
		,	m_bSwapped( false )
		,	m_nTimeStampMultiplier( 1000 )
		,	m_nPacketHeaderLength( sizeof( TcpDumpPacketHeader ) )
	{}
	virtual	~CPCapCaptureReader() { Close(); }

	virtual void	CopyMetaData( const CFileMetaData& inData ) {
		CPCapCapture::CopyMetaData( inData );
	}
	void			Close() { CCaptureReader::Close(); }

	virtual tMediaType	GetMediaType() const {
		return CPCapCapture::GetMediaType();
	}
	UInt32			GetPacketHeaderLength() const {
		return m_nPacketHeaderLength;
	}
	UInt64			GetTimeStampMultiplier() const {
		return m_nTimeStampMultiplier;
	}

	static bool	IsKind( const std::string& inKind ) {
		return CPCapCapture::IsKind( inKind );
	}
	bool	IsSwapped() const { return m_bSwapped; }
	bool	IsVersion1() const { return (m_nVersion == TCPDUMP_VERSION_1); }
	bool	IsVersion2() const { return (m_nVersion == TCPDUMP_VERSION_2); }

	virtual bool	Open( const path& inPath );

	virtual UInt64	MoveTo( UInt64 inPosition ) {		// Absolute
		return CCaptureReader::MoveTo( inPosition );
	}
	virtual UInt64	Seek( SInt64 inOffset ) {			// Relative
		return CCaptureReader::Seek( inOffset );
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32& outError );
	virtual bool	ReadFilePackets( UInt32 inCount, CFilePacketList& outPackets, UInt32& outError );

	void	SetPacketHeaderLength( UInt32 inLength ) { m_nPacketHeaderLength = inLength; }
	void	SetSwapped( bool inEnabled = true ) { m_bSwapped = inEnabled; }
	void	SetTimeStampMultiplier( UInt64 inMultiplier ) { m_nTimeStampMultiplier = inMultiplier; }
	void	SetVersion( int inVersion ) { m_nVersion = inVersion; }
};


// ============================================================================
//		CPCapCaptureWriter
// ============================================================================

class CPCapCaptureWriter
	:	public CCaptureWriter
	,	public CPCapCapture
{
protected:
	int		m_nVersion;
	UInt32	m_nPacketCount;

public:
	;		CPCapCaptureWriter( CF_ACCESS_TYPE inAccess, tMediaType inType )
		:	CCaptureWriter( CaptureFile::CF_FORMAT_PCAP, inAccess )
		,	CPCapCapture( inType )
		,	m_nVersion( 0 )
		,	m_nPacketCount( 0 )
	{
		SetLinkType( inType );
	}
	virtual	~CPCapCaptureWriter() { CCaptureWriter::Close(); }

	virtual void	CopyMetaData( const CFileMetaData& inData ) {
		CPCapCapture::CopyMetaData( inData );
	}

	virtual tMediaType	GetMediaType() const {
		return CPCapCapture::GetMediaType();
	}
	UInt32	GetPacketHeaderLength() const {
		return sizeof( TcpDumpPacketHeader );
	}

	virtual void	Init();
	static bool		IsKind( const std::string& inKind ) {
		return CPCapCapture::IsKind( inKind );
	}

	virtual UInt64	MoveTo( UInt64 inPosition ) {		// Absolute
		return CCaptureWriter::MoveTo( inPosition );
	}
	virtual UInt64	Seek( SInt64 inOffset ) {			// Relative
		return CCaptureWriter::Seek( inOffset );
	}

	virtual bool	Open( const path& inFileName );

	virtual bool	WriteFilePacket( const CFilePacket& inPacket );
};

}	// namespace CaptureFile
