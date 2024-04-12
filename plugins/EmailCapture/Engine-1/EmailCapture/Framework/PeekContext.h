// =============================================================================
//	PeekContext.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "GlobalId.h"
#include "PeekDataModeler.h"
#include "PeekContextProxy.h"
#include "PeekMessage.h"
#include "PeekProxy.h"

class CPeekPlugin;
class CRemotePlugin;
class COptions;
class COptionsPtr;


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
	CPeekPlugin*		m_pPlugin;
	CPeekPlugin*		m_pGraphsPlugin;
	CPeekContextProxy	m_PeekContextProxy;

protected:
	void	SetPlugin( CPeekPlugin* inPlugin ) { m_pPlugin = inPlugin; }
	void	SetState( tContextState inState ) { m_nState = inState; }

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

	const CPeekString&	GetCaptureName() const { return m_PeekContextProxy.GetCaptureName(); }
	CPeekPlugin*		GetGraphsPlugin() { return m_pGraphsPlugin; }
	const CGlobalId&	GetId() const { return m_Id; }
	tMediaType			GetMediaType() const {
		return tMediaType( m_PeekContextProxy.GetMediaType(), m_PeekContextProxy.GetMediaSubType() );
	}
	CPeekPlugin*			GetPlugin() const { return m_pPlugin; }
	virtual CPeekProxy*		GetProxy() = 0;
	tContextState			GetState() { return m_nState; }

	bool				HasGraphs() const { return (m_pGraphsPlugin != NULL); }

	bool				IsGlobalContext() { return( m_Id.IsNull() ); }
#ifdef IMPLEMENT_PLUGINMESSAGE
	bool				IsInstalledOnEngine() const;
#endif // IMPLEMENT_PLUGINMESSAGE

	void				PreDispose( CPeekPlugin* inPlugin ) {
		if ( inPlugin == m_pGraphsPlugin ) {
			m_pGraphsPlugin = NULL;
		}
	}

	// Operational
	virtual COptionsPtr	GetOptions() = 0;

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
			_ASSERTE( CheckState( kContextState_Created ) );
			if ( CheckState( kContextState_Created ) ) {
				SetState( kContextState_PrefsSet );
			}
		}
		return PEEK_PLUGIN_SUCCESS;
	}

	void			SetId( CGlobalId inId ) { m_Id = inId; }
	void			SetInterfacePointers( IHeUnknown* inSite, CPeekPlugin* inPlugin ) {
		SetPlugin( inPlugin );
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

class CPeekContextPtr
	:	public NSTR1::shared_ptr<CPeekContext>
{
public:
	;		CPeekContextPtr( CPeekContext* inContext = NULL )
		:	NSTR1::shared_ptr<CPeekContext>( inContext )
	{}

	;		operator CPeekContext*() { return get(); }

	bool	IsNotValid() const { return (get() == NULL); }
	bool	IsValid() const { return (get() != NULL); }
};

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
		_ASSERTE( inContext );
		size_t	nIndex( kIndex_Invalid );
		if ( Find( inContext, nIndex ) ) {
			RemoveAt( nIndex );
		}
	}
};
