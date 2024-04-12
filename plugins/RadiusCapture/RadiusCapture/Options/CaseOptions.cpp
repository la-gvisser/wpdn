// =============================================================================
//	CaseOptions.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "CaseOptions.h"
#include "UserName.h"
#include "ByteStream.h"


#ifdef MODE_OPTIONS
const CString	g_strPen( "Pen" );
const CString	g_strFull( "Full" );
#endif

const PCWSTR	kPrefix( L"Prefix" );
const PCWSTR	kCaptureId( L"CaptureId" );
const PCWSTR	kName( L"Name" );
const PCWSTR	kTriggerOptions( L"TriggerOptions" );
const PCWSTR	kStartTime( L"StartTime" );
const PCWSTR	kStopTime( L"StopTime" );
const PCWSTR	kUsers( L"Users" );
const PCWSTR	kGeneralOptions( L"GeneralOptions" );
const PCWSTR	kOutputDirectory( L"OutputDirectory" );


////////////////////////////////////////////////////////////////////////////////
//		CCaseOptions
////////////////////////////////////////////////////////////////////////////////

CCaseOptions::CCaseOptions()
	: m_GeneralOptions( *this ),
  #ifdef INTEGRITY_OPTIONS
		m_pPlugin( NULL ),
  #endif
		m_bEnabled( false )
	,	m_bDuplicate( false )
	,	m_bHighlighted( false )
	,	m_nLastTab( kTab_General )
{
}

// -----------------------------------------------------------------------------
//		Init
// -----------------------------------------------------------------------------

void
CCaseOptions::Init(
		CPeekString inName
	,	bool bIsUseFileNumber
#ifdef INTEGRITY_OPTIONS
	,	CPeekPlugin*	inPlugin
#endif
	)
{
	SetName( inName );
	SetUseFileNumber( bIsUseFileNumber );
	
  #ifdef INTEGRITY_OPTIONS
	ASSERT( inPlugin );
	m_Integrity.Init( inPlugin );
  #endif
}

// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
CCaseOptions::Copy(
	const CCaseOptions& in )
{
	m_bEnabled = in.m_bEnabled;
	m_bDuplicate = in.m_bDuplicate;
	m_bHighlighted = in.m_bHighlighted;
	m_GeneralOptions = in.m_GeneralOptions;
	m_TriggerOptions = in.m_TriggerOptions;
	m_UserNameOptions = in.m_UserNameOptions;
	m_CasePortOptions = in.m_CasePortOptions;
	m_CaptureOptions = in.m_CaptureOptions;

  #ifdef MODE_OPTIONS
	m_Mode = in.m_Mode;
  #endif
  #ifdef INTEGRITY_OPTIONS
	m_Integrity = in.m_Integrity;
  #endif
}


// -----------------------------------------------------------------------------
//		CanCaseBeEnabled
// -----------------------------------------------------------------------------

bool
CCaseOptions::CanCaseBeEnabled( int& nReason )
{
	nReason = kCaseReason_OK;

	if ( !AreGeneralOptionsValid() ) {
		nReason = kCaseReason_GeneralOpts;
		return false;
	}

	if ( !AreEnabledUsers() ) {
		nReason = kCaseReason_NoUsers;
		return false;
	}

	if ( HasStopTimeExpired() ) {
		nReason = kCaseReason_TimeExpired;
		return false;
	}

	if ( !AreEnabledPorts() ) {
		nReason = kCaseReason_NoPorts;
		return false;
	}

	if ( !AreCaptureOptionsValid() ) {
		nReason = kCaseReason_CaptureOpts;
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
//		GetFormattedStartStopTime
// -----------------------------------------------------------------------------

CPeekString
CCaseOptions::GetFormattedStartStopTime( bool bStart ) {
	SYSTEMTIME theSystemTime;
	UInt64 theFileTime;
	if ( bStart ) theFileTime = GetStartTime();
	else theFileTime = GetStopTime();
	::FileTimeToSystemTime( (const FILETIME*)&theFileTime, &theSystemTime );
	return ( ::FormatTimeGreenwich( &theSystemTime ) );
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CCaseOptions::Model(
	CPeekDataElement& ioElement )
{
	ioElement.Enabled( m_bEnabled );

	m_GeneralOptions.Model( ioElement );
	m_TriggerOptions.Model( ioElement );
	m_UserNameOptions.Model( ioElement );
	m_CasePortOptions.Model( ioElement, true );
	m_CaptureOptions.Model( ioElement, true );

	return true;
}


#ifdef INTEGRITY_OPTIONS
// -----------------------------------------------------------------------------
//		GetIntegrityOptions
// -----------------------------------------------------------------------------
bool
CCaseOptions::GetIntegrityOptions(
	tIntegrityOptions&	inIntegrityOptions )
{
	inIntegrityOptions.nEnabled = 0;
	inIntegrityOptions.nHashAlgorithm = 1;
	inIntegrityOptions.strKeyContainer.Empty();

	if ( m_pPlugin == NULL ) return false;

	try {
		LONG	nResult;
		TCHAR	szPrefsPath[MAX_PATH];
		m_pPlugin->DoPrefsGetPrefsPath( szPrefsPath );

		CString	strCapture;
		strCapture.Format( _T("%s\\%s"), szPrefsPath, kKeyName_Capture );

		CRegKey	rk;
		nResult = rk.Open( HKEY_CURRENT_USER, strCapture );

		DWORD	dwIntegrity = 0;
		nResult = rk.QueryDWORDValue( kKeyValue_Integrity, dwIntegrity );
		if ( FAILED( nResult ) ) {
			rk.SetDWORDValue( kKeyValue_Integrity, kIntegrity_Disabled );
			rk.SetDWORDValue( kKeyValue_HashAlgorithm, kIntegrity_Algorithm_MD5 );
			rk.SetBinaryValue(
				kKeyValue_KeyContainer,
				m_Integrity.m_strDefaultKeyName.GetBuffer(),
				((m_Integrity.m_strDefaultKeyName.GetLength() + 1) * sizeof( TCHAR )) );
			return false;
		}

		DWORD	dwHashAlgorithm = 0;
		nResult = rk.QueryDWORDValue( kKeyValue_HashAlgorithm, dwHashAlgorithm );
		if ( FAILED( nResult ) ||
			!((dwHashAlgorithm == kIntegrity_Algorithm_MD5) ||
			(dwHashAlgorithm == kIntegrity_Algorithm_SHA)) ) {
				return false;
		}

		CString		strKeyContainer;
		ULONG		nKeyContainerLength = MAX_PATH * sizeof( TCHAR );
		nResult = rk.QueryStringValue( kKeyValue_KeyContainer, strKeyContainer.GetBuffer( MAX_PATH ), &nKeyContainerLength );
		strKeyContainer.ReleaseBuffer();

		inIntegrityOptions.nEnabled = dwIntegrity;
		inIntegrityOptions.nHashAlgorithm = dwHashAlgorithm;
		if ( strKeyContainer.GetLength() == 0 ) {
			inIntegrityOptions.strKeyContainer = m_Integrity.m_strDefaultKeyName;
		}
		else {
			inIntegrityOptions.strKeyContainer = strKeyContainer;
		}

		rk.Close();
		return true;
	}
	catch ( ... ) {
		inIntegrityOptions.nEnabled = 0;
	}

	return false;
}

// -----------------------------------------------------------------------------
//		SetIntegrityOptions
// -----------------------------------------------------------------------------

bool
CCaseOptions::SetIntegrityOptions(
	tIntegrityOptions&	inIntegrityOptions )
{
	if ( m_pPlugin == NULL ) return false;
	try {
		LONG	nResult;
		TCHAR	szPrefsPath[MAX_PATH];
		m_pPlugin->DoPrefsGetPrefsPath( szPrefsPath );

		CString	strCapture;
		strCapture.Format( _T("%s\\%s"), szPrefsPath, kKeyName_Capture );

		CRegKey	rk;
		nResult = rk.Open( HKEY_CURRENT_USER, strCapture );

		if ( SUCCEEDED( nResult ) ) {
			rk.SetDWORDValue( kKeyValue_Integrity, inIntegrityOptions.nEnabled );
			rk.SetDWORDValue( kKeyValue_HashAlgorithm, inIntegrityOptions.nHashAlgorithm );
			rk.SetStringValue( kKeyValue_KeyContainer, inIntegrityOptions.strKeyContainer );
			return true;
		}
	}
	catch ( ... ) {
		;
	}

	return false;
}
#endif //INTEGRITY_OPTIONS


// -----------------------------------------------------------------------------
//		ImportCaseItems
// -----------------------------------------------------------------------------

void
CCaseOptions::ImportCaseItems( 
	CCaseOptions*		pEditCaseOptions,
	const CCaseOptions* inImportCaseOptions,
	CCaptureOptions&	inCaptureOptions,
	bool				bIsCapturing )
{
	CCaseOptions TempCaseOptions = *pEditCaseOptions;

	CPeekString SaveName = pEditCaseOptions->GetName();
	CPeekString SavePrefix = pEditCaseOptions->GetPrefix();

	ASSERT( SaveName.CompareNoCase( pEditCaseOptions->GetName() ) == 0 );

	*pEditCaseOptions = *inImportCaseOptions;

	if ( bIsCapturing ) {

		pEditCaseOptions->SetName( SaveName );
		pEditCaseOptions->SetPrefix( SavePrefix );
		pEditCaseOptions->SetOutputDirectory( TempCaseOptions.GetOutputDirectory() );

		CPortOptions& TempPortOptions = TempCaseOptions.GetCasePortOptions();
		CPortOptions& EditPortOptions = pEditCaseOptions->GetCasePortOptions();
		EditPortOptions = TempPortOptions;

		pEditCaseOptions->SetUseCaptureGlobalDefaults( TempCaseOptions.IsUseCaptureGlobalDefaults() );

		if ( pEditCaseOptions->IsUseCaptureGlobalDefaults() ) {
			pEditCaseOptions->SyncCaptureOptionsToGlobal( inCaptureOptions );
		}
		else {
			pEditCaseOptions->SetCaptureToFile( TempCaseOptions.IsCaptureToFile() );
			pEditCaseOptions->SetLogMsgsToFileButton( TempCaseOptions.IsLogMsgsToFileButton() );
			pEditCaseOptions->SetCaptureToScreen( TempCaseOptions.IsCaptureToScreen() );
			pEditCaseOptions->SetLogMsgsToScreen( TempCaseOptions.IsLogMsgsToScreen() );
		}
	}

	pEditCaseOptions->SetEnabledState( TempCaseOptions.IsEnabled() );
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CCaseOptions::CGeneralOptions::Model(
	CPeekDataElement& ioElement )
{
	CPeekDataElement	elemGeneralOptions( kGeneralOptions, ioElement );

		elemGeneralOptions.Attribute( kCaptureId, m_nCaseCaptureId );
		elemGeneralOptions.Attribute( kPrefix, m_strPrefix );
		elemGeneralOptions.Attribute( kName, m_strCaseName );

		CPeekDataElement	elemOutputDir( kOutputDirectory, elemGeneralOptions );
			elemOutputDir.Value( m_strOutputDirectory );
		elemOutputDir.End();

	elemGeneralOptions.End();

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CCaseOptions::CTriggerOptions::Model (
	CPeekDataElement& ioElement )
{
	CPeekString strStartTime;
	CPeekString strStopTime;

	if ( ioElement.IsStoring() ) {
		strStartTime = FormatTimeGreenwich( m_StartTime );
		strStopTime = FormatTimeGreenwich( m_StopTime );
	}

	CPeekDataElement	elemTriggerOptions( kTriggerOptions, ioElement );

		CPeekDataElement	elemStartTime( kStartTime, elemTriggerOptions );
			elemStartTime.Value( strStartTime );
		elemStartTime.End();

		CPeekDataElement	elemStopTime( kStopTime, elemTriggerOptions );
			elemStopTime.Value( strStopTime );
		elemStopTime.End();

	elemTriggerOptions.End();

	if ( ioElement.IsLoading() ) {
		m_StartTime = ::ParseTimeStampGreenwich( strStartTime );
		m_StopTime = ::ParseTimeStampGreenwich( strStopTime );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CCaseOptions::CUserNameOpts::Model(
	CPeekDataElement& ioElement )
{
	CPeekDataElement	elemUsers( kUsers, ioElement );
	if ( elemUsers ) {
		m_UserList.Model( elemUsers );
		elemUsers.End();
	}
	return true;
}


// -----------------------------------------------------------------------------
//		SuggestOutputDirectory
// -----------------------------------------------------------------------------

CPeekString
CCaseOptions::CGeneralOptions::SuggestOutputDirectory( 
	CPeekString strOutputRootDir, 
	CPeekString strCasePrefix )
{
	CPeekString	strPath( strOutputRootDir );
	if ( strPath.GetLength() == 0 ) {
		strPath = kDefaultOutputDir;
	}

	if ( m_Container.IsFoldersForCases() ) {
		if ( strCasePrefix.GetLength() > 0 ) {
			if ( strPath.Right( 1 ) != L"\\" ) {
				strPath += L'\\';
			}
			strPath += strCasePrefix;
		}
	}

	return strPath;
}
