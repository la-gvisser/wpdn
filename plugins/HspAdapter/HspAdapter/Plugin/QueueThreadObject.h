// =============================================================================
//	QueueThreadObject.h
// =============================================================================
//	Copyright (c) 2017. All rights reserved.

#pragma once

#include "PeekAdapter.h"
#include "PeekContextProxy.h"
#include "GlobalId.h"
#include "FileEx.h"
#include "Options.h"
#include "threadhelpers.h"
#include "hlsync.h"
#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <thread>
#include <vector>

using namespace ThreadHelpers;

#define MAX_NULL_READS 10

class COmniAdapter;

// =============================================================================
//		QueueThreadObject
// =============================================================================

class QueueThreadObject
{
#ifdef IMPLEMENT_SUMMARY
public:
	// Statistics
	struct OmniSummaryStats {
		Peek::SummaryStatEntry	PacketStat;
		Peek::SummaryStatEntry	DropStat;
	};

	static CPeekString		s_strPacketLabel;
	static CPeekString		s_strDropLabel;
#endif	// IMPLEMENT_SUMMARY

private:
	CHePtr<IPacket>		m_spPacket;
	Event*				m_pPauseEvent;
	Event				m_QueueEvent;
	COmniAdapter*		m_pOadap;
	std::unique_ptr<std::thread>	m_upThread;
	CPacketHandler		m_PacketHandler;
	std::atomic_bool	m_bRunning;

#ifdef IMPLEMENT_SUMMARY
	CGlobalId			m_ParentStat;
	OmniSummaryStats	m_Stats;
#endif	// IMPLEMENT_SUMMARY

	UInt64			m_InitialPktCount;
	UInt64			m_InitialByteCount;

	UInt64			m_Init_p0_pkt_count;
	UInt64			m_Init_p0_byte_count;
	UInt64			m_Init_p1_pkt_count;
	UInt64			m_Init_p1_byte_count;

	UInt64			m_Init_t0_pkt_count;
	UInt64			m_Init_t0_byte_count;
	UInt64			m_Init_t1_pkt_count;
	UInt64			m_Init_t1_byte_count;

	UInt32			m_filters;

	std::vector<std::string*>	m_ayLogMessages;
	std::string					m_strLog;
	HeLib::CHeCriticalSection*	m_pLogMutex;

public:
	int				m_reader_i;
	UInt64			m_q_reads;
	UInt64			m_Qpkts;
	UInt64			m_Bytes;
	UInt64			m_qDiff;
	UInt32			m_oruns;
	UInt32			m_prime_oruns;
	int				m_iNullRds;
	UInt64			m_u64_oThresh;
	PeekPacket		m_thePeekPacket;

public:
	QueueThreadObject();
	QueueThreadObject( COmniAdapter *poa, CPacketHandler hndlr );
	~QueueThreadObject();

	HeResult Init( UInt64 pkts, UInt64 bytes, UInt64 in0Pkts, UInt64 in0Bytes, 
		UInt64 in1Pkts, UInt64 in1Bytes, UInt64 out0Pkts, UInt64 out0Bytes,
		UInt64 out1Pkts, UInt64 out1Bytes );

	COmniAdapter*				GetAdapterPtr() { return m_pOadap; }
	HeLib::CHeCriticalSection*	GetMutexPtr() { return m_pLogMutex; }
	CHePtr<IPacket>				GetOmniSPacket() { return m_spPacket; }
	CPacketHandler				GetPacketHandler() { return m_PacketHandler; }

	int			OnUpdateSummaryStats( CSnapshot inSnapshot );

	Event*		GetPauseEvent() { return m_pPauseEvent; }
	Event*		GetQueueEvent() { return &m_QueueEvent; }

	bool		IsRunning() const { return m_bRunning; }
	void		StopRunning() { m_bRunning = false; }

	void		SetThread( std::unique_ptr<std::thread>& upThread ) { m_upThread = std::move( upThread ); }
	void		GetThread( std::unique_ptr<std::thread>& upThread ) { upThread = std::move( m_upThread ); }

	void		SetInitialByteCount( UInt64 ibc ) { m_InitialByteCount = ibc; }
	UInt64		GetInitialPktCount() { return m_InitialPktCount; }

	void		SetInitialPktCount( UInt64 ipc ) { m_InitialPktCount = ipc; }
	UInt64		GetInitialByteCount() { return m_InitialByteCount; }

	void		SetInput0InitStatPkts( UInt64 inPkts ) { m_Init_p0_pkt_count = inPkts; }
	void		SetInput0InitStatBytes( UInt64 inBytes ) { m_Init_p0_byte_count = inBytes; }
	void		SetInput1InitStatPks( UInt64 inPkts ) { m_Init_p1_pkt_count = inPkts; }
	void		SetInput1InitStatBytes( UInt64 inBytes ) { m_Init_p1_byte_count = inBytes; }
	UInt64		GetInput0InitStatPkts() { return m_Init_p0_pkt_count; }
	UInt64		GetInput0InitStatBytes() { return m_Init_p0_byte_count; }
	UInt64		GetInput1InitStatPkts() { return m_Init_p1_pkt_count; }
	UInt64		GetInput1InitStatBytes() { return m_Init_p1_byte_count; }

	void		SetOutputT0InitStatPkts( UInt64 inPkts ) { m_Init_t0_pkt_count = inPkts; }
	void		SetOutputT0InitStatBytes( UInt64 inBytes ) { m_Init_t0_byte_count = inBytes; }
	void		SetOutputT1InitStatPks( UInt64 inPkts ) { m_Init_t1_pkt_count = inPkts; }
	void		SetOutputT1InitStatBytes( UInt64 inBytes ) { m_Init_t1_byte_count = inBytes; }
	UInt64		GetOutputT0InitStatPkts() { return m_Init_t0_pkt_count; }
	UInt64		GetOutputT0InitStatBytes() { return m_Init_t0_byte_count; }
	UInt64		GetOutputT1InitStatPkts() { return m_Init_t1_pkt_count; }
	UInt64		GetOutputT1InitStatBytes() { return m_Init_t1_byte_count; }

	void		AddLogMsg( const char* pszMsg );
	void		AddLogMsg( const std::string& strMsg );
	std::vector<std::string*>*	GetLog() { return &m_ayLogMessages; }
};
