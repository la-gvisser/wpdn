// ============================================================================
//	PortNumberPage.h
//		interface for the CPortNumberPage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#pragma once

#include "Options.h"

class CPortNumberPage :
	public CPropertyPage
{
protected:
	CPortNumberList&	m_PortNumbers;
	bool				m_bInPortEdit;
	UInt16				m_nLastPort;

	void			ControlToList();
	void			ListToControl();

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_PORT_NUMBERS };

	;				CPortNumberPage( CPortNumberList& inOther );
	;				~CPortNumberPage() {}

	virtual BOOL	OnInitDialog();
	afx_msg void	OnBnClickedAddPort();
	afx_msg void	OnBnClickedRemovePort();
	afx_msg void	OnEnChangePortNumber();
};
