// =============================================================================
//	NotifyService.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "NotifyService.h"
#include <process.h>

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

	for ( size_t i = 0; i < m_Array1.GetCount(); i++ ) {
		NotifyElementPtr	spElement = m_Array1.GetAt( i );
		spElement.reset();
	}
	m_Array1.RemoveAll();

	for ( size_t i = 0; i < m_Array2.GetCount(); i++ ) {
		NotifyElementPtr	spElement = m_Array2.GetAt( i );
		spElement.reset();
	}
	m_Array2.RemoveAll();

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

		if ( m_AddNotifyArrayPtr->GetCount() == 0 ) return;
		ASSERT( m_AddNotifyArrayPtr != NULL );

		ResetAddArray();
	}

	ASSERT( m_SendNotifyArrayPtr != NULL );

	NotifyElementPtr spEmpty;

	for ( size_t i = 0; i < m_SendNotifyArrayPtr->GetCount(); i++ ) {
		NotifyElementPtr	spElement = m_SendNotifyArrayPtr->GetAt( i );
		ASSERT( spElement );
		if ( !spElement ) continue;

		Peek::ThrowIfFailed( m_spNotifyService->Notify( spElement->fContextId, spElement->fSourceId,
			spElement->fSourceKey, spElement->fTimeStamp, static_cast<PeekSeverity>( spElement->fSeverity ), 
			spElement->fShortMessage, spElement->fLongMessage ) );

//		m_SendNotifyArrayPtr->RemoveAt( i );
	}
	m_SendNotifyArrayPtr->RemoveAll();
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

	if ( !IsValid() || m_spAction == NULL ) return false;

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

	return SUCCEEDED( hr );
}


//------------------------------------------------------------------------------
//		RunSendThread
//------------------------------------------------------------------------------


UINT
CNotifyService::RunSendThread()
{
	UINT	uiReturnCode = 0;

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
	ASSERT( dwWait != WAIT_FAILED );	dwWait;

	return uiReturnCode;
}


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

		if ( !IsValid() || m_spAction == NULL ) {
			bIsReceiveNotifies = false;
		}

		if ( bIsReceiveNotifies ) {
			DoSetReceiveNotifications( false );
		}
		m_spAction.Release();

		(reinterpret_cast<IAction*>(inRemote))->QueryInterface( &m_spAction.p );
		if ( !m_spAction ) {
			ASSERT( !IsReceiveNotifies() );
			bIsReceiveNotifies = false;
			if ( IsReceiveNotifies() ) {
				DoSetReceiveNotifications( false );
			}
		}

		if ( bIsReceiveNotifies ) {
			ASSERT( m_spAction != NULL );
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

	try {
		// Setup the Stop Event
		ASSERT( m_hStopEvent == NULL );
		if ( m_hStopEvent == NULL ) {
			m_hStopEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
			if ( m_hStopEvent == NULL ) Peek::Throw( E_FAIL );
		}

		// Setup the Thread Start Event
		ASSERT( m_hThreadStartEvent == NULL );
		if ( m_hThreadStartEvent == NULL ) {
			m_hThreadStartEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
			if ( m_hThreadStartEvent == NULL ) Peek::Throw( E_FAIL );
		}

		// Create the processing thread
		unsigned int	uThreadId;
		::ResetEvent( m_hThreadStartEvent );

		ASSERT( m_hSendThread == NULL );
		if ( m_hSendThread != NULL ) {
			::CloseHandle( m_hSendThread );
		}

		uintptr_t hSendThread = _beginthreadex( NULL, 0, SendThreadProc, this, 0, &uThreadId );
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

	::SetEvent( m_hStopEvent );

	// Now wait until the thread stops!
	DWORD dwRet = ::WaitForSingleObject( m_hSendThread, 60000 );
	if ( dwRet == WAIT_TIMEOUT ) {
		::TerminateThread( m_hSendThread, 0xAAAAAAAA );
		if ( m_hSendThread != NULL ) {
			::CloseHandle( m_hSendThread );
			m_hSendThread = NULL;
		}
	}

	CloseStopEvent();
	CloseThreadStartEvent();
	CloseSendThread();
}


//------------------------------------------------------------------------------
//		SendThreadProc	[static]
//------------------------------------------------------------------------------

UINT WINAPI
CNotifyService::SendThreadProc( 
	LPVOID	inParameter )
{
	CNotifyService* pNotifyService = reinterpret_cast<CNotifyService*>( inParameter );
	if ( pNotifyService != NULL ) {
		pNotifyService->RunSendThread();
	}

	return 0;
}
