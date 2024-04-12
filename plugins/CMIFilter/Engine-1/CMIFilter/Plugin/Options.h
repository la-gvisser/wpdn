// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "resource.h"
#include "PeekTime.h"
#include "CMIHeaders.h"
#include "ByteVectRef.h"
#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// ============================================================================
//		Tags
// ============================================================================

namespace Tags
{
	const CPeekString	kTimeUnitsSingle[4] = {
		L"Second",
		L"Minute",
		L"Hour",
		L"Day"
	};
	const CPeekString	kTimeUnitsPlural[4] = {
		L"Seconds",
		L"Minutes",
		L"Hours",
		L"Days"
	};
};

namespace ConfigTags
{
	const CPeekString	kSent( L"Address 1 to 2" );
	const CPeekString	kReceived( L"Address 2 to 1" );
	const CPeekString	kEither( L"Both Directions" );
};


// ============================================================================
//		CInterceptId
// ============================================================================

class CInterceptId
{
public:
	enum {
		kIdType_None,
		kIdType_PCLI,
		kIdType_SecurityId,
		kIdType_Ericsson,
		kIdType_Arris,
		kIdType_Nokia
	};

protected:
	int			m_nType;
	UInt32		m_Id;
	std::string	m_strSite;

public:
	;		CInterceptId()
		:	m_nType( kIdType_None )
		,	m_Id( 0 )
	{
	}
	;		CInterceptId( UInt32 inId )
		:	m_nType( kIdType_None )
		,	m_Id( 0 )
	{
		Set( kIdType_SecurityId, inId );
	}
	;		CInterceptId( int inType, const CPeekString& inValue )
		:	m_nType( kIdType_None )
		,	m_Id( 0 )
	{
		Parse( inType, inValue );
	}
	;		CInterceptId( int inType, const CPeekString& inId, const CPeekString& inSite )
		:	m_nType( kIdType_None )
		,	m_Id( 0 )
	{
		Set( inType, inId, inSite );
	}
	;		CInterceptId( const CPeekString& inValue );

	bool	operator==( const CInterceptId& inOther ) const {
		if (m_nType != inOther.m_nType) return false;
		switch ( m_nType ) {
		case kIdType_None:
		case kIdType_PCLI:
			return true;

		case kIdType_SecurityId:
			return (m_Id == inOther.m_Id);

		case kIdType_Ericsson:
			if ( (inOther.m_Id != 0) && (m_Id != inOther.m_Id) ) return false;
			if ( !inOther.m_strSite.empty() ) {
				return (m_strSite.compare( inOther.m_strSite ) == 0);
			}
			return true;

		case kIdType_Arris:
		case kIdType_Nokia:
			return (m_Id == inOther.m_Id);
		}
		return false;
	}
	bool	operator!=( const CInterceptId& inOther ) const {
		if ( m_nType == inOther.m_nType ) return false;
		switch ( m_nType ) {
		case kIdType_None:
		case kIdType_PCLI:
			return true;

		case kIdType_SecurityId:
			return (m_Id != inOther.m_Id);
			break;

		case kIdType_Ericsson:
			if ( m_Id == inOther.m_Id ) return false;
			return (m_strSite.compare( inOther.m_strSite ) != 0);
			break;

		case kIdType_Arris:
		case kIdType_Nokia:
			return (m_Id != inOther.m_Id);
			break;
		}
		return true;
	}

	CPeekString 	Format( bool inPad = true ) const;

	void	Get( CPeekString& outId, CPeekString& outSite ) {
		switch ( m_nType ) {
		case kIdType_PCLI:
			break;

		case kIdType_SecurityId:
			outId = Format();
			break;

		case kIdType_Ericsson:
			if ( m_Id > 0 ) {
				outId = std::to_wstring( m_Id );
			}
			outSite.Convert( m_strSite );
			break;

		case kIdType_Arris:
		case kIdType_Nokia:
			if ( m_Id > 0 ) {
				outId = std::to_wstring( m_Id );
			}
			break;
		}
	}

	bool	HasId() const {
		switch ( m_nType ) {
		case kIdType_None:
		case kIdType_PCLI:
			return false;

		case kIdType_SecurityId:
			return (m_Id != 0);

		case kIdType_Ericsson:
			return ((m_Id != 0) || !m_strSite.empty());

		case kIdType_Arris:
		case kIdType_Nokia:
			return (m_Id != 0);
		}
		return false;
	}

	bool	IsNull() const { return (m_nType == kIdType_None); }

	void	Parse( int inType, const CPeekString& inValue );

	void	Reset() {
		m_nType = kIdType_None;
		m_Id = 0;
		m_strSite.clear();
	}
	
	void	Set( int inType, UInt32 inId ) {
		m_nType = inType;
		switch ( m_nType ) {
		case kIdType_SecurityId:
			m_Id = (inId & 0x0FFFFFFF);
			m_strSite.clear();
			break;

		case kIdType_Arris:
		case kIdType_Nokia:
			m_Id = inId;
			m_strSite.clear();
			break;
		}
	}
	void	Set( UInt32 inId, const std::string& inValue ) {
		m_nType = kIdType_Ericsson;
		std::string::size_type nIndex = inValue.find_last_not_of( " \t\n\v\f\0" );
		m_Id = inId;
		m_strSite = inValue.substr( 0, nIndex );
	}
	void	Set( UInt32 inId, const CPeekString& inValue ) {
		CPeekStringA	strValue( inValue );
		m_nType = kIdType_Ericsson;
		m_Id = inId;
		strValue.Trim();
		m_strSite = strValue;
	}
	void	Set( int inType, const CPeekString& inId, const CPeekString& inSite ) {
		Reset();
		UInt32 nId( 0 );
		switch ( inType ) {
		case kIdType_SecurityId:
			Parse( inType, inId );
			break;

		case kIdType_Ericsson:
			nId = wcstol( inId.c_str(), nullptr, 10 );
			Set( nId, inSite );
			break;

		case kIdType_Arris:
			nId = wcstol( inId.c_str(), nullptr, 10 );
			Set( kIdType_Arris, nId );
			break;

		case kIdType_Nokia:
			nId = wcstol( inId.c_str(), nullptr, 10 );
			Set( kIdType_Nokia, nId );
			break;

		default:
			break;
		}
	}
};


// ============================================================================
//		CSecurityId
// ============================================================================

class CSecurityId
{
public:
	static const UInt16	s_nSize = 16;
	static const UInt16	s_nByteCount = 16;	// s_nSize / sizeof( UInt8 )
	static const UInt16	s_nWordCount = 4;	// s_nSize / sizeof( UInt32 )
	static const UInt16	s_nLongCount = 2;	// s_nSize / sizeof( UInt64 )

protected:
	union {
		UInt64	fLong[s_nLongCount];
		UInt32	fWord[s_nWordCount];
		UInt8	fByte[s_nByteCount];
	} fId;
	CInterceptId	m_InterceptId;

protected:
	void	SetInterceptId() {
		m_InterceptId.Set( CInterceptId::kIdType_SecurityId, MemUtil::Swap32( fId.fWord[0] ) );
	}

public:
	;		CSecurityId() { Reset(); }
	;		CSecurityId( const CPeekString& inValue );
	;		CSecurityId( const size_t inLength, const UInt8* inValue )
	{
		memcpy( &fId, inValue, std::min( inLength, static_cast<size_t>( s_nSize ) ) );
		SetInterceptId();
	}

	bool	operator==( const CSecurityId& inOther ) const {
		return ( (fId.fLong[0] == inOther.fId.fLong[0]) && (fId.fLong[1] == inOther.fId.fLong[1]) );
	}
	bool	operator!=( const CSecurityId& inOther ) const {
		return ( (fId.fLong[0] != inOther.fId.fLong[0]) || (fId.fLong[1] != inOther.fId.fLong[1]) );
	}

	bool	CompareInterceptId( const CInterceptId& inId ) const {
		return (m_InterceptId == inId);
	}

	CPeekString	Format() const;

	bool	IsNull() const {
		return ( (fId.fLong[0] == 0) && (fId.fLong[1] == 0) );
	}
	
	void	Reset() {
		fId.fLong[0] = 0;
		fId.fLong[1] = 0;

		SetInterceptId();
	}
	
	void	Set( const UInt32* inWords ) {
		fId.fWord[0] = inWords[0];
		fId.fWord[1] = inWords[1];
		fId.fWord[2] = inWords[2];
		fId.fWord[3] = inWords[3];

		SetInterceptId();
	}
};


// ============================================================================
//		CEricsson
// ============================================================================

class CEricsson
{
protected:
	UInt16			m_nLength;
	CInterceptId	m_InterceptId;

public:
	;		CEricsson() : m_nLength( 0 ) {}
	;		CEricsson( const size_t inLength, const UInt8* inValue )
		:	m_nLength( 0 )
	{
		Parse( inLength, inValue );
	}

	bool	operator==( const CEricsson& inOther ) const {
		if ( m_nLength != inOther.m_nLength ) return false;
		return (m_InterceptId == inOther.m_InterceptId);
	}
	bool	operator!=( const CEricsson& inOther ) const {
		if ( m_nLength == inOther.m_nLength ) return false;
		return (m_InterceptId != inOther.m_InterceptId);
	}

	bool	CompareInterceptId( const CInterceptId& inId ) const {
		return (m_InterceptId == inId);
	}

	UInt16	GetLength() const { return m_nLength; }

	bool	IsNull() const {
		return m_InterceptId.IsNull();
	}
	
	bool	Parse( size_t inLength, const UInt8* inData );

	void	Reset() {
		m_nLength = 0;
		m_InterceptId.Reset();
	}
};


// ============================================================================
//		CArris
// ============================================================================

class CArris
{
protected:
	CInterceptId	m_InterceptId;

public:
	;		CArris() {}
	;		CArris( const size_t inLength, const UInt8* inValue )
	{
		Parse( inLength, inValue );
	}

	bool	operator==( const CArris& inOther ) const {
		return (m_InterceptId == inOther.m_InterceptId);
	}
	bool	operator!=( const CArris& inOther ) const {
		return (m_InterceptId != inOther.m_InterceptId);
	}

	bool	CompareInterceptId( const CInterceptId& inId ) const {
		return (m_InterceptId == inId);
	}

	UInt16	GetLength() const { return 4; }

	bool	IsNull() const {
		return m_InterceptId.IsNull();
	}
	
	bool	Parse( size_t inLength, const UInt8* inData );

	void	Reset() {
		m_InterceptId.Reset();
	}
};




// ============================================================================
//		CNokia
// ============================================================================

class CNokia
{
protected:
	CInterceptId	m_InterceptId;

public:
	;		CNokia() {}
	;		CNokia( const size_t inLength, const UInt8* inValue )
	{
		Parse( inLength, inValue );
	}

	bool	operator==( const CNokia& inOther ) const {
		return (m_InterceptId == inOther.m_InterceptId);
	}
	bool	operator!=( const CNokia& inOther ) const {
		return (m_InterceptId != inOther.m_InterceptId);
	}

	bool	CompareInterceptId( const CInterceptId& inId ) const {
		return (m_InterceptId == inId);
	}

	UInt16	GetLength() const { return 8; }

	bool	IsNull() const {
		return m_InterceptId.IsNull();
	}
	
	bool	Parse( size_t inLength, const UInt8* inData );

	void	Reset() {
		m_InterceptId.Reset();
	}
};


// ============================================================================
//		CIpFilter
// ============================================================================

class CIpFilter
{
public:
	typedef enum {
		kDirection_Either,
		kDirection_Sent,
		kDirection_Received
	} tDirectionType;

protected:
	CIpAddress		m_Address1;
	CIpAddress		m_Address2;
	bool			m_bAnyOtherAddress;
	int				m_Direction;		// tDirectionType
	bool			m_bPort;
	CPort			m_Port;

public:
	;		CIpFilter()
		:	m_bAnyOtherAddress( TRUE )
		,	m_Direction( kDirection_Either )
		,	m_bPort( FALSE )
	{}

	bool	operator==( const CIpFilter& inOther ) const {
		if ( m_Address1 != inOther.m_Address1 ) return false;
		if ( m_bAnyOtherAddress != inOther.m_bAnyOtherAddress ) return false;
		if ( !m_bAnyOtherAddress ) {
			if ( m_Address2 != inOther.m_Address2 ) return false;
		}
		if ( m_Direction != inOther.m_Direction ) return false;
		if ( m_bPort != inOther.m_bPort ) return false;
		if ( m_bPort ) {
			if ( m_Port != inOther.m_Port ) return false;
		}
		return true;
	}

	bool			Model( CPeekDataElement& ioElement );

	CPeekString		Format() const;
	CPeekString		FormatAddress1() const { return m_Address1.Format(); }
	CPeekString		FormatAddress2() const {
		CPeekString	str;
		if ( IsAnyOtherAddress() ) {
			str = L"Any Address";
		}
		else {
			str = m_Address2.Format();
		}
		return str;
	}
	CPeekString		FormatDirection() const {
		CPeekString str;
		switch ( m_Direction ) {
			case kDirection_Sent:
				str = ConfigTags::kSent;
				break;
			case kDirection_Received:
				str = ConfigTags::kReceived;
				break;
			case kDirection_Either:
			default:
				str = ConfigTags::kEither;
				break;
		}
		return str;
	}
	CPeekString		FormatPort() const {
		CPeekString	str;
		if ( IsPort() ) {
			str = m_Port.Format();
		}
		return str;
	}

	CIpAddress		GetAddress1() const { return m_Address1; }
	CIpAddress		GetAddress2() const { return m_Address2; }
	tDirectionType	GetDirection() const { return static_cast<tDirectionType>( m_Direction ); }
	CPort			GetPort() const { return m_Port; }

	bool		IsAnyOtherAddress() const { return (m_bAnyOtherAddress != false); }
	bool		IsMatch( const CIpAddressPair& inOther, bool inIsUdp, const CPort& inPort ) const;
	bool		IsPort() const { return (m_bPort != FALSE); }

	void		SetAddress1( const CIpAddress& in ) { m_Address1 = in; }
	void		SetAddress2( const CIpAddress& in ) { m_Address2 = in; }
	void		SetAnyAddress( bool inEnable ) { m_bAnyOtherAddress = inEnable; }
	void		SetDirection( tDirectionType in ) { m_Direction = in; }
	void		SetDirection( CPeekString in ) {
		if ( in.CompareNoCase( ConfigTags::kSent ) == 0 ) {
			m_Direction = kDirection_Sent;
		}
		else if ( in.CompareNoCase( ConfigTags::kReceived ) == 0 ) {
			m_Direction = kDirection_Received;
		}
		else {
			m_Direction = kDirection_Either;
		}
	}
	void		SetPort( bool inEnable ) { m_bPort = inEnable; }
	void		SetPort( const CPort& inPort ) { m_Port = inPort; }
};


// ============================================================================
//		CIpFilterList
// ============================================================================

class CIpFilterList
	:	public std::vector<CIpFilter>
{
public:
	;		CIpFilterList() {}

	bool	Compare( const CIpFilterList& inOther ) const;

	bool		Find( const CIpFilter& inIpFilter ) const;
	CPeekString	Format() const;

	bool	IsMatch( const CIpAddressPair& inOther, bool inIsUdp, const CPort& inPort ) const;
};


// =============================================================================
//		COptions
// =============================================================================

class COptions
{
	friend class COptionsDialog;

public:
	typedef enum {
		kDirection_Either,
		kDirection_Sent,
		kDirection_Received
	} tDirectionType;

	typedef enum {
		kType_CMI,
		kType_PCLI,
		kType_SecurityId,
		kType_Ericsson,
		kType_Arris,
		kType_Juniper,
		kType_Max
	} tProtocolType;

	typedef enum {
		kUnits_Seconds,
		kUnits_Minutes,
		kUnits_Hours,
		kUnits_Days
	} tTimeUntis;

protected:
	static const CPeekString	s_ayProtocolNames[kType_Max];

protected:
	int					m_ProtocolType;		// tProtocolType
	CInterceptId		m_InterceptId;
	CIpFilterList		m_ayIpFilters;
	BOOL				m_bInsertMAC;
	CCmiEthernetAddress	m_SrcEthernet;
	CCmiEthernetAddress m_DstEthernet;
	BOOL				m_bIP;
	UInt16				m_nMACProtocolType;
	BOOL				m_bInsertVLAN;
	UInt16				m_nVlanId;
	UInt32				m_nFragmentAge;
	BOOL				m_bSaveOrphans;
	UInt32				m_nSaveInterval;
	CPeekString			m_strFileName;

protected:
	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther );

public:
	;			COptions() { Reset(); }
	virtual		~COptions() {}

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool		operator==( const COptions& inOther ) { return IsEqual( inOther ); }
	bool		operator!=( const COptions& inOther ) { return !IsEqual( inOther ); }

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		AddIpFilter( const CIpFilter& inIpFilter ) {
		m_ayIpFilters.push_back( inIpFilter );
	}

	CPeekString		FormatDstMAC() const {
		CPeekString	str;
		if ( IsInsertMAC() )  {
			str = m_DstEthernet.Format();
		}
		return str;
	}

	CPeekString		FormatFileName() const {
		CPeekString	str;
		if ( IsSaveOrphans() ) {
			str = m_strFileName;
		}
		return str;
	}
	CPeekString		FormatFragmentAge() const {
		return std::to_wstring( m_nFragmentAge );
	}
	CPeekString		FormatInterceptId() const {
		return m_InterceptId.Format();
	}
	CPeekString		FormatMACProtocolType() const {
		CPeekString	str;
		if ( IsInsertMAC() ) {
			str = (m_bIP) ? L"IP" : std::to_wstring( m_nMACProtocolType );
		}
		return str;
	}
	CPeekString		FormatProtocolTypeHex() const {
		CPeekOutString	str;
		if ( IsInsertMAC() && !m_bIP )  {
			str << std::hex << std::uppercase << std::setw( 4 ) << std::setfill( L'0' ) <<
				m_nMACProtocolType << std::dec << std::endl;
		}
		return str;
	}
	CPeekString		FormatProtocolType() const {
		return s_ayProtocolNames[GetProtocolType()];
	}
	CPeekString		FormatSaveInterval() const {
		CPeekString	str;
		if ( IsSaveOrphans() )  {
			str = std::to_wstring( m_nSaveInterval );
		}
		return str;
	}
	CPeekString		FormatSaveIntervalLong() const {
		CPeekString	str;
		if ( IsSaveOrphans() )  {
			UInt32	nCount;
			int		nUnits;
			GetSaveInterval( nCount, nUnits );
			const wchar_t*	strUnits = (nCount == 1)
				? Tags::kTimeUnitsSingle[nUnits]
				: Tags::kTimeUnitsPlural[nUnits];
			str = std::to_wstring( nCount ) + L" " + strUnits;
		}
		return str;
	}
	CPeekString		FormatSrcMAC() const {
		CPeekString	str;
		if ( IsInsertMAC() )  {
			str = m_SrcEthernet.Format();
		}
		return str;
	}
	CPeekString		FormatVlanId() const {
		CPeekString	str;
		if ( IsInsertVlan() ) {
			str = std::to_wstring( m_nVlanId );
		}
		return str;
	}

	CCmiEthernetAddress	GetDstEthernet() const { return m_DstEthernet; }
	CPeekString			GetFileName() const { return m_strFileName; }
	UInt32				GetFragmentAge() const { return m_nFragmentAge; }
	CInterceptId		GetInterceptId() const { return m_InterceptId; }
	const CIpFilterList&	GetIpFilterList() const { return m_ayIpFilters; }
	UInt16				GetMACProtocolType() const { return m_nMACProtocolType; }
	UInt32				GetProtocolType() const { return m_ProtocolType; }
	UInt32				GetSaveInterval() const { return m_nSaveInterval; }
	void				GetSaveInterval( UInt32& outCount, int& outUnits ) const {
		if ( (m_nSaveInterval % kSecondsInAnHour ) == 0 ) {
			outCount = m_nSaveInterval / kSecondsInAnHour;
			outUnits = kUnits_Hours;
		}
		else if ( (m_nSaveInterval % kSecondsInAMinute) == 0 ) {
			outCount = m_nSaveInterval / kSecondsInAMinute;
			outUnits = kUnits_Minutes;
		}
		else {
			outCount = m_nSaveInterval;
			outUnits = kUnits_Seconds;
		}
	}
	CCmiEthernetAddress	GetSrcEthernet() const { return m_SrcEthernet; }
	CPeekString			GetUnits( int inUnits ) const {
		return Tags::kTimeUnitsPlural[inUnits];
	}
	UInt16		GetVlanId() const { return m_nVlanId; }

	bool		HasInterceptId() const { return m_InterceptId.HasId(); }

	bool		IsArris() const { return (m_ProtocolType == kType_Arris); }
	bool		IsCMI() const { return (m_ProtocolType == kType_CMI); }
	bool		IsInsertMAC() const { return (m_bInsertMAC != FALSE); }
	bool		IsInsertVlan() const { return (m_bInsertVLAN != FALSE); }
	bool		IsIpFilter() const { return !m_ayIpFilters.empty(); }
	bool		IsIpFilterMatch( const CIpAddressPair& inAddressPair, bool inIsUdp, const CPort& inPort ) const {
		return m_ayIpFilters.IsMatch( inAddressPair, inIsUdp, inPort );
	}
	bool		IsEricsson() const { return (m_ProtocolType == kType_Ericsson); }
	bool		IsMACProtocolIp() const { return (m_bIP != FALSE); }
	bool		IsJuniper() const { return (m_ProtocolType == kType_Juniper); }
	bool		IsSaveOrphans() const { return (m_bSaveOrphans != FALSE); }
	bool		IsSecurityId() const { return (m_ProtocolType == kType_SecurityId); }

	void		Reset();
	void		RemoveIpFilter( int inIndex ) {
		if ( inIndex < 0 ) return;
		m_ayIpFilters.erase( m_ayIpFilters.begin() + inIndex );
	}

	void		SetDstEthernet( const CCmiEthernetAddress& in ) { m_DstEthernet = in; }
	void		SetDstEthernet( const CPeekString& in ) { m_DstEthernet = in; }
	void		SetFileName( const CPeekString& in ) { m_strFileName = in; }
	void		SetFragmentAge( UInt32 in ) { m_nFragmentAge = (in >=5) ? in : 0; }
	void		SetInsertMAC( bool inEnable ) { m_bInsertMAC = inEnable; }
	void		SetInsertVlan( bool inEnable ) { m_bInsertVLAN = inEnable; }
	void		SetInterceptId( const CInterceptId& inId ) { m_InterceptId = inId; }
	void		SetInterceptId( int inType, const CPeekString& inId, const CPeekString& inSite ) {
		m_InterceptId.Set( inType, CPeekString( inId ), CPeekString( inSite ) );
	}
	void		SetIpFilterList( const CIpFilterList& inList ) { m_ayIpFilters.clear(); m_ayIpFilters = inList; }
	void		SetMACProtocolType( bool inIsIP, UInt16 inType ) { m_bIP = inIsIP; m_nMACProtocolType = inType; }
	void		SetProtocolType( int inType ) { m_ProtocolType = (inType >= 0) ? inType : 0; }
	void		SetSaveInterval( UInt32 in ) { m_nSaveInterval = in; }
	void		SetSaveInterval( UInt32 inCount, int inUnits ) {
		switch ( inUnits ) {
			case kUnits_Minutes:
				m_nSaveInterval = inCount * kSecondsInAMinute;
				break;
			case kUnits_Hours:
				m_nSaveInterval = inCount * kSecondsInAnHour;
				break;
			case kUnits_Seconds:
			default:
				m_nSaveInterval = inCount;
		}
	}
	void		SetSaveOrphans( bool inEnable ) { m_bSaveOrphans = inEnable; }
	void		SetSrcEthernet( const CCmiEthernetAddress& in ) { m_SrcEthernet = in; }
	void		SetSrcEthernet( const CPeekString& in ) { m_SrcEthernet = in; }
	void		SetVlanId( UInt16 in ) { m_nVlanId = (in & 0x0FFF); }	// only 0-4095

	void		UpdateIpFilter( int inIndex, const CIpFilter& inIpFilter ) {
		if ( (inIndex >= 0) && (inIndex < static_cast<int>( m_ayIpFilters.size() )) ) {
			m_ayIpFilters[inIndex] = inIpFilter;
		}
	}
};
