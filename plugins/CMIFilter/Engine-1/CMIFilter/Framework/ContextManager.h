// =============================================================================
//	ContextManager.h
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekContext.h"
#include <vector>

using std::vector;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Context Manager
//
//	The Context Manager is used by the Framework to insulate OmniPlugins from
//	the Remote Plugin life cycle.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CContextManager
// =============================================================================

class
CContextManager
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
protected:
	vector<CPeekContextPtr>	m_ayContexts;

	bool	InternalFind( const CPeekContext* inContext, size_t& outIndex ) const {
		if ( inContext ) {
			size_t	nCount = m_ayContexts.size();
			for ( size_t i = 0; i < nCount; i++ ) {
				if ( m_ayContexts[i].get() == inContext ) {
					outIndex = i;
					return true;
				}
			}
		}
		return false;
	}
	bool	InternalFind( const CGlobalId& inId, size_t& outIndex ) const {
		size_t	nCount = m_ayContexts.size();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContextPtr spContext = m_ayContexts[i];
			if ( spContext.IsValid() ) {
				if ( spContext->GetId() == inId ) {
					outIndex = i;
					return true;
				}
			}
		}
		return false;
	}

	bool	InternalFind( const CPeekContext* inContext,
				std::vector<CPeekContextPtr>::const_iterator& outIterator ) const {
		if ( inContext ) {
			std::vector<CPeekContextPtr>::const_iterator i =
				std::find( m_ayContexts.begin(), m_ayContexts.end(), inContext );
			if ( i != m_ayContexts.end() ) {
				outIterator = i;
				return true;
			}
		}
		return false;
	}
	bool	InternalFind( const CPeekContext* inContext,
				std::vector<CPeekContextPtr>::iterator& outIterator ) {
		if ( inContext ) {
			std::vector<CPeekContextPtr>::iterator i =
				std::find( m_ayContexts.begin(), m_ayContexts.end(), inContext );
			if ( i != m_ayContexts.end() ) {
				outIterator = i;
				return true;
			}
		}
		return false;
	}

public:
	;		CContextManager() {}
	;		~CContextManager() {}

	void	Add( CPeekContextPtr inContext ) { m_ayContexts.push_back( inContext ); }

	bool	Find( const CGlobalId& inId, CPeekContextPtr& outContext ) const {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inId, nIndex ) ) {
			outContext = m_ayContexts[nIndex];
			return true;
		}
		return false;
	}
	bool	Find( CPeekContextPtr inContext, size_t& outIndex ) const {
		return InternalFind( inContext.get(), outIndex );
	}
	bool	Find( CPeekContextPtr inContext, std::vector<CPeekContextPtr>::const_iterator& outItem ) const {
		return InternalFind( inContext.get(), outItem );
	}
	bool	Find( CPeekContextPtr inContext, std::vector<CPeekContextPtr>::iterator& outItem ) {
		return InternalFind( inContext.get(), outItem );
	}
	bool	Find( const CPeekContext* inContext, size_t& outIndex ) const {
		return InternalFind( inContext, outIndex );
	}

	size_t	GetCount() const { return m_ayContexts.size(); }
	CPeekContext*	GetItem( size_t inElement ) {
		if ( inElement > GetCount() ) return nullptr;
		return m_ayContexts[inElement].get();
	}

	bool	IsIdInManager( const CGlobalId& inId ) {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inId, nIndex ) ) return true;

		return false;
	}
	bool	IsEmpty() const { return m_ayContexts.empty(); }

	void	Remove( CPeekContextPtr inContext ) {
		vector<CPeekContextPtr>::iterator i;
		if ( Find( inContext, i ) ) {
			// 3: The Plugin, the manager and inData.
			if ( i->use_count() == 3 ) {
				m_ayContexts.erase( i );
				// use_count == 2.
				// Upon return, use_count == 1.
			}
		}
	}
};


// =============================================================================
//		CContextManagerPtr
// =============================================================================

class CContextManagerPtr
{
protected:
	CContextManager*		m_pManager;

public:
	;	CContextManagerPtr( CContextManager* inManager )
		:	m_pManager( inManager )
	{
		_ASSERTE( m_pManager != nullptr );
		m_pManager->Lock();
	}
	;	~CContextManagerPtr() {
		m_pManager->Unlock();
	}

	;	operator CContextManager*() const {
		return( m_pManager );
	}
	CContextManager*	operator->() const {
		_ASSERTE( IsValid() );
		return( m_pManager );
	}

	// Attach to an existing pointer (takes ownership)
	void				Attach( CContextManager* inManager ) {
		_ASSERTE( IsNotValid() );
		m_pManager = inManager;
	}

	// Detach the pointer (releases ownership)
	CContextManager*	Detach() {
		CContextManager*	pManager = m_pManager;
		m_pManager = nullptr;
		return( pManager );
	}

	bool	IsNotValid() const { return (m_pManager == nullptr); }
	bool	IsValid() const { return (m_pManager != nullptr); }
};


// =============================================================================
//		CSafeContextManager
// =============================================================================

class
CSafeContextManager
{
protected:
	CContextManager	m_Manager;

public:
	;		CSafeContextManager() {}

	CContextManagerPtr	Get() {
		return CContextManagerPtr( &m_Manager );
	}
};
