// ============================================================================
//	AboutDlg.cpp:
//		implementation of the CAboutDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.h"
#include "Utils.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ============================================================================
//		CAboutDlg
// ============================================================================

CAboutDlg::CAboutDlg(
	CWnd*	pParent /* =NULL */ )
	:	CDialog( CAboutDlg::IDD, pParent )
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CAboutDlg::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );
}


BEGIN_MESSAGE_MAP( CAboutDlg, CDialog )
END_MESSAGE_MAP()

// ////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL
CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ENV_VERSION	evFile;
	ENV_VERSION	evProduct;
	if ( GetPluginVersion( evFile, evProduct) ) {
		CWnd*	pWnd;
		pWnd = GetDlgItem( IDC_VERSION );
		ASSERT( pWnd != NULL );
		if ( pWnd != NULL ) {
			CString	strVersionTemplate;
			strVersionTemplate.LoadString( IDS_VERSION_TEMPLATE );
			if ( strVersionTemplate.IsEmpty() ) {
				strVersionTemplate = _T( "%d.%d.%d.%d" );
			}

			CString	strVersion;
			strVersion.Format(
				strVersionTemplate,
				evFile.MajorVer,
				evFile.MinorVer,
				evFile.MajorRev,
				evFile.MinorRev );

			pWnd->SetWindowText( strVersion );
		}
	}

	return TRUE;	// return TRUE  unless you set the focus to a control
}
