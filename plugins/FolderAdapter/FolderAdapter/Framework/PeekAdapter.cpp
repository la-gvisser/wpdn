// =============================================================================
//	PeekAdapter.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekAdapter.h"


// =============================================================================
//		CPeekAdapter
// =============================================================================

void
CPeekAdapter::DoLogMessage(
	UInt64				inTimeStamp,
	int					inSeverity,
	const CPeekString&	inShortMessage,
	const CPeekString&	inLongMessage )
{
	CLogService&	logger = m_EngineProxy.GetLogService();
	CGlobalId		idNull( GUID_NULL );
	logger.DoLogMessage( idNull, idNull, 0,
		inTimeStamp, inSeverity, inShortMessage, inLongMessage );
}
