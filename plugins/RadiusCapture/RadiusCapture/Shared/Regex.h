// ============================================================================
//	Regex.h
//		
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#pragma once

#pragma warning(push)
#pragma warning(disable:4018) // signed/unsigned mismatch
#pragma warning(disable:4389) // signed/unsigned mismatch
#include <atlrx.h>
#pragma warning(pop)

typedef	ATL::CAtlRegExp<ATL::CAtlRECharTraitsA>			CRegExpA;
typedef ATL::CAtlREMatchContext<ATL::CAtlRECharTraitsA>	CREMatchContextA;
typedef CREMatchContextA::MatchGroup					CREMatchGroupA;

