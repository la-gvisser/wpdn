// ============================================================================
//	LogQueue.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "peeknotify.h"

class CPeekEngineContext;


// -----------------------------------------------------------------------------
//		CLogItem
// -----------------------------------------------------------------------------

class CLogItem
{
	friend class CLogItemTraits;

public:
protected:
	UInt8			m_nSeverity;
	CPeekString		m_strMessage;

public:
	;		CLogItem() : m_nSeverity( peekSeverityInformational ){}
	;		CLogItem( UInt8 inSeverity, const CPeekString& inMessage ) :
	m_nSeverity( inSeverity ), m_strMessage( inMessage ){}

	void	Post( CPeekEngineContext& inContext );
};


// -----------------------------------------------------------------------------
//		CLogItemTraits
// -----------------------------------------------------------------------------

class CLogItemTraits
	: public CElementTraits< CLogItem >
{
public:
	static bool	CompareElements( const CLogItem& inItem1, const CLogItem& inItem2 ) {
		return (inItem1.m_strMessage.CompareNoCase( inItem2.m_strMessage ) == 0);
	}
};


// -----------------------------------------------------------------------------
//		CLogQueue
// -----------------------------------------------------------------------------

class CLogQueue
{
protected:
	CPeekEngineContext*					m_pContext;
	CSemaphore							m_Semaphore;
	CAtlArray<CLogItem, CLogItemTraits>	m_Messages;

	void	Add( const CPeekString& inMessage, UInt8 inSeverity );

public:
	;		CLogQueue()		:	m_pContext( NULL ) {}
	;		~CLogQueue() { Clean(); }

	void	AddMessage( const CPeekString& inMessage, UInt8 inSeverity = peekSeverityInformational );
	void	AddTimedOutMessages( const CArrayString& inRemovedList,
		UInt8 inSeverity = peekSeverityInformational );

	void	Clean()	{ RemoveAll(); }

	void	Init( CPeekEngineContext* inContext ) { m_pContext = inContext; }

	void	PostMessages() {
		for ( size_t i = 0; i < m_Messages.GetCount(); i++ ) {
			m_Messages[i].Post( *m_pContext );
		}
		m_Messages.RemoveAll();
	}
	void	RemoveAll() {
		m_Messages.RemoveAll();
	}
};
