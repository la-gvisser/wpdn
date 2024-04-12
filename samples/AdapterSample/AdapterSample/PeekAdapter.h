// =============================================================================
//	PeekAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekContextProxy.h"
#include <map>


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
	bool				m_bCapturing;
	bool				m_bPaused;

	HeLib::CHeCriticalSection	m_CritSection;
	CPacketHandlerArray			m_ayPacketHandlers;
	CIdIpMap					m_CaptureIdToIpAddress;

public:
	;		CPeekAdapter()
		:	m_bCapturing( false )
		,	m_bPaused( false )
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
	bool			IsPaused() const { return m_bPaused; }

	virtual void	RemovePacketHandler( CPacketHandler inHandler ) = 0;

	void			SetCapturing( bool inState ) { m_bCapturing = inState; }
	virtual void	SetDataPath( const CPeekString& inPath ) = 0;
	void			SetPaused( bool inState ) { m_bPaused = inState; }
	virtual void	SetPacketCount( UInt32 inCount ) = 0;
	virtual int		StartCapture() = 0;
	virtual int		StopCapture() = 0;
};
