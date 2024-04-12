// =============================================================================
//	CaptureFile.cpp
//		implementation of the CCaptureFile class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "CaptureFile.h"

#ifdef CF_PEEK_FILE
#include "PeekCaptureFile.h"
#endif

#ifdef CF_PCAP_FILE
#include "PCapCaptureFile.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <sstream>

using std::string;
using std::wstring;
using std::stringstream;
using namespace TaggedFile;


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

	if ( inOther == NULL ) return strResult;

	size_t	nLength = (inLength > 0) ? inLength : strlen( inOther );
	if ( nLength == 0 ) return strResult;

	size_t	nSize = ((inLength > 0) ? inLength : strlen( inOther )) + 1;
	std::auto_ptr<wchar_t>	spBuf( new wchar_t[nSize] );
	if ( spBuf.get() == NULL ) return strResult;

	size_t	nNewSize = 0;
	errno_t nErr = mbstowcs_s( &nNewSize, spBuf.get(), nSize, inOther, _TRUNCATE );
	if ( nErr == 0 ) {
		strResult.assign( spBuf.get(), (nNewSize - 1) );
	}

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

	if ( inOther == NULL ) return strResult;

	size_t	nSize = ((inLength > 0) ? inLength : wcslen( inOther )) + 1;
	std::auto_ptr<char>	spBuf( new char[nSize] );
	if ( spBuf.get() == NULL ) return strResult;

	size_t	nNewSize = 0;
	errno_t nErr = wcstombs_s( &nNewSize, spBuf.get(), nSize, inOther, _TRUNCATE );
	if ( nErr == 0 ) {
		strResult.assign( spBuf.get(), (nNewSize - 1) );
	}

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
	fm.fType = CF_TYPE_READER;
	return fm;
}
tFileMode
FileModeWriter(
	const tFileMode inMode )
{
	tFileMode fm = inMode;
	fm.fType = CF_TYPE_WRITER;
	return fm;
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

	AddName( kCaptureFileVersionBlock_Root );				// Version_Root
	AddName( kCaptureFileVersionBlock_AppVersion );			// Version_AppVersion
	AddName( kCaptureFileVersionBlock_ProductVersion );		// Version_ProductVersion
	AddName( kCaptureFileVersionBlock_FileVersion );		// Version_FileVersion

	AddName( kCaptureFileSessionBlock_Root );				// Session_Root
	AddName( kCaptureFileSessionBlock_RawTime );			// Session_RawTime
	AddName( kCaptureFileSessionBlock_Time );				// Session_Time
	AddName( kCaptureFileSessionBlock_TimeZoneBias );		// Session_TimeZoneBias
	AddName( kCaptureFileSessionBlock_MediaType );			// Session_MediaType
	AddName( kCaptureFileSessionBlock_MediaSubType );		// Session_MediaSubType
	AddName( kCaptureFileSessionBlock_LinkSpeed );			// Session_LinkSpeed
	AddName( kCaptureFileSessionBlock_PacketCount );		// Session_PacketCount
	AddName( kCaptureFileSessionBlock_Comment );			// Session_Comment
	AddName( kCaptureFileSessionBlock_SessionStartTime );	// Session_SessionStartTime
	AddName( kCaptureFileSessionBlock_SessionEndTime );		// Session_SessionEndTime
	AddName( kCaptureFileSessionBlock_AdapterName );		// Session_AdapterName
	AddName( kCaptureFileSessionBlock_AdapterAddr );		// Session_AdapterAddr
	AddName( kCaptureFileSessionBlock_CaptureName );		// Session_CaptureName
	AddName( kCaptureFileSessionBlock_CaptureID );			// Session_CaptureID
	AddName( kCaptureFileSessionBlock_Owner );				// Session_Owner
	AddName( kCaptureFileSessionBlock_FileIndex );			// Session_FileIndex
	AddName( kCaptureFileSessionBlock_Host );				// Session_Host
	AddName( kCaptureFileSessionBlock_EngineName );			// Session_EngineName
	AddName( kCaptureFileSessionBlock_EngineAddr );			// Session_EngineAddr
	AddName( kCaptureFileSessionBlock_MediaDomain );		// Session_MediaDomain
	AddName( kCaptureFileSessionBlock_DataRates );			// Session_DataRates
	AddName( kCaptureFileSessionBlock_Rate );				// Session_Rate
	AddName( kCaptureFileSessionBlock_ChannelList );		// Session_ChannelList
	AddName( kCaptureFileSessionBlock_Channel );			// Session_Channel
	AddName( kCaptureFileSessionBlock_Channel_Number );		// Session_Channel_Number
	AddName( kCaptureFileSessionBlock_Channel_Freq );		// Session_Channel_Freq
	AddName( kCaptureFileSessionBlock_Channel_Band );		// Session_Channel_Band

	AddName( kCaptureFileIdBlock_Root );					// FileId_Root
	AddName( kCaptureFileIdBlock_Id );						// FileId_Id
	AddName( kCaptureFileIdBlock_Index );					// FileId_Index

	AddName( L"Invalid Index" );							// Attrib_Count
}


// ============================================================================
//		CAttributeNameList
// ============================================================================

CAttributeNameList::CAttributeNameList()
{
	m_ayNames.reserve( 18 );

	AddName( "ActualLength" );
	AddName( "TimeStampLo" );
	AddName( "TimeStampHi" );
	AddName( "FlagsStatus" );
	AddName( "ChannelNumber" );
	AddName( "DataRate" );
	AddName( "SignalStrength" );
	AddName( "SignaldBm" );
	AddName( "NoiseStrength" );
	AddName( "NoisedBm" );
	AddName( "MediaSpecType" );
	AddName( "Protocol" );
	AddName( "Direction" );
	AddName( "ChannelFreq" );
	AddName( "ChannelBand" );
	AddName( "PacketDataLength" );
	AddName( "SliceLength" );
	AddName( "InvalidIndex" );
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

	for ( int i = 0; i < s_dBmCount; i++ ) {
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
	outAttribs.Add( TaggedFile::Attrib_TimeStampLo, ((tLongTime*)(&m_nTimeStamp))->sTime.TimeLo );
	outAttribs.Add( TaggedFile::Attrib_TimeStampHi, ((tLongTime*)(&m_nTimeStamp))->sTime.TimeHi );
	outAttribs.Add( TaggedFile::Attrib_FlagsStatus, (m_nFlags << 16) | (m_nStatus) );
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
	CWinFile&	inFile,
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
				((tLongTime*)(&m_nTimeStamp))->sTime.TimeLo = nValue;
				break;
			case Attrib_TimeStampHi:
				((tLongTime*)(&m_nTimeStamp))->sTime.TimeHi = nValue;
				break;
			case Attrib_FlagsStatus:
				m_nFlags = nValue >> 16;
				m_nStatus = nValue & 0x0FFFF;
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
	ULARGE_INTEGER	liSize = { 0, 0 };
	if ( IsOpen() ) {
		liSize.LowPart = ::GetFileSize(
			m_hFile, reinterpret_cast<LPDWORD>( &liSize.HighPart ) );
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
	DWORD	dwDesiredAccess = 0;
	DWORD	dwCreationDisposition = 0;
	if ( !ParseMode( inMode, dwDesiredAccess, dwCreationDisposition ) ) {
		return false;
	}

	DWORD	dwShareMode = FILE_SHARE_READ;
	LPSECURITY_ATTRIBUTES lpsa = NULL;
	DWORD	dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE	hTemplateFile = NULL;

	HANDLE hFile = ::CreateFile( inFileName.c_str(), dwDesiredAccess, dwShareMode, lpsa,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
	if ( hFile != INVALID_HANDLE_VALUE ) {
		m_hFile = hFile;
		SetName( inFileName );
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
	if ( (inLength > 0) && IsOpen() ) {
		UInt32	nRead = 0;
		if ( ::ReadFile( m_hFile, outData, inLength, &nRead, NULL ) != FALSE ) {
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
		wchar_t*	pszName = NULL;

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
		if ( ::WriteFile( m_hFile, inData, inLength, &nWrite, NULL ) != FALSE ) {
			return nWrite;
		}
	}
	return 0;
}


// ============================================================================
//		CBaseFile
// ============================================================================

bool
CBaseFile::Open(
	const wstring&	inFileName,
	int				inMode )
{
	m_File.Open( inFileName, inMode );

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
	if ( CBaseFile::Open( inFileName, CWinFile::FILEMODE_READ ) ) {
		Parse( inScanPackets );
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CCaptureReader::ReadFilePackets(
	CFilePacketList&	outPackets,
	UInt32				inCount,
	UInt32				inFirst /*= NEXT_PACKET*/)
{
	UInt32	nLast = inCount + ((inFirst == NEXT_PACKET) ? 0 : inFirst);
	for ( UInt32 i = inFirst; i < nLast; i++ ) {
		CFilePacket	pkt;
		if ( !ReadFilePacket( pkt, i ) ) break;
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
	UInt32	nMode = CWinFile::FILEMODE_WRITE | CWinFile::FILEMODE_CREATE;
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

// -----------------------------------------------------------------------------
//      GetFileType
// -----------------------------------------------------------------------------

int
CCaptureFile::GetFileType(
	const wstring&	inFileName )
{
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

#ifdef CF_PEEK_FILE
		if ( CPeekCaptureReader::IsKind( strExt ) ) {
			return CF_FORMAT_PEEK;
		}
#endif

#ifdef CF_PCAP_FILE
		if ( CPCapCaptureReader::IsKind( strExt ) ) {
			return CF_FORMAT_PCAP;
		}
#endif

	}

	return CF_FORMAT_UNKNOWN;
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
	CCaptureReader*	pReader = NULL;

	int	nFileType = GetFileType( inFileName );
	switch ( nFileType ) {

#ifdef CF_PEEK_FILE
	case CF_FORMAT_PEEK:
		pReader = new CPeekCaptureReader();
		break;
#endif

#ifdef CF_PCAP_FILE
	case CF_FORMAT_PCAP:
		pReader = new CPCapCaptureReader();
		break;
#endif
	}

	if ( pReader ) {
		pReader->Open( inFileName, inMode.fScan );
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
	CCaptureWriter*	pWriter = NULL;

	int	nFileType = GetFileType( inFileName );
	switch ( nFileType ) {

#ifdef CF_PEEK_FILE
	case CF_FORMAT_PEEK:
		pWriter = new CPeekCaptureWriter( inMode.fAccess, inMode.fMediaType );
		break;
#endif

#ifdef CF_PCAP_FILE
	case CF_FORMAT_PCAP:
		pWriter = new CPCapCaptureWriter( inMode.fAccess, inMode.fMediaType );
		break;
#endif
	}

	if ( pWriter ) {
		pWriter->Open( inFileName );
	}

	return pWriter;
}

}	// namespace CaptureFile
