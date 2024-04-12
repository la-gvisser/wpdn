// ============================================================================
//	CaseOptionsList.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "CaseOptionsList.h"


// ============================================================================
//	CCaseOptionsList
// ============================================================================

// Options modeling labels
const PCWSTR	kEmpty( L"" );
const PCWSTR	kCase( L"Case" );
const PCWSTR	kCases( L"Cases" );


// -----------------------------------------------------------------------------
//		GetUniqueName
// -----------------------------------------------------------------------------

CPeekString
CCaseOptionsList::GetUniqueName( CPeekString theName )
{
	if ( IsNameUnique( theName, -1 ) ) return theName;

	CPeekString theNewName;
	int theNumber = 0;
	BOOL bUnique = false;

	do 
	{
		theNumber++;
		CPeekOutString theOutName;
		theOutName << theName << theNumber;
		theNewName = theOutName;
		if ( theNewName.GetLength() > MAX_NAME_LENGTH ) {
			break;
		}
		bUnique = IsNameUnique( (CPeekString)theNewName, -1 );
	} 
	while( !bUnique );

	if ( !bUnique ) {
		size_t nLen = theNewName.GetLength();
		ASSERT( nLen >= MAX_NAME_LENGTH );
		theNewName = theNewName.Left( MAX_NAME_LENGTH );
		nLen = theNewName.GetLength();
		for ( size_t i = nLen - 1; i >= 0; i-- ) {
			theNewName = theNewName.Left( i ) + _T("_") + theNewName.Right( nLen - i - 1 );
			if ( (bUnique = IsNameUnique( (CPeekString)theNewName, -1 )) == true ) {
				break;

			}
		}
	}

	if ( !bUnique ) {
		theNewName = _T("");
	}

	return (CPeekString)theNewName;
}

// -----------------------------------------------------------------------------
//		GetUniquePrefix
// -----------------------------------------------------------------------------

CPeekString
CCaseOptionsList::GetUniquePrefix( CPeekString inPrefix )
{
	if ( IsPrefixUnique( inPrefix ) ) return inPrefix;

	CString theNewPrefix;
	CString thePrefix( inPrefix );

	int theNumber = 0;
	BOOL bUnique = false;

	do 
	{
		theNumber++;
		theNewPrefix.Format( _T("%s%d"), thePrefix, theNumber);
		if ( theNewPrefix.GetLength() > MAX_NAME_LENGTH ) {
			break;
		}
		bUnique = IsPrefixUnique( (CPeekString)theNewPrefix, -1 );
	} 
	while( !bUnique );

	if ( !bUnique ) {
		int nLen = theNewPrefix.GetLength();
		ASSERT( nLen >= MAX_NAME_LENGTH );
		theNewPrefix = theNewPrefix.Left( MAX_NAME_LENGTH );
		nLen = theNewPrefix.GetLength();
		for ( int i = nLen - 1; i >= 0; i-- ) {
			theNewPrefix = theNewPrefix.Left( i ) + _T("_") + theNewPrefix.Right( nLen - i - 1 );
			if ( (bUnique = IsPrefixUnique( (CPeekString)theNewPrefix, -1 )) == true ) {
				break;

			}
		}
	}

	if ( !bUnique ) {
		theNewPrefix = _T("");
	}

	return (CPeekString)theNewPrefix;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CCaseOptionsList::Model(
	CPeekDataElement&	ioElement,
	bool				bUseFileNumber )
{
	const bool bIsStoring = ioElement.IsStoring();

	size_t nCount( 0 );

	CPeekDataElement	elemCaseOptions( kCases, ioElement );
	if ( !elemCaseOptions ) return false;

	if ( bIsStoring ) {
		nCount = GetCount();
	}
	else {
		nCount = elemCaseOptions.GetChildCount();
		Reset();
	}

	for ( UInt32 i = 0; i < nCount; i++ ) {
		CPeekDataElement elemCase( kCase, elemCaseOptions, i );
		if ( elemCase ) {
			CCaseOptions* pCaseOptions( NULL );

			if ( bIsStoring ) {
				pCaseOptions = GetAt( i );
			}
			else {
				pCaseOptions = new CCaseOptions;
				if ( pCaseOptions ) {
					pCaseOptions->Init( kEmpty, bUseFileNumber );
					Add( pCaseOptions );
				}
			}

			if ( !pCaseOptions ) {
				ASSERT( 0 );
				return false;
			}
			
			pCaseOptions->Model( elemCase );

			elemCase.End();
		}
	}

	elemCaseOptions.End();

	ASSERT( nCount == GetCount() );

	return true;
}


// -----------------------------------------------------------------------------
//		Sort
// -----------------------------------------------------------------------------

void
CCaseOptionsList::Sort()
{
	int nCount = (int)GetCount();
	if ( nCount < 2 ) return;

	int nSmallest = 0;

	for ( int i = 0; i < nCount; i++ ) {
		nSmallest = FindSmallest( i );
		if ( i != nSmallest ) Swap( i, nSmallest );
	}
}

// -----------------------------------------------------------------------------
//		FindSmallest
// -----------------------------------------------------------------------------

int
CCaseOptionsList::FindSmallest( int nStart )
{
	int nCount = (int)GetCount();
	int nSmallestIdx = nStart;

	for ( int i = nStart + 1; i < nCount; i++ ) {
		CCaseOptions* CaseOptions1 = GetAt( nSmallestIdx );
		CString csOne = CaseOptions1->GetName().MakeLower();

		CCaseOptions* CaseOptions2 = GetAt( i );
		CString csTwo = CaseOptions2->GetName().MakeLower();

		if ( csTwo < csOne ) {
			nSmallestIdx = i;	
		}
	}
	return nSmallestIdx;
}

// -----------------------------------------------------------------------------
//		Swap
// -----------------------------------------------------------------------------

void
CCaseOptionsList::Swap( int idx1, int idx2 )
{
	CCaseOptions* CaseOptions1 = GetAt( idx1 );
	CCaseOptions* CaseOptions2 = GetAt( idx2 );

	CCaseOptions* CaseOptionsTemp = CaseOptions1;

	SetAt( idx1, CaseOptions2 );
	SetAt( idx2, CaseOptionsTemp );
}
