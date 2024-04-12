// ============================================================================
// TargetMap.cpp:
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "TargetIP.h"

// ============================================================================
//		CTargetIP
// ============================================================================

// -----------------------------------------------------------------------------
//		FormatUserIPString
// -----------------------------------------------------------------------------

CPeekString
CTargetIP::FormatUserIPString()
{
	ASSERT( m_uIPAddress != 0 );

	UInt8* pIPAddress = reinterpret_cast<UInt8*>( &m_uIPAddress );

	CPeekOutString theString;
	theString << m_strUserName << L" @ " << pIPAddress[3] << L"." << pIPAddress[2] << L"." << pIPAddress[1] << L"." << pIPAddress[0];

	return theString;
}


// -----------------------------------------------------------------------------
//		ProcessIPPacket
// -----------------------------------------------------------------------------

bool
CTargetIP::ProcessIPPacket( 
	CPacket& inPacket,
	bool& bTriggerTimeExpired, 
	UInt32 nValidationNumber, 
	bool bWritePacket )
{
	bTriggerTimeExpired = false;

	bool bPacketProcessed = false;

	const int nCount = static_cast<int>( m_TargetIPActiveCaseMgr.GetCount() );
	if ( nCount == 0 ) {
		ASSERT( 0 );
		return false;
	}

	for ( int i = nCount - 1; i >= 0; i-- ) {
		CTargetIPActiveCase& theTargetIPActiveCase = m_TargetIPActiveCaseMgr.GetAt( i );
		if ( !theTargetIPActiveCase.IsValid() ) {
			ASSERT( 0 );
			continue;
		}
		ASSERT( theTargetIPActiveCase.IsCapturing() );

#ifdef _DEBUG
		UInt8 nTriggerTimeStatus = GetTriggerTimeStatus( theTargetIPActiveCase, i );
#else
		UInt8 nTriggerTimeStatus = GetTriggerTimeStatus( theTargetIPActiveCase );
#endif
		if ( nTriggerTimeStatus != kTriggerTimeStatusActive ) {
			if ( nTriggerTimeStatus == kTriggerTimeStatusExpired ) {
				bTriggerTimeExpired = true;
			}
			continue;
		}

		ASSERT( nTriggerTimeStatus == kTriggerTimeStatusActive );
#ifndef PACKET_WRITER_ERROR_HANDLING
		if ( bWritePacket && theTargetIPActiveCase.IsCaptureToFile() ) {
			theTargetIPActiveCase.ProcessIPPacket( inPacket, nValidationNumber );
		}
		bPacketProcessed = true;
	}
#else
		if ( bWritePacket && theTargetIPActiveCase.IsCaptureToFile() ) {
			if ( theTargetIPActiveCase.ProcessIPPacket( inPacket, nValidationNumber ) ) {
				bPacketProcessed = true;
			}
			else {
				m_TargetIPActiveCaseMgr.RemoveAt( i );
			}
		}
	}

//	if ( !bPacketProcessed ) {
//		if ( AllCasesAreStopped() ) {
//			theTargetIP.
//		}
//		else if ( tAllTargetIPActiveCaseTimesHaveExpired() ) {
//			ASSERT( uIPAddress == theTargetIP.GetIP() );
//			StopIPCapture( theTargetIP );
//		}
//	}
#endif // #ifndef PACKET_WRITER_ERROR_HANDLING

	return bPacketProcessed;
}

// -----------------------------------------------------------------------------
//		ProcessRadiusPacket
// -----------------------------------------------------------------------------

bool
CTargetIP::ProcessRadiusPacket( 
	CPacket& inPacket )
{
	bool		 bProcessSuccess = true;

	const int	 nNumActiveCases = static_cast<int>( m_TargetIPActiveCaseMgr.GetCount() );

	for ( int i = nNumActiveCases - 1; i >= 0; i-- ) {
		CTargetIPActiveCase& theTargetIPActiveCase = m_TargetIPActiveCaseMgr.GetAt( i );
		if ( !theTargetIPActiveCase.IsValid() ) {
			ASSERT( 0 );
			continue;
		}

#ifdef _DEBUG
		ASSERT( theTargetIPActiveCase.IsCapturing() );
		UInt8 nTriggerTimeStatus = GetTriggerTimeStatus( theTargetIPActiveCase, i );
		ASSERT( nTriggerTimeStatus == kTriggerTimeStatusActive );
#endif
		if ( !theTargetIPActiveCase.IsActive() ) {
			continue;
		}

		ASSERT( theTargetIPActiveCase.IsActive() );

		if ( theTargetIPActiveCase.IsCaptureToFile() ) {
			if ( !theTargetIPActiveCase.ProcessRadiusPacket( inPacket ) ) {
				bProcessSuccess = false;
			}				
		}
	}

	return bProcessSuccess;
}


// -----------------------------------------------------------------------------
//		DetermineCaptureOptions
// -----------------------------------------------------------------------------

void
CTargetIP::DetermineCaptureOptions( CCaptureOptions& theCaptureOptions )
{
	theCaptureOptions.Initialize();

	const int	 nNumActiveCases = static_cast<int>( m_TargetIPActiveCaseMgr.GetCount() );
	for ( int i = nNumActiveCases - 1; i >= 0; i-- ) {
		CTargetIPActiveCase& theTargetIPActiveCase = m_TargetIPActiveCaseMgr.GetAt( i );
		if ( !theTargetIPActiveCase.IsValid() ) {
			ASSERT( 0 );
			continue;
		}
		theTargetIPActiveCase.IncludeCaptureOptions( theCaptureOptions );
	}
}


// ============================================================================
//		CTargetIPMap
// ============================================================================

// -----------------------------------------------------------------------------
//		StartNewTargetIPActiveCaseMgrs
// -----------------------------------------------------------------------------

bool
CTargetIPMap::StartNewTargetIPActiveCaseMgrs( CActiveCaseList& ActiveCaseList ) 
{
	POSITION	pos = GetStartPosition();
	UInt32		uIPAddress;
	CTargetIP	theTargetIP;
	bool		bNewStartsAdded = false;

	while ( pos != NULL ) {
		GetNextAssoc( pos, uIPAddress, theTargetIP );

		ASSERT( theTargetIP.GetIP() != 0 && theTargetIP.GetIP() == uIPAddress );
		bool bNewTargetIPActiveCaseStarted = theTargetIP.StartNewTargetIPActiveCases( ActiveCaseList );
		if ( bNewTargetIPActiveCaseStarted ) {
			Set( theTargetIP );
			bNewStartsAdded = true;
		}
	}

	return bNewStartsAdded;
}

// -----------------------------------------------------------------------------
//		UpdateTargetIPActiveCaseMgrs
// -----------------------------------------------------------------------------

void
CTargetIPMap::UpdateTargetIPActiveCaseMgrs( CActiveCaseList& theActiveCaseList ) 
{
	POSITION	pos = GetStartPosition();
	UInt32		uIPAddress;
	CTargetIP	theTargetIP;
  #ifdef _DEBUG
	bool		bHasActiveTarget = false;
  #endif
	while ( pos != NULL ) {
		GetNextAssoc( pos, uIPAddress, theTargetIP );

		ASSERT( theTargetIP.GetIP() != 0 && theTargetIP.GetIP() == uIPAddress );

		bool bChangeMade = false;
		bool bActive = theTargetIP.UpdateActiveCases( theActiveCaseList, bChangeMade );
		if ( !bActive ) {
			Remove( uIPAddress );
		}
		else {
			if ( bChangeMade ) {
				Set( theTargetIP );
			}
			#ifdef _DEBUG
			 bHasActiveTarget = true;
			#endif
		}
	}

	ASSERT( bHasActiveTarget ? (GetCount() > 0) : (GetCount() == 0) );
}

// -----------------------------------------------------------------------------
//		StopExpiredTargetIPActiveCaseMgrs
// -----------------------------------------------------------------------------

bool
CTargetIPMap::StopExpiredTargetIPActiveCaseMgrs() 
{
	POSITION	pos = GetStartPosition();
	UInt32		uIPAddress;
	CTargetIP	theTargetIP;
	bool		bHasActiveTarget = false;

	while ( pos != NULL ) {
		GetNextAssoc( pos, uIPAddress, theTargetIP );

		ASSERT( theTargetIP.GetIP() != 0 && theTargetIP.GetIP() == uIPAddress );

		bool bChangeMade = false;
		bool bActive = theTargetIP.StopExpiredTargetIPActiveCases( bChangeMade );
		if ( !bActive ) {
			ASSERT( bChangeMade );
			Remove( uIPAddress );
		}
		else {
			if ( bChangeMade ) {
				Set( theTargetIP );
			}
			bHasActiveTarget = true;
		}
	}

  #ifdef _DEBUG
	if ( !bHasActiveTarget ) {
		ASSERT( GetCount() == 0 );
	}
  #endif

	return bHasActiveTarget;
}

// -----------------------------------------------------------------------------
//		StopAllCaseCaptures
// -----------------------------------------------------------------------------

void
CTargetIPMap::StopAllCaseCaptures()
{
	POSITION	pos = GetStartPosition();
	UInt32		uIPAddress;
	CTargetIP	theTargetIP;

	while ( pos != NULL ) {
		GetNextAssoc( pos, uIPAddress, theTargetIP );
		theTargetIP.StopCapture( true );
	}

	RemoveAll();
}

#ifdef PACKET_WRITER_ERROR_HANDLING
// -----------------------------------------------------------------------------
//		FailureAllertAllTargetIPs
// -----------------------------------------------------------------------------

bool
CTargetIPMap::FailureAllertAllTargetIPs( const CPeekString& inCaseName )
{
	POSITION	pos = GetStartPosition();
	UInt32		uIPAddress;
	CTargetIP	theTargetIP;

	bool bActiveIPs = false;

	while ( pos != NULL ) {
		GetNextAssoc( pos, uIPAddress, theTargetIP );
		if ( theTargetIP.ProcessFailureAllert( inCaseName ) ) {
			bActiveIPs = true;
		}
		else {
			Remove( uIPAddress );
		}
	}
	return bActiveIPs;
}
#endif // PACKET_WRITER_ERROR_HANDLING
