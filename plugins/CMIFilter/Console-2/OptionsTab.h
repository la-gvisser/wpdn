// ============================================================================
// OptionsTab.h:
//      interface for the COptionsTab class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "resource.h"
#include "XString.h"
#include "afxwin.h"


// ============================================================================
//		COptionsTab
// ============================================================================
 
class COptionsTab
	:	public CDialog
	,	public CBroadcaster
	,	public CListener
{
	DECLARE_DYNAMIC(COptionsTab)

protected:
	COptions		m_Options;
	bool			m_bEditable;

	CStatic			m_MainBox;
	CButton			m_Address2EnabledRadio;
	CButton			m_AnyAddressRadio;
	CStatic			m_Address1Static;
	CStatic			m_Address2Static;
	//CStatic			m_FilterStatic;
	//CButton			m_CMICheck;
	//CStatic			m_CMIBox;
	CIPAddressCtrl	m_Address1Edit;
	CIPAddressCtrl	m_Address2Edit;
	CButton			m_PortCheck;
	CStatic			m_PortStatic;
	CEdit			m_PortEdit;
	CButton			m_MACCheck;
	CStatic			m_MACBox;
	CStatic			m_MACSrcStatic;
	CStatic			m_MACDstStatic;
	CEdit			m_MACSrcEdit;
	CEdit			m_MACDstEdit;
	CStatic			m_MACPrototypeStatic;
	CEdit			m_MACPrototypeEdit;
	CButton			m_VLANCheck;
	CStatic			m_VLANBox;
	CStatic			m_VLANStatic;
	CEdit			m_VLANEdit;
	CButton			m_SaveCheck;
	CStatic			m_SaveBox;
	CStatic			m_SaveDirStatic;
	CEdit			m_SaveDirEdit;
	CButton			m_SaveDirButton;
	CStatic			m_SaveEveryStatic;
	CEdit			m_SaveIntervalEdit;
	CComboBox		m_SaveMultipleCombo;
	CComboBox		m_DirectionCombo;
	CStatic			m_DirectionStatic;
	CButton			m_AgeCheck;
	CEdit			m_AgeEdit;
	CStatic			m_AgeBox;
	CStatic			m_AgeStatic;
	CStatic			m_Age2Static;
	CDialog*		m_pParentDialog;

	X_DECLARE_BOOL( Tab );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_TAB };

	;			COptionsTab( bool inEditable = true );
	virtual		~COptionsTab(){}

	void		SetParentDialog( CDialog* pDialog ) { 
		m_pParentDialog = pDialog;
		m_MainBox.ShowWindow( FALSE );
		//m_CMICheck.SetCheck( TRUE );
		//m_CMICheck.ShowWindow( FALSE );
		//OnBnClickedCMICheck();
	}

	const COptions&	GetOptions() const { return m_Options; };
	CStringA		GetPrefs() const;
	void			SetOptions( const COptions& inOptions );
	void			SetPrefs( CStringA inPrefs );
	BOOL			SetOptionsFromUI();
	void			SetUIFromOptions();

	void			EnableUI( BOOL bEnable );
	//void			EnableCMICheck( BOOL bEnable );
	//void			EnableFilterCheck( BOOL bEnable );
	void			EnablePortCheck( BOOL bEnable );
	void			EnableMacCheck( BOOL bEnable );
	void			EnableVlanCheck( BOOL bEnable );
	void			EnableSaveCheck( BOOL bEnable );
	void			EnableAgeCheck( BOOL bEnable );

	// CListener
	virtual void	ListenToMessage( CBLMessage& inMessage );

	//afx_msg void	OnBnClickedCMICheck();
	afx_msg void	OnBnClickedPortCheck();
	afx_msg void	OnBnClickedMacCheck();
	afx_msg void	OnBnClickedVlanCheck();
	afx_msg void	OnBnClickedSaveCheck();
	afx_msg void	OnBnClickedSaveBrowse();
	afx_msg void	OnBnClickedAgeCheck();
	afx_msg void	OnBnClickedAddress2enabledRadio();
	afx_msg void	OnBnClickedAddress2anyRadio();
	afx_msg void	OnOK();
};
