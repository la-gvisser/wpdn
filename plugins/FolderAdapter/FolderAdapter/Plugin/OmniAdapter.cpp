// =============================================================================
//	OmniAdapter.cpp
// =============================================================================
//	Copyright (c) 2008-2018 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniAdapter.h"
#include "CaptureFile.h"
#include "ContextManager.h"
#include "Options.h"
#include <array>
#include <map>
#include <thread>

#ifndef _WIN32
#include <unistd.h>
#include <sys/file.h>
#include <sys/inotify.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/stat.h>

#define EVENT_SIZE      (sizeof (struct inotify_event))
#define EVENT_BUF_LEN   (1024 * ( EVENT_SIZE + 16 ))
#endif

#include <chrono>
using namespace std;
using namespace std::chrono;

using std::map;
using namespace HeLib;
using namespace FilePath;
using namespace CaptureFile;

extern CContextManagerPtr	GetContextManager();
extern GUID					g_PluginId;

#define PEEK_ADAPTER_QUERY_DROP_AFTER_STOP			0x00002000 // Adapter supports querying dropped packets after stop capture


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

CPeekString		COmniAdapter::s_strName( L"FolderAdapterOE" );
CPeekString		COmniAdapter::s_strDisplayName( L"Folder Adapter-OE" );
CPeekString		COmniAdapter::s_strAdapterName( L"Folder Adapter-OE" );
CPeekString		COmniAdapter::s_strFilesLabel( L"Files" );
CPeekString		COmniAdapter::s_strPacketsLabel( L"Packets" );
UInt64			COmniAdapter::s_nLinkSpeed = 1000000000L;

const GUID		g_idParentStat = { 0xA011C73C, 0xE58F, 0x4A9D, { 0x9F, 0xAE, 0x18, 0xC0, 0xA2, 0x2A, 0xE2, 0x79 } };
const GUID		g_idAdapter =    { 0xBB24A0CB, 0x33FA, 0x4AD1, { 0xB6, 0xE0, 0x94, 0x27, 0x33, 0x1B, 0x50, 0xBE } };
const GUID		g_idFileStat =   { 0xCB91B06C, 0x2531, 0x4BC6, { 0x93, 0x38, 0x8F, 0x8C, 0xEA, 0x9C, 0x48, 0x4F } };
const GUID		g_idPacketStat = { 0xD0F26C91, 0x847E, 0x41DC, { 0x8E, 0x98, 0xEF, 0xBC, 0x45, 0x18, 0xEF, 0xB9 } };

const size_t	g_nPathLen( 1024 );

const UInt64	g_timeTimeout( 10 * WPTIME_SECONDS );

CPeekString		g_ayErrorLable[CF_ERROR_MAX] = {
	L"None",	// CF_ERROR_NONE
	L"EOF",		// CF_ERROR_EOF
	L"UNX-EOF",	// CF_ERROR_UNEXPECTED_EOF
	L"INC-HDR",	// CF_ERROR_INCOMPLETE_HEADER
	L"INV-HDR",	// CF_ERROR_INVALID_HEADER
	L"INV-OFF",	// CF_ERROR_DATA_OFFSET
	L"INC-PAY",	// CF_ERROR_INCOMPLETE_PAYLOAD
	L"NON-PKT",	//	CF_ERROR_UNHANDLED_PACKET
};



// -----------------------------------------------------------------------------
//      FilePacketToPacket
// -----------------------------------------------------------------------------

CPacket
FilePacketToPacket(
	tMediaType			inMediaType,
	const CFilePacket&	inPacket )
{
	PeekPacket	pktHeader;
	pktHeader.fTimeStamp = inPacket.GetTimeStamp();
	pktHeader.fProtoSpec = 0;
	pktHeader.fFlags = inPacket.GetFlags();
	pktHeader.fStatus = inPacket.GetStatus();
	pktHeader.fPacketLength = static_cast<UInt16>( inPacket.GetLength() );
	pktHeader.fSliceLength = static_cast<UInt16>( inPacket.GetSliceLength() );
	pktHeader.fMediaSpecInfoBlock = nullptr;
	memset( &pktHeader.fAppID, 0, sizeof( pktHeader.fAppID ) );
	pktHeader.fFlowID = 0;

	CPacket	pkt( &pktHeader, inPacket.GetPacketData(), inMediaType );
	return pkt;
}


// =============================================================================
//		PacketThread
// =============================================================================

void
PacketThread(
	void*	lpParam )
{
	COmniAdapter*	pOmniAdapter = reinterpret_cast<COmniAdapter*>( lpParam );
	if ( pOmniAdapter != nullptr ) {
		pOmniAdapter->Run();
	}
}


// =============================================================================
//		COmniAdapter
// =============================================================================

COmniAdapter::COmniAdapter(
	 const CGlobalId&	inId )
	:	m_Id( inId )
	,	m_StopFlag( false )
	,	m_nRunningCaptures( 0 )
	,	m_bDontCheckFileOpen( true )
{
}

COmniAdapter::COmniAdapter()
	:	COmniAdapter( GUID_NULL )
{
}

COmniAdapter::~COmniAdapter()
{
	if ( m_nRunningCaptures > 0 ) {
		m_nRunningCaptures = 1;
		StopCapture();
	}
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
//		CreateAdapterInfo											[static]
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
		paiDefault.SetPluginName( s_strName );
		paiDefault.SetPersistent( false );	// adapter manager will not save on shutdown (we have to do the work)
		paiDefault.SetTitle( s_strDisplayName );
		paiDefault.SetIdentifier( inId );
		paiDefault.SetMediaType( mt );
		paiDefault.SetDescription( inDescription );
//		paiDefault.SetFeatures( /* PEEK_ADAPTER_QUERY_DROP_AFTER_STOP | */ PEEK_ADAPTER_SETS_PER_CAPTURE_DROP );
		paiDefault.SetLinkSpeed( s_nLinkSpeed );
		paiDefault.SetDeletable( true );

		CHePtr<IMediaInfo> spMediaInfo;
		if ( HE_SUCCEEDED( spMediaInfo.CreateInstance( "PeekCore.MediaInfo" ) ) ) {
			(void)spMediaInfo->SetMediaType( mt.fType );
			(void)spMediaInfo->SetMediaSubType( mt.fSubType );
			(void)spMediaInfo->SetMediaDomain( kMediaDomain_Null );
			(void)spMediaInfo->SetLinkSpeed( s_nLinkSpeed );

			CMediaInfo mi( spMediaInfo );
			paiDefault.SetMediaInfo( mi );

			spMediaInfo.Release();
		}
	}

	return aiNew;
}


// -----------------------------------------------------------------------------
//		GetOptions
// -----------------------------------------------------------------------------

CAdapterOptions
COmniAdapter::GetOptions() const
{
	wstring	strMask;
	for ( wstring str : m_ayMask ) {
		strMask += str + L", ";
	}
	strMask.resize( strMask.size() - 2 );

	CAdapterOptions	options;
	options.SetId( m_Id );
	options.SetMonitor( m_pathMonitor.get() );
	options.SetMask( strMask );
	options.SetSave( m_pathSave.get() );
	options.SetSpeed( m_nSpeed );
	options.SetDontCheckFileOpen( m_bDontCheckFileOpen );

	return options;
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

	m_ParentStat = g_idParentStat;

	m_Stats.Adapter.Id = g_idAdapter;
	m_Stats.Adapter.Stat.Type = Peek::kSummaryStatType_Null;
	m_Stats.Adapter.Stat.Flags = 0;
	m_Stats.Adapter.Stat.Value.IntValue = 0;

	m_Stats.FileStat.Id = g_idFileStat;
	m_Stats.FileStat.Stat.Type = Peek::kSummaryStatType_Int;
	m_Stats.FileStat.Stat.Flags = 0;
	m_Stats.FileStat.Stat.Value.IntValue = 0;

	m_Stats.PacketStat.Id = g_idPacketStat;
	m_Stats.PacketStat.Stat.Type = Peek::kSummaryStatType_Packets;
	m_Stats.PacketStat.Stat.Flags = 0;
	m_Stats.PacketStat.Stat.Value.Packets = 0;

	return true;
}

#ifdef HE_LINUX
// -----------------------------------------------------------------------------
//		IsFileOpen
// -----------------------------------------------------------------------------

bool
COmniAdapter::IsFileOpen(
	const CFileDetail&	inFileDetail )
{
	struct rlimit	rlp = { 0, 0 };
	int	  ret = getrlimit( RLIMIT_NOFILE, &rlp );
	if ( ret == 0 ) {
		CPeekStringA	strPath( "/proc/self/fd/" );
		for ( rlim_t i = 3; i < rlp.rlim_cur; ++i ) {
			CPeekStringA	strNo( std::to_string( i ) );
			CPeekStringA	strFullPath( strPath + strNo );
			struct stat buffer;
			if ( stat( strFullPath.c_str(), &buffer ) == 0 ) {
				char	szFileName[PATH_MAX];
				size_t	r = readlink( strFullPath.c_str(), szFileName, PATH_MAX );
				if ( r > 0 ) {
					return true;
				}
			}
		}
	}

	return false;
}
#endif // HE_LINUX


// -----------------------------------------------------------------------------
//		MoveDeleteFile
// -----------------------------------------------------------------------------

void
COmniAdapter::MoveDeleteFile(
	const CFileDetail&	inFileDetail )
{
	bool			bMovedFile( false );
	Path			pathSource( m_pathMonitor, inFileDetail.GetName() );
	CPeekString		strSource = pathSource.get();

	if ( IsSave() ) {
		Path		pathDestination( m_pathSave, inFileDetail.GetName() );
		CPeekString	strDestination( pathDestination.get() );
		bMovedFile = CFileEx::MoveFile( strSource, strDestination );

		if ( !bMovedFile ) {
#ifdef _WIN32
			DWORD	dwError( ::GetLastError() );
#else
			UInt32	dwError( 1 );
#endif
			if ( dwError != 0 ) {
				// Rename the file in-place with new numeric extension.
				for ( int j = 0; j < 10000; ++j ) {
					CPeekString	strNewPath( strDestination );
					strNewPath.append( L"." );

					CPeekString	strExt( std::to_wstring( j ) );
					switch ( strExt.size() ) {
						case 1:
							strNewPath.append( L"00" );
							break;
						case 2:
							strNewPath.append( L"0" );
							break;
					}
					strNewPath.append( strExt.c_str() );

					bMovedFile = CFileEx::MoveFile( strSource, strNewPath );
					if ( bMovedFile ) {
						strDestination = strNewPath;
						break;
					}
				}
			}
		}

		CPeekString	strMsg( L"FolderAdapterOE: " );
		if ( bMovedFile ) {
			strMsg.Append( L"Archived " );
			strMsg.Append( strSource.c_str() );
			strMsg.Append( L" to " );
			strMsg.Append( strDestination );
		}
		else {
			strMsg.Append( L"Failed to archived " );
			strMsg.Append( strSource.c_str() );
		}
		const wchar_t*	szMsg( strMsg.c_str() );
		DoLogMessage( 0, 0, szMsg, szMsg );
	}

	// If the file was moved, then DeleteFile will fail.
	if ( CFileEx::DeleteFile( strSource ) ) {
		CPeekString	strMsg( L"FolderAdapterOE: Deleted " );
		strMsg.append( strSource.c_str() );
		DoLogMessage( 0, 0, strMsg.c_str(), strMsg.c_str() );
	}
	else if ( !bMovedFile ) {
#if defined(_WIN32) && defined(_DEBUG)
		DWORD	dwError( ::GetLastError() );
		if ( dwError != 0 ) {
			(void) dwError;
		}
#endif
		// Only Log the error if the file was not moved.
		CPeekString	strMsg( L"FolderAdapterOE: Failed to deleted " );
		strMsg.append( strSource.c_str() );
		DoLogMessage( 0, 0, strMsg.c_str(), strMsg.c_str() );
	}
}


// -----------------------------------------------------------------------------
//		MoveFileList
// -----------------------------------------------------------------------------

void
COmniAdapter::MoveFileList(
	const CFileDetailList&	inFileList )
{
	size_t	nCount( inFileList.GetCount() );
	for ( size_t i = 0; i < nCount; ++i ) {
		const CFileDetail&	fdNext( inFileList[i] );
		MoveDeleteFile( fdNext );
	}
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


// -----------------------------------------------------------------------------
//		OnUpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniAdapter::OnUpdateSummaryStats(
	CSnapshot inSnapshot )
{
	if ( !inSnapshot.HasItem( m_ParentStat ) ) {
		Peek::SummaryStat	statGroup;
		statGroup.Type = Peek::SummaryStatType::kSummaryStatType_Null;
		statGroup.Flags = Peek::SummaryStatFlags::kSummaryStats_SortSubItems;
		statGroup.Value.IntValue = 0;
		inSnapshot.SetItem( m_ParentStat, GUID_NULL, s_strAdapterName, &statGroup );
	}
	if ( !inSnapshot.HasItem( m_Stats.Adapter.Id ) ) {
		inSnapshot.SetItem( m_Stats.Adapter.Id, m_ParentStat, m_pathMonitor.get().c_str(), &m_Stats.Adapter.Stat );
	}
	if ( !inSnapshot.HasItem( m_Stats.PacketStat.Id ) ) {
		inSnapshot.SetItem( m_Stats.PacketStat.Id, m_ParentStat, s_strPacketsLabel, &m_Stats.PacketStat.Stat );
	}
	if ( !inSnapshot.HasItem( m_Stats.FileStat.Id ) ) {
		inSnapshot.SetItem( m_Stats.FileStat.Id, m_ParentStat, s_strFilesLabel, &m_Stats.FileStat.Stat );
	}
	inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.PacketStat );
	inSnapshot.UpdateSummaryStat( m_ParentStat, m_Stats.FileStat );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		PerformanceDelay
// -----------------------------------------------------------------------------

void
COmniAdapter::PerformanceDelay(
	SInt64	inNanoseconds )
{
	if ( inNanoseconds <= 0 ) return;

	// Windows uses the QueryPerformanceCounter for high_resolution_clock.
	seconds								durationOneSecond( 1 );
	nanoseconds							durationWait( inNanoseconds );
	high_resolution_clock::time_point	timeEnd = high_resolution_clock::now() + durationWait;
	high_resolution_clock::time_point	timeClose = timeEnd - (durationOneSecond * 2);

	while ( high_resolution_clock::now() < timeClose ) {
		PlatformSleep( 1000 );
		if ( m_StopFlag ) return;
	}

	while ( timeEnd > high_resolution_clock::now() ) {
		if ( m_StopFlag ) break;
	}
}


// -----------------------------------------------------------------------------
//		ProcessFile
// -----------------------------------------------------------------------------

void
COmniAdapter::ProcessFile(
	const CFileDetail&	inFileDetail )
{
#ifdef _DEBUG
	const wchar_t*	szFileName( inFileDetail.GetName().c_str() ); (void) szFileName;
#endif

	Path				pathFile( m_pathMonitor, inFileDetail.GetName() );
	const CPeekString	strPathFile( pathFile.get() );

#ifdef _DEBUG
	const wchar_t*	szPathFile( strPathFile.c_str() ); (void) szPathFile;
#endif

	CCaptureFile	pktReader( strPathFile, CF_TYPE_READER );
	if ( !pktReader.IsOpen() ) {
		// Can't open or parse the file.
		// Don't rename or delete the file.
		// The file will be retried until it is removed.
		return;
	}

	++m_Stats.FileStat.Stat.Value.IntValue;

	tMediaType	mtFile = pktReader.GetMediaType();
	CFilePacket	filePkt;
	UInt64		nPackets = 0;
	UInt32		nError = 0;
	UInt64		nPreviousPacketTime = 0;
	SInt64		nDelay = (m_nSpeed > 0) ? (1000000000 / m_nSpeed) : 0;
	std::vector<UInt32>	ayErrors;
	CPeekTime	timeFirstRetry;

	bool	bDone = false;
	while ( !bDone ) {
		if ( pktReader.ReadFilePacket( filePkt, nError, static_cast<UInt32>( NEXT_PACKET ) ) ) {
			timeFirstRetry.Reset();
			CPacket	pkt( FilePacketToPacket( mtFile, filePkt ) );

			// If nPacketRate is greater or equal to 0, then regulate the playback speed.
			// Otherwise play packets as fast possible (nPacketRate < 0).
			if ( m_nSpeed >= 0 ) {
				// If packetRate is set to 0, play packets at captured rate
				if ( m_nSpeed == 0 ) {
					// Calculate time to wait to send this packet
					UInt64	nCurrentPacketTime = pkt.GetTimeStamp();
					nDelay = (nPreviousPacketTime == 0)
						? 0
						: (nCurrentPacketTime - nPreviousPacketTime);
					nPreviousPacketTime = nCurrentPacketTime;
				}
				PerformanceDelay( nDelay );
			}

			if ( !m_StopFlag ) {
				CHeCritSecLock<CHeCriticalSection> lock( &m_csHandlersMutex );
				UInt32 nHandled = 0;
				UInt32 nFailed = 0;
				m_ayPacketHandlers.HandlePacket( pkt, nHandled, nFailed );
				if ( nFailed == 0 ) {
					++nPackets;
				}
				else {
					ayErrors.push_back( CF_ERROR_UNHANDLED_PACKET );
				}

				++m_Stats.PacketStat.Stat.Value.Packets;
			}
		}
		else {
			if ( nError == CF_ERROR_EOF ) {
				PlatformSleep( 1 );
				bDone = pktReader.IsEOF();
				if ( bDone) {
					PlatformSleep( 1 );
					bDone = pktReader.IsEOF();
				}
			}
			else if ( nError > CF_ERROR_EOF ) {
				ayErrors.push_back( nError );
				if ( (nError == CF_ERROR_INCOMPLETE_HEADER) || (nError == CF_ERROR_INCOMPLETE_PAYLOAD) ) {
					PlatformSleep( 1 );
				}
				else {
					bDone = true;
				}
			}
			if ( !bDone && (timeFirstRetry == 0) ) {
				timeFirstRetry.SetNow();
			}
		}

		if ( timeFirstRetry != 0 ) {
			CPeekTime timeNow( true );
			if ( (timeNow - timeFirstRetry) > g_timeTimeout ) break;
		}
		bDone |= m_StopFlag;
	}

	pktReader.Close();

	CPeekString	strMsg( L"FolderAdapterOE: Processed " );
	strMsg.append( strPathFile );
	strMsg.append( L" - " );
	strMsg.append( std::to_wstring( nPackets ).c_str() );
	strMsg.append( L" packets." );
	DoLogMessage( 0, 0, strMsg.c_str(), strMsg.c_str() );

	if ( !ayErrors.empty() ) {
		std::array<UInt64, CF_ERROR_MAX>	ayCounts{ 0 };
		for ( UInt32& i: ayErrors ) {
			++ayCounts[i];
		}
		size_t	nErrorCount( ayErrors.size() );
		CPeekOutString	ssmMsg;
		ssmMsg << L"FolderAdapteOE: File error count: " << std::to_wstring( nErrorCount ).c_str() << L" (";
		for ( size_t i = 0; i < ayCounts.size(); ++i ) {
			if ( ayCounts[i] > 0 ) {
				ssmMsg << L" " << g_ayErrorLable[i] << L":" << std::to_wstring( ayCounts[i] );
			}
		}
		ssmMsg << L")";
		strMsg = ssmMsg;
		DoLogMessage( 0, 0, strMsg.c_str(), strMsg.c_str() );
	}

	MoveDeleteFile( inFileDetail );
}


// -----------------------------------------------------------------------------
//		ProcessFileList
// -----------------------------------------------------------------------------

void
COmniAdapter::ProcessFileList(
	const CFileDetailList&	inFileList )
{
	for ( auto itr = inFileList.begin(); itr != inFileList.end(); ++itr ) {
#ifdef HE_LINUX
		if ( m_bDontCheckFileOpen || !IsFileOpen( *itr ) ) {
			ProcessFile( *itr );
		}
#else
		ProcessFile( *itr );
#endif
	}
}


// -----------------------------------------------------------------------------
//		Run
// -----------------------------------------------------------------------------

int
COmniAdapter::Run()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	m_Stats.FileStat.Stat.Value.IntValue = 0;
	m_Stats.PacketStat.Stat.Value.Packets = 0;

	try {
		int					nNoteFailure( 1 );
		const CPeekString	strMonitor( m_pathMonitor.get() );

		for ( CPacketHandler& pktHandler : m_ayPacketHandlers ) {
			CHeQIPtr<IPacketProviderEvents>	spEvents( pktHandler.GetPtr() );
			if ( spEvents.get() != nullptr ) {
				(void) spEvents->OnStartPacketProvider();
			}
		}

#ifdef _WIN32
		HANDLE	hChange( FindFirstChangeNotification( strMonitor.c_str(), FALSE,
					FILE_NOTIFY_CHANGE_CREATION ) );
		if ( hChange == INVALID_HANDLE_VALUE ) {
			const wchar_t*	pszMsg = L"FolderAdapteOE: Failure from first FindNextChangeNotification";
			DoLogMessage( 0, peekSeveritySevere, pszMsg, pszMsg );
		}
#endif
#ifdef HE_LINUX
		int	fd( inotify_init() );
		if ( fd < 0 ) {
			const wchar_t*	pszMsg = L"FolderAdapteOE: Failure from inotify_init()";
			DoLogMessage( 0, peekSeveritySevere, pszMsg, pszMsg );
		}

		fd_set	set;
		FD_ZERO( &set );	// clear the set
		FD_SET( fd, &set );	// add our file descriptor to the set
		const CPeekStringA	strMonitorA( strMonitor );
		int					wd( inotify_add_watch( fd, strMonitorA.c_str(), IN_CREATE ) );
#endif

		// Loop until capture stops.
		while( !m_StopFlag ) {
			try {
				CFileDetailList	ayFileDetails;
				for ( wstring strMask : m_ayMask ) {
					GetFileDetailList( strMonitor, strMask, ayFileDetails );
				}
				if ( !ayFileDetails.empty() ) {
					std::sort( ayFileDetails.begin(), ayFileDetails.end(), [](const CFileDetail& a, const CFileDetail& b) {
						return (a.GetTime() < b.GetTime());
					});
					ProcessFileList( ayFileDetails );
				}
			}
			catch ( ... ) {
				CPeekString	strMsg( L"FolderAdapteOE: Caught exception processing files in " );
				strMsg.append( strMonitor.c_str() );
				DoLogMessage( 0, peekSeveritySevere, strMsg.c_str(), strMsg.c_str() );
			}

#ifdef _WIN32
			DWORD dwWaitStatus = WaitForSingleObject( hChange, 500 );	// 1/2 second.
			if ( dwWaitStatus == WAIT_OBJECT_0 ) {
				if ( FindNextChangeNotification( hChange ) == FALSE ) {
					if ( --nNoteFailure == 0 ) {
						const wchar_t*	pszMsg = L"FolderAdapteOE: Failure from FindNextChangeNotification";
						DoLogMessage( 0, peekSeveritySevere, pszMsg, pszMsg );
						nNoteFailure = 20;	// 20 * 500ms = 10 seconds... (but not really).
					}
				}
			}
#endif
#ifdef HE_LINUX
			struct timeval	timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 500000;	// 1/2 second.

			FD_SET( fd, &set );

			int rv = select( (fd + 1), &set, nullptr, nullptr, &timeout );
			// rv == 0 is a timeout.
			if ( rv == -1 ) {
				if ( --nNoteFailure == 0 ) {
					const wchar_t*	pszMsg = L"FolderAdapteOE: Failure from select()";
					DoLogMessage( 0, peekSeveritySevere, pszMsg, pszMsg );
					nNoteFailure = 20;	// 20 * 500ms = 10 seconds... (but not really).
				}
			}
			else if ( rv > 0 ) {
				char	buffer[EVENT_BUF_LEN];
				int		length = read( fd, buffer, EVENT_BUF_LEN );
				if ( length < 0 ) {
					if ( --nNoteFailure == 0 ) {
						const wchar_t*	pszMsg = L"FolderAdapteOE: Failure from read()";
						DoLogMessage( 0, peekSeveritySevere, pszMsg, pszMsg );
						nNoteFailure = 20;	// 20 * 500ms = 10 seconds... (but not really).
					}
				}
				else {
					int i( 0 );
					while ( i < length ) {
						struct inotify_event *event = (struct inotify_event*)&buffer[ i ];
						i += EVENT_SIZE + event->len;
#if 0
						if ( event->len ) {
							if ( event->mask & IN_CREATE ) {
								if ( event->mask & IN_ISDIR ) {
								    // printf( "New directory %s created.\n", event->name );
								}
								else {
								    // printf( "New file %s created.\n", event->name );
								}
							}
						}
#endif
					}
				}
			}
#endif
		}

#ifdef _WIN32
		FindCloseChangeNotification( hChange );
#endif
#ifdef HE_LINUX
		inotify_rm_watch( fd, wd );
		close( fd );
#endif

		for ( CPacketHandler& pktHandler : m_ayPacketHandlers ) {
			CHeQIPtr<IPacketProviderEvents>	spEvents( pktHandler.GetPtr() );
			if ( spEvents.get() != nullptr ) {
				(void) spEvents->OnStopPacketProvider();
			}
		}
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniAdapter::SetOptions(
	const CAdapterOptions&	inOptions )
{
	if ( m_Id.IsNull() ) {
		m_Id = inOptions.GetId();
	}
	else {
		_ASSERTE( m_Id == inOptions.GetId() );
	}
	m_pathMonitor = inOptions.GetMonitor();
	if ( !m_pathMonitor.empty() ) {
		CFileEx::MakePath( m_pathMonitor.get() );
	}
	m_strName = m_pathMonitor.get();

	CPeekString	strMask = inOptions.GetMask();
	size_t	nOffset = 0;
	CPeekString	str = strMask.Tokenize( L",", nOffset );
	m_ayMask.clear();
	while ( nOffset != wstring::npos ) {
		m_ayMask.push_back( str.Trim() );
		str = strMask.Tokenize( L",", nOffset );
	}

	m_nSpeed = inOptions.GetSpeed();
	m_pathSave = inOptions.GetSave();
	if ( !m_pathSave.empty() ) {
		CFileEx::MakePath( m_pathSave.get() );
	}
	m_bDontCheckFileOpen = inOptions.IsDontCheckFileOpen();

	m_AdapterInfo.SetDescription( m_strName );
}


// -----------------------------------------------------------------------------
//		StartCapture
// -----------------------------------------------------------------------------

int
COmniAdapter::StartCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		if ( m_nRunningCaptures == 0 ) {
			m_StopFlag = false;
			m_CaptureThread = std::thread( PacketThread, this );
		}
		++m_nRunningCaptures;
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

int
COmniAdapter::StopCapture()
{
 	int	nResult = PEEK_PLUGIN_SUCCESS;
	try {
		--m_nRunningCaptures;
		if ( m_nRunningCaptures == 0 ) {
			m_StopFlag = true;
			if ( m_CaptureThread.joinable() ) {
				m_CaptureThread.join();
			}
		}
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		UpdateOptions
// -----------------------------------------------------------------------------

bool
COmniAdapter::UpdateOptions(
	const CAdapterOptions&	inOptions )
{
	bool bResult( false );

	if ( IsCapturing() ) {
		size_t	nRunningCaptures = m_nRunningCaptures;
		m_nRunningCaptures = 1;
		if ( StopCapture() != PEEK_PLUGIN_SUCCESS ) {
			m_nRunningCaptures = nRunningCaptures;
			return false;
		}

		SetOptions( inOptions );

		bResult = (StartCapture() == PEEK_PLUGIN_SUCCESS);
		m_nRunningCaptures = nRunningCaptures;
	}
	else {
		SetOptions( inOptions );
		bResult = true;
	}

	return bResult;
}
