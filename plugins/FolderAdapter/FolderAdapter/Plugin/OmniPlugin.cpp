// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2010-2018 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "Peek.h"
#include "MemUtil.h"
#include "Options.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "MediaTypes.h"
#include "PluginHandlersInterface.h"
#include "RemotePlugin.h"
#include "ContextManager.h"
#include "FileEx.h"
#include "PeekXml.h"
#include "OmniAdapter.h"

#ifdef _WIN32
#include <wtypes.h>
#endif // _WIN32

#define LOG_DETAIL

// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0x7355AD46, 0xB1B0, 0x459F, { 0x87, 0xFF, 0xCE, 0xAF, 0x1B, 0x2F, 0xCE, 0x25 } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"FolderAdapterOE" );
CPeekString		COmniPlugin::s_strDisplayName( L"FolderAdapterOE" );
CPeekString		COmniPlugin::s_strPublisher( L"LiveAction, Inc." );
CVersion		COmniPlugin::s_Version( 1, 12, 0, 0 );

#ifdef _WIN32
int				COmniPlugin::s_nCaptureTabId( IDR_ZIP_CAPTURETAB );
int				COmniPlugin::s_nOptionsId( IDR_ZIP_OPTIONS );
#endif // _WIN32

// Single Adapter Object Id: {11999B07-B460-4617-BD4E-6881E4D1763E}
#define kAdapterIdentifier	L"{11999B07-B460-4617-BD4E-6881E4D1763E}"
GUID g_idFolderAdapter =
	{ 0x11999B07, 0xB460, 0x4617, { 0xBD, 0x4E, 0x68, 0x81, 0xE4, 0xD1, 0x76, 0x3E } };
CGlobalId	g_idAdapterObject( g_idFolderAdapter );

GUID			g_idUIHandler = CefPreferencesHandler_ID;
// GUID			g_idUIHandler = HTMLPreferencesHandler_ID;


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
	,	m_nRemoteClientCount( 0 )
{
}


// -----------------------------------------------------------------------------
//		CreateAdapter
// -----------------------------------------------------------------------------

bool
COmniPlugin::CreateAdapter(
	CAdapterOptions&	inAdapterOptions,
	bool				inWritePrefs )
{
	try {
		// Check for an existing plugin adapter with the same name.
		CAdapterManager		adptManager( m_EngineProxy.GetAdapterManager() );

		// The Adapter Manager is not reliably finding existing adapters.
		// CAdapter			adptExisting = adptManager.GetAdapter(
		// 	peekPluginAdapter, GUID_NULL, inAdapterOptions.GetName() );
		// void*	pAdapter( adptExisting.GetPtr() );
		// if ( adptExisting.IsValid() ) return false;

		CAdapterInfoList	InfoList( adptManager.GetAdapterInfoList() );
		CPeekString 		strInfoName( inAdapterOptions.GetName() );
		SInt32				nCount( InfoList.GetCount() );
		for ( SInt32 i = 0; i < nCount; i++ ) {
			CAdapterInfo	info( InfoList.GetItem( i ) );
			if ( info.GetAdapterType() == peekPluginAdapter ) {
				CPeekString	strInfoDescription( info.GetDescription() );
				if ( strInfoDescription == strInfoName ) {
					return false;
				}
			}
		}

		CRemotePlugin*		pRemotePlugin( GetRemotePlugin() );
		if ( pRemotePlugin ) {
			CAdapterInfo	adptInfo( COmniAdapter::CreateAdapterInfo(
				inAdapterOptions.GetName(), inAdapterOptions.GetId() ) );
			CAdapter	adapter = pRemotePlugin->CreateAdapter( adptInfo );
			if ( adapter.IsNotValid() ) return false;

			CHeQIPtr<Helium::IHePersistXml>	spPersist( adapter.GetPtr() );
			if ( !spPersist ) return false;

			// Load the Adapter Options into the Adapter.
			CPeekDataModeler dmAdapter( L"CreateAdapter", kModeler_Store );
			inAdapterOptions.Model( dmAdapter );

			CPeekXmlDocument	spDocument;
			CPeekXmlElement		spElement;
			if ( !dmAdapter.GetXml().GetLoadStore( spDocument, spElement ) ) return false;

			// Load with the new modeler.
			if ( !HE_SUCCEEDED( spPersist->Load( spDocument, spElement ) ) ) {
				// Delete the adapter?
				return false;
			}

			if ( !RegisterAdapter( adapter ) ) {
				// Delete the adapter
				return false;
			}

			m_Options.Add( inAdapterOptions );
			if ( inWritePrefs ) {
				OnWritePrefs();
			}

			CPeekString	strMsg( L"FolderAdapterOE: Created Adapter: " );
			strMsg.Append( inAdapterOptions.GetId().Format() );
			strMsg.Append( L": " );
			strMsg.Append( inAdapterOptions.GetName() );
			DoLogMessage( 0, peekSeverityInformational, strMsg, strMsg );
		}
	}
	catch (...) {
		;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		BuildPluginPath
// -----------------------------------------------------------------------------

void
COmniPlugin::BuildPluginPath()
{
	if ( m_strPluginPath.empty() ) {
		CAppConfig	AppConfig( m_EngineProxy.GetAppConfig() );
		if ( AppConfig.IsValid() ) {
			CPeekString	strDirectory( AppConfig.GetLibraryDirectory() );
			if ( !strDirectory.empty() ) {
				FilePath::Path	path( strDirectory );
				path.Append( L"plugins", true );	// Lowercase 'p' for Linux.
				m_strPluginPath = path.get();
			}
		}
	}

	_ASSERTE( !m_strPluginPath.empty() );
}


// -----------------------------------------------------------------------------
//		DeleteAdapter
// -----------------------------------------------------------------------------

void
COmniPlugin::DeleteAdapter(
	CAdapter&	inAdapter )
{
	if ( inAdapter.IsValid() ) {
		CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
		adptManager.RemoveAdapter( inAdapter );

		CRemotePlugin*		pRemotePlugin( GetRemotePlugin() );
		if ( pRemotePlugin ) {
			IAdapter*	pAdapter = inAdapter.GetPtr();
			inAdapter.ReleasePtr();
			pRemotePlugin->DeleteAdapter( pAdapter  );
		}
	}
}


// -----------------------------------------------------------------------------
//		DeleteAllAdapters
// -----------------------------------------------------------------------------

void
COmniPlugin::DeleteAllAdapters()
{
	CAdapterManager					adptManager( m_EngineProxy.GetAdapterManager() );
	const COptions::CAdapterList&	ayAdapters( m_Options.GetAdapterList() );
	for ( auto itr = ayAdapters.begin(); itr != ayAdapters.end(); ++itr ) {
		CAdapter	adpt = adptManager.GetAdapter( peekPluginAdapter, itr->GetId(), nullptr );
		DeleteAdapter( adpt );
	}
}


// -----------------------------------------------------------------------------
//		DoLogMessage
// -----------------------------------------------------------------------------

void
COmniPlugin::DoLogMessage(
	UInt64				inTimeStamp,
	int					inSeverity,
	const CPeekString&	inShortMessage,
	const CPeekString&	inLongMessage )
{
	CLogService&	logger = m_EngineProxy.GetLogService();
	CGlobalId		idNull( GUID_NULL );
	logger.DoLogMessage( idNull, idNull, 0,
		inTimeStamp, inSeverity, inShortMessage, inLongMessage );
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

const CPeekString&
COmniPlugin::GetName()
{
	if ( s_strDisplayName.IsEmpty() ) {
#ifdef PEEK_UI
		s_strDisplayName.LoadString( IDS_PLUGIN_NAME );
#endif // PEEK_UI
	}

	_ASSERTE( !s_strDisplayName.IsEmpty() );

	return s_strDisplayName;
}


// -----------------------------------------------------------------------------
//		GetPublisher
// -----------------------------------------------------------------------------

const CPeekString&
COmniPlugin::GetPublisher()
{
	_ASSERTE( !s_strPublisher.IsEmpty() );

	return s_strPublisher;
}


// -----------------------------------------------------------------------------
//		GetVersion
// -----------------------------------------------------------------------------

const CVersion&
COmniPlugin::GetVersion()
{
	return s_Version;
}


// -----------------------------------------------------------------------------
//		ModifyAdapter
// -----------------------------------------------------------------------------

bool
COmniPlugin::ModifyAdapter(
	CAdapter&				inAdapter,
	const CAdapterOptions&	inOptions )
{
	bool	bResult( false );

	if ( inAdapter.IsValid() ) {
		COmniAdapter*	pAdapter( inAdapter.GetOmniAdapter() );
		bResult = pAdapter->UpdateOptions( inOptions );
		if ( bResult ) {
			m_Options.Update( inOptions );
			OnWritePrefs();
		}
	}

	return bResult;
}


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	// Test ioPrefs.IsLoading() or ioPrefs->IsStoring()
	//  to take load/store specific actions.

	// To always display the default options, don't load
	// the preferences when the Id is nullptr (case 2 above).
	CRemotePlugin*	pRemotePlugin( GetRemotePlugin() );
	if ( ioPrefs.IsLoading() && (pRemotePlugin && pRemotePlugin->GetContextId().IsNull()) ) {
		m_Options.Reset();
	}
	else {
		m_Options.Model( ioPrefs );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnLoad
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoad(
	CRemotePlugin*	inRemotePlugin )
{
	int	nResult( CPeekPlugin::OnLoad( inRemotePlugin ) );
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}

	BuildPluginPath();

	CPeekString		strFileName( s_strName );
	strFileName.Append( L".xml" );
	FilePath::Path	pathPrefs( m_strPluginPath );
	pathPrefs.Append( strFileName );
	m_strPrefsFileName = pathPrefs.get();

	// The About, Options and Capture Tab web page/site are embedded as "ZIP" resources.
	// The following allows the plugin to look for alternate versions of the zip file.
	// The plugin will look for the files:
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\FolderAdapter-About.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\FolderAdapter-Options.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\FolderAdapter-Tab.zip
	// and will use them instead of the ZIP resource.
	// This is useful for the development of the web pages/sites.

	if ( !m_strPluginPath.empty() ) {
		m_nCaptureTabFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathCaptureTab( m_strPluginPath );
		pathCaptureTab.SetFileName( s_strName + L"-CaptureTab.zip" );
		m_strCaptureTabFilePath = pathCaptureTab.get();

		// The NoDefaultsPrefs flag instructs the Cef UI Handler to ignore the settings
		// of the previous capture. Remove this flag to initialize new capture settings
		// with the previous capture's settings.
		// Don't set this for an Adapter.
		m_nOptionsFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathOptions( m_strPluginPath );
		pathOptions.SetFileName( s_strName + L"-Options.zip" );
		m_strOptionsFilePath = pathOptions.get();
	}

	m_bInitialized = true;

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
COmniPlugin::OnUnload(
	CRemotePlugin*	inRemotePlugin )
{
	// OnLoad is not called if the plugin is initialized.
	// There is no reference counting, don't uninitialize.
	// m_bInitialized = false;

	//
	// DeleteAllAdapters();

	int	nResult( CPeekPlugin::OnUnload( inRemotePlugin ) );
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetOptionsHandlerId
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetOptionsHandlerId(
	CGlobalId&	outId )
{
	outId = g_idUIHandler;

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnDeleteAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::OnDeleteAdapter(
	CGlobalId	inAdapterId )
{
	CAdapterManager		adptManager( m_EngineProxy.GetAdapterManager() );
	CAdapter			adptExisting = adptManager.GetAdapter(
		peekPluginAdapter, inAdapterId, nullptr );
	if ( adptExisting.IsNotValid() ) return PEEK_PLUGIN_FAILURE;

	DeleteAdapter( adptExisting );

	m_Options.Delete( inAdapterId );
	OnWritePrefs();

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetCaptureTabData
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetCaptureTabData(
	UInt32*			outFlags,
	CPeekStream&	outData )
{
	_ASSERTE( outFlags != nullptr );
	if ( outFlags == nullptr ) return PEEK_PLUGIN_FAILURE;

	CFileEx	fWebPage( m_strCaptureTabFilePath, CFileEx::modeRead );
	if ( fWebPage.IsOpen() ) {
		UInt64	nFileSize( fWebPage.GetLength() );
		if ( nFileSize < kMaxUInt32 ) {
			CPeekStream	psWebPage;
			psWebPage.SetSize( nFileSize );
			fWebPage.Read( static_cast<UInt32>( nFileSize ), psWebPage.GetData() );

			*outFlags = m_nCaptureTabFlags;
			outData.Append( psWebPage );
		}
	}
#ifdef _WIN32
	else {
		HMODULE	hModule( AfxGetResourceHandle() );
		HRSRC	hRsrc( ::FindResource( hModule, MAKEINTRESOURCE( s_nCaptureTabId ), L"ZIP" ) );
		if ( hRsrc ){
			DWORD	dwSize( ::SizeofResource( hModule, hRsrc ) );
			HGLOBAL	hResource( ::LoadResource( hModule, hRsrc ) );
			if ( hResource ) {
				const char*	pResource( reinterpret_cast<const char*>( ::LockResource( hResource ) ) );

				CPeekStream	psOptionsData;
				psOptionsData.SetSize( dwSize );
				memcpy( psOptionsData.GetData(), pResource, dwSize );

				*outFlags = m_nCaptureTabFlags;
				outData.Append( psOptionsData );
			}
		}
	}
#endif

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetOptionsData
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetOptionsData(
	UInt32*			outFlags,
	CPeekStream&	outData )
{
	_ASSERTE( outFlags != nullptr );
	if ( outFlags == nullptr ) return PEEK_PLUGIN_FAILURE;

	CFileEx	fWebPage( m_strOptionsFilePath, CFileEx::modeRead );
	if ( fWebPage.IsOpen() ) {
		UInt64	nFileSize( fWebPage.GetLength() );
		if ( nFileSize < kMaxUInt32 ) {
			CPeekStream	psWebPage;
			psWebPage.SetSize( nFileSize );
			fWebPage.Read( static_cast<UInt32>( nFileSize ), psWebPage.GetData() );

			*outFlags = m_nOptionsFlags;
			outData.Append( psWebPage );
		}
	}
#ifdef _WIN32
	else {
		HMODULE	hModule( AfxGetResourceHandle() );
		HRSRC	hRsrc( ::FindResource( hModule, MAKEINTRESOURCE( s_nOptionsId ), L"ZIP" ) );
		if ( hRsrc ){
			DWORD	dwSize( ::SizeofResource( hModule, hRsrc ) );
			HGLOBAL	hResource( ::LoadResource( hModule, hRsrc ) );
			if ( hResource ) {
				const char*	pResource( reinterpret_cast<const char*>( ::LockResource( hResource ) ) );

				CPeekStream	psOptionsData;
				psOptionsData.SetSize( dwSize );
				memcpy( psOptionsData.GetData(), pResource, dwSize );

				*outFlags = m_nOptionsFlags;
				outData.Append( psOptionsData );
			}
		}
	}
#endif

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReadPrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnReadPrefs()
{
	// Uncomment the sleep() to debug engine when the service is started:
	//		set a break-point on the next line.
	//		start omnid ($ systemctl start omnid) and Attach to the process within 20 seconds.
	//		wait until the process stops at the break-point. 
	// sleep( 20 );

	if ( m_Options.IsEmpty() ) {
		CPeekDataModeler	dmOptions( m_strPrefsFileName, s_strName );

		CPeekDataElement	elemRoot( L"FolderAdapters", dmOptions );
		if ( elemRoot ) {
			UInt32	nCount = elemRoot.GetChildCount( L"FolderAdapter" );
			for ( UInt32 nIndex = 0; nIndex < nCount; ++nIndex ) {
				CPeekDataElement	elemAdapter( L"FolderAdapter", elemRoot, nIndex );
				if ( elemAdapter.IsNotValid() ) {
					continue;
				}

				CAdapterOptions		optAdapter;
				optAdapter.Model( elemAdapter );
				CreateAdapter( optAdapter, false );
			}
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnWritePrefs()
{
	CPeekDataModeler	dmOptions( s_strName );
	m_Options.Model( dmOptions );

	CPeekPersistFile	spFile = dmOptions.GetXml();
	if ( spFile ) {
		spFile->Save( m_strPrefsFileName, FALSE );
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

int
COmniPlugin::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	// Create and Load the appropriate Message object.
	std::unique_ptr<CPeekMessage>	spMessage = MessageFactory( inMessage );
	if ( spMessage.get() != nullptr ) {
		switch ( spMessage->GetType() ) {
			case CMessageCreateAdapter::s_nMessageType:
				nResult = ProcessCreateAdapter( spMessage.get() );
				break;

			case CMessageGetAdapter::s_nMessageType:
				nResult = ProcessGetAdapter( spMessage.get() );
				break;

			case CMessageModifyAdapter::s_nMessageType:
				nResult = ProcessModifyAdapter( spMessage.get() );
				break;
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			spMessage->StoreResponse( outResponse );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessCreateAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessCreateAdapter(
	CPeekMessage* ioMessage )
{
	CMessageCreateAdapter*	pMessage( dynamic_cast<CMessageCreateAdapter*>( ioMessage ) );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	CAdapterOptions	optAdapter;
	optAdapter.SetId( CGlobalId( true ) );
	optAdapter.SetMonitor( pMessage->GetMonitor() );
	optAdapter.SetMask( pMessage->GetMask() );
	optAdapter.SetName( pMessage->GetMonitor() );
	optAdapter.SetSave( pMessage->GetSave() );
	optAdapter.SetSpeed( pMessage->GetSpeed() );

	bool	bResult = CreateAdapter( optAdapter, true );

	// Create the response.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Result: 0: Success, 1: Failure.
	pMessage->SetResult( (bResult) ? 0 : 1 );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ProcessGetAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessGetAdapter(
	CPeekMessage* ioMessage )
{
	CMessageGetAdapter*	pMessage( dynamic_cast<CMessageGetAdapter*>( ioMessage ) );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	CGlobalId		id = pMessage->GetId();
	CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
	CAdapter		adapter = adptManager.GetAdapter( peekPluginAdapter, id, nullptr );

	// Create the response.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	if ( adapter.IsValid() ) {
		CAdapterInfo	adptInfo = adapter.GetAdapterInfo();
		const COptions&	opt = GetOptions();
		CAdapterOptions	adptOptions;
		if ( opt.Find( id, adptOptions ) ) {
			pMessage->SetAdapterOptions( adptOptions );
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ProcessModifyAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessModifyAdapter(
	CPeekMessage* ioMessage )
{
	CMessageModifyAdapter*	pMessage( dynamic_cast<CMessageModifyAdapter*>( ioMessage ) );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	CAdapterOptions	optAdapter( pMessage->GetAdapterOptions() );
	CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
	CAdapter		adapter = adptManager.GetAdapter( peekPluginAdapter, optAdapter.GetId(), nullptr );

	bool	bResult( ModifyAdapter( adapter, optAdapter ) );

	// Create the response.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Result: 0: Success, 1: Failure.
	pMessage->SetResult( (bResult) ? 0 : 1 );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniPlugin::SetOptions(
	CPeekDataModeler&	inPrefs )
{
	m_Options.Model( inPrefs );

	OnWritePrefs();
}
