// =============================================================================
//	hecore.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef HECORE_H
#define HECORE_H

#include "heresult.h"

namespace Helium
{

#ifndef HE_NIX
// NOTE: define in config.h
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__) || defined(TARGET_OS_WIN32)
#ifndef HE_WIN32
#define HE_WIN32
#endif /* HE_WIN32 */
#elif defined(TARGET_OS_LINUX)
#ifndef HE_LINUX
#define HE_LINUX
#define HE_NIX
#endif /* HE_LINUX */
#elif defined(MACOS) || defined(TARGET_OS_MAC)
#ifndef HE_MACOSX
#define HE_MACOSX
#define HE_NIX
#endif /* HE_MACOSX */
#elif defined(__unix)
#ifndef HE_UNIX
#define HE_UNIX
#define HE_NIX
#endif /* HE_UNIX */
#endif
#endif

#if defined(_MSC_VER)

#define HE_STDCALL					__stdcall
#define HE_FASTCALL					__fastcall
#define HE_NOTHROW					__declspec(nothrow)
#define HE_NORETURN					__declspec(noreturn)
#define HE_NOINLINE					__declspec(noinline)
#define HE_FORCEINLINE				__forceinline
#define HE_SELECTANY				__declspec(selectany)
#define HE_HIDDEN					
#define HE_IMPORT					__declspec(dllimport)
#define HE_EXPORT					__declspec(dllexport)
#define HE_IMETHOD_(type)			virtual HE_HIDDEN HE_NOTHROW type HE_STDCALL
#define HE_IMETHODIMP_(type)		type HE_STDCALL
#define HE_METHOD_(type)			type HE_STDCALL

#if (_MSC_VER >= 1100)
#define HE_NO_VTABLE				__declspec(novtable)
#endif

#if (_MSC_VER >= 1600)
#define HE_HAS_RVALUE_REFS
#endif

#define HE_LIKELY(x)				(x)
#define HE_UNLIKELY(x)				(x)

#elif defined(__GNUC__)

#define HE_STDCALL					
#if defined(__i386__)
#define HE_FASTCALL					__attribute__ ((regparm (3), stdcall))
#else
#define HE_FASTCALL					
#endif
#define HE_NOTHROW					__attribute__ ((nothrow))
#define HE_NORETURN					__attribute__ ((noreturn))
#define HE_NOINLINE					__attribute__ ((noinline))
#define HE_FORCEINLINE				__attribute__ ((always_inline))
#define HE_SELECTANY				__attribute__ ((weak))
#define HE_HIDDEN					__attribute__ ((visibility("hidden")))
#define HE_IMPORT					
#define HE_EXPORT					__attribute__ ((visibility("default")))
#define HE_IMETHOD_(type)			virtual HE_HIDDEN HE_NOTHROW type
#define HE_IMETHODIMP_(type)		type
#define HE_METHOD_(type)			type

// Use #pragma interface with GCC.
#define HE_NO_VTABLE				

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
// See: http://gcc.gnu.org/projects/cxx0x.html
#define HE_HAS_RVALUE_REFS
#endif

#define HE_LIKELY(x)				(__builtin_expect(!!(x), 1))
#define HE_UNLIKELY(x)				(__builtin_expect(!!(x), 0))

#endif

#define HE_IMETHOD					HE_IMETHOD_(HeResult)
#define HE_IMETHODIMP				HE_IMETHODIMP_(HeResult)
#define HE_METHOD					HE_METHOD_(HeResult)

#ifdef HE_DLL
#define HE_API_DECL					HE_EXPORT
#else
#define HE_API_DECL					HE_IMPORT
#endif

} /* namespace Helium */

#endif /* HECORE_H */
