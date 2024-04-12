// =============================================================================
//	MessageOptions.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
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

	if ( strRoot == CMessageCreateAdapter::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageCreateAdapter( inMessage ) );
	}
	if ( strRoot == CMessageGetAdapter::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageGetAdapter( inMessage ) );
	}
	if ( strRoot == CMessageModifyAdapter::s_strMessageName ) {
		return std::unique_ptr<CPeekMessage>( new CMessageModifyAdapter( inMessage ) );
	}

	return std::unique_ptr<CPeekMessage>();
}


// =============================================================================
//	CMessageCreateAdapter
// =============================================================================

const CPeekString	CMessageCreateAdapter::s_strMessageName( L"CreateAdapter" );
const CPeekString	CMessageCreateAdapter::s_strTagFolderAdapter( L"FolderAdapter" );
const CPeekString	CMessageCreateAdapter::s_strTagId( L"Id" );
const CPeekString	CMessageCreateAdapter::s_strTagMonitor( L"Monitor" );
const CPeekString	CMessageCreateAdapter::s_strTagMask( L"Mask" );
const CPeekString	CMessageCreateAdapter::s_strTagSpeed( L"Speed" );
const CPeekString	CMessageCreateAdapter::s_strTagSave( L"Save" );
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

	CPeekDataElement	elemRoot( s_strTagFolderAdapter, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagMonitor, m_strMonitor );
		elemRoot.ChildValue( s_strTagMask, m_strMask );
		elemRoot.ChildValue( s_strTagSpeed, m_strSpeed );
		elemRoot.ChildValue( s_strTagSave, m_strSave );
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

	CPeekDataElement	elemRoot( s_strTagFolderAdapter, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagResult, m_nResult );
	}

	return CPeekMessage::Store( outResponse );
}


// =============================================================================
//	CMessageGetAdapter
// =============================================================================

const CPeekString	CMessageGetAdapter::s_strMessageName( L"GetAdapter" );
const CPeekString	CMessageGetAdapter::s_strTagFolderAdapter( L"FolderAdapter" );
const CPeekString	CMessageGetAdapter::s_strTagId( L"Id" );
const CPeekString	CMessageGetAdapter::s_strTagMonitor( L"Monitor" );
const CPeekString	CMessageGetAdapter::s_strTagMask( L"Mask" );
const CPeekString	CMessageGetAdapter::s_strTagSpeed( L"Speed" );
const CPeekString	CMessageGetAdapter::s_strTagSave( L"Save" );

CMessageGetAdapter::CMessageGetAdapter()
	:	CPeekMessage( s_strMessageName )
{
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageGetAdapter::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagFolderAdapter, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagId, m_Id );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		SetAdapterOptions
// -----------------------------------------------------------------------------

void
CMessageGetAdapter::SetAdapterOptions(
	const CAdapterOptions&	inAdapterOptions )
{
	if ( m_Id.IsNull() ) {
		m_Id = inAdapterOptions.GetId();
	}
	else {
		_ASSERTE( m_Id == inAdapterOptions.GetId() );
	}

	m_strMonitor = inAdapterOptions.GetMonitor();
	m_strMask = inAdapterOptions.GetMask();
	m_nSpeed = inAdapterOptions.GetSpeed();
	m_strSave = inAdapterOptions.GetSave();
}


// -----------------------------------------------------------------------------
//		StoreResponse
// -----------------------------------------------------------------------------

bool
CMessageGetAdapter::StoreResponse(
	CPeekStream&	outResponse )
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagFolderAdapter, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagId, m_Id);
		elemRoot.ChildValue( s_strTagMonitor, m_strMonitor );
		elemRoot.ChildValue( s_strTagMask, m_strMask );
		elemRoot.ChildValue( s_strTagSpeed, m_nSpeed );
		elemRoot.ChildValue( s_strTagSave, m_strSave );
	}

	return CPeekMessage::Store( outResponse );
}


// =============================================================================
//	CMessageModifyAdapter
// =============================================================================

const CPeekString	CMessageModifyAdapter::s_strMessageName( L"ModifyAdapter" );
const CPeekString	CMessageModifyAdapter::s_strTagFolderAdapter( L"FolderAdapter" );
const CPeekString	CMessageModifyAdapter::s_strTagId( L"Id" );
const CPeekString	CMessageModifyAdapter::s_strTagMonitor( L"Monitor" );
const CPeekString	CMessageModifyAdapter::s_strTagMask( L"Mask" );
const CPeekString	CMessageModifyAdapter::s_strTagSpeed( L"Speed" );
const CPeekString	CMessageModifyAdapter::s_strTagSave( L"Save" );
const CPeekString	CMessageModifyAdapter::s_strTagResult( L"Result" );

CMessageModifyAdapter::CMessageModifyAdapter()
	:	CPeekMessage( s_strMessageName )
	,	m_nResult( 0 )
{
}


// -----------------------------------------------------------------------------
//		GetAdapterOptions
// -----------------------------------------------------------------------------

CAdapterOptions
CMessageModifyAdapter::GetAdapterOptions() const
{
	CAdapterOptions	optAdapter;
	optAdapter.SetId( m_Id );
	optAdapter.SetMonitor( m_strMonitor );
	optAdapter.SetMask( m_strMask );
	optAdapter.SetName( m_strMonitor );
	optAdapter.SetSave( m_strSave );
	optAdapter.SetSpeed( m_strSpeed );

	return optAdapter;
}


// -----------------------------------------------------------------------------
//		LoadMessage
// -----------------------------------------------------------------------------

bool
CMessageModifyAdapter::LoadMessage()
{
	_ASSERTE( IsModeled() );
	if ( !IsModeled() ) return false;

	CPeekDataElement	elemRoot( s_strTagFolderAdapter, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagId, m_Id );
		elemRoot.ChildValue( s_strTagMonitor, m_strMonitor );
		elemRoot.ChildValue( s_strTagMask, m_strMask );
		elemRoot.ChildValue( s_strTagSpeed, m_strSpeed );
		elemRoot.ChildValue( s_strTagSave, m_strSave );
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

	CPeekDataElement	elemRoot( s_strTagFolderAdapter, *m_spModeler );
	if ( elemRoot ) {
		elemRoot.ChildValue( s_strTagResult, m_nResult );
	}

	return CPeekMessage::Store( outResponse );
}
