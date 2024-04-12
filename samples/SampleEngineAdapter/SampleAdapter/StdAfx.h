// =============================================================================
//	StdAfx.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#pragma warning(disable: 4996)
#pragma warning(disable: 4127)

#define HE_NO_STRING_CONVERSION_MACROS

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

#define NOMINMAX
#define STRICT					1
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT			0x0600
#endif

#ifndef WINVER
#define WINVER					_WIN32_WINNT
#endif

#ifndef _WIN32_IE
#define _WIN32_IE				0x0700
#endif

#ifndef _AFX
#include <windows.h>
#endif

#include <oleauto.h>
#include <crtdbg.h>
#include <tchar.h>
#include <string>

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

#define ASSERT(x)
#define TRACE(x) 
//#define VERIFY(x)

#define kMaxStringLength 1024

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "WPTypes.h"
#include "MemUtil.h"
#include <helib.h>
#include <peekcore.h>
#include <winsock2.h>


#ifndef TRACE_FUNCTION
#define TRACE_FUNCTION()		TRACE( "%s\n", __FUNCTION__ )
#endif

using namespace Helium;
using namespace HeLib;

//#pragma message( "todo: generate new 3 GUIDs and update the .he file.")
// In the .he file:
// - Replace the temporary GUIDs with the new ones.
// - Replace the 3 instances of "SampleAdapter" with the name of the new adapter.
// - - Do not add a .dll extension to file name attribute.

// {00000000-0000-4000-A000-000000000003}
#define CAdapterManager_CID \
	{ 0x00000000, 0x0000, 0x4000, { 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 } }

// {00000000-0000-4000-A000-000000000001}
#define CSamplePlugin_CID \
	{ 0x00000000, 0x0000, 0x4000, { 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 } }

// {00000000-0000-4000-A000-000000000002}
#define CSampleAdapter_CID \
	{ 0x00000000, 0x0000, 0x4000, { 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 } }
