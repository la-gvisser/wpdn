// =============================================================================
//	PeekLock.h
// =============================================================================
//	Copyright (c) 2010 WildPackets, Inc. All rights reserved.

#pragma once

#include "Peek.h"


/////////////////////////////////////////////////////////////////////////////
// CPeekCriticalSection

class CPeekCriticalSection
{

protected:
	CRITICAL_SECTION m_sect;

public:
	;		CPeekCriticalSection()  { ::InitializeCriticalSection(&m_sect); }
	virtual ~CPeekCriticalSection() { ::DeleteCriticalSection(&m_sect); }

	CPeekCriticalSection&	operator=( const CPeekCriticalSection& inOther ) {
		if ( &inOther != this ) {
			memcpy( &m_sect, &inOther.m_sect, sizeof( CRITICAL_SECTION ) );
		}
		return *this;
	}

	bool Lock()   { ::EnterCriticalSection(&m_sect); return true; }
	bool Unlock() { ::LeaveCriticalSection(&m_sect); return true; }
};


class CPeekLock;

/////////////////////////////////////////////////////////////////////////////
// PeekLock
typedef class CPeekLock PeekLock;

/////////////////////////////////////////////////////////////////////////////
// CPeekLock

class CPeekLock
{
protected:
	CPeekCriticalSection&	m_Mutex;
	bool					m_bAcquired;

public:
	CPeekLock( CPeekCriticalSection& inMutex, bool bInitialLock = true ) 
		: m_Mutex( inMutex ), m_bAcquired( false) {
		if ( bInitialLock ) Lock();
	}
	~CPeekLock() { Unlock(); }

	CPeekLock&	operator=( const CPeekLock& inOther ) {
		if ( &inOther != this ) {
			m_Mutex = inOther.m_Mutex;
			m_bAcquired = inOther.m_bAcquired;
		}
	}

	bool Lock()   {
		ASSERT(!m_bAcquired);
		m_bAcquired = m_Mutex.Lock(); 
		return m_bAcquired;
	}

	bool Unlock() { 
		if ( m_bAcquired ) {
			m_bAcquired = !m_Mutex.Unlock();
		}
		return !m_bAcquired;
	}
};


/*
/////////////////////////////////////////////////////////////////////////////
// CPeekEvent

class CPeekEvent
{

protected:
	HANDLE  m_hEvent;

public:
	;		CPeekEvent(bool bInitiallyOwn = false, bool bManualReset = false,
					LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL) {
			m_hEvent = ::CreateEvent( lpsaAttribute, bManualReset,
			bInitiallyOwn, lpszNAme );
			if ( m_hEvent == NULL ) Peek::Throw( HE_E_FAIL );
	}
	virtual	~CPeekEvent() {}

	bool PulseEvent() { ASSERT(m_hEvent != NULL); return ( ::PulseEvent(m_hEvent) == TRUE ); }
	bool ResetEvent() { ASSERT(m_hEvent != NULL); return ( ::ResetEvent(m_hEvent) == TRUE ); }
	bool SetEvent()	  { ASSERT(m_hEvent != NULL); return ( ::SetEvent(m_hEvent) == TRUE ); }
};


/////////////////////////////////////////////////////////////////////////////
// CPeekMutex

class CPeekMutex
{
protected:
	HANDLE m_mtx;

public:
	CPeekMutex(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL,
		LPSECURITY_ATTRIBUTES lpsaAttribute = NULL) {
			m_mtx = ::CreateMutex( lpsaAttribute, bInitiallyOwn, lpszName );
	}

public:
	virtual ~CPeekMutex() { Unlock(); }

	void Lock() {
		DWORD dwWaitResult = ::WaitForSingleObject( m_mtx, INFINITE );
		dwWaitResult;
	}
	void Unlock() {
		::ReleaseMutex( m_mtx );
	}
};
*/
