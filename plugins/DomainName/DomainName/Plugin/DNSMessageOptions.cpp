// =============================================================================
//	DNSMessageOptions.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "DNSMessageOptions.h"
#include "PeekDataModeler.h"
//#include "FileEx.h"
//#include "OmniEngineContext.h"


// ============================================================================
//		Data Modeling Tags
// ============================================================================

namespace DNSProcessTags
{
	const CPeekString	kElem_Domain( L"Name" );

	const CPeekString	kElem_ResolvedNameList( L"ResolvedNameList" );
	const CPeekString	kElem_ResolvedName( L"ResolvedName" );
	const CPeekString	kElem_AddressList( L"AddressList" );

	const CPeekString	kElem_DomainCountedList( L"DomainCountedList" );
	const CPeekString	kElem_DomainCounted( L"DomainCounted" );
	const CPeekString	kElem_QueryCount( L"QueryCount" );
	const CPeekString	kElem_ResponseCount( L"ResponseCount" );
}

// =============================================================================
//		CDNSMessageOptions
// =============================================================================

bool
CDNSMessageOptions::Model( 
	CPeekDataModeler& ioPrefs,
	bool			  bSetDomainNames )
{
	bSetDomainNames;
	return m_Options.Model( ioPrefs );
/*
	bool bSuccess = m_Options.Model( ioPrefs );
//ptst?
//bSetDomainNames;
	if ( bSuccess && bSetDomainNames ) {
		SetDomainNames();
	}
	return bSuccess;
*/
//ptst

//	if ( bSetDomainNames ) {
//		SetDomainNames();
//	}
//
//	return Model( ioPrefs );
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CDNSMessageOptions::Model( 
	CPeekDataElement& ioElement ) {

	//ptst??
//	bool bIsStoring( ioElement.IsStoring() );
//	bool bModelResults( true ); //(bIsStoring && m_bIsEngine)  || !(bIsStoring && m_bIsEngine ) );

//	if ( bModelResults ) {
		if ( !ModelCountedList( ioElement ) ) return false;
		if ( !ModelResolvedList( ioElement ) ) return false;
//	}

	return m_Options.Model( ioElement );
}


// -----------------------------------------------------------------------------
//		ModelResolvedList
// -----------------------------------------------------------------------------

bool
CDNSMessageOptions::ModelResolvedList(
	CPeekDataElement&	ioElement )
{
	// Note: this modeling is only required `on behalf of the view tabs.
	// The storing logic below is used by COmniEngineContext on formatting
	// a message return to a console.  The loading logic is used by 
	// COmniConsoleContext to create arrays simply for display by the 
	// view tabs.

	const bool bIsStoring = ioElement.IsStoring();	// Engine side only
	const bool bIsLoading = !bIsStoring;			// Console side only

	size_t nCount( 0 );

	CPeekDataElement	elemNames( DNSProcessTags::kElem_ResolvedNameList, ioElement );
	if ( !elemNames ) return false;

	if ( bIsStoring ) {
		nCount = m_ResolvedNames.GetCount();
	}
	else {
		nCount = elemNames.GetChildCount();
		m_ResolvedNames.Reset();
	}

	for ( UInt32 i = 0; i < nCount; i++ ) {

		CPeekDataElement elemResolvedName( DNSProcessTags::kElem_ResolvedName, elemNames, i );
		if ( elemResolvedName ) {

			CPeekString strResolvedName;
			CPeekString strAddressList;

			if ( bIsStoring ) {
				const CResolvedDomain&	theResolvedDomain( m_ResolvedNames.GetAt( i ) );
				strResolvedName = theResolvedDomain.GetName();
				strAddressList = theResolvedDomain.FormatAddressList();
			}

			CPeekDataElement elemName( DNSProcessTags::kElem_Domain, elemResolvedName );
			if ( elemName ) {
				elemName.Value( strResolvedName );
				elemName.End();
			}
			CPeekDataElement Elem_AddressList( DNSProcessTags::kElem_AddressList, elemResolvedName );
			if ( Elem_AddressList ) {
				Elem_AddressList.Value( strAddressList );
				Elem_AddressList.End();
			}

			if ( bIsLoading ) {
				size_t iDx = m_ResolvedNames.Add();
				CResolvedDomain&	theResolvedDomain( m_ResolvedNames.GetAt( iDx ) );
				theResolvedDomain.SetName( strResolvedName );
				theResolvedDomain.SetAddressString( strAddressList );
			}

			elemResolvedName.End();
		}

	}

	elemNames.End();

	ASSERT( nCount == m_ResolvedNames.GetCount() );

	return ( nCount == m_ResolvedNames.GetCount() );
}


// -----------------------------------------------------------------------------
//		ModelCountedList
// -----------------------------------------------------------------------------

bool
CDNSMessageOptions::ModelCountedList(
	CPeekDataElement&	ioElement )
{
	// Note: this modeling is only required on behalf of the view tabs.
	// The storing logic below is used by COmniEngineContext on formatting
	// a message return to a console.  The loading logic is used by 
	// COmniConsoleContext to create arrays simply for display by the 
	// view tabs.

	const bool bIsStoring = ioElement.IsStoring();	// Engine side only
	const bool bIsLoading = !bIsStoring;			// Console side only

	size_t nCount( 0 );

	CPeekDataElement	elemNames( DNSProcessTags::kElem_DomainCountedList, ioElement );
	if ( !elemNames ) return false;

	if ( bIsStoring ) {
		nCount = m_DomainCountedList.GetCount();
	}
	else {
		nCount = elemNames.GetChildCount();
		m_DomainCountedList.Reset();
	}

	for ( UInt32 i = 0; i < nCount; i++ ) {
		CPeekString strCountedName;
		UInt32		nQueryCount( 0 ); 
		UInt32		nResponseCount( 0 );

		CPeekDataElement elemDomainCounted( DNSProcessTags::kElem_DomainCounted, elemNames, i );
		if ( elemDomainCounted ) {

			if ( bIsStoring ) {
				const CDomainCounted&	theCountedDomain( m_DomainCountedList.GetAt( i ) );
				strCountedName = theCountedDomain.GetName();

				nQueryCount = theCountedDomain.GetQueryCount();
				nResponseCount = theCountedDomain.GetResponseCount();
			}

			CPeekDataElement elemName( DNSProcessTags::kElem_Domain, elemDomainCounted );
			if ( elemName ) {
				elemName.Value( strCountedName );
				elemName.End();
			}

			CPeekDataElement elemQueryCount( DNSProcessTags::kElem_QueryCount, elemDomainCounted );
			if ( elemQueryCount ) {
				elemQueryCount.Value( nQueryCount );
				elemQueryCount.End();
			}

			CPeekDataElement elemResponseCount( DNSProcessTags::kElem_ResponseCount, elemDomainCounted );
			if ( elemResponseCount ) {
				elemResponseCount.Value( nResponseCount );
				elemResponseCount.End();
			}

			if ( bIsLoading ) {
				size_t iDx = m_DomainCountedList.Add();

				CDomainCounted&	theCountedDomain( m_DomainCountedList.GetAt( iDx ) );
				theCountedDomain.SetName( strCountedName );
				theCountedDomain.SetQueryCount( nQueryCount );
				theCountedDomain.SetResponseCount( nResponseCount );
			}

			elemDomainCounted.End();
		}
	}

	elemNames.End();

	ASSERT( nCount == m_DomainCountedList.GetCount() );

//ptst
/*
	if ( bIsLoading ) {
		// Fill out the list with any uncounted domain names
		CDomainList& ayDomains( m_Options.GetDomainList() );

		const size_t nDomainCount( ayDomains.GetCount() );
		for ( size_t i = 0; i < nDomainCount; i++ ) {
			m_DomainCountedList.AddUnique( ayDomains.GetAt( i ) );
		}
	}
*/

	return true;
}
