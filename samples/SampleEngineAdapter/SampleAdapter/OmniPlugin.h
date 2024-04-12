// =============================================================================
//	OmniPlugin.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "OmniContext.h"
#include "Options.h"


// =============================================================================
//		COmniPlugin
// =============================================================================

class COmniPlugin
	:	public CPeekPlugin
{
protected:
	CHePtr<IHeServiceProvider>	m_spServices;	
	COmniContext				m_Context;

public:
	;				COmniPlugin( CPeekProxy& PeekProxy );
	virtual			~COmniPlugin() {}

	int				GetSourceIP() const { return m_Context.GetSourceIP(); }

	virtual int		OnCreateContext();
	virtual int		OnDisposeContext();
	virtual int		OnFilter( CPacket& Packet, UInt16* pcbBytesToAccept, GUID& guidFilter );
	static int		OnGetClassId( GUID& guidClassId );
	virtual int		OnGetContextPrefs( CXmlElement& xmlElement, int bClearDirty );
	static int		OnGetName( CString& strName );
	virtual	int		OnGetPacketString( CPacket& Packet, CString& strSummary,
		UInt32& Color, SummaryLayer* pSummaryLayer );
	virtual	int		OnLoad( const GUID& guidContext );
	virtual	int		OnProcessPacket( CPacket& Packet );
	virtual int		OnReadPrefs();
	virtual	int		OnReset();
	virtual int		OnSetContextPrefs( CXmlElement& xmlElement );
	virtual	int		OnShowAbout();
	virtual	int		OnShowOptions();
	virtual int		OnStartCapture();
	virtual int		OnStopCapture();
	virtual	int		OnSummary( CSnapshot& Snapshot );
	virtual	int		OnUnload();
	virtual int		OnWritePrefs();
	virtual int		OnGetContextPrefs( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode,int bClearDirty );
	virtual int		OnSetContextPrefs( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode,int bClearDirty );
};
