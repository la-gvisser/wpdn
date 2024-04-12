// ============================================================================
//	CaptureOptions.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "CaptureOptions.h"
#include "GlobalConstants.h"
#include "Utils.h"


////////////////////////////////////////////////////////////////////////////////
//  CCaptureOptions
////////////////////////////////////////////////////////////////////////////////

// Options modeling labels
const PCWSTR	kCaptureOptions( L"CaptureOptions" );
const PCWSTR	kCaptureToFile( L"CaptureToFile" );
const PCWSTR	kLogMsgsToFile( L"LogMsgsToFile" );
const PCWSTR	kCreateFoldersForCases( L"CreateFoldersForCases" );
const PCWSTR	kCaptureToScreen( L"CaptureToScreen" );
const PCWSTR	kLogMsgsToScreen( L"LogMsgsToScreen" );
const PCWSTR	kSaveAge( L"SaveAge" );
const PCWSTR	kCount( L"Count" );
const PCWSTR	kUnits( L"Units" );
const PCWSTR	kSaveSize( L"SaveSize" );
const PCWSTR	kUseGlobalDefaults( L"UseGlobalDefaults" );

// ----------------------------------------------------------------------------
//		GetSaveAgeValue
// ----------------------------------------------------------------------------

void
CCaptureOptions::GetSaveAgeValues(
	UInt32&			outCount,
	CPeekString&	outUnits ) const
{
	int	nCount;

	if ( (m_nSaveAge % kSecondsInADay) == 0 ) {
		nCount = m_nSaveAge / kSecondsInADay;
		outUnits = kDaysStr;
	}
	else if ( (m_nSaveAge % kSecondsInAnHour) == 0 ) {
		nCount = m_nSaveAge / kSecondsInAnHour;
		outUnits = kHoursStr;
	}
	else {
		nCount = m_nSaveAge / kSecondsInAMinute;
		outUnits = kMinutesStr;
	}

	outCount = nCount;
}


// ----------------------------------------------------------------------------
//		GetSaveSizeBytes
// ----------------------------------------------------------------------------

void
CCaptureOptions::GetSaveSizeBytes(
	UInt64&			outCount,
	CPeekString&	outUnits ) const
{
	UInt32	nCount;

	if ( (m_nSaveSize % kGigabyte) == 0 ) {
		nCount = (UInt32) (m_nSaveSize / kGigabyte);
		outUnits = kGigabytesStr;
	}
	else if ( (m_nSaveSize % kMegabyte) == 0 ) {
		nCount = (UInt32) (m_nSaveSize / kMegabyte);
		outUnits = kMegabytesStr;
	}
	else {
		nCount = (UInt32) (m_nSaveSize / kKilobyte);
		outUnits = kKilobytesStr;
	}
	
	outCount = nCount;
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CCaptureOptions::Model(
	CPeekDataElement&	ioElement,
	bool				bIsCase )
{
	const bool	bIsLoading = ioElement.IsLoading();
	const bool	bIsStoring = ioElement.IsStoring();

	UInt32		nSaveAge( 0 );
	CPeekString	strAgeUnits;
	UInt64		nSaveSize( 0 );
	CPeekString	strSizeUnits;

	if ( bIsStoring ) {
		GetSaveAgeValues( nSaveAge, strAgeUnits );
		GetSaveSizeBytes( nSaveSize, strSizeUnits );
	}

	CPeekDataElement	elemCaptureOptions( kCaptureOptions, ioElement );
	if ( !elemCaptureOptions ) return false;

		CPeekDataElement	elemCaptureToFile( kCaptureToFile, elemCaptureOptions );
			elemCaptureToFile.Enabled( m_bCaptureToFile );
		elemCaptureToFile.End();

		CPeekDataElement	elemLogMsgsToFile( kLogMsgsToFile, elemCaptureOptions );
			elemLogMsgsToFile.Enabled( m_bLogMsgsToFileButton );
		elemLogMsgsToFile.End();

		CPeekDataElement	elemFoldersForCases ( kCreateFoldersForCases, elemCaptureOptions );
			elemFoldersForCases.Enabled( m_bFoldersForCasesButton );
		elemFoldersForCases.End();

		CPeekDataElement	elemCaptureToScreen( kCaptureToScreen, elemCaptureOptions );
			elemCaptureToScreen.Enabled( m_bCaptureToScreen);
		elemCaptureToScreen.End();

		CPeekDataElement	elemLogMsgsToScreen( kLogMsgsToScreen, elemCaptureOptions );
			elemLogMsgsToScreen.Enabled( m_bLogMsgsToScreen );
		elemLogMsgsToScreen.End();

		CPeekDataElement	elemSaveAge( kSaveAge, elemCaptureOptions );
			elemSaveAge.Enabled( m_bSaveAge );
			elemSaveAge.Attribute( kCount, nSaveAge );
			elemSaveAge.Attribute( kUnits, strAgeUnits );
		elemSaveAge.End();

		CPeekDataElement	elemSaveSize( kSaveSize, elemCaptureOptions );
			elemSaveSize.Enabled( m_bSaveSize );
			elemSaveSize.Attribute( kCount, nSaveSize );
			elemSaveSize.Attribute( kUnits, strSizeUnits );
		elemSaveSize.End();

		if ( bIsCase ) {
			CPeekDataElement	elemUseGlobalDefaults( kUseGlobalDefaults, elemCaptureOptions );
				elemUseGlobalDefaults.Enabled( m_bUseGlobalDefaults );
			elemUseGlobalDefaults.End();
		}

	elemCaptureOptions.End();

	if ( bIsLoading ) {
		SetSaveAgeValue( nSaveAge, strAgeUnits );
		SetSaveSizeValue( nSaveSize, strSizeUnits );
	}

	return true;
}


// ----------------------------------------------------------------------------
//		SetSaveAgeValue
// ----------------------------------------------------------------------------

void
CCaptureOptions::SetSaveAgeValue(
	UInt32	inCount,
	CPeekString	inUnits )
{
	if ( inUnits.CompareNoCase(kDaysStr) == 0 ) {
		m_nSaveAge = inCount * kSecondsInADay;
	}
	else if ( inUnits.CompareNoCase(kHoursStr) == 0 ) {
		m_nSaveAge = inCount * kSecondsInAnHour;
	}
	else{
		m_nSaveAge = inCount * kSecondsInAMinute;
	}
}


// ----------------------------------------------------------------------------
//		SetSaveSizeValue
// ----------------------------------------------------------------------------

void
CCaptureOptions::SetSaveSizeValue(
	UInt64		inCount,
	CPeekString	inUnits )
{
	if ( inUnits.CompareNoCase(kGigabytesStr) == 0 ) {
		m_nSaveSize = inCount * kGigabyte;
	}
	else if ( inUnits.CompareNoCase(kMegabytesStr) == 0 ) {
		m_nSaveSize = inCount * kMegabyte;
	}
	else{
		m_nSaveSize = inCount * kKilobyte;
	}
}

// ----------------------------------------------------------------------------
//		AreOptionsValid
// ----------------------------------------------------------------------------

bool
CCaptureOptions::AreOptionsValid()
{
	if ( !m_bCaptureToFile && !m_bCaptureToScreen ) return false;

	if ( m_bCaptureToFile ) {
		if ( !m_bSaveAge && !m_bSaveSize ) return false;
		if ( m_nSaveAge == 0 && m_nSaveSize == 0 ) return false;
	}

	return true;
}
