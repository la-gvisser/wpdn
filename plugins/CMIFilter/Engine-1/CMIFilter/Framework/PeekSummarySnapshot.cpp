// =============================================================================
//	PeekSummarySnapshot.cpp
//		This is a modified and abridged version of OmniPeek SummarySnapshot.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#include "PeekSummarySnapshot.h"


// =============================================================================
//		CPeekSummarySnapshot
// =============================================================================

CPeekSummarySnapshot::CPeekSummarySnapshot()
{
	memset( &m_LabelHash, 0, sizeof( m_LabelHash ) );
}

CPeekSummarySnapshot::~CPeekSummarySnapshot()
{
	// Delete the label hash table.
	for ( int i = 0; i < kLabelHashCount; i++ ) {
		// Get the hash entry.
		SHashEntry*	pHashEntry = m_LabelHash[i];
		m_LabelHash[i] = nullptr;

		// Delete the chain of entries.
		while ( pHashEntry != nullptr ) {
			SHashEntry*	pNextHashEntry = pHashEntry->fNext;
			delete pHashEntry;
			pHashEntry = pNextHashEntry;
		}
	}

	m_Entries.RemoveAll();
}


// -----------------------------------------------------------------------------
//		LabelIndexToValue
// -----------------------------------------------------------------------------

bool
CPeekSummarySnapshot::LabelIndexToValue(
	size_t			inLabelIndex,
	SSummaryValue&	outValue,
	UInt32&			outType ) const
{
	size_t	nEntryIndex;
	if ( LabelIndexToEntryIndex( inLabelIndex, nEntryIndex ) ) {
		SSummaryStatEntryPtr	spEntry;
		if ( EntryIndexToEntryPtr( nEntryIndex, spEntry ) ) {
			outValue = spEntry->fValue;
			outType = spEntry->fType;
			return true;
		}
	}
	return false;
}


// -----------------------------------------------------------------------------
//		LabelIndexToEntryIndex
// -----------------------------------------------------------------------------

bool
CPeekSummarySnapshot::LabelIndexToEntryIndex(
	size_t		inLabelIndex,
	size_t&		outIndex ) const
{
	size_t		nHashIndex = CalculateLabelHash( inLabelIndex );
	SHashEntry*	pHashEntry = m_LabelHash[nHashIndex];

	while ( pHashEntry != nullptr ) {
		SSummaryStatEntryPtr	spEntry = m_Entries.GetAt( inLabelIndex );
		_ASSERTE( spEntry != nullptr );

		if ( inLabelIndex == spEntry->fLabelIndex ) {
			outIndex = pHashEntry->fIndex;
			return true;
		}
		pHashEntry = pHashEntry->fNext;
	}
	return false;
}


// -----------------------------------------------------------------------------
//		EntryIndexToEntry
// -----------------------------------------------------------------------------

bool
CPeekSummarySnapshot::EntryIndexToEntry(
	 size_t				inIndex,
	 SSummaryStatEntry	&outEntry ) const
{
	SSummaryStatEntryPtr	spEntry = m_Entries.GetAt( inIndex );
	if ( spEntry ) {
		outEntry = *spEntry;
		return true;
	}
	return false;
}


// -----------------------------------------------------------------------------
//		EntryIndexToEntryPtr
// -----------------------------------------------------------------------------

bool
CPeekSummarySnapshot::EntryIndexToEntryPtr(
	 size_t					inIndex,
	 SSummaryStatEntryPtr&	outEntryPtr ) const
{
	if ( inIndex >= m_Entries.GetCount() ) return false;

	outEntryPtr = m_Entries.GetAt( inIndex );
	return (outEntryPtr != nullptr);
}


// -----------------------------------------------------------------------------
//		AddEntry
// -----------------------------------------------------------------------------

bool
CPeekSummarySnapshot::AddEntry(
	size_t		inLabelIndex,
	size_t&		outIndex )
{
	outIndex = kIndex_Invalid;

	SHashEntry*				pHashEntry = nullptr;
	SSummaryStatEntryPtr	spSummaryStatEntry;

	try {
		spSummaryStatEntry = SSummaryStatEntryPtr( new SSummaryStatEntry );
		if ( spSummaryStatEntry == nullptr ) throw -1;

		spSummaryStatEntry->fLabelIndex = inLabelIndex;
		spSummaryStatEntry->fType = 0;
		memset( &spSummaryStatEntry->fValue, 0, sizeof( SSummaryValue ) );

		outIndex = m_Entries.Add( spSummaryStatEntry );
		if ( outIndex < 0 ) throw -1;

		// add a hash entry item
		pHashEntry = new SHashEntry;
		if ( pHashEntry == nullptr ) throw -1;
		size_t	nHashIndex = CalculateLabelHash( spSummaryStatEntry->fLabelIndex );

		pHashEntry->fIndex = outIndex;
		pHashEntry->fNext = m_LabelHash[nHashIndex];
		m_LabelHash[nHashIndex] = pHashEntry;
	}
	catch( ... ) {
		if ( outIndex != kIndex_Invalid ) {
			m_Entries.RemoveAt( outIndex );
		}
		delete pHashEntry;
		return false;
	}
	return true;
}

#endif // IMPLEMENT_SUMMARYMODIFYENTRY