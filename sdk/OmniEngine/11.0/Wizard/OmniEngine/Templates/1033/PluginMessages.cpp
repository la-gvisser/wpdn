// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PluginMessages.h"
#include "RemotePlugin.h"
#include "PeekEngineContext.h"


// =============================================================================
//	MessageFactory
// =============================================================================

std::unique_ptr<CPeekMessage>
MessageFactory(
	const CPeekStream& inMessage )
{
	CPeekMessage	msgReceived( inMessage );
	CPeekString		strRoot = msgReceived.GetModelerRoot();

	if ( strRoot == CMessageGetAllOptions::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageGetAllOptions( inMessage ) );
	}
	else if ( strRoot == CMessageGetOptions::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageGetOptions( inMessage ) );
	}
[!if NOTIFY_CODEEXAMPLES && NOTIFY_TAB && NOTIFY_UIHANDLER]
	else if ( strRoot == CMessageTab::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageTab( inMessage ) );
	}
[!endif]
[!if !NOTIFY_UIHANDLER]
	else if ( strRoot == CMessagePing::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessagePing( inMessage ) );
	}
	else if ( strRoot == CMessageExample::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageExample( inMessage ) );
	}
[!endif]

	return std::unique_ptr<CPeekMessage>();
}


// =============================================================================
//	CMessageGetAllOptions
// =============================================================================

const CPeekString	CMessageGetAllOptions::s_strMessageName( L"GetAllOptions" );
const CPeekString	CMessageGetAllOptions::s_strTagId( L"Id" );
const CPeekString	CMessageGetAllOptions::s_strTagOptionsList( L"OptionsList" );
const CPeekString	CMessageGetAllOptions::s_strTagOptions( L"Options" );
const CPeekString	CMessageGetAllOptions::s_strTagDefault( L"default" );
const CPeekString	CMessageGetAllOptions::s_strTagName( L"Name" );

CMessageGetAllOptions::CMessageGetAllOptions()
	:	CPeekMessage( s_strMessageName )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageGetAllOptions::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptionsList( s_strTagOptionsList, *m_spModeler );
	if ( !elemOptionsList ) return false;

	m_ayNamedOptions.RemoveAll();

	UInt32	nChildCount = elemOptionsList.GetChildCount();
	for ( UInt32 i = 0; i < nChildCount; i++ ) {
		CPeekDataElement	elemOptions( s_strTagOptions, elemOptionsList, i );
		if ( elemOptions ) {
			COptions	Options;
			if ( Options.Model( elemOptions ) ) {
				CGlobalId	idContext;
				elemOptions.Attribute( s_strTagId, idContext );
				CPeekString	strName;
				elemOptions.Attribute( s_strTagName, strName );

				TNamedOptions	NamedOpts = { idContext, strName, Options };
				m_ayNamedOptions.Add( NamedOpts );
			}
		}
	}

	return true;
}
[!if !NOTIFY_UIHANDLER]


// -----------------------------------------------------------------------------
//		LoadResponse
// -----------------------------------------------------------------------------

bool
CMessageGetAllOptions::LoadResponse(
	const CPeekStream&	inResponse )
{
	if ( !CPeekMessage::LoadResponse( inResponse ) ) {
		return false;
	}

	CPeekDataElement	elemOptionsList( s_strTagOptionsList, *m_spModeler );
	if ( !elemOptionsList ) return false;

	m_ayNamedOptions.RemoveAll();

	UInt32	nChildCount = elemOptionsList.GetChildCount( s_strTagOptions);
	for ( UInt32 i = 0; i < nChildCount; i++ ) {
		CPeekDataElement	elemOptions( s_strTagOptions, elemOptionsList, i );
		if ( elemOptions ) {
			COptions	Options;
			if ( Options.Model( elemOptions ) ) {
				CGlobalId	idContext;
				elemOptions.Attribute( s_strTagId, idContext );

				CPeekString	strName;
				elemOptions.Attribute( s_strTagName, strName );
				if ( strName.IsEmpty() ) continue;

				TNamedOptions	NamedOpts = { idContext, strName, Options };
				m_ayNamedOptions.Add( NamedOpts );
			}
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		StoreMessage
// -----------------------------------------------------------------------------

bool
CMessageGetAllOptions::StoreMessage(
	CPeekStream&	outMessage )
{
	// Nothing to store.
	return CPeekMessage::StoreMessage( outMessage );
}
[!endif]


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageGetAllOptions::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptionsList( s_strTagOptionsList, *m_spModeler );
	size_t				nCount = m_ayNamedOptions.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CPeekDataElement	elemOptions( s_strTagOptions, elemOptionsList );
		elemOptions.Attribute( s_strTagName, m_ayNamedOptions[i].fName );
		elemOptions.Attribute( s_strTagId, m_ayNamedOptions[i].fId );
		m_ayNamedOptions[i].fOptions.Model( elemOptions );

		elemOptions.End();
	}
	elemOptionsList.End();

	return CPeekMessage::Store( outResponse );
}


// =============================================================================
//		CMessageGetOptions
// =============================================================================

const CPeekString	CMessageGetOptions::s_strMessageName( L"GetOptions" );
const CPeekString	CMessageGetOptions::s_strTagName( L"Name" );
const CPeekString	CMessageGetOptions::s_strTagId( L"Id" );
const CPeekString	CMessageGetOptions::s_strTagOptions( L"Options" );

CMessageGetOptions::CMessageGetOptions(
	CGlobalId	inId )
	:	CPeekMessage( s_strMessageName )
	,	m_idContext( inId )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageGetOptions::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptions( s_strTagOptions, *m_spModeler );
	if ( elemOptions ) {
		elemOptions.Attribute( s_strTagName, m_strName );
		elemOptions.Attribute( s_strTagId, m_idContext );
		return m_Options.Model( elemOptions );
	}

	return false;
}
[!if !NOTIFY_UIHANDLER]


// -----------------------------------------------------------------------------
//		LoadResponse
// -----------------------------------------------------------------------------

bool
CMessageGetOptions::LoadResponse(
	const CPeekStream&	inResponse )
{
	if ( !CPeekMessage::LoadResponse( inResponse ) ) {
		return false;
	}

	CPeekDataElement	elemOptions( s_strTagOptions, *m_spModeler );
	if ( !elemOptions ) return false;

	if ( !m_Options.Model( elemOptions ) ) return false;

	return true;
}

// -----------------------------------------------------------------------------
//		StoreMessage
// -----------------------------------------------------------------------------

bool
CMessageGetOptions::StoreMessage(
	CPeekStream&	outMessage )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptions( s_strTagOptions, *m_spModeler );
	if ( elemOptions ) {
		elemOptions.Attribute( s_strTagName, m_strName );
		elemOptions.Attribute( s_strTagId, m_idContext );
		m_Options.Model( elemOptions );
	}

	return CPeekMessage::StoreMessage( outMessage );
}
[!endif]


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageGetOptions::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptions( s_strTagOptions, *m_spModeler );
	elemOptions.Attribute( s_strTagId, m_idContext );
	m_Options.Model( elemOptions );

	elemOptions.End();

	return CPeekMessage::StoreResponse( outResponse );
}
[!if NOTIFY_CODEEXAMPLES && NOTIFY_TAB && NOTIFY_UIHANDLER]


// =============================================================================
//	CMessageTab
// =============================================================================

const CPeekString	CMessageTab::s_strMessageName( L"Tab" );
const CPeekString	CMessageTab::s_strTagMessage( L"Message" );

CMessageTab::CMessageTab()
	: CPeekMessage( s_strMessageName )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageTab::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemMessage( s_strTagMessage, *m_spModeler );
	if ( !elemMessage ) return false;

	elemMessage.Value( m_strMessage );

	return true;
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageTab::StoreResponse(
CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemMessage( s_strTagMessage, *m_spModeler );
	;	elemMessage.Value( m_strResponse );
	elemMessage.End();

	return CPeekMessage::Store( outResponse );
}
[!endif]
[!if !NOTIFY_UIHANDLER]


// =============================================================================
//	CMessagePing
// =============================================================================

const CPeekString	CMessagePing::s_strMessageName( L"Ping" );

CMessagePing::CMessagePing()
	: CPeekMessage( s_strMessageName )
{
}


// =============================================================================
//	CMessageExample
// =============================================================================

const CPeekString	CMessageExample::s_strMessageName( L"MessageExample" );
const CPeekString	CMessageExample::s_strTagMessageLabel( L"Message" );
const CPeekString	CMessageExample::s_strTagMessage( L"I'm sending you some data." );
const CPeekString	CMessageExample::s_strTagResponse( L"Data received and sending some back!" );


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageExample::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement elemString( s_strTagMessageLabel, *m_spModeler );
	;	elemString.Value( m_strMessage );
	elemString.End();

	m_MessageData = GetBinaryData();

	return true;
}


// -----------------------------------------------------------------------------
//		LoadResponse
// -----------------------------------------------------------------------------

bool
CMessageExample::LoadResponse(
	const CPeekStream& inResponse )
{
	if ( !CPeekMessage::LoadResponse( inResponse ) ) return false;

	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	// Get the message and binary data from the modeler.
	CPeekDataElement elemString( s_strTagMessageLabel, *m_spModeler );
	;	elemString.Value( m_strResponse );
	elemString.End();

	m_ResponseData = GetBinaryData();

	return true;
}


// -----------------------------------------------------------------------------
//		StoreMessage
// -----------------------------------------------------------------------------

bool
CMessageExample::StoreMessage(
	CPeekStream&	outMessage )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement elemString( s_strTagMessageLabel, *m_spModeler );
	;	elemString.Value( m_strMessage );
	elemString.End();

	SetBinaryData( m_MessageData );

	return CPeekMessage::StoreMessage( outMessage );
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageExample::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement elemString( s_strTagMessageLabel, *m_spModeler );
	;	elemString.Value( m_strResponse );
	elemString.End();

	SetBinaryData( m_ResponseData );

	return CPeekMessage::StoreResponse( outResponse );
}
[!endif]

#endif // IMPLEMENT_PLUGINMESSAGE
