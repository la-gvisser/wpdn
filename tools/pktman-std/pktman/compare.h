// =============================================================================
//	compare.h
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

struct tCompareData {
	CPathList	fSources;
	path		fDestination;
	UInt8		fFlags;
	bool		fVerbose;

	bool	IsVerbose() const { return fVerbose; }
};


// =============================================================================
//		MergeFiles
// =============================================================================

int	CompareFiles( const tCompareData& inData );
