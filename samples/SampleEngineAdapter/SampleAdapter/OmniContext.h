// =============================================================================
//	OmniContext.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekContext.h"
#include "Options.h"


// =============================================================================
//		COmniContext
// =============================================================================

class COmniContext
	:	public CPeekContext
{
protected:	
	COptions		m_Options;
	UInt32			m_nSrcIP;

public:
	;				COmniContext( CPeekProxy& Proxy );
	virtual			~COmniContext() {}

	UInt32			GetSourceIP() const { return m_nSrcIP; }

	virtual int		OnFilter( CPacket& Packet, UInt16* pcbBytesToAccept, GUID& guidFilter );
	virtual int		OnGetContextPrefs( CXmlElement& xmlElement, int bClearDirty );
	virtual	int		OnGetPacketString( CPacket& Packet, CString& strSummary, UInt32& Color, SummaryLayer* pSummaryLayer );
	virtual	int		OnProcessPacket( CPacket& Packet );
	virtual	int		OnReset();
	virtual int		OnSetContextPrefs( CXmlElement& xmlElement );
	virtual	int		OnShowOptions();
	virtual int		OnStartCapture();
	virtual int		OnStopCapture();
	virtual	int		OnSummary( CSnapshot& Snapshot );
	int				OnGetContextPrefs( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode, int bClearDirty );
	int				OnSetContextPrefs( IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode, int bClearDirty );
};
