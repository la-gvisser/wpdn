// =============================================================================
//	PluginModule.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#include "RemotePlugin.h"
#include "PluginManager.h"
#include "ContextManager.h"

extern GUID		g_PluginId;
Helium::HeCID	g_ClassId( *(reinterpret_cast<Helium::HeID*>( &g_PluginId )) );

CSafePluginManager	g_SafePluginManager;
CPluginManagerPtr	GetPluginManager() { return g_SafePluginManager.Get(); }

CSafeContextManager	g_SafeContextManager;
CContextManagerPtr	GetContextManager() { return g_SafeContextManager.Get(); }


// =============================================================================
//		Class Table
// =============================================================================

HE_CLASS_TABLE_BEGIN(g_ClassTable)
	HE_CLASS_TABLE_ENTRY(g_ClassId,CRemotePlugin)
HE_CLASS_TABLE_END()


// =============================================================================
//		Global Objects
// =============================================================================

static HeLib::CHeModule	g_Module;
HeLib::CHeModule*		HeLib::_pModule = &g_Module;


// -----------------------------------------------------------------------------
//		HeModuleGetClassObject
// -----------------------------------------------------------------------------

extern "C" HeResult HE_EXPORT
HeModuleGetClassObject(
	const Helium::HeCID&	cid,
	const Helium::HeIID&	iid,
	void**			ppv )
{
	return HeLib::HeClassTableGetClassObject( g_ClassTable, cid, iid, ppv );
}


// -----------------------------------------------------------------------------
//		HeModuleCanUnloadNow
// -----------------------------------------------------------------------------

extern "C" HeResult HE_EXPORT
HeModuleCanUnloadNow( void )
{
	return g_Module.CanUnloadNow();
}
