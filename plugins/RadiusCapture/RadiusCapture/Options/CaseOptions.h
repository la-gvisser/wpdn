// ============================================================================
//	CaseOptions.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "UserName.h"
#include "PortOptions.h"
#include "CaptureOptions.h"
#include "PeekDataModeler.h"

class	CPeekPlugin;
class COptions;

#define kPref_Cases					_T( "Cases" )
#define kPref_Case					_T( "Case" )
#define kPref_Prefix				_T( "Prefix" )
#define kPref_Name					_T( "Name" )
#define kPref_CaptureId				_T( "CaptureId" )
#define kPref_StartTime				_T( "StartTime" )
#define kPref_StopTime				_T( "StopTime" )

#define kTriggerTimeStatusActive		0
#define kTriggerTimeStatusNotYetStarted 1
#define kTriggerTimeStatusExpired		2

#define kTab_General					0
#define kTab_UserNames					1
#define kTab_TriggerTimes				2
#define kTab_Ports						3
#define kTab_Capture					4

#ifdef CAPTURE_ALL
  #define kPref_CaptureAll			_T( "CaptureAll" )
#endif

#ifdef MODE_OPTIONS
  #define kPref_CaptureMode			_T( "CaptureMode" )
  #define kPref_PenEml				_T( "PenEml" )
  #define kPref_PenText				_T( "PenText" )
#endif

#ifdef INTEGRITY_OPTIONS
#define kIntegrity_Disabled				0
#define kIntegrity_Hash					1
#define kIntegrity_Sign					2
#define kIntegrity_Algorithm_MD5		1
#define kIntegrity_Algorithm_SHA		2
#define kIntegrity_Algorithm_RSA		3
#define kIntegrity_Undefined			-1
// Registry Keys
#define kCompanyName				_T("WildPackets")
#define kKeyName_Capture			_T("Capture")
#define kPref_EnableIntegrity		_T( "EnableIntegrity" )
#define kKeyValue_Integrity			_T("Integrity")				// OPT_INTEGRITY
#define kKeyValue_HashAlgorithm		_T("HashAlgorithm")			// OPT_INTEGRITY
#define	kKeyValue_KeyContainer		_T("KeyContainer")			// OPT_INTEGRITY
#endif // INTEGRITY_OPTIONS


// =============================================================================
//		CCaseOptions
// =============================================================================

class CCaseOptions
{
	friend class CCaseOptionsDlg;
	friend class CGeneralOptionsDlg;

public:

	CCaseOptions();
	~CCaseOptions() {}

	enum tCaseNotEnabled {
		kCaseReason_OK = 0,
		kCaseReason_TimeExpired,
		kCaseReason_NoPorts,
		kCaseReason_NoUsers,
		kCaseReason_GeneralOpts,	
		kCaseReason_CaptureOpts
	};

	bool operator==( CCaseOptions& in ) {
		if ( m_bEnabled != in.m_bEnabled ) return false;
		if ( m_bDuplicate != in.m_bDuplicate ) return false;
		if ( m_bHighlighted != in.m_bHighlighted ) return false;
		if ( m_GeneralOptions != in.m_GeneralOptions ) return false;
		if ( m_UserNameOptions != in.m_UserNameOptions ) return false;
		if ( m_TriggerOptions != in.m_TriggerOptions ) return false;
		if ( m_CasePortOptions != in.m_CasePortOptions ) return false;
		if ( m_CaptureOptions != in.m_CaptureOptions ) return false;
		return true;
	}
	bool operator!=( CCaseOptions& in ) {
		return !operator==(in);
	}
	CCaseOptions& operator=( const CCaseOptions& in ){
		Copy( in );
		return *this;
	}

	void Copy( const CCaseOptions& in );

	bool Model( CPeekDataElement& ioElement );

	//
	// General Options
	//

	class CGeneralOptions {
		friend class CCaseOptions;

	  public:
		CGeneralOptions( CCaseOptions& inContainer )
			:	m_Container( inContainer )
			,	m_bUseFileNumber( false )
			,	m_nCaseCaptureId( 0 )
		{}

		CGeneralOptions& operator=( const CGeneralOptions& in ) {
			m_strCaseName 			= in.m_strCaseName;
			m_strPrefix 			= in.m_strPrefix;
			m_bUseFileNumber		= in.m_bUseFileNumber;
			m_nCaseCaptureId		= in.m_nCaseCaptureId;
			m_strOutputDirectory	= in.m_strOutputDirectory;
			return *this;
		}
		bool operator==( CGeneralOptions& in ) {
			if ( m_strCaseName.CompareNoCase( in.m_strCaseName ) ) return false;
			if ( m_strPrefix.CompareNoCase( in.m_strPrefix ) ) return false;
			if ( m_bUseFileNumber != in.m_bUseFileNumber ) return false;
			if ( m_nCaseCaptureId != in.m_nCaseCaptureId ) return false;
			if ( m_strOutputDirectory.CompareNoCase( in.m_strOutputDirectory ) ) return false;
			return true;
		}
		bool operator!=( CGeneralOptions& in ) {
			return !operator==(in);
		}
		const CPeekString& GetName() {
			return m_strCaseName;
		}

		bool Model( CPeekDataElement& ioElement );

		void SetName( CPeekString& strName ) {
			m_strCaseName = strName;
		}
		const CPeekString& GetPrefix() {
			return m_strPrefix;
		}
		void SetPrefix( CPeekString& strPrefix ) {
			m_strPrefix = strPrefix;
		}
		const CPeekString& GetOutputDirectory() {
				return m_strOutputDirectory;
		}
		void SetOutputDirectory( CPeekString& strOutputDirectory ) {
			m_strOutputDirectory = strOutputDirectory;
		}
		void	SetUseFileNumber( bool bUseFileNumber ) {
			m_bUseFileNumber = bUseFileNumber;
		}
		bool	IsUseFileNumber() {
			return m_bUseFileNumber;
		}
		bool AreOptionsValid( bool bValidateOutputDir ) {
			if ( m_strCaseName.GetLength() == 0 ) return false;
			if ( bValidateOutputDir && m_strOutputDirectory.GetLength() == 0 ) return false;
			return true;
		}
		CPeekString	SuggestOutputDirectory( CPeekString strOutputRootDir, CPeekString strCasePrefix );

	protected:
		CPeekString		m_strCaseName;
		CPeekString		m_strPrefix;
		bool			m_bUseFileNumber;
		UInt32			m_nCaseCaptureId;
		CPeekString		m_strOutputDirectory;
		CCaseOptions&	m_Container;
	};

	//
	// Trigger Options
	//
	class CTriggerOptions {
		friend class CCaseOptions;

	public:
		CTriggerOptions()
			: m_StartTime( 0 )
			, m_StopTime( 0 )
		  {}

		CTriggerOptions& operator=( const CTriggerOptions& in ) {
			Copy( in );
			return *this;
		}
		void Copy( const CTriggerOptions& in ) {
			m_StartTime = in.m_StartTime;
			m_StopTime = in.m_StopTime;
		}
		bool operator==( CTriggerOptions& in ) {
			if ( m_StartTime != in.m_StartTime ) return false;
			if ( m_StopTime != in.m_StopTime ) return false;
			return true;
		}
		bool operator!=( CTriggerOptions& in ) {
			return !operator==(in);
		}
		UInt64	GetStartTime() const {
			return m_StartTime;
		}
		void	SetStartTime( UInt64 inStartTime ) {
			m_StartTime = inStartTime;
		}
		UInt64	GetStopTime() const {
			return m_StopTime;
		}

		bool	Model ( CPeekDataElement& ioElement );

		void	SetStopTime( UInt64 inStopTime ) {
			m_StopTime = inStopTime;
		}

	protected:
		UInt64		m_StartTime;
		UInt64		m_StopTime;
	};

	//
	// User Name Options
	//
	class CUserNameOpts {
		friend class CCaseOptions;
		friend class CUserNameOptionsDlg;

	 public:
		CUserNameOpts() {}
	   ~CUserNameOpts() {}
		 
		CUserNameOpts& operator=( const CUserNameOpts& in ) {
			return Copy( in );
		}
		bool operator==( CUserNameOpts& in ) {
			return ( m_UserList == in.m_UserList );
		}
		bool operator!=( CUserNameOpts& in ) {
			return !operator==(in);
		}
		CUserNameOpts& Copy( const CUserNameOpts& in ) { 
			m_UserList.Reset();
			m_UserList = in.m_UserList;
			return *this;
		}
		CUserNameList& GetUserNameList() {
			return m_UserList;
		}

		bool	Model ( CPeekDataElement& ioElement );

	protected:
		CUserNameList m_UserList;
	};

	//
	// CCaseOptions Member Functions
	//

	static void			ImportCaseItems( CCaseOptions* pEditCaseOptions, const CCaseOptions* inImportCaseOptions, 
											CCaptureOptions& inCaptureOptions, bool bIsCapturing );

  #ifdef INTEGRITY_OPTIONS
	void			Init( CPeekString inName, bool bIsUseFileNumber, CPeekPlugin* inPlugin );
  #else
	void			Init( CPeekString inName, bool bIsUseFileNumber );
  #endif
	CPeekString			GetFormattedStartStopTime( bool bStart );
	bool			CanCaseBeEnabled( int& nReason );
	CPeekString			GetName() {
		return m_GeneralOptions.m_strCaseName;
	}
	void				SetName( CPeekString& strName ) {
		m_GeneralOptions.m_strCaseName = strName;
	}
	CPeekString			GetPrefix() {
		return m_GeneralOptions.m_strPrefix;
	}
	void				SetPrefix( CPeekString& strPrefix ) {
		m_GeneralOptions.m_strPrefix = strPrefix;
	}
	void	SetUseFileNumber( bool bUseFileNumber ) {
		m_GeneralOptions.SetUseFileNumber( bUseFileNumber );
	}
	bool	IsUseFileNumber() {
		return m_GeneralOptions.IsUseFileNumber();
	}
	UInt32				GetCaseCaptureId() {
		return m_GeneralOptions.m_nCaseCaptureId;
	}
	CPeekString			GetCaseCaptureIdString() {
		CPeekOutString strCaseCaptureId;
		strCaseCaptureId << m_GeneralOptions.m_nCaseCaptureId;
		return strCaseCaptureId;
	}
	void				IncrementCaseCaptureId() {
		if ( m_GeneralOptions.m_nCaseCaptureId >= 0xFFFFFFFE ) m_GeneralOptions.m_nCaseCaptureId = 0;
		m_GeneralOptions.m_nCaseCaptureId++;
	}
	void				SetCaseCaptureId( UInt32 nCaseCaptureId ) {
		m_GeneralOptions.m_nCaseCaptureId = nCaseCaptureId; 
	}
	void				SetCaseCaptureId( CPeekString strCaseCaptureId ) {
		m_GeneralOptions.m_nCaseCaptureId = _ttol( strCaseCaptureId ); 
	}

	CPeekString			GetOutputDirectory() {
		return m_GeneralOptions.GetOutputDirectory();
	}
	void				SetOutputDirectory( CPeekString strOutputDirectory ) {
		m_GeneralOptions.SetOutputDirectory( strOutputDirectory );
	}
	CUserNameList&		GetUserList() {
		return m_UserNameOptions.m_UserList;
	}
	CPortOptions& GetCasePortOptions() {
		return m_CasePortOptions;
	}
	CPortNumberList&	GetPortNumberList() {
		return m_CasePortOptions.GetPortNumberList();
	}
	void SyncPortsToGlobal( CPortNumberList& inPortsList ) {
		if ( m_CasePortOptions.IsUsingGlobalDefaults() ) {
			m_CasePortOptions.SyncGlobalDefaults( inPortsList );
		}
	}
	void SyncCaptureOptionsToGlobal( CCaptureOptions& inCaptureOptions ) {
		if ( m_CaptureOptions.IsUseGlobalDefaults() ) {
			m_CaptureOptions.SyncGlobalDefaults( inCaptureOptions );
		}
	}
	void SyncGeneralOptionsToGlobal( CPeekString strOutputRootDir, bool bUseFileNumber ){
		if ( m_GeneralOptions.GetOutputDirectory().GetLength() == 0 ) m_GeneralOptions.SetOutputDirectory( strOutputRootDir );
		SetUseFileNumber( bUseFileNumber );
	}
	bool	IsLogMsgsToFile() const {
		return m_CaptureOptions.IsLogMsgsToFile();
	}
	bool	IsLogMsgsToFileButton() const {
		return m_CaptureOptions.IsLogMsgsToFileButton();
	}
	bool	IsFoldersForCases() const {
		return m_CaptureOptions.IsFoldersForCases();
	}
	bool	IsFoldersForCasesButton() const {
		return m_CaptureOptions.IsFoldersForCasesButton();
	}
	void	SetLogMsgsToFileButton( bool bLogMsgsToFileButton ) {
		m_CaptureOptions.SetLogMsgsToFileButton( bLogMsgsToFileButton );
	}
	void	SetFoldersForCasesButton( bool bFoldersForCases ) {
		m_CaptureOptions.SetFoldersForCasesButton( bFoldersForCases );
	}
	bool	IsCaptureToFile() {
		return m_CaptureOptions.IsCaptureToFile();
	}
	void	SetCaptureToFile( bool bCaptureToFile ) {
		m_CaptureOptions.SetCaptureToFile( bCaptureToFile );
	}
	bool	IsCaptureToScreen() {
		return m_CaptureOptions.IsCaptureToScreen();
	}
	void	SetCaptureToScreen( bool bCaptureToScreen ) {
		m_CaptureOptions.SetCaptureToScreen( bCaptureToScreen );
	}
	bool	IsLogMsgsToScreen() const {
		return m_CaptureOptions.IsLogMsgsToScreen();
	}
	void	SetLogMsgsToScreen( bool bLogMsgsToScreen ) {
		m_CaptureOptions.SetLogMsgsToScreen( bLogMsgsToScreen );
	}
	UInt32	GetSaveAgeSeconds() const {
		return m_CaptureOptions.GetSaveAgeSeconds();
	}
	UInt64	GetSaveSizeOrZero() const {
		return m_CaptureOptions.GetSaveSizeOrZero();
	}
	void	GetSaveSizeBytes( UInt64& outCount, CPeekString& outUnits ) const {
		return m_CaptureOptions.GetSaveSizeBytes( outCount, outUnits );
	}
	UInt64	GetSaveSizeBytes() const {
		return m_CaptureOptions.GetSaveSizeBytes();
	}
	void	SetSaveAgeSelected( bool inEnable ) {
		m_CaptureOptions.SetSaveAgeSelected( inEnable );
	}
	void	SetSaveAgeValue( UInt32 inValue ) {
		m_CaptureOptions.SetSaveAgeValue( inValue );
	}
	void	SetSaveAgeValue( UInt32 inCount, CPeekString inUnits ) {
		return m_CaptureOptions.SetSaveAgeValue( inCount, inUnits );
	}
	void	SetSaveSizeSelected( bool inEnable ) {
		m_CaptureOptions.SetSaveSizeSelected( inEnable );
	}
	void	SetSaveSizeValue( UInt64 inValue ) {
		m_CaptureOptions.SetSaveSizeValue( inValue );
	}
	UInt32	GetSaveAgeOrZero() {
		return m_CaptureOptions.GetSaveAgeOrZero();
	}
	UInt64	GetSaveSizeOrZero() {
		return m_CaptureOptions.GetSaveSizeOrZero();
	}
	bool	IsUseCaptureGlobalDefaults() {
		return m_CaptureOptions.IsUseGlobalDefaults();
	}
	void	SetUseCaptureGlobalDefaults( bool bUseGlobalDefaults ) {
		m_CaptureOptions.SetUseGlobalDefaults( bUseGlobalDefaults );
	}
	CPeekString GetFormattedActivePortsString() {
		return m_CasePortOptions.GetFormattedActivePortsString();
	}
	CCaptureOptions& GetCaptureOptions() {
		return m_CaptureOptions;
	}
	bool	HasAssociatedUserName( CPeekString inUserName ) {
      #ifdef CAPTURE_ALL
		if ( m_UserNameOptions.m_UserList.CaptureAll() ) return true;
      #endif
		return m_UserNameOptions.m_UserList.HasAssociatedName( inUserName );
	}
	bool	UserIsActiveTarget( CPeekString inUserName ) {
		if ( !IsActive() ) return false;
		return m_UserNameOptions.m_UserList.UserIsActiveTarget( inUserName );
	}
	bool IsActivePort( const UInt16 uPort ) {
		if ( !IsActive() ) return false;
		return m_CasePortOptions.GetPortNumberList().PortIsActive( uPort );
	}
	CPeekString GetTargetListString() {
		return m_UserNameOptions.m_UserList.GetTargetListString();
	}
	bool AreEnabledUsers() { 
		return m_UserNameOptions.m_UserList.AreEnabledUsers();
	}
	bool AreEnabledPorts() {
		return m_CasePortOptions.AreEnabledPorts();
	}
	bool AreGeneralOptionsValid() {
		bool bNeedOutputDir = m_CaptureOptions.IsCaptureToFile();
		return m_GeneralOptions.AreOptionsValid( bNeedOutputDir );
	}
	bool AreCaptureOptionsValid() {
		return m_CaptureOptions.AreOptionsValid();
	}
	bool HasStopTimeExpired() {
		UInt64 theStopTime = GetStopTime();
		UInt64 theTime = GetTimeStamp();
		if ( theTime > theStopTime  ) {
			return true;
		}
		return false;
	}
	UInt8 GetTriggerTimeStatus() {
		UInt64 theTime = GetTimeStamp();
		if ( theTime < GetStartTime() ) {
			return kTriggerTimeStatusNotYetStarted;
		}
		if ( theTime > GetStopTime() ) {
			return kTriggerTimeStatusExpired;
		}			
		return kTriggerTimeStatusActive;
	}
	bool	CanCaseBeEnabled() {
		int nReason;
		return CanCaseBeEnabled( nReason );
	}
	UInt64	GetStartTime() const {
		return m_TriggerOptions.m_StartTime;
	}
	void	SetStartTime( UInt64 inStartTime ) {
		 m_TriggerOptions.m_StartTime = inStartTime;
	}
	UInt64	GetStopTime() const {
		return m_TriggerOptions.m_StopTime;
	}
	bool	CaptureAll() const {
		return m_UserNameOptions.m_UserList.CaptureAll();
	}
	bool	ContainsUserName( const CPeekString& inUserName ) {
		return m_UserNameOptions.m_UserList.Contains( inUserName );
	}
	void	SetEnabled() {
		m_bEnabled = true;
	}
	void	SetDisabled() {
		m_bEnabled = false;
	}
	void	SetEnabledState( bool bEnabled ) {
		m_bEnabled = bEnabled;
	}
	bool	IsEnabled() {
		return m_bEnabled;
	}
	void SetHighlighted( bool bHighlighted ) {
		m_bHighlighted = bHighlighted;
	}
	bool IsHighlighted() {
		return m_bHighlighted;
	}
	bool	IsActive() {
		if ( IsEnabled() ) {
			ASSERT( AreEnabledUsers() );
			return !HasStopTimeExpired();
		}
		return false;
	}
	void SetDuplicate( bool bDuplicate ) {
		m_bDuplicate = bDuplicate;
	}
	bool GetDuplicate() {
		return m_bDuplicate;
	}

	protected:
		CGeneralOptions		m_GeneralOptions;
		CUserNameOpts		m_UserNameOptions;
		CTriggerOptions		m_TriggerOptions;
		CPortOptions		m_CasePortOptions;
		CCaptureOptions		m_CaptureOptions;
		bool				m_bEnabled;
		bool				m_bHighlighted;
		bool				m_bDuplicate;	
		UInt32				m_nLastTab;


  #ifdef MODE_OPTIONS
	bool	IsFullCapture() const { return (m_Mode.m_nCaptureMode != 0); }
	bool	FullEml() const { return (m_Mode.m_bFullEml != FALSE); }
	bool	FullPkt() const { return (m_Mode.m_bFullPkt != FALSE); }
	bool	PenEml() const { return (m_Mode.m_bPenEml != FALSE); }
	bool	PenText() const { return (m_Mode.m_bPenText != FALSE); }

	// Mode Options
	class CModeOptions {
	public:
		;	CModeOptions()
			:	m_nCaptureMode( 0 )
			,	m_bPenEml( TRUE )
			,	m_bPenText( TRUE )
			,	m_bFullEml( TRUE )
			,	m_bFullPkt( TRUE )
		{}

		int				m_nCaptureMode;
		BOOL			m_bPenEml;
		BOOL			m_bPenText;
		BOOL			m_bFullEml;
		BOOL			m_bFullPkt;
		CModeOptions	m_Mode;
	};
  #endif

  #ifdef INTEGRITY_OPTIONS
	bool	GetIntegrityOptions( tIntegrityOptions& inIntegrityOptions );
	bool	SetIntegrityOptions( tIntegrityOptions& inIntegrityOptions );

	// Integrity Options
	class CIntegrityOptions {
	public:
		CIntegrityOptions() :	
		  m_nEnabled( FALSE ),
			  m_nHashAlgorithm( 1 )
		  {}

		  void	Init( CPeekPlugin *inPlugin )
		  {
			  ASSERT( inPlugin );

			  CPeekString	strPrefsPath;
			  inPlugin->DoPrefsGetPrefsPath( strPrefsPath.GetBuffer( MAX_PATH ) );
			  strPrefsPath.ReleaseBuffer();

			  // an extra for the '\'.
			  int nLen = strPrefsPath.GetLength() - strPrefsPath.Find( _T('\\') ) - 1;
			  m_strDefaultKeyName = strPrefsPath.Right( nLen );
			  m_strDefaultKeyName.Replace( _T('\\'), _T('-') );
		  }

		  void	GetOptions( tIntegrityOptions& out )
		  {
			  out.nEnabled = m_nEnabled;
			  out.nHashAlgorithm = m_nHashAlgorithm;
			  out.strKeyContainer = m_strKeyContainer;
		  }

		  void	SetOptions( tIntegrityOptions& in, CPeekString inStr )
		  {
			  m_nEnabled = in.nEnabled;
			  m_nHashAlgorithm = in.nHashAlgorithm;
			  m_strKeyContainer = in.strKeyContainer;
			  m_strDefaultKeyName = inStr;
		  }

		  int				m_nEnabled;
		  int				m_nHashAlgorithm;
		  CPeekString		m_strKeyContainer;
		  CPeekString		m_strDefaultKeyName;
		  CIntegrityOptions	m_Integrity;
		  CPeekPlugin*		m_pPlugin;
	};
  #endif

};
