// =============================================================================
//	StdAfx.h
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#pragma once

#ifdef _WIN32
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

#define __WPTYPES_H__

#pragma warning(push)
#pragma warning(disable:4996)	// Warning: This function or variable may be unsafe. Use '_s' instead.
#endif

#define NOMINMAX
#define STRICT					1
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
//#define _WIN32_WINNT			0x0501			// Windows XP
#define _WIN32_WINNT			0x0600			// Windows Vista
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

#endif	// _WIN32

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))
#endif

#include "CaptureFile/MemUtil.h"
#include "CaptureFile/strutil.h"

#include <string>
#include <ostream>
#include <iomanip>
#include <algorithm>
#include <vector>


// =============================================================================
//		Data Structures
// =============================================================================

typedef	std::vector<std::string>	CStringAList;
typedef	std::vector<std::wstring>	CStringList;


// =============================================================================
//		Constants
// =============================================================================

#define CF_PEEK_FILE
#define CF_PCAP_FILE

enum {
	kAction_None,
	kAction_Merge,
	kAction_Append,
	kAction_Compare,
	kAction_Report,
	kAction_Test,
	kAction_Max
};
