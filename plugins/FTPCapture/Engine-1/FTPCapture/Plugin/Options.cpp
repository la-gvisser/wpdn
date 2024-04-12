// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "Options.h"
#include "PeekDataModeler.h"


// ============================================================================
//		Global Tags
// ============================================================================

const CPeekString	kDefaultRootDir( L"C:\\FTPCapture" );
const CPeekString	kDirectoryFileName( L"FtpDirectory.txt" );
const CPeekString	kLoggingFileName( L"FtpLog.txt" );

// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	const CPeekString	kRoot( COmniPlugin::GetName() );
	const CPeekString	kVersion( L"Version" );
	const CPeekString	kValue( L"Value" );
	const CPeekString	kHeight( L"Height" );
	const CPeekString	kWidth( L"Width" );

	// Options
	const CPeekString	kAlerting( _T( "Alerting" ) );
	const CPeekString	kCaptureAll( _T( "CaptureAll" ) );
	const CPeekString	kFacility( _T( "Facility" ) );
	const CPeekString	kOutputDirectory( _T( "OutputDirectory" ) );
	const CPeekString	kLoggingToScreen( _T( "LoggingToScreen" ) );
	const CPeekString	kObscureNames( _T( "ObscureNames" ) );
  #ifdef IMPLEMENT_PASSWORD_LOGGING
	const CPeekString	kShowPassword( _T( "ShowPassword" ) );
  #endif // IMPLEMENT_PASSWORD_LOGGING
	const CPeekString	kTargetList( _T( "TargetList" ) );
	const CPeekString	kTarget( _T( "Target" ) );
	const CPeekString	kPortList( _T( "PortList" ) );
	const CPeekString	kPort( _T( "Port" ) );
	const CPeekString	kSource( _T( "Source" ) );
	const CPeekString	kName( _T( "Name" ) );
	const CPeekString	kTransferOptions( _T( "TransferOptions" ) );
	const CPeekString	kEnableDataTransfer( _T( "EnableDataTransfer" ) );
	const CPeekString	kEnableFileCapture( L"EnableFileCapture" );
	const CPeekString	kFileCaptureDir( _T( "FileCaptureDir" ) );
	const CPeekString	kEnableListResultsToFile( L"EnableListResultsToFile" );
	const CPeekString	kListResultsToFileDir( _T( "ListResultsToFileDir" ) );
	const CPeekString	kEnableLogFtpCmds( L"EnableLogFtpCmds" );
	const CPeekString	kLogFtpCmdsDir( _T( "LogFtpCmdsDir" ) );
}


// ============================================================================
//		CTargetName
// ============================================================================

// ============================================================================
//		CTargetNameList
// ============================================================================

// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CTargetName::Model(
	CPeekDataElement& inParentElement )
{
	// Perform the modeling
	inParentElement.Enabled( m_bEnabled );
	inParentElement.Attribute( ConfigTags::kName, m_strName );

	return true;
}


// ----------------------------------------------------------------------------
//		Validate
// ----------------------------------------------------------------------------

bool
CTargetName::Validate(
	CPeekString& str )
{
	return !str.IsEmpty();
}


// ============================================================================
//		CTargetNameList
// ============================================================================

// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CTargetNameList::Model(
	CPeekDataElement& inParentElement )
{
	const bool			bIsStoring( inParentElement.IsStoring() );
	const bool			bIsLoading( !bIsStoring );
	UInt32				nTargetCount( 0 );
	bool				bResult( true );

	CPeekDataElement	elemTargetList( ConfigTags::kTargetList, inParentElement );
	if ( elemTargetList.IsNotValid() ) {
		return false;
	}

	if ( bIsStoring ) {
		nTargetCount = static_cast<UInt32>( GetCount() );
	}
	else {
		nTargetCount = elemTargetList.GetChildCount();
		RemoveAll();
	}

	CPeekDataElement	elemCaptureAll( ConfigTags::kCaptureAll, inParentElement );
	if ( elemCaptureAll.IsValid() ) {
		elemCaptureAll.Enabled( m_bCaptureAll );
	}
	elemCaptureAll.End();


	for ( UInt32 nTargetNum = 0; nTargetNum < nTargetCount; nTargetNum++ ) {
		CTargetName	Target;

		if ( bIsStoring ) {
			Target =  GetAt( nTargetNum );
		}

		bResult = false;

		CPeekDataElement	elemTarget( ConfigTags::kTarget, elemTargetList, nTargetNum );
		if ( elemTarget.IsValid() ) {
			bResult = Target.Model( elemTarget );
		}
		elemTarget.End();

		if ( bIsLoading ) {
			Add( Target );
		}
	}

	return true;
}


// ============================================================================
//		CPortNumber
// ============================================================================

// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CPortNumber::Model(
	CPeekDataElement& inParentElement )
{
	// Perform the modeling
	inParentElement.Enabled( m_bEnabled );
	inParentElement.Attribute( ConfigTags::kName, m_nPort );

	return true;
}


// ----------------------------------------------------------------------------
//		Validate
// ----------------------------------------------------------------------------

bool
CPortNumber::Validate(
	CPeekString& str )
{
	if ( str.GetLength() == 0 ) return false;

	long	nValue = _tstol( str );
	if ( nValue < 0 ) return false;
	if ( nValue > 0x0000FFFF ) return false;

	return true;
}


// ============================================================================
//		CPortNumberList
// ============================================================================

// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CPortNumberList::Model(
	CPeekDataElement& inParentElement )
{

	const bool			bIsStoring( inParentElement.IsStoring() );
	const bool			bIsLoading( !bIsStoring );
	UInt32				nPortCount( 0 );
	bool				bResult( true );

	CPeekDataElement	elemPortList( ConfigTags::kPortList, inParentElement );
	if ( elemPortList.IsNotValid() ) {
		return false;
	}

	if ( bIsStoring ) {
		nPortCount = static_cast<UInt32>( GetCount() );
	}
	else {
		nPortCount = elemPortList.GetChildCount();
		RemoveAll();
	}

	for ( UInt32 nPortNum = 0; nPortNum < nPortCount; nPortNum++ ) {
		CPortNumber	Port;

		if ( bIsStoring ) {
			Port =  GetAt( nPortNum );
		}

		bResult = false;

		CPeekDataElement	elemTarget( ConfigTags::kPort, elemPortList, nPortNum );
		if ( elemTarget.IsValid() ) {
			bResult = Port.Model( elemTarget );
		}
		elemTarget.End();

		if ( bIsLoading && bResult ) {
			Add( Port );
		}
	}

	return true;
}


// ============================================================================
//		CAlerting
// ============================================================================

// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CAlerting::Model(
	CPeekDataElement& inParentElement )
{
	bool			bReturn( true );

	// Perform the modeling
	CPeekDataElement	elemAlerting( ConfigTags::kAlerting, inParentElement );
	if ( elemAlerting.IsValid() ) {
		elemAlerting.Enabled( m_bEnabled );
		elemAlerting.Attribute( ConfigTags::kSource, m_strSource );
		elemAlerting.Attribute( ConfigTags::kFacility, m_strFacility );
		elemAlerting.Attribute( ConfigTags::kOutputDirectory, m_strOutputDirectory );
	}
	else {
		bReturn = false;
	}
	elemAlerting.End();

	return bReturn;
}


// ============================================================================
//		CLogging
// ============================================================================

// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CLogging::Model(
	CPeekDataElement& inParentElement )
{
	bool		bReturn( true );

	// Perform the modeling

	CPeekDataElement	elemLogging( ConfigTags::kLoggingToScreen, inParentElement );
	if ( elemLogging.IsValid() ) {
		elemLogging.Enabled( m_bLoggingToScreen );
	}
	else {
		bReturn = false;
	}
	elemLogging.End();

	CPeekDataElement	elemObscureNames( ConfigTags::kObscureNames, inParentElement );
	if ( elemObscureNames.IsValid() ) {
		elemObscureNames.Enabled( m_bObscureNames );
	}
	else {
		bReturn = false;
	}
	elemObscureNames.End();

  #ifdef IMPLEMENT_PASSWORD_LOGGING
	CPeekDataElement	kShowPassword( ConfigTags::kShowPassword, inParentElement );
	if ( kShowPassword.IsValid() ) {
		kShowPassword.Enabled( m_bShowPassword );
	}
	else {
		bReturn = false;
	}
	kShowPassword.End();
  #endif // IMPLEMENT_PASSWORD_LOGGING

	return bReturn;
}


#ifdef IMPLEMENT_DATA_CAPTURE
// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

void	
CDataTransfer::Copy( 
	 const CDataTransfer& inOther ) 
{
	m_bEnableDataTransfer = inOther.m_bEnableDataTransfer;
	m_bEnableFileCapture = inOther.m_bEnableFileCapture;
	m_strFileCaptureDir = inOther.m_strFileCaptureDir;
	m_bEnableListResultsToFile = inOther.m_bEnableListResultsToFile;
	m_strListResultsToFileDir = inOther.m_strListResultsToFileDir;
	m_bEnableLogFtpCmds = inOther.m_bEnableLogFtpCmds;
	m_strLogFtpCmdsDir = inOther.m_strLogFtpCmdsDir;
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CDataTransfer::Model(
	CPeekDataElement& inParentElement )
{
	bool		bReturn( true );

	// Perform the modeling
	CPeekDataElement	elemTransferOptions( ConfigTags::kTransferOptions, inParentElement );
	if ( elemTransferOptions.IsValid() ) {

		CPeekDataElement	elemEnableDataTransfer( ConfigTags::kEnableDataTransfer, elemTransferOptions );
		if ( elemEnableDataTransfer.IsValid() ) {
			elemEnableDataTransfer.Value( m_bEnableDataTransfer );
		}
		else bReturn = false;
		elemEnableDataTransfer.End();

		CPeekDataElement	elemEnableFileCapture( ConfigTags::kEnableFileCapture, elemTransferOptions );
		if ( elemEnableFileCapture.IsValid() ) {
			elemEnableFileCapture.Value( m_bEnableFileCapture );
		}
		else bReturn = false;
		elemEnableFileCapture.End();

		CPeekDataElement	elemFileCaptureDir( ConfigTags::kFileCaptureDir, elemTransferOptions );
		if ( elemFileCaptureDir.IsValid() ) {
			elemFileCaptureDir.Value( m_strFileCaptureDir );
		}
		else bReturn = false;
		elemFileCaptureDir.End();

		CPeekDataElement	elemEnableListResultsToFile( ConfigTags::kEnableListResultsToFile, elemTransferOptions );
		if ( elemEnableListResultsToFile.IsValid() ) {
			elemEnableListResultsToFile.Value( m_bEnableListResultsToFile );
		}
		else bReturn = false;
		elemEnableListResultsToFile.End();

		CPeekDataElement	elemListResultsToFileDir( ConfigTags::kListResultsToFileDir, elemTransferOptions );
		if ( elemListResultsToFileDir.IsValid() ) {
			elemListResultsToFileDir.Value( m_strListResultsToFileDir );
		}
		else bReturn = false;
		elemListResultsToFileDir.End();

		CPeekDataElement	elemEnableLogFtpCmds( ConfigTags::kEnableLogFtpCmds, elemTransferOptions );
		if ( elemEnableLogFtpCmds.IsValid() ) {
			elemEnableLogFtpCmds.Value( m_bEnableLogFtpCmds );
		}
		else bReturn = false;
		elemEnableLogFtpCmds.End();

		CPeekDataElement	elemLogFtpCmdsDir( ConfigTags::kLogFtpCmdsDir, elemTransferOptions );
		if ( elemLogFtpCmdsDir.IsValid() ) {
			elemLogFtpCmdsDir.Value( m_strLogFtpCmdsDir );
		}
		else bReturn = false;
		elemLogFtpCmdsDir.End();

	}
	else bReturn = false;

	elemTransferOptions.End();

	return bReturn;
}

// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
CDataTransfer::Reset() 
{
	m_bEnableDataTransfer = false;
	m_bEnableFileCapture = false;
	m_strFileCaptureDir = kDefaultRootDir;
	m_bEnableListResultsToFile = false;
	m_strListResultsToFileDir = kDefaultRootDir;
	m_bEnableLogFtpCmds = false;
	m_strLogFtpCmdsDir =kDefaultRootDir;
}


// -----------------------------------------------------------------------------
//		ValidateFilePath
// -----------------------------------------------------------------------------

bool
CDataTransfer::ValidateFilePath( 
	CPeekString&	inFilePath, 
	bool			bNormalize ) 
{
	const size_t	nLenth( inFilePath.GetLength() );

	if ( nLenth < 3 || nLenth > MAX_PATH )	return false;
	if ( inFilePath[1] != L':' )			return false;

	for( size_t i = 0; i < nLenth; i++ ) {
		wchar_t c( inFilePath[i] );
		if ( !FilePath::IsValidPathChar( inFilePath[i] ) )	return false;
		if ( bNormalize) inFilePath.at(i) = FilePath::NormalizeSeparator( c );
	}

	return true;
}
#endif // IMPLEMENT_DATA_CAPTURE


// =============================================================================
//		COptions
// =============================================================================

CPeekString		COptions::s_strTagForensics( L"Forensic Search" );

// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	m_strConfigFileName = inOther.m_strConfigFileName;
	m_strChangeLogFileName = inOther.m_strChangeLogFileName;
	m_TargetNames = inOther.m_TargetNames;
	m_PortNumbers = inOther.m_PortNumbers;
	m_Alerting = inOther.m_Alerting;
	m_Logging = inOther.m_Logging;
  #ifdef IMPLEMENT_DATA_CAPTURE
	m_DataTransfer = inOther.m_DataTransfer;
  #endif // IMPLEMENT_DATA_CAPTURE
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther )
{
	inOther;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elemOptions( L"Options", ioPrefs );
	if ( elemOptions ) {
		return Model( elemOptions );
	}
	return false;
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	m_TargetNames.Model( ioElement );
	m_PortNumbers.Model( ioElement );
	m_Alerting.Model( ioElement );
	m_Logging.Model( ioElement );
  #ifdef IMPLEMENT_DATA_CAPTURE
	m_DataTransfer.Model( ioElement );
  #endif // IMPLEMENT_DATA_CAPTURE

	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset() 
{
	m_TargetNames.Reset();
	m_PortNumbers.Reset();
	m_Alerting.Reset();
	m_Logging.Reset();
  #ifdef IMPLEMENT_DATA_CAPTURE
	m_DataTransfer.Reset();
  #endif IMPLEMENT_DATA_CAPTURE
}
