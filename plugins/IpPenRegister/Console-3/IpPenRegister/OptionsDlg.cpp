// ============================================================================
//	OptionsDlg.cpp:
//		implementation of the COptionsDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "OptionsDlg.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ============================================================================
//		COptionsDlg
// ============================================================================

COptionsDlg::COptionsDlg(
	UINT	nIDCaption,
	CWnd*	pParentWnd,
	UINT	iSelectPage )
	:	CPropertySheet( nIDCaption, pParentWnd, iSelectPage )
	,	m_GeneralPage( m_Options )
{
	AddPage( &m_GeneralPage );
}

COptionsDlg::COptionsDlg(
	LPCTSTR	pszCaption,
	CWnd*	pParentWnd,
	UINT	iSelectPage )
	:	CPropertySheet( pszCaption, pParentWnd, iSelectPage )
	,	m_GeneralPage( m_Options )
{
	AddPage( &m_GeneralPage );
}


// ////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BEGIN_MESSAGE_MAP(COptionsDlg, CPropertySheet )
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
COptionsDlg::OnInitDialog()
{
	(void)CPropertySheet::OnInitDialog();

	return TRUE;
}
