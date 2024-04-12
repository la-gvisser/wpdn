// ============================================================================
//	TargetNamePage.h
//		interface for the CTargetNamePage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.
#pragma once

#include "Options.h"

class CTargetNamePage :
	public CPropertyPage
{
protected:
	BOOL				m_bCaptureAll;
	CTargetNameList&	m_TargetNames;

	void			ControlToList();
	void			ListToControl();

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_TARGET_NAMES };

	;				CTargetNamePage( CTargetNameList& inOther );
	;				~CTargetNamePage() {}

	bool			IsCaptureAll() const { return (m_bCaptureAll != FALSE); }

	virtual BOOL	OnInitDialog();
	afx_msg void	OnBnClickedAnyName();
	afx_msg void	OnBnClickedAddTarget();
	afx_msg void	OnBnClickedRemoveTarget();
};
