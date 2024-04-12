// ============================================================================
//	PluginDebug.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#if defined(_DEBUG) && !defined(__GNUC__)
#define PLUGIN_ASSERT(condition)	\
	((condition) ? ((void) 0) : _PluginAssertFailed(_T(#condition),_T(__FILE__), __LINE__))
#define PLUGIN_DEBUGSTR(str)	_PluginOutputDebugString( str )
#else /* !_DEBUG */
#define PLUGIN_ASSERT(condition)
#define PLUGIN_DEBUGSTR(str)
#endif


void _PluginAssertFailed( TCHAR* inCondition, TCHAR* inFileName, int inLine );
void _PluginOutputDebugString( TCHAR* inFormat, ... );
