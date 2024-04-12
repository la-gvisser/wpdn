// =============================================================================
//	AGTypes.h
// =============================================================================
//	Copyright (c) 1997-2007 WildPackets, Inc. All rights reserved.

#ifndef AGTYPES_H
#define AGTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

// Types for ISO Standard C (ISO/IEC 9899:1999).
typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef unsigned long UInt32;
typedef signed long SInt32;
typedef signed long long SInt64;
typedef unsigned long long UInt64;

#if defined(_WPDLL)
	#define DECLSPEC __declspec(dllexport)
#else
	#define DECLSPEC 
#endif

#ifdef __cplusplus
}
#endif

#endif
