// =============================================================================
//	PeekAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "GlobalId.h"
#include "IpHeaders.h"
#include "PeekProxy.h"
#include "peekcore.h"
#include <map>

class CRemoteAdapter;


// =============================================================================
//		CIdIpMap
// =============================================================================

class CIdIpMap
	:	public std::map<CGlobalId, CIpAddress>
{
public:
	;		CIdIpMap() {}
	;		~CIdIpMap() {}

	void	Add( const CGlobalId inId, const CIpAddress& inAddress );

	bool	Find( const CGlobalId inId, CIpAddress& outAddress ) const;

	void	Remove( const CGlobalId inId );
};


// =============================================================================
//		CSafeIdIpMap
// =============================================================================

class CSafeIdIpMap
	:	public CIdIpMap
{
protected:
	mutable HeLib::CHeCriticalSection	m_CritSection;

public:
	;		CSafeIdIpMap()
	{
		m_CritSection.Init();
	}
	;		~CSafeIdIpMap()
	{
		m_CritSection.Term();
	}

	void	Add( const CGlobalId inId, const CIpAddress& inAddress ) {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_CritSection );
		CIdIpMap::Add( inId, inAddress );
	}

	bool	Find( const CGlobalId inId, CIpAddress& outAddress ) const {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_CritSection );
		return CIdIpMap::Find( inId, outAddress );
	}

	void	Remove( const CGlobalId inId ) {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_CritSection );
		CIdIpMap::Remove( inId );
	}
};


// =============================================================================
//		CPeekAdapter
// =============================================================================

class CPeekAdapter
{
protected:
	CPeekAdapterProxy	m_PeekAdapterProxy;
	CRemoteAdapter&		m_RemoteAdapter;

	bool					m_bCapturing;

	CPeekCriticalSection	m_Lock;
	CPacketHandlerArray		m_ayPacketHandlers;
	CIdIpMap				m_CaptureIdToIpAddress;

public:
	;		CPeekAdapter( CRemoteAdapter* inRemoteAdapter )
		:	m_RemoteAdapter( *inRemoteAdapter )
		,	m_bCapturing( false )
	{}
	virtual ~CPeekAdapter() {}

	virtual void	AddPacketHandler( CPacketHandler inHandler ) = 0;

	virtual void	CopyAdapterInfo( IAdapterInfo** outAdapterInfo ) = 0;

	virtual CAdapterInfo	GetAdapterInfo() const = 0;
	virtual const CPeekString&	GetAdapterName() const = 0;
	virtual bool			GetAddress( CEthernetAddress& outAddress ) const = 0;
	virtual UInt64			GetLinkSpeed() const = 0;
	virtual tMediaType		GetMediaType() const = 0;

	virtual int		HandlePacket( const CPacket& inPacket, const CIpAddress& inAddress );

	bool			IsCapturing() const { return m_bCapturing; }

	virtual void    PauseCapture() = 0;

	virtual void	RemovePacketHandler( CPacketHandler inHandler ) = 0;
	virtual void    ResumeCapture() = 0;

	void			SetCapturing( bool inState ) { m_bCapturing = inState; }
	void			SetInterfacePointers( IHeUnknown* inSite ) {
		if ( inSite ) {
			m_PeekAdapterProxy.SetInterfacePointers( inSite );
		}
	}
	virtual void	SetPacketCount( UInt32 inCount ) = 0;
	virtual int		StartCapture() = 0;
	virtual int		StopCapture() = 0;
};
