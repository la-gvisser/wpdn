// ============================================================================
//	CaseOptionsDlg.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "resource.h"
#include "CaseOptionsList.h"

#include "GeneralOptsDlg.h"
#include "UserNameOptions.h"
#include "TriggerOptions.h"
#include "PortOptionsDlg.h"
#include "CaptureOptionsDlg.h"

#ifdef MODE_OPTIONS
#include "ModeOpts.h"
#endif
#ifdef INTEGRITY_OPTIONS
#include "IntegrityOpts.h"
#endif


////////////////////////////////////////////////////////////////////////////////
//		CCaseOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CCaseOptionsDlg 
	: public CDialog
{

	DECLARE_DYNAMIC(CCaseOptionsDlg)

public:

	CCaseOptionsDlg(CCaseOptions* CaseOptions, COptions* inOptions, bool bIsCapturing,
					bool bIsDockedTab = false, int Idx = -1, CWnd* pParent = NULL);
	virtual ~CCaseOptionsDlg();

	enum { IDD = IDD_CASE_DLG };

	class CTabItem {
	public:
		CDialog&	m_Tab;
		UINT		m_nResId;
		UINT		m_nId;

		CTabItem( CDialog& pDlg, UINT nResId, UINT nId ) : m_Tab( pDlg ), m_nResId( nResId ), m_nId( nId ) {}
	};

	CAtlArray<CTabItem>	m_TabArray;

	void SetCaseNameDisplay( CPeekString strCaseName ) {	
		CEdit* pEditName = (CEdit*)GetDlgItem( IDC_CASE_NAME_DISPLAY );
		pEditName->SetWindowText( strCaseName );
	}

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK() {}

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();
	void CreateTabs();
	bool ValidateOutputDirectory();
	bool ValidatePorts();
	void ShowTab( UInt32 nTab );
	void InitializeControls();

	afx_msg void OnBnClickedOk();
	
	afx_msg void OnBnClickedTabGeneral() {
		ShowTab( kTab_General );
	}
	afx_msg void OnBnClickedTabUserNames() {
		ShowTab( kTab_UserNames );
	}
	afx_msg void OnBnClickedTabTriggerTimes() {
		ShowTab( kTab_TriggerTimes );
	}
	afx_msg void OnBnClickedTabPorts() {
		ShowTab( kTab_Ports );
	}
	afx_msg void OnBnClickedTabCapture() {
		ShowTab( kTab_Capture );
	}
	CPeekString GetCaseName() {
		return m_GeneralOptsDlg.GetCaseNameEditText();
	}
	void SetCaseName( CPeekString strCaseName ) {
		m_GeneralOptsDlg.SetCaseNameEditText( strCaseName );
	}
	CPeekString GetPrefix() {
		return m_GeneralOptsDlg.GetPrefixEditText();
	}
	void SetPrefix( CPeekString strPrefix ) {
		m_GeneralOptsDlg.SetPrefixEditText( strPrefix );
	}
	void SetActiveTab( UINT nTab ) {
		ShowTab( nTab );
	}
	void ShowNotAllowedDialog() {
		ASSERT( m_bIsCapturing );
		MessageBox( _T("This item cannot be changed during an active capture."), _T("Radius Options"), MB_OK );
	}
	
protected:
	CCaseOptions*		m_pCaseOptions;
	CCaseOptionsList&	m_CaseOptionsList;
	CPeekString			m_strGlobalOutputDirectory;
	int					m_CaseOptionsListIdx;
	bool				m_bIsCapturing;
	bool				m_bIsDockedTab;
	CPeekString			m_strDefaultKeyName;
	CGeneralOptionsDlg	m_GeneralOptsDlg;
	CUserNameOptionsDlg	m_UserNameOptsDlg;
	CTriggerOptionsDlg	m_TriggerOptionsDlg;
	CCasePortOptionsDlg	m_CasePortOptionsDlg;
	CCaptureOptionsDlg	m_CaptureOptionsDlg;
	bool				m_bInitializing;
	COptions*			m_pOptions;

  #ifdef MODE_OPTIONS
	CModeOpts			m_ModeOpts;
  #endif
  #ifdef INTEGRITY_OPTIONS
	CIntegrityOpts		m_IntegrityOpts;
  #endif

};
