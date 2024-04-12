// =============================================================================
//	OptionsDialog.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef PEEK_UI

#include "OptionsDialog.h"
#include "AboutDialog.h"
#include "PeekTime.h"


// =============================================================================
//		COptionsDialog
// =============================================================================

COptionsDialog::COptionsDialog(
	const COptions&	inOptions,
	CWnd*			inParent /*= NULL*/  )
	:	CDialog( COptionsDialog::IDD, inParent )
	,	m_Options( inOptions )
{
}

// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
COptionsDialog::DoDataExchange(
	CDataExchange* pDX )
{
	pDX;

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
  #ifdef IMPLEMENT_NOTIFICATIONS
	BOOL	bReceiveNotifies = FALSE;
  #endif // IMPLEMENT_NOTIFICATIONS
	CString		strLabel;
	CString		strEthernet;
	UInt32		nIpAddress;
	UInt32		nPort;
	FILETIME	ftTime;
	FILETIME	ftDate;

	if ( !pDX->m_bSaveAndValidate ) {
  #ifdef IMPLEMENT_NOTIFICATIONS
		bReceiveNotifies = m_Options.IsReceiveNotifies() ? TRUE : FALSE;
  #endif // IMPLEMENT_NOTIFICATIONS
		strLabel = m_Options.m_strLabelExample;
		strEthernet = m_Options.m_EthernetExample.Format();
		nIpAddress = m_Options.m_IpAddressExample;
		nPort = m_Options.m_nPortExample;
		m_Options.m_TimeExample.GetLocalTimeDate( ftDate, ftTime );
	}

  #ifdef IMPLEMENT_NOTIFICATIONS
	DDX_Check( pDX, IDC_CHK_RECEIVENOTIFIES, bReceiveNotifies );
  #endif // IMPLEMENT_NOTIFICATIONS
	DDX_Check( pDX, IDC_ENABLE_EXAMPLE, m_Options.m_bEnabledExample );
	DDX_Text( pDX, IDC_LABEL_EXAMPLE, strLabel );
	DDX_Text( pDX, IDC_ETHERNET_EXAMPLE, strEthernet );
	DDX_IPAddress( pDX, IDC_ADDRESS_EXAMPLE, nIpAddress );
	DDX_Text( pDX, IDC_PORT_EXAMPLE, nPort );
	DDX_DateTimeCtrl( pDX, IDC_TIME_EXAMPLE, ftTime );
	DDX_MonthCalCtrl( pDX, IDC_DATE_EXAMPLE, ftDate );

	if ( pDX->m_bSaveAndValidate ) {
	  #ifdef IMPLEMENT_NOTIFICATIONS
		m_Options.SetReceiveNotifies( bReceiveNotifies ? true : false );
	  #endif // IMPLEMENT_NOTIFICATIONS
		m_Options.m_strLabelExample = strLabel.GetString();
		m_Options.m_EthernetExample = strEthernet.GetString();
		m_Options.m_IpAddressExample = nIpAddress;
		m_Options.m_nPortExample = static_cast<UInt16>( nPort );
		m_Options.m_TimeExample.SetLocal( ftDate, ftTime );
	}
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnBnClickedEnableExample();

	return TRUE;
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( COptionsDialog, CDialog )
	ON_BN_CLICKED(IDC_ENABLE_EXAMPLE, &COptionsDialog::OnBnClickedEnableExample)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedEnableExample
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedEnableExample()
{
	BOOL	bIsEnabled = (IsDlgButtonChecked( IDC_ENABLE_EXAMPLE ) == BST_CHECKED) ? TRUE : FALSE;

	int		nCtrlIds[] = {
		IDC_LABEL_EXAMPLE,
		IDC_LABEL_LABEL_EXAMPLE,
		IDC_ETHERNET_EXAMPLE,
		IDC_ETHERNET_LABEL_EXAMPLE,
		IDC_ADDRESS_EXAMPLE,
		IDC_ADDRESS_LABEL_EXAMPLE,
		IDC_PORT_EXAMPLE,
		IDC_PORT_LABEL_EXAMPLE,
		IDC_TIME_EXAMPLE,
		IDC_TIME_LABEL_EXAMPLE,
		IDC_DATE_EXAMPLE,
		IDC_DATE_LABEL_EXAMPLE
	};

	for ( int i = 0; i < COUNTOF( nCtrlIds ); i++ ) {
		CWnd*	pWnd = GetDlgItem( nCtrlIds[i] );
		ASSERT( pWnd != NULL );
		if ( pWnd ) pWnd->EnableWindow( bIsEnabled );
	}
}

#endif // HE_WIN32
