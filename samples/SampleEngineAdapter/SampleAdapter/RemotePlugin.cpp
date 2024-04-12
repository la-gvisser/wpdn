// =============================================================================
//	RemotePlugin.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "hlcom.h"

#include "Protospecs.h"

#include "RemotePlugin.h"
#include "resource.h"

#include "OmniPlugin.h"
#include "PluginXml.h"
#include "Snapshot.h"
#include "CapturePlugin.h"
#include "SampleAdapter.h"

#include "omnigraphs.h"
#include "peekfilters.h"
#include "peeknames.h"
#include "xmlutil.h"
#include "ixmldom.h"


// ============================================================================
//		CRemotePlugin
// ============================================================================

CRemotePlugin::CRemotePlugin()
	:	m_pPlugin( NULL )
{
	m_CaptureId.SetNull();
}


// -----------------------------------------------------------------------------
//		BeginCommand
// -----------------------------------------------------------------------------

HeResult
CRemotePlugin::BeginCommand()
{
	HeResult	hr = HE_S_OK;

	try {
		// Get the service provider.
		CHePtr<IHeServiceProvider>	spServices;
		hr = GetSite( IHeServiceProvider::GetIID(), (void**) &spServices.p );
		if ( HE_SUCCEEDED( hr ) ) {
			// Get the notification service.
			CHePtr<INotify>	spNotify;
			hr = spServices->QueryService( INotify::GetIID(), &spNotify.p );
			if ( HE_SUCCEEDED( hr ) ) {
				m_PeekProxy.SetPtr( CPeekProxy::kNotify, spNotify );
			}

			// Get the name table and Lookup Name Entry services.
			const HeID			sidNameTable = NameTable_SID;
			CHePtr<INameTable>	spNameTable;
			hr = spServices->QueryService( sidNameTable, &spNameTable.p );
			if ( HE_SUCCEEDED( hr ) ) {
				m_PeekProxy.SetPtr( CPeekProxy::kNameTable, spNameTable );
			}
			CHePtr<ILookupNameEntry>	spLookupNameEntry;
			hr = spServices->QueryService( sidNameTable, &spLookupNameEntry.p );
			if ( HE_SUCCEEDED( hr ) ) {
				m_PeekProxy.SetPtr( CPeekProxy::kLookupNameEntry, spLookupNameEntry );
			}

			// Additional Interfaces
			CHePtr<IPeekSetup>	spPeekSetup;
			hr = spServices->QueryInterface( &spPeekSetup.p );

			CHePtr<IGraphTemplateMgr>	spGraphTmplMgr;
			hr = spServices->QueryService( IGraphTemplateMgr::GetIID(), &spGraphTmplMgr.p );

			CHePtr<IQueryLog>	spQueryLog;
			hr = spServices->QueryService( ILogMessage::GetIID(), &spQueryLog.p );

			CHePtr<IAdapterManager>	spAdapterManager;
			hr = spServices->QueryService( IAdapterManager::GetIID(), &spAdapterManager.p );

			CHePtr<IFilterCollection>	spFilters;
			hr = spServices->QueryService( IFilterCollection::GetIID(), &spFilters.p );

			CHePtr<IAlarmCollection>	spAlarms;
			hr = spServices->QueryService( IAlarmCollection::GetIID(), &spAlarms.p );

#if 0
			try {
				// Open the database.
				SQLiteDB			db;
				CHePtr<IAppConfig>	spAppConfig;
				HeThrowIfFailed( spServices->QueryService( IAppConfig::GetIID(), &spAppConfig.p ) );
				HeThrowIfFailed( OmniDatabaseUtil::OpenDatabase( db, spAppConfig ) );

				SQLiteQuery	query = db.Query(
					"SELECT COUNT(*) FROM PacketFiles WHERE (Status = 1) OR (Status = 2);" );
				if ( query.NextRow() ) {
					SetProperty( spPropertyBag, L"FileCount", query.GetSInt32( 0 ) );
				}
			}
			catch ( SQLiteException& e ) {
				e = e;	// Unused in release build.
				TRACE( "Database error(%d) getting packet file count\n", (int) e );
			}
			catch ( ... ) {
			}
#endif

		}
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		EndCommand
// -----------------------------------------------------------------------------

HeResult
CRemotePlugin::EndCommand()
{
	HeResult	hr = HE_S_OK;

	try {
		m_PeekProxy.ReleasePtr( CPeekProxy::kNotify );
		m_PeekProxy.ReleasePtr( CPeekProxy::kNameTable );
		m_PeekProxy.ReleasePtr( CPeekProxy::kLookupNameEntry );
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

HeResult
CRemotePlugin::FinalConstruct()
{
	HeResult	hr = HE_S_OK;

	try {
		hr = m_PeekProxy.FinalConstruct();			

		m_pPlugin = new COmniPlugin( m_PeekProxy );
		if ( m_pPlugin ) {
			m_pPlugin->OnReadPrefs();
			m_pPlugin->OnCreateContext();
		}
	}
	HE_CATCH( hr );

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
CRemotePlugin::FinalRelease()
{
	if ( m_pPlugin ) {
		m_pPlugin->OnWritePrefs();
	}
	delete m_pPlugin;
	m_pPlugin = NULL;
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetName(
	HEBSTR*	pbstrName )
{
//	ObjectLock	lock( this );

	// Sanity checks.
	HE_ASSERT( pbstrName != NULL );
	if ( pbstrName == NULL ) return HE_E_NULL_POINTER;

	CStringW	strName;
	if ( COmniPlugin::OnGetName( strName ) == PEEK_PLUGIN_SUCCESS ) {
		*pbstrName = HeAllocStringLen( strName.GetBuffer(), strName.GetLength() );
		if ( *pbstrName == NULL ) return HE_E_OUT_OF_MEMORY;
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		GetClassID
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::GetClassID(
	HeCID*	pCLSID )
{
	// Sanity checks.
	HE_ASSERT( pCLSID != NULL );
	if ( pCLSID == NULL ) return HE_E_NULL_POINTER;

	GUID	guid;
	if ( COmniPlugin::OnGetClassId( guid ) == PEEK_PLUGIN_SUCCESS ) {
		*pCLSID = *(reinterpret_cast<HeCID*>( &guid ));
	}

	return HE_S_OK;
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Load(
	const HeID&	guidContext )
{
	HeResult	hr = HE_S_OK;

	try {
		if (!guidContext.IsNull())
		{	
			// associate capture id to plugin object in the adapter class
			m_CaptureId = guidContext;
			CHePtr<IHeServiceProvider>	spServices = CSampleAdapter::m_spServices;

			if (spServices)
			{
				CHePtr<IPeekSetup>		spPeekSetup;
				HeThrowIfFailed( spServices.QueryInterface( &spPeekSetup ) );
				CHePtr<IPeekCaptures>	spCaptures;
				HeThrowIfFailed( spPeekSetup->GetCaptures( &spCaptures.p ) );
				CHePtr<IPeekCapture>	spCapture;
				HeThrowIfFailed( spCaptures->ItemFromID(m_CaptureId, &spCapture.p));

				CHePtr<IAdapter>	spAdapter;
				hr = spCapture->GetAdapter( &spAdapter.p );
				HeThrowIfFailed(hr);

				CHePtr<IPluginCaptureOption>	spPluginCaptureOption;
				HeThrowIfFailed( spAdapter->QueryInterface(IPluginCaptureOption::GetIID(), (void**) &spPluginCaptureOption.p ) );

				//UInt32 nOption = dynamic_cast<COmniPlugin*>(m_pPlugin)->GetSourceIP();
				HeThrowIfFailed( spPluginCaptureOption->AddPluginCaptureOption( m_CaptureId ) );
			}
		}
		
		if ( m_pPlugin ) {
			GUID	guid( HeID2GUID( guidContext ) );
			m_pPlugin->OnLoad( guid );
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		Unload
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Unload()
{
	HeResult	hr = HE_S_OK;
	try {
		if ( m_pPlugin ) {
			m_pPlugin->OnUnload();

			// remove association of capture id to plugin object from the adapter class
			CHePtr<IHeServiceProvider>	spServices = CSampleAdapter::m_spServices;
			if ( spServices ) {
				CHePtr<IPeekSetup>		spPeekSetup;
				HeThrowIfFailed( spServices.QueryInterface( &spPeekSetup ) );

				CHePtr<IPeekCaptures>	spCaptures;
				HeThrowIfFailed( spPeekSetup->GetCaptures( &spCaptures.p ) );

				CHePtr<IPeekCapture>	spCapture;
				HeThrowIfFailed( spCaptures->ItemFromID(m_CaptureId, &spCapture.p));

				CHePtr<IAdapter>	spAdapter;
				HeThrowIfFailed( spCapture->GetAdapter( &spAdapter.p ) );

				CHePtr<IPluginCaptureOption>	spPluginCaptureOption;
				HeThrowIfFailed( spAdapter->QueryInterface( IPluginCaptureOption::GetIID(), (void**) &spPluginCaptureOption.p ) );

				HeThrowIfFailed( spPluginCaptureOption->RemovePluginCaptureOption( m_CaptureId ) );
			}
		
			delete m_pPlugin;
			m_pPlugin = NULL;
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Load(
	IHeUnknown*	pXmlDoc,
	IHeUnknown*	pXmlNode )
{
	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;

	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			CHeQIPtr<Xml::IXMLDOMDocument>	spXmlDoc( pXmlDoc );
			CHeQIPtr<Xml::IXMLDOMNode>		spXmlNode( pXmlNode );

#if 0
			XmlUtil::SaveXmlFile(kDumpLoadFile, spXmlDoc);
#endif

			CXmlDocument	xmlDocument( spXmlDoc );
			CXmlElement		xmlElement( &xmlDocument, spXmlNode );
			m_pPlugin->OnSetContextPrefs( pXmlDoc, pXmlNode, FALSE);

			CHePtr<IHeServiceProvider>	spServices = CSampleAdapter::m_spServices;

			if ( spServices ) {
				CHePtr<IPeekSetup>		spPeekSetup;
				HeThrowIfFailed( spServices.QueryInterface( &spPeekSetup ) );

				CHePtr<IPeekCaptures>	spCaptures;
				HeThrowIfFailed( spPeekSetup->GetCaptures( &spCaptures.p ) );

				CHePtr<IPeekCapture>	spCapture;
				HeThrowIfFailed( spCaptures->ItemFromID(m_CaptureId, &spCapture.p));

				CHePtr<IAdapter>	spAdapter;
				HeThrowIfFailed( spCapture->GetAdapter( &spAdapter.p ) );

				CHePtr<IPluginCaptureOption>	spPluginCaptureOption;
				HeThrowIfFailed( spAdapter->QueryInterface( IPluginCaptureOption::GetIID(), (void**) &spPluginCaptureOption.p ) );

				UInt32 nOption = dynamic_cast<COmniPlugin*>(m_pPlugin)->GetSourceIP();
				HeThrowIfFailed( spPluginCaptureOption->SetPluginCaptureOption( m_CaptureId, &nOption ) );
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		Save
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::Save(
	IHeUnknown*	pXmlDoc,
	IHeUnknown*	pXmlNode,
	int			bClearDirty )
{
	ObjectLock	lock( this );

	HeResult	hr = HE_S_OK;

	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			CHeQIPtr<Xml::IXMLDOMDocument>	spXmlDoc( pXmlDoc );
			CHeQIPtr<Xml::IXMLDOMNode>		spXmlNode( pXmlNode );
#if 0

			XmlUtil::SaveXmlFile(kDumpStoreFile, spXmlDoc);
#endif

			CXmlDocument	xmlDocument( spXmlDoc );
			CXmlElement		xmlElement( &xmlDocument, spXmlNode );
			m_pPlugin->OnGetContextPrefs(  pXmlDoc, pXmlNode, bClearDirty );
			hr = HE_S_OK;
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		ShowAbout
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ShowAbout()
{
#ifdef HE_WIN32
	HeResult	hr = HE_S_OK;

	try {
		if ( m_pPlugin ) m_pPlugin->OnShowAbout();
	}
	HE_CATCH( hr );

	return hr;
#else
	return HE_E_NOT_IMPLEMENTED;
#endif
}


// -----------------------------------------------------------------------------
//		ShowOptions
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ShowOptions()
{
#ifdef HE_WIN32
	HeResult	hr = HE_S_OK;

	try {
		if ( m_pPlugin ) m_pPlugin->OnShowOptions();
	}
	HE_CATCH( hr );

	return hr;
#else
	return HE_E_NOT_IMPLEMENTED;
#endif
}


// -----------------------------------------------------------------------------
//		FilterPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::FilterPacket(
	IPacket*		pPacket,
	UInt16*			pcbBytesToAccept,
	Helium::HeID*	pFilterID )
{
	ObjectLock	lock( this );

	// Sanity checks.
	HE_ASSERT( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pcbBytesToAccept != NULL );
	if ( pcbBytesToAccept == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pFilterID != NULL );
	if ( pFilterID == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			CPacket		Packet( pPacket );
			int			nResult = m_pPlugin->OnFilter( Packet, pcbBytesToAccept,
				reinterpret_cast<GUID&>( *pFilterID ) );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}

// -----------------------------------------------------------------------------
//		SummarizePacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::SummarizePacket(
	IPacket*			pPacket,
	HEBSTR*				pbstrSummary,
	UInt32*				pColor,
	PeekSummaryLayer*	pLayer )
{
	ObjectLock	lock( this );

	// Sanity checks.
	HE_ASSERT( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pbstrSummary != NULL );
	if ( pbstrSummary == NULL ) return HE_E_NULL_POINTER;
	HE_ASSERT( pColor != NULL );
	if ( pColor == NULL ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			CPacket		Packet( pPacket );
			CString		strSummary;
			int			nResult = m_pPlugin->OnGetPacketString( Packet, strSummary,
				*pColor, reinterpret_cast<SummaryLayer*>( pLayer ) );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		UpdateSummaryStats
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::UpdateSummaryStats(
	IHeUnknown*	pSnapshot )
{
	// Sanity checks.
	HE_ASSERT( pSnapshot != NULL );
	if ( pSnapshot == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			CSnapshot	Snapshot( pSnapshot );
			int			nResult = m_pPlugin->OnSummary( Snapshot );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		ProcessPacket
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ProcessPacket(
	IPacket*	pPacket )
{
	// Sanity checks.
	HE_ASSERT( pPacket != NULL );
	if ( pPacket == NULL ) return HE_E_NULL_POINTER;

	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			CPacket		Packet( pPacket );
			int			nResult = m_pPlugin->OnProcessPacket( Packet );
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		ResetProcessing
// -----------------------------------------------------------------------------

HE_IMETHODIMP
CRemotePlugin::ResetProcessing()
{
	ObjectLock	lock( this );

	HeResult	hr = HE_E_FAIL;
	try {
		if ( m_pPlugin ) {
			CProxyState	ProxyState( *this );
			int	nResult = m_pPlugin->OnReset();
			if ( nResult == PEEK_PLUGIN_SUCCESS ) {
				hr = HE_S_OK;
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}
