// ============================================================================
//	UserNameOptions.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CaseOptions.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////
//		CUserNameEdit
///////////////////////////////////////////////////////////////////////////////

class CUserNameEdit 
	: public CEdit
{
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
};


///////////////////////////////////////////////////////////////////////////////
//		CUserNameOptionsDlg
///////////////////////////////////////////////////////////////////////////////

class CUserNameOptionsDlg 
	: public CDialog
{

public:
	enum { IDD = IDD_USERNAME_OPTS };

	CCaseOptions::CUserNameOpts&	m_Options;

	CUserNameOptionsDlg( CCaseOptions::CUserNameOpts& inOptions, bool bIsDockedTab );
	virtual	~CUserNameOptionsDlg() {}

protected:

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	void			UserListToControl();
	void			ControlToUserList();
	void			SetControlsInitialState();

	virtual BOOL	OnInitDialog();

	afx_msg void	OnBnClickedAddUserName();
	afx_msg void	OnBnClickedDeleteUserName();
	afx_msg void	OnNotifyDoubleClickedUserNameList( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnBnClickedClear();
	afx_msg void	OnUserNameChange();
#ifdef CAPTURE_ALL
	afx_msg void	OnBnClickedCaptureAll();
#endif

protected:
	CUserNameEdit	m_UserNameEdit;
	bool			m_bIsDockedTab;
#ifdef CAPTURE_ALL
	BOOL			m_bCaptureAll;
#endif

};
