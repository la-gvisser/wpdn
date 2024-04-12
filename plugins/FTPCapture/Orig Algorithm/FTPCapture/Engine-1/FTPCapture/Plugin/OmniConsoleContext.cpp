// =============================================================================
//	OmniConsoleContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniConsoleContext.h"
#include "AboutDialog.h"
#include "MemUtil.h"
#include "Packet.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
  #include "PluginMessages.h"
#endif // #ifdef IMPLEMENT_PLUGINMESSAGE
#include "Snapshot.h"
#include "MediaTypes.h"


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnCreateContext()
{
	int	nResult = CPeekConsoleContext::OnCreateContext();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnDisposeContext()
{
	int	nResult = CPeekConsoleContext::OnDisposeContext();

	return nResult;
}


#ifdef IMPLEMENT_SUMMARIZEPACKET
// -----------------------------------------------------------------------------
//		OnGetPacketString
//		: Implements ISummarizePacket::SummarizePacket
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnGetPacketString(
	CPacket&		inPacket,
	CPeekString&	outSummary,
	UInt32&			outColor,
	Peek::SummaryLayer*	inSummaryLayer )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		// No error packets.
		if ( inPacket.IsError() ) return PEEK_PLUGIN_FAILURE;

		outSummary;
		outColor;
		inSummaryLayer;

	}
	catch( int ) {
		;
	}

	return nResult;
}
#endif // IMPLEMENT_SUMMARIZEPACKET


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CPeekContext::OnContextPrefs( ioPrefs );

	m_Options.Model( ioPrefs );

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		OnProcessPluginResponse
//		: Implements IPluginRecv::Recv
// -----------------------------------------------------------------------------

int
COmniConsoleContext::OnProcessPluginResponse(
	CPeekStream&	inMessage,
	HeResult		inMsgResult,
	DWORD			inTransId )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	CPeekMessageItem&	miMessage = m_MessageQueue.GetAt( inTransId );
	if ( miMessage.IsValid() ) {
		miMessage.CallResponseProcessor( inMsgResult, inMessage );
		nResult = PEEK_PLUGIN_SUCCESS;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse							[FnResponseProcessor]
// -----------------------------------------------------------------------------

void
COmniConsoleContext::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	inResult;
	inMessage;
	inResponse;

	if ( inMessage->GetType() == CMessageExample::GetMessageType() ) {
	}

	return;
}


#ifdef IMPLEMENT_CODEEXAMPLES
// -----------------------------------------------------------------------------
//		ProcessResponseExample
// -----------------------------------------------------------------------------

void
COmniConsoleContext::ProcessResponseExample(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	CMessageExample*	pMessage = dynamic_cast<CMessageExample*>( inMessage.get() );
	if ( pMessage != NULL ) {
		if ( HE_SUCCEEDED( inResult ) ) {
			if ( pMessage->LoadResponse( inResponse ) ) {
				// Validate response.
				const CPeekString&	strResponse = pMessage->GetResponse();
				ASSERT( strResponse == L"Data received and sending some back!" );

				const CPeekStream&	psData = pMessage->GetResponseData();
		ASSERT( psData.GetLength() == 500 );
		if ( psData.GetLength() == 500 ) {
			const UInt8* pData = psData.GetData();
			ASSERT( pData != NULL );
			if ( pData ) {
				ASSERT( *pData == 0x56 );
			}
		}
	}
		}
	}
}
#endif // IMPLEMENT_CODEEXAMPLES
#endif // IMPLEMENT_PLUGIN_MESSAGE
