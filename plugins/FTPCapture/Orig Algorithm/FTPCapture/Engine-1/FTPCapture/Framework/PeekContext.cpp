// =============================================================================
//	PeekEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekContext.h"


// ============================================================================
//		CPeekContextPtr
// ============================================================================

bool
operator== (const CPeekContextPtr& lhs, const CPeekContext* rhs ) {
	return (lhs.get() == rhs);
}
