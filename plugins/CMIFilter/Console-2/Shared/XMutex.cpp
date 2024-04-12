// =============================================================================
//	XMutex.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2001-2004. All rights reserved.

#include "stdafx.h"
#include "XMutex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//--------------------------------------------------

XMutex::XMutex()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_Mutex);
#else
	pthread_mutex_init(&m_Mutex, NULL);
#endif

}

//--------------------------------------------------

XMutex::~XMutex()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutex_destroy(&m_Mutex);
#endif
}



//--------------------------------------------------

XLock::XLock(XMutex* pMutex, bool bLock)
{
	m_pMutex = pMutex;
	m_bLocked = false;

	if (bLock)
	{
		Lock();
	}
}

//--------------------------------------------------

XLock::~XLock()
{
	if (m_bLocked)
	{
		Unlock();
	}
}

//--------------------------------------------------

#ifdef _WIN32

void XLock::Lock()
{
	EnterCriticalSection(m_pMutex->GetMutex());
	m_bLocked = true;
}

//--------------------------------------------------

void XLock::Unlock()
{
	m_bLocked = false;
	LeaveCriticalSection(m_pMutex->GetMutex());
}



#else

void XLock::Lock()
{
	//pthread_mutex_lock(m_pMutex->GetMutex());
	m_bLocked = true;
}


//--------------------------------------------------

void XLock::Unlock()
{
	m_bLocked = false;
	//pthread_mutex_unlock(m_pMutex->GetMutex());
}


#endif

