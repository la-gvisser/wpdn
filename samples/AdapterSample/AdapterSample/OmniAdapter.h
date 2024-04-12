// =============================================================================
//	PeekSocket.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekAdapter.h"
#include "AdapterStats.h"
#include "PeekSocket.h"
#include "PeekContextProxy.h"
#include "GlobalId.h"
#include <map>


// =============================================================================
//		COmniAdapter
// =============================================================================

class COmniAdapter
	:	public CPeekAdapter
	,	public CAdapterStats
{
protected:
	static UInt64		m_dbgPacketNumber;

protected:
	CPeekString			m_strAdapterName;
	CPluginAdapterInfo	m_AdapterInfo;
	CPacketBuffer		m_PacketBuffer;
	CMediaInfo			m_MediaInfo;

	tMediaType	m_MediaType;
	UInt64		m_nLinkSpeed;


	CPeekSocket	m_Socket;
	CPort		m_Port;
	HANDLE		m_hCaptureThread;
	HANDLE		m_hStopEvent;

	bool		m_bInsertingPacket;
	bool		m_bGotPackets;

	UInt32		m_nPacketCount;
	CPeekString	m_strDataPath;

	size_t					m_nRunningCaptures;
	CPacketHandlerArray		m_spPacketHandler;

protected:
	void	CloseSocket() { m_Socket.Close(); }
	void	ProcessBuffer( UInt32 inLength, UInt8* inBuffer, const CIpAddress& inRecvAddress );

public:
	;		COmniAdapter();
	virtual	~COmniAdapter();

	virtual void	AddPacketHandler( CPacketHandler inHandler );

	virtual void	CopyAdapterInfo( IAdapterInfo** outAdapterInfo ) {
		m_AdapterInfo.Copy( outAdapterInfo );
	}

	virtual CAdapterInfo	GetAdapterInfo() const { return m_AdapterInfo; }
	virtual const CPeekString&	GetAdapterName() const {
		return m_strAdapterName;
	}
	virtual bool			GetAddress( CEthernetAddress& /*outAddress*/ ) const {
		return false;
	}
	const CPeekString&		GetDataPath() const { return m_strDataPath; }
	virtual UInt64			GetLinkSpeed() const { return m_nLinkSpeed; }
	virtual tMediaType		GetMediaType() const { return m_MediaType; }
	UInt32					GetPacketCount() const { return m_nPacketCount; }

	bool	Initialize();
	bool	IsGotPackets() const { return m_bGotPackets; }
	bool	IsInsertingPacket() const { return m_bInsertingPacket; }

	virtual void	RemovePacketHandler( CPacketHandler inHandler );
	virtual int		Run();

	virtual void	SetDataPath( const CPeekString& inPath ) { m_strDataPath = inPath; }
	virtual void	SetPacketCount( UInt32 inCount ) { m_nPacketCount = inCount; }
	void			SetPort( UInt16 inPort ) { m_Port = inPort; }
	void			SetPort( const wchar_t* inPort ) { m_Port.Parse( inPort ); }
	virtual int		StartCapture();
	virtual int		StopCapture();

	void	Terminate();

	// CAdapterStats
	int				GetStatistics( IPacketHandler* pHandler, void** ppStatistics );
};
