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
	bool						m_bCapturing;
	bool						m_bPaused;
	CPeekEngineProxy			m_EngineProxy;
	HeLib::CHeCriticalSection	m_csHandlersMutex;
	CPacketHandlerArray			m_ayPacketHandlers;

#ifdef IMPLEMENT_NAVL
	bool					m_bNavlSupport;
	CPacketHandlerArray		m_ayAddHandlers;	
	CPacketHandlerArray		m_ayRemoveHandlers;
#endif	// IMPLEMENT_NAVL

public:
	;		CPeekAdapter()
		:	m_bCapturing( false )
		,	m_bPaused( false )
#ifdef IMPLEMENT_NAVL
		,	m_bNavlSupport( false )
#endif	// IMPLEMENT_NAVL
	{
		m_csHandlersMutex.Init();
	}
	virtual ~CPeekAdapter() { m_csHandlersMutex.Term(); }

	void			DoLogMessage( UInt64 inTimeStamp, int inSeverity,
				const CPeekString& inShortMessage, const CPeekString& inLongMessage );

	virtual CAdapterInfo		GetAdapterInfo() const = 0;
	virtual bool				GetAddress( CEthernetAddress& outAddress ) const = 0;
	virtual UInt64				GetLinkSpeed() const = 0;
	virtual tMediaType			GetMediaType() const = 0;
 	virtual const CPeekString&	GetAdapterName() const = 0;

	virtual bool	Initialize( CAdapterInfo /*inInfo*/, const CPeekEngineProxy& inEngineProxy ) {
		m_bCapturing = false;
		m_bPaused = false;
		m_EngineProxy = inEngineProxy;
		return true;
	}
	bool			IsCapturing() const { return m_bCapturing; }
	virtual bool	IsNotValid() const = 0;
	bool			IsPaused() const { return m_bPaused; }
	virtual bool	IsValid() const = 0;

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs ) = 0;
#ifdef IMPLEMENT_SUMMARY
	virtual int		OnUpdateSummaryStats( CSnapshot inSnapshot, void* inContext ) = 0;
#endif	// IMPLEMENT_SUMMARY

	void			SetCapturing( bool inState ) { m_bCapturing = inState; }
	void			SetPaused( bool inState ) { m_bPaused = inState; }
	virtual int		StartCapture() = 0;
	virtual int		StopCapture() = 0;

#ifdef IMPLEMENT_NAVL
	virtual void	AddPacketHandler( CPacketHandler inHandler );
	bool			DoPacketHandlers();
	virtual void	RemovePacketHandler( CPacketHandler inHandler );
#endif	// IMPLEMENT_NAVL

};
