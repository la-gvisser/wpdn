// ============================================================================
// Options.h:
//      interface for the COptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "resource.h"
#include "XString.h"
#include "IpAddress.h"
#include "ByteStream.h"
#include "LockedPtr.h"
#include "Utils.h"
#include <vector>
#include <string>


// ============================================================================
//		Tags
// ============================================================================

namespace Tags
{
	const CString	kTimeUnitsSingle[4] = {
		_T( "Second" ),
		_T( "Minute" ),
		_T( "Hour" ),
		_T( "Day" )
	};
	const CString	kTimeUnitsPlural[4] = {
		_T( "Seconds" ),
		_T( "Minutes" ),
		_T( "Hours" ),
		_T( "Days" )
	};
};

namespace ConfigTags
{
	const CString	kSent( L"Address 1 to 2" );
	const CString	kReceived( L"Address 2 to 1" );
	const CString	kEither( L"Both Directions" );
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
		kIdType_Juniper
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
	;		CInterceptId( int inType, const CString inValue )
		:	m_nType( kIdType_None )
		,	m_Id( 0 )
	{
		Parse( inType, inValue );
	}
	;		CInterceptId( int inType, const CString inId, const CString inSite )
		:	m_nType( kIdType_None )
		,	m_Id( 0 )
	{
		Set( inType, inId, inSite );
	}
	;		CInterceptId( const CString inValue );

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
			return (m_Id == inOther.m_Id);

		case kIdType_Juniper:
			return ((m_Id & 0x3FFFFFFF) == (inOther.m_Id & 0x3FFFFFFF));
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

		case kIdType_Ericsson:
			if ( m_Id == inOther.m_Id ) return false;
			return (m_strSite.compare( inOther.m_strSite ) != 0);

		case kIdType_Arris:
			return (m_Id != inOther.m_Id);

		case kIdType_Juniper:
			return ((m_Id & 0x3FFFFFFF) != (inOther.m_Id & 0x3FFFFFFF));
		}
		return true;
	}

	CString	Format( bool inPad = true ) const;

	void	Get( CString& outId, CString& outSite ) {
		switch ( m_nType ) {
		case kIdType_PCLI:
			break;

		case kIdType_SecurityId:
			outId = Format();
			break;

		case kIdType_Ericsson:
			outId.Format( L"%u", m_Id );
			outSite = m_strSite.c_str();
			break;

		case kIdType_Arris:
			outId.Format( L"%u", m_Id );
			break;

		case kIdType_Juniper:
			outId.Format( L"%u", m_Id );
			break;
		}
	}

	bool	HasId() const {
		switch ( m_nType ) {
		case kIdType_None:
			return false;

		case kIdType_PCLI:
			return false;

		case kIdType_SecurityId:
			return (m_Id != 0);

		case kIdType_Ericsson:
			return ((m_Id != 0) || !m_strSite.empty());

		case kIdType_Arris:
			return (m_Id != 0);

		case kIdType_Juniper:
			return (m_Id != 0);
		}
		return false;
	}

	bool	IsNull() const { return (m_nType == kIdType_None); }

	void	Parse( int inType, const CString inValue );

	void	Reset() {
		m_nType = kIdType_None;
		m_Id = 0;
		m_strSite.clear();
	}
	
	void	Set( int inType, UInt32 inId ) {
		Reset();
		m_nType = inType;
		switch (m_nType) {
		case kIdType_SecurityId:
			m_Id = (inId & 0x0FFFFFFF);
			m_strSite.clear();
			break;

		case kIdType_Arris:
			m_Id = inId;
			m_strSite.clear();
			break;

		case kIdType_Juniper:
			m_Id = (inId & 0x3FFFFFFF);
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
	void	Set( UInt32 inId, const CString inValue ) {
		CStringA	strValue( inValue );
		m_nType = kIdType_Ericsson;
		m_Id = inId;
		m_strSite = strValue;
	}
	void	Set( int inType, const CString inId, const CString inSite ) {
		Reset();
		switch ( inType ) {
		case kIdType_SecurityId:
			Parse( inType, inId );
			break;

		case kIdType_Ericsson:
			Set( _wtoi( inId ), inSite );
			break;

		case kIdType_Arris:
			Set( kIdType_Arris, _wtoi( inId ) );
			break;

		case kIdType_Juniper:
			Set( inType, _wtoi( inId ) );
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
	;		CSecurityId( const CString inValue );
	;		CSecurityId( const size_t inLength, const UInt8* inValue )
	{
		memcpy( &fId, inValue, min( inLength, s_nSize ) );
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

	CString	Format() const;

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
//		CJuniper & Nokia
// ============================================================================

class CJuniper
{
protected:
	CInterceptId	m_InterceptId;

public:
	;		CJuniper() {}
	;		CJuniper( const size_t inLength, const UInt8* inValue )
	{
		Parse( inLength, inValue );
	}

	bool	operator==( const CJuniper& inOther ) const {
		return (m_InterceptId == inOther.m_InterceptId);
	}
	bool	operator!=( const CJuniper& inOther ) const {
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
	BOOL			m_bAnyOtherAddress;
	tDirectionType	m_Direction;
	BOOL			m_bPort;
	CIpPort			m_Port;

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

	CString		Format() const;
	CString		FormatAddress1() const { return m_Address1.Format(); }
	CString		FormatAddress2() const {
		CString	str;
		if ( IsAnyOtherAddress() ) {
			str = "Any Address";
		}
		else {
			str = m_Address2.Format();
		}
		return str;
	}
	CString		FormatDirection() const {
		CString str;
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
	CString		FormatPort() const {
		CString	str;
		if ( IsPort() ) {
			str = m_Port.Format();
		}
		return str;
	}

	CIpAddress		GetAddress1() const { return m_Address1; }
	CIpAddress		GetAddress2() const { return m_Address2; }
	tDirectionType	GetDirection() const { return m_Direction; }
	CIpPort			GetPort() const { return m_Port; }

	bool		IsAnyOtherAddress() const { return (m_bAnyOtherAddress != FALSE); }
	bool		IsMatch( const CIpAddressPair& inOther, bool inIsUdp, const CIpPort& inPort ) const;
	bool		IsPort() const { return (m_bPort != FALSE); }

	void		SetAddress1( const CIpAddress& in ) { m_Address1 = in; }
	void		SetAddress2( const CIpAddress& in ) { m_Address2 = in; }
	void		SetAnyAddress( bool inEnable ) { m_bAnyOtherAddress = inEnable; }
	void		SetDirection( tDirectionType in ) { m_Direction = in; }
	void		SetDirection( CString in ) {
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
	void		SetPort( const CIpPort& inPort ) { m_Port = inPort; }
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

	bool	Find( const CIpFilter& inIpFilter ) const;
	CString	Format() const;

	bool	IsMatch( const CIpAddressPair& inOther, bool inIsUdp, const CIpPort& inPort ) const;
};


// ============================================================================
//		COptions
// ============================================================================

class COptions
{
	friend class COptionsDialog;
	friend class COptionsStatus;

public:
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
	static CString	s_ayProtocolNames[kType_Max];

protected:
	mutable CMutex	m_Mutex;			// used by COptionsPtr

	int				m_ProtocolType;		// tProtocolType
	CInterceptId	m_InterceptId;
	CIpFilterList	m_ayIpFilters;
	BOOL			m_bInsertMAC;
	CMacAddress		m_SrcMAC;
	CMacAddress		m_DstMAC;
	BOOL			m_bIP;
	CUInt16			m_nMACProtocolType;
	BOOL			m_bInsertVLAN;
	CUInt16			m_nVLANID;
	CUInt32			m_nFragmentAge;
	BOOL			m_bSaveOrphans;
	CUInt32			m_nSaveInterval;
	CString			m_strFileName;

public:
	;		COptions()
		:	m_ProtocolType( kType_CMI )
		,	m_bInsertMAC( FALSE )
		,	m_bIP( TRUE )
		,	m_nMACProtocolType( 0x0800 )
		,	m_bInsertVLAN( FALSE )
		,	m_nVLANID( 0 )
		,	m_nFragmentAge( 5 )
		,	m_bSaveOrphans( FALSE )
		,	m_nSaveInterval( kSecondsInADay )
	{
	}

	void		AddIpFilter( const CIpFilter& inIpFilter ) {
		m_ayIpFilters.push_back( inIpFilter );
	}

	void		Copy( const COptions& in );

	CString		FormatDstMAC() const {
		CString	str;
		if ( IsInsertMAC() )  {
			str = m_DstMAC.Format();
		}
		return str;
	}
	CString		FormatFileName() const {
		CString	str;
		if ( IsSaveOrphans() ) {
			str = m_strFileName;
		}
		return str;
	}
	CString		FormatFragmentAge() const {
		return m_nFragmentAge.Format();
	}
	CString		FormatInterceptId() const {
		return m_InterceptId.Format();
	}
	CString		FormatMACProtocolType() const {
		CString	str;
		if ( IsInsertMAC() )  {
			str = (m_bIP) ? "IP" : m_nMACProtocolType.Format();
		}
		return str;
	}
	CString		FormatMACProtocolTypeHex() const {
		CString	str;
		if ( IsInsertMAC() & !m_bIP )  {
			str.Format( _T( "%04X" ), m_nMACProtocolType );
		}
		return str;
	}
	CString		FormatProtocolType() const {
		return s_ayProtocolNames[GetProtocolType()];
	}
	CString		FormatSaveInterval() const {
		CString	str;
		if ( IsSaveOrphans() )  {
			str = m_nSaveInterval.Format();
		}
		return str;
	}
	CString		FormatSaveIntervalLong() const {
		CString	str;
		if ( IsSaveOrphans() )  {
			UInt32	nCount;
			int		nUnits;
			GetSaveInterval( nCount, nUnits );
			CString	strUnits = (nCount == 1)
				? Tags::kTimeUnitsSingle[nUnits]
				: Tags::kTimeUnitsPlural[nUnits];
			str.Format( _T( "%u %s" ), nCount, strUnits );
		}
		return str;
	}
	CString		FormatSrcMAC() const {
		CString	str;
		if ( IsInsertMAC() )  {
			str = m_SrcMAC.Format();
		}
		return str;
	}
	CString		FormatVLANID() const {
		CString	str;
		if ( IsInsertVLAN() )  {
			str = m_nVLANID.Format();
		}
		return str;
	}

	CStringA		GetContextPrefs() const;
	CMacAddress		GetDstMAC() const { return m_DstMAC; }
	CString			GetFileName() const { return m_strFileName; }
	CUInt32			GetFragmentAge() const { return m_nFragmentAge; }
	CInterceptId	GetInterceptId() const { return m_InterceptId; }
	const CIpFilterList&	GetIpFilterList() const { return m_ayIpFilters; }
	CUInt16			GetMACProtocolType() const { return m_nMACProtocolType; }
	UInt32			GetProtocolType() const { return m_ProtocolType; }
	CUInt32			GetSaveInterval() const { return m_nSaveInterval; }
	void			GetSaveInterval( UInt32& outCount, int& outUnits ) const {
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
	CMacAddress		GetSrcMAC() const { return m_SrcMAC; }
	CString			GetUnits( int inUnits ) const {
		return Tags::kTimeUnitsPlural[inUnits];
	}
	CUInt16			GetVLANID() const { return m_nVLANID; }

	bool		HasInterceptId() const { return m_InterceptId.HasId(); }

	bool		IsArris() const { return (m_ProtocolType == kType_Arris); }
	bool		IsCMI() const { return (m_ProtocolType == kType_CMI); }
	bool		IsEricsson() const { return (m_ProtocolType == kType_Ericsson); }
	bool		IsInsertMAC() const { return (m_bInsertMAC != FALSE); }
	bool		IsInsertVLAN() const { return (m_bInsertVLAN != FALSE); }
	bool		IsIpFilter() const { return !m_ayIpFilters.empty(); }
	bool		IsIpFilterMatch( const CIpAddressPair& inAddressPair, bool inIsUdp, const CIpPort& inPort ) const {
		return m_ayIpFilters.IsMatch( inAddressPair, inIsUdp, inPort );
	}
	bool		IsJuniper() const { return (m_ProtocolType == kType_Juniper); }
	bool		IsMACProtocolIp() const { return (m_bIP != FALSE); }
	bool		IsSaveOrphans() const { return (m_bSaveOrphans != FALSE); }
	bool		IsSecurityId() const { return (m_ProtocolType == kType_SecurityId); }

	bool		Lock() const { return (m_Mutex.Lock( DEFAULT_TIMEOUT ) != FALSE); }	// used by COptionsPtr

	void		RemoveIpFilter( int inIndex ) {
		if ( inIndex < 0 ) return;
		m_ayIpFilters.erase( m_ayIpFilters.begin() + inIndex );
	}

	bool		SetContextPrefs( wchar_t* inPrefs ) { return SetContextPrefs( CStringA( inPrefs ) ); }
	bool		SetContextPrefs( CStringA inPrefs );
	void		SetDstMAC( const CMacAddress& inAddress ) { m_DstMAC = inAddress; }
	void		SetDstMAC( CString inAddress ) { m_DstMAC = inAddress; }
	void		SetFileName( CString inFileName ) { m_strFileName = inFileName; }
	void		SetFragmentAge( CUInt32 inAge ) { m_nFragmentAge = (inAge >=5 ) ? inAge : 0; }
	void		SetInsertMAC( bool inEnable ) { m_bInsertMAC = inEnable; }
	void		SetInsertVLAN( bool inEnable ) { m_bInsertVLAN = inEnable; }
	void		SetInterceptId( const CInterceptId& inId ) { m_InterceptId = inId; }
	void		SetInterceptId( int inType, const CString inId, const CString inSite ) {
		m_InterceptId.Set( inType, inId, inSite );
	}
	void		SetIpFilterList( const CIpFilterList& inList ) { m_ayIpFilters.clear(); m_ayIpFilters = inList; }
	void		SetMACProtocolType( bool inIsIP, CUInt16 inType ) { m_bIP = inIsIP; m_nMACProtocolType = inType; }
	void		SetProtocolType( int inType ) { m_ProtocolType = (inType >= 0) ? inType : 0; }
	void		SetProtocolType( const CString inType );
	void		SetSaveInterval( CUInt32 inInterval ) { m_nSaveInterval = inInterval; }
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
	void		SetSaveInterval( UInt32 inCount, CString inUnits );
	void		SetSaveOrphans( bool inEnable ) { m_bSaveOrphans = inEnable; }
	void		SetSrcMAC( const CMacAddress& inAddress ) { m_SrcMAC = inAddress; }
	void		SetSrcMAC( CString inAddress ) { m_SrcMAC = inAddress; }
	void		SetVLANID( CUInt16 inId ) { m_nVLANID = (inId & 0x0FFF); }	// only 0-4095

	void		Unlock() const { m_Mutex.Unlock(); }		// used by COptionsPtr
	void		UpdateIpFilter( int inIndex, const CIpFilter& inIpFilter ) {
		if ( (inIndex >= 0) && (inIndex < static_cast<int>( m_ayIpFilters.size() )) ) {
			m_ayIpFilters[inIndex] = inIpFilter;
		}
	}
};


// ============================================================================
//	COptionsPtr
// ============================================================================

typedef class TLockedPtr<COptions>	COptionsPtr;


// ============================================================================
//	CSafeOptions
// ============================================================================

class CSafeOptions
{
protected:
	COptions	m_Options;

public:
	;		CSafeOptions() {}

	COptionsPtr	Get() { return COptionsPtr( &m_Options ); }

	bool		Copy( const COptions& in ) {
		if ( m_Options.Lock() ) {
			m_Options.Copy( in );
			m_Options.Unlock();
			return true;
		}
		return false;
	};
	bool		Duplicate( COptions& out ) const {
		if ( m_Options.Lock() ) {
			out.Copy( m_Options );
			m_Options.Unlock();
			return true;
		}
		return false;
	};
};
