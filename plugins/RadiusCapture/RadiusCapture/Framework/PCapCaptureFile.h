// ============================================================================
//	PCapCaptureFile.h
//		interface for CPCapCaptureFile class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "CaptureFile.h"

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	PCap Capture File
//
//	A PCap Capture File support reading and writing of PCap format files.
//	PCap files have the extension: pcap or dmp.
//
//	PCap Capture File is part of the Capture File module.
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


namespace CaptureFile {


// ============================================================================
//		Constants and Data Structures
// ============================================================================

#include "wppushpack.h"

typedef struct _TcpDumpFileHeader {
	UInt32	magic;
	UInt16	major;		// Major version number.
	UInt16	minor;		// Minor version number.
	SInt32	zone;		// GMT to local correction.
	UInt32	sigfigs;	// Accuracy of timestamps.
	UInt32	snaplen;	// Max length of captured packets, in octets.
	UInt32	linktype;	// Data link type.
} TcpDumpFileHeader;

typedef struct _TcpDumpPacketHeader {
	UInt32	tshi;		// Timestamp seconds.
	UInt32	tslo;		// Timestamp microseconds.
	UInt32	caplen;		// Captured packet length.
	UInt32	len;		// Original packet length.
} TcpDumpPacketHeader;

// Note: this stupid modification is courtesy of Alexey Kuznetsov,
// see <http://ftp.sunet.se/pub/os/Linux/ip-routing/lbl-tools/>.
typedef struct _TcpDumpPacketHeader2 {
	TcpDumpPacketHeader	hdr;
	UInt32				ifindex;	// Index, in *capturing* machine's interfaces.
	UInt16				protocol;	// Ethernet packet type.
	UInt8				pkt_type;	// Broadcast/multicast/etc. indication.
	UInt8				pad;		// Padding.
} TcpDumpPacketHeader2;

// See http://madwifi.org/wiki/DevDocs/RadiotapHeader.
typedef struct _ieee80211_radiotap_header {
	UInt8	it_version;	// set to 0
	UInt8	it_pad;
	UInt16	it_len;		// entire length
	UInt32	it_present;	// fields present
} ieee80211_radiotap_header;

#include "wppoppack.h"

const UInt32 TCPDUMP_MAGIC				= 0xA1B2C3D4;
const UInt32 TCPDUMP_MAGIC_SWAPPED		= 0xD4C3B2A1;
const UInt32 TCPDUMP_MAGIC_2			= 0xA1B2CD34;
const UInt32 TCPDUMP_MAGIC_2_SWAPPED	= 0x34CDB2A1;

#define TCPDUMP_VERSION_1		0
#define TCPDUMP_VERSION_2		1

// Link types.
#define DLT_EN10MB				1		// kMediaType_802_3 / kMediaSubType_Native
#define DLT_IEEE802				6		// kMediaType_802_5 / 0
#define DLT_RAW					12
#define DLT_IEEE802_11			105		// kMediaType_802_3 / kMediaSubType_802_11_gen
#define DLT_FRELAY				107
#define DLT_LINUX_SLL			113		//Linux cooked sockets
#define DLT_IEEE802_11_RADIO	127

// Peek Defaults
#define DEFAULT_MAGIC			TCPDUMP_MAGIC
#define DEFAULT_MAJOR			2
#define DEFAULT_MINOR			4
#define DEFAULT_ZONE			0
#define DEFAULT_SIGFLAGS		0
#define DEFAULT_SNAPLEN			65535
#define DEFAULT_LINKTYPE		0


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
	;		CPCapCapture( tMediaType inType = MT_ETHERNET )
		:	m_MediaType( inType )
		,	m_nMajorVersion( DEFAULT_MAJOR )
		,	m_nMinorVersion( DEFAULT_MINOR )
		,	m_nZone( DEFAULT_ZONE )
		,	m_nSignificantFigures( DEFAULT_SIGFLAGS )
		,	m_nSnapLength( DEFAULT_SNAPLEN )
		,	m_nLinkType( DEFAULT_LINKTYPE )
	{}
	;		~CPCapCapture() {}

	virtual void	CopyMetaData( const CCaptureFile& inOther ) {
		inOther;
	}

	virtual tMediaType	GetMediaType() const { return m_MediaType; }

	static bool	IsKind( const std::wstring& inKind ) {
		if (inKind.compare( L"pcap" ) == 0) return true;
		if (inKind.compare( L"dmp" ) == 0) return true;
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
	};

protected:
	int				m_nVersion;
	bool			m_bSwapped;
	CFileOffsets	m_ayOffsets;

protected:
	void	Parse( bool inScanPackets );

	bool	ReadPacketHeader( TPCapPacketHeader& outHeader );

	bool	SeekToPacket( UInt32 inIndex );

public:
	;		CPCapCaptureReader()
		:	CCaptureReader( CF_FORMAT_PCAP )
		,	m_nVersion( 0 )
		,	m_bSwapped( false )
	{}
	virtual	~CPCapCaptureReader() { Close(); }

	virtual void	CopyMetaData( const CCaptureFile& inOther ) {
		CPCapCapture::CopyMetaData( inOther );
	}
	void			Close() { CCaptureReader::Close(); }

	virtual tMediaType	GetMediaType() const {
		return CPCapCapture::GetMediaType();
	}
	virtual UInt32	GetPacketCount() const {
		size_t	nHighest = m_ayOffsets.GetHighest() + 1;
		_ASSERTE( nHighest < kMaxUInt32 );
		if ( nHighest >= kMaxUInt32 ) return kMaxUInt32;

		return static_cast<UInt32>( nHighest );
	}
	UInt32			GetPacketHeaderLength() const {
		return (IsVersion1())
			? sizeof( TcpDumpPacketHeader )
			: sizeof( TcpDumpPacketHeader2 );
	}

	static bool	IsKind( const std::wstring& inKind ) {
		return CPCapCapture::IsKind( inKind );
	}
	bool	IsSwapped() const { return m_bSwapped; }
	bool	IsVersion1() const { return (m_nVersion == TCPDUMP_VERSION_1); }
	bool	IsVersion2() const { return (m_nVersion == TCPDUMP_VERSION_2); }

	bool	Open( const std::wstring& inFileName, bool inScanPackets );
	bool	Open( const wchar_t* inFileName, bool inScanPackets ) {
		return Open( std::wstring( inFileName ), inScanPackets );
	}
	bool	Open( const std::string& inFileName, bool inScanPackets ) {
		return Open( AtoW( inFileName ), inScanPackets );
	}
	bool	Open( const char* inFileName, bool inScanPackets ) {
		return Open( AtoW( inFileName ), inScanPackets );
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32 inIndex );

	void	SetSwapped( bool inEnabled = true ) { m_bSwapped = inEnabled; }
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

protected:
#if (0)
	virtual void	Init();
#endif

public:
	;		CPCapCaptureWriter( int inAccess, tMediaType inType )
		:	CCaptureWriter( CF_FORMAT_PCAP, inAccess )
		,	CPCapCapture( inType )
		,	m_nVersion( 0 )
		,	m_nPacketCount( 0 )
	{
		SetLinkType( inType );
	}
	virtual	~CPCapCaptureWriter() { CCaptureWriter::Close(); }

	virtual void	CopyMetaData( const CCaptureFile& inOther ) {
		CPCapCapture::CopyMetaData( inOther );
	}

	virtual tMediaType	GetMediaType() const {
		return CPCapCapture::GetMediaType();
	}
	UInt32	GetPacketCount() const { return m_nPacketCount; }
	UInt32	GetPacketHeaderLength() const {
		return sizeof( TcpDumpPacketHeader );
	}

	virtual void	Init( const CCaptureFile* inTemplate );
	static bool		IsKind( const std::wstring& inKind ) {
		return CPCapCapture::IsKind( inKind );
	}

	bool	Open( const std::wstring& inFileName );
	bool	Open( const wchar_t* inFileName ) {
		return Open( std::wstring( inFileName ) );
	}
	bool	Open( const std::string& inFileName ) {
		return Open( AtoW( inFileName ) );
	}
	bool	Open( const char* inFileName ) {
		return Open( AtoW( inFileName ) );
	}

	virtual bool	WriteFilePacket( const CFilePacket& inPacket );
};

}	// namespace CaptureFile
