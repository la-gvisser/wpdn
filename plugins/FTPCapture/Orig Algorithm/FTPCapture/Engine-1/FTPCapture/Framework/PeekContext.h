// =============================================================================
//	PeekContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "GlobalId.h"
#include "PeekContextProxy.h"
#include "PeekMessage.h"
#include "PeekProxy.h"

class CRemotePlugin;
class COptions;


// =============================================================================
//		CPeekContext
// =============================================================================

class CPeekContext
{
public:
	enum tContextState {
		kContextState_Uninitialized,
		kContextState_Initialized,
		kContextState_Created,
		kContextState_PrefsSet
	};

protected:
	tContextState		m_nState;
	CGlobalId			m_Id;
	CRemotePlugin*		m_pPlugin;
	CRemotePlugin*		m_pGraphsPlugin;
	CPeekContextProxy	m_PeekContextProxy;

protected:
	void		SetState( tContextState inState ) { m_nState = inState; }

public:
	;			CPeekContext( const CGlobalId& inId )
		:	m_nState( kContextState_Uninitialized )
		,	m_Id( inId )
		,	m_pPlugin( NULL )
		,	m_pGraphsPlugin( NULL )
	{
		SetState( kContextState_Initialized );
	}
	virtual			~CPeekContext() {}

	// Framework
	bool				CheckState( tContextState inState ) { return (GetState() >= inState); }
	bool				EqualState( tContextState inState ) { return (GetState() == inState); }

	CRemotePlugin*		GetGraphsPlugin() { return m_pGraphsPlugin; }
	const CGlobalId&	GetId() const { return m_Id; }
	CRemotePlugin*		GetPlugin() { return m_pPlugin; }
	virtual CPeekProxy*	GetProxy() = 0;
	tContextState		GetState() { return m_nState; }

	bool				HasGraphs() const { return (m_pGraphsPlugin != NULL); }

	bool				IsGlobalContext() { return( m_Id.IsNull() ); }

	void				PreDispose( CRemotePlugin* inPlugin ) {
		if ( inPlugin == m_pGraphsPlugin ) {
			m_pGraphsPlugin = NULL;
		}
	}

	// Operational
	virtual COptions&	GetOptions() = 0;

	virtual int		OnCreateContext() {
		SetState( kContextState_Created );
		return PEEK_PLUGIN_SUCCESS;
	}
	virtual int		OnDisposeContext() {
		SetState( kContextState_Initialized );
		return PEEK_PLUGIN_SUCCESS;
	}

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) {
		if ( ioPrefs.IsLoading() ) {
			ASSERT( CheckState( kContextState_Created ) );
			if ( CheckState( kContextState_Created ) ) {
				SetState( kContextState_PrefsSet );
			}
		}
		return PEEK_PLUGIN_SUCCESS;
	}

	void			SetId( CGlobalId inId ) { m_Id = inId; }
	void			SetInterfacePointers( IHeUnknown* inSite, CRemotePlugin* inPlugin ) {
		m_pPlugin = inPlugin;
		if ( inSite ) {
			bool	bGraphs = false;
			m_PeekContextProxy.SetInterfacePointers( inSite, bGraphs );
			if ( bGraphs ) {
				m_pGraphsPlugin = inPlugin;
			}
		}
	}
	virtual void	SetOptions( const COptions& inOptions ) = 0;
};


// ============================================================================
//		CPeekContextPtr
// ============================================================================

typedef NSTR1::shared_ptr<CPeekContext>	CPeekContextPtr;

bool operator== (const CPeekContextPtr& lhs, const CPeekContext* rhs );


// ============================================================================
//		CPeekContextList
// ============================================================================

class CPeekContextList
	:	public CPeekArray<CPeekContext*>
{
public:
	;				CPeekContextList() {}

	bool	Find( const GUID& inId, CPeekContext*& outContext ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContext* pContext = GetAt( i );
			if ( pContext ) {
				if ( pContext->GetId() == inId ) {
					outContext = pContext;
					return true;
				}
			}
		}
		return false;
	}
	bool	Find( const CPeekContext* inContext, size_t& outIndex ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContext* pContext = GetAt( i );
			if ( pContext == inContext ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	void	Remove( CPeekContext* inContext ) {
		HE_ASSERT( inContext );
		size_t	nIndex( kIndex_Invalid );
		if ( Find( inContext, nIndex ) ) {
			RemoveAt( nIndex );
		}
	}
};
