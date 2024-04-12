// =============================================================================
//	PeekAdapter.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekAdapter.h"


// =============================================================================
//		CPeekAdapter
// =============================================================================

// ----------------------------------------------------------------------------
//		DoLogMessage
// ----------------------------------------------------------------------------

void
CPeekAdapter::DoLogMessage(
	UInt64				inTimeStamp,
	int					inSeverity,
	const CPeekString&	inShortMessage,
	const CPeekString&	inLongMessage )
{
	CLogService&	logger = m_EngineProxy.GetLogService();
	CGlobalId		idNull( GUID_NULL );
	logger.DoLogMessage( idNull, idNull, 0,
		inTimeStamp, inSeverity, inShortMessage, inLongMessage );
}


#ifdef IMPLEMENT_NAVL
// ----------------------------------------------------------------------------
//		AddPacketHandler
// ----------------------------------------------------------------------------

void	
CPeekAdapter::AddPacketHandler(
	CPacketHandler	inHandler )
{
	if ( m_bNavlSupport ) {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_csHandlersMutex );
	
		m_ayPacketHandlers.AddUnique( inHandler );
		m_ayAddHandlers.AddUnique( inHandler );
	}
}


// ----------------------------------------------------------------------------
//		DoPacketHandlers
// ----------------------------------------------------------------------------

bool
CPeekAdapter::DoPacketHandlers()
{
	if ( m_bNavlSupport ) {
		m_csHandlersMutex.Lock();

		for each ( auto itr in m_ayAddHandlers ) {
			if ( !itr.StartPacketProvider() ) return E_FAIL;
		}
		m_ayAddHandlers.clear();

		for each ( auto itr in m_ayRemoveHandlers ) {
			if ( !itr.StopPacketProvider() ) return E_FAIL;
		}
		m_ayRemoveHandlers.clear();

		m_csHandlersMutex.Unlock();
	}

	return true;
}


// ----------------------------------------------------------------------------
//		RemovePacketHandler
// ----------------------------------------------------------------------------

void	
CPeekAdapter::RemovePacketHandler(
	CPacketHandler	inHandler )
{
	if ( m_bNavlSupport ) {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_csHandlersMutex );

		m_ayPacketHandlers.Remove( inHandler );

		if ( m_bCapturing ) {
			m_ayRemoveHandlers.AddUnique( inHandler );
		}
	}
}
#endif	// IMPLEMENT_NAVL
