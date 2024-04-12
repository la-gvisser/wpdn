// ============================================================================
// OptionsTab.cpp:
//      implementation of the COptionsTab class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "CMIFilter.h"
#include "Options.h"
#include "MediaSpec.h"
#include "MediaSpecUtil.h"
#include "OptionsTab.h"

#define BASE_CLASS CDialog


// ============================================================================
//		COptionsTab
// ============================================================================

IMPLEMENT_DYNAMIC(COptionsTab, BASE_CLASS)

COptionsTab::COptionsTab(
	bool	inEditable /* = true */ )
	:	BASE_CLASS()
	,	m_bEditable( inEditable )
	,	m_pParentDialog( NULL )
{
	// Initialize options to NULL
	SetTab( FALSE );
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
COptionsTab::DoDataExchange(
	CDataExchange*	pDX )
{
	BASE_CLASS::DoDataExchange( pDX );

	//DDX_Control( pDX, IDC_CMI_CHECK, m_CMICheck );
	//DDX_Control( pDX, IDC_FILTER_STATIC, m_FilterStatic );
	DDX_Control( pDX, IDC_ADDRESS1_STATIC, m_Address1Static );
	DDX_Control( pDX, IDC_ADDRESS2_STATIC, m_Address2Static );
	DDX_Control( pDX, IDC_ADDRESS1_EDIT, m_Address1Edit );
	DDX_Control( pDX, IDC_ADDRESS2_EDIT, m_Address2Edit );
	DDX_Control( pDX, IDC_ADDRESS2ANY_RADIO, m_AnyAddressRadio );
	DDX_Control( pDX, IDC_ADDRESS2ENABLED_RADIO, m_Address2EnabledRadio );
	DDX_Control( pDX, IDC_DIRECTION_COMBO, m_DirectionCombo );
	DDX_Control( pDX, IDC_DIRECTION_STATIC, m_DirectionStatic );
	DDX_Control( pDX, IDC_PORT_STATIC, m_PortStatic );
	DDX_Control( pDX, IDC_PORT_CHECK, m_PortCheck );
	DDX_Control( pDX, IDC_PORT_EDIT, m_PortEdit );
	DDX_Control( pDX, IDC_MAC_CHECK, m_MACCheck );
	DDX_Control( pDX, IDC_MAC_BOX, m_MACBox );
	DDX_Control( pDX, IDC_SRCMAC_STATIC, m_MACSrcStatic );
	DDX_Control( pDX, IDC_DSTMAC_STATIC, m_MACDstStatic );
	DDX_Control( pDX, IDC_SRCMAC_EDIT, m_MACSrcEdit );
	DDX_Control( pDX, IDC_DSTMAC_EDIT, m_MACDstEdit );
	DDX_Control( pDX, IDC_PROTOTYPE_STATIC, m_MACPrototypeStatic );
	DDX_Control( pDX, IDC_PROTOTYPE_EDIT, m_MACPrototypeEdit );
	DDX_Control( pDX, IDC_VLAN_CHECK, m_VLANCheck );
	DDX_Control( pDX, IDC_VLAN_BOX, m_VLANBox );
	DDX_Control( pDX, IDC_VLAN_STATIC, m_VLANStatic );
	DDX_Control( pDX, IDC_VLAN_EDIT, m_VLANEdit );
	DDX_Control( pDX, IDC_SAVE_CHECK, m_SaveCheck );
	DDX_Control( pDX, IDC_SAVE_BOX, m_SaveBox );
	DDX_Control( pDX, IDC_SAVEDIR_STATIC, m_SaveDirStatic );
	DDX_Control( pDX, IDC_SAVEDIR_EDIT, m_SaveDirEdit );
	DDX_Control( pDX, IDC_SAVE_BROWSE, m_SaveDirButton );
	DDX_Control( pDX, IDC_SAVEFILE_STATIC, m_SaveEveryStatic );
	DDX_Control( pDX, IDC_SAVE_INTERVAL, m_SaveIntervalEdit );
	DDX_Control( pDX, IDC_SAVE_MULTIPLE, m_SaveMultipleCombo );
	DDX_Control( pDX, IDC_AGE_CHECK, m_AgeCheck );
	DDX_Control( pDX, IDC_AGE_EDIT, m_AgeEdit );
	DDX_Control( pDX, IDC_AGE_BOX, m_AgeBox );
	DDX_Control( pDX, IDC_AGE_STATIC, m_AgeStatic );
	DDX_Control( pDX, IDC_AGE2_STATIC, m_Age2Static );
	DDX_Control( pDX, IDC_MAIN_BOX, m_MainBox );
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsTab::OnInitDialog()
{
	// Call inherited.
	BASE_CLASS::OnInitDialog();

	SetUIFromOptions();
	return TRUE;
}


// -----------------------------------------------------------------------------
//		GetPrefs
// -----------------------------------------------------------------------------

CStringA
COptionsTab::GetPrefs() const
{
	CStringA	strPrefs;
	strPrefs = m_Options.GetContextPrefs();
	return strPrefs;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COptionsTab::SetOptions(
	const COptions& inOptions )
{
	m_Options.CopyFrom( inOptions );
}

// -----------------------------------------------------------------------------
//		SetPrefs
// -----------------------------------------------------------------------------

void
COptionsTab::SetPrefs(
	CStringA	inPrefs )
{
	m_Options.SetContextPrefs( inPrefs );
}


// -----------------------------------------------------------------------------
//		SetOptionsFromUI
// -----------------------------------------------------------------------------

BOOL
COptionsTab::SetOptionsFromUI()
{
	USES_CONVERSION;

	// Address 1
	DWORD	dwAddress1( 0 );
	m_Address1Edit.GetAddress( dwAddress1 );
	if ( dwAddress1 == 0 ) {
		AfxMessageBox( IDS_IPADDR_ERROR );
		m_Address1Edit.SetFocus();
		return FALSE;
	}
	m_Options.SetAddress1( dwAddress1 );

	// Any Address enabled
	DWORD	dwAddress2( 0 );
	bool	bAnyAddress( false );
	bAnyAddress = (m_AnyAddressRadio.GetCheck() == BST_CHECKED);
	m_Options.SetAnyAddress( bAnyAddress );

	// Address 2
	m_Address2Edit.GetAddress( dwAddress2 );
	if ( !bAnyAddress && (dwAddress2 == 0) ) {
		AfxMessageBox( IDS_IPADDR_ERROR );
		m_Address2Edit.SetFocus();
		return FALSE;
	}
	m_Options.SetAddress2( dwAddress2 );

	// Set Direction
	CString	strDirection;
	m_DirectionCombo.GetWindowText( strDirection );
	m_Options.SetDirection( strDirection );

	bool	bPort( false );
	bPort = (m_PortCheck.GetCheck() == BST_CHECKED);
	m_Options.SetFilterPort( bPort );

	if ( bPort ) {
		CString	strPort;
		m_PortEdit.GetWindowText( strPort );
		UInt16	nPort( _ttoi( strPort ) );
		if ( nPort == 0 ) {
			AfxMessageBox( IDS_PORT_ERROR );
			m_PortEdit.SetFocus();
			m_PortEdit.SetSel( 0x0100 );
			return FALSE;
		}
		m_Options.SetPort( nPort );
	}

	// MAC Header Options
	bool	bMac( false );
	bMac = (m_MACCheck.GetCheck() == BST_CHECKED);
	m_Options.SetInsertMAC( bMac );

	if ( bMac ) {
		bool		bResult;
		CString		strSrcMac;
		CMediaSpec	theSpec;

		m_MACSrcEdit.GetWindowText( strSrcMac );
		bResult = UMediaSpecUtils::StringToSpec( strSrcMac, kMediaSpecType_EthernetAddr, theSpec );
		if ( !bResult ) {
			AfxMessageBox( IDS_SRCMAC_ERROR );
			m_MACSrcEdit.SetFocus();
			return FALSE;
		}
		m_Options.SetSrcMAC( strSrcMac.GetBuffer() );

		CString	strDstMac;
		m_MACDstEdit.GetWindowText( strDstMac );
		bResult = UMediaSpecUtils::StringToSpec( strDstMac, kMediaSpecType_EthernetAddr, theSpec );
		if ( !bResult ) {
			AfxMessageBox( IDS_DSTMAC_ERROR );
			m_MACDstEdit.SetFocus();
			return FALSE;
		}
		m_Options.SetDstMAC( strDstMac.GetBuffer() );

		CString	strPrototype;
		m_MACPrototypeEdit.GetWindowText( strPrototype );
		UInt16	nProtoType( _ttoi( strPrototype ) );
		if ( nProtoType == 0 ) {
			AfxMessageBox( IDS_PROTOCOL_ERROR );
			m_MACPrototypeEdit.SetFocus();
			return FALSE;
		}
		m_Options.SetProtoType( nProtoType );
	}

	// VLAN Options
	bool	bVLAN( false );
	bVLAN = (m_VLANCheck.GetCheck() == BST_CHECKED);
	m_Options.SetInsertVLAN( bVLAN );

	if ( bVLAN ) {
		CString	strVLAN;
		m_VLANEdit.GetWindowText( strVLAN );
		UInt16	nVLAN( _ttoi( strVLAN ) );
		if ( nVLAN == 0 ) {
			AfxMessageBox( IDS_VLANID_ERROR );
			m_VLANEdit.SetFocus();
			return FALSE;
		}
		m_Options.SetVLANID( nVLAN );
	}

	// Save Options
	bool	bSave( false );
	bSave = (m_SaveCheck.GetCheck() == BST_CHECKED);
	m_Options.SetSaveOrphans( bSave );

	if ( bSave ) {
		CString	strSaveDir;
		m_SaveDirEdit.GetWindowText( strSaveDir );
		m_Options.SetFileName( strSaveDir );

		CString	strSaveInterval;
		m_SaveIntervalEdit.GetWindowText( strSaveInterval );
		UInt32	nInterval( _ttoi( strSaveInterval ) );
		if ( nInterval == 0 ) {
			AfxMessageBox( IDS_INTERVAL_ERROR );
			m_SaveIntervalEdit.SetFocus();
			return FALSE;
		}
		m_Options.SetInterval( nInterval );

		CString	strMultiple;
		m_SaveMultipleCombo.GetWindowText( strMultiple );
		m_Options.SetMultiple( strMultiple );
	}

	// Age Options
	bool	bAge;
	bAge = (m_AgeCheck.GetCheck() == BST_CHECKED);
	m_Options.SetAgeLimit( bAge );

	if ( bAge ) {
		CString	strAge;
		m_AgeEdit.GetWindowText( strAge );
		UInt32	nAge( _ttoi( strAge ) );
		if ( nAge == 0 ) {
			AfxMessageBox( IDS_AGE_ERROR );
			m_AgeEdit.SetFocus();
			return FALSE;
		}
		m_Options.SetAge( nAge );
	}

	return TRUE;
}


// -----------------------------------------------------------------------------
//		SetUIFromOptions
// -----------------------------------------------------------------------------

void
COptionsTab::SetUIFromOptions()
{
	USES_CONVERSION;

	m_Address1Edit.SetAddress( m_Options.GetAddress1() );

	if ( m_Options.GetDirection() == kDirection_Either ) {
		m_DirectionCombo.SetCurSel( 0 );
	}
	else if ( m_Options.GetDirection() == kDirection_Sent ) {
		m_DirectionCombo.SetCurSel( 1 );
	}
	else if ( m_Options.GetDirection() == kDirection_Received ) {
		m_DirectionCombo.SetCurSel( 2 );
	}

	m_Address2Edit.SetAddress( m_Options.GetAddress2() );

	if ( m_Options.IsAnyAddress() ) {
		m_AnyAddressRadio.SetCheck( true );
	}
	else {
		m_Address2EnabledRadio.SetCheck( true );
	}

	m_PortCheck.SetCheck( m_Options.IsFilterPort() );
	m_PortEdit.SetWindowText( m_Options.GetPort().Format() );

	m_MACCheck.SetCheck( m_Options.IsInsertMAC() ? TRUE : FALSE );
	m_MACSrcEdit.SetWindowText( m_Options.GetSrcMAC().Format() );
	m_MACDstEdit.SetWindowText( m_Options.GetDstMAC().Format() );
	m_MACPrototypeEdit.SetWindowText( m_Options.GetProtoType().Format() );

	m_VLANCheck.SetCheck( m_Options.IsInsertVLAN() ? TRUE : FALSE );
	m_VLANEdit.SetWindowText( m_Options.GetVLANID().Format() );

	m_SaveCheck.SetCheck( m_Options.IsSaveOrphans() ? TRUE : FALSE );
	m_SaveDirEdit.SetWindowText( m_Options.GetFileName() );
	m_SaveIntervalEdit.SetWindowText( m_Options.GetInterval().Format() );

	if ( m_Options.GetMultiple() == _T( "Seconds" ) ) {
		m_SaveMultipleCombo.SetCurSel( 0 );
	}
	else if ( m_Options.GetMultiple() == _T( "Minutes" ) ) {
		m_SaveMultipleCombo.SetCurSel( 1 );
	}
	else if ( m_Options.GetMultiple() == _T( "Hours" ) ) {
		m_SaveMultipleCombo.SetCurSel( 2 );
	}

	// Get the current state of the options
	BOOL bAgeEnabled = m_Options.IsAgeLimit();

	// Prepare a checked state variable based on the boolean
	int nAgeEnabled = (bAgeEnabled) ? BST_CHECKED : BST_UNCHECKED;

	// Set the checked state of the checkbox
	m_AgeCheck.SetCheck( nAgeEnabled );
	m_AgeEdit.SetWindowText( m_Options.GetAge().Format() );

	// Force an update of the groups
	if ( m_bEditable ) {
		OnBnClickedAddress2anyRadio();
		OnBnClickedAddress2enabledRadio();
		OnBnClickedPortCheck();
		OnBnClickedMacCheck();
		OnBnClickedVlanCheck();
		OnBnClickedSaveCheck();
		OnBnClickedAgeCheck();
	}
	else {
		EnableUI( FALSE );
	}
}


//// ----------------------------------------------------------------------------
////		OnBnClickedCMICheck
//// ----------------------------------------------------------------------------
//
//void
//COptionsTab::OnBnClickedCMICheck()
//{
//	// Get the checked state of the CMI button
//	BOOL	bChecked = m_CMICheck.GetCheck();
//	EnableCMICheck( bChecked );
//	EnableFilterCheck( bChecked );
//}
//
//
// ----------------------------------------------------------------------------
//		EnableCMICheck
// ----------------------------------------------------------------------------

//void
//COptionsTab::EnableCMICheck(
//	BOOL	bChecked )
//{
//	m_FilterStatic.EnableWindow( bChecked );
//	m_MACCheck.EnableWindow( bChecked );
//	m_VLANCheck.EnableWindow( bChecked );
//	m_SaveCheck.EnableWindow( bChecked );
//	m_PortCheck.EnableWindow( bChecked );
//	m_AgeCheck.EnableWindow( bChecked );
//
//	if ( bChecked ) {
//		OnBnClickedMacCheck();
//		OnBnClickedVlanCheck();
//		OnBnClickedSaveCheck();
//		OnBnClickedAgeCheck();
//	}
//	else {
//		EnableFilterCheck( FALSE );
//		EnableMacCheck( FALSE );
//		EnableVlanCheck( FALSE );
//		EnableSaveCheck( FALSE );
//		EnableAgeCheck( FALSE );
//	}
//}


// ----------------------------------------------------------------------------
//		EnableFilterCheck
// ----------------------------------------------------------------------------

//void
//COptionsTab::EnableFilterCheck(
//	BOOL	bChecked )
//{
//	// Enable the windows in this group
//	m_Address2AnyRadio.EnableWindow( bChecked );
//	m_Address2EnabledRadio.EnableWindow( bChecked );
//	m_Address1Static.EnableWindow( bChecked );
//	m_Address2Static.EnableWindow( bChecked );
//	m_Address1Edit.EnableWindow( bChecked );
//	m_Address2Edit.EnableWindow( bChecked );
//	m_DirectionStatic.EnableWindow( bChecked );
//	m_DirectionCombo.EnableWindow( bChecked );
//
//	m_PortStatic.EnableWindow( bChecked );
//	m_PortCheck.EnableWindow( bChecked );
//
//	if ( bChecked ) {
//		OnBnClickedAddress2anyRadio();
//		OnBnClickedAddress2enabledRadio();
//		OnBnClickedPortCheck();
//	}
//	else {
//		EnablePortCheck( FALSE );
//	}
//}


// ----------------------------------------------------------------------------
//		OnBnClickedPortCheck
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedPortCheck()
{
	// Get the checked state of the MACCheck button
	BOOL	bChecked = m_PortCheck.GetCheck();
	EnablePortCheck( bChecked );
}


// ----------------------------------------------------------------------------
//		EnablePortCheck
// ----------------------------------------------------------------------------

void
COptionsTab::EnablePortCheck(
	BOOL	bChecked )
{
	// Enable the windows in this group
	m_PortEdit.EnableWindow( bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedMacCheck
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedMacCheck()
{
	// Get the checked state of the MACCheck button
	BOOL	bChecked = m_MACCheck.GetCheck();
	EnableMacCheck( bChecked );
}


// ----------------------------------------------------------------------------
//		EnableMacCheck
// ----------------------------------------------------------------------------

void
COptionsTab::EnableMacCheck(
	BOOL	bChecked )
{
	// Enable the windows in this group
	//m_MACBox.EnableWindow( bChecked );
	m_MACSrcStatic.EnableWindow( bChecked );
	m_MACSrcEdit.EnableWindow( bChecked );
	m_MACDstStatic.EnableWindow( bChecked );
	m_MACDstEdit.EnableWindow( bChecked );
	m_MACPrototypeStatic.EnableWindow( bChecked );
	m_MACPrototypeEdit.EnableWindow( bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedVlanCheck
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedVlanCheck()
{
	// Get the checked state of the m_VLANCheck button
	BOOL	bChecked = m_VLANCheck.GetCheck();
	EnableVlanCheck( bChecked );
}


// ----------------------------------------------------------------------------
//		EnableVlanCheck
// ----------------------------------------------------------------------------

void
COptionsTab::EnableVlanCheck(
	BOOL	bChecked )
{
	// Enable the windows in this group
	//m_VLANBox.EnableWindow( bChecked );
	m_VLANStatic.EnableWindow( bChecked );
	m_VLANEdit.EnableWindow( bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedSaveCheck
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedSaveCheck()
{
	// Get the checked state of the m_VLANCheck button
	BOOL	bChecked = m_SaveCheck.GetCheck();
	EnableSaveCheck( bChecked );
}


// ----------------------------------------------------------------------------
//		EnableSaveCheck
// ----------------------------------------------------------------------------

void
COptionsTab::EnableSaveCheck(
	BOOL	bChecked )
{
	// Enable the windows in this group
	//m_SaveBox.EnableWindow( bChecked );
	m_SaveDirStatic.EnableWindow( bChecked );
	m_SaveDirEdit.EnableWindow( bChecked );
	m_SaveDirButton.EnableWindow( bChecked );
	m_SaveEveryStatic.EnableWindow( bChecked );
	m_SaveIntervalEdit.EnableWindow( bChecked );
	m_SaveMultipleCombo.EnableWindow( bChecked );
}


// ----------------------------------------------------------------------------
//		EnableUI
// ----------------------------------------------------------------------------

void
COptionsTab::EnableUI(
	BOOL bChecked )
{
	// Filter UI
	m_Address1Static.EnableWindow( bChecked );
	m_Address2Static.EnableWindow( bChecked );
	m_Address1Edit.EnableWindow( bChecked );
	m_Address2Edit.EnableWindow( bChecked );
	m_AnyAddressRadio.EnableWindow( bChecked );
	m_Address2EnabledRadio.EnableWindow( bChecked );
	m_PortStatic.EnableWindow( bChecked );
	m_PortEdit.EnableWindow( bChecked );
	m_PortCheck.EnableWindow( bChecked );
	m_DirectionStatic.EnableWindow( bChecked );
	m_DirectionCombo.EnableWindow( bChecked );

	// MAC UI
	m_MACCheck.EnableWindow( bChecked );
	m_MACSrcEdit.EnableWindow( bChecked );
	m_MACDstEdit.EnableWindow( bChecked );
	m_MACPrototypeEdit.EnableWindow( bChecked );
	m_MACSrcStatic.EnableWindow( bChecked );
	m_MACDstStatic.EnableWindow( bChecked );
	m_MACPrototypeStatic.EnableWindow( bChecked );

 	// VLAN UI
	m_VLANCheck.EnableWindow( bChecked );
	m_VLANEdit.EnableWindow( bChecked );
	m_VLANStatic.EnableWindow( bChecked );

	// Save UI
	m_SaveCheck.EnableWindow( bChecked );
	m_SaveDirEdit.EnableWindow( bChecked );
	m_SaveDirButton.EnableWindow( bChecked );
	m_SaveIntervalEdit.EnableWindow( bChecked );
	m_SaveMultipleCombo.EnableWindow( bChecked );
	m_SaveDirStatic.EnableWindow( bChecked );
	m_SaveDirButton.EnableWindow( bChecked );
	m_SaveEveryStatic.EnableWindow( bChecked );

	// Age UI
	m_AgeCheck.EnableWindow( bChecked );
	m_AgeEdit.EnableWindow( bChecked );
	m_AgeStatic.EnableWindow( bChecked );
	m_Age2Static.EnableWindow( bChecked );
}


// ----------------------------------------------------------------------------
//		ListenToMessage
// ----------------------------------------------------------------------------

void
COptionsTab::ListenToMessage(
	CBLMessage&	ioMessage )
{
	switch ( ioMessage.GetId() ) {
	case kBLM_OptionsUpdate:
		{
			CBLOptionsUpdate&	OU( dynamic_cast<CBLOptionsUpdate&>( ioMessage ) );
			const COptions&		NewOptions( OU.GetMessage() );
			m_Options.CopyFrom( NewOptions );
			SetUIFromOptions();
			UpdateData( FALSE );
		}
		break;
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsTab, BASE_CLASS)
	//ON_BN_CLICKED(IDC_CMI_CHECK, OnBnClickedCMICheck)
	ON_BN_CLICKED(IDC_MAC_CHECK, OnBnClickedMacCheck)
	ON_BN_CLICKED(IDC_PORT_CHECK, OnBnClickedPortCheck)
	ON_BN_CLICKED(IDC_VLAN_CHECK, OnBnClickedVlanCheck)
	ON_BN_CLICKED(IDC_SAVE_CHECK, OnBnClickedSaveCheck)
	ON_BN_CLICKED(IDC_SAVE_BROWSE, OnBnClickedSaveBrowse)
	ON_BN_CLICKED(IDC_AGE_CHECK, OnBnClickedAgeCheck)
	ON_BN_CLICKED(IDC_ADDRESS2ENABLED_RADIO, OnBnClickedAddress2enabledRadio)
	ON_BN_CLICKED(IDC_ADDRESS2ANY_RADIO, OnBnClickedAddress2anyRadio)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedSaveBrowse
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedSaveBrowse()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Show the open file dialog.
	CString		strFilePath;
	strFilePath.GetBuffer( MAX_PATH );
	m_SaveDirEdit.GetWindowText( strFilePath );
	CString		strFilter;
	strFilter.LoadString( IDS_FILE_TYPES );

	CFileDialog	dlg(
		TRUE, NULL, NULL,
		(OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_HIDEREADONLY),
		strFilter, NULL, 0 );

	dlg.m_ofn.lpstrFile = strFilePath.GetBuffer( MAX_PATH );
	int	nResult = dlg.DoModal();
	strFilePath.ReleaseBuffer();
	if ( nResult != IDOK ) return;
	XString	csFilePath( strFilePath );
	m_SaveDirEdit.SetWindowText( strFilePath );
}


// ----------------------------------------------------------------------------
//		OnBnClickedAgeCheck
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedAgeCheck()
{
	// Get the checked state of the m_AgeCheck button
	BOOL	bChecked = m_AgeCheck.GetCheck();
	EnableAgeCheck( bChecked );
}


// ----------------------------------------------------------------------------
//		EnableAgeCheck
// ----------------------------------------------------------------------------

void
COptionsTab::EnableAgeCheck(
	BOOL	bChecked )
{
	// Enable the windows in this group
	m_AgeStatic.EnableWindow( bChecked );
	m_Age2Static.EnableWindow( bChecked );
	m_AgeEdit.EnableWindow( bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedAddress2enabledRadio
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedAddress2enabledRadio()
{
	// Get the checked state of the m_AgeCheck button
	BOOL	bChecked = m_Address2EnabledRadio.GetCheck();
	if ( bChecked ) {
		m_Address2Edit.EnableWindow( true );
	}
}

// ----------------------------------------------------------------------------
//		OnBnClickedAddress2anyRadio
// ----------------------------------------------------------------------------

void
COptionsTab::OnBnClickedAddress2anyRadio()
{
	BOOL	bChecked = m_AnyAddressRadio.GetCheck();
	if ( bChecked ) {
		m_Address2Edit.EnableWindow( false );
	}
}


// ----------------------------------------------------------------------------
//		OnOK
// ----------------------------------------------------------------------------

void COptionsTab::OnOK()
{
	if ( m_pParentDialog ) {
		m_pParentDialog->PostMessage( WM_COMMAND, IDOK );
	}
}
