// =============================================================================
//	hlalloc.h
// =============================================================================
//	Copyright (c) 2007-2015 Savvius, Inc. All rights reserved.

#ifndef HLALLOC_H
#define HLALLOC_H

#include "hlruntime.h"

namespace HeLib
{

class HeTaskMemAllocator
{
public:
	static void* Reallocate( void* p, std::size_t cb ) throw()
	{
		return HeTaskMemRealloc( p, cb );
	}

	static void* Allocate( std::size_t cb ) throw()
	{
		return HeTaskMemAlloc( cb );
	}

	static void Free( void* p ) throw()
	{
		HeTaskMemFree( p );
	}
};

} /* namespace HeLib */

#endif /* HLALLOC_H */
