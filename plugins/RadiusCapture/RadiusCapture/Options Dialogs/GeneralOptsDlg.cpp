// =============================================================================
//	GeneralOptsDlg.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "GeneralOptsDlg.h"
#include "CaseOptionsDlg.h"
#include "FileEx.h"

//IMPLEMENT_DYNAMIC(CGeneralOptsDlg, CDialog)

////////////////////////////////////////////////////////////////////////////////
//		CGeneralOptionsDlg
////////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
//	Construction
// -----------------------------------------------------------------------------

CGeneralOptionsDlg::CGeneralOptionsDlg(
	CCaseOptions& inCaseOptions
	, CPeekString strGlobalOutputRootDirectory
	, CCaseOptionsDlg* pCaseOptionsDlg
	, bool bIsCapturing
	, bool bIsDockedTab )
	: CDialog( CGeneralOptionsDlg::IDD )
	, m_CaseOptions( inCaseOptions )
	, m_GeneralOptions( inCaseOptions.m_GeneralOptions )
	, m_strGlobalOutputRootDir( strGlobalOutputRootDirectory )
	, m_pCaseOptionsDlg( pCaseOptionsDlg )
	, m_bIsCapturing( bIsCapturing )
	, m_bIsDockedTab( bIsDockedTab )
	, m_bInitializing( false )
{
}

// -----------------------------------------------------------------------------
//	OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CGeneralOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInitializing = true;

	SetDataToScreen();
	SetControlsInitialState();

	m_bInitializing = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CGeneralOptionsDlg, CDialog)
	ON_EN_CHANGE(IDC_NAME_EDIT, OnCaseNameChange)
	ON_EN_CHANGE(IDC_EDIT_PREFIX, OnPrefixChange)
	ON_EN_CHANGE(IDC_EDIT_CASE_FILEDIRECTORY, OnFileDirectoryChange)
	ON_BN_CLICKED(IDC_BTN_CASE_SUGGEST_OUTPUT, OnBnClickedSuggestOutput)
	ON_BN_CLICKED(IDC_CASE_BROWSE_BTN, OnBnClickedCaseBrowseBtn)
END_MESSAGE_MAP()


// -----------------------------------------------------------------------------
//		OnCaseNameChange
// -----------------------------------------------------------------------------

void 
CGeneralOptionsDlg::OnCaseNameChange()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bInitializing ) return;

	ASSERT( !m_bIsCapturing );

	CEdit* pEditCase = (CEdit*)GetDlgItem( IDC_NAME_EDIT );

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		pEditCase->SetWindowText( m_GeneralOptions.GetName() );
		m_bInitializing = false;
		return;
	}

	CPeekString strCaseName; // = m_GeneralOptions.GetName();
	if ( !GetEditControlText( pEditCase, strCaseName, 30 ) ) {
		return;
	}

	m_bInitializing = true;

	const size_t nLength( strCaseName.GetLength() );
	for ( size_t i = 0; i < nLength; i++ ) {
		wchar_t ch( strCaseName[i] );
		if ( !FilePath::IsValidFileNameChar( ch ) || ch == L':' ) {
			strCaseName = m_GeneralOptions.GetName();
			SetCaseNameEditText( strCaseName );
			m_bInitializing = false;
			return;
		}
	}

	m_GeneralOptions.SetName( strCaseName );

	m_pCaseOptionsDlg->SetCaseNameDisplay( strCaseName );

	const CPeekString& strPrefix = m_GeneralOptions.GetPrefix();

	size_t nCaseNameLen( strCaseName.GetLength() );
	size_t nPrefixTestLen( strPrefix.GetLength() );
	nPrefixTestLen = nPrefixTestLen > 0 ? nPrefixTestLen - 1 : 0;

	if ( nPrefixTestLen  > nCaseNameLen || strPrefix.Left( nPrefixTestLen ) != strCaseName.Left( nPrefixTestLen ) ) {
		m_bInitializing = false;
		return;
	}

	m_GeneralOptions.SetPrefix( strCaseName );
	SetPrefixEditText( strCaseName );

	m_bInitializing = false;
}

// -----------------------------------------------------------------------------
//		OnPrefixChange
// -----------------------------------------------------------------------------

void 
CGeneralOptionsDlg::OnPrefixChange()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( m_bInitializing ) return;

	ASSERT( !m_bIsCapturing );

	CEdit* pEditPrefix = (CEdit*)GetDlgItem( IDC_EDIT_PREFIX );

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		pEditPrefix->SetWindowText( m_GeneralOptions.GetName() );
		m_bInitializing = false;
		return;
	}

	CPeekString strPrefix; // = m_GeneralOptions.GetPrefix();
	if ( !GetEditControlText( pEditPrefix, strPrefix, 30 ) ) {
		return;
	}

	m_bInitializing = true;

	const size_t nLength( strPrefix.GetLength() );
	for ( size_t i = 0; i < nLength; i++ ) {
		wchar_t ch( strPrefix[i] );
		if ( !FilePath::IsValidFileNameChar( ch ) || ch == L':' ) {
			SetPrefixEditText( m_GeneralOptions.GetPrefix() );
			m_bInitializing = false;
			return;
		}
	}

	m_GeneralOptions.SetPrefix( strPrefix );

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//		OnBnClickedSuggestOutput
// -----------------------------------------------------------------------------

void CGeneralOptionsDlg::OnBnClickedSuggestOutput()
{
	if ( m_bInitializing ) return;

	ASSERT( !m_bIsDockedTab );
	ASSERT( !m_bIsCapturing );

	m_bInitializing = true;

	SuggestAndSetOutputDir();

	m_bInitializing = false;

}


// -----------------------------------------------------------------------------
//		SuggestAndSetOutputDir
// -----------------------------------------------------------------------------

void CGeneralOptionsDlg::SuggestAndSetOutputDir()
{
	ASSERT( m_bInitializing );
	ASSERT( !m_bIsDockedTab );
	ASSERT( !m_bIsCapturing );

 	ASSERT( m_strGlobalOutputRootDir.GetLength() > 0 );

	CPeekString strOutputDirectory;

	CEdit* pEditOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
	ASSERT( pEditOutputDir );
	if ( !pEditOutputDir ) return;

	if ( m_bIsDockedTab || m_bIsCapturing ) {
		strOutputDirectory = m_GeneralOptions.GetOutputDirectory();
		ASSERT( strOutputDirectory.GetLength() > 0 );
	}
	else {
		strOutputDirectory = m_GeneralOptions.SuggestOutputDirectory( m_strGlobalOutputRootDir, GetPrefixEditText() );
		m_GeneralOptions.SetOutputDirectory( strOutputDirectory );
	}

	pEditOutputDir->SetWindowText( strOutputDirectory );
}


// -----------------------------------------------------------------------------
//		OnBnClickedCaseBrowseBtn
// -----------------------------------------------------------------------------

void
CGeneralOptionsDlg::OnBnClickedCaseBrowseBtn()
{
	if ( m_bInitializing ) return;
	if ( m_bIsDockedTab ) return;

	ASSERT( !m_bIsCapturing );

	TCHAR	szTempLocation[512];

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
			CPeekString strOutputDirectory = szTempLocation;
			CEdit* pEditOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
			pEditOutputDir->SetWindowText( strOutputDirectory );
			m_GeneralOptions.SetOutputDirectory( strOutputDirectory );
		}
	}

	LPMALLOC pMalloc = NULL;
	if ( SUCCEEDED( ::SHGetMalloc( &pMalloc ) ) ) {
		pMalloc->Free( pidlPath );
	}
}


// -----------------------------------------------------------------------------
//		SetControlsInitialState
// -----------------------------------------------------------------------------

void
CGeneralOptionsDlg::SetControlsInitialState()
{
	ASSERT( m_bInitializing );

	CEdit* pEditName = (CEdit*)GetDlgItem( IDC_NAME_EDIT );
	CEdit* pEditPrefix = (CEdit*)GetDlgItem( IDC_EDIT_PREFIX );
	CEdit* pEditOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
	CButton* pBtnSuggest = (CButton*)GetDlgItem( IDC_BTN_CASE_SUGGEST_OUTPUT );

	ASSERT( pEditName && pEditPrefix );

	if ( m_bIsDockedTab ) {
		pBtnSuggest->ShowWindow( SW_HIDE );
		pEditName->EnableWindow( FALSE );
		pEditPrefix->EnableWindow( FALSE );
		pEditOutputDir->EnableWindow( FALSE );
		pBtnSuggest->EnableWindow( FALSE );

		return;
	}

	if ( m_bIsCapturing ) {
		pEditName->EnableWindow( FALSE );
		pEditPrefix->EnableWindow( FALSE );
		pEditOutputDir->EnableWindow( FALSE );
		pBtnSuggest->EnableWindow( FALSE );
	}
	else {
		pEditName->EnableWindow( TRUE );
		pEditPrefix->EnableWindow( TRUE );
		pEditOutputDir->EnableWindow( TRUE );
		pBtnSuggest->EnableWindow( TRUE );

		SetControlsState();
	}
}

// -----------------------------------------------------------------------------
//		SetDataToScreen
// -----------------------------------------------------------------------------

void
CGeneralOptionsDlg::SetDataToScreen()
{
	ASSERT( m_bInitializing );

	const CPeekString& theCaseName = m_GeneralOptions.GetName();
	m_pCaseOptionsDlg->SetCaseNameDisplay( theCaseName );

	CEdit* pEditName = (CEdit*)GetDlgItem( IDC_NAME_EDIT );
	ASSERT(pEditName);
	pEditName->SetWindowText( theCaseName );
	if ( !m_bIsDockedTab ) {
		pEditName->SetFocus();
		pEditName->SetSel( 0, -1 );
	}

	CEdit* pEditPrefix = (CEdit*)GetDlgItem( IDC_EDIT_PREFIX );
	ASSERT( pEditPrefix );
	if ( !pEditPrefix ) return;

	pEditPrefix->SetWindowText( (CString)m_GeneralOptions.GetPrefix() );

	if ( m_GeneralOptions.GetOutputDirectory() == 0 ) {
		SuggestAndSetOutputDir();
	}
	else {
		CEdit* pEditOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
		ASSERT( pEditOutputDir );
		pEditOutputDir->SetWindowText( m_GeneralOptions.GetOutputDirectory() );
	}
}


// -----------------------------------------------------------------------------
//		OnFileDirectoryChange
// -----------------------------------------------------------------------------

void 
CGeneralOptionsDlg::OnFileDirectoryChange()
{
	if ( m_bInitializing ) return;

	CEdit* pEditOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
	ASSERT( pEditOutputDir );

	if ( m_bIsDockedTab ) {
		m_bInitializing = true;
		pEditOutputDir->SetWindowText( m_GeneralOptions.GetOutputDirectory() );
		m_bInitializing = false;
		return;
	}

	CPeekString strOutputDirectory = m_GeneralOptions.GetOutputDirectory();
	if ( !GetEditControlText( pEditOutputDir, strOutputDirectory, MAX_PATH ) ) {
		return;
	}

	m_bInitializing = true;

	const int nLength( static_cast<int>( strOutputDirectory.GetLength() ) );

	// Validate that this is a proper path
	for ( int i = 0; i < nLength; i++ ) {
		bool bError( false );
		wchar_t ch( strOutputDirectory[i] );
		if ( !FilePath::IsValidPathChar( ch ) ) bError = true;
		else if ( i == 1 && ch != L':' ) bError = true;
		else if ( i != 1 && ch == L':' ) bError = true;
		else if ( i == 2 && (ch != L'\\' && ch != L'/') ) bError = true;
		else if ( i > 2 && ( (ch == L'\\' || ch == L'/') && 
			(strOutputDirectory[i-1] == L'\\' || strOutputDirectory[i-1] == L'/') ) ) bError = true;

		if ( bError ) {
			SetOutputDirText( m_GeneralOptions.GetOutputDirectory() );
			m_bInitializing = false;
			return;
		}
	}


	m_GeneralOptions.SetOutputDirectory( strOutputDirectory );

	m_bInitializing = false;
}


// -----------------------------------------------------------------------------
//		SetControlsState
// -----------------------------------------------------------------------------

void
CGeneralOptionsDlg::SetControlsState()
{
	ASSERT( !m_bIsCapturing );
	ASSERT( !m_bIsDockedTab );

	ASSERT( m_bInitializing );

	CButton* pBtnSuggest = (CButton*)GetDlgItem( IDC_BTN_CASE_SUGGEST_OUTPUT );
	CEdit* pEditOutputDir = (CEdit*)GetDlgItem( IDC_EDIT_CASE_FILEDIRECTORY );
	ASSERT( pBtnSuggest && pEditOutputDir );
	pBtnSuggest->EnableWindow( TRUE );
}


// -----------------------------------------------------------------------------
//		GetEditControlText
// -----------------------------------------------------------------------------

bool
CGeneralOptionsDlg::GetEditControlText( 
	CEdit*			pEditControl,
	CPeekString&	InOutEditText,
	int				nMaxLength ) 
{
	ASSERT( pEditControl );

	CString strNewContents;
	pEditControl->GetWindowText( strNewContents );

	bool bWasInitializing = m_bInitializing;
	int	 nLen = strNewContents.GetLength();
	if ( (strNewContents.Left( 1 ) == L" ") || ( nLen > nMaxLength ) ) {
		m_bInitializing = true;
		pEditControl->SetWindowText( (CString)InOutEditText );
		pEditControl->SetSel( nLen, nLen );

		m_bInitializing = bWasInitializing;
		return false;
	}

	InOutEditText = (CPeekString)strNewContents;
	m_bInitializing = bWasInitializing;
	return true;
}
