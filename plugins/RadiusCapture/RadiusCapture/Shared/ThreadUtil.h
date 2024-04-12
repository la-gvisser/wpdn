// ============================================================================
//	ThreadUtil.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#pragma once

#include "AGTypes.h"

#ifndef INFINITE
#define INFINITE	0xFFFFFFFF
#endif

// ============================================================================
//	CSyncObjectBase
// ============================================================================

class DECLSPEC CSyncObjectBase
{
	friend class CSingleLockEx;
//	friend class CMultiLockEx;
public:
					CSyncObjectBase() {}
	virtual			~CSyncObjectBase() {}

	virtual	bool	Lock( UInt32 inTimeOut = INFINITE ) = 0;
	virtual bool	Unlock() = 0;
};


// ============================================================================
//	CCriticalSectionBase
// ============================================================================

class DECLSPEC CCriticalSectionBase : public CSyncObjectBase
{
public:
					CCriticalSectionBase() {}
	virtual			~CCriticalSectionBase() {}

	virtual bool	Lock() = 0;
	virtual bool	Lock( UInt32 /*inTimeOut = INFINITE*/ ) { return Lock(); }
	virtual bool	Unlock() = 0;
};


// ============================================================================
//	CSingleLockEx
// ============================================================================

class DECLSPEC CSingleLockEx
{
public:
	CSingleLockEx( CSyncObjectBase* inSyncObject, bool bInitialLock );
	~CSingleLockEx();

	bool	Lock( UInt32 inTimeOut = INFINITE );
	bool	Unlock();
	bool	IsLocked() const { return m_bLocked; }

protected:
	CSyncObjectBase*	m_pSyncObject;
	bool				m_bLocked;

private:
	CSingleLockEx() {}
};


// ----------------------------------------------------------------------------
//		CSingleLockEx(CSyncObjectBase*,bool)
// ----------------------------------------------------------------------------

inline
CSingleLockEx::CSingleLockEx(
	CSyncObjectBase*	inSyncObject,
	bool				bInitialLock )
		: m_pSyncObject( inSyncObject ),
			m_bLocked( false )
{
	ASSERT( inSyncObject != NULL );

	if ( bInitialLock )
	{
		Lock();
	}
}


// ----------------------------------------------------------------------------
//		~CSingleLockEx
// ----------------------------------------------------------------------------

inline
CSingleLockEx::~CSingleLockEx()
{
	Unlock();
}


// ----------------------------------------------------------------------------
//		Lock
// ----------------------------------------------------------------------------

inline bool
CSingleLockEx::Lock(
	UInt32	inTimeOut )
{
	// Sanity check.
	ASSERT( m_pSyncObject != NULL );
	if ( m_pSyncObject == NULL ) return false;

	// Lock the sync object.
	m_bLocked = m_pSyncObject->Lock( inTimeOut );

	return m_bLocked;
}


// ----------------------------------------------------------------------------
//		Unlock
// ----------------------------------------------------------------------------

inline bool
CSingleLockEx::Unlock()
{
	// Sanity check.
	ASSERT( m_pSyncObject != NULL );
	if ( m_pSyncObject == NULL ) return false;

	if ( m_bLocked )
	{
		// Unlock the sync object.
		m_bLocked = !m_pSyncObject->Unlock();
	}

	return !m_bLocked;
}


#if TARGET_OS_WIN32

#include <process.h>

// ============================================================================
//	CWin32CriticalSection
// ============================================================================

class CWin32CriticalSection : public CCriticalSectionBase
{
public:
	CWin32CriticalSection();
	~CWin32CriticalSection();

	bool	Lock();
	bool	Unlock();

protected:
	CRITICAL_SECTION	m_CritSection;
};


// ----------------------------------------------------------------------------
//		CWin32CriticalSection
// ----------------------------------------------------------------------------

inline
CWin32CriticalSection::CWin32CriticalSection()
{
	::InitializeCriticalSection( &m_CritSection );
}


// ----------------------------------------------------------------------------
//		~CWin32CriticalSection
// ----------------------------------------------------------------------------

inline
CWin32CriticalSection::~CWin32CriticalSection()
{
	::DeleteCriticalSection( &m_CritSection );
}


// ----------------------------------------------------------------------------
//		Lock
// ----------------------------------------------------------------------------

inline bool
CWin32CriticalSection::Lock()
{
	::EnterCriticalSection( &m_CritSection );
	return true;
}


// ----------------------------------------------------------------------------
//		Unlock
// ----------------------------------------------------------------------------

inline bool
CWin32CriticalSection::Unlock()
{
	::LeaveCriticalSection( &m_CritSection );
	return true;
}

typedef CWin32CriticalSection CCriticalSectionEx;


// ============================================================================
//	CThread
// ============================================================================

class CThread
{
public:
				CThread() throw();
				CThread( CThread& t ) throw();
	explicit	CThread( HANDLE hThread ) throw();
	virtual		~CThread() throw();

	CThread&	operator=( CThread& t ) throw();

	void		Attach( HANDLE hThread ) throw();
	HANDLE		Detach() throw();

	operator	HANDLE() const throw();

	DWORD		Start() throw();
	DWORD		Suspend() throw();
	DWORD		Resume() throw();

	int			GetPriority() const throw();
	bool		SetPriority( int nPriority ) throw();

	UInt32		WaitForExit( UInt32 nMilliseconds = INFINITE ) throw();

	UInt32		GetExitCode() const throw();

protected:
	HANDLE		m_hThread;
	DWORD		m_dwThreadId;

	virtual DWORD		Create() throw();
	void				Close() throw();
	virtual UInt32		Run() = 0;

private:
	static UINT WINAPI	Execute( void* lpThreadParameter );
};


// ----------------------------------------------------------------------------
//		CThread
// ----------------------------------------------------------------------------

inline
CThread::CThread()
	: m_hThread( NULL ),
		m_dwThreadId( 0 )
{
}


// ----------------------------------------------------------------------------
//		CThread(CThread&)
// ----------------------------------------------------------------------------

inline
CThread::CThread(
	CThread&	t ) throw()
		: m_hThread( NULL ),
			m_dwThreadId( 0 )
{
	Attach( t.Detach() );
}


// ----------------------------------------------------------------------------
//		CThread(HANDLE)
// ----------------------------------------------------------------------------

inline
CThread::CThread(
	HANDLE	hThread ) throw()
		: m_hThread( hThread ),
			m_dwThreadId( 0 )
{
}


// ----------------------------------------------------------------------------
//		~CThread
// ----------------------------------------------------------------------------

inline
CThread::~CThread()
{
	Close();
}


// ----------------------------------------------------------------------------
//		operator=
// ----------------------------------------------------------------------------

inline CThread&
CThread::operator=(
	CThread&	rhs ) throw()
{
	if ( this != &rhs )
	{
		Close();
		Attach( rhs.Detach() );
	}

	return *this;
}


// ----------------------------------------------------------------------------
//		operator HANDLE
// ----------------------------------------------------------------------------

inline
CThread::operator HANDLE() const throw()
{
	return m_hThread;
}


// ----------------------------------------------------------------------------
//		Attach
// ----------------------------------------------------------------------------

inline void
CThread::Attach(
	HANDLE	hThread ) throw()
{
	ASSERT( m_hThread == NULL );
	m_hThread = hThread;	// Take ownership.
}


// ----------------------------------------------------------------------------
//		Detach
// ----------------------------------------------------------------------------

inline HANDLE
CThread::Detach() throw()
{
	HANDLE	hThread;
	hThread = m_hThread;	// Release ownership.
	m_hThread = NULL;
	return hThread;
}


// ----------------------------------------------------------------------------
//		Start
// ----------------------------------------------------------------------------

inline DWORD
CThread::Start() throw()
{
	// Close the previous thread handle.
	Close();

	// Create the thread.
	DWORD	dwResult = Create();
	if ( dwResult != ERROR_SUCCESS )
	{
		return dwResult;
	}

	// Start the thread running.
	return Resume();
}


// ----------------------------------------------------------------------------
//		Suspend
// ----------------------------------------------------------------------------

inline DWORD
CThread::Suspend() throw()
{
	// Sanity check.
	ASSERT( m_hThread != NULL );
	if ( m_hThread == NULL ) return ERROR_INVALID_HANDLE;

	// Suspend the thread.
	return ::SuspendThread( m_hThread );
}


// ----------------------------------------------------------------------------
//		Resume
// ----------------------------------------------------------------------------

inline DWORD
CThread::Resume() throw()
{
	// Sanity check.
	ASSERT( m_hThread != NULL );
	if ( m_hThread == NULL ) return ERROR_INVALID_HANDLE;

	// Resume the thread.
	return ::ResumeThread( m_hThread );
}


// ----------------------------------------------------------------------------
//		GetPriority
// ----------------------------------------------------------------------------

inline int
CThread::GetPriority() const throw()
{
	// Sanity check.
	ASSERT( m_hThread != NULL );
	if ( m_hThread == NULL ) return 0;

	// Get the thread priority.
	return ::GetThreadPriority( m_hThread );
}


// ----------------------------------------------------------------------------
//		SetPriority
// ----------------------------------------------------------------------------

inline bool
CThread::SetPriority(
	int	nPriority ) throw()
{
	// Sanity check.
	ASSERT( m_hThread != NULL );
	if ( m_hThread == NULL ) return false;

	// Set the thread priority.
	return (::SetThreadPriority( m_hThread, nPriority ) ? true : false);
}


// ----------------------------------------------------------------------------
//		WaitForExit
// ----------------------------------------------------------------------------

inline UInt32
CThread::WaitForExit(
	UInt32	nMilliseconds ) throw()
{
	// Sanity check.
	ASSERT( m_hThread != NULL );
	if ( m_hThread == NULL ) return ERROR_INVALID_HANDLE;

	// Wait on the thread handle.
	if ( ::WaitForSingleObject( m_hThread, nMilliseconds ) == WAIT_OBJECT_0 )
	{
		// Return the thread exit code.
		return GetExitCode();
	}

	return ERROR_TIMEOUT;
}


// ----------------------------------------------------------------------------
//		GetExitCode
// ----------------------------------------------------------------------------

inline DWORD
CThread::GetExitCode() const throw()
{
	// Sanity check.
	ASSERT( m_hThread != NULL );
	if ( m_hThread == NULL ) return ERROR_INVALID_HANDLE;

	// Get the thread exit code.
	DWORD	dwExitCode = 0;
	::GetExitCodeThread( m_hThread, &dwExitCode );
	return dwExitCode;
}


// ----------------------------------------------------------------------------
//		Create
// ----------------------------------------------------------------------------

inline DWORD
CThread::Create() throw()
{
	// Create the thread.
	m_hThread = (HANDLE) _beginthreadex(
		NULL,							// Default security.
		0,								// Default stack size.
		Execute,						// Static run function.
		this,							// Parameters to run function.
		CREATE_SUSPENDED,				// Start suspended.
		(unsigned*) &m_dwThreadId );	// Thread id.

	if ( (m_hThread == NULL) || (m_hThread == (void*)-1) )
	{
		m_hThread = NULL;
		m_dwThreadId = 0;
		return ::GetLastError();
	}

	return ERROR_SUCCESS;
}


// ----------------------------------------------------------------------------
//		Close
// ----------------------------------------------------------------------------

inline void
CThread::Close() throw()
{
	if ( m_hThread != NULL )
	{
		::CloseHandle( m_hThread );
		m_hThread = NULL;
		m_dwThreadId = 0;
	}
}


// ----------------------------------------------------------------------------
//		Execute [static]
// ----------------------------------------------------------------------------

inline UINT WINAPI
CThread::Execute(
	void*	lpThreadParameter )
{
	// Get the thread.
	CThread*	pThread = static_cast<CThread*>(lpThreadParameter);
	ASSERT( pThread != NULL );
	if ( pThread == NULL ) return ERROR_INVALID_PARAMETER;

	// Run the thread.
	return (UINT) pThread->Run();
}


#elif TARGET_OS_MAC

// ============================================================================
//	CMacOSCriticalSection
// ============================================================================

class CMacOSCriticalSection : public CCriticalSectionBase
{
public:
	CMacOSCriticalSection() {}
	~CMacOSCriticalSection() {}

	bool	Lock() { return true; }
	bool	Unlock() { return true; }
};

typedef CMacOSCriticalSection CCriticalSectionEx;

#else
#pragma message( "Target OS not supported" );
#endif
