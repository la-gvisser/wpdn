// =============================================================================
//	XTools.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2001-2004. All rights reserved.

#ifndef XTOOLS_H
#define XTOOLS_H

#include "XTypes.h"

typedef int (*compareFunc) (const void*, const void*);


class DECLSPEC XTools
{
 public:
	static void Sort(void* list, int size, compareFunc pFunc = NULL);
};

#endif

