// =============================================================================
//	CaptureFile.cpp
//		implementation of the CCaptureFile class.
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "CaptureFile.h"
#include "Packet.h"

#ifdef _LINUX
#include "zlib.h"
#endif

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
#include <cassert>
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

#ifdef _LINUX
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#endif

using std::string;
using std::unique_ptr;
using std::wstring;
using std::stringstream;
using namespace CaptureFile;
using namespace TaggedFile;

// Use (void) to silence unused warnings.
#define assertm(exp, msg)	assert(((void)msg, exp))

// extern size_t wcslen( const wchar_t* );


namespace CaptureFile {

// ============================================================================
//		Globals
// ============================================================================

CBlockNameList		CAttribute::s_BlockNames;
CAttributeNameList	CAttribute::s_AttributeNames;


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

tTimeLong
tTimeLong::Now()
{
	tTimeLong ltNow;
//#ifdef _WIN32
//	::GetSystemTimeAsFileTime( &ltNow.ft );
//	ltNow.i *= kPeekTime_FileTime;
//#else
	auto	now( std::chrono::time_point_cast<std::chrono::nanoseconds>(
				std::chrono::high_resolution_clock::now()) );
	ltNow.i = now.time_since_epoch().count() + (kFileTimeToAnsi * kResolutionPerSecond);
//#endif
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
	CTimeFields	tf( m_ltTime.i );
	std::wostringstream	ssTime;
	ssTime << std::setfill( L'0' ) <<
			std::setw( 4 ) << tf.fYear << L"-" <<
			std::setw( 2 ) << tf.fMonth << L"-" <<
			std::setw( 2 ) << tf.fDay << L"T" <<
			std::setw( 2 ) << tf.fHour << L"." <<
			std::setw( 2 ) << tf.fMinute << L"." <<
			std::setw( 2 ) << tf.fSecond << L".";
	if ( inFormat == kFormat_Seconds ) {
		ssTime << L"Z";
	}
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
	CTimeFields	tf( m_ltTime.i );
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
	CTimeFields	tfTime;

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
	CTimeFields	tfTime;

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
	assertm( (m_nLength < std::numeric_limits<UInt32>::max()), "The are too many bytes to read." );

	if ( m_nLength >= std::numeric_limits<UInt32>::max() ) {
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

	assertm( (m_PacketData.size() < std::numeric_limits<UInt32>::max()), "There is too much packet data." );
	if ( m_PacketData.size() >= std::numeric_limits<UInt32>::max() ) {
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
	UInt64		inPosition )
{
	inFile;
	inOffset;
	return false;

	const UInt32	nAttribMax = Attrib_FlagsN + 2;
	const UInt32	nAttribSize = nAttribMax * sizeof( PacketAttribute );
	byte			ayAttribData[nAttribSize];

	inFile.MoveTo( inPosition );

	UInt32	nRead = inFile.Read( nAttribSize, ayAttribData );
	if ( nRead < 6 ) return false;

	ResetPacketData();

	PacketAttribute*	pAttribs = reinterpret_cast<PacketAttribute*>( ayAttribData );
	UInt32				nDataLength = 0;
	UInt32				nAttribCount = Parse( (nRead / 6), pAttribs, nDataLength );
	if ( nDataLength > 0 ) {
		UInt64	nDataOffset = inPosition + (nAttribCount * sizeof( PacketAttribute ));
		inFile.Seek( nDataOffset );

		m_PacketData.resize( nDataLength );
	
		UInt32	nRead = inFile.Read( m_PacketData.size(), &m_PacketData[0] );
		assertm( (nRead == m_PacketData.size()), "The amount of data read did not match the request." );
	}

	return (nDataLength != 0);
}
#endif


// -----------------------------------------------------------------------------
//      ParsePacketData
// -----------------------------------------------------------------------------

bool
CFilePacket::ParsePacketData(
	CPacket& outPacket )
{
	return outPacket.ParsePacketData( m_PacketData );
}


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
				assertm( false, "Unrecognized Attribute." );
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
//		CCaptureReader
// ============================================================================

// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CCaptureReader::Open(
	const path&	inPath )
{
	m_File.open( inPath, (std::ios_base::in | std::ios_base::binary) );
	if ( m_File.is_open() ) {
		m_Path = inPath;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Read
// -----------------------------------------------------------------------------

UInt32
CCaptureReader::Read(
	UInt32	inLength,
	void*	outData )
{
	m_File.read( reinterpret_cast<char*>( outData ), inLength );
	return static_cast<UInt32>( m_File.gcount() );
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CCaptureReader::ReadFilePackets(
	UInt32				inCount,
	CFilePacketList&	outPackets,
	UInt32&				outError )
{
	for ( UInt32 i = 0; i < inCount; i++ ) {
		CFilePacket	pkt;
		if ( !ReadFilePacket( pkt, outError ) ) break;
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
	const path&	inPath )
{
	m_File.open( inPath, (std::ios_base::out | std::ios_base::binary) );
	if ( m_File.is_open() ) {
		m_Path = inPath;
		Init();
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

UInt32
CCaptureWriter::Write(
	UInt32		inLength,
	const void*	inData )
{
	UInt64	nBegin( m_File.tellp() );
	m_File.write( reinterpret_cast<const char*>( inData), inLength );
	UInt64	nEnd( m_File.tellp() );
	if ( nBegin > nEnd ) return -1;

	return static_cast<UInt32>( nEnd - nBegin );
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

// -----------------------------------------------------------------------------
//      GetFileFormat
// -----------------------------------------------------------------------------

CF_FILE_FORMAT
CCaptureFile::GetFileFormat(
	const path&	inPath )
{
	if ( !inPath.has_extension() ) { return CF_FORMAT_UNKNOWN; }

	path	pathPath( inPath );
	string	strExt( pathPath.extension().string() );
	strExt.erase( std::remove( strExt.begin(), strExt.end(), '.' ), strExt.end() );
	std::transform( strExt.begin(), strExt.end(), strExt.begin(), tolower );

	bool	bZipped( strExt == "gz" );
	if ( bZipped ) {
		pathPath.replace_extension( "" );
	}

	if ( !pathPath.has_extension() ) { return CF_FORMAT_UNKNOWN; }
	strExt = pathPath.extension().string();
	strExt.erase( std::remove( strExt.begin(), strExt.end(), '.' ), strExt.end() );
	std::transform( strExt.begin(), strExt.end(), strExt.begin(), tolower );

#ifdef CF_PEEK_FILE
	if ( CPeekCaptureReader::IsKind( strExt ) ) {
		return CF_FORMAT_PEEK;
	}
#endif

#ifdef CF_PCAP_FILE
	if ( CPCapCaptureReader::IsKind( strExt ) ) {
		return CF_FORMAT_PCAP;
	}
	else if ( CPCapNgCaptureReader::IsKind( strExt ) ) {
		return CF_FORMAT_PCAP_NG;
	}
#endif

	return CF_FORMAT_UNKNOWN;
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CCaptureFile::Open(
	const path&			inPath,
	const tFileMode&	inMode )
{
	Close();
	if ( inMode.IsReader() ) {
		CCaptureReader*	pReader( OpenReader( inPath ) );
		CBaseFile*		pBase = pReader;
		m_pFile = pBase;
	}
	else if ( inMode.IsWriter() ) {
		CCaptureWriter*	pWriter = OpenWriter( inPath, inMode );
		CBaseFile*		pBase = pWriter;
		m_pFile = pBase;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      OpenReader
// -----------------------------------------------------------------------------

CCaptureReader*
CCaptureFile::OpenReader(
	const path&	inPath )
{
	std::unique_ptr<CCaptureReader>	spReader;
	CF_FILE_FORMAT					nFileFormat = GetFileFormat( inPath );

	switch ( nFileFormat & kFileFormatTypeMask ) {

#ifdef CF_PEEK_FILE
	case kFileFormatPeek:
		spReader = std::make_unique<CPeekCaptureReader>();
		break;
#endif

#ifdef CF_PCAP_FILE
	case kFileFormatPCap:
		spReader = std::make_unique<CPCapCaptureReader>();
		break;

	case kFileFormatPCapNg:
		spReader = std::make_unique<CPCapNgCaptureReader>();
		break;
#endif
	}

	if ( spReader && !spReader->Open( inPath ) ) {
		spReader->Close();
	}

	return spReader.release();
}


// -----------------------------------------------------------------------------
//      OpenWriter
// -----------------------------------------------------------------------------

CCaptureWriter*
CCaptureFile::OpenWriter(
	const path&			inPath,
	const tFileMode&	inMode )
{
	std::unique_ptr<CCaptureWriter>		spWriter;
	CF_FILE_FORMAT	nFileFormat = GetFileFormat( inPath );

	switch ( nFileFormat ) {

#ifdef CF_PEEK_FILE
	case CF_FORMAT_PEEK:
		spWriter = std::make_unique<CPeekCaptureWriter>( inMode.fAccess, inMode.fMediaType );
		break;
#endif

#ifdef CF_PCAP_FILE
	case CF_FORMAT_PCAP:
		spWriter = std::make_unique<CPCapCaptureWriter>( inMode.fAccess, inMode.fMediaType );
		break;

	case CF_FORMAT_PCAP_NG:
		spWriter = std::make_unique<CPCapNgCaptureWriter>( inMode.fAccess, inMode.fMediaType );
		break;
#endif

	case CF_FORMAT_UNKNOWN:
	default:
		throw -1;
		break;
	}

	if ( spWriter && !spWriter->Open( inPath ) ) {
		spWriter->Close();
	}

	return spWriter.release();
}

}	// namespace CaptureFile
