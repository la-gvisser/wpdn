// =============================================================================
//	PluginTab.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "PluginTab.h"
#include "OmniPlugin.h"
#include "PeekContext.h"
#include "OptionsDialog.h"
#include "ContextManager.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"

extern CContextManagerPtr	GetContextManager();


// =============================================================================
//		COptionsView
// =============================================================================

COptionsView::COptionsView(
	CWnd*			inParent /*= NULL*/  )
	:	CDialog( COptionsView::IDD, inParent )
	,	m_pParent( dynamic_cast<CPluginTab*>( inParent ) )
	,	m_bInitialized( false )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
COptionsView::DoDataExchange(
	CDataExchange* pDX )
{
	pDX;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsView::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
COptionsView::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		/*inMessage*/,
	const CPeekStream&	/*inResponse*/ )
{
	if ( inResult != HE_S_OK ) return;

	// New options were set.
	if ( m_pParent != NULL ) {
		m_pParent->GetAllOptions();
	}

	return;
}
#endif // IMPLEMENT_PLUGINMESSAGE

// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

void
COptionsView::Refresh(
	CGlobalId			inId,
	const CPeekString&	inName,
	const COptions&		inOptions )
{
	m_idContext = inId;
	m_strName = inName;
	m_Options = inOptions;

	m_bInitialized = true;

	UpdateData( FALSE );
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( COptionsView, CDialog )
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
COptionsView::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CDialog::OnShowWindow( bShow, nStatus );
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
COptionsView::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CDialog::OnSize( nType, cx, cy );
}

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
