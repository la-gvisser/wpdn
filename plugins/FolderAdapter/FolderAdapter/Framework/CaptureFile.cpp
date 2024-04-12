// =============================================================================
//	CaptureFile.cpp
//		implementation of the CCaptureFile class.
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "CaptureFile.h"
#include "FileEx.h"
#include "zlib.h"

#ifdef CF_PEEK_FILE
#include "PeekCaptureFile.h"
#endif

#ifdef CF_PCAP_FILE
#include "PCapCaptureFile.h"
#include "PCapNgCaptureFile.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <cwchar>
#include <memory>
#include <sstream>
#include <vector>

#ifdef _WIN32
#define CRT_OPEN				_open
#else
#define CRT_OPEN				open
#define INVALID_HANDLE_VALUE	nullptr
#define MAX_PATH				260
#endif

#ifdef WP_LINUX
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#endif

using std::string;
using std::unique_ptr;
using std::wstring;
using std::stringstream;
using namespace CaptureFile;
using namespace FilePath;
using namespace TaggedFile;

extern size_t wcslen( const wchar_t* );


namespace CaptureFile {

// ============================================================================
//		Globals
// ============================================================================

CBlockNameList		CAttribute::s_BlockNames;
CAttributeNameList	CAttribute::s_AttributeNames;

#ifdef _WIN32
static CWinFile			s_WinFile;
#else
CStandardFile			s_StandardFile;
#endif


// -----------------------------------------------------------------------------
//      ChannelFrequency
// -----------------------------------------------------------------------------

UInt32
ChannelFrequency( UInt32 inChannel )
{
	return (inChannel <= 14 )
		? s_n802_11_BaseFrequecy_B + (inChannel * 5)
		: s_n802_11_BaseFrequecy_A + (inChannel * 5);
}


// ============================================================================
//		tTimeFields
// ============================================================================

// -----------------------------------------------------------------------------
//		Decode
// -----------------------------------------------------------------------------

UInt64
tTimeFields::Decode()
{
	const UInt32	nMonths = (fMonth < 3) ? (fMonth + 13) : (fMonth + 1);
	const UInt32	nYears = (fMonth < 3) ? (fYear - 1) : fYear;
	const UInt32	nLeaps = (3 * (nYears / 100) + 3) / 4;
	const UInt32	nDays  = ((36525 * nYears) / 100) - nLeaps + ((1959 * nMonths) / 64) + fDay - 584817;

	return (((((((static_cast<UInt64>( nDays * 24 ) + fHour) * 60) +
		fMinute) * 60) + fSecond) * kResolutionPerSecond) + fNanosecond);
}


// -----------------------------------------------------------------------------
//		Encode
// -----------------------------------------------------------------------------

void
tTimeFields::Encode(
	UInt64	inTime )
{
	if ( inTime == 0 ) {
		memset( this, 0, sizeof( tTimeFields ) );
		return;
	}

	// Convert to seconds.
	const UInt64	nSeconds = inTime / kResolutionPerSecond;

	// Split into days and seconds within the day.
	UInt32	nDays = static_cast<UInt32>( nSeconds / kSecondsPerDay );
	UInt32	nSecondsInDay = static_cast<UInt32>( nSeconds % kSecondsPerDay );

	// Time.
	fHour = nSecondsInDay / kSecondsPerHour;
	nSecondsInDay =  nSecondsInDay % kSecondsPerHour;
	fMinute = nSecondsInDay / kSecondsPerMinute;
	fSecond = nSecondsInDay % kSecondsPerMinute;
	fNanosecond = static_cast<UInt32>( inTime % kResolutionPerSecond );

	// Date.
	// See, e.g., "30.6 Days Hath September" by James Miller,
	// http://www.amsat.org/amsat/articles/g3ruh/100.html.
	const UInt32	nLeaps = (3 * (((4 * nDays) + 1227) / kDaysPerQuadriCentennium) + 3 ) / 4;
	nDays += 28188 + nLeaps;
	const UInt32	nYears = ((20 * nDays) - 2442) / (5 * kDaysPerNormalQuadrennium);
	const UInt32	nYearday = nDays - ((nYears * kDaysPerNormalQuadrennium) / 4);
	const UInt32	nMonths = (64 * nYearday) / 1959;
	if ( nMonths < 14 ) {
		fMonth = nMonths - 1;
		fYear = nYears  + 1524;
	}
	else {
		fMonth = nMonths - 13;
		fYear = nYears + 1525;
	}
	fDay = nYearday - (1959 * nMonths) / 64;
}


// -----------------------------------------------------------------------------
//      AtoW
// -----------------------------------------------------------------------------

wstring
AtoW(
	const char*	inOther,
	size_t		inLength )
{
	wstring	strResult;

	if ( inOther == nullptr ) return strResult;

	size_t	nLength = (inLength > 0) ? inLength : strlen( inOther );
	if ( nLength == 0 ) return strResult;

	size_t	nSize = ((inLength > 0) ? inLength : strlen( inOther )) + 1;
	std::wstring	strBuf( nSize, L' ' );

#ifdef _WIN32
	size_t	nNewSize = 0;
	errno_t nErr = mbstowcs_s( &nNewSize, &strBuf[0], nSize, inOther, _TRUNCATE );
	if ( nErr == 0 ) {
		strResult = std::move( strBuf );
	}
#else
	size_t	nNewSize =  mbstowcs( &strBuf[0], inOther, nSize );
	if ( nNewSize > 0 ) {
		strResult = std::move( strBuf );
	}
#endif // _WIN32

	return strResult;
}

wstring
AtoW(
	const char*	inOther )
{
	return AtoW( inOther, 0 );
}

wstring
AtoW(
	const string&	inOther )
{
	return AtoW( inOther.c_str(), inOther.length() );
}


// -----------------------------------------------------------------------------
//      WtoA
// -----------------------------------------------------------------------------

string
WtoA(
	const wchar_t*	inOther,
	size_t			inLength )
{
	string	strResult;

	if ( inOther == nullptr ) return strResult;

	size_t	nSize = ((inLength > 0) ? inLength : wcslen( inOther )) + 1;
	std::string	strBuf( nSize, ' ' );

#ifdef _WIN32
	size_t	nNewSize = 0;
	errno_t nErr = wcstombs_s( &nNewSize, &strBuf[0], nSize, inOther, _TRUNCATE );
	if ( nErr == 0 ) {
		strResult = std::move( strBuf );
	}
#else
	size_t	nNewSize = wcstombs( &strBuf[0], inOther, nSize );
	if ( nNewSize > 0 ) {
		strResult = std::move( strBuf );
	}
#endif // _WIN32

	return strResult;
}

string
WtoA(
	const wchar_t*	inOther )
{
	return WtoA( inOther, 0 );
}

string
WtoA(
	const wstring&	inOther )
{
	return WtoA( inOther.c_str(), inOther.length() );
}


// -----------------------------------------------------------------------------
//      FileMode
// -----------------------------------------------------------------------------

tFileMode
FileModeReader(
	const tFileMode inMode )
{
	tFileMode fm = inMode;
	fm.fType = CaptureFile::CF_TYPE_READER;
	return fm;
}

tFileMode
FileModeWriter(
	const tFileMode inMode )
{
	tFileMode fm = inMode;
	fm.fType = CaptureFile::CF_TYPE_WRITER;
	return fm;
}


// ============================================================================
//		tTimeLong
// ============================================================================

_tTimeLong
tTimeLong::Now()
{
	_tTimeLong ltNow;
#ifdef _WIN32
	::GetSystemTimeAsFileTime( &ltNow.ft );
	ltNow.i *= kPeekTime_FileTime;
#else
	auto	now( std::chrono::time_point_cast<std::chrono::nanoseconds>(
				std::chrono::high_resolution_clock::now()) );
	ltNow.i = now.time_since_epoch().count() + (kFileTimeToAnsi * kResolutionPerSecond);
#endif
	return ltNow;
}


// ============================================================================
//		CFileTime
// ============================================================================

// -----------------------------------------------------------------------------
//      Format
// -----------------------------------------------------------------------------

std::wstring
CFileTime::Format(
	 tTimeFormat inFormat /*= kFormat_Nanoseconds*/ )
{
	// As close to ISO 8601 as possible periods instead of colons
	// Nanoseconds 31 characters:  'YYYY-MM-DDTHH.MM.SS.nnnnnnnnnZ'
	// Milliseconds 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	tTimeFields	tf( m_ltTime.i );
	std::wostringstream	ssTime;
	ssTime << std::setfill( L'0' ) <<
			std::setw( 4 ) << tf.fYear << L"-" <<
			std::setw( 2 ) << tf.fMonth << L"-" <<
			std::setw( 2 ) << tf.fDay << L"T" <<
			std::setw( 2 ) << tf.fHour << L"." <<
			std::setw( 2 ) << tf.fMinute << L"." <<
			std::setw( 2 ) << tf.fSecond << L".";
	if ( inFormat == kFormat_Milliseconds ) {
		ssTime << std::setw( 3 ) << (tf.fNanosecond / 1000000) << L"Z";
	}
	else {
		ssTime << std::setw( 9 ) << tf.fNanosecond << L"Z";
	}
	return ssTime.str();
}


// -----------------------------------------------------------------------------
//      FormatA
// -----------------------------------------------------------------------------

std::string
CFileTime::FormatA(
	 tTimeFormat inFormat /*= kFormat_Nanoseconds*/ )
{
	// As close to ISO 8601 as possible periods instead of colons
	// Nanoseconds 31 characters:  'YYYY-MM-DDTHH.MM.SS.nnnnnnnnnZ'
	// Milliseconds 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	tTimeFields	tf( m_ltTime.i );
	std::ostringstream	ssTime;
	ssTime << std::setfill( '0' ) <<
			std::setw( 4 ) << tf.fYear << "-" <<
			std::setw( 2 ) << tf.fMonth << "-" <<
			std::setw( 2 ) << tf.fDay << "T" <<
			std::setw( 2 ) << tf.fHour << "." <<
			std::setw( 2 ) << tf.fMinute << "." <<
			std::setw( 2 ) << tf.fSecond << ".";
	if ( inFormat == kFormat_Milliseconds ) {
		ssTime << std::setw( 3 ) << (tf.fNanosecond / 1000000) << "Z";
	}
	else {
		ssTime << std::setw( 9 ) << tf.fNanosecond << "Z";
	}
	return ssTime.str();
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CFileTime::Parse(
	const std::wstring& inTime )
{
	// 2010-01-01T01:02:03.123456789Z
	wchar_t		cSeparators[7] = { L' ', L' ', L' ', L' ', L' ', L' ', L' ' };
	tTimeFields	tfTime;
	memset( &tfTime, 0, sizeof( tfTime ) );

	std::wistringstream	ssTime( inTime );
	ssTime >> tfTime.fYear >> cSeparators[0] >>
		tfTime.fMonth >> cSeparators[1] >>
		tfTime.fDay >> cSeparators[2] >>
		tfTime.fHour >> cSeparators[3] >>
		tfTime.fMinute >> cSeparators[4] >>
		tfTime.fSecond >> cSeparators[5] >>
		tfTime.fNanosecond >> cSeparators[6];

	m_ltTime.i = tfTime.Decode();
}

void
CFileTime::Parse(
	const std::string& inTime )
{
	// 2010-01-01T01:02:03.123456789Z
	char		cSeparators[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	tTimeFields	tfTime;
	memset( &tfTime, 0, sizeof( tfTime ) );

	std::istringstream	ssTime( inTime );
	ssTime >> tfTime.fYear >> cSeparators[0] >>
		tfTime.fMonth >> cSeparators[1] >>
		tfTime.fDay >> cSeparators[2] >>
		tfTime.fHour >> cSeparators[3] >>
		tfTime.fMinute >> cSeparators[4] >>
		tfTime.fSecond >> cSeparators[5] >>
		tfTime.fNanosecond >> cSeparators[6];

	m_ltTime.i = tfTime.Decode();
}


// ============================================================================
//		CBlockNameList
// ============================================================================

CBlockNameList::CBlockNameList()
{
	m_ayNames.reserve( 37 );

	wstring	strCaptureFileVersionBlock_Root( kCaptureFileVersionBlock_Root );
	AddName( strCaptureFileVersionBlock_Root );				// Version_Root

	wstring	strCaptureFileVersionBlock_AppVersion( kCaptureFileVersionBlock_AppVersion );
	AddName( strCaptureFileVersionBlock_AppVersion );		// Version_AppVersion

	wstring	strCaptureFileVersionBlock_ProductVersion( kCaptureFileVersionBlock_ProductVersion );
	AddName( strCaptureFileVersionBlock_ProductVersion );	// Version_ProductVersion

	wstring	strCaptureFileVersionBlock_FileVersion( kCaptureFileVersionBlock_FileVersion );
	AddName( strCaptureFileVersionBlock_FileVersion );		// Version_FileVersion


	wstring	strCaptureFileSessionBlock_Root( kCaptureFileSessionBlock_Root );
	AddName( strCaptureFileSessionBlock_Root );				// Session_Root

	wstring	strCaptureFileSessionBlock_RawTime( kCaptureFileSessionBlock_RawTime );
	AddName( strCaptureFileSessionBlock_RawTime );			// Session_RawTime

	wstring	strCaptureFileSessionBlock_Time( kCaptureFileSessionBlock_Time );
	AddName( strCaptureFileSessionBlock_Time );				// Session_Time

	wstring	strCaptureFileSessionBlock_TimeZoneBias( kCaptureFileSessionBlock_TimeZoneBias );
	AddName( strCaptureFileSessionBlock_TimeZoneBias );		// Session_TimeZoneBias

	wstring	strCaptureFileSessionBlock_MediaType( kCaptureFileSessionBlock_MediaType );
	AddName( strCaptureFileSessionBlock_MediaType );		// Session_MediaType

	wstring	strCaptureFileSessionBlock_MediaSubType( kCaptureFileSessionBlock_MediaSubType );
	AddName( strCaptureFileSessionBlock_MediaSubType );		// Session_MediaSubType

	wstring	strCaptureFileSessionBlock_LinkSpeed( kCaptureFileSessionBlock_LinkSpeed );
	AddName( strCaptureFileSessionBlock_LinkSpeed );		// Session_LinkSpeed

	wstring	strCaptureFileSessionBlock_PacketCount( kCaptureFileSessionBlock_PacketCount );
	AddName( strCaptureFileSessionBlock_PacketCount );		// Session_PacketCount

	wstring	strCaptureFileSessionBlock_Comment( kCaptureFileSessionBlock_Comment );
	AddName( strCaptureFileSessionBlock_Comment );			// Session_Comment

	wstring	strCaptureFileSessionBlock_SessionStartTime( kCaptureFileSessionBlock_SessionStartTime );
	AddName( strCaptureFileSessionBlock_SessionStartTime );	// Session_SessionStartTime

	wstring	strCaptureFileSessionBlock_SessionEndTime( kCaptureFileSessionBlock_SessionEndTime );
	AddName( strCaptureFileSessionBlock_SessionEndTime );	// Session_SessionEndTime

	wstring	strCaptureFileSessionBlock_AdapterName( kCaptureFileSessionBlock_AdapterName );
	AddName( strCaptureFileSessionBlock_AdapterName );		// Session_AdapterName

	wstring	strCaptureFileSessionBlock_AdapterAddr( kCaptureFileSessionBlock_AdapterAddr );
	AddName( strCaptureFileSessionBlock_AdapterAddr );		// Session_AdapterAddr

	wstring	strCaptureFileSessionBlock_CaptureName( kCaptureFileSessionBlock_CaptureName );
	AddName( strCaptureFileSessionBlock_CaptureName );		// Session_CaptureName

	wstring	strCaptureFileSessionBlock_CaptureID( kCaptureFileSessionBlock_CaptureID );
	AddName( strCaptureFileSessionBlock_CaptureID );		// Session_CaptureID

	wstring	strCaptureFileSessionBlock_Owner( kCaptureFileSessionBlock_Owner );
	AddName( strCaptureFileSessionBlock_Owner );			// Session_Owner

	wstring	strCaptureFileSessionBlock_FileIndex( kCaptureFileSessionBlock_FileIndex );
	AddName( strCaptureFileSessionBlock_FileIndex );		// Session_FileIndex

	wstring	strCaptureFileSessionBlock_Host( kCaptureFileSessionBlock_Host );
	AddName( strCaptureFileSessionBlock_Host );				// Session_Host

	wstring	strCaptureFileSessionBlock_EngineName( kCaptureFileSessionBlock_EngineName );
	AddName( strCaptureFileSessionBlock_EngineName );		// Session_EngineName

	wstring	strCaptureFileSessionBlock_EngineAddr( kCaptureFileSessionBlock_EngineAddr );
	AddName( strCaptureFileSessionBlock_EngineAddr );		// Session_EngineAddr

	wstring	strCaptureFileSessionBlock_MediaDomain( kCaptureFileSessionBlock_MediaDomain );
	AddName( strCaptureFileSessionBlock_MediaDomain );		// Session_MediaDomain

	wstring	strCaptureFileSessionBlock_DataRates( kCaptureFileSessionBlock_DataRates );
	AddName( strCaptureFileSessionBlock_DataRates );		// Session_DataRates

	wstring	strCaptureFileSessionBlock_Rate( kCaptureFileSessionBlock_Rate );
	AddName( strCaptureFileSessionBlock_Rate );				// Session_Rate

	wstring	strCaptureFileSessionBlock_ChannelList( kCaptureFileSessionBlock_ChannelList );
	AddName( strCaptureFileSessionBlock_ChannelList );		// Session_ChannelList

	wstring	strCaptureFileSessionBlock_Channel( kCaptureFileSessionBlock_Channel );
	AddName( strCaptureFileSessionBlock_Channel );			// Session_Channel

	wstring	strCaptureFileSessionBlock_Channel_Number( kCaptureFileSessionBlock_Channel_Number );
	AddName( strCaptureFileSessionBlock_Channel_Number );	// Session_Channel_Number

	wstring	strCaptureFileSessionBlock_Channel_Freq( kCaptureFileSessionBlock_Channel_Freq );
	AddName( strCaptureFileSessionBlock_Channel_Freq );		// Session_Channel_Freq

	wstring	strCaptureFileSessionBlock_Channel_Band( kCaptureFileSessionBlock_Channel_Band );
	AddName( strCaptureFileSessionBlock_Channel_Band );		// Session_Channel_Band


	wstring	strCaptureFileIdBlock_Root( kCaptureFileIdBlock_Root );
	AddName( strCaptureFileIdBlock_Root );					// FileId_Root

	wstring	strCaptureFileIdBlock_Id( kCaptureFileIdBlock_Id );
	AddName( strCaptureFileIdBlock_Id );					// FileId_Id

	wstring	strCaptureFileIdBlock_Index( kCaptureFileIdBlock_Index );
	AddName( strCaptureFileIdBlock_Index );					// FileId_Index


	wstring	strAttribCount( L"Invalid Index" );
	AddName( strAttribCount );								// Attrib_Count
}


// ============================================================================
//		CAttributeNameList
// ============================================================================

static const char	s_szActualLength[] = "ActualLength";
static const char	s_szTimeStampLo[] = "TimeStampLo";
static const char	s_szTimeStampHi[] = "TimeStampHi";
static const char	s_szFlagsStatus[] = "FlagsStatus";
static const char	s_szChannelNumber[] = "ChannelNumber";
static const char	s_szDataRate[] = "DataRate";
static const char	s_szSignalStrength[] = "SignalStrength";
static const char	s_szSignaldBm[] = "SignaldBm";
static const char	s_szNoiseStrength[] = "NoiseStrength";
static const char	s_szNoisedBm[] = "NoisedBm";
static const char	s_szMediaSpecType[] = "MediaSpecType";
static const char	s_szProtocol[] = "Protocol";
static const char	s_szDirection[] = "Direction";
static const char	s_szChannelFreq[] = "ChannelFreq";
static const char	s_szChannelBand[] = "ChannelBand";
static const char	s_szPacketDataLength[] = "PacketDataLength";
static const char	s_szSliceLength[] = "SliceLength";
static const char	s_szInvalidIndex[] = "InvalidIndex";


CAttributeNameList::CAttributeNameList()
{
	m_ayNames.reserve( 18 );

	AddName( s_szActualLength );
	AddName( s_szTimeStampLo );
	AddName( s_szTimeStampHi );
	AddName( s_szFlagsStatus );
	AddName( s_szChannelNumber );
	AddName( s_szDataRate );
	AddName( s_szSignalStrength );
	AddName( s_szSignaldBm );
	AddName( s_szNoiseStrength );
	AddName( s_szNoisedBm );
	AddName( s_szMediaSpecType );
	AddName( s_szProtocol );
	AddName( s_szDirection );
	AddName( s_szChannelFreq );
	AddName( s_szChannelBand );
	AddName( s_szPacketDataLength );
	AddName( s_szSliceLength );
	AddName( s_szInvalidIndex );
}


// ============================================================================
//		CXmlString
// ============================================================================

CXmlString::CXmlString(
	const string&	inName )
{
	reserve( 43 + (inName.size() * 2) );
	assign( "<></>" );
	insert( 4, inName );
	insert( 1, inName );
	insert( 0, "<?xml version=\"1.0\" encoding=\"utf-8\"?>" );
}


// -----------------------------------------------------------------------------
//      AddAttribute
// -----------------------------------------------------------------------------

void
CXmlString::AddAttribute(
	const string&	inAttribute,
	const string&	inValue )
{
	size_t	nIndex = rfind( '/' );
	if ( nIndex == npos ) return;
	nIndex--;

	string	str;
	str.reserve( 5 + (inAttribute.size() * 2) + inValue.size() );
	str.assign( "<></>" );
	str.insert( 4, inAttribute );
	str.insert( 2, inValue );
	str.insert( 1, inAttribute );

	insert( nIndex, str );
}


// -----------------------------------------------------------------------------
//      GetAttribute
// -----------------------------------------------------------------------------

bool
CXmlString::GetAttribute(
	const std::string&	inAttribute,
	std::string&		outValue ) const
{
	string	strTag;
	strTag.reserve( 2 + inAttribute.size() );
	strTag.assign( "<>" );
	strTag.insert( 1, inAttribute );
	string	strTagEnd;
	strTagEnd.reserve( 3 + inAttribute.size() );
	strTagEnd.assign( "</>" );
	strTagEnd.insert( 2, inAttribute );

	size_t	nBegin = find( strTag );
	if ( nBegin == string::npos ) return false;

	size_t	nEnd = find( strTagEnd );
	if ( nEnd == string::npos ) return false;

	nBegin += strTag.size();
	outValue = substr( nBegin, (nEnd - nBegin) );

	return true;
}

bool
CXmlString::GetAttribute(
	const std::string&	inAttribute,
	size_t&				ioOffset,
	std::string&		outValue ) const
{
	string	strTag;
	strTag.reserve( 2 + inAttribute.size() );
	strTag.assign( "<>" );
	strTag.insert( 1, inAttribute );
	string	strTagEnd;
	strTagEnd.reserve( 3 + inAttribute.size() );
	strTagEnd.assign( "</>" );
	strTagEnd.insert( 2, inAttribute );

	size_t	nBegin = find( strTag, ioOffset );
	if ( nBegin == string::npos ) return false;

	size_t	nEnd = find( strTagEnd, ioOffset );
	if ( nEnd == string::npos ) return false;

	nBegin += strTag.size();
	outValue = substr( nBegin, (nEnd - nBegin) );

	ioOffset = nEnd + strTagEnd.size();

	return true;
}


// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

void
CXmlString::Load(
	size_t		inLength,
	const byte*	inData )
{
	assign( reinterpret_cast<const char*>( inData ), inLength );
}


// -----------------------------------------------------------------------------
//      SetAttribute
// -----------------------------------------------------------------------------

void
CXmlString::Set(
	const string&	inName )
{
	reserve( 5 + (inName.size() * 2) );
	assign( "<></>" );
	insert( 4, inName );
	insert( 1, inName );
}


// -----------------------------------------------------------------------------
//      SetAttribute
// -----------------------------------------------------------------------------

void
CXmlString::SetAttribute(
	const string&	inAttribute,
	const string&	inValue )
{
	reserve( 6 + (inAttribute.size() * 2) + inValue.size() );
	assign( "<></>" );
	insert( 4, inAttribute );
	insert( 2, inValue );
	insert( 1, inAttribute );
}


// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

void
CXmlString::Store(
	size_t	inLength,
	byte*	outData )
{
	size_t	nCount = (inLength < size()) ? inLength : size();
	memcpy( outData, &((*this)[0]), nCount );
}


// ============================================================================
//		CFilePacket
// ============================================================================

CFilePacket::CFilePacket(
	tMediaType	inType )
	:	m_MediaType( inType )
	,	m_nLength( 0 )
	,	m_nTimeStamp( 0 )
	,	m_nFlags( 0 )
	,	m_nStatus( 0 )
	,	m_nChannelNumber( 0 )
	,	m_nDataRate( 0 )
	,	m_nSignalStrength( 0 )
	,	m_nNoiseStrength( 0 )
	,	m_nMediaSpecType( 0 )
	,	m_nProtocol( 0 )
	,	m_nDirection( 0 )
	,	m_nChannelFreq( 0 )
	,	m_nChannelBand( 0 )
	,	m_nFlagsN( 0 )
{
	for ( size_t i = 0; i < s_dBmCount; i++ ) {
		m_nSignaldBm[i] = 0;
		m_nNoisedBm[i] = 0;
	}
}


// -----------------------------------------------------------------------------
//      Copy
// -----------------------------------------------------------------------------

void
CFilePacket::Copy(
	const CFilePacket&	inOther )
{
	m_MediaType = inOther.m_MediaType;

	m_nLength = inOther.m_nLength;
	m_nTimeStamp = inOther.m_nTimeStamp;
	m_nFlags = inOther.m_nFlags;
	m_nStatus = inOther.m_nStatus;
	m_nChannelNumber = inOther.m_nChannelNumber;
	m_nDataRate = inOther.m_nDataRate;
	m_nSignalStrength = inOther.m_nSignalStrength;
	m_nNoiseStrength = inOther.m_nNoiseStrength;
	m_nMediaSpecType = inOther.m_nMediaSpecType;
	m_nProtocol = inOther.m_nProtocol;
	m_nDirection = inOther.m_nDirection;
	m_nChannelFreq = inOther.m_nChannelFreq;
	m_nChannelBand = inOther.m_nChannelBand;
	m_nFlagsN = inOther.m_nFlagsN;
	m_PacketData = inOther.m_PacketData;

	for ( size_t i = 0; i < s_dBmCount; i++ ) {
		m_nSignaldBm[i] = inOther.m_nSignaldBm[i];
		m_nNoisedBm[i] = inOther.m_nNoisedBm[i];
	}
}


// -----------------------------------------------------------------------------
//      GetAttributes
// -----------------------------------------------------------------------------

void
CFilePacket::GetAttributes(
	CPacketAttributeList&	outAttribs ) const
{
	_ASSERTE( m_nLength < kMaxUInt32 );
	if ( m_nLength >= kMaxUInt32 ) {
		throw -1;
	}
	outAttribs.Add( TaggedFile::Attrib_ActualLength, static_cast<UInt32>( m_nLength ) );
	outAttribs.Add( TaggedFile::Attrib_TimeStampLo,
		(reinterpret_cast<const tTimeLong*>( &m_nTimeStamp ))->sTime.TimeLo );
	outAttribs.Add( TaggedFile::Attrib_TimeStampHi,
		(reinterpret_cast<const tTimeLong*>( &m_nTimeStamp ))->sTime.TimeHi );
	// From tagfile.h
	// high 16 bits are PEEK status bits, low 16 are PEEK flags
	outAttribs.Add( TaggedFile::Attrib_FlagsStatus,
		((static_cast<UInt32>( m_nStatus ) << 16) | m_nFlags) );
	outAttribs.Add( TaggedFile::Attrib_MediaSpecType, m_nMediaSpecType );

	if ( IsWireless() ) {
		outAttribs.Add( Attrib_ChannelNumber, m_nChannelNumber );
		outAttribs.Add( Attrib_DataRate, m_nDataRate );
		outAttribs.Add( Attrib_SignalStrength, m_nSignalStrength );
		outAttribs.Add( Attrib_SignaldBm, m_nSignaldBm[0] );
		outAttribs.Add( Attrib_SignaldBm1, m_nSignaldBm[1] );
		outAttribs.Add( Attrib_SignaldBm2, m_nSignaldBm[2] );
		outAttribs.Add( Attrib_SignaldBm3, m_nSignaldBm[3] );
		outAttribs.Add( Attrib_NoiseStrength, m_nNoiseStrength );
		outAttribs.Add( Attrib_NoisedBm, m_nNoisedBm[0] );
		outAttribs.Add( Attrib_NoisedBm1, m_nNoisedBm[1] );
		outAttribs.Add( Attrib_NoisedBm2, m_nNoisedBm[2] );
		outAttribs.Add( Attrib_NoisedBm3, m_nNoisedBm[3] );
		outAttribs.Add( Attrib_ChannelFreq, m_nChannelFreq );
		outAttribs.Add( Attrib_ChannelBand, m_nChannelBand );
		outAttribs.Add( Attrib_FlagsN, m_nFlagsN );
	}

	if ( IsWAN() ) {
		outAttribs.Add( Attrib_Protocol, m_nProtocol );
		outAttribs.Add( Attrib_Direction, m_nDirection );
	}

	_ASSERTE( m_PacketData.size() < kMaxUInt32 );
	if ( m_PacketData.size() >= kMaxUInt32 ) {
		throw -1;
	}
	outAttribs.Add( TaggedFile::Attrib_PacketDataLength,
		static_cast<UInt32>( m_PacketData.size() ) );
}


#if (0)
// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

size_t
CFilePacket::Load(
	size_t		inLength,
	const byte*	inData )
{
	inLength;
	inData;
	return 0;

	PacketAttribute*	pAttribs = (PacketAttribute*) inData;
	UInt32				nDataLength = 0;
	UInt32				nAttribCount = Parse( (inLength / sizeof( PacketAttribute )), pAttribs, nDataLength );
	const byte*			pData = inData + (nAttribCount * sizeof( PacketAttribute ));
	if ( nDataLength > 0 ) {
		m_PacketData.resize( nDataLength );
		std::copy( pData, (pData + nDataLength), m_PacketData.begin() );
	}
	return (nAttribCount * sizeof( PacketAttribute )) + nDataLength;
}

bool
CFilePacket::Load(
	COSFile&	inFile,
	UInt64		inOffset )
{
	inFile;
	inOffset;
	return false;

	const UInt32	nAttribMax = Attrib_FlagsN + 2;
	const UInt32	nAttribSize = nAttribMax * sizeof( PacketAttribute );
	byte			ayAttribData[nAttribSize];

	inFile.SeekTo( inOffset );

	UInt32	nRead = inFile.Read( nAttribSize, ayAttribData );
	if ( nRead < 6 ) return false;

	ResetPacketData();

	PacketAttribute*	pAttribs = reinterpret_cast<PacketAttribute*>( ayAttribData );
	UInt32				nDataLength = 0;
	UInt32				nAttribCount = Parse( (nRead / 6), pAttribs, nDataLength );
	if ( nDataLength > 0 ) {
		UInt64	nDataOffset = inOffset + (nAttribCount * sizeof( PacketAttribute ));
		inFile.SeekTo( nDataOffset );

		m_PacketData.resize( nDataLength );

		UInt32	nRead = inFile.Read( m_PacketData.size(), &m_PacketData[0] );
		_ASSERTE( nRead == m_PacketData.size() );
	}

	return (nDataLength != 0);
}
#endif


// -----------------------------------------------------------------------------
//      Reset
// -----------------------------------------------------------------------------

void
CFilePacket::Reset()
{
	m_nLength = 0;
	m_nTimeStamp = 0;
	m_nFlags = 0;
	m_nStatus = 0;
	m_nChannelNumber = 0;
	m_nDataRate = 0;
	m_nSignalStrength = 0;
	m_nNoiseStrength = 0;
	m_nMediaSpecType = 0;
	m_nProtocol = 0;
	m_nDirection = 0;
	m_nChannelFreq = 0;
	m_nChannelBand = 0;
	m_PacketData.clear();

	for ( size_t i = 0; i < s_dBmCount; i++ ) {
		m_nSignaldBm[i] = 0;
		m_nNoisedBm[i] = 0;
	}
}


// -----------------------------------------------------------------------------
//      SetAttributes
// -----------------------------------------------------------------------------

void
CFilePacket::SetAttributes(
	const CPacketAttributeList&	inAttribs,
	UInt32&						outDataLength )
{
	outDataLength = 0;

	size_t	nCount = inAttribs.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		UInt32	nValue = inAttribs[i].GetValue();
		switch ( inAttribs[i].GetId() ) {
			case Attrib_ActualLength:
				m_nLength = nValue;
				break;
			case Attrib_TimeStampLo:
				(reinterpret_cast<tTimeLong*>( &m_nTimeStamp ))->sTime.TimeLo = nValue;
				break;
			case Attrib_TimeStampHi:
				(reinterpret_cast<tTimeLong*>( &m_nTimeStamp ))->sTime.TimeHi = nValue;
				break;
			case Attrib_FlagsStatus:
				// From tagfile.h
				// high 16 bits are PEEK status bits, low 16 are PEEK flags
				m_nStatus = nValue >> 16;
				m_nFlags = nValue & 0x0FFFF;
				break;
			case Attrib_ChannelNumber:
				m_nChannelNumber = nValue;
				break;
			case Attrib_DataRate:
				m_nDataRate = nValue;
				break;
			case Attrib_SignalStrength:
				m_nSignalStrength = nValue;
				break;
			case Attrib_SignaldBm:
				m_nSignaldBm[0] = nValue;
				break;
			case Attrib_NoiseStrength:
				m_nNoiseStrength = nValue;
				break;
			case Attrib_NoisedBm:
				m_nNoisedBm[0] = nValue;
				break;
			case Attrib_MediaSpecType:
				m_nMediaSpecType = nValue;
				break;
			case Attrib_Protocol:
				m_nProtocol = nValue;
				break;
			case Attrib_Direction:
				m_nDirection = nValue;
				break;
			case Attrib_ChannelFreq:
				m_nChannelFreq = nValue;
				break;
			case Attrib_ChannelBand:
				m_nChannelBand = nValue;
				break;
			case Attrib_SignaldBm1:
				m_nSignaldBm[1] = nValue;
				break;
			case Attrib_SignaldBm2:
				m_nSignaldBm[2] = nValue;
				break;
			case Attrib_SignaldBm3:
				m_nSignaldBm[3] = nValue;
				break;
			case Attrib_NoisedBm1:
				m_nNoisedBm[1] = nValue;
				break;
			case Attrib_NoisedBm2:
				m_nNoisedBm[2] = nValue;
				break;
			case Attrib_NoisedBm3:
				m_nNoisedBm[3] = nValue;
				break;
			case Attrib_FlagsN:
				m_nFlagsN = nValue;
				break;

			case Attrib_PacketDataLength:
				outDataLength = nValue;
				return;

			default:
				_ASSERTE( 0 );
				break;
		}
	}
}


#if (0)
// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

size_t
CFilePacket::Store(
	CByteVector&	Data ) const
{
	size_t	nPacketDataCount = GetPacketDataLength();

	CPacketAttributeList	ayAttribs;
	Build( ayAttribs );
	size_t	nAttribCount = ayAttribs.size();

	size_t	nLength = (sizeof( PacketAttribute ) * nAttribCount) + nPacketDataCount;
	if ( nLength > 0 ) {
		Data.resize( nLength );

		// Refactor to a single copy.
		PacketAttribute*	pAttrib = reinterpret_cast<PacketAttribute*>( &Data[0] );
		for ( size_t i = 0; i < nAttribCount; i++ ) {
			pAttrib->fieldType = ayAttribs[i].GetId();
			pAttrib->fieldValue = ayAttribs[i].GetValue();
			pAttrib++;
		}

		if ( nPacketDataCount ) {
			pAttrib->fieldType = Attrib_PacketDataLength;
			pAttrib->fieldValue = static_cast<UInt32>( nPacketDataCount );
			pAttrib++;
			memcpy( pAttrib, GetPacketData(), nPacketDataCount );
		}
	}

	return nLength;
}
#endif


// ============================================================================
//		CStreamFile
// ============================================================================

// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CStreamFile::Open(
	const wstring&	inFileName,
	UInt32			/*inMode*/ )
{
#ifdef _WIN32
	gzFile	zFile = gzopen_w( inFileName.c_str(), "rb" );
#else
	gzFile	zFile = gzopen( HeLib::CW2A( inFileName.c_str() ), "rb" );
#endif
	if ( zFile == nullptr ) return false;

	static const UInt32		nChunkSize( 1024 * 1024 );
	vector<UInt8>			vecData( nChunkSize );
	int						nRead;

#ifdef _DEBUG
	memset( vecData.data(), 0, vecData.size() );
#endif

	m_streamData.Reset();

	do {
		nRead = gzread( zFile, vecData.data(), nChunkSize );
		if ( nRead <= 0 ) break;
		UInt64	nWrite = m_streamData.Write( nRead, vecData.data() );
		if ( nWrite != static_cast<UInt64>( nRead ) ) {
			Close();
			break;
		}
	} while ( nRead > 0 );

	gzclose( zFile );
	SetName( inFileName );
	m_streamData.Rewind();

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      SetName
// -----------------------------------------------------------------------------

void
CStreamFile::SetName(
	const wstring&	inName )
{
	if ( IsOpen() ) {
		FilePath::Path	path( inName );
		m_strPathName = path.GetDir();
		m_strFileName = path.GetFileName();
	}
	else {
		m_strFileName.clear();
		m_strFileName.clear();
	}
}

#ifdef _WIN32
// ============================================================================
//		CWinFile
// ============================================================================

// -----------------------------------------------------------------------------
//      Close
// -----------------------------------------------------------------------------

void
CWinFile::Close()
{
	if ( IsOpen() ) {
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
		m_strFileName.clear();
	}
}


// -----------------------------------------------------------------------------
//      GetPosition
// -----------------------------------------------------------------------------

UInt64
CWinFile::GetPosition() const
{
	LARGE_INTEGER	liPosition = { 0, 0 };
	if ( IsOpen() ) {
		LARGE_INTEGER	liZero = { 0, 0 };
		::SetFilePointerEx( m_hFile, liZero, &liPosition, FILE_CURRENT );
	}
	return liPosition.QuadPart;
}


// -----------------------------------------------------------------------------
//      GetSize
// -----------------------------------------------------------------------------

UInt64
CWinFile::GetSize() const
{
	LARGE_INTEGER	liSize = { 0, 0 };
	if ( IsOpen() ) {
		::GetFileSizeEx( m_hFile, &liSize );
	}
	return liSize.QuadPart;
}


// -----------------------------------------------------------------------------
//      MoveTo
// -----------------------------------------------------------------------------

UInt64
CWinFile::MoveTo(
	SInt64	inOffset)
{
	LARGE_INTEGER	liNewPosition = { 0, 0 };
	if ( IsOpen() ) {
		LARGE_INTEGER	liOffset;
		liOffset.QuadPart = inOffset;

		::SetFilePointerEx( m_hFile, liOffset, &liNewPosition, FILE_CURRENT );
	}
	return liNewPosition.QuadPart;
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CWinFile::Open(
	const wstring&	inFileName,
	UInt32			inMode )
{
	UInt32	dwDesiredAccess = 0;
	UInt32	dwCreationDisposition = 0;
	if ( !ParseMode( inMode, dwDesiredAccess, dwCreationDisposition ) ) {
		return false;
	}

	UInt32	dwShareMode = 0;	// Exclusive access.
	LPSECURITY_ATTRIBUTES lpsa = nullptr;
	DWORD	dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE	hTemplateFile = nullptr;

	HANDLE hFile = ::CreateFile( inFileName.c_str(), dwDesiredAccess, dwShareMode, lpsa,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
	if ( hFile != INVALID_HANDLE_VALUE ) {
		m_hFile = hFile;
		SetName( inFileName );
	}
	else {
		DWORD	dwError = ::GetLastError();
		if ( dwError == 0 ) {
			hFile = INVALID_HANDLE_VALUE;
		}
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      ParseMode
// -----------------------------------------------------------------------------

bool
CWinFile::ParseMode(
	UInt32	inMode,
	UInt32&	outAccess,
	UInt32&	outCreate )
{
	outAccess = 0;
	outCreate = 0;

	if ( (inMode & FILEMODE_READ) && (inMode & FILEMODE_WRITE) ) {
		return false;
	}

	if ( inMode & FILEMODE_READ ) {
		outAccess |= GENERIC_READ;
	}

	if ( inMode & FILEMODE_WRITE ) {
		outAccess |= GENERIC_WRITE;
	}

	if ( inMode & FILEMODE_CREATE ) {
		outCreate = CREATE_ALWAYS;
	}
	else {
		outCreate = OPEN_EXISTING;
	}

	return true;
}


// -----------------------------------------------------------------------------
//      Read
// -----------------------------------------------------------------------------

UInt32
CWinFile::Read(
	UInt32	inLength,
	void*	outData )
{
#if _DEBUG
	DWORD	dwErrorDbg( 0 );
#endif

	if ( (inLength > 0) && IsOpen() ) {
		UInt32	nRead = 0;
		if ( ::ReadFile( m_hFile, outData, inLength, &nRead, nullptr ) != FALSE ) {
#if _DEBUG
			if ( nRead == 0 ) {
				DWORD	dwError( ::GetLastError() );
				dwErrorDbg = dwError;
			}
#endif
			return nRead;
		}
	}
	return 0;
}


// -----------------------------------------------------------------------------
//      SeekTo
// -----------------------------------------------------------------------------

UInt64
CWinFile::SeekTo(
	UInt64	inPosition )
{
	LARGE_INTEGER	liNewPosition = { 0, 0 };
	if ( IsOpen() ) {
		LARGE_INTEGER	liSeekTo;
		liSeekTo.QuadPart = inPosition;

		::SetFilePointerEx( m_hFile, liSeekTo, &liNewPosition, FILE_BEGIN );
	}
	return liNewPosition.QuadPart;
}


// -----------------------------------------------------------------------------
//      SetName
// -----------------------------------------------------------------------------

void
CWinFile::SetName(
	const wstring&	inName )
{
	if ( IsOpen() ) {
		wchar_t		szName[MAX_PATH + 1];
		wchar_t*	pszName = nullptr;

		DWORD dwLength = ::GetFullPathName( inName.c_str(), MAX_PATH, szName, &pszName );
		if ( (dwLength > 0) && (dwLength < MAX_PATH) ) {
			m_strPathName = szName;
			m_strFileName = pszName;
		}
	}
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

UInt32
CWinFile::Write(
	UInt32		inLength,
	const void*	inData )
{
	if ( (inLength > 0) && IsOpen() ) {
		UInt32	nWrite = 0;
		if ( ::WriteFile( m_hFile, inData, inLength, &nWrite, nullptr ) != FALSE ) {
			return nWrite;
		}
	}
	return 0;
}

#else

// ============================================================================
//		CStandardFile
// ============================================================================

// TODO: check all of this.

// -----------------------------------------------------------------------------
//      Close
// -----------------------------------------------------------------------------

void
CStandardFile::Close()
{
	if ( IsOpen() ) {
		fclose( m_hFile );
		m_hFile = nullptr;
		m_strFileName.clear();
	}
}


// -----------------------------------------------------------------------------
//      GetPosition
// -----------------------------------------------------------------------------

UInt64
CStandardFile::GetPosition() const
{
	return ftell( m_hFile );
}


// -----------------------------------------------------------------------------
//      GetSize
// -----------------------------------------------------------------------------

UInt64
CStandardFile::GetSize() const
{
	int				fd( fileno( m_hFile ) );
	struct stat		buf;
	if ( fstat( fd, &buf ) == 0 ) {
		return buf.st_size;
	}
	return 0;
}


// -----------------------------------------------------------------------------
//      MoveTo
// -----------------------------------------------------------------------------

UInt64
CStandardFile::MoveTo(
	SInt64	inOffset)
{
	fseek( m_hFile, inOffset, SEEK_CUR );
	return ftell( m_hFile );
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CStandardFile::Open(
	const wstring&	inFileName,
	UInt32			inMode )
{
	char	szMode[8] = "";
	if ( inMode & FILEMODE_READ ) {
		strcat( szMode, "r" );
	}
	if ( inMode & FILEMODE_WRITE ) {
		strcat( szMode, "w" );
	}
	strcat( szMode, "b" );
	if ( inMode & FILEMODE_CREATE ) {
		strcat( szMode, "+" );
	}

	CPeekStringA	strFileName( inFileName );
	m_hFile = fopen( strFileName, szMode );
	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Read
// -----------------------------------------------------------------------------

UInt32
CStandardFile::Read(
	UInt32	inLength,
	void*	outData )
{
	if ( (inLength > 0) && IsOpen() ) {
		return fread( outData, 1, inLength, m_hFile );
	}
	return 0;
}


// -----------------------------------------------------------------------------
//      SeekTo
// -----------------------------------------------------------------------------

UInt64
CStandardFile::SeekTo(
	UInt64	inPosition )
{
	fseek( m_hFile, inPosition, SEEK_SET );
	return ftell( m_hFile );
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

UInt32
CStandardFile::Write(
	UInt32		inLength,
	const void*	inData )
{
	if ( (inLength > 0) && IsOpen() ) {
		return fwrite( inData, 1, inLength, m_hFile );
	}
	return 0;
}
#endif /* _WIN32 */

// ============================================================================
//		CBaseFile
// ============================================================================

bool
CBaseFile::Open(
	const wstring&	inFileName,
	int				inMode )
{
#ifdef _WIN32
	unique_ptr<CWinFile>	spWinFile( new CWinFile() );
	m_spFile = move( spWinFile );
 #else
	unique_ptr<CStandardFile>	spStandardFile( new CStandardFile() );
	m_spFile = move( spStandardFile );
#endif

	if ( m_spFile ) {
		if ( !m_spFile->Open( inFileName, inMode ) ) {
			m_spFile->Close();
		}
	}

	return IsOpen();
}


// ============================================================================
//		CCaptureReader
// ============================================================================

// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CCaptureReader::Open(
	const wstring&	inFileName,
	bool			inScanPackets )
{
	if ( CBaseFile::Open( inFileName, (COSFile::FILEMODE_READ | COSFile::FILEMODE_READ) ) ) {
		if ( !Parse( inScanPackets ) ) {
			return false;
		}
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CCaptureReader::ReadFilePackets(
	CFilePacketList&	outPackets,
	UInt32&				outError,
	UInt32				inCount,
	UInt32				inFirst /*= NEXT_PACKET*/)
{
	UInt32	nLast = inCount + ((inFirst == (UInt32)NEXT_PACKET) ? 0 : inFirst);
	for ( UInt32 i = inFirst; i < nLast; i++ ) {
		CFilePacket	pkt;
		if ( !ReadFilePacket( pkt, outError, i ) ) break;
		outPackets.push_back( pkt );
	}

	return !outPackets.empty();
}


// ============================================================================
//		CCaptureWriter
// ============================================================================

// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CCaptureWriter::Open(
	const wstring&	inFileName )
{
	UInt32	nMode = COSFile::FILEMODE_WRITE | COSFile::FILEMODE_CREATE;
	if ( CBaseFile::Open( inFileName, nMode ) ) {
		Init();
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      WriteFilePackets
// -----------------------------------------------------------------------------

bool
CCaptureWriter::WriteFilePackets(
	const CFilePacketList&	inPackets,
	UInt32&					outCount )
{
	outCount = 0;
	for ( size_t i = 0; i < inPackets.size(); i++ ) {
		if ( !WriteFilePacket( inPackets[i] ) ) return false;
		++outCount;
	}
	return true;
}


// ============================================================================
//		CCaptureFile
// ============================================================================

// ----------------------------------------------------------------------------
//      GetFile
// ----------------------------------------------------------------------------

#ifdef _WIN32
COSFile&
CCaptureFile::GetFile()
{
	return (m_spBaseFile.IsValid())
		? m_spBaseFile->GetFile()
		: s_WinFile;
}
#else
COSFile&
CCaptureFile::GetFile()
{
	return (m_spBaseFile.IsValid())
		? m_spBaseFile->GetFile()
		: s_StandardFile;
}
#endif


// ----------------------------------------------------------------------------
//      GetFileHandle
// ----------------------------------------------------------------------------

#ifdef _WIN32
HANDLE
CCaptureFile::GetFileHandle()
{
	if ( !m_spBaseFile.IsValid() ) return INVALID_HANDLE_VALUE;
	CWinFile*	pWinFile = dynamic_cast<CWinFile*>( &m_spBaseFile->GetFile() );
	if ( pWinFile == nullptr ) return INVALID_HANDLE_VALUE;
	return pWinFile->GetHandle();
}
#else
FILE*
CCaptureFile::GetFileHandle()
{
	if ( !m_spBaseFile.IsValid() ) return INVALID_HANDLE_VALUE;
	CStandardFile*	pStdFile = dynamic_cast<CStandardFile*>( &m_spBaseFile->GetFile() );
	if ( pStdFile == nullptr ) return INVALID_HANDLE_VALUE;
	return pStdFile->GetHandle();
}
#endif


// -----------------------------------------------------------------------------
//      GetFileFormat
// -----------------------------------------------------------------------------

UInt32
CCaptureFile::GetFileFormat(
	const wstring&	inFileName )
{
	UInt32				nFileFormat( kFileFormatUnknown );
	wstring::size_type	nPos = inFileName.rfind( L'.' );
	if ( (nPos != wstring::npos) && (nPos < inFileName.length()) ) {
		wstring	strExt = inFileName.substr( nPos + 1 );
#ifdef CF_USE_TRANSFORM
		std::transform( strExt.begin(), strExt.end(), strExt.begin(), towlower );
#else
		{
			size_t	nCount = strExt.size();
			for ( size_t i = 0; i < nCount; i++ ) {
				strExt[i] = static_cast<wchar_t>( towlower( strExt[i] ) );
			}
		}
#endif

		bool	bZipped( strExt == L"gz" );
		if ( bZipped ) {
			nFileFormat |= kFileFormatZipped;

			wstring::size_type	nPos2 = inFileName.rfind( L'.', (nPos - 1) );
			if ( (nPos2 != wstring::npos) && (nPos < inFileName.length()) ) {
				strExt = inFileName.substr( nPos2 + 1, (nPos - nPos2 - 1) );
#ifdef CF_USE_TRANSFORM
				std::transform( strExt.begin(), strExt.end(), strExt.begin(), towlower );
#else
				{
					size_t	nCount = strExt.size();
					for ( size_t i = 0; i < nCount; i++ ) {
						strExt[i] = static_cast<wchar_t>( towlower( strExt[i] ) );
					}
				}
#endif
			}
		}

#ifdef CF_PEEK_FILE
		if ( CPeekCaptureReader::IsKind( strExt ) ) {
			nFileFormat |= kFileFormatPeek;
		}
#endif

#ifdef CF_PCAP_FILE
		if ( CPCapCaptureReader::IsKind( strExt ) ) {
			nFileFormat |= kFileFormatPCap;
		}
		else if ( CPCapNgCaptureReader::IsKind( strExt ) ) {
			nFileFormat |= kFileFormatPCapNg;
		}
#endif

	}

	return nFileFormat;
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CCaptureFile::Open(
	const std::wstring&	inFileName,
	const tFileMode&	inMode )
{
	if ( inMode.IsReader() ) {
		CCaptureReader*	pReader = OpenReader( inFileName, inMode );
		CBaseFile*		pBase = pReader;
		m_spBaseFile.reset( pBase );
	}
	else if ( inMode.IsWriter() ) {
		CCaptureWriter*	pWriter = OpenWriter( inFileName, inMode );
		CBaseFile*		pBase = pWriter;
		m_spBaseFile.reset( pBase );
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      OpenReader
// -----------------------------------------------------------------------------

CCaptureReader*
CCaptureFile::OpenReader(
	const wstring&		inFileName,
	const tFileMode&	inMode )
{
	CCaptureReader*		pReader = nullptr;
	unique_ptr<COSFile>	spOSFile;

	UInt32	nFileFormat = GetFileFormat( inFileName );
	if ( nFileFormat & kFileFormatZipped ) {
		unique_ptr<CStreamFile>	spStreamFile( new CStreamFile() );
		if ( spStreamFile.get() == nullptr ) return nullptr;

		if ( !spStreamFile->Open( inFileName, inMode.fScan ) ) {
			return nullptr;
		}
		spOSFile = std::move( spStreamFile );
	}

	switch ( nFileFormat & kFileFormatTypeMask ) {

#ifdef CF_PEEK_FILE
	case kFileFormatPeek:
		pReader = new CPeekCaptureReader( std::move( spOSFile ) );
		if ( pReader && !pReader->Open( inFileName, inMode.fScan ) ) {
			pReader->Close();
		}
		break;
#endif

#ifdef CF_PCAP_FILE
	case kFileFormatPCap:
		pReader = new CPCapCaptureReader( std::move( spOSFile ) );
		if ( pReader && !pReader->Open( inFileName, inMode.fScan ) ) {
			spOSFile = pReader->MoveFile();
			delete pReader;
			pReader = new CPCapNgCaptureReader( std::move( spOSFile ) );
			if ( pReader && !pReader->Open( inFileName, inMode.fScan ) ) {
				pReader->Close();
			}
		}
		break;

	case kFileFormatPCapNg:
		pReader = new CPCapNgCaptureReader( std::move( spOSFile ) );
		if ( pReader && !pReader->Open( inFileName, inMode.fScan ) ) {
			spOSFile = pReader->MoveFile();
			delete pReader;
			pReader = new CPCapCaptureReader( std::move( spOSFile ) );
			if ( pReader && !pReader->Open( inFileName, inMode.fScan ) ) {
				pReader->Close();
			}
		}
		break;
#endif
	}

	return pReader;
}


// -----------------------------------------------------------------------------
//      OpenWriter
// -----------------------------------------------------------------------------

CCaptureWriter*
CCaptureFile::OpenWriter(
	const wstring&		inFileName,
	const tFileMode&	inMode )
{
	CCaptureWriter*	pWriter = nullptr;

	UInt32	nFileFormat = GetFileFormat( inFileName );
	switch ( nFileFormat ) {

#ifdef CF_PEEK_FILE
	case kFileFormatPeek:
		pWriter = new CPeekCaptureWriter( inMode.fAccess, inMode.fMediaType );
		break;
#endif

#ifdef CF_PCAP_FILE
	case kFileFormatPCap:
		pWriter = new CPCapCaptureWriter( inMode.fAccess, inMode.fMediaType );
		break;

	case kFileFormatPCapNg:
		pWriter = new CPCapNgCaptureWriter( inMode.fAccess, inMode.fMediaType );
		break;
#endif
	}

	if ( pWriter ) {
		pWriter->Open( inFileName );
	}

	return pWriter;
}

}	// namespace CaptureFile
