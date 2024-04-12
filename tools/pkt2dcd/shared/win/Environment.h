// =============================================================================
//	Environment.h
// =============================================================================
//	Copyright (c) 2000-2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "AGTypes.h"
#include "ByteStream.h"
#include <atldef.h>
#include <atlstr.h>

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
#if (_ATL_VER >= 0x0700) && (_ATL_VER <= 0x0800)
	virtual bool		GetFileHash( 
							const CString&	inFilePath,
							CByteStream&	outHash );
#endif
	virtual void		FormatVersion( const ENV_VERSION& inVersion, CString& outString );	
};
