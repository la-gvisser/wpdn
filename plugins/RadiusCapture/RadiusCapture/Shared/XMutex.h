// ============================================================================
//	XMutex.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2001-2004. All rights reserved.

#ifndef XMUTEX_H
#define XMUTEX_H

#include "XObject.h"


//-----------------------------------------------------------

#ifdef _WIN32
#define XMUTEX CRITICAL_SECTION
#else
#include <pthread.h>
#define XMUTEX pthread_mutex_t
#endif

// {2BE2BF7A-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XMutexID, 
0x2be2bf7a, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);


class DECLSPEC XMutex : 
	virtual public XObject
{
  public:

	X_BEGIN_ID_MAP(XMutex)
		X_ID_ENTRY(XMutexID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	X_DECLARE_CREATE( XMutex )

	XMutex(  );
	virtual ~XMutex();	
	
	XMUTEX	m_Mutex;
	XMUTEX* GetMutex() { return &m_Mutex; }
};

// {2BE2BF7b-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XLockID, 
0x2be2bf7b, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

class DECLSPEC XLock : 
	virtual public XObject
{
  public:

	X_BEGIN_ID_MAP(XLock)
		X_ID_ENTRY(XLockID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	XLock( XMutex* pMutex, bool bLock = false );
	virtual ~XLock();	
	virtual void Lock();
	virtual void Unlock();
	void SetMutex( XMutex* pMutex ) { m_pMutex = pMutex; }
	
	XMutex*	m_pMutex;
	bool	m_bLocked;
};

#endif 
