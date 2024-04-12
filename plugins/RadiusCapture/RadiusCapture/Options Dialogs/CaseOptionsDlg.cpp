// ============================================================================
// CaseOptionsDlg.cpp:
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "CaseOptionsDlg.h"

////////////////////////////////////////////////////////////////////////////////
//		CCaseOptionsDlg
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CCaseOptionsDlg, CDialog)

CCaseOptionsDlg::CCaseOptionsDlg(CCaseOptions* inCaseOptions, COptions* inOptions, bool bIsCapturing,
								 bool bIsDockedTab, int nIdx,  CWnd* pParent /*=NULL*/)
	 : m_pCaseOptions( inCaseOptions )
	 , CDialog(CCaseOptionsDlg::IDD, pParent)
	 , m_CaseOptionsList( inOptions->GetCaseOptionsList() )
	 , m_CaseOptionsListIdx( nIdx )
	 , m_bIsCapturing( bIsCapturing )
	 , m_bIsDockedTab( bIsDockedTab )
	 , m_pOptions( inOptions )
	 , m_GeneralOptsDlg( *m_pCaseOptions, inOptions->GetOutputRootDirectory(), this, bIsCapturing, bIsDockedTab )
	 , m_UserNameOptsDlg( m_pCaseOptions->m_UserNameOptions, bIsDockedTab )
	 , m_TriggerOptionsDlg( m_pCaseOptions->m_TriggerOptions, bIsDockedTab )
	 , m_CasePortOptionsDlg( m_pCaseOptions->m_CasePortOptions, inOptions, m_bIsCapturing, bIsDockedTab )
	 , m_CaptureOptionsDlg( m_pCaseOptions->m_CaptureOptions, inOptions, m_bIsCapturing, bIsDockedTab )
	 , m_strGlobalOutputDirectory( inOptions->GetOutputRootDirectory() )
	 , m_bInitializing( false )
{
	m_TabArray.Add( CTabItem( m_GeneralOptsDlg, CGeneralOptionsDlg::IDD, IDC_BTN_GENERAL ) );
	m_TabArray.Add( CTabItem( m_UserNameOptsDlg, CUserNameOptionsDlg::IDD, IDC_BTN_USERNAMES ) );
	m_TabArray.Add( CTabItem( m_TriggerOptionsDlg, CTriggerOptionsDlg::IDD, IDC_BTN_TRIGGER ) );
	m_TabArray.Add( CTabItem( m_CasePortOptionsDlg, CCasePortOptionsDlg::IDD, IDC_BTN_PORTS ) );
	m_TabArray.Add( CTabItem( m_CaptureOptionsDlg, CCaptureOptionsDlg::IDD, IDC_BTN_CAPTURE ) );
}

// -----------------------------------------------------------------------------
//		Destruction
// -----------------------------------------------------------------------------

CCaseOptionsDlg::~CCaseOptionsDlg()
{
}

// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void CCaseOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCaseOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_GENERAL, OnBnClickedTabGeneral)
	ON_BN_CLICKED(IDC_BTN_USERNAMES, OnBnClickedTabUserNames)
	ON_BN_CLICKED(IDC_BTN_TRIGGER, OnBnClickedTabTriggerTimes)
	ON_BN_CLICKED(IDC_BTN_PORTS, OnBnClickedTabPorts)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, OnBnClickedTabCapture)
END_MESSAGE_MAP()

// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CCaseOptionsDlg::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	__super::OnInitDialog();

	m_bInitializing = true;

	m_pCaseOptions->SyncGeneralOptionsToGlobal( m_strGlobalOutputDirectory, m_pOptions->IsUseFileNumber() );

	CreateTabs();

	InitializeControls();

	m_bInitializing = false;

	return TRUE;
}

// -----------------------------------------------------------------------------
//		CreateTabs
// -----------------------------------------------------------------------------

void
CCaseOptionsDlg::CreateTabs()
{
	CRect	rcWindow;
	GetWindowRect( &rcWindow );

	CPoint	ptMax( 0, 0 );
	CRect rcLargest( ptMax, ptMax );

	for ( size_t i = 0; i < m_TabArray.GetCount(); i++ ) {
		CTabItem& item = m_TabArray[i];
		item.m_Tab.Create( item.m_nResId, this );

		CRect rc;
		item.m_Tab.GetWindowRect( &rc );

		if ( (rcLargest.left == 0) || (rcLargest.left > rc.left) ) {
			rcLargest.left = rc.left;
		}
		if ( (rcLargest.right == 0) || (rcLargest.right < rc.right) ) {
			rcLargest.right = rc.right;
		}
		if ( (rcLargest.top == 0) || (rcLargest.top > rc.top) ) {
			rcLargest.top = rc.top;
		}
		if ( (rcLargest.bottom == 0) || (rcLargest.bottom < rc.bottom) ) {
			rcLargest.bottom = rc.bottom;
		}

		if ( rc.Height() > ptMax.x ) {
			ptMax.x = rc.Height();
		}
		if ( rc.Width() > ptMax.y ) {
			ptMax.y = rc.Width();
		}
	}

	const int nHeightAdj = + 230 + GetSystemMetrics( SM_CYCAPTION );
	const int nWidthAdj = + 120;

	rcWindow.bottom = rcWindow.top + ptMax.x + nHeightAdj;
	rcWindow.right = rcWindow.left + ptMax.y + nWidthAdj;
	MoveWindow( &rcWindow );
	SetActiveTab( kTab_General );

	int nWidthDiff = rcWindow.Width() - rcLargest.Width();
	int nNewLeft = (nWidthDiff / 2) + rcWindow.left;
	int nNewTop = rcWindow.top + 145 + GetSystemMetrics( SM_CYCAPTION );

	rcLargest.MoveToX( nNewLeft );
	rcLargest.MoveToY( nNewTop );

	ScreenToClient( rcLargest );

	for ( size_t i = 0; i < m_TabArray.GetCount(); i++ ) {
		CTabItem&	item = m_TabArray[i];
		item.m_Tab.MoveWindow( &rcLargest );
	}
}

// -----------------------------------------------------------------------------
//		OnBnClickedOk
// -----------------------------------------------------------------------------

void CCaseOptionsDlg::OnBnClickedOk()
{
	if ( m_bIsDockedTab ) return;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString theCaseName, strTemp;

	m_GeneralOptsDlg.UpdateData();
	m_UserNameOptsDlg.UpdateData();
	m_TriggerOptionsDlg.UpdateData();
	m_CasePortOptionsDlg.UpdateData();
	m_CaptureOptionsDlg.UpdateData();

	CEdit* pEditName = (CEdit*)m_GeneralOptsDlg.GetDlgItem( IDC_NAME_EDIT );
	ASSERT(pEditName);
	pEditName->GetWindowText( theCaseName );
	strTemp = theCaseName;
	theCaseName.Trim();
	if ( theCaseName != strTemp ) {
		SetCaseName( (CPeekString)theCaseName );
	}
	if (theCaseName.GetLength()==0)
	{
		if ( m_bIsCapturing ) {
			ASSERT( 0 );
			return;
		}
		AfxMessageBox( _T("Please a enter a name.") );
		SetActiveTab( kTab_General );
		pEditName->SetFocus();
		pEditName->SetSel( 0, -1 );
		return;
	}

	CString thePrefix;

	CEdit* pEditPrefix = (CEdit*)m_GeneralOptsDlg.GetDlgItem( IDC_EDIT_PREFIX );
	ASSERT(pEditPrefix);
	pEditPrefix->GetWindowText( thePrefix );
	strTemp = thePrefix;
	thePrefix.Trim();
	if ( thePrefix != strTemp ) {
		SetPrefix( (CPeekString)thePrefix );
	}

	bool bIsNameUnique = m_CaseOptionsList.IsNameUnique( (CPeekString)theCaseName, m_CaseOptionsListIdx );
	if ( !bIsNameUnique ) {
		if ( m_bIsCapturing ) {
			ASSERT( 0 );
			return;
		}
		CString strMessage;
		strMessage.Format( _T("There is already an entry with the case name %s.  Please enter a unique case name."), theCaseName ); 
		AfxMessageBox( strMessage );
		SetActiveTab( kTab_General );
		pEditName->SetFocus();
		pEditName->SetSel( 0, -1 );
		return;
	}

	bool bIsPrefixUnique = m_CaseOptionsList.IsPrefixUnique( (CPeekString)thePrefix, m_CaseOptionsListIdx );
	if ( !bIsPrefixUnique ) {
		if ( m_bIsCapturing ) {
			ASSERT( 0 );
			return;
		}
		CString strMessage;
		strMessage.Format( _T("There is already a prefix entry with the name %s.  Please enter a unique prefix name."), thePrefix ); 
		AfxMessageBox( strMessage );
		SetActiveTab( kTab_General );
		pEditPrefix->SetFocus();
		pEditPrefix->SetSel( 0, -1 );
		return;
	}

	if ( !ValidateOutputDirectory() ) {
		SetActiveTab( kTab_General );
		CEdit* pOutputDir = (CEdit*)m_GeneralOptsDlg.GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
		pOutputDir->SetFocus();
		return;
	}

	if ( !ValidatePorts() ) {
		if ( m_bIsCapturing ) {
			ASSERT( 0 );
			return;
		}
		SetActiveTab( kTab_Ports );
		return;
	}

	UInt64 uStartTime = m_pCaseOptions->GetStartTime();
	UInt64 uEndTime = m_pCaseOptions->GetStopTime();
	if ( uStartTime >= uEndTime ) {
		CString strMessage;
		if ( uStartTime == uEndTime ) strMessage = _T("Trigger time error: the start time is the same as the end time."); 
		else strMessage = _T("Trigger time error: the start time is later than the end time."); 
		AfxMessageBox( strMessage );
		SetActiveTab( kTab_TriggerTimes );
		return;
	}

	m_pCaseOptions->Init( (CPeekString)theCaseName, m_pOptions->IsUseFileNumber() );
	CPeekString	strPrefix( thePrefix );
	m_pCaseOptions->SetPrefix( strPrefix );

	CString strOutputDirectory;
	CEdit* pEditOutputDir = (CEdit*)m_GeneralOptsDlg.GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
	ASSERT(pEditOutputDir);
	pEditOutputDir->GetWindowText( strOutputDirectory );

	strOutputDirectory.Trim();
	if ( strOutputDirectory.CompareNoCase( (CString)m_pCaseOptions->GetOutputDirectory() ) != 0 ) {
		m_pCaseOptions->SetOutputDirectory( (CPeekString)strOutputDirectory );
	}

	m_pCaseOptions->SyncGeneralOptionsToGlobal( m_strGlobalOutputDirectory, m_pOptions->IsUseFileNumber() );

	__super::OnOK();
}

// -----------------------------------------------------------------------------
//		ShowTab
// -----------------------------------------------------------------------------

void
CCaseOptionsDlg::ShowTab( UInt32 nTab )
{
	if ( m_bInitializing ) return;

	m_bInitializing = true;

	int nShow;

	m_pCaseOptions->m_nLastTab = nTab;
	
	for ( UInt32 i = 0; i < m_TabArray.GetCount(); i++ ) {
		if ( i == nTab ) {
			nShow = SW_SHOW;
		}
		else {
			nShow = SW_HIDE;
		}
		m_TabArray[i].m_Tab.ShowWindow( nShow );
	}

	CTabItem& itemFirst = m_TabArray[kTab_General];
	CTabItem& itemLast = m_TabArray[kTab_Capture];
	CTabItem& itemCurrent = m_TabArray[nTab];
	CheckRadioButton( itemFirst.m_nId, itemLast.m_nId, itemCurrent.m_nId );

	m_bInitializing = false;
}

// -----------------------------------------------------------------------------
//  ValidateOutputDirectory
// -----------------------------------------------------------------------------

bool
CCaseOptionsDlg::ValidateOutputDirectory()
{
	if ( m_bIsDockedTab ) return true;

	CCaptureOptions& theCaptureOptions = m_pCaseOptions->m_CaptureOptions;
	if ( !theCaptureOptions.IsCaptureToFile() ) {
		return true;
	}

	CString strOutputDirectory;

	CEdit* pOutputDir = (CEdit*)m_GeneralOptsDlg.GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
	pOutputDir->GetWindowText( strOutputDirectory );
	strOutputDirectory.Trim();

	if ( strOutputDirectory.GetLength() == 0 ) {
		AfxMessageBox( _T("Please enter a directory for case files.") );
		return false;
	}

	if ( strOutputDirectory.GetAt(1) != _T(':') ) {
		AfxMessageBox( _T("Case directory name is improper file path.") );
		return false;
	}

	if ( strOutputDirectory.GetLength() < 4 ){
		AfxMessageBox( _T("Please specify a directory other than the root directory.") );
		return false;
	}

	bool bReturn = ::IsPathValid( (CPeekString)strOutputDirectory );
	if ( !bReturn ) {
		CString strMessage;
		strMessage.Format( _T("\"%s\" is not a valid directory path."), strOutputDirectory );
		AfxMessageBox( strMessage );
		return false;
	}

	m_pCaseOptions->SetOutputDirectory( (CPeekString)strOutputDirectory );

	return true;
}

// -----------------------------------------------------------------------------
//  ValidatePort
// -----------------------------------------------------------------------------

bool
CCaseOptionsDlg::ValidatePorts()
{
	if ( m_bIsDockedTab ) return true;

	// Validate port selection
	CPortNumberList& thePortNumberList = m_pCaseOptions->GetPortNumberList();
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
//  InitializeControls
// -----------------------------------------------------------------------------

void
CCaseOptionsDlg::InitializeControls()
{
	if ( !m_bIsDockedTab ) return;

	CWnd* pOKButton = GetDlgItem( IDC_BTN_OK );
	pOKButton->ShowWindow( SW_HIDE );

	// Center the Cancel button
	CRect theClientRect;
	GetClientRect( &theClientRect );
	int nMiddle = theClientRect.Width() / 2;

	CRect theCancelButtonRect;
	CWnd* pCancelButton;

	pCancelButton = GetDlgItem( IDCANCEL );

	pCancelButton->GetWindowRect( &theCancelButtonRect );
	ScreenToClient( theCancelButtonRect );

	int nNewLeftPos = nMiddle - 35;
	int nNewTopPos = theCancelButtonRect.top - 5;
	pCancelButton->MoveWindow( nNewLeftPos, nNewTopPos, theCancelButtonRect.Width(), theCancelButtonRect.Height()  );		
}
