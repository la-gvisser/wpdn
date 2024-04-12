// ============================================================================
//	GeneralOptsDlg.h
//		interface for the CGeneralOpts class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CaseOptions.h"
#include "resource.h"

class CCaseOptionsDlg;


////////////////////////////////////////////////////////////////////////////////
//		CGeneralOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CGeneralOptionsDlg 
	: public CDialog
{

public:
	enum { IDD = IDD_GENERAL_OPTS };

	CGeneralOptionsDlg( CCaseOptions& inCaseOptions, CPeekString strOutputRootDirectory,
					CCaseOptionsDlg* pCaseOptionsDlg, bool bCapturing, bool bIsDockedTab );

	virtual	~CGeneralOptionsDlg(){}

	CPeekString GetCaseNameEditText() {
		CString strCaseName;
		GetDlgItemText( IDC_NAME_EDIT, strCaseName );
		return (CPeekString)strCaseName;
	}
	void SetCaseNameEditText( CPeekString strCaseName ) {
		bool bISaveInitializing( m_bInitializing );
		m_bInitializing = true;
		CEdit* pCaseNameEdit = (CEdit*)GetDlgItem( IDC_NAME_EDIT );
		if ( !pCaseNameEdit ) return;
		pCaseNameEdit->SetWindowText( (CString)strCaseName );
		int nLen( static_cast<int>( strCaseName.GetLength() ) );
		pCaseNameEdit->SetSel( nLen, nLen, FALSE );
		m_bInitializing = bISaveInitializing;
	}
	CPeekString GetPrefixEditText() {
		CString strPrefix;
		GetDlgItemText( IDC_EDIT_PREFIX, strPrefix );
		return (CPeekString)strPrefix;
	}

	void SetPrefixEditText( CPeekString strPrefix ) {
		bool bISaveInitializing( m_bInitializing );
		m_bInitializing = true;
		CEdit* pCaseNameEdit = (CEdit*)GetDlgItem( IDC_EDIT_PREFIX );
		if ( !pCaseNameEdit ) return;
		pCaseNameEdit->SetWindowText( (CString)strPrefix );
		int nLen( static_cast<int>( strPrefix.GetLength() ) );
		pCaseNameEdit->SetSel( nLen, nLen, FALSE );
		m_bInitializing = bISaveInitializing;
	}

	void SetOutputDirText( CPeekString strOutputDir ) {
		bool bISaveInitializing( m_bInitializing );
		m_bInitializing = true;
		CEdit* pOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
		if ( !pOutputDir ) return;
		pOutputDir->SetWindowText( (CString)strOutputDir );
		int nLen( static_cast<int>( strOutputDir.GetLength() ) );
		pOutputDir->SetSel( nLen, nLen, FALSE );
		m_bInitializing = bISaveInitializing;
	}


protected:

	DECLARE_MESSAGE_MAP()

	void		SetControlsInitialState();
	void		SetControlsState();
	void		SetDataToScreen();
	void		SuggestAndSetOutputDir();
	bool		GetEditControlText( CEdit* pEditControl, CPeekString& InOutText, int nMaxLength );


	virtual BOOL	OnInitDialog();
	afx_msg void	OnCaseNameChange();
	afx_msg void	OnPrefixChange();
	afx_msg void	OnBnClickedSuggestOutput();
	afx_msg void	OnBnClickedCaseBrowseBtn();
	afx_msg void	OnFileDirectoryChange();

protected:
	CCaseOptions&					m_CaseOptions;
	CCaseOptions::CGeneralOptions&	m_GeneralOptions;
	CPeekString						m_strGlobalOutputRootDir;
	bool							m_bIsCapturing;
	bool							m_bIsDockedTab;
	CCaseOptionsDlg*				m_pCaseOptionsDlg;
	bool							m_bInitializing;
};
