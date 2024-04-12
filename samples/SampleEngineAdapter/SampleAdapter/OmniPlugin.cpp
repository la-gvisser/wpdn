// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "AboutDialog.h"
#include "resource.h"

#ifndef ASSERT
#define ASSERT(b)	ATLASSERT(b)
#endif

using namespace Plugin;

static const GUID GUID_SamplePlugin = CSamplePlugin_CID;


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin(
	CPeekProxy& Proxy )
	:	CPeekPlugin( Proxy )
	,	m_Context( Proxy )
{	
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniPlugin::OnCreateContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniPlugin::OnDisposeContext()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnFilter
// -----------------------------------------------------------------------------

int
COmniPlugin::OnFilter(
	CPacket&	Packet,
	UInt16*		pcbBytesToAccept,
	GUID&		guidFilter )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnFilter( Packet, pcbBytesToAccept, guidFilter );
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnGetClassId
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetClassId(
	GUID&	guidClassId )
{
	guidClassId = GUID_SamplePlugin;

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetContextPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetContextPrefs(
	CXmlElement&	xmlElement,
	int				bClearDirty )
{
	return m_Context.OnGetContextPrefs( xmlElement, bClearDirty );
}


// -----------------------------------------------------------------------------
//		OnGetName
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetName(
	CString&	strName )
{
	strName.LoadString( IDS_PLUGIN_NAME );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetPacketString
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetPacketString(
	CPacket&		Packet,
	CString&		strSummary,
	UInt32&			Color,
	SummaryLayer*	pSummaryLayer )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnGetPacketString( Packet, strSummary, Color, pSummaryLayer );
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnLoad
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoad(
	const GUID&	guidContext )
{
	guidContext;
	
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
// -----------------------------------------------------------------------------

int
COmniPlugin::OnProcessPacket(
	CPacket&	Packet )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnProcessPacket( Packet );
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReadPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnReadPrefs()
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReset
// -----------------------------------------------------------------------------

int
COmniPlugin::OnReset()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnReset();
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnSetContextPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnSetContextPrefs(
	CXmlElement&	xmlElement )
{
	return m_Context.OnSetContextPrefs( xmlElement );
}


// -----------------------------------------------------------------------------
//		OnStartCapture
// -----------------------------------------------------------------------------

int
COmniPlugin::OnStartCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnStartCapture();
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
// -----------------------------------------------------------------------------

int
COmniPlugin::OnStopCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnStopCapture();
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnShowAbout
// -----------------------------------------------------------------------------

int
COmniPlugin::OnShowAbout()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		CAboutDialog	dlg;
		dlg.DoModal();
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnShowOptions
// -----------------------------------------------------------------------------

int
COmniPlugin::OnShowOptions()
{
	return m_Context.OnShowOptions();
}


// -----------------------------------------------------------------------------
//		OnSummary
// -----------------------------------------------------------------------------

int
COmniPlugin::OnSummary(
	CSnapshot&	Snapshot )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try	{
		m_Context.OnSummary( Snapshot );
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
COmniPlugin::OnUnload()
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnWritePrefs()
{
	return PEEK_PLUGIN_SUCCESS;
}

// -----------------------------------------------------------------------------
//		OnGetContextPrefs
// -----------------------------------------------------------------------------

int		
COmniPlugin::OnGetContextPrefs(
	IHeUnknown*	pXmlDoc,
	IHeUnknown*	pXmlNode,
	int			bClearDirty )
{
	return m_Context.OnGetContextPrefs( pXmlDoc, pXmlNode, bClearDirty );
}

// -----------------------------------------------------------------------------
//		OnSetContextPrefs
// -----------------------------------------------------------------------------

int		
COmniPlugin::OnSetContextPrefs(
	IHeUnknown*	pXmlDoc,
	IHeUnknown*	pXmlNode,
	int			bClearDirty )
{
	return m_Context.OnSetContextPrefs( pXmlDoc, pXmlNode, bClearDirty );
}
