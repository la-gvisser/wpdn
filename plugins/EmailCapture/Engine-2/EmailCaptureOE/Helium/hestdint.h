// =============================================================================
//	hestdint.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef HESTDINT_H
#define HESTDINT_H

// Integer types for ISO Standard C (ISO/IEC 9899:1999).

#ifndef OPT_64BITINTTYPES
#if defined(__GNUC__) && defined(__x86_64__)
#define OPT_64BITINTTYPES
#endif
#endif

typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;
#ifdef OPT_64BITINTTYPES
typedef unsigned int UInt32;
typedef signed int SInt32;
#else
typedef unsigned long UInt32;
typedef signed long SInt32;
#endif /* OPT_64BITINTTYPES */
typedef signed long long SInt64;
typedef unsigned long long UInt64;

#ifndef UINT8_CONST

#define UINT8_CONST(c)		static_cast<UInt8>(c##U)
#define SINT8_CONST(c)		static_cast<SInt8>(c)
#define UINT16_CONST(c)		static_cast<UInt16>(c##U)
#define SINT16_CONST(c)		static_cast<SInt16>(c)
#ifdef OPT_64BITINTTYPES
#define UINT32_CONST(c)		c##U
#define SINT32_CONST(c)		c 
#else
#define UINT32_CONST(c)		c##UL
#define SINT32_CONST(c)		c##L
#endif /* OPT_64BITINTTYPES */
#define UINT64_CONST(c)		c##ULL
#define SINT64_CONST(c)		c##LL

#endif /* UINT8_CONST */

#endif /* HESTDINT_H */
