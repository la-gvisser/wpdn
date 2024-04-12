// =============================================================================
//	RemotePlugin.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "peekcore.h"
#include "peekstats.h"
#include "peeknames.h"
#include "objectwithsiteimpl.h"
#include "PeekProxy.h"

class CPeekPlugin;


// =============================================================================
//		CRemotePlugin
// =============================================================================

class HE_NO_VTABLE CRemotePlugin : 
	public CHeObjRoot<CHeMultiThreadModel>,
	public CHeClass<CRemotePlugin>,
	public IPeekPlugin,
	public IPeekPluginLoad,
	public IPeekPluginAbout,
	public IPeekPluginOptions,
	public IFilterPacket,
	public ISummarizePacket,
	public IUpdateSummaryStats,
	public IProcessPacket,
	public IResetProcessing,
	public IHePersistXml,
	public IHeObjectWithSiteImpl<CRemotePlugin>
{
	friend class CProxyState;

public:
	HE_INTERFACE_MAP_BEGIN(CRemotePlugin)
		HE_INTERFACE_ENTRY_IID(IPeekPlugin_IID,IPeekPlugin)
		HE_INTERFACE_ENTRY_IID(IPeekPluginLoad_IID,IPeekPluginLoad)
		HE_INTERFACE_ENTRY_IID(IPeekPluginAbout_IID,IPeekPluginAbout)
		HE_INTERFACE_ENTRY_IID(IPeekPluginOptions_IID,IPeekPluginOptions)
		HE_INTERFACE_ENTRY_IID(IFilterPacket_IID,IFilterPacket)
		HE_INTERFACE_ENTRY_IID(ISummarizePacket_IID,ISummarizePacket)
		HE_INTERFACE_ENTRY_IID(IUpdateSummaryStats_IID,IUpdateSummaryStats)
		HE_INTERFACE_ENTRY_IID(IProcessPacket_IID,IProcessPacket)
		HE_INTERFACE_ENTRY_IID(IResetProcessing_IID,IResetProcessing)
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSIST_IID,IHePersist)
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSISTXML_IID,IHePersistXml)
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

protected:
	HeCID			m_cid;
	CPeekPlugin*	m_pPlugin;
	CPeekProxy		m_PeekProxy;

	HeResult		BeginCommand();
	HeResult		EndCommand();

public:
	;				CRemotePlugin();
	virtual			~CRemotePlugin() {}

	HeResult		FinalConstruct();
	void			FinalRelease();

	// IPeekPlugin
public:
	HE_IMETHOD		GetName( HEBSTR* pbstrName );
	HE_IMETHOD		GetClassID( HeCID* pCLSID );

	// IPeekPluginLoad
public:
	HE_IMETHOD		Load( const HeID& guidContext );
	HE_IMETHOD		Unload();

	// IHePersistXml
public:
	HE_IMETHOD		Load( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode );
	HE_IMETHOD		Save( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode, int bClearDirty );

	// IPeekPluginAbout
public:
	HE_IMETHOD		ShowAbout();

	// IPeekPluginOptions
public:
	HE_IMETHOD		ShowOptions();

	// IFilterPacket
public:
	HE_IMETHOD		FilterPacket(IPacket* pPacket, UInt16* pcbBytesToAccept, Helium::HeID* pFilterID );

	// ISummarizePacket
public:
	HE_IMETHOD		SummarizePacket( IPacket* pPacket, HEBSTR* pbstrSummary,
		UInt32* pColor, PeekSummaryLayer* pLayer );

	// IUpdateSummaryStats
public:
	HE_IMETHOD		UpdateSummaryStats( IHeUnknown* pSnapshot );

	// IProcessPacket
public:
	HE_IMETHOD		ProcessPacket( IPacket* pPacket );

	// IResetProcessing
public:
	HE_IMETHOD		ResetProcessing();

private:
	HeID	m_CaptureId;
};


// =============================================================================
//		CProxyState
// =============================================================================

class CProxyState
{
protected:
	CRemotePlugin&	m_Plugin;

public:
	;		CProxyState( CRemotePlugin& Plugin ) : m_Plugin( Plugin ) { m_Plugin.BeginCommand(); }
	;		~CProxyState() { m_Plugin.EndCommand(); }
};
