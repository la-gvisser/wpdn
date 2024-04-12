// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define STRICT					1
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT			0x0601
#endif

#ifndef WINVER
#define WINVER					_WIN32_WINNT
#endif

#ifndef _WIN32_IE
#define _WIN32_IE				0x0700
#endif

#define TARGET_OS_WIN32 1

#include <iostream>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>
#include <comutil.h>

// TODO: reference additional headers your program requires here

#pragma warning(disable:4201)	// Warning: nameless struct/union (in many windows headers).
#pragma warning(disable:4710)	// Informational: function not inlined.
#pragma warning(disable:4711)	// Informational: function selected for inlining.
#pragma warning(disable:4250)	// Warning: inherits [Something] via dominance
#pragma warning(disable:4267)	// Warning: conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable:4996)	// Warning: unsafe
#pragma warning(disable:4312)	// Warning: conversion


// constants
// const int kMaxRegisters		= 64;
// const int kMaxLocals		= 64;
// const int kMaxStackDepth	= 256;
// const int kMaxVariableStack	= 256;


#define DECODE_RESULT_OK(nResult) \
	( (nResult == DECODE_RESULT_NOERROR) || \
	(nResult == DECODE_RESULT_DECODE_FINISHED) || \
	(nResult == DECODE_RESULT_DECODE_CONTINUE) )

#include "memutil.h"
#include "AGTypes.h"
#include "MapEx.h"
#include "Dcod.h"
#include <memory>

//for _bstr_t
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "kernel32.lib")

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x)/sizeof(x[0]))
#endif

#undef CT2UTF8
#define CT2UTF8	HeLib::CW2UTF8
#undef CUTF82T
#define CUTF82T	HeLib::CUTF82W
//#define OPT_LOGGER 1