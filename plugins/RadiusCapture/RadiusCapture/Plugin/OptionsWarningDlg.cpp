// ============================================================================
//	OptionsWarningDlg.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "OptionsWarningDlg.h"


//////////////////////////////////////////////////////////////////////////
// COptionsWarningDlg dialog
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(COptionsWarningDlg, CDialog)

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

COptionsWarningDlg::COptionsWarningDlg( bool bDisplayWarning,  CWnd* pParent /*=NULL*/ )
	: CDialog(COptionsWarningDlg::IDD, pParent)
{
	SetDisplayWarning( bDisplayWarning );
}

// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsWarningDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void COptionsWarningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_MESSAGE, CEdtMessage);
	DDX_Control(pDX, IDC_CHK_WARN, m_BtnDisplayWarning);
}

// -----------------------------------------------------------------------------
//	OnInitDialog
// -----------------------------------------------------------------------------

#define	OPTIONS_WARNING_MESSAGE  _T("Warning: there is currently a live capture taking place.  ")			\
_T("Changing options at this time can have a significant impact on the capture.\r\n\r\n")					\
_T("Press \"Do not apply new options\" if you do not want to apply these changes at this time.\r\n\r\n")    \
_T("Press \"Continue capture with new options\" if you want to apply the changes during this live capture.\r\n\r\n")

BOOL
COptionsWarningDlg::OnInitDialog()
{
	 CDialog::OnInitDialog();

	 m_Message = _T("Warning: options will be changed during a live capture.  Changing options at this ")	+
		 (CString)_T("time can have significant impact on the capture.\r\n\r\n")							+
		 (CString)_T("Press \"\" if you do not want to continue to make these changes.");

	 CEdtMessage.SetWindowText( OPTIONS_WARNING_MESSAGE );

		 m_BtnDisplayWarning.SetCheck( GetDisplayWarning() );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// -----------------------------------------------------------------------------
//	OnBnClickedOk
// -----------------------------------------------------------------------------

void
COptionsWarningDlg::OnBnClickedOk()
{
	OnOK();

	SetDisplayWarning( m_BtnDisplayWarning.GetCheck() == TRUE );
}
