// ============================================================================
//	PortOptionsDlg.h
//		interface for the CPortOptionsDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once
#include "Options.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////
//		CPortOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CPortOptionsDlg
	: public CDialog
{
	DECLARE_DYNAMIC(CPortOptionsDlg)

public:
	enum { IDD = IDD_PORT_OPTS };

	CPortOptionsDlg( COptions* inOptions, bool bDisableControls, CWnd* pParent = NULL )
		:	CDialog(CPortOptionsDlg::IDD, pParent)
		,	m_bDisableControls( bDisableControls )
		,	m_PortNumberList( m_PortOptions.GetPortNumberList() ) 
		,	m_bInitializing( false )
		{
			m_PortOptions = inOptions->GetPortOptions();
		}

	virtual	~CPortOptionsDlg() {}

	CPortOptions& GetPortOptions() { return m_PortOptions; }

protected:
	virtual BOOL	OnInitDialog();

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	afx_msg void OnBnClickedAddPort();
	afx_msg void OnBnClickedDeletePort();
	afx_msg void OnEnChangeEditPort();
	afx_msg void OnBnClickedSetDefaultPorts();
	afx_msg void OnLvnItemchangedPortList(NMHDR* /*pNMHDR*/, LRESULT *pResult);

	void ControlToList();
	void ListToControl();
	void SetControlsInitialState();

protected:
	CPortOptions	 m_PortOptions;
	CPortNumberList& m_PortNumberList;
	bool			 m_bDisableControls;
	bool			 m_bInitializing;
};


////////////////////////////////////////////////////////////////////////////////
//		CCasePortOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CCasePortOptionsDlg
	: public CDialog
{
	DECLARE_DYNAMIC(CCasePortOptionsDlg)

public:
	CCasePortOptionsDlg( CPortOptions& inPortOptions, COptions* inOptions, bool bIsCapturing, bool bIsDockedTab )
		:	m_PortOptions( inPortOptions )
		,	m_PortNumberList( inPortOptions.GetPortNumberList() )
		,	m_GlobalPortsList( inOptions->GetPortNumberList() ) 
		,	m_bIsCapturing( bIsCapturing )
		,	m_bIsDockedTab( bIsDockedTab )
	{}

	virtual ~CCasePortOptionsDlg() {}

	enum { IDD = IDD_PORT_PROP_OPTS };

protected:

	CPortOptions&	 m_PortOptions;
	CPortNumberList& m_PortNumberList;
	CPortNumberList& m_GlobalPortsList;
	bool			 m_bIsCapturing;
	bool			 m_bIsDockedTab;

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	void ControlToList();
	void ListToControl();
	void SetControlsState();
	void SetControlsInitialState();

	void SetDefaultCheckbox() {
		CButton* chkUseGlobalDefaults = (CButton*)GetDlgItem( IDC_CHK_USE_GLOBAL_PORTS );
		BOOL bChecked = m_PortOptions.IsUsingGlobalDefaults();
		chkUseGlobalDefaults->SetCheck( bChecked );
	}

	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL	OnInitDialog();
	afx_msg void	OnBnClickedAddPort();
	afx_msg void	OnBnClickedDeletePort();

	CPortOptions& GetPortOptions() { return m_PortOptions; }
	afx_msg void OnEnChangeEditPort();
	afx_msg void OnBnClickedChkUseGlobalPorts();
};
