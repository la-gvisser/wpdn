// ============================================================================
// RadiusProcess.cpp:
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "RadiusProcess.h"
#include "RadiusPacketItem.h"
#include "OmniEngineContext.h"


// ============================================================================
//		CRadiusProcess
// ============================================================================

// -----------------------------------------------------------------------------
// Construction and Destruction
// -----------------------------------------------------------------------------

CRadiusProcess::CRadiusProcess()
	: m_pRadiusContext( NULL )
	, m_nValidationNumber( 0 )
{
}

CRadiusProcess::~CRadiusProcess() 
{
	if ( !m_pRadiusContext ) return;

	COmniEngineContext* pRadiusContext( reinterpret_cast<COmniEngineContext*>(m_pRadiusContext) );

	ASSERT( !pRadiusContext->IsCapturing() );
	if ( pRadiusContext->IsCapturing() ) {
		StopCapture();
	}

}


// -----------------------------------------------------------------------------
//		ProcessPacket
// -----------------------------------------------------------------------------

bool
CRadiusProcess::ProcessPacket(
	CPacket&	inPacket, 
	COptions&	inOptions )
{
	bool bDisplayPacketToScreen = false;

	if ( inPacket.IsError() )					return false;

	CLayerEthernet layerEthernet;
	if ( !inPacket.GetLayer( layerEthernet ) )	return false;

	CLayerIP layerIP;
	if ( !inPacket.GetLayer( layerIP ) )	return false;

	// Keep this test until IPv6 is supported
	if ( layerIP.GetVersion() != kIpVersion_4 ) return false;

	CRadiusPacketItem thePacketItem;

	thePacketItem.SetIPAddresses( layerIP.GetSource(), layerIP.GetDestination() );

	CLayerUDP layerUDP;
	if( inPacket.GetLayer(layerUDP) ) {
		if (layerUDP.IsNotValid() ) { 
			ASSERT( 0 );
			return false;
		}

		bool bPacketIsRadius = thePacketItem.GetRadiusInfo( layerUDP, inOptions );

		if ( bPacketIsRadius ) {
			bDisplayPacketToScreen = ProcessRadiusPacket( inOptions, thePacketItem, inPacket );
			return bDisplayPacketToScreen;
		}
	}

	bDisplayPacketToScreen = ProcessIPPacket( inOptions, inPacket, thePacketItem );

	return bDisplayPacketToScreen;
}


// -----------------------------------------------------------------------------
//		ProcessRadiusPacket
// -----------------------------------------------------------------------------

bool
CRadiusProcess::ProcessRadiusPacket(
	COptions&			inOptions,
	CRadiusPacketItem&	thePacketItem,
	CPacket&			inPacket )
{
	inPacket;

	CTargetIP	theTargetIP;

	bool bPacketItemUserIsATarget = ProcessTargetCandidate( thePacketItem, inOptions );

	bool bProcessRadius = thePacketItem.ProcessRadius();
	if ( !bProcessRadius ) {
		return false;
	}

	const CPeekString strPacketItemUserName = thePacketItem.GetRadiusUserName();
	if ( strPacketItemUserName.IsEmpty() ) {
		ASSERT( 0 );
		return false;
	}

	bool bCurrentlyMonitoringIP = false;

	const UInt32 uPacketItemIPAddress = thePacketItem.GetFramedIPAddr();

	if ( !bPacketItemUserIsATarget ) {
		if( uPacketItemIPAddress == 0 ) {
			return false;
		}

		bCurrentlyMonitoringIP = IsCurrentlyMonitoringIP( uPacketItemIPAddress );
		if ( !bCurrentlyMonitoringIP ) {
			return false;
		}
	}

	bool bTargetIPIsActive = SetActiveTargetIP( inOptions, thePacketItem, &theTargetIP, bCurrentlyMonitoringIP );
	if ( !bTargetIPIsActive ) {
		return false;
	}

	// It's possible that the user is no longer active, check again
	bPacketItemUserIsATarget = inOptions.IsUserAnActiveTarget( strPacketItemUserName );

	const UInt32 uMonitoring = thePacketItem.GetMonitoring();

	// Now find out if the name associated with the IP address has changed.
	// If so, call special processing for this condition.
	if ( bCurrentlyMonitoringIP && (uMonitoring != MONITORING_NO_ACTION) ) {
		CPeekString strTargetIPUserName = theTargetIP.GetUserName();
		ASSERT( (uPacketItemIPAddress > 0) && (strPacketItemUserName.GetLength() > 0)
				&& (strTargetIPUserName.GetLength() > 0) );

		if ( (strPacketItemUserName.GetLength() > 0) && (strTargetIPUserName.GetLength() > 0) ) {
			if ( strPacketItemUserName.CompareNoCase( strTargetIPUserName ) != 0 ) {
				bool bContinueToProcess = ProcessRadiusNameChanged( inOptions, thePacketItem, theTargetIP, bCurrentlyMonitoringIP );
				if ( !bContinueToProcess ) {
					return false;
				}
			}
		}
	}

	if ( !bPacketItemUserIsATarget ) {
		return false;
	}

	InitializeCaptureOptions( thePacketItem );

	bool bProcessedToWriter = false;

	CCaptureOptions& theCaptureOptions = thePacketItem.GetCaptureOptions();
	theTargetIP.DetermineCaptureOptions( theCaptureOptions );

	bool bCaptureToFile = theCaptureOptions.IsCaptureToFile();
	bool bCaptureToScreen = theCaptureOptions.IsCaptureToScreen();

	ASSERT( !strPacketItemUserName.IsEmpty() );
	ASSERT( inOptions.IsUserAnActiveTarget( strPacketItemUserName ) == true );

	bool bPacketIsDisplayable = true;

	if( uMonitoring == MONITORING_START || uMonitoring == MONITORING_START_NOREPORT	) {
		if( theTargetIP.GetIP() == 0 ) {
			ASSERT( 0 );
			return false;
		}
		if ( bCaptureToFile ) {
			bPacketIsDisplayable = ProcessRadiusPktToTargetIP( inPacket, &theTargetIP );
			bProcessedToWriter = true;
		}
		bool bDisplayIfCapturing = false;
		if( uMonitoring == MONITORING_START ) {
			bDisplayIfCapturing = true;
		}

		StartContinueTargetCapture( thePacketItem, theTargetIP, bCurrentlyMonitoringIP, bDisplayIfCapturing );
	}
	else if ( uMonitoring == MONITORING_STOP ) {
		if( theTargetIP.GetIP() == 0 ) {
			ASSERT( 0 );
		}
		if ( bCaptureToFile ) {
			bPacketIsDisplayable = ProcessRadiusPktToTargetIP( inPacket, &theTargetIP );
			bProcessedToWriter = true;
		}

		StopIPCapture( theTargetIP, true );
	}

	bool bDisplayPacketToScreen = false;

	if ( !bProcessedToWriter && bPacketIsDisplayable && bCaptureToFile ) {
		bPacketIsDisplayable = ProcessRadiusPktToTargetIP( inPacket, &theTargetIP );
	}

	if ( bPacketIsDisplayable && bCaptureToScreen ) {
	  #ifdef DISPLAY_PACKET_NUMBERS
		(reinterpret_cast<COmniEngineContext*>(m_pRadiusContext))->IncrementPacketsDisplayedCount();
	  #endif
		bDisplayPacketToScreen = true;
	}

	return bDisplayPacketToScreen;
}


// -----------------------------------------------------------------------------
//		StartContinueTargetCapture
// -----------------------------------------------------------------------------

void
CRadiusProcess::StartContinueTargetCapture(
	CRadiusPacketItem&	thePacketItem,
	CTargetIP&			inTargetIP,
	bool				bCurrentlyCapturing,
	bool				bDisplayIfCapturing )
{
	CCaptureOptions& theCaptureOptions = thePacketItem.GetCaptureOptions();
	const bool bLogMessages = theCaptureOptions.IsLogMsgsToFile() || theCaptureOptions.IsLogMsgsToScreen();

	if ( !bCurrentlyCapturing ) {
		StartIPCapture( inTargetIP, true );
	}

	if ( bCurrentlyCapturing ) {
		bool bOutNewlyActivatedCaseStarted = false;

		inTargetIP.ContinueCapture( bLogMessages, bOutNewlyActivatedCaseStarted, true, bDisplayIfCapturing );
		if ( bOutNewlyActivatedCaseStarted ) {
			m_TargetIPMap.Set( inTargetIP );
		}
	}
}


// -----------------------------------------------------------------------------
//		ProcessIPPacket
// -----------------------------------------------------------------------------

bool	
CRadiusProcess::ProcessIPPacket( 
	COptions&			inOptions,
	CPacket&			inPacket,
	CRadiusPacketItem&	thePacketItem )
{
	CTargetIP theTargetIP;
	bool	  bATriggerTimeHasExpired = false;
	bool	  bPacketProcessed = false;
	UInt32	  uIPAddress = 0;

	bool bCaptureToFile = false;
	bool bCaptureToScreen = false;

	m_nValidationNumber++;

	// We must always loop twice in case one target is talking to a second target
	for ( UInt32 i = 0; i < 2; i++ ) {
		if ( i == 0 ) {
			uIPAddress = thePacketItem.GetSourceIPAddr();
		}
		else {
			ASSERT( i == 1 );
			uIPAddress = thePacketItem.GetDestIPAddr();
		}

		if ( uIPAddress == 0 ) continue;

		bool bTargetIPIsATarget = m_TargetIPMap.Find( uIPAddress, theTargetIP );

		if ( bTargetIPIsATarget ) {
			CCaptureOptions theCaptureOptions;
			theTargetIP.DetermineCaptureOptions( theCaptureOptions );

			bCaptureToFile = theCaptureOptions.IsCaptureToFile();
			bCaptureToScreen = theCaptureOptions.IsCaptureToScreen();

			bPacketProcessed = theTargetIP.ProcessIPPacket( inPacket, bATriggerTimeHasExpired, m_nValidationNumber, bCaptureToFile );

			if ( bATriggerTimeHasExpired ) {
				bool bMapHasActiveCases;
				StopExpiredCases( bMapHasActiveCases, inOptions ) ;
			  #ifdef _DEBUG
				bTargetIPIsATarget = m_TargetIPMap.Find( uIPAddress, theTargetIP );
				if ( bTargetIPIsATarget ) {
					ASSERT( bMapHasActiveCases );
					ASSERT( bPacketProcessed );
				}
			  #endif
			}

			if ( !bPacketProcessed ) {
			  #ifdef PACKET_WRITER_ERROR_HANDLING
				if ( !theTargetIP.AllCasesAreStopped() ) {
					if ( theTargetIP.AllTargetIPActiveCaseTimesHaveExpired() ) {
						ASSERT( uIPAddress == theTargetIP.GetIP() );
						StopIPCapture( theTargetIP );
					}
				}
			  #else
				ASSERT( bATriggerTimeHasExpired );
			  #endif
 			}
		}
	}

	bool bDisplayPacketToScreen = false;

	if ( bPacketProcessed && bCaptureToScreen ) {
	  #ifdef DISPLAY_PACKET_NUMBERS
		(reinterpret_cast<COmniEngineContext*>(m_pRadiusContext))->IncrementPacketsDisplayedCount();
	  #endif
		bDisplayPacketToScreen = true;
	}

	return bDisplayPacketToScreen;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

void
CRadiusProcess::StopCapture( )
{
	size_t nItemCount = m_RadiusPacketItemMgr.GetPacketItemCount();
	
	m_nValidationNumber = 0;

	if ( nItemCount > 0 ) {
	  #ifndef TEST_NO_MONITORING
		m_RadiusPacketItemMgr.CleanupExpired();
	  #endif
		m_RadiusPacketItemMgr.RemoveAll();
	}

	m_TargetIPMap.StopAllCaseCaptures();
	m_ActiveCaseList.CloseAllPacketWriters();
}


// -----------------------------------------------------------------------------
//		RebuildActiveCaseList
// -----------------------------------------------------------------------------

void
CRadiusProcess::RebuildActiveCaseList(
	COptions* pCurrentOptions,
	COptions* pNewOptions )
{
	COmniEngineContext* pRadiusContext( (reinterpret_cast<COmniEngineContext*>(m_pRadiusContext)) );
	ASSERT( pRadiusContext );
	if ( !pRadiusContext ) return;

	CCaseOptionsList& theNewCaseOptionsList = pNewOptions->GetCaseOptionsList();
	const UInt16 nNewCaseCount = static_cast<UInt16>( theNewCaseOptionsList.GetCount() );

	CCaseOptionsList& theContextCaseOptionsList = pCurrentOptions->GetCaseOptionsList();
	int  nContextCaseCount = 0;
	bool bIsCapturing = false;

	ASSERT( *pCurrentOptions != *pNewOptions );

	const CPeekString strInitString = GetCaptureName();
	
	bIsCapturing = pRadiusContext->IsCapturing(); // true );

	theNewCaseOptionsList.Sort();

	if ( !bIsCapturing ) {
 		m_ActiveCaseList.Reset();

		*pCurrentOptions = *pNewOptions;  // Copy over all the options

		nContextCaseCount = static_cast<int>( theContextCaseOptionsList.GetCount() );

		for ( int i = 0; i < nContextCaseCount; i++ ) {
			CCaseOptions* theCase = theContextCaseOptionsList.GetAt( i );
			if ( theCase->IsActive() ){
				CActiveCase* pActiveCase = new CActiveCase( theCase, m_pRadiusContext );
				m_ActiveCaseList.Add( pActiveCase );
			}
		}
		return;
	}

	ASSERT( bIsCapturing );

	// Assertions for items that cannot be changed during a capture
	ASSERT ( pCurrentOptions->IsLogMsgsToFile() == pNewOptions->IsLogMsgsToFile() );
	ASSERT (pCurrentOptions->GetOutputRootDirectory() == pNewOptions->GetOutputRootDirectory() );
//	ASSERT (pCurrentOptions->GetFacility() == pNewOptions->GetFacility() );

	// Stop any cases that have expired trigger time
	bool bHasActiveCases = true;
	StopExpiredCases( bHasActiveCases, *pCurrentOptions );

	bool bUpdateActiveCaseMgrs = false;

	// Stop and delete any active cases that are no longer active.
	int nActiveCaseCount =  static_cast<int>( m_ActiveCaseList.GetCount() );
	for ( int i = nActiveCaseCount - 1; i >= 0; i-- ){
		CActiveCase* pActiveCase = m_ActiveCaseList.GetAt( i );
		CPeekString theCaseName = pActiveCase->GetCaseName();
		ASSERT ( theCaseName != L"" );
		CCaseOptions* pNewCaseOptions = theNewCaseOptionsList.Lookup( theCaseName );
		if ( (pNewCaseOptions == NULL) || (pNewCaseOptions->IsActive() == false) ) {
			pActiveCase->StopCapturing( kChangeReasonDeselectStop );
			bUpdateActiveCaseMgrs = true;
			pActiveCase = m_ActiveCaseList.DeleteAt( i );
		}
		else {
			UInt8 nTriggerTimeStatus = pNewCaseOptions->GetTriggerTimeStatus();
			if ( nTriggerTimeStatus != kTriggerTimeStatusActive ) {
				if ( nTriggerTimeStatus == kTriggerTimeStatusExpired ) {
					pActiveCase->StopCapturing( kChangeReasonTimeout );
				}
				else {
					ASSERT( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted );
					pActiveCase->StopCapturing( kChangeReasonDeselectStop );
				}
				bUpdateActiveCaseMgrs = true;
				pActiveCase = m_ActiveCaseList.DeleteAt( i );
			}
		}
	}

	if ( bUpdateActiveCaseMgrs ) {
		UpdateActiveCaseMgrs();
	}

	// Delete any context case options that no longer exist in the new context options
	nContextCaseCount = static_cast<int>( theContextCaseOptionsList.GetCount() );
	for ( int i = nContextCaseCount - 1; i >= 0; i-- ) {
		CCaseOptions* theCaseOptions = theContextCaseOptionsList.GetAt( i );
		CPeekString theCaseName = theCaseOptions->GetName();
		ASSERT ( theCaseName != L"" );
		CCaseOptions* pJNewCaseOptions = theNewCaseOptionsList.Lookup( theCaseName );
		if ( !pJNewCaseOptions ) {
			theContextCaseOptionsList.DeleteItem( theCaseName );
		}
	}

	// Update the current Context and Active Cases
	nContextCaseCount = static_cast<int>( theContextCaseOptionsList.GetCount() );
	for ( int i = 0; i < nContextCaseCount; i++ ) {
		CCaseOptions* theCaseOptions = theContextCaseOptionsList.GetAt( i );
		CPeekString theCaseName = theCaseOptions->GetName();
		ASSERT ( theCaseName != L"" );
		CCaseOptions* pNewCaseOptions = theNewCaseOptionsList.Lookup( theCaseName );
		if ( !pNewCaseOptions ) {
			ASSERT( 0 );
			continue;
		}
		CActiveCase* pActiveCase = m_ActiveCaseList.Find( theCaseName );  // Locate the old active case, if there was one
		if ( pActiveCase ) {
			pActiveCase->ProcessOptionsChange( pNewCaseOptions );
		}

		// Copy over the new case options
		*theCaseOptions = *pNewCaseOptions;

		// Start capturing on any active cases for which the trigger time just became active
		if ( pActiveCase && theCaseOptions->IsActive() ) {
			pActiveCase->SetCaseOptions( theCaseOptions );
			if ( !pActiveCase->IsCapturing() ) {
				if ( pActiveCase->HasTriggerTimeStartedYet() ) {
					ASSERT( pActiveCase->IsNewlyActivated() );
					(reinterpret_cast<COmniEngineContext*>(m_pRadiusContext))->CreateAnUniqueCaseCaptureId( pNewCaseOptions, theCaseOptions  );
					pActiveCase->StartCapturing( strInitString );
				}
			}
		}
	}

	// Set the global default options
	pCurrentOptions->SetGeneralOptions( pNewOptions->GetGeneralOptions() );
	pCurrentOptions->SetPortOptions( pNewOptions->GetPortOptions() );
	pCurrentOptions->SetCaptureOptions( pNewOptions->GetCaptureOptions() );

	// Now add any new context case options
	for ( UInt16 i = 0; i < nNewCaseCount; i++ ) {
		CCaseOptions* theNewCaseOptions = theNewCaseOptionsList.GetAt( i );
		CPeekString theCaseName = theNewCaseOptions->GetName();

		if ( !theContextCaseOptionsList.Lookup( theCaseName ) ) {
			CCaseOptions* theNewContextOptions = new CCaseOptions;//theContextCaseOptionsList.GetAt( nContextItem );
			*theNewContextOptions = *theNewCaseOptions;  // Copy over the case options
			theContextCaseOptionsList.Add( theNewContextOptions );
		}
	}

	// Sort the cases
	theContextCaseOptionsList.Sort();

	// Now add any new active cases
	nContextCaseCount = static_cast<int>( theContextCaseOptionsList.GetCount() );
	for ( UInt16 i = 0; i < nContextCaseCount; i++ ) {
		CCaseOptions* theCaseOptions = theContextCaseOptionsList.GetAt( i );
		CPeekString theCaseName = theCaseOptions->GetName();
		
		if ( theCaseOptions->IsActive() && !m_ActiveCaseList.Find( theCaseName ) ) {
			CActiveCase* pNewActiveCase = new CActiveCase( theCaseOptions, m_pRadiusContext );
			size_t nPos = m_ActiveCaseList.Add( pNewActiveCase );
			ASSERT ( nPos == m_ActiveCaseList.GetCount() - 1 );
			CActiveCase* pNewlyAddedActiveCase = m_ActiveCaseList.GetAt( nPos );
			CCaseOptions* pNewCaseOptions = theNewCaseOptionsList.Lookup( theCaseName );
			if ( !pNewCaseOptions ) {
				ASSERT( 0 );
				continue;
			}
			(reinterpret_cast<COmniEngineContext*>(m_pRadiusContext))->CreateAnUniqueCaseCaptureId( pNewCaseOptions, theCaseOptions  );
			pNewlyAddedActiveCase->StartCapturing( strInitString );
		}
	}

	// Set a flag if there are active cases not yet started due to trigger time
	m_ActiveCaseList.InitializeUnstartedActiveCasesFlag();

	// Now update the Active Case Managers to incorporate any changes
	UpdateActiveCaseMgrs();

  #ifdef _DEBUG
	// Now perform a sanity check
	const int nTestActiveCaseCount =  static_cast<int>( m_ActiveCaseList.GetCount() );
	for ( int i = 0; i < nTestActiveCaseCount; i++ ){
		CActiveCase* pActiveCase = m_ActiveCaseList.GetAt( i );
		pActiveCase->DebugValidateCase();
		CPeekString theCaseName = pActiveCase->GetCaseName();
		ASSERT ( theCaseName != L"" );
		CCaseOptions* pNewCaseOptions = theNewCaseOptionsList.Lookup( theCaseName );
		ASSERT ( pNewCaseOptions && pNewCaseOptions->IsActive() && pNewCaseOptions->AreEnabledUsers() );
	}
  #endif

	return;
}


// -----------------------------------------------------------------------------
//		UpdateOptions
// -----------------------------------------------------------------------------

void
CRadiusProcess::UpdateOptions(
	COptions* pCurrentOptions,
	COptions* pNewOptions )
{
	ASSERT( m_pRadiusContext );

	// Update existing options to be new options
	RebuildActiveCaseList( pCurrentOptions, pNewOptions );

	pCurrentOptions->RebuildActivePortsList();
	pCurrentOptions->RebuildAllActiveUsersList();
	pCurrentOptions->ResetActiveTimeSpan();

}


// -----------------------------------------------------------------------------
//		StopExpiredCases
// -----------------------------------------------------------------------------

bool
CRadiusProcess::StopExpiredCases(
	bool&		bOutHasActiveCases,
	COptions&	theOptions ) 
{
	bool bExpiredCaseNeedsToStop = m_ActiveCaseList.CheckStopExpiredCases( bOutHasActiveCases );
	if ( !bExpiredCaseNeedsToStop ) {
		return false;
	}

	bOutHasActiveCases = m_TargetIPMap.StopExpiredTargetIPActiveCaseMgrs();
	bool bExpiredCaseStopped = m_ActiveCaseList.StopExpiredCases( bOutHasActiveCases );

	theOptions.RebuildAllActiveUsersList();
	theOptions.ResetActiveTimeSpan();
	m_RadiusPacketItemMgr.CleanupItemsForUnusedNames( theOptions );

	return bExpiredCaseStopped;
}

// -----------------------------------------------------------------------------
//		StartNewlyActiveCases
// -----------------------------------------------------------------------------

bool
CRadiusProcess::StartNewlyActiveCases(
	bool& bOutHasActiveCases ) 
{
	if ( m_ActiveCaseList.CheckNewlyActiveCases() == false ) {
		return false;
	}
	const CPeekString strInitString = GetCaptureName();

	bool bNewCaseStarted = m_ActiveCaseList.StartNewlyActiveCases( strInitString, bOutHasActiveCases );
	ASSERT( bNewCaseStarted );
	if ( bNewCaseStarted ) {
		m_TargetIPMap.StartNewTargetIPActiveCaseMgrs( m_ActiveCaseList );
	}
	return bNewCaseStarted;
}


#ifndef TEST_NO_MONITORING
// -----------------------------------------------------------------------------
//		Monitor
// -----------------------------------------------------------------------------

void
CRadiusProcess::Monitor(
	COptions& inOptions ) {

	bool bHasActiveCases = true;

	StopExpiredCases( bHasActiveCases, inOptions );

#ifndef TEST_NO_MONITORING
	m_RadiusPacketItemMgr.CleanupExpired();
#endif

	StartNewlyActiveCases( bHasActiveCases );

	if ( inOptions.IsSaveAgeSelected() ) {
		m_ActiveCaseList.MonitorFiles( &inOptions );
	}
}
#endif


// -----------------------------------------------------------------------------
//		Initialize
// -----------------------------------------------------------------------------

void
CRadiusProcess::Initialize(
	COptions& inContextOptions ) 
{
	m_TargetIPMap.Initialize();
	m_RadiusPacketItemMgr.Initialize( inContextOptions.GetActiveUsersList() );

	m_nValidationNumber = 0;

	bool bHasActiveCases = true;
	StopExpiredCases( bHasActiveCases, inContextOptions );
#ifndef TEST_NO_MONITORING
	m_RadiusPacketItemMgr.CleanupExpired();
#endif

	const CPeekString strInitString = GetCaptureName();

	m_ActiveCaseList.InitializeWriters( strInitString );
}

#ifdef PACKET_WRITER_ERROR_HANDLING
// -----------------------------------------------------------------------------
//		FailureAllertAllTargetIPs
// -----------------------------------------------------------------------------

bool
CRadiusProcess::FailureAllertAllTargetIPs( const CPeekString& strCaseName )
{
	return m_TargetIPMap.FailureAllertAllTargetIPs( strCaseName );
}
#endif // PACKET_WRITER_ERROR_HANDLING

// -----------------------------------------------------------------------------
//		ProcessTargetCandidate
// -----------------------------------------------------------------------------

bool
CRadiusProcess::ProcessTargetCandidate(
	CRadiusPacketItem&	theRadiusPacketItem,
	COptions&			inOptions )
{
	bool bPacketItemUserIsAnActiveTarget = false;

	POSITION	myPos = m_RadiusPacketItemMgr.GetFirstPosition();
	POSITION	theCurrentPos;

	if ( !theRadiusPacketItem.IsRadiusOfInterest() ) {
		return false;
	}

	while ( myPos ) {
		theCurrentPos = myPos;
		CRadiusPacketItem& theListPacketItem = m_RadiusPacketItemMgr.GetNextRadiusPacktetItem( myPos );

		if ( theListPacketItem.IsMatch( theRadiusPacketItem ) ) {
			theListPacketItem.Update( theRadiusPacketItem );
			theRadiusPacketItem = theListPacketItem;

			bPacketItemUserIsAnActiveTarget = IsPacketItemUserAnActiveTarget( theRadiusPacketItem, inOptions );
			if ( bPacketItemUserIsAnActiveTarget ) {			
				if ( theListPacketItem.RadiusTransactionComplete() ){
					m_RadiusPacketItemMgr.Remove( theCurrentPos );
				}
				else {
					m_RadiusPacketItemMgr.SetAt( theCurrentPos, theListPacketItem );
				}
			}
			else {
				m_RadiusPacketItemMgr.Remove( theCurrentPos );
			}

			return bPacketItemUserIsAnActiveTarget;
		}
	}

	bPacketItemUserIsAnActiveTarget = IsPacketItemUserAnActiveTarget( theRadiusPacketItem, inOptions );
	if ( bPacketItemUserIsAnActiveTarget ) {
		m_RadiusPacketItemMgr.Add( theRadiusPacketItem );
	}

	return bPacketItemUserIsAnActiveTarget;
}


// -----------------------------------------------------------------------------
//		IsPacketItemUserAnActiveTarget
// -----------------------------------------------------------------------------

bool
CRadiusProcess::IsPacketItemUserAnActiveTarget(
	CRadiusPacketItem&	thePacketItem,
	COptions&			inOptions )
{
	bool bUserIsATarget = false;

	CPeekString theUserName = thePacketItem.GetRadiusUserName();
	if ( !theUserName.IsEmpty() ) {
		bUserIsATarget = inOptions.IsUserAnActiveTarget( theUserName );
	}

	return bUserIsATarget;
}


// -----------------------------------------------------------------------------
//		SetActiveTargetIP
// -----------------------------------------------------------------------------

bool
CRadiusProcess::SetActiveTargetIP( 
	COptions&			inOptions,
	CRadiusPacketItem&	thePacketItem,
	CTargetIP*			pTargetIP,
	bool&				bOutCurrentlyMonitoringIP ) 
{

	if ( !pTargetIP ) {
		ASSERT( 0 );
		return false;
	}

	bool bTargetIPIsActive = false;
	
	bOutCurrentlyMonitoringIP = false;

	const UInt32  uPacketItemIPAddress = thePacketItem.GetFramedIPAddr();
	const CPeekString strPacketItemUserName = thePacketItem.GetRadiusUserName();

	ASSERT( strPacketItemUserName.GetLength() > 0 );

	CPeekString strTargetIPUserName;

	if ( uPacketItemIPAddress > 0 ) {
		bOutCurrentlyMonitoringIP = m_TargetIPMap.Find( uPacketItemIPAddress, *pTargetIP );
	}

	if ( !bOutCurrentlyMonitoringIP ) {
		pTargetIP->SetIP( uPacketItemIPAddress );
		pTargetIP->SetUserName( strPacketItemUserName );
	}

	bool bHasActiveCases = false;
	bool bExpiredCaseStopped = StopExpiredCases( bHasActiveCases, inOptions );

  #ifdef _DEBUG
	if ( bExpiredCaseStopped && !bHasActiveCases ) {
		ASSERT( inOptions.IsUserAnActiveTarget( strPacketItemUserName ) == true );
	}
  #endif

	bool bNewCaseStarted = StartNewlyActiveCases( bHasActiveCases );

	if ( !bHasActiveCases ) {
  #ifdef _DEBUG
		if ( uPacketItemIPAddress > 0 ) {
			ASSERT( m_TargetIPMap.Find( uPacketItemIPAddress, *pTargetIP ) == false );
		}
  #endif
		ASSERT( !bTargetIPIsActive );
		return bTargetIPIsActive;
	}

	bool bHasChanged = false;
	if ( bExpiredCaseStopped || bNewCaseStarted ) {
		bHasChanged = true;
		bOutCurrentlyMonitoringIP = false;
		if ( uPacketItemIPAddress > 0 ) {
			bOutCurrentlyMonitoringIP = m_TargetIPMap.Find( uPacketItemIPAddress, *pTargetIP );
		}
	}

	bTargetIPIsActive = bOutCurrentlyMonitoringIP;
	if ( bTargetIPIsActive ) {
		if ( bHasChanged ) {
			bool bChangedTargetIPActiveCases = false;
			bTargetIPIsActive = pTargetIP->UpdateActiveCases( m_ActiveCaseList, bChangedTargetIPActiveCases );
		}
	}
	else {
		bool bNewCaptureStarted = false;
		const CPeekString strInitString = GetCaptureName();
		bTargetIPIsActive = pTargetIP->RebuildActiveCaseMgr( m_ActiveCaseList, thePacketItem, strInitString, bNewCaptureStarted );
	}

	if ( !bTargetIPIsActive ) {
		if ( bOutCurrentlyMonitoringIP ) {
			ASSERT( uPacketItemIPAddress > 0 );
			bOutCurrentlyMonitoringIP = m_TargetIPMap.Find( uPacketItemIPAddress, *pTargetIP );
			if ( bOutCurrentlyMonitoringIP ) {
				m_TargetIPMap.Remove( uPacketItemIPAddress );
			}
			bOutCurrentlyMonitoringIP = false;
		}
		ASSERT( !bTargetIPIsActive );
		return bTargetIPIsActive;
	}

	if ( bOutCurrentlyMonitoringIP && bHasChanged ) {
		m_TargetIPMap.Set( *pTargetIP );
	}

	ASSERT( bTargetIPIsActive );
	return bTargetIPIsActive;
}


// -----------------------------------------------------------------------------
//		ProcessRadiusNameChanged
// -----------------------------------------------------------------------------

bool
CRadiusProcess::ProcessRadiusNameChanged(
	COptions&			inOptions,
	CRadiusPacketItem&	thePacketItem,
	CTargetIP&			theTargetIP,
	bool&				bOutCurrentlyMonitoringIP ) 
{
	const UInt32		uMonitoring = thePacketItem.GetMonitoring();
	const CPeekString	strPacketItemUserName = thePacketItem.GetRadiusUserName();
	const bool			bPacketItemUserIsATarget = inOptions.IsUserAnActiveTarget( strPacketItemUserName );
	CPeekOutString		strStopMessage;

#ifdef _DEBUG
	ASSERT( thePacketItem.HasFramedIPAddr() );
	CPeekString strTargetIPUserName = theTargetIP.GetUserName();

	// We assume the name has changed if this process has been called.  
	// This validates the assumption in debug mode:
	if ( strPacketItemUserName.CompareNoCase( strTargetIPUserName ) == 0 ) {
		ASSERT( 0 );
		return true;
	}
#endif

	if( uMonitoring == MONITORING_START || uMonitoring == MONITORING_START_NOREPORT	) {
		if ( bPacketItemUserIsATarget ) {
			strStopMessage << L"IP address reassigned to " << strPacketItemUserName << L" - monitoring will stop.";
			theTargetIP.LogTargetIPMessage( strStopMessage );
			StopIPCapture( theTargetIP, false );
			bool bTargetIPIsActive = SetActiveTargetIP( inOptions, thePacketItem, &theTargetIP, bOutCurrentlyMonitoringIP );
			if ( !bTargetIPIsActive ) {
				return false;
			}
		}
		else {  // !bPacketItemUserIsATarget
			strStopMessage << L"IP address reassigned to a non-target user - monitoring will stop.";
//			strStopMessage = _T( "IP address reassigned to a non-target user - monitoring will stop." );
			theTargetIP.LogTargetIPMessage( strStopMessage );
			StopIPCapture( theTargetIP, false );
		}
	}
	else if ( uMonitoring == MONITORING_STOP ) {
		if ( bPacketItemUserIsATarget ) {
			strStopMessage << L"a RADIUS stop has been detected on " << strPacketItemUserName << L" at this IP address - monitoring will stop.";
			theTargetIP.LogTargetIPMessage( strStopMessage );
			StopIPCapture( theTargetIP, false );
			bool bTargetIPIsActive = SetActiveTargetIP( inOptions, thePacketItem, &theTargetIP, bOutCurrentlyMonitoringIP );
			if ( !bTargetIPIsActive ) {
				return false;
			}
		}
		else {  // !bPacketItemUserIsATarget
			strStopMessage << L"a RADIUS stop has been detected on a non-target at this IP address - monitoring will stop.";
//			strStopMessage = _T( "a RADIUS stop has been detected on a non-target at this IP address - monitoring will stop." );
			theTargetIP.LogTargetIPMessage( strStopMessage );
			StopIPCapture( theTargetIP, false );
		}
	}
	else{
		ASSERT( 0 );
	}
	
	return bPacketItemUserIsATarget;
}


CPeekString
CRadiusProcess::GetCaptureName()
{
	return (reinterpret_cast<COmniEngineContext*>(m_pRadiusContext))->GetCaptureName();
}