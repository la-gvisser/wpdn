// ============================================================================
//	CaseSheet.cpp
//		implementation of the CCaseSheet class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "CaseSheet.h"
#include <afxwin.h>


// ============================================================================
//		CCaseSheet
// ============================================================================

IMPLEMENT_DYNAMIC(CCaseSheet, CPropertySheet)

CCaseSheet::CCaseSheet(
	UINT		nIDCaption,
	CCase&		inCase,
	CWnd*		pParentWnd,
	UINT		iSelectPage )
	:	CPropertySheet( nIDCaption, pParentWnd, iSelectPage )
	,	m_Case( inCase )
	,	m_ModePage( m_Case )
	,	m_AddressPage( m_Case )
	,	m_TriggerPage( m_Case )
#ifdef _DEBUG
	,	m_AttachmentPage( m_Case )
#endif
{
}

CCaseSheet::CCaseSheet(
	LPCTSTR	pszCaption,
	CCase&	inCase,
	CWnd*	pParentWnd,
	UINT	iSelectPage )
	:	CPropertySheet( pszCaption, pParentWnd, iSelectPage )
	,	m_Case( inCase )
	,	m_ModePage( m_Case )
	,	m_AddressPage( m_Case )
	,	m_TriggerPage( m_Case )
#ifdef _DEBUG
	,	m_AttachmentPage( m_Case )
#endif
{
}


// ----------------------------------------------------------------------------
//		EndDialog
// ----------------------------------------------------------------------------

void 
CCaseSheet::EndDialog(
	int	nEndID )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int nCount( GetPageCount() );
	for ( int i = 0; i < nCount; i++ ) {
		CPropertyPage*	pPage( GetPage( i ) );
		if ( pPage && (pPage->GetSafeHwnd() != NULL) ) {
			pPage->UpdateData( (nEndID == IDOK) ? TRUE : FALSE );
		}
	}

	CPropertySheet::EndDialog( nEndID );
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CCaseSheet::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CPropertySheet::OnInitDialog();

	ModifyStyleEx( 0, WS_EX_CONTROLPARENT );

	int nCount( GetPageCount() );
	for ( int i = (nCount - 1); i < 0; i-- ) {
		CPropertyPage*	pPage = GetPage( i );
		if ( pPage ) {
			SetActivePage( pPage );
		}
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CCaseSheet, CPropertySheet )
END_MESSAGE_MAP()

#endif // HE_WIN32
