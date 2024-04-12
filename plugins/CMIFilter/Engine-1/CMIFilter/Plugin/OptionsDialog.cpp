// =============================================================================
//	OptionsDialog.cpp
// =============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#if defined(_WIN32) && defined(PEEK_UI)

#include "OptionsDialog.h"
#include "IpFilterDlg.h"
#include "ItemList.h"
#include "resource.h"
#include "afxcmn.h"
#include "afxdlgs.h"
#include "FileEx.h"


// =============================================================================
//		COptionsDialog
// =============================================================================

COptionsDialog::COptionsDialog(
	const COptions&	inOptions,
	CWnd*			inParent /*= nullptr*/  )
		:	CDialog( COptionsDialog::IDD, inParent )
		,	m_Options( inOptions )
		,	m_bInEditValidation( false )
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

	m_bInEditValidation = true;

	BOOL	bInterceptId;
	CString	strInterceptId;
	BOOL	bInterceptSite;
	CString	strInterceptSite;
	CString	strSrcMAC;
	CString	strDstMAC;
	UInt32	nMACProtocolType;
	UInt32	nVLANID;
	UInt32	nFragmentAge;
	UInt32	nSaveCount;
	int		nSaveUnits;
	CString strFileName;

	if ( !pDX->m_bSaveAndValidate ) {
		bInterceptId = m_Options.HasInterceptId();
		if ( bInterceptId ) {
			CPeekString	strId;
			CPeekString	strSite;
			m_Options.GetInterceptId().Get( strId, strSite );
			strInterceptId = strId;
			strInterceptSite = strSite;

			// An Ericsson Intercept Id may have a Site Name, but not an Id.
			bInterceptId = !strInterceptId.IsEmpty();
			bInterceptSite = !strInterceptSite.IsEmpty();
		}
		strSrcMAC = m_Options.GetSrcEthernet().Format();
		strDstMAC = m_Options.GetDstEthernet().Format();
		nMACProtocolType = m_Options.GetMACProtocolType();
		nVLANID = m_Options.GetVlanId();
		nFragmentAge = m_Options.GetFragmentAge();
		m_Options.GetSaveInterval( nSaveCount, nSaveUnits );
		strFileName = m_Options.GetFileName();

		UpdateIpFilterList();
	}

	DDX_CBIndex( pDX, IDC_PROTOCOL_TYPE, m_Options.m_ProtocolType );
	DDX_Check( pDX, IDC_INTERCEPTID_CHECK, bInterceptId );
	DDX_Text( pDX, IDC_INTERCEPTID_EDIT, strInterceptId );
	DDX_Check( pDX, IDC_INTERCEPTSITE_CHECK, bInterceptSite );
	DDX_Text( pDX, IDC_INTERCEPTSITE_EDIT, strInterceptSite );

	DDX_Check( pDX,  IDC_MAC_CHECK, m_Options.m_bInsertMAC );
	DDX_Text( pDX, IDC_SRCMAC_EDIT, strSrcMAC );
	DDX_Text( pDX, IDC_DSTMAC_EDIT, strDstMAC );
	DDX_Check( pDX, IDC_PROTOCOLTYPE_IP, m_Options.m_bIP );
	DDX_Text( pDX, IDC_PROTOCOLTYPE_EDIT, nMACProtocolType );

	DDX_Check( pDX,  IDC_VLAN_CHECK, m_Options.m_bInsertVLAN );
	DDX_Text( pDX, IDC_VLAN_EDIT, nVLANID );

	DDX_Text( pDX, IDC_AGE_EDIT, nFragmentAge );

	DDX_Check( pDX,  IDC_SAVE_CHECK, m_Options.m_bSaveOrphans );
	DDX_Text( pDX, IDC_SAVEDIR_EDIT, strFileName );
	DDX_Text( pDX, IDC_SAVE_INTERVAL, nSaveCount );
	DDX_CBIndex( pDX, IDC_SAVE_MULTIPLE, nSaveUnits );

	if ( pDX->m_bSaveAndValidate ) {
		if ( !bInterceptId ) {
			strInterceptId.Empty();
		}
		if ( !bInterceptSite ) {
			strInterceptSite.Empty();
		}

		CPeekString	strId(strInterceptId);
		CPeekString	strSite(strInterceptSite);
		m_Options.SetInterceptId( m_Options.m_ProtocolType, strId, strSite );

		m_Options.SetSrcEthernet( CPeekString( strSrcMAC ) );
		m_Options.SetDstEthernet( CPeekString( strDstMAC ) );
		m_Options.SetMACProtocolType( m_Options.IsMACProtocolIp(),
			static_cast<UInt16>( nMACProtocolType ) );
		m_Options.SetVlanId( static_cast<UInt16>( nVLANID ) );
		m_Options.SetFragmentAge( nFragmentAge );
		m_Options.SetSaveInterval( nSaveCount, nSaveUnits );
		if ( strFileName.GetLength() > 0 ) {
			wchar_t* pFileName = strFileName.GetBuffer();
			size_t	nLength( strFileName.GetLength() );
			for ( size_t i = 0; i < nLength; i++ ) {
				pFileName[i] = FilePath::NormalizeSeparator( pFileName[i] );
			}
			strFileName.ReleaseBuffer();
		}
		m_Options.SetFileName( CPeekString( strFileName ) );
	}

	m_bInEditValidation = false;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsDialog::OnInitDialog()
{
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
		UInt32	nUnit = (rcList.Width() - nScrollWidth) / 23;
		UInt32	nHalfUnit = nUnit / 2;

		pList->InsertColumn( 0, L"Address 1" );
		pList->SetColumnWidth( 0, ((nUnit * 7) + nHalfUnit) );
		pList->InsertColumn( 1, L"Direction" );
		pList->SetColumnWidth( 1, ((nUnit * 6) + nHalfUnit) );
		pList->InsertColumn( 2, L"Address 2" );
		pList->SetColumnWidth( 2, ((nUnit * 7) + nHalfUnit) );
		pList->InsertColumn( 3, L"Port" );
		pList->SetColumnWidth( 3, (nUnit * 3) );
	}

	// Call inherited.
	CDialog::OnInitDialog();

	CRichEditCtrl*	pProtocolType = static_cast<CRichEditCtrl*>( GetDlgItem( IDC_PROTOCOLTYPE_EDIT ) );
	if ( pProtocolType ) {
		pProtocolType->SetEventMask( pProtocolType->GetEventMask() | ENM_CHANGE );
	}
	CRichEditCtrl*	pHex = static_cast<CRichEditCtrl*>( GetDlgItem( IDC_HEX_EDIT ) );
	if ( pHex ) {
		pHex->SetEventMask( pHex->GetEventMask() | ENM_CHANGE );
	}

	OnCbnSelChangeProtocolType();
	OnLbnSelChangeIpFilterList();
	OnBnClickedMacCheck();
	OnEnChangeProtocolTypeEdit();
	OnBnClickedVlanCheck();
	OnBnClickedSaveCheck();

	return TRUE;
}


// -----------------------------------------------------------------------------
//		OnOK
// -----------------------------------------------------------------------------

void
COptionsDialog::OnOK()
{
	bool bIsError( false );

	if ( IsDlgButtonChecked( IDC_SAVE_CHECK ) ) {

		CString strFileName;
		GetDlgItemText( IDC_SAVEDIR_EDIT, strFileName );

		CPeekString theFileName( strFileName );
		CPeekString	strErrorText;

		if ( strFileName.GetLength() == 0 ) {
			bIsError = true;
			strErrorText = L"You must enter a destination file path for orphan packet saving.";	
		}
		else {
			if ( !CFileEx::IsLegalFilePath( theFileName ) ) {
				bIsError = true;
				CPeekOutString strOutMessage;
				strOutMessage << L"Illegal file path: \"" << theFileName << L"\""; 
				strErrorText = strOutMessage;
			}
		}

		if ( bIsError ) {
			MessageBox( strErrorText );
			return;
		}
	}

	if ( !bIsError ) {
		CDialog::OnOK();
	}
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


// -----------------------------------------------------------------------------
//		ValidateNumeric
// -----------------------------------------------------------------------------

void
COptionsDialog::ValidateNumeric(
	int		nDlgNumber,
	UInt32	nMaxValue,
	UInt32&	theValue,
	bool	bHexValue )
{
	if ( m_bInEditValidation ) return;
	
	m_bInEditValidation = true;

	if ( nMaxValue >= kMaxUInt32 ) {
		nMaxValue = kMaxUInt32 - 1;
	}
	if ( theValue > nMaxValue ) {
		theValue = nMaxValue;
	}

	CString	strValue;
	GetDlgItemText( nDlgNumber, strValue );

	int nOrigLen = strValue.GetLength();

	if ( strValue == L"" ) {
		theValue = 0;
		m_bInEditValidation = false;
		return;
	}
	if ( _wtoi( (LPCTSTR)strValue ) == 0 ) {
		int i( 0 );
		for ( ; i < nOrigLen; i++ ) {
			if ( strValue[i] != '0' ) break;
		}
		if ( i >= nOrigLen ) {
			theValue = 0;
			m_bInEditValidation = false;
			return;
		}
	}

	LPCTSTR ptr = (LPCTSTR) strValue; 
	LPTSTR endptr; 

	UInt32 nNewValue( 0 );

	int base = bHexValue ? 16 : 10;

	nNewValue = wcstol( ptr, &endptr, base );

	SInt32	nGoodLen( static_cast<SInt32>( endptr - ptr ) );
	bool	bGoodLength( nOrigLen == nGoodLen );
	bool	bAllGood( true );

	CString strNew;

	if ( bGoodLength ) {
		if ( nNewValue <= nMaxValue ) {
			theValue = nNewValue;
			if ( !bHexValue) {
				m_bInEditValidation = false;
				return;
			}
			strNew = strValue;
		}
		else {
			bAllGood = false;
		}
	}
	else {
		bAllGood = false;
	}

	if ( nNewValue > 0 ) {
		if ( bHexValue){
			if ( !bAllGood ) strNew.Format( L"%X", theValue );
			strNew.MakeUpper();
		}
		else {
			ASSERT( !bAllGood );
			strNew.Format( L"%d", theValue );
		}
	}
	else {
		strNew = L"";
	}

	CEdit* pEditBox = (CEdit*)GetDlgItem( nDlgNumber );
	pEditBox->SetWindowText( strNew );
	pEditBox->SetSel( nGoodLen, nGoodLen );

	m_bInEditValidation = false;
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
	ON_BN_CLICKED(IDC_PROTOCOLTYPE_IP, OnBnClickedIpCheck)
	ON_BN_CLICKED(IDC_VLAN_CHECK, OnBnClickedVlanCheck)
	ON_BN_CLICKED(IDC_SAVE_CHECK, OnBnClickedSaveCheck)
	ON_BN_CLICKED(IDC_SAVE_BROWSE, OnBnClickedSaveBrowse)
	ON_CBN_SELCHANGE(IDC_PROTOCOL_TYPE, OnCbnSelChangeProtocolType)
	ON_EN_CHANGE(IDC_PROTOCOLTYPE_EDIT, OnEnChangeProtocolTypeEdit)
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
	case COptions::kType_PCLI:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, false );
		break;

	case COptions::kType_SecurityId:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, bChecked );
		break;

	case COptions::kType_Ericsson:
		CItemList::EnableItem( this, IDC_INTERCEPTID_EDIT, bChecked );
		OnBnClickedInterceptSiteCheck();
		break;

	case COptions::kType_Arris:
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
		IDC_PROTOCOLTYPE_EDIT,
		IDC_HEX_STATIC,
		IDC_HEX_EDIT,
		0
	};
	CItemList ids( this, ayIds );

	// Get the checked state of the MACCheck button
	bool	bMacChecked = (IsDlgButtonChecked( IDC_MAC_CHECK ) == BST_CHECKED);

	// Get the checked state of the IP Check button
	bool	bChecked = (IsDlgButtonChecked( IDC_PROTOCOLTYPE_IP ) == BST_CHECKED);

	ids.EnableItems( bMacChecked && !bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedMacCheck
// ----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedMacCheck()
{
	UINT	ayIds[] = {
		IDC_SRCMAC_STATIC,
		IDC_SRCMAC_EDIT,
		IDC_DSTMAC_STATIC,
		IDC_DSTMAC_EDIT,
		IDC_PROTOCOLTYPE_IP,
		IDC_PROTOCOLTYPE_STATIC,
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
	CString		strFilePath;
	GetDlgItemText( IDC_SAVEDIR_EDIT, strFilePath );
	CString		strFilter;
	strFilter.LoadString( IDS_FILE_TYPES );

	CFileDialog	dlg(
		TRUE, nullptr, nullptr,
		(OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_HIDEREADONLY),
		strFilter, nullptr, 0 );

	dlg.m_ofn.lpstrFile = strFilePath.GetBuffer( MAX_PATH );
	INT_PTR	nResult = dlg.DoModal();
	strFilePath.ReleaseBuffer();
	if ( nResult != IDOK ) return;
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
		IDC_INTERCEPTID_NOTE,
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
	if ( m_bInEditValidation ) return;

	UInt32 theValue = m_Options.GetProtocolType();
	ValidateNumeric( IDC_HEX_EDIT, kMaxUInt16, theValue, true );
	m_Options.SetProtocolType( static_cast<UInt16>( theValue ) );

	// Set ProtocolType integer edit box to have the same value
	m_bInEditValidation = true;
	SetDlgItemInt( IDC_PROTOCOLTYPE_EDIT, theValue );
	m_bInEditValidation = false;
}


// ----------------------------------------------------------------------------
//		OnEnChangeProtocolTypeEdit
// ----------------------------------------------------------------------------

void
COptionsDialog::OnEnChangeProtocolTypeEdit()
{
	if ( m_bInEditValidation ) return;

	UInt32 theValue = m_Options.GetProtocolType();
	ValidateNumeric( IDC_PROTOCOLTYPE_EDIT, kMaxUInt16, theValue );
	m_Options.SetProtocolType( static_cast<UInt16>( theValue ) );

	// Set ProtocolType hex edit box to have the same value
	m_bInEditValidation = true;
	CString	strHexValue;
	strHexValue.Format( _T( "%04X" ), theValue );
	SetDlgItemText( IDC_HEX_EDIT, strHexValue );
	m_bInEditValidation = false;
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
	NMHDR*		/*pNMHDR*/,
	LRESULT*	pResult )
{
// 	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

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

#endif // _WIN32 && PEEK_UI
