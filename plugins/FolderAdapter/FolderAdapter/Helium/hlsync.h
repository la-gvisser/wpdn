// =============================================================================
//	hlsync.h
// =============================================================================
//	Copyright (c) 2005-2013 WildPackets, Inc. All rights reserved.

#ifndef HLSYNC_H
#define HLSYNC_H

#include "hecore.h"
#ifdef HE_WIN32
#include <cstddef>
#else
#include <stdint.h>
#include <pthread.h>
#endif

namespace HeLib
{

/// \class CHeFakeCriticalSection
/// \brief Corresponds to ATL::CComFakeCriticalSection
class CHeFakeCriticalSection
{
public:
	HeResult Lock() throw()    { return HE_S_OK; }
	HeResult Unlock() throw()  { return HE_S_OK; }
	bool     TryLock() throw() { return true; }
	HeResult Init() throw()    { return HE_S_OK; }
	HeResult Term() throw()    { return HE_S_OK; }
	UInt32   SetSpinCount( UInt32 ) throw() { return 0; }
};

/// \class CHeCriticalSection
/// \brief Corresponds to ATL::CComCriticalSection
#ifdef HE_WIN32
class CHeCriticalSection
{
public:
	HeResult Lock() throw()
	{
		::EnterCriticalSection( &m_Mutex );
		return HE_S_OK;
	}

	bool TryLock() throw()
	{
		return (::TryEnterCriticalSection( &m_Mutex ) != FALSE);
	}

	HeResult Unlock() throw()
	{
		::LeaveCriticalSection( &m_Mutex );
		return HE_S_OK;
	}

	HeResult Init() throw()
	{
		HeResult	hr = HE_S_OK;

		if ( !::InitializeCriticalSectionAndSpinCount( &m_Mutex, 0 ) )
		{
			hr = Helium::HeResultFromWin32( ::GetLastError() );
		}

		return hr;
	}

	HeResult Term() throw()
	{
		::DeleteCriticalSection( &m_Mutex );
		return HE_S_OK;
	}

	UInt32 SetSpinCount( UInt32 nSpinCount ) throw()
	{
		return ::SetCriticalSectionSpinCount( &m_Mutex, nSpinCount );
	}

private:
	CRITICAL_SECTION	m_Mutex;
};
#else
class CHeCriticalSection
{
public:
	HeResult Lock() throw()
	{
		const int	ret = pthread_mutex_lock( &m_Mutex );
		return (ret == 0) ? HE_S_OK : Helium::HeResultFromErrorCodeBase( ret );
	}

	bool TryLock() throw()
	{
		return (pthread_mutex_trylock( &m_Mutex ) == 0);
	}

	HeResult Unlock() throw()
	{
		const int	ret = pthread_mutex_unlock( &m_Mutex );
		return (ret == 0) ? HE_S_OK : Helium::HeResultFromErrorCodeBase( ret );
	}

	HeResult Init() throw()
	{
		pthread_mutexattr_t	attr;
		int	ret = pthread_mutexattr_init( &attr );
		if ( ret == 0 )
		{
			ret = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
			if ( ret == 0 )
			{
				ret = pthread_mutex_init( &m_Mutex, &attr );
			}
			pthread_mutexattr_destroy( &attr );
		}
		return (ret == 0) ? HE_S_OK : Helium::HeResultFromErrorCodeBase( ret );
	}

	HeResult Term() throw()
	{
		const int	ret = pthread_mutex_destroy( &m_Mutex );
		return (ret == 0) ? HE_S_OK : Helium::HeResultFromErrorCodeBase( ret );
	}

	UInt32 SetSpinCount( UInt32 /*nSpinCount*/ ) throw()
	{
		return 0;
	}

private:
	pthread_mutex_t		m_Mutex;
};
#endif

/// \class CHeAutoCriticalSection
/// \brief Corresponds to ATL::CComAutoCriticalSection
class CHeAutoCriticalSection :
	public CHeCriticalSection
{
public:
	CHeAutoCriticalSection()
	{
		HeThrowIfFailed( CHeCriticalSection::Init() );
	}

	~CHeAutoCriticalSection() throw()
	{
		(void) CHeCriticalSection::Term();
	}

private:
	HeResult Init();	// Not implemented to prevent use.
	HeResult Term();	// Not implemented to prevent use.
};

template <class T>
class CHeCritSecLock
{
public:
	CHeCritSecLock( T* pT )
	{
		if ( pT != nullptr )
		{
			pT->Lock();
		}

		m_pT = pT;
	}

	~CHeCritSecLock()
	{
		if ( m_pT != nullptr )
		{
			m_pT->Unlock();
		}
	}

public:
	T*	m_pT;
};

} /* namespace HeLib */

#endif /* HLSYNC_H */
