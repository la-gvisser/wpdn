// =============================================================================
//	wppoppack.h
// =============================================================================
//	Copyright (c) 2010 WildPackets, Inc. All rights reserved.

/// \file wppoppack.h
/// \brief Cross-platform support for using packed structures.
/// \see wppushpack.h
///
/// Example:
///   #include "wppushpack.h"
///   struct foo
///   {
///       char  bar;
///       int   baz;
///   } WP_PACK_STRUCT;
///   #include "wppoppack.h"

#if defined(_MSC_VER)

// Disable warning: "'filename' : used #pragma pack to change alignment".
#pragma warning(disable:4103)

// Microsoft Visual Studio uses #pragma pack.
#if !(defined(MIDL_PASS)) || defined(__midl)
#pragma pack(pop)
#else
#pragma pack()
#endif

#elif defined(__GNUC__)

// GCC uses packed attribute.

#else

// Error generated in wppushpack.h.

#endif
