// ============================================================================
// OptionsStatus.cpp:
//      implementation of the COptionsStatus class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "Options.h"
#include "OptionsStatus.h"
#include "OptionsDlg.h"

#define BASE_CLASS CDialog


// ============================================================================
//		COptionsStatus
// ============================================================================

IMPLEMENT_DYNAMIC(COptionsStatus, BASE_CLASS)

COptionsStatus::COptionsStatus()
	:	BASE_CLASS()
	,	m_pParentDialog( NULL )
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
COptionsStatus::DoDataExchange(
	CDataExchange*	pDX )
{
	BASE_CLASS::DoDataExchange( pDX );

	if ( !pDX->m_bSaveAndValidate ) {
		DDX_Text( pDX, IDC_PROTOCOL_VALUE, m_Options.FormatProtocolType() );
		DDX_Text( pDX, IDC_INTERCEPTID_EDIT, m_Options.GetInterceptId().Format() );
		DDX_Text( pDX, IDC_INTERCEPTID_NOTE, m_strNotes[m_Options.GetProtocolType()] );

		DDX_Text( pDX, IDC_SRCMAC_VALUE, m_Options.FormatSrcMAC() );
		DDX_Text( pDX, IDC_DSTMAC_VALUE, m_Options.FormatDstMAC() );
		DDX_Text( pDX, IDC_PROTOTYPE_VALUE, m_Options.FormatMACProtocolType() );
		DDX_Text( pDX, IDC_HEX_EDIT, m_Options.FormatMACProtocolTypeHex() );

		DDX_Text( pDX, IDC_VLAN_VALUE, m_Options.FormatVLANID() );

		DDX_Text( pDX, IDC_AGE_VALUE, m_Options.FormatFragmentAge() );

		DDX_Text( pDX, IDC_SAVEDIR_VALUE, m_Options.FormatFileName() );
		DDX_Text( pDX, IDC_SAVE_INTERVAL, m_Options.FormatSaveIntervalLong() );

		CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
		if ( pList ) {
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
	}
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsStatus::OnInitDialog()
{
	m_strNotes[COptions::kType_CMI];
	m_strNotes[COptions::kType_PCLI].LoadString( IDS_NOTE_PCLI );
	m_strNotes[COptions::kType_SecurityId].LoadString( IDS_NOTE_SECURITYID );
	m_strNotes[COptions::kType_Ericsson].LoadString( IDS_NOTE_ERICSSON );
	m_strNotes[COptions::kType_Arris].LoadString( IDS_NOTE_ARRIS );
	m_strNotes[COptions::kType_Juniper].LoadString( IDS_NOTE_JUNIPER );

	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( pList ) {
		pList->SetExtendedStyle( pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		CRect	rcList;
		pList->GetClientRect( &rcList );

		int	nScrollWidth = GetSystemMetrics( SM_CXVSCROLL );
		if ( nScrollWidth < 10 ) {
			nScrollWidth = 16;
		}
		UInt32	nUnit = (rcList.Width() - nScrollWidth) / 16;

		pList->InsertColumn( 0, L"Address 1" );
		pList->SetColumnWidth( 0, (nUnit * 5) );
		pList->InsertColumn( 1, L"Direction" );
		pList->SetColumnWidth( 1, (nUnit * 4) );
		pList->InsertColumn( 2, L"Address 2" );
		pList->SetColumnWidth( 2, (nUnit * 5) );
		pList->InsertColumn( 3, L"Port" );
		pList->SetColumnWidth( 3, (nUnit * 2) );
	}

	// Call inherited.
	BASE_CLASS::OnInitDialog();

	return TRUE;
}


// -----------------------------------------------------------------------------
//		GetPrefs
// -----------------------------------------------------------------------------

CStringA
COptionsStatus::GetPrefs() const
{
	CStringA	strPrefs;
	strPrefs = m_Options.GetContextPrefs();
	return strPrefs;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COptionsStatus::SetOptions(
	const COptions& inOptions )
{
	m_Options.Copy( inOptions );
}

// -----------------------------------------------------------------------------
//		SetPrefs
// -----------------------------------------------------------------------------

void
COptionsStatus::SetPrefs(
	CStringA	inPrefs )
{
	m_Options.SetContextPrefs( inPrefs );
}


// ----------------------------------------------------------------------------
//		ListenToMessage
// ----------------------------------------------------------------------------

void
COptionsStatus::ListenToMessage(
	CBLMessage&	ioMessage )
{
	switch ( ioMessage.GetId() ) {
	case kBLM_OptionsUpdate:
		{
			CBLOptionsUpdate&	OU( dynamic_cast<CBLOptionsUpdate&>( ioMessage ) );
			const COptions&		NewOptions( OU.GetMessage() );
			m_Options.Copy( NewOptions );
			UpdateData( FALSE );
		}
		break;
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsStatus, BASE_CLASS)
	ON_BN_CLICKED(IDC_MODIFY, OnBnClickedModify)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedModify
// ----------------------------------------------------------------------------

void
COptionsStatus::OnBnClickedModify()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	COptionsDialog	theDialog( NULL );
	theDialog.SetOptions( GetOptions() );

	if ( theDialog.DoModal() == IDOK ) {
		SetOptions( theDialog.GetOptions() );
		BroadcastMessage( CBLOptionsUpdate( GetOptions() ) );
		UpdateData( FALSE );
	}
}
