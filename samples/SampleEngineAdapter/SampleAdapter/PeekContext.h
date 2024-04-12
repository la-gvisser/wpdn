// =============================================================================
//	PeekContext.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "Packet.h"
#include "PeekProxy.h"
#include "PluginXml.h"
#include "Snapshot.h"


// =============================================================================
//		CPeekContext
// =============================================================================

class CPeekContext
{
protected:
	CPeekProxy&		m_PeekProxy;

public:
	;				CPeekContext( CPeekProxy& Proxy );
	virtual			~CPeekContext();

	UInt32			DoAddNameEntry( const NameEntry& Entry, UInt32 Options ) {
		return m_PeekProxy.AddNameEntry( Entry, Options );
	}
	UInt32			DoLookupNameEntry( TMediaSpec& Spec, NameEntry& Entry, bool& bResult ) {
		return m_PeekProxy.LookupNameEntry( Spec, Entry, bResult );
	}
	UInt32			DoNotify( GUID& guidContext, GUID& guidSource, UInt32 nSourceKey, UInt64 nTimeStamp,
						int Severity, CStringW strShortMessage, CStringW strLongMessage ) {
		return m_PeekProxy.Notify( guidContext, guidSource, nSourceKey, nTimeStamp, Severity, strShortMessage, strLongMessage );
	}
	UInt32			DoRemoveNameEntry( const TMediaSpec& Spec ) {
		return m_PeekProxy.RemoveNameEntry( Spec );
	}
	UInt32			DoSpecToString( const TMediaSpec& Spec, CStringW& strText ) {
		return m_PeekProxy.SpecToString( Spec, strText );
	}

	virtual int		OnFilter( CPacket& Packet, UInt16* pcbBytesToAccept, GUID& guidFilter ) = 0;
	virtual int		OnGetContextPrefs( CXmlElement& xmlElement, int bClearDirty ) = 0;
	virtual	int		OnGetPacketString( CPacket& Packet, CString& strSummary, UInt32& Color, SummaryLayer* pSummaryLayer ) = 0;
	virtual	int		OnProcessPacket( CPacket& Packet ) = 0;
	virtual	int		OnReset() = 0;
	virtual int		OnSetContextPrefs( CXmlElement& xmlElement ) = 0;
	virtual int		OnStartCapture() = 0;
	virtual int		OnStopCapture() = 0;
	virtual	int		OnSummary( CSnapshot& Snapshot ) = 0;
};
