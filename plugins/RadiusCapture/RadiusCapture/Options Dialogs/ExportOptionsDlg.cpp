// ============================================================================
//	ExportOptionsDlg.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "stdafx.h"
#include "ExportOptionsDlg.h"
#include "CaseOptionsDlg.h"
#include "PluginMessages.h"
#include "CaseViewDlg.h"
#include "FileEx.h"

#define kColumn_Name				0
#define kColumn_UserNames			1
#define kColumn_Start				2
#define kColumn_Stop				3


////////////////////////////////////////////////////////////////////////////////
//		CExportOptionsDlg
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CExportOptionsDlg, CDialog)

// -----------------------------------------------------------------------------
//  Construction
// -----------------------------------------------------------------------------

CExportOptionsDlg::CExportOptionsDlg(
	 bool			bIsCapturing,
	 COptions&		inOptions,
 	 COmniPlugin*	inPlugin,
	 CWnd*			pParent /*=NULL*/ )
	 :	CDialog( CExportOptionsDlg::IDD_MODAL, pParent )
	 ,	m_Options( inOptions )
	 ,	m_pPlugin( inPlugin )
	 ,	m_bWaitingForOptions( false )
	 ,	m_nTimer( 0 )
{
	bIsCapturing;
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CExportOptionsDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CWnd* pImportBtn = GetDlgItem( IDOK );
	if ( pImportBtn )	pImportBtn->EnableWindow( FALSE );

	return FALSE;
}


BEGIN_MESSAGE_MAP(CExportOptionsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDT_EXPORT_FILE, OnEnChangeExportFile)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


/*
// -----------------------------------------------------------------------------
//		OnBnClickedBrowseForFile
// -----------------------------------------------------------------------------

void 
CExportOptionsDlg::OnBnClickedBrowseForFile()
{
	CString	strFilePath;
//	WCHAR	szFilter[] = _T( "XML Files (*.xml)|*.xml|All Files (*.*)|*.*|" );

	CWnd* pExportFileEdit = GetDlgItem( IDC_EDT_Export_FILE );
	ASSERT( pExportFileEdit );
	if ( !pExportFileEdit ) return;

	CString strExportFileName; 
	pExportFileEdit->GetWindowText( strExportFileName );

	if ( strExportFileName.GetLength() >  3 ) {
		strExportFileName.Replace( _T("/"), _T("\\") );
		if ( (strExportFileName.Right(4)).MakeLower() == _T(".xml") ) {
			if ( (ValidateExportFile( strExportFileName, false ) == true) ) {
				strFilePath = strExportFileName;
			}
		}
		else { 
			if ( strExportFileName.Find( '.' ) < 0 ) {
				if ( strExportFileName.Right(1) == _T("/") ) {
					strExportFileName = strExportFileName.Left( strExportFileName.GetLength() - 1 ); 
				}
				if ( strExportFileName.Right(1) != _T("\\") ) {
					strExportFileName += _T("\\");
				}
				if ( ::DirectoryExists( strExportFileName ) ) {
					strFilePath = strExportFileName;
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
//	pExportFileEdit->SetWindowText( strFilePath );
//
//	OnBnClickedReadDisplayFile();
}
*/


// -----------------------------------------------------------------------------
//		OnBnClickedCancel
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::OnBnClickedCancel()
{
	m_bWaitingForOptions = false;

	KillTimer( m_nTimer );   

	OnCancel();
}


// -----------------------------------------------------------------------------
//		OnBnClickedOk
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::OnBnClickedOk()
{
 	if ( m_bWaitingForOptions ) return;

	CString strFilePath;

	CWnd* pExportFileEdit = GetDlgItem( IDC_EDT_EXPORT_FILE );
	ASSERT( pExportFileEdit );
	if ( !pExportFileEdit ) return;

	pExportFileEdit->GetWindowText( strFilePath );
	strFilePath.Trim();

	pExportFileEdit->SetWindowText( strFilePath );

	CPeekString	strPath( strFilePath );
	if ( !ValidateExportPath( strPath ) ) {
		return;
	}

	// Disable gui objects
	SetWaitForResponse( true );

#ifdef IMPLEMENT_PLUGINMESSAGE
	RequestOptionsExport( strPath );
#endif // IMPLEMENT_PLUGINMESSAGE

}


// -----------------------------------------------------------------------------
//  DoDataExchange
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::DoDataExchange(
	CDataExchange* pDX )
{
	if ( pDX->m_bSaveAndValidate ) return;


}


// -----------------------------------------------------------------------------
//  ValidateExportPath
// -----------------------------------------------------------------------------

bool
CExportOptionsDlg::ValidateExportPath( 
	CPeekString& inExportFilePath )
{
	CWnd* pExportFileEdit = GetDlgItem( IDC_EDT_EXPORT_FILE );
	ASSERT( pExportFileEdit );
	if ( !pExportFileEdit ) return false;

	if ( inExportFilePath.GetLength() == 0 ) {
		AfxMessageBox( _T("Please enter an export file path and name.") );
		pExportFileEdit->SetFocus();
		return false;
	}

	if ( inExportFilePath.GetAt(1) != _T(':') ) {
		AfxMessageBox( _T("Export path is incorrectly formatted.") );
		pExportFileEdit->SetFocus();
		return false;
	}

	if ( inExportFilePath.GetLength() < 4 ){
		AfxMessageBox( _T("File path is not valid.") );
		pExportFileEdit->SetFocus();
		return false;
	}

	const size_t nLength( inExportFilePath.GetLength() );
	for ( size_t i = 0; i < nLength; i++ ) {
		if ( !FilePath::IsValidPathChar( inExportFilePath[i] ) ) {
			CString	strErrorMsg;
			strErrorMsg.Format( L"Invalid character \"%c\" in file path name.", inExportFilePath[i] );
			AfxMessageBox( strErrorMsg );
			pExportFileEdit->SetFocus();
			return false;
		}
	}

	if ( inExportFilePath.Right( 1 ) == L"\\" || inExportFilePath.Right( 1 ) == L"/" ) {
		AfxMessageBox( _T("Please enter a valid file name.") );
		pExportFileEdit->SetFocus();
		return false;
	}

	return true;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		RequestOptionsExport
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::RequestOptionsExport( 
	CPeekString& inFilePath )
{
	_ASSERTE( m_pPlugin != NULL );
	if ( m_pPlugin == NULL ) return;

	CMessageImportExport*	pMsgExport = new CMessageImportExport();
	if ( (pMsgExport == NULL) || (!pMsgExport->IsModeled()) ) return;

	pMsgExport->SetImportExportMessage( inFilePath );
	pMsgExport->SetAsExport();
	pMsgExport->SetOptions( m_Options );
		
	FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
	m_pPlugin->PostPluginMessage( CPeekMessagePtr( pMsgExport ), this, fnProcess );
}


// -----------------------------------------------------------------------------
//		GetOptionsResponse
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::GetOptionsResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( inResult != HE_S_OK ) return;

	CMessageImportExport*	pMessage = dynamic_cast<CMessageImportExport*>( inMessage.get() );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return;

	if ( !pMessage->LoadResponse( inResponse ) ) return;

	ASSERT( pMessage->IsExport() );
	if ( !pMessage->IsExport() )	return;

	CPeekString	strResultMessage( pMessage->GetImportExportMessage() );

	const  bool bSuccess = strResultMessage == L"Success";
	if ( !bSuccess ) {
		CString	strErrorMessage;
		strErrorMessage.Format( 
			L"Attempt to export options on the engine was not successful.\r\nError message is:\r\n\r\n%s",
			strResultMessage.Format() );
		AfxMessageBox( strErrorMessage );
		
		CWnd* pWnd = GetDlgItem( IDC_EDT_EXPORT_FILE );
		if ( pWnd ) pWnd->SetFocus();

		return;
	}

	AfxMessageBox( L"Options export was successful." );

	SetWaitForResponse( false );

	OnBnClickedCancel();
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	inResponse )
{
	if ( !m_bWaitingForOptions )	return;

	SetWaitForResponse( false );

	int	nType = inMessage->GetType();
	switch ( nType ) {
		case CMessageImportExport::s_nMessageType:
			GetOptionsResponse( inResult, inMessage, inResponse );
			break;
		default:
			break;
	}
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		SetWaitForResponse
// -----------------------------------------------------------------------------

void 
CExportOptionsDlg::SetWaitForResponse(
	bool inStartWait )
{
	BOOL	bEnable( inStartWait == true ? FALSE : TRUE );

	m_bWaitingForOptions = inStartWait;

	CWnd* pWnd = GetDlgItem( IDOK );
	if ( pWnd ) pWnd->EnableWindow( bEnable );

	pWnd = GetDlgItem( IDC_EDT_EXPORT_FILE );
	if ( pWnd ) pWnd->EnableWindow( bEnable );

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
CExportOptionsDlg::OnTimer(
	UINT_PTR nIDEvent )
{
	nIDEvent;

	AfxMessageBox( L"Wait for engine return of Export options has timed out." );

	SetWaitForResponse( false );
}


// -----------------------------------------------------------------------------
//  OnEnChangeExportFile
// -----------------------------------------------------------------------------

void
CExportOptionsDlg::OnEnChangeExportFile()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd* pExportFileEdit = GetDlgItem( IDC_EDT_EXPORT_FILE );
	ASSERT( pExportFileEdit );
	if ( !pExportFileEdit ) return;

	CWnd* pImportButton = GetDlgItem( IDOK );
	ASSERT( pImportButton );
	if ( !pImportButton ) return;

	static CString	strLastText = L"";
	CString			strExportFilePath;

	BOOL bEnabled = FALSE;

	pExportFileEdit->GetWindowText( strExportFilePath );
	if ( strExportFilePath.GetLength() > 4 && strExportFilePath != strLastText ) bEnabled = TRUE;

	pImportButton->EnableWindow( bEnabled );
}


// -----------------------------------------------------------------------------
//		OnSetCursor
// -----------------------------------------------------------------------------

BOOL
CExportOptionsDlg::OnSetCursor(
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
