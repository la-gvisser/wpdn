// =============================================================================
//	PersistedOptions.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PersistedOptions.h"
#include "PeekDataModeler.h"
#include "FileEx.h"


// ============================================================================
//		Data Modeling Tags
// ============================================================================

namespace OptionsTags
{
	const CPeekString	kElem_Option( L"DNSCaptureOE" );
	const CPeekString	kElem_Version( L"Version" );
	const CPeekString	kElem_DomainNameList( L"DomainNameList" );
	const CPeekString	kElem_Domain( L"Name" );
	const CPeekString	kElem_TrackIP( L"TrackIP" );
}


// =============================================================================
//		CPersistedOptions
// =============================================================================

CPeekString		CPersistedOptions::s_strTagForensics( L"Forensic Search" );

CPersistedOptions::CPersistedOptions()
{
	Reset();
}

CPersistedOptions::~CPersistedOptions()
{
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
CPersistedOptions::Copy(
	const CPersistedOptions& inOther )
{
	m_bTrackIP = inOther.m_bTrackIP;
	m_ayDomainNames.Copy( inOther.m_ayDomainNames );
	m_strVersion = inOther.m_strVersion;
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
CPersistedOptions::IsEqual(
	const CPersistedOptions& inOther )
{
	if ( m_bTrackIP != inOther.m_bTrackIP ) return false;
//	if ( m_strVersion != inOther.m_strVersion ) return false;
	if ( !m_ayDomainNames.IsEqual( inOther.m_ayDomainNames ) ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CPersistedOptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elemRoot( OptionsTags::kElem_Option, ioPrefs );
	if ( !elemRoot ) return false;

	CPeekString strVersion;
	if ( ioPrefs.IsStoring() ) {
		strVersion = m_strVersion;
	}

	elemRoot.Attribute( OptionsTags::kElem_Version, strVersion );

	Model( elemRoot );

	elemRoot.End();

	return true;
}

bool
CPersistedOptions::Model(
	CPeekDataElement& ioElement )
{
	ModelDomainList( ioElement );

	CPeekDataElement	elemTrackIP( OptionsTags::kElem_TrackIP, ioElement );
	if ( elemTrackIP ) {
		elemTrackIP.Enabled( m_bTrackIP );
		elemTrackIP.End();
	}

	return true;
}


// -----------------------------------------------------------------------------
//		ModelDomainList
// -----------------------------------------------------------------------------

bool
CPersistedOptions::ModelDomainList(
	CPeekDataElement&	ioElement )
{
	const bool bIsStoring = ioElement.IsStoring();
	const bool bIsLoading = !bIsStoring;

	size_t nCount( 0 );

	CPeekDataElement	elemNames( OptionsTags::kElem_DomainNameList, ioElement );
	if ( !elemNames ) return false;

	if ( bIsStoring ) {
		nCount = m_ayDomainNames.GetCount();
	}
	else {
		nCount = elemNames.GetChildCount();
		m_ayDomainNames.Reset();
	}

	for ( UInt32 i = 0; i < nCount; i++ ) {
		CPeekDataElement elemName( OptionsTags::kElem_Domain, elemNames, i );
		if ( elemName ) {
			CPeekString	strDomain;

			if ( bIsStoring ) {
				strDomain = m_ayDomainNames.GetAt( i );
			}

			elemName.Value( strDomain );

			if ( bIsLoading ) {
				m_ayDomainNames.Add( strDomain );
			}

			elemName.End();
		}
	}

	elemNames.End();

	ASSERT( nCount == m_ayDomainNames.GetCount() );

	return ( nCount == m_ayDomainNames.GetCount() );
}
