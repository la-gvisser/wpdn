// =============================================================================
//	RemotePlugin.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "peekcore.h"
#include "peeknames.h"
#include "hecom.h"
#include "iomniengineplugin.h"
#include "objectwithsiteimpl.h"
#include "OmniPlugin.h"
#include "Peek.h"
#include "PeekArray.h"
#include "PeekProxy.h"
#include "PeekContext.h"
#include "PeekEngineContext.h"
#include <memory>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Remote Plugin
//
//	A Remote Plugin is the root object of a plugin Framework. OmniPeek and
//	OmniEngine create instances of and directly communicate with Remote Plugin.
//	A Remote Plugin instantiates the Omni Plugin, Omni Console Context and Omni
//	Engine Context objects. It manages the life-cycle of these objects.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#if (0)
// When defined, creates a log file in /tmp or C:\Temp and logs
// all (most) entry points.
#define DEBUG_LOGINTERFACE
#endif

// =============================================================================
//		CRemotePlugin
// =============================================================================

class HE_NO_VTABLE CRemotePlugin
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
	,	public HeLib::CHeClass<CRemotePlugin>
	,	public IPeekPlugin
	,	public IPeekPluginLoad
	,	public IProcessPluginMessage
	,	public IPluginAdapter
	,	public IPluginUISource
	,	public Helium::IHePersistXml
	,	public IHeObjectWithSiteImpl<CRemotePlugin>
{
	friend class CPeekPlugin;

	enum {
		kState_Uninitialized = 0x01030507,
		kState_Initialized = 0x02040608
	};

	typedef std::unique_ptr<CPeekDataModeler>	CPeekDataModelerPtr;
	typedef int (CPeekEngineContext::*FnReset)();
	typedef int (CPeekEngineContext::*FnSummary)( CSnapshot& );

protected:
	COmniPlugin*		m_pPlugin;
	SInt32				m_nPluginAdded;
	int					m_nState;

#ifdef _WIN32
	HINSTANCE			m_hInstance;
#endif

	CGlobalId			m_idContext;
	CPeekContextPtr		m_spContext;
	CPeekDataModelerPtr	m_spdmPrefs;
	bool				m_bGraphsContext;

	CPeekArray<std::string>		m_ayMsgLog;

#ifdef DEBUG_LOGINTERFACE
	void			AddLogMsg( const std::string& inMessage );
	void			AddLogMsg( const std::ostringstream& inStream ) {
		AddLogMsg( inStream.str() );
	}
	void			SaveMsgLog();
#endif

	HeResult		CreateContext();
	HeResult		ReleaseContext();

	void			DoCreateContext();
	void			DoDisposeContext();
	bool			DoLoadPlugin();
	void			DoUnloadPlugin();

	CPeekContext*	GetContext() const { return m_spContext.get(); }

	bool			HasContext() const { return m_spContext.IsValid(); }
	bool			HasContextId() const { return m_idContext.IsValid(); }
	bool			HasInitializedPlugin() const {
		return (m_pPlugin && m_pPlugin->IsInitialized());
	}
	bool			HasNoInitializedPlugin() const {
		return (!m_pPlugin || !m_pPlugin->IsInitialized());
	}
	bool			HasNoPlugin() const { return (m_pPlugin == nullptr); }
	bool			HasPlugin() const { return (m_pPlugin != nullptr); }
	bool			HasSite() const { return (m_spUnkSite.get() != nullptr); }

	void			InvalidateContextId() {
		m_idContext.Invalidate();
		if ( HasContext() ) {
			m_spContext->SetId( m_idContext );
		}
	}
	bool			IsValidContext() const { return (HasSite() && HasContext()); }

	bool			LoadContext();

	void			SetContextId( const HeID& inId ) { SetContextId( CGlobalId( inId ) ); }
	void			SetContextId( const CGlobalId& inId ) {
		m_idContext = inId;
		if ( HasContext() ) {
			m_spContext->SetId( m_idContext );
		}
	}

public:
	HE_INTERFACE_MAP_BEGIN(CRemotePlugin)
		HE_INTERFACE_ENTRY_IID(IPeekPlugin_IID,IPeekPlugin)
		HE_INTERFACE_ENTRY_IID(IPeekPluginLoad_IID,IPeekPluginLoad)
		HE_INTERFACE_ENTRY_IID(IProcessPluginMessage_IID,IProcessPluginMessage)
		HE_INTERFACE_ENTRY_IID(IPluginAdapter_IID, IPluginAdapter)
		HE_INTERFACE_ENTRY_IID(IPluginUISource_IID,IPluginUISource)
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSIST_IID,IHePersist)
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSISTXML_IID,IHePersistXml)
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

public:
	;				CRemotePlugin();
	virtual			~CRemotePlugin();

	CAdapter		CreateAdapter();
	CAdapter		CreateAdapter( CAdapterInfo inInfo );

	void			DeleteAdapter( IAdapter* inAdapter );

	HeResult		FinalConstruct();
	void			FinalRelease();

	CGlobalId		GetContextId() const { return m_idContext; }
	COmniPlugin*	GetPlugin() { return m_pPlugin; }

	bool			IsGraphs() const { return m_bGraphsContext; }

	// IPeekPlugin
public:
	HE_IMETHOD		GetName( Helium::HEBSTR* outName );
	HE_IMETHOD		GetClassID( Helium::HeCID* outClassId );

	// IPeekPluginLoad
public:
	HE_IMETHOD		Load( const Helium::HeID& inContextId );
	HE_IMETHOD		Unload();

	// IProcessPluginMessage
public:
	HE_IMETHOD		ProcessPluginMessage( const Helium::HeCID& inId,
		Helium::IHeStream* inMessage, Helium::IHeStream* outResponse );

	// IPluginAdapter
public:
	HE_IMETHOD		DeleteAdapter( const Helium::HeID& adapterID );

	// IPluginUISource
public:
	HE_IMETHOD		GetHandlerID( Helium::HeID* pHandlerID );
	HE_IMETHOD		SetHandlerID( const Helium::HeID* pHandlerID );
	HE_IMETHOD		GetData( int inType, UInt32* pFlags, Helium::IHeStream* pOutData );

	// IHePersistXml
public:
	HE_IMETHOD		Load( IHeUnknown* inDocument, IHeUnknown* inNode );  // Only used by console
	HE_IMETHOD		Save( IHeUnknown* ioDocument, IHeUnknown* ioNode, int inClearDirty );

	// IHeObjectWithSiteImpl override
public:
	HE_IMETHOD		SetSite( IHeUnknown *inUnkSite );
};


// =============================================================================
//		CRemotePluginList
// =============================================================================

class CRemotePluginList
	:	public CPeekArray<CRemotePlugin*>
{
public:
	;		CRemotePluginList() {}

	bool	Find( const CGlobalId& inId, CRemotePlugin*& outPlugin ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRemotePlugin*	pPlugin = GetAt( i );
			if ( pPlugin->GetContextId() == inId ) {
				outPlugin = pPlugin;
				return true;
			}
		}
		return false;
	}
	bool	Find( CRemotePlugin* inPlugin, size_t& outIndex ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRemotePlugin*	pPlugin = GetAt( i );
			if ( pPlugin == inPlugin ) {
				outIndex = i;
				return true;
			}
		}
		return false;
	}

	void			UnloadAll() {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRemotePlugin*	pPlugin = GetAt( i );
			if ( pPlugin ) {
				pPlugin->Unload();
			}
		}
	}
};
