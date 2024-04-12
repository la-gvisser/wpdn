// =============================================================================
//	PeekCaptureFile.cpp
//		implementation of the CPeekCaptureFile class.
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekCaptureFile.h"
#include "tagfile.h"
#include "MemUtil.h"

using std::wstring;
using std::string;
using std::stringstream;
using namespace TaggedFile;
using namespace MemUtil;


namespace CaptureFile {

// ============================================================================
//		Constants and Data Structures
// ============================================================================

CVersionBlock::CIdList			CVersionBlock::s_AttribIds;
CSessionBlock::CIdList			CSessionBlock::s_AttribIds;
CSessionBlock::CChannelIdList	CSessionBlock::s_ChannelAttribIds;
CFileIdBlock::CIdList			CFileIdBlock::s_AttribIds;


// ============================================================================
//		CBlock
// ============================================================================

// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CBlock::Parse(
	size_t					inLength,
	const byte*				inData,
	const CAttributeIdList&	inIds,
	CBlockAttributeList&	outAttribs )
{
	CXmlString	strXml;
	strXml.Load( inLength, inData );

	size_t	nCount = inIds.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		string	strValue;

		if ( strXml.GetAttribute( CAttribute::GetBlockName( inIds[i] ), strValue ) ) {
			outAttribs.Add( inIds[i], strValue );
		}
	}
}


// -----------------------------------------------------------------------------
//      ParseList
// -----------------------------------------------------------------------------

void
CBlock::ParseList(
	const string&			inData,
	UInt16					inId,
	CBlockAttributeList&	outAttribs )
{
	CXmlString		strXml( inData );
	const string&	strId( CAttribute::GetBlockName( inId ) );
	size_t			nOffset = 0;
	string			strValue;
	while ( strXml.GetAttribute( strId, nOffset, strValue ) ) {
		outAttribs.ListAdd( inId, strValue );
	} 
}


// -----------------------------------------------------------------------------
//      Read
// -----------------------------------------------------------------------------

bool
CBlock::Read(
	COSFile&	inFile )
{
	BLOCK_HEADER	hdrBlock;
	UInt32			nBytesRead = inFile.Read(  sizeof( hdrBlock ), &hdrBlock );
	if ( nBytesRead >= sizeof( hdrBlock ) ) {
		m_nType = hdrBlock.nType;
		m_nFlags = hdrBlock.nFlags;
		m_nLength = hdrBlock.nLength;
		return true;
	}

	return false;
}


// -----------------------------------------------------------------------------
//      ReadData
// -----------------------------------------------------------------------------

bool
CBlock::ReadData(
	COSFile&	inFile )
{
	if ( m_nLength == 0 ) return true;
	_ASSERTE( m_nLength < kMaxUInt32 );
	if ( m_nLength >= kMaxUInt32 ) return false;

	UInt32	nLength = static_cast<UInt32>( m_nLength );
	m_bvData.resize( nLength );
	UInt32	nBytesRead = inFile.Read( nLength, &m_bvData[0] );
	return (nBytesRead >= nLength );
}


// ============================================================================
//		CVersionBlock
// ============================================================================

CVersionBlock::CVersionBlock()
	:	m_nVersion( 9 )
{
	m_nVersion = 9;
	m_AppVersion.Parse( L"6.6.0.0" );
	m_ProductVersion.Parse( L"6.5.0.0" );
}

CVersionBlock::CVersionBlock(
	const CBlock&	inOther )
	:	CFileBlock( inOther, false )
	,	m_nVersion( 9 )
{
	if ( inOther.GetType() != kCaptureFileVersionBlock ) return;

	if ( !inOther.GetData().empty() ) {
		Parse( inOther.GetData() );
		return;
	}

	const CVersionBlock*	pVersion = dynamic_cast<const CVersionBlock*>( &inOther );
	if ( pVersion ) {
		m_nVersion = pVersion->m_nVersion;
		m_AppVersion = pVersion->m_AppVersion;
		m_ProductVersion = pVersion->m_ProductVersion;
	}
}


// -----------------------------------------------------------------------------
//      s_AttribIds
// -----------------------------------------------------------------------------

CVersionBlock::CIdList::CIdList()
{
	push_back( CBlockNameList::Version_FileVersion );
	push_back( CBlockNameList::Version_AppVersion );
	push_back( CBlockNameList::Version_ProductVersion );
}


// -----------------------------------------------------------------------------
//      Build
// -----------------------------------------------------------------------------

void
CVersionBlock::Build(
	CBlockAttributeList&	outAttribs )
{
	stringstream	ssVersion;
	ssVersion << m_nVersion;
	outAttribs.Add( CBlockNameList::Version_FileVersion, ssVersion.str() );

	if ( !m_AppVersion.IsNull() ) {
		outAttribs.Add( CBlockNameList::Version_AppVersion, m_AppVersion.FormatA() );
	}

	if ( !m_ProductVersion.IsNull() ) {
		outAttribs.Add( CBlockNameList::Version_ProductVersion, m_ProductVersion.FormatA() );
	}
}


// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

size_t
CVersionBlock::Load(
	size_t				inLength,
	const BLOCK_HEADER*	inHeader )
{
	size_t	nBlockSize = sizeof( BLOCK_HEADER ) + inHeader->nLength;
	byte*	pData = ((byte*) inHeader) + sizeof( BLOCK_HEADER );

	if ( nBlockSize <= inLength) {
		Parse( inHeader->nLength, pData );
	}

	return nBlockSize;
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CVersionBlock::Parse(
	size_t		inLength,
	const byte*	inData )
{
	CBlockAttributeList	ayAttribs;
	CBlock::Parse( inLength, inData, s_AttribIds, ayAttribs );
	
	size_t	nCount = ayAttribs.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		switch ( ayAttribs[i].GetId() ) {
			case CBlockNameList::Version_FileVersion:
				m_nVersion = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Version_AppVersion:
				m_AppVersion.ParseA( ayAttribs[i].GetValue() );
				break;
			case CBlockNameList::Version_ProductVersion:
				m_ProductVersion.ParseA( ayAttribs[i].GetValue() );
				break;
			default:
				_ASSERTE( ayAttribs[i].GetId() != ayAttribs[i].GetId() );
				break;
		}
	}
}


// -----------------------------------------------------------------------------
//      ReadData
// -----------------------------------------------------------------------------

bool
CVersionBlock::ReadData(
	COSFile&	inFile )
{
	if ( !CBlock::ReadData( inFile ) ) return false;

	Parse( m_bvData );

	return true;
}


// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

size_t
CVersionBlock::Store(
	CByteVector&	outData )
{
	CXmlString	strXml( CAttribute::GetBlockName( CBlockNameList::Version_Root ).c_str() );
	size_t		nEmptyXmlLen = strXml.size();

	CBlockAttributeList	ayAttribs;
	Build( ayAttribs );
	for ( size_t i = 0; i < ayAttribs.size(); i++ ) {
		strXml.AddAttribute(
			CAttribute::GetBlockName( ayAttribs[i].GetId() ),
			ayAttribs[i].GetValue() );
	}

	if ( strXml.size() == nEmptyXmlLen ) return 0;

	const UInt32	nBlockSize = 0x0800;
	outData.resize( nBlockSize, ' ' );

	BLOCK_HEADER*	pBlockHeader = reinterpret_cast<BLOCK_HEADER*>( &outData[0] );
	pBlockHeader->nType = Swap32( kCaptureFileVersionBlock );
	pBlockHeader->nLength = nBlockSize - sizeof( BLOCK_HEADER );
	pBlockHeader->nFlags = BLOCK_FLAGS( Uncompressed, XMLFormat );

	std::copy( strXml.begin(), strXml.end(), (outData.begin() + sizeof( BLOCK_HEADER )) );

	return outData.size();
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

bool
CVersionBlock::Write(
	COSFile&	inFile )
{
	CByteVector	bvData;
	Store( bvData );

	_ASSERTE( bvData.size() < kMaxUInt32 );
	if ( bvData.size() >= kMaxUInt32 ) {
		return false;
	}

	UInt32	nLength = static_cast<UInt32>( bvData.size() );
	UInt32	nWrite = inFile.Write( nLength, &bvData[0] );
	return (nWrite == nLength);
}


// ============================================================================
//		CSessionBlock
// ============================================================================

CSessionBlock::CSessionBlock(
	tMediaType	inType )
	:	m_RawTime( tLongTime::Now() )
	,	m_TimeZoneBias( 0 )
	,	m_LinkSpeed( 0 )
	,	m_PacketCount( 0 )
	,	m_FileIndex( 0 )
{
	m_Time = m_RawTime.FormatA( CFileTime::kFormat_Milliseconds );
	m_TimeZoneBias = CFileTime::GetTimeBias();
	m_LinkSpeed = 11000000;

	SetMediaType( inType );
}

CSessionBlock::CSessionBlock(
	const CBlock&	inOther )
	:	CFileBlock( inOther, false )
	,	m_RawTime( 0 )
	,	m_TimeZoneBias( 0 )
	,	m_MediaType( kMediaType_802_3, kMediaSubType_Native )
	,	m_LinkSpeed( 0 )
	,	m_PacketCount( 0 )
	,	m_FileIndex( 0 )
{
	if ( inOther.GetType() != kCaptureFileSessionBlock ) return;

	if ( !inOther.GetData().empty() ) {
		Parse( inOther.GetData() );
		return;
	}

	const CSessionBlock*	pSession = dynamic_cast<const CSessionBlock*>( &inOther );
	if ( pSession ) {
		m_RawTime = pSession->m_RawTime;
		m_Time = pSession->m_Time;
		m_TimeZoneBias = pSession->m_TimeZoneBias;
		m_MediaType = pSession->m_MediaType;
		m_LinkSpeed = pSession->m_LinkSpeed;
		m_PacketCount = pSession->m_PacketCount;
		m_Comment = pSession->m_Comment;
		m_SessionStartTime = pSession->m_SessionStartTime;
		m_SessionEndTime = pSession->m_SessionEndTime;
		m_AdapterName = pSession->m_AdapterName;
		m_AdapterAddr = pSession->m_AdapterAddr;
		m_CaptureName = pSession->m_CaptureName;
		m_CaptureId = pSession->m_CaptureId;
		m_Owner = pSession->m_Owner;
		m_FileIndex = pSession->m_FileIndex;
		m_Host = pSession->m_Host;
		m_EngineName = pSession->m_EngineName;
		m_EngineAddr = pSession->m_EngineAddr;
		m_MediaDomain = pSession->m_MediaDomain;

		m_ayRates = pSession->m_ayRates;
		m_ayChannels = pSession->m_ayChannels;
	}
}


// -----------------------------------------------------------------------------
//      s_AttribIds
// -----------------------------------------------------------------------------

CSessionBlock::CIdList::CIdList()
{
	push_back( CBlockNameList::Session_RawTime );
	push_back( CBlockNameList::Session_Time );
	push_back( CBlockNameList::Session_TimeZoneBias );
	push_back( CBlockNameList::Session_MediaType );
	push_back( CBlockNameList::Session_MediaSubType );
	push_back( CBlockNameList::Session_LinkSpeed );
	push_back( CBlockNameList::Session_PacketCount );
	push_back( CBlockNameList::Session_Comment );
	push_back( CBlockNameList::Session_SessionStartTime );
	push_back( CBlockNameList::Session_SessionEndTime );
	push_back( CBlockNameList::Session_AdapterName );
	push_back( CBlockNameList::Session_AdapterAddr );
	push_back( CBlockNameList::Session_CaptureName );
	push_back( CBlockNameList::Session_CaptureID );
	push_back( CBlockNameList::Session_Owner );
	push_back( CBlockNameList::Session_FileIndex );
	push_back( CBlockNameList::Session_Host );
	push_back( CBlockNameList::Session_EngineName );
	push_back( CBlockNameList::Session_EngineAddr );
	push_back( CBlockNameList::Session_MediaDomain );
	push_back( CBlockNameList::Session_DataRates );
	push_back( CBlockNameList::Session_ChannelList );
}


// -----------------------------------------------------------------------------
//      s_ChannelAttribIds
// -----------------------------------------------------------------------------

CSessionBlock::CChannelIdList::CChannelIdList()
{
	push_back( CBlockNameList::Session_Channel_Number );
	push_back( CBlockNameList::Session_Channel_Frequency );
	push_back( CBlockNameList::Session_Channel_Band );
}


// -----------------------------------------------------------------------------
//      Build
// -----------------------------------------------------------------------------

void
CSessionBlock::Build(
	CBlockAttributeList&	outAttribs )
{
	bool	bWireless = m_MediaType.IsWireless();

	if ( m_RawTime != 0 ) {
		stringstream	ss;
		ss << m_RawTime;
		outAttribs.Add( CBlockNameList::Session_RawTime, ss.str() );
	}
	if ( !m_Time.empty() ) {
		outAttribs.Add( CBlockNameList::Session_Time, m_Time );
	}
	{
		stringstream	ss;
		ss << m_MediaType.fType;
		outAttribs.Add( CBlockNameList::Session_MediaType, ss.str() );
	}
	{
		stringstream	ss;
		ss << m_MediaType.fSubType;
		outAttribs.Add( CBlockNameList::Session_MediaSubType, ss.str() );
	}
	if ( m_TimeZoneBias != 0) {
		stringstream	ss;
		ss << m_TimeZoneBias;
		outAttribs.Add( CBlockNameList::Session_TimeZoneBias, ss.str() );
	}
	if ( m_LinkSpeed != 0 ) {
		stringstream	ss;
		ss << m_LinkSpeed;
		outAttribs.Add( CBlockNameList::Session_LinkSpeed, ss.str() );
	}
	if ( m_PacketCount != 0 ) {
		stringstream	ss;
		ss << m_PacketCount;
		outAttribs.Add( CBlockNameList::Session_PacketCount, ss.str() );
	}
	if ( !m_Comment.empty() ) {
		outAttribs.Add( CBlockNameList::Session_Comment, m_Comment );
	}
	if ( !m_SessionStartTime.IsNull() ) {
		outAttribs.Add( CBlockNameList::Session_SessionStartTime, m_SessionStartTime.FormatA() );
	}
	if ( !m_SessionEndTime.IsNull() ) {
		outAttribs.Add( CBlockNameList::Session_SessionEndTime, m_SessionEndTime.FormatA() );
	}
	if ( !m_AdapterName.empty() ) {
		outAttribs.Add( CBlockNameList::Session_AdapterName, m_AdapterName );
	}
	if ( !m_AdapterAddr.empty() ) {
		outAttribs.Add( CBlockNameList::Session_AdapterAddr, m_AdapterAddr );
	}
	if ( !m_CaptureName.empty() ) {
		outAttribs.Add( CBlockNameList::Session_CaptureName, m_CaptureName );
	}
	if ( !m_CaptureId.empty() ) {
		outAttribs.Add( CBlockNameList::Session_CaptureID, m_CaptureId );
	}
	if ( !m_Owner.empty() ) {
		outAttribs.Add( CBlockNameList::Session_Owner, m_Owner );
	}
	if ( m_FileIndex != 0 ) {
		stringstream	ss;
		ss << m_FileIndex;
		outAttribs.Add( CBlockNameList::Session_FileIndex, ss.str() );
	}
	if ( !m_Host.empty() ) {
		outAttribs.Add( CBlockNameList::Session_Host, m_Host );
	}
	if ( !m_EngineName.empty() ) {
		outAttribs.Add( CBlockNameList::Session_EngineName, m_EngineName );
	}
	if ( !m_EngineAddr.empty() ) {
		outAttribs.Add( CBlockNameList::Session_EngineAddr, m_EngineAddr );
	}
	if ( !m_MediaDomain.empty() ) {
		outAttribs.Add( CBlockNameList::Session_MediaDomain, m_MediaDomain );
	}
	if ( bWireless ) {
		string	strRates;
		if ( BuildRateList( strRates ) ) {
			outAttribs.Add( CBlockNameList::Session_DataRates, strRates );
		}
		string	strChannels;
		if ( BuildChannelList( strChannels ) ) {
			outAttribs.Add( CBlockNameList::Session_ChannelList, strChannels );
		}
	}
}


// -----------------------------------------------------------------------------
//      BuildChannelList
// -----------------------------------------------------------------------------

bool
CSessionBlock::BuildChannelList(
	string&	outChannelList )
{
	static const string&	strNameChannel =
		CAttribute::GetBlockName( CBlockNameList::Session_Channel );
	static const string&	strNameNumber =
		CAttribute::GetBlockName( CBlockNameList::Session_Channel_Number );
	static const string&	strNameFrequency =
		CAttribute::GetBlockName( CBlockNameList::Session_Channel_Frequency );
	static const string&	strNameBand =
		CAttribute::GetBlockName( CBlockNameList::Session_Channel_Band );

	size_t	nCount = m_ayChannels.size();
	if ( nCount == 0 ) return false;

	for ( size_t i = 0; i < nCount; i++ ) {
		tChannel	chanItem = m_ayChannels[i];

		CXmlString		strChannel;
		strChannel.Set( strNameChannel );

		stringstream	ssNumber;
		ssNumber << chanItem.fNumber;
		strChannel.AddAttribute( strNameNumber, ssNumber.str() );

		stringstream	ssFrequency;
		ssFrequency << chanItem.fFrequency;
		strChannel.AddAttribute( strNameFrequency, ssFrequency.str() );

		stringstream	ssBand;
		ssBand << chanItem.fBand;
		strChannel.AddAttribute( strNameBand, ssBand.str() );

		outChannelList.append( strChannel );
	}

	return true;
}


// -----------------------------------------------------------------------------
//      BuildRateList
// -----------------------------------------------------------------------------

bool
CSessionBlock::BuildRateList(
	string&	outRateList )
{
	size_t	nCount = m_ayRates.size();
	if ( nCount == 0 ) return false;

	const string&	strName = CAttribute::GetBlockName( CBlockNameList::Session_Rate );
	for ( size_t i = 0; i < nCount; i++ ) {
		stringstream	ss;
		ss << m_ayRates[i];

		CXmlString	strXml;
		strXml.SetAttribute( strName, ss.str() );

		outRateList.append( strXml );
	}

	return true;
}


// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

size_t
CSessionBlock::Load(
	size_t				inLength,
	const BLOCK_HEADER*	inHeader )
{
	size_t	nBlockSize = sizeof( BLOCK_HEADER ) + inHeader->nLength;
	byte*	pData = ((byte*) inHeader) + sizeof( BLOCK_HEADER );

	if ( nBlockSize <= inLength ) {
		Parse( inHeader->nLength, pData );
	}

	return nBlockSize;
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CSessionBlock::Parse(
	size_t		inLength,
	const byte*	inData )
{
	CBlockAttributeList	ayAttribs;
	CBlock::Parse( inLength, inData, s_AttribIds, ayAttribs );
	
	size_t	nCount = ayAttribs.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		switch ( ayAttribs[i].GetId() ) {
			case CBlockNameList::Session_RawTime:
				m_RawTime = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_Time:
				m_Time = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_TimeZoneBias:
				m_TimeZoneBias = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_MediaType:
				m_MediaType.fType = static_cast<TMediaType>( atoi( ayAttribs[i].GetValue().c_str() ) );
				break;
			case CBlockNameList::Session_MediaSubType:
				m_MediaType.fSubType = static_cast<TMediaSubType>( atoi( ayAttribs[i].GetValue().c_str() ) );
				break;
			case CBlockNameList::Session_LinkSpeed:
				m_LinkSpeed = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_PacketCount:
				m_PacketCount = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_Comment:
				m_Comment = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_SessionStartTime:
				m_SessionStartTime.Parse( ayAttribs[i].GetValue() );
				break;
			case CBlockNameList::Session_SessionEndTime:
				m_SessionEndTime.Parse( ayAttribs[i].GetValue() );
				break;
			case CBlockNameList::Session_AdapterName:
				m_AdapterName = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_AdapterAddr:
				m_AdapterAddr = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_CaptureName:
				m_CaptureName = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_CaptureID:
				m_CaptureId = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_Owner:
				m_Owner = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_FileIndex:
				m_FileIndex = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_Host:
				m_Host = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_EngineName:
				m_EngineName = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_EngineAddr:
				m_EngineAddr = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_MediaDomain:
				m_MediaDomain = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::Session_DataRates:
				{
					string	strRates = ayAttribs[i].GetValue();
					ParseRateList( strRates );
				}
				break;
			case CBlockNameList::Session_ChannelList:
				{
					string strChannels = ayAttribs[i].GetValue();
					ParseChannelList( strChannels );
				}
				break;
			default:
				_ASSERTE( ayAttribs[i].GetId() != ayAttribs[i].GetId() );
				break;
		}
	}
}


// -----------------------------------------------------------------------------
//      ParseChannel
// -----------------------------------------------------------------------------

bool
CSessionBlock::ParseChannel(
	const string&				inChannel,
	CSessionBlock::tChannel&	outChannel )
{
	//  <Channel>
	//    <Number>1</Number>
	//    <Frequency>2412</Frequency>
	//    <Band>4</Band>
	//  </Channel>

	CBlockAttributeList	ayAttribs;
	const UInt8*		pData = reinterpret_cast<const UInt8*>( &inChannel[0] );
	CBlock::Parse( inChannel.size(), pData, s_ChannelAttribIds, ayAttribs );
	
	size_t	nCount = ayAttribs.size();
	if ( nCount < 3 ) return false;

	for ( size_t i = 0; i < nCount; i++ ) {
		// Parse each Channel
		switch ( ayAttribs[i].GetId() ) {
			case CBlockNameList::Session_Channel_Number:
				outChannel.fNumber = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_Channel_Frequency:
				outChannel.fFrequency = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			case CBlockNameList::Session_Channel_Band:
				outChannel.fBand = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			default:
				_ASSERTE( ayAttribs[i].GetId() != ayAttribs[i].GetId() );
				break;
		}
	}
	return true;
}


// -----------------------------------------------------------------------------
//      ParseChannelList
// -----------------------------------------------------------------------------

void
CSessionBlock::ParseChannelList(
	const string&	inChannels )
{
	//	<ChannelList>
	//    <Channel>
	//      <Number>1</Number>
	//      <Frequency>2412</Frequency>
	//      <Band>4</Band>
	//    </Channel>
	//  </ChannelList>

	static const int	nChannelId = CBlockNameList::Session_Channel;

	m_ayChannels.clear();

	CBlockAttributeList	ayAttribs;
	CBlock::ParseList( inChannels, nChannelId, ayAttribs );

	size_t	nCount = ayAttribs.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		string	strChannel;
		// Parse each Channel
		if ( ayAttribs[i].GetId() == nChannelId ) {
			strChannel = ayAttribs[i].GetValue();
			if ( !strChannel.empty() ) {
				tChannel	chanItem;
				if ( ParseChannel( strChannel, chanItem ) ) {
					m_ayChannels.push_back( chanItem );
				}
			}
		}
	}
}


// -----------------------------------------------------------------------------
//      ParseRateList
// -----------------------------------------------------------------------------

void
CSessionBlock::ParseRateList(
	const string&	inRates )
{
	//  <DataRates>
	//    <Rate>1</Rate>
	//    <Rate>2</Rate>
	//  </DataRates>

	static const int	nRateId = CBlockNameList::Session_Rate;

	m_ayRates.clear();

	CBlockAttributeList	ayAttribs;
	CBlock::ParseList( inRates, nRateId, ayAttribs );

	size_t	nCount = ayAttribs.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		if ( ayAttribs[i].GetId() == nRateId ) {
			m_ayRates.push_back( atoi( ayAttribs[i].GetValue().c_str() ) );
		}
	}
}


// -----------------------------------------------------------------------------
//      ReadData
// -----------------------------------------------------------------------------

bool
CSessionBlock::ReadData(
	COSFile&	inFile )
{
	if ( !CBlock::ReadData( inFile ) ) return false;

	Parse( m_bvData );

	return true;
}


// -----------------------------------------------------------------------------
//      SetMediaType
// -----------------------------------------------------------------------------

void
CSessionBlock::SetMediaType(
	tMediaType inType )
{
#define ADD_RATES(_t) {												\
	for ( size_t i = 0; i < COUNTOF( _t ); i++ ) {					\
		m_ayRates.push_back( _t[i] );								\
	}																\
}
#define ADD_CHANNELS(_t,_b) {										\
	for ( size_t i = 0; i < COUNTOF( _t ); i++ ) {					\
		tChannel	ch = { _t[i], ChannelFrequency( _t[i] ), _b };	\
		m_ayChannels.push_back( ch );								\
	}																\
}

	m_MediaType = inType;

	if ( m_MediaType.IsWireless() ) {
		switch ( m_MediaType.fSubType ) {
			case kMediaSubType_802_11_b:
				ADD_RATES( s_ay802_11B_Rates );
				ADD_CHANNELS( s_ay802_11B_Channels, s_n802_11_Band_A );
				break;

			case kMediaSubType_802_11_a:
				ADD_RATES( s_ay802_11A_Rates );
				ADD_CHANNELS( s_ay802_11A_Channels, s_n802_11_Band_B );
				break;

			case kMediaSubType_802_11_gen:
				ADD_RATES( s_ay802_11_Rates );
				ADD_CHANNELS( s_ay802_11A_Channels, s_n802_11_Band_A );
				ADD_CHANNELS( s_ay802_11B_Channels, s_n802_11_Band_B );
				ADD_CHANNELS( s_ay802_11N_Low_Channels, s_n802_11_Band_N_Low );
				ADD_CHANNELS( s_ay802_11N_High_Channels, s_n802_11_Band_N_High );
				break;

			default:
				break;
		}
	}
}


// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

size_t
CSessionBlock::Store(
	CByteVector&	outData )
{
	CXmlString	strXml( CAttribute::GetBlockName( CBlockNameList::Session_Root ).c_str() );
	size_t		nEmptyXmlLen = strXml.size();

	CBlockAttributeList	ayAttribs;
	Build( ayAttribs );
	for ( size_t i = 0; i < ayAttribs.size(); i++ ) {
		strXml.AddAttribute(
			CAttribute::GetBlockName( ayAttribs[i].GetId() ),
			ayAttribs[i].GetValue() );
	}

	if ( strXml.size() == nEmptyXmlLen ) return 0;

	const size_t	nBlockSize = ((strXml.size() + 0x1000) / 0x1000) * 0x1000;
	_ASSERTE( nBlockSize < kMaxUInt32 );
	if ( nBlockSize >= kMaxUInt32 ) {
		throw -1;
	}
	outData.resize( nBlockSize, ' ' );

	BLOCK_HEADER*	pBlockHeader = reinterpret_cast<BLOCK_HEADER*>( &outData[0] );
	pBlockHeader->nType = Swap32( kCaptureFileSessionBlock );
	pBlockHeader->nLength = static_cast<UInt32>( nBlockSize - sizeof( BLOCK_HEADER ) );
	pBlockHeader->nFlags = BLOCK_FLAGS( Uncompressed, XMLFormat );

	std::copy( strXml.begin(), strXml.end(), (outData.begin() + sizeof( BLOCK_HEADER )) );

	return outData.size();
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

bool
CSessionBlock::Write(
	COSFile&	inFile )
{
	CByteVector	bvData;
	Store( bvData );

	_ASSERTE( bvData.size() < kMaxUInt32 );
	if ( bvData.size() >= kMaxUInt32 ) {
		return false;
	}

	UInt32	nLength = static_cast<UInt32>( bvData.size() );
	UInt32	nWrite = inFile.Write( nLength, &bvData[0] );
	return (nWrite == nLength);
}


// ============================================================================
//		CFileIdBlock
// ============================================================================

CFileIdBlock::CFileIdBlock(
	const CBlock&	inOther )
	:	CFileBlock( inOther, false )
{
	if ( inOther.GetType() != kCaptureFileIdBlock ) return;

	if ( !inOther.GetData().empty() ) {
		Parse( inOther.GetData() );
		return;
	}

	const CFileIdBlock*	pFileId = dynamic_cast<const CFileIdBlock*>( &inOther );
	if ( pFileId ) {
		m_strId = pFileId->m_strId;
		m_nIndex = pFileId->m_nIndex;
	}
}


// -----------------------------------------------------------------------------
//      s_AttribIds
// -----------------------------------------------------------------------------

CFileIdBlock::CIdList::CIdList()
{
	push_back( CBlockNameList::FileId_Id );
	push_back( CBlockNameList::FileId_Index );
}


// -----------------------------------------------------------------------------
//      Build
// -----------------------------------------------------------------------------

void
CFileIdBlock::Build(
	CBlockAttributeList&	outAttribs )
{
	if ( !m_strId.empty() ) {
		outAttribs.Add( CBlockNameList::FileId_Id, m_strId );
	}

	stringstream	ssIndex;
	ssIndex << m_nIndex;
	outAttribs.Add( CBlockNameList::FileId_Index, ssIndex.str() );
}


// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

size_t
CFileIdBlock::Load(
	size_t				inLength,
	const BLOCK_HEADER*	inHeader )
{
	size_t	nBlockSize = sizeof( BLOCK_HEADER ) + inHeader->nLength;
	byte*	pData = ((byte*) inHeader) + sizeof( BLOCK_HEADER );

	if ( nBlockSize <= inLength ) {
		Parse( inHeader->nLength, pData );
	}

	return nBlockSize;
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CFileIdBlock::Parse(
	size_t		inLength,
	const byte*	inData )
{
	CBlockAttributeList	ayAttribs;
	CBlock::Parse( inLength, inData, s_AttribIds, ayAttribs );
	
	size_t	nCount = ayAttribs.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		switch ( ayAttribs[i].GetId() ) {
			case CBlockNameList::FileId_Id:
				m_strId = ayAttribs[i].GetValue();
				break;
			case CBlockNameList::FileId_Index:
				m_nIndex = atoi( ayAttribs[i].GetValue().c_str() );
				break;
			default:
				_ASSERTE( ayAttribs[i].GetId() != ayAttribs[i].GetId() );
				break;
		}
	}
}


// -----------------------------------------------------------------------------
//      ReadData
// -----------------------------------------------------------------------------

bool
CFileIdBlock::ReadData(
	COSFile&	inFile )
{
	if ( !CBlock::ReadData( inFile ) ) return false;

	Parse( m_bvData );

	return true;
}


// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

size_t
CFileIdBlock::Store(
	CByteVector&	outData )
{
	CXmlString	strXml( CAttribute::GetBlockName( CBlockNameList::FileId_Root ).c_str() );
	size_t		nEmptyXmlLen = strXml.size();

	CBlockAttributeList	ayAttribs;
	Build( ayAttribs );
	for ( size_t i = 0; i < ayAttribs.size(); i++ ) {
		strXml.AddAttribute(
			CAttribute::GetBlockName( ayAttribs[i].GetId() ),
			ayAttribs[i].GetValue() );
	}

	if ( strXml.size() == nEmptyXmlLen ) return 0;

	const UInt32	nBlockSize = 0x0800;
	outData.resize( nBlockSize, ' ' );

	BLOCK_HEADER*	pBlockHeader = reinterpret_cast<BLOCK_HEADER*>( &outData[0] );
	pBlockHeader->nType = Swap32( kCaptureFileIdBlock );
	pBlockHeader->nLength = nBlockSize - sizeof( BLOCK_HEADER );
	pBlockHeader->nFlags = BLOCK_FLAGS( Uncompressed, XMLFormat );

	std::copy( strXml.begin(), strXml.end(), (outData.begin() + sizeof( BLOCK_HEADER )) );
	//std::fill( (outData.begin() + sizeof( BLOCK_HEADER ) + strXml.size()), outData.end(), ' ' );

	return outData.size();
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

bool
CFileIdBlock::Write(
	COSFile&	inFile )
{
	CByteVector	bvData;
	Store( bvData );

	_ASSERTE( bvData.size() < kMaxUInt32 );
	if ( bvData.size() >= kMaxUInt32 ) {
		return false;
	}

	UInt32	nLength = static_cast<UInt32>( bvData.size() );
	UInt32	nWrite = inFile.Write( nLength, &bvData[0] );
	return (nWrite == nLength);
}


// ============================================================================
//		CGPSBlock
// ============================================================================

CGPSBlock::CGPSBlock(
	const CBlock&	inOther )
	:	CFileBlock( inOther, false )
{
	if ( inOther.GetType() != kCaptureFileGpsBlock ) return;

	if ( !inOther.GetData().empty() ) {
		m_ayGPSInfo.Deserialize( inOther.GetData() );
		return;
	}

#if (0)
	const CGPSBlock*	pFileId = dynamic_cast<const CGPSBlock*>( &inOther );
	if ( pFileId ) {
		m_strId = pFileId->m_strId;
		m_nIndex = pFileId->m_nIndex;
	}
#endif
}


// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

size_t
CGPSBlock::Load(
	size_t				inLength,
	const BLOCK_HEADER*	inHeader )
{
	size_t	nBlockSize = sizeof( BLOCK_HEADER ) + inHeader->nLength;
	byte*	pData = ((byte*) inHeader) + sizeof( BLOCK_HEADER );

	if ( nBlockSize <= inLength ) {
		m_ayGPSInfo.Deserialize( inHeader->nLength, pData );
	}

	return nBlockSize;
}


// -----------------------------------------------------------------------------
//      ReadData
// -----------------------------------------------------------------------------

bool
CGPSBlock::ReadData(
	COSFile&	inFile )
{
	if ( !CBlock::ReadData( inFile ) ) return false;

	m_ayGPSInfo.Deserialize( m_bvData );

	return true;
}


// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

size_t
CGPSBlock::Store(
	CByteVector&	outData )
{
	return outData.size();
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

bool
CGPSBlock::Write(
	COSFile&	inFile )
{
	CByteVector	bvData;
	Store( bvData );

	_ASSERTE( bvData.size() < kMaxUInt32 );
	if ( bvData.size() >= kMaxUInt32 ) {
		return false;
	}

	UInt32	nLength = static_cast<UInt32>( bvData.size() );
	UInt32	nWrite = inFile.Write( nLength, &bvData[0] );
	return (nWrite == nLength);
}


// ============================================================================
//		CPacketsBlock
// ============================================================================

CPacketsBlock::CPacketsBlock(
	const CBlock&	inOther,
	bool			/*inCopyData*/ )
	:	CFileBlock( inOther, false )
{
}

CPacketsBlock::CPacketsBlock(
	const CPacketsBlock&	inOther )
	:	CFileBlock( inOther, false )
{
}


// -----------------------------------------------------------------------------
//      Load
// -----------------------------------------------------------------------------

size_t
CPacketsBlock::Load(
	tMediaType			/*inType*/,
	size_t				/*inLength*/,
	const BLOCK_HEADER*	/*inHeader*/ )
{
	return sizeof( BLOCK_HEADER );
#if (0)
	//const byte*	pData = ((const byte*) inHeader) + sizeof( BLOCK_HEADER );
	size_t	nDataSize =
		(inHeader->nLength)
		? inHeader->nLength
		: (inLength - sizeof( BLOCK_HEADER )); // if nLength == 0, then remainder is pkts.

	LoadPackets( inType, nDataSize, pData );

	size_t	nBlockSize = nDataSize + sizeof( BLOCK_HEADER );
	return nBlockSize;
#endif
}


#if (0)
// -----------------------------------------------------------------------------
//      LoadPackets
// -----------------------------------------------------------------------------

size_t
CPacketsBlock::LoadPackets(
	tMediaType	inType,
	size_t		inLength,
	const byte*	inData )
{
	const byte*	pData = inData;
	const byte*	pEndData = inData + inLength;

	while ( pData < pEndData ) {
		CFilePacket	thePacket( inType );

		size_t	nPacketLength = thePacket.Load( inLength, pData );
		if ( nPacketLength ) {
			pData += nPacketLength;
			nLength -= nPacketLength;
		}
		else {
			pData = pEndData;
		}
		m_Packets.push_back( thePacket );
	}
	return m_Packets.size();
}
#endif

#if (0)
// -----------------------------------------------------------------------------
//      PacketLength
// -----------------------------------------------------------------------------

size_t
CPacketsBlock::PacketLength(
	const byte*	inData )
{
	size_t	nPacketLength = 0;

	const PacketAttribute*	pAttrib = reinterpret_cast<const PacketAttribute*>( inData );

	switch ( pAttrib->fieldType ) {
	case Attrib_PacketDataLength:
		nPacketLength = pAttrib->fieldValue;
		break;

	default:
		break;
	}

	return nPacketLength;
}
#endif

// -----------------------------------------------------------------------------
//      ReadData
// -----------------------------------------------------------------------------

bool
CPacketsBlock::ReadData(
	COSFile&	inFile )
{
	if ( m_nLength > 0 ) {
		inFile.MoveTo( m_nLength );
	}
	else {
		inFile.SeekTo( inFile.GetSize() );
	}

	return true;
}


// -----------------------------------------------------------------------------
//      Store
// -----------------------------------------------------------------------------

size_t
CPacketsBlock::Store(
	CByteVector&	outData )
{
	outData.resize( sizeof( BLOCK_HEADER ) );

	{
		BLOCK_HEADER*	pBlockHeader = reinterpret_cast<BLOCK_HEADER*>( &outData[0] );

		pBlockHeader->nType = Swap32( kCaptureFilePacketsBlock );
		pBlockHeader->nLength = 0;
		pBlockHeader->nFlags = BLOCK_FLAGS( Uncompressed, BinaryFormat );
	}

	size_t	nLength = sizeof( BLOCK_HEADER );

#if (0)
	for ( size_t i = 0; i < m_Packets.size(); i++ ) {
		CByteVector	PacketData;
		if ( m_Packets[i].Store( PacketData ) ) {
			outData.insert( outData.end(), PacketData.begin(), PacketData.end() );
		}
	}

	size_t	nLength = outData.size();
	{
		// pBlockHeader is invalid after Data.Append()
		BLOCK_HEADER*	pBlockHeader = reinterpret_cast<BLOCK_HEADER*>( &outData[0] );
		pBlockHeader->nLength = static_cast<UInt32>( nLength - sizeof( BLOCK_HEADER ) );
	}
#endif

	return nLength;
}


// -----------------------------------------------------------------------------
//      Write
// -----------------------------------------------------------------------------

bool
CPacketsBlock::Write(
	COSFile&	inFile )
{
	BLOCK_HEADER	bhPackets;
	bhPackets.nType = Swap32( kCaptureFilePacketsBlock );
	bhPackets.nLength = 0;
	bhPackets.nFlags = BLOCK_FLAGS( Uncompressed, BinaryFormat );

	UInt32	nWrite = inFile.Write( sizeof( BLOCK_HEADER ), &bhPackets );
	return (nWrite == sizeof( BLOCK_HEADER ));
}


// ============================================================================
//		CPeekCaptureFile
// ============================================================================

// ============================================================================
//		CPeekCaptureReader
// ============================================================================

// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPeekCaptureReader::Open(
	const wstring&	inFileName,
	bool			inScanPackets )
{
	if ( CCaptureReader::Open( inFileName, inScanPackets ) ) {
		m_nFileFormat = CF_FORMAT_PEEK;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      Parse
// -----------------------------------------------------------------------------

void
CPeekCaptureReader::Parse(
	bool	inScanPackets /*= false*/ )
{
	if ( !IsOpen() ) return;

	COSFile&	wfRead( GetFile() );
	UInt64		nFileOffset = 0;
	CBlock		blk;
	while ( blk.Read( wfRead ) ) {
		switch ( blk.GetType() ) {
		case kCaptureFileVersionBlock:
			m_VersionBlock = blk;
			m_VersionBlock.SetOffset( nFileOffset );
			m_VersionBlock.ReadData( wfRead );
			break;

		case kCaptureFileSessionBlock:
			m_SessionBlock = blk;
			m_SessionBlock.SetOffset( nFileOffset );
			m_SessionBlock.ReadData( wfRead );
			break;

		case kCaptureFilePacketsBlock:
			m_PacketsBlock = blk;
			m_PacketsBlock.SetOffset( nFileOffset );
			m_PacketsBlock.ReadData( wfRead );
			break;

		case kCaptureFileIdBlock:
			m_FileIdBlock = blk;
			m_FileIdBlock.SetOffset( nFileOffset );
			m_FileIdBlock.ReadData( wfRead );
			break;

		case kCaptureFileGpsBlock:
			m_GPSBlock = blk;
			m_GPSBlock.SetOffset( nFileOffset );
			m_GPSBlock.ReadData( wfRead );
			break;

		default:
			_ASSERTE( blk.GetType() != blk.GetType() );
			break;
		}

		nFileOffset = wfRead.GetPosition();
	}

	if ( !m_SessionBlock.IsValid() || !m_PacketsBlock.IsValid() ) return;

	UInt64	nOffset = m_PacketsBlock.GetOffset() + sizeof( BLOCK_HEADER );
	UInt32	nPacketCount = m_SessionBlock.GetPacketCount();
	if ( inScanPackets && (nPacketCount > 0) ) {
		m_ayOffsets.SetFirstOffset( wfRead.GetSize(), nPacketCount, nOffset );
		SeekToPacket( nPacketCount );
	}
	else {
		m_ayOffsets.SetFirstOffset( wfRead.GetSize(), 1, nOffset );
	}
	SeekToPacket( 0 );
}


// -----------------------------------------------------------------------------
//      ReadFilePacket
// -----------------------------------------------------------------------------

bool
CPeekCaptureReader::ReadFilePacket(
	CFilePacket&	outPacket,
	UInt32&			outError,
	UInt32			inIndex )
{
	outError = CF_ERROR_NONE;

	if ( inIndex != static_cast<UInt32>( NEXT_PACKET ) ) {
		if ( !SeekToPacket( inIndex ) ) return false;
		if ( inIndex >= m_ayOffsets.size() ) return false;
	}

	outPacket.SetMediaType( m_SessionBlock.GetMediaType() );

	COSFile&		wfRead( GetFile() );
	const UInt32	nAttribMax = Attrib_FlagsN + 2;
	const UInt32	nAttribSize = nAttribMax * sizeof( PacketAttribute );
	byte			ayAttribData[nAttribSize];

	UInt32	nRead = wfRead.Read( nAttribSize, ayAttribData );
	if ( nRead < 6 ) {
		outError = CF_ERROR_UNEXPECTED_EOF;
		return false;
	}

	CPacketAttributeList	ayAttribs;
	ayAttribs.AddList( nAttribMax, reinterpret_cast<PacketAttribute*>( ayAttribData ) );
	if ( ayAttribs.empty() ) {
		outError = CF_ERROR_INVALID_HEADER;
		return false;
	}

	const SInt64	nDataSize( ayAttribs.size() * sizeof( PacketAttribute) );
	const SInt64	nOffset( nDataSize - nRead );
	wfRead.MoveTo( nOffset );

	UInt32	nDataLength = 0;
	outPacket.SetAttributes( ayAttribs, nDataLength );

	CByteVector	bvPacketData;
	bvPacketData.resize( nDataLength );

	UInt32	nDataRead = wfRead.Read( static_cast<UInt32>( bvPacketData.size() ), &bvPacketData[0] );
	_ASSERTE( nDataRead == bvPacketData.size() );
	if ( nDataRead != bvPacketData.size() ) {
		outError = CF_ERROR_INCOMPLETE_PAYLOAD;
		return false;
	}

	outPacket.SetPacketData( bvPacketData );

	// Update the packet Offsets.
	if ( m_SessionBlock.GetPacketCount() &&
		 (inIndex + 1) < m_SessionBlock.GetPacketCount() ) {
		m_ayOffsets.SetOffset( (inIndex + 1), wfRead.GetPosition() );
	}

	return true;
}


// -----------------------------------------------------------------------------
//      ReadFilePackets
// -----------------------------------------------------------------------------

bool
CPeekCaptureReader::ReadFilePackets(
	CFilePacketList&	outPackets,
	UInt32&				outError,
	UInt32				inCount,
	UInt32				inFirst /*= NEXT_PACKET*/)
{
	outError = CF_ERROR_NONE;

	if ( !SeekToPacket( inFirst ) ) return false;

	size_t	nPacketCount = m_ayOffsets.size();
	if ( inFirst > nPacketCount ) return false;

	size_t	nEndIndex = inFirst + inCount;
	if ( nEndIndex > nPacketCount ) {
		nEndIndex = nPacketCount;
	}

	try {
		outPackets.reserve( nEndIndex - inFirst );
		for ( UInt32 nIndex = inFirst; nIndex < nEndIndex; ++nIndex ) {
			CFilePacket	pkt( m_SessionBlock.GetMediaType() );
			if ( !ReadFilePacket( pkt, outError, nIndex ) ) break;
			outPackets.push_back( pkt );
		}
	}
	catch ( ... ) {
		;	// catch and squelch.
	}

	return !outPackets.empty();
}


// -----------------------------------------------------------------------------
//      SeekToPacket
// -----------------------------------------------------------------------------

bool
CPeekCaptureReader::SeekToPacket(
	UInt32	inIndex )
{
	if ( m_ayOffsets.empty() ) return false;

	COSFile&	wfRead( GetFile() );
	
	if ( m_ayOffsets.HasIndex( inIndex ) ) {
		wfRead.SeekTo( m_ayOffsets[inIndex] );
		return true;
	}

	if ( m_ayOffsets.IsComplete() ) return false;

	size_t	nIndex = m_ayOffsets.GetHighest();
	while ( nIndex <= inIndex ) {
		UInt64	nOffset = m_ayOffsets[nIndex];
		wfRead.SeekTo( nOffset );

		const UInt32	nAttribMax = Attrib_FlagsN + 2;
		const UInt32	nCount = nAttribMax * sizeof( PacketAttribute );
		byte			ayPacketData[nCount];
		UInt32			nRead = wfRead.Read( nCount, ayPacketData );
		if ( nRead < 6 ) return false;

		size_t				nPacketLength = 0;
		PacketAttribute*	pAttrib = reinterpret_cast<PacketAttribute*>( ayPacketData );
		for ( UInt32 a = 0; a < nAttribMax; a++ ) {
			if ( pAttrib->fieldType == Attrib_PacketDataLength ) {
				UInt32 nPacketDataLength = pAttrib->fieldValue;
				pAttrib++;
				nPacketLength =
					(reinterpret_cast<byte*>( pAttrib ) - reinterpret_cast<byte*>( ayPacketData ))
					+ nPacketDataLength;
				break;
			}
			pAttrib++;
		}
		_ASSERTE( nPacketLength > 0 );
		if ( nPacketLength == 0 ) {
			return false;
		}

		nIndex++;
		nOffset += nPacketLength;
		if ( !m_ayOffsets.SetOffset( nIndex, nOffset ) ) {
			if ( m_ayOffsets.IsEOF( nOffset ) ) {
				m_ayOffsets.SetComplete();
			}
			break;
		}
	}

	if ( !m_ayOffsets.HasIndex( inIndex ) ) return false;

	wfRead.SeekTo( m_ayOffsets[inIndex] );
	return true;
}


// ============================================================================
//		CPeekCaptureWriter
// ============================================================================

// -----------------------------------------------------------------------------
//      Close
// -----------------------------------------------------------------------------

void
CPeekCaptureWriter::Close()
{
	if ( IsOpen() ) {
		COSFile&	wfWrite( GetFile() );
		if ( (static_cast<SInt64>( m_SessionBlock.GetOffset() ) != -1) &&
				(wfWrite.GetSize() > m_PacketsBlock.GetOffset()) ) {
			wfWrite.SeekTo( m_SessionBlock.GetOffset() );
			m_SessionBlock.Write( wfWrite );
		}
		CCaptureWriter::Close();
	}
}


// -----------------------------------------------------------------------------
//      Init
// -----------------------------------------------------------------------------

void
CPeekCaptureWriter::Init(
	const CCaptureFile*	inTemplate )
{
	if ( !IsOpen() ) return;

	const CPeekCaptureWriter*	pTemplate = dynamic_cast<const CPeekCaptureWriter*>( inTemplate );
	if ( pTemplate ) {
		m_VersionBlock = pTemplate->GetVersionBlock();
		m_SessionBlock = pTemplate->GetSessionBlock();
		m_PacketsBlock = pTemplate->GetPacketsBlock();
		m_FileIdBlock.SetId( pTemplate->GetFileIdBlock().GetId() );

		m_SessionBlock.SetPacketCount( 0 );
	}

	COSFile&	wfWrite( GetFile() );

	m_VersionBlock.SetOffset( wfWrite.GetPosition() );
	m_VersionBlock.Write( wfWrite );

	m_SessionBlock.SetOffset( wfWrite.GetPosition() );
	m_SessionBlock.Write( wfWrite );

	(void)m_FileIdBlock;
	(void)m_GPSBlock;
	
	m_PacketsBlock.SetOffset( wfWrite.GetPosition() );
	m_PacketsBlock.Write( wfWrite );
}


// -----------------------------------------------------------------------------
//      Open
// -----------------------------------------------------------------------------

bool
CPeekCaptureWriter::Open(
	const wstring& inFileName )
{
	if ( CCaptureWriter::Open( inFileName ) ) {
		m_nFileFormat = CF_FORMAT_PEEK;
	}

	return IsOpen();
}


// -----------------------------------------------------------------------------
//      WriteFilePacket
// -----------------------------------------------------------------------------

bool
CPeekCaptureWriter::WriteFilePacket(
	const CFilePacket&	inPacket )
{
	size_t	nPacketDataLength = inPacket.GetPacketDataLength();
	_ASSERTE( nPacketDataLength < kMaxUInt32 );
	if ( nPacketDataLength >= kMaxUInt32 ) return false;

	CPacketAttributeList	ayAttribs;
	inPacket.GetAttributes( ayAttribs );
	size_t	nAttribCount = ayAttribs.size();

	COSFile&	wfWrite( GetFile() );

	// Refactor to a single write...
	for ( size_t i = 0; i < nAttribCount; i++ ) {
		PacketAttribute	pa = { ayAttribs[i].GetId(), ayAttribs[i].GetValue() };
		wfWrite.Write( sizeof( PacketAttribute ), &pa );
	}

	wfWrite.Write( static_cast<UInt32>( inPacket.GetPacketDataLength() ), inPacket.GetPacketData() );

	m_SessionBlock.AddPacket( inPacket.GetTimeStamp(), inPacket.GetLength() );

	return true;
}

}	// namespace CaptureFile
