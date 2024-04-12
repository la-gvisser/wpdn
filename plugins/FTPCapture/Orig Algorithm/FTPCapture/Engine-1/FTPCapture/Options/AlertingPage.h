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
	CAlerting&		m_Alerting;
	CLogging&		m_Logging;

	virtual void	DoDataExchange( CDataExchange* pDX );
	virtual BOOL	OnInitDialog();

public:
	enum { IDD = IDD_ALERT };

	;		CAlertingPage( CAlerting& inAlerting, CLogging& inLogging );
	;		~CAlertingPage() {}

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnBnClickedAlerting();
	afx_msg void	OnBnClickedBrowse();
};
