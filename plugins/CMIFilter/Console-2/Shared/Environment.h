// ============================================================================
//	Environment.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include <atlstr.h>
#include "WPTypes.h"

struct ENV_VERSION {
	UInt16	MajorVer;
	UInt16	MinorVer;
	UInt16	MajorRev;
	UInt16	MinorRev;
};

class CEnvironment
{
public:
	static void			GetOSVersion( CString& outVersion );
	static bool			GetModuleVersion(
							ENV_VERSION& outFileVersion,
							ENV_VERSION& outProductVersion );
	static bool			GetFileVersion(
							const CString& inFilePath,
							ENV_VERSION& outFileVersion,
							ENV_VERSION& outProductVersion );
	static void			FormatVersion( const ENV_VERSION& inVersion, CString& outString );
};
