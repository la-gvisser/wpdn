// ============================================================================
//	ProxySummaryStats.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#pragma once

#include "PeekSummaryStats.h"
#include "PeekStats.h"
#include "PeekSummarySnapshot.h"


// ============================================================================
//		CProxySummaryStats
// ============================================================================

class CProxySummaryStats
	: public CPeekSummaryStats
{
public:
	;		CProxySummaryStats() {}
	;		~CProxySummaryStats() {}

	HRESULT UpdateSummaryStats( Helium::IHeUnknown* pSnapshot );

protected:
	bool	GetSummaryItems( size_t inIndex, CPeekString& outGroup, CPeekString& outLabel, CGlobalId& outGroupId,
				CGlobalId& outLabelId, PeekSummaryStat& outSummaryStat );
	bool	TranslateSummaryValue( const SSummaryStatEntryPtr inStatEntry, PeekSummaryStat& outSummaryStat ) const;
};

#endif // IMPLEMENT_SUMMARYMODIFYENTRY
