// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"
#include "FileEx.h"
#include "resource.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;
class COptions;

extern const CPeekString	kDefaultRootDir;
extern const CPeekString	kDirectoryFileName;
extern const CPeekString	kLoggingFileName;

#define kDefault_PortNumber			21

// -----------------------------------------------------------------------------
//		CTargetName
// -----------------------------------------------------------------------------

class CTargetName
{
protected:
	bool		m_bEnabled;
	CPeekString	m_strName;

public:
	;		CTargetName() { Reset(); }
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

	void	Reset() { m_bEnabled = false; }

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
	;		CTargetNameList() { Reset(); }
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

	void	Reset() { m_bCaptureAll = true; RemoveAll(); }

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
	;		CPortNumber() { Reset(); }
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

	void	Reset() { m_bEnabled = false; }

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
	;		CPortNumberList() { Reset(); }
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

	void	Reset() { 
		RemoveAll();
		Add( CPortNumber( true, kDefault_PortNumber ) );
	}

};


// -----------------------------------------------------------------------------
//		CAlerting
// -----------------------------------------------------------------------------

class CAlerting
{
	friend class CAlertingPage;

protected:
	bool		m_bEnabled;
	CPeekString	m_strSource;
	CPeekString	m_strFacility;
	CPeekString	m_strOutputDirectory;

public:
	;		CAlerting() { Reset(); }
	;		CAlerting( const CAlerting& inOther ) { Copy( inOther); }
	;		CAlerting( bool inEnabled, CPeekString inSource, CPeekString inFacilitiy, CPeekString inOutputDir )
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

	bool	IsAlerting() const { return m_bEnabled; }

	bool	Model( CPeekDataElement& inParentElement );

	void	Reset() { 
		m_bEnabled = false; 
		m_strSource = L"";
		m_strFacility = L"";
		m_strOutputDirectory = kDefaultRootDir; 
	}

	void	SetAlerting( bool inEnable ) { m_bEnabled = inEnable; }
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
	BOOL	m_bLoggingToScreen;
	BOOL	m_bObscureNames;
#ifdef IMPLEMENT_PASSWORD_LOGGING
	BOOL	m_bShowPassword;
#endif // IMPLEMENT_PASSWORD_LOGGING

public:

#ifdef IMPLEMENT_PASSWORD_LOGGING
	;		CLogging() : m_bLoggingToScreen( TRUE ), m_bObscureNames( FALSE ), m_bShowPassword( FALSE ) {}
	;		CLogging( BOOL inObscureNames, BOOL inShowPassword ) 
		: m_bObscureNames( inObscureNames ), m_bShowPassword( inShowPassword ) {}
#else // IMPLEMENT_PASSWORD_LOGGING
	;		CLogging() { Reset(); }
	;		CLogging( BOOL inLogging, BOOL inObscureNames ) 
		: m_bLoggingToScreen( inLogging ), m_bObscureNames( inObscureNames ) {}
#endif // IMPLEMENT_PASSWORD_LOGGING

	;		CLogging( const CLogging& inOther ) { Copy( inOther ); }
	;		~CLogging() {}

	void	operator=( const CLogging& inOther ) {
		Copy( inOther );
	}

	void	Copy( const CLogging& inOther ) {
		m_bLoggingToScreen = inOther.m_bLoggingToScreen;
		m_bObscureNames = inOther.m_bObscureNames; 
#ifdef IMPLEMENT_PASSWORD_LOGGING
		m_bShowPassword = inOther.m_bShowPassword;
#endif // IMPLEMENT_PASSWORD_LOGGING
	}

	bool	IsLoggingToScreen() const { return (m_bLoggingToScreen != FALSE); }
	//	bool	IsObscureNames() const { return ( (IsLogging() == true) && (m_bObscureNames != FALSE) ); }
	bool	IsObscureNames() const { return ( m_bObscureNames != FALSE ); }
#ifdef IMPLEMENT_PASSWORD_LOGGING
	bool	IsShowPassword() const { return ( (m_bObscureNames == FALSE) && (m_bShowPassword != FALSE) ); }
#endif // IMPLEMENT_PASSWORD_LOGGING

	bool	Model( CPeekDataElement& inParentElement );

	void	Reset() { m_bLoggingToScreen = TRUE; m_bObscureNames = FALSE; }

	void	SetLoggingToScreen( bool inEnable ) { m_bLoggingToScreen = (inEnable) ? TRUE : FALSE; }
	void	SetObscureNames( bool inEnable ) { m_bObscureNames = (inEnable) ? TRUE : FALSE; }
#ifdef IMPLEMENT_PASSWORD_LOGGING
	void	SetShowPassword( bool inShowPassword ) { m_bShowPassword = inShowPassword; }
#endif // IMPLEMENT_PASSWORD_LOGGING
};


#ifdef IMPLEMENT_DATA_CAPTURE
// -----------------------------------------------------------------------------
//		CDataTransfer
// -----------------------------------------------------------------------------

class CDataTransfer
{

protected:
	bool		m_bEnableDataTransfer;
	bool		m_bEnableFileCapture;
	CPeekString	m_strFileCaptureDir;
	bool		m_bEnableListResultsToFile;
	CPeekString	m_strListResultsToFileDir;
	bool		m_bEnableLogFtpCmds;
	CPeekString	m_strLogFtpCmdsDir;

	bool	ValidateFilePath( CPeekString& inFilePath, bool bNormalize = true );

public:
	;		  CDataTransfer() { Reset(); }
	;		  CDataTransfer( const CDataTransfer& inOther ) { Copy( inOther ); }
	;virtual ~CDataTransfer() {}

	void	operator=( const CDataTransfer& inOther ) {
		Copy( inOther );
	}

	void	Copy( const CDataTransfer& inOther );

	CPeekString		GetFileCaptureDir() {
		ASSERT( ValidateFilePath( m_strFileCaptureDir, false ) );	// Should always be valid per code logic
		return m_strFileCaptureDir;
	}
	CPeekString		GetListResultsToFileDir() {
		ASSERT( ValidateFilePath( m_strListResultsToFileDir, false ) );	// Should always be valid per code logic
		return m_strListResultsToFileDir;
	}
	CPeekString		GetLogFtpCmdsDir() {
		ASSERT( ValidateFilePath( m_strLogFtpCmdsDir, false ) );	// Should always be valid per code logic
		return m_strLogFtpCmdsDir;
	}

	bool	Model( CPeekDataElement& inParentElement );

	bool	IsDataTransferEnabled() const { return m_bEnableDataTransfer; }
	bool	IsFileCaptureEnabled() const { return m_bEnableFileCapture; }
	bool	IsListResultsToFileEnabled() const { return m_bEnableListResultsToFile; }
	bool	IsLogFtpCmdsEnabled() const { return m_bEnableLogFtpCmds; }

	void	Reset();

	void	SetFileCaptureDir( CPeekString inFilePath ) {
		inFilePath.Trim();
		if ( !ValidateFilePath( inFilePath ) )	{
			if ( !ValidateFilePath( m_strFileCaptureDir ) ) m_strFileCaptureDir = kDefaultRootDir;
			return;
		}
		if ( inFilePath.Right(1) == L"\\" ) {
			inFilePath = inFilePath.Left( inFilePath.GetLength() - 1 );
		}
		m_strFileCaptureDir = inFilePath;
	}
	void	SetListResultsToFileDir( CPeekString inFilePath ) {
		inFilePath.Trim();
		if ( !ValidateFilePath( inFilePath ) )	{
			if ( !ValidateFilePath( m_strListResultsToFileDir ) ) m_strListResultsToFileDir = kDefaultRootDir;
			return;
		}
		if ( inFilePath.Right(1) == L"\\" ) {
			inFilePath = inFilePath.Left( inFilePath.GetLength() - 1 );
		}
		m_strListResultsToFileDir = inFilePath;
	}
	void	SetLogFtpCmdsDir( CPeekString inFilePath ) {
		inFilePath.Trim();
		if ( !ValidateFilePath( inFilePath ) )	{
			if ( !ValidateFilePath( m_strLogFtpCmdsDir ) ) m_strLogFtpCmdsDir = kDefaultRootDir;
			return;
		}
		if ( inFilePath.Right(1) == L"\\" ) {
			inFilePath = inFilePath.Left( inFilePath.GetLength() - 1 );
		}
		m_strLogFtpCmdsDir = inFilePath;
	}

	void	SetDataTransferEnabled( bool inEnable ) { m_bEnableDataTransfer = inEnable; }
	void	SetFileCaptureEnabled( bool inEnable ) { m_bEnableFileCapture = inEnable; }
	void	SetListResultsToFileEnabled( bool inEnable ) { m_bEnableListResultsToFile = inEnable; }
	void	SetLogFtpCmdsEnabled( bool inEnable ) { m_bEnableLogFtpCmds = inEnable; }
};
#endif // IMPLEMENT_DATA_CAPTURE


// =============================================================================
//		COptions
// =============================================================================

class COptions
{
	friend class COptionsDialog;

protected:
	static CPeekString		s_strTagForensics;

	CPeekString				m_strConfigFileName;
	CPeekString				m_strChangeLogFileName;
	CTargetNameList			m_TargetNames;
	CPortNumberList			m_PortNumbers;
	CAlerting				m_Alerting;
	CLogging				m_Logging;
#ifdef IMPLEMENT_DATA_CAPTURE
	CDataTransfer			m_DataTransfer;
#endif IMPLEMENT_DATA_CAPTURE

	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther );

public:
	static CPeekString&	GetTagForensics() { return s_strTagForensics; }

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
#ifdef IMPLEMENT_DATA_CAPTURE
	const CDataTransfer&	GetDataTransfer() const { return m_DataTransfer; }
	CDataTransfer&			GetDataTransfer() { return m_DataTransfer; }
	bool	IsDataTransferEnabled() const { 
		return ( !IsObscureNames() && m_DataTransfer.IsDataTransferEnabled() ); 
	}
	bool	IsCaptureToFileEnabled() const { 
		return ( IsDataTransferEnabled() && m_DataTransfer.IsFileCaptureEnabled() ); 
	}
	bool	IsListResultsToFileEnabled() const { 
		return ( IsDataTransferEnabled() && m_DataTransfer.IsListResultsToFileEnabled() ); 
	}
	bool	IsLogFtpCmds() const { 
		return ( IsDataTransferEnabled() && m_DataTransfer.IsLogFtpCmdsEnabled() ); 
	}
#endif // IMPLEMENT_DATA_CAPTURE

	bool	IsAlerting() const { return m_Alerting.IsAlerting(); }
	bool	IsLoggingToScreen() const { return m_Logging.IsLoggingToScreen(); }

#ifdef IMPLEMENT_PASSWORD_LOGGING
	bool	IsIncludePassword() { return m_Logging.IsShowPassword(); }
#endif // IMPLEMENT_PASSWORD_LOGGING
	bool	IsNameOfInterest( CPeekString inName ) const { return m_TargetNames.IsMatch( inName ); }
	bool	IsObscureNames() const { return m_Logging.IsObscureNames(); }
	bool	IsFtpPortOfInterest( UInt16 inPort ) { return m_PortNumbers.IsMatch( inPort ); }
	bool	IsFtpPortOfInterest( const CIpAddressPortPair& inIpPortPair ) const {
		if ( m_PortNumbers.IsMatch( inIpPortPair.GetSrcPort() ) ) return true;
		if ( m_PortNumbers.IsMatch( inIpPortPair.GetDstPort() ) ) return true;
		return false;
	}

	void	Reset();

	void	SetOptions( const COptions& in ) {
		*this = in;
	}

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );
};
