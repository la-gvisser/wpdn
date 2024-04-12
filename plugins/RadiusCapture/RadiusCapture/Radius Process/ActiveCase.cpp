// ============================================================================
//	ActiveCase.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "ActiveCase.h"
#include "OmniEngineContext.h"

////////////////////////////////////////////////////////////////////////////////
//  CActiveCase
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------
  CActiveCase::CActiveCase( 
	  CCaseOptions* pCaseOptions, 
	  IRadiusContext* pRadiusContext )
	: m_pCaseOptions( pCaseOptions )
	, m_bHasEntries( false )
	, m_pDualWriter( NULL ) 
	, m_nPacketWriterErrors( 0 )
	, m_nCaseCaptureId( 0 )
	, m_bIsCapturing( false )
	, m_pOwningRadiusContext( pRadiusContext )
	, m_nLastValidationNumber( 0 )
{
	ASSERT( m_pCaseOptions->IsEnabled() );
  #ifdef INTEGRITY_OPTIONS
//	m_pDualWriterOld->SetCaseOptions( m_pCaseOptions );
  #endif
}

CActiveCase::~CActiveCase()
{
	if ( m_pDualWriter ) {
		m_pDualWriter->StopCapture();
		delete m_pDualWriter;
	}
}

// -----------------------------------------------------------------------------
//		ProcessIPPacket
// -----------------------------------------------------------------------------

bool
CActiveCase::ProcessIPPacket(
	CPacket& inPacket,
	UInt32	nValidationNumber ) 
{
	if ( !IsActive() || !IsCapturing() || !m_pDualWriter ) {
		ASSERT( 0 );
		return false;
	}

	// If the following is true then this case has already been processed.
	// This happens when one target is talking to another target.
	if ( nValidationNumber == m_nLastValidationNumber ) {
		return true;
	}

	m_nLastValidationNumber = nValidationNumber;

	bool bPacketWritten = m_pDualWriter->AddPacket( inPacket, false );
	if ( !bPacketWritten ) {
		m_nPacketWriterErrors++;
		if ( IsLoggingMsgsToFile() ) {
			CPeekString strErrorText = ::GetTimeStringGreenwich() + L"* Packet writer error - packet not written\r\n";
			m_pDualWriter->WriteLog( strErrorText );
		}
	}

	if ( !bPacketWritten && m_nPacketWriterErrors >= kMaxPacketWriterErrors ) {
		ProcessWriteFailure();
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
//		ProcessRadiusPacket
// -----------------------------------------------------------------------------

bool
CActiveCase::ProcessRadiusPacket( 
	CPacket& inPacket )
{
	if ( !IsActive() || !IsCapturing() || !m_pDualWriter ) {
		ASSERT( 0 );
		return false;
	}
	bool bPacketWritten =  m_pDualWriter->AddPacket( inPacket, true );
	if ( !bPacketWritten ) {
		m_nPacketWriterErrors++;
		if ( IsLoggingMsgsToFile() ) {
			CPeekString strErrorText = ::GetTimeStringGreenwich() + L"* Packet writer error - packet not written\r\n";
			m_pDualWriter->WriteLog( strErrorText );
		}
	}

	if ( !bPacketWritten && m_nPacketWriterErrors >= kMaxPacketWriterErrors ) {
		ProcessWriteFailure();
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
//		StartCapturing
// -----------------------------------------------------------------------------

bool
CActiveCase::StartCapturing( 
	const CPeekString&	strInitString )
{
	ASSERT( IsActive() );
	ASSERT( !IsCapturing() );

	m_nLastValidationNumber = 0;

	if ( !HasTriggerTimeStartedYet() ) {
		return true;
	}

	if ( m_pDualWriter ) {
		ASSERT( 0 );
		delete m_pDualWriter;
		m_pDualWriter = NULL;
	}

	if ( !m_pCaseOptions->CanCaseBeEnabled() ) {
		m_pCaseOptions->SetDisabled();
		return false;
	}

	SetCapturing( true );


	const bool bIsCaptureToFile = IsCaptureToFile();
	const bool bIsLogMsgsToScreen = IsLogMsgsToScreen();

	if ( !bIsCaptureToFile && !bIsLogMsgsToScreen ) {
		return true;
	}

	CRadiusMessage	theMessage;
	bool			bHaveMessage = false;

	if ( bIsCaptureToFile ) {

		CPeekString	strOutputDirectory = m_pCaseOptions->GetOutputDirectory();

		if ( !MakePath( strOutputDirectory ) ) {
			return false;
		}

		bool bIsLogMsgsToFile = IsLogMsgsToFile();
		m_pDualWriter = new CDualWriter( bIsLogMsgsToFile );
		ASSERT( m_pDualWriter );
		if ( !m_pDualWriter ) {
			SetCapturing( false );
			return false;
		}

//		CPeekString strFacility = m_pCaseOptions->GetFacility();
//		CPeekString strPrefix = L"";
//		SetCaseCaptureId( 0 );
		SetCaseCaptureId( m_pCaseOptions->GetCaseCaptureId() );

		CPeekString strCasePrefix = m_pCaseOptions->GetPrefix();
/*
		if ( m_pCaseOptions->IsUseCaseCaptureId() ) {
			SetCaseCaptureId( m_pCaseOptions->GetCaseCaptureId() );

			CPeekString strCasePrefix = m_pCaseOptions->GetPrefix();
//			if ( strFacility.GetLength() > 0 ) {
//				CPeekOutString strOutPrefix;
//				strOutPrefix << strCasePrefix << L"_" << strFacility;
//				strPrefix = strOutPrefix;
//			}
//			else {
				strPrefix = strCasePrefix;
//			}
		}
//		else {
//			strPrefix = strFacility;
//		}
*/

//		m_pDualWriter->SetPreAmble( strPrefix, m_pCaseOptions->IsUseFileNumber() );
		m_pDualWriter->SetPreAmble( strCasePrefix, m_pCaseOptions->IsUseFileNumber() );

		UInt32	nAge = m_pCaseOptions->GetSaveAgeOrZero();
		UInt64	nSize = m_pCaseOptions->GetSaveSizeOrZero();

		m_pDualWriter->RegisterFileRefreshCallback( FileRefreshCallback, (void*)this );

		bHaveMessage = GetFormattedCaptureStartMsg( strInitString, theMessage );

		CPeekString strCaptureStartString = theMessage.GetLogFileMessage();
		bool bRet = m_pDualWriter->StartCapture( strOutputDirectory, nAge, nSize, strCaptureStartString );
		if ( !bRet ) {
			ASSERT( 0 );
			delete m_pDualWriter;
			m_pDualWriter = NULL;
			SetCapturing( false );
			return false;
		}

		ASSERT( GetFileNumber() == 1 );
	}

	if ( bIsLogMsgsToScreen ) {
		if ( !bHaveMessage ) {
			bHaveMessage = GetFormattedCaptureStartMsg( strInitString, theMessage );
		}

		CPeekString strScreenMessage = theMessage.GetScreenMessage();
		(reinterpret_cast<COmniEngineContext*>(m_pOwningRadiusContext))->AddLogToScreenMessage( strScreenMessage );
	}

	return true;
}

// -----------------------------------------------------------------------------
//		StopCapturing
// -----------------------------------------------------------------------------

void
CActiveCase::StopCapturing(  
	UInt8	nReason ) 
{
	CRadiusMessage theMessage;

	m_nLastValidationNumber = 0;

 	GetFormattedCaptureStopMsg( theMessage, nReason );

	if ( IsCapturing() && IsLogMsgsToScreen() && (nReason != kChangeReasonStopCapture) ) {
		CPeekString theScreenMessage = theMessage.GetScreenMessage();
		(reinterpret_cast<COmniEngineContext*>(m_pOwningRadiusContext))->AddLogToScreenMessage( theScreenMessage );
	}

	SetCapturing( false );

	if ( m_pDualWriter ) {		
		CPeekString theLogFileMessage = theMessage.GetLogFileMessage();
		m_pDualWriter->StopCapture( theLogFileMessage );
		delete m_pDualWriter;
		m_pDualWriter = NULL;
	}

	if ( nReason == kChangeReasonTimeout ) {
		if ( m_pCaseOptions->IsEnabled() ) {
			ASSERT( GetTriggerTimeStatus() == kTriggerTimeStatusExpired );
			m_pCaseOptions->SetDisabled();
		}
	}
}

// -----------------------------------------------------------------------------
//		StopCaseLogger
// -----------------------------------------------------------------------------

void
CActiveCase::StopCaseLogger()
{
	if( m_pDualWriter ) {
		CPeekString strMessage = ::GetTimeStringGreenwich() + L" Case logging is being turned off\r\n";
		m_pDualWriter->WriteLog( strMessage );
		m_pDualWriter->SetIsLogMessages( false );
	}
}

#ifdef DISPLAY_PACKET_NUMBERS
// -----------------------------------------------------------------------------
//		GetPacketNumberString
// -----------------------------------------------------------------------------

CPeekString
CActiveCase::GetPacketNumberString( 
	bool bDisplayingToScreen )
{
	CPeekOutString strPacketNumber;

	if ( bDisplayingToScreen ) {
		ASSERT( m_pOwningRadiusContext );
		strPacketNumber << L" [" << 
			(reinterpret_cast<COmniEngineContext*>(m_pOwningRadiusContext))->GetCurrentDisplayPacketNumber() << L"]";
	}
	else {
		strPacketNumber << L" [" << m_pDualWriter->GetNumPacketsWritten() << L"]";
	}
	return strPacketNumber;
}
#endif

// -----------------------------------------------------------------------------
//		GetFormattedCaptureStopMsg
// -----------------------------------------------------------------------------

bool
CActiveCase::GetFormattedCaptureStopMsg( 
	CRadiusMessage& theMessage, 
	UInt8 nChangeReason )
{
	CPeekString strTimeGreenwich = ::GetTimeStringGreenwich() + L" ";
	CPeekString strMessage;
	CPeekString strLogFileMessage;
	CPeekString strScreenMessage;

	if ( !m_pCaseOptions ) {
		ASSERT( 0 );
		return false;		
	}

	switch( nChangeReason )
	{
		case kChangeReasonTimeout:
			strMessage = L"Capture end time for this case has elapsed";
			break;

		case kChangeReasonStopCapture:
			strMessage = L"The capture session was stopped (stop button pressed or window closed)";
			break;

		case kChangeReasonDeselectStop:
			strMessage =  L"Case deselected or deleted in the Radius Capture Options Dialog";
			break;

		case kChangeReasonWriterFailed:
			strMessage =L"Error: the packet writer has failed";
			break;

		default:
			ASSERT( 0 );
			strMessage =L"The capture session ended";
			break;
	}

	ASSERT( strMessage.GetLength() > 0 );

	strLogFileMessage = strTimeGreenwich +L"*** " + strMessage + L" ***\r\n" + strTimeGreenwich + L"*** " + GetCaseName() + L" capture ended ***\r\n";
	theMessage.SetLogFileMessage( strLogFileMessage );

	strScreenMessage = L"Radius - " + GetCaseName() + L":  *** " + strMessage + L" - capture ended ***";
	theMessage.SetScreenMessage( strScreenMessage );

	return true;
}

// -----------------------------------------------------------------------------
//		GetFormattedCaptureChangeString
// -----------------------------------------------------------------------------

CPeekString
CActiveCase::GetFormattedCaptureChangeString( 
	CCaseOptions*		pNewCase, 
	const CPeekString&	strRefreshMessage, 
	CPeekString&		strTimeGreenwich )
{
	CPeekString strMessage = L"";

	if ( !pNewCase ) {
		ASSERT( 0 );
		return strMessage;
	}

	if ( strTimeGreenwich.GetLength() == 0 ) {
		strTimeGreenwich = ::GetTimeStringGreenwich() + L" ";
	}

	bool	bChangeNoted = false;

	if ( !m_pCaseOptions ) {
		ASSERT( 0 );
		return L"";		
	}

	strMessage = strTimeGreenwich;

	strMessage += L"*** Capture options changed ***\r\n";

	if ( m_pCaseOptions->GetStartTime() != pNewCase->GetStartTime() ) {
		strMessage += strTimeGreenwich + L"* Case Start Time: " + pNewCase->GetFormattedStartStopTime( true ) + L"\r\n";
		bChangeNoted = true;
	}

	if ( m_pCaseOptions->GetStopTime() != pNewCase->GetStopTime() ) {
		strMessage += strTimeGreenwich + L"* Case Stop Time:  " + pNewCase->GetFormattedStartStopTime( false ) + L"\r\n";
		bChangeNoted = true;
	}

	if ( strRefreshMessage.GetLength() > 0 ) {
		bChangeNoted = true;
		strMessage += strRefreshMessage;
	}

	CPeekString strNewTargetList = pNewCase->GetTargetListString();
	if ( strNewTargetList.GetLength() == 0 ) {
		strNewTargetList = L"[none]";
	}

	CPeekString strOldTargetList = m_pCaseOptions->GetTargetListString();
	if ( strOldTargetList.GetLength() == 0 ) {
		strOldTargetList = L"[none]";
	}

	bool bTargetListChanged = ( strOldTargetList.CompareNoCase(strNewTargetList) != 0 );

	if ( bTargetListChanged ) {
		strMessage += strTimeGreenwich + L"* Targets:  " + strNewTargetList + L"\r\n";
		bChangeNoted = true;
	}

	if ( !pNewCase->IsActive() ) {
		if ( pNewCase->HasStopTimeExpired() ) {
			strMessage += strTimeGreenwich + L"*** Capture stop time has expired ***\r\n";
		}
		if ( !pNewCase->IsEnabled() ) {
			strMessage += strTimeGreenwich + L"*** Case is no longer active ***\r\n";
		}
		strMessage += strTimeGreenwich + L"*** " + GetCaseName() + L" capture ended ***\r\n";
	}
	else {
		if ( bChangeNoted ) {
			strMessage += strTimeGreenwich + L"*** Capture continuing ***\r\n";
		}
		else {
			strMessage = L"";
		}
	}

	return strMessage;
}

// -----------------------------------------------------------------------------
//		GetFormattedCaptureStartMsg
// -----------------------------------------------------------------------------

bool
CActiveCase::GetFormattedCaptureStartMsg( 
	const CPeekString&	strInitString, 
	CRadiusMessage&		theMessage )
{
	ASSERT( IsActive() );
	ASSERT( HasTriggerTimeStartedYet() == true );

	bool bReturn = false;

	if ( IsLogMsgsToFile() ) {
		CPeekString strLogFileMessage = L"";
		CPeekString strTimeGreenwich = ::GetTimeStringGreenwich() + L" ";

		UInt32 nNewSaveAge = m_pCaseOptions->GetSaveAgeOrZero();
		UInt64 nNewSaveSize = m_pCaseOptions->GetSaveSizeOrZero();

		CPeekString strFileRefreshMessage = GetFormattedFileRefreshText( nNewSaveAge, nNewSaveSize, strTimeGreenwich );

		AddStdLogHeaderLines( strTimeGreenwich, strLogFileMessage, strInitString, strFileRefreshMessage );
		strLogFileMessage += strTimeGreenwich + L"*** Capture started ***\r\n";

		theMessage.SetLogFileMessage( strLogFileMessage );

		bReturn = true;
	}

	if ( IsLogMsgsToScreen() ) {
		CPeekString strTargetListString = GetTargetListString();
		CPeekString strScreenMessage = L"Radius - " + GetCaseName() + L":  Monitoring started on " + strTargetListString;
		theMessage.SetScreenMessage( strScreenMessage );

		bReturn = true;
	}

	return bReturn;
}

// -----------------------------------------------------------------------------
//		AddStdLogHeaderLines
// -----------------------------------------------------------------------------

void
CActiveCase::AddStdLogHeaderLines( 
	const CPeekString&	strTimeGreenwich, 
	CPeekString&		strMessage, 
	const CPeekString&	strInitString, 
	const CPeekString&	strFileRefreshMessage )
{
	CPeekString strTargetListString = GetTargetListString();
	ASSERT( strTargetListString.GetLength() > 0 );
	ASSERT( IsLogMsgsToFile() );

	strMessage += strTimeGreenwich + L"*** " + GetCaseName() + L" ***\r\n";
//	if ( m_pCaseOptions->IsUseCaseCaptureId() ) {
		strMessage += strTimeGreenwich + L"* Capture id for this case is " + GetCaseCaptureIdString() + L" *\r\n";
//	}
//	if ( GetFileNumber() > 0 ) {
		strMessage += strTimeGreenwich + L"* This is file #" + GetFileNumberString() + L" in the capture series *\r\n";
//	}
	if ( strInitString.GetLength() > 0 ) {
		strMessage += strTimeGreenwich + L"* Capture title is \"" + strInitString + L"\"\r\n";
	}
	strMessage += strTimeGreenwich + L"* Times are Greenwich Mean Time\r\n";
	CPeekString theActivePortsString = + GetFormattedActivePortsString();
	ASSERT ( theActivePortsString.GetLength() > 0 );
	strMessage += strTimeGreenwich + L"* Radius ports are: " + theActivePortsString + L"\r\n";
	strMessage += strTimeGreenwich + L"* Packet numbers associated with an action are enclosed in brackets: \"[]\"\r\n";
	strMessage += strTimeGreenwich + L"* Case Start Time: " + GetFormattedStartStopTime( true ) + L"\r\n";
	strMessage += strTimeGreenwich + L"* Case Stop Time:  " + GetFormattedStartStopTime( false ) + L"\r\n";
	if ( strFileRefreshMessage.GetLength() > 0 ) {
		strMessage += strFileRefreshMessage;
	}
	strMessage += strTimeGreenwich + L"* Targets:  " + strTargetListString + L"\r\n";
}

// -----------------------------------------------------------------------------
//		FileRefreshResponse
// -----------------------------------------------------------------------------

void
CActiveCase::FileRefreshResponse( 
	bool	bAboutToRestart )
{
	if( !m_pDualWriter ) {
		ASSERT( 0 );
		return;
	}

	if ( !IsLoggingMsgsToFile() ) {
		return;
	}

	ASSERT( IsActive() );
	ASSERT( HasTriggerTimeStartedYet() == true );
	CPeekString strMessage;
	CPeekString strTimeGreenwich = ::GetTimeStringGreenwich() + L" ";

	if ( !bAboutToRestart ) {
		strMessage  = strTimeGreenwich + L"***************************************************************************\r\n";
		strMessage += strTimeGreenwich + L"***  Continuing capture in progress                                     ***\r\n";
		strMessage += strTimeGreenwich + L"***  Capture file was refreshed due to exceeded time or file size limit ***\r\n";
		strMessage += strTimeGreenwich + L"****************************************************************************\r\n";

		AddStdLogHeaderLines( strTimeGreenwich, strMessage, L"" );
		strMessage += strTimeGreenwich + L"*** Continuing capture ***\r\n";
	}
	else {
		strMessage  = strTimeGreenwich + L"**********************************************************************************\r\n";
		strMessage += strTimeGreenwich + L"***  Capture will continue in new file due to exceeded time or file size limit ***\r\n";
		strMessage += strTimeGreenwich + L"**********************************************************************************\r\n";
	}

	m_pDualWriter->WriteLog( strMessage ); 
}

// -----------------------------------------------------------------------------
//		GetFormattedFileRefreshText
// -----------------------------------------------------------------------------

CPeekString
CActiveCase::GetFormattedFileRefreshText( 
	const UInt32		nSaveAge, 
	const UInt64		nSaveSize, 
	const CPeekString&	strTimeGreenwich ) 
{
	CPeekString strMessage = L"";

	if ( !IsLoggingMsgsToFile() ) {
		return strMessage;
	}

	ASSERT( nSaveAge != 0 || nSaveSize != 0 );

	UInt32	nAgeCount;
	UInt32	nSizeCount;

	CPeekString strAgeText;
	CPeekString strSizeText;

	if ( nSaveAge != 0 && nSaveSize != 0 ) {
		strMessage = strTimeGreenwich + L"* File refresh will occur when the first threshold is met:\r\n";
	}

	if ( nSaveAge != 0 ) {
		if ( (nSaveAge % kSecondsInADay) == 0 ) {
			nAgeCount = static_cast<UInt32>(nSaveAge / kSecondsInADay);
			strAgeText = kDaysStr;
		}
		else if ( (nSaveAge % kSecondsInAnHour) == 0 ) {
			nAgeCount = nSaveAge / kSecondsInAnHour;
			strAgeText = kHoursStr;
		}
		else {
			ASSERT( (nSaveAge % kSecondsInAMinute) == 0 );
			nAgeCount = nSaveAge / kSecondsInAMinute;
			strAgeText = kMinutesStr;
		}

		CPeekOutString strOutText;
		if ( nSaveSize != 0 ) {
			strOutText << L"*   Every " << nAgeCount << L" " << strAgeText << L"\r\n";
		}
		else {
			strOutText << L"* File refresh every " << nAgeCount << L" " << strAgeText << L"\r\n";
		}
		strMessage += strTimeGreenwich + (CPeekString)strOutText;
	}

	if ( nSaveSize != 0 ) {
		if ( (nSaveSize % kGigabyte) == 0 ) {
			nSizeCount = static_cast<UInt32>(nSaveSize / kGigabyte);
			strSizeText = kGigabytesStr;
		}
		else if ( (nSaveSize % kMegabyte) == 0 ) {
			nSizeCount = static_cast<UInt32>(nSaveSize / kMegabyte);
			strSizeText = kMegabytesStr;
		}
		else {
			nSizeCount = static_cast<UInt32>(nSaveSize / kKilobyte);
			strSizeText = kKilobytesStr;
		}

		CPeekOutString strOutText;
		if ( nSaveAge != 0 ) {
			strOutText << L"*   Every " << nSizeCount << L" " << strSizeText << L"\r\n";
		}
		else {
			strOutText << L"* File refresh every " << nSizeCount << L" " << strSizeText << L"\r\n";
		}
		strMessage += strTimeGreenwich + (CPeekString)strOutText;
	}

	return strMessage;
}

// -----------------------------------------------------------------------------
//		ProcessOptionsChange
// -----------------------------------------------------------------------------

void
CActiveCase::ProcessOptionsChange( 
	CCaseOptions*	pNewCaseOptions )
{
	if ( !pNewCaseOptions ) {
		ASSERT( 0 );
		return;
	}
	if ( !m_pDualWriter ) {
		return;
	}

	CPeekString strTimeGreenwich = ::GetTimeStringGreenwich() + L" ";
	CPeekString strRefreshMessage = ProcessFileRefreshValuesChange( pNewCaseOptions, strTimeGreenwich );
	CPeekString strMessage = GetFormattedCaptureChangeString( pNewCaseOptions, strRefreshMessage, strTimeGreenwich );
	if ( strMessage.GetLength() > 0 ) {
		m_pDualWriter->WriteLog( strMessage );
	}
}

// -----------------------------------------------------------------------------
//		ProcessFileRefreshValuesChange
// -----------------------------------------------------------------------------

CPeekString
CActiveCase::ProcessFileRefreshValuesChange( 
	CCaseOptions*		pNewCaseOptions,
	const CPeekString&	strTimeGreenwich )
{
	CPeekString strMessage = L"";

	if ( !m_pDualWriter ) {
	   ASSERT( 0 );
	   return strMessage;
	}

	CCaptureOptions& theOldCaptureOptions = m_pCaseOptions->GetCaptureOptions();
	CCaptureOptions& theNewCaptureOptions = pNewCaseOptions->GetCaptureOptions();

	const UInt32 nOldSaveAge = theOldCaptureOptions.GetSaveAgeOrZero();
	const UInt32 nNewSaveAge = theNewCaptureOptions.GetSaveAgeOrZero();

	const UInt64 nOldSaveSize = theOldCaptureOptions.GetSaveSizeOrZero();
	const UInt64 nNewSaveSize = theNewCaptureOptions.GetSaveSizeOrZero();

	if ( (nOldSaveAge != nNewSaveAge) || (nOldSaveSize != nNewSaveSize) ) {
			m_pDualWriter->ResetFileRefreshValues( nNewSaveAge, nNewSaveSize );
			strMessage = GetFormattedFileRefreshText( nNewSaveAge, nNewSaveSize, strTimeGreenwich );
	}

	return strMessage;
}

// -----------------------------------------------------------------------------
//		LogMessage
// -----------------------------------------------------------------------------

void
CActiveCase::LogMessage( 
	CRadiusMessage& inMessage, 
	const bool		bDisplayPacketNumber )
{
	if ( !IsEnabled() || !IsCapturing() ) {
		ASSERT( 0 );
		return;
	}

	const bool bLoggingMsgsToFile = IsLoggingMsgsToFile();
	const bool bLoggingMsgsToScreen = IsLoggingMsgsToScreen();

	if ( bLoggingMsgsToScreen ) {
		CPeekString strScreenMessage = L"Radius - " + GetCaseName() + L":  " +  inMessage.GetMessage();

#ifdef DISPLAY_PACKET_NUMBERS
		const bool bCapturingToScreen = IsCapturingToScreen();
		if ( bDisplayPacketNumber && bCapturingToScreen ) {
			strScreenMessage += GetPacketNumberString( true );
		}
#endif
		(reinterpret_cast<COmniEngineContext*>(m_pOwningRadiusContext))->AddLogToScreenMessage( strScreenMessage );
	}

	if ( bLoggingMsgsToFile ) {
		CPeekString strFileMessage = inMessage.GetTime() + L" " + inMessage.GetMessage();

		if ( bDisplayPacketNumber ) {
			strFileMessage += GetPacketNumberString();
		}

		strFileMessage += L"\r\n";
		m_pDualWriter->WriteLog( strFileMessage ); 
	}
}

// -----------------------------------------------------------------------------
//		IsLoggingMsgsToFile
// -----------------------------------------------------------------------------

bool
CActiveCase::IsLoggingMsgsToFile()
{
	if ( !IsCapturing() ) {
		return false;
	}
	bool bIsLoggingMsgsToFile = IsLogMsgsToFile();
	if ( bIsLoggingMsgsToFile ) {
		if ( m_pDualWriter ) {
			return m_pDualWriter->IsLogMessages(); 
		}
	}
	return false;
}

// -----------------------------------------------------------------------------
//		FileRefreshCallback
// -----------------------------------------------------------------------------

void CALLBACK 
CActiveCase::FileRefreshCallback( 
	void*	pUserArg, 
	bool	bAboutToRestart ) 
{
	ASSERT( pUserArg );
	if ( !pUserArg ) return;
	CActiveCase* pActiveCase = static_cast<CActiveCase*>(pUserArg);
	pActiveCase->FileRefreshResponse( bAboutToRestart );
}

// -----------------------------------------------------------------------------
//		IsLogMsgsToScreen
// -----------------------------------------------------------------------------

bool
CActiveCase::IsLogMsgsToScreen() 
{
	return m_pCaseOptions->IsLogMsgsToScreen();
}


#ifdef _DEBUG
// -----------------------------------------------------------------------------
//		DebugValidateCase
// -----------------------------------------------------------------------------

void
CActiveCase::DebugValidateCase() 
{
	ASSERT( IsActive() );
	if ( IsCapturing() ) {
		if ( m_pDualWriter ) {
			m_pDualWriter->DebugValidateCaptureSettings( this );
		}
	}
	else { 
		ASSERT( m_pDualWriter == NULL );
	}
}
#endif // _DEBUG

#ifdef PACKET_WRITER_ERROR_HANDLING
// -----------------------------------------------------------------------------
//		FailureAllertAllTargetIPs
// -----------------------------------------------------------------------------

bool
CActiveCase::FailureAllertAllTargetIPs()
{
	ASSERT( m_pOwningRadiusContext );
	return (reinterpret_cast<COmniEngineContext*>(m_pOwningRadiusContext))->FailureAllertAllTargetIPs( m_pCaseOptions->GetName() );
}

/*
// -----------------------------------------------------------------------------
//		ProcessFailureAllert
// -----------------------------------------------------------------------------

void
CActiveCase::ProcessFailureAllert()
{
	if ( IsCapturing() && IsLogging() ) {
		CPeekString strMessage;
		strMessage = ::GetTimeStringGreenwich() + L"* ";
	}
}
*/
#endif // PACKET_WRITER_ERROR_HANDLING


////////////////////////////////////////////////////////////////////////////////
//  CActiveCaseList
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		CloseAllPacketWriters
// -----------------------------------------------------------------------------

void
CActiveCaseList::CloseAllPacketWriters()
{
	const int nActiveCaseCount = static_cast<int>( GetCount() );
	for ( int i = nActiveCaseCount - 1; i >= 0; i-- ) {
		CActiveCase* pActiveCase = GetAt( i );
		pActiveCase->StopCapturing( kChangeReasonStopCapture );
	}

	m_bUnstartedActiveCasesPending = false;
}

// -----------------------------------------------------------------------------
//		StartNewlyActiveCases
// -----------------------------------------------------------------------------

bool
CActiveCaseList::StartNewlyActiveCases( 
	const CPeekString&	strInitString, 
	bool&				bOutHasActiveCases )
{
	if ( !m_bUnstartedActiveCasesPending ) {
		return false;
	}

	bool bNewCaseStarted = false;

	const size_t nActiveCaseCount = GetCount();
	for ( size_t i = 0; i < nActiveCaseCount; i++ ) {
		CActiveCase* pActiveCase = GetAt( i );
		if ( pActiveCase->IsNewlyActivated() ) { 
			pActiveCase->StartCapturing( strInitString );
			bNewCaseStarted = true;
		}
		if ( pActiveCase->IsActive() ) {
			bOutHasActiveCases = true;
		}
	}

	if ( bNewCaseStarted ) {
		InitializeUnstartedActiveCasesFlag();
	}

	return ( bNewCaseStarted );
}

// -----------------------------------------------------------------------------
//		StopExpiredCases
// -----------------------------------------------------------------------------

bool
CActiveCaseList::StopExpiredCases( 
	bool&	bOutHasActiveCases )
{
	bool bCaseStopped = false;

	const int nActiveCaseCount = static_cast<int>( GetCount() );
	for ( int i = nActiveCaseCount - 1; i >= 0; i-- ) {
		CActiveCase* pActiveCase = GetAt( i );
		if ( pActiveCase->IsActive() ) {
			bOutHasActiveCases = true;
		}
		else {
			ASSERT( pActiveCase->GetTriggerTimeStatus() == kTriggerTimeStatusExpired );
			pActiveCase->StopCapturing( kChangeReasonTimeout );
			pActiveCase = DeleteAt( i );
			bCaseStopped = true;
		}
	}

	ASSERT( bCaseStopped );
	return bCaseStopped;
}

// -----------------------------------------------------------------------------
//		MonitorFiles
// -----------------------------------------------------------------------------

void
CActiveCaseList::MonitorFiles( 
	COptions*	pOptions )
{
	ASSERT( pOptions->IsSaveAgeSelected() );
	if ( !pOptions->IsSaveAgeSelected() ) {
		return;
	}

	const size_t nCount = GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CActiveCase* pActiveCase = GetAt( i );
		if ( pActiveCase->IsActive() ) {
			pActiveCase->MonitorFiles( pOptions );
		}
	}
}

// -----------------------------------------------------------------------------
//		InitializeUnstartedActiveCasesFlag
// -----------------------------------------------------------------------------

void
CActiveCaseList::InitializeUnstartedActiveCasesFlag()
{
	m_bUnstartedActiveCasesPending = false;

	const int nCount = static_cast<int>( GetCount() );
	for ( int i = nCount - 1; i >= 0; i-- ) {
		CActiveCase* pActiveCase = GetAt( i );
		UInt8 nTriggerTimeStatus = pActiveCase->GetTriggerTimeStatus();
		if ( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted ) {
			ASSERT( pActiveCase->IsActive() );
			m_bUnstartedActiveCasesPending = true;
			return;
		}
	}
}

// -----------------------------------------------------------------------------
//		InitializeWriters
// -----------------------------------------------------------------------------

bool
CActiveCaseList::InitializeWriters( 
	const CPeekString& strInitString ) 
{
	m_bUnstartedActiveCasesPending = false;

	const size_t nActiveCaseCount = GetCount();
	for ( size_t i = 0; i < nActiveCaseCount; i++ ){
		CActiveCase* pActiveCase = GetAt( i );
		if  ( !pActiveCase->StartCapturing( strInitString ) ){
			return false;
		}
	}

	InitializeUnstartedActiveCasesFlag();
	return true;
}

// =============================================================================
//		CTargetIPActiveCase
// =============================================================================

// -----------------------------------------------------------------------------
//		ProcessStartContinue
// -----------------------------------------------------------------------------

bool
CTargetIPActiveCase::ProcessStartContinue( 
//	const CPeekString&	strUserName, 
	const CPeekString&	strFormat, 
	const bool			bDisplayPacketNumber, 
	const bool			bDisplayIfCapturing )
{
	ASSERT( HasTriggerTimeStartedYet() && m_pActiveCase->IsCapturing() );
	ASSERT( bDisplayPacketNumber );

	bool	bIsNewlyStarted = false;
	CPeekString strMessage;
	CPeekString strMonitoringMessage;

	if ( m_pActiveCase->IsLoggingMsgsToFile() || m_pActiveCase->IsLoggingMsgsToScreen() ) {
		if ( m_bHasStartedCapturing ) {
			if ( !bDisplayIfCapturing ) {
				return false;
			}
			strMonitoringMessage = L"Logon by " + strFormat + L" - continuing to monitor";
		}
		else {
			strMonitoringMessage = L"Begin Monitoring " + strFormat;
		}

		CPeekString strTimeString = ::GetTimeStringGreenwich(); // + L" " + strMonitoringMessage;
		
		CRadiusMessage theMessage( strTimeString, strMonitoringMessage );
		m_pActiveCase->LogMessage( theMessage, bDisplayPacketNumber );
	}

	if ( !m_bHasStartedCapturing ) {
		bIsNewlyStarted = true;
	}

	m_bHasStartedCapturing = true;

	return bIsNewlyStarted;
}

#ifdef PACKET_WRITER_ERROR_HANDLING
/*
// -----------------------------------------------------------------------------
//		FailureAllert
// -----------------------------------------------------------------------------

void
CTargetIPActiveCase::FailureAllert()
{
	if ( !m_pActiveCase ) return;

	m_pActiveCase->FailureAllert();
//	m_pActiveCase = NULL;
}
*/
#endif


// =============================================================================
//		CTargetIPActiveCaseMgr
// =============================================================================

// -----------------------------------------------------------------------------
//		RebuildList
// -----------------------------------------------------------------------------

bool
CTargetIPActiveCaseMgr::RebuildList( 
	CActiveCaseList&	theActiveCaseList, 
	CRadiusPacketItem&	thePacketItem, 
	const CPeekString&	strInitString, 
	bool&				bNewCaptureStarted ) 
{

	CPeekString strUserName = thePacketItem.GetRadiusUserName();
	UInt16 uPort = thePacketItem.GetRadiusPort();

	ASSERT ( strUserName != L"" );
	m_bUnstartedActiveCasesPending = false;

	ASSERT( GetCount() == 0 );

	RemoveAll();
	bool bHasCaseMembers = false;
	bNewCaptureStarted = false;

	const int nActiveCaseCount = static_cast<int>( theActiveCaseList.GetCount() );

	for ( int i = nActiveCaseCount - 1; i >= 0; i-- ) {
		CActiveCase* pActiveCase = theActiveCaseList.GetAt( i );

		UInt8 nTriggerTimeStatus = pActiveCase->GetTriggerTimeStatus();
		ASSERT( nTriggerTimeStatus != kTriggerTimeStatusExpired );

		if ( pActiveCase->IsActive() && pActiveCase->UserIsActiveTarget( strUserName ) && pActiveCase->IsActivePort( uPort ) ) {
			if ( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted ) {
				m_bUnstartedActiveCasesPending = true;
			}
			else if ( nTriggerTimeStatus == kTriggerTimeStatusActive ) {
				ASSERT( pActiveCase->IsCapturing() );
				if ( !pActiveCase->IsCapturing() ) {
					if ( pActiveCase->HasTriggerTimeStartedYet() ) {
						ASSERT( pActiveCase->IsNewlyActivated() );
						pActiveCase->StartCapturing( strInitString );
						bNewCaptureStarted = true;
					}
				}

				CTargetIPActiveCase theTargetIPActiveCase( pActiveCase );
				Add( theTargetIPActiveCase );
				bHasCaseMembers = true;
			}
		}
	}

	ASSERT( DebugUserNameIsProperlyContained( strUserName ) );

	return bHasCaseMembers;
}

// -----------------------------------------------------------------------------
//		RefreshList
// -----------------------------------------------------------------------------

bool
CTargetIPActiveCaseMgr::RefreshList( 
	CActiveCaseList&	theActiveCaseList, 
	const CPeekString&	theUserName, 
	const CPeekString&	strFormat, 
	bool&				bChangeMade )
{
	ASSERT ( theUserName != L"" );
	
	m_bUnstartedActiveCasesPending = false;
	bChangeMade = false;

	UInt32 nUpdates = 0;
	UInt32 nAdds = 0;
	UInt32 nRemoves = 0;
	bool   bHasCaseMembers = false;
	const int nActiveCaseCount = static_cast<int>( theActiveCaseList.GetCount() );
  #ifdef _DEBUG
	const int nOrigTargetIPActiveCaseCount = static_cast<int>( GetCount() );
  #endif

	// Perform removes of any case that no longer exists or is no longer active
	int nTargetIPActiveCaseCount = static_cast<int>( GetCount() );
	for ( int i = nTargetIPActiveCaseCount - 1; i >= 0 ; i-- ) {
		CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
		CActiveCase* theActiveCase = theTargetIPActiveCase.GetActiveCasePtr();
		bool bFoundActiveCase = theActiveCaseList.Find( theActiveCase );
		if ( !bFoundActiveCase || !theActiveCase->UserIsActiveTarget( theUserName )) {
			if ( bFoundActiveCase ) {
				CPeekString strTimeString = ::GetTimeStringGreenwich();
				CPeekString strMessage = strFormat + L" - monitoring stopped due to options change";
				CRadiusMessage theMessage( strTimeString, strMessage );
				theActiveCase->LogMessage( theMessage );
			}
			RemoveAt( i );
			nRemoves++;
			continue;
		}
	}

	// Perform adds of cases we are not now monitoring
	for ( int i = nActiveCaseCount - 1; i >= 0; i-- ) {
		CActiveCase* pActiveCase = theActiveCaseList.GetAt( i );
		UInt8 nTriggerTimeStatus = pActiveCase->GetTriggerTimeStatus();

		// If the case is active but has yet not reached the trigger start time, then flag this
		if ( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted ) {
			ASSERT( !pActiveCase->IsCapturing() );
			m_bUnstartedActiveCasesPending = true;
		}

		// If the case is active and started then check whether we are already monitoring
		else if ( nTriggerTimeStatus == kTriggerTimeStatusActive ) {
			ASSERT( pActiveCase->IsCapturing() );
			bool bFoundActiveCase = Find( pActiveCase );
			if ( bFoundActiveCase ) { // We are already monitoring for this case
				bFoundActiveCase = true;
				bHasCaseMembers = true;
			}
			else { // We need to start monitoring for this case
				bool bUserIsActive = pActiveCase->UserIsActiveTarget( theUserName );
				if ( bUserIsActive ) {
					CTargetIPActiveCase theTargetIPActiveCase( pActiveCase );
					Add( theTargetIPActiveCase );
					bHasCaseMembers = true;
					nAdds++;
				}
			}
		}
	}

	if ( nAdds > 0 || nUpdates > 0 || nRemoves > 0 ) {
		bChangeMade = true;
	}

  #ifdef _DEBUG
	ASSERT ( ( GetCount() ) == ( nOrigTargetIPActiveCaseCount + nAdds - nRemoves ) );
	
	if ( !m_bUnstartedActiveCasesPending || bChangeMade ) {
		ASSERT( bHasCaseMembers ? GetCount() > 0 : GetCount() == 0 );
		ASSERT( DebugUserNameIsProperlyContained( theUserName ) );
	}
  #endif

	return bHasCaseMembers;
}

// -----------------------------------------------------------------------------
//		RefreshListForNewlyStarted
// -----------------------------------------------------------------------------

bool
CTargetIPActiveCaseMgr::RefreshListForNewlyStarted( 
	CActiveCaseList&	ActiveCaseList, 
	const CPeekString&	theUserName )
{
	bool bChangeMade = false;

	ASSERT ( theUserName != L"" );

	#ifdef _DEBUG
	  const int nTargetIPCaseCount = static_cast<int>( GetCount() );
	  size_t nNewCasesAdded = 0;
	#endif
	m_bUnstartedActiveCasesPending = false;

	const int nActiveCaseCount = static_cast<int>( ActiveCaseList.GetCount() );
	for ( int i = nActiveCaseCount - 1; i >= 0; i-- ) {
		CActiveCase* pActiveCase = ActiveCaseList.GetAt( i );

		bool bFoundActiveCase = Find( pActiveCase );
		if ( !bFoundActiveCase ) {
			UInt8 nTriggerTimeStatus = pActiveCase->GetTriggerTimeStatus();
			ASSERT( nTriggerTimeStatus != kTriggerTimeStatusExpired ); // This is because we always process stops before starts
			if ( nTriggerTimeStatus == kTriggerTimeStatusExpired ) {
				pActiveCase->StopCapturing( kChangeReasonTimeout );
				pActiveCase = ActiveCaseList.DeleteAt( i );
			}

			if ( pActiveCase && pActiveCase->IsActive() && pActiveCase->UserIsActiveTarget( theUserName ) ) {
				if ( nTriggerTimeStatus == kTriggerTimeStatusNotYetStarted ) {
					m_bUnstartedActiveCasesPending = true;
				}
				else if ( nTriggerTimeStatus == kTriggerTimeStatusActive ) {
					ASSERT( pActiveCase->IsCapturing() );
					CTargetIPActiveCase theTargetIPActiveCase( pActiveCase );
					Add( theTargetIPActiveCase );
					bChangeMade = true;
					#ifdef _DEBUG
					  nNewCasesAdded++;
					#endif
				}
			}
		}
	}

  #ifdef _DEBUG
	ASSERT ( GetCount() == nTargetIPCaseCount + nNewCasesAdded );
	ASSERT( DebugUserNameIsProperlyContained( theUserName ) );
  #endif
	return bChangeMade;
}

// -----------------------------------------------------------------------------
//		StopExpiredTargetIPActiveCases 
// -----------------------------------------------------------------------------

bool
CTargetIPActiveCaseMgr::StopExpiredTargetIPActiveCases( 
	const CPeekString&	theUserName, 
	const UInt32		uIPAddress, 
	bool&				bChangeMade ) 
{
	ASSERT ( theUserName != L"" );
	bChangeMade = false;
	bool bActive = false;

	const int nTargetIPActiveCaseCount = static_cast<int>( GetCount() );

	for ( int i = nTargetIPActiveCaseCount - 1; i >= 0 ; i-- ) {
		CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
		if ( theTargetIPActiveCase.IsActive() ) {
			ASSERT( theTargetIPActiveCase.GetTriggerTimeStatus() != kTriggerTimeStatusExpired );
			ASSERT( theTargetIPActiveCase.GetInternalCaseName() == theTargetIPActiveCase.GetCaseName() );
			bActive = true;
		}
		else {
			ASSERT( theTargetIPActiveCase.GetTriggerTimeStatus() == kTriggerTimeStatusExpired );
			StopExpiredTargetIPActiveCase( theTargetIPActiveCase, i, theUserName, uIPAddress );
			bChangeMade = true;
		}
	}

	ASSERT( DebugUserNameIsProperlyContained( theUserName ) );

	return bActive;
}

#ifdef PACKET_WRITER_ERROR_HANDLING
// -----------------------------------------------------------------------------
//		ProcessFailureAllert
// -----------------------------------------------------------------------------

bool
CTargetIPActiveCaseMgr::ProcessFailureAllert( 
	const CPeekString& inCaseName )
{
	bool bActiveCases = false;
	int nCount = static_cast<int>(GetCount());
	for( int i = nCount - 1; i >= 0; i-- ) {
		CTargetIPActiveCase& theTargetIPActiveCase = GetAt( i );
		if ( theTargetIPActiveCase.GetCaseName() == inCaseName ) {
//			theTargetIPActiveCase.ProcessFailureAllert();
			RemoveAt( i );
		}
		else {
			bActiveCases = true;
		}
	}
	return bActiveCases;
}
#endif // PACKET_WRITER_ERROR_HANDLING


// -----------------------------------------------------------------------------
//		FormatUserIPString
// -----------------------------------------------------------------------------

CPeekString
CTargetIPActiveCaseMgr::FormatUserIPString( 
	const CPeekString&	strUserName, 
	UInt32				uIPAddress )
{
	UInt8* pIPAddress = reinterpret_cast<UInt8*>( &uIPAddress );

	CPeekOutString theString;
	theString << strUserName << L" @ " << pIPAddress[3] << L"." << pIPAddress[2]<< L"."  << pIPAddress[1]<< L"."  << pIPAddress[0];

	return theString;
}
