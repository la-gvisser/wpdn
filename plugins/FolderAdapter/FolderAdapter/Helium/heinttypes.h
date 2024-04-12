// =============================================================================
//	heinttypes.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#ifndef HEINTTYPES_H
#define HEINTTYPES_H

#include "hestdint.h"

// Equivalents for ISO C99 formatting macros. The real macros should come from
// inttypes.h when __STDC_FORMAT_MACROS is defined.

#if defined(_MSC_VER)

#define HE_PRIi8	"i"
#define HE_PRId8	"d"
#define HE_PRIu8	"u"
#define HE_PRIx8	"x"
#define HE_PRIX8	"X"

#define HE_SCNi8	"i"
#define HE_SCNd8	"d"
#define HE_SCNu8	"u"
#define HE_SCNx8	"x"

#define HE_PRIi16	"i"
#define HE_PRId16	"d"
#define HE_PRIu16	"u"
#define HE_PRIx16	"x"
#define HE_PRIX16	"X"

#define HE_SCNi16	"hi"
#define HE_SCNd16	"hd"
#define HE_SCNu16	"hu"
#define HE_SCNx16	"hx"

#define HE_PRIi32	"li"
#define HE_PRId32	"ld"
#define HE_PRIu32	"lu"
#define HE_PRIx32	"lx"
#define HE_PRIX32	"lX"

#define HE_SCNi32	"li"
#define HE_SCNd32	"ld"
#define HE_SCNu32	"lu"
#define HE_SCNx32	"lx"

#define HE_PRIi64	"lli"
#define HE_PRId64	"lld"
#define HE_PRIu64	"llu"
#define HE_PRIx64	"llx"
#define HE_PRIX64	"llX"

#define HE_SCNi64	"lli"
#define HE_SCNd64	"lld"
#define HE_SCNu64	"llu"
#define HE_SCNx64	"llx"

// Wide character versions:

#define HE_PRIi8W	L"i"
#define HE_PRId8W	L"d"
#define HE_PRIu8W	L"u"
#define HE_PRIx8W	L"x"
#define HE_PRIX8W	L"X"

#define HE_SCNi8W	L"i"
#define HE_SCNd8W	L"d"
#define HE_SCNu8W	L"u"
#define HE_SCNx8W	L"x"

#define HE_PRIi16W	L"i"
#define HE_PRId16W	L"d"
#define HE_PRIu16W	L"u"
#define HE_PRIx16W	L"x"
#define HE_PRIX16W	L"X"

#define HE_SCNi16W	L"hi"
#define HE_SCNd16W	L"hd"
#define HE_SCNu16W	L"hu"
#define HE_SCNx16W	L"hx"

#define HE_PRIi32W	L"li"
#define HE_PRId32W	L"ld"
#define HE_PRIu32W	L"lu"
#define HE_PRIx32W	L"lx"
#define HE_PRIX32W	L"lX"

#define HE_SCNi32W	L"li"
#define HE_SCNd32W	L"ld"
#define HE_SCNu32W	L"lu"
#define HE_SCNx32W	L"lx"

#define HE_PRIi64W	L"lli"
#define HE_PRId64W	L"lld"
#define HE_PRIu64W	L"llu"
#define HE_PRIx64W	L"llx"
#define HE_PRIX64W	L"llX"

#define HE_SCNi64W	L"lli"
#define HE_SCNd64W	L"lld"
#define HE_SCNu64W	L"llu"
#define HE_SCNx64W	L"llx"

#elif defined(__GNUC__)

#define HE_PRIi8	"i"
#define HE_PRId8	"d"
#define HE_PRIu8	"u"
#define HE_PRIx8	"x"
#define HE_PRIX8	"X"

#define HE_SCNi8	"hhi"
#define HE_SCNd8	"hhd"
#define HE_SCNu8	"hhu"
#define HE_SCNx8	"hhx"

#define HE_PRIi16	"i"
#define HE_PRId16	"d"
#define HE_PRIu16	"u"
#define HE_PRIx16	"x"
#define HE_PRIX16	"X"

#define HE_SCNi16	"hi"
#define HE_SCNd16	"hd"
#define HE_SCNu16	"hu"
#define HE_SCNx16	"hx"

#ifdef OPT_64BITINTTYPES

#define HE_PRIi32	"i"
#define HE_PRId32	"d"
#define HE_PRIu32	"u"
#define HE_PRIx32	"x"
#define HE_PRIX32	"X"

#define HE_SCNi32	"i"
#define HE_SCNd32	"d"
#define HE_SCNu32	"u"
#define HE_SCNx32	"x"

#else

#define HE_PRIi32	"li"
#define HE_PRId32	"ld"
#define HE_PRIu32	"lu"
#define HE_PRIx32	"lx"
#define HE_PRIX32	"lX"

#define HE_SCNi32	"li"
#define HE_SCNd32	"ld"
#define HE_SCNu32	"lu"
#define HE_SCNx32	"lx"

#endif /* OPT_64BITINTTYPES */

#define HE_PRIi64	"lli"
#define HE_PRId64	"lld"
#define HE_PRIu64	"llu"
#define HE_PRIx64	"llx"
#define HE_PRIX64	"llX"

#define HE_SCNi64	"lli"
#define HE_SCNd64	"lld"
#define HE_SCNu64	"llu"
#define HE_SCNx64	"llx"

// Wide character versions:

#define HE_PRIi8W	L"i"
#define HE_PRId8W	L"d"
#define HE_PRIu8W	L"u"
#define HE_PRIx8W	L"x"
#define HE_PRIX8W	L"X"

#define HE_SCNi8W	L"hhi"
#define HE_SCNd8W	L"hhd"
#define HE_SCNu8W	L"hhu"
#define HE_SCNx8W	L"hhx"

#define HE_PRIi16W	L"i"
#define HE_PRId16W	L"d"
#define HE_PRIu16W	L"u"
#define HE_PRIx16W	L"x"
#define HE_PRIX16W	L"X"

#define HE_SCNi16W	L"hi"
#define HE_SCNd16W	L"hd"
#define HE_SCNu16W	L"hu"
#define HE_SCNx16W	L"hx"

#ifdef OPT_64BITINTTYPES

#define HE_PRIi32W	L"i"
#define HE_PRId32W	L"d"
#define HE_PRIu32W	L"u"
#define HE_PRIx32W	L"x"
#define HE_PRIX32W	L"X"

#define HE_SCNi32W	L"i"
#define HE_SCNd32W	L"d"
#define HE_SCNu32W	L"u"
#define HE_SCNx32W	L"x"

#else

#define HE_PRIi32W	L"li"
#define HE_PRId32W	L"ld"
#define HE_PRIu32W	L"lu"
#define HE_PRIx32W	L"lx"
#define HE_PRIX32W	L"lX"

#define HE_SCNi32W	L"li"
#define HE_SCNd32W	L"ld"
#define HE_SCNu32W	L"lu"
#define HE_SCNx32W	L"lx"

#endif /* OPT_64BITINTTYPES */

#define HE_PRIi64W	L"lli"
#define HE_PRId64W	L"lld"
#define HE_PRIu64W	L"llu"
#define HE_PRIx64W	L"llx"
#define HE_PRIX64W	L"llX"

#define HE_SCNi64W	L"lli"
#define HE_SCNd64W	L"lld"
#define HE_SCNu64W	L"llu"
#define HE_SCNx64W	L"llx"

#else

#error Need std format macros for your compiler

#endif

#endif /* HEINTTYPES_H */
