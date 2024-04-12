// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"


#define kDefault_PortNumber			21


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekString	kWPOmniPeekPlugins( _T( "WildPackets\\OmniPeek\\Plugins" ) );
	const CPeekString	kAppData( _T( "APPDATA" ) );
	const CPeekString	kConfigFileName( _T( "Config.xml" ) );
	const CPeekString	kChangeLogFileName( _T( "ChangeLog.txt" ) );
}


// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	const CPeekString	kRoot( _T( "FTPCapture" ) );
	const CPeekString	kVersion( _T( "Version" ) );
	const CPeekString	kValue( _T( "Value" ) );
	const CPeekString	kHeight( _T( "Height" ) );
	const CPeekString	kWidth( _T( "Width" ) );

	// Options
	const CPeekString	kAlerting( _T( "Alerting" ) );
	const CPeekString	kCaptureAll( _T( "CaptureAll" ) );
	const CPeekString	kFacility( _T( "Facility" ) );
	const CPeekString	kOutputDirectory( _T( "OutputDirectory" ) );
	const CPeekString	kLogging( _T( "Logging" ) );
	const CPeekString	kObscureNames( _T( "ObscureNames" ) );
	const CPeekString	kTargetList( _T( "TargetList" ) );
	const CPeekString	kTarget( _T( "Target" ) );
	const CPeekString	kPortList( _T( "PortList" ) );
	const CPeekString	kPort( _T( "Port" ) );
	const CPeekString	kSource( _T( "Source" ) );
	const CPeekString	kName( _T( "Name" ) );

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
		nTargetCount = GetCount();
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
		nPortCount = GetCount();
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
	CPeekDataElement	elemObscureNames( ConfigTags::kObscureNames, inParentElement );
	if ( elemObscureNames.IsValid() ) {
		elemObscureNames.Enabled( m_bObscureNames );
	}
	else {
		bReturn = false;
	}
	elemObscureNames.End();

	return bReturn;
}


// =============================================================================
//		COptions
// =============================================================================

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

	return true;
}
