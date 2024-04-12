// ============================================================================
//	ReplaceCaseDlg.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "resource.h"


#define IMPORT_RESPONSE_CANCEL		0x00
#define IMPORT_RESPONSE_YES			0x01
#define IMPORT_RESPONSE_NO			0x02
#define IMPORT_RESPONSE_ALWAYS		0x04


////////////////////////////////////////////////////////////////////////////////
//		CReplaceCaseDlg
////////////////////////////////////////////////////////////////////////////////

class CReplaceCaseDlg 
	: public CDialog
{
	DECLARE_DYNAMIC(CReplaceCaseDlg)

public:

	CReplaceCaseDlg::CReplaceCaseDlg( CString inMessage, int& nResponse, CWnd* pParent = NULL );
	virtual ~CReplaceCaseDlg() {}

	enum { IDD_MODAL = IDD_REPLACE_CASE_DLG };

protected:

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedYes();
	afx_msg void OnBnClickedNo();

protected:
	CString	m_strMessage;
	int&	m_nResponse;

};
