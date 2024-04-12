// ============================================================================
// RadiusStats.cpp:
//      implementation of the CRadiusStats class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#include "StdAfx.h"

#ifdef RADIUS_STATS
#include "RadiusStats.h"
#include "Radius.h"


///////////////////////////////////////////////////////////////////////////////
//		CStatsLabels
///////////////////////////////////////////////////////////////////////////////

CRadiusStats::CStatsLabels::CStatsLabels(
	CPeekPlugin&	inPlugin )
{
	HINSTANCE	hInst = inPlugin.GetResourceHandle();
	for ( size_t i = 0; i < CRadiusStats::kStat_MaxStat; i++ ) {
		CString	str;
		str.LoadString( hInst, (IDS_SMTP_TOTAL + i) );
		Add( str );
	}
#ifdef _DEBUG
	try {
		size_t	nCount = GetCount();
		CString	str = GetAt( CRadiusStats::kStat_StatCount );

		ASSERT( nCount == CRadiusStats::kStat_MaxStat );
		ASSERT( str.Compare( _T("Invalid Index") ) == 0 );
	}
	catch ( ... ) {
		int Oops = 1;
		Oops;
	}
#endif
}


///////////////////////////////////////////////////////////////////////////////
//		CRadiusStats
///////////////////////////////////////////////////////////////////////////////

CRadiusStats::CRadiusStats(
	CPeekPlugin&	inPlugin )
	:	m_aLabels( inPlugin )
{
	Reset();
}


CRadiusStats::~CRadiusStats()
{
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
CRadiusStats::Reset()
{
	for ( size_t i = 0; i < kStat_MaxStat; i++ ) {
		m_aStats[i] = 0;
	}
}
#endif //RADIUS_STATS
