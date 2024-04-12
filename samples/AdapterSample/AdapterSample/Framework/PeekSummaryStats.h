// ============================================================================
//	PeekSummaryStats.h
//		This is a modified and abridged version of OmniPeek SummaryStats.h
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY

#pragma once

#include "PeekArray.h"
#include "PeekStrings.h"
#include "GlobalId.h"
#include <memory>
#include <std-tr1.h>

class CPeekSummarySnapshot;
typedef NSTR1::shared_ptr<CPeekSummarySnapshot>	CPeekSummarySnapshotPtr;
union SSummaryValue;


// ----------------------------------------------------------------------------
//		Summary Constants
// ----------------------------------------------------------------------------

enum
{
	kSummarySize_Mask			= 0x00F0,
	kSummarySize_UInt8 			= 0x0010,
	kSummarySize_UInt16			= 0x0020,
	kSummarySize_UInt32 		= 0x0030,
	kSummarySize_UInt64 		= 0x0040,
	kSummarySize_BCPair			= 0x0050,
	kSummarySize_Double			= 0x0060,

	kSummaryType_Mask			= 0x000F,
	kSummaryType_Date			= 0x0001,
	kSummaryType_Time			= 0x0002,
	kSummaryType_Duration		= 0x0003,
	kSummaryType_PacketCount	= 0x0004,
	kSummaryType_ByteCount		= 0x0005,
	kSummaryType_BothCount		= 0x0006,
	kSummaryType_OtherCount		= 0x0007,

	kSummaryFlag_Mask			= 0x0F00,
	kSummaryFlag_Ungraphable	= 0x0100,
	kSummaryFlag_IsOverTime		= 0x0200,
	kSummaryFlag_IsSampled		= 0x0400
};

enum
{
	kSummaryStats_GroupLength	= 64,
	kSummaryStats_LabelLength	= 256
};


// ----------------------------------------------------------------------------
//		SSummaryGroup
// ----------------------------------------------------------------------------

struct SSummaryGroup
{
	CPeekString	fName;
	CGlobalId	fId;
};


// ----------------------------------------------------------------------------
//		SSummaryLabel
// ----------------------------------------------------------------------------

struct SSummaryLabel
{
	CPeekString	fName;
	size_t		fGroupId;
	CGlobalId	fId;

	SSummaryLabel() { fGroupId = 0;	}
};


// ============================================================================
//		CPeekSummaryStats
// ============================================================================

class CPeekSummaryStats
{
public:
	enum {
		kMsg_SummaryNewSnapshot		= 'SUMS',
		kMsg_SummaryDeleteSnapshot	= 'SUMD'
	};

protected:
	enum {
		kGroupHashTableSize			= 23,
		kLabelHashTableSize			= 3079
	};

	struct SHashItem {
		size_t			fIndex;
		SHashItem*		fNext;
	};

	CPeekArray<SSummaryLabel>			m_Labels;
	CPeekArray<SSummaryGroup>			m_Groups;
	CPeekArray<CPeekSummarySnapshotPtr>	m_Snapshots;
	CPeekSummarySnapshotPtr				m_spCurrentSnapshot;
	SHashItem*							m_GroupHash[kGroupHashTableSize];
	SHashItem*							m_LabelHash[kLabelHashTableSize];
	size_t								m_nResetCount;
	bool								m_bResetAllSnapshots;

	size_t				AddGroup( const CPeekString& inGroup );
	size_t				AddLabel( const CPeekString& inLabel, const CPeekString& inGroup );
	unsigned long		CalculateGroupHash( const CPeekString& psz ) const {
		return StringHash::DJB2( psz ) % kGroupHashTableSize;
	}
	unsigned long		CalculateLabelHash( const CPeekString& psz ) const {
		return StringHash::DJB2( psz ) % kLabelHashTableSize;
	}

public:
	;					CPeekSummaryStats();
	virtual				~CPeekSummaryStats();

	void				InitData();

	size_t				GetLabelCount() const { return m_Labels.GetCount(); }
	size_t				GroupToIndex( const CPeekString& inGroup ) const;

	bool				IndexToLabel( size_t inIndex, CPeekString& outLabel, size_t& outGroupId, CGlobalId& outId ) const;
	bool				IndexToGroup( size_t inGroup, CPeekString& outGroup, CGlobalId& outId ) const;

	size_t				LabelToIndex( const CPeekString& inLabel, size_t inGroupId = kIndex_Invalid ) const;

	void				ModifyEntry( const CPeekString& inLabel, const CPeekString& inGroup, UInt32 inType, const void* inData );

	void				ResetData();
};

#endif // IMPLEMENT_SUMMARYMODIFYENTRY
