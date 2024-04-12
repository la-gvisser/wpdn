// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once


#include "CaseOptionsList.h"
#include "ActiveLists.h"
#include "PortNumber.h"
#include "PortOptions.h"
#include "CaptureOptions.h"

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;

#define kPref_Options				_T( "Options" )
#define kPref_RadiusCapture			_T( "RadiusCaptureOE" )
#define kPref_General				_T( "General" )
#define kPref_DisplayCapture		_T( "DisplayCapture" )
#define kPref_DisplayWarning		_T( "DisplayWarning" )
#define kPref_ProcessPackets		_T( "ProcessPackets" )
#define kPref_OutputDirectory		_T( "OutputDirectory" )
#define kPref_UseFileNumber			_T( "UseCaseFileNumber" )
#define kPref_Enabled				_T( "Enabled" )

// =============================================================================
//		COptions
// =============================================================================

class COptions
{
	friend class COptionsDialog;

protected:
	static CPeekString		s_strTagForensics;

protected:

	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther );

public:
	static CPeekString&	GetTagForensics() { return s_strTagForensics; }

public:
	;			COptions() { Reset(); }
	;			COptions( const COptions& inOther ) { Copy( inOther ); }
	virtual		~COptions() { ResetCaseOptions(); }

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}

	COptions*	GetLockedClientPtr() { return this; }

	bool		operator==( const COptions& inOther ) { return IsEqual( inOther ); }
	bool		operator!=( const COptions& inOther ) { return !IsEqual( inOther ); }

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		Reset();

	//
	// CGeneralOptions
	//
	class CGeneralOptions {

	public:
		CGeneralOptions() {
			Reset();
		}
		~CGeneralOptions() {}

		CGeneralOptions&	operator=( const CGeneralOptions& inOther ) {
			if ( this != &inOther ) {
				m_strOutputRootDirectory = inOther.m_strOutputRootDirectory;
				m_bDisplayWarning = inOther.m_bDisplayWarning;
				m_bSupportsCaptureOptions = inOther.m_bSupportsCaptureOptions;
				m_bUseFileNumber = inOther.m_bUseFileNumber;
			}
			return *this;
		}

		bool operator==( const CGeneralOptions& in ) {
			if ( m_bDisplayWarning != in.m_bDisplayWarning ) return false;
			if ( m_strOutputRootDirectory != in.m_strOutputRootDirectory ) return false;
			if ( m_bSupportsCaptureOptions != in.m_bSupportsCaptureOptions ) return false;
			if ( m_bUseFileNumber != in.m_bUseFileNumber ) return false;

			return true;
		}
		bool operator!=( const CGeneralOptions& in ) {
			return !operator==(in);
		}

		CPeekString GetOutputRootDirectory() { return m_strOutputRootDirectory; }
		void	SetOutputRootDirectory( CPeekString strOutputRootDirectory ) {
			m_strOutputRootDirectory = strOutputRootDirectory;
		}

		bool	Model( CPeekDataElement& ioElement );

		void	Reset() {
			m_bDisplayWarning = true;
			m_bSupportsCaptureOptions = true;
			m_bUseFileNumber = false;
			m_strOutputRootDirectory = kDefaultOutputDir;
		}

		void	SetDisplayWarning( bool bDisplayWarning ) {
			m_bDisplayWarning = bDisplayWarning;
		}
		bool	GetDisplayWarning() {
			return m_bDisplayWarning;
		}
		void	SetSupportsCaptureOptions( bool bSupportsCaptureOptions = true ) {
			m_bSupportsCaptureOptions = bSupportsCaptureOptions;
		}
		bool	IsSupportsCaptureOptions() {
			return m_bSupportsCaptureOptions;
		}
		void	SetUseFileNumber( bool bUseFileNumber ) {
			m_bUseFileNumber = bUseFileNumber;
		}
		bool	IsUseFileNumber() {
			return m_bUseFileNumber;
		}

	protected:
		CPeekString	m_strOutputRootDirectory;
		bool		m_bDisplayWarning;
		bool		m_bSupportsCaptureOptions;
		bool		m_bUseFileNumber;
	};
	// End CGeneralOptions


public:
	void	SetPortOptions( CPortOptions& PortNumber ) { m_PortOptions = PortNumber; }
	void	SetGeneralOptions( CGeneralOptions& GeneralOptions ) { m_GeneralOptions = GeneralOptions; }

	CPortOptions&	 GetPortOptions() { return m_PortOptions; }
	CPortNumberList& GetPortNumberList() { return m_PortOptions.GetPortNumberList(); }
	CGeneralOptions& GetGeneralOptions() { return m_GeneralOptions; }

	void	SyncCasesToGlobal() {
		SyncCasePortsToGlobal();
		SyncCaseCaptureOptionsToGlobal();
		DisableInvalidCases();
	}
	void	ResetCaseOptions(){
		m_CaseOptionsList.Reset();
	}
	void	SyncCasePortsToGlobal() {
		CPortNumberList& thePortNumberList = m_PortOptions.GetPortNumberList();
		return m_CaseOptionsList.SyncPortsToGlobal( thePortNumberList );
	}
	void	SyncCaseCaptureOptionsToGlobal() {
		return m_CaseOptionsList.SyncCaptureOptionsToGlobal( m_CaptureOptions );
	}
	CCaseOptionsList&	GetCaseOptionsList() {
		return m_CaseOptionsList;
	}
	bool	IsLogMsgsToFile() const {
		return m_CaptureOptions.IsLogMsgsToFile();
	}
	bool	IsLogMsgsToFileButton() const {
		return m_CaptureOptions.IsLogMsgsToFileButton();
	}
	void	SetLogMsgsToFileButton( bool bLogMsgsToFileButton ) {
		m_CaptureOptions.SetLogMsgsToFileButton( bLogMsgsToFileButton );
	}
	bool IsLogMsgsToScreen() const {
		return m_CaptureOptions.IsLogMsgsToScreen(); 
	}
	void SetLogMsgsToScreen( bool bLogMsgsToScreen ) {
		m_CaptureOptions.SetLogMsgsToScreen( bLogMsgsToScreen );
	}
	void SetFoldersForCasesButton( bool inFoldersForCases ) {
			m_CaptureOptions.SetFoldersForCasesButton( inFoldersForCases );
	}
	UInt16	GetRadiusPort( UInt16 inPort1, UInt16 inPort2 ) {
		return m_AllActiveRadiusPortsList.GetRadiusPort( inPort1, inPort2 );
	}
	bool	IsThereAnActivePort() {
		return m_AllActiveRadiusPortsList.IsThereAnActivePort();
	}
	bool	IsUserAnActiveTarget( CPeekString theUserName ) {
		return m_AllActiveUsersList.IsUserAnActiveTarget( theUserName );
	}
	bool	IsTimeActive() {
		return m_ActiveTimeSpan.IsTimeActive();
	}
	void	RebuildActivePortsList() {
		m_AllActiveRadiusPortsList.RebuildList( m_CaseOptionsList );
	}
	void	RebuildAllActiveUsersList() {
		m_AllActiveUsersList.RebuildList( m_CaseOptionsList );
	}
	void	ResetActiveTimeSpan();

	void	InitializePortNumberDefaults() {
		m_PortOptions.InitializePortNumberDefaults(); 
	}
	bool	IsSaveAgeSelected() const {
		return m_CaptureOptions.IsSaveAgeSelected();
	}
	bool	IsSaveSizeSelected() const {
		return m_CaptureOptions.IsSaveSizeSelected();
	}
	UInt32	GetSaveAgeOrZero() const {
		return m_CaptureOptions.GetSaveAgeOrZero();
	}
	void	GetSaveAgeValues( UInt32& outCount, CPeekString& outUnits ) {
		return m_CaptureOptions.GetSaveAgeValues( outCount, outUnits );
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
	void	SetSaveAgeSelected( bool inSelect ) {
		m_CaptureOptions.SetSaveAgeSelected( inSelect );
	}
	void	SetSaveAgeValue( UInt32 inValue ) {
		m_CaptureOptions.SetSaveAgeValue( inValue );
	}
	void	SetSaveAgeValue( UInt32 inCount, CPeekString inUnits ) {
		return m_CaptureOptions.SetSaveAgeValue( inCount, inUnits );
	}
	void	SetSaveSizeSelected( bool inSelect ) {
		m_CaptureOptions.SetSaveSizeSelected( inSelect );
	}
	void	SetSaveSizeValue( UInt64 inValue ) {
		m_CaptureOptions.SetSaveSizeValue( inValue );
	}
	bool	IsCaptureToFile() {
		return m_CaptureOptions.IsCaptureToFile();
	}
	bool	IsCaptureToScreen() {
		return m_CaptureOptions.IsCaptureToScreen();
	}
	bool	IsFoldersForCasesButton() {
		return m_CaptureOptions.IsFoldersForCasesButton();
	}
	bool	IsFoldersForCases() {
		return m_CaptureOptions.IsFoldersForCases();
	}
	void	SetCaptureToFile( bool bCaptureToFile ) {
		m_CaptureOptions.SetCaptureToFile( bCaptureToFile );
	}
	void	SetCaptureToScreen( bool bCaptureToScreen ) {
		m_CaptureOptions.SetCaptureToScreen( bCaptureToScreen );
	}
	CPeekString GetOutputRootDirectory() {
		return m_GeneralOptions.GetOutputRootDirectory();
	}
	void	SetOutputRootDirectory( CPeekString strOutputRootDirectory ) {
		m_GeneralOptions.SetOutputRootDirectory( strOutputRootDirectory );
	}
	CCaptureOptions& GetCaptureOptions() {
		return m_CaptureOptions;
	}
	void	SetCaptureOptions( CCaptureOptions& inCaptureOptions ) {
		m_CaptureOptions = inCaptureOptions;
	}
	void DisableInvalidCases() {
		m_CaseOptionsList.DisableInvalidCases();
	}
	void SetSupportsCaptureOptions( bool bSupportsCaptureOptions = true ) {
		m_GeneralOptions.SetSupportsCaptureOptions(  bSupportsCaptureOptions );
	}
	bool IsSupportsCaptureOptions() {
		return m_GeneralOptions.IsSupportsCaptureOptions();
	}
	void	SetUseFileNumber( bool bUseFileNumber ) {
		m_GeneralOptions.SetUseFileNumber( bUseFileNumber );
	}
	bool	IsUseFileNumber() {
		return m_GeneralOptions.IsUseFileNumber();
	}
	void SetDisplayWarning( bool bDisplayWarning ) {
		m_GeneralOptions.SetDisplayWarning( bDisplayWarning );
	}
	bool GetDisplayWarning() {
		return m_GeneralOptions.GetDisplayWarning();
	}
	CActiveUsersList& GetActiveUsersList() {
		return m_AllActiveUsersList;
	}

#ifdef DISPLAY_LOCK_ERROR
	// This defeats the optimizer
	char	DoNothing(){
		char x = ' ';
		x = 'a';
		return x;
	}
#endif

#ifdef SCREEN_INITIAL_START_STOP_TIMES
	bool	IsTimeOfInitialInterest() {
		return m_ActiveTimeSpan.IsTimeOfInterest();
	}
#endif

protected:
	CCaseOptionsList		m_CaseOptionsList;
	CPortOptions			m_PortOptions;
	CActiveRadiusPortsList	m_AllActiveRadiusPortsList;
	CActiveUsersList		m_AllActiveUsersList;
	CActiveTimeSpan			m_ActiveTimeSpan;
	CGeneralOptions			m_GeneralOptions;
	CCaptureOptions			m_CaptureOptions;

#ifdef INTEGRITY_OPTIONS
	CPeekPlugin* m_pPlugin;
#endif

};

typedef COptions*		COptionsPtr;
typedef COptions		CSafeOptions;
