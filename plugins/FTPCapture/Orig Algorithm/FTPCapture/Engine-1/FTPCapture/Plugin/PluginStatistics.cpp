// =============================================================================
//	FtpSession.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#include "StdAfx.h"
#include "PluginStatistics.h"
//#include "FTPCaptureContext.h"
#include "OmniEngineContext.h"


//////////////////////////////////////////////////////////////////////
//		CPluginStatistics
//////////////////////////////////////////////////////////////////////

CPluginStatistics::CPluginStatistics()
{
	m_strLabels[kStat_ActiveUsers]			= _T("Active Users");
	m_strLabels[kStat_ActiveDataChannels]	= _T("Active Data Channels");
	m_strLabels[kStat_Logons]				= _T("Total Logons");
	m_strLabels[kStat_Logoffs]				= _T("Total Logoffs");
	m_strLabels[kStat_Received]				= _T("Total Received files");
	m_strLabels[kStat_Stored]				= _T("Total Stored files");
	m_strLabels[kStat_TimedOutControl]		= _T("Timed-out Users");
	m_strLabels[kStat_TimedOutData]			= _T("Timed-out Data Channels");
}

CPluginStatistics::~CPluginStatistics()
{
	size_t	nCount = m_Stats.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		delete m_Stats[i];
	}
	m_Stats.RemoveAll();
}


// -----------------------------------------------------------------------------
//		Init
// -----------------------------------------------------------------------------

void
CPluginStatistics::Init(
	COmniEngineContext* inContext )
//	CFTPCaptureContext*	inContext )
{
	m_Stats.Add( new CPluginStatFunc( inContext->m_FtpSessions ) );		// kStat_ActiveUsers,
	m_Stats.Add( new CPluginStatFunc( inContext->m_DataChannels ) );	// kStat_ActiveDataChannels,
	m_Stats.Add( new CPluginStatValue() );								// kStat_Logons,
	m_Stats.Add( new CPluginStatValue() );								// kStat_Logoffs,
	m_Stats.Add( new CPluginStatFunc( inContext->m_FtpSessions, 1 ) );	// kStat_Received,
	m_Stats.Add( new CPluginStatFunc( inContext->m_FtpSessions, 2 ) );	// kStat_Stored,
	m_Stats.Add( new CPluginStatValue() );								// kStat_TimedOutControl,
	m_Stats.Add( new CPluginStatValue() );								// kStat_TimedOutData,
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
CPluginStatistics::Reset()
{
	for ( int i = 0; i < kStat_Maximum; i++ ) {
		m_Stats[i]->Reset();
	}

}


// -----------------------------------------------------------------------------
//		Increment
// -----------------------------------------------------------------------------

void
CPluginStatistics::Increment(
	size_t	inIndex,
	size_t	inValue )
{
	if ( inIndex < kStat_Maximum ) {
		m_Stats[inIndex]->Increment( inValue );
	}
}


// -----------------------------------------------------------------------------
//		Get
// -----------------------------------------------------------------------------

bool
CPluginStatistics::Get(
	size_t			inIndex,
	UInt32&			outStat,
	CPeekString&	outLabel )
{
	if ( inIndex < kStat_Maximum ) {
		outStat = m_Stats[inIndex]->Get();
		outLabel = m_strLabels[inIndex];
		return true;
	}
	return false;
}
