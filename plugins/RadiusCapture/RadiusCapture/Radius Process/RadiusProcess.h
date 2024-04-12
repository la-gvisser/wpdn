// ============================================================================
// RadiusProcess.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.


#pragma once

#include "RadiusPacketItem.h"
#include "Options.h"
#include "Packet.h"
#include "RadiusPacketItemMgr.h"
#include "TargetIP.h"
#include "ActiveCase.h"
#include "RadiusContextInterface.h"

class CRadiusContext;


// ============================================================================
//		CRadiusProcess
// ============================================================================

class CRadiusProcess
{

public:
	CRadiusProcess();
	~CRadiusProcess();

	bool	ProcessPacket( CPacket& inPacket, COptions&	inOptions );
	void	UpdateOptions( COptions* pCurrentOptions, COptions* pNewOptions );

	void	StopCapture();
	void	Initialize( COptions& inContextOptions );
	bool	IsThereAnActiveCase() { return m_ActiveCaseList.IsThereAnActiveCase(); }

  #ifndef TEST_NO_MONITORING
	void	Monitor( COptions& inOptions );
  #endif

  #ifdef PACKET_WRITER_ERROR_HANDLING
	bool    FailureAllertAllTargetIPs( const CPeekString& strCaseName );
  #endif

	void    Init( IRadiusContext* pRadiusContext ) { 
		ASSERT( pRadiusContext );
		m_pRadiusContext = pRadiusContext;
	}

protected:
	CPeekString	GetCaptureName();

	bool	ProcessRadiusPacket( COptions& inOptions, CRadiusPacketItem& Radius, CPacket& inPacket );
	void	StartContinueTargetCapture( CRadiusPacketItem& thePacketItem, CTargetIP& inTargetIP, bool bCurrentlyCapturing, bool bDisplay );
	bool	ProcessTargetCandidate( CRadiusPacketItem& theRadiusPacketItem, COptions& inOptions );
	bool	ProcessIPPacket( COptions& inOptions, CPacket& inPacket, CRadiusPacketItem& thePacketItem );
	bool	SetActiveTargetIP( COptions& inOptions, CRadiusPacketItem& thePacketItem, CTargetIP* pTargetIP, bool& bOutCurrentlyMonitoringIP );
	bool	StartNewlyActiveCases( bool& bOutHasActiveCases );
	bool	StopExpiredCases( bool& bOutHasActiveCases, COptions& inOptions );

	void	RebuildActiveCaseList( COptions* pCurrentOptions, COptions* pNewOptions );

	bool	IsPacketItemUserAnActiveTarget( CRadiusPacketItem& thePacketItem, COptions& inOptions );

	bool	ProcessRadiusNameChanged( COptions& inOptions, CRadiusPacketItem& thePacketItem,
				CTargetIP& theTargetIP, bool& bOutCurrentlyMonitoringIP );
	bool	ProcessRadiusPktToTargetIP( CPacket& inPacket, CTargetIP* pTargetIP ) {
		return pTargetIP->ProcessRadiusPacket( inPacket );
	}

	void	StartIPCapture( CTargetIP& theTargetIP, const bool bDisplayPacketNumber = false ) {
		m_TargetIPMap.StartIPCapture( theTargetIP, bDisplayPacketNumber );
	}
	void	StopIPCapture ( CTargetIP& theTargetIP, const bool bDisplayPacketNumber = false ) {
			m_TargetIPMap.StopIPCapture( theTargetIP, bDisplayPacketNumber );
	}
	void	UpdateActiveCaseMgrs() {
		m_TargetIPMap.UpdateTargetIPActiveCaseMgrs( m_ActiveCaseList );
	}

	bool	IsCurrentlyMonitoringIP( UInt32 uIPAddress ) {
		CTargetIP  MapTargetIP;
		return m_TargetIPMap.Find( uIPAddress, MapTargetIP );
	}
	void	InitializeCaptureOptions( CRadiusPacketItem& inPacketItem ) {
		inPacketItem.InitializeCaptureOptions();
	}

protected:
	CRadiusPacketItemMgr	m_RadiusPacketItemMgr;
	CTargetIPMap			m_TargetIPMap;
	IRadiusContext*			m_pRadiusContext;
	CActiveCaseList			m_ActiveCaseList;
	UInt32					m_nValidationNumber;

};
