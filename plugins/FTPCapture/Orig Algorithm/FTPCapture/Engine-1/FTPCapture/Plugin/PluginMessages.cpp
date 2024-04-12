// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PluginMessages.h"
#include "RemotePlugin.h"
#include "PeekEngineContext.h"


// =============================================================================
//	MessageFactory
// =============================================================================

CPeekMessagePtr
MessageFactory(
	const CPeekStream& inMessage )
{
	CPeekMessage*	pMessage( NULL );

	CPeekMessage	msgReceived( inMessage );
	CPeekString		strRoot = msgReceived.GetModelerRoot();

	if ( strRoot == CMessageGetAllOptions::GetMessageName() ) {
		pMessage = new CMessageGetAllOptions( inMessage );
	}
	else if ( strRoot == CMessageSetOptions::GetMessageName() ) {
		pMessage = new CMessageSetOptions( inMessage );
	}
	else if ( strRoot == CMessageExample::GetMessageName() ) {
		pMessage = new CMessageExample( inMessage );
	}

	return CPeekMessagePtr( pMessage );
}


// =============================================================================
//	CMessageExample
// =============================================================================

int			CMessageExample::s_nMessageType( 'MSGX' );
CPeekString	CMessageExample::s_strMessageName( L"MessageExample" );

CPeekString CMessageExample::s_strTagMessageLabel( L"Message" );
CPeekString	CMessageExample::s_strTagMessage( L"I'm sending you some data." );
CPeekString	CMessageExample::s_strTagResponse( L"Data received and sending some back!" );


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageExample::LoadMessage()
{
	ASSERT( IsModeled() );
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

	ASSERT( IsModeled() );
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
	ASSERT( IsModeled() );
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
	ASSERT( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement elemString( s_strTagMessageLabel, *m_spModeler );
	;	elemString.Value( m_strResponse );
	elemString.End();

	SetBinaryData( m_ResponseData );

	return CPeekMessage::StoreResponse( outResponse );
}


// =============================================================================
//	CMessageGetAllOptions
// =============================================================================

int			CMessageGetAllOptions::s_nMessageType( 'MGAO' );
CPeekString	CMessageGetAllOptions::s_strMessageName( L"GetOptions" );
CPeekString CMessageGetAllOptions::s_strTagId( L"Id" );
CPeekString	CMessageGetAllOptions::s_strTagOptionsList( L"OptionsList" );
CPeekString	CMessageGetAllOptions::s_strTagOptions( L"Options" );
CPeekString	CMessageGetAllOptions::s_strTagDefault( L"default" );
CPeekString	CMessageGetAllOptions::s_strTagName( L"Name" );

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
	ASSERT( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptionsList( s_strTagOptionsList, *m_spModeler );
	if ( !elemOptionsList ) return false;

	UInt32	nChildCount = elemOptionsList.GetChildCount();
	if ( nChildCount == 0 ) return false;

	m_ayNamedOptions.RemoveAll();

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

	UInt32	nChildCount = elemOptionsList.GetChildCount( s_strTagOptions);
	if ( nChildCount == 0 ) return false;

	m_ayNamedOptions.RemoveAll();

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


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageGetAllOptions::StoreResponse(
	CPeekStream&	outResponse )
{
	ASSERT( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptionsList( s_strTagOptionsList, *m_spModeler );
	size_t				nCount = m_ayNamedOptions.GetCount();
	if ( nCount == 0 ) return false;

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
//	CMessageSetOptions
// =============================================================================

int			CMessageSetOptions::s_nMessageType( 'MSOP' );
CPeekString	CMessageSetOptions::s_strMessageName( L"SetOptions" );
CPeekString	CMessageSetOptions::s_strTagName( L"Name" );
CPeekString	CMessageSetOptions::s_strTagId( L"Id" );
CPeekString	CMessageSetOptions::s_strTagOptions( L"Options" );

CMessageSetOptions::CMessageSetOptions(
	CGlobalId			inId,
	const CPeekString&	inName,
	const COptions&		inOptions )
	:	CPeekMessage( s_strMessageName )
	,	m_idContext( inId )
	,	m_strName( inName )
	,	m_Options( inOptions )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageSetOptions::LoadMessage()
{
	ASSERT( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptions( s_strTagOptions, *m_spModeler );
	if ( elemOptions ) {
		elemOptions.Attribute( s_strTagName, m_strName );
		elemOptions.Attribute( s_strTagId, m_idContext );
		return m_Options.Model( elemOptions );
	}

	return false;
}


// -----------------------------------------------------------------------------
//		LoadResponse
// -----------------------------------------------------------------------------

bool
CMessageSetOptions::LoadResponse(
	const CPeekStream&	inResponse )
{
	return CPeekMessage::LoadResponse( inResponse );
}


// -----------------------------------------------------------------------------
//		StoreMessage
// -----------------------------------------------------------------------------

bool
CMessageSetOptions::StoreMessage(
	CPeekStream&	outMessage )
{
	ASSERT( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemOptions( s_strTagOptions, *m_spModeler );
	if ( elemOptions ) {
		elemOptions.Attribute( s_strTagName, m_strName );
		elemOptions.Attribute( s_strTagId, m_idContext );
		m_Options.Model( elemOptions );
	}

	return CPeekMessage::StoreMessage( outMessage );
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageSetOptions::StoreResponse(
	CPeekStream&	outResponse )
{
	ASSERT( IsModeled() );
	if ( !IsModeled() ) return false;

	return CPeekMessage::StoreResponse( outResponse );
}

#endif // IMPLEMENT_PLUGINMESSAGE