// =============================================================================
//	HspAdapterStats.h
// =============================================================================
//	Copyright (c) 2012. All rights reserved.

#pragma once

#include "PeekContextProxy.h"
#include "heunk.h"
// #include "afxmt.h"
#include "QueueThreadObject.h"

#define HSP_STAT_COUNT 42

// Status Message Offsets
#define HSP_STAT_MSG_OFFSET_LEN				(0 * sizeof( UInt32 )) 
#define HSP_STAT_MSG_OFFSET_SEQ_NUM			(1 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_MTYPE			(2 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_SW_OS_TYPE		(3 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_SW_KRN_VER		(4 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_EM_DRV_VER		(5 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_EM_APP_VER		(6 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_EM_RFS_VER		(7 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_FPGA_VER		(8 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P0_FRMS_LO		(9 * sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P0_FRMS_HI		(10* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P0_BYTES_LO		(11* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P0_BYTES_HI		(12* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P0_ERROR		(13* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_CHAN_STAT		(14* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P1_FRMS_LO		(15* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P1_FRMS_HI		(16* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P1_BYTES_LO		(17* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P1_BYTES_HI		(18* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_P1_ERROR		(19* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_SENT_FRM_LO		(20* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_SENT_FRM_HI		(21* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_SENT_BYT_LO		(22* sizeof( UInt32 ))
#define HSP_STAT_MSG_OFFSET_SENT_BYT_HI		(23* sizeof( UInt32 ))

class CHspHandlerStats;

// =============================================================================
//		CHspAdapterStats
// =============================================================================

#define IHspAdapterStats_IID \
	{ 0xF01DAAAA, 0xF2C4, 0x45A9, { 0x80, 0x9D, 0x2B, 0x8C, 0x73, 0x3C, 0x21, 0x09 } }

class HE_NO_VTABLE IHspAdapterStats
	: public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IHspAdapterStats_IID);

	/// Get the number of statistics supported.
	HE_IMETHOD GetStatCount(/*out*/ UInt32* nCount) = 0;
	/// Get a Summary Statistics value by index.
	HE_IMETHOD GetStat(/*in*/ UInt32 nIndex, /*out*/ UInt64* pnCount) = 0;
	/// Get statistics specific to each thread feeding packets to a handler.
	HE_IMETHOD GetHandlerStats(/*in*/CPacketHandler &hndlr, /*in/out*/ CHspHandlerStats &hstat) = 0;
	/// Get QueueThreadObject.
	HE_IMETHOD GetQueueThreadObject(/*in*/CPacketHandler &hndlr, /*in/out*/ QueueThreadObject **ppqto) = 0;
};

enum HspAdapterStats
{
	hspAdapterStasNone,				///< undefined.
	hspAdapterStasByteCount,		///< Number of bytes forwarded.
	hspAdapterStasPacketCount,		///< Number of packets forwarded.
	hspAdapterStasOruns,			///< Number of full queue buffers to process
	hspAdapterStasQpkts,			///< Number of packets to queue to Omni
//	hspAdapterStasCrcErrors,		///< Number of crc errors
//	HSP Status Message Stats
	hspAdapterStasP0PktCount,		///< Packets seen by input 0.
	hspAdapterStasP0ByteCount,		///< Bytes seen by input 0.
	hspAdapterStasP1PktCount,		///< Packets seen by input 1.
	hspAdapterStasP1ByteCount,		///< Bytes seen by input 1.
	hspAdapterStasTx0PktCount,		///< Same as hspAdapterStasPacketCount stat above
	hspAdapterStasTx0ByteCount,		///< Same as hspAdapterStasByteCount stat above
	hspAdapterStasTx1PktCount,
	hspAdapterStasTx1ByteCount,
	hspAdapterStasP0CrcErrors,		///< Bad frames seen by input 0
	hspAdapterStasP1CrcErrors,		///< Bad frames seen by input 1
	hspAdapterStasRxStatusBit7,		///< Rx Status for inputs 0/1:
	hspAdapterStasRxStatusBit6,		///< 
	hspAdapterStasRxStatusBit5,			///< Bit 7: Xfp/sfp missing port 1  Bit 3: Loss-of-sync port 1
	hspAdapterStasRxStatusBit4,			///< Bit 6: Xfp/sfp missing port 0  Bit 2: Loss-of-sync port 0
	hspAdapterStasRxStatusBit3,			///< Bit 5: Loss-of-signal port 1   Bit 1: Other error port 1
	hspAdapterStasRxStatusBit2,			///< Bit 4: Loss-of-signal port 0   Bit 0: Other error port 0
	hspAdapterStasRxStatusBit1,		///<
	hspAdapterStasRxStatusBit0,
	hspAdapterStasPhy1Rate,			///< Connection Rate (Always 1 Gbps)
	hspAdapterStasActiveSessions,	///< Active Sessions
	hspAdapterStasBoardType,		///< Board Type
	hspAdapterStasBoardRevMaj,		///< Board Revision Major
	hspAdapterStasBoardRevMin,		///< Board Revision Minor
	hspAdapterStasOverflows,		///< Overflows
	hspAdapterStasDspSwRevMaj,		///< Dsp Sw Revision Major
	hspAdapterStasDspSwRevMin,		///< Dsp Sw Revision Minor
	hspAdapterStasDspSwRevSub,		///< Dsp Sw Revision Sub
	hspAdapterStasFilterCapBit7,	///< Supported filter types
	hspAdapterStasFilterCapBit6,		///< Bit 7:   Bit 3: 
	hspAdapterStasFilterCapBit5,		///< Bit 6:   Bit 2:
	hspAdapterStasFilterCapBit4,		///< Bit 5:   Bit 1:
	hspAdapterStasFilterCapBit3,		///< Bit 4:   Bit 0:
	hspAdapterStasFilterCapBit2,
	hspAdapterStasFilterCapBit0,
	hspAdapterStasFpgaRev,
	hspAdapterStasMaxIpSessions,
	hspAdapterStasMaxStaticIps,
	hspAdapterStasMaxIpRanges,
	hspAdapterStasMaxStrings,
	hspAdapterStasMaxPorts,
	hspAdapterStasDspSwRevRelease,
	hspAdapterStasIpsCurrent,
	hspAdapterStasRangesCurrent,
	hspAdapterStasStringsCurrent,
	hspAdapterStasPortsCurrent,
	hspAdapterStasP0LinkSpeed,
	hspAdapterStasP1LinkSpeed,
	hspAdapterStasCoProcSwRevOs,
	hspAdapterStasCoProcSwRevKv,
	hspAdapterStasCoProcSwRevDv,
	hspAdapterStasCoProcSwRevAv,
	hspAdapterStasCoProcSwRevRv
};

// =============================================================================
//		CHspHandlerStats
// =============================================================================

class HE_NO_VTABLE CHspHandlerStats
{
private:
	UInt64	m_inBrdQBytes;
	UInt64	m_inBrdQPackets;
	UInt32	m_inBrdQOruns;
	UInt64	m_outOmniBytes;
	UInt64	m_outOmniPackets;
	UInt64	m_initBrdQPkts;
	UInt64	m_initBrdQBytes;
	UInt64	m_initBrd0Pkts;
	UInt64	m_initBrd0Bytes;
	UInt64	m_initBrd1Pkts;
	UInt64	m_initBrd1Bytes;
	UInt64	m_initT0Pkts;
	UInt64	m_initT0Bytes;
	UInt64	m_initT1Pkts;
	UInt64	m_initT1Bytes;
	std::vector<std::string *>*	m_log;
	HeLib::CHeCriticalSection*	m_pLogMutex;
	UInt64	m_u64_Thresh;

public:
	CHspHandlerStats()
		:	m_inBrdQBytes( 0 )
		,	m_inBrdQPackets( 0 )
		,	m_inBrdQOruns( 0 )
		,	m_outOmniBytes( 0 )
		,	m_outOmniPackets( 0 )
		,	m_initBrdQPkts( 0 )
		,	m_initBrdQBytes( 0 )
		,	m_initBrd0Pkts( 0 )
		,	m_initBrd0Bytes( 0 )
		,	m_initBrd1Pkts( 0 )
		,	m_initBrd1Bytes( 0 )
		,	m_initT0Pkts( 0 )
		,	m_initT0Bytes( 0 )
		,	m_initT1Pkts( 0 )
		,	m_initT1Bytes( 0 )
		,	m_log( nullptr )
		,	m_pLogMutex( nullptr )
		,	m_u64_Thresh( 0 )
	{}

	bool operator== (void *pinPtr) {return ((void*)this == (void*)pinPtr);}

	UInt64	GetBrdQBytes() { return m_inBrdQBytes; }
	UInt64	GetBrdQPkts() { return m_inBrdQPackets; }
	UInt32	GetOverruns() { return m_inBrdQOruns; }
	UInt64	GetOmniQPkts() { return m_outOmniPackets; }
	UInt64	GetOmniQBytes() { return m_outOmniBytes; }
	UInt64	GetBrdQInitPktCount() { return m_initBrdQPkts; }
	UInt64	GetBrdQInitBytesCount() { return m_initBrdQBytes; }

	UInt64	GetBrdInput0PktCount() { return m_initBrd0Pkts; }
	UInt64	GetBrdInput0ByteCount() { return m_initBrd0Bytes; }
	UInt64	GetBrdInput1PktCount() { return m_initBrd1Pkts; }
	UInt64	GetBrdInput1ByteCount() { return m_initBrd1Bytes; }

	UInt64	GetBrdOutputT0PktCount() { return m_initT0Pkts; }
	UInt64	GetBrdOutputT0ByteCount() { return m_initT0Bytes; }
	UInt64	GetBrdOutputT1PktCount() { return m_initT1Pkts; }
	UInt64	GetBrdOutputT1ByteCount() { return m_initT1Bytes; }

	void	SetBrdInput0PktCount( UInt64 inPkts ) { m_initBrd0Pkts = inPkts; }
	void	SetBrdInput0ByteCount( UInt64 inBytes ) { m_initBrd0Bytes = inBytes; }
	void	SetBrdInput1PktCount( UInt64 inPkts ) { m_initBrd1Pkts = inPkts; }
	void	SetBrdInput1ByteCount( UInt64 inBytes ) { m_initBrd1Bytes = inBytes; }

	void	SetBrdOutputT0PktCount( UInt64 inPkts ) { m_initT0Pkts = inPkts; }
	void	SetBrdOutputT0ByteCount( UInt64 inBytes ) { m_initT0Bytes = inBytes; }
	void	SetBrdOutputT1PktCount( UInt64 inPkts ) { m_initT1Pkts = inPkts; }
	void	SetBrdOutputT1ByteCount( UInt64 inBytes ) { m_initT1Bytes = inBytes; }

	void	SetBrdQBytes( UInt64 brdBytes ) { m_inBrdQBytes = brdBytes; }
	void	SetBrdQPkts( UInt64 brdPkts ) { m_inBrdQPackets = brdPkts; }
	void	SetOverruns( UInt32 oruns ) { m_inBrdQOruns = oruns; }
	void	SetOmniQPkts( UInt64 oQPkts ) { m_outOmniPackets = oQPkts; }
	void	SetOmniQBytes( UInt64 oQBytes ) { m_outOmniBytes = oQBytes; }
	void	SetBrdQInitPktCount( UInt64 pktCount ) { m_initBrdQPkts = pktCount; }
	void	SetBrdQInitByteCount( UInt64 byteCount ) { m_initBrdQBytes = byteCount; }
	void	SetLogPtr( std::vector<std::string*>* il ) { m_log = il; }

	std::vector<std::string*>*	GetLogPtr() { return m_log; }
	void	SetMutexPtr( HeLib::CHeCriticalSection* pm ) {  m_pLogMutex = pm; }
	HeLib::CHeCriticalSection*	GetMutexPtr() { return m_pLogMutex; }
	void	SetThreshold( UInt64 thr ) { m_u64_Thresh = thr; }
	UInt64	GetThreshold() { return m_u64_Thresh; }

	void Clear() {
		m_inBrdQBytes = 0;
		m_inBrdQPackets = 0;
		m_outOmniPackets = 0;
		m_outOmniBytes = 0;
		m_initBrdQPkts = 0;
		m_initBrdQBytes = 0;
		m_initBrd0Pkts = 0;
		m_initBrd0Bytes = 0;
		m_initBrd1Pkts = 0;
		m_initBrd1Bytes = 0;
		m_initT0Pkts = 0;
		m_initT0Bytes = 0;
		m_initT1Pkts = 0;
		m_initT1Bytes = 0;
		m_inBrdQOruns = 0;
	}
};

// =============================================================================
//		CHspAdapterStats
// =============================================================================

class CHspAdapterStats
{
protected:
	CHePtr<IHspAdapterStats>	m_spHspAdapterStats;

protected:
	IHspAdapterStats*	GetPtr() { return m_spHspAdapterStats; }
	void				ReleasePtr() { m_spHspAdapterStats.Release(); }
	void				SetPtr( IHspAdapterStats* inStats ) {
		m_spHspAdapterStats = inStats;
	}

public:
	;		CHspAdapterStats( IHspAdapterStats* inStats = nullptr )
						: m_spHspAdapterStats( inStats ) {}
	;		CHspAdapterStats( CAdapter inAdapter ) {
		HeLib::CHeQIPtr<IHspAdapterStats>	spStats( inAdapter.GetPtr() );
		SetPtr( spStats );
	}
	;		~CHspAdapterStats() {}

	bool	IsNotValid() const { return (m_spHspAdapterStats == nullptr); }
	bool	IsValid() const { return (m_spHspAdapterStats != nullptr); }

	void	GetHandlerStats( CPacketHandler &hndlr, CHspHandlerStats &hstat ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spHspAdapterStats->GetHandlerStats( hndlr, hstat ) );
		}
	}
	bool	GetQueueThreadObject( CPacketHandler &hndlr, QueueThreadObject **pphqto ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spHspAdapterStats->GetQueueThreadObject( hndlr, pphqto ) );
			if ( *pphqto == nullptr ) return false;
		}
		return true;
	}
	UInt64	GetStat( UInt32 inIndex ) {
		UInt64	nValue( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spHspAdapterStats->GetStat( inIndex, &nValue ) );
		}
		return nValue;
	}
	UInt32	GetStatCount() {
		UInt32	nCount( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spHspAdapterStats->GetStatCount( &nCount ) );
		}
		return nCount;
	}
};
