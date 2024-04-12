// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PluginMessages.h"

#ifdef IMPLEMENT_PLUGINMESSAGE
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

	if ( strRoot == CMessageCreateAdapter::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageCreateAdapter( inMessage ) );
	}
	if ( strRoot == CMessageGetAdapterOptions::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageGetAdapterOptions( inMessage ) );
	}

	return std::unique_ptr<CPeekMessage>();
}


// =============================================================================
//	CMessageCreateAdapter
// =============================================================================

const CPeekString	CMessageCreateAdapter::s_strMessageName( L"CreateAdapter" );
const CPeekString	CMessageCreateAdapter::s_strTagId( L"Id" );
const CPeekString	CMessageCreateAdapter::s_strTagAdapterConfig( L"HspAdapter" );
const CPeekString	CMessageCreateAdapter::s_strTagResult( L"Result" );

CMessageCreateAdapter::CMessageCreateAdapter()
	:	CPeekMessage( s_strMessageName )
	,	m_nResult( 0 )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageCreateAdapter::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagAdapterConfig, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagId, m_Id );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageCreateAdapter::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagAdapterConfig, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagResult, m_nResult );
	}

	return CPeekMessage::Store( outResponse );
}


// =============================================================================
//	CMessageGetAdapterOptions
// =============================================================================

const CPeekString	CMessageGetAdapterOptions::s_strMessageName( L"GetAdapterOptions" );
const CPeekString	CMessageGetAdapterOptions::s_strTagAdapterOptions( L"AdapterOptions" );
const CPeekString	CMessageGetAdapterOptions::s_strTagId( L"Id" );

CMessageGetAdapterOptions::CMessageGetAdapterOptions()
	:	CPeekMessage( s_strMessageName )
{
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageGetAdapterOptions::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagAdapterOptions, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagId, m_Id );
	}

	return CPeekMessage::Store( outResponse );
}


// =============================================================================
//	CMessageModifyAdapter
// =============================================================================

const CPeekString	CMessageModifyAdapter::s_strMessageName( L"ModifyAdapter" );
const CPeekString	CMessageModifyAdapter::s_strTagId( L"Id" );
const CPeekString	CMessageModifyAdapter::s_strTagAdapterConfig( L"HspAdapter" );
const CPeekString	CMessageModifyAdapter::s_strTagResult( L"Result" );

CMessageModifyAdapter::CMessageModifyAdapter()
	:	CPeekMessage( s_strMessageName )
	,	m_nResult( 0 )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageModifyAdapter::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagAdapterConfig, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagId, m_Id );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageModifyAdapter::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagAdapterConfig, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagResult, m_nResult );
	}

	return CPeekMessage::Store( outResponse );
}

#endif	// IMPLEMENT_PLUGINMESSAGE
