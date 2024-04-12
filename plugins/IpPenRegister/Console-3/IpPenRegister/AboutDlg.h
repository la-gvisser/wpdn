// ============================================================================
//	AboutDlg.h:
//		interface for the CAboutDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

// ============================================================================
//		CAboutDlg
// ============================================================================

class CAboutDlg
	:	public CDialog
{
protected:
	enum { IDD = IDD_ABOUT };

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange( CDataExchange* pDX );

public:
	;				CAboutDlg( CWnd* pParent = NULL );

	virtual BOOL	OnInitDialog();
};
