// =============================================================================
//	PluginTab.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#if defined(_WIN32) && defined(IMPLEMENT_TAB)

#include "PluginTab.h"
#include "OmniPlugin.h"
#include "PeekContext.h"
#include "OptionsDialog.h"
#include "ContextManager.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"
#include "ItemList.h"


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
	static UINT	aySecurityIdIds[] = {
		IDC_SECURITYID_VALUE,
		IDC_SECURITYID_STATIC,
		0
	};
	CItemList	securityidIds( this, aySecurityIdIds );

	static UINT	aySiteNameIds[] = {
		IDC_SITENAME_VALUE,
		IDC_SITENAME_STATIC,
		0
	};
	CItemList	sitenameIds( this, aySiteNameIds );

	if ( pDX->m_bSaveAndValidate || !m_bInitialized ) return;

	CString	strProtocolType = m_Options.FormatProtocolType();
	
	CPeekString	strId;
	CPeekString	strName;
	m_Options.GetInterceptId().Get( strId, strName );
	CString	strSecurityId( strId );
	CString	strSiteName( strName );
	
	CString strSrcMAC = m_Options.FormatSrcMAC();
	CString strDstMAC = m_Options.FormatDstMAC();
	CString strMACProtocolType = m_Options.FormatMACProtocolType();
	CString strProtocolTypeHex = m_Options.FormatProtocolTypeHex();
	CString strVLANID = m_Options.FormatVlanId();
	CString strFragmentAge = m_Options.FormatFragmentAge();
	CString strFileName = m_Options.GetFileName();
	CString strSaveInterval = m_Options.FormatSaveIntervalLong();

	switch ( m_Options.GetProtocolType() ) {
		case COptions::kType_CMI:
			securityidIds.ShowItems( false );
			sitenameIds.ShowItems( false );
			break;

		case COptions::kType_SecurityId:
			securityidIds.ShowItems( true );
			sitenameIds.ShowItems( false );
			break;

		case COptions::kType_Ericsson:
			securityidIds.ShowItems( true );
			sitenameIds.ShowItems( true );
			break;

		case COptions::kType_Arris:
		case COptions::kType_Juniper:
			securityidIds.ShowItems( true );
			sitenameIds.ShowItems( false );
			break;
	}

	UpdateIpFilterList();

	DDX_Text( pDX, IDC_PROTOCOL_VALUE, strProtocolType );
	DDX_Text( pDX, IDC_SECURITYID_VALUE, strSecurityId );
	DDX_Text( pDX, IDC_SITENAME_VALUE, strSiteName );
	DDX_Text( pDX, IDC_SRCMAC_VALUE, strSrcMAC );
	DDX_Text( pDX, IDC_DSTMAC_VALUE, strDstMAC );
	DDX_Text( pDX, IDC_PROTOCOLTYPE_VALUE, strMACProtocolType );
	DDX_Text( pDX, IDC_HEX_EDIT, strProtocolTypeHex );
	DDX_Text( pDX, IDC_VLAN_VALUE, strVLANID );
	DDX_Text( pDX, IDC_AGE_VALUE, strFragmentAge );
	DDX_Text( pDX, IDC_SAVEDIR_VALUE, strFileName );
	DDX_Text( pDX, IDC_SAVE_INTERVAL, strSaveInterval );
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsView::OnInitDialog()
{
	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( pList ) {
		pList->SetExtendedStyle( pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		CRect	rcList;
		pList->GetClientRect( &rcList );

		int	nScrollWidth = GetSystemMetrics( SM_CXVSCROLL );
		if ( nScrollWidth < 10 ) {
			nScrollWidth = 16;
		}
		UInt32	nUnit = (rcList.Width() - nScrollWidth) / 23;
		UInt32	nHalfUnit = nUnit / 2;

		pList->InsertColumn( 0, L"Address 1" );
		pList->SetColumnWidth( 0, ((nUnit * 7) + nHalfUnit) );
		pList->InsertColumn( 1, L"Direction" );
		pList->SetColumnWidth( 1, ((nUnit * 6) + nHalfUnit) );
		pList->InsertColumn( 2, L"Address 2" );
		pList->SetColumnWidth( 2, ((nUnit * 7) + nHalfUnit) );
		pList->InsertColumn( 3, L"Port" );
		pList->SetColumnWidth( 3, (nUnit * 3) );
	}

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


// -----------------------------------------------------------------------------
//		UpdateIpFilterList
// -----------------------------------------------------------------------------

void
COptionsView::UpdateIpFilterList()
{
	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( !pList ) return;

	pList->DeleteAllItems();

	UInt32									nIndex = 0;
	const CIpFilterList&					theList = m_Options.GetIpFilterList();
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = theList.begin(); itr != theList.end(); ++itr, ++nIndex ) {
		pList->InsertItem( nIndex, _T( "" ) );
		pList->SetItemText( nIndex, 0, itr->FormatAddress1() );
		pList->SetItemText( nIndex, 1, itr->FormatDirection() );
		pList->SetItemText( nIndex, 2, itr->FormatAddress2() );
		pList->SetItemText( nIndex, 3, itr->FormatPort() );
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


// -----------------------------------------------------------------------------
//		Update
// -----------------------------------------------------------------------------

void
COptionsView::Update(
	CGlobalId			inId,
	const CPeekString&	inName,
	const COptions&		inOptions )
{
	m_idContext = inId;
	m_strName = inName;
	m_Options = inOptions;

	UpdateData( FALSE );
}

#endif // _WIN32 && IMPLEMENT_TAB
