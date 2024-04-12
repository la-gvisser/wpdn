// =============================================================================
//	Extent.cpp
//		implementation of the CExtent class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#include "StdAfx.h"
#include "Extent.h"
#include "EMailCapture.h"

extern CEMailCaptureApp theApp;

CStringA	g_strTrue( "True" );
CStringA	g_strFalse( "False" );
CStringA	g_strPen( "Pen" );
CStringA	g_strFull( "Full" );
CStringA	g_strChecked( "Checked" );
CStringA	g_strUnchecked( "Unchecked" );
CStringA	g_strSeparator( "------------------------------------------------------------------" );


///////////////////////////////////////////////////////////////////////////////
//		CExtent
///////////////////////////////////////////////////////////////////////////////

CExtent::CExtent(
	CPeekPlugin* inPlugin )
	:	m_pPlugin( inPlugin )
	,	m_Options( inPlugin )
{
}


// -----------------------------------------------------------------------------
//		GetOptions
// -----------------------------------------------------------------------------

COptions&
CExtent::GetOptions()
{
	tIntegrityOptions	Options;

	CString strDefaultName;
	m_pPlugin->DoPrefsGetPrefsPath( strDefaultName.GetBuffer( MAX_PATH ) );
	strDefaultName.ReleaseBuffer();

	int nLen = strDefaultName.GetLength() - strDefaultName.Find( _T('\\') ) - 1;	// an extra for the '\'.
	strDefaultName = strDefaultName.Right( nLen );
	strDefaultName.Replace( _T('\\'), _T('-') );

	GetIntegrityOptions( Options );
	m_Options.m_Integrity.SetOptions( Options, strDefaultName );

	return m_Options;
}

// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
CExtent::SetOptions(
	COptions&	inOptions )
{
	TCHAR 	szFilePath[MAX_PATH];
	::GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );

	CString	strFileName( szFilePath );
	int	nIndex = strFileName.ReverseFind( _T('.') );
	if ( nIndex >= 0 ) {
		strFileName = strFileName.Left( nIndex );
	}
	strFileName.Append( _T("ConfigLog.txt") );

	//bool	bDirty = false;

	CFile	theFile;
	if ( theFile.Open( strFileName, (CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) ) ) {
		CString	strTime;
		::GetTimeString( strTime );
		CStringA	strTimeStamp( strTime );

		try {
			CStringA	str;

			theFile.SeekToEnd();

			// General tab settings
			if ( inOptions.m_General.m_strOutputDirectory.CompareNoCase(
						m_Options.m_General.m_strOutputDirectory ) != 0 ) {
				str.Format(
					"%s - Output Directory changed from %s to %s\r\n",
					strTimeStamp,
					CStringA( m_Options.m_General.m_strOutputDirectory ),
					CStringA( inOptions.m_General.m_strOutputDirectory ) );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_General.m_bMonitorSmtp != m_Options.m_General.m_bMonitorSmtp ) {
				str.Format(
					"%s - Monitor SMTP changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_General.m_bMonitorSmtp) ? g_strTrue : g_strFalse,
					(inOptions.m_General.m_bMonitorSmtp) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_General.m_bMonitorPop3 != m_Options.m_General.m_bMonitorPop3 ) {
				str.Format(
					"%s - Monitor POP3 changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_General.m_bMonitorPop3) ? g_strTrue : g_strFalse,
					(inOptions.m_General.m_bMonitorPop3) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_General.m_bDisplayCapture != m_Options.m_General.m_bDisplayCapture ) {
				str.Format(
					"%s - Display Capture changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_General.m_bDisplayCapture) ? g_strTrue : g_strFalse,
					(inOptions.m_General.m_bDisplayCapture) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_General.m_bDisplaySummary != m_Options.m_General.m_bDisplaySummary ) {
				str.Format(
					"%s - Display Summary changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_General.m_bDisplaySummary) ? g_strTrue : g_strFalse,
					(inOptions.m_General.m_bDisplaySummary) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_General.m_bDisplayLog != m_Options.m_General.m_bDisplayLog ) {
				str.Format(
					"%s - Display Log changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_General.m_bDisplayLog) ? g_strTrue : g_strFalse,
					(inOptions.m_General.m_bDisplayLog) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}

			// Address tab settings
			if ( inOptions.m_Address.m_TargetList.CaptureAll() ^
				 m_Options.m_Address.m_TargetList.CaptureAll() ) {
				str.Format(
					"%s - the status of Capure all Addresses was changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_Address.m_TargetList.CaptureAll()) ? g_strChecked : g_strUnchecked,
					(inOptions.m_Address.m_TargetList.CaptureAll()) ? g_strChecked : g_strUnchecked );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			// Note new addresses and Enable status changes.
			for ( size_t i = 0; i < inOptions.m_Address.m_TargetList.GetCount(); i++ ) {
				const CUserItem&	uiItem = inOptions.m_Address.m_TargetList.GetAt( i );
				bool				bChecked = false;
				if ( m_Options.m_Address.m_TargetList.HasAddress( uiItem.GetName(), bChecked ) ) {
					if ( bChecked ^ uiItem.IsChecked() ) {
						str.Format(
							"%s - Email Address %s Enabled Status was changed from %s to %s\r\n",
							strTimeStamp,
							uiItem.GetName(),
							(bChecked) ? g_strChecked : g_strUnchecked,
							(uiItem.IsChecked()) ? g_strChecked : g_strUnchecked );
						theFile.Write( str.GetBuffer(), str.GetLength() );
						//bDirty = true;
					}
				}
				else {
					str.Format(
						"%s - Email Address %s was added with Enabled status of %s\r\n",
						strTimeStamp,
						uiItem.GetName(),
						(uiItem.IsChecked()) ? g_strChecked : g_strUnchecked );
					theFile.Write( str.GetBuffer(), str.GetLength() );
					//bDirty = true;
				}
			}
			// Note deleted address.
			for ( size_t i = 0; i < m_Options.m_Address.m_TargetList.GetCount(); i++ ) {
				const CUserItem&	uiItem = m_Options.m_Address.m_TargetList.GetAt( i );
				bool				bChecked;
				if ( !inOptions.m_Address.m_TargetList.HasAddress( uiItem.GetName(), bChecked ) ) {
					str.Format(
						"%s - %s was removed from the List of Email Addresses\r\n",
						strTimeStamp,
						uiItem.GetName() );
					theFile.Write( str.GetBuffer(), str.GetLength() );
					//bDirty = true;
				}
			}

			// Mode tab settings.
			if ( inOptions.m_Mode.m_nCaptureMode != m_Options.m_Mode.m_nCaptureMode ) {
				str.Format(
					"%s - Capture Mode changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_Mode.m_nCaptureMode == 0) ? g_strPen : g_strFull,
					(inOptions.m_Mode.m_nCaptureMode == 0) ? g_strPen : g_strFull );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_Mode.m_bPenEml != m_Options.m_Mode.m_bPenEml ) {
				str.Format(
					"%s - Pen Eml changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_Mode.m_bPenEml) ? g_strTrue : g_strFalse,
					(inOptions.m_Mode.m_bPenEml) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_Mode.m_bPenText != m_Options.m_Mode.m_bPenText ) {
				str.Format(
					"%s - Pen Text changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_Mode.m_bPenText) ? g_strTrue : g_strFalse,
					(inOptions.m_Mode.m_bPenText) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_Mode.m_bFullEml != m_Options.m_Mode.m_bFullEml ) {
				str.Format(
					"%s - Full Eml changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_Mode.m_bFullEml) ? g_strTrue : g_strFalse,
					(inOptions.m_Mode.m_bFullEml) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}
			if ( inOptions.m_Mode.m_bFullPkt != m_Options.m_Mode.m_bFullPkt ) {
				str.Format(
					"%s - Full Pkt changed from %s to %s\r\n",
					strTimeStamp,
					(m_Options.m_Mode.m_bFullPkt) ? g_strTrue : g_strFalse,
					(inOptions.m_Mode.m_bFullPkt) ? g_strTrue : g_strFalse );
				theFile.Write( str.GetBuffer(), str.GetLength() );
				//bDirty = true;
			}

			//if ( bDirty ) {
			//	str.Format( "%s\r\n", g_strSeparator );
			//	theFile.Write( str.GetBuffer(), str.GetLength() );
			//}
		}
		catch ( ... ) {
			NoteWriteError( strFileName );
		}
	}

	tIntegrityOptions	IntegrityOptions;
	inOptions.m_Integrity.GetOptions( IntegrityOptions );
	m_Options.SetIntegrityOptions( IntegrityOptions );

	m_Options = inOptions;
}


// -----------------------------------------------------------------------------
//		NoteDetect
// -----------------------------------------------------------------------------

void
CExtent::NoteDetect(
	CEMailStats::EMailStatType	inType )
{
	static CEMailStats::EMailStat	Detects[CEMailStats::kStat_TypeCount] = {
		CEMailStats::kStat_SmtpEmailsDetected,
		CEMailStats::kStat_Pop3EmailsDetected
	};

	m_Stats.Increment( Detects[inType] );
	if ( DisplayLog() ) m_LogQueue.AddDetectMessage( inType );
}

void
CExtent::NoteDetect(
	CEMailStats::EMailStatType	inType,
	const CTargetAddress&		inTarget )
{
	static CEMailStats::EMailStat	Detects[CEMailStats::kStat_TypeCount] = {
		CEMailStats::kStat_SmtpEmailsDetected,
		CEMailStats::kStat_Pop3EmailsDetected
	};

	m_Stats.Increment( Detects[inType] );
	if ( DisplayLog() ) m_LogQueue.AddDetectMessage( inType, inTarget );
}


// -----------------------------------------------------------------------------
//		NoteTarget
// -----------------------------------------------------------------------------

void
CExtent::NoteTarget(
	const CString&	inTarget,
	const CString&	inTemplate )
{
	CString	strMsg;

	strMsg.Format( inTemplate, inTarget );
	LogMessage( strMsg );
}

//void
//CExtent::NoteTarget(
//	const CTargetAddress&	inTarget,
//	const CString&			inTemplate )
//{
//	CString	strTarget;
//	CString	strMsg;
//
//	inTarget.FormatLong( strTarget );
//	strMsg.Format( inTemplate, strTarget );
//	LogMessage( strMsg );
//}


// -----------------------------------------------------------------------------
//		NoteTargets
// -----------------------------------------------------------------------------

void
CExtent::NoteTargets(
	const CArrayString&	inTargets,
	const CString&		inTemplate )
{
	for ( size_t i = 0; i < inTargets.GetCount(); i++ ) {
		CString	strMsg;

		strMsg.Format( inTemplate, inTargets[i] );
		LogMessage( strMsg );
	}
}

//void
//CExtent::NoteTargets(
//	const CTargetAddressList&	inTargets,
//	const CString&				inTemplate )
//{
//	for ( size_t i = 0; i < inTargets.GetCount(); i++ ) {
//		CString	strTarget;
//		CString	strMsg;
//
//		inTargets[i].FormatLong( strTarget );
//		strMsg.Format( inTemplate, strTarget );
//		LogMessage( strMsg );
//	}
//}


// -----------------------------------------------------------------------------
//		NoteFileSave
// -----------------------------------------------------------------------------

void
CExtent::NoteFileSave(
	const CString&	inTarget )
{
	if ( DisplayLog() ) m_LogQueue.AddSaveMessage( inTarget );
}


// -----------------------------------------------------------------------------
//		NoteReset
// -----------------------------------------------------------------------------

void
CExtent::NoteReset(
	CEMailStats::EMailStatType	inType )
{
	static CEMailStats::EMailStat	Resets[CEMailStats::kStat_TypeCount] = {
		CEMailStats::kStat_SmtpEmailResets,
		CEMailStats::kStat_Pop3EmailResets
	};

	m_Stats.Increment( Resets[inType] );
	if ( DisplayLog() ) m_LogQueue.AddResetMessage( inType );
}

void
CExtent::NoteReset(
	CEMailStats::EMailStatType	inType,
	const CTargetAddress&		inTarget )
{
	static CEMailStats::EMailStat	Resets[CEMailStats::kStat_TypeCount] = {
		CEMailStats::kStat_SmtpEmailResets,
		CEMailStats::kStat_Pop3EmailResets
	};

	m_Stats.Increment( Resets[inType] );
	if ( DisplayLog() ) m_LogQueue.AddResetMessage( inType, inTarget );
}


// -----------------------------------------------------------------------------
//		NoteMalformed
// -----------------------------------------------------------------------------

void
CExtent::NoteMalformed(
	CEMailStats::EMailStatType	inType )
{
	static CEMailStats::EMailStat	Aborts[CEMailStats::kStat_TypeCount] = {
		CEMailStats::kStat_SmtpEmailMalformed,
		CEMailStats::kStat_Pop3EmailMalformed
	};

	m_Stats.Increment( Aborts[inType] );
}


// -----------------------------------------------------------------------------
//		NoteFileError
// -----------------------------------------------------------------------------

void
CExtent::NoteFileError(
	const CString& inFileName )
{
	m_Stats.Increment( CEMailStats::kStat_FileError );
	if ( DisplayLog() ) m_LogQueue.AddFileErrorMessage( inFileName );
}


// -----------------------------------------------------------------------------
//		NoteWriteError
// -----------------------------------------------------------------------------

void
CExtent::NoteWriteError(
	const CString&	inFileName )
{
	m_Stats.Increment( CEMailStats::kStat_FileError );
	if ( DisplayLog() ) m_LogQueue.AddWriteErrorMessage( inFileName );
}


// -----------------------------------------------------------------------------
//		NoteTimedOutMessages
// -----------------------------------------------------------------------------

void
CExtent::NoteTimedOutMessages(
	CEMailStats::EMailStatType	inType,
	const CArrayString&			inRemovedList )
{
	static CEMailStats::EMailStat	Resets[CEMailStats::kStat_TypeCount] = {
		CEMailStats::kStat_SmtpSessionsTimeOut,
		CEMailStats::kStat_Pop3SessionsTimeOut
	};

	m_Stats.Increment( Resets[inType], inRemovedList.GetCount() );
	if ( DisplayLog() ) m_LogQueue.AddTimedOutMessages( inType, inRemovedList );
}
