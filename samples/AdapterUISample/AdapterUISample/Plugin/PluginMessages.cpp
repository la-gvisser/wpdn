// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

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

	if ( strRoot == CMessageCreate::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageCreate( inMessage ) );
	}
	else if ( strRoot == CMessageList::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageList( inMessage ) );
	}

	return std::unique_ptr<CPeekMessage>();
}


// =============================================================================
//	CMessageCreate
// =============================================================================

const CPeekString	CMessageCreate::s_strMessageName( L"create" );
const CPeekString	CMessageCreate::s_strTagAdapter( L"adapter" );
const CPeekString	CMessageCreate::s_strTagName( L"name" );
const CPeekString	CMessageCreate::s_strTagResult( L"result" );

CMessageCreate::CMessageCreate()
	:	CPeekMessage( s_strMessageName )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageCreate::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemAdapter( s_strTagAdapter, *m_spModeler );
	if ( !elemAdapter ) return false;

	elemAdapter.Value( m_strAdapterName );

	return true;
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageCreate::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemResult( s_strTagResult, *m_spModeler );
	;	elemResult.Value( m_strResult );
	elemResult.End();

	return CPeekMessage::Store( outResponse );
}


// =============================================================================
//	CMessageList
// =============================================================================

const CPeekString	CMessageList::s_strMessageName( L"list" );

const CPeekString	CMessageList::s_strTagAdapterList( L"adapterlist" );
const CPeekString	CMessageList::s_strTagAdapter( L"adapter" );
const CPeekString	CMessageList::s_strTagId( L"id" );
const CPeekString	CMessageList::s_strTagName( L"name" );

CMessageList::CMessageList()
	:	CPeekMessage( s_strMessageName )
{
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageList::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemAdapterList( s_strTagAdapterList, *m_spModeler );
	for ( auto i = m_ayAdapterList.begin(); i != m_ayAdapterList.end(); ++i ) {
		CPeekDataElement	elemAdapter( s_strTagAdapter, elemAdapterList );
		elemAdapter.Attribute( s_strTagId, i->fId );
		elemAdapter.Attribute( s_strTagName, i->fName );

		elemAdapter.End();
	}
	elemAdapterList.End();

	return CPeekMessage::Store( outResponse );
}

#endif // IMPLEMENT_PLUGINMESSAGE
