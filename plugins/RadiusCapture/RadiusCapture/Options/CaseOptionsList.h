// ============================================================================
//	CaseOptionsList.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CaseOptions.h"

class CCaseOptionsList
	: public CAtlArray < CCaseOptions* >
{

public:

	CCaseOptionsList() {}
	~CCaseOptionsList() {
		Reset();
	}

	bool operator==( const CCaseOptionsList& in) {
		size_t nMyCount = GetCount();
		size_t nInCount = in.GetCount();
		if ( nMyCount != nInCount ) return false;
		for ( size_t i = 0; i < GetCount(); i++ ) {
			CCaseOptions* MyCaseOptions = GetAt( i );
			CCaseOptions* InCaseOptions = in.GetAt( i );
			if ( *MyCaseOptions != *InCaseOptions ) return false;
		}
		return true;
	}
	bool operator!=( const CCaseOptionsList& in ) {
		return !operator==(in);
	}
	CCaseOptionsList& operator=( const CCaseOptionsList& in ){
		Reset();
		const size_t nInCount = in.GetCount();
		for ( size_t i = 0; i < nInCount; i++ ) {
			CCaseOptions* inCaseOptions = in.GetAt( i );
			CCaseOptions* newCaseOptions = new CCaseOptions;
			if ( !newCaseOptions ) {
				ASSERT( 0 );
				return *this;
			}
			*newCaseOptions = *inCaseOptions;
			Add( newCaseOptions );
		}
		return *this;
	}

	bool		IsNameMatch( CPeekString strName ) { return ( Lookup(strName) != NULL ); }
	CPeekString GetUniqueName( CPeekString theName );
	CPeekString GetUniquePrefix( CPeekString thePrefix );

	CCaseOptions* Lookup( CPeekString strName ) {
		for ( size_t i = 0; i < GetCount(); i++ ) {
			CCaseOptions* CaseOptions = GetAt( i );
			if ( CaseOptions->GetName().CompareNoCase( strName ) == 0 ){
				return CaseOptions;
			}
		}
		return NULL;
	}
	void DeleteItem( CPeekString strName )
	{
		int nPos = Find( strName );
		if ( nPos >= 0 ) {
			CCaseOptions* theCaseOptions = GetAt( nPos );
			delete theCaseOptions;
			RemoveAt( nPos );
		}
	}

	bool Model( CPeekDataElement& ioElement, bool bUseFileNumber );

	void SetEnabled( CPeekString strName, bool bEnabled ) {
		CCaseOptions* pCaseOptions = Lookup( strName );
		if ( pCaseOptions != NULL ) {
			if ( bEnabled == true ) pCaseOptions->SetEnabled();
			else pCaseOptions->SetDisabled();
		}
	}
	void SetEnabled( size_t nItem, bool bEnabled ) {
		ASSERT( nItem < GetCount() );
		CCaseOptions* theCaseOptions = GetAt( nItem );
		if ( bEnabled == true ) theCaseOptions->SetEnabled();
		else theCaseOptions->SetDisabled();
	}
	bool IsEnabled( size_t nItem ) {
		ASSERT( nItem < GetCount() );
		CCaseOptions* theCaseOptions = GetAt( nItem );
		return theCaseOptions->IsEnabled();
	}
	bool IsNameUnique( CPeekString strName, int nCaseOptionsListIdx = -1 ){
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			if ( i == nCaseOptionsListIdx ) continue;
			CCaseOptions* CaseOptions = GetAt( i );
			if ( CaseOptions->GetName().CompareNoCase( strName ) == 0 ){
				return false;
			}
		}
		return true;
	}
	bool IsPrefixUnique( CPeekString strPrefix, int nCaseOptionsListIdx = -1 ) {
		if ( strPrefix.GetLength() == 0 ) return true;
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			if ( i == nCaseOptionsListIdx ) continue;
			CCaseOptions* CaseOptions = GetAt( i );
			if ( CaseOptions->GetPrefix().CompareNoCase( strPrefix ) == 0 ){
				return false;
			}
		}
		return true;
	}
	CPeekString RemoveSpaces( CPeekString theString ) {
		CPeekString theNewString;
		size_t nNewIdx = 0;

		for ( size_t nOldIdx = 0; nOldIdx < theString.GetLength(); nOldIdx++ ) {
			if ( theString[nOldIdx] != ' ' ) {
				nNewIdx++;
				theNewString += theString[nOldIdx];
			}
		}
		return theNewString;
	}
	void ClearHighlighted() {
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = GetAt( i );
			theCaseOptions->SetHighlighted( false );
		}		
	}
	void SyncPortsToGlobal( CPortNumberList& inPortsList ) {
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = GetAt( i );
			theCaseOptions->SyncPortsToGlobal( inPortsList );
		}		
	}
	void SyncCaptureOptionsToGlobal( CCaptureOptions& inCaptureOptions ) {
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = GetAt( i );
			theCaseOptions->SyncCaptureOptionsToGlobal( inCaptureOptions );
		}		
	}
//	void SyncGeneralOptionsToGlobal( CPeekString inGlobalOutputRootDir, bool bUseCaseCaptureId, bool bUseFileNumber ){
	void SyncGeneralOptionsToGlobal( CPeekString inGlobalOutputRootDir, bool bUseFileNumber ){
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = GetAt( i );
			theCaseOptions->SyncGeneralOptionsToGlobal( inGlobalOutputRootDir, bUseFileNumber );
//			theCaseOptions->SyncGeneralOptionsToGlobal( inFacility, inGlobalOutputRootDir, bUseCaseCaptureId, bUseFileNumber );
		}		
	}	
//	void SyncGeneralOptionsToGlobal( CPeekString inFacility, CPeekString inGlobalOutputRootDir, bool bUseCaseCaptureId, bool bUseFileNumber ){
//		for ( int i = 0; i < (int)GetCount(); i++ ) {
//			CCaseOptions* theCaseOptions = GetAt( i );
//			theCaseOptions->SyncGeneralOptionsToGlobal( inFacility, inGlobalOutputRootDir, bUseCaseCaptureId, bUseFileNumber );
//		}		
//	}	
	bool	DisableInvalidCases() {
		bool bIsValidCase = false;
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			CCaseOptions* theCaseOptions = GetAt( i );
			if ( theCaseOptions->IsEnabled() ) {
				if ( theCaseOptions->CanCaseBeEnabled() ) {
					bIsValidCase = true;
				}
				else {
					theCaseOptions->SetDisabled();
				}
			}
		}
		return bIsValidCase;
	}
	void Reset() {
		const int nCount = static_cast<int>( GetCount() );
		for ( int i = nCount - 1; i >= 0; i-- ) {
			CCaseOptions* theCaseOptions = GetAt( i );
			delete theCaseOptions;
			RemoveAt( i );
		}
		RemoveAll(); 
	}
	void Sort();

protected:
	int		FindSmallest( int nStart );
	void	Swap( int idx1, int idx2 );

	int	Find( CPeekString strName ) {
		for ( int i = 0; i < (int)GetCount(); i++ ) {
			CCaseOptions* CaseOptions = GetAt( i );
			if ( CaseOptions->GetName().CompareNoCase( strName ) == 0 ){
				return i;
			}
		}
		return -1;
	}
};
