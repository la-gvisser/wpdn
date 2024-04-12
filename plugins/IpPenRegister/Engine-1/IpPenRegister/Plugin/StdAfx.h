// =============================================================================
//	StdAfx.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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

#define NOMINMAX
#define STRICT					1
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT			0x0603			// Windows 7
#endif

#ifndef WINVER
#define WINVER					_WIN32_WINNT
#endif

#ifndef _WIN32_IE
#define _WIN32_IE				0x0700			// IE 7
#endif

#include <afxwin.h>
#include <afxdisp.h>
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
  #ifndef _DEBUG
	#define ASSERT(_p)
  #else // _DEBUG
    #define ASSERT(_p)	if( !(_p) ) DebugBreak();
  #endif // _DEBUG
#endif // ASSERT

#ifndef _ASSERTE
  #ifndef _DEBUG
	#define _ASSERTE(_p)
  #else // _DEBUG
    #define _ASSERTE(_p)	if( !(_p) ) DebugBreak();
  #endif // _DEBUG
#endif // _ASSERTE


// =============================================================================
//	Comment or uncomment these to include or remove functionality
// =============================================================================

#define	IMPLEMENT_PLUGINMESSAGE
#define	IMPLEMENT_TAB
//#define	IMPLEMENT_FORENSICS
//#define	IMPLEMENT_RESETPROCESSING
//#define	IMPLEMENT_PACKETPROCESSOREVENTS
#define	IMPLEMENT_FILTERPACKET
//#define	IMPLEMENT_PROCESSPACKET
//#define	IMPLEMENT_PROCESSTIME
//#define	IMPLEMENT_UPDATESUMMARYSTATS
//#define	IMPLEMENT_SUMMARYMODIFYENTRY
//#define	IMPLEMENT_NOTIFICATIONS
//#define	IMPLEMENT_SUMMARIZEPACKET
//#define	IMPLEMENT_CODEEXAMPLES

#define	HAS_OPTIONS_DLG

#ifdef WIN32
#define PEEKFILE_WIN	// Use the native Windows file API.
	// Otherwise Standard C file stream API is used. (fopen, fclose, ...)
	// (The 32-bit Windows STL implementation uses 32-bit stream position and offset.)
	// 64-bit Windows: consider _not_ using the Windows API. The native 64-bit
	//   Windows file API uses 32-bit lengths for Read and Write.
#endif

#define USE_MEDIA_UTILS


// =============================================================================
//		Internal definitions
// =============================================================================

#ifdef IMPLEMENT_TAB
  #undef IMPLEMENT_PLUGINMESSAGE
  #define	IMPLEMENT_PLUGINMESSAGE
#endif // IMPLEMENT_TAB

#if (0)
#ifdef IMPLEMENT_PLUGINMESSAGE
#undef IMPLEMENT_TAB
#define	IMPLEMENT_TAB
#endif // IMPLEMENT_PLUGINMESSAGE
#endif

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
  #define IMPLEMENT_ALL_UPDATESUMMARY
#endif // IMPLEMENT_SUMMARYMODIFYENTRY

#ifdef IMPLEMENT_UPDATESUMMARYSTATS
  #undef IMPLEMENT_ALL_UPDATESUMMARY
  #define IMPLEMENT_ALL_UPDATESUMMARY
#endif // IMPLEMENT_UPDATESUMMARYSTATS

#ifdef IMPLEMENT_CODEEXAMPLES
  #define SHOW_EXAMPLE
#else  // IMPLEMENT_CODEEXAMPLES
  #define SHOW_EXAMPLE /##/
#endif // IMPLEMENT_CODEEXAMPLES


// =============================================================================
//		Includes
// =============================================================================

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)	// Warning: This function or variable may be unsafe. Use '_s' instead.
#endif

#include "helib.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#if defined(_WIN32) && defined(_AFX)
#define PEEK_UI
#endif

// =============================================================================
//		Local Includes
// =============================================================================

#include "WPTypes.h"
#include "PeekUnits.h"
#include "PeekStrings.h"


// =============================================================================
//		Helium types
// =============================================================================

using Helium::IHeUnknown;
using Helium::HeID;
using HeLib::CHePtr;
using HeLib::CHeBSTR;
