// =============================================================================
//	StdAfx.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#pragma once

// -----------------------------------------------------------------------------
//	Project Defines
// -----------------------------------------------------------------------------

#if defined(macintosh)
	// Targets are defined in ConditionalMacros.h.
	#include <ConditionalMacros.h>
#elif defined(_WIN32)
	// Targets for Win32 defined here.
	#define TARGET_OS_MAC		0
	#define TARGET_OS_WIN32		1
#endif

#define COUNTOF(array)			(sizeof(array)/sizeof(array[0]))
#define	DEFAULT_TIMEOUT			1000

// -----------------------------------------------------------------------------
//	Project Pragmas
// -----------------------------------------------------------------------------

#if TARGET_OS_WIN32
#pragma warning(disable:4201)	// Warning: nameless struct/union (in many windows headers).
#pragma warning(disable:4996)	// This function or variable may be unsafe.
#if !defined(_DEBUG)
#pragma warning(disable:4100)	// Informational: Unused parameters.
#pragma warning(disable:4710)	// Warning: function not inlined.
#pragma warning(disable:4711)	// Informational: function selected for inlining.
#endif
#endif

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#define _ATL_APARTMENT_THREADED

// -----------------------------------------------------------------------------
//  Project Headers
// -----------------------------------------------------------------------------

#define X_OK	0


#include <afxwin.h>
#include <afxdisp.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlcoll.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxmt.h>


// Options
#define _NPROTOSPECS


#include "AGTypes.h"
#include "XTypes.h"

#include "PeekPlugin.h"
#include "PeekPSIDs.h"
#include "PluginDebug.h"
#include "MemUtil.h"
#include "resource.h"
#include "CMIFilterPlugin.h"
#include "CMIFilter.h"

extern CCMIFilterApp theApp;

// Macros
#define AFX_MANAGE 	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
