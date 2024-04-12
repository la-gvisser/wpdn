// ============================================================================
//	AddressPage.h
//		interface for the CAddressPage class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "resource.h"
#include "Case.h"


// ============================================================================
//		CAddressEdit
// ============================================================================

class CAddressEdit
	:	public CEdit
{
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
};


// ============================================================================
//		CAddressPage
// ============================================================================

class CAddressPage
	:	public CDialog
{
	DECLARE_DYNAMIC( CAddressPage )

protected:
	CTargetAddressList&	m_AddressList;

	static CPeekString	m_strRegEmail;
	static CPeekString	m_strRegDomain;

	CAddressEdit	m_AddrEdit;

	void			UserListToControl();
	void			ControlToUserList();
	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_ADDRESS_OPTS };

	;				CAddressPage( CCase& inCase );
	virtual			~CAddressPage() {}

	virtual void	OnCancel();
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();

	afx_msg void	OnBnClickedCaptureAll();
	afx_msg void	OnBnClickedAddAddress();
	afx_msg void	OnBnClickedDeleteAddress();
	afx_msg void	OnNotifyDoubleClickedAddressList( NMHDR* pNMHDR, LRESULT* pResult );
};
