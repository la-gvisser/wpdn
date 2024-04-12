// ============================================================================
//	Utils.h
// ============================================================================
//	Coypright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "AGTypes.h"
#include <time.h>
#include <math.h>

extern const	UInt32	kSecondsInAMinute;
extern const	UInt32	kSecondsInAnHour;
extern const	UInt32	kSecondsInADay;

extern const	UInt64	kKilobyte;
extern const	UInt64	kMegabyte;
extern const	UInt64	kGigabyte;

extern const	UInt64	kFileTimeInASecond;

// ============================================================================
//		Utility Functions
// ============================================================================

UInt64	GetPeekTimeStamp();
UInt64	GetTimeStamp();
CString	GetTimeStringFileName();
