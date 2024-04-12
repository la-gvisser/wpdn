// ============================================================================
//	SafeClient.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#ifdef DISPLAY_LOCK_RETURN
  #ifdef _DEBUG
    #define LOCK_ERROR_RESPONSE(ErrText) ASSERT( 0 );
  #else // _DEBUG
    #define LOCK_ERROR_RESPONSE(ErrText) AfxMessageBox( _T(ErrText) );
  #endif // _DEBUG
#else // DISPLAY_LOCK_RETURN
  #define LOCK_ERROR_RESPONSE(ErrText);
#endif // DISPLAY_LOCK_RETURN


template< typename T > class TSafeClientPtr;

// =============================================================================
//	TSafeClient
// =============================================================================

template< typename TClass >
class TSafeClient
{
	friend class TSafeClientPtr< TClass >;

protected:
	mutable CMutex		m_LockObject;
	TClass				m_Client;

	bool	Lock( DWORD dwTimeOut = DEFAULT_TIMEOUT ) const { 
		bool bGotLock = m_LockObject.Lock( dwTimeOut ) != FALSE;
	  #ifdef DISPLAY_LOCK_RETURN
		if ( !bGotLock ) {
			LOCK_ERROR_RESPONSE( "Lock failed" )
		}
	  #endif
		return bGotLock;
	}

	void	Unlock() const {
	  #ifdef DISPLAY_LOCK_RETURN
		bool bUnlocked  = m_LockObject.Unlock() != FALSE; 
		if ( !bUnlocked ) {
			LOCK_ERROR_RESPONSE( "Unlock failed" )
		}
	  #else // DISPLAY_LOCK_RETURN
		m_LockObject.Unlock();
	  #endif // DISPLAY_LOCK_RETURN
	}

	TClass* GetClientPtr() {
		return &m_Client;
	}

public:
	TSafeClient() {}
	~TSafeClient() {}

  #ifdef PTR_DOES_NOT_LOCK
	TClass* GetLockedClientPtr() { 
		return ( &m_Client ); 
	}
  #else // PTR_DOES_NOT_LOCK
	TSafeClientPtr< TClass > GetLockedClientPtr() { 
		return TSafeClientPtr<TClass>( this ); 
	}
  #endif // PTR_DOES_NOT_LOCK
};


#ifndef TEST_NO_LOCKING

// =============================================================================
//		TSafeClientPtr
// =============================================================================

template< typename T  >
class TSafeClientPtr
{
protected:
	T*					m_pRealClient;
	TSafeClient<T>*		m_pSafeClient;

public:
	TSafeClientPtr() throw()
		:	m_pRealClient( NULL )
		,	m_pSafeClient( NULL ) {
	}

	explicit TSafeClientPtr( TSafeClient<T>* pSafeClient ) throw()
		:	m_pSafeClient( pSafeClient )
		,	m_pRealClient( NULL ) {
		ASSERT( m_pSafeClient );
		if ( m_pSafeClient == NULL ) {
			return;
		}
		if ( m_pSafeClient->Lock() ) {
			m_pRealClient = m_pSafeClient->GetClientPtr();
			ASSERT( m_pRealClient );
		}
	}

	~TSafeClientPtr() throw() {
		if ( m_pRealClient ) {
			if ( m_pSafeClient ) {
				m_pSafeClient->Unlock();
			}
			else {
				ASSERT( 0 );
			}
		}
	}

	operator T*() const throw() {
		return( m_pRealClient );
	}

	T* operator->() const throw() {
		ASSERT( m_pRealClient );
		return( m_pRealClient );
	}
};
#endif // TEST_NO_LOCKING
