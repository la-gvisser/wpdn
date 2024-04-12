// =============================================================================
//	FilePacketArray.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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
