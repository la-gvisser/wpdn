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
	CPeekMessage*	pMessage( nullptr );

	CPeekMessage	msgReceived( inMessage );
	CPeekString		strRoot = msgReceived.GetModelerRoot();

	if ( strRoot == CMessagePing::s_strMessageName ) {
		pMessage = new CMessagePing( inMessage );
	}
	if ( strRoot == CMessageGetAllOptions::s_strMessageName ) {
		pMessage = new CMessageGetAllOptions( inMessage );
	}
	else if ( strRoot == CMessageGetOptions::s_strMessageName ) {
		pMessage = new CMessageGetOptions( inMessage );
	}

	return CPeekMessagePtr( pMessage );
}


// =============================================================================
//	CMessagePing
// =============================================================================

const CPeekString	CMessagePing::s_strMessageName( L"Ping" );

CMessagePing::CMessagePing()
	:	CPeekMessage( s_strMessageName )
{
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
	_ASSERTE( IsModeled() );
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

#endif // IMPLEMENT_PLUGINMESSAGE

