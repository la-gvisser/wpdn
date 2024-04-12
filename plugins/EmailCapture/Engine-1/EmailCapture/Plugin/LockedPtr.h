// ============================================================================
//	LockedPtr.h
//		interface for the LockedPtr template.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

#pragma once


// ============================================================================
//	TLockedPtr
// ============================================================================

template< typename T >
class TLockedPtr
{
protected:
	mutable T*		m_p;

protected:
	// Attach to an existing pointer (takes ownership)
	void	Attach( T* p ) throw() {
		ATLASSERT( m_p == NULL );
		m_p = p;
	}

	// Detach the pointer (releases ownership)
	T*		Detach() const throw() {
		T* p;

		p = m_p;
		m_p = NULL;

		return( p );
	}

	// Unlock the object pointed to, and set the pointer to NULL
	void	release() throw() {
		if ( m_p ) {
			m_p->Unlock();
			m_p = NULL;
		}
	}

public:
	;			TLockedPtr() throw()
		:	m_p( NULL )
	{
	}
	;			TLockedPtr( const TLockedPtr<T>& inOther ) throw()
		:	m_p( NULL )
	{
		Attach( inOther.Detach() );  // Transfer ownership
	}
	//template< typename TSrc >
	//TLockedPtr( TLockedPtr< TSrc >& p ) throw()
	//{
	//	m_p = p.Detach();  // Transfer ownership
	//}
	explicit	TLockedPtr( T* p ) throw()
		:	m_p( p )
	{
		_ASSERTE( m_p != NULL );
		m_p->Lock();
	}
	;		~TLockedPtr() throw()
	{
		release();
	}

	// Templated version to allow pBase = pDerived
	template< typename TSrc >
	TLockedPtr< T >&	operator=( TLockedPtr< TSrc >& inOther ) throw() {
		if ( &inOther != this ) {
			release();
			Attach( inOther.Detach() );  // Transfer ownership
		}

		return *this;
	}

	const TLockedPtr<T>& 	operator =( const TLockedPtr<T>& inOther ) {
		if ( &inOther != this ) {
			release();
			m_p = inOther.Detach();
		}
		return *this;
	}

	;		operator T*() const throw() {
		return( m_p );
	}
	T*		operator->() const throw() {
		ATLASSERT( m_p != NULL );
		return( m_p );
	}
};

// static_cast_auto template functions.  Used like static_cast, only they work on TLockedPtr objects
template< class Dest, class Src >
Dest* static_cast_auto( const TLockedPtr< Src >& pSrc ) throw()
{
	return( static_cast<Dest*>( static_cast<Src*>( pSrc ) ) );
}
