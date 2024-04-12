// =============================================================================
//	wppushpack.h
// =============================================================================
//	Copyright (c) 2006-2017 Savvius, Inc. All rights reserved.

/// \file wppushpack.h
/// \brief Cross-platform support for using packed structures.
/// \see wppoppack.h
///
/// Example:
///   #include "wppushpack.h"
///   struct foo
///   {
///       char  bar;
///       int   baz;
///   } WP_PACK_STRUCT;
///   #include "wppoppack.h"

/// \def WP_PACK_STRUCT
/// \brief Adds packed struct attribute for compilers that use attributes.
/// \see wppushpack.h

#if defined(_MSC_VER)

#ifndef WP_PACK_STRUCT
#define WP_PACK_STRUCT
#endif

// Disable warning: "'filename' : used #pragma pack to change alignment".
#pragma warning(disable:4103)

// Microsoft Visual Studio uses #pragma pack.
#if !(defined(MIDL_PASS)) || defined(__midl)
#pragma pack(push,1)
#else
#pragma pack(1)
#endif

#elif defined(__GNUC__)

#ifndef WP_PACK_STRUCT
#define WP_PACK_STRUCT	__attribute__ ((packed))
#endif

#else

#ifndef WP_PACK_STRUCT
#define WP_PACK_STRUCT	"don't know how to pack with this compiler"
#endif

#endif
