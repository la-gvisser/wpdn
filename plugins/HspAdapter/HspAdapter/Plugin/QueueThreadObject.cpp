// =============================================================================
//	QueueThreadObject.cpp
// =============================================================================
//	Copyright (c) 2017 All rights reserved.

#include "StdAfx.h"
#include "OmniAdapter.h"
#include "QueueThreadObject.h"


// =============================================================================
//	QueueThreadObject
// =============================================================================

#ifdef IMPLEMENT_SUMMARY
const GUID	g_idParentStat =  { 0xA97AAAF4, 0x6AF1, 0x4825, { 0x8E, 0xFE, 0xAE, 0x50, 0x93, 0xC9, 0x61, 0xD2 } };
const GUID	g_idPacketStat =  { 0xC6269A4E, 0x1AAE, 0x42D5, { 0x98, 0xC4, 0x0F, 0xC1, 0x98, 0xAB, 0x08, 0xF5 } };
const GUID	g_idDropStat =    { 0xDD4FB0F7, 0xFE1C, 0x4196, { 0x83, 0xC7, 0x9D, 0x8B, 0x24, 0x30, 0xE4, 0x3B } };

CPeekString		QueueThreadObject::s_strPacketLabel( L"Packets" );
CPeekString		QueueThreadObject::s_strDropLabel( L"Dropped" );
#endif	// IMPLEMENT_SUMMARY

QueueThreadObject::QueueThreadObject()
	: m_pPauseEvent( nullptr )
	, m_pOadap( nullptr )
	, m_bRunning( true )
	, m_InitialPktCount( 0 )
	, m_InitialByteCount( 0 )
	, m_Init_p0_pkt_count( 0 )
	, m_Init_p0_byte_count( 0 )
	, m_Init_p1_pkt_count( 0 )
	, m_Init_p1_byte_count( 0 )
	, m_Init_t0_pkt_count( 0 )
	, m_Init_t0_byte_count( 0 )
	, m_Init_t1_pkt_count( 0 )
	, m_Init_t1_byte_count( 0 )
	, m_filters( 0 )
	, m_pLogMutex( nullptr )
	, m_reader_i( 0 )
	, m_q_reads( 0 )
	, m_Qpkts( 0 )
	, m_Bytes( 0 )
	, m_qDiff( 0 )
	, m_oruns( 0 )
	, m_prime_oruns( 0 )
	, m_iNullRds( 0 )
	, m_u64_oThresh( 0 )
{
	m_QueueEvent.Create( nullptr );
}

QueueThreadObject::QueueThreadObject(
	COmniAdapter*	pOmniAdapter,
	CPacketHandler	PktHandler )
	: m_pPauseEvent( nullptr )
	, m_pOadap( pOmniAdapter )
	, m_PacketHandler( PktHandler )
	, m_bRunning( true )
	, m_InitialPktCount( 0 )
	, m_InitialByteCount( 0 )
	, m_Init_p0_pkt_count( 0 )
	, m_Init_p0_byte_count( 0 )
	, m_Init_p1_pkt_count( 0 )
	, m_Init_p1_byte_count( 0 )
	, m_Init_t0_pkt_count( 0 )
	, m_Init_t0_byte_count( 0 )
	, m_Init_t1_pkt_count( 0 )
	, m_Init_t1_byte_count( 0 )
	, m_filters( 0 )
	, m_pLogMutex( nullptr )
	, m_reader_i( 0 )
	, m_q_reads( 0 )
	, m_Qpkts( 0 )
	, m_Bytes( 0 )
	, m_qDiff( 0 )
	, m_oruns( 0 )
	, m_prime_oruns( 0 )
	, m_iNullRds( 0 )
	, m_u64_oThresh( 0 )
{
	m_QueueEvent.Create( nullptr );
	m_reader_i = pOmniAdapter->GetWriterIndex();
	m_pPauseEvent = pOmniAdapter->GetPauseEvent();
	m_pLogMutex = new HeLib::CHeCriticalSection(); // CMutex( FALSE, L"LogMutex", nullptr );
	m_ayLogMessages.clear();
	m_u64_oThresh = 0;
}

QueueThreadObject::~QueueThreadObject()
{
	m_QueueEvent.Close();

	delete m_pLogMutex;
	m_pLogMutex = NULL;
}


// -----------------------------------------------------------------------------
//		AddLogMsg
// -----------------------------------------------------------------------------

void
QueueThreadObject::AddLogMsg(
	const char* pszMsg )
{
	std::string	strMsg( pszMsg );
	AddLogMsg( strMsg );
}

void
QueueThreadObject::AddLogMsg(
	const std::string&	strMsg )
{
	if ( m_pLogMutex->Lock() ) {	// Lock(2000)
		m_ayLogMessages.push_back( new std::string( strMsg ) );
		m_pLogMutex->Unlock();
	}
}


// -----------------------------------------------------------------------------
//		Init
// -----------------------------------------------------------------------------

HeResult
QueueThreadObject::Init(
	UInt64 pkts, 
	UInt64 bytes, 
	UInt64 in0Pkts, 
	UInt64 in0Bytes, 
	UInt64 in1Pkts, 
	UInt64 in1Bytes,
	UInt64 out0Pkts, 
	UInt64 out0Bytes, 
	UInt64 out1Pkts, 
	UInt64 out1Bytes )
{
	m_q_reads = 0;
	m_Qpkts = 0;
	m_qDiff = 0;
	m_bRunning = true;
	m_oruns = 0;
    m_prime_oruns = 0;

	m_iNullRds = 0;
	SetInitialPktCount( pkts );
	SetInitialByteCount( bytes );
	SetInput0InitStatPkts( in0Pkts );
	SetInput0InitStatBytes( in0Bytes );
	SetInput1InitStatPks( in1Pkts );
	SetInput1InitStatBytes( in1Bytes );

	SetOutputT0InitStatPkts( out0Pkts );
	SetOutputT0InitStatBytes( out0Bytes );
	SetOutputT1InitStatPks( out1Pkts );
	SetOutputT1InitStatBytes( out1Bytes );

#ifdef IMPLEMENT_SUMMARY
	m_ParentStat = g_idParentStat;

	m_Stats.PacketStat.Id = g_idPacketStat;
	m_Stats.PacketStat.Stat.Type = Peek::kSummaryStatType_Packets;
	m_Stats.PacketStat.Stat.Flags = 0;
	m_Stats.PacketStat.Stat.Value.Packets = 0;

	m_Stats.DropStat.Id = g_idDropStat;
	m_Stats.DropStat.Stat.Type = Peek::kSummaryStatType_Packets;
	m_Stats.DropStat.Stat.Flags = 0;
	m_Stats.DropStat.Stat.Value.Packets = 0;
#endif	// IMPLEMENT_SUMMARY

	m_u64_oThresh = 0;
	//
	// TEST CODE ONLY: REMOVE FOR RELEASE
	//std::string stest("QueueThreadObject::Init() routine");
	//AddLogMsg(stest);
	// TEST CODE ONLY: REMOVE FOR RELEASE
	//
	return m_spPacket.CreateInstance( "PeekCore.Packet" );
}


// -----------------------------------------------------------------------------
//		OnUpdateSummaryStats
// -----------------------------------------------------------------------------

int
QueueThreadObject::OnUpdateSummaryStats(
	CSnapshot	inSnapshot )
{
	if ( !inSnapshot.HasItem( m_ParentStat ) ) {
		Peek::SummaryStat	statGroup;
		statGroup.Type = Peek::SummaryStatType::kSummaryStatType_Null;
		statGroup.Flags = Peek::SummaryStatFlags::kSummaryStats_SortSubItems;
		statGroup.Value.IntValue = 0;
		inSnapshot.SetItem( m_ParentStat, GUID_NULL, GetAdapterName(), &statGroup );
	}
	if ( !inSnapshot.HasItem( m_Stats.PacketStat.Id ) ) {
		inSnapshot.SetItem( m_Stats.PacketStat.Id, m_ParentStat, s_strPacketLabel, &m_Stats.PacketStat.Stat );
	}
	if ( !inSnapshot.HasItem( m_Stats.DropStat.Id ) ) {
		inSnapshot.SetItem( m_Stats.DropStat.Id, m_ParentStat, s_strDropLabel, &m_Stats.DropStat.Stat );
	}

	m_Stats.DropStat.Stat.Value.IntValue = 0; // m_nDroppedPackets;

	inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.PacketStat );
	inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.DropStat );

	return PEEK_PLUGIN_SUCCESS;
}
