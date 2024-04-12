// =============================================================================
//	OptionsView.cpp
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#if defined(_WIN32) && defined(IMPLEMENT_TAB)

#include "PluginTab.h"
#include "OmniPlugin.h"
#include "PeekContext.h"
#include "OptionsDialog.h"
#include "ContextManager.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"


// =============================================================================
//		COptionsView
// =============================================================================

COptionsView::COptionsView(
	CWnd*			inParent /*= nullptr*/  )
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
[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
  #ifdef IMPLEMENT_NOTIFICATIONS
	CString	strNotify;
  #endif
	CString	strCurrent;
	CString	strLabel;
	CString	strEthernet;
	CString	strIP;
	CString	strPort;
	CString	strTime;
	CString	strDate;

	if ( !pDX->m_bSaveAndValidate && m_bInitialized ) {
	  #ifdef IMPLEMENT_NOTIFICATIONS
		strNotify = m_Options.IsReceiveNotifies() ? L"Receiving" : L"Not Receiving";
	  #endif
		strCurrent = m_Options.IsEnabledExample() ? L"Enabled" : L"Disabled";
		strLabel = m_Options.GetLabelExample();
		strEthernet = m_Options.GetEthernetExample().Format();
		strIP = m_Options.GetIpAddressExample().Format();
		strPort = m_Options.GetPortExample().Format();
		strTime = m_Options.GetTimeExample().FormatTime();
		strDate = m_Options.GetTimeExample().FormatDate();
	}

  #ifdef IMPLEMENT_NOTIFICATIONS
	DDX_Text( pDX, IDC_NOTIFY_STATE, strNotify );
  #endif
	DDX_Text( pDX, IDC_CURRENT_STATE, strCurrent );
	DDX_Text( pDX, IDC_LABEL_STATE, strLabel );
	DDX_Text( pDX, IDC_ETHERNET_STATE, strEthernet );
	DDX_Text( pDX, IDC_IP_STATE, strIP );
	DDX_Text( pDX, IDC_PORT_STATE, strPort );
	DDX_Text( pDX, IDC_TIME_STATE, strTime );
	DDX_Text( pDX, IDC_DATE_STATE, strDate );
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
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
	if ( m_pParent != nullptr ) {
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

	if ( ::IsWindow( m_hWnd ) ) {
		m_bInitialized = true;
		UpdateData( FALSE );
	}
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

#endif // _WIN32 && IMPLEMENT_TAB
