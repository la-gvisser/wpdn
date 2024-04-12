// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_PLUGINMESSAGE

#include "PluginMessages.h"
#include "RemotePlugin.h"
#include "PeekEngineContext.h"

// Helium::IHeStream::HE_STREAM_SEEK::STREAM_SEEK_END
#define SEEK_END	2

// =============================================================================
//	MessageFactory
// =============================================================================

std::unique_ptr<CPeekMessage>
MessageFactory(
	const CPeekStream& inMessage )
{
	CPeekMessage	msgReceived( inMessage );
	CPeekString		strRoot = msgReceived.GetModelerRoot();

	if ( strRoot == CMessagePing::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessagePing( inMessage ) );
	}
	else if ( strRoot == CMessageTab::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageTab( inMessage ) );
	}
	else if ( strRoot == CMessageGetAllOptions::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageGetAllOptions( inMessage ) );
	}
	else if ( strRoot == CMessageGetOptions::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageGetOptions( inMessage ) );
	}
	else if ( strRoot == CMessageExample::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageExample( inMessage ) );
	}

	return std::unique_ptr<CPeekMessage>();
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
//	CMessageTab
// =============================================================================

const CPeekString	CMessageTab::s_strMessageName( L"Tab" );
const CPeekString	CMessageTab::s_strTagMessage( L"Message" );

CMessageTab::CMessageTab()
	:	CPeekMessage( s_strMessageName )
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
//		LoadResponse
// -----------------------------------------------------------------------------

bool
CMessageTab::LoadResponse(
	const CPeekStream&	inResponse )
{
	if ( inResponse.GetLength() == 0 ) return true;
	if ( !CPeekMessage::LoadResponse( inResponse ) ) {
		return false;
	}

	CPeekDataElement	elemMessage( s_strTagMessage, *m_spModeler );
	if ( !elemMessage ) return false;

	elemMessage.Value( m_strResponse );

	return true;
}


// -----------------------------------------------------------------------------
//		StoreMessage
// -----------------------------------------------------------------------------

bool
CMessageTab::StoreMessage(
	CPeekStream&	outMessage )
{
	CPeekDataElement	elemMessage( s_strTagMessage, *m_spModeler );
	if ( !elemMessage ) return false;
	;	elemMessage.Value( m_strMessage );
	elemMessage.End();

	return CPeekMessage::StoreMessage( outMessage );
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
	if ( inResponse.GetLength() == 0 ) return true;
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


// =============================================================================
//	CMessageGetPrefsPage
// =============================================================================

const CPeekString	CMessageGetPrefsPage::s_strMessageName( L"GetPreferencePage" );


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageGetPrefsPage::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		LoadResponse
// -----------------------------------------------------------------------------

bool
CMessageGetPrefsPage::LoadResponse(
	const CPeekStream& inResponse )
{
	if ( !CPeekMessage::LoadResponse( inResponse ) ) return false;

	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	m_PrefsPage = GetBinaryData();

	char	szTerminate[4] = { 0, 0, 0, 0 };
	m_PrefsPage.Seek( 0, SEEK_END );
	m_PrefsPage.Write( 4, &szTerminate );
	m_PrefsPage.Rewind();

	return true;
}


// -----------------------------------------------------------------------------
//		StoreMessage
// -----------------------------------------------------------------------------

bool
CMessageGetPrefsPage::StoreMessage(
	CPeekStream&	outMessage )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	SetBinaryData( m_PrefsPage );

	return CPeekMessage::StoreMessage( outMessage );
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageGetPrefsPage::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	SetBinaryData( m_PrefsPage );

	return CPeekMessage::StoreResponse( outResponse );
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

#endif // IMPLEMENT_PLUGINMESSAGE
