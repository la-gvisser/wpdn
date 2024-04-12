// ============================================================================
//	PeekSummarySnapshot.h
//		This is a modified and abridged version of OmniPeek SummarySnapshot.h
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#pragma once

#include "PeekArray.h"
#include <memory>
#include <std-tr1.h>


// ----------------------------------------------------------------------------
//		BCPair
// ----------------------------------------------------------------------------

struct BCPair {
	UInt64	fBytes;
	UInt64	fPackets;
};


// ----------------------------------------------------------------------------
//		SSummaryValue
// ----------------------------------------------------------------------------

union SSummaryValue {
	UInt32	fUInt32;
	UInt64	fUInt64;
	BCPair	fBCPair;
	double	fDouble;
};


// ----------------------------------------------------------------------------
//		SSummaryStatEntry
// ----------------------------------------------------------------------------

struct SSummaryStatEntry {
	size_t			fLabelIndex;
	UInt32			fType;
	SSummaryValue	fValue;
};

typedef NSTR1::shared_ptr<SSummaryStatEntry>	SSummaryStatEntryPtr;


// ============================================================================
//		CPeekSummarySnapshot
// ============================================================================

class CPeekSummarySnapshot
{
protected:
	struct SHashEntry {
		size_t		fIndex;
		SHashEntry*	fNext;
	};

	enum {
		kLabelHashCount	= 103
	};

	CPeekArray<SSummaryStatEntryPtr>	m_Entries;
	SHashEntry*						m_LabelHash[kLabelHashCount];

	bool	AddEntry( size_t inLabelIndex, size_t& outIndex );

	static size_t	CalculateLabelHash( size_t inLabelIndex ) {
		return (inLabelIndex % kLabelHashCount);
	}

public:
 	;		CPeekSummarySnapshot();
	;		~CPeekSummarySnapshot();

	bool	EntryIndexToEntry( size_t inEntryIndex, SSummaryStatEntry& outEntry ) const;
	bool	EntryIndexToEntryPtr( size_t inEntryIndex, SSummaryStatEntryPtr& outEntryPtr ) const;

	bool	LabelIndexToEntryIndex( size_t inLabelIndex, size_t& outEntryIndex ) const;
	void	LabelIndexToEntryIndexOrAdd( size_t inLabelIndex, size_t& outEntryIndex ) {
		if ( !LabelIndexToEntryIndex( inLabelIndex, outEntryIndex ) ) {
			AddEntry( inLabelIndex, outEntryIndex );
		}
	}
	bool	LabelIndexToValue( size_t inLabelIndex, SSummaryValue& outValue, UInt32& outType ) const;
};

typedef NSTR1::shared_ptr<CPeekSummarySnapshot>	CPeekSummarySnapshotPtr;

#endif // IMPLEMENT_SUMMARYMODIFYENTRY
