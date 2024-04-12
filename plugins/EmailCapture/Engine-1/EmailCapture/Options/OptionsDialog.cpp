// ============================================================================
//	OptionsDlg.cpp
//		implementation of the COptionsDialog class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008-2100. All rights reserved.

#include "StdAfx.h"

#ifdef PEEK_UI

#include "OptionsDialog.h"
#include "OmniPlugin.h"
#include "OmniConsoleContext.h"
#include "GlobalTags.h"
#include <afxwin.h>


// ============================================================================
//		COptionsDialog
// ============================================================================

IMPLEMENT_DYNAMIC( COptionsDialog, CDialog )

COptionsDialog::COptionsDialog(
	const COptions&	inOptions,
	CWnd*			pParent /*=NULL*/ )
	:	CDialog( COptionsDialog::IDD, pParent )
	,	m_CasesOpts( inOptions )
{
}

COptionsDialog::~COptionsDialog()
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void COptionsDialog::DoDataExchange(
	CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
COptionsDialog::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnInitDialog();

	const int	nBorder( 12 );
	const int	nButtonHeight( 23 );

	CRect	rcClient;
	GetClientRect( rcClient );
	rcClient.bottom -= (nBorder * 2) + nButtonHeight;

	BOOL	bResult( m_CasesOpts.Create( CCasesOpts::IDD, this ) );
	ASSERT( bResult != FALSE );
	m_CasesOpts.MoveWindow( rcClient );
	m_CasesOpts.ShowWindow( SW_SHOW );

	SetTimer( 1, 100, NULL );

	return TRUE;
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void 
COptionsDialog::OnOK()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnOK();
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnTimer
// ----------------------------------------------------------------------------

void
COptionsDialog::OnTimer(
	UINT_PTR	nIDEvent )
{
	CDialog::OnTimer( nIDEvent );

	CWnd*	pOk( GetDlgItem( IDOK ) );
	if ( pOk ) {
		pOk->Invalidate();
	}

	CWnd*	pCancel( GetDlgItem( IDCANCEL ) );
	if ( pCancel ) {
		pCancel->Invalidate();
	}
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void 
COptionsDialog::OnSize(
	UINT	nType,
	int		cx,
	int		cy ) 
{
	CDialog::OnSize( nType, cx, cy );

	const int	nBorder( 12 );
	const int	nButtonGap( 8 );
	int			nButtonHeight( 23 );
	const int	nButtonTop( cy - (nBorder + nButtonHeight) );

	CWnd*	pOk( GetDlgItem( IDOK ) );
	if ( pOk ) {
		CRect	rc;
		pOk->GetWindowRect( &rc );
		CPoint	ptXY( (cx - ((rc.Width() + nButtonGap + 4) * 2)), nButtonTop );
		rc.MoveToXY( ptXY );
		pOk->MoveWindow( &rc );
		nButtonHeight = rc.Height();
	}

	CWnd*	pCancel( GetDlgItem( IDCANCEL ) );
	if ( pCancel ) {
		CRect	rc;
		pCancel->GetWindowRect( &rc );
		CPoint	ptXY( (cx - (rc.Width() + nButtonGap + 4)), nButtonTop );
		rc.MoveToXY( ptXY );
		pCancel->MoveWindow( &rc );
		nButtonHeight = rc.Height();
	}

	if ( m_CasesOpts.GetSafeHwnd() ) {
		int		nOptHeight( cy - ((nBorder * 2) + nButtonHeight) );
		CRect	rcOpt( 0, 0, cx, nOptHeight );	// l, t, r, b
		m_CasesOpts.MoveWindow( rcOpt );
	}
}

#endif // PEEK_UI
