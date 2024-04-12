// ============================================================================
//	CMIFilterAbout.cpp:
//		implementation of the CCMIFilterAbout class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "CMIFilterAbout.h"
#include "Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ============================================================================
//		CCMIFilterAbout
// ============================================================================

CCMIFilterAbout::CCMIFilterAbout(
	CWnd* pParent /*=NULL*/)
	:	CDialog( CCMIFilterAbout::IDD, pParent )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
CCMIFilterAbout::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CCMIFilterAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	ENV_VERSION		evFile;
	ENV_VERSION		evProduct;

	TCHAR 	szFilePath[MAX_PATH];
	::GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );

	if ( CEnvironment::GetFileVersion( szFilePath, evFile, evProduct ) ) {
		CWnd*	pWnd( GetDlgItem( IDC_VERSION ) );
		ASSERT( pWnd != NULL );
		if ( pWnd != NULL ) {
			CString	strVersionTemplate;
			strVersionTemplate.LoadString( IDS_VERSION_TEMPLATE );

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

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CCMIFilterAbout, CDialog )
END_MESSAGE_MAP()
