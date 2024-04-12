// ============================================================================
//	ImportOptionsDlg.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "stdafx.h"
#include "ReplaceCaseDlg.h"


////////////////////////////////////////////////////////////////////////////////
//		CReplaceCaseDlg
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CReplaceCaseDlg, CDialog)


// -----------------------------------------------------------------------------
//  Construction
// -----------------------------------------------------------------------------

CReplaceCaseDlg::CReplaceCaseDlg(
	 CPeekString inMessage,
	 int& nResponse,
	 CWnd* pParent /*=NULL*/ )
	 :	CDialog( CReplaceCaseDlg::IDD_MODAL, pParent )
	 ,	m_strMessage( inMessage )
	 ,	m_nResponse( nResponse )
{
}


// -----------------------------------------------------------------------------
//  OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CReplaceCaseDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CEdit* pReplaceMsgEdit = (CEdit*)GetDlgItem( IDC_EDT_REPLACE_MSG );
	ASSERT( pReplaceMsgEdit );
	if ( pReplaceMsgEdit ) {
		pReplaceMsgEdit->SetWindowText( m_strMessage );
	}
	return FALSE;
}


BEGIN_MESSAGE_MAP(CReplaceCaseDlg, CDialog)
	ON_BN_CLICKED(ID_YES, OnBnClickedYes)
	ON_BN_CLICKED(ID_NO, OnBnClickedNo)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//  OnBnClickedYes
// -----------------------------------------------------------------------------

void
CReplaceCaseDlg::OnBnClickedYes()
{
	m_nResponse |= IMPORT_RESPONSE_YES;
	
	CButton* pAlwaysAction = (CButton*)GetDlgItem( IDC_CHK_ALWAYS );
	if ( pAlwaysAction->GetCheck() == TRUE ) m_nResponse |= IMPORT_RESPONSE_ALWAYS;
	
	OnOK();
}


// -----------------------------------------------------------------------------
//  OnBnClickedNo
// -----------------------------------------------------------------------------

void
CReplaceCaseDlg::OnBnClickedNo()
{
	m_nResponse |= IMPORT_RESPONSE_NO;

	CButton* pAlwaysAction = (CButton*)GetDlgItem( IDC_CHK_ALWAYS );
	if ( pAlwaysAction->GetCheck() == TRUE ) m_nResponse |= IMPORT_RESPONSE_ALWAYS;

	OnOK();
}
