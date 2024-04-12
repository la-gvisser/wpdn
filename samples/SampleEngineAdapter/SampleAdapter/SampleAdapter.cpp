// =============================================================================
//	SampleAdapter.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "SampleAdapter.h"
#include "ixmldom.h"
#include "xmlutil.h"
#include <algorithm>
#include <string>

using namespace std;

#ifndef ASSERT
#define ASSERT(b)	ATLASSERT(b)
#endif

#define kPluginName				L"Sample Adapter"
#define kTag_ConfigFileName		L"SampleAdapter.xml" 
#define kTag_Options			L"Options"
#define kTag_Port				L"Port"

static HeID GUID_SampleAdapter = CSampleAdapter_CID;

CHePtr<IHeServiceProvider>	CSampleAdapter::m_spServices = 0;
UInt32						CSampleAdapter::m_dbgPacketNumber = 0;


// =============================================================================
//		PacketThread
// =============================================================================

DWORD WINAPI
PacketThread(
	LPVOID	lpParam )
{
	CSampleAdapter* pServer = (CSampleAdapter*)lpParam;
	if ( pServer ) pServer->Run();	
	return 0;
}


// =============================================================================
//		CSampleAdapter
// =============================================================================

CSampleAdapter::CSampleAdapter()
	:	m_bCapturing( FALSE )
	,	m_bPaused( FALSE )
	,	m_MediaType( kMediaType_802_3 )
	,	m_MediaSubType( kMediaSubType_802_11_gen )
	,	m_ullLinkSpeed( 54000000ULL )
	,	m_ulPacketCount( 0 )
	,	m_Socket( INVALID_SOCKET )
{
	m_strAdapterName	= "SampleAdapter";
	m_strAdapterID		= "Sample Adapter";

	m_hCaptureThread	= NULL;
	m_hStopEvent		= NULL;
	m_nPort				= 5000;
	m_bInsertingPacket	= FALSE;
	m_bGotPackets		= FALSE;
	m_nRunningCaptures	= 0;
}


// -----------------------------------------------------------------------------
//		CloseSocket
// -----------------------------------------------------------------------------

void
CSampleAdapter::CloseSocket()
{
	if ( m_Socket != INVALID_SOCKET ) {
		::shutdown( m_Socket, SD_BOTH );
		::closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}
}


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult CSampleAdapter::FinalConstruct()
{
	HeResult hr = HE_S_OK;
	
	HE_TRY {
		m_heCritSection.Init();		
		m_heCapIdPluginLock.Init();
		
		// Create a file buffer.
		CHePtr<IHePersistFile>	spFilePacketBuffer;
		HeThrowIfFailed( spFilePacketBuffer.CreateInstance( "PeekCore.FilePacketBuffer" ) );

		// Get the packet buffer interface.
		m_spPacketBuffer.Release();
		HeThrowIfFailed( spFilePacketBuffer->QueryInterface( &m_spPacketBuffer.p ) );
 
		HeThrowIfFailed( m_spPacketBuffer->SetMediaType( m_MediaType ) );
		HeThrowIfFailed( m_spPacketBuffer->SetMediaSubType( m_MediaSubType ) );
		HeThrowIfFailed( m_spPacketBuffer->SetLinkSpeed( m_ullLinkSpeed ) );
		HeThrowIfFailed( m_spPacketBuffer->GetPacketCount( &m_ulPacketCount ) );

		HeThrowIfFailed( m_spPacketBuffer->GetMediaInfo( &m_spMediaInfo.p ) );		
		HeThrowIfFailed( m_spMediaInfo->SetMediaType( m_MediaType ) );
		HeThrowIfFailed( m_spMediaInfo->SetMediaSubType( m_MediaSubType ) );
		HeThrowIfFailed( m_spMediaInfo->SetLinkSpeed( m_ullLinkSpeed ) );

		hr = m_spInfo.CreateInstance( "PeekCore.PluginAdapterInfo" );
		if ( HE_FAILED( hr ) ) return hr;

		// now set the adapter info
		CHeQIPtr<IPluginAdapterInfo> spInfo = m_spInfo;
		HeThrowIf( spInfo == NULL );

		HeThrowIfFailed( spInfo->SetPluginName( CHeBSTR(kPluginName) ) );
		HeThrowIfFailed( spInfo->SetPersistent( FALSE ) );	// adapter manager will not save on shutdown (we have to do the work)
		HeThrowIfFailed( spInfo->SetTitle( CHeBSTR( kPluginName ) ) );
		HeThrowIfFailed( spInfo->SetIdentifier( CHeBSTR( m_strAdapterID ) ) );		
		HeThrowIfFailed( spInfo->SetMediaType( m_MediaType ) );
		HeThrowIfFailed( spInfo->SetMediaSubType( m_MediaSubType ) );
		HeThrowIfFailed( spInfo->SetDescription( CHeBSTR( m_strAdapterName ) ) );
		HeThrowIfFailed( spInfo->SetLinkSpeed( m_ullLinkSpeed ) );	
		HeThrowIfFailed( spInfo->SetMediaInfo( m_spMediaInfo ) );

		CHePtr<IAppConfig>	spAppConfig;
		if ( m_spServices ) {
			hr = m_spServices->QueryService( IAppConfig::GetIID(), IAppConfig::GetIID(),
				reinterpret_cast<void**>( &spAppConfig.p ) );
		}
		
		hr = spAppConfig->GetConfigDir( &m_strDataPath.m_str );
				
		CHeBSTR hestrConfigFile = GetDataFileFullName();
		CHePtr<Xml::IXMLDOMDocument>	spDoc;
		wstring strConfigFile( hestrConfigFile.m_str );
		XmlUtil::LoadXmlFile( strConfigFile.data(), &(reinterpret_cast<Xml::IXMLDOMDocument*>( spDoc.p )) );
		if ( !spDoc.p ) return hr;
		
		CHePtr<Xml::IXMLDOMElement> spRootNode;
		spDoc->get_documentElement( &(reinterpret_cast<Xml::IXMLDOMElement*>(spRootNode.p)) );
		if ( !spRootNode.p ) return hr;

		string strFlowPort;
		if ( XmlUtil::GetAttribute( spRootNode, kTag_Port, strFlowPort ) ) {
			int nPort = atoi( strFlowPort.c_str() );
			if ( (nPort > 0) && (nPort < 65536) ) {
				m_nPort = (UInt16)nPort;
			}
		}		
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void CSampleAdapter::FinalRelease()
{ 
	m_heCapIdPluginLock.Term();
	m_heCritSection.Term();
}


// -----------------------------------------------------------------------------
//		GetAdapterInfo
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetAdapterInfo(
	IAdapterInfo**	ppAdapterInfo )
{
	if ( ppAdapterInfo == NULL ) return HE_E_NULL_POINTER;

	HeResult hr = HE_S_OK;
	HE_TRY {
		return m_spInfo.CopyTo( ppAdapterInfo );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		GetHardwareOptions
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetHardwareOptions(
	IHeUnknown**	ppHardwareOptions )
{
	HeResult hr = HE_S_OK;

	if ( ppHardwareOptions ) {
		UInt64*	pValue = new UInt64( 10000000 );
		ppHardwareOptions = (IHeUnknown**)pValue;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		SetHardwareOptions
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::SetHardwareOptions(
	IHeUnknown*	pHardwareOptions )
{
	HeResult hr = HE_S_OK;	

	if ( pHardwareOptions ) {
		;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		GetAdapterType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetAdapterType(
	enum PeekAdapterType*	pAdapterType )
{
	if ( pAdapterType ) {
		*pAdapterType = peekPluginAdapter;
	}
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetName(
	HEBSTR* pbstrName )
{
	if ( pbstrName == NULL ) return HE_E_NULL_POINTER;

	return m_strAdapterName.CopyTo( pbstrName );
}


// -----------------------------------------------------------------------------
//		GetMediaType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetMediaType(
	enum TMediaType*	pMediaType )
{
	if ( pMediaType == NULL ) return HE_E_NULL_POINTER;

	*pMediaType = m_MediaType;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetMediaSubType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetMediaSubType(
	enum TMediaSubType*	pMediaSubType )
{
	if ( pMediaSubType == NULL ) return HE_E_NULL_POINTER;

	*pMediaSubType = m_MediaSubType;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetLinkSpeed
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetLinkSpeed(
	ULONGLONG*	pullLinkSpeed )
{
	if ( pullLinkSpeed == NULL ) return HE_E_NULL_POINTER;

	*pullLinkSpeed = m_ullLinkSpeed;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetAddress
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetAddress(
	BYTE	pAddress[6] )
{
	if ( pAddress == NULL ) return HE_E_NULL_POINTER;

	memset( pAddress, 0, 6 ); // From BYTE pAddress[6] - size of Ethernet Address.
	return HE_S_OK;	
}


// -----------------------------------------------------------------------------
//		GetNetSupportDuringCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetNetSupportDuringCapture(
	BOOL*	pbNetSupp )
{
	if ( pbNetSupp == NULL ) return HE_E_NULL_POINTER;

	*pbNetSupp = TRUE;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		Open
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::Open()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::Close()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// ----------------------------------------------------------------------------
//		GetClassID
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetClassID(
	HeCID* pCLSID )
{
	// Sanity checks.
	HE_ASSERT( pCLSID != NULL );
	if ( pCLSID == NULL ) return HE_E_NULL_POINTER;

	*pCLSID = *(reinterpret_cast<HeCID*>( &GUID_SampleAdapter ));

	return HE_S_OK;
}


// ICapturePackets

// -----------------------------------------------------------------------------
//		IsCapturing
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::IsCapturing(
	BOOL*	pbCapturing )
{
	pbCapturing;
	HeResult hr = HE_S_OK;
	return hr;
}


// -----------------------------------------------------------------------------
//		PauseCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::PauseCapture()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		ResumeCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::ResumeCapture()
{
	HeResult hr = HE_S_OK;
	
	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		StartCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::StartCapture()
{
	HeResult hr = HE_S_OK;

	HE_TRY {	
		if ( m_nRunningCaptures == 0 ) {
			m_hStopEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
			m_hCaptureThread = ::CreateThread( NULL, 0, PacketThread, this, 0, NULL );
		}
		m_nRunningCaptures++;
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::StopCapture()
{
	HeResult hr = HE_S_OK;
	HE_TRY {
		m_nRunningCaptures--;
		if ( m_nRunningCaptures == 0 ) {
			const UInt32 TIMEOUT = INFINITE;
			if ( m_hCaptureThread == NULL ) return 0;

			// signal a stop
			::SetEvent( m_hStopEvent );
			::WaitForSingleObject( m_hCaptureThread, TIMEOUT );
			::CloseHandle( m_hCaptureThread );
			::CloseHandle( m_hStopEvent );
			m_hCaptureThread = NULL;
			m_hStopEvent = NULL;

			CloseSocket();
		}

	}
	HE_CATCH( hr )

	return hr;
}


// IPacketProvider

// -----------------------------------------------------------------------------
//		AddPacketHandler
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::AddPacketHandler(
	IPacketHandler*	pHandler )
{
	//ObjectLock	lock( this );
	CHeCritSecLock<CHeCriticalSection> lockPacketHandlerVec( &m_heCritSection );

	// Sanity checks.
	ASSERT( pHandler != NULL );
	if ( pHandler == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {		
		vector< CHePtr<IPacketHandler> >::const_iterator Iter = find(
			m_vecPackethandler.begin(), m_vecPackethandler.end(), pHandler );

		if ( Iter == m_vecPackethandler.end() ) {
			m_vecPackethandler.push_back( pHandler );
		}
		m_pPackethandler = pHandler;		
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		RemovePacketHandler
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::RemovePacketHandler(
	IPacketHandler* pHandler )
{	
	//ObjectLock	lock( this );
	CHeCritSecLock<CHeCriticalSection> lockPacketHandlerVec( &m_heCritSection );

	// Sanity checks.
	ASSERT( pHandler != NULL );
	if ( pHandler == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {
		vector< CHePtr<IPacketHandler> >::iterator Iter = find(
			m_vecPackethandler.begin(), m_vecPackethandler.end(), pHandler );

		if ( Iter != m_vecPackethandler.end() ) {
			m_vecPackethandler.erase( Iter );
		}
	}
	HE_CATCH( hr )

	return hr;
}


// ICaptureList

// -----------------------------------------------------------------------------
//		AddCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::AddCapture(
	IHeUnknown* pUnknown )
{
	pUnknown;
	HeResult hr = HE_S_OK;
	return hr;
}


// -----------------------------------------------------------------------------
//		RemoveCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::RemoveCapture(
	IHeUnknown*	pUnknown )
{
	pUnknown;
	HeResult hr = HE_S_OK;
	return hr;
}


// -----------------------------------------------------------------------------
//		SetSite
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::SetSite(
	IHeUnknown*	pUnkSite )
{			   
	if ( pUnkSite == NULL ) return S_OK;

	HeResult hr = S_OK;
	
	HE_TRY {	
		HeThrowIfFailed( __super::SetSite( pUnkSite ) );
		CHePtr<IHeServiceProvider>	spServices;
		hr = GetSite( IHeServiceProvider::GetIID(), (void**) &spServices.p );
		HeThrowIfFailed( hr );

		CHePtr<IPeekCapture>	spCapture;
		hr = spServices->QueryInterface( &spCapture.p );
		HeThrowIfFailed( hr );		
		
		hr = spCapture->QueryInterface( &m_spPacketHandler.p );
		HeThrowIfFailed( hr );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		Run
// -----------------------------------------------------------------------------

void
CSampleAdapter::Run()
{
	int nResult = BindSocket();
	if (nResult != 0) return;

	UInt8	DataBuf[4096];

	// read peek header
	SOCKADDR_IN sockAddr;
	memset( &sockAddr, 0, sizeof( sockAddr ) );
	int nAddrLen = sizeof( sockAddr );

	struct timeval	timeout;  /* Timeout for select */
	fd_set			fdSockets;

	// loop until capture stops
	while ( ::WaitForSingleObject(m_hStopEvent, 0) != WAIT_OBJECT_0 ) {
		try {
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			// populate fd sockets and find high socket
			FD_ZERO( &fdSockets );
			FD_SET( m_Socket, &fdSockets );

			// MSDN indicates that the first argument (nfds) is ignored.
			int nReadSocket = select(
				0, &fdSockets, (fd_set *) 0, (fd_set *) 0, &timeout );
			
			if ( nReadSocket < 0 ) {
				continue;	// this is bad
			}
			else if ( nReadSocket == 0 ) {
				continue;	// normal timeout
			}	

			int nRecv = ::recvfrom( m_Socket, (char*) DataBuf,
				4096, 0, (SOCKADDR*)&sockAddr, &nAddrLen );

			if ( nRecv == SOCKET_ERROR ) {
				int iError = WSAGetLastError();
				if ( iError == WSAEMSGSIZE ) {
					HE_ASSERT( FALSE );
				}
			}

			UInt32	nRecvAddress = sockAddr.sin_addr.S_un.S_addr;

			m_dbgPacketNumber++;
			AtlTrace( L"Packet received # %d \n", m_dbgPacketNumber );

			BOOL bKeeper = TRUE;
			if ( bKeeper ) {
				ProcessBuffer( DataBuf, nRecv, nRecvAddress );
			}

		}
		catch (...) {
			::MessageBox( NULL, L"Exception",
				L"Caught an exception the capture loop", MB_OK );
		}
	}

	m_dbgPacketNumber = 0;

	CloseSocket();
}


// -----------------------------------------------------------------------------
//		BindSocket
// -----------------------------------------------------------------------------

int CSampleAdapter::BindSocket()
{
	ObjectLock	lock( this );

	CloseSocket();

	m_Socket = socket( PF_INET, SOCK_DGRAM, 0 );
	if ( m_Socket == INVALID_SOCKET ) return -1;

	// Set the socket options
	int no = 1;
	int nErr = setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &no, sizeof( no ) );
	ASSERT( nErr == 0 );
	if ( nErr != 0 ) return nErr;
	
	// bind socket to any address on port 9996
	SOCKADDR_IN	sockAddr;
	memset( &sockAddr, 0, sizeof( sockAddr ) );
	sockAddr.sin_family			= AF_INET;
 
	sockAddr.sin_addr.s_addr	= INADDR_ANY;
	sockAddr.sin_port			= htons( (u_short)m_nPort );
 
	nErr = ::bind( m_Socket, (SOCKADDR*)&sockAddr, sizeof( sockAddr ) );
	ASSERT( nErr == 0 );

	if ( nErr == 0 ) {
		// make nonblocking
		u_long	nArg = 1;
		nErr = ::ioctlsocket( m_Socket, FIONBIO, &nArg );
		ASSERT( nErr == 0 );
	}
	else {
		DWORD	nLastErr = WSAGetLastError();
		CString	csErr;
		csErr.Format( L"Socket Error: %d", nLastErr );
		//AfxMessageBox( csErr );
		return -1;
	}

	return 0;
}


// -----------------------------------------------------------------------------
//		GetDataFileFullName
// -----------------------------------------------------------------------------

CHeBSTR
CSampleAdapter::GetDataFileFullName()
{
	HeLib::CHeBSTR DataFileFullName = GetDataPath();
	if ( DataFileFullName.Length() != 0 ) {		
		DataFileFullName += (CHeBSTR) kTag_ConfigFileName;
	}
	return	DataFileFullName;
}

// -----------------------------------------------------------------------------
//		GetDataPath
// -----------------------------------------------------------------------------

CHeBSTR
CSampleAdapter::GetDataPath()
{	
	return m_strDataPath;
}


// -----------------------------------------------------------------------------
//		AddPluginCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::AddPluginCapture(
	const Helium::HeID& idCapture,
	void*				pPlugin )
{
	if ( !pPlugin ) return HE_S_FALSE;

	HeResult hr = HE_S_OK;

	m_heCapIdPluginLock.Lock();
	HE_TRY {
		m_mapCapIDToPluginObj.insert(
			make_pair( idCapture, static_cast<COmniPlugin*>( pPlugin ) ) );
	}
	HE_CATCH( hr )
	m_heCapIdPluginLock.Unlock();

	return hr;
}


// -----------------------------------------------------------------------------
//		RemovePluginCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::RemovePluginCapture(
	const Helium::HeID& idCapture )
{	
	HeResult hr = HE_S_OK;

	m_heCapIdPluginLock.Lock();
	HE_TRY {
		m_mapCapIDToPluginObj.erase( idCapture );
	}
	HE_CATCH( hr )
	m_heCapIdPluginLock.Unlock();

	return hr;
}


// -----------------------------------------------------------------------------
//		GetPluginCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetPluginCapture(
	const Helium::HeID&	idCapture,
	void*				pPlugin )
{	
	HeResult hr = HE_S_OK;

	m_heCapIdPluginLock.Lock();
	HE_TRY {
		pPlugin = NULL;
		map<HeID, COmniPlugin*>::iterator mapIter;
		mapIter = m_mapCapIDToPluginObj.find( idCapture );
		if ( mapIter != m_mapCapIDToPluginObj.end() ) {
			pPlugin = mapIter->second;
		}
	}
	HE_CATCH( hr )	
	m_heCapIdPluginLock.Unlock();

	return hr;
}


// -----------------------------------------------------------------------------
//		AddPluginCaptureOption
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::AddPluginCaptureOption(
	const Helium::HeID& idCapture )
{
	HeResult hr = HE_S_OK;
	
	HE_TRY
	{
		SetCaptureSrcIP( idCapture, 0 );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		GetPluginCaptureOption
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::GetPluginCaptureOption(
	const Helium::HeID&	idCapture,
	void*				pOption )
{
	if ( !pOption ) return HE_S_FALSE;

	HeResult hr = HE_S_OK;

	HE_TRY {
		GetCaptureSrcIP( idCapture, static_cast<UInt32*>( pOption ) );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		SetPluginCaptureOption
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::SetPluginCaptureOption(
	const Helium::HeID&	idCapture,
	void*				pOption )
{
	if ( !pOption ) return HE_S_FALSE;

	HeResult hr = HE_S_OK;

	HE_TRY {
		SetCaptureSrcIP( idCapture, *static_cast<UInt32*>( pOption ) );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		RemovePluginCaptureOption
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CSampleAdapter::RemovePluginCaptureOption(
	const Helium::HeID& idCapture )
{
	HeResult hr = HE_S_OK;
	
	HE_TRY {
		RemoveCaptureSrcIP( idCapture );
	}
	HE_CATCH( hr )

	return hr;
}
