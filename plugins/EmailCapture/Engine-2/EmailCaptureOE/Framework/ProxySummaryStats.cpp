// =============================================================================
//	ProxySummaryStats.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#include "ProxySummaryStats.h"


// =============================================================================
//		CProxySummaryStats
// =============================================================================

// ----------------------------------------------------------------------------
//		GetSummaryItem
// ----------------------------------------------------------------------------

bool
CProxySummaryStats::GetSummaryItems(
	size_t				inIndex,
	CPeekString&		outGroup,
	CPeekString&		outLabel,
	CGlobalId&			outGroupId,
	CGlobalId&			outLabelId,
	PeekSummaryStat&	outSummaryStat )
{
	bool	bReturn = false;
	size_t	nEntryIndex = kIndex_Invalid;

	bReturn = m_spCurrentSnapshot->LabelIndexToEntryIndex( inIndex, nEntryIndex );
	_ASSERTE( bReturn );
	if ( !bReturn ) return false;

	SSummaryStatEntryPtr spStatEntry;
	bReturn =  m_spCurrentSnapshot->EntryIndexToEntryPtr( nEntryIndex, spStatEntry );
	_ASSERTE( bReturn );
	if ( !bReturn ) return false;

	size_t		nGroupIndex = kIndex_Invalid;
	CGlobalId	idLabel;

	bReturn = IndexToLabel( nEntryIndex, outLabel, nGroupIndex, idLabel );
	_ASSERTE( bReturn );
	if ( !bReturn ) return false;

	outLabelId = idLabel;

	CGlobalId	idGroup;
	bReturn = IndexToGroup( nGroupIndex, outGroup, idGroup );
	_ASSERTE( bReturn );
	if ( !bReturn ) return false;

	outGroupId = idGroup;

	bReturn = TranslateSummaryValue( spStatEntry, outSummaryStat );
	_ASSERTE( bReturn );
	if ( !bReturn ) return false;

	return true;
}


// ----------------------------------------------------------------------------
//		TranslateSummaryValue
// ----------------------------------------------------------------------------

bool
CProxySummaryStats::TranslateSummaryValue(
	const SSummaryStatEntryPtr	inStatEntry,
	PeekSummaryStat&			outSummaryStat ) const
{
	outSummaryStat.ValueType = peekSummaryStatTypeNull;
	outSummaryStat.Value.ValuePair.Bytes = 0;
	outSummaryStat.Value.ValuePair.Packets = 0;

//#pragma message( "petertest: what about kSummaryFlag_Mask" )
	UInt32 inType = inStatEntry->fType & kSummaryType_Mask;
	UInt32 inSize = inStatEntry->fType & kSummarySize_Mask;

	enum  {
		kSummaryType_OtherCount1 = kSummaryType_OtherCount + 1
	};

	bool bValueSet = false;

	if ( inType == kSummaryType_Date && inSize == kSummarySize_UInt64 ) {
		outSummaryStat.ValueType = peekSummaryStatTypeDate;
		outSummaryStat.Value.Date = (UInt64)inStatEntry->fValue.fUInt64;
		bValueSet = true;
	}
	else if ( inType == kSummaryType_Time && inSize == kSummarySize_UInt64 ) {
		outSummaryStat.ValueType = peekSummaryStatTypeTime;
		outSummaryStat.Value.Time = (UInt64)inStatEntry->fValue.fUInt64;
		bValueSet = true;
	}
	else if ( inType == kSummaryType_Duration ){
		SInt64 llDuration = 0;
		if ( inSize == kSummarySize_UInt64 ) {
			llDuration = (UInt64)inStatEntry->fValue.fUInt64;
			bValueSet = true;
		}
		else {// if ( inSize == kSummarySize_UInt32 ) {
			llDuration = (SInt64)inStatEntry->fValue.fUInt32;
			bValueSet = true;
		}

		if( bValueSet ) {
			outSummaryStat.ValueType = peekSummaryStatTypeDuration;
			outSummaryStat.Value.Duration = llDuration;
		}
	}
	else if ( inType == kSummaryType_PacketCount ) {
		UInt64 ullPackets;
		if ( inSize == kSummarySize_UInt64 ){
			ullPackets = (UInt64)inStatEntry->fValue.fUInt64;
			bValueSet = true;
		}
		else {// if ( inSize == kSummarySize_UInt32 ) {
			ullPackets = (UInt64)inStatEntry->fValue.fUInt32;
			bValueSet = true;
		}

		if( bValueSet ) {
			outSummaryStat.ValueType = peekSummaryStatTypePackets;
			outSummaryStat.Value.Packets = ullPackets;
		}
	}
	else if ( inType == peekSummaryStatTypeBytes ) {
		UInt64 ullBytes;
		if ( inSize == kSummarySize_UInt64 ) {
			ullBytes = (UInt64)inStatEntry->fValue.fUInt64;
			bValueSet = true;
		}
		else {// if ( inSize == kSummarySize_UInt32 ) {
			ullBytes = (UInt64)inStatEntry->fValue.fUInt32;
			bValueSet = true;
		}

		if( bValueSet ) {
			outSummaryStat.Value.Bytes = ullBytes;
			outSummaryStat.ValueType = peekSummaryStatTypeBytes;
		}
	}
	else if ( inType == kSummaryType_BothCount ) {//&& inSize == kSummarySize_BCPair ) {
		outSummaryStat.ValueType = peekSummaryStatTypeValuePair;
		outSummaryStat.Value.ValuePair.Bytes = inStatEntry->fValue.fBCPair.fBytes;
		outSummaryStat.Value.ValuePair.Packets = inStatEntry->fValue.fBCPair.fPackets;
		bValueSet = true;
	}
	if ( inType == kSummaryType_OtherCount ) {
		switch ( inSize )
		{
		case kSummarySize_UInt8:
		case kSummarySize_UInt16:
		case kSummarySize_UInt32:
			outSummaryStat.ValueType = peekSummaryStatTypeInt;
			outSummaryStat.Value.IntValue = static_cast<UInt64>( inStatEntry->fValue.fUInt32 );
			bValueSet = true;
			break;
		case kSummarySize_UInt64:
			outSummaryStat.ValueType = peekSummaryStatTypeInt;
			outSummaryStat.Value.IntValue = static_cast<UInt64>( inStatEntry->fValue.fUInt32 );
			bValueSet = true;
			break;
		case kSummarySize_BCPair:
			break;
		case kSummarySize_Double:
			outSummaryStat.ValueType = peekSummaryStatTypeDouble;
			outSummaryStat.Value.DoubleValue = static_cast<double>( inStatEntry->fValue.fDouble );
			bValueSet = true;
			break;
		default:
			_ASSERTE( inSize != inSize );
			break;
		}
	}

	return bValueSet;
}


// ----------------------------------------------------------------------------
//		UpdateSummaryStats
// ----------------------------------------------------------------------------

HRESULT
CProxySummaryStats::UpdateSummaryStats(
	IHeUnknown* pSnapshot )
{
	// Sanity checks.
	_ASSERTE( pSnapshot != NULL );
	if ( pSnapshot == NULL ) return E_POINTER;

	HRESULT	hr = S_OK;

	const size_t nCount = GetLabelCount();
	if ( nCount == 0 ) return S_OK;

	CHePtr<ISummaryStatsSnapshot>	spSnapshot;
	hr = pSnapshot->QueryInterface( &spSnapshot.p );
	_ASSERTE( SUCCEEDED(hr) );
	if ( FAILED(hr) ) throw( hr );

	CPeekString		strGroup;
	CPeekString		strLabel;
	CGlobalId		guidGroup;
	CGlobalId		guidLabel;
	PeekSummaryStat SummaryStat;

	for ( size_t i = 0; i < nCount; i++ ) {
		bool bReturn = GetSummaryItems( i, strGroup, strLabel, guidGroup, guidLabel, SummaryStat );
		_ASSERTE( bReturn );
		if ( !bReturn ) {
			return E_FAIL;
		}

		BOOL bHasGroup = FALSE;
		hr = spSnapshot->HasItem( guidGroup, &bHasGroup );
		_ASSERTE( SUCCEEDED( hr ) );
		if ( FAILED(hr) ) throw( hr );

		if ( !bHasGroup ) {
			CHeBSTR		bstrGroup( strGroup );
			CGlobalId	guidParent( GUID_NULL );
			hr = spSnapshot->SetItem( guidGroup, guidParent, bstrGroup, NULL );
			_ASSERTE( SUCCEEDED( hr ) );
			if ( FAILED( hr ) ) throw( hr );
		}
		CHeBSTR bstrLabel( strLabel );
		hr = spSnapshot->SetItem( guidLabel, guidGroup, bstrLabel, &SummaryStat );
	}
	return hr;
}

#endif // IMPLEMENT_SUMMARYMODIFYENTRY
