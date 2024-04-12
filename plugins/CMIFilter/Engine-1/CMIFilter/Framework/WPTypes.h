// =============================================================================
//	WPTypes.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifndef OPT_64BITINTTYPES
#if defined(__GNUC__) && defined(__x86_64__)
#define OPT_64BITINTTYPES
#endif
#endif

typedef unsigned char		UInt8;
typedef signed char			SInt8;
typedef unsigned short		UInt16;
typedef signed short		SInt16;
#ifdef OPT_64BITINTTYPES
typedef unsigned int		UInt32;
typedef signed int			SInt32;
#else
typedef unsigned long		UInt32;
typedef signed long			SInt32;
#endif /* OPT_64BITINTTYPES */
typedef signed long long	SInt64;
typedef unsigned long long	UInt64;

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

#if defined(_WPDLL)
	#define DECLSPEC __declspec(dllexport)
#else
	#define DECLSPEC
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
	UInt32	Data1;
	UInt16	Data2;
	UInt16	Data3;
	UInt8	Data4[8];
} GUID;
#endif	// GUID_DEFINED

#ifndef IID
typedef GUID	IID;
#endif

#ifndef CLSID
typedef GUID	CLSID;
#endif

#if defined(__GNUC__)
typedef int				BOOL;
typedef unsigned int	COLORREF;
typedef int				HRESULT;
#endif

