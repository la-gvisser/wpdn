// =============================================================================
//	Snapshot.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#include "peekcore.h"
#include "peekstats.h"
#include "Snapshot.h"
#include "PeekTime.h"


// =============================================================================
//		CSummaryStat
// =============================================================================

CPeekString
CSummaryStat::Format() const
{
	CPeekOutString	strValue;

	switch( GetType() ) {
	case Peek::kSummaryStatType_Date:		// Date in nanoseconds since midnight 1/1/1601 UTC.
		{
			CPeekTime	ptDate( m_Stat.Value.Date );
			strValue << ptDate.FormatDate();
		}
		break;

	case Peek::kSummaryStatType_Time:		// Time in nanoseconds since midnight 1/1/1601 UTC.
		{
			CPeekTime	ptTime( m_Stat.Value.Time );
			strValue << ptTime.FormatTime();
		}
		break;

	case Peek::kSummaryStatType_Duration:	// Duration in nanoseconds.
		{
			// Convert to seconds.
			double dblDuration = m_Stat.Value.Duration / static_cast<double>( WPTIME_SECONDS );
//	#pragma message( "todo: setprecision" )
			//strValue.Format( L"%3.1f", dblDuration );
			strValue << dblDuration;
		}
		break;

	case Peek::kSummaryStatType_Packets:	// Packet count.
		strValue << m_Stat.Value.Packets;
		break;

	case Peek::kSummaryStatType_Bytes:	// Byte count.
		strValue << m_Stat.Value.Bytes;
		break;

	case Peek::kSummaryStatType_ValuePair:	// Packet count/byte count pair.
		strValue << m_Stat.Value.Packets << " / " << m_Stat.Value.ValuePair.Bytes;
		break;

	case Peek::kSummaryStatType_Int:		// Integer value.
		strValue << m_Stat.Value.IntValue;
		break;

	case Peek::kSummaryStatType_Double:	// Double value.
//	#pragma message( "todo: setprecision" )
		//strValue.Format( L"%3.1f", m_Stat.Value.DoubleValue );
		strValue << m_Stat.Value.DoubleValue;
		break;

	case Peek::kSummaryStatType_Null:
	default:
		break;
	}

	return strValue;
}


// =============================================================================
//		CSnapshot
// =============================================================================

// -----------------------------------------------------------------------------
//		SetItem
// -----------------------------------------------------------------------------

void
CSnapshot::SetItem(
	const GUID&			inId,
	const GUID&			inParentId,
	PCTSTR				inLabel,
	const Peek::SummaryStat*	inSummaryStat )
{
	if ( IsValid() ) {
		Helium::HeCID	cid( *(reinterpret_cast<const Helium::HeCID*>( &inId )) );
		Helium::HeCID	cidParent( *(reinterpret_cast<const Helium::HeCID*>( &inParentId )) );
		CHeBSTR	bstrLabel( inLabel );
		Peek::ThrowIfFailed( m_spSnapshot->SetItem( cid, cidParent, bstrLabel,
			reinterpret_cast<const PeekSummaryStat*>( inSummaryStat ) ) );
	}
}


// -----------------------------------------------------------------------------
//		UpdateSummaryStat
// -----------------------------------------------------------------------------

void
CSnapshot::UpdateSummaryStat(
	const GUID&						inParentId,
	const Peek::SummaryStatEntry&	inStatEntry,
	UInt32							nLabelId,
	bool							inNeedLabel )
{
	if ( IsValid() ) {
		Helium::HeCID	cid( *(reinterpret_cast<const Helium::HeCID*>( &inStatEntry.Id )) );
		Helium::HeCID	cidParent( *(reinterpret_cast<const Helium::HeCID*>( &inParentId )) );
		CHeBSTR	bstrLabel;

		if ( inNeedLabel ) {
			CPeekString	str;
#ifdef PEEK_UI
			str.LoadString( nLabelId );
#else
			nLabelId;
//	#pragma message( "todo: LoadString replacement.")
#endif
			bstrLabel = str;
		}

		Peek::ThrowIfFailed( m_spSnapshot->SetItem( cid, cidParent, bstrLabel,
			reinterpret_cast<const PeekSummaryStat*>( &inStatEntry.Stat ) ) );
	}
}
