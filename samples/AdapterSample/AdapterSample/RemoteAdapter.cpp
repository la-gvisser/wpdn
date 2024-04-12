// =============================================================================
//	SampleAdapter.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "RemoteAdapter.h"

#include "ixmldom.h"
#include "xmlutil.h"
#include <algorithm>
#include <string>

using namespace std;
using namespace Helium;
using namespace HeLib;

#define kPluginName				L"Sample Adapter"
#define kTag_ConfigFileName		L"SampleAdapter.xml"
#define kTag_Options			L"Options"
#define kTag_Port				L"Port"

static HeID GUID_RemoteAdapter = CRemoteAdapter_CID;

CHePtr<Helium::IHeServiceProvider>	CRemoteAdapter::m_spServices = 0;


// =============================================================================
//		CRemoteAdapter
// =============================================================================

// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
CRemoteAdapter::FinalConstruct()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
		CHePtr<IAppConfig>	spAppConfig;
		if ( m_spServices ) {
			hr = m_spServices->QueryService( IAppConfig::GetIID(), IAppConfig::GetIID(),
				reinterpret_cast<void**>( &spAppConfig.p ) );

			if ( spAppConfig ) {
				CHeBSTR	bstrDataPath;
				hr = spAppConfig->GetConfigDir( &bstrDataPath.m_str );
				if ( HE_SUCCEEDED( hr ) ) {
					CPeekString	strDataPath( bstrDataPath );
					m_Adapter.SetDataPath( bstrDataPath.m_str );
				}
			}
		}

		CHeBSTR hestrConfigFile = GetDataFileFullName();
		CHePtr<Xml::IXMLDOMDocument>	spDoc;
		wstring strConfigFile( hestrConfigFile.m_str );
		XmlUtil::LoadXmlFile( strConfigFile.data(), &(reinterpret_cast<Xml::IXMLDOMDocument*>( spDoc.p )) );
		if ( !spDoc.p ) return hr;

		CHePtr<Xml::IXMLDOMElement> spRootNode;
		spDoc->get_documentElement( &(reinterpret_cast<Xml::IXMLDOMElement*>(spRootNode.p)) );
		if ( !spRootNode.p ) return hr;

#pragma message( "todo: move to Omni Adapter." )

		string strFlowPort;
		if ( XmlUtil::GetAttribute( spRootNode, kTag_Port, strFlowPort ) ) {
			CPeekString	strPort( strFlowPort );
			m_Adapter.SetPort( strPort );
		}
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
CRemoteAdapter::FinalRelease()
{
}


// -----------------------------------------------------------------------------
//		GetDataFileFullName
// -----------------------------------------------------------------------------

CPeekString
CRemoteAdapter::GetDataFileFullName()
{
	CPeekString strDataFileFullName( GetDataPath() );
	if ( !strDataFileFullName.IsEmpty() ) {
		strDataFileFullName += kTag_ConfigFileName;
	}
	return strDataFileFullName;
}


// -----------------------------------------------------------------------------
//		Run
// -----------------------------------------------------------------------------

HeResult
CRemoteAdapter::Run()
{
	HeResult hr = HE_S_OK;
	int	nResult = m_Adapter.Run();
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		hr = E_FAIL;
	}

	return hr;

}


// ============================================================================
//	Interface Methods
// ============================================================================

// -----------------------------------------------------------------------------
//		IAdapter::GetAdapterInfo
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetAdapterInfo(
	IAdapterInfo**	ppAdapterInfo )
{
	if ( ppAdapterInfo == NULL ) return HE_E_NULL_POINTER;

	HeResult hr = HE_S_OK;
	HE_TRY {
		m_Adapter.CopyAdapterInfo( ppAdapterInfo );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetHardwareOptions
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetHardwareOptions(
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
//		IAdapter::SetHardwareOptions
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::SetHardwareOptions(
	IHeUnknown*	pHardwareOptions )
{
	HeResult hr = HE_S_OK;

	if ( pHardwareOptions ) {
		;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetAdapterType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetAdapterType(
	enum PeekAdapterType*	pAdapterType )
{
	if ( pAdapterType ) {
		*pAdapterType = peekPluginAdapter;
	}
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetName
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetName(
	HEBSTR* pbstrName )
{
	// HeHEBSTRFromString will check for pbstrName == NULL
	const CPeekString&	strAdapterName( m_Adapter.GetAdapterName() );
	return HeHEBSTRFromString( strAdapterName, pbstrName );
}


// -----------------------------------------------------------------------------
//		IAdapter::GetMediaType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetMediaType(
	enum TMediaType*	pMediaType )
{
	if ( pMediaType == NULL ) return HE_E_NULL_POINTER;

	tMediaType mt = m_Adapter.GetMediaType();
	*pMediaType = mt.fType;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetMediaSubType
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetMediaSubType(
	enum TMediaSubType*	pMediaSubType )
{
	if ( pMediaSubType == NULL ) return HE_E_NULL_POINTER;

	tMediaType mt = m_Adapter.GetMediaType();
	*pMediaSubType = mt.fSubType;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetLinkSpeed
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetLinkSpeed(
	ULONGLONG*	pullLinkSpeed )
{
	if ( pullLinkSpeed == NULL ) return HE_E_NULL_POINTER;

	*pullLinkSpeed = m_Adapter.GetLinkSpeed();
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetAddress
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetAddress(
	BYTE	pAddress[6] )
{
	if ( pAddress == NULL ) return HE_E_NULL_POINTER;

#pragma message( "todo: refactor" )
	memset( pAddress, 0, 6 );	// 6 - from BYTE pAddress[6] Ethernet Address size.
	CEthernetAddress	eaAddress;
	if ( m_Adapter.GetAddress( eaAddress ) ) {
		tEthernetAddress ea = eaAddress;
		memcpy( pAddress, ea.bytes, sizeof( eaAddress ) );
	}
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetNetSupportDuringCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetNetSupportDuringCapture(
	BOOL*	pbNetSupp )
{
	if ( pbNetSupp == NULL ) return HE_E_NULL_POINTER;

	*pbNetSupp = TRUE;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::Open
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::Open()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		IAdapter::Close
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::Close()
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
CRemoteAdapter::GetClassID(
	HeCID* pCLSID )
{
	// Sanity checks.
	HE_ASSERT( pCLSID != NULL );
	if ( pCLSID == NULL ) return HE_E_NULL_POINTER;

	*pCLSID = *(reinterpret_cast<HeCID*>( &GUID_RemoteAdapter ));

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapterStats::GetStatistics
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetStatistics(
	IPacketHandler*	pHandler,
	void**			ppStatistics )
{
	HeResult hr = HE_S_OK;
	int	nResult = m_Adapter.GetStatistics( pHandler, ppStatistics );
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		hr = E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::IsCapturing
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::IsCapturing(
	BOOL*	pbCapturing )
{
	pbCapturing;
	HeResult hr = HE_S_OK;
	return hr;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::PauseCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::PauseCapture()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::ResumeCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::ResumeCapture()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::StartCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::StartCapture()
{
	HeResult hr = HE_S_OK;
	int	nResult = m_Adapter.StartCapture();
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		hr = E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::StopCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::StopCapture()
{
	HeResult hr = HE_S_OK;
	int	nResult = m_Adapter.StopCapture();
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		hr = E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		IPacketProvider::AddPacketHandler
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::AddPacketHandler(
	IPacketHandler*	pHandler )
{
	//ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pHandler != NULL );
	if ( pHandler == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {
		m_Adapter.AddPacketHandler( CPacketHandler( pHandler ) );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		IPacketProvider::RemovePacketHandler
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::RemovePacketHandler(
	IPacketHandler* pHandler )
{
	//ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pHandler != NULL );
	if ( pHandler == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {
		m_Adapter.RemovePacketHandler( CPacketHandler( pHandler ) );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		IHeObjectWithSiteImpl::SetSite
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::SetSite(
	IHeUnknown*	pUnkSite )
{
	if ( pUnkSite == NULL ) return S_OK;

	HeResult hr = S_OK;

	HE_TRY {
		HeThrowIfFailed( IHeObjectWithSiteImpl<CRemoteAdapter>::SetSite( pUnkSite ) );
		CHePtr<IHeServiceProvider>	spServices;
		hr = GetSite( IHeServiceProvider::GetIID(), (void**) &spServices.p );
		HeThrowIfFailed( hr );

		CHePtr<IPeekCapture>	spCapture;
		hr = spServices->QueryInterface( &spCapture.p );
		HeThrowIfFailed( hr );

		CHePtr<IPacketHandler>	spPacketHandler;
		hr = spCapture->QueryInterface( &spPacketHandler.p );
		HeThrowIfFailed( hr );

		CPacketHandler	ph( spPacketHandler );
		m_PacketHandler = ph;
	}
	HE_CATCH( hr )

	return hr;
}
