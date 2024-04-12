// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"
#include "UserName.h"
#include "ByteStream.h"


// =============================================================================
//		COptions
// =============================================================================

CPeekString		COptions::s_strTagForensics( L"Forensic Search" );

// Options modeling labels
const PCWSTR	kRadius( L"Radius" );
const PCWSTR	kGeneralOptions( L"GeneralOptions" );
const PCWSTR	kDisableWarning( L"DisableWarning" );
const PCWSTR	kOutputDirectory( L"OutputDirectory" );
const PCWSTR	kUseCaseFileNumber( L"UseCaseFileNumber" );


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	m_GeneralOptions = inOther.m_GeneralOptions;
	m_CaptureOptions = inOther.m_CaptureOptions;
	m_PortOptions = inOther.m_PortOptions;
	m_CaseOptionsList = inOther.m_CaseOptionsList;
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther )
{
	if ( m_GeneralOptions != inOther.m_GeneralOptions ) return false;
	if ( m_PortOptions != inOther.m_PortOptions ) return false;
	if ( m_CaptureOptions != inOther.m_CaptureOptions ) return false;
	if ( m_CaseOptionsList != inOther.m_CaseOptionsList ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elemRoot( kRadius, ioPrefs );
	if ( !elemRoot ) return false;

	Model( elemRoot );

	elemRoot.End();

	return true;
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	m_GeneralOptions.Model( ioElement );
	m_CaptureOptions.Model( ioElement, false );
	m_PortOptions.Model( ioElement, false );
	m_CaseOptionsList.Model( ioElement, m_GeneralOptions.IsUseFileNumber() );

	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset()
{
	m_GeneralOptions.Reset();
	m_CaptureOptions.Reset();
	m_PortOptions.Reset();
	m_CaseOptionsList.Reset();
}


// -----------------------------------------------------------------------------
//		ResetActiveTimeSpan
// -----------------------------------------------------------------------------

void
COptions::ResetActiveTimeSpan()
{
	m_ActiveTimeSpan.InitializeActiveTimeSpan();

	CCaseOptionsList& theCaseOptionsList = GetCaseOptionsList();
	for ( size_t i = 0; i < theCaseOptionsList.GetCount(); i++ ) {
		CCaseOptions* theCaseOptions = theCaseOptionsList.GetAt( i );

		if( theCaseOptions->IsEnabled() ) {					
			UInt64 theStartTime = theCaseOptions->GetStartTime();
			UInt64 theStopTime = theCaseOptions->GetStopTime();

			m_ActiveTimeSpan.SetEarliestStartTime( theStartTime );
			m_ActiveTimeSpan.SetLatestStopTime( theStopTime );
		}
	}
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::CGeneralOptions::Model (
	CPeekDataElement& ioElement )
{
	CPeekDataElement	elemGeneralOptions( kGeneralOptions, ioElement );
	if ( !elemGeneralOptions ) return false;

		CPeekDataElement	elemDisableWarning( kDisableWarning, elemGeneralOptions );
		elemDisableWarning.Enabled( m_bDisplayWarning );
		elemDisableWarning.End();

		CPeekDataElement	elemOutputDir( kOutputDirectory, elemGeneralOptions );
		elemOutputDir.Value( m_strOutputRootDirectory );
		elemOutputDir.End();

		CPeekDataElement	elemUseCaseFileNumber( kUseCaseFileNumber, elemGeneralOptions );
		elemUseCaseFileNumber.Enabled( m_bUseFileNumber );
		elemUseCaseFileNumber.End();

	elemGeneralOptions.End();

	return true;
}
