// ============================================================================
// WizardInstallerDlg.h : header file
// ============================================================================
//	Copyright (c) Savvius, Inc. 2006-2017. All rights reserved.

#pragma once


///////////////////////////////////////////////////////////////////////////////
// CWizardInstallerDlg dialog
///////////////////////////////////////////////////////////////////////////////

class CWizardInstallerDlg
	:	public CDialog
{
public:
	static const int	g_nMaxVersions = 2;

protected:
	enum { IDD = IDD_WIZARDINSTALLER_DIALOG };
	typedef enum {
		kState_Load,
		kState_Asking,
		kState_Done
	} tInstall_States;


	HICON			m_hIcon;

	tInstall_States	m_nState;

	CWnd			m_lblInstall;
	CWnd			m_lblStatus;
	CButton			m_btnOK;
	CButton			m_btnCancel;
	CButton			m_btnUninstall;
	CButton			m_btnVS[g_nMaxVersions];
	CWnd			m_lblVS[g_nMaxVersions];
	CString			m_strVSPath[g_nMaxVersions];

	bool			CheckInstalls();

	tInstall_States	Setup();
	bool			MakePath( const CString& inPath );
	bool			InstallFile( CString inSrc, CString inDst );
	bool			InstallWizard( int inIndex );
	tInstall_States	Install();
	bool			RemovePlugin( int inIndex );
	tInstall_States	Uninstall();

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange(CDataExchange* pDX);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg HCURSOR	OnQueryDragIcon();

public:
	;			CWizardInstallerDlg(CWnd* pParent = NULL);
	virtual		~CWizardInstallerDlg();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedUninstall();
};
