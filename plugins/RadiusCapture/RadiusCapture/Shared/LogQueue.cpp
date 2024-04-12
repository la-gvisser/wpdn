
// =============================================================================
//	LogQueue.cpp
//		implementation of the CLogQueue class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#include "StdAfx.h"
#include "LogQueue.h"
#include "OmniEngineContext.h"

///////////////////////////////////////////////////////////////////////////////
//		CLogItem
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Post
// -----------------------------------------------------------------------------

void
CLogItem::Post(
	IRadiusContext*	inContext )
{
	(reinterpret_cast<COmniEngineContext*>(inContext))->DoLogMessage( m_nSeverity, m_strMessage );
}


///////////////////////////////////////////////////////////////////////////////
//		CLogItemTraits
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		CompareElements
// -----------------------------------------------------------------------------

bool
CLogItemTraits::CompareElements(
								const CLogItem&	inItem1,
								const CLogItem&	inItem2 )
{
	return (inItem1.m_strMessage.CompareNoCase( inItem2.m_strMessage ) == 0);
}


///////////////////////////////////////////////////////////////////////////////
//		CLogQueue
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

void
CLogQueue::Add(
			   const CPeekString&	inMessage ) //,
//			   UInt8			inSeverity )
{
	CSingleLock	Lock( &m_Semaphore );

	Lock.Lock( 500 );
	if ( Lock.IsLocked() ) {
		m_Messages.Add( CLogItem( kNotifySeverity_Informational, inMessage ) );
//		m_Messages.Add( CLogItem( inSeverity, inMessage ) );
	}
}


// -----------------------------------------------------------------------------
//		PostMessages
// -----------------------------------------------------------------------------

void
CLogQueue::PostMessages()
{
	CSingleLock	Lock( &m_Semaphore );

	Lock.Lock( 500 );
	if ( Lock.IsLocked() ) {
		ASSERT( m_pPeekContext );
		for ( size_t i = 0; i < m_Messages.GetCount(); i++ ) {
			m_Messages[i].Post( m_pPeekContext );
		}
		m_Messages.RemoveAll();
	}
}


// -----------------------------------------------------------------------------
//		RemoveAll
// -----------------------------------------------------------------------------

void
CLogQueue::RemoveAll()
{
	CSingleLock	Lock( &m_Semaphore );

	Lock.Lock( 500 );
	if ( Lock.IsLocked() ) {
		m_Messages.RemoveAll();
	}
}
