// =============================================================================
//	PluginManager.h
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekContext.h"
#include "RemotePlugin.h"
#include "hldef.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Plugin Manager, Plugin Manager Ptr, Safe Plugin Manager
//
//	The Plugin Manger is a Framework object that manages instances of Remote
//  Plugins. The Plugin Manager Ptr unlocks the Plugin Manager when the Ptr is
//	destroyed. The Safe Plugin Manager provides access control to the Plugin
//	Manager.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPluginManager
// =============================================================================

class
CPluginManager
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
protected:
	CRemotePluginList	m_ayInstances;

public:
	;		CPluginManager() {}
	;		~CPluginManager() {}

	void	Add( CRemotePlugin* inInstance ) { m_ayInstances.Add( inInstance ); }

	bool	Find( const CGlobalId& inId, CRemotePlugin*& outPlugin ) {
		return m_ayInstances.Find( inId, outPlugin );
	}
	bool	Find( CRemotePlugin* inInstance, size_t& outIndex ) {
		return m_ayInstances.Find( inInstance, outIndex );
	}

	size_t	GetCount() const { return m_ayInstances.GetCount(); }

	void	Remove( CRemotePlugin* inInstance ) {
		size_t	nIndex( kIndex_Invalid );
		if ( Find( inInstance, nIndex ) ) m_ayInstances.RemoveAt( nIndex );
	}

	void	UnloadAll() {
		m_ayInstances.UnloadAll();
		m_ayInstances.RemoveAll();
	}
};


// =============================================================================
//		CPluginManagerPtr
// =============================================================================

class CPluginManagerPtr
{
protected:
	CPluginManager*		m_pManager;

public:
	;	CPluginManagerPtr( CPluginManager* pManager )
		:	m_pManager( pManager )
	{
		_ASSERTE( m_pManager != nullptr );
		m_pManager->Lock();
	}
	;	~CPluginManagerPtr() {
		m_pManager->Unlock();
	}

	;					operator CPluginManager*() const {
		return( m_pManager );
	}
	CPluginManager*		operator->() const {
		_ASSERTE( m_pManager != nullptr );
		return( m_pManager );
	}

	// Attach to an existing pointer (takes ownership)
	void				Attach( CPluginManager* p ) {
		_ASSERTE( m_pManager == nullptr );
		m_pManager = p;
	}

	// Detach the pointer (releases ownership)
	CPluginManager*		Detach() {
		CPluginManager* pManager;
		pManager = m_pManager;
		m_pManager = nullptr;
		return( pManager );
	}
};


// =============================================================================
//		CSafePluginManager
// =============================================================================

class
CSafePluginManager
{
protected:
	CPluginManager	m_PluginManager;

public:
	;					CSafePluginManager() {}
	CPluginManagerPtr	Get() {
		return CPluginManagerPtr( &m_PluginManager );
	}
};
