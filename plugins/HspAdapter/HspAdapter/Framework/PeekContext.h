// =============================================================================
//	PeekContext.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "GlobalId.h"
#include "PeekDataModeler.h"
#include "PeekContextProxy.h"
#include "PeekMessage.h"
#include "PeekProxy.h"
#include <vector>

using std::vector;

class CPeekPlugin;
class CRemotePlugin;
class CRemoteStatsPlugin;
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
	CPeekPlugin*		m_pPlugin;
	CPeekPlugin*		m_pGraphsPlugin;
	CPeekContextProxy	m_PeekContextProxy;
	CRemotePlugin*		m_pRemotePlugin;
#ifdef IMPLEMENT_SUMMARY
	CRemoteStatsPlugin*	m_pRemoteStatsPlugin;
#endif	// IMPLEMENT_SUMMARY

protected:
	void		SetPlugin( CPeekPlugin* inPlugin ) { m_pPlugin = inPlugin; }
	void		SetState( tContextState inState ) { m_nState = inState; }

public:
	;			CPeekContext( const CGlobalId& inId, CRemotePlugin* inRemotePlugin )
		:	m_nState( kContextState_Uninitialized )
		,	m_Id( inId )
		,	m_pPlugin( nullptr )
		,	m_pGraphsPlugin( nullptr )
		,	m_pRemotePlugin( inRemotePlugin )
#ifdef IMPLEMENT_SUMMARY
		,	m_pRemoteStatsPlugin( nullptr )
#endif
	{
		SetState( kContextState_Initialized );
	}
#ifdef IMPLEMENT_SUMMARY
	;			CPeekContext( const CGlobalId& inId, CRemoteStatsPlugin* inRemotePlugin )
#else	// IMPLEMENT_SUMMARY
	;			CPeekContext( const CGlobalId& inId )
#endif
		:	m_nState( kContextState_Uninitialized )
		,	m_Id( inId )
		,	m_pPlugin( nullptr )
		,	m_pGraphsPlugin( nullptr )
		,	m_pRemotePlugin( nullptr )
#ifdef IMPLEMENT_SUMMARY
		,	m_pRemoteStatsPlugin( inRemotePlugin )
#endif	// IMPLEMENT_SUMMARY
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
	virtual CPeekPlugin*	GetPlugin() const { return m_pPlugin; }
	virtual CPeekProxy*		GetProxy() = 0;
	const CRemotePlugin*	GetRemotePlugin() const { return m_pRemotePlugin; }
	tContextState			GetState() { return m_nState; }

	bool				HasGraphs() const { return (m_pGraphsPlugin != nullptr); }

	bool				IsGlobalContext() { return( m_Id.IsNull() ); }

	void				PreDispose( CPeekPlugin* inPlugin ) {
		if ( inPlugin == m_pGraphsPlugin ) {
			m_pGraphsPlugin = nullptr;
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
			_ASSERTE( CheckState( kContextState_Created ) );
			if ( CheckState( kContextState_Created ) ) {
				SetState( kContextState_PrefsSet );
			}
		}
		return PEEK_PLUGIN_SUCCESS;
	}

	void			SetId( const CGlobalId& inId ) { m_Id = inId; }
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
	:	public std::shared_ptr<CPeekContext>
{
public:
	;		CPeekContextPtr( CPeekContext* inContext = nullptr )
		:	std::shared_ptr<CPeekContext>( inContext )
	{}

	;		operator CPeekContext*() { return get(); }

	bool	IsNotValid() const { return (get() == nullptr); }
	bool	IsValid() const { return (get() != nullptr); }
};

bool operator== (const CPeekContextPtr& lhs, const CPeekContext* rhs );


// ============================================================================
//		CPeekContextList
// ============================================================================

class CPeekContextList
	:	public vector<CPeekContext*>
{
public:
	;		CPeekContextList() {}

	bool	Find( const GUID& inId, CPeekContext*& outContext ) const {
		vector<CPeekContext*>::const_iterator itr = begin();
		for ( ; itr != end(); ++itr ) {
			if ( (*itr)->GetId() == inId ) {
				outContext = *itr;
				return true;
			}
		}
		return false;
	}
	bool	Find( const CPeekContext* inContext, size_t& outIndex ) const {
		vector<CPeekContext*>::const_iterator itr = begin();
		for ( size_t nIndex = 0; itr != end(); ++itr, ++nIndex ) {
			if ( *itr == inContext ) {
				outIndex = nIndex;
				return true;
			}
		}
		return false;
	}

	void	Remove( CPeekContext* inContext ) {
		_ASSERTE( inContext );
		vector<CPeekContext*>::iterator itr = std::find( begin(), end(), inContext );
		if ( itr != end() ) {
			erase( itr );
		}
	}
};
