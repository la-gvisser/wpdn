// ============================================================================
//	AboutDialog.cpp
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef _WIN32

#include "AboutDialog.h"
#include "PeekStrings.h"
#include "Version.h"

using namespace Environment;


// ============================================================================
//		CAboutDialog
// ============================================================================

CAboutDialog::CAboutDialog(
	HINSTANCE	inInstance,
	CWnd*		inParent /*=nullptr*/ )
	:	CDialog( CAboutDialog::IDD, inParent )
	,	m_hInstance( inInstance )
{
}

CAboutDialog::~CAboutDialog()
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CAboutDialog::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CAboutDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

#if (0)
	CVersion	verFile;
	CVersion	verProduct;
	if ( Environment::GetModuleVersion( verFile, verProduct, m_hInstance ) ) {
		CWnd*	pWnd( GetDlgItem( IDC_PLUGIN_VERSION ) );
		_ASSERTE( pWnd != nullptr );
		if ( pWnd != nullptr ) {
			CPeekString	strVersion( L"Version: " + verFile.Format() );
			pWnd->SetWindowText( strVersion );
		}
	}
#endif // (0)

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CAboutDialog, CDialog )
END_MESSAGE_MAP()

#endif // HE_WIN32
