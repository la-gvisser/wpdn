// ============================================================================
//	OptionsDlg.cpp:
//		implementation of the COptionsDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#include "StdAfx.h"
#include "Domain.h"


// ============================================================================
//		CDomain
// ============================================================================

// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CDomain::IsMatch(
	CPeekString	inAddress ) const
{
	if ( IsWildCard() ) {
		size_t	nLength = GetLength() - 1; // Pickup the leading '.'.
		CPeekString	strPattern = Right( nLength );
		CPeekString	strAddress = inAddress.Right( nLength );
		return (strPattern.CompareNoCase( strAddress ) == 0);
	}
	return (CompareNoCase( inAddress ) == 0);
}


// ----------------------------------------------------------------------------
//		Validate
// ----------------------------------------------------------------------------

bool
CDomain::Validate() const {
	if ( IsEmpty() ) return false;
	
	int	nStart( 0 );
	if ( GetAt( 0 ) == L'*' ) {
		if ( GetLength() < 2 ) return false;
		if ( GetAt( 1 ) != L'.' ) return false;
		nStart = 2;
	}

	if ( Find( L'*', nStart ) >= 0 ) return false;
	if ( GetAt( 0 ) == L'.' ) return false;

	if ( Right( 1 ) == (CPeekString)L"." ) return false;
	if ( Find( L".." ) >= 0 ) return false;

	return true;
}


// ============================================================================
//		CDomainList
// ============================================================================

// ============================================================================
//		CDomainCounted
// ============================================================================

// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CDomainCounted::IsMatch(
	CPeekString	inName ) const
{
	if ( m_Name.IsWildCard() ) {
		size_t	nLength = m_Name.GetLength() - 1; // Pickup the leading '.'.

		if ( inName.GetLength() < nLength ) {
			return false;
		}

		CPeekString	strMyPattern = m_Name.Right( nLength );
		CPeekString	strInPattern = inName.Right( nLength );

		return (strMyPattern.CompareNoCase( strInPattern ) == 0);
	}

	return (m_Name.CompareNoCase( inName ) == 0);
}


// ============================================================================
//		CDomainCountedList
// ============================================================================

// ----------------------------------------------------------------------------
//		operator =
// ----------------------------------------------------------------------------

CDomainCountedList&
CDomainCountedList::operator=( const CDomainList& inOther )
{
	PruneList( inOther );

	size_t	nCount = inOther.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		const CDomain&	theDomain( inOther.GetAt( i ) );
		AddUnique( theDomain );
	}
	return *this;
}


// ----------------------------------------------------------------------------
//		PruneList
// ----------------------------------------------------------------------------

void
CDomainCountedList::PruneList(
	const CDomainList&	inNames )
{
	CAtlArray<size_t>	ayPrune;
	size_t	nCount = GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		size_t			nIndex( 0 );
		CDomainCounted&	theDomain( GetAt( i ) );
		if ( !inNames.Find( theDomain.GetName(), nIndex ) ) {
			ayPrune.InsertAt( 0, i ); // Insert at the head of the array.
		}
	}

	size_t	nPruneCount = ayPrune.GetCount();
	for ( size_t i = 0; i < nPruneCount; i++ ) {
		RemoveAt( ayPrune[i] );
	}
}


// ============================================================================
//		CResolvedDomain
// ============================================================================

// ============================================================================
//		CResolvedDomainList
// ============================================================================

// ----------------------------------------------------------------------------
//		PruneList
// ----------------------------------------------------------------------------

void
CResolvedDomainList::PruneList(
	const CDomainList&	inNames )
{
	CAtlArray<size_t>	ayPrune;
	size_t	nCount = GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CResolvedDomain&	theDomain( GetAt( i ) );
		if ( !inNames.IsMatch( theDomain.GetName() ) ) {
			ayPrune.InsertAt( 0, i ); // Insert at the head of the array.
		}
	}

	size_t	nPruneCount = ayPrune.GetCount();
	for ( size_t i = 0; i < nPruneCount; i++ ) {
		RemoveAt( ayPrune[i] );
	}
}


// ----------------------------------------------------------------------------
//		UpdateAddUnique
// ----------------------------------------------------------------------------

size_t
CResolvedDomainList::UpdateAddUnique(
	const CResolvedDomain&	inResolved )
{
	size_t	nIndex( 0 );
	if ( !Find( inResolved.GetName(), nIndex ) ) {
		nIndex = Add( inResolved );
	}
	CResolvedDomain&	theResolve( GetAt( nIndex ) );
	theResolve.SetAddressList( inResolved.GetAddressList() );
	return nIndex;
}
