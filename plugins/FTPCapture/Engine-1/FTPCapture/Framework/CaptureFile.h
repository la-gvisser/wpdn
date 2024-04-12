// ============================================================================
//	CaptureFile.h
//		interface for CCaptureFile class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "TagFile.h"
#include "MediaTypes.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#define CF_PEEK_FILE
#define CF_PCAP_FILE
#define CF_USE_TRANSFORM


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
//	and WP Types. A StdAfx.h is also required. Or download the package from
//	MyPeek.
//
//	Capture File is part of the Capture File module.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

#ifdef _WIN32

// Set up __BYTE_ORDER
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN	1234
#endif
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN	4321
#endif
#ifndef __PDP_ENDIAN
#define __PDP_ENDIAN	3412
#endif
#define __BYTE_ORDER	__LITTLE_ENDIAN

// Set up bswap_16, bswap_32 and bswap_64
#include <stdlib.h>
#ifndef bswap_16
#define bswap_16(x)		_byteswap_ushort(x)
#endif
#ifndef bswap_32
#define bswap_32(x)		_byteswap_ulong(x)
#endif
#ifndef bswap_64
#define bswap_64(x)		_byteswap_uint64(x)
#endif

#pragma intrinsic(_byteswap_ushort,_byteswap_ulong,_byteswap_uint64)

#ifndef NETWORKTOHOST16
#define NETWORKTOHOST16(x)		(bswap_16(x))
#endif
#ifndef HOSTTONETWORK16
#define HOSTTONETWORK16(x)		(bswap_16(x))
#endif
#ifndef NETWORKTOHOST32
#define NETWORKTOHOST32(x)		(bswap_32(x))
#endif
#ifndef HOSTTONETWORK32
#define HOSTTONETWORK32(x)		(bswap_32(x))
#endif
#ifndef NETWORKTOHOST64
#define NETWORKTOHOST64(x)		(bswap_64(x))
#endif
#ifndef HOSTTONETWORK64
#define HOSTTONETWORK64(x)		(bswap_64(x))
#endif

#endif

inline UInt16 Swap16( UInt16 x ) { return _byteswap_ushort( x ); }
inline UInt32 Swap32( UInt32 x ) { return _byteswap_ulong( x ); }

inline UInt16 SwapIf16( bool b, UInt16 x ) { return (b) ? _byteswap_ushort( x ) : (x); }
inline UInt32 SwapIf32( bool b, UInt32 x ) { return (b) ? _byteswap_ulong( x ) : (x); }

std::wstring AtoW( const char* inOther, size_t inLength );
std::wstring AtoW( const char* inOther );
std::wstring AtoW( const std::string& inOther );

std::string WtoA( const wchar_t* inOther, size_t inLength );
std::string WtoA( const wchar_t* inOther );
std::string WtoA( const std::wstring& inOther );

class CBlock;
class CBlockNameList;
class CPacketNameList;
class CCaptureReader;
class CCaptureWriter;
class CCaptureFile;

typedef unsigned char		byte;
typedef std::vector<byte>	CByteVector;

// File Types
enum {
	CF_TYPE_UNKNOWN,
	CF_TYPE_READER,
	CF_TYPE_WRITER,
	CF_TYPE_MAX
};

// File Formats
enum {
	CF_FORMAT_UNKNOWN,
	CF_FORMAT_PEEK,
	CF_FORMAT_PCAP,
	CF_FORMAT_MAX
};

// File Access
enum {
	CF_ACCESS_UNKNOWN,
	CF_ACCESS_SERIAL,
	CF_ACCESS_RANDOM,
	CF_ACCESS_MAX
};

#define MT_ETHERNET		tMediaType(kMediaType_802_3,kMediaSubType_Native)
#define MT_WIRELESS		tMediaType(kMediaSubType_802_11_gen,kMediaSubType_Native)

#define NEXT_PACKET		(-1)
#define OFFSET_EOF		(static_cast<size_t>( -1 ))
#define Attrib_Invalid	(static_cast<UInt16>( -1 ))

typedef struct _tFileMode {
	int			fType;
	bool		fScan;
	int			fAccess;
	tMediaType	fMediaType;

	struct _tFileMode( int inType, bool inScan = false,
			int inAccess = CF_ACCESS_SERIAL,
			tMediaType inMediaType = MT_ETHERNET ) {
		fType = inType;
		fAccess = inAccess;
		fScan = inScan;
		fMediaType = inMediaType;
	}
	struct _tFileMode( int inType, int inAccess,
			bool inScan = false, tMediaType inMediaType = MT_ETHERNET ) {
		fType = inType;
		fAccess = inAccess;
		fScan = inScan;
		fMediaType = inMediaType;
	}

	bool	IsRandom() const { return (fAccess == CF_ACCESS_RANDOM); }
	bool	IsReader() const { return (fType == CF_TYPE_READER); }
	bool	IsScan() const { return fScan; }
	bool	IsSerial() const { return (fAccess == CF_ACCESS_SERIAL); }
	bool	IsWriter() const { return (fType == CF_TYPE_WRITER); }

} tFileMode;

static const UInt32 kMaxUInt32 = (static_cast<UInt32>( -1 ));
static const size_t kMaxIndex = (static_cast<size_t>( -1 ));

// Seconds between 1/1/1601 and 1/1/1970
// (see KB167296: http://support.microsoft.com/default.aspx?scid=KB;en-us;q167296).
static const UInt64 kFileTimeToAnsi = 11644473600ull;
static const UInt64 kPeekTimeToAnsi = kFileTimeToAnsi;
static const UInt64 kResolutionPerSecond = 1000000000ull;		// nanoseconds
// number of Peek Time counts in a File Time.
static const UInt64 kPeekTime_FileTime = 100ull;
static const UInt64	kSecondsPerDay = 86400ull;
static const UInt64	kSecondsPerHour = 3600ull;
static const UInt64	kSecondsPerMinute = 60ull;
static const SInt32	kDaysPerQuadriCentennium  = (365 * 400) + 97;
static const SInt32	kDaysPerNormalQuadrennium = (365 * 4) + 1;

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
static const UInt32 s_ay802_11_Rates[] = {	// In 0.5 Mbs units.
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


extern int	CaptureFile( size_t nFileSize, byte* pFileData );

tFileMode	FileModeReader( const tFileMode inMode );
tFileMode	FileModeWriter( const tFileMode inMode );

UInt32		ChannelFrequency( UInt32 inChannel );


// -----------------------------------------------------------------------------
//		tVersion
// -----------------------------------------------------------------------------

typedef struct _tVersion {
	UInt32	fMajorVersion;
	UInt32	fMinorVersion;
	UInt32	fMajorRevision;
	UInt32	fMinorRevision;

	struct _tVersion() : fMajorVersion( 0 ), fMinorVersion( 0 ), fMajorRevision( 0 ), fMinorRevision( 0 ) {}
	std::wstring	Format() const {
		std::wostringstream	ss;
		ss << fMajorVersion << L"." << fMinorVersion << L"." << fMajorRevision
			<< L"." << fMinorRevision;
		return ss.str();
	}
	std::string	FormatA() const {
		std::ostringstream	ss;
		ss << fMajorVersion << "." << fMinorVersion << "." << fMajorRevision
			<< "." << fMinorRevision;
		return ss.str();
	}
	bool	IsNull() const { return (fMajorVersion == 0) && (fMinorVersion == 0) &&
		(fMajorRevision == 0) && (fMinorRevision == 0);
	}
	void	Parse( const std::wstring& inVersion ) {
		wchar_t				cDot;
		std::wistringstream	ssVersion( inVersion );
		ssVersion >> fMajorVersion >> cDot >> fMajorRevision >>
			cDot >> fMinorVersion >> cDot >> fMinorRevision;
	}
	void	ParseA( const std::string& inVersion ) {
		char				cDot;
		std::istringstream	ssVersion( inVersion );
		ssVersion >> fMajorVersion >> cDot >> fMajorRevision >>
			cDot >> fMinorVersion >> cDot >> fMinorRevision;
	}
	void	Reset() { fMajorVersion = 0; fMinorVersion = 0;
		fMajorRevision = 0; fMinorRevision = 0;
	}
} tVersion;



// -----------------------------------------------------------------------------
//		tTimeFields
// -----------------------------------------------------------------------------

typedef struct _tTimeFields{
	UInt32	fYear;
	UInt32	fMonth;
	UInt32	fDay;
	UInt32	fHour;
	UInt32	fMinute;
	UInt32	fSecond;
	UInt32	fNanosecond;

	struct _tTimeFields( UInt64 inTime = 0 ) {
		Encode( inTime );
	}
	UInt64	Decode();
	void	Encode( UInt64 inTime );
	void	Reset() { fYear = 0; fMonth = 0; fDay = 0; fHour = 0; fMinute = 0;
		fSecond = 0; fNanosecond = 0;
	}
} tTimeFields;


// -----------------------------------------------------------------------------
//		tLongTime
// -----------------------------------------------------------------------------

typedef union _tLongTime {
	UInt64		i;
	struct {
		UInt32	TimeLo;
		UInt32	TimeHi;
	} sTime;
	FILETIME	ft;

	union _tLongTime( UInt64 inTime = 0 ) : i( inTime ) {}
	bool	IsNull() const { return (i == 0); }
	static _tLongTime Now() {
		tLongTime ltNow;
		::GetSystemTimeAsFileTime( &ltNow.ft );
		ltNow.i *= kPeekTime_FileTime;
		return ltNow;
	}
	void	Reset() { i = 0; }
} tLongTime;


// ============================================================================
//		CFileTime
// ============================================================================

class CFileTime
{
public:
	typedef enum {
		kFormat_Nanoseconds,
		kFormat_Milliseconds
	} tTimeFormat;

protected:
	tLongTime	m_ltTime;	// Nanoseconds since 1/1/1601.

public:
	static UInt32	GetTimeBias() {
		TIME_ZONE_INFORMATION	tzInfo;
		DWORD dwResult = ::GetTimeZoneInformation( &tzInfo );
		if ( dwResult == TIME_ZONE_ID_INVALID ) {
			return 0;
		}
		return tzInfo.Bias;
	}

	static CFileTime	Now() { return tLongTime::Now(); }

public:
	;		CFileTime() {}
	;		CFileTime( const tLongTime inTime ) : m_ltTime( inTime ) {}
	;		CFileTime( UInt64 inTime )
	{
		m_ltTime.i = inTime;
	}
	;		CFileTime( FILETIME inTime ) {
		m_ltTime.ft = inTime;
		m_ltTime.i *= kPeekTime_FileTime;
	}
	;		CFileTime( const std::wstring& inTime )
	{
		Parse( inTime );
	}
	;		CFileTime( const std::string& inTime )
	{
		Parse( inTime );
	}

	;		operator UInt64() const { return m_ltTime.i; }
	;		operator FILETIME() const {
		tLongTime	ltTime( m_ltTime.i / kPeekTime_FileTime );
		return ltTime.ft;
	}

	std::wstring	Format( tTimeFormat inFormat = kFormat_Nanoseconds );
	std::string		FormatA( tTimeFormat inFormat = kFormat_Nanoseconds );

	bool	IsNull() const { return (m_ltTime.i == 0); }

	void	Parse( const std::wstring& inTime );
	void	Parse( const std::string& inTime );

	void	Reset() { m_ltTime.Reset(); }
};


// ============================================================================
//		CBlockNameList
// ============================================================================

class CBlockNameList
{
public:
	typedef enum {
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
	} BlockAttrib;

protected:
	std::vector<std::string>	m_ayNames;

protected:
	void	AddName( wchar_t* inName ) {
		m_ayNames.push_back( WtoA( inName ) );
	}

public:
	;		CBlockNameList();


	const std::string&	GetName( size_t inIndex ) {
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
	std::vector<std::string>	m_ayNames;

	void	AddName( char* inName ) {
		m_ayNames.push_back( inName );
	}

public:
	;		CAttributeNameList();

	const std::string&	GetName( size_t inIndex ) {
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
	UInt16		m_nId;
	std::string	m_strName;

public:
	static const std::string&	GetBlockName( size_t inIndex ) {
		return s_BlockNames.GetName( inIndex );
	}
	static const std::string&	GetAttributeName( size_t inIndex ) {
		return s_AttributeNames.GetName( inIndex );
	}

public:
	;		CAttribute() : m_nId( Attrib_Invalid ) {}
	;		CAttribute( UInt16 inId, const char* inName )
		:	m_nId( inId )
		,	m_strName( inName )
	{}
	;		CAttribute( UInt16 inId, const std::string& inName )
		:	m_nId( inId )
		,	m_strName( inName )
	{}

	UInt16				GetId() const { return m_nId; }
	const std::string&	GetName() const { return m_strName; }
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
	std::string		m_strValue;

public:
	;		CBlockAttribute() {}
	;		CBlockAttribute( UInt16 inId, const std::string& inValue )
		: CAttribute( inId, CAttribute::GetBlockName( inId ) ), m_strValue( inValue )
	{}
	;		CBlockAttribute( UInt16 inId, const char * inValue )
		: CAttribute( inId, CAttribute::GetBlockName( inId ) ), m_strValue( inValue )
	{}

	const std::string&	GetValue() const { return m_strValue; }

	void	SetValue( const char* inValue ) { m_strValue = inValue; }
	void	SetValue( const std::string& inValue ) { m_strValue = inValue; }
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
	size_t	Add( UInt16 inId, const std::string& inValue ) {
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
	size_t	ListAdd( UInt16 inId, const std::string& inValue ) {
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
//		CWinFile
// ============================================================================

class CWinFile
{
public:
	enum {
		FILEMODE_READ = 0x01,
		FILEMODE_WRITE = 0x02,
		FILEMODE_CREATE = 0x04,
	};

protected:
	HANDLE			m_hFile;
	std::wstring	m_strPathName;	// Fully qualified file and path name.
	std::wstring	m_strFileName;	// Just the file name.

protected:
	bool	ParseMode( UInt32 inMode, UInt32& outAccess, UInt32& outCreate );

	void	SetName( const std::wstring& inName );

public:
	;		CWinFile( HANDLE inFile = INVALID_HANDLE_VALUE )
		:	m_hFile( inFile )
	{}

	void	Close();

	std::wstring	GetFileName() const { return m_strFileName; }	// Return just the file name.
	std::string		GetFileNameA() const { return WtoA( m_strFileName ); }
	std::wstring	GetPathName() const { return m_strPathName; }	// Returns fully qualified file and pathname.
	std::string		GetPathNameA() const { return WtoA( m_strPathName ); }
	UInt64			GetSize() const;		// Returns size of the file.
	UInt64			GetPosition() const;	// Returns current file position.

	bool	IsOpen() const { return (m_hFile != INVALID_HANDLE_VALUE); }

	bool	Open( const wchar_t* inFileName, UInt32 inMode ) {
		return Open( std::wstring( inFileName ), inMode );
	}
	bool	Open( const std::wstring& inFileName, UInt32 inMode );
	bool	Open( const char* inFileName, UInt32 inMode ) {
		return Open( std::string( inFileName ), inMode );
	}
	bool	Open( const std::string& inFileName, UInt32 inMode ) {
		return Open( AtoW( inFileName ), inMode );
	}

	UInt64	MoveTo( SInt64 inOffset );		// Move to a relative position.

	UInt32	Read( UInt32 inLength, void* outData );

	UInt64	SeekTo( UInt64 inPosition );	// Move to an absolute position.

	UInt32	Write( UInt32 inLength, const void* inData );
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
	//	_ASSERTE( 0 );
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
		_ASSERTE( m_nHighestIndex < kMaxUInt32 );
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
			_ASSERTE( m_nFileSize > 0 );
			UInt64	nMoreOffsets = (m_nFileSize > 0)
				? ((m_nFileSize - inOffset) / 256 )
				: m_ayOffsets.size();
			_ASSERTE( nMoreOffsets < kMaxUInt32 );
			if ( nMoreOffsets >= kMaxUInt32 ) {
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
	static const int	s_dBmCount = 4;

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
		for ( int i = 0; i < s_dBmCount; i++ ) {
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
			for ( int i = 0; i < s_dBmCount; i++ ) {
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
		for ( int i = 0; i < s_dBmCount; i++ ) outList[i] = m_nNoisedBm[i];
	}
	UInt32			GetNoiseStrength() const { return m_nNoiseStrength; }
	UInt8*			GetPacketData() { return (m_PacketData.empty()) ? NULL : &m_PacketData[0]; }
	const UInt8*	GetPacketData() const { return (m_PacketData.empty()) ? NULL : &m_PacketData[0]; }
	size_t			GetPacketDataLength() const { return m_PacketData.size(); }
	UInt32			GetProtocol() const { return m_nProtocol; }
	SInt32			GetSignaldBm( size_t inIndex ) const { return (inIndex < s_dBmCount) ? m_nSignaldBm[inIndex] : 0; }
	void			GetSignaldBm( SInt32* outList ) const {
		for ( int i = 0; i < s_dBmCount; i++ ) outList[i] = m_nSignaldBm[i];
	}
	UInt32			GetSignalStrength() const { return m_nSignalStrength; }
	size_t			GetSliceLength() const { return (GetLength() == GetPacketDataLength()) ? 0 : GetPacketDataLength(); }
	UInt16			GetStatus() const { return m_nStatus; }
	UInt64			GetTimeStamp() const { return m_nTimeStamp; }

	bool	IsMutable() const { return !m_PacketData.empty(); }
	bool	IsSliced() const { return (GetPacketDataLength() < m_nLength); }
	bool	IsWAN() const { return m_MediaType.IsWAN(); }
	bool	IsWireless() const { return m_MediaType.IsWireless(); }


	void	ResetPacketData(){ SetPacketData( 0, 0, NULL ); }

	void	SetAttributes( const CPacketAttributeList& inAttribs, UInt32& outDataLength );
	void	SetChannelBand( UInt32 inBand ) { m_nChannelBand = inBand; }
	void	SetChannelFreq( UInt32 inFreq ) { m_nChannelFreq = inFreq; }
	void	SetChannelNumber( UInt32 inChannel ) { m_nChannelNumber = inChannel; }
	void	SetDataRate( UInt32 inRate ) { m_nDataRate = inRate; }
	void	SetDirection( UInt32 inDirection ) { m_nDirection = inDirection; }
	void	SetFlags( UInt16 inFlags ) { m_nFlags = inFlags; }
	void	SetMediaSpecType( UInt32 inType ) { m_nMediaSpecType = inType; }
	void	SetMediaType( tMediaType inMediaType ) { m_MediaType = inMediaType; }
	void	SetNoisedBm( size_t inIndex, SInt32 indBm ) { if ( inIndex < s_dBmCount ) m_nNoisedBm[inIndex] = indBm; }
	void	SetNoisedBm( UInt32* inList ) {
		for ( int i = 0; i < s_dBmCount; i++ ) m_nNoisedBm[i] = inList[i];
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
	void	SetSignaldBm( size_t inIndex, UInt32 indBm ) { if ( inIndex < s_dBmCount ) m_nSignaldBm[inIndex] = indBm; }
	void	SetSignaldBm( UInt32* inList ) {
		for ( int i = 0; i < s_dBmCount; i++ ) m_nSignaldBm[i] = inList[i];
	}
	void	SetSignalStrength( UInt32 inStrength ) { m_nSignalStrength = inStrength; }
	void	SetStatus( UInt16 inStatus ) { m_nStatus = inStatus; }
	void	SetTimeStamp( UInt64 inTime ) { m_nTimeStamp = inTime; }

#if (0)
	size_t	Load( size_t inLength, const byte* inData );
	bool	Load( CWinFile& inFile, UInt64 inOffset );
	size_t	Store( CByteVector& inData ) const;
	bool	Store( CWinFile& inFile ) const;
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
	:	public std::string
{
public:
	;		CXmlString() {}
	;		CXmlString( const std::string& inOther );

	void	AddAttribute( const std::string& inAttribute, const std::string& inValue );

	bool	GetAttribute( const std::string& inAttribute, std::string& outValue ) const;
	bool	GetAttribute( const std::string& inAttribute, size_t& ioOffset, std::string& outValue ) const;

	void	Load( const std::string& inData ) { assign( inData ); }
	void	Load( size_t inLength, const byte* inData );

	void	Set( const std::string& inName );
	void	SetAttribute( const std::string& inAttribute, const std::string& inValue );
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
	std::string		m_Time;
	UInt32			m_TimeZoneBias;
	UInt32			m_LinkSpeed;
	UInt32			m_PacketCount;
	std::string		m_Comment;
	std::string		m_SessionStartTime;
	std::string		m_SessionEndTime;
	std::string		m_AdapterName;
	std::string		m_AdapterAddr;
	std::string		m_CaptureName;
	std::string		m_CaptureId;
	std::string		m_Owner;
	UInt32			m_FileIndex;
	std::string		m_Host;
	std::string		m_EngineName;
	std::string		m_EngineAddr;
	std::string		m_MediaDomain;
	std::string		m_DataRates;
	std::string		m_ChannelList;
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
private:
	CWinFile		m_File;

protected:
	int				m_nFileType;
	int				m_nFileFormat;
	int				m_nFileAccess;

public:
	;		CBaseFile( int inType, int inFormat, int inAccess )
		:	m_nFileType( inType )
		,	m_nFileFormat( inFormat )
		,	m_nFileAccess( inAccess )
	{}
	virtual	~CBaseFile() { Close(); }

	virtual void	CopyMetaData( const CCaptureFile& inOther ) = 0;
	virtual	void	Close() { m_File.Close(); }

	virtual CWinFile&	GetFile() { return m_File; }
	int					GetFileAccess() const { return m_nFileAccess; }
	int					GetFileFormat() const { return m_nFileFormat; }
	std::wstring		GetFileName() const { return m_File.GetFileName(); }
	std::string			GetFileNameA() const { return m_File.GetFileNameA(); }
	UInt64				GetFileSize() const { return m_File.GetSize(); }
	int					GetFileType() const { return m_nFileType; }
	virtual tMediaType	GetMediaType() const = 0;
	virtual UInt32		GetPacketCount() const = 0;
	std::wstring		GetPathName() const { return m_File.GetPathName(); }
	std::string			GetPathNameA() const { return m_File.GetPathNameA(); }

	bool			IsOpen() const { return m_File.IsOpen(); }

	virtual bool	Open( const std::wstring& inFileName, int inMode );
	virtual bool	Open( const wchar_t* inFileName, int inMode ) {
		return Open( std::wstring( inFileName ), inMode );
	}
	virtual bool	Open( const std::string& inFileName, int inMode ) {
		return Open( AtoW( inFileName ), inMode );
	}
	virtual bool	Open( const char* inFileName, int inMode ) {
		return Open( AtoW( inFileName ), inMode );
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32 inIndex = NEXT_PACKET ) = 0;
	virtual bool	ReadFilePackets( CFilePacketList& outPackets, UInt32 inCount, UInt32 inFirst = NEXT_PACKET ) = 0;

	virtual bool	WriteFilePacket( const CFilePacket& inPacket ) = 0;
	virtual bool	WriteFilePackets( const CFilePacketList& inPackets, UInt32& outCount ) = 0;
};


// ============================================================================
//		CBaseFilePtr
// ============================================================================

class CBaseFilePtr
	:	public std::auto_ptr<CBaseFile>
{
public:
	;		CBaseFilePtr( CBaseFile* inBaseFile = NULL )
		:	std::auto_ptr<CBaseFile>( inBaseFile )
	{}

	bool	IsValid() const { return (get() != NULL); }
};


// ============================================================================
//		CCaptureReader
// ============================================================================

class CCaptureReader
	:	public CBaseFile
{
protected:
	virtual void	Parse( bool inScanPackets ) = 0;

public:
	;		CCaptureReader(
		int inFormat = CF_FORMAT_UNKNOWN,
		int inAccess = CF_ACCESS_UNKNOWN )
		:	CBaseFile( CF_TYPE_READER, inFormat, inAccess )
	{}
	virtual ~CCaptureReader() {}

	virtual UInt32	GetPacketCount() const { return 0; }

	virtual bool	Open( const std::wstring& inFileName, bool inScanPackets );
	virtual bool	Open( const wchar_t* inFileName, bool inScanPackets ) = 0;
	virtual bool	Open( const std::string& inFileName, bool inScanPackets ) = 0;
	virtual bool	Open( const char* inFileName, bool inScanPackets ) = 0;

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32 inIndex ) = 0;
	virtual bool	ReadFilePackets( CFilePacketList& outPackets, UInt32 inCount, UInt32 inFirst = NEXT_PACKET );

	virtual bool	WriteFilePacket( const CFilePacket& /*inPacket*/ ) {
		return false;
	}
	virtual bool	WriteFilePackets( const CFilePacketList& /*inPackets*/,
		UInt32& /*outCount*/ ) {
		return false;
	}
};


// ============================================================================
//		CCaptureWriter
// ============================================================================

class CCaptureWriter
	:	public CBaseFile
{
public:
	;		CCaptureWriter(
		int inFormat = CF_FORMAT_UNKNOWN,
		int inAccess = CF_ACCESS_UNKNOWN )
		:	CBaseFile( CF_TYPE_WRITER, inFormat, inAccess )
	{}
	virtual	~CCaptureWriter() {}

	virtual UInt32	GetPacketCount() const { return 0; }

	virtual void	Init( const CCaptureFile* inTemplate = NULL ) = 0;

	virtual bool	Open( const std::wstring& inFileName );
	virtual bool	Open( const wchar_t* inFileName ) = 0;
	virtual bool	Open( const std::string& inFileName ) = 0;
	virtual bool	Open( const char* inFileName ) = 0;

	virtual bool	ReadFilePacket( CFilePacket& /*outPacket*/, UInt32 /*inIndex*/ ) {
		return false;
	}
	virtual bool	ReadFilePackets( CFilePacketList& /*outPackets*/,
		UInt32 /*inCount*/, UInt32 /*inFirst*/ = NEXT_PACKET ) {
		return false;
	}
	virtual bool	WriteFilePacket( const CFilePacket& inPacket ) = 0;
	virtual bool	WriteFilePackets( const CFilePacketList& inPackets, UInt32& outCount );
};


// ============================================================================
//		CCaptureFile
// ============================================================================

class CCaptureFile
{
public:

private:
	CBaseFilePtr	m_spBaseFile;

protected:
	static CCaptureReader*	OpenReader( const std::wstring& inFileName, const tFileMode& inMode );
	static CCaptureWriter*	OpenWriter( const std::wstring& inFileName, const tFileMode& inMode );

public:
	static int	GetFileType( const std::wstring& inFileName );

public:
	;		CCaptureFile( const std::wstring& inFileName, const tFileMode& inMode ) {
		if ( !inFileName.empty() ) {
			Open( inFileName, inMode );
		}
	}
	;		CCaptureFile( const std::wstring& inFileName, int inType, const CCaptureFile& inOther ) {
		if ( !inFileName.empty() ) {
			tFileMode	fm = inOther;
			fm.fType = inType;
			Open( inFileName, fm );
			if ( IsOpen() ) {
				CopyMetaData( inOther );
			}
		}
	}
	;		CCaptureFile( const wchar_t* inFileName, const tFileMode& inMode ) {
		if ( inFileName != NULL ) {
			Open( inFileName, inMode );
		}
	}
	;		CCaptureFile( const std::string& inFileName, const tFileMode& inMode ) {
		if ( !inFileName.empty() ) {
			Open( inFileName, inMode );
		}
	}
	;		CCaptureFile( const char* inFileName, const tFileMode& inMode ) {
		if ( inFileName != NULL ) {
			Open( std::string( inFileName ), inMode );
		}
	}
	virtual	~CCaptureFile() { Close(); }

	;	operator tFileMode() const {
		tFileMode	fm( GetFileType(), false, GetFileAccess(), GetMediaType() );
		return fm;
	}

	virtual void		CopyMetaData( const CCaptureFile& inOther ) {
		if ( m_spBaseFile.IsValid() ) {
			m_spBaseFile->CopyMetaData( inOther );
		}
	}
	virtual void		Close() {
		if ( m_spBaseFile.IsValid() ) {
			m_spBaseFile->Close();
		}
	}

	virtual CWinFile&	GetFile() {
		CWinFile	wf;
		return (m_spBaseFile.IsValid())
			? m_spBaseFile->GetFile()
			: wf;
	}
	int					GetFileAccess() const {
		return (m_spBaseFile.IsValid())
			? m_spBaseFile->GetFileAccess()
			: CF_ACCESS_UNKNOWN;
	}
	int					GetFileFormat() const {
		return (m_spBaseFile.IsValid())
			? m_spBaseFile->GetFileFormat()
			: CF_FORMAT_UNKNOWN;
	}
	std::wstring		GetFileName() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetFileName()
			:	std::wstring();
	}
	std::string			GetFileNameA() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetFileNameA()
			:	std::string();
	}
	UInt64				GetFileSize() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetFileSize()
			:	0;
	}
	int					GetFileType() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetFileType()
			:	CF_TYPE_UNKNOWN;
	}
	virtual tMediaType	GetMediaType() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetMediaType()
			:	MT_ETHERNET;
	}
	virtual UInt32		GetPacketCount() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetPacketCount()
			:	0;
	}
	std::wstring		GetPathName() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetPathName()
			:	std::wstring();
	}
	std::string			GetPathNameA() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->GetPathNameA()
			:	std::string();
	}

	bool				IsOpen() const {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->IsOpen()
			:	false;
	}
	bool				IsReader() const {
		return (m_spBaseFile.IsValid())
			?	(m_spBaseFile->GetFileType() == CF_TYPE_READER)
			:	false;
	}
	bool				IsWriter() const {
		return (m_spBaseFile.IsValid())
			?	(m_spBaseFile->GetFileType() == CF_TYPE_WRITER)
			:	false;
	}

	virtual bool	Open( const std::wstring& inFileName, const tFileMode& inMode );
	virtual bool	Open( const std::string& inFileName, const tFileMode& inMode ) {
		return Open( AtoW( inFileName ), inMode );
	}

	virtual bool	ReadFilePacket( CFilePacket& outPacket, UInt32 inIndex = NEXT_PACKET  ) {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->ReadFilePacket( outPacket, inIndex )
			: false;
	}
	virtual bool	ReadFilePackets( CFilePacketList& outPackets, UInt32 inCount, UInt32 inFirst = NEXT_PACKET ) {
		return (m_spBaseFile.IsValid())
			?	m_spBaseFile->ReadFilePackets( outPackets, inCount, inFirst )
			: false;
	}

	virtual bool	WriteFilePacket( const CFilePacket& inPacket ) {
		return (m_spBaseFile.IsValid())
			? m_spBaseFile->WriteFilePacket( inPacket )
			: false;
	}
	virtual bool	WriteFilePackets( const CFilePacketList& inPackets, UInt32& outCount ) {
		return (m_spBaseFile.IsValid())
			? m_spBaseFile->WriteFilePackets( inPackets, outCount )
			: false;
	}
};

} // namespace CaptureFile
