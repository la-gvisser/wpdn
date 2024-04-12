// =============================================================================
//	FilePacketArray.h
// =============================================================================
//	Copyright (c) 2011-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "PacketArray.h"
#include "RefreshFile.h"


// =============================================================================
//		CFilePacketArray
// =============================================================================

class CFilePacketArray
	:	public CPacketArray
	,	public CRefreshFile
{
public:
	;		 CFilePacketArray() {}
	virtual	~CFilePacketArray() {}
};
