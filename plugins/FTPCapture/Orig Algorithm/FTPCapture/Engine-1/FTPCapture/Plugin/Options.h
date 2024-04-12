// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;

// -----------------------------------------------------------------------------
//		CTargetName
// -----------------------------------------------------------------------------

class CTargetName
{
protected:
	bool		m_bEnabled;
	CPeekString	m_strName;

public:
	;		CTargetName() : m_bEnabled( false ) {}
	;		CTargetName( const CTargetName& inOther ) { Copy( inOther ); }
	;		CTargetName( bool inEnabled, CPeekString inName ) : m_bEnabled( inEnabled ), m_strName( inName ) {}
	;		~CTargetName() {}

	void	operator=( const CTargetName& inOther ) {
		Copy( inOther );
	}

	void	Copy( const CTargetName& inOther ) {
		m_bEnabled = inOther.m_bEnabled;
		m_strName = inOther.m_strName;
	}

	void	Disable() { m_bEnabled = false; }

	void	Enable() { m_bEnabled = true; }

	void	Get( bool& outEnabled, CPeekString& outName ) {
		outEnabled = m_bEnabled;
		outName = m_strName;
	}
	CPeekString	GetName() const { return m_strName; }

	CPeekString	Format() {
		CPeekOutString	str;
		str << m_bEnabled << m_strName;
		return str;
	}
	CPeekString	FormatName() const { return m_strName; }

	bool	IsEnabled() const { return m_bEnabled; }
	bool	IsMatch( CPeekString inName ) const {
		return (m_bEnabled)
			? (m_strName.CompareNoCase( inName ) == 0)
			: false;
	}

	bool	Model( CPeekDataElement& inParentElement );

	void	SetEnable( bool in ) { m_bEnabled = in; }
	void	SetName( CPeekString in ) { m_strName = in; }

	static bool	Validate( CPeekString& str );
};


// -----------------------------------------------------------------------------
//		CTargetNameList
// -----------------------------------------------------------------------------

class CTargetNameList
	:	public CAtlArray<CTargetName>
{
	bool	m_bCaptureAll;

public:
	;		CTargetNameList() : m_bCaptureAll( false ) {}
	;		CTargetNameList( const CTargetNameList& inOther ) { Copy( inOther ); }
	;		~CTargetNameList() {}

	CTargetNameList&	operator=( const CTargetNameList& inOther ) {
		Copy( inOther );
		return *this;
	}

	void	Copy( const CTargetNameList& inOther ) {
		m_bCaptureAll = inOther.IsCaptureAll();
		__super::Copy( inOther );
	}

	bool	IsCaptureAll() const { return m_bCaptureAll; }
	bool	IsMatch( CPeekString inName ) const {
		if ( IsCaptureAll() ) return true;
		for ( size_t i = 0; i < GetCount(); i++ ) {
			if ( GetAt( i ).IsMatch( inName ) ) return true;
		}
		return false;
	}

	bool	Model( CPeekDataElement& inParentElement );

	void	Reset() { m_bCaptureAll = false; RemoveAll(); }
	void	SetCaptureAll( bool inCaptureAll ) { m_bCaptureAll = inCaptureAll; }
};


// -----------------------------------------------------------------------------
//		CPortNumber
// -----------------------------------------------------------------------------

class CPortNumber
{
protected:
	bool	m_bEnabled;
	UInt16	m_nPort;

public:
	;		CPortNumber() : m_bEnabled( false ) {}
	;		CPortNumber( const CPortNumber& inOther ) { Copy( inOther ); }
	;		CPortNumber( bool inEnabled, UInt16 inPort ) : m_bEnabled( inEnabled ), m_nPort( inPort ) {}
	;		CPortNumber( bool inEnabled, CPeekString inPort )
		:	m_bEnabled( inEnabled ), m_nPort( (UInt16) _tstol( inPort ) ) {}
	;		~CPortNumber(){};

	void	operator=( const CPortNumber& inOther ) {
		Copy( inOther );
	}

	void	Copy( const CPortNumber& inOther ) {
		m_bEnabled = inOther.m_bEnabled;
		m_nPort = inOther.m_nPort;
	}

	void	Disable() { m_bEnabled = false; }

	void	Enable() { m_bEnabled = true; }

	CPeekString	Format() {
		CPeekOutString	str;
		str << m_bEnabled << m_nPort;
		return str;
	}
	CPeekString	FormatPort() const {
		CPeekOutString	str;
		str << m_nPort;
		return str;
	}

	void	Get( bool& outEnabled, CPeekString& outPort ) {
		outEnabled = m_bEnabled;
		outPort = FormatPort();
	}
	UInt16	GetPort() const { return m_nPort; }

	bool	IsEnabled() const { return m_bEnabled; }
	bool	IsMatch( UInt16 inPort ) const {
		return (m_bEnabled) ? (m_nPort == inPort) : false;
	}

	bool	Model( CPeekDataElement& inParentElement );

	void	SetEnable( bool in ) { m_bEnabled = in; }
	void	SetPort( UInt16 in ) { m_nPort = in; }
	void	SetPort( CPeekString inStr ) { m_nPort = static_cast<UInt16>( _tstol( inStr ) ); }

	static bool	Validate( CPeekString& str );
};


// -----------------------------------------------------------------------------
//		CPortNumberList
// -----------------------------------------------------------------------------

class CPortNumberList
	:	public CAtlArray<CPortNumber>
{
public:
	;		CPortNumberList() {}
	;		CPortNumberList( const CPortNumberList& inOther ) { Copy( inOther ); }
	;		~CPortNumberList() {}

	CPortNumberList&	operator=( const CPortNumberList& inOther ) {
		Copy( inOther );
		return *this;
	}

	bool	IsMatch( UInt16 inPort ) const {
		for ( size_t i = 0; i < GetCount(); i++ ) {
			if ( GetAt( i ).IsMatch( inPort ) ) return true;
		}
		return false;
	}

	bool	Model( CPeekDataElement& inParentElement );
};


// -----------------------------------------------------------------------------
//		CAlerting
// -----------------------------------------------------------------------------

class CAlerting
{
	friend class CAlertingPage;

protected:
	BOOL		m_bEnabled;
	CPeekString	m_strSource;
	CPeekString	m_strFacility;
	CPeekString	m_strOutputDirectory;

public:
	;		CAlerting() : m_bEnabled( FALSE ), m_strOutputDirectory( L"C:\\FTPCapture" ) {}
	;		CAlerting( const CAlerting& inOther ) { Copy( inOther); }
	;		CAlerting( BOOL inEnabled, CPeekString inSource, CPeekString inFacilitiy, CPeekString inOutputDir )
				: m_bEnabled( inEnabled ), m_strSource( inSource ), m_strFacility( inFacilitiy ), m_strOutputDirectory( inOutputDir ) 
			{}
	;		~CAlerting() {}

	void	operator=( const CAlerting& inOther ) {
		Copy( inOther );
	}

	void	Copy( const CAlerting& inOther ) {
		m_bEnabled = inOther.m_bEnabled;
		m_strSource = inOther.m_strSource;
		m_strFacility = inOther.m_strFacility;
		m_strOutputDirectory = inOther.m_strOutputDirectory;
	}

	CPeekString	GetFacility() const { return m_strFacility; }
	CPeekString	GetOutputDirectory() const { return m_strOutputDirectory; }
	CPeekString	GetSource() const { return m_strSource; }

	bool	IsAlerting() const { return (m_bEnabled != FALSE); }

	bool	Model( CPeekDataElement& inParentElement );

	void	SetAlerting( bool inEnable ) { m_bEnabled = (inEnable) ? TRUE : FALSE; }
	void	SetFacility( CPeekString inFacility ) { m_strFacility = inFacility; }
	void	SetOutputDirectory( CPeekString inFolder ) { m_strOutputDirectory = inFolder; }
	void	SetSource( CPeekString inSource ) { m_strSource = inSource; }
};


// -----------------------------------------------------------------------------
//		CLogging
// -----------------------------------------------------------------------------

class CLogging
{
	friend class CAlertingPage;

protected:
	BOOL	m_bObscureNames;

public:
	;		CLogging() : m_bObscureNames( FALSE ) {}
	;		CLogging( BOOL inObscureNames ) { m_bObscureNames = inObscureNames; }
	;		CLogging( const CLogging& inOther ) { Copy( inOther ); }
	;		~CLogging() {}

	void	operator=( const CLogging& inOther ) {
		Copy( inOther );
	}

	void	Copy( const CLogging& inOther ) { m_bObscureNames = inOther.m_bObscureNames; }

	bool	IsObscureNames() const { return (m_bObscureNames != FALSE); }

	bool	Model( CPeekDataElement& inParentElement );

	void	SetObscureNames( bool inEnable ) { m_bObscureNames = (inEnable) ? TRUE : FALSE; }
};


// =============================================================================
//		COptions
// =============================================================================

class COptions
{
	friend class COptionsDialog;

protected:
	CPeekString				m_strConfigFileName;
	CPeekString				m_strChangeLogFileName;
	CTargetNameList			m_TargetNames;
	CPortNumberList			m_PortNumbers;
	CAlerting				m_Alerting;
	CLogging				m_Logging;

	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther );

public:
	;			COptions() {}
	;			COptions( const COptions& inOther ) { Copy( inOther ); }
	virtual		~COptions() {}

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}

	bool		operator==( const COptions& inOther ) { return IsEqual( inOther ); }
	bool		operator!=( const COptions& inOther ) { return !IsEqual( inOther ); }

	const CTargetNameList&	GetTargetNames() const { return m_TargetNames; }
	CTargetNameList&		GetTargetNames() { return m_TargetNames; }
	const CPortNumberList&	GetPortList() const { return m_PortNumbers; }
	CPortNumberList&		GetPortList() { return m_PortNumbers; }
	const CAlerting&		GetAlerting() const { return m_Alerting; }
	CAlerting&				GetAlerting() { return m_Alerting; }
	const CLogging&			GetLogging() const { return m_Logging; }
	CLogging&				GetLogging() { return m_Logging; }

	bool	IsAlerting() const { return m_Alerting.IsAlerting(); }
	bool	IsNameOfInterest( CPeekString inName ) const { return m_TargetNames.IsMatch( inName ); }
	bool	IsObscuringNames() const { return m_Logging.IsObscureNames(); }
	bool	IsFtpPortOfInterest( UInt16 inPort ) { return m_PortNumbers.IsMatch( inPort ); }
	bool	IsFtpPortOfInterest( const CIpAddressPortPair& inIpPortPair ) const {
		if ( m_PortNumbers.IsMatch( inIpPortPair.GetSrcPort() ) ) return true;
		if ( m_PortNumbers.IsMatch( inIpPortPair.GetDstPort() ) ) return true;
		return false;
	}

	void	SetOptions( const COptions& in ) {
		*this = in;
	}

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );
};
