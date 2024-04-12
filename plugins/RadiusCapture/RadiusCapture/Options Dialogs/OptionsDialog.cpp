// ============================================================================
//	OptionsDialog.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "stdafx.h"
#include "OptionsDialog.h"
#include "ImportOptionsDlg.h"
#include "ExportOptionsDlg.h"
#include "ReplaceCaseDlg.h"
#include "CaseOptionsDlg.h"
#include "PeekTime.h"
#include "CaseViewDlg.h"
#include "FileEx.h"

#ifdef MODE_OPTIONS
 #define DIALOG_DEFAULT_WIDTH 1093
#else
 #define DIALOG_DEFAULT_WIDTH 1275
#endif

#define kColumn_Name				0
#define kColumn_UserNames			1

#define kColumn_Start				2
#define kColumn_Stop				3
#define kColumn_OutputDir			4
#define kColumn_ToScreen			5
#define kColumn_LogEventsToScreen	6
#define kColumn_ToFile				7
#define kColumn_LogEventsToFile		8

#define DIALOG_MIN_WIDTH		960
#define DIALOG_MIN_HEIGHT_MODAL	350
#define DIALOG_MIN_HEIGHT_TAB	220

// Length maxima:
#define MAX_NAME_LEN			30
#define MAX_PATH_LEN			256

////////////////////////////////////////////////////////////////////////////////
//		COptionsDialog
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(COptionsDialog, CDialog)

// -----------------------------------------------------------------------------
//  Construction
// -----------------------------------------------------------------------------

COptionsDialog::COptionsDialog(
	COptions*		inOptions,
	bool			bIsCapturing,
	bool			bIsDockedTab, 
	COmniPlugin*	inPlugin,
	int				nCaseItem,
	CWnd*			pParent /*=NULL*/ )
	: CDialog( bIsDockedTab ? COptionsDialog::IDD : COptionsDialog::IDD_MODAL, pParent )
	,	m_pOptions( inOptions )
	, 	m_pCaseOptionsList( &(m_pOptions->GetCaseOptionsList()) )
	,	m_bIsDockedTab( bIsDockedTab )
	,	m_bInitializing( false )
	,	m_nCaseItem( nCaseItem )
	,   m_bIsCapturing( bIsCapturing )
	,	m_pPlugin( inPlugin )
{
}

// -----------------------------------------------------------------------------
//  DoDataExchange
// -----------------------------------------------------------------------------

void
COptionsDialog::DoDataExchange(
	CDataExchange* pDX )
{
	UInt32				nAgeCount;
	int					nAgeUnits;
	UInt64				nSizeCount;
	int					nSizeUnits;

	ASSERT( !m_bIsDockedTab );

	if ( !m_bIsDockedTab) {
		DDX_Control(pDX, IDC_CHK_DISPLAY_WARNING, m_BtnDisplayWarning);
	}

	if ( !pDX->m_bSaveAndValidate ) {

		if ( !m_bIsDockedTab ) {
			BOOL bIsCaptureToFile = static_cast<BOOL>( m_pOptions->IsCaptureToFile() );
			DDX_Check( pDX, IDC_CAPTURE_TO_FILE, bIsCaptureToFile );
		}

		BOOL bOptionsLogMsgsToFileButton = static_cast<BOOL>( m_pOptions->IsLogMsgsToFileButton() );
		DDX_Check( pDX, IDC_LOG_MESSAGES_TO_FILE, bOptionsLogMsgsToFileButton );

		BOOL bOptionsFoldersForCasesButton = static_cast<BOOL>( m_pOptions->IsFoldersForCasesButton() );
		DDX_Check( pDX, IDC_FOLDERS_FOR_CASES, bOptionsFoldersForCasesButton );

		BOOL bIsCaptureToScreen = static_cast<BOOL>( m_pOptions->IsCaptureToScreen() );
		DDX_Check( pDX, IDC_CAPTURE_TO_SCREEN, bIsCaptureToScreen );

		BOOL bOptionsLogMsgsToScreen = static_cast<BOOL>( m_pOptions->IsLogMsgsToScreen() );
		DDX_Check( pDX, IDC_LOG_MESSAGES_TO_SCREEN, bOptionsLogMsgsToScreen );

		CString strOutputRootDirectory = m_pOptions->GetOutputRootDirectory();
		DDX_Text( pDX, IDC_OUTPUT_ROOT_EDIT, strOutputRootDirectory );

		BOOL bIsUseFileNumber = static_cast<BOOL>( m_pOptions->IsUseFileNumber() );
		DDX_Check( pDX, IDC_CHK_USE_FILE_NUMBER, bIsUseFileNumber );

		if ( !m_pOptions->IsSaveAgeSelected() && !m_pOptions->IsSaveSizeSelected() ) {
			m_pOptions->SetSaveAgeSelected( true );
			if ( m_pOptions->GetSaveAgeSeconds() == 0 ) {
				m_pOptions->SetSaveAgeValue( kSecondsInADay );	
			}
		}

		BOOL bIsSaveAge = m_pOptions->IsSaveAgeSelected();
		DDX_Check( pDX, IDC_FILE_AGE, bIsSaveAge );

		BOOL bIsSaveSize = m_pOptions->IsSaveSizeSelected();
		DDX_Check( pDX, IDC_FILE_SIZE, bIsSaveSize );

		const UInt32 nSaveAgeValue = m_pOptions->GetSaveAgeSeconds();
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

		const UInt64 nSaveSizeValue = m_pOptions->GetSaveSizeBytes();
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

	CDialog::DoDataExchange(pDX);

	if ( pDX->m_bSaveAndValidate ) {

		if ( m_bIsDockedTab ) {
			return;
		}

		BOOL bIsSaveAge = false;
		DDX_Check( pDX, IDC_FILE_AGE, bIsSaveAge );
		if ( bIsSaveAge != static_cast<BOOL>(m_pOptions->IsSaveAgeSelected()) ) {
			m_pOptions->SetSaveAgeSelected( bIsSaveAge == TRUE );
		}
		
		BOOL bIsSaveSize = false;
		DDX_Check( pDX, IDC_FILE_SIZE, bIsSaveSize );
		if ( bIsSaveSize != static_cast<BOOL>(m_pOptions->IsSaveSizeSelected()) ) {
			m_pOptions->SetSaveSizeSelected( bIsSaveSize == TRUE );
		}

		CString strValue;
		CEdit* pAgeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_AGE_COUNT );
		pAgeCountEdit->GetWindowText( strValue );
		if ( strValue.GetLength() == 0 ) {
			pAgeCountEdit->SetWindowText( _T("0") );
		}

		DDX_Text( pDX, IDC_FILE_AGE_COUNT, nAgeCount );
		DDX_CBIndex( pDX, IDC_FILE_AGE_UNITS, nAgeUnits );

		CEdit* pSizeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_SIZE_COUNT );
		pSizeCountEdit->GetWindowText( strValue );
		if ( strValue.GetLength() == 0 ) {
			pSizeCountEdit->SetWindowText( _T("0") );
		}

		DDX_Text( pDX, IDC_FILE_SIZE_COUNT, nSizeCount );
		DDX_CBIndex( pDX, IDC_FILE_SIZE_UNITS, nSizeUnits );

		UInt32 nSaveAgeValue = 0;
		switch ( nAgeUnits ) {
			case kSave_Days:
				nSaveAgeValue = nAgeCount * kSecondsInADay;
				break;
			case kSave_Hours:
				nSaveAgeValue = nAgeCount * kSecondsInAnHour;
				break;
			default:	// kSave_Minutes
				nSaveAgeValue = nAgeCount * kSecondsInAMinute;
				break;
		}

		if ( nSaveAgeValue == 0 ) {
			if ( m_pOptions->IsSaveAgeSelected() ) {
				m_pOptions->SetSaveAgeSelected( false );
			}
		}

		if ( nSaveAgeValue !=  m_pOptions->GetSaveAgeSeconds() ) {
			m_pOptions->SetSaveAgeValue( nSaveAgeValue );
		}

		UInt64 nSaveSizeValue = 0;
		switch ( nSizeUnits ) {
			case kMax_Gigabytes:
				nSaveSizeValue = nSizeCount * kGigabyte;
				break;
			case kMax_Megabytes:
				nSaveSizeValue = nSizeCount * kMegabyte;
				break;
			default:	// kMax_Kilobytes
				nSaveSizeValue = nSizeCount * kKilobyte;
				break;
		}

		if ( nSaveSizeValue == 0 ) {
			if ( m_pOptions->IsSaveSizeSelected() ) {
				m_pOptions->SetSaveSizeSelected( false );
			}
		}

		if ( nSaveSizeValue !=  m_pOptions->GetSaveSizeBytes() ) {
			m_pOptions->SetSaveSizeValue( nSaveSizeValue );
		}

		ASSERT( m_pOptions->IsCaptureToFile() ? (m_pOptions->IsSaveAgeSelected() || m_pOptions->IsSaveSizeSelected()) == true : true );

		if ( !m_bIsCapturing ) {

			BOOL bIsCaptureToFile = FALSE;
			DDX_Check( pDX, IDC_CAPTURE_TO_FILE, bIsCaptureToFile );
			if ( bIsCaptureToFile != static_cast<BOOL>(m_pOptions->IsCaptureToFile()) ) {
				m_pOptions->SetCaptureToFile( bIsCaptureToFile == TRUE );
			}

			BOOL bIsUseFileNumber = FALSE;
			DDX_Check( pDX, IDC_CHK_USE_FILE_NUMBER, bIsUseFileNumber );
			if ( bIsUseFileNumber != static_cast<BOOL>( m_pOptions->IsUseFileNumber() ) ) {
				m_pOptions->SetUseFileNumber( bIsUseFileNumber == TRUE );
			}

			BOOL bIsCaptureToScreen = FALSE;
			DDX_Check( pDX, IDC_CAPTURE_TO_SCREEN, bIsCaptureToScreen );
			if ( bIsCaptureToScreen != static_cast<BOOL>(m_pOptions->IsCaptureToScreen()) ) {
				m_pOptions->SetCaptureToScreen( bIsCaptureToScreen == TRUE );
			}

			if ( !bIsCaptureToFile && !bIsCaptureToScreen) {
				ASSERT ( 0 );
				bIsCaptureToScreen = TRUE;
			}

			BOOL bIsLogMsgsToFileButton;
			DDX_Check( pDX, IDC_LOG_MESSAGES_TO_FILE, bIsLogMsgsToFileButton );
			if ( bIsLogMsgsToFileButton != static_cast<BOOL>(m_pOptions->IsLogMsgsToFileButton()) ) {
				m_pOptions->SetLogMsgsToFileButton( (bIsLogMsgsToFileButton == TRUE) );
			}

			BOOL bIsFoldersForCasesButton;
			DDX_Check( pDX, IDC_FOLDERS_FOR_CASES, bIsFoldersForCasesButton );
			if ( bIsFoldersForCasesButton != static_cast<BOOL>(m_pOptions->IsFoldersForCasesButton()) ) {
				m_pOptions->SetFoldersForCasesButton( (bIsFoldersForCasesButton == TRUE) );
			}

			BOOL bIsLogMsgsToScreen;
			DDX_Check( pDX, IDC_LOG_MESSAGES_TO_SCREEN, bIsLogMsgsToScreen );
			if ( bIsLogMsgsToScreen != static_cast<BOOL>(m_pOptions->IsLogMsgsToScreen()) ) {
				m_pOptions->SetLogMsgsToScreen( (bIsLogMsgsToScreen == TRUE) );
			}

			CString strOutputRootDirectory;
			GetDlgItemText( IDC_OUTPUT_ROOT_EDIT, strOutputRootDirectory );
			strOutputRootDirectory.Trim();
			if ( strOutputRootDirectory != (CString)m_pOptions->GetOutputRootDirectory() ) {
				m_pOptions->SetOutputRootDirectory( (CPeekString)strOutputRootDirectory );
			}
		}

		ASSERT( !m_bIsDockedTab );
	}
}


// -----------------------------------------------------------------------------
//  Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(IDC_BTN_ADD_CASE, OnBnClickedAdd)
	ON_COMMAND(IDC_BTN_EDIT_CASE, OnBnClickedEdit)
	ON_COMMAND(IDC_BTN_VIEW_CASE, OnBnClickedEdit)
	ON_COMMAND(IDC_BTN_COPY_CASE, OnBnClickedDuplicate)
	ON_COMMAND(IDC_BTN_DELETE_CASE, OnBnClickedDelete)
	ON_COMMAND(IDC_BTN_IMPORT_CASE, OnBnClickedImport)
	ON_COMMAND(IDC_BTN_EXPORT_CASE, OnBnClickedExport)
	ON_COMMAND(ID_MENU_ENABLEALL, EnableAll)
	ON_COMMAND(ID_MENU_DISABLEALL, DisableAll)
	ON_COMMAND(ID_MENU_DELETEALL, DeleteAllCases)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CASES, OnNMDblclkList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CASES, OnLvnItemchangedList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_CASES, OnLvnDeleteitemList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CASES, OnNMRClickList)
	ON_UPDATE_COMMAND_UI( ID_MENU_ADD, OnUpdateMenuAdd )
	ON_UPDATE_COMMAND_UI(ID_MENU_EDIT, OnUpdateMenuEdit)
	ON_UPDATE_COMMAND_UI(ID_MENU_VIEW, OnUpdateMenuView)
	ON_UPDATE_COMMAND_UI(ID_MENU_DUPLICATE, OnUpdateMenuDuplicate)
	ON_UPDATE_COMMAND_UI(ID_MENU_DELETE, OnUpdateMenuDelete)
	ON_BN_CLICKED(IDC_PORTS, OnBnClickedPorts)
	ON_BN_CLICKED(IDC_BROWSE_BTN, OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_BTN_SUGGEST_OUTPUT, OnBnClickedSuggestOutput)
	ON_BN_CLICKED(IDC_FILE_AGE, OnBnClickedFileAge)
	ON_BN_CLICKED(IDC_FILE_SIZE, OnBnClickedFileSize)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_OUTPUT_ROOT_EDIT, OnEnChangeOutputRootEdit)
	ON_BN_CLICKED(IDC_CHK_USE_FILE_NUMBER, OnBnClickedUseFileNumber)
	ON_EN_CHANGE(IDC_FILE_AGE_COUNT, OnEnChangeFileAgeCount)
	ON_CBN_SELCHANGE(IDC_FILE_AGE_UNITS, OnCbnSelchangeFileAgeUnits)
	ON_EN_CHANGE(IDC_FILE_SIZE_COUNT, OnEnChangeFileSizeCount)
	ON_CBN_SELCHANGE(IDC_FILE_SIZE_UNITS, OnCbnSelchangeFileSizeUnits)
	ON_BN_CLICKED(IDC_CAPTURE_TO_FILE, OnBnClickedCaptureToFile)
	ON_BN_CLICKED(IDC_LOG_MESSAGES_TO_FILE, OnBnClickedLogMsgsToFile)
	ON_BN_CLICKED(IDC_FOLDERS_FOR_CASES, OnBnClickedFoldersForCases)
	ON_BN_CLICKED(IDC_CAPTURE_TO_SCREEN, OnBnClickedCaptureToScreen)
	ON_BN_CLICKED(IDC_LOG_MESSAGES_TO_SCREEN, OnBnClickedLogMsgsToScreen)
	ON_BN_CLICKED(IDC_BTN_MAKE_CHANGES, OnBnClickedBtnMakeChanges)
 #ifdef SHOW_CHILD_DIALOG_ON_INVOKE
	ON_WM_SHOWWINDOW()
 #endif
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
COptionsDialog::OnInitDialog()
{
	m_bInitializing = true;

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if ( !pList ) {
		MessageBox( _T("Load Error") );
		return FALSE;
	}

	__super::OnInitDialog();

	m_pCaseOptionsList->ClearHighlighted();

	SetPortDisplay( true );

	if ( !m_bIsDockedTab ) {
		InitializeWindowSize();
	}

	if ( !InitializeCaseList() ) {
		m_bInitializing = false;
		return false;
	}

	if ( m_bIsDockedTab ) {
		m_Menu.LoadMenu(IDR_CASES_TAB_MENU);
	}
	else {
		m_Menu.LoadMenu(IDR_CASES_MENU);
	}

	SyncCaseCaptureOptionsToGlobal();
	SyncCasePortsToGlobal();
	SyncGeneralOptionsToGlobal();

	if ( !m_bIsDockedTab ) {
		bool bDisplayWarning = m_pOptions->GetDisplayWarning();
		m_BtnDisplayWarning.SetCheck( static_cast<BOOL>( bDisplayWarning ) );
	}

	CWnd* pWnd;

	if ( !m_bIsDockedTab ) {
		pWnd = GetDlgItem( IDC_BTN_EDIT_CASE );
		pWnd->EnableWindow( FALSE );
	}

	if ( m_bIsDockedTab ) {
		pWnd = GetDlgItem( IDC_BTN_VIEW_CASE );
		pWnd->EnableWindow( FALSE );
		
		ASSERT(pList);
		if ( pList->GetItemCount() > 0 ) {
			pList->SetFocus();
		}

		m_bInitializing = false;
		return FALSE;
	}

	BnClickedCaptureToFileAction();
	BnClickedCaptureToScreenAction();

	InitializeDuplicateFlag();

	if ( m_pCaseOptionsList->GetCount() > 0 ) {
		ASSERT(pList);
		if ( pList ) {
			if ( m_nCaseItem >= 0 ) {
				ASSERT( !m_bIsDockedTab );
				pList->SetItemState( m_nCaseItem, LVIS_SELECTED, LVIS_SELECTED );
			}
			pList->SetFocus();
		}
	}

	SetControlState();

	if ( !m_bIsDockedTab && m_nCaseItem == -2 ) {
		CWnd* pWnd = GetDlgItem( IDC_PORTS );
		ASSERT( pWnd );
		if ( pWnd ) {
			pWnd->SetFocus();
		}
	}

	m_nCaseItem = -1;

	if ( m_bIsCapturing && !m_bIsDockedTab) {
		CButton* pEditPorts = (CButton*)GetDlgItem( IDC_PORTS );
		if ( pEditPorts ) {
			pEditPorts->SetWindowText( _T("View Ports") );
		}
	}

	m_bInitializing = false;

	return FALSE;
}

// -----------------------------------------------------------------------------
//		AddCase
// -----------------------------------------------------------------------------

void
COptionsDialog::AddCase( CCaseOptions* theCaseOptions )
{
	if ( m_bIsDockedTab ) {
		if ( !theCaseOptions->IsEnabled() || !theCaseOptions->CanCaseBeEnabled() )	return;
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
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
COptionsDialog::SetCaseData( CCaseOptions* theCaseOptions, int nIndex )
{
	//	USES_CONVERSION;

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	pList->SetItemText( nIndex, kColumn_Name, theCaseOptions->GetName() );
  #ifdef CAPTURE_ALL
	if (theCaseOptions->CaptureAll()) {
		pList->SetItemText( nIndex, kColumn_UserNames, _T("All") );
	}
	else
  #endif
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

		{
			CString strOutputDir = theCaseOptions->GetOutputDirectory();
			pList->SetItemText( nIndex, kColumn_OutputDir, strOutputDir );
		}
		{
			CCaptureOptions& theCaptureOptions = theCaseOptions->GetCaptureOptions();

			CString strValue;
			strValue = (theCaptureOptions.IsCaptureToFile()) ? _T("Yes") : _T("No");
			pList->SetItemText( nIndex, kColumn_ToFile, strValue );
			strValue = (theCaptureOptions.IsCaptureToFile() && theCaptureOptions.IsLogMsgsToFile()) ? _T("Yes") : _T("No");
			pList->SetItemText( nIndex, kColumn_LogEventsToFile, strValue );

			strValue = (theCaptureOptions.IsCaptureToScreen()) ? _T("Yes") : _T("No");
			pList->SetItemText( nIndex, kColumn_ToScreen, strValue );

			strValue = (theCaptureOptions.IsLogMsgsToScreen()) ? _T("Yes") : _T("No");
			pList->SetItemText( nIndex, kColumn_LogEventsToScreen, strValue );
		}

	  #ifdef MODE_OPTIONS
		if ( theCaseOptions->IsFullCapture() )
			pList->SetItemText( nIndex, kColumn_Mode, _T("Full") );
		else
			pList->SetItemText( nIndex, kColumn_Mode, _T("Pen") );
	  #endif

		{
			bool	bSetActive = false;

			if ( theCaseOptions->IsEnabled() ) {
				if ( !m_bIsDockedTab ) {
					if ( theCaseOptions->CanCaseBeEnabled() ) {
						pList->SetCheck( nIndex );
						bSetActive = true;
					}
					else {
						theCaseOptions->SetDisabled();
					}
				}
			}
		}
}

// -----------------------------------------------------------------------------
//		OnSize
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnSize(
	UINT nType, 
	int cx, 
	int cy ) 
{
	__super::OnSize(nType, cx, cy);

	CRect theWindowRect;
	GetWindowRect( &theWindowRect );

	int nWidth = theWindowRect.Width();
	int nHeight= theWindowRect.Height();

	if ( !m_bIsDockedTab ) {
		if ( nWidth < DIALOG_MIN_WIDTH || nHeight < DIALOG_MIN_HEIGHT_MODAL ) {
			nWidth = (nWidth < DIALOG_MIN_WIDTH) ? DIALOG_MIN_WIDTH : nWidth;
			nHeight = (nHeight < DIALOG_MIN_HEIGHT_MODAL) ? DIALOG_MIN_HEIGHT_MODAL : nHeight;
			
			SIZE BtnRightOffset;
			BtnRightOffset.cx = nWidth;
			BtnRightOffset.cy = nHeight;

			CRect newRect( theWindowRect.TopLeft(), theWindowRect.TopLeft() + BtnRightOffset );

			MoveWindow( newRect );
		}
	}

	CRect theClientRect;
	GetClientRect( &theClientRect );

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );

	if ( m_bIsDockedTab ) {
		if (pList) {
			CRect theListRect = theClientRect;

			theListRect.top += 145;
			theListRect.left += 19;
			theListRect.right = cx - theListRect.left - 30;
			theListRect.bottom = cy - theListRect.top - 50;

			pList->MoveWindow( &theListRect );

			CWnd* pWnd = GetDlgItem( IDC_BTN_VIEW_CASE );
			if ( pWnd ) {
				if ( theListRect.Height() < 43 || theListRect.Width() < 305 ) {
					pWnd->ShowWindow( SW_HIDE );
				}
				else {
					pWnd->ShowWindow( SW_SHOW );
					
					int nHeightAdj = theListRect.bottom + 30;
					MoveRadiusButton( IDC_BTN_VIEW_CASE, nHeightAdj );
				}
			}
		}
	}
	else {
		if (pList) {
			pList->MoveWindow( 24, 150, theClientRect.Width() - 59, theClientRect.Height() - 249 );
		}

		int nHeightAdj = theClientRect.Height() - 86;

		MoveRadiusButton( IDC_BTN_ADD_CASE, nHeightAdj );
		if ( !m_bIsDockedTab ) {
			MoveRadiusButton( IDC_BTN_EDIT_CASE, nHeightAdj );
		}
		MoveRadiusButton( IDC_BTN_COPY_CASE, nHeightAdj );
		MoveRadiusButton( IDC_BTN_DELETE_CASE, nHeightAdj );
		MoveRadiusButton( IDC_BTN_IMPORT_CASE, nHeightAdj );
		MoveRadiusButton( IDC_BTN_EXPORT_CASE, nHeightAdj );
		MoveRadiusButton( IDC_CHK_DISPLAY_WARNING, nHeightAdj );
		MoveOKCancelButtons( theClientRect.Height() - 47,  theClientRect.Width() / 2 );
	}
}


// -----------------------------------------------------------------------------
//  MoveRadiusButton
// -----------------------------------------------------------------------------

void
COptionsDialog::MoveRadiusButton( int nId, int nTop )
{
	CRect theButtonRect;
	CWnd* pButton;
	pButton = GetDlgItem( nId );

	if ( !pButton ) return;

	pButton->GetWindowRect( &theButtonRect );
	ScreenToClient( theButtonRect );
	pButton->MoveWindow( theButtonRect.left, nTop, theButtonRect.Width(), theButtonRect.Height()  );
}

// -----------------------------------------------------------------------------
//  MoveOKCancelButtons
// -----------------------------------------------------------------------------

void
COptionsDialog::MoveOKCancelButtons( int nTop, int nMiddle )
{
	CRect theOKButtonRect;
	CRect theCancelButtonRect;
	CWnd* pOKButton;
	CWnd* pCancelButton;

	pOKButton = GetDlgItem( IDOK );
	pCancelButton = GetDlgItem( IDCANCEL );
	if ( !pOKButton || !pCancelButton ) return;

	pOKButton->GetWindowRect( &theOKButtonRect );
	pCancelButton->GetWindowRect( &theCancelButtonRect );
	ScreenToClient( theOKButtonRect );
	ScreenToClient( theCancelButtonRect );

	int nOKButtonAdjust = (nMiddle - 35) - (theOKButtonRect.Width() / 2);
	int nOKCancelAdjust = nMiddle + 35;
	pOKButton->MoveWindow( nOKButtonAdjust, nTop, theOKButtonRect.Width(), theOKButtonRect.Height()  );
	pCancelButton->MoveWindow( nOKCancelAdjust, nTop, theCancelButtonRect.Width(), theCancelButtonRect.Height()  );
}


// -----------------------------------------------------------------------------
//		OnBnClickedImport
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedImport()
{
	CaptureOptionsToData();
	SyncCaseCaptureOptionsToGlobal();

	COptions SaveOptions;
	SaveOptions = *m_pOptions;

	COptions UseOptions;
	UseOptions = *m_pOptions;

	bool bSaveDisplayWarning = m_pOptions->GetDisplayWarning();
	bool bSupportsCaptureOptions = m_pOptions->IsSupportsCaptureOptions();

	try 
	{
		COptions ImportOptions;

		CImportOptionsDlg theDialog( m_bIsCapturing, ImportOptions, UseOptions, m_pPlugin );

		INT_PTR nResult = theDialog.DoModal();
		if (nResult == IDOK) {

			bool bImportEntire = theDialog.IsImportEntire();

			if ( bImportEntire ) {
				if ( *m_pOptions != ImportOptions ) {
					if ( m_pOptions->GetCaseOptionsList().GetCount() > 0 ) {
						LPCTSTR pMessage = _T( "All cases in the current list will be deleted.  Do you want to proceed with the import?" );
						LPCTSTR pCaption = _T( "Import Options File" );
						int nResponse = ::MessageBox( this->m_hWnd, pMessage, pCaption, MB_YESNO );
						if ( nResponse != IDYES ) {
							return;
						}
					}
					*m_pOptions = ImportOptions;
				}
			}
			else {
				CCaseOptionsList& theImportCaseOptionsList = ImportOptions.GetCaseOptionsList();
				int nResponse = IMPORT_RESPONSE_CANCEL;

				if ( theImportCaseOptionsList.GetCount() > 0 ) {

					for ( size_t nItem = 0; nItem < theImportCaseOptionsList.GetCount(); nItem++ ) {
						CCaseOptions* pImportCaseOptions = theImportCaseOptionsList.GetAt( nItem );
						pImportCaseOptions->SetEnabled();
						pImportCaseOptions->SetHighlighted( true );

						CPeekString strImportCaseName = pImportCaseOptions->GetName();
						if ( m_pCaseOptionsList->IsNameMatch( strImportCaseName ) ) {

							if ( (nResponse & IMPORT_RESPONSE_ALWAYS) != IMPORT_RESPONSE_ALWAYS ) {
								nResponse = CallReplaceCaseDlg( strImportCaseName );
								if ( nResponse == IMPORT_RESPONSE_CANCEL ) AtlThrow( E_FAIL );
							}
							if ( (nResponse & IMPORT_RESPONSE_NO) == IMPORT_RESPONSE_NO ) continue;
							ASSERT( (nResponse & IMPORT_RESPONSE_YES) == IMPORT_RESPONSE_YES );

							CCaseOptions* pEditCaseOptions = m_pCaseOptionsList->Lookup( strImportCaseName );
							ASSERT( pEditCaseOptions );
							if ( !pEditCaseOptions ) AtlThrow( E_FAIL );
							CCaseOptions::ImportCaseItems( pEditCaseOptions, pImportCaseOptions,
															m_pOptions->GetCaptureOptions(), m_bIsCapturing );
						}
						else {
							CCaseOptions* pNewCaseOption = new CCaseOptions;
							ASSERT( pNewCaseOption );
							if ( !pNewCaseOption ) AtlThrow( E_FAIL );
							*pNewCaseOption = *pImportCaseOptions;
							m_pCaseOptionsList->Add( pNewCaseOption );
						}
					}
				}
				else {
					return;
				}
			}
		}
		else {
		}
	}
	catch(...) 
	{
		MessageBox( _T("Import failed") );
		*m_pOptions = SaveOptions;
	}

	m_pOptions->SetDisplayWarning( bSaveDisplayWarning );
	m_pOptions->SetSupportsCaptureOptions( bSupportsCaptureOptions );

	RefreshOptionsDisplay();

}

// -----------------------------------------------------------------------------
//		RefreshOptionsDisplay
// -----------------------------------------------------------------------------

void 
COptionsDialog::RefreshOptionsDisplay( COptions* inOptions )
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bInitializing = true;

	try 
	{
		if ( inOptions != NULL ) {
			m_pOptions = inOptions;
			m_pCaseOptionsList = &(m_pOptions->GetCaseOptionsList());
		}
		
		EmptyCaseOptionsList();

		UpdateData( FALSE );

		for( UInt32 i = 0; i < m_pCaseOptionsList->GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( i );
			AddCase( theCaseOptions );
		}

		m_pCaseOptionsList->SyncPortsToGlobal( m_pOptions->GetPortNumberList() );
		m_pCaseOptionsList->SyncCaptureOptionsToGlobal( m_pOptions->GetCaptureOptions() );
		m_pCaseOptionsList->SyncGeneralOptionsToGlobal( m_pOptions->GetOutputRootDirectory(), m_pOptions->IsUseFileNumber() );
		m_pCaseOptionsList->DisableInvalidCases();

		SetPortDisplay( false );
		SortAndDisplayList();
	}
	catch(...)
	{

	}

	SetControlState();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if ( pList->GetItemCount() > 0 ) {
		pList->SetFocus();
	}

	m_bInitializing = false;
}

// -----------------------------------------------------------------------------
//		OnBnClickedExport
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedExport()
{
	if ( m_bIsDockedTab ) return;

	CaptureOptionsToData();
	SyncCaseCaptureOptionsToGlobal();

	COptions ExportOptions( *m_pOptions );

	CExportOptionsDlg theDialog( m_bIsCapturing, ExportOptions, m_pPlugin );
	theDialog.DoModal();
}

// -----------------------------------------------------------------------------
//		OnBnClickedAdd
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedAdd()
{
	if ( m_bIsDockedTab ) return;

	CPeekString theName;
	for (int i=1; i < 1000; i++)
	{
		CPeekOutString strOutCase;
		strOutCase << L"Case " << i;
		theName = strOutCase;
		if ( m_pCaseOptionsList->IsNameMatch( theName ) == FALSE )
			break;
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	int nOldItem = -1;
	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos != NULL) {
		nOldItem = pList->GetNextSelectedItem(pos);
	}

	CCaseOptions* theCaseOptions = new CCaseOptions;
	if ( !theCaseOptions ) {
		ASSERT( 0 );
		return;
	}
//	theCaseOptions->Init( theName, m_pOptions->IsUseCaseCaptureId(), m_pOptions->IsUseFileNumber() );
	theCaseOptions->Init( theName, m_pOptions->IsUseFileNumber() );

	theCaseOptions->SetPrefix( theName );

	CPeekString strOutputRootDir;
	if ( !ValidateRootDirectory( strOutputRootDir ) ) {
		return;
	}

	theCaseOptions->SyncPortsToGlobal( m_pOptions->GetPortNumberList() );
	CaptureOptionsToData();
	theCaseOptions->SyncCaptureOptionsToGlobal( m_pOptions->GetCaptureOptions() );
	theCaseOptions->SyncGeneralOptionsToGlobal( m_pOptions->GetOutputRootDirectory(), m_pOptions->IsUseFileNumber() );

	CCaseOptionsDlg theDialog( theCaseOptions, m_pOptions, false );

	INT_PTR nResult = theDialog.DoModal();
	if (nResult == IDOK) {
		bool bEnableCase = theCaseOptions->CanCaseBeEnabled();
		if ( bEnableCase ) {
			theCaseOptions->SetEnabled();
		}

		m_pCaseOptionsList->Add( theCaseOptions );

		SortAndDisplayList( theCaseOptions->GetName() );
	}
	else {
		delete theCaseOptions;
	}

	pList->SetFocus();
}

// -----------------------------------------------------------------------------
//		OnBnClickedEdit
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedEdit()
{
	if ( !m_bIsDockedTab ) {
		CaptureOptionsToData();
		SyncCaseCaptureOptionsToGlobal();
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int nItem = pList->GetNextSelectedItem(pos);
	CString theName = pList->GetItemText( nItem, 0 );
	
	if ( m_bIsDockedTab ) {
		CPeekString	strName( theName );
		const int nCount( static_cast<int>(m_pCaseOptionsList->GetCount()) );
		int i( 0 );
		for ( ; i < nCount; i++ ) {
			CCaseOptions* theCase = m_pCaseOptionsList->GetAt(i);
			if ( theCase->GetName() == strName ) {
				nItem = i;
				break;
			}
		}
		if ( i >= nCount ) {
			ASSERT( 0 );
			return;
		}
	}
	CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( nItem );

	CPeekString strOutputRootDir;
	if ( !ValidateRootDirectory( strOutputRootDir ) ) {
		return;
	}

	CCaseOptions theCaseOptionsEdit;
	theCaseOptionsEdit = *theCaseOptions;

	bool bDuplicate = theCaseOptionsEdit.GetDuplicate();
	bool bDisableNameEdit = (m_bIsCapturing & !bDuplicate) ? true : false;

	if ( m_bIsDockedTab ) {
		CCaseViewDlg theDialog( &theCaseOptionsEdit, m_pOptions, bDisableNameEdit, nItem );
		theDialog.DoModal();		
	}
	else {
		CCaseOptionsDlg theDialog( &theCaseOptionsEdit, m_pOptions, bDisableNameEdit, m_bIsDockedTab, nItem );
		INT_PTR nResult = theDialog.DoModal();
		if (nResult == IDOK) {
			if ( theCaseOptionsEdit.IsEnabled() && !theCaseOptionsEdit.CanCaseBeEnabled() ) {
				theCaseOptionsEdit.SetDisabled();
			}
			if ( *theCaseOptions != theCaseOptionsEdit ) {
				theCaseOptions->Copy( theCaseOptionsEdit );
			}
			SortAndDisplayList( theCaseOptionsEdit.GetName() );
		}
	}

	pList->SetFocus();
}


// -----------------------------------------------------------------------------
//		OnBnClickedDelete
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedDelete()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bInitializing || m_bIsDockedTab ) return;

	BnClickedDeleteAction();
}


// -----------------------------------------------------------------------------
//		OnBnClickedDuplicate
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedDuplicate()
{
	if ( m_bIsDockedTab ) return;

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int nItem = pList->GetNextSelectedItem(pos);
	CString theName = pList->GetItemText( nItem, 0 );

	CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( nItem );
	CCaseOptions* theCaseOptionsCopy = new CCaseOptions;
	if ( !theCaseOptionsCopy ) {
		ASSERT( 0 );
		return;
	}
	*theCaseOptionsCopy = *theCaseOptions;
	theCaseOptionsCopy->SetDuplicate( true );
	theCaseOptionsCopy->SetCaseCaptureId( 0 );

	CPeekString theNewName = m_pCaseOptionsList->GetUniqueName( (CPeekString)theName );
	theCaseOptionsCopy->Init( theNewName, m_pOptions->IsUseFileNumber() );

	CPeekString thePrefix = theCaseOptionsCopy->GetPrefix();
	CPeekString theNewPrefix = m_pCaseOptionsList->GetUniquePrefix( thePrefix );
	theCaseOptionsCopy->SetPrefix( theNewPrefix );

	theCaseOptionsCopy->SetOutputDirectory( theCaseOptions->GetOutputDirectory() );

	m_pCaseOptionsList->Add( theCaseOptionsCopy );

	SortAndDisplayList( theCaseOptionsCopy->GetName() );

	pList->SetFocus();
}

// -----------------------------------------------------------------------------
//		EnableAll
// -----------------------------------------------------------------------------

void 
COptionsDialog::EnableAll( )
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	int nCount = pList->GetItemCount();	
	BOOL bChecked = FALSE;
	bool bWarn = false;
	for (int i=0; i < nCount; i++)
	{
		bChecked = pList->GetCheck( i );
		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( static_cast<size_t>(i) );
		if ( theCaseOptions->CanCaseBeEnabled() ) {
			pList->SetCheck( i, TRUE );
		}
		else {
			bWarn = true;
		}
	}

	if ( bWarn ) {
		AfxMessageBox( _T("One or more cases could not be enabled.  You must select each case to discover what options are insufficient.") );
	}
}


// -----------------------------------------------------------------------------
//		DisableAll
// -----------------------------------------------------------------------------

void 
COptionsDialog::DisableAll( )
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	int nCount = pList->GetItemCount();	
	BOOL bChecked = FALSE;
	for (int i=0; i < nCount; i++)
	{
		bChecked = pList->GetCheck( i );
		if (bChecked == TRUE) {
			pList->SetCheck( i, FALSE );
		}
	}
}


// -----------------------------------------------------------------------------
//  OnNMDblclkList
// -----------------------------------------------------------------------------

void
COptionsDialog::OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ( m_bInitializing ) return;

	pNMHDR;
		
	OnBnClickedEdit();

	*pResult = 0;
}


// -----------------------------------------------------------------------------
//  DeleteAllCases
// -----------------------------------------------------------------------------

void
COptionsDialog::DeleteAllCases()
{
	if ( m_bInitializing || m_bIsDockedTab ) return;

	m_bInitializing = true;

	m_pCaseOptionsList->Reset();
	EmptyCaseOptionsList();

	m_bInitializing = false;
}


// Returns TRUE if the item is checked, false if it is not or uState == 0
#define IsChecked(x) (((x & LVIS_STATEIMAGEMASK) >> 12) - 1)

// -----------------------------------------------------------------------------
//  OnLvnItemchangedList
// -----------------------------------------------------------------------------

void
COptionsDialog::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT *pResult)
{
	if ( m_bInitializing ) return;

	m_bInitializing = true;

	if ( !m_bIsDockedTab ) {
		CaptureOptionsToData();
		SyncCaseCaptureOptionsToGlobal();
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) {
		m_bInitializing = false;
		return;
	}

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	bool bWasCheckeded = IsChecked(pNMListView->uOldState) > 0 ? true : false;
	bool bIsChecked = IsChecked(pNMListView->uNewState) > 0 ? true : false;
	bool bCaseHasChanged = bWasCheckeded != bIsChecked ? true: false;

	if ( bCaseHasChanged ) {

		int nItem = pNMListView->iItem;

		if ( m_bIsDockedTab ) {
			BOOL bState = bWasCheckeded ? TRUE : FALSE;
			pList->SetCheck( nItem, bState );

			SetEditControlsSelected();
			*pResult = 0;
			m_bInitializing = false;
			return;
		}

		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( nItem );

		if ( bIsChecked ) {
			int nReason;
			if ( !theCaseOptions->CanCaseBeEnabled( nReason ) ) {
				CString msg;
				switch ( nReason ) {
					case theCaseOptions->kCaseReason_TimeExpired:
						msg = "The case cannot be activated because the trigger time is in the past.";
						break;
					case theCaseOptions->kCaseReason_NoPorts:
						msg = "The case cannot be activated because there are no enabled port numbers.";
						break;
					case theCaseOptions->kCaseReason_NoUsers:
						msg = "The case cannot be activated because there are no enabled user names.";
						break;
					case theCaseOptions->kCaseReason_GeneralOpts:
						msg = "The case cannot be activated due to insufficient General Case Options.";
						break;
					case theCaseOptions->kCaseReason_CaptureOpts:
						msg = "The case cannot be activated due to insufficient Case Capture Options.";
						break;
					default:
						ASSERT( 0 );
						msg = "The case cannot be activated.";
						break;
				}
				AfxMessageBox( msg );
				ASSERT( theCaseOptions->IsEnabled() == false );
				pList->SetCheck( nItem, FALSE );
				m_bInitializing = false;
				return;
			}
		}

		theCaseOptions->SetEnabledState( (bIsChecked == TRUE) );
	}

	SetEditControlsSelected();

	m_bInitializing = false;


	// return FALSE (or 0) to allow change, TRUE to prevent
	*pResult = 0;
}

// -----------------------------------------------------------------------------
//  OnLvnDeleteitemList
// -----------------------------------------------------------------------------

void
COptionsDialog::OnLvnDeleteitemList(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bInitializing || m_bIsDockedTab ) return;

	BnClickedDeleteAction();
	*pResult = 0;
}

// -----------------------------------------------------------------------------
//  nNMRClickList
// -----------------------------------------------------------------------------

void COptionsDialog::OnNMRClickList( NMHDR* /* pNMHDR */, LRESULT *pResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bInitializing ) return;

	CPoint point;
	GetCursorPos(&point);

	CMenu mnuTop;
	if ( m_bIsDockedTab ) {
		mnuTop.LoadMenu( IDR_CASES_TAB_MENU );
	}
	else {
		mnuTop.LoadMenu( IDR_CASES_MENU );
	}

	CMenu* pPopup = mnuTop.GetSubMenu( 0 );
	ASSERT_VALID( pPopup );
	if ( !pPopup ) return;

	SetMenuItemsSelected( pPopup );

	pPopup->TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON |
		TPM_LEFTALIGN, point.x, point.y, this, NULL);

	*pResult = 0;
}


// ----------------------------------------------------------------------------
//		OnUpdateMenuAdd
// ----------------------------------------------------------------------------

void
COptionsDialog::OnUpdateMenuAdd( CCmdUI* pCmdUI )
{
	if ( m_bIsDockedTab ) return;

	OnBnClickedAdd();
	pCmdUI->Enable( TRUE );
}

// -----------------------------------------------------------------------------
//  SortAndDisplayList
// -----------------------------------------------------------------------------

void
COptionsDialog::SortAndDisplayList( CPeekString strName )
{
	bool bPriorState = m_bInitializing;

	m_bInitializing = true;

	m_pCaseOptionsList->Sort();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) {
		m_bInitializing = bPriorState;
		return;
	}
	pList->DeleteAllItems();
	ASSERT( pList->GetItemCount() == 0 );

	bool bItemWasHighlighted = false;
	for( UInt32 i = 0; i < m_pCaseOptionsList->GetCount(); i++ ) {
		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( i );
		AddCase( theCaseOptions );
		if ( (strName != L"") && (strName == theCaseOptions->GetName()) ) {
			theCaseOptions->SetHighlighted( true );
		}
		if ( theCaseOptions->IsHighlighted() ) {
			pList->SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
			bItemWasHighlighted = true;
		}
		theCaseOptions->SetHighlighted( false );
	}

	m_bInitializing = bPriorState;
}

// -----------------------------------------------------------------------------
//  SetControlsSelected
// -----------------------------------------------------------------------------

void
COptionsDialog::SetEditControlsSelected()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	BOOL bSelected = static_cast<BOOL>( pos != NULL );

	if ( !m_bIsDockedTab ) {
		CWnd* pEdit = GetDlgItem( IDC_BTN_EDIT_CASE );
		pEdit->EnableWindow( bSelected );
	}

	if ( m_bIsDockedTab ) {
		CWnd* pView = GetDlgItem( IDC_BTN_VIEW_CASE );
		pView->EnableWindow( bSelected );
	}

	if ( m_bIsDockedTab ) {
		return;
	}

	CWnd* pCopy = GetDlgItem( IDC_BTN_COPY_CASE );
	CWnd* pDelete = GetDlgItem( IDC_BTN_DELETE_CASE );
	pCopy->EnableWindow( bSelected );
	pDelete->EnableWindow( bSelected );
}

// -----------------------------------------------------------------------------
//  SetMenuItemsSelected
// -----------------------------------------------------------------------------

void
COptionsDialog::SetMenuItemsSelected( CMenu* pPopup )
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	bool bSelected = (pos != NULL);

	if ( bSelected ) {
		pPopup->EnableMenuItem( ID_MENU_EDIT, MF_BYCOMMAND | MF_ENABLED );
		pPopup->EnableMenuItem( ID_MENU_DUPLICATE, MF_BYCOMMAND | MF_ENABLED );
		pPopup->EnableMenuItem( ID_MENU_DELETE, MF_BYCOMMAND | MF_ENABLED );
	}
	else {
		pPopup->EnableMenuItem( ID_MENU_EDIT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pPopup->EnableMenuItem( ID_MENU_DUPLICATE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pPopup->EnableMenuItem( ID_MENU_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}

	bool bAreCases = ( m_pCaseOptionsList->GetCount() > 0 );
	if ( bAreCases ){
		pPopup->EnableMenuItem( ID_MENU_ENABLEALL, MF_BYCOMMAND | MF_ENABLED );
		pPopup->EnableMenuItem( ID_MENU_DISABLEALL, MF_BYCOMMAND | MF_ENABLED );
		pPopup->EnableMenuItem( ID_MENU_DELETEALL, MF_BYCOMMAND | MF_ENABLED );
	}
	else {
		pPopup->EnableMenuItem( ID_MENU_ENABLEALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pPopup->EnableMenuItem( ID_MENU_DISABLEALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pPopup->EnableMenuItem( ID_MENU_DELETEALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
}

// -----------------------------------------------------------------------------
//		OnUpdateMenuEdit
// -----------------------------------------------------------------------------

void
COptionsDialog::OnUpdateMenuEdit( CCmdUI *pCmdUI )
{
	if ( m_bInitializing ) return;

	OnBnClickedEdit();
	pCmdUI->Enable( TRUE );
}

// -----------------------------------------------------------------------------
//		OnUpdateMenuView
// -----------------------------------------------------------------------------

void
COptionsDialog::OnUpdateMenuView( CCmdUI *pCmdUI )
{
	if ( m_bInitializing ) return;

	ASSERT( m_bIsDockedTab );
	if ( !m_bIsDockedTab ) return;

	OnBnClickedView();
	pCmdUI->Enable( TRUE );
}

// -----------------------------------------------------------------------------
//		OnUpdateMenuDuplicate
// -----------------------------------------------------------------------------

void
COptionsDialog::OnUpdateMenuDuplicate( CCmdUI *pCmdUI )
{
	if ( m_bInitializing || m_bIsDockedTab ) return;

	OnBnClickedDuplicate();
	pCmdUI->Enable( TRUE );
}

// -----------------------------------------------------------------------------
//		OnUpdateMenuDelete
// -----------------------------------------------------------------------------

void
COptionsDialog::OnUpdateMenuDelete( CCmdUI *pCmdUI )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bInitializing || m_bIsDockedTab ) return;

	BnClickedDeleteAction();
	pCmdUI->Enable( TRUE );
}

// -----------------------------------------------------------------------------
//		OnBnClickedPorts
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedPorts()
{
	bool bDisableControls = false;

	if ( m_bIsCapturing || m_bIsDockedTab ) bDisableControls = true;

	CPortOptionsDlg theDialog( m_pOptions, bDisableControls );

	INT_PTR nResult = theDialog.DoModal();
	if (nResult == IDOK) {
		CPortOptions thePortOptions = theDialog.GetPortOptions();
		if ( thePortOptions != m_pOptions->GetPortOptions() ) {
			m_pOptions->SetPortOptions( thePortOptions );
			PortListToControl();
			m_pCaseOptionsList->SyncPortsToGlobal( m_pOptions->GetPortNumberList() );
		}
	}
}

// -----------------------------------------------------------------------------
//		OnBnClickedBrowse
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedBrowse()
{
	ASSERT( !m_bIsCapturing );

	if ( m_bIsDockedTab ) return;

	TCHAR	szTempLocation[512];
	_tcscpy_s( szTempLocation, (LPCTSTR)m_pOptions->GetOutputRootDirectory() );

	BROWSEINFO bInfo;
	::ZeroMemory( (PVOID) &bInfo, sizeof( BROWSEINFO ) );

	bInfo.hwndOwner = m_hWnd;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = NULL;
	bInfo.lpszTitle = _T("Choose Output Directory:");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bInfo.lpfn = NULL;

	LPITEMIDLIST pidlPath = ::SHBrowseForFolder( &bInfo );
	if ( pidlPath != NULL ) {
		if ( ::SHGetPathFromIDList( pidlPath, szTempLocation ) ) {
			m_pOptions->SetOutputRootDirectory( szTempLocation );
			SetDlgItemText( IDC_OUTPUT_ROOT_EDIT, m_pOptions->GetOutputRootDirectory() );
		}
	}

	LPMALLOC pMalloc = NULL;
	if ( SUCCEEDED( ::SHGetMalloc( &pMalloc ) ) ) {
		pMalloc->Free( pidlPath );
	}
}


// -----------------------------------------------------------------------------
//		OnBnClickedSuggestOutput
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedSuggestOutput()
{
	ASSERT( !m_bIsCapturing );

	if ( m_bIsDockedTab ) return;

	ASSERT( !m_bInitializing );

	m_bInitializing = true;

	CPeekString theOutputDir = kDefaultOutputDir;

	if ( theOutputDir.Right(1) == L"\\" ) {
		theOutputDir = theOutputDir.Left(theOutputDir.GetLength() - 1 );
	}

	m_pOptions->SetOutputRootDirectory( theOutputDir );

	SetDlgItemText( IDC_OUTPUT_ROOT_EDIT, theOutputDir );

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//		PortListToControl
// -----------------------------------------------------------------------------

void
COptionsDialog::PortListToControl()
{
	wchar_t	szPort[20];
	
	CPortNumberList& thePortNumberList = m_pOptions->GetPortNumberList();

	CListCtrl* pPortList = (CListCtrl*)GetDlgItem( IDC_LISTBOX_PORTS );		
	ASSERT( pPortList );
	if ( pPortList == NULL ) return;

	pPortList->DeleteAllItems();

	int nCount = 0;

	const int nNumPortOptions = static_cast<int>( thePortNumberList.GetCount() );
	for ( int i = 0; i < nNumPortOptions; i++ ) {
		CPortNumber* thePortNumber = thePortNumberList.GetAt( i );
		if ( thePortNumber->IsEnabled() ) {
			wsprintf( szPort, _T("%d"), thePortNumber->GetPort() );
			pPortList->InsertItem( nCount, static_cast<LPCTSTR>(szPort) );
			nCount++;
		}
	}
		
	CRect theRect;
	pPortList->GetWindowRect( &theRect );
	int nWindowHeight = theRect.bottom - theRect.top;

	if ( nCount == 0 ) {
		pPortList->InsertItem( nCount, static_cast<LPCTSTR>( _T("0") ) );
		nCount++;
	}

	RECT theItemRect;
	pPortList->GetItemRect( 0, &theItemRect, LVIR_BOUNDS  );
	const int nItemHeight = theItemRect.bottom - theItemRect.top;

	nWindowHeight = ( nCount * nItemHeight ) + 5;
	if ( nCount > 4 ) {
		nWindowHeight = ( nWindowHeight > 73 ) ? 73 : nWindowHeight;
		theRect.right = theRect.left + 65;
	}
	else {
		theRect.right = theRect.left + 45;
	}

	theRect.bottom = theRect.top + nWindowHeight;

	ScreenToClient( theRect );
	pPortList->MoveWindow( theRect.left, theRect.top, theRect.Width(), theRect.Height()  );
}

// -----------------------------------------------------------------------------
//		OnBnClickedFileAge
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedFileAge()
{
	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CAPTURE_TO_FILE );
	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();

	CButton* chkFileAge = (CButton*)GetDlgItem( IDC_FILE_AGE );
	CButton* chkSizeAge = (CButton*)GetDlgItem( IDC_FILE_SIZE );
	BOOL bFileAgeChecked = chkFileAge->GetCheck();
	BOOL bFileSizeChecked = chkSizeAge->GetCheck();

	CWnd* pFileAgeCount = GetDlgItem( IDC_FILE_AGE_COUNT );
	CWnd* pFileAgeUnits = GetDlgItem( IDC_FILE_AGE_UNITS );

	if ( !bFileAgeChecked && !bFileSizeChecked ) {
		chkSizeAge->SetCheck( TRUE );
		OnBnClickedFileSize();
	}

	if ( !bCaptureToFileChecked || m_bIsDockedTab ) {
		pFileAgeCount->EnableWindow( FALSE );
		pFileAgeUnits->EnableWindow( FALSE );
		return;
	}

	pFileAgeCount->EnableWindow( bFileAgeChecked );
	pFileAgeUnits->EnableWindow( bFileAgeChecked );
}

// -----------------------------------------------------------------------------
//		OnBnClickedFileSize
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedFileSize()
{
	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	CButton* chkFileAge = (CButton*)GetDlgItem( IDC_FILE_AGE );
	CButton* chkSizeAge = (CButton*)GetDlgItem( IDC_FILE_SIZE );

	BOOL bFileAgeChecked = chkFileAge->GetCheck();
	BOOL bFileSizeChecked = chkSizeAge->GetCheck();

	if ( !bFileAgeChecked && !bFileSizeChecked ) {
		chkFileAge->SetCheck( TRUE );
		OnBnClickedFileAge();
	}

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CAPTURE_TO_FILE );
	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();

	CWnd* pFileSizeCount = GetDlgItem( IDC_FILE_SIZE_COUNT );
	CWnd* pFileSizeUnits = GetDlgItem( IDC_FILE_SIZE_UNITS );

	if ( !bCaptureToFileChecked || m_bIsDockedTab ) {
		pFileSizeCount->EnableWindow( FALSE );
		pFileSizeUnits->EnableWindow( FALSE );
		return;
	}

	pFileSizeCount->EnableWindow( bFileSizeChecked );
	pFileSizeUnits->EnableWindow( bFileSizeChecked );
}

// -----------------------------------------------------------------------------
//		OnBnClickedOk
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedOk()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( !m_bIsDockedTab );

	CaptureOptionsToData();
	SyncCasePortsToGlobal();
	SyncCaseCaptureOptionsToGlobal();
	SyncGeneralOptionsToGlobal();

	CPeekString strOutputRootDir;
	if ( !ValidateRootDirectory( strOutputRootDir ) ) {
		return;
	}

	if ( !ValidateCaptureOptions() ) {
		return;
	}

	if ( !ValidatePort() ) {
		return;
	}

	m_bInitializing = true;

	InitializeDuplicateFlag();

	m_pCaseOptionsList->ClearHighlighted();

	OnOK();

	if ( !m_bIsDockedTab ) {
		m_pOptions->SetDisplayWarning( m_BtnDisplayWarning.GetCheck() == TRUE );
	}
}

// -----------------------------------------------------------------------------
//  OnCancel
// -----------------------------------------------------------------------------

void
COptionsDialog::OnCancel()
{
	m_bInitializing = true;
	m_pCaseOptionsList->ClearHighlighted();

	__super::OnCancel();
}


// -----------------------------------------------------------------------------
//  OnEnChangeOutputRootEdit
// -----------------------------------------------------------------------------

void
COptionsDialog::OnEnChangeOutputRootEdit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( !m_bIsCapturing );

	if ( m_bInitializing ) return;

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		CEdit* pControl = (CEdit*)GetDlgItem( IDC_OUTPUT_ROOT_EDIT );
		pControl->SetWindowText( m_pOptions->GetOutputRootDirectory() );
		m_bInitializing = false;
		return;
	}

	CString strOutputDir, strTemp;

	CEdit* pOutputDir = (CEdit*)GetDlgItem( IDC_OUTPUT_ROOT_EDIT );

	pOutputDir->GetWindowText( strOutputDir );

	if ( strOutputDir == m_pOptions->GetOutputRootDirectory() ) return;

	m_bInitializing = true;

	strTemp = strOutputDir;
	strOutputDir.TrimLeft();

	int nLength( static_cast<int>( strOutputDir.GetLength() ) );

	// Validate that this is a proper path
	for ( int i = 0; i < nLength; i++ ) {
		bool bError( false );
		wchar_t ch( strOutputDir[i] );
		if ( !FilePath::IsValidPathChar( ch ) ) bError = true;
		else if ( i == 1 && ch != L':' ) bError = true;
		else if ( i != 1 && ch == L':' ) bError = true;
		else if ( i == 2 && (ch != L'\\' && ch != L'/') ) bError = true;
		else if ( i > 2 && ( (ch == L'\\' || ch == L'/') && 
			(strOutputDir[i-1] == L'\\' || strOutputDir[i-1] == L'/') ) ) bError = true;

		if ( bError ) {
			pOutputDir->SetWindowText( m_pOptions->GetOutputRootDirectory() );
			pOutputDir->SetSel( nLength, nLength, FALSE );
			m_bInitializing = false;
			return;
		}
	}

	const int nMaxRootPathLen = MAX_PATH_LEN - MAX_NAME_LEN - 2;
	if ( nLength > nMaxRootPathLen ) { 
		strOutputDir = strOutputDir.Left( nMaxRootPathLen );
		nLength = strOutputDir.GetLength();
	}

	if ( strOutputDir != strTemp ) {
		pOutputDir->SetWindowText( strOutputDir );
		pOutputDir->SetSel( nLength, nLength, FALSE );
	}

	m_pOptions->SetOutputRootDirectory( (CPeekString)strOutputDir );

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//  OnBnClickedUseFileNumber
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedUseFileNumber()
{
	if ( m_bInitializing ) return;

	CButton* pChkUseFileNumber = (CButton*)GetDlgItem( IDC_CHK_USE_FILE_NUMBER );
	ASSERT( pChkUseFileNumber );
	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		pChkUseFileNumber->SetCheck( m_pOptions->IsUseFileNumber() );
		m_bInitializing = false;
		return;
	}

	ASSERT( !m_bIsCapturing );

	BOOL bUseFileNumber = pChkUseFileNumber->GetCheck();
	m_pOptions->SetUseFileNumber( bUseFileNumber == TRUE );
}


// -----------------------------------------------------------------------------
//  InitializePortDisplay
// -----------------------------------------------------------------------------

void
COptionsDialog::SetPortDisplay( bool bInit )
{
	CPortNumberList& thePortNumberList = m_pOptions->GetPortNumberList();

	if ( thePortNumberList.GetCount() == 0 ) {
		thePortNumberList.Initialize();
	}

	// Configure port list display
	CListCtrl* pPortList = (CListCtrl*)GetDlgItem( IDC_LISTBOX_PORTS );

	if ( bInit ) {
		CRect theRect;
		pPortList->GetWindowRect( &theRect );
		theRect.top -= 1;
		ScreenToClient( theRect );
		pPortList->MoveWindow( theRect.left, theRect.top, theRect.Width(), theRect.Height()  );
		pPortList->InsertColumn( 0, _T("") );
		pPortList->SetColumnWidth( 0, 40 );
	}

	int ShadeOfGray = 235;
	COLORREF BkColor = RGB(ShadeOfGray, ShadeOfGray, ShadeOfGray);

	pPortList->SetBkColor( BkColor );
	pPortList->SetTextBkColor( BkColor );

	PortListToControl();
}

// -----------------------------------------------------------------------------
//  InitializeCaseList
// -----------------------------------------------------------------------------

bool
COptionsDialog::InitializeCaseList()
{
	size_t nCount = m_pCaseOptionsList->GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( i );
		theCaseOptions->SetDuplicate( false );
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) {
		return FALSE;
	}

	pList->ModifyStyle( 0, WS_CLIPCHILDREN );

	pList->InsertColumn(   kColumn_Name, _T("Case Identifier") );
	pList->SetColumnWidth( kColumn_Name, 120 );
	pList->InsertColumn(   kColumn_UserNames, _T("User Names") );
	pList->SetColumnWidth( kColumn_UserNames, 155 );
  #ifdef MODE_OPTIONS
	pList->InsertColumn(   kColumn_Mode, _T("Mode") );
	pList->SetColumnWidth( kColumn_Mode, 50 );
  #endif
	pList->InsertColumn(   kColumn_Start, _T("Start Time") );
	pList->SetColumnWidth( kColumn_Start, 257 );
	pList->InsertColumn(   kColumn_Stop, _T("Stop Time") );
	pList->SetColumnWidth( kColumn_Stop, 257 );
	pList->InsertColumn(   kColumn_OutputDir, _T("Output Directory") );
	pList->SetColumnWidth( kColumn_OutputDir, 200 );
	pList->InsertColumn(   kColumn_ToScreen, _T("Cap Scrn") );
	pList->SetColumnWidth( kColumn_ToScreen, 55 );
	pList->InsertColumn(   kColumn_LogEventsToScreen, _T("Log Scrn") );
	pList->SetColumnWidth( kColumn_LogEventsToScreen, 53 );
	pList->InsertColumn(   kColumn_ToFile, _T("Cap File") );
	pList->SetColumnWidth( kColumn_ToFile, 50 );
	pList->InsertColumn(   kColumn_LogEventsToFile, _T("Log File") );
	pList->SetColumnWidth( kColumn_LogEventsToFile, 48 );

	if ( m_bIsDockedTab ) {
		pList->SetExtendedStyle( pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		pList->ModifyStyle( 0, WS_CLIPCHILDREN | LVS_SHOWSELALWAYS );
	}
	else {
		pList->SetExtendedStyle( pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES );
		pList->ModifyStyle( 0, WS_CLIPCHILDREN | LVS_SHOWSELALWAYS );
	}

	for( UInt32 i = 0; i < m_pCaseOptionsList->GetCount(); i++ ) {
		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( i );
		AddCase( theCaseOptions );
	}

	return true;
}

// -----------------------------------------------------------------------------
//  SetControlState
// -----------------------------------------------------------------------------

void
COptionsDialog::SetControlState()
{
	CWnd* pWnd;

	if ( m_bIsDockedTab ) return;

	if ( m_bIsCapturing ) {
		pWnd = GetDlgItem( IDC_CAPTURE_TO_FILE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CAPTURE_TO_SCREEN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_LOG_MESSAGES_TO_SCREEN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_LOG_MESSAGES_TO_FILE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_FOLDERS_FOR_CASES );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_OUTPUT_ROOT_LBL );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_OUTPUT_ROOT_EDIT );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_BROWSE_BTN );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_BTN_SUGGEST_OUTPUT );
		pWnd->EnableWindow( FALSE );
		pWnd->EnableWindow( FALSE );
		pWnd = GetDlgItem( IDC_CHK_USE_FILE_NUMBER );
		pWnd->EnableWindow( FALSE );
	}

	BnClickedCaptureToFileAction();
	BnClickedCaptureToScreenAction();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	BOOL bSetItemSelected = (pList->GetItemCount() > 0) ? TRUE : FALSE;
	if ( bSetItemSelected ) {
		pList->SetFocus();
	}

	pWnd = GetDlgItem( IDC_BTN_EDIT_CASE );
	pWnd->EnableWindow( bSetItemSelected );
	pWnd = GetDlgItem( IDC_BTN_COPY_CASE );
	pWnd->EnableWindow( bSetItemSelected );
	pWnd = GetDlgItem( IDC_BTN_DELETE_CASE );
	pWnd->EnableWindow( bSetItemSelected );

	SetEditControlsSelected();
}


// -----------------------------------------------------------------------------
//  InitializeWindowSize
// -----------------------------------------------------------------------------

void
COptionsDialog::InitializeWindowSize()
{
	CRect theWindowRect;
	GetWindowRect( &theWindowRect );

	theWindowRect.right = theWindowRect.left + DIALOG_DEFAULT_WIDTH;
	MoveWindow( &theWindowRect );
}

// -----------------------------------------------------------------------------
//  OnEnChangeFileAgeCount
// -----------------------------------------------------------------------------

void
COptionsDialog::OnEnChangeFileAgeCount()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	CString strAgeCount;
	int		nAgeUnits;

	CEdit* pAgeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_AGE_COUNT );
	CComboBox* pAgeCountCombo = (CComboBox*)GetDlgItem( IDC_FILE_AGE_UNITS );

	pAgeCountEdit->GetWindowText( strAgeCount );

	if ( strAgeCount.GetLength() == 0 ) {
		return;
	}

	if ( strAgeCount == _T("0")) {
		pAgeCountEdit->SetWindowText( _T("1"));
		pAgeCountEdit->SetFocus();
		pAgeCountEdit->SetSel( 1, 1 );
		return;
	}

	UInt32 nAgeCount = _ttol( strAgeCount );

	if ( nAgeCount == 0 ) {
		pAgeCountEdit->SetWindowText( _T(""));
		pAgeCountEdit->SetFocus();
		return;
	}

	nAgeUnits = pAgeCountCombo->GetCurSel();

	if ( nAgeUnits == kSave_Minutes ) {
		if ( nAgeCount > kMinutesInAnHour * kAgeSizeEditFactor ) {
			CString strAgeMax;;
			strAgeMax.Format( _T("%d"), kMinutesInAnHour * kAgeSizeEditFactor );
			pAgeCountEdit->SetWindowText( strAgeMax );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeMax.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
	else if ( nAgeUnits == kSave_Hours ) {
		if ( nAgeCount > kHoursInADay * kAgeSizeEditFactor ) {
			CString strAgeMax;;
			strAgeMax.Format( _T("%d"), kHoursInADay * kAgeSizeEditFactor );
			pAgeCountEdit->SetWindowText( strAgeMax );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeMax.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
	else if ( nAgeUnits == kSave_Days ) {
		if ( nAgeCount > kDaysEditMax ) {
			CString strAgeMax;;
			strAgeMax.Format( _T("%d"), kDaysEditMax );
			pAgeCountEdit->SetWindowText( strAgeMax );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeMax.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}

	for ( int i = 0; i < strAgeCount.GetLength(); i++ ) {
		wchar_t c = strAgeCount[i];
		if ( !isdigit(c) ) {
			strAgeCount.Format( _T("%d"), nAgeCount );
			pAgeCountEdit->SetWindowText( strAgeCount );
			pAgeCountEdit->SetFocus();
			int nLen = strAgeCount.GetLength();
			pAgeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}

}

// -----------------------------------------------------------------------------
//  OnCbnSelchangeFileAgeUnits
// -----------------------------------------------------------------------------

void
COptionsDialog::OnCbnSelchangeFileAgeUnits()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	OnEnChangeFileAgeCount();
}

// -----------------------------------------------------------------------------
//  OnEnChangeFileSizeCount
// -----------------------------------------------------------------------------

void
COptionsDialog::OnEnChangeFileSizeCount()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	CString strSizeCount;

	CEdit* pSizeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_SIZE_COUNT );

	pSizeCountEdit->GetWindowText( strSizeCount );

	if ( strSizeCount.GetLength() == 0 ) {
		return;
	}

	if ( strSizeCount == _T("0")) {
		pSizeCountEdit->SetWindowText( _T("1") );
		pSizeCountEdit->SetFocus();
		pSizeCountEdit->SetSel( 1, 1 );
		return;
	}

	UInt32 nSizeCount = _ttol( strSizeCount );

	if ( nSizeCount == 0 ) {
		pSizeCountEdit->SetWindowText( _T("") );
		pSizeCountEdit->SetFocus();
		return;
	}

	const UInt32 nSizeEditMax = 1024 * kAgeSizeEditFactor;

	if ( nSizeCount > nSizeEditMax ) {
		CString strSizeEditMax;
		strSizeEditMax.Format( _T("%d"), nSizeEditMax );
		pSizeCountEdit->SetWindowText( strSizeEditMax );
		pSizeCountEdit->SetFocus();
		int nLen = strSizeEditMax.GetLength();
		pSizeCountEdit->SetSel( nLen, nLen );
		return;
	}

	for ( int i = 0; i < strSizeCount.GetLength(); i++ ) {
		wchar_t c = strSizeCount[i];
		if ( !isdigit(c) ) {
			strSizeCount.Format( _T("%d"), nSizeCount );
			pSizeCountEdit->SetWindowText( strSizeCount );
			pSizeCountEdit->SetFocus();
			int nLen = strSizeCount.GetLength();
			pSizeCountEdit->SetSel( nLen, nLen );
			return;
		}
	}
}

// -----------------------------------------------------------------------------
//  OnCbnSelchangeFileSizeUnits
// -----------------------------------------------------------------------------

void
COptionsDialog::OnCbnSelchangeFileSizeUnits()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	OnEnChangeFileSizeCount();
}

// -----------------------------------------------------------------------------
//  OnBnClickedCaptureToFile
// -----------------------------------------------------------------------------

void COptionsDialog::OnBnClickedCaptureToFile()
{
	ASSERT( !m_bIsDockedTab );

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	ASSERT( !m_bIsCapturing );

	BnClickedCaptureToFileAction( true );
}

// -----------------------------------------------------------------------------
//  OnBnClickedCaptureToScreen
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedCaptureToScreen()
{
	ASSERT( !m_bIsDockedTab );

	if ( m_bIsDockedTab ) {
		return;
	}

	ASSERT( !m_bIsCapturing );

	BnClickedCaptureToScreenAction( true );
}

// -----------------------------------------------------------------------------
//  ValidateRootDirectory
// -----------------------------------------------------------------------------

bool
COptionsDialog::ValidateRootDirectory( CPeekString& strOutputRootDir )
{
	if ( m_bIsDockedTab ) return true;

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CAPTURE_TO_FILE );
	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();

	if ( !bCaptureToFileChecked ) {
		return true;
	}

	CString strOutDir;

	CEdit* pOutputDir = (CEdit*)GetDlgItem( IDC_OUTPUT_ROOT_EDIT );
	pOutputDir->GetWindowText( strOutDir );
	strOutDir.Trim();

	if ( strOutDir.GetLength() == 0 ) {
		AfxMessageBox( _T("Please enter a root directory for case files.") );
		pOutputDir->SetFocus();
		return false;
	}

	if ( strOutDir.GetAt(1) != _T(':') ) {
		AfxMessageBox( _T("Root case directory name is improper file path.") );
		pOutputDir->SetFocus();
		return false;
	}

	if ( strOutDir.GetLength() < 4 ){
		AfxMessageBox( _T("Please specify a directory other than the root directory.") );
		pOutputDir->SetFocus();
		return false;
	}

	strOutputRootDir = (CPeekString)strOutDir;

	return true;
}

// -----------------------------------------------------------------------------
//  ValidatePort
// -----------------------------------------------------------------------------

bool
COptionsDialog::ValidatePort()
{
	if ( m_bIsDockedTab ) return true;

	CPortNumberList& thePortNumberList = m_pOptions->GetPortNumberList();
	bool bPortFound = false;

	const size_t nNumPortOptions = thePortNumberList.GetCount();
	for ( size_t i = 0; (i < nNumPortOptions) && (bPortFound == false); i++ ) {
		CPortNumber* thePortNumber = thePortNumberList.GetAt( i );
		if ( thePortNumber->IsEnabled() ) {
			bPortFound = true;
		}
	}

	if ( !bPortFound ) {
		AfxMessageBox( _T("There must be at least one Radius port selected.") );
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
//  ValidateCaptureOptions
// -----------------------------------------------------------------------------

bool
COptionsDialog::ValidateCaptureOptions()
{
	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) return true;

	CButton* chkCaptureToFile = (CButton*)GetDlgItem( IDC_CAPTURE_TO_FILE );
	if ( !chkCaptureToFile->GetCheck() ) {
		return true;
	}

	bool bNoFileAge = false;
	bool bNoFileSize = false;

	CButton* chkFileAge = (CButton*)GetDlgItem( IDC_FILE_AGE );
	CButton* chkSizeAge = (CButton*)GetDlgItem( IDC_FILE_SIZE );
	BOOL bFileAgeChecked = chkFileAge->GetCheck();
	BOOL bFileSizeChecked = chkSizeAge->GetCheck();

	if ( !bFileAgeChecked ) bNoFileAge = true;
	if ( !bFileSizeChecked ) bNoFileSize = true;

	CString strValue;
	CEdit* pAgeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_AGE_COUNT );
	pAgeCountEdit->GetWindowText( strValue );
	if ( strValue.GetLength() == 0 || strValue == "0" ) {
		bNoFileAge = true;
	}

	CEdit* pSizeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_SIZE_COUNT );
	pSizeCountEdit->GetWindowText( strValue );
	if ( strValue.GetLength() == 0 || strValue == "0" ) {
		bNoFileSize = true;
	}

	if ( bNoFileAge && bNoFileSize ) {
		AfxMessageBox( _T("You must enter a default save size or save age value") );
		return false;
	}
	return true;
}

// -----------------------------------------------------------------------------
//  DeselectInvalidCases
// -----------------------------------------------------------------------------

void
COptionsDialog::DeselectInvalidCases()
{
	if ( m_bIsDockedTab ) return;

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	int nCount = pList->GetItemCount();	
	for (int i=0; i < nCount; i++)
	{
		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( static_cast<size_t>(i) );
		if ( !theCaseOptions->CanCaseBeEnabled() ) {
			pList->SetCheck( i, FALSE );
		}
	}
}

// -----------------------------------------------------------------------------
//  OnBnClickedLogMsgsToFile
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedLogMsgsToFile()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) return;

	CButton* pLogMsgsToFileButton = (CButton*)GetDlgItem( IDC_LOG_MESSAGES_TO_FILE );

	ASSERT( !m_bIsCapturing );

	BOOL bLogMsgsToFileButton = pLogMsgsToFileButton->GetCheck();
	m_pOptions->SetLogMsgsToFileButton( bLogMsgsToFileButton == TRUE );

	DefaultCaptureSettingsToList();
}


// -----------------------------------------------------------------------------
//  OnBnClickedFoldersForCases
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedFoldersForCases()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	ASSERT( !m_bIsCapturing );

	CButton* pFoldersForCasesButton = (CButton*)GetDlgItem( IDC_FOLDERS_FOR_CASES );
	BOOL bFoldersForCasesButton = pFoldersForCasesButton->GetCheck();
	m_pOptions->SetFoldersForCasesButton( bFoldersForCasesButton == TRUE );

	DefaultCaptureSettingsToList();
}


// -----------------------------------------------------------------------------
//		OnBnClickedLogMsgsToScreen
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedLogMsgsToScreen()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	ASSERT( !m_bIsCapturing );

	CButton* pLogMsgsToScreen = (CButton*)GetDlgItem( IDC_LOG_MESSAGES_TO_SCREEN );
	BOOL bLogMsgsToScreen = pLogMsgsToScreen->GetCheck();
	m_pOptions->SetLogMsgsToScreen( bLogMsgsToScreen == TRUE );

	DefaultCaptureSettingsToList();
}

// -----------------------------------------------------------------------------
//  OnBnClickedBtnMakeChanges
// -----------------------------------------------------------------------------

void
COptionsDialog::OnBnClickedBtnMakeChanges()
{
	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;
}


// -----------------------------------------------------------------------------
//  SetAgeValues
// -----------------------------------------------------------------------------

void
COptionsDialog::SetAgeValues()
{
	if ( m_bInitializing ) return;
	
	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) return;

	m_bInitializing = true;

	UInt32				nAgeCount;
	int					nAgeUnits;

	const UInt32 nSaveAgeValue = m_pOptions->GetSaveAgeSeconds();
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

	CString strSaveAge;
	strSaveAge.Format( _T("%u"),  nAgeCount );

	CEdit* pAgeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_AGE_COUNT );
	pAgeCountEdit->SetWindowText( strSaveAge );
	CComboBox* pAgeCountCombo = (CComboBox*)GetDlgItem( IDC_FILE_AGE_UNITS );
	pAgeCountCombo->SetCurSel( nAgeUnits );

	m_bInitializing = false;
}

// -----------------------------------------------------------------------------
//  SetSizeValues
// -----------------------------------------------------------------------------

void
COptionsDialog::SetSizeValues()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) return;

	m_bInitializing = true;

	UInt64				nSizeCount;
	int					nSizeUnits;

	const UInt64 nSaveSizeValue = m_pOptions->GetSaveSizeBytes();
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

	CString strSaveSize;
	strSaveSize.Format( _T("%u"),  nSizeCount );

	CEdit* pSizeCountEdit = (CEdit*)GetDlgItem( IDC_FILE_SIZE_COUNT );
	pSizeCountEdit->SetWindowText( strSaveSize );
	CComboBox* pSizeCountCombo = (CComboBox*)GetDlgItem( IDC_FILE_SIZE_UNITS );
	pSizeCountCombo->SetCurSel( nSizeUnits );

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//  InitializeDuplicateFlag
// -----------------------------------------------------------------------------

void
COptionsDialog::InitializeDuplicateFlag()
{
	const size_t nCount = m_pCaseOptionsList->GetCount();

	for ( size_t i = 0; i < nCount; i++ ) {
		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( i );
		theCaseOptions->SetDuplicate( false );
	}
}


// -----------------------------------------------------------------------------
//  DefaultCaptureSettingsToList
// -----------------------------------------------------------------------------

void
COptionsDialog::DefaultCaptureSettingsToList()
{
	SyncCaseCaptureOptionsToGlobal();

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	const int nCount = static_cast<int>( m_pCaseOptionsList->GetCount() );

	// Note: assumes the case display list is in sync with the case options
	ASSERT( pList->GetItemCount() == static_cast<int>( nCount ) );

	for( int i = 0; i < nCount; i++ ) {

		CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( i );
		ASSERT( theCaseOptions->GetName() == pList->GetItemText( static_cast<int>( i ), kColumn_Name ) );

		CCaptureOptions& theCaptureOptions = theCaseOptions->GetCaptureOptions();

		CString strOutputDir = theCaseOptions->GetOutputDirectory();
		pList->SetItemText( i, kColumn_OutputDir, strOutputDir );

		CString strValue;
		strValue = (theCaptureOptions.IsCaptureToFile()) ? _T("Yes") : _T("No");
		pList->SetItemText( i, kColumn_ToFile, strValue );
		strValue = (theCaptureOptions.IsCaptureToFile() && theCaptureOptions.IsLogMsgsToFile()) ? _T("Yes") : _T("No");
		pList->SetItemText( i, kColumn_LogEventsToFile, strValue );

		strValue = (theCaptureOptions.IsCaptureToScreen()) ? _T("Yes") : _T("No");
		pList->SetItemText( i, kColumn_ToScreen, strValue );

		strValue = (theCaptureOptions.IsLogMsgsToScreen()) ? _T("Yes") : _T("No");
		pList->SetItemText( i, kColumn_LogEventsToScreen, strValue );
	}
}


// -----------------------------------------------------------------------------
//  BnClickedCaptureToFileAction
// -----------------------------------------------------------------------------

void
COptionsDialog::BnClickedCaptureToFileAction( bool bSyncToList )
{
	ASSERT( !m_bIsDockedTab );
	if ( m_bIsDockedTab ) {
		return;
	}

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CAPTURE_TO_FILE );
	CButton* pCaptureToScreen = (CButton*)GetDlgItem( IDC_CAPTURE_TO_SCREEN );
	CButton* pLogMsgsToFileButton = (CButton*)GetDlgItem( IDC_LOG_MESSAGES_TO_FILE );
	CButton* pFoldersForCasesButton = (CButton*)GetDlgItem( IDC_FOLDERS_FOR_CASES );

	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();
	BOOL bCaptureToScreenChecked = pCaptureToScreen->GetCheck();

	if ( !m_bIsCapturing ) {
		pLogMsgsToFileButton->EnableWindow( bCaptureToFileChecked );
		pFoldersForCasesButton->EnableWindow( bCaptureToFileChecked );
	}

	if ( !bCaptureToFileChecked && !bCaptureToScreenChecked ) {
		bCaptureToScreenChecked = TRUE;
		pCaptureToScreen->SetCheck( bCaptureToScreenChecked );
		BnClickedCaptureToFileAction();
		BnClickedCaptureToScreenAction();
	}

	CWnd* pWnd = NULL;

	BOOL bEnableCaptureToFile = bCaptureToFileChecked;
	if ( m_bIsDockedTab ) {
		bEnableCaptureToFile = FALSE;
	}

	pWnd = GetDlgItem( IDC_FILE_AGE );
	pWnd->EnableWindow( bEnableCaptureToFile );
	pWnd = GetDlgItem( IDC_FILE_SIZE );
	pWnd->EnableWindow( bEnableCaptureToFile );
	pWnd = GetDlgItem( IDC_NEW_FILE_CAPTION );
	pWnd->EnableWindow( bEnableCaptureToFile );
	pWnd = GetDlgItem( IDC_CHK_USE_FILE_NUMBER );
	pWnd->EnableWindow( bEnableCaptureToFile );

	BOOL bEnableRootDirCtls;
	if ( m_bIsCapturing || m_bIsDockedTab ) {
		bEnableRootDirCtls = FALSE;
	}
	else {
		bEnableRootDirCtls = bCaptureToFileChecked;
	}

	pWnd = GetDlgItem( IDC_OUTPUT_ROOT_LBL );
	pWnd->EnableWindow( bEnableRootDirCtls );
	pWnd = GetDlgItem( IDC_OUTPUT_ROOT_EDIT );
	pWnd->EnableWindow( bEnableRootDirCtls );
	pWnd = GetDlgItem( IDC_BROWSE_BTN );
	pWnd->EnableWindow( bEnableRootDirCtls );
	pWnd = GetDlgItem( IDC_BTN_SUGGEST_OUTPUT );
	pWnd->EnableWindow( bEnableRootDirCtls );

	OnBnClickedFileAge();
	OnBnClickedFileSize();

	bCaptureToFileChecked = pCaptureToFile->GetCheck();
	m_pOptions->SetCaptureToFile( (bCaptureToFileChecked == TRUE) );

	if ( bSyncToList ) {
		DefaultCaptureSettingsToList();
	}

	if ( bCaptureToFileChecked ) {
		DeselectInvalidCases();
	}
}


// -----------------------------------------------------------------------------
//  BnClickedCaptureToScreenAction
// -----------------------------------------------------------------------------

void
COptionsDialog::BnClickedCaptureToScreenAction( bool bSyncToList )
{
	if ( m_bIsDockedTab ) {
		return;
	}

	CButton* pCaptureToFile = (CButton*)GetDlgItem( IDC_CAPTURE_TO_FILE );
	CButton* pCaptureToScreen = (CButton*)GetDlgItem( IDC_CAPTURE_TO_SCREEN );

	BOOL bCaptureToFileChecked = pCaptureToFile->GetCheck();
	BOOL bCaptureToScreenChecked = pCaptureToScreen->GetCheck();

	if ( !bCaptureToFileChecked && !bCaptureToScreenChecked ) {
		bCaptureToFileChecked = TRUE;
		pCaptureToFile->SetCheck( bCaptureToFileChecked );
		BnClickedCaptureToScreenAction();
		BnClickedCaptureToFileAction();
	}

	ASSERT ( bCaptureToScreenChecked == pCaptureToScreen->GetCheck() );

	m_pOptions->SetCaptureToScreen( (bCaptureToScreenChecked == TRUE) );

	if ( bSyncToList ) {
		DefaultCaptureSettingsToList();
	}
}

// -----------------------------------------------------------------------------
//		BnClickedDeleteAction
// -----------------------------------------------------------------------------

void 
COptionsDialog::BnClickedDeleteAction()
{
	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int nItem = pList->GetNextSelectedItem(pos);
	CString theName = pList->GetItemText( nItem, 0 );

	m_pCaseOptionsList->DeleteItem( (CPeekString)theName );
	pList->DeleteItem( nItem );
}

#ifdef SHOW_CHILD_DIALOG_ON_INVOKE

// -----------------------------------------------------------------------------
//		OnShowWindow
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnShowWindow( BOOL bShow, UINT /*nStatus*/ )
{
	if ( m_bIsDockedTab ) return;

	if ( m_nCaseItem != -1 ) {
		if ( bShow && !m_bIsDockedTab ) {
			CWnd* pWnd = NULL;

			if ( m_nCaseItem >= 0 ) {
				pWnd = GetDlgItem( IDC_BTN_EDIT_CASE );
			}
			else if ( m_nCaseItem == -2 ) {
				pWnd = GetDlgItem( IDC_PORTS );
			}

			if ( pWnd ) {
				pWnd->PostMessage( BM_CLICK, 0, 0 ); 
			}
		}
	}

	m_nCaseItem = -1;
}
#endif //SHOW_CHILD_DIALOG_ON_INVOKE

// -----------------------------------------------------------------------------
//		CallReplaceCaseDlg
// -----------------------------------------------------------------------------

int
COptionsDialog::CallReplaceCaseDlg( CPeekString strCaseName )
{
	int nResponse = IMPORT_RESPONSE_CANCEL;

	CPeekOutString strMessage;
	strMessage << L"Case " << strCaseName << L" already exists.  Do you want to replace it with the imported case?";

	CReplaceCaseDlg dlg( strMessage, nResponse );

	INT_PTR	nDlgResult = dlg.DoModal();
	if ( nDlgResult != IDOK ) return IMPORT_RESPONSE_CANCEL;

	return nResponse;
}


// -----------------------------------------------------------------------------
//		OnBnClickedView
// -----------------------------------------------------------------------------

void 
COptionsDialog::OnBnClickedView()
{
	ASSERT( m_bIsDockedTab );
	if ( !m_bIsDockedTab ) {
		return;
	}

	CListCtrl* pList = (CListCtrl*)GetDlgItem( IDC_LIST_CASES );
	ASSERT(pList);
	if (pList == NULL) return;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	int nItem = pList->GetNextSelectedItem(pos);
	CString theName = pList->GetItemText( nItem, 0 );

	CCaseOptions* theCaseOptions = m_pCaseOptionsList->GetAt( nItem );

	CPeekString strOutputRootDir;
	if ( !ValidateRootDirectory( strOutputRootDir ) ) {
		return;
	}

	CCaseOptions theCaseOptionsEdit;
	theCaseOptionsEdit = *theCaseOptions;

	bool bDuplicate = theCaseOptionsEdit.GetDuplicate();
	bool bDisableNameEdit = (m_bIsCapturing & !bDuplicate) ? true : false;

	//CString strFacility = m_pOptions->GetFacility();

	CCaseOptionsDlg theDialog( &theCaseOptionsEdit, m_pOptions, bDisableNameEdit, m_bIsDockedTab, nItem );
	INT_PTR nResult = theDialog.DoModal();
	if (nResult == IDOK) {
		if ( theCaseOptionsEdit.IsEnabled() && !theCaseOptionsEdit.CanCaseBeEnabled() ) {
			theCaseOptionsEdit.SetDisabled();
		}
		if ( *theCaseOptions != theCaseOptionsEdit ) {
			theCaseOptions->Copy( theCaseOptionsEdit );
		}
		SortAndDisplayList( theCaseOptionsEdit.GetName() );
	}

	pList->SetFocus();
}
