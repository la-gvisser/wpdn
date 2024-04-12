// ============================================================================
//	CaptureOptions.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekTime.h"
#include "Utils.h"
#include "PeekDataModeler.h"

#define	kPref_CaptureOptions		_T( "CaptureOptions" )
#define kPref_CaptureToFile			_T( "CaptureToFile" )
#define kPref_LogMsgsToFile			_T( "LogMsgsToFile" )
#define kPref_CaptureToScreen		_T( "CaptureToScreen" )
#define kPref_LogMsgsToScreen		_T( "LogMsgsToScreen" )
#define kPref_SaveAge				_T( "SaveAge" )
#define kPref_SaveSize				_T( "SaveSize" )
#define kPref_Count					_T( "Count" )
#define kPref_Units					_T( "Units" )
#define kPref_UseGlobalDefaults		_T( "UseGlobalDefaults" )


// =============================================================================
//		CCaptureOptions
// =============================================================================

class CCaptureOptions
{
public:
	CCaptureOptions() {
		Reset();
	}

	~CCaptureOptions() {}

	CCaptureOptions& operator=( const CCaptureOptions& in ) {
		m_bCaptureToFile = in.m_bCaptureToFile;
		m_bCaptureToScreen = in.m_bCaptureToScreen;
		m_bLogMsgsToFileButton = in.m_bLogMsgsToFileButton;
		m_bFoldersForCasesButton = in.m_bFoldersForCasesButton;
		m_bLogMsgsToScreen = in.m_bLogMsgsToScreen;
		m_bSaveAge = in.m_bSaveAge;
		m_nSaveAge = in.m_nSaveAge;
		m_bSaveSize = in.m_bSaveSize;
		m_nSaveSize = in.m_nSaveSize;
		m_bUseGlobalDefaults = in.m_bUseGlobalDefaults;

		return *this;
	}
	bool operator==( const CCaptureOptions& in ) {
		if (
			m_bCaptureToFile == in.m_bCaptureToFile		&&
			m_bCaptureToScreen == in.m_bCaptureToScreen &&
			m_bLogMsgsToFileButton == in.m_bLogMsgsToFileButton &&
			m_bFoldersForCasesButton == in.m_bFoldersForCasesButton &&
			m_bLogMsgsToScreen == in.m_bLogMsgsToScreen &&
			m_bSaveAge == in.m_bSaveAge &&
			m_nSaveAge == in.m_nSaveAge &&
			m_bSaveSize == in.m_bSaveSize &&
			m_nSaveSize == in.m_nSaveSize &&
			m_bUseGlobalDefaults == in.m_bUseGlobalDefaults ) {
				return true;
			}
		return false;
	}
	bool operator!=( const CCaptureOptions& inCaptureOptions ) {
		return !operator==(inCaptureOptions);
	}
	void	Initialize() {
		m_bCaptureToFile = false;
		m_bCaptureToScreen = false;
		m_bLogMsgsToFileButton = false;
		m_bFoldersForCasesButton = false;
		m_bLogMsgsToScreen = false;
	}

	bool		 AreOptionsValid();
	void		Reset() {
		m_bCaptureToFile = true;
		m_bCaptureToScreen = true;
		m_bLogMsgsToFileButton = true;
		m_bFoldersForCasesButton = false;
		m_bLogMsgsToScreen = true;
		m_bSaveAge = true;
		m_nSaveAge = kSecondsInADay;
		m_bSaveSize = false;
		m_nSaveSize = kMegabyte;
		m_bUseGlobalDefaults = true;
	}
	void		 GetSaveSizeBytes( UInt64& outCount, CPeekString& outUnits ) const;
	void		 GetSaveAgeValues( UInt32& outCount, CPeekString& outUnits ) const;
	void		 SetSaveSizeValue( UInt64 inCount, CPeekString inUnits );
	void		 SetSaveAgeValue( UInt32 inCount, CPeekString inUnits );

	void	Include( CCaptureOptions& inCaptureOptions ) {
		if ( inCaptureOptions.IsCaptureToFile() ) SetCaptureToFile( true );
		if ( inCaptureOptions.IsLogMsgsToFileButton() ) SetLogMsgsToFileButton( true );
		if ( inCaptureOptions.IsFoldersForCasesButton() ) SetFoldersForCasesButton( true );
		if ( inCaptureOptions.IsCaptureToScreen() ) SetCaptureToScreen( true );
		if ( inCaptureOptions.IsLogMsgsToScreen() ) SetLogMsgsToScreen( true );
	}
	bool	IsCaptureToFile() {
		return m_bCaptureToFile;
	}
	bool	IsCaptureToScreen() {
		return m_bCaptureToScreen;
	}
	void	SetCaptureToFile( bool bCaptureToFile ) {
		m_bCaptureToFile = bCaptureToFile;
	}
	void	SetCaptureToScreen( bool bCaptureToScreen ) {
		m_bCaptureToScreen = bCaptureToScreen;
	}
	bool	IsLogMsgsToFile() const {
		if ( m_bCaptureToFile && m_bLogMsgsToFileButton ) return true;
		return false; 
	}
	bool	IsLogMsgsToFileButton() const {
		return m_bLogMsgsToFileButton;
	}
	bool	IsFoldersForCases() const {
		if ( m_bCaptureToFile && m_bFoldersForCasesButton ) return true;
		return false; 
	}
	bool	IsFoldersForCasesButton() const {
		return m_bFoldersForCasesButton;
	}
	bool	Model( CPeekDataElement& ioElement, bool bIsCase );

	void	SetLogMsgsToFileButton( bool bLogMsgsToFileButton ) {
		m_bLogMsgsToFileButton = bLogMsgsToFileButton;
	}
	void	SetFoldersForCasesButton( bool bFoldersForCasesButton ) {
		m_bFoldersForCasesButton = bFoldersForCasesButton;
	}
	bool	IsLogMsgsToScreen() const {
		return m_bLogMsgsToScreen;
	}
	void	SetLogMsgsToScreen( bool bLogMsgsToScreen ) {
		m_bLogMsgsToScreen = bLogMsgsToScreen;
	}
	bool	IsSaveAgeSelected() const {
		return m_bSaveAge;
	}
	bool	IsSaveSizeSelected() const {
		return m_bSaveSize;
	}
	UInt32	GetSaveAgeSeconds() const {
		return m_nSaveAge;
	}
	UInt32	GetSaveAgeOrZero() const {
		return m_bSaveAge ? m_nSaveAge : 0;
	}
	UInt64	GetSaveSizeBytes() const {
		return m_nSaveSize;
	}
	UInt64	GetSaveSizeOrZero() const {
		return m_bSaveSize ? m_nSaveSize : 0;
	}
	void	SetSaveAgeSelected( bool inEnable ) {
		m_bSaveAge = inEnable;
	}
	void	SetSaveAgeValue( UInt32 inValue ) {
		m_nSaveAge = inValue;
	}
	void	SetSaveSizeSelected( bool inEnable ) {
		m_bSaveSize = inEnable;
	}
	void	SetSaveSizeValue( UInt64 inValue ) {
		m_nSaveSize = inValue;
	}
	void	SetGlobalDefaultsFlag( bool bUsingGlobalDefaults ) {
		m_bUseGlobalDefaults = bUsingGlobalDefaults;
	}
	void	SyncGlobalDefaults( CCaptureOptions& inGlobalCaptureOptions ) {
		m_bUseGlobalDefaults = true;
		*this = inGlobalCaptureOptions;
	}
	bool	IsUseGlobalDefaults() {
		return m_bUseGlobalDefaults;
	}
	void	SetUseGlobalDefaults( bool bUseGlobalDefaults ) {
		m_bUseGlobalDefaults = bUseGlobalDefaults;
	}

protected:
	bool	m_bCaptureToFile;
	bool	m_bLogMsgsToFileButton;
	bool	m_bFoldersForCasesButton;
	bool	m_bCaptureToScreen;
	bool	m_bLogMsgsToScreen;
	bool	m_bSaveAge;
	UInt32	m_nSaveAge;
	bool	m_bSaveSize;
	UInt64	m_nSaveSize;
	bool	m_bUseGlobalDefaults;
};
