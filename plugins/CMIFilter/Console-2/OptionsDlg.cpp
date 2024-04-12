// ============================================================================
// OptionsDlg.cpp:
//      implementation of the COptionsDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "CMIFilter.h"
#include "MediaSpec.h"
#include "MediaSpecUtil.h"
#include "OptionsDlg.h"
#include "IpFilterDlg.h"


// ============================================================================
//		COptionsDialog
// ============================================================================

IMPLEMENT_DYNAMIC(COptionsDialog, CDialog)

COptionsDialog::COptionsDialog(
	CWnd*	pParent /*=NULL*/ )
	:	CDialog( COptionsDialog::IDD, pParent )
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
COptionsDialog::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );

	BOOL	bInterceptId;
	CString	strInterceptId;
	BOOL	bInterceptSite;
	CString	strInterceptSite;
	CString	strSrcMAC;
	CString	strDstMAC;
	UInt32	nProtoType;
	UInt32	nVLANID;
	UInt32	nFragmentAge;
	UInt32	nSaveCount;
	int		nSaveUnits;

	if ( !pDX->m_bSaveAndValidate ) {
		bInterceptId = m_Options.HasInterceptId();
		if ( bInterceptId ) {
			m_Options.GetInterceptId().Get( strInterceptId, strInterceptSite );
			bInterceptId = !strInterceptId.IsEmpty();
			bInterceptSite = !strInterceptSite.IsEmpty();
		}
		strSrcMAC = m_Options.GetSrcMAC().Format();
		strDstMAC = m_Options.GetDstMAC().Format();
		nProtoType = m_Options.GetMACProtocolType();
		nVLANID = m_Options.GetVLANID();
		nFragmentAge = m_Options.GetFragmentAge();
		m_Options.GetSaveInterval( nSaveCount, nSaveUnits );

		UpdateIpFilterList();
	}

	DDX_CBIndex( pDX, IDC_PROTOCOL_TYPE, m_Options.m_ProtocolType );
	DDX_Check( pDX, IDC_INTERCEPTID_CHECK, bInterceptId );
	DDX_Text( pDX, IDC_INTERCEPTID_EDIT, strInterceptId );
	DDX_Check( pDX, IDC_INTERCEPTSITE_CHECK, bInterceptSite );
	DDX_Text( pDX, IDC_INTERCEPTSITE_EDIT, strInterceptSite );

	DDX_Check( pDX, IDC_MAC_CHECK, m_Options.m_bInsertMAC );
	DDX_Text( pDX, IDC_SRCMAC_EDIT, strSrcMAC );
	DDX_Text( pDX, IDC_DSTMAC_EDIT, strDstMAC );
	DDX_Check( pDX, IDC_IP_CHECK, m_Options.m_bIP );
	DDX_Text( pDX, IDC_PROTOTYPE_EDIT, nProtoType );

	DDX_Check( pDX, IDC_VLAN_CHECK, m_Options.m_bInsertVLAN );
	DDX_Text( pDX, IDC_VLAN_EDIT, nVLANID );

	DDX_Text( pDX, IDC_AGE_EDIT, nFragmentAge );

	DDX_Check( pDX, IDC_SAVE_CHECK, m_Options.m_bSaveOrphans );
	DDX_Text( pDX, IDC_SAVEDIR_EDIT, m_Options.m_strFileName );
	DDX_Text( pDX, IDC_SAVE_INTERVAL, nSaveCount );
	DDX_CBIndex( pDX, IDC_SAVE_MULTIPLE, nSaveUnits );

	if ( pDX->m_bSaveAndValidate ) {
		if ( !bInterceptId ) {
			strInterceptId.Empty();
		}
		if ( !bInterceptSite ) {
			strInterceptSite.Empty();
		}
		m_Options.SetInterceptId( m_Options.GetProtocolType(), strInterceptId, strInterceptSite );
		m_Options.SetSrcMAC( strSrcMAC );
		m_Options.SetDstMAC( strDstMAC );
		m_Options.SetMACProtocolType( m_Options.IsMACProtocolIp(), static_cast<UInt16>( nProtoType ) );
		m_Options.SetVLANID( static_cast<UInt16>( nVLANID ) );
		m_Options.SetFragmentAge( nFragmentAge );
		m_Options.SetSaveInterval( nSaveCount, nSaveUnits );
	}
}


// -----------------------------------------------------------------------------
//		GetPrefs
// -----------------------------------------------------------------------------

CStringA
COptionsDialog::GetPrefs() const
{
	CStringA	strPrefs;
	strPrefs = m_Options.GetContextPrefs();
	return strPrefs;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsDialog::OnInitDialog()
{
	m_strNotes[COptions::kType_CMI];
	m_strNotes[COptions::kType_PCLI].LoadString( IDS_NOTE_PCLI );
	m_strNotes[COptions::kType_SecurityId].LoadString( IDS_NOTE_SECURITYID );
	m_strNotes[COptions::kType_Ericsson].LoadString( IDS_NOTE_ERICSSON );
	m_strNotes[COptions::kType_Arris].LoadString( IDS_NOTE_ARRIS );
	m_strNotes[COptions::kType_Juniper].LoadString( IDS_NOTE_JUNIPER );

	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( pList ) {
		pList->SetExtendedStyle( pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		CRect	rcList;
		pList->GetClientRect( &rcList );

		int	nScrollWidth = GetSystemMetrics( SM_CXVSCROLL );
		if ( nScrollWidth < 10 ) {
			nScrollWidth = 16;
		}
		UInt32	nUnit = (rcList.Width() - nScrollWidth) / 16;

		pList->InsertColumn( 0, L"Address 1" );
		pList->SetColumnWidth( 0, (nUnit * 5) );
		pList->InsertColumn( 1, L"Direction" );
		pList->SetColumnWidth( 1, (nUnit * 4) );
		pList->InsertColumn( 2, L"Address 2" );
		pList->SetColumnWidth( 2, (nUnit * 5) );
		pList->InsertColumn( 3, L"Port" );
		pList->SetColumnWidth( 3, (nUnit * 2) );
	}

	// Call inherited.
	CDialog::OnInitDialog();

	CRichEditCtrl*	pProtoType = static_cast<CRichEditCtrl*>( GetDlgItem( IDC_PROTOTYPE_EDIT ) );
	if ( pProtoType ) {
		pProtoType->SetEventMask( pProtoType->GetEventMask() | ENM_CHANGE );
	}
	CRichEditCtrl*	pHex = static_cast<CRichEditCtrl*>( GetDlgItem( IDC_HEX_EDIT ) );
	if ( pHex ) {
		pHex->SetEventMask( pHex->GetEventMask() | ENM_CHANGE );
	}

	OnCbnSelChangeProtocolType();
	OnLbnSelChangeIpFilterList();
	OnBnClickedMacCheck();
	OnEnChangePrototypeEdit();
	OnBnClickedVlanCheck();
	OnBnClickedSaveCheck();

	return TRUE;
}


// -----------------------------------------------------------------------------
//		SetPrefs
// -----------------------------------------------------------------------------

void
COptionsDialog::SetPrefs(
	CStringA	inPrefs )
{
	m_Options.SetContextPrefs( inPrefs );
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COptionsDialog::SetOptions(
	const COptions& inOptions )
{
	m_Options.Copy( inOptions );
}


// -----------------------------------------------------------------------------
//		UpdateIpFilterList
// -----------------------------------------------------------------------------

void
COptionsDialog::UpdateIpFilterList()
{
	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( !pList ) return;

	pList->DeleteAllItems();

	UInt32									nIndex = 0;
	const CIpFilterList&					theList = m_Options.GetIpFilterList();
	std::vector<CIpFilter>::const_iterator	itr;
	for ( itr = theList.begin(); itr != theList.end(); ++itr, ++nIndex ) {
		pList->InsertItem( nIndex, _T( "" ) );
		pList->SetItemText( nIndex, 0, itr->FormatAddress1() );
		pList->SetItemText( nIndex, 1, itr->FormatDirection() );
		pList->SetItemText( nIndex, 2, itr->FormatAddress2() );
		pList->SetItemText( nIndex, 3, itr->FormatPort() );
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	ON_BN_CLICKED(IDC_ADD_IPFILTER, OnBnClickedAddIpFilter)
	ON_BN_CLICKED(IDC_DELETE_IPFILTER, OnBnClickedDeleteIpFilter)
	ON_BN_CLICKED(IDC_EDIT_IPFILTER, OnBnClickedEditIpFilter)
	ON_BN_CLICKED(IDC_INTERCEPTID_CHECK, OnBnClickedInterceptIdCheck)
	ON_BN_CLICKED(IDC_INTERCEPTSITE_CHECK, OnBnClickedInterceptSiteCheck)
	ON_BN_CLICKED(IDC_MAC_CHECK, OnBnClickedMacCheck)
	ON_BN_CLICKED(IDC_VLAN_CHECK, OnBnClickedVlanCheck)
	ON_BN_CLICKED(IDC_SAVE_CHECK, OnBnClickedSaveCheck)
	ON_BN_CLICKED(IDC_SAVE_BROWSE, OnBnClickedSaveBrowse)
	ON_BN_CLICKED(IDC_IP_CHECK, OnBnClickedIpCheck)
	ON_CBN_SELCHANGE(IDC_PROTOCOL_TYPE, OnCbnSelChangeProtocolType)
	ON_EN_CHANGE(IDC_PROTOTYPE_EDIT, OnEnChangePrototypeEdit)
	ON_EN_CHANGE(IDC_HEX_EDIT, OnEnChangeHexEdit)
	ON_LBN_SELCHANGE(IDC_IPFILTER_LIST, OnLbnSelChangeIpFilterList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IPFILTER_LIST, OnLvnItemChangedIpFilterList)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedAddIpFilter
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedAddIpFilter()
{
	CIpFilterDialog	dlgIpFilter;

	if ( dlgIpFilter.DoModal() ) {
		const CIpFilter&	theIpFilter( dlgIpFilter.GetIpFilter() );
		m_Options.AddIpFilter( theIpFilter );
		UpdateIpFilterList();
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedDeleteIpFilter
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedDeleteIpFilter()
{
	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( !pList ) return;

	POSITION	pos( pList->GetFirstSelectedItemPosition() );
	if ( pos ) {
		int	nItem = pList->GetNextSelectedItem( pos );

		m_Options.RemoveIpFilter( nItem );
		UpdateIpFilterList();
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedEditIpFilter
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedEditIpFilter()
{
	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( !pList ) return;

	POSITION	pos( pList->GetFirstSelectedItemPosition() );
	if ( pos ) {
		int	nItem = pList->GetNextSelectedItem( pos );
		if ( nItem < 0 ) return;

		const CIpFilterList&	IpFilterList = m_Options.GetIpFilterList();
		if ( nItem >= static_cast<int>( IpFilterList.size() ) ) return;

		CIpFilterDialog	dlgIpFilter;
		dlgIpFilter.SetIpFilter( IpFilterList.at( nItem ) );

		if ( dlgIpFilter.DoModal() ) {
			const CIpFilter&	theIpFilter( dlgIpFilter.GetIpFilter() );
			m_Options.UpdateIpFilter( nItem, theIpFilter );
			UpdateIpFilterList();
		}
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedInterceptIdCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedInterceptIdCheck()
{
	CComboBox*	pType = reinterpret_cast<CComboBox*>( GetDlgItem( IDC_PROTOCOL_TYPE ) );
	if ( !pType ) return;

	int	nCurrentSelection( pType->GetCurSel() );

	bool	bChecked = (IsDlgButtonChecked( IDC_INTERCEPTID_CHECK ) == BST_CHECKED);
	switch ( nCurrentSelection ) {
	case COptions::kType_CMI:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, false );
		break;

	case COptions::kType_PCLI:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, bChecked );
		break;

	case COptions::kType_SecurityId:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, bChecked );
		OnBnClickedInterceptSiteCheck();
		break;

	case COptions::kType_Arris:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, bChecked );
		break;

	case COptions::kType_Juniper:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, bChecked );
		break;
	}
}


// ----------------------------------------------------------------------------
//		OnBnClickedInterceptSiteCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedInterceptSiteCheck()
{
	bool	bChecked = (IsDlgButtonChecked( IDC_INTERCEPTSITE_CHECK ) == BST_CHECKED);
	CItemList::EnableItem( this, IDC_INTERCEPTSITE_EDIT, bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedIpCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedIpCheck()
{
	static UINT ayIds[] = {
		IDC_PROTOTYPE_EDIT,
		IDC_HEX_STATIC,
		IDC_HEX_EDIT,
		0
	};
	CItemList ids( this, ayIds );

	// Get the checked state of the MACCheck button
	bool	bMacChecked = (IsDlgButtonChecked( IDC_MAC_CHECK ) == BST_CHECKED);

	// Get the checked state of the IP Check button
	bool	bChecked = (IsDlgButtonChecked( IDC_IP_CHECK ) == BST_CHECKED);

	ids.EnableItems( bMacChecked && !bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedMacCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedMacCheck()
{
	static UINT ayIds[] = {
		IDC_SRCMAC_STATIC,
		IDC_SRCMAC_EDIT,
		IDC_DSTMAC_STATIC,
		IDC_DSTMAC_EDIT,
		IDC_PROTOTYPE_STATIC,
		IDC_IP_CHECK,
		0
	};
	CItemList ids( this, ayIds );

	// Get the checked state of the MACCheck button
	bool	bChecked = (IsDlgButtonChecked( IDC_MAC_CHECK ) == BST_CHECKED);

	ids.EnableItems( bChecked );
	OnBnClickedIpCheck();
}


// ----------------------------------------------------------------------------
//		OnBnClickedSaveBrowse
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedSaveBrowse()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Show the open file dialog.
	CString	strFilePath;
	GetDlgItemText( IDC_SAVEDIR_EDIT, strFilePath );

	CString	strFilter;
	strFilter.LoadString( IDS_FILE_TYPES );

	CFileDialog	dlg(
		TRUE, NULL, NULL,
		(OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_HIDEREADONLY),
		strFilter, NULL, 0 );

	dlg.m_ofn.lpstrFile = strFilePath.GetBuffer( MAX_PATH );
	INT_PTR	nResult = dlg.DoModal();
	strFilePath.ReleaseBuffer();
	if ( nResult != IDOK ) return;
	XString	csFilePath( strFilePath );
	SetDlgItemText( IDC_SAVEDIR_EDIT, strFilePath );
}


// ----------------------------------------------------------------------------
//		OnBnClickedSaveCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedSaveCheck()
{
	static UINT ayIds[] = {
		IDC_SAVEDIR_STATIC,
		IDC_SAVEDIR_EDIT,
		IDC_SAVE_BROWSE,
		IDC_SAVEFILE_STATIC,
		IDC_SAVE_INTERVAL,
		IDC_SAVE_MULTIPLE,
		0
	};
	CItemList	ids( this, ayIds );

	// Get the checked state of the Save Check button
	bool	bChecked = (IsDlgButtonChecked( IDC_SAVE_CHECK ) == BST_CHECKED);
	ids.EnableItems( bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedVlanCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedVlanCheck()
{
	static UINT	ayIds[] = {
		IDC_VLAN_STATIC,
		IDC_VLAN_EDIT,
		0
	};
	CItemList	ids( this, ayIds );

	// Get the checked state of the m_VLANCheck button
	bool	bChecked = (IsDlgButtonChecked( IDC_VLAN_CHECK ) == BST_CHECKED);
	ids.EnableItems( bChecked );
}


// ----------------------------------------------------------------------------
//		OnCbnSelChangeProtocolType
// ----------------------------------------------------------------------------

void
COptionsDialog::OnCbnSelChangeProtocolType()
{
	static UINT	ayIds[] = {
		IDC_SECURITYID_BOX,
		IDC_INTERCEPTID_CHECK,
		IDC_INTERCEPTID_EDIT,
		IDC_INTERCEPTID_NOTE,
		IDC_INTERCEPTSITE_CHECK,
		IDC_INTERCEPTSITE_EDIT,
		0
	};
	CItemList	ids( this, ayIds );

	static UINT	ayInterceptIds[] = {
		IDC_SECURITYID_BOX,
		IDC_INTERCEPTID_CHECK,
		IDC_INTERCEPTID_EDIT,
		IDC_INTERCEPTID_NOTE,
		0
	};
	CItemList	interceptIds( this, ayInterceptIds );

	static UINT	ayEricssonIds[] = {
		IDC_SECURITYID_BOX,
		IDC_INTERCEPTID_CHECK,
		IDC_INTERCEPTID_EDIT,
		IDC_INTERCEPTSITE_CHECK,
		IDC_INTERCEPTSITE_EDIT,
		0
	};
	CItemList	ericssonIds( this, ayEricssonIds );

	static UINT	ayArrisIds[] = {
		IDC_SECURITYID_BOX,
		IDC_INTERCEPTID_CHECK,
		IDC_INTERCEPTID_EDIT,
		IDC_INTERCEPTID_NOTE,
		0
	};
	CItemList	arrisIds( this, ayArrisIds );

	static UINT	ayJuniperIds[] = {
		IDC_SECURITYID_BOX,
		IDC_INTERCEPTID_CHECK,
		IDC_INTERCEPTID_EDIT,
		IDC_INTERCEPTID_NOTE,
		0
	};
	CItemList	juniperIds( this, ayJuniperIds );

	CComboBox*	pType = reinterpret_cast<CComboBox*>( GetDlgItem( IDC_PROTOCOL_TYPE ) );
	if ( !pType ) return;

	int nType = pType->GetCurSel();
	switch ( nType ) {
	case COptions::kType_CMI:
		ids.ShowItems( false );
		ids.EnableItems( false );
		break;

	case COptions::kType_PCLI:
		ids.ShowItems( false );
		ids.EnableItems( false );
		break;

	case COptions::kType_SecurityId:
		ids.ShowItems( false );
		interceptIds.ShowItems( true );
		interceptIds.EnableItems( true );
		break;

	case COptions::kType_Ericsson:
		ids.ShowItems( false );
		ericssonIds.ShowItems( true );
		ericssonIds.EnableItems( true );
		break;

	case COptions::kType_Arris:
		ids.ShowItems( false );
		arrisIds.ShowItems( true );
		arrisIds.EnableItems( true );
		break;

	case COptions::kType_Juniper:
		ids.ShowItems( false );
		juniperIds.ShowItems( true );
		juniperIds.EnableItems( true );
		break;

	default:
		nType = 0;
	}

	SetDlgItemText( IDC_INTERCEPTID_NOTE, m_strNotes[nType] );

	OnBnClickedInterceptIdCheck();
	OnBnClickedInterceptSiteCheck();
}


// ----------------------------------------------------------------------------
//		OnEnChangeHexEdit
// ----------------------------------------------------------------------------

void
COptionsDialog::OnEnChangeHexEdit()
{
	CString	strHexValue;
	GetDlgItemText( IDC_HEX_EDIT, strHexValue );
	UInt32	nValue = _tcstol( strHexValue, NULL, 16 );

	// Check the new vs the current value to avoid an infinite loop.
	UINT	nCurValue = GetDlgItemInt( IDC_PROTOTYPE_EDIT );
	if ( nValue != nCurValue ) {
		SetDlgItemInt( IDC_PROTOTYPE_EDIT, nValue );
	}
}


// ----------------------------------------------------------------------------
//		OnEnChangePrototypeEdit
// ----------------------------------------------------------------------------

void
COptionsDialog::OnEnChangePrototypeEdit()
{
	UINT	nValue = GetDlgItemInt( IDC_PROTOTYPE_EDIT );
	CString	strHexValue;

	// Check the new vs the current value to avoid an infinite loop.
	GetDlgItemText( IDC_HEX_EDIT, strHexValue );
	UInt32	nCurValue = _tcstol( strHexValue, NULL, 16 );
	if ( nValue != nCurValue ) {
		CString	strHexValue;
		strHexValue.Format( _T( "%04X" ), nValue );
		SetDlgItemText( IDC_HEX_EDIT, strHexValue );
	}
}


// ----------------------------------------------------------------------------
//		OnLbnSelChangeIpFilterList
// ----------------------------------------------------------------------------

void COptionsDialog::OnLbnSelChangeIpFilterList()
{
	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( !pList ) return;

	CWnd*	pModify = GetDlgItem( IDC_EDIT_IPFILTER );
	CWnd*	pDelete = GetDlgItem( IDC_DELETE_IPFILTER );

	int	nCount = pList->GetSelectedCount();
	if ( pModify ) {
		pModify->EnableWindow( (nCount > 0) ? TRUE : FALSE );
	}
	if ( pDelete ) {
		pDelete->EnableWindow( (nCount > 0) ? TRUE : FALSE );
	}
}


// ----------------------------------------------------------------------------
//		OnLvnItemChangedIpFilterList
// ----------------------------------------------------------------------------

void COptionsDialog::OnLvnItemChangedIpFilterList(
	NMHDR*		pNMHDR,
	LRESULT*	pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CListCtrl*	pList = reinterpret_cast<CListCtrl*>( GetDlgItem( IDC_IPFILTER_LIST ) );
	if ( !pList ) return;

	CWnd*	pModify = GetDlgItem( IDC_EDIT_IPFILTER );
	CWnd*	pDelete = GetDlgItem( IDC_DELETE_IPFILTER );

	int	nCount = pList->GetSelectedCount();
	if ( pModify ) {
		pModify->EnableWindow( (nCount > 0) ? TRUE : FALSE );
	}
	if ( pDelete ) {
		pDelete->EnableWindow( (nCount > 0) ? TRUE : FALSE );
	}

	*pResult = 0;
}
