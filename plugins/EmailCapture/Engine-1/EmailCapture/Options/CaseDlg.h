// ============================================================================
//	CaseDlg.h
//		interface for the CCaseDlg class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include "resource.h"
#include "Options.h"
#include "AddressPage.h"
#include "ModePage.h"
#include "TriggerPage.h"
#if SAVE_ATTACHMENTS
#include "AttachmentPage.h"
#endif


// ============================================================================
//		CCaseDlg
// ============================================================================

class CCaseDlg
	:	public CDialog
{
	DECLARE_DYNAMIC( CCaseDlg )

protected:
	DECLARE_MESSAGE_MAP()

	CCase						m_Case;
	COptions::COptionsCases*	m_pCaseList;
	//CPeekString					m_strCaseName;
	CModePage					m_ModePage;
	CAddressPage				m_AddressPage;
	CTriggerPage				m_TriggerPage;
#if SAVE_ATTACHMENTS
	CAttachmentPage				m_AttachmentPage;
#endif

	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

public:
	enum { IDD = IDD_CASE_DLG };

	;				CCaseDlg( CCase& inCase, COptions::COptionsCases* inCaseList = NULL,
						CWnd* pParent = NULL );
	virtual			~CCaseDlg() {};

	const CCase&	GetCase() const { return m_Case; }

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();

	afx_msg void	OnTvnSelchangedCaseTree( NMHDR *pNMHDR, LRESULT *pResult );
};

#endif // HE_WIN32
