// =============================================================================
//	OptionsView.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32
#ifdef IMPLEMENT_TAB

#include "PluginTab.h"
#include "OmniPlugin.h"
#include "PeekContext.h"
#include "OptionsDialog.h"
#include "ContextManager.h"
#include "PluginMessages.h"
#include "RemotePlugin.h"

extern CContextManagerPtr	GetContextManager();


// =============================================================================
//		COptionsView
// =============================================================================

COptionsView::COptionsView(
	CWnd*			inParent /*= NULL*/  )
	:	COptionsDialog( &m_Options, false, true ) 
	,	m_pParent( dynamic_cast<CPluginTab*>( inParent ) )
	,	m_bInitialized( false )
{
}


// -----------------------------------------------------------------------------
//		DoDataExchange
// -----------------------------------------------------------------------------

void
COptionsView::DoDataExchange(
	CDataExchange* pDX )
{
	ASSERT( m_bIsDockedTab );

	if ( pDX->m_bSaveAndValidate || !m_bInitialized ) return;

	const bool	bCaptureToScreen( m_Options.IsCaptureToScreen() );
	const bool	bCaptureToFile( m_Options.IsCaptureToFile() );
	ASSERT( bCaptureToScreen || bCaptureToFile );

	const bool	bLogMsgsToScreen( m_Options.IsLogMsgsToScreen() );
	const bool	bLogMsgsToFile( m_Options.IsLogMsgsToFile() );
	const bool	bIsFoldersForCases( m_Options.IsFoldersForCases() );

	CString		strCaptureMsg;
	if ( bCaptureToScreen && bCaptureToFile )	strCaptureMsg = L"Both to screen packets window and to disk file.";
	else if ( bCaptureToScreen )				strCaptureMsg = L"To screen packets window only.";
	else										strCaptureMsg = L"To disk file only.";
	DDX_Text( pDX, IDC_VIEW_CAPTURED_DEST, strCaptureMsg );

	CString		strLogMsg;
	CString		strLogToMsg;
	if ( bLogMsgsToScreen || bLogMsgsToFile ) {
		strLogMsg = L" CAPTURE EVENTS WILL BE LOGGED:";

		if ( bLogMsgsToFile ) {
			if ( bLogMsgsToScreen )		strLogToMsg = L"Both to screen log window and to disk file.";
			else						strLogToMsg = L"To disk file only.";
		}
		else if ( bLogMsgsToScreen ) {
			strLogToMsg = L"To screen log window only.";
		}
	}
	DDX_Text( pDX, IDC_VIEW_LOGGING_MSG, strLogMsg );
	DDX_Text( pDX, IDC_VIEW_LOG_DEST, strLogToMsg );

	CString		strFoldersForCasesMsgs;
	if ( bIsFoldersForCases ) 	strFoldersForCasesMsgs = L"Default suggestion for case output directory is a separate folder for each case.";
	else						strFoldersForCasesMsgs = L"Default suggestion for case output directory is all cases to the same folder.";
	DDX_Text( pDX, IDC_VIEW_CASE_FOLDERS, strFoldersForCasesMsgs );

	CString strOutputRootDirectory = m_pOptions->GetOutputRootDirectory();
	DDX_Text( pDX, IDC_OUTPUT_ROOT_EDIT, strOutputRootDirectory );

	CString		strUseFileNumberMsg;
	const bool	bIsUseFileNumber = m_Options.IsUseFileNumber();
	if ( bIsUseFileNumber )	strUseFileNumberMsg = L"A consecutive numeric id will be used in the file name for each new capture file after refresh.";
	DDX_Text( pDX, IDC_VIEW_FILE_NUMBER_MSG, strUseFileNumberMsg );

	if ( !m_Options.IsSaveAgeSelected() && !m_Options.IsSaveSizeSelected() ) {
		m_Options.SetSaveAgeSelected( true );
		if ( m_Options.GetSaveAgeSeconds() == 0 ) {
			m_Options.SetSaveAgeValue( kSecondsInADay );	
		}
	}

	CString		strAgeSizeMessage1;
	CString		strAgeSizeMessage2;
	CString		strAgeMessage;
	CString		strSizeMessage;
	CString		strNewFileCaptionMsg;

	const bool bIsSaveAge = m_Options.IsSaveAgeSelected();
	const bool bIsSaveSize = m_Options.IsSaveSizeSelected();

	if ( bIsSaveAge ) {
		UInt32		nAgeCount;
		CString		strAgeUnits;

		const UInt32 nSaveAgeValue = m_Options.GetSaveAgeSeconds();
		if ( (nSaveAgeValue % kSecondsInADay) == 0 ) {
			nAgeCount = nSaveAgeValue / kSecondsInADay;
			strAgeUnits = L"day";
		}
		else if ( (nSaveAgeValue % kSecondsInAnHour) == 0 ) {
			nAgeCount = nSaveAgeValue / kSecondsInAnHour;
			strAgeUnits = L"hour";
		}
		else {
			ASSERT( (nSaveAgeValue % kSecondsInAMinute) == 0 );
			nAgeCount = nSaveAgeValue / kSecondsInAMinute;
			strAgeUnits = L"minute";
		}

		strAgeMessage.Format( L"%u %s%s has elapsed since the last refresh.", nAgeCount, strAgeUnits, nAgeCount == 1 ? L"" : L"s" );
	}

	if ( bIsSaveSize ) {
		UInt64		nSizeCount;
		CString		strSizeUnits;

		const UInt64 nSaveSizeValue = m_Options.GetSaveSizeBytes();
		if ( (nSaveSizeValue % kGigabyte) == 0 ) {
			nSizeCount = nSaveSizeValue / kGigabyte;
			strSizeUnits = L"gigabyte";
		}
		else if ( (nSaveSizeValue % kMegabyte) == 0 ) {
			nSizeCount = nSaveSizeValue / kMegabyte;
			strSizeUnits = L"megabyte";
		}
		else {
			nSizeCount = nSaveSizeValue / kKilobyte;
			strSizeUnits = L"kilobyte";
		}

		strSizeMessage.Format( L"File size has exceeded %llu %s%s.", nSizeCount, strSizeUnits, nSizeCount == 1 ? L"" : L"s" );
	}

	if ( bIsSaveAge && bIsSaveSize ) {
		strNewFileCaptionMsg = L"Capture file will be refreshed when the first of the following occurs:";
		strAgeSizeMessage1 = strAgeMessage;
		strAgeSizeMessage2 = strSizeMessage;
	}
	else if ( bIsSaveAge ) {
		strNewFileCaptionMsg = L"Capture file will be refreshed when the following occurs:";
		strAgeSizeMessage1 = strAgeMessage;
	}
	else if ( bIsSaveSize ) {
		strNewFileCaptionMsg = L"Capture file will be refreshed when the following occurs:";
		strAgeSizeMessage1 = strSizeMessage;
	}

	DDX_Text( pDX, IDC_NEW_FILE_CAPTION, strNewFileCaptionMsg );
	DDX_Text( pDX, IDC_VIEW_AGE_SIZE_MSG_1, strAgeSizeMessage1 );
	DDX_Text( pDX, IDC_VIEW_AGE_SIZE_MSG_2, strAgeSizeMessage2 );

	CDialog::DoDataExchange( pDX );

}


#ifdef IMPLEMENT_PLUGINMESSAGE
// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
COptionsView::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		/*inMessage*/,
	const CPeekStream&	/*inResponse*/ )
{
	if ( inResult != HE_S_OK ) return;

	// New options were set.
	if ( m_pParent != NULL ) {
		m_pParent->GetAllOptions();
	}

	return;
}
#endif // IMPLEMENT_PLUGINMESSAGE


// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

void
COptionsView::Refresh(
	CGlobalId			inId,
	const CPeekString&	inName,
	const COptions&		inOptions )
{
	m_idContext = inId;
	m_strName = inName;
	m_Options = inOptions;

	if ( ::IsWindow( m_hWnd ) ) {
		m_bInitialized = true;
		UpdateData( FALSE );

		RefreshOptionsDisplay();
	}
}

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
