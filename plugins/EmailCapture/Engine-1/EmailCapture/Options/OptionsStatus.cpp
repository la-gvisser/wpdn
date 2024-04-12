// ============================================================================
//	OptionStatus.cpp
//		implementation of the COptionStatus class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "OptionsStatus.h"
#include "GlobalTags.h"
#include <afxwin.h>
#include <afxcmn.h>


// ============================================================================
//		CCasesOpts
// ============================================================================

IMPLEMENT_DYNAMIC(COptionsStatus, CDialog)

COptionsStatus::COptionsStatus(
	const COptions&	inOptions )
	:	CDialog()
	,	m_Options( inOptions )
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
COptionsStatus::DoDataExchange(
	CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	if ( !pDX->m_bSaveAndValidate ) return;

	CListBox*	pCaseList( static_cast<CListBox*>( GetDlgItem( IDC_CASE_LIST ) ) );
	if ( pCaseList && pCaseList->GetSafeHwnd() ) {
		int	nIndex( pCaseList->GetCurSel() );
		if ( nIndex >= 0 ) {
			SetStatus( nIndex );
		}
	}
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
COptionsStatus::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	Reset();
	SetCaseList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------
//		Reset
// ----------------------------------------------------------------------------

void
COptionsStatus::Reset()
{
	CListBox*	pCaseList = static_cast<CListBox*>( GetDlgItem( IDC_CASE_LIST ) );
	if ( pCaseList && pCaseList->GetSafeHwnd() ) pCaseList->ResetContent();

	int	ayClearIds[] = {
		IDC_PROTOCOL_STATUS,
		IDC_PACKETS_STATUS,
		IDC_MESSAGES_STATUS,
		IDC_SUMMARY_STATUS,
		IDC_SAVE_STATUS,
		IDC_FILENAME_PREFIX,
		IDC_OUTPUT_STATUS,
		IDC_NAME_STATUS,
		IDC_TYPE_STATUS,
		IDC_START_STATUS,
		IDC_STOP_STATUS,
		IDC_FACILITY_STATUS

	};
	for ( size_t i( 0 ); i < COUNTOF( ayClearIds ); i++ ) {
		SetDlgItemText( ayClearIds[i], Tags::kxEmpty.GetW() );
	}

	int	ayHideIds[] = {
		IDC_CAPTURE_ALL_STATUS,
		IDC_FACILITY_LABEL,
		IDC_FACILITY_STATUS,
		IDC_ADDRESSES_STATUS
	};
	for ( size_t i( 0 ); i < COUNTOF( ayHideIds ); i++ ) {
		CWnd*	pItem( GetDlgItem( ayHideIds[i] ) );
		if ( pItem && pItem->GetSafeHwnd() ) pItem->ShowWindow( SW_HIDE );
	}
}


// ----------------------------------------------------------------------------
//		SetCaseList
// ----------------------------------------------------------------------------

void
COptionsStatus::SetCaseList()
{
	const COptions::COptionsCases&	ayCases( m_Options.GetCases() );
	size_t	nCount( ayCases.GetCount() );

	CListBox*	pCaseList = static_cast<CListBox*>( GetDlgItem( IDC_CASE_LIST ) );
	if ( pCaseList && pCaseList->GetSafeHwnd() ) {
		for ( size_t i( 0 ); i < nCount; i++ ) {
			const CCase&	cCase( ayCases.GetAt( i ) );
			const wchar_t*	strName( cCase.GetName().GetW() );
			pCaseList->AddString( strName );
		}
		pCaseList->SetCurSel( 0 );
	}
}


// ----------------------------------------------------------------------------
//		Refresh
// ----------------------------------------------------------------------------

void
COptionsStatus::Refresh(
	const COptions&	inOptions )
{
	m_Options = inOptions;

	if ( GetSafeHwnd() ) {
		Reset();
		SetCaseList();
		UpdateData();
	}
}


// ----------------------------------------------------------------------------
//		SetStatus
// ----------------------------------------------------------------------------

void
COptionsStatus::SetStatus(
	size_t	inIndex )
{
	const COptions::COptionsCases&	ayCases( m_Options.GetCases() );
	if ( inIndex >= ayCases.GetCount() ) return;

	const CCase&				cCase( ayCases.GetAt( inIndex ) );

	const CTargetAddressList&	talTargets( cCase.GetAddressList() );
	const CCaseSettings&		csSettings( cCase.GetSettings() );
	// const CReportMode&		rmMode( csSettings.GetReportMode() );

	SetDlgItemText( IDC_TYPE_STATUS, csSettings.FormatMode() );

	SetDlgItemText( IDC_PROTOCOL_STATUS, csSettings.FormatProtocol() );
	SetDlgItemText( IDC_PACKETS_STATUS, FormatEnabled( csSettings.IsAcceptPackets() ) );
	SetDlgItemText( IDC_MESSAGES_STATUS, FormatEnabled( csSettings.IsLogMessages() ) );
	SetDlgItemText( IDC_SUMMARY_STATUS, FormatEnabled( csSettings.IsSummaryStats() ) );

	SetDlgItemText( IDC_FILENAME_PREFIX, csSettings.GetFileNamePrefixAppend() );
	SetDlgItemText( IDC_OUTPUT_STATUS, csSettings.GetOutputDirectory() );

	SetDlgItemText( IDC_START_STATUS, csSettings.FormatStartTime() );
	SetDlgItemText( IDC_STOP_STATUS, csSettings.FormatStopTime() );

	if ( talTargets.IsCaptureAll() ) {
		CWnd*	pHide( GetDlgItem( IDC_ADDRESSES_STATUS ) );
		if ( pHide && pHide->GetSafeHwnd() ) {
			pHide->ShowWindow( SW_HIDE );
		}

		int	ayShowIds[] = {
			IDC_CAPTURE_ALL_STATUS,
		};
		for ( size_t i( 0 ); i < COUNTOF( ayShowIds ); i++ ) {
			CWnd*	pItem( GetDlgItem( ayShowIds[i] ) );
			if ( pItem && pItem->GetSafeHwnd() ) pItem->ShowWindow( SW_SHOW );
		}
	}
	else {
		int	ayHideIds[] = {
			IDC_CAPTURE_ALL_STATUS,
		};
		for ( size_t i( 0 ); i < COUNTOF( ayHideIds ); i++ ) {
			CWnd*	pHide( GetDlgItem( ayHideIds[i] ) );
			if ( pHide && pHide->GetSafeHwnd() ) pHide->ShowWindow( SW_HIDE );
		}

		CListBox*	pAddressList( static_cast<CListBox*>( GetDlgItem( IDC_ADDRESSES_STATUS ) ) );
		if ( pAddressList ) {
			pAddressList->ResetContent();
			pAddressList->ShowWindow( SW_SHOW );

			const size_t	nCount( talTargets.GetCount() );
			for ( size_t i( 0 ); i < nCount; i++ ) {
				CPeekString	strName( talTargets[i].Format() );
				pAddressList->AddString( strName );
			}
		}
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsStatus, CDialog)
	ON_BN_CLICKED(IDC_EXPORT_ADDRESS, OnBnClickedExportAddress)
	ON_LBN_SELCHANGE(IDC_CASE_LIST, &COptionsStatus::OnLbnSelchangeCaseList)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedExportAddress
// ----------------------------------------------------------------------------

void
COptionsStatus::OnBnClickedExportAddress()
{
#if (TODO)
	try {
		// XML
		CEasyXMLDocument	XmlDoc( CEasyXML::kEncoding_Utf8 );

		// the root node
		CEasyXMLElement*	pRoot( XmlDoc.AddRootElement( ConfigTags::kCaseList ) );
		ASSERT( pRoot != NULL );
		if ( pRoot == NULL ) return;

		CPeekString			strCaseName;
		CEasyXMLElement*	pCaseList( pRoot );
		if ( pCaseList == NULL ) return;

		int	nItem( -1 );
		for ( UINT i = 0; i < nCount; i++ ) {
			nItem = pList->GetNextItem( nItem, LVNI_SELECTED );
			ASSERT( nItem != -1 );

			const CCase&	Case( m_Options.GetCases().GetAt( nItem ) );
			if ( strCaseName.IsEmpty() ) {
				strCaseName = static_cast<LPCSTR>( Case.GetName() );
			}

			CEasyXMLElement*	pCase( pCaseList->AddChild( ConfigTags::kCase ) );
			ASSERT( pCase != NULL );
			if ( pCase != NULL ) {
				Case.Store( pCase );
			}
		}

		// Save XML file
		CFileDialog	FileDialog( FALSE );
		CPeekString	strTempName( LegalizeFilename( strCaseName ) );
		FileDialog.GetOFN().nMaxFile = MAX_PATH + 1;
		FileDialog.GetOFN().lpstrFile = strTempName.GetBufferSetLength( MAX_PATH + 1 );
		FileDialog.GetOFN().lpstrInitialDir = strCaseName;
		FileDialog.GetOFN().lpstrFilter = L"XML File\0*.xml\0\0";

		if ( FileDialog.DoModal() != IDOK ) {
			return;
		}
		strTempName.ReleaseBuffer();

		CPeekString	strFileName( FileDialog.GetPathName() );
		CPeekString	strExt( FileDialog.GetFileExt() );
		if ( strExt.IsEmpty() && (strFileName.Right( 1 ) != L".") ) {
			strFileName += L".xml";
		}

		CPeekStringA	strXml( XmlDoc.FormatA() );
		CFileEx	fXml( strFileName, (CFileEx::modeCreate | CFileEx::modeWrite) );
		if ( fXml.IsOpen() ) {
			fXml.Write( strXml );
		}
		else {
			CPeekString	strMsg;
			strMsg.Format(
				L"Failed to create the settings file:\r\n   %s\r\n\r\n(The error code is: 0x%08X)\r\n\r\nCopy the settings to the clipboard?",
				strFileName, fXml.GetLastError() );

			CPeekString	strTitle;
			strTitle.LoadString( IDS_NAME );

			int	nReturnVal( ::MessageBox( NULL, strMsg, strTitle, MB_YESNO ) );
			if ( nReturnVal == IDYES ) {
				CopyToClipboard( strXml );
			}
		}
	}
	catch ( ... ) {
	}
#endif
}


// ----------------------------------------------------------------------------
//		OnBnClickedExportAddress
// ----------------------------------------------------------------------------

void
COptionsStatus::OnLbnSelchangeCaseList()
{
	CListBox*	pCaseList = static_cast<CListBox*>( GetDlgItem( IDC_CASE_LIST ) );
	if ( pCaseList && pCaseList->GetSafeHwnd() ) {
		int	nItem( pCaseList->GetCurSel() );
		if ( nItem >= 0 ) {
			SetStatus( nItem );
		}
	}
}


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
COptionsStatus::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CDialog::OnShowWindow( bShow, nStatus );
	OnLbnSelchangeCaseList();
}

#endif // HE_WIN32

