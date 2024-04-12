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
