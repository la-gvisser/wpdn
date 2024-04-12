// ============================================================================
//	IpPenRegisterPlugin.h
//		interface for the CIpPenRegisterPlugin class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "IpPenRegisterContext.h"
#include "Options.h"

const UInt16	kAttributes( kPluginAttr_Filters );
const UInt64	kAttributesEx( 0 );
const UInt16	kSupportedActions( kPluginAction_Display | kPluginAction_Notify );
const UInt16	kDefaultActions( kPluginAction_Display | kPluginAction_Notify );


// ============================================================================
//		CIpPenRegisterPlugin
// ============================================================================

class CIpPenRegisterPlugin
	:	public CPeekPlugin
{
protected:
	static PluginID		s_ID;
	static UInt32		s_SupportedProtoSpecs[];
	static UInt32		s_SupportedProtoSpecCount;

	CIpPenRegisterContextList	m_aContextList;
	CIpPenRegisterContext*		m_pGlobalContext;
	CString				m_strPluginName;
	COptions			m_Options;
	CString				m_strPrefs;

public:
	;				CIpPenRegisterPlugin() : m_pGlobalContext( NULL ) {}
	virtual			~CIpPenRegisterPlugin() {};

	COptions*		GetOptions() { return &m_Options; }

	CString			GetPluginName() { return m_strPluginName; }
	virtual void	GetName( TCHAR* outName, int nBufSize ) const;
	virtual void	GetID( PluginID* outID ) const { *outID = s_ID; };
	virtual UInt16	GetAttributes() const { return kAttributes; };
	virtual UInt64	GetAttributesEx() const { return kAttributesEx; }
	virtual UInt16	GetSupportedActions() const { return kSupportedActions; };
	virtual UInt16	GetDefaultActions() const { return kDefaultActions; };
	virtual void	GetSupportedProtoSpecs( UInt32* outNumProtoSpecs, UInt32** outProtoSpecs ) const;
	CIpPenRegisterContext*	GetGlobalContext() const { return m_pGlobalContext; }

	//	Plugin Message Handlers
	virtual int		OnLoad( PluginLoadParam* ioParams );
	virtual int		OnUnload();
	virtual int		OnReadPrefs();
	virtual int		OnWritePrefs();
	virtual int		OnAbout();
	virtual int		OnOptions();
	virtual int		OnFilterOptions();
	virtual int		OnCreateContext( PluginCreateContextParam* ioParams );
	virtual int		OnDisposeContext( PluginDisposeContextParam* ioParams );
	virtual int		OnNameTableUpdate( PluginNameTableUpdateParam* ioParams );
	virtual int		OnCreateNewAdapter( PluginCreateNewAdapterParam* ioParams );
	virtual int		OnGetAdapterList( PluginGetAdapterListParam* ioParams );
	virtual int		OnDeleteAdapter( PluginDeleteAdapterParam* ioParams );
	virtual int		OnSetAdapterAttribs( PluginAdapterAttribsParam* ioParams );
	virtual int		OnGetAdapterAttribs( PluginAdapterAttribsParam* ioParams );
	virtual int		OnAdapterProperties( PluginAdapterPropertiesParam* ioParams );
	virtual int		OnUserDecode( PluginUserDecodeParam* ioParams );
	virtual int		OnDecodersLoaded();
	virtual int		OnIsMediaSupported( PluginMediaSupportedParam* ioParams );

	//	Context Specific Message Handlers
	virtual int		OnReset( PluginResetParam* ioParams );
	virtual int		OnStartCapture( PluginStartCaptureParam* ioParams );
	virtual int		OnStopCapture( PluginStopCaptureParam* ioParams );
	virtual int		OnProcessPacket( PluginProcessPacketParam* ioParams );
	virtual int		OnGetPacketString( PluginGetPacketStringParam* ioParams );
	virtual int		OnApply( PluginApplyParam* ioParams );
	virtual int		OnSelect( PluginSelectParam* ioParams );
	virtual int		OnPacketsLoaded( PluginPacketsLoadedParam* ioParams );
	virtual int		OnSummary( PluginSummaryParam* ioParams );
	virtual int		OnFilter( PluginFilterParam* ioParams );
	virtual int		OnGetPacketAnalysis( PluginGetPacketStringParam* ioParams );
	virtual int		OnDecodePacket( PluginDecodePacketParam* ioParams );
	virtual int		OnProcessPluginMessage( PluginProcessPluginMessageParam* ioParams );
	virtual int		OnHandleNotify( PluginHandleNotifyParam* ioParams );
	virtual int		OnGetTextForPacketListCell( PluginGetTextForPacketListCellParam* ioParams );
	virtual int		OnMeasurePacketListCell( PluginMeasurePacketListCellParam* ioParams );
	virtual int		OnDrawPacketListCell( PluginDrawPacketListCellParam* ioParams );
	virtual int		OnContextOptions( PluginContextOptionsParam* ioParams );
	virtual int		OnSetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnGetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnProcessTime( PluginProcessTimeParam* ioParams );
};
