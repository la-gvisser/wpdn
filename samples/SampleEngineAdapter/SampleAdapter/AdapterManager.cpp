// =============================================================================
//	AdapterManager.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "AdapterManager.h"
#include "SampleAdapter.h"


// =============================================================================
//		CAdapterManager
// =============================================================================

// -----------------------------------------------------------------------------
//		LoadSettings
// -----------------------------------------------------------------------------

HeResult
CAdapterManager::LoadSettings()
{
	HeResult hr = HE_S_OK;

	HE_TRY {		
		// create a new SampleAdapter
		CHePtr<IAdapter> spAdapter;
		HeThrowIfFailed( CHeCreator< CHeObject<CSampleAdapter> >::CreateInstance(
			NULL, IAdapter::GetIID(), reinterpret_cast<void**>( &spAdapter.p ) ) );
		HeThrowIfFailed( AddAdapter( spAdapter.p ) );		
		m_spAdapter = spAdapter;			
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		AddAdapter
// -----------------------------------------------------------------------------

HeResult
CAdapterManager::AddAdapter(
	IAdapter*	pAdapter )
{
	HeResult hr = HE_S_OK;
	HE_TRY {		
		CHePtr<IAdapterInfo> spInfo;
		HeThrowIfFailed( pAdapter->GetAdapterInfo( &spInfo.p ) );

		CHePtr<IAdapterConfig> spConfig;
		HeThrowIfFailed( spConfig.CreateInstance( "PeekCore.AdapterConfig" ) );
		
		ULONGLONG ullLinkSpeed;
		HeThrowIfFailed( spInfo->GetLinkSpeed( &ullLinkSpeed ) );
		HeThrowIfFailed( spConfig->SetDefaultLinkSpeed( ullLinkSpeed ) );
		HeThrowIfFailed( spConfig->SetLinkSpeed( ullLinkSpeed ) );
		HeThrowIfFailed( spConfig->SetRingBufferSize( 0 ) );	// N/A
		HeThrowIfFailed( m_spAdapterManager->AddAdapter( pAdapter, spInfo, spConfig ) );
	}
	HE_CATCH( hr )

	return hr;
}


// -----------------------------------------------------------------------------
//		SetSite
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CAdapterManager::SetSite(
	IHeUnknown*	pUnkSite )
{
	HeResult hr = HE_S_OK;
	HE_TRY {
		HeThrowIfFailed( __super::SetSite( pUnkSite ) );
		m_spUnkSite = pUnkSite;
		m_spServices = m_spUnkSite;
		HeThrowIf( m_spServices == NULL );
		CSampleAdapter::m_spServices = m_spServices;		
		
		HeThrowIfFailed( m_spServices->QueryService( IAdapterManager::GetIID(), &m_spAdapterManager.p ) );
		HeThrowIfFailed( LoadSettings() );	
	}
	HE_CATCH( hr )

	return hr;
}
