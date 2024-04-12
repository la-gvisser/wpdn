// =============================================================================
//	compare.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include <WPTypes.h>

// =============================================================================
//		Data Structures
// =============================================================================

typedef struct _tCompareData {
	CStringList		fSources;
	std::wstring	fDestination;
	UInt8			fFlags;
	bool			fVerbose;

	bool	IsVerbose() const { return fVerbose; }
} tCompareData;


// =============================================================================
//		MergeFiles
// =============================================================================

int	CompareFiles( const tCompareData& inData );
