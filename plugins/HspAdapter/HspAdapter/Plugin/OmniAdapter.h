// =============================================================================
//	OmniAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekAdapter.h"
#include "PeekContextProxy.h"
#include "AdapterOptions.h"
#include "Crc32.h"
#include "GlobalId.h"
#include "FileEx.h"
#include "HspAdapterStats.h"
#include "threadhelpers.h"
#include <atomic>
#include <map>
#include <string>
#include <thread>
#include <vector>

#ifdef __GLIBC__
#include <sys/ioctl.h>
#endif

using namespace ThreadHelpers;

#define NUM_WAIT_EVENTS				2
#define ETHERNET_DELIMITER_SIZE		8
#define ETHERNET_SMALLEST_PKT_SIZE	58
#define DMA_XFER_BUFFER_SIZE		(64 * 1048576)
#define DMA_XFER_BUFFER_COUNT		16
#define DMA_XFER_BUFFER_TOTAL_SIZE	(DMA_XFER_BUFFER_COUNT * DMA_XFER_BUFFER_SIZE)
#define DMA_RA_TIMEOUT				60000
			// sizeof(PVOID) equates to 4 bytes inside OmniPeek instead of normal 8 bytes within Windows 7 OS, 
#define DMA_PTRS_SIZE				(DMA_XFER_BUFFER_COUNT * sizeof(void*)) 
#define ETHERNET_HDR_SIZE			14
#define ETHERTYPE_OFFSET			14
			// sizeof(PVOID) equates to 4 bytes inside OmniPeek instead of normal 8 bytes within Windows 7 OS, 
#define DMA_STAT_PTR_SIZE			sizeof(void*) // 8
#define DMA_XFER_MAX_MSG_SIZE		4096

const UInt32 CRC32_POLYNOMIAL = 0xEDB88320;

#define NUM_PKT_PTRS				(131072 * 32)
//#define NUM_PKT_PTRS				(131072 * 8)
#ifndef UInt64_MAX
#define UInt64_MAX					18446744073709551615
#endif	// UInt64_MAX
//HANDLE hExitAndFwdHandles[NUM_WAIT_EVENTS];
//HANDLE hReadEvent, hWriteAckEvent, hFwdEvent;

enum ePktPartTypes {
	PART_TYPE_NONE,
	PART_TYPE_DELIM_FULL,
	PART_TYPE_DELIM_PART,
	PART_TYPE_BODY,
	PART_TYPE_CRC,
	PART_TYPE_LEN,
	PART_TYPE_ETHERTYPE,
	PART_HSP_PKT_BODY
};

enum eEventTypes {
	EVT_READ = 0,
	EVT_WRITE,
	EVT_FWD,
	EVT_MAX
};

typedef struct _bdry_struct {
	byte*	m_pPkt;
	int		size;
	int		offset;
	int		holdIndex;
	int		preBufBytes;
	ePktPartTypes	type;
} BDRY_STRUCT, *PBDRY_STRUCT;

typedef struct _pkt_struct {
	UInt8*	m_pPkt;
	int		size;
} PKT_STRUCT, *PPKT_STRUCT;

struct IPacketHandlerComparator {
	// Compare the address of the handler.
	bool operator() (
			const IPacketHandler*	pLeft,
			const IPacketHandler*	pRight ) const
	{
		return pLeft > pRight;
	}
};

extern const CPeekString& GetAdapterName();


// =============================================================================
//		COmniAdapter
// =============================================================================

class COmniAdapter
	:	public CPeekAdapter
{
public:
	static CAdapterInfo	CreateAdapterInfo( const CPeekString& inDescription,
		const CGlobalId& inId );

protected:
	static CPeekStringX		m_ayEventNames[EVT_MAX];
// #ifdef IMPLEMENT_SUMMARY
// 	static CPeekString		s_strPacketLabel;
// 	static CPeekString		s_strDropLabel;
// #endif	// IMPLEMENT_SUMMARY

protected:
	CGlobalId			m_Id;
	CPluginAdapterInfo	m_AdapterInfo;
	CPeekString			m_strName;
	CChannelManager		m_ChannelManager;
	UInt64				m_nDroppedPackets;

#ifdef IMPLEMENT_SUMMARY
// 	// Statistics
// 	struct OmniSummaryStats {
// 		Peek::SummaryStatEntry	PacketStat;
// 		Peek::SummaryStatEntry	DropStat;
// 	};
// 	CGlobalId					m_ParentStat;
// 	OmniSummaryStats			m_Stats;
#endif	// IMPLEMENT_SUMMARY

	// HSP Adapter
	std::thread			m_CaptureThread;
	std::thread			m_StatusThread;
	Event				m_StopEvent;
	Event				m_HspReadEvent;
	Event				m_HspForwardEvent;
 	EventPtrList		m_ayExitAndReadEvents;
 	EventPtrList		m_ayExitAndFwdEvents;

	mutable std::atomic_size_t	m_nRunningCaptures;
	CPacketHandlerArray	m_spPacketHandler;

	UInt32				m_HeaderBytes;
	UInt8*				m_DataBufs[DMA_XFER_BUFFER_COUNT];
	UInt8				m_MapPtrs[DMA_PTRS_SIZE];			// void*	m_MapPtrs[DMA_XFER_BUFFER_COUNT];
	UInt8				m_MapStatPtr[DMA_STAT_PTR_SIZE];	// void*	m_MapStatPtr[DMA_XFER_BUFFER_COUNT];
	UInt8				m_MapSysStatPtr[DMA_STAT_PTR_SIZE];	// void*	m_MapSysStatPtr[DMA_XFER_BUFFER_COUNT];
	UInt8*				m_StatDataBuf;
	HDEVICE				m_hHspAdapter;
	UInt8*				m_pCurrentData;
	bool				m_AdapterOpened;
	UInt64				m_crcErrors;
	UInt64				m_bytes;
	UInt64				m_nPacketCount;
	UInt64				m_qDiff;
	UInt32				m_uiPartOffset;
	byte				m_pktPart[16384];
	int					m_preBytes;
	int					m_holdPktLen;
	UInt32				m_holdCrc;
	int					m_holdIndex;
	ePktPartTypes		m_eType;
	int					m_bufIndex;
	int					m_iOffset;
	CCrc32				m_crc32;

	UInt8				m_Crc_nb[4];
	UInt8				m_Body_nb[16384];  // large enough for jumbo frame
	UInt8				m_Len_nb[26];
	UInt8				m_Crc_bo[4];
	UInt8				m_Body_bo[16384];  // large enough for jumbo frame
	UInt8				m_TmpPkt[16384];
	UInt8				m_Len_bo[26];
	UInt8				m_EthType[14];

	int					m_delims;
	int					m_oruns;

	int					m_writer_i;
	int					m_reader_i;
	int					m_iDiff;
	bool				m_CfgDone;
	int					m_vlanSz;
	UInt64				m_Qpkts;
	bool				m_bPauseCapture;
	Event				m_PauseEvent;
	UInt64				m_q_reads;
	UInt64				m_q_writes;
	UInt16				m_lastPktSize;
	CHePtr<IPacket>		m_spPacket;

	// HSP Statistics
	UInt64				m_p0_pkt_count;
	UInt64				m_p0_byte_count;
	UInt64				m_p1_pkt_count;
	UInt64				m_p1_byte_count;
	UInt64				m_tx0_pkt_count;
	UInt64				m_tx0_byte_count;
	UInt64				m_tx1_pkt_count;
	UInt64				m_tx1_byte_count;
	UInt32				m_p0_crc_errors;
	UInt32				m_p1_crc_errors;
	UInt8				m_rx_status;
	UInt32				m_phy1gRate;
	UInt16				m_active_sessions;
	UInt8				m_board_type;
	UInt8				m_board_rev_maj;
	UInt8				m_board_rev_min;
	UInt8				m_overflows;
	UInt8				m_dsp_sw_rev_maj;
	UInt8				m_dsp_sw_rev_min;
	UInt8				m_dsp_sw_rev_sub;
	UInt8				m_filter_capability;
	UInt64				m_fpga_rev;
	UInt16				m_max_ip_sessions;
	UInt16				m_max_static_ips;
	UInt16				m_max_ip_ranges;
	UInt32				m_max_strings;
	UInt16				m_max_ports;
	UInt8				m_dsp_sw_rev_release;
	UInt16				m_static_ips_current;
	UInt16				m_ranges_current;
	UInt32				m_strings_current;
	UInt16				m_ports_current;
	int					m_state;
	Event				m_StatusEvent;
	bool				m_SystemRunSinceBoot;
	UInt8*				m_pStatusLoc[1];
	UInt16				m_p0_link_speed;
	UInt16				m_p1_link_speed;
	UInt32				m_CoProcSwRevOs;
	UInt32				m_CoProcSwRevKv;
	UInt32				m_CoProcSwRevDv;
	UInt32				m_CoProcSwRevAv;
	UInt32				m_CoProcSwRevRv;
	UInt32				m_StatMsgCount;

	typedef std::map<IPacketHandler*, QueueThreadObject*, IPacketHandlerComparator>	THandlerMap;

	std::vector< CHePtr<IPacketHandler> >*		m_pHandlers;
	HeLib::CHeAutoCriticalSection*				m_pHandlersCritSect;
	THandlerMap									m_HandlerMap;
	CFileEx										m_DbgFile;
	std::list<PBDRY_STRUCT>						m_bdryPtrs;

protected:
	void	AddToMap( CPacketHandler& pHandler, QueueThreadObject* pThreadObject );

	bool	CloseEvent( eEventTypes inEvent );
	void	CloseEvents();
	void	CloseHspAdapter();
	bool	ConfigHspAdapter();
	bool	CreateEvents();

	bool	GetEvent( eEventTypes inEvent );

	bool	MapSysStatusSegment();
	bool	MapUserMemorySegments();
	bool	MapUserStatusSegment();

	bool	OpenHspAdapter();

	bool	IoControl( int inControlCode, void* inInBuffer, UInt32 inInBufferSize,
		void* outOutBuffer, UInt32 inOutBufferSize, UInt32* outBytesReturned ) {
#ifdef _WIN32

#ifdef _DEBUG
		(void)inControlCode;
		(void)inInBuffer;
		(void)inInBufferSize;
		(void)outOutBuffer;
		(void)inOutBufferSize;
		(void)outBytesReturned;
		return true;
#else
		return (::DeviceIoControl( m_hHspAdapter, inControlCode, inInBuffer, inInBufferSize,
			outOutBuffer, inOutBufferSize, outBytesReturned, nullptr ) != FALSE);
#endif

#else
		(void)inInBufferSize;
		(void)outOutBuffer;
		(void)inOutBufferSize;
		(void)outBytesReturned;
		return ioctl( m_hHspAdapter, inControlCode, inInBuffer );
#endif
}

	void	PostDebugMsg( const char* pMessage ) {
		if ( m_DbgFile.IsOpen() ) {
			m_DbgFile.Write( pMessage );
			m_DbgFile.Flush();
		}
	}
	void	PostDebugMsg( const CPeekStringA& strMsg ) {
		PostDebugMsg( strMsg.c_str() );
	}

	void	ResetPktCounts();

	bool	SendEventToDriver( const CPeekStringA& inEventName, UInt32 inControlCode );
	bool	StartAdapter();
	int		StartCaptureFromAddHandler();
	bool	StopAdapter();
	int		StopCaptureFromRemoveHandler();

	void	UnconfigHspAdapter();

public:
	;		COmniAdapter( const CGlobalId& inId, bool inNavl );
	;		COmniAdapter();
	virtual	~COmniAdapter();

	void			AddPacketHandler( CPacketHandler inHandler );

	virtual void	CopyAdapterInfo( IAdapterInfo** outAdapterInfo ) {
		m_AdapterInfo.Copy( outAdapterInfo );
	}

	virtual CAdapterInfo		GetAdapterInfo() const { return m_AdapterInfo; }
	virtual const CPeekString&	GetAdapterName() const;
	virtual bool				GetAddress( CEthernetAddress& /*outAddress*/ ) const { return false; }
	bool*						GetCapturingPtr() { return &m_bCapturing; }
	UInt64						GetDroppedPackets() const { return m_nDroppedPackets; }
	UInt32						GetFwdParams();
	virtual UInt64				GetLinkSpeed() const { return m_AdapterInfo.GetLinkSpeed(); }
	virtual tMediaType			GetMediaType() const { return m_AdapterInfo.GetMediaType(); }
	CAdapterOptions				GetOptions() const;
	bool*						GetPauseCapturePtr() { return &m_bPauseCapture; }
	Event*						GetPauseEvent() { return &m_PauseEvent; }
	int							GetWriterIndex() { return m_writer_i; }

	void			InitBufAndOffset();
	virtual bool	Initialize( CAdapterInfo inInfo, CPeekEngineProxy inEngineProxy );
	bool			IsNotValid() const { return m_AdapterInfo.IsNotValid(); }
	bool			IsValid() const { return m_AdapterInfo.IsValid(); }

	virtual int		OnContextPrefs( CPeekDataModeler& ioPrefs );
#ifdef IMPLEMENT_SUMMARY
	virtual int		OnUpdateSummaryStats( CSnapshot inSnapshot, void* inContext );
#endif	// IMPLEMENT_SUMMARY

	void			PauseCapture();
	UInt32			ProcessBodyInCurrentBuf( UInt8* pPacket, int* offset, int pktLen, int skipBytes );
	bool			ProcessBoundaryData( UInt8* pBuffer, int index, int* offset, int iEndOffset );
	void			ProcessBuffer( UInt8* pBuffer, UInt32 nLength, UInt32 nRecvAddress );
	void			ProcessDataBuffer( UInt8* pBuffer, int index );
	void			ProcessFullBuffers();
	int				ProcessLenInNextBuf( UInt8* pPacket, int* offset, int index, UInt16 );
	bool			ProcessPacket( UInt8* pPacket, int* offset, int index, UInt16 pktSize );
	bool			ProcessPacketPartial( UInt8* pPacket, int* offset, int index, int iEndOffset );
	void			ProcessPartialBuffer( UInt8* pBuffer, int index, UInt32 brdIndex, UInt32 brdOffset );
	int				ProcessQueue( QueueThreadObject* inThreadObject );
	void			ProcessStatus();
	void			ProcessStatusMessage( UInt8* pMessage );

	void			RemovePacketHandler( CPacketHandler inPacketHandler );
	void			ResumeCapture();
	virtual int		Run();

	byte*			SearchForDelim( UInt8* pData, int* offset, int index, bool* bContinue, unsigned short* pPktSize );
	byte*			SearchForDelimPartial( UInt8* pData, int* offset, int index, int iEndOffset,
			bool* bContinue, unsigned short* pPktSize );
	void			SendQPktsToOmni( QueueThreadObject* pThreadObject );
	void			SetHandlerPtrs( std::vector< CHePtr<IPacketHandler> > *pPktHndlrs,
			HeLib::CHeAutoCriticalSection *ptrSection ) {
		m_pHandlers = pPktHndlrs; 
		m_pHandlersCritSect = ptrSection;
	}
	void			SetOptions( const CAdapterOptions& inOptions );
	virtual int		StartCapture();
	virtual int		StopCapture();
	void			StorePktPart( UInt8* pSegment, int nBytes );

	// HspAdapter
// 	bool		GetHandlerStats(CPacketHandler &hndlr, CHspHandlerStats &hstat);
// 	bool		GetQueueThreadObject(CPacketHandler &hndlr, QueueThreadObject **pphqto);
};
