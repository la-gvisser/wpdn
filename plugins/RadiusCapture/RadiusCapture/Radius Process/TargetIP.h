// ============================================================================
// TargetIP.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "ActiveCase.h"

// ============================================================================
//		CTargetIP
// ============================================================================

class CTargetIP
{

public:
	CTargetIP::CTargetIP()
		: m_uIPAddress( 0 )
	  {}
	~CTargetIP() {}

	CTargetIP &		operator= ( CTargetIP& inOriginal ) { 
		m_uIPAddress = inOriginal.m_uIPAddress;
		m_strUserName = inOriginal.m_strUserName;
		m_TargetIPActiveCaseMgr = inOriginal.m_TargetIPActiveCaseMgr;
		return *this;
	}

	CPeekString		FormatUserIPString();
	void			DetermineCaptureOptions( CCaptureOptions& theCaptureOptions );
	bool			ProcessIPPacket( CPacket& inPacket, bool& bTriggerTimeExpired,
						UInt32 nValidationNumber, bool bWritePacket );
	bool			ProcessRadiusPacket( CPacket& inPacket );

	const CPeekString&	GetUserName() {
		return m_strUserName;
	}

	void	SetUserName( const CPeekString& strName ) {
		m_strUserName = strName;
	}
	void	SetIP( UInt32 uIP ) {
		m_uIPAddress = uIP;
	}
	UInt32	GetIP() {
		return m_uIPAddress;
	}
	bool	RebuildActiveCaseMgr( CActiveCaseList& ActiveCaseList, CRadiusPacketItem& thePacketItem, 
										const CPeekString& strInitString, bool& bNewCaptureStarted ) {
		ASSERT( thePacketItem.GetRadiusUserName() == m_strUserName );
		return m_TargetIPActiveCaseMgr.RebuildList( ActiveCaseList, thePacketItem, strInitString, bNewCaptureStarted );
	}
	bool	StartNewTargetIPActiveCases( CActiveCaseList& ActiveCaseList ) {
		return m_TargetIPActiveCaseMgr.RefreshListForNewlyStarted( ActiveCaseList, m_strUserName );
	}
	bool	UpdateActiveCases( CActiveCaseList& ActiveCaseList, bool& bChangeMade ) {
		return m_TargetIPActiveCaseMgr.RefreshList( ActiveCaseList, m_strUserName, FormatUserIPString(), bChangeMade );
	}
	bool	StopExpiredTargetIPActiveCases( bool& bChangeMade ) {
		return m_TargetIPActiveCaseMgr.StopExpiredTargetIPActiveCases( m_strUserName, m_uIPAddress, bChangeMade );
	}
	void	StartCapture( const bool bDisplayPacketNumber = false ) {
		m_TargetIPActiveCaseMgr.ProcessStartContinue( FormatUserIPString(), bDisplayPacketNumber );
//		m_TargetIPActiveCaseMgr.ProcessStartContinue( m_strUserName, FormatUserIPString(), bDisplayPacketNumber );
	}
	void	StopCapture( bool bEndCapture = false, const bool bDisplayPacketNumber = false ) {
		CPeekString strMessage = _T("");
		if ( !bEndCapture ) {
			strMessage = _T("Stop Monitoring ") + FormatUserIPString();
		}
		m_TargetIPActiveCaseMgr.StopAllCaseCaptures( strMessage, bDisplayPacketNumber );
	}
//	void	LogNonMonitoringStopMsg( const bool bDisplayPacketNumber = false ) {
//		CPeekString strMessage = _T("RADIUS Stop on target not being monitored ") + FormatUserIPString();
//		m_TargetIPActiveCaseMgr.LogMessageToActiveCases( strMessage, bDisplayPacketNumber );
//	}
	void	ContinueCapture( const bool bLogMessages, bool& bOutNewlyActivatedCaseStarted,
						const bool bDisplayPacketNumber = false, const bool bDisplayIfCapturing = true ) {
		bOutNewlyActivatedCaseStarted = false;
		ASSERT( m_strUserName != L"" );
		if ( bLogMessages ) {
//			if ( m_TargetIPActiveCaseMgr.ProcessStartContinue( m_strUserName, FormatUserIPString(), bDisplayPacketNumber, bDisplayIfCapturing ) ) {
			if ( m_TargetIPActiveCaseMgr.ProcessStartContinue( FormatUserIPString(), bDisplayPacketNumber, bDisplayIfCapturing ) ) {
				bOutNewlyActivatedCaseStarted = true;
			}
		}
	}
	bool	IsAnActiveCaseUnstarted() {
		return m_TargetIPActiveCaseMgr.IsAnActiveCaseUnstarted();
	}
	bool	AllTargetIPActiveCaseTimesHaveExpired() {
		return m_TargetIPActiveCaseMgr.AllActiveCaseTimesHaveExpired();
	}
	void	LogTargetIPMessage( const CPeekString& inMessage ) {
		CPeekString strMessage = FormatUserIPString() + _T(" - ") + inMessage;
		m_TargetIPActiveCaseMgr.LogMessageToActiveCases( strMessage );
	}

  #ifdef PACKET_WRITER_ERROR_HANDLING
	bool	ProcessFailureAllert( const CPeekString& inCaseName ) {
		return m_TargetIPActiveCaseMgr.ProcessFailureAllert( inCaseName );
	}
	bool	AllCasesAreStopped() {
		return m_TargetIPActiveCaseMgr.AllCasesAreStopped();
	}
  #endif // PACKET_WRITER_ERROR_HANDLING

protected:
#ifdef _DEBUG
	UInt8	GetTriggerTimeStatus( CTargetIPActiveCase& inTargetIPActiveCase, int i ) {
#else
	UInt8	GetTriggerTimeStatus( CTargetIPActiveCase& inTargetIPActiveCase ) {
#endif
		ASSERT( inTargetIPActiveCase.IsValid() );
		ASSERT ( i < static_cast<int>( m_TargetIPActiveCaseMgr.GetCount() ) );
		UInt8 nTriggerTimeStatus = inTargetIPActiveCase.GetTriggerTimeStatus();
		if ( nTriggerTimeStatus != kTriggerTimeStatusActive ) {
			if ( nTriggerTimeStatus == kTriggerTimeStatusExpired ) {
			}
		  #ifdef _DEBUG
			else {
				ASSERT( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted );
			}
		  #endif
		}
		return nTriggerTimeStatus;
	}

protected:
	UInt32					m_uIPAddress;
	CPeekString				m_strUserName;
	CTargetIPActiveCaseMgr	m_TargetIPActiveCaseMgr;

};


// ============================================================================
//		CTargetMap
// ============================================================================

class CTargetIPMap
	: public CMap<UInt32, UInt32, CTargetIP, CTargetIP& >
{

public:
	bool StartNewTargetIPActiveCaseMgrs( CActiveCaseList& ActiveCaseList );
	void UpdateTargetIPActiveCaseMgrs( CActiveCaseList& theActiveCaseList );
	bool StopExpiredTargetIPActiveCaseMgrs();
	void StopAllCaseCaptures();

	bool Find( const UInt32 uIPAddress, CTargetIP& outTargetIP ) {
		return ( Lookup( uIPAddress, outTargetIP ) == TRUE );
	}
	void Initialize() {
		RemoveAll();
	}
	bool Remove( const UInt32 uIPAddress ) { 
		return ( RemoveKey( uIPAddress ) == TRUE ); 
	}
	void Set( CTargetIP& theTargetIP ) {
		SetAt( theTargetIP.GetIP(), theTargetIP );
	}
	void StartIPCapture( CTargetIP& theTargetIP, const bool bDisplayPacketNumber = false ) {
		if ( bDisplayPacketNumber ) {
			theTargetIP.StartCapture( bDisplayPacketNumber );
		}
		Set( theTargetIP );
	}
	void StopIPCapture( CTargetIP& theTargetIP, const bool bDisplayPacketNumber = false ) {
		if ( theTargetIP.GetUserName().IsEmpty() || theTargetIP.GetIP() == 0 ) {
			ASSERT( 0 );
			return;
		}
		CTargetIP IPDummy;
		if ( !Find( theTargetIP.GetIP(), IPDummy ) ) {
//			theTargetIP.LogNonMonitoringStopMsg( bDisplayPacketNumber );
			return;
		}
		ASSERT( theTargetIP.GetIP() == IPDummy.GetIP() 
			&& theTargetIP.GetUserName() == IPDummy.GetUserName() );
		theTargetIP.StopCapture( false, bDisplayPacketNumber );
		Remove( theTargetIP.GetIP() );
	}
  #ifdef PACKET_WRITER_ERROR_HANDLING
	bool FailureAllertAllTargetIPs( const CPeekString& inCaseName );
  #endif
};
