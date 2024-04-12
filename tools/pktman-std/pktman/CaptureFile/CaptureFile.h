// ============================================================================
//	CaptureFile.h
//		interface for CCaptureFile class.
// ============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#include "MediaTypes.h"
#include "MemUtil.h"
#include "tagfile.h"
#include "strutil.h"
#include <stdlib.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

using std::string;
using std::wstring;
using std::filesystem::path;

// Use (void) to silence unused warnings.
#define assertm(exp, msg)	assert(((void)msg, exp))

#define CF_PEEK_FILE
#define CF_PCAP_FILE
#define CF_USE_TRANSFORM

#ifndef _WIN32
#include <endian.h>		// For __BYTE_ORDER
#include <byteswap.h>	// For bswap_16, bswap_32 and bswap_64
#endif


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Capture File
//
//	A Capture File is a file of Packets and the corresponding meta data: Media
//	Type and adapter information. Access is performed one packet at a time.
//	Packets are read and written as File Packets which are easily translated
//	into Packets.
//
//	The CaptureFile namespace is fully self contained, and may be extracted
//	from the Framework: Capture File, GPS Info, PCap Capture File, Peek Capture
//	File along with the Framework's Media Type, Tag File, WP Pop and Push Pack,
//	and WP Types. Or download the package from MyPeek.
//
//	Capture File is part of the Capture File module.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

#define STREAM_SEEK_SET			SEEK_SET
#define STREAM_SEEK_CUR			SEEK_CUR
#define STREAM_SEEK_END			SEEK_END

// Link types. https://www.tcpdump.org/linktypes.html
#define DLT_NULL				0		// BSD loopback encapsulation
#define DLT_EN10MB				1		// kMediaType_802_3 / kMediaSubType_Native
#define DLT_IEEE802				6		// kMediaType_802_5 / 0
#define DLT_RAW					12		// Not defined in the spec.
#define DLT_IEEE802_11			105		// kMediaType_802_3 / kMediaSubType_802_11_gen
#define DLT_FRELAY				107		// Frame Relay
#define DLT_LINUX_SLL			113		// Linux cooked sockets
#define DLT_IEEE802_11_RADIO	127		// Radio Link-Layer information

#define LINKTYPE_NULL					DLT_NULL
#define LINKTYPE_ETHERNET				DLT_EN10MB
#define LINKTYPE_IEEE802_5				DLT_IEEE802
#define LINKTYPE_IEEE802_11				DLT_IEEE802_11
#define LINKTYPE_FRELAY					DLT_FRELAY
#define LINKTYPE_LINUX_SLL				DLT_LINUX_SLL
#define LINKTYPE_IEEE802_11_RADIOTAP	DLT_IEEE802_11_RADIO

class CCaptureFile;
class CPacket;

typedef unsigned char		byte;
typedef std::vector<byte>	CByteVector;
typedef std::vector<path>	CPathList;

// File Types
enum CF_FILE_TYPE {
	CF_TYPE_UNKNOWN,
	CF_TYPE_READER,
	CF_TYPE_WRITER,
	CF_TYPE_MAX
};

// File Formats
enum CF_FILE_FORMAT {
	CF_FORMAT_UNKNOWN,
	CF_FORMAT_PEEK,
	CF_FORMAT_PCAP,
	CF_FORMAT_PCAP_NG,
	CF_FORMAT_ZIPPED = 0x00100
};

// File Access
enum CF_ACCESS_TYPE {
	CF_ACCESS_UNKNOWN,
	CF_ACCESS_SERIAL,
	CF_ACCESS_RANDOM,
	CF_ACCESS_MAX
};

static const UInt32		kFileFormatUnknown	= 0x0000;
static const UInt32		kFileFormatPeek		= 0x0001;
static const UInt32		kFileFormatPCap		= 0x0002;
static const UInt32		kFileFormatPCapNg	= 0x0003;
static const UInt32		kFileFormatTypeMask	= 0x00FF;
static const UInt32		kFileFormatZipped	= 0x0100;
static const UInt32		kFileFormatFlagMask	= 0x00FF;

// Read Errors
enum {
	CF_ERROR_NONE,
	CF_ERROR_EOF,
	CF_ERROR_UNEXPECTED_EOF,
	CF_ERROR_INCOMPLETE_HEADER,
	CF_ERROR_INVALID_HEADER,
	CF_ERROR_DATA_OFFSET,
	CF_ERROR_INCOMPLETE_PAYLOAD,
	CF_ERROR_UNHANDLED_PACKET,
	CF_ERROR_MAX
};

const tMediaType	kMediaType_Ethernet{ kMediaType_802_3, kMediaSubType_Native };
const tMediaType	kMediaType_Wireless{ kMediaType_WirelessWan, kMediaSubType_Native };

// #define NEXT_PACKET		(-1)
#define OFFSET_EOF		(static_cast<size_t>( -1 ))
#define Attrib_Invalid	(static_cast<UInt16>( -1 ))

struct tFileMode {
	CF_FILE_TYPE	fType;
	CF_FILE_FORMAT	fFormat;
	CF_ACCESS_TYPE	fAccess;
	bool			fScan;
	tMediaType		fMediaType;

	tFileMode( CF_FILE_TYPE inType,
			CF_FILE_FORMAT inFormat,
			CF_ACCESS_TYPE inAccess = CF_ACCESS_SERIAL,
			bool inScan = false,
			tMediaType inMediaType = kMediaType_Ethernet ) {
		fType = inType;
		fFormat = inFormat;
		fAccess = inAccess;
		fScan = inScan;
		fMediaType = inMediaType;
	}

	bool	IsRandom() const { return (fAccess == CF_ACCESS_RANDOM); }
	bool	IsReader() const { return (fType == CF_TYPE_READER); }
	bool	IsScan() const { return fScan; }
	bool	IsSerial() const { return (fAccess == CF_ACCESS_SERIAL); }
	bool	IsWriter() const { return (fType == CF_TYPE_WRITER); }
};

static const UInt32	s_n802_11_Band_A = 3;
static const UInt32 s_n802_11_Band_B = 4;
static const UInt32 s_n802_11_Band_N_Low = 302;
static const UInt32 s_n802_11_Band_N_High = 303;

// 802.11 data rates. Rates are in 0.5Mbps: 11Mbps has a rate value of 22.
// all A, B, G, and N rates
// A :  6, 9, 12, 18, 24, 36, 48, 54
//
// B : 1, 2, 5.5, 11
//
// G : 6, 9, 11, 12, 18, 22, 24, 33, 36, 48, 54
//
// N : 1, 2, 5.5, 6, 6.5, 9, 11, 12, 13, 13.5, 18, 19.5, 24, 26, 27,
//     36, 39(78), 39(79), 40.5, 48, 52, 54, 58.5, 65, 78, 81, 81.5, 104,
//     105, 108, 117, 121.5, 130, 135, 162, 216, 243, 270, 300
static const UInt32 s_ay802_11_Rates[] = {	// In 0.5 Mbps units.
	2, 4, 11, 12, 13, 18, 22, 24, 26, 27, 36, 39, 48, 52, 54, 
	72, 78, 79, 81, 96, 104, 108, 117, 130, 154, 162, 163, 208,
	210, 216, 234, 243, 260, 270, 324, 432, 486, 540, 600
};

// A : 6, 9, 12, 18, 24, 36, 48, 54 Mbps
static const UInt32 s_ay802_11A_Rates[] = {	// In 0.5 Mbs units.
	12, 18, 24, 36, 48, 72, 96, 108
};

// B : 1, 2, 5.5, 11 Mbps
static const UInt32	s_ay802_11B_Rates[] = {	// In 0.5 Mbs units.
	2, 4, 11, 22
};

// G : 1, 2, 5.5, 6, 9, 11, 12, 18, 22, 24, 33, 36, 48, 54 Mbps
static const UInt32 s_ay802_11G_Rates[] = {	// In 0.5 Mbs units.
	2, 4, 11, 12, 18, 22, 24, 36, 48, 72, 96, 108
};

// N : 1, 2, 5.5, 6, 6.5, 9, 11, 12, 13, 13.5, 18, 19.5, 24, 26, 27,     
//		39(78), 39(79), 40.5, 48, 52, 54, 58.5, 65, 78, 81, 81.5, 104,
//		105, 108, 117, 121.5, 130 Mbps
static const UInt32 s_ay802_11N_Rates[] = {
	2, 4, 11, 12, 13, 18, 22, 24, 26, 27, 36, 39, 48, 52, 54, 
	72, 78, 79, 81, 96, 104, 108, 117, 130, 154, 162, 163, 208,
	210, 216, 234, 243, 260
};

// 802.11 Channels. The ChannelFrequency() function will return it's frequency.

static const UInt32	s_ay802_11A_Channels[] = {
	36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
};

static const UInt32	s_ay802_11B_Channels[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
};

static const UInt32	s_ay802_11N_Low_Channels[] = {
	5, 6, 7, 8, 9, 10, 11, 40, 48, 56, 64, 153, 161
};

static const UInt32	s_ay802_11N_High_Channels[] = {
	1, 2, 3, 4, 5, 6, 7, 36, 44, 52, 60, 149, 157
};

static const UInt32	s_n802_11_BaseFrequecy_A = 5000;
static const UInt32	s_n802_11_BaseFrequecy_B = 2407;


// extern int	CaptureFile( size_t nFileSize, byte* pFileData );

tFileMode	FileModeReader( const tFileMode inMode );
tFileMode	FileModeWriter( const tFileMode inMode );

UInt32		ChannelFrequency( UInt32 inChannel );


// -----------------------------------------------------------------------------
//		tVersion
// -----------------------------------------------------------------------------

struct tVersion {
	UInt32	fMajorVersion;
	UInt32	fMinorVersion;
	UInt32	fMajorRevision;
	UInt32	fMinorRevision;

	tVersion()
		:	fMajorVersion( 0 )
		,	fMinorVersion( 0 )
		,	fMajorRevision( 0 )
		,	fMinorRevision( 0 )
	{}
	wstring	Format() const {
		std::wostringstream	ss;
		ss << fMajorVersion << L"." << fMinorVersion << L"." << fMajorRevision
			<< L"." << fMinorRevision;
		return ss.str();
	}
	string	FormatA() const {
		std::ostringstream	ss;
		ss << fMajorVersion << "." << fMinorVersion << "." << fMajorRevision
			<< "." << fMinorRevision;
		return ss.str();
	}
	bool	IsNull() const { return (fMajorVersion == 0) && (fMinorVersion == 0) &&
		(fMajorRevision == 0) && (fMinorRevision == 0);
	}
	void	Parse( const wstring& inVersion ) {
		wchar_t				cDot;
		std::wistringstream	ssVersion( inVersion );
		ssVersion >> fMajorVersion >> cDot >> fMajorRevision >>
			cDot >> fMinorVersion >> cDot >> fMinorRevision;
	}
	void	ParseA( const string& inVersion ) {
		char				cDot;
		std::istringstream	ssVersion( inVersion );
		ssVersion >> fMajorVersion >> cDot >> fMajorRevision >>
			cDot >> fMinorVersion >> cDot >> fMinorRevision;
	}
	void	Reset() { fMajorVersion = 0; fMinorVersion = 0;
		fMajorRevision = 0; fMinorRevision = 0;
	}
};


// -----------------------------------------------------------------------------
//		tTimeLong
// -----------------------------------------------------------------------------

#include "wppushpack.h"

union tTimeLong {
	UInt64		i;
	struct {
		UInt32	TimeLo;
		UInt32	TimeHi;
	} sTime;

	tTimeLong( UInt64 inTime = 0 ) : i( inTime ) {}
	operator UInt64() const { return i; }
	bool	IsNull() const { return (i == 0); }
	static tTimeLong Now();
	void	Reset() { i = 0; }
} WP_PACK_STRUCT;
#include "wppoppack.h"


// ============================================================================
//		CFileTime
// ============================================================================

class CFileTime
{
public:
	enum tTimeFormat {
		kFormat_Nanoseconds,
		kFormat_Milliseconds,
		kFormat_Seconds
	};

protected:
	tTimeLong	m_ltTime;	// Nanoseconds since 1/1/1601.

public:
	static UInt32	GetTimeBias() {
		// #pragma(TODO)
		return 0;
	}

	static CFileTime	Now() { return tTimeLong::Now(); }

public:
	;		CFileTime() {}
	;		CFileTime( const tTimeLong inTime ) : m_ltTime( inTime ) {}
	;		CFileTime( UInt64 inTime )
	{
		m_ltTime.i = inTime;
	}
	;		CFileTime( const wstring& inTime )
	{
		Parse( inTime );
	}
	;		CFileTime( const string& inTime )
	{
		Parse( inTime );
	}

	;		operator UInt64() const { return m_ltTime.i; }

	wstring	Format( tTimeFormat inFormat = kFormat_Nanoseconds );
	string	FormatA( tTimeFormat inFormat = kFormat_Nanoseconds );

	bool	IsNull() const { return (m_ltTime.i == 0); }

	void	Parse( const wstring& inTime );
	void	Parse( const string& inTime );

	void	Reset() { m_ltTime.Reset(); }
};


// ============================================================================
//		CBlockNameList
// ============================================================================

class CBlockNameList
{
public:
	enum BlockAttrib {
		Version_Root,
		Version_AppVersion,
		Version_ProductVersion,
		Version_FileVersion,

		Session_Root,
		Session_RawTime,
		Session_Time,
		Session_TimeZoneBias,
		Session_MediaType,
		Session_MediaSubType,
		Session_LinkSpeed,
		Session_PacketCount,
		Session_Comment,
		Session_SessionStartTime,
		Session_SessionEndTime,
		Session_AdapterName,
		Session_AdapterAddr,
		Session_CaptureName,
		Session_CaptureID,
		Session_Owner,
		Session_FileIndex,
		Session_Host,
		Session_EngineName,
		Session_EngineAddr,
		Session_MediaDomain,
		Session_DataRates,
		Session_Rate,
		Session_ChannelList,
		Session_Channel,
		Session_Channel_Number,
		Session_Channel_Frequency,
		Session_Channel_Band,
		
		FileId_Root,
		FileId_Id,
		FileId_Index,

		Attrib_Count
	};

protected:
	std::vector<string>	m_ayNames;

protected:
	void	AddName( wstring& inName ) {
		m_ayNames.push_back( WtoA( inName ) );
	}

public:
	;		CBlockNameList();


	const string&	GetName( size_t inIndex ) {
		if ( inIndex < (m_ayNames.size() - 1) ) return m_ayNames[inIndex];
		if ( inIndex == TaggedFile::Attrib_PacketDataLength ) {
			return m_ayNames[m_ayNames.size() - 2];
		}
		return m_ayNames[m_ayNames.size() - 1];
	}
};


// ============================================================================
//		CAttributeNameList
// ============================================================================

class CAttributeNameList
{
protected:
	std::vector<string>	m_ayNames;

	void	AddName( const char* inName ) {
		m_ayNames.push_back( inName );
	}

public:
	;		CAttributeNameList();

	const string&	GetName( size_t inIndex ) {
		if ( inIndex < (m_ayNames.size() - 1) ) return m_ayNames[inIndex];
		if ( inIndex == TaggedFile::Attrib_PacketDataLength ) {
			return m_ayNames[m_ayNames.size() - 2];
		}
		return m_ayNames[m_ayNames.size() - 1];
	}
};


// ============================================================================
//		CAttribute
// ============================================================================

class CAttribute
{
protected:
	static CBlockNameList		s_BlockNames;
	static CAttributeNameList	s_AttributeNames;

protected:
	UInt16	m_nId;
	string	m_strName;

public:
	static const string&	GetBlockName( size_t inIndex ) {
		return s_BlockNames.GetName( inIndex );
	}
	static const string&	GetAttributeName( size_t inIndex ) {
		return s_AttributeNames.GetName( inIndex );
	}

public:
	;		CAttribute() : m_nId( Attrib_Invalid ) {}
	;		CAttribute( UInt16 inId, const char* inName )
		:	m_nId( inId )
		,	m_strName( inName )
	{}
	;		CAttribute( UInt16 inId, const string& inName )
		:	m_nId( inId )
		,	m_strName( inName )
	{}

	UInt16			GetId() const { return m_nId; }
	const string&	GetName() const { return m_strName; }
};


// ============================================================================
//		CAttributeIdList
// ============================================================================

typedef	std::vector<UInt16>	CAttributeIdList;


// ============================================================================
//		CBlockAttribute
// ============================================================================

class CBlockAttribute
	:	public CAttribute
{
protected:
	string		m_strValue;

public:
	;		CBlockAttribute() {}
	;		CBlockAttribute( UInt16 inId, const string& inValue )
		: CAttribute( inId, CAttribute::GetBlockName( inId ) ), m_strValue( inValue )
	{}
	;		CBlockAttribute( UInt16 inId, const char* inValue )
		: CAttribute( inId, CAttribute::GetBlockName( inId ) ), m_strValue( inValue )
	{}

	const string&	GetValue() const { return m_strValue; }

	void	SetValue( const char* inValue ) { m_strValue = inValue; }
	void	SetValue( const string& inValue ) { m_strValue = inValue; }
};


// ============================================================================
//		CPacketAttribute
// ============================================================================

class CPacketAttribute
	:	public CAttribute
{
protected:
	UInt32	m_nValue;

public:
	CPacketAttribute() : m_nValue( 0 ) {}
	CPacketAttribute( UInt16 inId, UInt32 inValue )
		: CAttribute( inId, CAttribute::GetAttributeName( inId ) ), m_nValue( inValue )
	{}
	UInt32	GetValue() const { return m_nValue; }
	void	SetValue( UInt32 inValue ) { m_nValue = inValue; }
};


// ============================================================================
//		CBlockAttributeList
// ============================================================================

class CBlockAttributeList
	:	public std::vector<CBlockAttribute>
{
public:
	const CBlockAttributeList&	operator=( const CBlockAttributeList& inOther ) {
		if ( &inOther != this ) {
			resize( inOther.size() );
			copy( inOther.begin(), inOther.end(), begin() );
		}
		return *this;
	}

	size_t	Add( UInt16 inId, const char* inValue ) {
		size_t	nIndex = 0;
		if ( Find( inId, nIndex ) ) {
			at( nIndex ).SetValue( inValue );
			return nIndex;
		}
		std::vector<CBlockAttribute>::push_back( CBlockAttribute( inId, inValue ) );
		return size();
	}
	size_t	Add( UInt16 inId, const string& inValue ) {
		return Add( inId, inValue.c_str() );
	}

	bool	Find( UInt16 nId, size_t& outIndex ) const {
		size_t	nCount = size();
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( at( i ).GetId() == nId ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	size_t	ListAdd( UInt16 inId, const char* inValue ) {
		std::vector<CBlockAttribute>::push_back( CBlockAttribute( inId, inValue ) );
		return size();
	}
	size_t	ListAdd( UInt16 inId, const string& inValue ) {
		return ListAdd( inId, inValue.c_str() );
	}
};


// ============================================================================
//		CPacketAttributeList
// ============================================================================

class CPacketAttributeList
	:	public std::vector<CPacketAttribute>
{
public:
	size_t	Add( UInt16 inId, UInt32 inValue ) {
		push_back( CPacketAttribute( inId, inValue ) );
		return size();
	}
	void	AddList( size_t inCount, const TaggedFile::PacketAttribute* inAttribs ) {
		for ( size_t i = 0; i < inCount; ++i, ++inAttribs ) {
			push_back( CPacketAttribute( inAttribs->fieldType, inAttribs->fieldValue ) );
			if ( inAttribs->fieldType == TaggedFile::Attrib_PacketDataLength ) {
				break;
			}
		}
	}
	bool	Find( UInt16 nId, size_t& outIndex ) const {
		for ( size_t i = 0; i < size(); i++ ) {
			const CPacketAttribute&	paAttrib = at( i );
			if ( paAttrib.GetId() == nId ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}
};


// ============================================================================
//		CFileRollback
// ============================================================================

// Stores current file position. Restores file position unless Commit is called.
class CFileRollback
{
protected:
	bool			m_bRollback;
	std::ifstream&	m_File;
	UInt64			m_nReadPosition;
	UInt64			m_nWritePosition;

public:
	;	CFileRollback( std::ifstream& inFile )
			:	m_bRollback( true )
			,	m_File( inFile )
			,	m_nReadPosition( inFile.tellg() )
	{
		m_nReadPosition = m_File.tellg();
	}
	virtual ~CFileRollback()
	{
		if ( m_bRollback ) {
			m_File.seekg( m_nReadPosition );
		}
	}

	void	Commit() { m_bRollback = false; }
};


// ============================================================================
//		CFileOffsets
// ============================================================================

class CFileOffsets
{
public:
	typedef std::vector<UInt64>::reference reference;
	typedef std::vector<UInt64>::const_reference const_reference;

protected:
	bool				m_bComplete;
	size_t				m_nHighestIndex;
	std::vector<UInt64>	m_ayOffsets;
	UInt64				m_nFileSize;

protected:
	// Must use SetOffset.
	//reference operator[]( size_t /*inIndex*/ ) {
	//	assertm( 0, "Don't do this." );
	//}

public:
	;		CFileOffsets() : m_bComplete( false ), m_nHighestIndex( 0 ), m_nFileSize( 0 ) {}

	const_reference operator[]( size_t inIndex ) const {
		return m_ayOffsets[inIndex];
	}

	bool	empty() const { return m_ayOffsets.empty(); }

	UInt64	GetFileSize() const { return m_nFileSize; }
	size_t	GetHighest() const { return m_nHighestIndex; }
	UInt32	GetHighest32() const {
		assertm( (m_nHighestIndex < std::numeric_limits<UInt32>::max()), "Highest 32-bit Index is too large." );
		return static_cast<UInt32>( m_nHighestIndex );
	}

	bool	HasIndex( size_t inIndex ) const { return (inIndex <= m_nHighestIndex); }

	bool	IsComplete() const { return m_bComplete; }
	bool	IsEOF( UInt64 inOffset ) const { return (inOffset >= m_nFileSize); }

	void	resize( size_t inCount ) { m_ayOffsets.resize( inCount ); }

	void	SetComplete() { m_bComplete = true; }
	void	SetFileSize( UInt64 inSize ) { m_nFileSize = inSize; }
	void	SetFirstOffset( UInt64 inFileSize, size_t inCount, UInt64 inOffset ) {
		m_nFileSize = inFileSize;
		m_ayOffsets.resize( inCount );
		m_ayOffsets[0] = inOffset;
		m_nHighestIndex = 0;
	}
	bool	SetOffset( size_t inIndex, UInt64 inOffset ) {
		if ( IsEOF( inOffset ) ) return false;

		if ( m_ayOffsets.size() < (inIndex + 1) ) {
			// Estimate average attributes + packets size at 256 bytes.
			// Will need Wireless estimate.
			assertm( (m_nFileSize > 0), "File size can not be 0." );
			UInt64	nMoreOffsets = (m_nFileSize > 0)
				? ((m_nFileSize - inOffset) / 256 )
				: m_ayOffsets.size();
			assertm( (nMoreOffsets < std::numeric_limits<UInt32>::max()), "File size is greater than 32-bit max." );
			if ( nMoreOffsets >= std::numeric_limits<UInt32>::max() ) {
				throw -1;
			}
			size_t	nNewSize = m_ayOffsets.size() +
				((nMoreOffsets > 64) ? static_cast<size_t>( nMoreOffsets ) : 64);
			m_ayOffsets.resize( nNewSize );
		}
		m_ayOffsets[inIndex] = inOffset;
		if ( inIndex > m_nHighestIndex ) {
			m_nHighestIndex = inIndex;
		}
		return true;
	}
	size_t	size() const { return m_ayOffsets.size(); }
};


// ============================================================================
//		CFilePacket
// ============================================================================

class CFilePacket
{
public:
	enum {
		kFlag_None					= 0x0000,
		kFlag_NoTimeStamp			= 0x0001,
		kFlag_NoCRC					= 0x0002,
		kFlag_NoEthernetDestination	= 0x0004,
		kFlag_NoEthernetSource		= 0x0008,
		kFlag_MatchSlice			= 0x0010,
		kFlag_Ports                 = 0x0020
	};
	static const size_t	s_dBmCount = 4;

protected:
	tMediaType	m_MediaType;		// Not in the Packet.

	size_t		m_nLength;			// Packet Length on the wire.
	UInt64		m_nTimeStamp;		// PeekTime.
	UInt16		m_nFlags;			// Attrib_FlagsStatus,	// high 16 bits are PEEK status bits
	UInt16		m_nStatus;			// Attrib_FlagsStatus,	// low 16 are PEEK flags
	UInt32		m_nChannelNumber;	// wireless channel number.
	UInt32		m_nDataRate;		// wireless data rate in 0.5Mbs
	UInt32		m_nSignalStrength;	// signal strength, 0-100%
	SInt32		m_nSignaldBm[s_dBmCount];	// signal dBm, (signed)
	UInt32		m_nNoiseStrength;	// noise strength, 0-100%
	SInt32		m_nNoisedBm[s_dBmCount];	// noise dBm (signed)
	UInt32		m_nMediaSpecType;	// type of media specific info that follows
	UInt32		m_nProtocol;		// WAN Protocol (X.25, Frame Relay, PPP,....)
	UInt32		m_nDirection;		// WAN Direction (to DTE or to DCE)
	UInt32		m_nChannelFreq;		// channel frequency (wireless)
	UInt32		m_nChannelBand;		// channel band (a, b, turbo, etc.)
	UInt32		m_nFlagsN;			// 802.11N flags

	CByteVector		m_PacketData;	// Packet Data; slice length is GetCount()

protected:
	void	Copy( const CFilePacket& inOther );

	void	Reset();

public:
	;		CFilePacket( tMediaType inType = tMediaType( kMediaType_802_3, kMediaSubType_Native ) );
	;		CFilePacket( const CFilePacket& inOther )
		:	m_MediaType( inOther.m_MediaType )
		,	m_nLength( inOther.m_nLength )
		,	m_nTimeStamp( inOther.m_nTimeStamp )
		,	m_nFlags( inOther.m_nFlags )
		,	m_nStatus( inOther.m_nStatus )
		,	m_nChannelNumber( inOther.m_nChannelNumber )
		,	m_nDataRate( inOther.m_nDataRate )
		,	m_nSignalStrength( inOther.m_nSignalStrength )
		,	m_nNoiseStrength( inOther.m_nNoiseStrength )
		,	m_nMediaSpecType( inOther.m_nMediaSpecType )
		,	m_nProtocol( inOther.m_nProtocol )
		,	m_nDirection( inOther.m_nDirection )
		,	m_nChannelFreq( inOther.m_nChannelFreq )
		,	m_nChannelBand( inOther.m_nChannelBand )
		,	m_nFlagsN( inOther.m_nFlagsN )
		,	m_PacketData( inOther.m_PacketData )
	{
		for ( size_t i = 0; i < s_dBmCount; i++ ) {
			m_nSignaldBm[i] = inOther.m_nSignaldBm[i];
			m_nNoisedBm[i] = inOther.m_nNoisedBm[i];
		}
	}

	CFilePacket&	operator=( const CFilePacket& inOther ) {
		if ( &inOther != this ) {
			Copy( inOther );
		}
		return *this;
	}
	bool			operator==( const CFilePacket& inOther ) {
		return Compare( inOther, false );
	}
	bool			operator!=( const CFilePacket& inOther ) {
		return !Compare( inOther, false );
	}

	bool			Compare( const CFilePacket& inOther, bool inNoTime = false ) {
		if ( &inOther == this ) return true;
		if ( inOther.m_MediaType.fType != m_MediaType.fType ) return false;
		if ( inOther.m_MediaType.fSubType != m_MediaType.fSubType ) return false;
		if ( inOther.m_nLength != m_nLength ) return false;
		if ( !inNoTime ) if ( inOther.m_nTimeStamp != m_nTimeStamp ) return false;
		//if ( inOther.m_nFlags != m_nFlags ) return false;
		//if ( inOther.m_nStatus != m_nStatus ) return false;;

		if ( IsWireless() ) {
			if ( inOther.m_nChannelNumber != m_nChannelNumber ) return false;
			if ( inOther.m_nDataRate != m_nDataRate ) return false;
			if ( inOther.m_nSignalStrength != m_nSignalStrength ) return false;
			if ( inOther.m_nNoiseStrength != m_nNoiseStrength ) return false;
			if ( inOther.m_nMediaSpecType != m_nMediaSpecType ) return false;
			if ( inOther.m_nProtocol != m_nProtocol ) return false;
			if ( inOther.m_nDirection != m_nDirection ) return false;
			if ( inOther.m_nChannelFreq != m_nChannelFreq ) return false;
			if ( inOther.m_nChannelBand != m_nChannelBand ) return false;
			if ( inOther.m_nFlagsN != m_nFlagsN ) return false;
			for ( size_t i = 0; i < s_dBmCount; i++ ) {
				if ( inOther.m_nSignaldBm[i] != m_nSignaldBm[i] ) return false;
				if ( inOther.m_nNoisedBm[i] != m_nNoisedBm[i] ) return false;
			}
		}

		if ( inOther.m_PacketData.size() != m_PacketData.size() ) return false;
		if ( memcmp( &inOther.m_PacketData[0], &m_PacketData[0], m_PacketData.size() ) != 0 ) {
			return false;
		}

		return true;
	}

	void			GetAttributes( CPacketAttributeList& outAttribs ) const;
	UInt32			GetChannelBand() const { return m_nChannelBand; }
	UInt32			GetChannelFreq() const { return m_nChannelFreq; }
	UInt32			GetChannelNumber() const { return m_nChannelNumber; }
	UInt32			GetDataRate() const { return m_nDataRate; }
	UInt32			GetDirection() const { return m_nDirection; }
	UInt16			GetFlags() const { return m_nFlags; }
	size_t			GetLength() const { return m_nLength; }
	UInt32			GetMediaSpecType() const { return m_nMediaSpecType; }
	SInt32			GetNoisedBm( size_t inIndex ) const { return (inIndex < s_dBmCount) ? m_nNoisedBm[inIndex] : 0; }
	void			GetNoisedBm( SInt32* outList ) const {
		for ( size_t i = 0; i < s_dBmCount; i++ ) outList[i] = m_nNoisedBm[i];
	}
	UInt32			GetNoiseStrength() const { return m_nNoiseStrength; }
	UInt8*			GetPacketData() { return (m_PacketData.empty()) ? nullptr : &m_PacketData[0]; }
	const UInt8*	GetPacketData() const { return (m_PacketData.empty()) ? nullptr : &m_PacketData[0]; }
	size_t			GetPacketDataLength() const { return m_PacketData.size(); }
	UInt32			GetProtocol() const { return m_nProtocol; }
	SInt32			GetSignaldBm( size_t inIndex ) const { return (inIndex < s_dBmCount) ? m_nSignaldBm[inIndex] : 0; }
	void			GetSignaldBm( SInt32* outList ) const {
		for ( size_t i = 0; i < s_dBmCount; i++ ) outList[i] = m_nSignaldBm[i];
	}
	UInt32			GetSignalStrength() const { return m_nSignalStrength; }
	size_t			GetSliceLength() const { return (GetLength() == GetPacketDataLength()) ? 0 : GetPacketDataLength(); }
	UInt16			GetStatus() const { return m_nStatus; }
	UInt64			GetTimeStamp() const { return m_nTimeStamp; }

	bool	IsMutable() const { return !m_PacketData.empty(); }
	bool	IsSliced() const { return (GetPacketDataLength() < m_nLength); }
	bool	IsWAN() const { return m_MediaType.IsWAN(); }
	bool	IsWireless() const { return m_MediaType.IsWireless(); }

	bool	ParsePacketData( CPacket& outPacket );

	void	ResetPacketData(){ SetPacketData( 0, 0, nullptr ); }

	void	SetAttributes( const CPacketAttributeList& inAttribs, UInt32& outDataLength );
	void	SetChannelBand( UInt32 inBand ) { m_nChannelBand = inBand; }
	void	SetChannelFreq( UInt32 inFreq ) { m_nChannelFreq = inFreq; }
	void	SetChannelNumber( UInt32 inChannel ) { m_nChannelNumber = inChannel; }
	void	SetDataRate( UInt32 inRate ) { m_nDataRate = inRate; }
	void	SetDirection( UInt32 inDirection ) { m_nDirection = inDirection; }
	void	SetFlags( UInt16 inFlags ) { m_nFlags = inFlags; }
	void	SetMediaSpecType( UInt32 inType ) { m_nMediaSpecType = inType; }
	void	SetMediaType( tMediaType inMediaType ) { m_MediaType = inMediaType; }
	void	SetNoisedBm( size_t inIndex, SInt32 indBm ) {
		if ( inIndex < s_dBmCount )	m_nNoisedBm[inIndex] = indBm;
	}
	void	SetNoisedBm( UInt32* inList ) {
		for ( size_t i = 0; i < s_dBmCount; i++ ) m_nNoisedBm[i] = inList[i];
	}
	void	SetNoiseStrength( UInt32 inStrength ) { m_nNoiseStrength = inStrength; }
	void	SetPacketData( const CByteVector& inData ) {
		m_PacketData = inData;
	}
	void	SetPacketData( size_t inLength, size_t inDataLength, UInt8* inData ) {
		m_nLength = inLength;
		if ( inDataLength > 0 ) {
			m_PacketData.assign( inData, (inData + inDataLength) );
		}
		else {
			m_PacketData.clear();
		}
	}
	void	SetProtocol( UInt32 inProtocol ) { m_nProtocol = inProtocol; }
	void	SetSignaldBm( size_t inIndex, SInt32 indBm ) {
		if ( inIndex < s_dBmCount ) m_nSignaldBm[inIndex] = indBm;
	}
	void	SetSignaldBm( SInt32* inList ) {
		for ( size_t i = 0; i < s_dBmCount; i++ ) m_nSignaldBm[i] = inList[i];
	}
	void	SetSignalStrength( UInt32 inStrength ) { m_nSignalStrength = inStrength; }
	void	SetStatus( UInt16 inStatus ) { m_nStatus = inStatus; }
	void	SetTimeStamp( UInt64 inTime ) { m_nTimeStamp = inTime; }

#if (0)
	size_t	Load( size_t inLength, const byte* inData );
	bool	Load( COSFile& inFile, UInt64 inPosition );
	size_t	Store( CByteVector& inData ) const;
	bool	Store( COSFile& inFile ) const;
#endif
};


// ============================================================================
//		CFilePacketList
// ============================================================================

typedef std::vector<CFilePacket>	CFilePacketList;


// ============================================================================
//		CXmlString
// ============================================================================

class CXmlString
	:	public string
{
public:
	;		CXmlString() {}
	;		CXmlString( const string& inOther );

	void	AddAttribute( const string& inAttribute, const string& inValue );

	bool	GetAttribute( const string& inAttribute, string& outValue ) const;
	bool	GetAttribute( const string& inAttribute, size_t& ioOffset, string& outValue ) const;

	void	Load( const string& inData ) { assign( inData ); }
	void	Load( size_t inLength, const byte* inData );

	void	Set( const string& inName );
	void	SetAttribute( const string& inAttribute, const string& inValue );
	void	Store( size_t inLength, byte* outData );
};


// ============================================================================
//		CFileMetaData
// ============================================================================

class CFileMetaData
{
public:
	tMediaType		m_MediaType;
	int				m_nFileFormat;
	std::wstring	m_strFileName;
	size_t			m_nFileSize;
	size_t			m_nPacketCount;

	UInt32			m_nVersion;
	tVersion		m_AppVersion;
	tVersion		m_ProductVersion;

	UInt64			m_RawTime;
	string			m_Time;
	UInt32			m_TimeZoneBias;
	UInt32			m_LinkSpeed;
	UInt32			m_PacketCount;
	string			m_Comment;
	string			m_SessionStartTime;
	string			m_SessionEndTime;
	string			m_AdapterName;
	string			m_AdapterAddr;
	string			m_CaptureName;
	string			m_CaptureId;
	string			m_Owner;
	UInt32			m_FileIndex;
	string			m_Host;
	string			m_EngineName;
	string			m_EngineAddr;
	string			m_MediaDomain;
	string			m_DataRates;
	string			m_ChannelList;
	std::vector<UInt32>		m_ayRates;

#if (0)
	std::vector<tChannel>	m_ayChannels;
#endif

public:
	;		CFileMetaData() {}

};


// ============================================================================
//		CBaseFile
// ============================================================================

class CBaseFile
{
public:
	enum FILE_MODES {
		FILEMODE_READ = 0x01,
		FILEMODE_WRITE = 0x02
	};

protected:
	CF_FILE_TYPE	m_nFileType;
	CF_FILE_FORMAT	m_nFileFormat;
	CF_ACCESS_TYPE	m_nFileAccess;
	path			m_Path;
	size_t			m_nPacketsPosition;

public:
	;		CBaseFile( CF_FILE_TYPE inType, CF_FILE_FORMAT inFormat, CF_ACCESS_TYPE inAccess )
			:	m_nFileType( inType )
			,	m_nFileFormat( inFormat )
			,	m_nFileAccess( inAccess )
			,	m_nPacketsPosition( 0 )
	{}
	virtual	~CBaseFile() {}

	virtual	void	Close() {};
	virtual void	CopyMetaData( const CFileMetaData& inMetaData ) = 0;

	CF_ACCESS_TYPE		GetAccess() const { return m_nFileAccess; }
	CF_FILE_FORMAT		GetFormat() const { return m_nFileFormat; }
	CF_FILE_TYPE		GetType() const { return m_nFileType; }
	size_t				GetPacketsOffset() const { return m_nPacketsPosition; }
	const path&			GetPath() const { return m_Path; }

	virtual tMediaType	GetMediaType() const = 0;
	virtual UInt64		GetSize() const = 0;

	virtual bool		IsEOF() const = 0;
	virtual bool		IsOpen() const = 0;

	virtual bool	Open( const path& inPath ) = 0;
	virtual bool	Open() {
		if ( std::filesystem::exists( m_Path ) ) {
			return Open( m_Path );
		}
		return IsOpen();
	}

	virtual UInt64	GetPosition() = 0;
	virtual UInt64	MoveTo( UInt64 inPosition ) = 0;		// Move to an absolute position.
	virtual UInt32	Read( UInt32 inLength, void* outData ) = 0;
	virtual UInt64	Seek( SInt64 inOffset ) = 0;			// Seek to a relative position.
	virtual void	SetPacketsPosition( size_t inPosition ) {
		m_nPacketsPosition = inPosition;
		MoveTo( m_nPacketsPosition );
	}
	virtual UInt32	Write( UInt32 inLength, const void* inData ) = 0;

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32& outError ) = 0;
	virtual bool	ReadFilePackets( UInt32 inCount, CFilePacketList& outPackets, UInt32& outError ) = 0;

	virtual bool	WriteFilePacket( const CFilePacket& inPacket ) = 0;
	virtual bool	WriteFilePackets( const CFilePacketList& inPackets, UInt32& outCount ) = 0;
};


// ============================================================================
//		CCaptureReader
// ============================================================================

class CCaptureReader
	:	public CBaseFile
{
protected:
	std::ifstream	m_File;

	virtual bool	Parse() = 0;

public:
	;		CCaptureReader( CF_FILE_FORMAT inFormat, CF_ACCESS_TYPE inAccess )
		:	CBaseFile( CF_TYPE_READER, inFormat, inAccess )
	{}
	;		CCaptureReader( const path& inPath, CF_FILE_FORMAT inFormat, CF_ACCESS_TYPE inAccess )
			:	CBaseFile( CF_TYPE_READER, inFormat, inAccess )
	{
		Open( inPath );
	}
	virtual ~CCaptureReader() {
		Close();
		CBaseFile::Close();
	}

	virtual	void	Close() {
		m_File.close();
		CBaseFile::Close();
	}
	virtual void	CopyMetaData( const CFileMetaData& inMetaData ) = 0;

	virtual tMediaType		GetMediaType() const = 0;
	virtual UInt64			GetSize() const {
		return std::filesystem::file_size( m_Path );
	}

	virtual bool	IsEOF() const { return m_File.eof(); }
	virtual bool	IsOpen() const { return m_File.is_open(); }

	virtual bool	Open( const path& inPath );

	virtual UInt64	GetPosition() { return m_File.tellg(); }
	virtual UInt64	MoveTo( UInt64 inPosition ) {
		m_File.seekg( inPosition, std::ios_base::beg );
		return GetPosition();
	}
	virtual UInt32	Read( UInt32 inLength, void* outData );
	virtual UInt64	Seek( SInt64 inOffset ) {
		m_File.seekg( inOffset, std::ios_base::cur );
		return GetPosition();
	}
	virtual UInt32	Write( UInt32 /*inLength*/, const void* /*inData*/ ) {
		assertm( false, "Can't write on a reader.");
		return 0;
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32& outError ) {
		CFilePacketList	fpl;
		bool			bResult = ReadFilePackets( 1, fpl, outError );
		if ( bResult && (outError == 0) ) {
			outPacket = fpl.front();
		}
		return bResult;
	}
	virtual bool	ReadFilePackets( UInt32 inCount, CFilePacketList& outPackets, UInt32& outError );

	virtual bool	WriteFilePacket( const CFilePacket& /*inPacket*/ ) {
		assertm( false, "Can't write on a reader.");
		return false;
	}
	virtual bool	WriteFilePackets( const CFilePacketList& /*inPackets*/, UInt32& /*outCount*/ ) {
		assertm( false, "Can't write on a reader.");
		return false;
	}
};


// ============================================================================
//		CCaptureWriter
// ============================================================================

class CCaptureWriter
	:	public CBaseFile
{
protected:
	std::ofstream	m_File;

public:
	;		CCaptureWriter( CF_FILE_FORMAT inFormat, CF_ACCESS_TYPE inAccess )
			:	CBaseFile( CF_TYPE_WRITER, inFormat, inAccess )
	{}
	;		CCaptureWriter( const path& inPath, CF_FILE_FORMAT inFormat,
							CF_ACCESS_TYPE inAccess )
			:	CBaseFile( CF_TYPE_WRITER, inFormat, inAccess )
	{
		Open( inPath );
	}
	virtual	~CCaptureWriter() {
		Close();
		CBaseFile::Close();
	}

	virtual void	Close() { m_File.close(); }
	virtual void	CopyMetaData( const CFileMetaData& inMetaData ) = 0;

	virtual tMediaType		GetMediaType() const = 0;
	virtual UInt64			GetSize() const {
		return std::filesystem::file_size( m_Path );
	}

	virtual void	Init() = 0;

	virtual bool	IsEOF() const { return m_File.eof(); }
	virtual bool	IsOpen() const { return m_File.is_open(); }

	virtual bool	Open( const path& inPath );

	virtual UInt64	GetPosition() { return m_File.tellp(); }
	virtual UInt64	MoveTo( UInt64 inPosition ) {
		m_File.seekp( inPosition, std::ios_base::beg );
		return GetPosition();
	}
	UInt32	Read( UInt32 /*inLength*/, void* /*outData*/ ) {
		assertm( false, "Can't read on a writer.");
		return 0;
	}
	virtual UInt64	Seek( SInt64 inOffset ) {
		m_File.seekp( inOffset, std::ios_base::cur );
		return GetPosition();
	}
	UInt32	Write( UInt32 inLength, const void* inData );

	virtual bool	ReadFilePacket( CFilePacket& /*outPacket*/, UInt32& /*outError*/ ) {
		assertm( false, "Can't read on a writer.");
		return false;
	}
	virtual bool	ReadFilePackets( UInt32 /*inCount*/, CFilePacketList& /*outPackets*/, UInt32& /*outError*/ ) {
		assertm( false, "Can't read on a writer.");
		return false;
	}

	virtual bool	WriteFilePacket( const CFilePacket& inPacket ) {
		UInt32			nCount( 0 );
		CFilePacketList	cpl;
		cpl.push_back( inPacket );

		bool	bResult = WriteFilePackets( cpl, nCount );
		if ( bResult ) {
			bResult = nCount == 1;
		}
		return bResult;
	}
	virtual bool	WriteFilePackets( const CFilePacketList& inPackets, UInt32& outCount );
};


// ============================================================================
//		CCaptureFile
// ============================================================================

class CCaptureFile
{
public:
	CBaseFile*	m_pFile;

protected:
	static CCaptureReader*	OpenReader( const path& inPath );
	static CCaptureWriter*	OpenWriter( const path& inPath, const tFileMode& inMode );

public:
	static CF_FILE_FORMAT	GetFileFormat( const path& inPath );

public:
	;		CCaptureFile( const CCaptureFile& inOther )
			:	m_pFile( inOther.m_pFile )
	{}
	;		CCaptureFile( const path& inPath, const tFileMode& inMode )
			:	m_pFile{}
	{
		Open( inPath, inMode );
	}
	;		CCaptureFile( const path& inPath, CF_FILE_TYPE inType )
			:	m_pFile{}
	{
		Open( inPath, inType );
	}
	virtual	~CCaptureFile() { Close(); }

	;	operator tFileMode() const {
		tFileMode	fm( GetType(), GetFormat(), GetAccess(), false, GetMediaType() );
		return fm;
	}

	virtual void		CopyMetaData( const CFileMetaData& inData ) {
		if ( m_pFile ) {
			m_pFile->CopyMetaData( inData );
		}
	}
	virtual void		Close() {
		if ( m_pFile ) {
			m_pFile->Close();
			m_pFile = nullptr;
		}
	}

	CF_ACCESS_TYPE		GetAccess() const {
		return (m_pFile)
			? m_pFile->GetAccess()
			: CF_ACCESS_UNKNOWN;
	}
	CF_FILE_FORMAT		GetFormat() const {
		return (m_pFile)
			?	m_pFile->GetFormat()
			:	CF_FORMAT_UNKNOWN;
	}
	virtual tMediaType	GetMediaType() const {
		return (m_pFile)
			?	m_pFile->GetMediaType()
			:	kMediaType_Ethernet;
	}
	UInt64				GetSize() const {
		return (m_pFile)
			?	m_pFile->GetSize()
			:	0;
	}
	CF_FILE_TYPE		GetType() const {
		return (m_pFile)
			?	m_pFile->GetType()
			:	CF_TYPE_UNKNOWN;
	}

	bool				IsEOF() {
		return (m_pFile)
			? m_pFile->IsEOF()
			: true;
	}

	bool				IsOpen() const {
		return (m_pFile)
			?	m_pFile->IsOpen()
			:	false;
	}
	bool				IsReader() const {
		return (m_pFile)
			?	(m_pFile->GetType() == CF_TYPE_READER)
			:	false;
	}
	bool				IsWriter() const {
		return (m_pFile)
			?	(m_pFile->GetType() == CF_TYPE_WRITER)
			:	false;
	}

	virtual bool	Open( const path& inPath, CF_FILE_TYPE inType ) {
		tFileMode	fm( inType, CF_FORMAT_UNKNOWN );
		return Open( inPath, fm );
	};
	virtual bool	Open( const path& inPath, const tFileMode& inMode );

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32& outError  ) {
		outError = CF_ERROR_EOF;
		return (m_pFile)
			?	m_pFile->ReadFilePacket( outPacket, outError )
			: false;
	}
	virtual bool	ReadFilePackets( UInt32 inCount, CFilePacketList& outPackets, UInt32& outError ) {
		outError = CF_ERROR_EOF;
		return (m_pFile)
			?	m_pFile->ReadFilePackets( inCount, outPackets, outError )
			: false;
	}

	virtual bool	WriteFilePacket( const CFilePacket& inPacket ) {
		return (m_pFile)
			? m_pFile->WriteFilePacket( inPacket )
			: false;
	}
	virtual bool	WriteFilePackets( const CFilePacketList& inPackets, UInt32& outCount ) {
		return (m_pFile)
			? m_pFile->WriteFilePackets( inPackets, outCount )
			: false;
	}
};

} // namespace CaptureFile
