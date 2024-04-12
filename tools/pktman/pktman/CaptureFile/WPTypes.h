// ============================================================================
//	WPTypes.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once 

#ifdef __cplusplus
extern "C" {
#endif

#include "wppushpack.h"

#if defined(_WIN32)
typedef unsigned char		UInt8;
typedef signed char			SInt8;
typedef unsigned short		UInt16;
typedef signed short		SInt16;
typedef unsigned long		UInt32;
typedef signed long			SInt32;
typedef signed __int64		SInt64;
typedef unsigned __int64	UInt64;

#if defined(_MSC_VER)
#if (_MSC_VER < 1100)
typedef int bool;
#define true	1
#define false	0
#endif
#endif

#elif macintosh
// Note: other types are already defined
// in the MacOS Universal Headers.
#include <MacTypes.h>
typedef long long			SInt64;
typedef unsigned long long	UInt64;
#endif

#define MAX_UINT16	((UInt16) (-1))	// 0xFFFF
#define MAX_UINT32  ((UInt32) (-1)) // 0xFFFFFFFF
#define MAX_UINT64  ((UInt64) (-1)) // 0xFFFFFFFFFFFFFFFF

#include "wppoppack.h"

#if defined(_WPDLL)
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC 
#endif

#ifdef __cplusplus
}
#endif
