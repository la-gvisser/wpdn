// ============================================================================
//	LogQueue.h
//		interface for the Log Queue class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2007. All rights reserved.

#pragma once

#include "RadiusContextInterface.h"


// Severity Levels for NotifyProc.
// Passed as UInt8
enum
{
	kNotifySeverity_Informational,
	kNotifySeverity_Minor,
	kNotifySeverity_Major,
	kNotifySeverity_Severe
};


// ============================================================================
//		CLogItem
// ============================================================================

class CLogItem
{
	friend class CLogItemTraits;

public:
protected:
	UInt8		m_nSeverity;
	CPeekString	m_strMessage;

public:
	;		CLogItem() : m_nSeverity( kNotifySeverity_Informational ){}
	;		CLogItem( UInt8 inSeverity, const CPeekString& inMessage ) :
	m_nSeverity( inSeverity ), m_strMessage( inMessage ){}

//	void	Post( CPeekContext* inContext );
	void	Post( IRadiusContext* inContext );
};


// ============================================================================
//		CLogItemTraits
// ============================================================================

class CLogItemTraits
	: public CElementTraits< CLogItem >
{
public:
	static bool	CompareElements( const CLogItem& inItem1, const CLogItem& inItem2 );
};


// ============================================================================
//		CLogQueue
// ============================================================================

class CLogQueue
{
protected:
	IRadiusContext*						m_pPeekContext;
	CSemaphore							m_Semaphore;
	CAtlArray<CLogItem, CLogItemTraits>	m_Messages;

	void	Add( const CPeekString& inMessage ); //, UInt8 inSeverity );

public:
	;		CLogQueue(  ) :	m_pPeekContext( NULL ) {}
	;		~CLogQueue() {}

//	void	Init( CPeekContext* inContext ) { ASSERT( inContext ); m_pPeekContext = inContext; }
	void	Init( IRadiusContext* inContext ) { ASSERT( inContext ); m_pPeekContext = inContext; }

	void	PostMessages();
	void	RemoveAll();

	void	AddMessage( const CPeekString& inMessage ) { //, UInt8 inSeverity = kNotifySeverity_Informational ) {
		Add( inMessage );
//		Add( inMessage, inSeverity );
	}
};
