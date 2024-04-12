// ============================================================================
// CMIFilterContext.cpp:
//      implementation of the CCMIFilterContext class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "CMIFilterContext.h"
#include "Memutil.h"
#include "XMutex.h"
#include "ByteStream.h"
#include "MediaSpec.h"
#include "MediaSpecUtil.h"
#include "FileUtil.h"
#include "PeekPacket.h"
#include "PeekFile.h"
#include "TimeConv.h"
#include "ParentStatic.h"
#include "OptionsStatus.h"
#include "OptionsDlg.h"
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if _DEBUG
#define X_DEBUG_INCR(x)	x++
#else
#define X_DEBUG_INCR(x)
#endif

#define kLogFilename   _T( "ErrorLog.txt" )

const UInt8		kProtocol_UDP( 17 );


#if OPT_EVENT
// -----------------------------------------------------------------------------
//		CaptureThreadProc
// -----------------------------------------------------------------------------

UINT
CaptureThreadProc(
	void*	pParam )
{
	ASSERT( pParam );
	if ( pParam == NULL ) return 0;

	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( pParam );
	//pContext->RunCaptureThread();
	return 0;
}
#endif


// ============================================================================
//		CCMIFilterContext
// ============================================================================

CCMIFilterContext::CCMIFilterContext(
	CCMIFilterPlugin*	pPlugin )
	:	CPeekContext()
	,	m_pPlugin( pPlugin )
	,	m_hTabWnd( NULL )
	,	m_pTabParent( NULL )
	,	m_pOptionsTab( NULL )
	,	m_bFailure( false )
	,	m_MonitorMode( kMode_CaptureThread )
#if OPT_EVENT
	,	m_hEvent( NULL )
	,	m_pThread( NULL )
#endif
	,	m_bCapturing( false )
	,	m_pInsertedPacket( NULL )
#if _DEBUG
	,	m_PacketNumber( 0 )
	,	m_nOrphanFiles( 0 )
	,	m_nOrphans( 0 )
	,	m_nPackets( 0 )
	,	m_nPacketsCreated( 0 )
	,	m_nPacketsDeleted( 0 )
	,	m_nPacketsDeletedInProcess( 0 )
	,	m_nPacketsDeletedInInsert( 0 )
	,	m_nInsertErrors( 0 )
	,	m_nIPIDOrphans( 0 )
#endif
{
	CFragmentMapPtr	spFragMap;
	CAddressMapPtr	spAddrMap;
	if ( GetMaps( spFragMap, spAddrMap ) ) {
		spFragMap->InitHashTable( 197 );
		spAddrMap->InitHashTable( 197 );
	}
}

CCMIFilterContext::~CCMIFilterContext()
{
	OnStopCapture( NULL );
	delete m_pTabParent;
	delete m_pOptionsTab;
}


// -----------------------------------------------------------------------------
//		BuildNewPacket
// -----------------------------------------------------------------------------

bool
CCMIFilterContext::BuildNewPacket(
	COptions*				inOptions,
	const CIPv4PacketList*	inPacketList,
	CByteArray&				inData,
	CAutoPtr<CPacket>&		outPacket )
{
	ASSERT( inOptions );
	ASSERT( inPacketList );
	if ( !inOptions || !inPacketList ) return false;
	if ( inPacketList->IsEmpty() ) return false;

	// Calculate the size of our new packet
	UInt16	nNewSize = static_cast<UInt16>( inData.GetCount() + 4 );

	// If Insert MAC is enabled add the Ethernet Header.
	if ( inOptions->IsInsertMAC() ) {
		nNewSize += sizeof( tEthernetHeader );
	}

	// If VLAN is enabled add the VLAN Header
	if ( inOptions->IsInsertVLAN() ) {
		nNewSize += sizeof( tVLANHeader );
	}

	// Get the address of the new data.
	const UInt8*	pNewData( inData.GetData() );
	UInt16			nNewDataSize( static_cast<UInt16>( inData.GetCount() ) );

	// Get the first packet from the packet list.
	const CPacket*	pPacketTemplate = inPacketList->GetAt( 0 );
	if ( pPacketTemplate == NULL ) return false;

	// Create a new Packet from the PacketRef.
	CPacket*	pNewPacket = new CPacket(
		pPacketTemplate->GetProtoSpecMatched(),
		pPacketTemplate->GetMediaType(),
		pPacketTemplate->GetMediaSubType(),
		pPacketTemplate->GetPacket() );
	ASSERT( pNewPacket );
	if ( pNewPacket == NULL ) return false;

	// Get the address of the packet data.
	UInt8*	pPacketData( pNewPacket->LockPacketData() );
	ASSERT( pPacketData );
	if ( pPacketData == NULL ) {
		delete pNewPacket;
		return false;
	}

	tEthernetHeader*	pEthernetHeader( reinterpret_cast<tEthernetHeader*>( pPacketData ) );
	if ( inOptions->IsInsertMAC() ) {
		inOptions->GetDstMAC().Duplicate( 6, reinterpret_cast<UInt8*>( &pEthernetHeader->Destination ) );
		inOptions->GetSrcMAC().Duplicate( 6, reinterpret_cast<UInt8*>( &pEthernetHeader->Source ) );
		UInt16	nProtocolType = (inOptions->IsMACProtocolIp())
			? (((*pNewData & 0xF0) == 0x60) ? 0x86DD : 0x0800)	// IPv6 else IPv4
			: inOptions->GetMACProtocolType();
		pEthernetHeader->ProtocolType = HOSTTONETWORK16( nProtocolType );
	}
	else {
		memcpy( pPacketData, pNewData, sizeof( tEthernetHeader ) );
		pNewData += sizeof( tEthernetHeader );
		nNewDataSize -= sizeof( tEthernetHeader );
	}
	pPacketData += sizeof( tEthernetHeader );

	if ( inOptions->IsInsertVLAN() ) {
		// Clear the 4 byte VLAN Header
		tVLANHeader*	pVLanHeader( reinterpret_cast<tVLANHeader*>( pPacketData ) );

		// Set the VLAN Header Protocol Type
		pVLanHeader->nProtocolType = pEthernetHeader->ProtocolType;

		// Set the VLAN ID
		UInt16	nVLANID = inOptions->GetVLANID();
		pVLanHeader->nVlanTag = HOSTTONETWORK16( nVLANID );

		// Set the MAC Header Protocol Type to VLAN
		UInt16	nProtocolType( 0x8100 );
		pEthernetHeader->ProtocolType = HOSTTONETWORK16( nProtocolType );

		pPacketData += sizeof( tVLANHeader );
	}

	// Copy the reassembled data.
	memcpy( pPacketData, pNewData, nNewDataSize );
	pPacketData += nNewDataSize;

	// Pad runts to 64 bytes.
	if ( nNewSize < 64 ) {
		// Includes the FCS.
		UInt16	nPadSize = 64 - nNewSize;
		memset( pPacketData, 0, nPadSize );
		// pPacketData += nPadSize;

		nNewSize = 64;
	}
	else {
		// Clear out the FCS
		*(reinterpret_cast<UInt32*>( pPacketData )) = 0;
	}

	// Set the packet length
	pNewPacket->ReleasePacketData( nNewSize );

	outPacket.Attach( pNewPacket );
	return true;
}


// -----------------------------------------------------------------------------
//		CheckForOrphans
// -----------------------------------------------------------------------------

void
CCMIFilterContext::CheckForOrphans(
	COptions*			inOptions,
	CFragmentMap*		inFragmentMap,
	CAddressMap*		inAddressMap )
{
	ASSERT( inOptions );
	ASSERT( inFragmentMap );
	ASSERT( inAddressMap );
	if ( !inOptions || !inFragmentMap || !inAddressMap ) return;

	if ( inFragmentMap->IsEmpty() ) return;

	UInt64		nDuration = inOptions->GetFragmentAge() * kFileTimeInASecond;
	UInt64		nTimeOut = GetTimeStamp() - nDuration;
	POSITION	fragPos = inFragmentMap->GetStartPosition();
	while ( fragPos ) {
		UInt64				nPacketKey = 0;
		CIPv4PacketList*	pPacketList = NULL;
		inFragmentMap->GetNextAssoc( fragPos, nPacketKey, pPacketList );
		ASSERT( pPacketList );
		if ( pPacketList == NULL ) {
			inFragmentMap->RemoveKey( nPacketKey );
			continue;
		}

		CIPv4Packet*	pLastPacket = pPacketList->GetLast();
		ASSERT( pLastPacket );
		if ( pLastPacket == NULL ) continue;

		// Determine whether to expire the Packet List.
		if ( pLastPacket->GetTimeStamp() < nTimeOut ) {
			// Remove the PacketListList from the Address Map.
			CIPv4PacketListList*	pPacketListList = NULL;
			UInt32					nAddrMapKey( pLastPacket->GetSrcIP() );
			BOOL					bFoundAddress = inAddressMap->Lookup( nAddrMapKey, pPacketListList );
			if ( bFoundAddress && pPacketListList ) {
				pPacketListList->Remove( pPacketList );
				if ( pPacketListList->IsEmpty() ) {
					inAddressMap->RemoveKey( nAddrMapKey );
					delete pPacketListList;
					pPacketListList = NULL;
				}
			}

			// Remove the list from the Fragment Map.
			BOOL	bResult = inFragmentMap->RemoveKey( nPacketKey );
			ASSERT( bResult != FALSE );
			// if ( bResult == FALSE ) continue;

			if ( inOptions->IsSaveOrphans() ) {
				// Move all the packets from this list into the Orphans list.
				m_Orphans.Append( *pPacketList );
			}

			delete pPacketList;
		}
	}
	return;
}


// -----------------------------------------------------------------------------
//		CheckSaveOrphans
// -----------------------------------------------------------------------------

void
CCMIFilterContext::CheckSaveOrphans(
	COptions*	inOptions )
{
	ASSERT( inOptions );
	if ( !inOptions ) return;

	UInt64	nNow( GetTimeStamp() );
	UInt64	nDuration = inOptions->GetSaveInterval() * kFileTimeInASecond;
	UInt64	nExpireTime = m_nLastSaveTime + nDuration;
	if ( nNow > nExpireTime ) {
		if ( SaveOrphans( inOptions ) ) {
			m_Orphans.RemoveAll();
		}
		m_nLastSaveTime = nNow;
	}
}


// -----------------------------------------------------------------------------
//		ClearPackets
// -----------------------------------------------------------------------------

void
CCMIFilterContext::ClearPackets()
{
	m_SafeMaps.Clear();
	m_SafeNewPackets.Clear();
	m_Orphans.RemoveAll();
}


// -----------------------------------------------------------------------------
//		InsertPackets
// -----------------------------------------------------------------------------

void
CCMIFilterContext::InsertPackets(
	CPacketList*	inPacketList )
{
	try {
		if ( inPacketList->IsEmpty() ) return;

		CPacketList ayPackets;
		ayPackets.Copy( *inPacketList );
		inPacketList->RemoveAll();

		bool	bRollback( false );
		while ( ayPackets.GetCount() ) {
			CPacket*	pPacket( ayPackets.GetAt( 0 ) );
			ASSERT( pPacket );
			if ( pPacket == NULL ) {
				ayPackets.RemoveAt( 0 );
				continue;
			}

			// Save the packet which we will check in OnFilter
			// ASSERT(m_pInsertedPacket == NULL);
			m_pInsertedPacket = pPacket->GetPacketData();

			int nResult( 0 );
			nResult = DoInsertPacket(
				pPacket->GetPacket(),
				pPacket->GetPacketData(),
				pPacket->GetMediaType(),
				pPacket->GetMediaSubType(),
				0 );

			m_pInsertedPacket = NULL;

			if ( nResult < 0 ) {
				bRollback = true;
				X_DEBUG_INCR( m_nInsertErrors );
				break;
			}

			// Now delete the packet
			ayPackets.RemoveAt( 0 );
			X_DEBUG_INCR( m_nPackets );
			X_DEBUG_INCR( m_nPacketsDeleted );
			X_DEBUG_INCR( m_nPacketsDeletedInInsert );
		}

		// If there was an error then put the packets
		// we have not inserted back into the
		// packets list for next time.
		if ( bRollback ) {
			inPacketList->Append( ayPackets );
		}
	}
	catch ( ... ) {
		m_bFailure = true;
	}
}


// -----------------------------------------------------------------------------
//		ListenToMessage
// -----------------------------------------------------------------------------

void
CCMIFilterContext::ListenToMessage(
	CBLMessage&	ioMessage )
{
	switch ( ioMessage.GetId() ) {
	case kBLM_OptionsUpdate:
		{
			CBLOptionsUpdate&	OU( dynamic_cast<CBLOptionsUpdate&>( ioMessage ) );
			const COptions&		NewOptions( OU.GetMessage() );
			m_SafeOptions.Copy( NewOptions );
			DoSaveContext();
		}
		break;
	}
}


// -----------------------------------------------------------------------------
//		LogMessage
// -----------------------------------------------------------------------------

void
CCMIFilterContext::LogMessage(
	CString	inMessage )
{
	// Format the file name.
	TCHAR 	szFilePath[MAX_PATH];
	::GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );

	CString	strFileName( szFilePath );
	int	nIndex = strFileName.ReverseFind( _T( '.' ) );
	if ( nIndex >= 0 ) {
		strFileName = strFileName.Left( nIndex );
	}
	strFileName.Append( kLogFilename );

	// Format the current time.
	SYSTEMTIME	theSystemTime;
	::GetSystemTime( &theSystemTime );

	LONG					nTimeZone = 0;
	TIME_ZONE_INFORMATION	tzInfo;
	DWORD					dwResult = ::GetTimeZoneInformation( &tzInfo );

	if ( (dwResult == TIME_ZONE_ID_STANDARD) || (dwResult == TIME_ZONE_ID_DAYLIGHT) ) {
		nTimeZone = -tzInfo.Bias;		// Invert for display.
	}

	char	szDate[32];
	::GetDateFormatA( LOCALE_SYSTEM_DEFAULT, 0, &theSystemTime, "MMMM dd, yyyy", szDate, sizeof( szDate ) );

	char	szTime[16];
	::GetTimeFormatA( LOCALE_SYSTEM_DEFAULT, 0, &theSystemTime, "hh:mm:ss tt", szTime, sizeof( szTime ) );

	// Write the message.
	CFile	theFile;
	if ( theFile.Open( strFileName, (CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) ) ) {
		theFile.SeekToEnd();
		// Format the message.
		CStringA	strMsg;
		strMsg.Format( "%s %s: %s\r\n", szDate, szTime, CT2CA( inMessage ) );
		theFile.Write( strMsg.GetBuffer(), strMsg.GetLength() );
	}
}


#if OPT_EVENT
// -----------------------------------------------------------------------------
//		ProcessPackets
// -----------------------------------------------------------------------------

//void
//CCMIFilterContext::ProcessPackets(
//	COptions*	inOptions )
//{
//	try {
//		// If there are new packets then put them in a different
//		// (unlocked) list
//		CPacketList ayPackets;
//		{
//			XLock	theLock( &m_NewMutex, true );
//			ayPackets = m_ayNewPackets;
//			m_ayNewPackets.RemoveAll();
//		}
//
//		UInt32	nCount = ayPackets.GetCount();
//		for ( UInt32 i = 0; i < nCount; i++ ) {
//			CIPv4Packet*	pPacket = ayPackets.GetAt( i );
//			ASSERT( pPacket );
//			if ( pPacket == NULL ) continue;
//			//if ( !ProcessPacket( inOptions, pPacket ) ) {
//			//	delete pPacket;
//			//	X_DEBUG_INCR( m_nPacketsDeleted );
//			//}
//		}
//	}
//	catch ( ... ) {
//		m_bFailure = true;
//	}
//}
#endif

// -----------------------------------------------------------------------------
//		ProcessPacket
// -----------------------------------------------------------------------------

bool
CCMIFilterContext::ProcessPacket(
	COptions*				inOptions,
	CAutoPtr<CIPv4Packet>	inPacket,
	CFragmentMap*			inFragmentMap,
	CAddressMap*			inAddressMap,
	CPacketList*			inNewPacketList )
{
	ASSERT( inOptions );
	ASSERT( inFragmentMap );
	ASSERT( inAddressMap );
	ASSERT( inNewPacketList );
	if ( !inOptions || !inFragmentMap || !inAddressMap || !inNewPacketList ) return false;

	// Create a new Packet.
	UInt32	nAddrMapKey( static_cast<UInt32>( inPacket->GetIPHeader().GetSource() ) );
	CIPv4PacketListList*	pPacketListList( NULL );

	CIPv4PacketList*	pPacketList( NULL );
	UInt64	nPacketKey = inPacket->GetPacketKey();
	bool	bFound( inFragmentMap->Lookup( nPacketKey, pPacketList ) != FALSE );
	bool	bListFound( inAddressMap->Lookup( nAddrMapKey, pPacketListList ) != FALSE );

	if ( !bFound ) {
		ASSERT( pPacketList == NULL );
		pPacketList = new CIPv4PacketList;
	}
	ASSERT( pPacketList );
	if ( pPacketList == NULL ) return false;
	pPacketList->Add( inPacket );

	bool	bReassemble( pPacketList->IsComplete( inOptions->IsInsertMAC() ) );
	if ( bReassemble ) {
		CByteArray	baData;
		bool		bResult( pPacketList->Reassemble( baData ) );
		if ( bResult ) {
			CAutoPtr<CPacket>	spNewPacket;
			if ( !BuildNewPacket( inOptions, pPacketList, baData, spNewPacket ) ) {
				return false;
			}
			inNewPacketList->Add( spNewPacket );
#if _DEBUG
			m_PacketNumber++;
#endif
		}
		if ( pPacketListList ) {
			pPacketListList->Remove( pPacketList );
			if ( pPacketListList->IsEmpty() ) {
				inAddressMap->RemoveKey( nAddrMapKey );
				delete pPacketListList;
				pPacketListList = NULL;
			}
		}
		if ( bFound ) {
			inFragmentMap->RemoveKey( nPacketKey );
		}
		delete pPacketList;
		pPacketList = NULL;
		return true;
	}

	// Done dealing with the Packet, manage the lists.
	ASSERT( pPacketList );
	if ( pPacketList == NULL ) return false;

	if ( !bFound ) {
		inFragmentMap->SetAt( nPacketKey, pPacketList );
	}
	if ( !bListFound ) {
		ASSERT( pPacketListList == NULL );
		if ( pPacketListList ) return false;

		pPacketListList = new CIPv4PacketListList;
		ASSERT( pPacketListList );
		if ( pPacketListList == NULL ) return false;
		pPacketListList->Add( pPacketList );
#ifdef DEBUG
		UInt32					nTempKey = nAddrMapKey;
		CIPv4PacketListList*	pTempListList = NULL;
		BOOL					bTempResult;
		bTempResult = inAddressMap->Lookup( nTempKey, pTempListList );
		ASSERT( bTempResult == FALSE );
#endif
		inAddressMap->SetAt( nAddrMapKey, pPacketListList );
	}
	return true;
}


// ----------------------------------------------------------------------------
//		Save [static]
// ----------------------------------------------------------------------------

DWORD
CCMIFilterContext::Save(
	const CString&		inFilePath,
	CIPv4PacketList*	inBuffer,
	int					inVersion,
	UInt32*				outFileSize )
{
	// Sanity check.
	ASSERT( inBuffer != NULL );
	if ( inBuffer == NULL ) return ERROR_INVALID_HANDLE;

	// Get the file extension.
	CFileName	theFileName( inFilePath );
	CString		strExtension = theFileName.GetExt();
	strExtension.MakeLower();
	theFileName.SetExt( strExtension );

	CString		strPath;
	strPath.Format( _T( "%s%s" ), theFileName.GetDrive(), theFileName.GetDir() );
	FileUtil::MakePath( strPath );

	UInt32	nPacketCount = static_cast<UInt32>( inBuffer->GetCount() );

	// Create and open the file.
	HANDLE	hFile = ::CreateFile( inFilePath, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return -1;
	}

	// Skip past the header.
	::SetFilePointer( hFile, sizeof( PeekFileHeader ), NULL, FILE_BEGIN );

	// Write each packet.
	UInt32	nPackets = 0;
	for ( UInt32 i = 0; i < nPacketCount; i++ ) {
		// Get the packet.
		CAutoPtr<CIPv4Packet>	spPacket( inBuffer->GetAt( i ) );

		// Byte swap the packet header while writing it out.
		PluginPacket	packetHeader( *spPacket->GetPacket() );
		PeekPacket7		thePacketHeader;

		thePacketHeader.fProtoSpec = 0;
		thePacketHeader.fFlags = static_cast<UInt8>( packetHeader.fFlags & 0x000000FF );	// Potentially losing info here.
		thePacketHeader.fStatus = static_cast<UInt8>( packetHeader.fStatus & 0x000000FF );	// Potentially losing info here.
		thePacketHeader.fPacketLength = HOSTTOBIG16( packetHeader.fPacketLength );
		thePacketHeader.fSliceLength = HOSTTOBIG16( packetHeader.fSliceLength );
		thePacketHeader.fTimeStamp = TimeConv::PeekToMicroPeek( packetHeader.fTimeStamp );
		thePacketHeader.fTimeStamp = HOSTTOBIG64( thePacketHeader.fTimeStamp );

		// Write the packet header.

		DWORD	nBytesWritten = 0;
		if ( !::WriteFile( hFile, &thePacketHeader, sizeof( PeekPacket7 ), &nBytesWritten, NULL ) ) {
			DWORD	nResult = ::GetLastError();
			::CloseHandle( hFile );
			::DeleteFile( inFilePath );
			return nResult;
		}

		// Calculate the number of bytes to be written.
		DWORD	nBytesToWrite = spPacket->GetPacketLength();

		// Write the packet data.
		nBytesWritten = 0;
		if ( !::WriteFile( hFile, spPacket->GetPacketData(), nBytesToWrite, &nBytesWritten, NULL ) ) {
			DWORD	nResult = ::GetLastError();
			::CloseHandle( hFile );
			::DeleteFile( inFilePath );
			return nResult;
		}

		if ( (nBytesToWrite % 2) != 0 ) {
			// Write a padding byte (stupid!!!).
			UInt8	Pad = 0;

			nBytesWritten = 0;
			if ( !::WriteFile( hFile, &Pad, 1, &nBytesWritten, NULL ) ) {
				DWORD	nResult = ::GetLastError();
				::CloseHandle( hFile );
				::DeleteFile( inFilePath );
				return nResult;
			}
		}

		// Increment the number of packets written.
		nPackets++;

	}

	// Get the file size
	DWORD	nFileSize = ::GetFileSize( hFile, NULL );

	// Back up and fill in the header length and count fields.
	PeekFileHeader	theFileHeader;
	memset( &theFileHeader, 0, sizeof( theFileHeader ) );
	theFileHeader.fVersion = kPacketFile_Version7;

	theFileHeader.fLength = HOSTTOBIG32( nFileSize );
	theFileHeader.fPacketCount = HOSTTOBIG32( nPackets );
	theFileHeader.fTimeDate = HOSTTOBIG32( TimeConv::AnsiToMac( GetTimeStamp() ) );
	theFileHeader.fTimeStart = 0;
	theFileHeader.fTimeStop = 0;
	theFileHeader.fMediaType = HOSTTOBIG32( static_cast<UInt32>( kMediaType_802_3 ) );
	theFileHeader.fMediaSubType = HOSTTOBIG32( static_cast<UInt32>( kMediaSubType_Native ) );

#if later
	// Set up the version field.
	ENV_VERSION	FileVersion, ProductVersion;
	if ( g_Environment.GetModuleVersion( FileVersion, ProductVersion ) ) {
		UInt8*	pAppVers = (UInt8*) &theFileHeader.fAppVers;
		pAppVers[3] = (UInt8) FileVersion.MajorVer;
		pAppVers[2] = (UInt8) FileVersion.MinorVer;
		pAppVers[1] = (UInt8) FileVersion.MajorRev;
		pAppVers[0] = (UInt8) FileVersion.MinorRev;
	}
#endif

	// Set up the link speed field.
	theFileHeader.fLinkSpeed = 0; // HOSTTOBIG32( static_cast<UInt32>( 0 ) ); //inBuffer->GetLinkSpeed() );

	// Write the file header.
	::SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
	DWORD	nBytesWritten = 0;
	if ( !::WriteFile( hFile, &theFileHeader, sizeof(theFileHeader), &nBytesWritten, NULL ) ) {
		DWORD	nResult = ::GetLastError();
		::CloseHandle( hFile );
		::DeleteFile( inFilePath );
		return nResult;
	}

	// Close the file handle.
	::CloseHandle( hFile );

	if ( outFileSize != NULL ) {
		// Return the file size.
		*outFileSize = nFileSize;
	}

	return ERROR_SUCCESS;
}


// ----------------------------------------------------------------------------
//		SaveOrphans
// ----------------------------------------------------------------------------

bool
CCMIFilterContext::SaveOrphans(
	COptions*	inOptions )
{
	if ( m_Orphans.IsEmpty() ) {
		return false;
	}

	// Construct a filename with the timestamp
	CFileName	theFileName( inOptions->GetFileName() );
	CString		theName = theFileName.GetName();
	theName = theName.Trim();
	theName += _T( " " );
	theName += GetTimeStringFileName();
	theFileName.SetName( theName );

	// Check for an extension
	CString theExt = theFileName.GetExt();
	if ( theExt.IsEmpty() ) {
		theFileName.SetExt( _T( ".pkt" ) );
	}

	// Save the packets to a file
	UInt32	nFileSize = 0;
	DWORD	nResult = Save( theFileName.GetPath(), &m_Orphans, 0, &nFileSize );
	ASSERT( nResult == 0 );
	if ( nResult == 0 ) {
		X_DEBUG_INCR( m_nOrphanFiles );
	}
	else {
		CString	theString;
		theString.LoadString( IDS_SAVE_ORPHANS_ERROR );
		CString	theMsg;
		theMsg.Format(
			_T( "%s (%d): %s" ),
			static_cast<const TCHAR*>( theString ),
			::GetLastError(),
			static_cast<const TCHAR*>( theFileName.GetPath() ) );
		AfxMessageBox( theMsg );
		return false;
	}
	return true;
}


// -----------------------------------------------------------------------------
//		Init
// -----------------------------------------------------------------------------

#undef SubclassWindow

int
CCMIFilterContext::Init(
	PluginCreateContextParam*	ioParams )
{
	int	nResult;

	COptionsPtr	spOptions( GetOptions() );
	ASSERT( spOptions );
	if ( spOptions == NULL ) return PLUGIN_RESULT_ERROR;

	nResult = CPeekContext::Init( ioParams );
	if ( nResult != PLUGIN_RESULT_SUCCESS ) {
		return nResult;
	}

	m_pTabParent = new CParentStatic;
	HWND hTabWnd = NULL;

	nResult = DoAddTab( _T( "CMI Filter" ), _T( "STATIC" ), reinterpret_cast<void**>( &hTabWnd ) );
	if ( FAILED( nResult ) ) return PLUGIN_RESULT_SUCCESS;

	// Subclass the static window WP created for us
	m_pTabParent->SubclassWindow( hTabWnd );

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// Create some dialog or other window class you want embedded in static window
	COptionsStatus* pTab = new COptionsStatus();
	ASSERT( pTab );
	if ( pTab == NULL ) return 0;
	AddListener( pTab );
	pTab->AddListener( this );

	m_pTabParent->SetTab( pTab );
	pTab->Create( COptionsStatus::IDD, m_pTabParent );
	pTab->ShowWindow( SW_SHOW );

	m_pOptionsTab = pTab;

	spOptions->Copy( *theApp.GetPlugin().GetOptions() );
	BroadcastMessage( CBLOptionsUpdate( *spOptions ) );

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      Term
// -----------------------------------------------------------------------------

int
CCMIFilterContext::Term( )
{
	CPeekContext::Term();

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReset
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnReset(
	PluginResetParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnStartCapture(
	PluginStartCaptureParam*	ioParams )
{
	COptionsPtr	spOptions( GetOptions() );
	ASSERT( spOptions );
	if ( spOptions == NULL ) return PLUGIN_RESULT_ERROR;

	// let's say that OnProcessTime and OnSummary are disabled until
	// they actually get called.
	ResetMonitorMode();

#if _DEBUG
	m_PacketNumber				= 0;
	m_nOrphanFiles				= 0;
	m_nOrphans					= 0;
	m_nPacketsCreated			= 0;
	m_nPacketsDeleted			= 0;
	m_nPacketsDeletedInProcess	= 0;
	m_nPacketsDeletedInInsert	= 0;
	m_nInsertErrors				= 0;
	m_nPackets					= 0;
	m_nIPIDOrphans				= 0;
#endif

	m_bCapturing = true;
	m_pInsertedPacket = NULL;
	m_nLastSaveTime = GetTimeStamp();

#if OPT_EVENT
	// start the capture thread
	m_hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	m_pThread = AfxBeginThread( CaptureThreadProc, this );
#endif

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnStopCapture(
	PluginStopCaptureParam*	ioParams )
{
	m_bCapturing = false;

#if OPT_EVENT
	if ( m_pThread ) {
		// copy the thread handle.  m_pThread deletes itself, so by the time
		// we wait for it it could be gone or in a bad state
		HANDLE hThread = m_pThread->m_hThread;
		// signal the thread to stop
		::SetEvent( m_hEvent );
		// wait on the thread to close
		::WaitForSingleObject( hThread, INFINITE );
		// close the stop event
		::CloseHandle( m_hEvent );
	}
	m_hEvent = NULL;
	m_pThread = NULL;
#endif

	//{
	//	CPacketListPtr	spNewPacketList = GetNewPacketList();
	//	InsertPackets( spNewPacketList );	// Insert any remaining packets
	//}

	ClearPackets();
	m_pInsertedPacket = NULL;

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnProcessPacket(
	PluginProcessPacketParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnGetPacketString
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnGetPacketString(
	PluginGetPacketStringParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnApply
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnApply(
	PluginApplyParam*	ioParams )
{
	if ( ioParams->fCommand == kApplyMsg_Start )	return PLUGIN_RESULT_SUCCESS;
	if ( ioParams->fCommand == kApplyMsg_End )		return PLUGIN_RESULT_SUCCESS;
	if ( ioParams->fCommand != kApplyMsg_Packet )	return PLUGIN_RESULT_ERROR;
	if ( ioParams->fPacket == NULL )				return PLUGIN_RESULT_ERROR;
    return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnSelect
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnSelect(
	PluginSelectParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnPacketsLoaded
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnPacketsLoaded(
	PluginPacketsLoadedParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnSummary
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnSummary(
	PluginSummaryParam*	ioParams )
{
	AFX_MANAGE

	if ( m_bFailure ) {
		m_bFailure = false;
		CString	strMessage;
		strMessage.Format( _T( "CMI Filter: %s: unhandled exception" ), GetCaptureTitle() );
		LogMessage( strMessage );
	}
	if ( m_pPlugin->m_bFailure ) {
		m_pPlugin->m_bFailure = false;
		CString	strMessage;
		strMessage.Format( _T( "CMI Filter: unhandled exception" ) );
		LogMessage( strMessage );
	}

	// If save orphans is enabled
	if ( !IsCapturing() ) return 0;

#ifdef _DEBUG
	DoSummaryModifyEntry( _T( "CMI Packets" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nPackets );

	DoSummaryModifyEntry( _T( "CMI Orphans"), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nOrphans );

	DoSummaryModifyEntry( _T( "CMI Files"), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nOrphanFiles );

	DoSummaryModifyEntry( _T( "CMI Packet Object Created" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nPacketsCreated );

	DoSummaryModifyEntry( _T( "CMI Packet Objects Deleted" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nPacketsDeleted );

	DoSummaryModifyEntry( _T( "CMI Packet Objects Deleted In Process" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nPacketsDeletedInProcess );

	DoSummaryModifyEntry( _T( "CMI Packet Objects Deleted In Insert" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nPacketsDeletedInInsert );

	DoSummaryModifyEntry( _T( "CMI Packet Objects Deleted by IP ID" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nIPIDOrphans );

	DoSummaryModifyEntry( _T( "CMI Packet Insert Errors" ), _T( "CMI" ),
		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt64),
		&m_nInsertErrors );

	//size_t	nNewPacketsCount( -1 );
	//{
	//	CPacketListPtr	pNewPackets( GetNewPacketList() );
	//	if ( pNewPackets ) {
	//		nNewPacketsCount = pNewPackets->GetCount();
	//	}
	//}
	//if ( nNewPacketsCount != -1 ) {
	//	DoSummaryModifyEntry( _T( "New Packet List Count" ), _T( "CMI" ),
	//		(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt32),
	//		&nNewPacketsCount );
	//}

	size_t	nFragMapCount( -1 );
	UInt32	nDeepFragCount( -1 );
	size_t	nAddrMapCount( -1 );
	UInt32	nDeepAddrCount( -1 );
	CFragmentMapPtr	spFragMap;
	CAddressMapPtr	spAddrMap;
	{
		if ( GetMaps( spFragMap, spAddrMap ) ) {
			nFragMapCount = spFragMap->GetCount();
			nDeepFragCount = spFragMap->GetDeepCount();
			nAddrMapCount = spAddrMap->GetCount();
			nDeepAddrCount = spAddrMap->GetDeepCount();
		}
	}
	if ( nFragMapCount != -1 ) {
		DoSummaryModifyEntry( _T( "Fragment Map Count" ), _T( "CMI" ),
			(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt32),
			&nFragMapCount );
	}
	if ( nDeepFragCount != -1 ) {
		DoSummaryModifyEntry( _T( "Fragment Count" ), _T( "CMI" ),
			(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt32),
			&nDeepFragCount );
	}
	if ( nAddrMapCount != -1 ) {
		DoSummaryModifyEntry( _T( "Address Map Count" ), _T( "CMI" ),
			(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt32),
			&nAddrMapCount );
	}
	if ( nDeepAddrCount != -1 ) {
		DoSummaryModifyEntry( _T( "Address List Count" ), _T( "CMI" ),
			(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt32),
			&nDeepAddrCount );
	}

	size_t	nOrphanCount( -1 );
	nOrphanCount = m_Orphans.GetCount();
	if ( nOrphanCount != -1 ) {
		DoSummaryModifyEntry( _T( "Orphan List Count" ), _T( "CMI" ),
			(kPluginSummaryType_PacketCount | kPluginSummarySize_UInt32),
			&nOrphanCount );
	}
#endif

	// Bloom: In Some testing I did inserting packets on a separate thread
	// would result in not capturing packets after a while.   So I put
	// the call here and the problem went away.   And I do remember
	// there being a problem with inserting packets on a different
	// thread.   However, the RCMP has asked to remove the dependency
	// on summary stats being on.  So, I have done it both ways.  If
	// summary stats is on I will insert packets here, otherwise we will
	// go ahead and insert them on the capture thread.

	// Visser: Upgrade to API 12 which has ProcessTime.

	//try {
	//	if ( CheckAndSetMonitorMode( kMode_OnSummary ) ) {
	//		InsertPackets();
	//		CheckForOrphans();
	//	}
	//}
	//catch ( ... ) {
	//	m_bFailure = true;
	//}

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnFilter
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnFilter(
	PluginFilterParam*	ioParams )
{
	USES_CONVERSION;

	try {
		// First check to see if it is a packet we inserted
		if ( m_pInsertedPacket == ioParams->fPacketData ) {
			m_pInsertedPacket = NULL;
			return	PLUGIN_PACKET_ACCEPT;
		}

		CIPv4PacketRef	PacketRef( ioParams->fPacket->fProtoSpec, ioParams->fMediaType, ioParams->fMediaSubType,
			ioParams->fPacket, ioParams->fPacketData );
		if ( !PacketRef.IsIP() ) {
			return PLUGIN_PACKET_REJECT;
		}
		// Only process UPD Packets: CMI is a UDP based protocol.
		if ( PacketRef.GetPayloadProtocol() != kProtocol_UDP ) {
			return PLUGIN_PACKET_REJECT;
		}

		// Get the Options.
		COptionsPtr	spOptions( GetOptions() );
		ASSERT( spOptions );
		if ( spOptions == NULL ) return PLUGIN_PACKET_REJECT;

		CAutoPtr<CIPv4Packet>	spPacket;
		{
			CIPv4Packet*	pPacket( NULL );
			CUDPPacket*		pUdpPacket( new CUDPPacket( PacketRef ) );
			if ( pUdpPacket->IsValid() ) {
				spPacket.Attach( pUdpPacket );
			}
			else {
				delete pUdpPacket;
				pUdpPacket = NULL;
				CIPv4Packet*	pIpPacket( new CIPv4Packet( PacketRef ) );
				if ( pIpPacket->IsValid() ) {
					spPacket.Attach( pIpPacket );
				}
				else {
					delete pIpPacket;
					return PLUGIN_PACKET_REJECT;
				}
			}
		}

		// If a Security Id was configured, then check the Id.
		if ( spPacket->IsPacketType( CPacket::kPacketType_UDP ) ) {
			UInt16			nBytesLeft( 0 );
			const UInt8*	pId( PacketRef.GetLayerData( kPacketLayer_UDP, &nBytesLeft ) );

			switch (spOptions->GetProtocolType()) {
			case CInterceptId::kIdType_PCLI:
				{
					spPacket->SetDataOffset( 4 );
				}
				break;

			case CInterceptId::kIdType_SecurityId:
				{
					if ( spOptions->HasInterceptId() ) {
						CSecurityId	idSecurity( nBytesLeft, pId );
						if ( !idSecurity.CompareInterceptId( spOptions->GetInterceptId() ) ) {
							return PLUGIN_PACKET_REJECT;
						}
					}

					spPacket->SetDataOffset( CSecurityId::s_nSize );
				}
				break;

			case CInterceptId::kIdType_Ericsson:
				{
					CEricsson	idEricsson( nBytesLeft, pId );
					if ( spOptions->HasInterceptId() ) {
						if ( !idEricsson.CompareInterceptId( spOptions->GetInterceptId() ) ) {
							return PLUGIN_PACKET_REJECT;
						}
					}
					spPacket->SetDataOffset( idEricsson.GetLength() );
				}
				break;

			case CInterceptId::kIdType_Arris:
				{
					CArris	idArris( nBytesLeft, pId );
					if ( spOptions->HasInterceptId() ) {
						if ( !idArris.CompareInterceptId( spOptions->GetInterceptId() ) ) {
							return PLUGIN_PACKET_REJECT;
						}
					}
					spPacket->SetDataOffset( idArris.GetLength() );
				}
				break;

			case CInterceptId::kIdType_Juniper:
				{
					// Juniper and Nokia
					CJuniper	idJuniper( nBytesLeft, pId );
					if ( spOptions->HasInterceptId() ) {
						if ( !idJuniper.CompareInterceptId( spOptions->GetInterceptId() ) ) {
							return PLUGIN_PACKET_REJECT;
						}
					}
					spPacket->SetDataOffset( idJuniper.GetLength() );
				}
				break;

			case CInterceptId::kIdType_None:
			default:
				break;
			}
		}

		// Check the IP Addresses based on direction.
		if ( spOptions->IsIpFilter() ) {
			CIpAddressPair	iapPacket( PacketRef.GetIPHeader().GetSource(), PacketRef.GetIPHeader().GetDestination() );
			CIpPort			thePort;
			bool			bIsUdp( spPacket->IsPacketType( CPacket::kPacketType_UDP ) );
			if ( bIsUdp ) {
				thePort = PacketRef.GetUDPDestinationPort();
			}
			if ( !spOptions->IsIpFilterMatch( iapPacket, bIsUdp, thePort ) ) {
				return PLUGIN_PACKET_REJECT;
			}
		}

		CFragmentMapPtr	spFragmentMap;
		CAddressMapPtr	spAddressMap;
		GetMaps( spFragmentMap, spAddressMap );
		if ( !spFragmentMap || !spAddressMap ) return PLUGIN_PACKET_REJECT;
		//CPacketListPtr	spNewPacketList = GetNewPacketList();
		//if ( !spNewPacketList ) return PLUGIN_PACKET_REJECT;
#if OPT_EVENT
		if ( !ProcessPacket( spOptions, spPacket, spFragmentMap, spAddressMap, spNewPacketList ) ) {
			SetEvent( m_hEvent );
			InsertPackets();
		}
#else
		CPacketList	ayNewPackets;
		ProcessPacket( spOptions, spPacket, spFragmentMap, spAddressMap, &ayNewPackets );
		//spAddressMap.Unlock();	// not a safe ptr.
		spFragmentMap.Unlock();
		spOptions.Unlock();

		InsertPackets( &ayNewPackets );
#endif
	}
	catch ( ... ) {
		m_bFailure = true;
	}

	return PLUGIN_PACKET_REJECT;
}


// -----------------------------------------------------------------------------
//      OnGetPacketAnalysis
// -----------------------------------------------------------------------------
// Called when a packet needs to be displayed in the main packet list.  Plug-ins
// will only get packets which correspond the the ProtoSpecs they supplied
// during Load (in GetSupportedProtoSpecs).

int
CCMIFilterContext::OnGetPacketAnalysis(
	PluginGetPacketStringParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnProcessPluginMessage
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnProcessPluginMessage(
	PluginProcessPluginMessageParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnHandleNotify
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnHandleNotify(
	PluginHandleNotifyParam*	ioParams )
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnContextOptions
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnContextOptions(
	PluginContextOptionsParam*	ioParams )
{
	COptionsDialog	theDialog( NULL );
	{
		COptionsPtr	spOptions( GetOptions() );
		theDialog.SetOptions( *spOptions );
	}

	if ( theDialog.DoModal() == IDOK ) {
		COptionsPtr	spOptions( GetOptions() );
		ASSERT( spOptions );
		if ( spOptions == NULL ) return PLUGIN_RESULT_ERROR;

		SetOptions( theDialog.GetOptions() );
		BroadcastMessage( CBLOptionsUpdate( *spOptions ) );

		m_strPrefs = spOptions->GetContextPrefs();
		if ( !m_strPrefs.IsEmpty() ) {
			ioParams->fOutNewPrefs = m_strPrefs.GetBuffer();
			ioParams->fOutNewPrefsLength = (m_strPrefs.GetLength() + 1) * sizeof( *(m_strPrefs.GetBuffer()) );
		}
	}
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnSetContextPrefs
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnSetContextPrefs(
	PluginContextPrefsParam*	ioParams )
{
	COptionsPtr	spOptions( GetOptions() );
	ASSERT( spOptions );
	if ( spOptions == NULL ) return 0;

	spOptions->SetContextPrefs( ioParams->fIoPrefs );
	BroadcastMessage( CBLOptionsUpdate( *spOptions ) );
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnGetContextPrefs
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnGetContextPrefs(
	PluginContextPrefsParam*	ioParams )
{
	COptionsPtr	spOptions( GetOptions() );
	ASSERT( spOptions );
	if ( spOptions == NULL ) return 0;

	m_strPrefs = spOptions->GetContextPrefs();
	ioParams->fIoPrefs = m_strPrefs.GetBuffer();
	ioParams->fIoPrefsLength = m_strPrefs.GetLength();
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//      OnProcessTime
// -----------------------------------------------------------------------------

int
CCMIFilterContext::OnProcessTime(
	PluginProcessTimeParam*	ioParams )
{
	try {
		if ( IsCapturing() ) {
			COptionsPtr			spOptions = GetOptions();
			if ( !spOptions ) return PLUGIN_RESULT_SUCCESS;
			CFragmentMapPtr		spFragmentMap;
			CAddressMapPtr		spAddressMap;
			GetMaps( spFragmentMap, spAddressMap );
			if ( !spFragmentMap || !spAddressMap ) return PLUGIN_RESULT_SUCCESS;
			if ( spOptions->IsSaveOrphans() ) {
				CheckForOrphans( spOptions, spFragmentMap, spAddressMap );
				CheckSaveOrphans( spOptions );
			}
			else {
				CheckForOrphans( spOptions, spFragmentMap, spAddressMap );
			}
		}
	}
	catch ( ... ) {
		m_bFailure = true;
	}

	return PLUGIN_RESULT_SUCCESS;
}


#if OPT_EVENT
// -----------------------------------------------------------------------------
//		RunCaptureThread
// -----------------------------------------------------------------------------

//void
//CCMIFilterContext::RunCaptureThread()
//{
//	UInt64 nPreviousSecond = 0;
//
//	while ( IsCapturing() ) {
//		::WaitForSingleObject( m_hEvent, 1000 );
//		ResetEvent( m_hEvent );
//
//		// If OnProcessTime and/or OnSummary are disabled then insert packets
//		// and check for orphans from here.
//		if ( IsMonitorMode( kMode_CaptureThread ) ) {
//			// Check for orphans every other second
//			UInt64	curTime( GetTimeStamp() );
//			if ( (nPreviousSecond + 1) < curTime ) {
//				try {
//					InsertPackets();
//					CheckForOrphans();
//					nPreviousSecond = GetTimeStamp();
//				}
//				catch ( ... ) {
//					m_bFailure = true;
//				}
//			}
//		}
//		{
//			// ProcessPackets( GetOptions() );
//		}
//	}
//}
#endif
