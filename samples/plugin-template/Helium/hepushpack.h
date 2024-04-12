// =============================================================================
//	hepushpack.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

/// \file hepushpack.h
/// \brief Cross-platform support for using packed structures.
/// \see hepoppack.h
///
/// Turns structure packing on. hepoppack.h is the complement to this file.
///
/// Example:
///   #include "hepushpack.h"
///   struct foo
///   {
///       char  bar;
///       int   baz;
///   } HE_PACK_STRUCT;
///   #include "hepoppack.h"

/// \def HE_PACK_STRUCT
/// \brief Adds packed struct attribute for compilers that use attributes.
/// \see hepushpack.h

#if defined(_MSC_VER)

#ifndef HE_PACK_STRUCT
#define HE_PACK_STRUCT
#endif

// Disable warning: "'filename' : used #pragma pack to change alignment".
#pragma warning(disable:4103)

// Microsoft Visual Studio uses #pragma pack.
#pragma pack(push,1)

#elif defined(__GNUC__)

#ifndef HE_PACK_STRUCT
#define HE_PACK_STRUCT	__attribute__ ((packed))
#endif

#else

#ifndef HE_PACK_STRUCT
#define HE_PACK_STRUCT	"don't know how to pack with this compiler"
#endif

#endif
