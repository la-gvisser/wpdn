// ============================================================================
//	OptionsWarningDlg.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once
#include "afxwin.h"

//////////////////////////////////////////////////////////////////////////
// COptionsWarningDlg dialog
//////////////////////////////////////////////////////////////////////////

class COptionsWarningDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsWarningDlg)

public:
	COptionsWarningDlg( bool bDisplayWarning, CWnd* pParent = NULL );
	virtual ~COptionsWarningDlg(){}

	enum { IDD = IDD_OPTIONS_WARNING };

	bool GetDisplayWarning() {
		return m_bDisplayWarning;
	}
	void SetDisplayWarning( bool bDisplayWarning ) {
		m_bDisplayWarning = bDisplayWarning;
	}

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

protected:
	CString	m_Message;
	bool	m_bDisplayWarning;
	CEdit	CEdtMessage;
	CButton m_BtnDisplayWarning;
};
