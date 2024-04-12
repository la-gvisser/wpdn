// =============================================================================
//	OmniContext.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniContext.h"
#include "OptionsDialog.h"

#ifndef ASSERT
#define ASSERT(b)	ATLASSERT(b)
#endif


// =============================================================================
//		Tags
// =============================================================================

namespace Tags {
	CString	kRoot( L"SampleAdapter" );
};


// =============================================================================
//		COmniContext
// =============================================================================

COmniContext::COmniContext(
	CPeekProxy&	Proxy )
	:	CPeekContext( Proxy )
	,	m_nSrcIP( 0 )
{
}


// -----------------------------------------------------------------------------
//		OnFilter
// -----------------------------------------------------------------------------

int
COmniContext::OnFilter(
	CPacket&	Packet,
	UInt16*		pcbBytesToAccept,
	GUID&		guidFilter )
{
	Packet;
	pcbBytesToAccept;
	guidFilter;
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetContextPrefs
// -----------------------------------------------------------------------------

int
COmniContext::OnGetContextPrefs(
	CXmlElement&	xmlElement,
	int				bClearDirty )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		m_Options.Store( xmlElement );
		bClearDirty;
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnGetPacketString
// -----------------------------------------------------------------------------

int
COmniContext::OnGetPacketString(
	CPacket&		Packet,
	CString&		strSummary,
	UInt32&			Color,
	SummaryLayer*	pSummaryLayer )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		// No error packets.
		bool	bErrorPacket;
		ThrowIfFailed( Packet.IsError( bErrorPacket ) );
		if ( bErrorPacket ) return PEEK_PLUGIN_FAILURE;

		strSummary;
		Color;
		pSummaryLayer;
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_SUCCESS;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
// -----------------------------------------------------------------------------

int
COmniContext::OnProcessPacket(
	CPacket&	Packet )
{
	Packet;
	int	nResult = PEEK_PLUGIN_SUCCESS;	

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReset
// -----------------------------------------------------------------------------

int
COmniContext::OnReset()
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
//		OnSetContextPrefs
// -----------------------------------------------------------------------------

int
COmniContext::OnSetContextPrefs(
	CXmlElement& xmlElement )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		m_Options.Load( xmlElement );
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
COmniContext::OnShowOptions()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		COptionsDialog	dlg( m_Options );
		if ( dlg.DoModal() == IDOK ) {
			m_Options = dlg.GetOptions();
		}
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
// -----------------------------------------------------------------------------

int
COmniContext::OnStartCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	m_nSrcIP = m_Options.GetIP();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
// -----------------------------------------------------------------------------

int
COmniContext::OnStopCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	m_nSrcIP = 0;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnSummary
// -----------------------------------------------------------------------------

int
COmniContext::OnSummary(
	CSnapshot&	Snapshot )
{
	int	nResult	= PEEK_PLUGIN_SUCCESS;

	try {
		// Update stats.
		Snapshot;
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnGetContextPrefs
// -----------------------------------------------------------------------------

int
COmniContext::OnGetContextPrefs(
	IHeUnknown*	pXmlDoc,
	IHeUnknown*	pXmlNode,
	int			bClearDirty )
{
	if( !pXmlDoc || !pXmlNode ) return PEEK_PLUGIN_FAILURE;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		m_Options.Store( pXmlDoc, pXmlNode  );
		bClearDirty;
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

int
COmniContext::OnSetContextPrefs(
	IHeUnknown*	pXmlDoc,
	IHeUnknown*	pXmlNode,
	int			bClearDirty )
{
	bClearDirty;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		m_Options.Load( pXmlDoc, pXmlNode );
		m_nSrcIP = m_Options.GetIP();
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}
