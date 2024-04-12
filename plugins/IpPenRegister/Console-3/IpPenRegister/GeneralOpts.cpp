// ============================================================================
//	GeneralOpts.cpp
//		implementation of the CGeneralOpts class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "GeneralOpts.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ============================================================================
//		CGeneralOpts
// ============================================================================

// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CGeneralOpts::DoDataExchange(
	CDataExchange*	pDX )
{
	CPropertyPage::DoDataExchange( pDX );

}


// ////////////////////////////////////////////////////////////////////////////
//	CGeneralOpts message handlers

BEGIN_MESSAGE_MAP(CGeneralOpts, CPropertyPage)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CGeneralOpts::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
