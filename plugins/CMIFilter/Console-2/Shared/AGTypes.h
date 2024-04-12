// ============================================================================
//	AGTypes.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#ifndef __AGTYPES_H
#define __AGTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(macintosh)
#pragma pack(push,1)
#endif

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
#define true				1
#define false				0
#endif
#endif
#elif macintosh
// Note: other types are already defined
// in the MacOS Universal Headers.
#include <MacTypes.h>
typedef long long			SInt64;
typedef unsigned long long	UInt64;
#endif

#define MAX_UINT16	((UInt16) (-1))	// 0xffff
#define MAX_UINT32	((UInt32) (-1))	// 0xffffffff

#if defined(_WIN32) || defined(macintosh)
#pragma pack(pop)
#endif

#if defined(_WPDLL)
#define DECLSPEC __declspec(dllexport)
#else
#define DECLSPEC
#endif

#ifdef __cplusplus
}
#endif

#endif // __AGTYPES_H
