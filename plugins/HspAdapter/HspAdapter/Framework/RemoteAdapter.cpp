// =============================================================================
//	RemoteAdapter.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "RemoteAdapter.h"
#include "RemoteStatsPlugin.h"

#include "ixmldom.h"
#include "XmlUtil.h"
#include <algorithm>
#include <string>

using namespace std;
using namespace Helium;
using namespace HeLib;

#define kPluginName				L"HspAdapter"
#define kTag_ConfigFileName		L"HspAdapter.xml"
#define kTag_Options			L"Options"

HeID		GUID_RemoteAdapter = CRemoteAdapter_CID;
CPeekString	g_strRemoteAdapterName( L"HspAdapter" );


// =============================================================================
//		CRemoteAdapter
// =============================================================================

CRemoteAdapter::CRemoteAdapter()
	:	m_OmniAdapter()
	,	m_nRingBufferSize( 1 )
{
}

CRemoteAdapter::~CRemoteAdapter()
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
	int	nResult = m_OmniAdapter.Run();
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		hr = E_FAIL;
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		Initialize
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::Initialize(
	IAdapterInfo*	pAdapterInfo )
{
	_ASSERTE( pAdapterInfo != nullptr );
	if ( pAdapterInfo == nullptr ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult hr = HE_S_OK;
	HE_TRY {
		m_OmniAdapter.Initialize( CAdapterInfo( pAdapterInfo ), m_EngineProxy );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetAdapterInfo
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetAdapterInfo(
	IAdapterInfo**	ppAdapterInfo )
{
	if ( ppAdapterInfo == nullptr ) return HE_E_NULL_POINTER;

	HeResult hr = HE_S_OK;
	HE_TRY {
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
	// HeHEBSTRFromString will check for pbstrName == nullptr
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
	if ( pMediaType == nullptr ) return HE_E_NULL_POINTER;

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
	if ( pMediaSubType == nullptr ) return HE_E_NULL_POINTER;

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
	if ( pullLinkSpeed == nullptr ) return HE_E_NULL_POINTER;

	*pullLinkSpeed = m_OmniAdapter.GetLinkSpeed();
	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		IAdapter::GetAddress
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetAddress(
	UInt8	pAddress[6] )
{
	if ( pAddress == nullptr ) return HE_E_NULL_POINTER;

	memset( pAddress, 0, 6 ); // From BYTE pAddress[6] - size of Ethernet Address.
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
	if ( pbNetSupp == nullptr ) return HE_E_NULL_POINTER;

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
	HE_ASSERT( pCLSID != nullptr );
	if ( pCLSID == nullptr ) return HE_E_NULL_POINTER;

	*pCLSID = *(reinterpret_cast<HeCID*>( &GUID_RemoteAdapter ));

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		ICapturePackets::IsCapturing
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::IsCapturing(
	BOOL*	pbCapturing )
{
	if ( pbCapturing == nullptr ) return HE_E_NULL_POINTER;

	HeResult hr = HE_S_OK;

	HE_TRY {
		*pbCapturing = (m_OmniAdapter.IsCapturing()) ? TRUE : FALSE;
	}
	HE_CATCH( hr )

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
		m_OmniAdapter.SetPaused( true );
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
		m_OmniAdapter.SetPaused( false );
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
	ObjectLock	lock( this );
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
	ObjectLock	lock( this );
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
	//ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pHandler != nullptr );
	if ( pHandler == nullptr ) return HE_E_NULL_POINTER;

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
	//ObjectLock	lock( this );

	// Sanity checks.
	ASSERT( pHandler != nullptr );
	if ( pHandler == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	HE_TRY {
		m_OmniAdapter.RemovePacketHandler( CPacketHandler( pHandler ) );
	}
	HE_CATCH( hr )

	return hr;
}


// ----------------------------------------------------------------------------
//		IPluginAdapter::DeleteAdapter
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::DeleteAdapter(
	const Helium::HeID&	adapterID )
{
	// Pass the request off to the Remote Plugin.
	return m_spPluginAdapter->DeleteAdapter( adapterID );
}


// ----------------------------------------------------------------------------
//		INdisAdapter::GetRingBufferSize
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetRingBufferSize(
	UInt32* pulRingBufferSize )
{
	*pulRingBufferSize = m_nRingBufferSize;
	return HE_S_OK;
}


// ----------------------------------------------------------------------------
//		INdisAdapter::SetRingBufferSize
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::SetRingBufferSize(
	UInt32 ulRingBufferSize )
{
	ulRingBufferSize = m_nRingBufferSize;
	return HE_S_OK;
}


// ----------------------------------------------------------------------------
//		INdisAdapter::GetPacketsDropped
// ----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::GetPacketsDropped(
	UInt64* pulPacktesDropped )
{
	_ASSERTE( pulPacktesDropped != nullptr );
	if ( pulPacktesDropped == nullptr ) return HE_E_NULL_POINTER;

	_ASSERTE( m_OmniAdapter.IsValid() );
	if ( m_OmniAdapter.IsNotValid() ) return HE_E_FAIL;

	*pulPacktesDropped  = m_OmniAdapter.GetDroppedPackets();
	return HE_S_OK;
}


#ifdef IMPLEMENT_SUMMARY
// -----------------------------------------------------------------------------
//		IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::UpdateSummaryStats(
	IHeUnknown*	pSnapshot )
{
	_ASSERTE( pSnapshot != nullptr );
	if ( pSnapshot == nullptr ) return HE_E_NULL_POINTER;

	CHeQIPtr<ISummaryStatsSnapshot2>	spSnapshot( pSnapshot );
	CSnapshot	snapshot( spSnapshot );

	void*		pContext( nullptr );
	CHeQIPtr<ISnapshotContext>	spSnapshotContext( pSnapshot );
	if ( spSnapshotContext ) {
		spSnapshotContext->GetContext( &pContext );
	}

	m_OmniAdapter.OnUpdateSummaryStats( snapshot, pContext );

	return HE_S_OK;
}
#endif	// IMPLEMENT_SUMMARY

// -----------------------------------------------------------------------------
//		IHePersistXml::Load
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::Load(
	IHeUnknown*	inDocument,
	IHeUnknown*	inNode )
{
	_ASSERTE( m_OmniAdapter.IsValid() );
	if ( m_OmniAdapter.IsNotValid() ) return HE_E_FAIL;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	HE_TRY {
		Peek::ThrowIf( !CPeekXml::Validate( inDocument, inNode ) );
		m_spPrefsModeler = CPeekDataModelerPtr( new CPeekDataModeler( inDocument, inNode, kModeler_Load ) );
		if ( m_spPrefsModeler.get() != nullptr ) {
			ASSERT( m_spPrefsModeler->IsOpen() );
			m_OmniAdapter.OnContextPrefs( *m_spPrefsModeler.get() );
		}
		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IHePersistXml::Save
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::Save(
	IHeUnknown*	ioDocument,
	IHeUnknown*	ioNode,
	int			/*inClearDirty*/ )
{
	_ASSERTE( m_OmniAdapter.IsValid() );
	if ( m_OmniAdapter.IsNotValid() ) return HE_E_FAIL;

	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;
	HE_TRY {
		Peek::ThrowIf( !CPeekXml::Validate( ioDocument, ioNode ) );
		CPeekDataModeler pPrefsModeler( ioDocument, ioNode, kModeler_Store );

		m_OmniAdapter.OnContextPrefs( pPrefsModeler );
		pPrefsModeler.End();

		hr = HE_S_OK;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		IHeObjectWithSiteImpl::SetSite
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemoteAdapter::SetSite(
	IHeUnknown*	pUnkSite )
{
	if ( pUnkSite == nullptr ) return HE_S_OK;

	HeResult hr = HE_S_OK;

	HE_TRY {
		HeThrowIfFailed( IHeObjectWithSiteImpl<CRemoteAdapter>::SetSite( pUnkSite ) );
		m_EngineProxy.SetInterfacePointers( pUnkSite, nullptr );
	}
	HE_CATCH( hr )

	return hr;
}
