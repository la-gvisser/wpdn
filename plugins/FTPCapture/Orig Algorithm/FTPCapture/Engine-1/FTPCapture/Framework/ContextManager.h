// =============================================================================
//	ContextManager.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "PeekContext.h"


// =============================================================================
//		CContextManager
// =============================================================================

class
CContextManager
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
protected:
	CPeekArray<CPeekContextPtr>	m_ayContexts;

	//CPeekContextPtr		Get( int inElement ) {
	//	return m_ayContexts.at( inElement );
	//}

	bool	InternalFind( const CPeekContext* inContext, size_t& outIndex ) const {
		if ( inContext ) {
			size_t	nCount = m_ayContexts.GetCount();
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
		size_t	nCount = m_ayContexts.GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContextPtr spContext = m_ayContexts[i];
			if ( spContext ) {
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

public:
	;		CContextManager() {}
	;		~CContextManager() {}

	void	Add( CPeekContextPtr inContext ) { m_ayContexts.Add( inContext ); }

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
	bool	Find( const CPeekContext* inContext, size_t& outIndex ) const {
		return InternalFind( inContext, outIndex );
	}

	size_t	GetCount() const { return m_ayContexts.GetCount(); }
	CPeekContext*	GetItem( size_t inElement ) {
		if ( inElement > GetCount() ) return NULL;
		return m_ayContexts[inElement].get();
	}

	bool	IsIdInManager( const CGlobalId& inId ) {
		size_t	nIndex( kIndex_Invalid );
		if ( InternalFind( inId, nIndex ) ) return true;

		return false;
	}
	bool	IsEmpty() const { return m_ayContexts.empty(); }

	void	Remove( CPeekContextPtr inContext ) {
		std::vector<CPeekContextPtr>::const_iterator i;
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
	;	CContextManagerPtr( CContextManager* pManager ) throw()
		:	m_pManager( pManager )
	{
		HE_ASSERT( m_pManager != NULL );
		m_pManager->Lock();
	}
	;	~CContextManagerPtr() throw() {
		m_pManager->Unlock();
	}

	;	operator CContextManager*() const throw() {
		return( m_pManager );
	}
	CContextManager*	operator->() const throw() {
		HE_ASSERT( m_pManager != NULL );
		return( m_pManager );
	}

	// Attach to an existing pointer (takes ownership)
	void				Attach( CContextManager* p ) throw() {
		HE_ASSERT( m_pManager == NULL );
		m_pManager = p;
	}

	// Detach the pointer (releases ownership)
	CContextManager*	Detach() throw() {
		CContextManager* pManager;
		pManager = m_pManager;
		m_pManager = NULL;
		return( pManager );
	}
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
	;					CSafeContextManager() {}
	CContextManagerPtr	Get() {
		return CContextManagerPtr( &m_Manager );
	}
};
