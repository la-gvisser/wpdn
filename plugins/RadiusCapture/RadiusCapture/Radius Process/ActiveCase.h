// ============================================================================
//	ActiveCase.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once
#include "RadiusContextInterface.h"

#include "Options.h"
#include "CaptureLogger.h"
#include "RadiusPacketItemMgr.h"
#include "DualWriter.h"
#include "RadiusMessage.h"

#define kChangeReasonOptionsChange		0
#define kChangeReasonTimeout			1
#define kChangeReasonStopCapture		2
#define kChangeReasonDeselectStop		3
#define kChangeReasonWriterFailed		4

#define kCaseNoStartStopChange			0
#define kCaseStarted					1
#define kCaseStopped					2


class CRadiusContext;


// =============================================================================
//		CActiveCase
// =============================================================================

class CActiveCase
{
public:
	CActiveCase() {
		ASSERT( 0 );	// Default constructor required by compiler - should never be called
	}

	CActiveCase( CCaseOptions* pCaseOptions, IRadiusContext* pRadiusContext );
	~CActiveCase();

	bool	HasStopTimeExpired() { return m_pCaseOptions->HasStopTimeExpired(); }
	bool	ProcessIPPacket( CPacket& inPacket, UInt32 nValidationNumber );
	bool	ProcessRadiusPacket( CPacket& inPacket );
	bool	StartCapturing( const CPeekString& strInitString );
	void	StopCapturing(  UInt8 nReason );
	void	LogMessage( CRadiusMessage& inMessage, const bool bDisplayPacketNumber = false );
	bool	IsLoggingMsgsToFile();
	void	ProcessOptionsChange( CCaseOptions* pNewCaseOptions );
	CPeekString ProcessFileRefreshValuesChange( CCaseOptions* pNewCaseOptions, const CPeekString& strTimeGreenwich );
	CPeekString GetFormattedFileRefreshText( UInt32 nSaveAge, UInt64 nSaveSize, const CPeekString& strTimeGreenwich );
	void	StopCaseLogger();
#ifdef DISPLAY_PACKET_NUMBERS
	CPeekString GetPacketNumberString( bool bDisplayingToScreen = false );
#else
	CPeekString GetPacketNumberString() {
		CPeekOutString strPacketNumber;
		strPacketNumber << m_pDualWriter->GetNumPacketsWritten();
		return strPacketNumber;
	}
#endif

	bool IsEnabled() {
		return( m_pCaseOptions->IsEnabled() );
	}
	bool IsActive() { 
		return( m_pCaseOptions->IsActive() );
	}
	bool IsCapturing() { 
		ASSERT( (m_pDualWriter && !m_bIsCapturing) ? false : true );
		return m_bIsCapturing;
	}
	void SetCapturing( bool bCapturing ) {
		m_bIsCapturing = bCapturing;
	}
	UInt32 GetCaseCaptureId() {
		return m_nCaseCaptureId;
	}
	void SetCaseCaptureId( UInt32 nCaseCaptureId ) {
		m_nCaseCaptureId = nCaseCaptureId;
	}
	CPeekString	GetCaseCaptureIdString() {
		CPeekOutString strCaseCaptureId;
		strCaseCaptureId << m_nCaseCaptureId;
		return strCaseCaptureId;
	}
	inline void SetCaseOptions( CCaseOptions* inCaseOptions ) { 
		m_pCaseOptions = inCaseOptions; 
	}
	inline CCaseOptions* GetCaseOptions() {
		return m_pCaseOptions;
	}
	bool HasAssociatedName( const CPeekString& strUserName ) {
		return m_pCaseOptions->HasAssociatedUserName( strUserName );
	}
	bool UserIsActiveTarget( const CPeekString& strUserName ) {
		return m_pCaseOptions->UserIsActiveTarget( strUserName );
	}
	bool IsActivePort( const UInt16 uPort ) {
		return m_pCaseOptions->IsActivePort( uPort );
	}
	UInt8 GetTriggerTimeStatus() {
		return ( m_pCaseOptions->GetTriggerTimeStatus() );
	}
	bool IsCaptureToFile() {
		return m_pCaseOptions->IsCaptureToFile();
	}
	bool IsLogMsgsToFile() {
		return m_pCaseOptions->IsLogMsgsToFile();
	}
	bool IsCaptureToScreen() {
		return m_pCaseOptions->IsCaptureToScreen();
	}
	bool IsCapturingToScreen() {
		ASSERT( IsCapturing() );
		if ( !IsCapturing() ) return false;
		return m_pCaseOptions->IsCaptureToScreen();
	}
	bool IsLoggingMsgsToScreen() {
		ASSERT( IsCapturing() );
		if ( !IsCapturing() ) return false;
		return IsLogMsgsToScreen();
	}
	bool IsLogMsgsToScreen();
	CPeekString GetCaseName(){
		return m_pCaseOptions->GetName();
	}
	CPeekString GetTargetListString() {
		return m_pCaseOptions->GetTargetListString();
	}
	UInt32	GetFileNumber() {
		return m_pDualWriter->GetFileNumber();
	}
	CPeekString GetFileNumberString() {
		if ( !m_pDualWriter ) {
			ASSERT( 0 );
			return _T("");
		}
		UInt32 nFileNumber = m_pDualWriter->GetFileNumber();
		CPeekOutString strFileNumber;
		if ( nFileNumber > 0 ) {
			strFileNumber << nFileNumber;
		}
		else {
			strFileNumber << L"";
		}
		return strFileNumber;
	}
	CPeekString GetFormattedStartStopTime( bool bStart ) {
		return m_pCaseOptions->GetFormattedStartStopTime( bStart );
	}
	bool	HasTriggerTimeStartedYet() {
				ASSERT( IsEnabled() );
				if ( !IsEnabled() ) return false;
				UInt8 nTriggerTimeStatus = m_pCaseOptions->GetTriggerTimeStatus();
				if ( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted ) return false;
				return true;
			}
	bool	IsNewlyActivated() { 
				if ( IsActive() && !IsCapturing() && HasTriggerTimeStartedYet() ) return true;
				return false;
			}
	void	MonitorFiles( COptions* pOptions ) {
		if ( m_pDualWriter ) {
			m_pDualWriter->Monitor( pOptions );
		}
	}
//	void SetCaptureFileNameSettings( bool bUseCaseCaptureId ) {
//		bUseCaseCaptureId;
//	}
	static void CALLBACK FileRefreshCallback( void* pUserArg, bool bAboutToRestart );
	void	IncludeCaptureOptions( CCaptureOptions& inCaptureOptions ) {
		inCaptureOptions.Include( m_pCaseOptions->GetCaptureOptions() );
	}
  #ifdef _DEBUG
	void DebugValidateCase();
  #endif

  #ifdef PACKET_WRITER_ERROR_HANDLING
	//	void ProcessFailureAllert();
  #endif

private:
	// Disallow copy constructor
	CActiveCase( const CActiveCase& inActiveCase ) {
		inActiveCase;
		ASSERT( 0 );
	}
	// Disallow operator=
	CActiveCase& operator=( const CActiveCase& inActiveCase ) {
		inActiveCase;
		ASSERT( 0 );
	}

protected:
	bool	GetFormattedCaptureStartMsg( const CPeekString& strInitString, CRadiusMessage& theMessage );
	CPeekString GetFormattedCaptureChangeString( CCaseOptions* pNewCase, const CPeekString& strRefreshMessage,
													CPeekString& strTimeGreenwich );
	bool	GetFormattedCaptureStopMsg( CRadiusMessage& theMessage, UInt8 nChangeReason );
	void	FileRefreshResponse( bool bAboutToRestart );
	void	AddStdLogHeaderLines( const CPeekString& strTimeGreenwich, CPeekString& strInitString,
									const CPeekString& strMessage = _T(""), const CPeekString& strFileRefreshMessage = _T("") );

	CPeekString GetFormattedActivePortsString() {
		return m_pCaseOptions->GetFormattedActivePortsString();
	}

  #ifdef PACKET_WRITER_ERROR_HANDLING
	bool	FailureAllertAllTargetIPs();
  #endif

	void 	ProcessWriteFailure() {
	  #ifdef PACKET_WRITER_ERROR_HANDLING
		FailureAllertAllTargetIPs();
	  #endif
		StopCapturing( kChangeReasonWriterFailed );
	}

protected:
	CCaseOptions*   m_pCaseOptions;
	CDualWriter*	m_pDualWriter;
	bool		    m_bHasEntries;
	UInt32			m_nPacketWriterErrors;
	UInt32			m_nCaseCaptureId;
	bool			m_bIsCapturing;
	IRadiusContext*	m_pOwningRadiusContext;
	UInt32			m_nLastValidationNumber;
};


// =============================================================================
//		CActiveCaseList
// =============================================================================

class CActiveCaseList
	: public CAtlArray < CActiveCase* >
{

public:
	CActiveCaseList() : m_bUnstartedActiveCasesPending( false ) {}
	~CActiveCaseList() {
		Reset();
	}

	bool InitializeWriters( const CPeekString& strInitString );
	void InitializeUnstartedActiveCasesFlag();
	void CloseAllPacketWriters();
	bool StartNewlyActiveCases( const CPeekString& strInitString, bool& bOutHasActiveCases );
	bool StopExpiredCases( bool& bOutHasActiveCases );
	void MonitorFiles( COptions * pOptions );

	CActiveCaseList& operator=( CActiveCaseList& inOldActiveCaseList ) {
		Reset();
		const size_t nCount = inOldActiveCaseList.GetCount();
		for ( size_t i = 0; i < nCount; i++ ){
			CActiveCase* pActiveCase = inOldActiveCaseList.GetAt( i );
			Add( pActiveCase );				
		}
		return *this;
	}
	bool IsThereAnActiveCase() {
		return ( GetCount() > 0 );
	}
	CActiveCase* Find( CPeekString& inCaseName ) {
		const size_t nActiveCaseCount = GetCount();
		for ( size_t i = 0; i < nActiveCaseCount; i++ ){
			CActiveCase* pActiveCase = GetAt( i );
			CPeekString	 theName = pActiveCase->GetCaseName();
			if ( theName.CompareNoCase( inCaseName ) == 0 ) {
				return pActiveCase;
			}
		}
		return NULL;
	}
	bool Find( CActiveCase* inActiveCase ) {
		const size_t nActiveCaseCount = GetCount();
		for ( size_t i = 0; i < nActiveCaseCount; i++ ){
			CActiveCase* theActiveCase = GetAt( i );
			if ( inActiveCase == theActiveCase ) {
				if ( inActiveCase->GetCaseName() == theActiveCase->GetCaseName() ) {
					return true;
				}
			}
		}
		return false;
	}
	void Reset() {
		const int nCount = static_cast<int>( GetCount() );
		for ( int i = nCount - 1; i >= 0; i-- ){
			CActiveCase* pActiveCase = GetAt( i );
			delete pActiveCase;
			RemoveAt( i );
		}
		RemoveAll();
	}
	bool CheckNewlyActiveCases() {
		if ( !m_bUnstartedActiveCasesPending ) {
				return false;
		}

		const size_t nActiveCaseCount = GetCount();
		for ( size_t i = 0; i < nActiveCaseCount; i++ ) {
			CActiveCase* pActiveCase = GetAt( i );
			if ( pActiveCase->IsNewlyActivated() ) { 
				return true;
			}
		}
		return false;
	}
	bool CheckStopExpiredCases( bool& bOutHasActiveCases ) {
		bool bCaseNeedsToStop = false;
		bOutHasActiveCases = false;

		// Find out if any Active Cases have expired
		const int nActiveCaseCount = static_cast<int>( GetCount() );
		for ( int i = nActiveCaseCount - 1; i >= 0; i-- ) {
			CActiveCase* pActiveCase = GetAt( i );
			if ( pActiveCase->IsActive() ) {
				bOutHasActiveCases = true;
				ASSERT( pActiveCase->GetTriggerTimeStatus() != kTriggerTimeStatusExpired );
			}
			else {
				ASSERT( pActiveCase->GetTriggerTimeStatus() == kTriggerTimeStatusExpired );
				bCaseNeedsToStop = true;
			}
		}

		return bCaseNeedsToStop;
	}
	CActiveCase* DeleteAt( size_t iElement ) {
		ASSERT( iElement < GetCount() );
		CActiveCase* pActiveCase = GetAt( iElement );
		delete pActiveCase;
		RemoveAt( iElement );
		return NULL;
	}
//	void SetCaptureFileNameSettings( bool bUseCaseCaptureId ) {
//		const size_t nActiveCaseCount = GetCount();
//		for ( size_t i = 0; i < nActiveCaseCount; i++ ) {
//			CActiveCase* pActiveCase = GetAt( i );
//			pActiveCase->SetCaptureFileNameSettings( bUseCaseCaptureId ); 
//		}
//	}

protected:
	bool	m_bUnstartedActiveCasesPending;

};


// =============================================================================
//		CTargetIPActiveCase
// =============================================================================

class CTargetIPActiveCase
{

public:

	CTargetIPActiveCase() { ASSERT( 0 ); }	// Not used: needed to make the compiler happy
	CTargetIPActiveCase( CActiveCase* pActiveCase )
		: m_pActiveCase( pActiveCase ) 
		, m_bHasStartedCapturing( false )
		, m_strInternalCaseName( pActiveCase->GetCaseName() ) {}
	~CTargetIPActiveCase()  {}

	void	SetActiveCasePtr( CActiveCase* pActiveCase ) { m_pActiveCase = pActiveCase; }
	bool	HasTriggerTimeStartedYet() { return m_pActiveCase->HasTriggerTimeStartedYet(); }
	CPeekString	GetCaseName() { return m_pActiveCase->GetCaseName(); }
//	bool	ProcessStartContinue( CPeekString& strUserName, const CPeekString& strFormat, const bool bDisplayPacketNumber = false,
//									const bool bDisplayIfCapturing = true );
	bool	ProcessStartContinue( const CPeekString& strFormat, const bool bDisplayPacketNumber = false,
		const bool bDisplayIfCapturing = true );

	void  LogMessage( CRadiusMessage& inMessage, const bool bDisplayPacketNumber = false ) { 
		if ( !m_pActiveCase ) {
			ASSERT( 0 );
			return;
		}
		m_pActiveCase->LogMessage( inMessage, bDisplayPacketNumber ); 
	}
	bool  IsActive() {
		return m_pActiveCase->IsActive();
	}
	UInt8 GetTriggerTimeStatus() {
		return m_pActiveCase->GetTriggerTimeStatus();
	}
	bool  HasStopTimeExpired() {
		return m_pActiveCase->HasStopTimeExpired();
	}
	bool  UserIsActiveTarget( const CPeekString& theUserName ) {
		return m_pActiveCase->UserIsActiveTarget( theUserName );
	}
	bool  IsValid() {
		return m_pActiveCase != NULL;
	}
	bool  IsEnabled() {
		return m_pActiveCase->IsEnabled();
	}
	bool  ProcessIPPacket( CPacket& inPacket, UInt32 nValidationNumber ) { 
		if ( m_bHasStartedCapturing && m_pActiveCase && m_pActiveCase->IsCaptureToFile() && m_pActiveCase->IsCapturing() ) {
			if ( !m_pActiveCase->ProcessIPPacket( inPacket, nValidationNumber ) ) {
				ASSERT( !m_pActiveCase->IsCapturing() );
				return false;
			}
		}
		return true;
	}
	bool  ProcessRadiusPacket( CPacket& inPacket ) {
		if ( !m_pActiveCase ) {
			ASSERT( 0 );
			return true;
		}
		return m_pActiveCase->ProcessRadiusPacket( inPacket );
	}
	CActiveCase* GetActiveCasePtr() {
		return m_pActiveCase;
	}
	bool IsCaptureToFile() {
		return m_pActiveCase->IsCaptureToFile();
	}
	bool IsCaptureToScreen() {
		return m_pActiveCase->IsCaptureToScreen();
	}
	CPeekString GetInternalCaseName() {
		ASSERT( m_strInternalCaseName.GetLength() > 0 );
		return m_strInternalCaseName;
	}
	void IncludeCaptureOptions( CCaptureOptions& inCaptureOptions ) {
		m_pActiveCase->IncludeCaptureOptions( inCaptureOptions );
	}

  #ifdef PACKET_WRITER_ERROR_HANDLING
  /*
	void	ProcessFailureAllert() {
		ASSERT( m_pActiveCase );
		m_pActiveCase->ProcessFailureAllert();
	}
  */
  #endif

  #ifdef _DEBUG
	bool  IsCapturing() { return m_pActiveCase->IsCapturing(); }
  #endif

protected:
	CActiveCase*	m_pActiveCase;
	bool			m_bHasStartedCapturing;
	CPeekString		m_strInternalCaseName;
};


// =============================================================================
//		CTargetIPActiveCaseMgr
// =============================================================================

class CTargetIPActiveCaseMgr
	: public CAtlArray < CTargetIPActiveCase >
{
public:

	CTargetIPActiveCaseMgr() 
		:  m_bUnstartedActiveCasesPending( false ) {}
	~CTargetIPActiveCaseMgr(){}

	bool RebuildList( CActiveCaseList& ActiveCaseList, CRadiusPacketItem& thePacketItem, const CPeekString& strInitString, bool& bNewCaptureStarted );
	bool RefreshList( CActiveCaseList& theActiveCaseList, const CPeekString& theUserName, const CPeekString& strFormat, bool& bChangeMade );
	bool RefreshListForNewlyStarted( CActiveCaseList& ActiveCaseList, const CPeekString& theUserName );
	bool StopExpiredTargetIPActiveCases( const CPeekString& theUserName, const UInt32 uIPAddress, bool& bChangeMade );

	CTargetIPActiveCaseMgr& operator= ( const CTargetIPActiveCaseMgr& inOriginal ) {
		RemoveAll();

		const size_t nCount = inOriginal.GetCount();
		for( size_t i = 0; i < nCount; i++ ) {
			const CTargetIPActiveCase& theTargetIPActiveCase = inOriginal.GetAt( i );
			Add( theTargetIPActiveCase );
		}

		m_bUnstartedActiveCasesPending = inOriginal.m_bUnstartedActiveCasesPending;

		return *this;
	}
	bool Find( CActiveCase* pActiveCase ) {
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ){
			CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
			if ( theTargetIPActiveCase.GetActiveCasePtr() == pActiveCase ) {
				if ( theTargetIPActiveCase.GetInternalCaseName() == pActiveCase->GetCaseName() ) {
					return true;
				}
			}
		}
		return false;
	}
	bool Find( CActiveCase* pActiveCase, size_t& nPos ) {
		const size_t nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ){
			CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
			if ( theTargetIPActiveCase.GetActiveCasePtr() == pActiveCase ) {
				if ( theTargetIPActiveCase.GetInternalCaseName() == pActiveCase->GetCaseName() ) {
					nPos = i;
					return true;
				}
			}
		}
		return false;
	}
	bool IsAnActiveCaseUnstarted() {
		return m_bUnstartedActiveCasesPending;
	}
	void LogMessageToActiveCases( CPeekString& inMessage, const bool bDisplayPacketNumber = false ) {
		CPeekString strTimeString = ::GetTimeStringGreenwich(); // + _T(" ") + inMessage;

		const int nCount = static_cast<int>( GetCount() );
		for( int i = nCount - 1; i >= 0; i-- ) {
			CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
			ASSERT( theTargetIPActiveCase.GetTriggerTimeStatus() != kTriggerTimeStatusNotYetStarted );
			CRadiusMessage theMessage( strTimeString, inMessage );
			theTargetIPActiveCase.LogMessage( theMessage, bDisplayPacketNumber );
		}		
	}
	bool AllActiveCaseTimesHaveExpired() {
		for( size_t i = 0; i < GetCount(); i++ ) {
			CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
			if ( !theTargetIPActiveCase.HasStopTimeExpired() ) return false;
		}
		return true;
	}
	void StopAllCaseCaptures( CPeekString& strMessage, const bool bDisplayPacketNumber = false ) {
		if ( strMessage != _T("") ) {
			LogMessageToActiveCases( strMessage, bDisplayPacketNumber );
		}
		RemoveAll();
	}
//	bool ProcessStartContinue( CPeekString& strUserName, const CPeekString& strFormat, const bool bDisplayPacketNumber = false,
	bool ProcessStartContinue( const CPeekString& strFormat, const bool bDisplayPacketNumber = false,
								const bool bDisplayIfCapturing  = true ) {
		bool bIsNewlyStarted = false;
		size_t nCount = GetCount();
		for( size_t i = 0; i < nCount; i++ ) {
			CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
//			if ( theTargetIPActiveCase.ProcessStartContinue( strUserName, strFormat, bDisplayPacketNumber, bDisplayIfCapturing ) ) {
			if ( theTargetIPActiveCase.ProcessStartContinue( strFormat, bDisplayPacketNumber, bDisplayIfCapturing ) ) {
				bIsNewlyStarted = true;
			}
		}
		return bIsNewlyStarted;
	}

  #ifdef PACKET_WRITER_ERROR_HANDLING
	bool AllCasesAreStopped() {
		return ( GetCount() == 0 );
	}
	bool ProcessFailureAllert( CPeekString& inCaseName );
  #endif

  #ifdef _DEBUG
	bool DebugUserNameIsProperlyContained( const CPeekString& theUserName ){
		const size_t nCount = GetCount();
		if ( nCount == 0 ) return true;
		for( size_t i = 0; i < nCount; i++ ) {
			CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
			ASSERT( theTargetIPActiveCase.IsActive() );
			if ( theTargetIPActiveCase.UserIsActiveTarget( theUserName ) ) return true;
		}
		return false;
	}
  #endif

protected:
	CPeekString FormatUserIPString( const CPeekString& strUserName, UInt32 uIPAddress );

	void	StopExpiredTargetIPActiveCase( CTargetIPActiveCase& theTargetIPActiveCase, int iPosition,
											const CPeekString& theUserName, UInt32 uIPAddress ) {
		CPeekString strMessage = FormatUserIPString( theUserName, uIPAddress );
		strMessage += _T(" - monitoring stopped due to expired case");

		CPeekString strTimeString = ::GetTimeStringGreenwich();
		CRadiusMessage theMessage( strTimeString, strMessage );
		theTargetIPActiveCase.LogMessage( theMessage );
		RemoveAt( iPosition );
	}

protected:
	bool			m_bUnstartedActiveCasesPending;
};
