// =============================================================================
//	SampleAdapter.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "RemoteAdapter.h"
#include "omnicommands.h"
#include <algorithm>
#include <string>

using namespace std;
using namespace Helium;
using namespace HeLib;

UInt64 nInfoLinkSpeed = 1000000000;
UInt64 nGetLinkSpeed = 1000000000;


// =============================================================================
//		CRemoteAdapter
// =============================================================================

CRemoteAdapter::CRemoteAdapter()
	: m_OmniAdapter( this )
{
}


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
CRemoteAdapter::FinalConstruct()
{
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
CRemoteAdapter::FinalRelease()
{
}


// -----------------------------------------------------------------------------
//		Run
// -----------------------------------------------------------------------------

HeResult
CRemoteAdapter::Run()
{
	HeResult hr = HE_S_OK;
	int	nResult = m_OmniAdapter.Run( nullptr );
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
		m_OmniAdapter.m_AdapterInfo.SetLinkSpeed( nInfoLinkSpeed );
		m_OmniAdapter.CopyAdapterInfo( ppAdapterInfo );
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
	const CPeekString&	strAdapterName( m_OmniAdapter.GetAdapterName() );
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

	tMediaType mt = m_OmniAdapter.GetMediaType();
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

	tMediaType mt = m_OmniAdapter.GetMediaType();
	*pMediaSubType = mt.fSubType;
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetLinkSpeed
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetLinkSpeed(
	UInt64*	pullLinkSpeed )
{
	if ( pullLinkSpeed == NULL ) return HE_E_NULL_POINTER;

	//*pullLinkSpeed = m_spAdapter->GetLinkSpeed();
	*pullLinkSpeed = nGetLinkSpeed;

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetAddress
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetAddress(
	UInt8*	pAddress )
{
	if ( pAddress == NULL ) return HE_E_NULL_POINTER;

	memset( pAddress, 0, 6 );	// 6 - from BYTE pAddress[6] Ethernet Address size.
	CEthernetAddress	eaAddress;
	if ( m_OmniAdapter.GetAddress( eaAddress ) ) {
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

	*pCLSID = CRemoteAdapter_CID;

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
	HeResult	hr = HE_S_OK;
	int			nResult = m_OmniAdapter.GetStatistics( pHandler, ppStatistics );
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
	if ( pbCapturing == nullptr ) return HE_E_NULL_POINTER;

	*pbCapturing = (m_OmniAdapter.IsCapturing()) ? TRUE : FALSE;
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::PauseCapture
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::PauseCapture()
{
	HeResult hr = HE_S_OK;

	HE_TRY {
		m_OmniAdapter.PauseCapture();
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
		m_OmniAdapter.ResumeCapture();
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
	int	nResult = m_OmniAdapter.StartCapture();
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
	int	nResult = m_OmniAdapter.StopCapture();
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
	// Sanity checks.
	ASSERT( pHandler != NULL );
	if ( pHandler == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {
		m_OmniAdapter.AddPacketHandler( CPacketHandler( pHandler ) );
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
	// Sanity checks.
	ASSERT( pHandler != NULL );
	if ( pHandler == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {
		m_OmniAdapter.RemovePacketHandler( CPacketHandler( pHandler ) );
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
	HeResult hr = S_OK;

	HE_TRY {
		if ( pUnkSite != NULL ) {
			HeThrowIfFailed( IHeObjectWithSiteImpl<CRemoteAdapter>::SetSite( pUnkSite ) );
			m_OmniAdapter.SetInterfacePointers( pUnkSite );
			m_OmniAdapter.Initialize();
		}
		else {
			m_OmniAdapter.Terminate();
		}
	}
	HE_CATCH( hr )

	return hr;
}
