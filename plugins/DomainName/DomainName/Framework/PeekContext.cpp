// =============================================================================
//	PeekEngineContext.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekContext.h"
#include "PeekPlugin.h"


// ============================================================================
//		CPeekContext
// ============================================================================

#ifdef IMPLEMENT_PLUGINMESSAGE
// ----------------------------------------------------------------------------
//		IsInstalledOnEngine
// ----------------------------------------------------------------------------

bool
CPeekContext::IsInstalledOnEngine() const
{
	return ((m_pPlugin) ? m_pPlugin->IsInstalledOnEngine() : false);
}
#endif // IMPLEMENT_PLUGINMESSAGE


// ============================================================================
//		CPeekContextPtr
// ============================================================================

bool
operator== (const CPeekContextPtr& lhs, const CPeekContext* rhs ) {
	return (lhs.get() == rhs);
}
