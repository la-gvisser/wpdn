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
	//petertest - decide if we should post to "0" as originally written.
	inContext.DoLogMessage( m_nSeverity, m_strMessage );
}


///////////////////////////////////////////////////////////////////////////////
//		CLogQueue
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

void
CLogQueue::Add(
	const CPeekString&	inMessage,
	UInt8				inSeverity )
{
	m_Messages.Add( CLogItem( inSeverity, inMessage ) );
}


// -----------------------------------------------------------------------------
//		AddMessage
// -----------------------------------------------------------------------------

void
CLogQueue::AddMessage(
	const CPeekString&	inMessage,
	UInt8				inSeverity )
{
	if ( inMessage.GetLength() ) {
		Add( inMessage, inSeverity );
	}
}


// -----------------------------------------------------------------------------
//		AddTimedOutMessages
// -----------------------------------------------------------------------------

void
CLogQueue::AddTimedOutMessages(
	const CArrayString&			inRemovedList,
	UInt8						inSeverity /* = peekSeverityInformational */ )
{
	CPeekOutString	strMsg;
	for ( size_t i = 0; i < inRemovedList.GetCount(); i++ ) {
		strMsg	<<  L"FTP Capture - The FTP Connection for <" << inRemovedList[i] 
				<< L"> Timed Out and was removed from the Tracking List";
		Add( strMsg, inSeverity );
	}
}
