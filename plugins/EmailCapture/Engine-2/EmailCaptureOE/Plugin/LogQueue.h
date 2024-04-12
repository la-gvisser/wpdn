// ============================================================================
//	LogQueue.h
//		interface for the Log Queue class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "EmailStats.h"
#include "MsgAddress.h"
#include "NotifyService.h"
#include <vector>


// ============================================================================
//		CLogItem
// ============================================================================

class CLogItem
{
protected:
	PeekSeverity	m_nSeverity;
	CPeekString		m_strMessage;

public:
	;		CLogItem() : m_nSeverity( peekSeverityInformational )
	{}
	;		CLogItem( PeekSeverity inSeverity, const CPeekString& inMessage ) :
				m_nSeverity( inSeverity ), m_strMessage( inMessage )
	{}

	void	Post( CPeekContext* inContext );
};


// ============================================================================
//		CLogQueue
// ============================================================================

class CLogQueue
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
protected:
	CPeekContext*			m_pContext;
	std::vector<CLogItem>	m_Messages;

	void	Add( const CPeekString& inMessage, PeekSeverity inSeverity );

public:
	;		CLogQueue() {}
	;		~CLogQueue() {}

	void	Init( CPeekContext* inContext ) { m_pContext = inContext; }

	void	PostMessages();
	void	RemoveAll();

	void	AddMessage( const CPeekString& inMessage, PeekSeverity inSeverity = peekSeverityInformational ) { Add( inMessage, inSeverity ); }
	void	AddDetectMessage( CEmailStats::EmailStatType inType, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddDetectMessage( CEmailStats::EmailStatType inType, const CMsgAddress& inTarget, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddSaveMessage( const CPeekString& inTarget, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddResetMessage( CEmailStats::EmailStatType inType, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddResetMessage( CEmailStats::EmailStatType inType, const CMsgAddress& inTarget, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddTimedOutMessages( CEmailStats::EmailStatType inType, const CArrayString& inRemovedList, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddFileErrorMessage( const CPeekString& inFileName, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddWriteErrorMessage( const CPeekString& inFileName, PeekSeverity inSeverity = peekSeverityInformational );
	void	AddFailureMessage( const CPeekString& inMessage );
};
