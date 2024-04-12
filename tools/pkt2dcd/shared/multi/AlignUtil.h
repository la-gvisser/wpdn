// ============================================================================
//	AlignUtil.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2003-2004. All rights reserved.

#ifndef ALIGNUTIL_H
#define ALIGNUTIL_H

#include <stddef.h>

namespace AlignUtil
{

template< typename N >
inline N AlignUp( N n, unsigned int a ) throw()
{
	return N((n + (a - 1)) & ~(N(a) - 1));
}

template< typename N >
inline N AlignDown( N n, unsigned int a ) throw()
{
	return N(n & ~(N(a) - 1));
}

template< typename N >
inline bool IsAligned( N n, unsigned int a ) throw()
{
	return (n & (a - 1)) == 0;
}

template< typename P >
inline P AlignPtrUp( P p, unsigned int a ) throw()
{
	return P(((uintptr_t)p + (a - 1)) & ~((uintptr_t)(a) - 1));
}

template< typename P >
inline P AlignPtrDown( P p, unsigned int a ) throw()
{
	return P((uintptr_t)p & ~((uintptr_t)(a) - 1));
}

template< typename P >
inline bool IsAlignedPtr( P p, unsigned int a ) throw()
{
	return ((uintptr_t)p & ((uintptr_t)(a) - 1)) == 0;
}

} /* namespace AlignUtil */

#endif
