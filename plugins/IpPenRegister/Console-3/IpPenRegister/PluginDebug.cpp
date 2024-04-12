// ============================================================================
//	PluginDebug.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PluginDebug.h"


// ----------------------------------------------------------------------------
//		_PluginAssertFailed
// ----------------------------------------------------------------------------

void
_PluginAssertFailed(
	TCHAR*	inCondition,
	TCHAR*	inFileName,
	int		inLine )
{
	CString	strMsg;
	strMsg.Format( _T( "Assertion failed: %s, line %d; %s" ), inFileName,
		inLine, inCondition );

	_PluginOutputDebugString( strMsg.GetBuffer() );
}


// ----------------------------------------------------------------------------
//		_PluginOutputDebugString
// ----------------------------------------------------------------------------

void
_PluginOutputDebugString(
	TCHAR*	inFormat, ... )
{
	va_list	list;
	va_start( list, inFormat );

	TCHAR	szBuff[kPluginNameMaxLength];
	_vsntprintf_s( szBuff, kPluginNameMaxLength, (kPluginNameMaxLength - 1),
		inFormat, list );

	::OutputDebugString( szBuff );
}
