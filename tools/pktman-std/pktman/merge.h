// =============================================================================
//	merge.h
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#include "CaptureFile/CaptureFile.h"
#include "CaptureFile/WPTypes.h"

using namespace CaptureFile;


// =============================================================================
//		Data Structures
// =============================================================================

struct tMergeData {
	CPathList	fSources;
	path		fDestination;
	bool		fVerbose;
	bool		fDeltaTime;
	UInt64		fDeltaDuration;
	bool		fThinOut;
	size_t		fThinEvery;
	bool		fInterleave;

	UInt64	GetDuration() const { return fDeltaDuration; }

	bool	IsDeltaTime() const { return fDeltaTime; }
	bool	IsInterleave() const { return fInterleave; }
	bool	IsThinOut() const { return fThinOut; }
	bool	IsVerbose() const { return fVerbose; }
};


// =============================================================================
//		MergeFiles
// =============================================================================

int	MergeFiles( const tMergeData& inData );
