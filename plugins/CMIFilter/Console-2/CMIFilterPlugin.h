// ============================================================================
//	CMIFilterPlugin.h
//		interface for the CCMIFilterPlugin class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "Options.h"

const UInt16	kAttributes =
	kPluginAttr_HasOptions			|
	kPluginAttr_Filters				|
	kPluginAttr_HandlesSummaries	|
	kPluginAttr_HandlesErrorPackets;
const UInt64	kAttributesEx =
	kPluginAttr_HasContextOptions	|
	kPluginAttr_HandlesProcessTime;
const UInt16	kSupportedActions =
	kPluginAction_Display	|
	kPluginAction_Notify;
const UInt16	kDefaultActions =
	kPluginAction_Display	|
	kPluginAction_Notify;

class CCMIFilterPlugin;

extern CCMIFilterPlugin* g_Plugin;


// ============================================================================
//		CCMIFilterPlugin
// ============================================================================

class CCMIFilterPlugin
	:	public CPeekPlugin
{
protected:
	static UInt32		s_ParentArray[];
	static UInt32		s_ParentCount;
	PluginID			s_ID;
	CSafeOptions		m_SafeOptions;
	CString				m_strPrefs;
	BOOL				m_bLoaded;
	static WNDCLASSEX	s_WndClass;
	static bool			s_bRegistered;
	CString				m_strConfigPathName;
	CString				m_strChangeLogPathName;

public:
	;				CCMIFilterPlugin();
	virtual			~CCMIFilterPlugin() {}

	virtual void	GetName( TCHAR* outName, int nBufSize ) const;
	virtual void	GetID( PluginID* outID ) const { *outID = s_ID; };
	virtual UInt16	GetAttributes() const { return kAttributes; };
	virtual UInt64	GetAttributesEx() const { return (kAttributes | kAttributesEx); };
	virtual UInt16	GetSupportedActions() const { return kSupportedActions; };
	virtual UInt16	GetDefaultActions() const { return kDefaultActions; };
	virtual void	GetSupportedProtoSpecs( UInt32* outNumProtoSpecs, UInt32** outProtoSpecs ) const;
	COptionsPtr		GetOptions() { return m_SafeOptions.Get(); }
	void			SetOptions( const COptions& inOptions ) { m_SafeOptions.Copy( inOptions ); }

	virtual int		OnLoad( PluginLoadParam* ioParams );
	virtual int		OnUnload( );
	virtual int		OnReadPrefs();
	virtual int		OnWritePrefs();
	virtual int		OnAbout();
	virtual int		OnOptions();
	virtual int		OnCreateContext( PluginCreateContextParam* ioParams );
	virtual int		OnDisposeContext( PluginDisposeContextParam* ioParams );

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
	virtual int		OnProcessPluginMessage( PluginProcessPluginMessageParam* ioParams );
	virtual int		OnContextOptions( PluginContextOptionsParam* ioParams );
	virtual int		OnSetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnGetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnProcessTime( PluginProcessTimeParam* ioParams );

	void			WritePrefs();
};
