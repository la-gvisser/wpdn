// =============================================================================
//	AdapterStats.h
// =============================================================================
//	Copyright (c) 2014 WildPackets, Inc. All rights reserved.

#ifndef ADAPTERSTATS_H
#define ADAPTERSTATS_H

#include "heunk.h"
#include "heid.h"
#include "hecom.h"
#include "peekcore.h"
#include "PeekContextProxy.h"


// =============================================================================
//		IAdapterStats
// =============================================================================

#define IAdapterStats_IID \
{ 0xDA6B1D89, 0xF944, 0x49B1, {0xAD, 0xB9, 0xA6, 0xEE, 0x17, 0xBA, 0xB7, 0xB3} }

class HE_NO_VTABLE IAdapterStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAdapterStats_IID);

	/// Get the adapter statistics.
	HE_IMETHOD GetStatistics(/*in*/ IPacketHandler* pHandler, /*out*/ void** ppStatistics) = 0;
};


// =============================================================================
//		CAdapterStats
// =============================================================================

class CAdapterStats
{
public:
	virtual int		GetStatistics( IPacketHandler* pPacketHandler, void** ppStatistics ) = 0;
};


#ifdef DO_SUMMARY
// =============================================================================
//		CAdapterStatsProxy
// =============================================================================

class CAdapterStatsProxy
{
	friend class COmniAdapter;

protected:
	CHePtr<IAdapterStats>	m_spAdapterStats;

protected:
	IAdapterStats*		GetPtr() { return m_spAdapterStats; }
	void				ReleasePtr() { m_spAdapterStats.Release(); }
	void				SetPtr( IAdapterStats* inAdapterStats ) {
		m_spAdapterStats = inAdapterStats;
	}

public:
	;	CAdapterStatsProxy( CAdapter inAdapter )
		:	m_spAdapterStats( NULL )
	{
		if ( inAdapter.IsValid() ) {
			CHePtr<IAdapter>		spAdapter = inAdapter.GetPtr();
			CHePtr<IAdapterStats>	spAdapterStats;
			if ( HE_SUCCEEDED( spAdapter->QueryInterface( &spAdapterStats.p ) ) ) {
				SetPtr( spAdapterStats );
			}
		}
	}
	;	~CAdapterStatsProxy() {}

	int	GetStatistics( CPacketHandler& PacketHandler, void** ppStatistics ) {
		CHePtr<IPacketHandler>	handler = PacketHandler.GetPtr();
		if ( handler != NULL ) {
			return m_spAdapterStats->GetStatistics( handler, ppStatistics );
		}
		return HE_E_NO_INTERFACE;
	}

	bool	IsValid() const { return (m_spAdapterStats != NULL); }
};
#endif // DO_SUMMARY

#endif // ADAPTERSTATS_H
