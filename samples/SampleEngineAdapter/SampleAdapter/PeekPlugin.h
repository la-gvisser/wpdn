// =============================================================================
//	PeekPlugin.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "Plugin.h"
#include "PeekProxy.h"

using namespace Plugin;

class CPacket;
class CXmlDocument;
class CXmlElement;
class CSnapshot;


// =============================================================================
//		CPeekPlugin
// =============================================================================

class CPeekPlugin
{
protected:
	GUID			m_guidContext;
	CPeekProxy&		m_PeekProxy;

public:
	;				CPeekPlugin( CPeekProxy& Proxy );
	virtual			~CPeekPlugin();

	virtual int		OnCreateContext() = 0;
	virtual int		OnDisposeContext() = 0;
	virtual int		OnFilter( CPacket& Packet, UInt16* pcbBytesToAccept, GUID& guidFilter ) = 0;
	static int		OnGetClassId( GUID& guidClassId );
	virtual int		OnGetContextPrefs( CXmlElement& xmlElement, int bClearDirty ) = 0;
	virtual int		OnGetContextPrefs( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode,int bClearDirty ) = 0;
	static int		OnGetName( CStringW& strName );
	virtual	int		OnGetPacketString( CPacket& Packet, CString& strSummary, UInt32& Color, SummaryLayer* pSummaryLayer ) = 0;
	virtual	int		OnLoad( const GUID& guidContext ) = 0;
	virtual	int		OnProcessPacket( CPacket& Packet ) = 0;
	virtual int		OnReadPrefs() = 0;
	virtual	int		OnReset() = 0;
	virtual int		OnSetContextPrefs( CXmlElement& xmlElement ) = 0;
	virtual int		OnSetContextPrefs( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode,int bClearDirty ) = 0;
	virtual	int		OnShowAbout() = 0;
	virtual	int		OnShowOptions() = 0;
	virtual	int		OnSummary( CSnapshot& Snapshot ) = 0;
	virtual	int		OnUnload() = 0;
	virtual int		OnWritePrefs() = 0;
};
