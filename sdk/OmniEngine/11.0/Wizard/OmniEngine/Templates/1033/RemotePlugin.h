// =============================================================================
//	RemotePlugin.h
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#pragma once

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
[!if !NOTIFY_UIHANDLER]
#ifdef _WIN32
#include "PeekConsoleContext.h"
#endif // _WIN32
[!endif]
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
// When defined, creates a log file in C:\Temp and logs
// all (most) entry points.
#define DEBUG_LOGINTERFACE
#endif
[!if !NOTIFY_UIHANDLER]

#ifdef PEEK_UI
// ============================================================================
//		CPluginApp
// ============================================================================

class CPluginApp
	:	public CWinApp
{
protected:
	DECLARE_MESSAGE_MAP()

public:
	;				CPluginApp();

	virtual BOOL	InitInstance();
};
#endif
[!endif]


// =============================================================================
//		CRemotePlugin
// =============================================================================

class HE_NO_VTABLE CRemotePlugin
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
	,	public HeLib::CHeClass<CRemotePlugin>
	,	public IPeekPlugin
	,	public IPeekPluginLoad
	,	public IEnginePluginCaptureEvents

#ifdef IMPLEMENT_RESETPROCESSING
	,	public IResetProcessing
#endif // IMPLEMENT_RESETPROCESSING

#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
	,	public IPacketProcessorEvents
	,	public IPeekCaptureEvents
#endif //IMPLEMENT_PACKETPROCESSOREVENTS

#ifdef IMPLEMENT_FILTERPACKET
	,	public IFilterPacket
#endif // IMPLEMENT_FILTERPACKET

#ifdef IMPLEMENT_PROCESSPACKET
	,	public IProcessPacket
#endif // IMPLEMENT_PROCESSPACKET

#ifdef IMPLEMENT_PROCESSTIME
	,	public IProcessTime
#endif // IMPLEMENT_PROCESSTIME

#if defined(IMPLEMENT_SUMMARIZEPACKET) && !defined(WP_LINUX)
	,	public ISummarizePacket
#endif // IMPLEMENT_SUMMARIZEPACKET && !WP_LINUX

#ifdef IMPLEMENT_ALL_UPDATESUMMARY
	,	public IUpdateSummaryStats
#endif // IMPLEMENT_ALL_UPDATESUMMARY

	,	public Helium::IHePersistXml

#ifdef IMPLEMENT_NOTIFICATIONS
	,	public IAction
	,	public INotify
#endif // IMPLEMENT_NOTIFICATIONS

	// Engine-Only Interfaces
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public IProcessPluginMessage
#endif // IMPLEMENT_PLUGINMESSAGE

[!if NOTIFY_UIHANDLER]
	,	public IPluginUISource
[!else]

#ifdef _WIN32
	// Console-Only Interfaces
	,	public IPluginUI
#ifdef IMPLEMENT_PLUGINMESSAGE
	,	public IPluginRecv
#endif // IMPLEMENT_PLUGINMESSAGE

	,	public IPeekPluginOptions
	,	public IPeekPluginAbout
#endif // _WIN32

[!endif]
	,	public IHeObjectWithSiteImpl<CRemotePlugin>
{
	friend class CPeekPlugin;

	enum {
		kStatus_Uninitialized,
		kStatus_Initialized
	};

	typedef std::unique_ptr<CPeekDataModeler>	CPeekDataModelerPtr;
	typedef int (CPeekEngineContext::*FnReset)();
#ifdef IMPLEMENT_ALL_UPDATESUMMARY
  #ifdef IMPLEMENT_UPDATESUMMARYSTATS
	typedef int (CPeekEngineContext::*FnSummary)( CSnapshot& );
  #else // IMPLEMENT_UPDATESUMMARYSTATS
	typedef int (CPeekEngineContext::*FnSummary)();
  #endif // IMPLEMENT_UPDATESUMMARYSTATS
#endif // IMPLEMENT_ALL_UPDATESUMMARY
protected:
	CPeekArray<std::string>		m_ayMsgLog;

#ifdef _WIN32
	HINSTANCE			m_hInstance;
[!if !NOTIFY_UIHANDLER]
	CConsoleUI			m_ConsoleUI;
[!endif]
#endif

	CPeekProxy			m_PeekProxy;
	COmniPluginPtr		m_spPlugin;

	CGlobalId			m_idContext;
	CPeekContextPtr		m_spContext;
	CPeekDataModelerPtr	m_spdmPrefs;
	bool				m_bGraphsContext;
	FnReset				m_pfnReset;
#ifdef IMPLEMENT_ALL_UPDATESUMMARY
	FnSummary			m_pfnSummary;
#endif

#ifdef IMPLEMENT_NOTIFICATIONS
	HeID						m_idAction;
	//	CPeekArray<HeID>			m_ayDisabledSources;
#endif // IMPLEMENT_NOTIFICATIONS

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

[!if !NOTIFY_UIHANDLER]
	bool			HasConsoleUI() const {
#ifdef _WIN32
		return m_ConsoleUI.IsValid();
#else
		return false;
#endif // _WIN32
	}
[!endif]
	bool			HasContext() const { return m_spContext.IsValid(); }
	bool			HasContextId() const { return m_idContext.IsValid(); }
	bool			HasInitializedPlugin() const {
		return (m_spPlugin.IsValid() && m_spPlugin->IsInitialized());
	}
	bool			HasNoInitializedPlugin() const {
		return (m_spPlugin.IsNotValid() || !m_spPlugin->IsInitialized());
	}
	bool			HasNoPlugin() const { return m_spPlugin.IsNotValid(); }
	bool			HasPlugin() const { return m_spPlugin.IsValid(); }
	bool			HasSite() const { return (m_spUnkSite != nullptr); }

	void			InvalidateContextId() {
		m_idContext.Invalidate();
		if ( HasContext() ) {
			m_spContext->SetId( m_idContext );
		}
	}
	bool			IsValidContext() const { return (HasSite() && HasContext()); }

	bool			LoadContext();

[!if !NOTIFY_UIHANDLER]
#ifdef _WIN32
	void			SetConsole( IConsoleUI* inConsoleUI ) {
		m_ConsoleUI.SetPtr( inConsoleUI );
	}
#else
	void			SetConsole( void* inConsoleUI ) {
		(void)inConsoleUI;
	}
#endif // _WIN32
[!endif]
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
		HE_INTERFACE_ENTRY_IID(IEnginePluginCaptureEvents_IID,IEnginePluginCaptureEvents)

#ifdef IMPLEMENT_RESETPROCESSING
		HE_INTERFACE_ENTRY_IID(IResetProcessing_IID,IResetProcessing)
#endif // IMPLEMENT_RESETPROCESSING

#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
		HE_INTERFACE_ENTRY_IID(IPacketProcessorEvents_IID,IPacketProcessorEvents)
		HE_INTERFACE_ENTRY_IID(IPeekCaptureEvents_IID,IPeekCaptureEvents)
#endif // IMPLEMENT_PACKETPROCESSOREVENTS

#ifdef IMPLEMENT_FILTERPACKET
		HE_INTERFACE_ENTRY_IID(IFilterPacket_IID,IFilterPacket)
#endif // IMPLEMENT_FILTERPACKET

#ifdef IMPLEMENT_PROCESSPACKET
		HE_INTERFACE_ENTRY_IID(IProcessPacket_IID,IProcessPacket)
#endif // IMPLEMENT_PROCESSPACKET

#ifdef IMPLEMENT_PROCESSTIME
		HE_INTERFACE_ENTRY_IID(IProcessTime_IID,IProcessTime)
#endif // IMPLEMENT_PROCESSTIME

#if defined(IMPLEMENT_SUMMARIZEPACKET) && !defined(WP_LINUX)
		HE_INTERFACE_ENTRY_IID(ISummarizePacket_IID,ISummarizePacket)
#endif // IMPLEMENT_SUMMARIZEPACKET && !WP_LINUX

#ifdef IMPLEMENT_ALL_UPDATESUMMARY
		HE_INTERFACE_ENTRY_IID(IUpdateSummaryStats_IID,IUpdateSummaryStats)
#endif // IMPLEMENT_ALL_UPDATESUMMARY

		HE_INTERFACE_ENTRY_IID(HE_IHEPERSIST_IID,IHePersist)
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSISTXML_IID,IHePersistXml)

#ifdef IMPLEMENT_NOTIFICATIONS
		HE_INTERFACE_ENTRY_IID(IAction_IID,IAction)
		HE_INTERFACE_ENTRY_IID(INotify_IID,INotify)
#endif // IMPLEMENT_NOTIFICATIONS

	// Engine-Only Interfaces
#ifdef IMPLEMENT_PLUGINMESSAGE
		HE_INTERFACE_ENTRY_IID(IProcessPluginMessage_IID,IProcessPluginMessage)
#endif // IMPLEMENT_PLUGINMESSAGE

[!if NOTIFY_UIHANDLER]
		HE_INTERFACE_ENTRY_IID(IPluginUISource_IID,IPluginUISource)
[!else]
#ifdef _WIN32
		// Console-Only Interfaces
		HE_INTERFACE_ENTRY_IID(IPluginUI_IID,IPluginUI)
#ifdef IMPLEMENT_PLUGINMESSAGE
		HE_INTERFACE_ENTRY_IID(IPluginRecv_IID,IPluginRecv)
#endif // IMPLEMENT_PLUGINMESSAGE

		HE_INTERFACE_ENTRY_IID(IPeekPluginOptions_IID,IPeekPluginOptions)
		HE_INTERFACE_ENTRY_IID(IPeekPluginAbout_IID,IPeekPluginAbout)
#endif // _WIN32
[!endif]

		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

public:
	;				CRemotePlugin();
	virtual			~CRemotePlugin();

	HeResult		FinalConstruct();
	void			FinalRelease();

#ifdef IMPLEMENT_PLUGINMESSAGE
	//bool			SendPluginMessage( CGlobalId inContextId,
	//					CPeekStream& inMessage, DWORD inTimeout, DWORD& outTransId ) {
	//	return m_Plugin.SendPluginMessage( inContextId, inMessage, inTimeout, outTransId );
	//}
#endif

[!if !NOTIFY_UIHANDLER]
#ifdef _WIN32
	CConsoleUI		GetConsole() const { return m_ConsoleUI; }
	HINSTANCE		GetInstanceHandle() const { return m_hInstance; }
#else
	void*			GetConsole() const { return nullptr; }
#endif // _WIN32
[!endif]
	CGlobalId		GetContextId() const { return m_idContext; }
	CPeekProxy		GetPeekProxy() const { return m_PeekProxy; }
	COmniPlugin*	GetPlugin() { return m_spPlugin.get(); }

	bool			IsGraphs() const { return m_bGraphsContext; }

	// .................................................................
	// Interfaces implemented in both the Engine and the Console.
	// .................................................................

	// IPeekPlugin
public:
	HE_IMETHOD		GetName( Helium::HEBSTR* outName );
	HE_IMETHOD		GetClassID( Helium::HeCID* outClassId );

	// IHeObjectWithSiteImpl override
public:
	HE_IMETHOD		SetSite( IHeUnknown *inUnkSite );

	// IPeekPluginLoad
public:
	HE_IMETHOD		Load( const Helium::HeID& inContextId );
	HE_IMETHOD		Unload();

	// IEnginePluginCaptureEvents
public:
	HE_IMETHOD		OnCaptureCreated( IHeUnknown* inCapture );
	HE_IMETHOD		OnCaptureDeleted( IHeUnknown* inCapture );

	// IHePersistXml
public:
	HE_IMETHOD		Load( IHeUnknown* inDocument, IHeUnknown* inNode );  // Only used by console
	HE_IMETHOD		Save( IHeUnknown* ioDocument, IHeUnknown* ioNode, int inClearDirty );

	// .................................................................
	// Interfaces implemented in only the Engine.
	// .................................................................

#ifdef IMPLEMENT_NOTIFICATIONS
	// IAction
public:
	HE_IMETHOD GetID( Helium::HeID* pID );
	HE_IMETHOD SetID( const Helium::HeID& id );
	//	HE_IMETHOD GetName( HEBSTR* pbstrName );
	HE_IMETHOD SetName( Helium::HEBSTR bstrName );
	HE_IMETHOD EnableSeverity( PeekSeverity severity, BOOL bEnable );
	HE_IMETHOD IsSeverityEnabled( PeekSeverity severity, BOOL* pbEnabled );
	HE_IMETHOD GetDisabledSources( Helium::HeID** ppSources, UInt32* pnSources );
	HE_IMETHOD SetDisabledSources( const Helium::HeID* pSources, UInt32 nSources );
	HE_IMETHOD IsSourceEnabled( const Helium::HeID& guidSource, BOOL* pbEnabled );

	// INotify
public:
	HE_IMETHOD Notify( const Helium::HeID& guidContext, const Helium::HeID& guidSource,
		UInt32 ulSourceKey, UInt64 ullTimeStamp, PeekSeverity severity,
		const wchar_t* pszShortMessage, const wchar_t* pszLongMessage );
#endif // IMPLEMENT_NOTIFICATIONS

#ifdef IMPLEMENT_PLUGINMESSAGE
	// IProcessPluginMessage
public:
	HE_IMETHOD		ProcessPluginMessage( const Helium::HeCID& inId,
		Helium::IHeStream* inMessage, Helium::IHeStream* outResponse );
#endif // IMPLEMENT_PLUGINMESSAGE

#ifdef IMPLEMENT_RESETPROCESSING
	// IResetProcessing
public:
	HE_IMETHOD		ResetProcessing();
#endif // IMPLEMENT_RESETPROCESSING

#ifdef IMPLEMENT_PACKETPROCESSOREVENTS
	// IPacketProcessorEvents
public:
	HE_IMETHOD		OnStartPacketProcessing( UInt64 ullStartTime );
	HE_IMETHOD		OnStopPacketProcessing( UInt64 ullStopTime );
	HE_IMETHOD		OnResetPacketProcessing();

	// IPeekCaptureEvents
public:
	HE_IMETHOD		OnCaptureStarting();
	HE_IMETHOD		OnCaptureStarted();
	HE_IMETHOD		OnCaptureStopping();
	HE_IMETHOD		OnCaptureStopped();
#endif // IMPLEMENT_PACKETPROCESSOREVENTS

#ifdef IMPLEMENT_FILTERPACKET
	// IFilterPacket
public:
	HE_IMETHOD		FilterPacket( IPacket* pPacket,
		UInt16* pcbBytesToAccept, Helium::HeID* pFilterID );
#endif // IMPLEMENT_FILTERPACKET

#ifdef IMPLEMENT_PROCESSPACKET
	// IProcessPacket
public:
	HE_IMETHOD		ProcessPacket( IPacket* pPacket );
#endif // IMPLEMENT_PROCESSPACKET

#ifdef IMPLEMENT_PROCESSTIME
	// IProcessTime
public:
	HE_IMETHOD		ProcessTime( UInt64 ullCurrentTime );
#endif // IMPLEMENT_PROCESSTIME

#ifdef IMPLEMENT_ALL_UPDATESUMMARY
	// IUpdateSummaryStats
public:
	HE_IMETHOD		UpdateSummaryStats( IHeUnknown* pSnapshot );
#endif // IUpdateSummaryStats

[!if NOTIFY_UIHANDLER]
	// IPluginUISource
public:
	HE_IMETHOD		GetHandlerID( Helium::HeID* pHandlerID );
	HE_IMETHOD		SetHandlerID( const Helium::HeID* pHandlerID );
	HE_IMETHOD		GetData( int inType, UInt32* pFlags, Helium::IHeStream* pOutData );
[!else]
#ifdef _WIN32
	// .................................................................
	// Interfaces implemented in only the Console.
	// .................................................................

	// IPluginUI
public:
	HE_IMETHOD		SetEngine( IHeUnknown* inEngine );
	HE_IMETHOD		SetConsoleUI( IConsoleUI* inConsoleUI );
	HE_IMETHOD		GetUIInfo( Helium::HEBSTR* outCommand, Helium::HEBSTR* outText );
	HE_IMETHOD		LoadView();
	HE_IMETHOD		UnloadView();

#ifdef IMPLEMENT_PLUGINMESSAGE
	// IPluginRecv
public:
	HE_IMETHOD		Recv( HeResult inMsgResult, UInt32 inTransId,
		Helium::IHeStream* inMessage );
#endif // IMPLEMENT_PLUGINMESSAGE

#if defined(IMPLEMENT_SUMMARIZEPACKET) && !defined(WP_LINUX)
	// ISummarizePacket
public:
	HE_IMETHOD		SummarizePacket( IPacket* pPacket,
		Helium::HEBSTR* pbstrSummary, UInt32* pColor, PeekSummaryLayer* pLayer );
#endif // IMPLEMENT_SUMMARIZEPACKET && !WP_LINUX

	// IPeekPluginAbout
public:
	HE_IMETHOD		ShowAbout();

	// IPeekPluginOptions
public:
	HE_IMETHOD		ShowOptions();
#endif // _WIN32
[!endif]
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
