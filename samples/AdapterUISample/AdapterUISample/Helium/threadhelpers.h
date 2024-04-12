// =============================================================================
//	threadhelpers.h
// =============================================================================
//	Copyright (c) 2005-2013 WildPackets, Inc. All rights reserved.
//
//	Dependencies:
//		OS and Standard C++ library only

#ifndef THREADHELPERS_H
#define THREADHELPERS_H

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <signal.h>
#include <sys/time.h>
#ifdef __GLIBC__
#include <sys/sysinfo.h>
#endif
#include <unistd.h>
#endif
#include <algorithm>
#include <list>
#include <queue>
#include <vector>
#include <stdexcept>

namespace ThreadHelpers
{

#ifdef _WIN32

#pragma warning(push)
// Warning C4706: Exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER.
// This might mask exceptions that were not intended to be handled
#pragma warning(disable:6320)
// Warning C6322: Empty _except block
#pragma warning(disable:6322)
// Warning C6312: Possible infinite loop: use of the constant EXCEPTION_CONTINUE_EXECUTION 
// in the exception-filter expression of a try-except. Execution restarts in the protected block
#pragma warning(disable:6312)

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD	dwType;		// must be 0x00001000
	LPCSTR	pszName;	// pointer to name (in user addr space)
	DWORD	dwThreadID;	// thread ID (-1=caller thread)
	DWORD	dwFlags;	// reserved for future use, must be zero
} THREADNAME_INFO;
#pragma pack(pop)

// See: https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
// Should be called from the thread being named. See: 
// https://randomascii.wordpress.com/2015/10/26/thread-naming-in-windows-time-for-something-better/
inline void SetThreadName( DWORD dwThreadID, LPCSTR pszThreadName )
{
	__try
	{
		THREADNAME_INFO	info;
		info.dwType     = 0x00001000;
		info.pszName    = pszThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags    = 0;
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*) &info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

#pragma warning(pop)

#endif	/* _WIN32 */

/// Sleep for n milliseconds.
inline void Sleep( unsigned long nMilliseconds ) throw()
{
#ifdef _WIN32
	::Sleep( nMilliseconds );
#else
	usleep( static_cast<unsigned int>(nMilliseconds * 1000) );
#endif
}

#ifdef _MSC_VER
#pragma push_macro("Yield")
#undef Yield
#endif /* _MSC_VER */

/// Yield the processor.
inline void Yield() throw()
{
#ifdef _WIN32
	::SwitchToThread();
#else
	sched_yield();
#endif
}

#ifdef _MSC_VER
#pragma pop_macro("Yield")
#endif /* _MSC_VER */

/// Get the number of processors available.
inline unsigned int GetHardwareConcurrency()
{
#if defined(_WIN32)
	SYSTEM_INFO	si = {};
	::GetSystemInfo( &si );
	return static_cast<unsigned int>(si.dwNumberOfProcessors);
#elif defined(_SC_NPROCESSORS_ONLN)
	const long count = sysconf( _SC_NPROCESSORS_ONLN );
	return (count > 0) ? static_cast<unsigned int>(count) : 0;
#elif defined(__GLIBC__)
	return static_cast<unsigned int>(get_nprocs());
#endif
}

/// Get the current thread ID.
inline unsigned int GetCurrentThreadID() throw()
{
#ifdef _WIN32
	return static_cast<unsigned int>(::GetCurrentThreadId());
#else
	return static_cast<unsigned int>(syscall( __NR_gettid ));
#endif
}

inline int SetCoreAffinity( int core )
{
#ifdef _WIN32
	if ( core >= 0 )
	{
		const DWORD_PTR	mask = static_cast<DWORD_PTR>(1) << core;
		return (::SetThreadAffinityMask( ::GetCurrentThread(), mask ) != 0) ? 0 : -1;
	}
	return 0;
#else
	cpu_set_t	cs;
	CPU_ZERO( &cs );
	if ( core >= 0 )
	{
		CPU_SET( core, &cs );
	}
	else
	{
		for ( int i = 0; i < static_cast<int>(GetHardwareConcurrency()); ++i )
		{
			CPU_SET( i, &cs );
		}
	}
	return pthread_setaffinity_np( pthread_self(), sizeof(cs), &cs );
#endif
}

template <typename M>
class LockGuard
{
public:
	typedef M Mutex;

	explicit LockGuard( M& m ) : m_Mutex( m )
	{
		m_Mutex.Lock();
	}

	~LockGuard()
	{
		m_Mutex.Unlock();
	}

private:
	LockGuard( LockGuard& );
	LockGuard& operator=( LockGuard& );

private:
	M&	m_Mutex;
};

template <typename M>
class UnlockGuard
{
public:
	typedef M Mutex;

	explicit UnlockGuard( M& m ) : m_Mutex( m )
	{
		m_Mutex.Unlock();
	}

	~UnlockGuard()
	{
		m_Mutex.Lock();
	}

private:
	UnlockGuard( UnlockGuard& );
	UnlockGuard& operator=( UnlockGuard& );

private:
	M&	m_Mutex;
};

#ifdef _WIN32

class Mutex
{
public:
	typedef LockGuard<Mutex> ScopedLock;
	typedef UnlockGuard<Mutex> ScopedUnlock;

	Mutex()
	{
		if ( !::InitializeCriticalSectionAndSpinCount( &m_Mutex, 0 ) )
		{
			throw std::runtime_error( "InitializeCriticalSectionAndSpinCount failed" );
		}
	}

	~Mutex()
	{
		::DeleteCriticalSection( &m_Mutex );
	}

	void Lock()
	{
		::EnterCriticalSection( &m_Mutex );
	}

	bool TryLock()
	{
		return (::TryEnterCriticalSection( &m_Mutex ) != FALSE);
	}

	void Unlock()
	{
		::LeaveCriticalSection( &m_Mutex );
	}

	typedef CRITICAL_SECTION* NativeHandle;
	NativeHandle GetNativeHandle() throw()
	{
		return &m_Mutex;
	}

private:
	Mutex( Mutex& );
	Mutex& operator=( Mutex& );

private:
	CRITICAL_SECTION	m_Mutex;
};

typedef Mutex RecursiveMutex;

#else

class Mutex
{
public:
	typedef LockGuard<Mutex> ScopedLock;
	typedef UnlockGuard<Mutex> ScopedUnlock;

	Mutex()
	{
		const int	res = pthread_mutex_init( &m_Mutex, NULL );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutex_init failed" );
		}
	}

	~Mutex()
	{
		pthread_mutex_destroy( &m_Mutex );
	}

	void Lock()
	{
		const int	res = pthread_mutex_lock( &m_Mutex );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutex_lock failed" );
		}
	}

	bool TryLock()
	{
		return (pthread_mutex_trylock( &m_Mutex ) == 0);
	}

	void Unlock()
	{
		const int	res = pthread_mutex_unlock( &m_Mutex );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutex_unlock failed" );
		}
	}

	typedef pthread_mutex_t* NativeHandle;
	NativeHandle GetNativeHandle() throw()
	{
		return &m_Mutex;
	}

private:
	Mutex( Mutex& );
	Mutex& operator=( Mutex& );

private:
	pthread_mutex_t		m_Mutex;
};

class RecursiveMutex
{
public:
	typedef LockGuard<RecursiveMutex> ScopedLock;
	typedef UnlockGuard<RecursiveMutex> ScopedUnlock;

	RecursiveMutex()
	{
		pthread_mutexattr_t	attr;
		int	res = pthread_mutexattr_init( &attr );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutexattr_init failed" );
		}
		res = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
		if ( res != 0 )
		{
			pthread_mutexattr_destroy( &attr );
			throw std::runtime_error( "pthread_mutexattr_settype failed" );
		}
		res = pthread_mutex_init( &m_Mutex, &attr );
		if ( res != 0 )
		{
			pthread_mutexattr_destroy( &attr );
			throw std::runtime_error( "pthread_mutex_init failed" );
		}
		pthread_mutexattr_destroy( &attr );
	}

	~RecursiveMutex()
	{
		pthread_mutex_destroy( &m_Mutex );
	}

	void Lock()
	{
		const int	res = pthread_mutex_lock( &m_Mutex );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutex_lock failed" );
		}
	}

	bool TryLock()
	{
		return (pthread_mutex_trylock( &m_Mutex ) == 0);
	}

	void Unlock()
	{
		const int	res = pthread_mutex_unlock( &m_Mutex );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutex_unlock failed" );
		}
	}

	typedef pthread_mutex_t* NativeHandle;
	NativeHandle GetNativeHandle() throw()
	{
		return &m_Mutex;
	}

private:
	RecursiveMutex( RecursiveMutex& );
	RecursiveMutex& operator=( RecursiveMutex& );

private:
	pthread_mutex_t		m_Mutex;
};

#endif

enum
{
	kWaitResult_Error	= -1,
	kWaitResult_Success	=  0,
	kWaitResult_Timeout	=  1
};

#ifndef _WIN32

class Condition
{
public:
	Condition()
	{
		const int	res = pthread_cond_init( &m_Cond, NULL );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_cond_init failed" );
		}
	}

	~Condition()
	{
		pthread_cond_destroy( &m_Cond );
	}

	void Signal()
	{
		const int	res = pthread_cond_signal( &m_Cond );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_cond_signal failed" );
		}
	}

	void Broadcast()
	{
		const int	res = pthread_cond_broadcast( &m_Cond );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_cond_broadcast failed" );
		}
	}

	int Wait( pthread_mutex_t* mutex, unsigned long nMilliseconds = 0 )
	{
		int	res = 0;
		errno = 0;
		if ( nMilliseconds == 0 )
		{
			res = pthread_cond_wait( &m_Cond, mutex );
		}
		else
		{
			// TODO: assumes it'll do the right thing if tv_nsec > 10^9.
			struct timeval	tv;
			gettimeofday( &tv, NULL );
			struct timespec	abs;
			abs.tv_sec  = tv.tv_sec + (nMilliseconds / 1000);
			abs.tv_nsec = (tv.tv_usec + (nMilliseconds % 1000) * 1000) * 1000;
			res = pthread_cond_timedwait( &m_Cond, mutex, &abs );
		}
		if ( res == 0 )
		{
			res = kWaitResult_Success;
		}
		else if ( (res == ETIMEDOUT) || (res == EAGAIN) )
		{
			res = kWaitResult_Timeout;
		}
		else
		{
			res = kWaitResult_Error;
		}
		return res;
	}

	template <typename M>
	int Wait( M& mutex, unsigned long nMilliseconds = 0 )
	{
		return Wait( mutex.GetNativeHandle(), nMilliseconds );
	}

private:
	pthread_cond_t	m_Cond;
};

#endif /* !_WIN32 */

/*********************************************
 * 
 * Event
 * 
 * This class basically implements a binary semaphore.
 * If the event is in the signalled state then Wait() returns immediately,
 * otherwise it blocks until the timeout expires or the Set() function is called.
 * On Windows it is implemented using CreateEvent/SetEvent.
 * On Linux it is implemented using mutex/condition variable
 * 
 *********************************************/
class Event
{
private:
#if defined(_WIN32)
	struct Impl
	{
	public:
		Impl()
		{
			m_Handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

			if (m_Handle == NULL)
			{
				throw std::runtime_error("Event handle initialization failed");
			}
		}

		~Impl()
		{
			::CloseHandle(m_Handle);
		}

		bool Set()
		{
			return (::SetEvent(m_Handle) != FALSE);
		}

		int Wait(unsigned long msTimeout)
		{
			if (msTimeout == 0)
			{
				msTimeout = INFINITE;
			}

			DWORD result = ::WaitForSingleObject(m_Handle, msTimeout);

			switch (result)
			{
			case WAIT_OBJECT_0: { return kWaitResult_Success; }
			case WAIT_TIMEOUT: { return kWaitResult_Timeout; }
			default: break;
			}

			return kWaitResult_Error;
		}

	public:
		HANDLE	m_Handle;
	};

#else
	struct Impl
	{
	public:
		Impl()
			:	m_Signalled(false)
		{
			if (pthread_mutex_init(&m_Mutex, NULL) != 0)
			{
				throw std::runtime_error("Mutex initialization failed");
			}

			if (pthread_cond_init(&m_Cond, NULL) != 0)
			{
				pthread_mutex_destroy(&m_Mutex);
				throw std::runtime_error("Condition variable initialization failed");
			}
		}

		~Impl()
		{
			pthread_cond_destroy(&m_Cond);
			pthread_mutex_destroy(&m_Mutex);
		}

		bool Set()
		{
			Lock();

			bool previousState = m_Signalled;

			m_Signalled = true;

			Unlock();

			int result = 0;

			if (!previousState)
			{
				result = pthread_cond_signal(&m_Cond);
			}

			return (result == 0);
		}

		int Wait(unsigned long msTimeout)
		{
			int result = 0;

			if (msTimeout == 0)
			{
				result = Wait();
			}
			else
			{
				result = TimedWait(msTimeout);
			}
			
			int status = kWaitResult_Error;

			switch (result)
			{
			case 0:
			{
				status = kWaitResult_Success;
			} break;
			case EAGAIN:
			case ETIMEDOUT:
			{
				status = kWaitResult_Timeout;
			} break;
			default:
			{
				status = kWaitResult_Error;
				
			} break;
			}

			return status;
		}

	private:
		void Lock()
		{
			pthread_mutex_lock(&m_Mutex);
		}

		void Unlock()
		{
			pthread_mutex_unlock(&m_Mutex);
		}

		int Wait()
		{
			Lock();

			int result = 0;
			while (!m_Signalled)
			{
				result = pthread_cond_wait(&m_Cond, &m_Mutex);

				if (result != 0)
				{
					break;
				}
			}

			if (result == 0)
			{
				m_Signalled = false;
			}

			Unlock();

			return result;
		}

		int TimedWait(unsigned long msTimeout)
		{
			static const unsigned long MILLI_PER_SEC = 1000;						// milliseconds per second
			static const unsigned long NANO_PER_MICRO = 1000;						// nanoseconds per microsecond
			static const unsigned long NANO_PER_MILLI = NANO_PER_MICRO * 1000;		// nanoseconds per millisecond
			static const unsigned long NANO_PER_SEC = NANO_PER_MILLI * 1000;		// nanoseconds per second

			//pthread_cond_timedwait fails if tv_nsec is >= 1,000,000,000
			//split timeout value into seconds/nanoseconds
			unsigned long seconds = msTimeout / MILLI_PER_SEC;
			unsigned long nanoseconds = (msTimeout % MILLI_PER_SEC) * NANO_PER_MILLI;

			struct timespec abstime = {};
			clock_gettime(CLOCK_REALTIME, &abstime);

			abstime.tv_sec += seconds;
			abstime.tv_nsec += nanoseconds;

			if(abstime.tv_nsec >= static_cast<time_t>(NANO_PER_SEC))
			{
				abstime.tv_sec += (abstime.tv_nsec / NANO_PER_SEC);
				abstime.tv_nsec = abstime.tv_nsec % NANO_PER_SEC;
			}
			
			Lock();

			int result = 0;
			while (!m_Signalled)
			{
				result = pthread_cond_timedwait(&m_Cond, &m_Mutex, &abstime);

				if (result != 0)
				{
					break;
				}
			}

			if (result == 0)
			{
				m_Signalled = false;
			}

			Unlock();

			return result;
		}

	public:
		pthread_cond_t		m_Cond;
		pthread_mutex_t		m_Mutex;
		bool				m_Signalled;
	};

#endif

public:
	Event() throw() :
		m_pImpl(nullptr)
	{
	}

	~Event() throw()
	{
		Close();
	}

	bool IsValid() const throw()
	{
		return (m_pImpl != nullptr);
	}

	void Close() throw()
	{
		if (m_pImpl != nullptr)
		{
			delete m_pImpl;
			m_pImpl = nullptr;
		}
	}

	bool Create()
	{
#ifdef ASSERT
		ASSERT(!IsValid());
#endif

		if (IsValid())
		{
			return false;
		}

		try
		{
			m_pImpl = new Impl();
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	bool Set()
	{
#ifdef ASSERT
		ASSERT(IsValid());
#endif
		if (!IsValid()) return false;

		return m_pImpl->Set();
	}

	/// \param nMilliseconds 0=infinite wait
	/// \retval 0=success, 1=timeout, -1=error.
	int Wait(unsigned long nMilliseconds = 0)
	{
#ifdef ASSERT
		ASSERT(IsValid());
#endif
		if (!IsValid()) return kWaitResult_Error;

		return m_pImpl->Wait(nMilliseconds);
	}
	
private:
	Impl*	m_pImpl;
};

// See: "Supporting Threads in Standard C++"
//		Part 1: http://accu.org/index.php/journals/508
//		Part 2: http://accu.org/index.php/journals/498
//		Part 2: http://accu.org/index.php/journals/490
//		Addendum: http://accu.org/index.php/journals/484
// This "is Thread-Runs-Polymorphic-Object"
// Also similar to ATL::CWorkerThread and IWorkerThreadClient

class Thread;
class IThreadFunction;

/// Derive your class from this to provide a concrete implementation.
class IThreadFunction
{
public:
	virtual ~IThreadFunction() {}

	virtual int Run( Thread* pThread ) = 0;
};

/// Creates a platform-specific thread and invokes the thread function.
class Thread
{
public:
#ifdef _WIN32
	typedef HANDLE NativeHandle;
#else
	typedef pthread_t NativeHandle;
#endif

	Thread() throw() :
		m_pFunc( NULL ),
		m_ExitCode( 0 ),
		m_nCore( -1 ),
		m_ThreadID( 0 ),
		m_hThread( 0 )
	{
	}

	~Thread() throw()
	{
		(void) Join();
	}

	operator bool() const throw()
	{
		return IsAlive();
	}

	bool IsAlive() const throw()
	{
		return (m_hThread != 0) ? true : false;
	}

	int GetExitCode() const
	{
		return m_ExitCode;
	}

	unsigned int GetThreadID() const
	{
		return m_ThreadID;
	}

	bool Create( IThreadFunction* pFunc, size_t cbStack = 0, const char* pszThreadName = NULL ) throw()
	{
		m_pFunc       = pFunc;
		m_ThreadID    = 0;
		cbStack       = cbStack;		// Avoid unused param warnings.
		pszThreadName = pszThreadName;	// Avoid unused param warnings.

#ifdef _WIN32
		if ( m_hThread != NULL )
		{
			::CloseHandle( m_hThread );
			m_hThread = NULL;
		}

		// Note: stack size intentionally ignored.
		m_hThread = (HANDLE) _beginthreadex( NULL, 0,
			ThreadFunc_, this, 0, &m_ThreadID );
		if ( m_hThread == NULL ) return false;

#ifdef _DEBUG
		if ( pszThreadName != NULL )
		{
			// Set the thread name so we can see it in the debugger.
			// Note: this may not work when called from here. See:
			// https://randomascii.wordpress.com/2015/10/26/thread-naming-in-windows-time-for-something-better/
			SetThreadName( m_ThreadID, pszThreadName );
		}
#endif
#else
		if ( m_hThread != 0 )
		{
			pthread_detach( m_hThread );
			m_hThread  = 0;
		}

		// Set up the thread attributes.
		pthread_attr_t*		pAttr = NULL;
		pthread_attr_t		attr;
		if ( cbStack > 0 )
		{
			pAttr = &attr;
			pthread_attr_init( pAttr );
			pthread_attr_setstacksize( pAttr, cbStack );
		}

		// Create the thread.
		const int	res = pthread_create( &m_hThread, pAttr,
						(void *(*)(void*)) ThreadFunc_, this );

		// Clean up the thread attributes.
		if ( pAttr != NULL )
		{
			pthread_attr_destroy( &attr );
		}

		// Check the result.
		if ( res != 0 ) return false;

		// Store the thread id.
		m_ThreadID = static_cast<unsigned int>(m_hThread);
#endif

		return true;
	}

	/// \param nMilliseconds 0=infinite wait
	/// \retval false if an error occurred
	bool Join( unsigned long nMilliseconds = 0 ) throw()
	{
#ifdef _WIN32
		bool	bResult = true;
		if ( m_hThread != NULL )
		{
			if ( nMilliseconds == 0 ) nMilliseconds = INFINITE;
			if ( ::WaitForSingleObject( m_hThread, nMilliseconds ) == WAIT_OBJECT_0 )
			{
				DWORD	dwExitCode = 0;
				if ( ::GetExitCodeThread( m_hThread, &dwExitCode ) )
				{
					m_ExitCode = static_cast<int>(dwExitCode);
				}
			}
			else
			{
				bResult    = false;
				m_ExitCode = 0x80070103;	// STILL_ACTIVE as HRESULT.
			}

			::CloseHandle( m_hThread );
			m_hThread = NULL;
		}

		return bResult;
#else
		nMilliseconds = nMilliseconds;	// Avoid unused param warnings.

		bool	bResult = true;
		if ( m_hThread != 0 )
		{
			void*	exit_code = 0;
			bResult    = (pthread_join( m_hThread, &exit_code ) == 0);
			m_ExitCode = (int) (intptr_t) exit_code;
			m_hThread  = 0;
			m_nCore    = -1;
		}

		return bResult;
#endif
	}

	void Detach() throw()
	{
#ifdef _WIN32
		if ( m_hThread != NULL )
		{
			::CloseHandle( m_hThread );
			m_hThread = NULL;
		}
#else
		if ( m_hThread != 0 )
		{
			pthread_detach( m_hThread );
			m_hThread  = 0;
			m_nCore    = -1;
		}
#endif
	}

#ifndef _WIN32
	void Kill( int signo ) throw()
	{
		if ( m_hThread != 0 )
		{
			pthread_kill( m_hThread, signo );
		}
	}
#endif

	int GetCore() throw()
	{
		return m_nCore;
	}

	void SetCore( int nCore ) throw()
	{
		m_nCore = nCore;
	}

	static void UseCore( int nCore ) throw()
	{
#ifdef ASSERT
		ASSERT( (nCore >= 0) && (nCore < static_cast<int>(GetCores().size())) );
#endif
		if ( (nCore >= 0) && (nCore < static_cast<int>(GetCores().size())) )
		{
			GetCores()[nCore]++;
		}
	}

	static void FreeCore( int nCore ) throw()
	{
#ifdef ASSERT
		ASSERT( (nCore >= 0) && (nCore < static_cast<int>(GetCores().size())) );
#endif
		if ( (nCore >= 0) && (nCore < static_cast<int>(GetCores().size())) )
		{
			GetCores()[nCore]--;
		}
	}

	static int AssignCore() throw()
	{
		int	nCore = -1;

		const int	nCoreCount = static_cast<int>(GetCores().size());
		for ( int i = 0; i < nCoreCount; ++i )
		{
			if ( GetCores()[i] == 0 )
			{
				GetCores()[i]++;
				nCore = i;
				break;
			}
		}

		return nCore;
	}

	NativeHandle GetNativeHandle() throw()
	{
		return m_hThread;
	}

private:
#ifdef _WIN32
	static UINT WINAPI ThreadFunc_( void* p )
	{
		Thread*	pThis = static_cast<Thread*>(p);
		SetCoreAffinity( pThis->GetCore() );
		return static_cast<UINT>(pThis->m_pFunc->Run( pThis ));
	}
#else
	static void* ThreadFunc_( void* p )
	{
		Thread*	pThis = static_cast<Thread*>(p);
		SetCoreAffinity( pThis->GetCore() );
		return reinterpret_cast<void*>(pThis->m_pFunc->Run( pThis ));
	}
#endif

	static std::vector<int>& GetCores()
	{
		static std::vector<int>	vCores( GetHardwareConcurrency() );
		return vCores;
	}

private:
	Thread( const Thread& );
	Thread& operator=( const Thread& );

private:
	IThreadFunction*	m_pFunc;
	int					m_ExitCode;
	int					m_nCore;
	unsigned int		m_ThreadID;
	NativeHandle		m_hThread;
};

class ThreadGroup
{
public:
	ThreadGroup()
	{
	}

	~ThreadGroup()
	{
		Clear();
	}

	void AddThread( Thread* pThread )
	{
		if ( pThread != NULL )
		{
			LockGuard<Mutex>	lock( m_Mutex );
			m_Threads.push_back( pThread );
		}
	}

	void RemoveThread( Thread* pThread )
	{
		LockGuard<Mutex>		lock( m_Mutex );
		ThreadList::iterator	end = m_Threads.end();
		ThreadList::iterator	it = std::find( m_Threads.begin(), end, pThread );
		if ( it != end )
		{
			m_Threads.erase( it );
		}
	}

	void JoinAll()
	{
		LockGuard<Mutex>	lock( m_Mutex );
		for ( ThreadList::iterator it = m_Threads.begin(), end = m_Threads.end();
			it != end; ++it )
		{
			(*it)->Join();
		}
	}

	void Clear()
	{
		for ( ThreadList::iterator it = m_Threads.begin(), end = m_Threads.end();
			it != end; ++it )
		{
			delete *it;
		}
		m_Threads.clear();
	}

	bool IsEmpty() const
	{
		LockGuard<Mutex>	lock( m_Mutex );
		return m_Threads.empty();
	}

	size_t GetSize() const
	{
		LockGuard<Mutex>	lock( m_Mutex );
		return m_Threads.size();
	}

private:
	ThreadGroup( const ThreadGroup& );
	ThreadGroup operator=( const ThreadGroup& );

private:
	typedef std::list<Thread*> ThreadList;
	ThreadList		m_Threads;
	mutable Mutex	m_Mutex;
};

template <typename T>
class BlockingQueue
{
public:
	BlockingQueue()
	{
#ifdef _WIN32
		if ( !::InitializeCriticalSectionAndSpinCount( &m_Mutex, 4000 ) )
		{
			throw std::runtime_error( "InitializeCriticalSectionAndSpinCount failed" );
		}
		m_Event = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		if ( m_Event == NULL )
		{
			::DeleteCriticalSection( &m_Mutex );
			throw std::runtime_error( "CreateEvent failed" );
		}
#else
		int	res = pthread_mutex_init( &m_Mutex, NULL );
		if ( res != 0 )
		{
			throw std::runtime_error( "pthread_mutex_init failed" );
		}
		res = pthread_cond_init( &m_Cond, NULL );
		if ( res != 0 )
		{
			pthread_mutex_destroy( &m_Mutex );
			throw std::runtime_error( "pthread_cond_init failed" );
		}
#endif
	}

	~BlockingQueue()
	{
#ifdef _WIN32
		::DeleteCriticalSection( &m_Mutex );
		::CloseHandle( m_Event );
#else
		pthread_mutex_destroy( &m_Mutex );
		pthread_cond_destroy( &m_Cond );
#endif
	}

	void Enqueue( T const & t )
	{
#if 1
#ifdef _WIN32
		::EnterCriticalSection( &m_Mutex );
		m_Items.push( t );
		::LeaveCriticalSection( &m_Mutex );
		::SetEvent( m_Event );
#else
		pthread_mutex_lock( &m_Mutex );
		m_Items.push( t );
		pthread_mutex_unlock( &m_Mutex );
		pthread_cond_signal( &m_Cond );
#endif
#else
		// Only signal if the queue is empty: more efficient for only
		// one consumer.
#ifdef _WIN32
		::EnterCriticalSection( &m_Mutex );
		const bool	bEmpty = m_Items.empty();
		m_Items.push( t );
		::LeaveCriticalSection( &m_Mutex );
		if ( bEmpty )
		{
			::SetEvent( m_Event );
		}
#else
		pthread_mutex_lock( &m_Mutex );
		const bool	bEmpty = m_Items.empty();
		m_Items.push( t );
		pthread_mutex_unlock( &m_Mutex );
		if ( bEmpty )
		{
			pthread_cond_signal( &m_Cond );
		}
#endif
#endif
	}

	T Dequeue()
	{
#ifdef _WIN32
		::EnterCriticalSection( &m_Mutex );
		while ( m_Items.empty() )
		{
			::LeaveCriticalSection( &m_Mutex );
			::WaitForSingleObject( m_Event, INFINITE );
			::EnterCriticalSection( &m_Mutex );
		}
		T	t = m_Items.front();
		m_Items.pop();
		::LeaveCriticalSection( &m_Mutex );
		return t;
#else
		pthread_mutex_lock( &m_Mutex );
		while ( m_Items.empty() )
		{
			pthread_cond_wait( &m_Cond, &m_Mutex );
		}
		T	t = m_Items.front();
		m_Items.pop();
		pthread_mutex_unlock( &m_Mutex );
		return t;
#endif
	}

	void Dequeue( T& t )
	{
#ifdef _WIN32
		::EnterCriticalSection( &m_Mutex );
		while ( m_Items.empty() )
		{
			::LeaveCriticalSection( &m_Mutex );
			::WaitForSingleObject( m_Event, INFINITE );
			::EnterCriticalSection( &m_Mutex );
		}
		t = m_Items.front();
		m_Items.pop();
		::LeaveCriticalSection( &m_Mutex );
#else
		pthread_mutex_lock( &m_Mutex );
		while ( m_Items.empty() )
		{
			pthread_cond_wait( &m_Cond, &m_Mutex );
		}
		t = m_Items.front();
		m_Items.pop();
		pthread_mutex_unlock( &m_Mutex );
#endif
	}

	bool TryDequeue( T& t, unsigned long timeout = 0)
	{
#ifdef _WIN32
		::EnterCriticalSection( &m_Mutex );
		while ( m_Items.empty() )
		{
			::LeaveCriticalSection( &m_Mutex );
			if (timeout == 0)
				return false;

			if(::WaitForSingleObject(m_Event, timeout) != WAIT_OBJECT_0)
				return false;
				
			::EnterCriticalSection(&m_Mutex);
			timeout = 0;
		}

		t = m_Items.front();
		m_Items.pop();
		::LeaveCriticalSection( &m_Mutex );
		return true;
#else
		pthread_mutex_lock( &m_Mutex );
		while ( m_Items.empty() )
		{		
			if (timeout == 0)
			{
				pthread_mutex_unlock( &m_Mutex );
				return false;
			}

			struct timespec timer = { 0, 0 };
			clock_gettime(CLOCK_REALTIME, &timer);
			timer.tv_sec += (timeout / 1000);
			timer.tv_nsec += ((timeout % 1000) * 1000000);
			
			pthread_cond_timedwait(&m_Cond, &m_Mutex, &timer);			
			timeout = 0;
		}
		t = m_Items.front();
		m_Items.pop();
		pthread_mutex_unlock( &m_Mutex );
		return true;
#endif
	}

	bool IsEmpty() const
	{
		return m_Items.empty();
	}

	size_t GetSize() const
	{
		return m_Items.size();
	}

public:
#ifdef _WIN32
	CRITICAL_SECTION	m_Mutex;
	HANDLE				m_Event;
#else
	pthread_mutex_t		m_Mutex;
	pthread_cond_t		m_Cond;
#endif
	std::queue<T>		m_Items;
};

} /* namespace ThreadHelpers */

#endif /* THREADHELPERS_H */
