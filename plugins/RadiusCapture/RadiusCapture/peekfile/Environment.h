// ============================================================================
//	Environment.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 2000. All rights reserved.

#pragma once

#include <atlstr.h>
#include "WPTypes.h"
#include "ByteStream.h"

#pragma comment(lib, "Version.lib")

struct ENV_VERSION
{
	UInt16	MajorVer;
	UInt16	MinorVer;
	UInt16	MajorRev;
	UInt16	MinorRev;
};

class CEnvironment
{
public:
	virtual void		GetOSVersion( CString& outVersion );
	virtual void		GetIEVersion( CString& outVersion );
	virtual bool		GetModuleVersion(
							ENV_VERSION& outFileVersion,
							ENV_VERSION& outProductVersion );
	virtual bool		GetFileVersion(
							const CString& inFilePath,
							ENV_VERSION& outFileVersion,
							ENV_VERSION& outProductVersion );
	virtual bool		GetFileHash( 
							const CString&	inFilePath,
							CByteStream&	outHash );
	virtual void		FormatVersion( const ENV_VERSION& inVersion, CString& outString );	
};
