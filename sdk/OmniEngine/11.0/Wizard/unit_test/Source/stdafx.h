// =============================================================================
//	StdAfx.h
// =============================================================================
//	Copyright (c) 2010 WildPackets, Inc. All rights reserved.

#pragma once

#if defined(_MSC_VER) && defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC
#include <malloc.h>
#include <stdlib.h>
#include <crtdbg.h>

// These are workarounds for leak detecting versions of memory allocation
// routines used by <cstdlib>.
namespace std {
	using ::_calloc_dbg;
	using ::_free_dbg;
	using ::_malloc_dbg;
	using ::_realloc_dbg;
}

#endif	// end defined(_MSC_VER) && defined(_DEBUG)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)	// Warning: This function or variable may be unsafe. Use '_s' instead.
#endif

#define NOMINMAX
#define STRICT					1
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT			0x0501			// Windows XP
#endif

#ifndef WINVER
#define WINVER					_WIN32_WINNT
#endif

#ifndef _WIN32_IE
#define _WIN32_IE				0x0700			// IE 7
#endif

#ifndef _AFX
#include <windows.h>
#endif

#include <oleauto.h>
#include <atlstr.h>
#include <atlcoll.h>

#if _MSC_VER < 1500
#ifndef vsnprintf
#define vsnprintf	_vsnprintf
#endif
#endif
#ifndef snprintf
#define snprintf	_snprintf
#endif
#ifndef strcasecmp
#define strcasecmp	_strcmpi
#endif
#ifndef strtoull
#define strtoull	_strtoui64
#endif

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))
#endif

#ifndef ASSERT
  #ifdef _DEBUG
    #define ASSERT(_p)	if( !(_p) ) DebugBreak();
  #else // _DEBUG
	#define ASSERT(_p)
  #endif // _DEBUG
#endif // ASSERT


#include "WPTypes.h"
#include "helib.h"


// =============================================================================
//		Constants
// =============================================================================

#if (0)
const	UInt64	kBytesInKilobyte = 1024ull;
const	UInt64	kBytesInMegabyte = 1024ull * 1024ull;
const	UInt64	kBytesInGigabyte = 1024ull * 1024ull * 1024ull;
const	UInt64	kBytesInTerabyte = 1024ull * 1024ull * 1024ull * 1024ull;
const	UInt64	kBytesInPetabyte = 1024ull * 1024ull * 1024ull * 1024ull * 1024ull;

const	UInt16  kMaxUInt16 = 0xFFFF;
const	UInt32  kMaxUInt32 = 0xFFFFFFFF;
const	UInt64	kMaxUInt64 = 0xFFFFFFFFFFFFFFFFull;

const	size_t	kIndex_Invalid = static_cast<size_t>( -1 );
#endif

using Helium::IHeUnknown;
using Helium::HeID;
using HeLib::CHePtr;
using HeLib::CHeBSTR;
