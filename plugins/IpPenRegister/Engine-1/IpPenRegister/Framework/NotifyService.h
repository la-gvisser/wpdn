// =============================================================================
//	NotifyService.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "peekcore.h"
#include "Peek.h"
#include "GlobalId.h"
#include "PeekLock.h"
#include "PeekArray.h"
#include <memory>

// Severity levels for SetSeverityMask() function
#define ENABLE_NOTIFY_SEVERITY_NONE			  0x0
#define ENABLE_NOTIFY_SEVERITY_INFORMATIONAL  0x1
#define ENABLE_NOTIFY_SEVERITY_MINOR		  0x2
#define ENABLE_NOTIFY_SEVERITY_MAJOR		  0x4
#define ENABLE_NOTIFY_SEVERITY_SEVERE		  0x8
#define ENABLE_NOTIFY_SEVERITY_ALL			  0xF

// Severity source exclusions for SetSeveritySrcExclusions
#define NOTIFY_SOURCES_EXCLUDE_NONE				 0 // Received notifies from all sources

// The following always include sources that are not instances of the current plugin:
#define NOTIFY_SOURCES_EXCLUDE_CURRENT			 1 // Exclude notifies from this current instance of this plugin
#define NOTIFY_SOURCES_EXCLUDE_OTHER_INSTANCES	 2 // Exclude notifies from all instances of this plugin except this one
#define NOTIFY_SOURCES_EXCLUDE_ALL_INSTANCES	 3 // Exclude notifies from all instances of this plugin including this one

// The following always exclude sources that are not instances of the current plugin:
#define NOTIFY_SOURCES_INCLUDE_CURRENT			 4 // Include notifies only from the the current plugin instance
#define NOTIFY_SOURCES_INCLUDE_OTHER_INSTANCES	 5 // Include notifies only from other instances of this plugin
#define NOTIFY_SOURCES_INCLUDE_ALL_INSTANCES	 6 // Include notifies from all instances of this plugin including this one

const size_t kInitialSize = 0;
const size_t kGrowAmount  = 256;

class CRemotePlugin;


// =============================================================================
//		NotifyElement
// =============================================================================

typedef struct _NotifyElement {
	CGlobalId	fContextId;
	CGlobalId	fSourceId;
	UInt32		fSourceKey;
	UInt64		fTimeStamp;
	int			fSeverity;
	CPeekString	fShortMessage;
	CPeekString	fLongMessage;
} NotifyElement;

typedef std::shared_ptr<NotifyElement>	NotifyElementPtr;


// =============================================================================
//		NotifyArray
// =============================================================================

class NotifyArray
	:	public CPeekArray<NotifyElementPtr>
{
protected:
	size_t	m_nGrowBy;

public:
	;		NotifyArray() : m_nGrowBy( 0 ) {}

	void	Add( const NotifyElementPtr& inItem ) {
		if ( (size() + 1) > capacity() ) {
			reserve( capacity() + m_nGrowBy );
		}
		CPeekArray<NotifyElementPtr>::Add( inItem );
	}

	void	Initialize( size_t inSize, size_t inGrowBy ) {
		reserve( inSize );
		m_nGrowBy = inGrowBy;
	}
};


// =============================================================================
//		CNotifyQueue
// =============================================================================

class CNotifyQueue
{
protected:
	CHePtr<INotify>			m_spNotifyService;
	CPeekCriticalSection	m_Mutex;

	NotifyArray				m_Array1;
	NotifyArray				m_Array2;

	NotifyArray*			m_AddNotifyArrayPtr;
	NotifyArray*			m_SendNotifyArrayPtr;

public:
	;		CNotifyQueue()
		: m_AddNotifyArrayPtr( NULL )
		, m_SendNotifyArrayPtr( NULL )
	{
		m_Array1.Initialize( kInitialSize, kGrowAmount );
		m_Array2.Initialize( kInitialSize, kGrowAmount );
	}
	virtual ~CNotifyQueue() { Close(); }

	void	AddNotification( const CGlobalId& inContextId, const CGlobalId& inSourceId,
		UInt32 inSourceKey, UInt64 inTimeStamp, int inSeverity,
		const CPeekString& inShortMessage, const CPeekString& inLongMessage );

	void	Close();

	void	DoNotifies();

	bool	HasNotifies() {
		PeekLock lock( m_Mutex );
		return ( !m_AddNotifyArrayPtr->IsEmpty() );
	}

	void	ResetAddArray() {
		if ( m_AddNotifyArrayPtr != &m_Array1 ) {
			m_AddNotifyArrayPtr = &m_Array1;
			m_SendNotifyArrayPtr = &m_Array2;
		}
		else {
			m_AddNotifyArrayPtr = &m_Array2;
			m_SendNotifyArrayPtr = &m_Array1;
		}

		m_AddNotifyArrayPtr->RemoveAll();
	}

	void	SetPtr( INotify* inNotify ) {
		PeekLock lock( m_Mutex );
		m_spNotifyService = inNotify;  // Same as the CNotifyService interface pointer
	}
};


// =============================================================================
//		CNotifyService
// =============================================================================

class CNotifyService
{
	friend class CPeekProxy;
	friend class CPeekEngineProxy;

protected:
	HANDLE			m_hStopEvent;
	HANDLE			m_hThreadStartEvent;
	HANDLE			m_hSendThread;
	bool			m_bIsReceiveNotifies;

	// The notify interface pointer is replicated in the CNotifyQueue class.
	// Note: there is really only one notification service across all captures
	// and plugins.
	CHePtr<INotify>	m_spNotifyService;
	CHePtr<IAction>	m_spAction;
	CNotifyQueue	m_SendNotifyQueue;

	UInt8			m_SeverityMask;
	UInt8			m_SeverityExclusions;

protected:
	void	Close();
	void	CloseStopEvent() { if ( m_hStopEvent ) {::CloseHandle( m_hStopEvent ); m_hStopEvent = NULL; } }
	void	CloseThreadStartEvent() {
		if ( m_hThreadStartEvent ) {
			::CloseHandle( m_hThreadStartEvent );
			m_hThreadStartEvent = NULL;
		}
	}
	void	CloseSendThread() { if ( m_hSendThread ) {::CloseHandle( m_hSendThread ); m_hSendThread = NULL; } }

	INotify*	GetPtr() { return m_spNotifyService; }

	bool	IsStopRequested() {
		bool bRet = false;
		DWORD dwWait = ::WaitForSingleObject( m_hStopEvent, 0 );
		if ( dwWait == WAIT_OBJECT_0 ) {
			bRet = true;
		}
		return bRet;
	}

	UINT	RunSendThread();

	void	SetPtr( Helium::IHeServiceProvider* inServiceProvider,
					CRemotePlugin* inRemote );
	bool	StartSendNotifies();

public:
	;		CNotifyService()
		: m_hStopEvent( NULL )
		, m_hThreadStartEvent( NULL )
		, m_hSendThread( NULL )
		, m_bIsReceiveNotifies( false )
		, m_SeverityMask( ENABLE_NOTIFY_SEVERITY_ALL )
		, m_SeverityExclusions( NOTIFY_SOURCES_EXCLUDE_ALL_INSTANCES ) {}
	;		~CNotifyService() { Close(); }

	void	DoNotify( const CGlobalId& inContextId, const CGlobalId& inSourceId,
				UInt32 inSourceKey, UInt64 inTimeStamp, int inSeverity,
				const CPeekString& inShortMessage, const CPeekString& inLongMessage ) {
			if ( IsValid() ) {
				StartSendNotifies();	// Does nothing if already sending.
				m_SendNotifyQueue.AddNotification( inContextId, inSourceId, inSourceKey,
						inTimeStamp, inSeverity, inShortMessage, inLongMessage );
			}
	}
	bool	DoSetReceiveNotifications( bool inReceive );

	UInt8	GetSeveritySourceExclusions() {
		return m_SeverityExclusions;
	}

	bool	IsValid() const { return (m_spNotifyService != NULL ); }
	bool	IsReceiveNotifies() const { return m_bIsReceiveNotifies; }
	bool	IsSending() const { return ( m_hThreadStartEvent != NULL ); }
	BOOL	IsSeverityEnabled( PeekSeverity inSeverity ) {
		if ( inSeverity > 3 ) return FALSE;
		UInt8 nTestSeverity = 0x01 << inSeverity;
		return ( (nTestSeverity & m_SeverityMask) ? TRUE : FALSE );
	}

	static UINT WINAPI	SendThreadProc( LPVOID lpParameter );
	void	SetSeveritySrcExclusions( UInt8 inSeverityExlusions ) {
		m_SeverityExclusions = inSeverityExlusions;
	}
	void	SetSeverityMask( UInt8 inSeverityMask ) {
		if ( inSeverityMask  < 16 ) {
			m_SeverityMask = inSeverityMask;
		}
	}
	void	StopSendNotifies();
};
