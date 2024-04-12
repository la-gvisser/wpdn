// =============================================================================
//	NotifyService.cpp
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "NotifyService.h"

#ifdef _WIN32
#include <process.h>
#endif

const UInt32	k_NotifySleepTime( 1000 );


// =============================================================================
//		CNotifyQueue
// =============================================================================

// ----------------------------------------------------------------------------
//		AddNotification
// ----------------------------------------------------------------------------

void
CNotifyQueue::AddNotification(
	const CGlobalId&	inContextId,
	const CGlobalId&	inSourceId,
	UInt32				inSourceKey,
	UInt64				inTimeStamp,
	int					inSeverity,
	const CPeekString&	inShortMessage,
	const CPeekString&	inLongMessage )
{
	PeekLock lock( m_Mutex );

	NotifyElementPtr spElement( new NotifyElement );

	if ( !spElement ) Peek::Throw( HE_E_OUT_OF_MEMORY );

	spElement->fContextId = inContextId;
	spElement->fSourceId = inSourceId;
	spElement->fSourceKey = inSourceKey;
	spElement->fTimeStamp = inTimeStamp;
	spElement->fSeverity = inSeverity;
	spElement->fShortMessage = inShortMessage;
	spElement->fLongMessage = inLongMessage;

	m_AddNotifyArrayPtr->Add( spElement );
}


// ----------------------------------------------------------------------------
//		Close
// ----------------------------------------------------------------------------

void
CNotifyQueue::Close()
{
	PeekLock lock( m_Mutex );

	for ( auto itr = m_Array1.begin(); itr != m_Array1.end(); itr++ ) {
		itr->reset();
	}
	m_Array1.clear();

	for ( auto itr = m_Array2.begin(); itr != m_Array2.end(); itr++ ) {
		itr->reset();
	}
	m_Array2.clear();

	m_spNotifyService.Release();
}


// ----------------------------------------------------------------------------
//		DoNotifies
// ----------------------------------------------------------------------------

void
CNotifyQueue::DoNotifies()
{
	{
		PeekLock lock( m_Mutex );

		if ( m_AddNotifyArrayPtr->size() == 0 ) return;
		_ASSERTE( m_AddNotifyArrayPtr != nullptr );

		ResetAddArray();
	}

	_ASSERTE( m_SendNotifyArrayPtr != nullptr );

	NotifyElementPtr spEmpty;

	for ( auto itr = m_SendNotifyArrayPtr->begin(); itr != m_SendNotifyArrayPtr->end(); itr++ ) {
		NotifyElementPtr	spElement = *itr;
		_ASSERTE( spElement );
		if ( !spElement ) continue;
		Peek::ThrowIfFailed( m_spNotifyService->Notify( spElement->fContextId, spElement->fSourceId,
				spElement->fSourceKey, spElement->fTimeStamp, static_cast<PeekSeverity>( spElement->fSeverity ),
				spElement->fShortMessage, spElement->fLongMessage ) );
	}
	m_SendNotifyArrayPtr->clear();
}


// =============================================================================
//		CNotifyService
// =============================================================================

// ----------------------------------------------------------------------------
//		Close
// ----------------------------------------------------------------------------

void
CNotifyService::Close()
{
	DoSetReceiveNotifications( false );
	m_spAction.Release();

	StopSendNotifies();

	m_SendNotifyQueue.Close();
	m_spNotifyService.Release();
}


// ----------------------------------------------------------------------------
//		DoSetReceiveNotifications
// ----------------------------------------------------------------------------

bool
CNotifyService::DoSetReceiveNotifications(
	bool inReceive )
{
	HeResult	hr = HE_E_FAIL;

	if ( !IsValid() || m_spAction == nullptr ) return false;

	try {
		CHePtr<IActionCollection> spActionCollection;
		Peek::ThrowIfFailed( (hr = m_spNotifyService->QueryInterface( &spActionCollection.p )) );

		if ( inReceive ) {
			Peek::ThrowIfFailed( (hr = spActionCollection->Add( m_spAction )) );
			m_bIsReceiveNotifies = true;
		}
		else {
			Peek::ThrowIfFailed( (hr = spActionCollection->Remove( m_spAction )) );
			m_bIsReceiveNotifies = false;
		}
	}
	HE_CATCH( hr );

	return HE_SUCCEEDED( hr );
}


#ifdef _WIN32
//------------------------------------------------------------------------------
//		RunSendThread
//------------------------------------------------------------------------------

UInt32
CNotifyService::RunSendThread()
{
	UInt32	uiReturnCode = 0;

	if ( !::SetEvent( m_hThreadStartEvent ) ) {
		uiReturnCode = ::GetLastError();
		return uiReturnCode;
	}

	::ResetEvent( m_hStopEvent );

	while ( IsSending() ) {
		Sleep( k_NotifySleepTime );
		m_SendNotifyQueue.DoNotifies();

		if ( IsStopRequested() ) {
			break;
		}
	}

	// Wait until we are asked to stop
	HANDLE	hWaiter = m_hStopEvent;
	DWORD	dwWait = ::WaitForMultipleObjects( 1, &hWaiter, FALSE, INFINITE );
	_ASSERTE( dwWait != WAIT_FAILED );	dwWait;

	return uiReturnCode;
}
#endif // _WIN32


// ----------------------------------------------------------------------------
//		SetPtr
// ----------------------------------------------------------------------------

void
CNotifyService::SetPtr(
	Helium::IHeServiceProvider*	inServiceProvider,
	CRemotePlugin*				inRemote )
{
	if ( !IsValid() ) {
		inServiceProvider->QueryService( INotify::GetIID(), &m_spNotifyService.p );
		m_SendNotifyQueue.SetPtr( m_spNotifyService );
	}

	if ( inRemote ) {
		bool	bIsReceiveNotifies = IsReceiveNotifies();

		if ( !IsValid() || m_spAction == nullptr ) {
			bIsReceiveNotifies = false;
		}

		if ( bIsReceiveNotifies ) {
			DoSetReceiveNotifications( false );
		}
		m_spAction.Release();

		(reinterpret_cast<IAction*>(inRemote))->QueryInterface( &m_spAction.p );
		if ( !m_spAction ) {
			_ASSERTE( !IsReceiveNotifies() );
			bIsReceiveNotifies = false;
			if ( IsReceiveNotifies() ) {
				DoSetReceiveNotifications( false );
			}
		}

		if ( bIsReceiveNotifies ) {
			_ASSERTE( m_spAction != nullptr );
			DoSetReceiveNotifications( true );
		}
	}
}


//------------------------------------------------------------------------------
//		StartSendNotifies
//------------------------------------------------------------------------------

bool
CNotifyService::StartSendNotifies()
{
//	PeekLock lock( m_Mutex );

	if ( IsSending() ) return true;

	m_SendNotifyQueue.ResetAddArray();

#if _WIN32
	try {
		// Setup the Stop Event
		_ASSERTE( m_hStopEvent == nullptr );
		if ( m_hStopEvent == nullptr ) {
			m_hStopEvent = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
			if ( m_hStopEvent == nullptr ) Peek::Throw( E_FAIL );
		}

		// Setup the Thread Start Event
		_ASSERTE( m_hThreadStartEvent == nullptr );
		if ( m_hThreadStartEvent == nullptr ) {
			m_hThreadStartEvent = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
			if ( m_hThreadStartEvent == nullptr ) Peek::Throw( E_FAIL );
		}

		// Create the processing thread
		unsigned int	uThreadId;
		::ResetEvent( m_hThreadStartEvent );

		_ASSERTE( m_hSendThread == nullptr );
		if ( m_hSendThread != nullptr ) {
			::CloseHandle( m_hSendThread );
		}

		uintptr_t hSendThread = _beginthreadex( nullptr, 0, SendThreadProc, this, 0, &uThreadId );
		if ( hSendThread == 0 ) Peek::Throw( E_FAIL );
		m_hSendThread = reinterpret_cast<HANDLE>( hSendThread );

	#ifdef _DEBUG
			// Set the thread name so we can see it in the debugger.
	//		ThreadUtil::SetThreadName( uThreadId, "TaskMgr" );
	#endif

		DWORD dwTimeout( 500 );
	#ifdef _DEBUG
		dwTimeout = INFINITE;
	#endif
		DWORD dwRet = WaitForSingleObject( m_hThreadStartEvent, dwTimeout );
		if ( dwRet ) Peek::Throw( E_FAIL );

	}
	catch ( ... ) {
		CloseStopEvent();
		CloseThreadStartEvent();
		CloseSendThread();

		Peek::Throw( HE_E_FAIL );
	}
#else
	// TODO
#endif

	return true;
}


//------------------------------------------------------------------------------
//		StopSendNotifies
//------------------------------------------------------------------------------

void
CNotifyService::StopSendNotifies()
{
//	PeekLock lock( m_Mutex );

	if ( !IsSending() ) return;

#if _WIN32
	::SetEvent( m_hStopEvent );

	// Now wait until the thread stops!
	DWORD dwRet = ::WaitForSingleObject( m_hSendThread, 60000 );
	if ( dwRet == WAIT_TIMEOUT ) {
		::TerminateThread( m_hSendThread, 0xAAAAAAAA );
		if ( m_hSendThread != nullptr ) {
			::CloseHandle( m_hSendThread );
			m_hSendThread = nullptr;
		}
	}
#else
	// TODO
#endif

#ifdef _WIN32
	CloseStopEvent();
	CloseThreadStartEvent();
	CloseSendThread();
#endif
}

#ifdef _WIN32
//------------------------------------------------------------------------------
//		SendThreadProc	[static]
//------------------------------------------------------------------------------

UINT WINAPI
CNotifyService::SendThreadProc(
	LPVOID	inParameter )
{
	CNotifyService* pNotifyService = reinterpret_cast<CNotifyService*>( inParameter );
	if ( pNotifyService != nullptr ) {
		pNotifyService->RunSendThread();
	}

	return 0;
}
#endif
