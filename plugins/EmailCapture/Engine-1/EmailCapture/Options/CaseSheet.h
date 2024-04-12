// ============================================================================
//	CaseSheet.h
//		interface for the CCaseSheet class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "Options.h"
#include "AddressPage.h"
#include "ModePage.h"
#include "TriggerPage.h"
#include "AttachmentPage.h"
#include "afxdlgs.h"


// ============================================================================
//		CCaseSheet
// ============================================================================

class CCaseSheet
	:	public CPropertySheet
{
	DECLARE_DYNAMIC( CCaseSheet )

protected:
	DECLARE_MESSAGE_MAP()

public:
	COptions		m_Options;

	CCase&			m_Case;

	CModePage		m_ModePage;
	CAddressPage	m_AddressPage;
	CTriggerPage	m_TriggerPage;
#ifdef _DEBUG
	CAttachmentPage	m_AttachmentPage;
#endif

	;				CCaseSheet( UINT nIDCaption, CCase& inCase, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	;				CCaseSheet( LPCTSTR pszCaption, CCase& inCase, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual			~CCaseSheet() {}

	virtual	BOOL	OnInitDialog();
	virtual void	EndDialog( int nEndID );
};
