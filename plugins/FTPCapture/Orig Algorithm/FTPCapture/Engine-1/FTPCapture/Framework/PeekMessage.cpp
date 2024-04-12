// =============================================================================
//	PeekMessage.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PeekMessage.h"
#include "OmniConsoleContext.h"
#include "RemotePlugin.h"

// =============================================================================
//	CPeekMessage
//
//	Class for assembling data for CPeekDataModeler type modeling and / or from 
//	allocated UInt8* data into a stream.  This stream may be passed as an 
//	argument to the DoSendPluginMessage and the OnProcessPluginMessage functions.
//
//	The class that is used for defining (i.e. modeling) data that will be sent
//	console to engine and engine to console is CMessageOptions.  CMessageOptions
//	derives from CPeekMessage, and CPeekMessage derives from CPeekDataModeler.
//
// =============================================================================

int			CPeekMessage::s_nMessageType( 'PMSG' );
CPeekString	CPeekMessage::s_strMessageName( L"PeekMessage" );


// -----------------------------------------------------------------------------
//		GetModelerRoot
// -----------------------------------------------------------------------------

CPeekString
CPeekMessage::GetModelerRoot()
{
	if ( IsModeled() ) {
		return m_spModeler->GetRootName();
	}
	return CPeekString();
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

bool
CPeekMessage::Load(
	const CPeekStream&	inMessage )
{
	size_t	nDataLength = inMessage.GetLength();
	if ( nDataLength < sizeof( tPeekMessageHeader ) ){
		ASSERT( 0 );
		return false;
	}
	const UInt8*	pData = inMessage.GetData();
	ASSERT( pData != NULL );
	if ( pData == NULL ) return false;

	const tPeekMessageHeader*	pHeader = reinterpret_cast<const tPeekMessageHeader*>( pData );
	ASSERT( pHeader != NULL );
	if ( pHeader == NULL ) return false;
	pData += sizeof( tPeekMessageHeader );

	size_t	nDataModelerLength = pHeader->GetDataModelerLength();
	size_t	nBinaryDataLength = pHeader->GetBinaryDataLength();
	size_t	nCalculatedSize = sizeof( tPeekMessageHeader ) + pHeader->TotalLength();

	ASSERT( nDataLength >= nCalculatedSize );
	if ( nDataLength < nCalculatedSize ) {
		return false;
	}

	Reset();

	if ( nDataModelerLength > 0 ) {
		m_spModeler = CPeekDataModelerPtr( new CPeekDataModeler( nDataModelerLength, pData ) );
		ASSERT( IsModeled() );
		if ( !IsModeled() ) Peek::Throw( HE_E_OUT_OF_MEMORY );
		m_nDataModelerLength = nDataModelerLength;
		pData += nDataModelerLength;
	}

	if ( nBinaryDataLength > 0 ) {
		m_pBinaryData = reinterpret_cast<UInt8*>( malloc( nBinaryDataLength ) );
		ASSERT( m_pBinaryData );
		if ( !m_pBinaryData ) Peek::Throw( HE_E_OUT_OF_MEMORY );
		m_nBinaryDataLength = nBinaryDataLength;
		m_bBinaryDataOwner = true;
		memcpy( m_pBinaryData, pData, nBinaryDataLength );
	}

	return true; // GetLength();
}


// -----------------------------------------------------------------------------
//		SetBinaryData
// -----------------------------------------------------------------------------

void
CPeekMessage::SetBinaryData(
	const CPeekStream&	inData )
{
	const UInt8*	pBinaryData = inData.GetData();
	size_t			nDataLength = inData.GetLength();
	if ( (pBinaryData == NULL) || (nDataLength == 0) ) {
		return;
	}

	ReleaseData();

	UInt8*	pData = reinterpret_cast<UInt8*>( malloc( nDataLength ) );
	ASSERT( pData );
	if ( !pData ) Peek::Throw( HE_E_OUT_OF_MEMORY );
	memcpy( pData, pBinaryData, nDataLength );

	m_nBinaryDataLength = nDataLength;
	m_pBinaryData = pData;
	m_bBinaryDataOwner = true;
}

void
CPeekMessage::SetBinaryData(
	size_t	inLength,
	UInt8*	inData )
{
	ASSERT( inLength && inData );

	ReleaseData();

	if ( !inLength || !inData ) {
		return;
	}

	m_nBinaryDataLength = inLength;
	m_pBinaryData = inData;
	m_bBinaryDataOwner = false;
}


// -----------------------------------------------------------------------------
//		StartResponse
// -----------------------------------------------------------------------------

bool
CPeekMessage::StartResponse(
	const CPeekString& inName )
{
	Reset();

	m_spModeler = CPeekDataModelerPtr( new CPeekDataModeler( inName ) );
	ASSERT( IsModeled() );
	if ( !IsModeled() ) Peek::Throw( HE_E_OUT_OF_MEMORY );

	return true;
}


// -----------------------------------------------------------------------------
//		Store
// -----------------------------------------------------------------------------

bool
CPeekMessage::Store(
	CPeekStream&	outMessage )
{
	CPeekStream			psDataModelerData;
	tPeekMessageHeader	omhHeader;
	omhHeader.Init();

	if ( IsModeled() ) {
		m_spModeler->Store( psDataModelerData );
		if ( psDataModelerData.GetLength() > MIN_MODELER_LEN ) {
			omhHeader.SetDataModelerLength( psDataModelerData.GetLength() );
		}
	}
	if ( IsBinary() ) {
		omhHeader.SetBinaryDataLength( m_nBinaryDataLength );
	}

	outMessage.Reset();	// clear the stream.
	outMessage.Write( sizeof( tPeekMessageHeader ), &omhHeader );

	if ( omhHeader.GetDataModelerLength() > 0 ) {
		outMessage.Append( psDataModelerData );
	}

	if ( omhHeader.GetBinaryDataLength() > 0 ) {
		ASSERT( (omhHeader.GetBinaryDataLength() > 0) && (m_pBinaryData != NULL) );
		outMessage.Write( omhHeader.GetBinaryDataLength(), m_pBinaryData );
	}

#ifdef _DEBUG
	size_t nTestDataLen = outMessage.GetLength();
	size_t nCalcDataLen = sizeof( tPeekMessageHeader ) + omhHeader.TotalLength();
	ASSERT( nTestDataLen == nCalcDataLen );
	if ( nTestDataLen != nCalcDataLen ) return false;
#endif

	return true; // omhHeader.TotalLength();
}


// ==============================================================================
//		CPeekMessageQueue
// ==============================================================================

// -----------------------------------------------------------------------------
//		PostPluginMessage
// -----------------------------------------------------------------------------

bool
CPeekMessageQueue::PostPluginMessage(
	CPeekMessagePtr			inMessage,
	CPeekMessageProcess*	inMsgProc,
	FnResponseProcessor		inResponseProc )
{
	ASSERT( inMsgProc != NULL );
	if ( inMsgProc == NULL ) return false;
	ASSERT( inResponseProc != NULL );
	if ( inResponseProc == NULL ) return false;

	DWORD			dwTransId = kMaxUint32;
	CPeekStream		psMessage;
	inMessage->StoreMessage( psMessage );

	if ( m_pContext != NULL ) {
		if ( !m_pContext->DoSendPluginMessage( psMessage, 500, dwTransId ) ) {
			return false;
		}
	}
	else {
		COmniPlugin&	Plugin = CRemotePlugin::GetPlugin();
		HeResult		hr = Plugin.DoSendPluginMessage( psMessage, 500, dwTransId );
		if ( FAILED( hr ) ) return false;
	}

	Add( dwTransId, inMessage, inMsgProc, inResponseProc );

	return true;
}
#endif // IMPLEMENT_PLUGINMESSAGE
