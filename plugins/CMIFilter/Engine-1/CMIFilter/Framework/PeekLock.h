// =============================================================================
//	PeekLock.h
// =============================================================================
//	Copyright (c) 2010 WildPackets, Inc. All rights reserved.

#pragma once

#include "Peek.h"
#include <hlsync.h>


/////////////////////////////////////////////////////////////////////////////
// CPeekCriticalSection

class CPeekCriticalSection
{

protected:
	HeLib::CHeCriticalSection	m_sect;

public:
	;		CPeekCriticalSection()  { m_sect.Init(); }
	virtual ~CPeekCriticalSection() { m_sect.Term(); }

	CPeekCriticalSection&	operator=( const CPeekCriticalSection& inOther ) {
		if ( &inOther != this ) {
			m_sect = inOther.m_sect;
		}
		return *this;
	}

	bool Lock()   { m_sect.Lock(); return true; }
	bool Unlock() { m_sect.Unlock(); return true; }
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
		return *this;
	}

	bool Lock()   {
		_ASSERTE( !m_bAcquired );
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
					LPCTSTR lpszNAme = nullptr, LPSECURITY_ATTRIBUTES lpsaAttribute = nullptr) {
			m_hEvent = ::CreateEvent( lpsaAttribute, bManualReset,
			bInitiallyOwn, lpszNAme );
			if ( m_hEvent == nullptr ) Peek::Throw( HE_E_FAIL );
	}
	virtual	~CPeekEvent() {}

	bool PulseEvent() { _ASSERTE(m_hEvent != nullptr); return ( ::PulseEvent(m_hEvent) == TRUE ); }
	bool ResetEvent() { _ASSERTE(m_hEvent != nullptr); return ( ::ResetEvent(m_hEvent) == TRUE ); }
	bool SetEvent()	  { _ASSERTE(m_hEvent != nullptr); return ( ::SetEvent(m_hEvent) == TRUE ); }
};


/////////////////////////////////////////////////////////////////////////////
// CPeekMutex

class CPeekMutex
{
protected:
	HANDLE m_mtx;

public:
	CPeekMutex(BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = nullptr,
		LPSECURITY_ATTRIBUTES lpsaAttribute = nullptr) {
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
