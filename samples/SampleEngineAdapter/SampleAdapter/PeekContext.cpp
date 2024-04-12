// =============================================================================
//	PeekContext.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekContext.h"


// =============================================================================
//		CPeekContext
// =============================================================================

CPeekContext::CPeekContext(
	CPeekProxy&	Proxy )
	:	m_PeekProxy( Proxy )
{
}

CPeekContext::~CPeekContext()
{
}
