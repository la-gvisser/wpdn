// ============================================================================
// EmailStats.cpp:
//      implementation of the CEmailStats class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "EmailStats.h"
#include "resource.h"


// ============================================================================
//		CStatsLabels
// ============================================================================

CEmailStats::CStatsLabels::CStatsLabels()
{
	for ( UInt32 i = 0; i < CEmailStats::kStat_MaxStat; i++ ) {
		CPeekString	str;
#ifdef PEEK_UI
		str.LoadString( IDS_SMTP_TOTAL + i );
#endif
		push_back( str );
	}
#ifdef _DEBUG
	try {
		size_t	nCount( size() );
		CPeekString	str( at( CEmailStats::kStat_StatCount ) );

		ASSERT( nCount == CEmailStats::kStat_MaxStat );
		ASSERT( str.Compare( L"Invalid Index" ) == 0 );
	}
	catch ( ... ) {
		int Oops( 1 );
		(void) Oops;
	}
#endif
}
