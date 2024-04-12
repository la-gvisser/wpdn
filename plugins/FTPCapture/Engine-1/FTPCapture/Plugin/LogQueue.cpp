// =============================================================================
//	LogQueue.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "LogQueue.h"
#include "PeekEngineContext.h"


///////////////////////////////////////////////////////////////////////////////
//		CLogItem
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Post
// -----------------------------------------------------------------------------

void
CLogItem::Post(
	CPeekEngineContext&	inContext )
{
	inContext.DoLogMessage( m_nSeverity, m_strMessage );
}


///////////////////////////////////////////////////////////////////////////////
//		CLogQueue
///////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
//		AddTimedOutMessages
// -----------------------------------------------------------------------------

void
CLogQueue::AddTimedOutMessages(
	const CArrayString&			inRemovedList,
	UInt8						inSeverity /* = peekSeverityInformational */ )
{
	const	size_t	nCount( inRemovedList.GetCount() );

	for ( size_t i = 0; i < nCount; i++ ) {
	CPeekOutString	strMsg;
	strMsg	<< inRemovedList[i] << L" timed out and was removed from the tracking list";
		AddMessage( strMsg, inSeverity );
	}
}
