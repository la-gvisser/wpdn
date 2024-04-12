// =============================================================================
//	PeekSocket.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekAdapter.h"
#include "AdapterStats.h"
#include "PeekSocket.h"
#include "PeekContextProxy.h"
#include "PluginMessages.h"
#include "GlobalId.h"
#include "threadhelpers.h"
#include <map>

extern const CGlobalId	s_AdapterId;


// =============================================================================
//		COmniAdapter
// =============================================================================

class COmniAdapter
	:	public CPeekAdapter
	,	public CAdapterStats
	,	public ThreadHelpers::IThreadFunction
{
	friend class CRemoteAdapter;

protected:
#ifdef _WIN32
	static int				s_nOptionsId;
#endif // _WIN32
	static UInt64			m_dbgPacketNumber;

protected:
	CPeekString				m_strAdapterName;
	CPeekString				m_strPluginPath;
	CPacketBuffer			m_PacketBuffer;
	CMediaInfo				m_MediaInfo;
	tMediaType				m_MediaType;
	UInt64					m_nLinkSpeed;
	CPluginAdapterInfo		m_AdapterInfo;

	ThreadHelpers::Thread	m_CaptureThread;
	volatile SInt32			m_PauseCount;
	ThreadHelpers::Event	m_PauseEvent;

	size_t					m_nRunningCaptures;
	CPacketHandlerArray		m_spPacketHandler;
	UInt32					m_nPacketCount;
	UInt64					m_nDroppedPackets;	// If Per Capture Dropped, then remove.

#if (0)
	CPeekSocket				m_Socket;
	CPort					m_Port;
#endif // 0

protected:
#if (0)
	void	CloseSocket() { m_Socket.Close(); }
	void	ProcessBuffer( UInt32 inLength, UInt8* inBuffer, const CIpAddress& inRecvAddress );
#endif // 0

public:
	;				COmniAdapter( CRemoteAdapter* inRemoteAdapter );
	virtual			~COmniAdapter();

	virtual void	AddPacketHandler( CPacketHandler inHandler );

	virtual void	CopyAdapterInfo( IAdapterInfo** outAdapterInfo ) {
		m_AdapterInfo.Copy( outAdapterInfo );
	}

	virtual CAdapterInfo		GetAdapterInfo() const { return m_AdapterInfo; }
	virtual const CPeekString&	GetAdapterName() const { return m_strAdapterName; }
	virtual bool				GetAddress( CEthernetAddress& /*outAddress*/ ) const { return false; }
#if (0)
	CPeekString					GetDataFileFullName();
#endif // 0
	// If m_nDroppedPackets is removed, return 0.
	virtual UInt64				GetDroppedPackets() const { return m_nDroppedPackets; }
	virtual UInt64				GetLinkSpeed() const { return m_nLinkSpeed; }
	virtual tMediaType			GetMediaType() const { return m_MediaType; }
	UInt32						GetPacketCount() const { return m_nPacketCount; }

	bool			Initialize();
	bool			IsCapturing() const { return (m_nRunningCaptures > 0); }

	virtual void    PauseCapture();

	virtual void	RemovePacketHandler( CPacketHandler inHandler );
	virtual void    ResumeCapture();

	virtual void	SetPacketCount( UInt32 inCount ) { m_nPacketCount = inCount; }
#if (0)
	void			SetPort( UInt16 inPort ) { m_Port = inPort; }
	void			SetPort( const wchar_t* inPort ) { m_Port.Parse( inPort ); }
#endif // 0
	virtual int		StartCapture();
	virtual int		StopCapture();

	void			Terminate();

	// CAdapterStats
	virtual int		GetStatistics( IPacketHandler* pHandler, void** ppStatistics );

	// ThreadHelpers::IThreadFunction
	int				Run( ThreadHelpers::Thread* pThread );

};


// =============================================================================
//		COmniAdapterPtr
// =============================================================================

class COmniAdapterPtr
	:	public std::shared_ptr<COmniAdapter>
{
public:
	;		COmniAdapterPtr( COmniAdapter* inAdapter = nullptr )
		:	std::shared_ptr<COmniAdapter>( inAdapter )
	{}

	;		operator COmniAdapter*() { return get(); }

	bool	IsNotValid() const { return (get() == nullptr); }
	bool	IsValid() const { return (get() != nullptr); }
};
