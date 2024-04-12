// =============================================================================
//	PeekSummaryStats.cpp
//		This is a modified and abridged version of OmniPeek SummaryStats.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#include "PeekSummarySnapshot.h"
#include "PeekSummaryStats.h"

#define OPT_SHOWBYTESANDPACKETS	0

//	#define kSummaryDatVersion	1	// this was the ANSI version
#define kSummaryDatVersion	2		// this is the UNICODE version


// =============================================================================
//		CPeekSummaryStats
// =============================================================================

CPeekSummaryStats::CPeekSummaryStats()
	:	m_bResetAllSnapshots( true )
	,	m_nResetCount( 0 )
{
	InitData();
}

CPeekSummaryStats::~CPeekSummaryStats()
{
	ResetData();
}


// -----------------------------------------------------------------------------
//		AddGroup
// -----------------------------------------------------------------------------

size_t
CPeekSummaryStats::AddGroup(
	const CPeekString&	inGroup )
{
	size_t	nGroupIndex = kIndex_Invalid;

	_ASSERTE( !inGroup.IsEmpty() );
	if ( inGroup.IsEmpty() ) return nGroupIndex;

	try {
		// Allocate space for another group entry.
		nGroupIndex = m_Groups.Add();
		SSummaryGroup&	Group( m_Groups.GetAt( nGroupIndex ) );

		// Copy the group name and create an unique id.
		Group.fName = inGroup;
		Group.fId.Create();

		// Add a hash entry for the group.
		SHashItem*	pHashItem = new SHashItem;
		size_t		nHashIndex = CalculateGroupHash( inGroup );
		pHashItem->fIndex = nGroupIndex;
		pHashItem->fNext = m_GroupHash[nHashIndex];
		m_GroupHash[nHashIndex] = pHashItem;
	}
	catch(...) {
		if ( nGroupIndex != kIndex_Invalid ) {
			m_Groups.RemoveAt( nGroupIndex );
			nGroupIndex = kIndex_Invalid;
		}
	}
	return nGroupIndex;
}


// -----------------------------------------------------------------------------
//		AddLabel
// -----------------------------------------------------------------------------

size_t
CPeekSummaryStats::AddLabel(
	const CPeekString&	inLabel,
	const CPeekString&	inGroup )
{
	size_t	nGroupIndex = kIndex_Invalid;
	size_t	nLabelIndex = kIndex_Invalid;

	SSummaryLabel*	pLabel = NULL;

	// Try to find the group.
	if ( !inGroup.IsEmpty() ) {
		nGroupIndex = GroupToIndex( inGroup );
		if ( nGroupIndex == kIndex_Invalid ) {
			nGroupIndex = AddGroup( inGroup );
		}
	}
	else {
		return nLabelIndex;	// NULL group name not allowed
	}

	if ( !inLabel.IsEmpty() ) {
		try {
			// Allocate space for another label entry.
			nLabelIndex = m_Labels.Add();
			SSummaryLabel&	Label( m_Labels.GetAt( nLabelIndex ) );

			// Copy the label name and create an unique id.
			Label.fName = inLabel;
			Label.fGroupId = nGroupIndex;
			Label.fId.Create();

			// Add a hash entry.
			SHashItem*	pHashItem = new SHashItem;
			size_t		nHashIndex = CalculateLabelHash( inLabel );
			pHashItem->fIndex = nLabelIndex;
			pHashItem->fNext = m_LabelHash[nHashIndex];
			m_LabelHash[nHashIndex] = pHashItem;
		}
		catch( ... ) {
			if ( nLabelIndex != kIndex_Invalid ) {
				m_Labels.RemoveAt( nLabelIndex );
				nLabelIndex = kIndex_Invalid;
			}
			delete pLabel;
		}
	}
	return nLabelIndex;
}


// -----------------------------------------------------------------------------
//		GroupToIndex
// -----------------------------------------------------------------------------

size_t
CPeekSummaryStats::GroupToIndex(
	const CPeekString&	inGroup ) const
{
	// Sanity check.
	_ASSERTE( !inGroup.IsEmpty() );
	if ( inGroup.IsEmpty() ) return kIndex_Invalid;

	// Calculate the hash.
	size_t		nHashValue = CalculateGroupHash( inGroup );
	SHashItem*	pHashItem = m_GroupHash[nHashValue];

	while ( pHashItem != NULL ) {
		// Get the group.
		_ASSERTE( pHashItem->fIndex < m_Groups.GetCount() );
		if ( pHashItem->fIndex < m_Groups.GetCount() ) {
			if ( m_Groups[pHashItem->fIndex].fName.CompareNoCase( inGroup ) == 0 ) {
				return pHashItem->fIndex;
			}
		}
		pHashItem = pHashItem->fNext;	// Get the next hash item.
	}
	return kIndex_Invalid;
}


// -----------------------------------------------------------------------------
//		IndexToLabel
// -----------------------------------------------------------------------------

bool
CPeekSummaryStats::IndexToLabel(
	size_t			inLabelIndex,
	CPeekString&	outLabel,
	size_t&			outGroupId,
	CGlobalId&		outId ) const
{
	if ( inLabelIndex >= m_Labels.GetCount() ) return false;
	outLabel = m_Labels[inLabelIndex].fName;
	outGroupId = m_Labels[inLabelIndex].fGroupId;
	outId = m_Labels[inLabelIndex].fId;
	return true;
}


// -----------------------------------------------------------------------------
//		IndexToGroup
// -----------------------------------------------------------------------------

bool
CPeekSummaryStats::IndexToGroup(
	size_t			inIndex,
	CPeekString&	outGroup,
	CGlobalId&		outId ) const
{
	_ASSERTE( inIndex < m_Groups.GetCount() );
	if ( inIndex >= m_Groups.GetCount() ) return false;

	outGroup = m_Groups[inIndex].fName;
	outId = m_Groups[inIndex].fId;
	return true;
}


// -----------------------------------------------------------------------------
//		InitData
// -----------------------------------------------------------------------------

void
CPeekSummaryStats::InitData()
{
	m_spCurrentSnapshot = CPeekSummarySnapshotPtr( new CPeekSummarySnapshot );
	if ( m_spCurrentSnapshot == NULL ) throw -1;

	m_Snapshots.Add( m_spCurrentSnapshot );

	// Initialize the group hash table and label hash table.
	memset( m_GroupHash, 0, sizeof( m_GroupHash ) );
	memset( m_LabelHash, 0, sizeof( m_LabelHash ) );
}


// -----------------------------------------------------------------------------
//		LabelToIndex
// -----------------------------------------------------------------------------

size_t
CPeekSummaryStats::LabelToIndex(
	const CPeekString&	inLabel,
	size_t				inGroupId ) const
{
	// Calculate the hash.
	size_t		nHashIndex = CalculateLabelHash( inLabel );
	SHashItem*	pHashItem = m_LabelHash[nHashIndex];

	while ( pHashItem != NULL ) {
		_ASSERTE( pHashItem->fIndex < m_Labels.GetCount() );
		// Compare the label.
		if ( m_Labels[pHashItem->fIndex].fName.CompareNoCase( inLabel ) == 0 ) {
			// does the caller care about group ID?
			if ( (inGroupId == kIndex_Invalid) || (inGroupId == m_Labels[pHashItem->fIndex].fGroupId) ) {
				return pHashItem->fIndex;
			}
		}
		pHashItem = pHashItem->fNext;	// Get the next hash item.
	}
	return kIndex_Invalid;
}


// -----------------------------------------------------------------------------
//		ModifyEntry
// -----------------------------------------------------------------------------

void
CPeekSummaryStats::ModifyEntry(
	const CPeekString&	inLabel,
	const CPeekString&	inGroup,
	UInt32				inType,
	const void*			inData )
{
	// Sanity checks.
	_ASSERTE( !inLabel.IsEmpty() );
	if ( inLabel.IsEmpty() ) return;
	_ASSERTE( inData != NULL );
	if ( inData == NULL ) return;

	size_t	nGroupIndex = GroupToIndex( inGroup );
	if ( nGroupIndex == kIndex_Invalid ) {
		nGroupIndex = AddGroup( inGroup );
	}

	// Convert the label to a label index.
	size_t	nLabelIndex = LabelToIndex( inLabel, nGroupIndex );

	if ( nLabelIndex == kIndex_Invalid ) {
		nLabelIndex = AddLabel( inLabel, inGroup );	// Add the label index.
	}

	_ASSERTE( nLabelIndex != kIndex_Invalid );
	if ( nLabelIndex == kIndex_Invalid ) return;

	if ( m_spCurrentSnapshot != NULL ) {
		size_t	nEntryIndex = kIndex_Invalid;
		m_spCurrentSnapshot->LabelIndexToEntryIndexOrAdd( nLabelIndex, nEntryIndex );

		SSummaryStatEntryPtr	spEntry;
		if ( (m_spCurrentSnapshot->EntryIndexToEntryPtr( nEntryIndex, spEntry )) && spEntry ) {
			_ASSERTE( spEntry );
			spEntry->fType = (inType & (kSummaryType_Mask | kSummaryFlag_Mask) );

			switch ( (inType & kSummarySize_Mask) ) {
			case kSummarySize_UInt8:
				spEntry->fValue.fUInt32 = *static_cast<const UInt8*>( inData );
				spEntry->fType |= kSummarySize_UInt32;
				break;

			case kSummarySize_UInt16:
				spEntry->fValue.fUInt32 = *static_cast<const UInt16*>( inData );
				spEntry->fType |= kSummarySize_UInt32;
				break;

			case kSummarySize_UInt32:
				spEntry->fValue.fUInt32 = *static_cast<const UInt32*>( inData );
				spEntry->fType |= kSummarySize_UInt32;
				break;

			case kSummarySize_UInt64:
				spEntry->fValue.fUInt64 = *static_cast<const UInt64*>( inData );
				spEntry->fType |= kSummarySize_UInt64;
				break;

			case kSummarySize_BCPair:
				spEntry->fValue.fBCPair = *static_cast<const BCPair*>( inData );
				spEntry->fType |= kSummarySize_BCPair;
				break;

			case kSummarySize_Double:
				spEntry->fValue.fDouble = *static_cast<const double*>( inData );
				spEntry->fType |= kSummarySize_Double;
				break;
			}
		}
	}
}


// -----------------------------------------------------------------------------
//		ResetData
// -----------------------------------------------------------------------------

void
CPeekSummaryStats::ResetData()
{
	// Delete all of the snapshots.
	m_Snapshots.RemoveAll();

	// Cleanup group hash.
	for ( int i = 0; i < kGroupHashTableSize; i++ ) {
		SHashItem*	pHashItem = m_GroupHash[i];
		while ( pHashItem != NULL ) {
			SHashItem*	pNextHashItem = pHashItem->fNext;
			delete pHashItem;
			pHashItem = pNextHashItem;
		}
	}

	// Cleanup label hash.
	for ( int j = 0; j < kLabelHashTableSize; j++ ) {
		SHashItem*	pHashItem = m_LabelHash[j];
		while ( pHashItem != NULL ) {
			SHashItem*		pNextHashItem = pHashItem->fNext;
			delete pHashItem;
			pHashItem = pNextHashItem;
		}
	}

	m_Groups.RemoveAll();
	m_Labels.RemoveAll();

	memset( m_GroupHash, 0, sizeof( m_GroupHash ) );
	memset( m_LabelHash, 0, sizeof( m_LabelHash ) );

	m_spCurrentSnapshot.reset();
}

#endif // IMPLEMENT_SUMMARYMODIFYENTRY
