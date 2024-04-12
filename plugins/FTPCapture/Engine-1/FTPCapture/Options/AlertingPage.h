// ============================================================================
//	AlertingPage.h
//		interface for the CAlertPage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "Options.h"


// ============================================================================
//		CAlertingPage
// ============================================================================

class CAlertingPage :
	public CPropertyPage
{
protected:
	COptions&		m_Options;
	CAlerting&		m_Alerting;
	CLogging&		m_Logging;

	virtual void	DoDataExchange( CDataExchange* pDX );
	virtual BOOL	OnInitDialog();

public:
	enum { IDD = IDD_ALERT };

	;		CAlertingPage( COptions& inOptions );
	;		~CAlertingPage() {}

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnBnClickedAlerting();
//	afx_msg void	OnBnClickedBrowse();
	afx_msg void	OnBnClickedObscureNames();
//	afx_msg void	OnBnClickedEnableLogging();
};
