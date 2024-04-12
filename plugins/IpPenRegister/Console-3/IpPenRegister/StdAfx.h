// ============================================================================
//	StdAfx.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

// ============================================================================
//	Project Defines
// ============================================================================

#if defined(_WIN32)
	// Targets for Win32 defined here.
	#define TARGET_OS_WIN32		1
#endif

#define COUNTOF(array)			(sizeof(array)/sizeof(array[0]))


// ============================================================================
//	Project Pragmas
// ============================================================================

#if defined(_MSC_VER)
#pragma warning(disable:4201)	// Warning: nameless struct/union (in many windows headers).
#if !defined(_DEBUG)
#pragma warning(disable:4100)	// Informational: Unused parameters.
#pragma warning(disable:4710)	// Warning: function not inlined.
#pragma warning(disable:4711)	// Informational: function selected for inlining.
#endif
#endif

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501		// Windows XP
#endif
#define _ATL_APARTMENT_THREADED

#if !defined(_MSC_VER) || (_MSC_VER < 1400)
#ifndef strcpy_s
#define strcpy_s(d,sd,s)			strcpy(d,s)
#endif
#ifndef wcscpy_s
#define wcscpy_s(d,sd,s)			wcscpy(d,s)
#endif
#ifndef _tcscpy_s
#define _tcscpy_s(d,sd,s)			_tcscpy(d,s)
#endif
#ifndef _tcsncpy_s
#define _tcsncpy_s(d,sd,s,n)		_tcsncpy(d,s,n)
#endif
#ifndef _vsntprintf_s
#define _vsntprintf_s(d,sd,c,f,a)	_vsntprintf(d,c,f,a)
#endif
#endif

// ============================================================================
//  Project Headers
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
	#include <afxwin.h>
	#include <afxdisp.h>
	#include <afxcmn.h>
	#include <afxdlgs.h>

	#include <atlbase.h>
	#include <atlcom.h>
	#include <atlcoll.h>

	#include <windowsx.h>
	#include <tchar.h>
	#include <malloc.h>
	#include <wtypes.h>
#elif defined(__GNUC__)
	#include "wpgnuc.h"
#endif

typedef enum {
	kType_ICMP,
	kType_TCP,
	kType_UDP,
	kType_IP,
	kType_Max
} tProtocolType;

enum {
	kProtocol_ICMP = 1,
	kProtocol_ICMPv6 = 0x3A,
	kProtocol_TCP = 6,
	kProtocol_UDP = 17,
	kProtocol_IP = 0
};

#include "WPTypes.h"
#include "PeekPlug.h"
#include "PeekPlugin.h"
#include "PeekPSIDs.h"
#include "PluginDebug.h"
#include "MemUtil.h"
#include "resource.h"
#include "Utils.h"
