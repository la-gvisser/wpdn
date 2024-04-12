// =============================================================================
//	PeekPlugin.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekPlugin.h"
#include "resource.h"


// =============================================================================
//		CPeekPlugin
// =============================================================================

CPeekPlugin::CPeekPlugin(
	CPeekProxy&	Proxy )
	:	m_guidContext( GUID_NULL )
	,	m_PeekProxy( Proxy )
{
}

CPeekPlugin::~CPeekPlugin()
{
}


// -----------------------------------------------------------------------------
//		OnGetClassId
// -----------------------------------------------------------------------------

int
CPeekPlugin::OnGetClassId(
	GUID&	guidClassId )
{
	guidClassId;
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetName
// -----------------------------------------------------------------------------

int
CPeekPlugin::OnGetName(
	CStringW&	strName )
{
	strName.Empty();
	strName.LoadString( IDS_PLUGIN_NAME );
	return PEEK_PLUGIN_SUCCESS;
}
