// =============================================================================
//	PluginDebug.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#include "StdAfx.h"
#include "PluginDebug.h"


// -----------------------------------------------------------------------------
//  _PluginDebugMessage
// -----------------------------------------------------------------------------

void
_PluginAssertFailed(
	TCHAR*	inCondition,
	TCHAR*	inFileName,
	int		inLine )
{
	TCHAR	szMsg[256];
	_stprintf( szMsg, _T( "Assertion failed: %s, line %d; %s" ), inFileName, inLine, inCondition );

	_PluginOutputDebugString( szMsg );
}


// -----------------------------------------------------------------------------
//  _PluginOutputDebugString
// -----------------------------------------------------------------------------

void
_PluginOutputDebugString(
	TCHAR*	inFormat, ... )
{
	va_list	list;
	va_start(list, inFormat);

	TCHAR	szBuff[256];
	_vsntprintf(szBuff, 255, inFormat, list);

#if TARGET_OS_WIN32
	::OutputDebugString( szBuff );
#elif TARGET_OS_MAC
	Str255	tmpStr;
	int		nStrLen = strlen( szBuff );
	if( nStrLen > 255 ) nStrLen = 255;

	memcpy( &tmpStr[1], szBuff, nStrLen );
	tmpStr[0] = nStrLen;
	::DebugStr( tmpStr );
#endif
}
