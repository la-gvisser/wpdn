// =============================================================================
//	PeekAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekContextProxy.h"
#include "PeekProxy.h"
#include "PeekDataModeler.h"
#include <map>


// =============================================================================
//		CPeekAdapter
// =============================================================================

class CPeekAdapter
{
protected:
	bool				m_bCapturing;
	bool				m_bPaused;
	CPeekEngineProxy	m_EngineProxy;

	HeLib::CHeCriticalSection	m_csHandlersMutex;
	CPacketHandlerArray			m_ayPacketHandlers;

public:
	;		CPeekAdapter()
		:	m_bCapturing( false )
		,	m_bPaused( false )
	{
		m_csHandlersMutex.Init();
	}
	virtual ~CPeekAdapter() { m_csHandlersMutex.Term(); }

	virtual void	AddPacketHandler( CPacketHandler inHandler ) {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_csHandlersMutex );
		m_ayPacketHandlers.AddUnique( inHandler );
	}

	void	DoLogMessage( UInt64 inTimeStamp, int inSeverity,
				const CPeekString& inShortMessage, const CPeekString& inLongMessage );

	virtual CAdapterInfo		GetAdapterInfo() const = 0;
	virtual bool				GetAddress( CEthernetAddress& outAddress ) const = 0;
	virtual UInt64				GetLinkSpeed() const = 0;
	virtual tMediaType			GetMediaType() const = 0;
 	virtual const CPeekString&	GetAdapterName() const = 0;

	virtual bool	Initialize( CAdapterInfo /*inInfo*/, CPeekEngineProxy inEngineProxy ) {
		m_bCapturing = false;
		m_bPaused = false;
		m_EngineProxy = inEngineProxy;
		return true;
	}
	virtual bool	IsCapturing() const { return m_bCapturing; }
	virtual bool	IsNotValid() const = 0;
	bool			IsPaused() const { return m_bPaused; }
	virtual bool	IsValid() const = 0;

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
	virtual int		OnUpdateSummaryStats( CSnapshot inSnapshot ) = 0;

	virtual void	RemovePacketHandler( CPacketHandler inHandler ) {
		HeLib::CHeCritSecLock<HeLib::CHeCriticalSection> lock( &m_csHandlersMutex );
		m_ayPacketHandlers.Remove( inHandler );
	}

	void			SetCapturing( bool inState ) { m_bCapturing = inState; }
	void			SetPaused( bool inState ) { m_bPaused = inState; }
	virtual int		StartCapture() = 0;
	virtual int		StopCapture() = 0;
};
