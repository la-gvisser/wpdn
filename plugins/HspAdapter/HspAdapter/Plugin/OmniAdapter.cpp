// =============================================================================
//	OmniAdapter.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniAdapter.h"
#include "CaptureFile.h"
#include "ContextManager.h"
#include "OmniEngineContext.h"
#include "Options.h"
#include "public.h"
#include <chrono>
#include <map>
#include <memory>
#include <thread>

#ifdef _WIN32
#include <VersionHelpers.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std::chrono;
using namespace HeLib;
using namespace FilePath;
using namespace CaptureFile;

using std::map;

extern CContextManagerPtr	GetContextManager();
extern GUID					g_PluginId;

typedef UInt64				(*GetTimeStampProc)();
static  GetTimeStampProc	s_fnGetTimeStamp = nullptr;
#ifdef _WIN32
static  HMODULE				s_hPeekDll = nullptr;
#endif

CPeekStringA	s_strWindowsHspDeviceName =
#ifdef _DEBUG
	// Use when HSP Adapter is not installed.
	"NUL";
#else
	"\\\\.\\HSPADAPTER";
#endif	// _DEBUG

CPeekStringA	s_strLinuxHspDeviceName =
#ifdef _DEBUG
	// Use when HSP Adapter is not installed.
	"/dev/null";
#else
	"/etc/hspadapter";
#endif	// _DEBUG

static CPeekString	s_strDbgFileName = 
#ifdef _WIN32
	L"C:\\hsp_ra_log.txt";
#else
	L"/var/log/hsp_ra.log";
#endif


#ifdef _DEBUG
// Use when HSP Adapter is not installed.
UInt8	g_DebugTransferBuffer[DMA_XFER_MAX_MSG_SIZE];
#endif

// =============================================================================
//		Functions to port to Linux
// =============================================================================

#ifdef _WIN32
#define CloseDevice(h)		::CloseHandle(h);
#else
#define CloseDevice(h)		close(h);
#endif

#ifdef TARGET_OS_LINUX
#define GENERIC_READ		(0x80000000L)
#define GENERIC_WRITE		(0x40000000L)
#define GENERIC_EXECUTE		(0x20000000L)
#define GENERIC_ALL			(0x10000000L)

#define FILE_SHARE_READ		(0x00000001)
#define FILE_SHARE_WRITE	(0x00000002)

#define CREATE_NEW			(1)
#define CREATE_ALWAYS		(2)
#define OPEN_EXISTING		(3)
#define OPEN_ALWAYS			(4)
#define TRUNCATE_EXISTING	(5)

#define GetLastError()		(0)
#endif


// =============================================================================
//		Globals and Constants
// =============================================================================


#ifdef _WIN32
#define FILE_SEPERATOR_A	'\\'
#define FILE_SEP_STR_A		"\\"
#define FILE_SEPERATOR_W	L'\\'
#define FILE_SEP_STR_W		L"\\"
#else
#define MAX_PATH			260
#define FILE_SEPERATOR_A	'/'
#define FILE_SEP_STR_A		"/"
#define FILE_SEPERATOR_W	L'/'
#define FILE_SEP_STR_W		L"/"
#endif // _WIN32

const UInt64			g_nLinkSpeed( 100000000 );
const size_t			g_nPathLen( 1024 );
const CPeekString		g_strPluginName( L"HspAdapter" );
const CPeekString		g_strAdapterName( L"HspAdapter" );
PKT_STRUCT*				g_Packets[NUM_PKT_PTRS];

#ifdef _WIN32
CPeekString		strPreAmble = (::IsWindowsVistaOrGreater()) ? L"Global\\" : L"";
#else
CPeekString		strPreAmble;
#endif

CPeekStringX	COmniAdapter::m_ayEventNames[EVT_MAX] = {
	CPeekStringX( strPreAmble + L"HspSyncReadEvent.PCIe0" ),
	CPeekStringX( strPreAmble + L"HspSyncWrDataEvent.PCIe0" ),
	CPeekStringX( strPreAmble + L"HspSyncFwdEvent.PCIe0" )
};

// #ifdef IMPLEMENT_SUMMARY
// const GUID	g_idParentStat =  { 0xA97AAAF4, 0x6AF1, 0x4825, { 0x8E, 0xFE, 0xAE, 0x50, 0x93, 0xC9, 0x61, 0xD2 } };
// const GUID	g_idPacketStat =  { 0xC6269A4E, 0x1AAE, 0x42D5, { 0x98, 0xC4, 0x0F, 0xC1, 0x98, 0xAB, 0x08, 0xF5 } };
// const GUID	g_idDropStat =    { 0xDD4FB0F7, 0xFE1C, 0x4196, { 0x83, 0xC7, 0x9D, 0x8B, 0x24, 0x30, 0xE4, 0x3B } };
// 
// CPeekString		COmniAdapter::s_strPacketLabel( L"Packets" );
// CPeekString		COmniAdapter::s_strDropLabel( L"Dropped" );
// #endif	// IMPLEMENT_SUMMARY

extern const CPeekString& GetAdapterName() { return g_strAdapterName; }


// -----------------------------------------------------------------------------
//		BuildPath
// -----------------------------------------------------------------------------

void
BuildPathString(
	const wchar_t*	inPath,
	const wchar_t*	inFileName,
	CPeekString&	outPath )
{
	wchar_t	szPath[g_nPathLen];
#ifdef _WIN32
	wcscpy_s( szPath, g_nPathLen, inPath );
	wcscat_s( szPath, g_nPathLen, FILE_SEP_STR_W );
	wcscat_s( szPath, g_nPathLen, inFileName );
#else
	wcscpy( szPath, inPath );
	wcscat( szPath, FILE_SEP_STR_W );
	wcscat( szPath, inFileName );
#endif
	outPath = szPath;
}


// -----------------------------------------------------------------------------
//		PerformanceDelay
// -----------------------------------------------------------------------------

void
PerformanceDelay(
	SInt64	inNanoseconds )
{
	if ( inNanoseconds <= 0 ) return;

	// Windows uses the QueryPerformanceCounter for high_resolution_clock.
	nanoseconds							durationWait( inNanoseconds );
	high_resolution_clock::time_point	timeEnd = high_resolution_clock::now() + durationWait;
	while ( timeEnd > high_resolution_clock::now() );
}


// -----------------------------------------------------------------------------
//		FilePacketToPacket
// -----------------------------------------------------------------------------

CPacket
FilePacketToPacket(
	tMediaType			inMediaType, 
	const CFilePacket&	inPacket )
{
	PeekPacket	pktHeader;
	pktHeader.fTimeStamp = CPeekTime::Now(); // inPacket.GetTimeStamp(); CPeekTime::Now();
	pktHeader.fProtoSpec = 0;
	pktHeader.fFlags = inPacket.GetFlags();
	pktHeader.fStatus = inPacket.GetStatus();
	pktHeader.fPacketLength = static_cast<UInt16>( inPacket.GetLength() );
	pktHeader.fSliceLength = static_cast<UInt16>( inPacket.GetSliceLength() );
	pktHeader.fMediaSpecInfoBlock = nullptr;

	CPacket	pkt( &pktHeader, inPacket.GetPacketData(), inMediaType );
	return pkt;
}


// -----------------------------------------------------------------------------
//		PacketToFilePacket
// -----------------------------------------------------------------------------

CFilePacket
PacketToFilePacket(
	tMediaType		inMediaType, 
	const CPacket&	inPacket )
{
	PeekPacket		pktHeader = inPacket.GetPeekPacket();
	CByteVectRef	bvData = inPacket.GetPacketDataRef();

	CFilePacket		filePkt( inMediaType );
	filePkt.SetPacketData( pktHeader.fPacketLength, static_cast<UInt32>( bvData.GetCount() ),
		bvData.GetDataPtr( bvData.GetCount() ) );
	filePkt.SetTimeStamp( pktHeader.fTimeStamp );
	filePkt.SetFlags( static_cast<UInt16>( pktHeader.fFlags ) );
	filePkt.SetStatus( static_cast<UInt16>( pktHeader.fStatus ) );

	if ( filePkt.IsWireless() ) {
		;
	}

	return filePkt;
}


// =============================================================================
//		PacketThread
// =============================================================================

UInt32 HE_STDCALL
PacketThread(
	COmniAdapter*	inOmniAdapter )
{
	if ( inOmniAdapter != nullptr ) {
		inOmniAdapter->InitBufAndOffset();
		inOmniAdapter->Run();
	}

	return 0;
}


// =============================================================================
//		QueueThread
// =============================================================================

UInt32 HE_STDCALL
QueueThread(
	QueueThreadObject*	inThreadObject )
{
	if ( inThreadObject != nullptr ) {
		COmniAdapter*	pAdapter = inThreadObject->GetAdapterPtr();
		if ( pAdapter != nullptr ) {
			// if a status message has not been processed yet, wait here until the first one is.

			// need unique bCapturing variables to stop correct thread
			pAdapter->ProcessQueue( inThreadObject );
		}
	}

	return 0;
}


// =============================================================================
//		StatusThread
// =============================================================================

UInt32 HE_STDCALL
StatusThread(
	COmniAdapter*	inOmniAdapter )
{
	if ( inOmniAdapter != nullptr ) {
		inOmniAdapter->ProcessStatus();
	}

	return 0;
}


// =============================================================================
//		COmniAdapter
// =============================================================================

COmniAdapter::COmniAdapter(
	const CGlobalId&	inId,
	bool				inNavl )
	:	m_Id( inId )
	,	m_nRunningCaptures( 0 )
	,	m_nDroppedPackets( 0 )
	,	m_HeaderBytes( 0 )
	,	m_StatDataBuf( nullptr )
	,	m_hHspAdapter( INVALID_HANDLE_VALUE )
	,	m_pCurrentData( nullptr )
	,	m_AdapterOpened( false )
	,	m_crcErrors( 0 )
	,	m_bytes( 0 )
	,	m_nPacketCount( 0 )
	,	m_qDiff( 0 )
	,	m_uiPartOffset( 0 )
	,	m_preBytes( 0 )
	,	m_holdPktLen( 0 )
	,	m_holdCrc( 0 )
	,	m_holdIndex( 0 )
	,	m_eType( PART_TYPE_NONE )
	,	m_bufIndex( 0 )
	,	m_iOffset( 0 )
	,	m_delims( 0 )
	,	m_oruns( 0 )
	,	m_writer_i( 0 )
	,	m_reader_i( 0 )
	,	m_iDiff( 0 )
	,	m_CfgDone( false )
	,	m_vlanSz( 0 )
	,	m_Qpkts( 0 )
	,	m_bPauseCapture( false )
	,	m_q_reads( 0 )
	,	m_q_writes( 0 )
	,	m_lastPktSize( 0 )
	,	m_p0_pkt_count( 0 )
	,	m_p0_byte_count( 0 )
	,	m_p1_pkt_count( 0 )
	,	m_p1_byte_count( 0 )
	,	m_tx0_pkt_count( 0 )
	,	m_tx0_byte_count( 0 )
	,	m_tx1_pkt_count( 0 )
	,	m_tx1_byte_count( 0 )
	,	m_p0_crc_errors( 0 )
	,	m_p1_crc_errors( 0 )
	,	m_rx_status( 0 )
	,	m_phy1gRate( 0 )
	,	m_active_sessions( 0 )
	,	m_board_type( 0 )
	,	m_board_rev_maj( 0 )
	,	m_board_rev_min( 0 )
	,	m_overflows( 0 )
	,	m_dsp_sw_rev_maj( 0 )
	,	m_dsp_sw_rev_min( 0 )
	,	m_dsp_sw_rev_sub( 0 )
	,	m_filter_capability( 0 )
	,	m_fpga_rev( 0 )
	,	m_max_ip_sessions( 0 )
	,	m_max_static_ips( 0 )
	,	m_max_ip_ranges( 0 )
	,	m_max_strings( 0 )
	,	m_max_ports( 0 )
	,	m_dsp_sw_rev_release( 0 )
	,	m_static_ips_current( 0 )
	,	m_ranges_current( 0 )
	,	m_strings_current( 0 )
	,	m_ports_current( 0 )
	,	m_state( 0 )
	,	m_SystemRunSinceBoot( false )
	,	m_p0_link_speed( 0 )
	,	m_p1_link_speed( 0 )
	,	m_CoProcSwRevOs( 0 )
	,	m_CoProcSwRevKv( 0 )
	,	m_CoProcSwRevDv( 0 )
	,	m_CoProcSwRevAv( 0 )
	,	m_CoProcSwRevRv( 0 )
	,	m_StatMsgCount( 0 )
{
#ifdef IMPLEMENT_NAVL
	m_bNavlSupport = inNavl;
#else
	(void) inNavl;
#endif	// IMPLEMENT_NAVL

	m_DbgFile.Open( s_strDbgFileName, (CFileEx::modeWrite | CFileEx::modeCreate | CFileEx::modeNoTruncate) );
	m_StatusEvent.Create( L"HspStatusEvent" );
}

COmniAdapter::COmniAdapter()
	:	COmniAdapter( GUID_NULL, false )
{
}

COmniAdapter::~COmniAdapter()
{
	if ( m_nRunningCaptures > 0 ) {
		m_nRunningCaptures = 1;
		StopCapture();
	}

	m_StatusEvent.Close();

#if (0)
	// Force deletion of these objects.
	if ( m_AdapterInfo.IsValid() ) {
		// Deleting an object of an abstract class is undefined behavior.
		CMediaInfo	mi( m_AdapterInfo.GetMediaInfo() );
		CHePtr<IMediaInfo>	spMI( mi.GetPtr() );
		mi.ReleasePtr();
		while ( spMI.p->Release() > 1 );
		spMI.Release();

		m_AdapterInfo.ReleasePtr();
	}

	if ( m_ChannelManager.IsValid() ) {
		CHePtr<IChannelManager>	spCM( m_ChannelManager.GetPtr() );
		m_ChannelManager.ReleasePtr();
		while ( spCM.p->Release() > 1 );
		spCM.Release();
	}
#endif
}


// -----------------------------------------------------------------------------
//		AddToMap
// -----------------------------------------------------------------------------

void
COmniAdapter::AddToMap(
	CPacketHandler&		pktHandler,
	QueueThreadObject*	pThreadObject )
{
	if ( m_HandlerMap.find( pktHandler.GetPtr() ) == m_HandlerMap.end() ) {
		m_HandlerMap[pktHandler.GetPtr()] = pThreadObject;
	}
}


// -----------------------------------------------------------------------------
//		AddPacketHandler
// -----------------------------------------------------------------------------

void
COmniAdapter::AddPacketHandler(
	CPacketHandler	inHandler )
{
#ifdef IMPLEMENT_NAVL
	CPeekAdapter::AddPacketHandler( inHandler );
#endif	// IMPLEMENT_NAVL

	if ( m_nRunningCaptures == 0 ) {
		StartCaptureFromAddHandler();
	}

	CPeekStringA	strMsg;
	strMsg.Format( "QueueThread: %p Captures: %zu\n", inHandler.GetPtr(),
		static_cast<size_t>( m_nRunningCaptures ) );
	PostDebugMsg( strMsg );

	QueueThreadObject* pThreadObject = new QueueThreadObject( this, inHandler );
	std::unique_ptr<std::thread>	spThread = make_unique<std::thread>( QueueThread, pThreadObject );
	pThreadObject->SetThread( spThread );
	AddToMap( inHandler, pThreadObject );
}

// -----------------------------------------------------------------------------
//		GetAdapterName
// -----------------------------------------------------------------------------

const CPeekString&
COmniAdapter::GetAdapterName() const
{
	return m_strName;
}


// -----------------------------------------------------------------------------
//		CloseEvent
// -----------------------------------------------------------------------------

bool 
COmniAdapter::CloseEvent(
	eEventTypes	inEvent )
{
	static UInt32	ayControlCodes[EVT_MAX] = {
		IOCTL_CLEAR_READ_EVENT,
		IOCTL_CLEAR_WRITE_EVENT,
		IOCTL_HSPADAPTER_CLOSE_FDEVT
	};

	Event*	ayEvent[EVT_MAX] = {
		&m_HspReadEvent,
		nullptr,
		&m_HspForwardEvent
	};

	if ( !ayEvent[inEvent]->IsValid() ) return true;

	UInt8	ayBuffer[MAX_PATH];
	UInt32	nBytesReturned( 0 );
	if ( !IoControl( ayControlCodes[inEvent], ayBuffer, MAX_PATH, ayBuffer,
			MAX_PATH, &nBytesReturned ) ) {
		CPeekStringA	strMsg;
		strMsg.Format( "Error Closing event name %d: %d in kernel.",
			m_ayEventNames[inEvent].GetA().c_str(), GetLastError() );
		PostDebugMsg( strMsg );
		// TODO: don't return to allow the Event to be closed.
		return false;
	}

	ayEvent[inEvent]->Close();
	
	return true;
}


// -----------------------------------------------------------------------------
//		CloseEvents
// -----------------------------------------------------------------------------

void 
COmniAdapter::CloseEvents()
{
	CloseEvent( EVT_READ );
	CloseEvent( EVT_FWD );
	m_StopEvent.Close();
}


// -----------------------------------------------------------------------------
//		CloseHspAdapter
// -----------------------------------------------------------------------------

void 
COmniAdapter::CloseHspAdapter()
{
	if ( m_hHspAdapter != INVALID_HANDLE_VALUE ) {
		CloseDevice( m_hHspAdapter );
		m_hHspAdapter = INVALID_HANDLE_VALUE;
	}
}


// -----------------------------------------------------------------------------
//		ConfigHspAdapter
// -----------------------------------------------------------------------------

bool 
COmniAdapter::ConfigHspAdapter()
{
	CPeekStringA	strMsg;

	strMsg.Format( "Configuring Adapter: %zu\n", static_cast<size_t>( m_nRunningCaptures ) );
	PostDebugMsg( strMsg );

	if ( !OpenHspAdapter() ) {
		PostDebugMsg( "Failed to open adapter.\n" );
		return false;
	}

	if ( !CreateEvents() ) {
		PostDebugMsg( "Failed to create events.\n" );
		return false;
	}

	if ( !MapUserMemorySegments() ) {
		PostDebugMsg( "Failed to map memory segments.\n" );
		CloseEvents();
		CloseHspAdapter();
		return false;
	}

	if ( !MapUserStatusSegment() ) {
		PostDebugMsg( "Failed to map status memory segments.\n" );
		CloseEvents();
		CloseHspAdapter();
		return false;
	}

	if ( !MapSysStatusSegment() ) {
		PostDebugMsg( "Failed to map system status memory segments.\n" );
		CloseEvents();
		CloseHspAdapter();
		return false;
	}

	int SIZEOF_64_BIT_OS_PVOID = sizeof( void* );
	// In this plug-in's environment, sizeof(PVOID) = 4 instead
	// of 8 as it is on 64-bit platforms in general.
	for ( int i = 0; i < DMA_XFER_BUFFER_COUNT; i++ ) {
		int offset = i * SIZEOF_64_BIT_OS_PVOID; //sizeof(void*);
		// TO DO: Make this code platform independent by using a for loop that runs from 0 to sizeof(void*)-1
		UInt64 iVal =
			((UInt64)m_MapPtrs[offset + 7] << 56) |
			((UInt64)m_MapPtrs[offset + 6] << 48) |
			((UInt64)m_MapPtrs[offset + 5] << 40) |
			((UInt64)m_MapPtrs[offset + 4] << 32) |
			((UInt64)m_MapPtrs[offset + 3] << 24) |
			((UInt64)m_MapPtrs[offset + 2] << 16) |
			((UInt64)m_MapPtrs[offset + 1] << 8) |
			(UInt64)m_MapPtrs[offset];

		m_DataBufs[i] = (UInt8*)iVal;

		strMsg.Format( "Ptr: %d Val: 0x%08llX\n", i, iVal );
		PostDebugMsg( strMsg );
	}

	UInt64 statVal = 
		((UInt64)m_MapStatPtr[7] << 56) |
		((UInt64)m_MapStatPtr[6] << 48) |
		((UInt64)m_MapStatPtr[5] << 40) |
		((UInt64)m_MapStatPtr[4] << 32) |
		((UInt64)m_MapStatPtr[3] << 24) |
		((UInt64)m_MapStatPtr[2] << 16) |
		((UInt64)m_MapStatPtr[1] << 8) |
		(UInt64)m_MapStatPtr[0];

	m_pStatusLoc[0] = (UInt8*)statVal;
	memset( &m_pStatusLoc[0][sizeof( unsigned int )], 0, (DMA_XFER_MAX_MSG_SIZE - sizeof( unsigned int )) );

	//	UInt64 sysstatVal = ((UInt64)m_MapSysStatPtr[7] << 56) | ((UInt64)m_MapSysStatPtr[6] << 48) | ((UInt64)m_MapSysStatPtr[5] << 40) | ((UInt64)m_MapSysStatPtr[4] << 32) |
	//				 ((UInt64)m_MapSysStatPtr[3] << 24) | ((UInt64)m_MapSysStatPtr[2] << 16) | ((UInt64)m_MapSysStatPtr[1] << 8) | (UInt64)m_MapSysStatPtr[0];
	//	m_pSysStatus[0] = (PUCHAR)sysstatVal;
	//	m_StatDataBuf = (PUCHAR)statVal;
	//	memset(m_StatDataBuf, 0, 4096);
	//	strMsg.Format( "Stat Ptr: 0x%08X\n", statVal );
	//	PostDebugMsg( strMsg );

	PostDebugMsg( "Adapter is configured.\n" );

	return true;
}

// -----------------------------------------------------------------------------
//		CreateAdapterInfo												[static]
// -----------------------------------------------------------------------------

CAdapterInfo
COmniAdapter::CreateAdapterInfo(
	const CPeekString&	inDescription,
	const CGlobalId&	inId )
{
	CAdapterInfo	aiNew;
	tMediaType		mt( kMediaType_802_3, kMediaSubType_Native );	// Explicit, but is the default.

	aiNew.Create();
	if ( aiNew.IsNotValid() ) return aiNew;

	CPluginAdapterInfo	paiDefault( aiNew );
	if ( paiDefault.IsValid() ) {
		CGlobalId	idPlugin( g_PluginId );
		paiDefault.SetPlugingId( idPlugin );
		paiDefault.SetPluginName( g_strPluginName );
		paiDefault.SetPersistent( false );	// adapter manager will not save on shutdown (we have to do the work)
		paiDefault.SetDeletable( false );
		paiDefault.SetTitle( g_strPluginName );
		paiDefault.SetIdentifier( inId );
		paiDefault.SetMediaType( mt );
		paiDefault.SetDescription( inDescription );
		paiDefault.SetAdapterFeatures( /* PEEK_ADAPTER_QUERY_DROP_AFTER_STOP || */ PEEK_ADAPTER_SETS_PER_CAPTURE_DROP );
		paiDefault.SetLinkSpeed( g_nLinkSpeed );

		CHePtr<IMediaInfo> spMediaInfo;
		if ( HE_SUCCEEDED( spMediaInfo.CreateInstance( "PeekCore.MediaInfo" ) ) ) {
			spMediaInfo->SetMediaType( mt.fType );
			spMediaInfo->SetMediaSubType( mt.fSubType );
			spMediaInfo->SetMediaDomain( kMediaDomain_Null );
			spMediaInfo->SetLinkSpeed( g_nLinkSpeed );

			CMediaInfo mi( spMediaInfo );
			paiDefault.SetMediaInfo( mi );

			spMediaInfo.Release();
		}
	}

	return aiNew;
}


// -----------------------------------------------------------------------------
//		CreateEvents
// -----------------------------------------------------------------------------

bool 
COmniAdapter::CreateEvents()
{
	m_StopEvent.Reset();

	m_ayExitAndReadEvents.push_back( &m_StopEvent );	
	m_ayExitAndFwdEvents.push_back( &m_StopEvent );

	// GetEvent will Reset the event.
	if ( !GetEvent( EVT_READ ) || 
		 !SendEventToDriver( m_ayEventNames[EVT_READ].GetA().c_str(), IOCTL_SET_READ_EVENT ) ) {
		return false;
	}
	m_ayExitAndReadEvents.push_back( &m_HspReadEvent );

	//	if( !GetEvent( EVT_WRITE ) ||
	//		!SendEventToDriver( m_ayAnsiEventNames[EVT_WRITE].c_str(), IOCTL_SET_WRITE_EVENT) ) {
	//		m_HspEventRead.Close();
	//		return false;
	//	}

	if ( !GetEvent( EVT_FWD ) ||
		 !SendEventToDriver( m_ayEventNames[EVT_FWD].GetA().c_str(), IOCTL_HSPADAPTER_SETFDEVT_NM ) ) {
		m_HspReadEvent.Close();
		return false;
	}
	m_ayExitAndFwdEvents.push_back( &m_HspForwardEvent );

	return true;
}


// -----------------------------------------------------------------------------
//		GetEvent
// -----------------------------------------------------------------------------

bool 
COmniAdapter::GetEvent(
	eEventTypes	inEvent )
{
	Event*	ayEvent[EVT_MAX] = {
		&m_HspReadEvent,
		nullptr,
		&m_HspForwardEvent
	};

	const wchar_t*	pszEventName = m_ayEventNames[inEvent].GetW().c_str();

	if ( !ayEvent[inEvent]->Open( pszEventName ) ) {
		if ( !ayEvent[inEvent]->Create( pszEventName ) ) {
			CPeekStringA	strMsg;
			strMsg.Format( "Error creating event %s: %d",
				m_ayEventNames[inEvent].GetA().c_str(), GetLastError() );
			PostDebugMsg( strMsg );

			return false;
		}
	}

	ayEvent[inEvent]->Reset();

	return true;
}


// -----------------------------------------------------------------------------
//		GetFwdParams
// -----------------------------------------------------------------------------

UInt32 
COmniAdapter::GetFwdParams()
{
	_ASSERTE( sizeof( UInt32 ) == 4 );

	UInt32	nBytesReturned( 0 );
	UInt8	outBuf[sizeof( UInt32 )];
	if ( !IoControl( IOCTL_HSPADAPTER_GET_FWD_PARAMS, nullptr, 0, outBuf, 
			sizeof( outBuf ), &nBytesReturned ) ) {
		// TO DO: log error here.
		return 0xFFFFFFFF;
	}

	return static_cast<UInt32>(
		(static_cast<UInt32>( outBuf[3] ) << 24 ) |
		(static_cast<UInt32>( outBuf[2] ) << 16 ) |
		(static_cast<UInt32>( outBuf[1] ) << 8 ) |
		static_cast<UInt32>( outBuf[0] ) );
}


// -----------------------------------------------------------------------------
//		GetOptions
// -----------------------------------------------------------------------------

CAdapterOptions
COmniAdapter::GetOptions() const
{
	CAdapterOptions	options;
	return options;
}


// -----------------------------------------------------------------------------
//		InitBufAndOffset
// -----------------------------------------------------------------------------

void
COmniAdapter::InitBufAndOffset()
{
	UInt32 fwdParams = GetFwdParams();
	UInt32 fwdBuf = (fwdParams & 0x0000000F);
	UInt32 fwdOffset = (fwdParams & 0xFFFFFFF0);
	m_uiPartOffset = fwdOffset;
	m_bufIndex = static_cast<int>( fwdBuf );
}


// -----------------------------------------------------------------------------
//		IAdapterInitialize::Initialize
// -----------------------------------------------------------------------------

bool
COmniAdapter::Initialize(
	CAdapterInfo		inInfo,
	CPeekEngineProxy	inEngineProxy )
{
	if ( !CPeekAdapter::Initialize( inInfo, inEngineProxy ) ) return false;

	CPluginAdapterInfo	info( inInfo );
	if ( info.IsNotValid() ) return false;

	m_AdapterInfo.Create();
	if ( m_AdapterInfo.IsNotValid() ) return false;

	CGlobalId	idPlugin = info.GetPluginId();
	m_AdapterInfo.SetPlugingId( idPlugin );
	m_AdapterInfo.SetPluginName( info.GetPluginName() );
	m_AdapterInfo.SetPersistent( info.GetPersistent() );
	m_AdapterInfo.SetDeletable( info.GetDeletable() );
	m_AdapterInfo.SetTitle( info.GetTitle() );
	m_AdapterInfo.SetIdentifier( info.GetId() );
	m_AdapterInfo.SetMediaType( info.GetMediaType() );
	m_AdapterInfo.SetDescription( info.GetDescription() );
	m_AdapterInfo.SetAdapterFeatures( info.GetAdapterFeatures() );
	m_AdapterInfo.SetLinkSpeed( info.GetLinkSpeed() );
	m_AdapterInfo.SetMediaInfo( inInfo.GetMediaInfo() );

	if ( m_ChannelManager.IsNotValid() ) {
		m_ChannelManager.Create();
		if ( m_ChannelManager.IsValid() ) {
			CPeekChannelData	data;
			m_ChannelManager.SetChannels( data );
		}
	}
	m_AdapterInfo.SetChannelManager( m_ChannelManager );

	m_strName = m_AdapterInfo.GetDescription();

	m_nDroppedPackets = 0;

// #ifdef IMPLEMENT_SUMMARY
// 	m_ParentStat = g_idParentStat;
// 
// 	m_Stats.PacketStat.Id = g_idPacketStat;
// 	m_Stats.PacketStat.Stat.Type = Peek::kSummaryStatType_Packets;
// 	m_Stats.PacketStat.Stat.Flags = 0;
// 	m_Stats.PacketStat.Stat.Value.Packets = 0;
// 
// 	m_Stats.DropStat.Id = g_idDropStat;
// 	m_Stats.DropStat.Stat.Type = Peek::kSummaryStatType_Packets;
// 	m_Stats.DropStat.Stat.Flags = 0;
// 	m_Stats.DropStat.Stat.Value.Packets = 0;
// #endif	// IMPLEMENT_SUMMARY

	return true;
}


// -----------------------------------------------------------------------------
//		MapSysStatusSegment
// -----------------------------------------------------------------------------

bool 
COmniAdapter::MapSysStatusSegment()
{
#ifdef _DEBUG
	memset( &m_MapSysStatPtr, 0, sizeof( m_MapSysStatPtr ) );
#endif

	UInt32	nBytesReturned( 0 );
	if ( !IoControl( IOCTL_HSPADAPTER_MAPSYSSTATUSUSR, m_MapSysStatPtr, 
			DMA_STAT_PTR_SIZE, m_MapSysStatPtr, DMA_STAT_PTR_SIZE, &nBytesReturned ) ) {
		CPeekStringA	strMsg;
		strMsg.Format( "IOCTL_HSPADAPTER_MAP_SYS_STATUSUSR ERROR: %d", GetLastError() );
		PostDebugMsg( strMsg );
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		MapUserMemorySegments
// -----------------------------------------------------------------------------

bool 
COmniAdapter::MapUserMemorySegments()
{
#ifdef _DEBUG
	memset( &m_MapPtrs, 0, sizeof( m_MapPtrs ) );
#endif

	UInt32	nBytesReturned( 0 );
	if ( !IoControl( IOCTL_HSPADAPTER_MAPUSR, m_MapPtrs, DMA_PTRS_SIZE, m_MapPtrs,
			DMA_PTRS_SIZE, &nBytesReturned ) ) {
		CPeekStringA	strMsg;
		strMsg.Format( "IOCTL_HSPADAPTER_MAPUSR ERROR: %d", GetLastError() );
		PostDebugMsg( strMsg );
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		MapUserStatusSegment
// -----------------------------------------------------------------------------

bool 
COmniAdapter::MapUserStatusSegment()
{
#ifdef _DEBUG
	memset( &m_MapStatPtr, 0, sizeof( m_MapStatPtr ) );
	memset( &g_DebugTransferBuffer, 0xFF, sizeof( g_DebugTransferBuffer ) );
	UInt8**	ppMapStatPtr = (UInt8**)&m_MapStatPtr;
	*ppMapStatPtr = &g_DebugTransferBuffer[0];
#endif

	UInt32	nBytesReturned( 0 );
	if ( !IoControl( IOCTL_HSPADAPTER_MAPSTATUSUSR, m_MapStatPtr, DMA_STAT_PTR_SIZE,
			m_MapStatPtr, DMA_STAT_PTR_SIZE, &nBytesReturned ) ) {
		CPeekStringA	strMsg;
		strMsg.Format( "IOCTL_HSPADAPTER_MAPSTATUSUSR ERROR: %d", GetLastError() );
		PostDebugMsg( strMsg );
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniAdapter::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CAdapterOptions	optAdapter;

	if ( ioPrefs.IsLoading() ) {
		optAdapter.Model( ioPrefs );
		SetOptions( optAdapter );
	}
	else {
		optAdapter = GetOptions();
		optAdapter.Model( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


#ifdef IMPLEMENT_SUMMARY
// -----------------------------------------------------------------------------
//		OnUpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniAdapter::OnUpdateSummaryStats(
	CSnapshot	inSnapshot,
	void*		inContext )
{
	QueueThreadObject*	pThreadObject( nullptr );
	COmniEngineContext*	pContext( reinterpret_cast<COmniEngineContext*>( inContext ) );
	if ( pContext ) {
		CPeekContextProxy* pProxy = pContext->GetPeekContextProxy();
		if ( pProxy ) {
			CPacketHandler	PktHandler( pProxy->GetPacketHandler() );
			if ( PktHandler.IsValid() ) {
				auto itr = m_HandlerMap.find( PktHandler.GetPtr() );
				if ( itr != m_HandlerMap.end() ) {
					pThreadObject = itr->second;
					if ( pThreadObject ) {
						return pThreadObject->OnUpdateSummaryStats( inSnapshot );
					}
				}
			}
		}
	}

#if (0)
    if ( !inSnapshot.HasItem( m_ParentStat ) ) 
    {
        Peek::SummaryStat statGroup; 
        statGroup.Type = Peek::SummaryStatType::kSummaryStatType_Null;
        statGroup.Flags = 0; //Peek::SummaryStatFlags::kSummaryStats_SortSubItems;
        statGroup.Value.IntValue = 0;
        inSnapshot.SetItem( m_ParentStat, GUID_NULL, g_strAdapterName, &statGroup );
    }

    if ( !inSnapshot.HasItem( m_Stats.F2O_CountStat.Id ) ) {
        inSnapshot.SetItem( 
                    m_Stats.F2O_CountStat.Id, 
                    m_ParentStat, 
                    s_strFTOCountLabel, 
                    &m_Stats.F2O_CountStat.Stat 
                );
    }

    if ( !inSnapshot.HasItem( m_Stats.RFHM_CountStat.Id ) ) {
        inSnapshot.SetItem( 
                    m_Stats.RFHM_CountStat.Id, 
                    m_ParentStat, 
                    s_strRFHMCountLabel, 
                    &m_Stats.RFHM_CountStat.Stat 
                 );
    }

    // . . . remaining SetItem calls
 
    QueueThreadObject* qthread(nullptr);
    COmniEngineContext* pContext(reinterpret_cast<COmniEngineContext*>(inContext));
    if (pContext) 
    {
      CPeekContextProxy* pProxy = pContext->GetPeekContextProxy();
      if (pProxy) 
      {
        CPacketHandler PktHandler(pProxy->GetPacketHandler());
        If (PktHandler.IsValid()) 
        {
            if(m_HandlerMap.size() > 0)
            {
                auto itr = m_HandlerMap.find(PktHandler.GetPtr());
                if (itr != m_HandlerMap.end()) 
                {
                    qthread = itr->second;
                }
            }
        }
      }
    }

    if (qthread != nullptr /*(QueueThreadObject *&)m_HandlerMap.end()*/)
    {
        m_Stats.F2O_CountStat.Stat.Value.Packets = qthread->m_Qpkts; // Forwarded To Omni
        m_Stats.PrimeOrunsStat.Stat.Value.IntValue = qthread->m_prime_oruns;
        m_Stats.SecondOrunsStat.Stat.Value.IntValue = qthread->m_oruns;
        // Read From Host Memory
        // bytes
        UInt64 nbp = m_bytes;
        INT64 diff = (INT64)(nbp - qthread->m_InitialByteCount);
        if (diff < 0)
            qthread->m_InitialByteCount = 0;
        nbp = nbp - qthread->m_InitialByteCount;
        m_Stats.RFHM_CountStat.Stat.Value.ValuePair.Bytes = nbp;
        // packets
        nbp = m_nPacketCount;
        diff = (INT64)(nbp - qthread->m_InitialPktCount);
        if (diff < 0)
            qthread->m_InitialPktCount = 0;
        nbp = nbp - qthread->m_InitialPktCount;
        m_Stats.RFHM_CountStat.Stat.Value.ValuePair.Packets = nbp;
        // Board To Host Memory
        // bytes
        nbp = m_tx0_byte_count;
        diff = (INT64)(nbp - qthread->m_Init_t0_byte_count);
        if (diff < 0)
            qthread->m_Init_t0_byte_count = 0;
        nbp = nbp - qthread->m_Init_t0_byte_count;
        m_Stats.BTHM_CountStat.Stat.Value.ValuePair.Bytes = nbp;
        // packets
        nbp = m_tx0_pkt_count;
        diff = (INT64)(nbp - qthread->m_Init_t0_pkt_count);
        if (diff < 0)
            qthread->m_Init_t0_pkt_count = 0;
        nbp = nbp - qthread->m_Init_t0_pkt_count;
        m_Stats.BTHM_CountStat.Stat.Value.ValuePair.Packets = nbp;
        // Bad Frame Count
        UInt64 crcs = (m_p0_crc_errors & 0x3FFFFFFF);
        diff = (INT64)(crcs - qthread->m_Init_p0_bad_frms);
        if (diff < 0)
            qthread->m_Init_p0_bad_frms = 0;
        crcs = crcs - qthread->m_Init_p0_bad_frms;
        m_Stats.Input0ErrorBadFrmsStat.Stat.Value.IntValue = crcs;
        crcs = (m_p1_crc_errors & 0x3FFFFFFF);
        diff = (INT64)(crcs - qthread->m_Init_p1_bad_frms);
        if (diff < 0)
            qthread->m_Init_p1_bad_frms = 0;
        crcs = crcs - qthread->m_Init_p1_bad_frms;
        m_Stats.Input1ErrorBadFrmsStat.Stat.Value.IntValue = crcs;
        // Board Count Packets
        nbp = m_p0_pkt_count;
        diff = (INT64)(nbp - qthread->m_Init_p0_pkt_count);
        if (diff < 0)
            qthread->m_Init_p0_pkt_count = 0;
        nbp = nbp - qthread->m_Init_p0_pkt_count;
        m_Stats.Input0CountStat.Stat.Value.ValuePair.Packets = nbp;
        nbp = m_p1_pkt_count;
        diff = (INT64)(nbp - qthread->m_Init_p1_pkt_count);
        if (diff < 0)
            qthread->m_Init_p1_pkt_count = 0;
        nbp = nbp - qthread->m_Init_p1_pkt_count;
        m_Stats.Input1CountStat.Stat.Value.ValuePair.Packets = nbp;
        // Board Count Bytes
        nbp = m_p0_byte_count;
        diff = (INT64)(nbp - qthread->m_Init_p0_byte_count);
        if (diff < 0)
            qthread->m_Init_p0_byte_count = 0;
        nbp = nbp - qthread->m_Init_p0_byte_count;
        m_Stats.Input0CountStat.Stat.Value.ValuePair.Bytes = nbp;
        nbp = m_p1_byte_count;
        diff = (INT64)(nbp - qthread->m_Init_p1_byte_count);
        if (diff < 0)
            qthread->m_Init_p1_byte_count = 0;
        nbp = nbp - qthread->m_Init_p1_byte_count;
        m_Stats.Input1CountStat.Stat.Value.ValuePair.Bytes = nbp;
        UInt8 rx_stat_bits[8] = { 0,0,0,0,0,0,0,0 };
        UInt32 rxCntOvrflw0 = m_p0_crc_errors;
        UInt32 rxCntOvrflw1 = m_p1_crc_errors;
        rx_stat_bits[7] = rxCntOvrflw0 >> 31; // Data FIFO Overflow
        rx_stat_bits[5] = (rxCntOvrflw0 & 0x40000000) >> 30; // Info FIFO Overflow
        rx_stat_bits[6] = rxCntOvrflw1 >> 31; // Data FIFO Overflow
        rx_stat_bits[4] = (rxCntOvrflw1 & 0x40000000) >> 30; // Info FIFO Overflow
        rx_stat_bits[3] = (UInt8)((UInt32)m_phy1gRate & 0x00000004) >> 2;  // p0 duplex
        rx_stat_bits[1] = (UInt8)((UInt32)m_phy1gRate & 0x00000008) >> 3;  // p0 link
        rx_stat_bits[2] = (UInt8)(((UInt32)m_phy1gRate & 0x00040000) >> 18);  // p1 duplex
        rx_stat_bits[0] = (UInt8)(((UInt32)m_phy1gRate & 0x00080000) >> 19);  // p1 link
        m_Stats.Input0ErrorDFifoOvrFlwStat.Stat.Value.IntValue = rx_stat_bits[7];
        m_Stats.Input0ErrorIFifoOvrflwStat.Stat.Value.IntValue = rx_stat_bits[5];
        m_Stats.Input1ErrorDFifoOvrFlwStat.Stat.Value.IntValue = rx_stat_bits[6];
        m_Stats.Input1ErrorIFifoOvrflwStat.Stat.Value.IntValue = rx_stat_bits[4];
        m_Stats.Input0DuplexModeStat.Stat.Value.IntValue = rx_stat_bits[3];
        m_Stats.Input0LinkStat.Stat.Value.IntValue = rx_stat_bits[1];
        m_Stats.Input1DuplexModeStat.Stat.Value.IntValue = rx_stat_bits[2];
        m_Stats.Input1LinkStat.Stat.Value.IntValue = rx_stat_bits[0];
        m_Stats.Embedded_SWOS_VerStat.Stat.Value.IntValue = m_CoProcSwRevOs;
        UInt32 kvMaj(0);
        kvMaj = ((m_CoProcSwRevKv >> 24) > 0x30) ? (m_CoProcSwRevKv >> 24) - 0x30 : 0;
        UInt32 kvMin(0);
        kvMin = (((m_CoProcSwRevKv >> 16) & 0xFF) > 0x30) ? ((m_CoProcSwRevKv >> 16) & 0xFF) - 0x30 : 0;
        UInt32 kvSMaj(0);
        kvSMaj = (((m_CoProcSwRevKv >> 8) & 0xFF) > 0x30) ? ((m_CoProcSwRevKv >> 8) & 0xFF) - 0x30 : 0;
        UInt32 kvSMin(0);
        kvSMin = ((m_CoProcSwRevKv >> 16) > 0x30) ? (m_CoProcSwRevKv & 0xFF) - 0x30 : 0;
        m_Stats.Embedded_SWKrnMaj_VerStat.Stat.Value.IntValue = kvMaj;
        m_Stats.Embedded_SWKrnMin_VerStat.Stat.Value.IntValue = kvMin;
        m_Stats.Embedded_SWKrnSubMaj_VerStat.Stat.Value.IntValue = kvSMaj;
        m_Stats.Embedded_SWKrnSubMin_VerStat.Stat.Value.IntValue = kvSMin;
        m_Stats.Embedded_SWDrvr_VerStat.Stat.Value.IntValue = m_CoProcSwRevDv;
        m_Stats.Embedded_SWApp_VerStat.Stat.Value.IntValue = m_CoProcSwRevAv;
        m_Stats.Embedded_SWRootFS_VerStat.Stat.Value.IntValue = m_CoProcSwRevRv;
        UInt16 fpga_rev[4] = { 0, 0, 0, 0 };
        fpga_rev[3] = (m_fpga_rev >> 24) & 0xFF;
        fpga_rev[2] = (m_fpga_rev >> 16) & 0xFF;
        fpga_rev[1] = (m_fpga_rev >> 8) & 0xFF;
        fpga_rev[0] = m_fpga_rev & 0xFF;
        m_Stats.Embedded_FPGAMaj_VerStat.Stat.Value.IntValue = fpga_rev[3];
        m_Stats.Embedded_FPGAMin_VerStat.Stat.Value.IntValue = fpga_rev[2];
        m_Stats.Embedded_FPGASubMaj_VerStat.Stat.Value.IntValue = fpga_rev[1];
        m_Stats.Embedded_FPGASubMin_VerStat.Stat.Value.IntValue = fpga_rev[0];
    }

    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.F2O_CountStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.RFHM_CountStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.BTHM_CountStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.PrimeOrunsStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.SecondOrunsStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.DropStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input0CountStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input0ErrorBadFrmsStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input0ErrorDFifoOvrFlwStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input0ErrorIFifoOvrflwStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input0DuplexModeStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input0LinkStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input1CountStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input1ErrorBadFrmsStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input1ErrorDFifoOvrFlwStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input1ErrorIFifoOvrflwStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input1DuplexModeStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Input1LinkStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWOS_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWKrnMaj_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWKrnMin_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWKrnSubMaj_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWKrnSubMin_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWDrvr_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWApp_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_SWRootFS_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_FPGAMaj_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_FPGAMin_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_FPGASubMaj_VerStat, 0, false );
    inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.Embedded_FPGASubMin_VerStat, 0, false );
#endif

	return PEEK_PLUGIN_SUCCESS;
}
#endif	// IMPLEMENT_SUMMARY


// -----------------------------------------------------------------------------
//		OpenHspAdapter
// -----------------------------------------------------------------------------
bool
COmniAdapter::OpenHspAdapter()
{
	// HSPAdapter Device Class Enumeration
	//	GUID gud = {0x783895B0, 0x37B6, 0x4940, 0xAA, 0xB6, 0x5B, 0xD9, 0xF9, 0x8A, 0x5B, 0x74};
	//	m_dvSetup = new CDeviceSetup(gud);
	//	CPtrArray *dvsFound = new CPtrArray();
	//	m_dvSetup->EnumDevices(dvsFound);

	if ( m_hHspAdapter != INVALID_HANDLE_VALUE ) {
		PostDebugMsg( "OpenHspAdapter Device already opened.\n" );
		return true;
	}

#ifdef _WIN32
	m_hHspAdapter = ::CreateFileA( s_strWindowsHspDeviceName.c_str(),
		(GENERIC_READ | GENERIC_WRITE),	(FILE_SHARE_READ | FILE_SHARE_WRITE),
		nullptr, OPEN_EXISTING, 0, nullptr );
#else
	m_hHspAdapter = open( s_strLinuxHspDeviceName.c_str(), (O_RDWR | O_NONBLOCK), 0 );
#endif
	if ( m_hHspAdapter == INVALID_HANDLE_VALUE ) {
		CPeekStringA	strMsg;
		strMsg.Format( "CreateFile ERROR opening HSPADAPTER device: %d\n", GetLastError() );
		PostDebugMsg( strMsg );
		return false;
	}

	PostDebugMsg( "OpenHspAdapter: device opened.`\n");

	return true;
}


// -----------------------------------------------------------------------------
//      PauseCapture
// -----------------------------------------------------------------------------

void
COmniAdapter::PauseCapture()
{
	m_bPauseCapture = true;
//	PostDebugMsg( "Capture paused.\n" );
}


// -----------------------------------------------------------------------------
//		ProcessBodyInCurrentBuf
// -----------------------------------------------------------------------------

UInt32 
COmniAdapter::ProcessBodyInCurrentBuf(
	UInt8*	pPacket,
	int*	offset,
	int		pktLen,
	int		skipBytes )
{
	int os = *offset;

	if ( m_nRunningCaptures > 0 ) {
		// Transfer packet to Omni here. Size includes CRC.
		ProcessBuffer( &pPacket[os], pktLen - ETHERNET_DELIMITER_SIZE, 0 );
	}

	os += skipBytes - ETHERNET_DELIMITER_SIZE;
    os = static_cast<int>( ((os % 16) == 0) ? os : ((os / 16 * 16) + 16) );

    *offset = os;
    m_nPacketCount++;

	m_bytes += pktLen - ETHERNET_DELIMITER_SIZE;

	return 0;
}


// -----------------------------------------------------------------------------
//		ProcessBoundaryData
// -----------------------------------------------------------------------------

bool 
COmniAdapter::ProcessBoundaryData(
	UInt8*	pBuffer,
	int		index,
	int*	offset,
	int		iEndOffset )
{
    int pktLen = 0;
	int bodyLeft = 0;
	int iLocalOffset = 0;
	int lenLeft = 0;
	int iBytesToCrc = 0;
	int os = *offset;
	
	// Determine what data is was in the previous buffer and what to 
	// do with it based on the number of bytes in that buffer.
	switch ( m_eType ) {
		case PART_TYPE_NONE:
			// do nothing
			break;

		case PART_TYPE_DELIM_PART:
			if ( (UInt8)pBuffer[os + 0] == (UInt8)0x55 && 
				 (UInt8)pBuffer[os + 1] == (UInt8)0x55 && 
				 (UInt8)pBuffer[os + 2] == (UInt8)0x55 && 
				 (UInt8)pBuffer[os + 3] == (UInt8)0xD5 ) {
				os += sizeof(UInt32);
				ProcessPacketPartial( pBuffer, &os, index, iEndOffset );
			}
			break;

		case PART_TYPE_DELIM_FULL:
			// Entire delim exists in previous buffer. Process new buff normally.
			ProcessPacketPartial( pBuffer, &os, index, iEndOffset );
			break;

		case PART_HSP_PKT_BODY:
			bodyLeft = m_lastPktSize - m_preBytes;

			// get bytes from previous buffer
			for ( int i = 0; i < m_preBytes; i++ ) {
				m_Body_bo[i] = m_pktPart[i];
			}

			pBuffer = m_DataBufs[(m_holdIndex + 1) % DMA_XFER_BUFFER_COUNT];

			// get remainder bytes
			for ( int i = 0; i < bodyLeft; i++ ) {
				m_Body_bo[m_preBytes + i] = pBuffer[i];
			}

			iLocalOffset = 0;
			/*crc = */ ProcessBodyInCurrentBuf( m_Body_bo, &iLocalOffset, m_lastPktSize, bodyLeft );
			os += iLocalOffset;
			break;

		case PART_TYPE_BODY:
			bodyLeft = m_holdPktLen + 18 + m_vlanSz - m_preBytes;

			// get bytes from previous buffer
			for (int i = 0; i < m_preBytes; i++) {
				m_Body_bo[i] = m_pktPart[i];
			}

			pBuffer = m_DataBufs[(m_holdIndex + 1) % DMA_XFER_BUFFER_COUNT];

			// get remainder bytes
			for ( int i = 0; i < bodyLeft; i++ ) {
				m_Body_bo[m_preBytes + i] = pBuffer[i];
			}

			iLocalOffset = 0;
			/* crc = */ ProcessBodyInCurrentBuf( m_Body_bo, &iLocalOffset, m_holdPktLen, bodyLeft );
			os += iLocalOffset;
			break;

		case PART_TYPE_LEN:
			lenLeft = 18 + m_vlanSz - m_preBytes;

			// get bytes from previous buffer
			for ( int i = 0; i < m_preBytes; i++ ) {
				m_Len_bo[i] = m_pktPart[i];
			}

			pBuffer = m_DataBufs[(m_holdIndex + 1) % DMA_XFER_BUFFER_COUNT];

			for ( int i = 0; i < lenLeft; i++ ) {
				m_Len_bo[m_preBytes + i] = pBuffer[i];
			}

			pktLen = (int)(((int)m_Len_bo[16 + m_vlanSz] << 8) | (int)m_Len_bo[17 + m_vlanSz]);

			m_bytes += static_cast<UInt64>( pktLen + 14 + 4 + m_vlanSz );
			for ( int i = 0; i < 18 + m_vlanSz; i++ ) {
				m_Body_bo[i] = m_Len_bo[i];
			}
			for ( int i = 0; i < pktLen; i++ ) {	// this copy will include the trailer.
				m_Body_bo[18 + m_vlanSz + i] = pBuffer[lenLeft + i];
			}

			iLocalOffset = 0;
			// offset is aligned into new buffer after this call
			/* crc = */ ProcessBodyInCurrentBuf( m_Body_bo, &iLocalOffset, pktLen, lenLeft + pktLen );  
			os += iLocalOffset;
			break;

		case PART_TYPE_CRC:
			iBytesToCrc = sizeof( UInt32 ) - m_preBytes;

			// Only step over the bytes in the new buffer.
			iLocalOffset += iBytesToCrc;			
			os += iLocalOffset;
			break;

		default:
			break;
	}

	os = (os + sizeof( UInt32 ) - 1) / sizeof( UInt32 ) * sizeof( UInt32 );
	*offset = os;

	return true;
}


// -----------------------------------------------------------------------------
//		ProcessBuffer
// -----------------------------------------------------------------------------

void
COmniAdapter::ProcessBuffer(
	UInt8*	pBuffer,
	UInt32	nLength,
	UInt32	/* nRecvAddress */ )
{
	// Write data to queue first, then increment pointer.
	// This guarantees the data is in place BEFORE the
	// reader forwards it to Omni.
	g_Packets[m_writer_i]->m_pPkt = pBuffer;
	g_Packets[m_writer_i]->size = nLength;

	m_q_writes++;
	++m_writer_i;
	m_writer_i %= NUM_PKT_PTRS;
}


// -----------------------------------------------------------------------------
//		ProcessDataBuffer
// -----------------------------------------------------------------------------

void 
COmniAdapter::ProcessDataBuffer(
	UInt8*	pBuffer,
	int		index )
{
	// If we're processing a full buffer AFTER it was processed as a partial
	// we need to start at the partial offset instead of the usual.
	int offset = (m_uiPartOffset != 0) ? static_cast<int>( m_uiPartOffset ) : m_iOffset;

	// Only process boundary when at beginning of buffer.
	if ( offset == m_iOffset ) {
		ProcessBoundaryData( pBuffer, index, &offset, DMA_XFER_BUFFER_SIZE );
	}

	// Clear partial data type to avoid re-processing.
	m_eType = PART_TYPE_NONE;
	m_holdPktLen = 0;
	m_preBytes = 0;

	int		packets( 0 );
	bool	bContinue( false );

	while ( offset < DMA_XFER_BUFFER_SIZE ) {
		unsigned short iPktSize( 0 );
		if ( SearchForDelim( pBuffer, &offset, index, &bContinue, &iPktSize ) != nullptr) {	// null when there are no more delims
			// IMPORTANT: Reading from the device while processing packets in this loop,
			//            prevented the app from keeping up with the card @ 1Gbps. 
			//            DO NOT DO THIS!!!
			packets++;

			// check for enough buffer to check for the packet length
			if ( !ProcessPacket( pBuffer, &offset, index, iPktSize ) ) break;
		}
		else {
			break; // continue search in the next buffer
		}

		// ensure offset is on a 32-bit boundary before searching for the next packet
		offset = (offset + sizeof( UInt32 ) - 1) / sizeof( UInt32 ) * sizeof( UInt32 );
	}

	m_uiPartOffset = 0;
}


// -----------------------------------------------------------------------------
//		ProcessFullBuffers
// -----------------------------------------------------------------------------

void 
COmniAdapter::ProcessFullBuffers()
{
	UInt8*	pBuf( nullptr );
	UInt32	fwdParams( *reinterpret_cast<UInt32*>( m_pStatusLoc[0] ) );			//GetFwdParams(); // TO DO: Simplify this IOCTL in the driver.
	UInt32	fwdBuf( fwdParams & 0x0000000F );
//		UInt32	fwdOffset( fwdParams & 0xFFFFFFF0 );

	int iDiff = ((fwdBuf - m_bufIndex) >= 0)
		? (fwdBuf - m_bufIndex)
		: (DMA_XFER_BUFFER_COUNT + (fwdBuf - m_bufIndex));
	if ( iDiff >= 14 ) {
		CPeekStringA	strMsg;
		strMsg.Format( "Diff: %d\n", iDiff );
		PostDebugMsg( strMsg );
	}

	while ( static_cast<int>( fwdBuf ) != m_bufIndex ) {
		pBuf = m_DataBufs[m_bufIndex];
		ProcessDataBuffer(pBuf, m_bufIndex);
		++m_bufIndex;
		m_bufIndex %= DMA_XFER_BUFFER_COUNT;
	}
}


// -----------------------------------------------------------------------------
//		ProcessLenInNextBuf
// -----------------------------------------------------------------------------

int 
COmniAdapter::ProcessLenInNextBuf(
	UInt8*	pPacket,
	int*	offset,
	int		index,
	UInt16	pktSize )
{
	int os = *offset;
	int iPreBufBytes = DMA_XFER_BUFFER_SIZE - os;

	// get bytes from previous buffer
	for ( int i = 0; i < iPreBufBytes; i++ ) {
		m_Body_nb[i] = pPacket[os + i];
	}

	m_eType = PART_HSP_PKT_BODY;
	m_lastPktSize = pktSize;
	
	StorePktPart( m_Body_nb, iPreBufBytes );
	m_holdIndex = index;
	*offset = m_iOffset;

	return -1;
}


// -----------------------------------------------------------------------------
//		ProcessPacket
// -----------------------------------------------------------------------------

bool 
COmniAdapter::ProcessPacket(
	UInt8*	pPacket,
	int*	offset,
	int		index,
	UInt16	pktSize )
{
	bool	bReturn( true );
	int		os( *offset );

	// Check for enough bytes to read the packet length.
	//	18 to len + 4 Nested Vlan + 4 Vlan
	if ( (os + pktSize - ETHERNET_DELIMITER_SIZE) < DMA_XFER_BUFFER_SIZE ) {
		ProcessBodyInCurrentBuf( pPacket, &os, pktSize, pktSize );
	}
	else {
		ProcessLenInNextBuf( pPacket, &os, index, pktSize );
		bReturn = false;
	}

	*offset = os;

	return bReturn;
}


// -----------------------------------------------------------------------------
//		ProcessPacketPartial
// -----------------------------------------------------------------------------

bool 
COmniAdapter::ProcessPacketPartial(
	UInt8*	pPacket,
	int*	offset,
	int		/* index */,
	int		iEndOffset )
{
	bool	bReturn( true );
	UInt32	endOfData = static_cast<UInt32>( (*offset + m_lastPktSize - ETHERNET_DELIMITER_SIZE) & 0xFFFFFFF0 ); // size includes delimiter

	if ( endOfData <= static_cast<UInt32>( iEndOffset ) ) {
		ProcessBodyInCurrentBuf( pPacket, offset, m_lastPktSize, m_lastPktSize );
	}
	else {
		bReturn = false;
	}

	return bReturn;
}


// -----------------------------------------------------------------------------
//		ProcessPartialBuffer
// -----------------------------------------------------------------------------

void 
COmniAdapter::ProcessPartialBuffer(
	UInt8*	pBuffer,
	int		index,
	UInt32	brdIndex,
	UInt32	brdOffset )
{
	int offset = (m_uiPartOffset == 0) ? m_iOffset : static_cast<int>( m_uiPartOffset );

	// If we're on the same buffer, we must not try to read to the end of the buffer.
	int iEndOffset = (index == static_cast<int>( brdIndex) ) ? brdOffset : DMA_XFER_BUFFER_SIZE;

	// Only process boundary when at beginning of buffer.
	if ( offset == m_iOffset ) {
		ProcessBoundaryData( pBuffer, index, &offset, iEndOffset );
	}

	// Clear partial data type to avoid re-processing.
	m_eType = PART_TYPE_NONE;
	m_preBytes = 0;
	m_holdPktLen = 0;
	m_holdCrc = 0;

	int		iLastGoodOffset = (m_uiPartOffset == 0) ? m_iOffset : static_cast<int>( m_uiPartOffset );
	bool	bContinue = false;
	UInt16	iPktSize = 0;
	while ( offset < iEndOffset ) {
		if ( SearchForDelimPartial( pBuffer, &offset, index, iEndOffset, &bContinue, &iPktSize) != nullptr ) {	// null when there are no more packets
			iLastGoodOffset = offset;

			m_lastPktSize = iPktSize;
			// check for enough buffer to check for the packet length
			if ( !ProcessPacketPartial( pBuffer, &offset, index, iEndOffset) ) break;
		}
		else {
			break;
		}

		// ensure offset is on a 32-bit boundary before searching for the next packet
		offset = (offset + sizeof( UInt32 ) - 1) / sizeof( UInt32 ) * sizeof( UInt32 );
	}

	// If this buffer goes full after we leave, let's set the offset so
	// ProcessDataBuffer can start at the right place.
	m_uiPartOffset = static_cast<UInt32>( (iLastGoodOffset + sizeof( UInt32 ) - 1) / sizeof( UInt32 ) * sizeof( UInt32 ) );
}


// -----------------------------------------------------------------------------
//		ProcessQueue
// -----------------------------------------------------------------------------

int
COmniAdapter::ProcessQueue(
	QueueThreadObject*	pThreadObject )
{
	int				nResult = PEEK_PLUGIN_SUCCESS;
	CPeekStringA	strMsg;

	// We must wait to process a status message before moving past the next 
	// line because we need the init values to be set by then.
	if ( !m_SystemRunSinceBoot ) {
		PostDebugMsg( "Waiting for status event.\n" );
		while ( !m_StatusEvent.IsValid() );

		m_StatusEvent.Wait( 7000 );
		m_SystemRunSinceBoot = true;
		PostDebugMsg( "Got it.\n" );
	}

	// 	strMsg.Format( "Pkt Cnt: 0x%016x Bytes: 0x%016x P0Pkts: 0x%016x P0Bytes: 0x%016x P1Pkts: 0x%016x P1Bytes: 0x%016x T0Pkts: 0x%016x T0Bytes: 0x%016x T1Pkts: 0x%016x T1Bytes: 0x%016x",
	// 				m_nPacketCount, m_bytes, m_p0_pkt_count, m_p0_byte_count, m_p1_pkt_count, m_p1_byte_count, m_tx0_pkt_count, m_tx0_byte_count, m_tx1_pkt_count, m_tx1_pkt_count );
	// 	PostDebugMsg( strMsg );

	HeResult hr = pThreadObject->Init( m_nPacketCount, m_bytes, m_p0_pkt_count, m_p0_byte_count,
		m_p1_pkt_count, m_p1_byte_count, m_tx0_pkt_count, m_tx0_byte_count, m_tx1_pkt_count, m_tx1_pkt_count );
		// , (m_p0_crc_errors & 0x3FFFFFFF), (m_p1_crc_errors & 0x3FFFFFFF) );

	if ( HE_FAILED( hr ) ) return -1;

	// Loop until capture stops.
	while ( pThreadObject->IsRunning() ) {
		while( m_bPauseCapture && pThreadObject->IsRunning() ) {
			if ( m_PauseEvent.Wait( 0 ) == kWaitResult_Success ) break;
		}

		SendQPktsToOmni( pThreadObject );

		//	Display a message on the engine?
		//	PostDebugMsg( "PQ: Exception 1\n" );
	}
		
	// SendQPktsToOmniFinal( pThreadObject );

	PostDebugMsg( "Leaving ProcessQueue\n" );
	// The OmniAdapter will create and delete the ThreadObject.
	// The OmniAdapter creates and deletes the thread.
	// The ThreadObject holds the thread while capturing.

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessStatus
// -----------------------------------------------------------------------------

void
COmniAdapter::ProcessStatus()
{
	static const seconds	tenSeconds( 10 );
	static const seconds	oneSecond( 1 );

	UInt32	lastSN( 0xFFFFFFFF );
	UInt8*	pMsg = reinterpret_cast<UInt8*>( &m_pStatusLoc );

	// tpNow is set to the top of the next second.
	auto tpNow = std::chrono::time_point_cast<std::chrono::seconds>( high_resolution_clock::now() ) + oneSecond;
	high_resolution_clock::time_point	tpNext = std::chrono::time_point_cast<std::chrono::nanoseconds>( tpNow ) + tenSeconds;

	while ( m_bCapturing ) {
		if ( m_StopEvent.Wait( 500 ) == kWaitResult_Success ) return;

		if ( (m_hHspAdapter != INVALID_HANDLE_VALUE) && (high_resolution_clock::now() >= tpNext) ) {
			tpNext += tenSeconds;

			UInt32	sqnum( *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SEQ_NUM] ) ) );
			if ( sqnum != lastSN ) {
				lastSN = sqnum;
				m_CoProcSwRevOs = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SW_OS_TYPE] ) );
				m_CoProcSwRevKv = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SW_KRN_VER] ) );
				m_CoProcSwRevDv = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_EM_DRV_VER] ) );
				m_CoProcSwRevAv = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_EM_APP_VER] ) );
				m_CoProcSwRevRv = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_EM_RFS_VER] ) );

				m_fpga_rev = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_FPGA_VER] ) );
				
				UInt32 p0_frames_lo = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P0_FRMS_LO] ) );
				UInt32 p0_frames_hi = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P0_FRMS_HI] ) );
				m_p0_pkt_count = (static_cast<UInt64>( p0_frames_hi ) << 32) | static_cast<UInt64>( p0_frames_lo );
				
				UInt32 p0_bytes_lo = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P0_BYTES_LO] ));
				UInt32 p0_bytes_hi = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P0_BYTES_HI] ));
				m_p0_byte_count = (static_cast<UInt64>( p0_bytes_hi ) << 32) | static_cast<UInt64>( p0_bytes_lo );
				
				m_p0_crc_errors = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P0_ERROR] ));
				m_phy1gRate = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_CHAN_STAT] ));

				UInt32 p1_frames_lo = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P1_FRMS_LO] ));
				UInt32 p1_frames_hi = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P1_FRMS_HI] ));
				m_p1_pkt_count = (static_cast<UInt64>( p1_frames_hi ) << 32) | static_cast<UInt64>( p1_frames_lo );

				UInt32 p1_bytes_lo  = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P1_BYTES_LO] ));
				UInt32 p1_bytes_hi  = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P1_BYTES_HI] ));
				m_p1_byte_count = (static_cast<UInt64>( p1_bytes_hi ) << 32) | static_cast<UInt64>( p1_bytes_lo );

				m_p1_crc_errors = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_P1_ERROR] ) );

				UInt32 snt_frms_lo  = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SENT_FRM_LO] ));
				UInt32 snt_frms_hi  = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SENT_FRM_HI] ));
				m_tx0_pkt_count = (static_cast<UInt64>( snt_frms_hi ) << 32) | static_cast<UInt64>( snt_frms_lo );

				UInt32 snt_bytes_lo = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SENT_BYT_LO] ));
				UInt32 snt_bytes_hi = *(reinterpret_cast<UInt32 *>( &pMsg[HSP_STAT_MSG_OFFSET_SENT_BYT_HI] ));
				m_tx0_byte_count = (static_cast<UInt64>( snt_bytes_hi ) << 32) | static_cast<UInt64>( snt_bytes_lo );

				m_StatusEvent.Set();
			}
		}
	}

	return;
}


// -----------------------------------------------------------------------------
//		ProcessStatusMessage
// -----------------------------------------------------------------------------

void
COmniAdapter::ProcessStatusMessage(
	UInt8*	pMsg )
{	
	if ( pMsg[13] == static_cast<UInt8>( 0x92 ) ) {
		m_StatusEvent.Set();

		// PostDebugMsg( "Process Status Message.\n" );

		int ioff = 14;

		// rx0 pkts
		//memcpy(&m_p0_pkt_count, &pMsg[ioff], sizeof(UInt64));
		m_p0_pkt_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64);

		// rx0 bytes
		//memcpy(&m_p0_byte_count, &pMsg[ioff], sizeof(UInt64));
		m_p0_byte_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64);

		// rx1 pkts
		//memcpy(&m_p1_pkt_count, &pMsg[ioff], sizeof(UInt64));
		m_p1_pkt_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64);
		// rx1 bytes
		//memcpy(&m_p1_byte_count, &pMsg[ioff], sizeof(UInt64));
		m_p1_byte_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64);

		// tx0 pkts
		//memcpy(&m_tx0_pkt_count, &pMsg[ioff], sizeof(UInt64));
		m_tx0_pkt_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64); // tx0 fwd pkts

		// tx0 bytes
		//memcpy(&m_tx0_byte_count, &pMsg[ioff], sizeof(UInt64));
		m_tx0_byte_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64); // fwd bytes
		
		// tx1 pkts
		//memcpy(&m_tx1_pkt_count, &pMsg[ioff], sizeof(UInt64));
		m_tx1_pkt_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64); // tx1 fwd pkts
		
		// tx1 bytes
		//memcpy(&m_tx1_byte_count, &pMsg[ioff], sizeof(UInt64));
		m_tx1_byte_count = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64); // fwd bytes

		// crc0 errors
		//memcpy(&m_p0_crc_errors, &pMsg[ioff], sizeof(UInt32));
		m_p0_crc_errors = *((UInt32 *)&pMsg[ioff]);
		ioff += sizeof(UInt32);

		// crc1 errors
		//memcpy(&m_p1_crc_errors, &pMsg[ioff], sizeof(UInt32));
		m_p1_crc_errors = *((UInt32 *)&pMsg[ioff]);
		ioff += sizeof(UInt32);

		// rx_status
		m_rx_status = pMsg[ioff];
		ioff += sizeof(UInt8);

		// phy1g 0 rate
		m_phy1gRate = pMsg[ioff];
		ioff += sizeof(UInt8);

		// phy1g 1 rate
		ioff += sizeof(UInt8);

		// active sessions count
		//memcpy(&m_active_sessions, &pMsg[ioff], sizeof(UInt16));
		m_active_sessions = *((UInt16 *)&pMsg[ioff]);

		ioff += sizeof(UInt16);
			
		ioff += sizeof(UInt8); // buffer 0 status
		ioff += sizeof(UInt8); // buffer 1 status
		ioff += sizeof(UInt16); // radius session count

		// board type
		m_board_type = pMsg[ioff];
		ioff += sizeof(UInt8);

		// board major rev
		m_board_rev_maj = pMsg[ioff];
		ioff += sizeof(UInt8);

		// board minor rev
		m_board_rev_min = pMsg[ioff];
		ioff += sizeof(UInt8);

		// buffer overflows
		m_overflows = pMsg[ioff];
		ioff += sizeof(UInt8);

		// co-processor sw major rev
		m_dsp_sw_rev_maj = pMsg[ioff];
		ioff += sizeof(UInt8);

		// co-processor sw minor rev
		m_dsp_sw_rev_min = pMsg[ioff];
		ioff += sizeof(UInt8);

		// co-processor sw sub rev
		m_dsp_sw_rev_sub = pMsg[ioff];
		ioff += sizeof(UInt8);

		// filter capability
		m_filter_capability = pMsg[ioff];
		ioff += sizeof(UInt8);

		// FPGA rev
		//memcpy(&m_fpga_rev, &pMsg[ioff], sizeof(UInt64));
		m_fpga_rev = *((UInt64 *)&pMsg[ioff]);
		ioff += sizeof(UInt64);

		// max IP sessions
		//memcpy(&m_max_ip_sessions, &pMsg[ioff], sizeof(UInt16));
		m_max_ip_sessions = *((UInt16 *)&pMsg[ioff]);
		ioff += sizeof(UInt16);

		// max static IPs
		//memcpy(&m_max_static_ips, &pMsg[ioff], sizeof(UInt16));
		m_max_static_ips = *((UInt16 *)&pMsg[ioff]);
		ioff += sizeof(UInt16);

		// max static IP range (CIDRS)
		//memcpy(&m_max_ip_ranges, &pMsg[ioff], sizeof(UInt16));
		m_max_ip_ranges = *((UInt16 *)&pMsg[ioff]);
		ioff += sizeof(UInt16);

		// max pattern filter count
		ioff += sizeof(UInt16); // pad

		// string filter count
		//memcpy(&m_max_strings, &pMsg[ioff], sizeof(UInt32));
		m_max_strings = *((UInt32 *)&pMsg[ioff]);
		ioff += sizeof(UInt32);

		ioff += sizeof(UInt16); // max radius IP filter count
		ioff += sizeof(UInt16); // max radius IP session filter count
		ioff += sizeof(UInt16); // max radius string filter count
		ioff += sizeof(UInt8); // cam op mode

//		memcpy(&m_max_ports, &pMsg[ioff], sizeof(UInt16));
//		ioff += sizeof(UInt16);

//		ioff += sizeof(UInt8); // cam op mode

		m_dsp_sw_rev_release = pMsg[ioff];
		ioff += sizeof(UInt8);

		ioff += sizeof(UInt32); // rx0 pkt rate
		ioff += sizeof(UInt32); // rx1 pkt rate
		ioff += sizeof(UInt32); // tx0 pkt rate
		ioff += sizeof(UInt32); // tx1 pkt rate
		ioff += sizeof(UInt32); // rx0 byte rate
		ioff += sizeof(UInt32); // rx1 byte rate
		ioff += sizeof(UInt32); // tx0 byte rate
		ioff += sizeof(UInt32); // tx1 byte rate

		// new status fields (only for Remote Adapter)
		//memcpy(&m_static_ips_current, &pMsg[ioff], sizeof(UInt16));
		m_static_ips_current = *((UInt16 *)&pMsg[ioff]);
		ioff += sizeof(UInt16);

		//memcpy(&m_ranges_current, &pMsg[ioff], sizeof(UInt16));
		m_ranges_current = *((UInt16 *)&pMsg[ioff]);
		ioff += sizeof(UInt16);

		//memcpy(&m_strings_current, &pMsg[ioff], sizeof(UInt16));
		m_strings_current = *((UInt16 *)&pMsg[ioff]);

		ioff += sizeof(UInt16);

		//memcpy(&m_ports_current, &pMsg[ioff], sizeof(UInt16));
		m_ports_current = *((UInt16 *)&pMsg[ioff]);
	}
}


// -----------------------------------------------------------------------------
//		RemovePacketHandler
// -----------------------------------------------------------------------------

void
COmniAdapter::RemovePacketHandler(
	CPacketHandler	inPacketHandler )
{
#ifdef IMPLEMENT_NAVL
	CPeekAdapter::RemovePacketHandler( inPacketHandler );
#endif	// IMPLEMENT_NAVL

	CPeekStringA		strMsg;
	auto				itr = m_HandlerMap.find( inPacketHandler.GetPtr() );
	if ( itr != m_HandlerMap.end() ) {
		QueueThreadObject*	pThreadObject( itr->second );
		if ( pThreadObject ) {
			pThreadObject->StopRunning();
			strMsg.Format( "Quitting thread: 0x%p Captures: %zu\n",
				inPacketHandler.GetPtr(), static_cast<size_t>( m_nRunningCaptures ) );

			//WaitForSingleObject(pqto->GetThreadHandle(), 10000); 
	
			std::unique_ptr<std::thread>	spThread;
			pThreadObject->GetThread( spThread );
			if( spThread->joinable() ) {
				spThread->join();
			}
		}

		if ( m_nRunningCaptures == 0 ) {
			StopCaptureFromRemoveHandler();
		}

		m_HandlerMap.erase( itr );
		delete pThreadObject;
	}
	else {
		strMsg.Format( "Handler missing from collection: 0x%p\n", inPacketHandler.GetPtr() );
	}		
}


// -----------------------------------------------------------------------------
//		ResetPktCounts
// -----------------------------------------------------------------------------

void 
COmniAdapter::ResetPktCounts()
{
	m_nPacketCount = 0;
	m_crcErrors = 0;
	m_bytes = 0;
}


// -----------------------------------------------------------------------------
//      ResumeCapture
// -----------------------------------------------------------------------------

void
COmniAdapter::ResumeCapture()
{
	m_PauseEvent.Set();
	m_bPauseCapture = false;
//	fprintf(m_pDbgFile, "Capture resumed.\n");
//	fflush(m_pDbgFile);
}


// -----------------------------------------------------------------------------
//		Run
// -----------------------------------------------------------------------------

int
COmniAdapter::Run()
{
	int		nResult( PEEK_PLUGIN_SUCCESS );
	UInt8*	pBuf( nullptr );

	m_nPacketCount = 0;

	PostDebugMsg( "Fwd Thread is running\n" );

	while ( m_bCapturing ) {
#ifdef IMPLEMENT_NAVL
		// Handle NAVL on this thread
		if ( !DoPacketHandlers() ) return false;
#endif	// IMPLEMENT_NAVL

		ThreadHelpers::Event::WaitResult	wrResult = ThreadHelpers::Event::WaitMultiple(
			m_ayExitAndFwdEvents, false, 10000 );
		switch ( wrResult.m_nResult ) {
			case kWaitResult_Success:
				if ( wrResult.m_nIndex == 0 ) {
					PostDebugMsg( "Exit event triggered..." );
				}
				else if ( wrResult.m_nIndex == 1 ) {
					// PostDebugMsg( "Forward event triggered..." );
					m_ayExitAndFwdEvents[1]->Reset();
					ProcessFullBuffers();
				}
				break;

			case kWaitResult_Abandoned:
				PostDebugMsg( "Wait abandoned..." );
				break;

			case kWaitResult_Timeout:
				// TO DO: Figure out if this is helping or hurting. 
				//        We're doing it with the HSP10G but not sure if impact has been tested.
				//        WE SHOULD NOT NEED TO DO THIS AT ALL!!!!!!!!!!!!!
				// m_hExitAndFwdHandles[1].Set();

				// We must flush cache buffers in the kernel before touching data.
				{
					UInt32 fwdParams( GetFwdParams() ); //FlushMappedBuffer(m_bufIndex);
					UInt32 fwdBuf( fwdParams & 0x0000000F );
					UInt32 fwdOffset( fwdParams & 0xFFFFFFF0 );
					if ( fwdOffset != m_uiPartOffset ) {
						while ( static_cast<int>( fwdBuf ) != m_bufIndex ) {
							pBuf = m_DataBufs[m_bufIndex];
							ProcessDataBuffer( pBuf, m_bufIndex );
							++m_bufIndex;
							m_bufIndex %= DMA_XFER_BUFFER_COUNT;
						}

						pBuf = m_DataBufs[m_bufIndex];
						ProcessPartialBuffer(pBuf, m_bufIndex, fwdBuf, fwdOffset);
					}
				}
				break;
		}
	}

	PostDebugMsg( "Exiting normally...\n" );

	return nResult;
}


// -----------------------------------------------------------------------------
//		SearchForDelim
// -----------------------------------------------------------------------------

UInt8* 
COmniAdapter::SearchForDelim(
	UInt8*			pData,
	int*			offset,
	int				/* index */,
	bool*			/* pbCont */,
	unsigned short*	pPktSize)
{
	int				os( *offset );
	unsigned short	pktSize( 0 );
	while ( (DMA_XFER_BUFFER_SIZE - os) > ETHERNET_DELIMITER_SIZE ) {
	   pktSize = (UInt16)(((UInt16)pData[os + 2] << 8) | (UInt16)pData[os + 3]);

	   if ( ((UInt8)pData[os + 4] == (UInt8)0x55) &&
			((UInt8)pData[os + 5] == (UInt8)0x55) &&
			((UInt8)pData[os + 6] == (UInt8)0x55) &&
			((UInt8)pData[os + 7] == (UInt8)0xD5) ) {
			os += ETHERNET_DELIMITER_SIZE;
			*offset = os;
			*pPktSize = pktSize;
			return pData;
		}
		else {
			os += sizeof( int ); // ASSUMPTION: Delimiters begin on a 32-bit boundary.
		}
	}

	// We must check for boundary conditions at the end of buffers. 
	//
	// Case 1: (Full Delimiter)
	//    XX XX XX XX 55 55 55 D5 | (end of buffer)
	if ( (DMA_XFER_BUFFER_SIZE - os) == ETHERNET_DELIMITER_SIZE ) {
		// Partial delimiters could be found in the last 8 bytes of the buffer.
		// If we see any part of a delimiter, save delim UInt8 count and go to next buffer.
		if ( ((UInt8)pData[os + 4] == (UInt8)0x55) &&
			 ((UInt8)pData[os + 5] == (UInt8)0x55) &&
			 ((UInt8)pData[os + 6] == (UInt8)0x55) &&
			 ((UInt8)pData[os + 7] == (UInt8)0xD5) ) {
			m_lastPktSize = ((UInt16)(((UInt16)pData[os + 2] << 8) | (UInt16)pData[os + 3]));
			m_eType = PART_TYPE_DELIM_FULL;
			os = m_iOffset;
			*offset = os;

			return nullptr;
		}
		else
			os += sizeof(int);
	}

	// Case 2: (Half Delimiter)
	//    XX XX XX XX | (end of buffer)
	//    55 55 55 D5   (start of next buffer)
	if ( (DMA_XFER_BUFFER_SIZE - os) == sizeof( int ) ) {
		m_lastPktSize =  ((UInt16)(((UInt16)pData[os + 2] << 8) | (UInt16)pData[os + 3]));
		m_eType = PART_TYPE_DELIM_PART;
		*offset = 0;

		return nullptr;
	}

	// Don't reset the pointer here if you need the position
	// to be remembered by the partial buffer read.
	return nullptr;
}


// -----------------------------------------------------------------------------
//		SearchForDelimPartial
// -----------------------------------------------------------------------------

UInt8* 
COmniAdapter::SearchForDelimPartial(
	UInt8*			pData,
	int*			offset,
	int				/* index */,
	int				iEndOffset,
	bool*			/* pbCont */,
	unsigned short*	pPktSize)
{
	int				os( *offset );
	unsigned short	pktSize( 0 );

	while ( os < iEndOffset ) {
		pktSize = (UInt16)(((UInt16)pData[os + 2] << 8) | (UInt16)pData[os + 3]);

		if ( ((UInt8)pData[os + 4] == (UInt8)0x55) &&
			 ((UInt8)pData[os + 5] == (UInt8)0x55) &&
			 ((UInt8)pData[os + 6] == (UInt8)0x55) &&
			 ((UInt8)pData[os + 7] == (UInt8)0xD5) ) {
			os += ETHERNET_DELIMITER_SIZE;
			*offset = os;
			*pPktSize = pktSize;
			return pData;
		}
		else {
			os += sizeof( int ); // FPGA REQUIREMENT: Delimiters begin on a 32-bit boundary.
		}
	}

	*offset = os;
	return nullptr;
}


// -----------------------------------------------------------------------------
//		SendEventToDriver
// -----------------------------------------------------------------------------

bool 
COmniAdapter::SendEventToDriver(
	const CPeekStringA&	inEventName,
	UInt32				inControlCode )
{
	ASSERT( (inEventName.size() + 1) < MAX_PATH );
	UInt8	evtNameArray[MAX_PATH];
	memcpy( &evtNameArray[0], inEventName.c_str(), inEventName.size() + 1 );
//	size_t	nLen = strEventName.size();
//		for ( size_t strIndex = 0; strIndex < nLen; strIndex++ ) {
//			evtNameArray[strIndex] = static_cast<UInt8>( pEventName[strIndex] );
//		}
//		evtNameArray[nLen] = 0x00;

	// Set User/Kernel shared event name.
	UInt32	nBytesReturned( 0 );
	if ( !IoControl( inControlCode, evtNameArray, MAX_PATH, evtNameArray, MAX_PATH,
			&nBytesReturned ) ) {
		CPeekStringA	strEvtNameArray( reinterpret_cast<char*>( evtNameArray ) );
		CPeekStringA	strMsg;
		strMsg.Format( "Error setting event name %s: %d in kernel.",
			strEvtNameArray.c_str(), GetLastError() );
		PostDebugMsg( strMsg );
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		SendQPktsToOmni
// -----------------------------------------------------------------------------

void 
COmniAdapter::SendQPktsToOmni(
	QueueThreadObject*	pThreadObject )
{
	bool bFirst( true );

	if ( pThreadObject->m_reader_i != m_writer_i ) {
		pThreadObject->m_iNullRds = 0;

		UInt64 uitest = static_cast<UInt64>( (m_writer_i >= pThreadObject->m_reader_i)
			? (m_writer_i - pThreadObject->m_reader_i)
			: (m_writer_i + NUM_PKT_PTRS - pThreadObject->m_reader_i) );
		if ( uitest > pThreadObject->m_qDiff ) {
			pThreadObject->m_qDiff = uitest;
		}

		//	if ( uitest < ((NUM_PKT_PTRS * 6)/10) ) {
		//		pThreadObject->GetQueueEvent()->Set();
		//	}
		//	pThreadObject->m_filters = m_strings_current;

		int diff;
		for ( int i = 0; i < (int)uitest; i++ ) {
			pThreadObject->m_thePeekPacket.fProtoSpec = 0;
			pThreadObject->m_thePeekPacket.fFlags = 0;
			pThreadObject->m_thePeekPacket.fStatus = 0;
			pThreadObject->m_thePeekPacket.fSliceLength = 0;
			pThreadObject->m_thePeekPacket.fMediaSpecInfoBlock = nullptr;
			//	memset( &pThreadObject->m_thePeekPacket, 0, sizeof( PeekPacket ) );
			pThreadObject->m_thePeekPacket.fTimeStamp = (s_fnGetTimeStamp)
				? (*s_fnGetTimeStamp)()
				: (CPeekTime::Now()).i;

			while ( m_bPauseCapture && pThreadObject->IsRunning() ) {
				if ( m_PauseEvent.Wait( 0 ) == kWaitResult_Success ) break;
			}
			if ( !pThreadObject->IsRunning() ) break;

			pThreadObject->m_thePeekPacket.fPacketLength = static_cast<UInt16>( g_Packets[pThreadObject->m_reader_i]->size );

			HRESULT hr = pThreadObject->GetOmniSPacket().get()->Initialize( nullptr, 0, 
				&pThreadObject->m_thePeekPacket, g_Packets[pThreadObject->m_reader_i]->m_pPkt,
				kMediaType_802_3, kMediaSubType_Native );
			if ( HE_SUCCEEDED( hr ) ) {
				CPacketHandler PktHandler( pThreadObject->GetPacketHandler() );
				if ( !PktHandler.HandlePacket( pThreadObject->GetOmniSPacket().get() ) ) {
					m_PauseEvent.Reset();
					m_bPauseCapture = true;

					if ( bFirst ) {
						pThreadObject->AddLogMsg( "Capture paused." );
						bFirst = false;
					}

					while ( m_bPauseCapture && pThreadObject->IsRunning() ) {
						if ( m_PauseEvent.Wait( 0 ) == kWaitResult_Success ) {
							if ( pThreadObject->IsRunning() ) {
								if ( !PktHandler.HandlePacket( pThreadObject->GetOmniSPacket().get()) ) {
									pThreadObject->m_q_reads++;
									pThreadObject->m_Qpkts++;
									pThreadObject->m_Bytes += g_Packets[pThreadObject->m_reader_i]->size;

									// Compare bytes processed from primary queue to bytes forwarded to Omni.
									//	INT64 bdiff = (INT64)(((m_bytes - pThreadObject->m_Bytes) >= 0)
									//		? (INT64)(m_bytes - pThreadObject->m_Bytes)
									//		: (INT64)((UInt64)UInt64_MAX - pThreadObject->m_Bytes + m_bytes));

									//	if ( bdiff > (UInt64)DMA_XFER_BUFFER_TOTAL_SIZE ) {
									//		pThreadObject->m_prime_oruns++;
									//		pThreadObject->m_Bytes = m_bytes;
									//		CPeekStringA strMsg;
									//		strMsg.Format( "Primary Queue OVERRUN in thread %p at line %d. Lost ~%d bytes.", pThreadObject->m_pHandler, __LINE__, DMA_XFER_BUFFER_TOTAL_SIZE);
									//		pThreadObject->AddLogMsg( strMsg );
									//	}

									bFirst = true;
									diff = (m_q_writes >= pThreadObject->m_q_reads)
										? static_cast<int>( m_q_writes - pThreadObject->m_q_reads )
										: static_cast<int>( NUM_PKT_PTRS - pThreadObject->m_q_reads + m_q_writes );

									if ( diff > NUM_PKT_PTRS ) {
										pThreadObject->m_q_reads = m_q_writes;
										pThreadObject->m_oruns++;

										CPeekStringA	strMsg;
										strMsg.Format( "Secondary queue OVERRUN in thread %p at line %d. Lost ~%d packets.",
											pThreadObject->GetPacketHandler().GetPtr(), __LINE__,  NUM_PKT_PTRS );
										pThreadObject->AddLogMsg( strMsg );
									}
									break;
								}
								else {
									pThreadObject->AddLogMsg( "Send failed." );
								}
							}
							else {
								break;
							}
						}
					}
				}
				else {
					pThreadObject->m_q_reads++;
					pThreadObject->m_Qpkts++;
					pThreadObject->m_Bytes += g_Packets[pThreadObject->m_reader_i]->size;

					// Compare bytes processed from primary queue to bytes forwarded to Omni.
					//	INT64 bdiff = (INT64)(((m_bytes - pThreadObject->m_Bytes) >= 0)
					//		? (INT64)(m_bytes - pThreadObject->m_Bytes)
					//		: (INT64)((UInt64)UInt64_MAX - pThreadObject->m_Bytes + m_bytes));

					//	if ( bdiff > (UInt64)DMA_XFER_BUFFER_TOTAL_SIZE ) {
					//		pThreadObject->m_prime_oruns++;
					//		pThreadObject->m_Bytes = m_bytes;
					//		CPeekStringA strMsg;
					//		strMsg.Format( "Primary Queue OVERRUN in thread %p at line %d. Lost ~%d bytes.", pThreadObject->m_pHandler, __LINE__, DMA_XFER_BUFFER_TOTAL_SIZE );
					//		pThreadObject->AddLogMsg( strMsg );
					//	}

					bFirst = true;
					diff = (m_q_writes >= pThreadObject->m_q_reads)
						? static_cast<int>( m_q_writes - pThreadObject->m_q_reads )
						: static_cast<int>( NUM_PKT_PTRS - pThreadObject->m_q_reads + m_q_writes );

					if ( diff > NUM_PKT_PTRS ) {
						CPeekStringA	strMsg;
						strMsg.Format( "diff: %lld  QPkts: %lld Rds: %lld  Wrts: %lld\n", diff, pThreadObject->m_Qpkts, pThreadObject->m_q_reads, m_q_writes );
						PostDebugMsg( strMsg );

						pThreadObject->m_q_reads = m_q_writes;
						pThreadObject->m_oruns++;

						strMsg.Format( "Secondary Queue OVERRUN in thread %p at line %d. Lost ~%d packets. Wr: %lld Rd: %lld",
							pThreadObject->GetPacketHandler().GetPtr(), __LINE__, NUM_PKT_PTRS, m_q_writes, pThreadObject->m_q_reads );
						pThreadObject->AddLogMsg( strMsg );
					}
				}
			}
			else {
				pThreadObject->AddLogMsg( "Initialize failed." );
			}

			// go to next buffer after processing current
			++pThreadObject->m_reader_i;
			pThreadObject->m_reader_i = pThreadObject->m_reader_i % NUM_PKT_PTRS;
		}
	}
	else {
		if ( pThreadObject->m_iNullRds++ > MAX_NULL_READS ) {
			pThreadObject->GetQueueEvent()->Set();
			ThreadHelpers::Event::WaitMultiple( m_ayExitAndFwdEvents, false, 1 );
			pThreadObject->m_iNullRds = 0;
		}

#ifdef _DEBUG
		CPacketHandler PktHandler( pThreadObject->GetPacketHandler() );
		if ( PktHandler.IsValid() ) {
			CPeekCapture	capture = PktHandler.GetPeekCapture();
			if ( capture.IsValid() ) {
				UInt64	nPacketsDropped;
				if ( capture.GetProperty( L"PacketsDropped", &nPacketsDropped ) ) {
					if ( nPacketsDropped == 0 ) {
						srand( static_cast<unsigned int>( time( nullptr ) ) );
					}
					nPacketsDropped += rand() % 10;
					capture.SetProperty( L"PacketsDropped", nPacketsDropped );
				}
			}
		}
#endif
	}
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniAdapter::SetOptions(
	const CAdapterOptions&	/* inOptions */ )
{
}


// -----------------------------------------------------------------------------
//		StartAdapter
// -----------------------------------------------------------------------------

bool 
COmniAdapter::StartAdapter()
{
	UInt32	nBytesReturned( 0 );
	if ( !IoControl( IOCTL_START_HSP10G, nullptr, 0, nullptr, 0, &nBytesReturned ) ) {
		CPeekStringA	strMsg;
		strMsg.Format( "IOCTL_START_HSP10G ERROR : %d", GetLastError() );
		PostDebugMsg( strMsg );
		return false;
	}

	PostDebugMsg( "Started Adapter...\n" );

	m_oruns = 0;

	return true;
}


// -----------------------------------------------------------------------------
//		StartCapture
// -----------------------------------------------------------------------------

int
COmniAdapter::StartCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	m_qDiff = 0;
	m_Qpkts = 0;

	try {
		m_nRunningCaptures++;
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		StartCaptureFromAddHandler
// -----------------------------------------------------------------------------

int
COmniAdapter::StartCaptureFromAddHandler()
{
	PostDebugMsg( "StartCaptureFromAddHandler:\n" );

	m_eType = PART_TYPE_NONE;

	PostDebugMsg( "Resetting pkt counts.:\n" );
	ResetPktCounts();
	m_bCapturing = true;

	m_StopEvent.Create( nullptr );
	ConfigHspAdapter();

	// Start HSP Adapter operations.
	if ( !StartAdapter() ) {
		UnconfigHspAdapter();
		return -1;
	}

#ifdef _WIN32
	PostDebugMsg( "Loading library:\n" );
	if ( s_hPeekDll == nullptr ) {
		s_hPeekDll = LoadLibrary( L"peek.dll" );
		if ( s_hPeekDll != nullptr ) {
			s_fnGetTimeStamp = reinterpret_cast<GetTimeStampProc>( GetProcAddress( s_hPeekDll, "PeekGetTimeStamp" ) );
		}
	}
#else
#endif	

	m_CfgDone = TRUE;

	PostDebugMsg( "InitPktPtrs\n" );
	//InitPktPtrs();

	PostDebugMsg( "Creating pause event\n" );
	m_PauseEvent.Create( nullptr );
	m_PauseEvent.Set();
	m_bPauseCapture = false;

	PostDebugMsg( "Starting Packet thread\n" );
	m_CaptureThread = std::thread( PacketThread, this );

	PostDebugMsg( "Starting Status thread\n" );
	m_StatusThread = std::thread( StatusThread, this );

	return 0;
}


// -----------------------------------------------------------------------------
//		StopAdapter
// -----------------------------------------------------------------------------

bool 
COmniAdapter::StopAdapter()
{
	UInt32	nBytesReturned( 0 );
	if ( m_hHspAdapter != INVALID_HANDLE_VALUE ) {
		if ( !IoControl( IOCTL_STOP_HSP10G, nullptr, 0, nullptr, 0, &nBytesReturned ) ) {
			CPeekStringA	strMsg;
			strMsg.Format( "IOCTL_STOP_HSP10G ERROR : %d", GetLastError() );
			PostDebugMsg( strMsg );
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

int
COmniAdapter::StopCapture()
{
 	int	nResult = PEEK_PLUGIN_SUCCESS;
	try {
		m_nRunningCaptures--;
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		StopCaptureFromRemoveHandler
// -----------------------------------------------------------------------------

int
COmniAdapter::StopCaptureFromRemoveHandler()
{
	m_bCapturing = false;

	// Signal a stop
	m_StopEvent.Set();
	if ( m_CaptureThread.joinable() ) {
		m_CaptureThread.join();
	}
	if ( m_StatusThread.joinable() ) {
		m_StatusThread.join();
	}
	// ::WaitForSingleObject( m_hCaptureThread, 1000 );
	// ::WaitForSingleObject( m_hStatusThread, 2000 );

	m_StopEvent.Close();
	m_PauseEvent.Close();

	CloseEvents();

//	CleanupPktPtrs();

	// Stopping the adapter here has the side-effect of ending the HSP2G GUI's
	// communications with the board. 
//			StopAdapter();

//	UnconfigHspAdapter();

	PostDebugMsg( "StopCaptureFromRemoveHandler: Leaving\n" );

	return 0;
}


// -----------------------------------------------------------------------------
//		StorePktPart
// -----------------------------------------------------------------------------

void 
COmniAdapter::StorePktPart(
	UInt8*	pSegment,
	int		nBytes )
{
	m_preBytes = nBytes;
	// get bytes from previous buffer
	for ( int i = 0; i < nBytes; i++ ) {
		m_pktPart[i] = pSegment[i];
	}
}


// -----------------------------------------------------------------------------
//		UnconfigHspAdapter
// -----------------------------------------------------------------------------

void 
COmniAdapter::UnconfigHspAdapter()
{
	CloseEvents();
	CloseHspAdapter();
}
