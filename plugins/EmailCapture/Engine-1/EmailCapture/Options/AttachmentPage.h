// ============================================================================
//	AttachmentPage.h
//		interface for the CAttachmentPage class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include "resource.h"
#include "Case.h"


// ============================================================================
//		CAttachmentPage
// ============================================================================

class CAttachmentPage
	:	public CDialog
{
	DECLARE_DYNAMIC( CAttachmentPage )

protected:
	CCaseSettings&		m_Settings;
	CPeekString			m_strAttachmentDirectory;

	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_ATTACHMENT_OPTS };

	;				CAttachmentPage( CCase& inCase, CWnd* inParentWnd = NULL );
	virtual			~CAttachmentPage() {}

	virtual void	OnCancel();
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();

	afx_msg void	OnBnClickedAttachmentSave();
	afx_msg void	OnBnClickedBrowse();
};

#endif // HE_WIN32
