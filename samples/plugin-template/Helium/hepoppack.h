// =============================================================================
//	hepoppack.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

/// \file hepoppack.h
/// \brief Cross-platform support for using packed structures.
/// \see hepushpack.h
///
/// hepoppack.h is the complement to hepushpack.h. An inclusion of hepoppack.h
/// <b>must always<b> be preceeded by an inclusion of hepushpack.h.
///
/// Example:
///   #include "hepushpack.h"
///   struct foo
///   {
///       char  bar;
///       int   baz;
///   } HE_PACK_STRUCT;
///   #include "hepoppack.h"

#if defined(_MSC_VER)

// Disable warning: "'filename' : used #pragma pack to change alignment".
#pragma warning(disable:4103)

// Microsoft Visual Studio uses #pragma pack.
#pragma pack(pop)

#elif defined(__GNUC__)

// GCC uses packed attribute.

#else

// Error generated in hepushpack.h.

#endif
