// =============================================================================
//	hlsync.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

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
	HeResult Lock() throw()   { return HE_S_OK; }
	HeResult Unlock() throw() { return HE_S_OK; }
	HeResult Init() throw()   { return HE_S_OK; }
	HeResult Term() throw()   { return HE_S_OK; }
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

	HeResult Unlock() throw()
	{
		::LeaveCriticalSection( &m_Mutex );
		return HE_S_OK;
	}

	HeResult Init() throw()
	{
		HeResult	hr = HE_S_OK;

		__try
		{
			::InitializeCriticalSection( &m_Mutex );
		}
		__except ( STATUS_NO_MEMORY == GetExceptionCode() )
		{
			hr = HE_E_OUT_OF_MEMORY;
		}

		return hr;
	}
	
	HeResult Term() throw()
	{
		::DeleteCriticalSection( &m_Mutex );
		return HE_S_OK;
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
		pthread_mutex_lock( &m_Mutex );
		return HE_S_OK;
	}

	HeResult Unlock() throw()
	{
		pthread_mutex_unlock( &m_Mutex );
		return HE_S_OK;
	}

	HeResult Init() throw()
	{
		pthread_mutexattr_init( &m_MutexAttr );
		pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE );
		pthread_mutex_init( &m_Mutex, &m_MutexAttr );
		return HE_S_OK;
	}
	
	HeResult Term() throw()
	{
		pthread_mutexattr_destroy( &m_MutexAttr );
		pthread_mutex_destroy( &m_Mutex );
		return HE_S_OK;
	}

private:
	pthread_mutex_t		m_Mutex;
	pthread_mutexattr_t	m_MutexAttr;
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
		if ( pT != NULL )
		{
			pT->Lock();
		}
		
		m_pT = pT;
	}

	~CHeCritSecLock()
	{
		if ( m_pT != NULL )
		{
			m_pT->Unlock();
		}
	}

public:
	T*	m_pT;
};

} /* namespace HeLib */

#endif /* HLSYNC_H */
