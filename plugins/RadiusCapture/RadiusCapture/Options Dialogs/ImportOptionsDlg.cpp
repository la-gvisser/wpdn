// ============================================================================
//	ImportOptionsDlg.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "stdafx.h"
#include "ImportOptionsDlg.h"
#include "CaseOptionsDlg.h"
#include "PluginMessages.h"
#include "CaseViewDlg.h"
#include "FileEx.h"

#define kColumn_Name				0
#define kColumn_UserNames			1
#define kColumn_Start				2
#define kColumn_Stop				3


////////////////////////////////////////////////////////////////////////////////
//		CImportOptionsDlg
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CImportOptionsDlg, CDialog)

// -----------------------------------------------------------------------------
//  Construction
// -----------------------------------------------------------------------------

CImportOptionsDlg::CImportOptionsDlg(
	 bool			bIsCapturing,
	 COptions&		inOptions,
	 COptions&		inParentOptions,
	 COmniPlugin*	inPlugin,
	 CWnd*			pParent /*=NULL*/ )
	 :	CDialog( CImportOptionsDlg::IDD_MODAL, pParent )
	 ,	m_bIsCapturing( bIsCapturing )
	 ,	m_bHaveImportFile( false )
	 ,	m_Options( inOptions )
	 ,	m_bInitializing( false )
	 ,	m_bImportEntire( false )
	 ,	m_ParentOptions( inParentOptions )
	 ,	m_pImportCaseOptionsList( &(m_Options.GetCaseOptionsList()) )
	 ,	m_pPlugin( inPlugin )
	 ,	m_bWaitingForOptions( false )
	 ,	m_nTimer( 0 )
{
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CImportOptionsDlg::OnInitDialog()
{
	m_bInitializing = true;

	SetControlState();

	m_bInitializing = true;

	if ( !InitializeCaseList() ) {
		m_bInitializing = false;
		return FALSE;
	}

	CWnd* pDisplayButton = GetDlgItem( IDC_BTN_READ_AND_DISPLAY );
	ASSERT( pDisplayButton );
	if ( pDisplayButton ) {
		pDisplayButton->EnableWindow( FALSE );
	}

	m_bInitializing = true;

	__super::OnInitDialog();

	m_bInitializing = false;

	return FALSE;
}


BEGIN_MESSAGE_MAP(CImportOptionsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
//	ON_BN_CLICKED(IDC_IMPORT_BROWSE, OnBnClickedBrowseForFile)
	ON_BN_CLICKED(IDC_IMPORT_ENTIRE_FILE, OnRadioImportEntire)
	ON_BN_CLICKED(IDC_IMPORT_CASES, OnRadioImportCases)
	ON_BN_CLICKED(IDC_VIEW_CASE, OnBnClickedView)
	ON_BN_CLICKED(IDC_BTN_READ_AND_DISPLAY, OnBnClickedReadDisplayFile)
	ON_EN_CHANGE(IDC_EDT_IMPORT_FILE, OnEnChangeImportFile)
	ON_NOTIFY(NM_DBLCLK, IDC_IMPORT_CASE_LIST, OnNMDblclkList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMPORT_CASE_LIST, OnLvnItemchangedList)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


/*
// -----------------------------------------------------------------------------
//		OnBnClickedBrowseForFile
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::OnBnClickedBrowseForFile()
{
	CString	strFilePath;
//	WCHAR	szFilter[] = _T( "XML Files (*.xml)|*.xml|All Files (*.*)|*.*|" );

	CWnd* pImportFileEdit = GetDlgItem( IDC_EDT_IMPORT_FILE );
	ASSERT( pImportFileEdit );
	if ( !pImportFileEdit ) return;

	CString strImportFileName; 
	pImportFileEdit->GetWindowText( strImportFileName );

	if ( strImportFileName.GetLength() >  3 ) {
		strImportFileName.Replace( _T("/"), _T("\\") );
		if ( (strImportFileName.Right(4)).MakeLower() == _T(".xml") ) {
			if ( (ValidateImportFile( strImportFileName, false ) == true) ) {
				strFilePath = strImportFileName;
			}
		}
		else { 
			if ( strImportFileName.Find( '.' ) < 0 ) {
				if ( strImportFileName.Right(1) == _T("/") ) {
					strImportFileName = strImportFileName.Left( strImportFileName.GetLength() - 1 ); 
				}
				if ( strImportFileName.Right(1) != _T("\\") ) {
					strImportFileName += _T("\\");
				}
				if ( ::DirectoryExists( strImportFileName ) ) {
					strFilePath = strImportFileName;
				}
			}
		}
	}

//	CFileDialog		dlg( TRUE, _T("xml"), NULL, 
//		OFN_ENABLESIZING | OFN_EXPLORER |
//		OFN_NOCHANGEDIR | OFN_HIDEREADONLY,
//		szFilter, NULL, 0 );
//
//	dlg.m_ofn.lpstrFile = strFilePath.GetBuffer( MAX_PATH );
//	int	nResult = dlg.DoModal();
//	strFilePath.ReleaseBuffer();
//	if ( nResult != IDOK ) return;
//
//	strFilePath.Trim();
//
//	pImportFileEdit->SetWindowText( strFilePath );
//
//	OnBnClickedReadDisplayFile();
}
*/


// -----------------------------------------------------------------------------
//		OnBnClickedReadDisplayFile
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::OnBnClickedReadDisplayFile()
{
	if ( m_bWaitingForOptions ) return;

//	HRESULT hr = 0;

	try 
	{
		CString strFilePath;

		CWnd* pImportFileEdit = GetDlgItem( IDC_EDT_IMPORT_FILE );
		ASSERT( pImportFileEdit );
		if ( !pImportFileEdit ) return;

		pImportFileEdit->GetWindowText( strFilePath );
		strFilePath.Trim();

		pImportFileEdit->SetWindowText( strFilePath );
			
		CPeekString	strPath( strFilePath );
		if ( !ValidateImportPath( strPath ) ) {
			return;
		}

		// Disable gui objects
		SetWaitForResponse( true );
				
#ifdef IMPLEMENT_PLUGINMESSAGE
		RequestOptionsImport( strPath );
#endif // IMPLEMENT_PLUGINMESSAGE

	}
	catch(...)
	{
		AfxMessageBox( L"Unknown failure importing options file." );
	}

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//		RefreshOptionsDisplay
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::RefreshOptionsDisplay()
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bInitializing = true;

	try 
	{
		EmptyCaseOptionsList();

		for( UInt32 i = 0; i < m_pImportCaseOptionsList->GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = m_pImportCaseOptionsList->GetAt( i );
			AddCase( theCaseOptions );
		}
	}
	catch(...) 
	{

	}

	SetEditControlsSelected();

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//		AddCase
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::AddCase(
	CCaseOptions* theCaseOptions )
{
	//	USES_CONVERSION;
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) return;

	int nIndex = pList->GetItemCount();

	pList->InsertItem( nIndex, theCaseOptions->GetName() );
	pList->SetItemData( nIndex, (DWORD_PTR)&theCaseOptions );
	SetCaseData( theCaseOptions, nIndex );
}


// -----------------------------------------------------------------------------
//		SetCaseData
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::SetCaseData( 
	CCaseOptions* theCaseOptions,
	int nIndex )
{
	//	USES_CONVERSION;

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) return;

	pList->SetItemText( nIndex, kColumn_Name, theCaseOptions->GetName() );

	CUserNameList& theUserList = theCaseOptions->GetUserList();
	if ( theUserList.GetCount() > 0 ) {
		CString theUserName;
		bool	bEnabledUserDisplayed = false;
		for ( size_t i = 0; i < theUserList.GetCount(); i++ ) {
			const CUserName& theUserItem = theUserList.GetAt( i );
			if ( theUserItem.IsEnabled() ) {
				if ( bEnabledUserDisplayed ) theUserName += ", ";
				bEnabledUserDisplayed = true;
				theUserName += theUserItem.GetName();
			}
		}
		pList->SetItemText( nIndex, kColumn_UserNames, theUserName );
	}

	{
		CString theStartTime;
		SYSTEMTIME theSystemTime;
		UInt64 theFileTime = theCaseOptions->GetStartTime();
		UInt64 theLocalFileTime;
		::FileTimeToLocalFileTime( (const FILETIME*)&theFileTime, (FILETIME*)&theLocalFileTime );
		::FileTimeToSystemTime( (const FILETIME*)&theLocalFileTime, &theSystemTime );
		theStartTime = FormatTimeLocal( &theSystemTime );
		pList->SetItemText( nIndex, kColumn_Start, theStartTime );
	}

	{
		CString theStopTime;
		SYSTEMTIME theSystemTime;
		UInt64 theFileTime = theCaseOptions->GetStopTime();
		UInt64 theLocalFileTime;
		::FileTimeToLocalFileTime( (const FILETIME*)&theFileTime, (FILETIME*)&theLocalFileTime );
		::FileTimeToSystemTime( (const FILETIME*)&theLocalFileTime, &theSystemTime );
		theStopTime = FormatTimeLocal( &theSystemTime );		
		pList->SetItemText( nIndex, kColumn_Stop, theStopTime );
	}
}


// -----------------------------------------------------------------------------
//  InitializeCaseList
// -----------------------------------------------------------------------------

bool
CImportOptionsDlg::InitializeCaseList()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) {
		return FALSE;
	}

	pList->ModifyStyle( 0, WS_CLIPCHILDREN );

	pList->InsertColumn(   kColumn_Name, _T("Case Identifier") );
	pList->SetColumnWidth( kColumn_Name, 120 );
	pList->InsertColumn(   kColumn_UserNames, _T("User Names") );
	pList->SetColumnWidth( kColumn_UserNames, 155 );
	pList->InsertColumn(   kColumn_Start, _T("Start Time") );
	pList->SetColumnWidth( kColumn_Start, 257 );
	pList->InsertColumn(   kColumn_Stop, _T("Stop Time") );
	pList->SetColumnWidth( kColumn_Stop, 257 );

	pList->SetExtendedStyle( pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	pList->ModifyStyle( 0, WS_CLIPCHILDREN | LVS_SHOWSELALWAYS );

	return true;
}


// -----------------------------------------------------------------------------
//		RetrieveListItemsSelected
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::RetrieveListItemsSelected()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) return;

	CCaseOptionsList SaveOptionsList;
	SaveOptionsList = *m_pImportCaseOptionsList;
	m_pImportCaseOptionsList->Reset();

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL) return;

	while ( pos ) {
		int nItem = pList->GetNextSelectedItem(pos);

		CCaseOptions* pNewCaseOption = new CCaseOptions;
		ASSERT( pNewCaseOption );
		if ( !pNewCaseOption ) return;

		CCaseOptions* pFromCaseOption = SaveOptionsList.GetAt( nItem );
		ASSERT( pFromCaseOption );
		if ( !pFromCaseOption ) return;

		*pNewCaseOption = *pFromCaseOption;

		m_pImportCaseOptionsList->Add( pNewCaseOption );
	}
}


// -----------------------------------------------------------------------------
//		OnBnClickedCancel
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnBnClickedCancel()
{
	m_bWaitingForOptions = false;

	KillTimer( m_nTimer );   

	OnCancel();
}


// -----------------------------------------------------------------------------
//		OnBnClickedOk
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnBnClickedOk()
{
	CButton* pImportEntireBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_ENTIRE_FILE ) );
	ASSERT( pImportEntireBtn );
	if ( !pImportEntireBtn ) return;

	m_bImportEntire = pImportEntireBtn->GetCheck() ? true : false; 

	if ( !m_bImportEntire ) {
		RetrieveListItemsSelected();
	}

	KillTimer( m_nTimer );   

	OnOK();
}


// -----------------------------------------------------------------------------
//		SetControlState
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::SetControlState()
{
	m_bInitializing = true;

	CButton* pImportEntireBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_ENTIRE_FILE ) );
	ASSERT( pImportEntireBtn );
	if ( !pImportEntireBtn ) { m_bInitializing = false; return; }

	CButton* pImportCasesBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_CASES ) );
	ASSERT( pImportCasesBtn );
	if ( !pImportCasesBtn ) { m_bInitializing = false; return; }

	CWnd* pHighLightText = GetDlgItem( IDC_EDT_HIGHLIGHT_TEXT );
	ASSERT( pHighLightText );
	if ( !pHighLightText ) { m_bInitializing = false; return; }

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) { m_bInitializing = false; return; }

	CButton* pOKBtn = static_cast<CButton*>( GetDlgItem( IDOK ) );
	ASSERT( pOKBtn );
	if ( !pOKBtn ) { m_bInitializing = false; return; }

	if ( m_bWaitingForOptions ) {
		pImportEntireBtn->EnableWindow( FALSE );
		pImportCasesBtn->EnableWindow( FALSE );
		pHighLightText->EnableWindow( FALSE );
		pList->EnableWindow( FALSE );
		pOKBtn->EnableWindow( FALSE );
		return;
	}

	BOOL bEnableImportChoiceBtns = m_bHaveImportFile ? TRUE : FALSE;
	pImportEntireBtn->EnableWindow( bEnableImportChoiceBtns );
	pImportCasesBtn->EnableWindow( bEnableImportChoiceBtns );
	pOKBtn->EnableWindow( bEnableImportChoiceBtns );

	BOOL bImportEntireSelected = FALSE;
	BOOL bImportCasesSelected = FALSE;

	if ( m_bHaveImportFile ) {
		if ( m_bIsCapturing ) {
			pImportEntireBtn->EnableWindow( FALSE );
			pImportCasesBtn->EnableWindow( TRUE );
			bImportCasesSelected = TRUE;
		}
		else {
			pImportEntireBtn->EnableWindow( TRUE );
			pImportCasesBtn->EnableWindow( TRUE );

			bImportEntireSelected = pImportEntireBtn->GetCheck();
			bImportCasesSelected = pImportCasesBtn->GetCheck();

			if ( !bImportEntireSelected && !bImportCasesSelected ) {
				bImportCasesSelected = TRUE;
			}
			if ( bImportEntireSelected && bImportCasesSelected ) {
				ASSERT( 0 );
				bImportEntireSelected = FALSE;
			}
		}
	}
	else {
		pImportEntireBtn->EnableWindow( FALSE );
		pImportCasesBtn->EnableWindow( FALSE );
	}

	pImportEntireBtn->SetCheck( bImportEntireSelected );
	pImportCasesBtn->SetCheck( bImportCasesSelected );

	ASSERT( (m_bHaveImportFile && !m_bIsCapturing) ? (bImportEntireSelected || bImportCasesSelected) : true );
	ASSERT( (m_bHaveImportFile && !m_bIsCapturing) ? (!(bImportEntireSelected && bImportCasesSelected)) : true );

	int nShowAction = bImportEntireSelected ? SW_SHOW : SW_HIDE;

	CWnd* pWnd = GetDlgItem( IDC_IMPORT_FILE_BORDER );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_CAPTURE_TO_SCREEN );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_LOG_MESSAGES_TO_SCREEN );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_CAPTURE_TO_FILE );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_LOG_MESSAGES_TO_FILE );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FOLDERS_FOR_CASES );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_OUTPUT_ROOT_LBL );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_OUTPUT_ROOT_EDIT );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_NEW_FILE_CAPTION );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FILE_AGE );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FILE_SIZE );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FILE_AGE_COUNT );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FILE_AGE_UNITS );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FILE_SIZE_COUNT );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );
	pWnd = GetDlgItem( IDC_FILE_SIZE_UNITS );
	ASSERT( pWnd ); if ( !pWnd ) { m_bInitializing = false; return; }
	pWnd->ShowWindow( nShowAction );	

	if ( bImportCasesSelected ) {
		pHighLightText->ShowWindow( SW_SHOW );
		pList->SetFocus();
	}
	else {
		int nCount = pList->GetItemCount();
		for ( int i = 0; i < nCount; i++ ) {
			pList->SetItemState( i, 0, LVIS_SELECTED );
			pList->SetItemState( i, 0, LVIS_FOCUSED );
		}

		pHighLightText->ShowWindow( SW_HIDE );
	}

	SetEditControlsSelected();

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//		OnRadioImportEntire
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnRadioImportEntire()
{
	SetControlState();
}


// -----------------------------------------------------------------------------
//		OnRadioImportCases
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnRadioImportCases()
{
	SetControlState();
}


// -----------------------------------------------------------------------------
//		OnBnClickedView
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::OnBnClickedView()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int nItem = pList->GetNextSelectedItem(pos);
	CPeekString theName = (CPeekString)pList->GetItemText( nItem, 0 );

	CButton* pImportEntireBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_ENTIRE_FILE ) );
	ASSERT( pImportEntireBtn );
	if ( !pImportEntireBtn ) return;

	CButton* pImportCasesBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_CASES ) );
	ASSERT( pImportCasesBtn );
	if ( !pImportCasesBtn ) return;

	BOOL bImportEntireSelected = pImportEntireBtn->GetCheck();
	BOOL bImportCasesSelected = pImportCasesBtn->GetCheck();

	ASSERT( !(bImportEntireSelected && bImportCasesSelected) && !(!bImportEntireSelected && !bImportCasesSelected) );
	if ( (bImportEntireSelected && bImportCasesSelected) || (!bImportEntireSelected && !bImportCasesSelected) ) return;

	CCaseOptionsList& theParentCaseOptionsList = m_ParentOptions.GetCaseOptionsList();

	CCaseOptions* pSelectedImportCase = m_pImportCaseOptionsList->GetAt( nItem );

	CCaseOptions theCaseOptionsToUse;
	COptions*	 pOptionsToUse = NULL;

	if ( bImportCasesSelected ) {
		ASSERT( !bImportEntireSelected );
		pOptionsToUse = &m_ParentOptions;

		if ( theParentCaseOptionsList.IsNameMatch( theName ) ) {
			CCaseOptions* pParentCaseOptions = theParentCaseOptionsList.Lookup( theName );
			ASSERT( pParentCaseOptions );
			if ( !pParentCaseOptions ) return;
			theCaseOptionsToUse = *pParentCaseOptions;
			CCaseOptions::ImportCaseItems( &theCaseOptionsToUse, pSelectedImportCase,
				pOptionsToUse->GetCaptureOptions(), m_bIsCapturing );
		}
		else {
			theCaseOptionsToUse = *pSelectedImportCase;
		}
	}
	else {
		ASSERT( bImportEntireSelected );
		pOptionsToUse = &m_Options;

		theCaseOptionsToUse = *pSelectedImportCase;
	}

	theCaseOptionsToUse.SyncPortsToGlobal( pOptionsToUse->GetPortNumberList() );
	theCaseOptionsToUse.SyncCaptureOptionsToGlobal( pOptionsToUse->GetCaptureOptions() );
	theCaseOptionsToUse.SyncGeneralOptionsToGlobal( pOptionsToUse->GetOutputRootDirectory(), pOptionsToUse->IsUseFileNumber() );

	bool bDuplicate = theCaseOptionsToUse.GetDuplicate();
	bool bDisableNameEdit = (m_bIsCapturing & !bDuplicate) ? true : false;

	CCaseViewDlg theDialog( &theCaseOptionsToUse, pOptionsToUse, bDisableNameEdit, nItem );
	theDialog.DoModal();		

	pList->SetFocus();
}


// -----------------------------------------------------------------------------
//  OnNMDblclkList
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnNMDblclkList(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	if ( m_bInitializing ) return;

	OnBnClickedView();

	*pResult = 0;
}


// -----------------------------------------------------------------------------
//  OnLvnItemchangedList
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnLvnItemchangedList(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	if ( m_bInitializing ) return;

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);	if (pList == NULL) return;

	m_bInitializing = true;

	SetEditControlsSelected();

	m_bInitializing = false;

	// return FALSE (or 0) to allow change, TRUE to prevent
	*pResult = 0;
}


// -----------------------------------------------------------------------------
//  SetControlsSelected
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::SetEditControlsSelected()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_IMPORT_CASE_LIST );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	BOOL bSelected = static_cast<BOOL>( pos != NULL );

	CWnd* pEdit = GetDlgItem( IDC_VIEW_CASE );
	pEdit->EnableWindow( bSelected );

	CButton* pImportEntireBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_ENTIRE_FILE ) );
	ASSERT( pImportEntireBtn );
	if ( !pImportEntireBtn ) return;

	CButton* pImportCasesBtn = static_cast<CButton*>( GetDlgItem( IDC_IMPORT_CASES ) );
	ASSERT( pImportCasesBtn );
	if ( !pImportCasesBtn ) return;

	CButton* pOKBtn = static_cast<CButton*>( GetDlgItem( IDOK ) );
	ASSERT( pOKBtn );
	if ( !pOKBtn ) return;

	BOOL bEnableProceedBtn = FALSE;
	BOOL bImportEntireSelected = pImportEntireBtn->GetCheck();
	BOOL bImportCasesSelected = pImportCasesBtn->GetCheck();

	if ( m_bHaveImportFile && bImportEntireSelected ) bEnableProceedBtn = TRUE;
	if ( m_bHaveImportFile && bImportCasesSelected && bSelected ) bEnableProceedBtn = TRUE;
	
	pList->EnableWindow( bImportCasesSelected );

	pOKBtn->EnableWindow( bEnableProceedBtn );
}


// -----------------------------------------------------------------------------
//  DoDataExchange
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::DoDataExchange(
	CDataExchange* pDX )
{
	if ( pDX->m_bSaveAndValidate ) return;

	UInt32				nAgeCount;
	int					nAgeUnits;
	UInt64				nSizeCount;
	int					nSizeUnits;

	if ( !m_bHaveImportFile ) return;

	BOOL bChecked = m_Options.IsCaptureToScreen() ? TRUE : FALSE;
	DDX_Check( pDX, IDC_CAPTURE_TO_SCREEN, bChecked );
	bChecked = m_Options.IsLogMsgsToScreen() ? TRUE : FALSE;
	DDX_Check( pDX, IDC_LOG_MESSAGES_TO_SCREEN, bChecked );
	bChecked = m_Options.IsCaptureToFile() ? TRUE : FALSE;
	DDX_Check( pDX, IDC_CAPTURE_TO_FILE, bChecked );
	bChecked = m_Options.IsLogMsgsToFileButton() ? TRUE : FALSE;
	DDX_Check( pDX, IDC_LOG_MESSAGES_TO_FILE, bChecked );
	bChecked = m_Options.IsFoldersForCasesButton() ? TRUE : FALSE;
	DDX_Check( pDX, IDC_FOLDERS_FOR_CASES, bChecked );

	CString strOutputRootDirectory = m_Options.GetOutputRootDirectory();
	DDX_Text( pDX, IDC_OUTPUT_ROOT_EDIT, strOutputRootDirectory );

//	CString strFacility = m_Options.GetFacility();
//	DDX_Text( pDX, IDC_FACILITY_EDIT, strFacility );

	if ( !m_Options.IsSaveAgeSelected() && !m_Options.IsSaveSizeSelected() ) {
		m_Options.SetSaveAgeSelected( true );
		if ( m_Options.GetSaveAgeSeconds() == 0 ) {
			m_Options.SetSaveAgeValue( kSecondsInADay );	
		}
	}

	BOOL bIsSaveAge = m_Options.IsSaveAgeSelected();
	DDX_Check( pDX, IDC_FILE_AGE, bIsSaveAge );

	BOOL bIsSaveSize = m_Options.IsSaveSizeSelected();
	DDX_Check( pDX, IDC_FILE_SIZE, bIsSaveSize );

	const UInt32 nSaveAgeValue = m_Options.GetSaveAgeSeconds();
	if ( (nSaveAgeValue % kSecondsInADay) == 0 ) {
		nAgeCount = nSaveAgeValue / kSecondsInADay;
		nAgeUnits = kSave_Days;
	}
	else if ( (nSaveAgeValue % kSecondsInAnHour) == 0 ) {
		nAgeCount = nSaveAgeValue / kSecondsInAnHour;
		nAgeUnits = kSave_Hours;
	}
	else {
		ASSERT( (nSaveAgeValue % kSecondsInAMinute) == 0 );
		nAgeCount = nSaveAgeValue / kSecondsInAMinute;
		nAgeUnits = kSave_Minutes;
	}

	const UInt64 nSaveSizeValue = m_Options.GetSaveSizeBytes();
	if ( (nSaveSizeValue % kGigabyte) == 0 ) {
		nSizeCount = nSaveSizeValue / kGigabyte;
		nSizeUnits = kMax_Gigabytes;
	}
	else if ( (nSaveSizeValue % kMegabyte) == 0 ) {
		nSizeCount = nSaveSizeValue / kMegabyte;
		nSizeUnits = kMax_Megabytes;
	}
	else {
		nSizeCount = nSaveSizeValue / kKilobyte;
		nSizeUnits = kMax_Kilobytes;
	}

	DDX_Text( pDX, IDC_FILE_AGE_COUNT, nAgeCount );
	DDX_CBIndex( pDX, IDC_FILE_AGE_UNITS, nAgeUnits );

	DDX_Text( pDX, IDC_FILE_SIZE_COUNT, nSizeCount );
	DDX_CBIndex( pDX, IDC_FILE_SIZE_UNITS, nSizeUnits );
}


// -----------------------------------------------------------------------------
//  ValidateImportPath
// -----------------------------------------------------------------------------

bool
CImportOptionsDlg::ValidateImportPath( 
	CPeekString& inImportFilePath )
{
	CWnd* pImportFileEdit = GetDlgItem( IDC_EDT_IMPORT_FILE );
	ASSERT( pImportFileEdit );
	if ( !pImportFileEdit ) return false;

	if ( inImportFilePath.GetLength() == 0 ) {
		AfxMessageBox( L"Please enter an import file path and name." );
		pImportFileEdit->SetFocus();
		return false;
	}

	if ( inImportFilePath.GetAt(1) != _T(':') ) {
		AfxMessageBox( L"Import path is incorrectly formatted." );
		pImportFileEdit->SetFocus();
		return false;
	}

	if ( inImportFilePath.GetLength() < 4 ){
		AfxMessageBox( L"File path is not valid." );
		pImportFileEdit->SetFocus();
		return false;
	}

	const size_t nLength( inImportFilePath.GetLength() );
	for ( size_t i = 0; i < nLength; i++ ) {
		if ( !FilePath::IsValidPathChar( inImportFilePath[i] ) ) {
			CString	strErrorMsg;
			strErrorMsg.Format( L"Invalid character \"%c\" in file path name.", inImportFilePath[i] );
			AfxMessageBox( strErrorMsg );
			pImportFileEdit->SetFocus();
			return false;
		}
	}

	if ( inImportFilePath.Right( 1 ) == L"\\" || inImportFilePath.Right( 1 ) == L"/" ) {
		AfxMessageBox( _T("Please enter a valid file name.") );
		pImportFileEdit->SetFocus();
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//  OnEnChangeImportFile
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::OnEnChangeImportFile()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd* pImportFileEdit = GetDlgItem( IDC_EDT_IMPORT_FILE );
	ASSERT( pImportFileEdit );
	if ( !pImportFileEdit ) return;

	CWnd* pDisplayButton = GetDlgItem( IDC_BTN_READ_AND_DISPLAY );
	ASSERT( pDisplayButton );
	if ( !pDisplayButton ) return;

	static CString	strLastText = _T("");
	CString			strCurrentText;

	BOOL bEnabled = FALSE;
	pImportFileEdit->GetWindowText( strCurrentText );
	if ( strCurrentText.GetLength() > 4 && strCurrentText != strLastText ) bEnabled = TRUE;

	pDisplayButton->EnableWindow( bEnabled );
}

// -----------------------------------------------------------------------------
//  ClearImportOptions
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::ClearImportOptions()
{
	m_Options.ResetCaseOptions();

	COptions TempOptions;
	m_Options = TempOptions;


	UpdateData( FALSE );
	RefreshOptionsDisplay();

	m_bHaveImportFile = false;

	CButton* pOKBtn = static_cast<CButton*>( GetDlgItem( IDOK ) );
	ASSERT( pOKBtn );
	if ( !pOKBtn ) { m_bInitializing = false; return; }

	pOKBtn->EnableWindow( FALSE );
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		RequestOptionsImport
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::RequestOptionsImport( 
	CPeekString& inFilePath )
{
	_ASSERTE( m_pPlugin != NULL );
	if ( m_pPlugin == NULL ) return;

	CMessageImportExport*	pMsgImport = new CMessageImportExport();
	if ( (pMsgImport == NULL) || (!pMsgImport->IsModeled()) ) return;

	pMsgImport->SetImportExportMessage( inFilePath );
	pMsgImport->SetAsImport();

	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pPlugin->PostPluginMessage( CPeekMessagePtr( pMsgImport ), this, fnProcess );
}


// -----------------------------------------------------------------------------
//		GetOptionsResponse
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::GetOptionsResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( inResult != HE_S_OK ) return;

	CMessageImportExport*	pMessage = dynamic_cast<CMessageImportExport*>( inMessage.get() );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	ASSERT( pMessage->IsImport() );
	if ( !pMessage->IsImport() )	return;

	CPeekString	strResultMessage( pMessage->GetImportExportMessage() );

	const  bool bSuccess = strResultMessage == L"Success";
	if ( !bSuccess ) {
		CString	strErrorMessage;
		strErrorMessage.Format( 
			L"Attempt to retrieve options from the engine was not successful.\r\nError message is:\r\n\r\n%s",
			strResultMessage.Format() );
		AfxMessageBox( strErrorMessage );
		return;
	}

	COptions& theNewOptions = pMessage->GetOptions();
	ProcessReturnedOptions( theNewOptions );

	return;
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
CImportOptionsDlg::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( !m_bWaitingForOptions )	return;

	int	nType = inMessage->GetType();
	switch ( nType ) {
		case CMessageImportExport::s_nMessageType:
			GetOptionsResponse( inResult, inMessage, inResponse );
			break;
		default:
			break;
	}

	SetWaitForResponse( false );

	m_bInitializing = false;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		ProcessReturnedOptions
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::ProcessReturnedOptions( COptions& theNewOptions )
{
	m_Options = theNewOptions;
	m_bHaveImportFile = true;

	UpdateData( FALSE );

	RefreshOptionsDisplay();

	SetControlState();

	CWnd* pDisplayButton = GetDlgItem( IDC_BTN_READ_AND_DISPLAY );
	ASSERT( pDisplayButton );
	if ( pDisplayButton ) {
		pDisplayButton->EnableWindow( FALSE );
	}
}


// -----------------------------------------------------------------------------
//		SetWaitForResponse
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::SetWaitForResponse(
	bool inStartWait )
{
	BOOL	bEnable( inStartWait == true ? FALSE : TRUE );

	m_bWaitingForOptions = inStartWait;

	CWnd* pWnd = GetDlgItem( IDC_BTN_READ_AND_DISPLAY );
	pWnd->EnableWindow( bEnable );

	pWnd = GetDlgItem( IDC_EDT_IMPORT_FILE );
	pWnd->EnableWindow( bEnable );

	SetControlState();

	if ( inStartWait ) {
		m_nTimer = SetTimer( 1, 57000, 0 );
		BeginWaitCursor();
	}
	else {
		KillTimer( m_nTimer );
		EndWaitCursor();
	}
}


// -----------------------------------------------------------------------------
//		OnTimer
// -----------------------------------------------------------------------------

void 
CImportOptionsDlg::OnTimer(
	UINT_PTR nIDEvent )
{
	nIDEvent;

	AfxMessageBox( L"Wait for engine return of import options has timed out." );

	SetWaitForResponse( false );
}


// -----------------------------------------------------------------------------
//		OnSetCursor
// -----------------------------------------------------------------------------

BOOL
CImportOptionsDlg::OnSetCursor(
	CWnd* pWnd, 
	UINT nHitTest, 
	UINT message )
{
	if ( m_bWaitingForOptions )
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor( IDC_WAIT ) );
		return TRUE;
	}

	return CDialog::OnSetCursor( pWnd, nHitTest, message );
}
