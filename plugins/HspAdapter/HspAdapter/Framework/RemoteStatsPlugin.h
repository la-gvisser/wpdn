// =============================================================================
//	RemoteStatsPlugin.h
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_SUMMARY

#include "peekcore.h"
#include "peekstats.h"
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
//	Remote Stats Plugin
//
//	A Remote Stts Plugin is the root object of a plugin Framework. OmniPeek and
//	OmniEngine create instances of and directly communicate with Remote Plugin.
//	A Remote Plugin instantiates the Omni Plugin, Omni Console Context and Omni
//	Engine Context objects. It manages the life-cycle of these objects.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#if (0)
// When defined, creates a log file in C:\Temp and logs
// all (most) entry points.
#define DEBUG_LOGINTERFACE
#endif


// {E6DC81E6-BAB2-4691-896E-69BD763A907E}
#define CSnapshotContext_CID \
	{ 0xE6DC81E6, 0xBAB2, 0x4691, { 0x89, 0x6E, 0x69, 0xBD, 0x76, 0x3A, 0x90, 0x7E } }

/// \interface ISnapshotContext
#define ISnapshotContext_IID \
	{ 0x6C4345E8, 0xA8C4, 0x4BA6, { 0x9A, 0xB8, 0x18, 0xE4, 0xB9, 0xB3, 0x20, 0x44 } }

class HE_NO_VTABLE ISnapshotContext : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISnapshotContext_IID);

	/// Initialize the object.
	HE_IMETHOD Initialize(/*in*/ IHeUnknown* inSnapshot, /*in*/ void* inContext ) = 0;
	HE_IMETHOD GetSnapshot(/*out*/ IUpdateSummaryStats** outSnapshot ) = 0;
	HE_IMETHOD GetContext(/*in*/ void** outContext ) = 0;
};


// =============================================================================
//		CSnapshotContext
// =============================================================================

class HE_NO_VTABLE CSnapshotContext
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
	,	public HeLib::CHeClass<CSnapshotContext>
	,	public ISnapshotContext
 	,	public IUpdateSummaryStats
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(CSnapshotContext_CID);

public:
	HE_INTERFACE_MAP_BEGIN(CSnapshotContext)
		HE_INTERFACE_ENTRY_IID(ISnapshotContext_IID,ISnapshotContext)
	HE_INTERFACE_MAP_END()

protected:
	CHePtr<IUpdateSummaryStats>	m_spSnapshot;
	void*						m_pContext;

public:
	;			CSnapshotContext()
		:	m_pContext( nullptr )
	{}
	virtual		~CSnapshotContext() {}

	HRESULT		FinalConstruct() { return HE_S_OK; }
	void		FinalRelease() {}

	// IAdapterInitialize
public:
	HE_IMETHOD	Initialize( IHeUnknown* inSnapshot,  void* inContext ) {
		ObjectLock	lock( this );
		m_pContext = inContext;
		return inSnapshot->QueryInterface( &m_spSnapshot.p );
	}

	HE_IMETHOD	GetSnapshot( IUpdateSummaryStats** outSnapshot ) {
		return m_spSnapshot.CopyTo( outSnapshot );
	}

	HE_IMETHOD	GetContext( void** outContext ) {
		*outContext = m_pContext;
		return HE_S_OK;
	}

	// IUpdateSummaryStats
public:
	HE_IMETHOD	UpdateSummaryStats( IHeUnknown* pSnapshot ) {
		if ( m_spSnapshot != nullptr ) {
			return m_spSnapshot->UpdateSummaryStats( pSnapshot );
		}
		return HE_E_NULL_POINTER;
	}

};


// =============================================================================
//		CRemoteStatsPlugin
// =============================================================================

class HE_NO_VTABLE CRemoteStatsPlugin
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
	,	public HeLib::CHeClass<CRemoteStatsPlugin>
	,	public IPeekPlugin
	,	public IPeekPluginLoad
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public IProcessPluginMessage
#endif	// IMPLEMENT_PLUGINMESSAGE
	,	public IUpdateSummaryStats
	,	public IPluginUISource
	,	public IHeObjectWithSiteImpl<CRemoteStatsPlugin>
{
	friend class CPeekPlugin;

	enum {
		kStatus_Uninitialized,
		kStatus_Initialized
	};

	typedef std::unique_ptr<CPeekDataModeler>	CPeekDataModelerPtr;
	typedef int (CPeekEngineContext::*FnReset)();
	typedef int (CPeekEngineContext::*FnSummary)( IHeUnknown* );

protected:
	COmniPlugin*		m_pPlugin;

#ifdef _WIN32
	HINSTANCE			m_hInstance;
#endif

	CGlobalId			m_idContext;
	CPeekContextPtr		m_spContext;
	CPeekDataModelerPtr	m_spdmPrefs;
	bool				m_bGraphsContext;
	FnSummary			m_pfnSummary;

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

	CPeekContext*	GetContext() { return m_spContext; }

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
	bool			HasSite() const { return (m_spUnkSite != nullptr); }

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
	HE_INTERFACE_MAP_BEGIN(CRemoteStatsPlugin)
		HE_INTERFACE_ENTRY_IID(IPeekPlugin_IID,IPeekPlugin)
		HE_INTERFACE_ENTRY_IID(IPeekPluginLoad_IID,IPeekPluginLoad)
#ifdef IMPLEMENT_PLUGINMESSAGE
		HE_INTERFACE_ENTRY_IID(IProcessPluginMessage_IID,IProcessPluginMessage)
#endif	// IMPLEMENT_PLUGINMESSAGE
		HE_INTERFACE_ENTRY_IID(IUpdateSummaryStats_IID,IUpdateSummaryStats)
		HE_INTERFACE_ENTRY_IID(IPluginUISource_IID,IPluginUISource)
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

public:
	;				CRemoteStatsPlugin();
	virtual			~CRemoteStatsPlugin();

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

#ifdef IMPLEMENT_PLUGINMESSAGE
	// IProcessPluginMessage
public:
	HE_IMETHOD		ProcessPluginMessage( const Helium::HeCID& inId,
		Helium::IHeStream* inMessage, Helium::IHeStream* outResponse );
#endif	// IMPLEMENT_PLUGINMESSAGE

	// IUpdateSummaryStats
public:
	HE_IMETHOD		UpdateSummaryStats( IHeUnknown* pSnapshot );

	// IPluginUISource
public:
	HE_IMETHOD		GetHandlerID( Helium::HeID* pHandlerID );
	HE_IMETHOD		SetHandlerID( const Helium::HeID* pHandlerID );
	HE_IMETHOD		GetData( int inType, UInt32* pFlags, Helium::IHeStream* pOutData );

	// IHeObjectWithSiteImpl override
public:
	HE_IMETHOD		SetSite( IHeUnknown *inUnkSite );
};


// =============================================================================
//		CRemoteStatsPluginList
// =============================================================================

class CRemoteStatsPluginList
	:	public CPeekArray<CRemoteStatsPlugin*>
{
public:
	;		CRemoteStatsPluginList() {}

	bool	Find( const CGlobalId& inId, CRemoteStatsPlugin*& outPlugin ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRemoteStatsPlugin*	pPlugin = GetAt( i );
			if ( pPlugin->GetContextId() == inId ) {
				outPlugin = pPlugin;
				return true;
			}
		}
		return false;
	}
	bool	Find( CRemoteStatsPlugin* inPlugin, size_t& outIndex ) const {
		size_t	nCount = GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CRemoteStatsPlugin*	pPlugin = GetAt( i );
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
			CRemoteStatsPlugin*	pPlugin = GetAt( i );
			if ( pPlugin ) {
				pPlugin->Unload();
			}
		}
	}
};

#endif // IMPLEMENT_SUMMARY

