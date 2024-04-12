// ============================================================================
//	LockedPtr.h
//		interface for the COptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once


// ============================================================================
//	TLockedPtr
// ============================================================================

template< typename T >
class TLockedPtr
{
public:
	T*		m_p;

public:
	;			TLockedPtr() throw()
		:	m_p( NULL ) {
	}
	template< typename TSrc >
	TLockedPtr( TLockedPtr< TSrc >& p ) throw() {
		m_p = p.Detach();  // Transfer ownership
	}
	explicit	TLockedPtr( T* p ) throw()
		:	m_p( p ) {
		ATLASSERT( m_p != NULL );
		if ( !m_p->Lock() ) {
			m_p = NULL;
		}
	}
	;		~TLockedPtr() throw() {
		Unlock();
	}

	// Templated version to allow pBase = pDerived
	template< typename TSrc >
	TLockedPtr< T >&	operator=( TLockedPtr< TSrc >& p ) throw() {
		Unlock();
		Attach( p.Detach() );  // Transfer ownership

		return( *this );
	}

	;		operator T*() const throw() {
		return( m_p );
	}
	T*		operator->() const throw() {
		ATLASSERT( m_p != NULL );
		return( m_p );
	}

	// Attach to an existing pointer (takes ownership)
	void	Attach( T* p ) throw() {
		ATLASSERT( m_p == NULL );
		m_p = p;
	}
	// Detach the pointer (releases ownership)
	T*		Detach() throw() {
		T* p;

		p = m_p;
		m_p = NULL;

		return( p );
	}
	// Unlock the object pointed to, and set the pointer to NULL
	void	Unlock() throw() {
		if ( m_p ) {
			m_p->Unlock();
			m_p = NULL;
		}
	}
};

// static_cast_auto template functions.  Used like static_cast, only they work on TLockedPtr objects
template< class Dest, class Src >
Dest* static_cast_auto( const TLockedPtr< Src >& pSrc ) throw()
{
	return( static_cast<Dest*>( static_cast<Src*>( pSrc ) ) );
}
