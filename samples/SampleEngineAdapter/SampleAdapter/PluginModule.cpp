// =============================================================================
//	PluginModule.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "RemotePlugin.h"
#include "AdapterManager.h"
 

// =============================================================================
//		Helium Class Table (.he file binding)
// =============================================================================

HE_CLASS_TABLE_BEGIN(g_ClassTable)
	HE_CLASS_TABLE_ENTRY(CSamplePlugin_CID,CRemotePlugin)
	HE_CLASS_TABLE_ENTRY(CAdapterManager_CID,CAdapterManager)	
HE_CLASS_TABLE_END()

static CHeModule	g_Module;
CHeModule*			HeLib::_pModule = &g_Module;


// -----------------------------------------------------------------------------
//		HeModuleGetClassObject
// -----------------------------------------------------------------------------

extern "C" HeResult HE_EXPORT
HeModuleGetClassObject(
	const HeCID&	cid,
	const HeIID&	iid,
	void**			ppv )
{
	return HeClassTableGetClassObject( g_ClassTable, cid, iid, ppv );
}


// -----------------------------------------------------------------------------
//		HeModuleCanUnloadNow
// -----------------------------------------------------------------------------

extern "C" HeResult HE_EXPORT
HeModuleCanUnloadNow()
{
	return g_Module.CanUnloadNow();
}
