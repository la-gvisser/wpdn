// ============================================================================
//	LogQueue.cpp
//		implementation of the CLogQueue class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "OmniConsoleContext.h"
#include "LogQueue.h"
#include "ProtocolType.h"
#include "GlobalTags.h"


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekString	kEmail( L" email " );
	const CPeekString	kEmailFor( L" Email for <" );
	const CPeekString	kEmailPre( L"Email Capture - A " );
	const CPeekString	kEmailStream( L" Email stream for <" );
	const CPeekString	kErrorSaving( L"Email Capture - Error saving file " );
	const CPeekString	kErrorWriting( L"Email Capture - Error writing to file " );
	const CPeekString	kSaveEmail( L"Email Capture - Saved email from <" );
	const CPeekString	kResetEmail( L" Email has been Reset by one of the peers and was removed from the Tracking List" );
	const CPeekString	kTimedOut( L" Timed Out and was removed from the Tracking List" );
	const CPeekString	kTrackingNew( L"Email Capture - Tracking new " );
}


// ============================================================================
//		CLogItem
// ============================================================================

// ----------------------------------------------------------------------------
//		Post
// ----------------------------------------------------------------------------

void
CLogItem::Post(
	CPeekContext*	inContext )
{
	CPeekEngineContext*	pEngineContext = dynamic_cast<CPeekEngineContext*>( inContext );
	if ( pEngineContext ) {
		pEngineContext->DoLogMessage( m_strMessage, m_nSeverity );
		return;
	}
#if (0)
	CPeekConsoleContext*	pConsoleContext = dynamic_cast<CPeekConsoleContext*>( inContext );
	if ( pConsoleContext ) {
		pConsoleContext->DoLogMessage( m_strMessage, m_nSeverity );
	}
#endif
}


// ============================================================================
//		CLogQueue
// ============================================================================

// ----------------------------------------------------------------------------
//		Add
// ----------------------------------------------------------------------------

void
CLogQueue::Add(
	const CPeekString&	inMessage,
	PeekSeverity		inSeverity )
{
	ObjectLock	lock( this );

	m_Messages.Add( CLogItem( inSeverity, inMessage ) );
}


// ----------------------------------------------------------------------------
//		PostMessages
// ----------------------------------------------------------------------------

void
CLogQueue::PostMessages()
{
	ObjectLock	lock( this );

	for ( size_t i = 0; i < m_Messages.GetCount(); i++ ) {
		m_Messages[i].Post( m_pContext );
	}
	m_Messages.RemoveAll();
}


// ----------------------------------------------------------------------------
//		RemoveAll
// ----------------------------------------------------------------------------

void
CLogQueue::RemoveAll()
{
	ObjectLock	lock( this );

	m_Messages.RemoveAll();
}


// ----------------------------------------------------------------------------
//		AddDetectMessage
// ----------------------------------------------------------------------------

void
CLogQueue::AddDetectMessage(
	CEmailStats::EmailStatType	inType,
	PeekSeverity				inSeverity /* = peekSeverityInformational */ )
{
	const CPeekString*	ayStrs[] = {
		&Tags::kTrackingNew,
		Tags::kxTypesProtocol[inType].RefW(),
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
}

void
CLogQueue::AddDetectMessage(
	CEmailStats::EmailStatType	inType,
	const CMsgAddress&			inTarget,
	PeekSeverity				inSeverity /* = peekSeverityInformational */ )
{
	CPeekString		strTarget( inTarget.FormatLong() );

	const CPeekString*	ayStrs[] = {
		&Tags::kTrackingNew,
		Tags::kxTypesProtocol[inType].RefW(),
		&Tags::kEmail,
		&strTarget,
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
}


// ----------------------------------------------------------------------------
//		AddSaveMessage
// ----------------------------------------------------------------------------

void
CLogQueue::AddSaveMessage(
	const CPeekString&	inTarget,
	PeekSeverity		inSeverity /* = peekSeverityInformational */ )
{
	const CPeekString*	ayStrs[] = {
		&Tags::kSaveEmail,
		&inTarget,
		Tags::kxAngleRight.RefW(),
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
}


// ----------------------------------------------------------------------------
//		AddResetMessage
// ----------------------------------------------------------------------------

void
CLogQueue::AddResetMessage(
	CEmailStats::EmailStatType	inType,
	PeekSeverity				inSeverity /* = peekSeverityInformational */ )
{
	const CPeekString*	ayStrs[] = {
		&Tags::kEmailPre,
		Tags::kxTypesProtocol[inType].RefW(),
		&Tags::kEmail,
		&Tags::kResetEmail,
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
}

void
CLogQueue::AddResetMessage(
	CEmailStats::EmailStatType	inType,
	const CMsgAddress&			inTarget,
	PeekSeverity				inSeverity /* = peekSeverityInformational */ )
{
#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&Tags::kEmailPre,
		Tags::kxTypesProtocol[inType].RefW(),
		&Tags::kEmailFor,
		inTarget.GetAddress().RefW(),
		Tags::kxAngleRight.RefW(),
		&Tags::kResetEmail,
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
#else
	CPeekOutString	ssMsg;
	ssMsg <<
		L"Email Capture - A " <<
		Tags::kProtocolTypes[inType] <<
		L" Email for <" <<
		inTarget.GetAddress().GetW() <<
		L"> has been Reset by one of the peers and was removed from the Tracking List";

	Add( ssMsg, inSeverity );
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		AddTimedOutMessages
// ----------------------------------------------------------------------------

void
CLogQueue::AddTimedOutMessages(
	CEmailStats::EmailStatType	inType,
	const CArrayString&			inRemovedList,
	PeekSeverity				inSeverity /* = peekSeverityInformational */ )
{
	for ( size_t i = 0; i < inRemovedList.GetCount(); i++ ) {
#if (TOVERIFY)
		const CPeekString*	ayStrs[] = {
			&Tags::kEmailPre,
			Tags::kxTypesProtocol[inType].RefW(),
			&Tags::kEmailStream,
			&inRemovedList[i],
			Tags::kxAngleRight.RefW(),
			&Tags::kTimedOut,
			NULL
		};
		CPeekString	strMsg( FastCat( ayStrs ) );

		Add( strMsg, inSeverity );
#else
		CPeekOutString	ssMsg;
		ssMsg <<
			L"Email Capture - The " <<
			Tags::kProtocolTypes[inType] <<
			L" Email stream for <" <<
			inRemovedList[i] <<
			L"> Timed Out and was removed from the Tracking List";

		Add( ssMsg, inSeverity );
#endif // TOVERIFY
	}
}


// ----------------------------------------------------------------------------
//		AddFileErrorMessage
// ----------------------------------------------------------------------------

void
CLogQueue::AddFileErrorMessage(
	const CPeekString&	inFileName,
	PeekSeverity		inSeverity /* = peekSeverityInformational */ )
{
#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&Tags::kErrorSaving,
		&inFileName,
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
#else
	CPeekOutString	ssMsg;
	ssMsg <<
		L"Email Capture - Error saving file " <<
		inFileName;

	Add( ssMsg, inSeverity );
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		AddWriteErrorMessage
// ----------------------------------------------------------------------------

void
CLogQueue::AddWriteErrorMessage(
	const CPeekString&	inFileName,
	PeekSeverity		inSeverity /* = peekSeverityInformational */ )
{
#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&Tags::kErrorWriting,
		&inFileName,
		NULL
	};
	CPeekString	strMsg( FastCat( ayStrs ) );

	Add( strMsg, inSeverity );
#else
	CPeekOutString ssMsg;
	ssMsg <<
		L"Email Capture - Error writing to file " <<
		inFileName;

	Add( ssMsg, inSeverity );
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		AddSaveMessage
// ----------------------------------------------------------------------------

void
CLogQueue::AddFailureMessage(
	const CPeekString&	inMessage )
{
	Add( inMessage, peekSeverityInformational );
}
