// =============================================================================
//	StdAfx.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

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

#endif	// _MSC_VER && _DEBUG

#define NOMINMAX
#define STRICT					1
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT			0x0601			// Windows 7
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

typedef HANDLE		HDEVICE;
typedef HANDLE		HEVENT;
typedef HANDLE		HTHREAD;

#endif // _WIN32

#ifdef TARGET_OS_LINUX
#include <errno.h>

#define WP_LINUX
#define HE_NIX
#define HE_LINUX
#define _LINUX
#define HE_ASSERT_ENABLE
#define SUCCEEDED(_p)			((_p) == 0)
#define FAILED(_p)				((_p) != 0)
#define S_OK					(0)
#define E_FAIL					(0x80004005)
#define E_OUTOFMEMORY			ENOMEM

#ifndef MAX_PATH
#define MAX_PATH				260
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE	(-1)
#endif

#define sprintf_s(s,l,f,...)	sprintf((s),/*l,*/(f),...)

typedef void*			HANDLE;
typedef int				HDEVICE;
typedef void*			HEVENT;
typedef void*			HTHREAD;
typedef void*			HMODULE;
typedef unsigned char	byte;
typedef unsigned char	UCHAR;
#endif // TARGET_OS_LINUX

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))
#endif // COUNTOF

#ifndef ASSERT
  #ifndef _DEBUG
	#define ASSERT(_p)
  #else // _DEBUG
	#ifdef _WIN32
		#define ASSERT(_p)	if( !(_p) ) DebugBreak();
	#else
		#define ASSERT(_p)	HE_ASSERT(_p)
	#endif
  #endif // _DEBUG
#endif // ASSERT

#ifndef _ASSERTE
  #ifndef _DEBUG
	#define _ASSERTE(_p)
  #else // _DEBUG
	#ifdef _WIN32
	    #define _ASSERTE(_p)	if( !(_p) ) DebugBreak();
	#else
		#define _ASSERTE(_p)	HE_ASSERT(_p)
	#endif
  #endif // _DEBUG
#endif // _ASSERTE

#ifndef make_unique
#include <memory>
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif // make_unique


// =============================================================================
//	Comment or uncomment these to include or remove functionality
// =============================================================================

// #define	IMPLEMENT_UIHANDLER
#define	IMPLEMENT_PLUGINMESSAGE
#define	IMPLEMENT_SUMMARY
#define	IMPLEMENT_PROCESSTIME
// #define IMPLEMENT_NAVL

#ifdef _WIN32
#define PEEKFILE_WIN	// Use the native Windows file API.
	// Otherwise Standard C file stream API is used. (fopen, fclose, ...)
	// (The 32-bit Windows STL implementation uses 32-bit stream position and offset.)
	// 64-bit Windows: consider _not_ using the Windows API. The native 64-bit
	//   Windows file API uses 32-bit lengths for Read and Write.
#endif // _WIN32

#define USE_MEDIA_UTILS
#define IS_ADAPTER


// =============================================================================
//		Internal definitions
// =============================================================================

#ifdef IMPLEMENT_UIHANDLER
  #undef IMPLEMENT_TAB
  #undef IMPLEMENT_SUMMARIZEPACKET
#endif

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


// =============================================================================
//		Includes
// =============================================================================

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)	// Warning: This function or variable may be unsafe. Use '_s' instead.
#endif // _MSC_VER

#include "helib.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#if !defined(IMPLEMENT_UIHANDLER) && defined(_WIN32) && defined(_AFX)
// #define PEEK_UI
#endif // !IMPLEMENT_UIHANDLER && _WIN32 && _AFX


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
using HeLib::CHeQIPtr;
using HeLib::CHeBSTR;
