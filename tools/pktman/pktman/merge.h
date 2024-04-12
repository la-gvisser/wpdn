// =============================================================================
//	merge.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include <WPTypes.h>


// =============================================================================
//		Data Structures
// =============================================================================

typedef struct _tMergeData {
	CStringList		fSources;
	std::wstring	fDestination;
	bool			fVerbose;
	bool			fDeltaTime;
	UInt64			fDeltaDuration;
	bool			fThinOut;
	size_t			fThinEvery;
	bool			fInterleave;

	UInt64	GetDuration() const { return fDeltaDuration; }

	bool	IsDeltaTime() const { return fDeltaTime; }
	bool	IsInterleave() const { return fInterleave; }
	bool	IsThinOut() const { return fThinOut; }
	bool	IsVerbose() const { return fVerbose; }
} tMergeData;


// =============================================================================
//		MergeFiles
// =============================================================================

int	MergeFiles( const tMergeData& inData );
