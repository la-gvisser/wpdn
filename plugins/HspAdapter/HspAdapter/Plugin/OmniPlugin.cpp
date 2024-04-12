// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "Peek.h"
#include "MemUtil.h"
#include "Packet.h"
#ifdef IMPLEMENT_PLUGINMESSAGE
#include "PeekMessage.h"
#endif	// IMPLEMENT_PLUGINMESSAGE
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


// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0xF7C3A8AB, 0x6EAD, 0x4494, { 0x94, 0x9F, 0xE5, 0xBF, 0xBF, 0x53, 0x53, 0xEA } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"HspAdapter" );
CPeekString		COmniPlugin::s_strPublisher( L"Savvius, Inc." );
CVersion		COmniPlugin::s_Version( 4, 3, 0, 0 );

#ifdef _WIN32
int				COmniPlugin::s_nCaptureTabId( IDR_ZIP_CAPTURETAB );
int				COmniPlugin::s_nOptionsId( IDR_ZIP_OPTIONS );
#endif // _WIN32

// Single Adapter Object Id:  {6CBEDBF2-F04D-45D0-A72D-165CD09412D9}
#define kAdapterIdentifier	L"{6CBEDBF2-F04D-45D0-A72D-165CD09412D9}"
GUID g_idHspAdapter =
	{ 0x6CBEDBF2, 0xF04D, 0x45D0, { 0xA7, 0x2D, 0x16, 0x5C, 0xD0, 0x94, 0x12, 0xD9 } };
CGlobalId	g_idAdapterObject( g_idHspAdapter );

//GUID			g_idUIHandler = CefPreferencesHandler_ID;
GUID			g_idUIHandler = HTMLPreferencesHandler_ID;


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
{
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
//		CreateAdapter
// -----------------------------------------------------------------------------

void
COmniPlugin::CreateAdapter(
	const CGlobalId&	inId,
	const CPeekString&	inName )
{
	try {
		// Check for an existing plugin adapter with the same name.
		CAdapterManager		adptManager( m_EngineProxy.GetAdapterManager() );
		CAdapter			adptExisting( adptManager.GetAdapter(
			peekPluginAdapter, GUID_NULL, inName ) );
		if ( adptExisting.IsValid() ) return;

		CRemotePlugin*		pRemotePlugin( GetRemotePlugin() );
		if ( pRemotePlugin ) {
			CAdapterInfo	adptInfo( COmniAdapter::CreateAdapterInfo(
				inName, inId ) );
			CAdapter	adapter( pRemotePlugin->CreateAdapter( adptInfo ) );
			if ( adapter.IsNotValid() ) return;
			if ( !RegisterAdapter( adapter ) ) {
				// Delete the adapter
			}
		}
	}
	catch ( ... ) {
		;
	}
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
	CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
	CAdapter		adpt = adptManager.GetAdapter( peekPluginAdapter, g_idAdapterObject, nullptr );
	DeleteAdapter( adpt );
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

const CPeekString&
COmniPlugin::GetName()
{
	if ( s_strName.IsEmpty() ) {
#ifdef PEEK_UI
		s_strName.LoadString( IDS_PLUGIN_NAME );
#endif // PEEK_UI
	}

	_ASSERTE( !s_strName.IsEmpty() );

	return s_strName;
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
//		OnContextPrefsm_pathMonitor
// -----------------------------------------------------------------------------

int
COmniPlugin::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	// Test ioPrefs.IsLoading() or ioPrefs->IsStoring()
	//  to take load/store specific actions.

	// To always display the default options, don't load
	// the preferences when the Id is nullptr (case 2 above).
	if ( ioPrefs.IsLoading() && (m_pRemotePlugin && m_pRemotePlugin->GetContextId().IsNull()) ) {
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
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\HspAdapter-About.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\HspAdapter-Options.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\HspAdapter-Tab.zip
	// and will use them instead of the ZIP resource.
	// This is useful for the development of the web pages/sites.

	if ( !m_strPluginPath.empty() ) {
		m_nCaptureTabFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathCaptureTab( m_strPluginPath );
		pathCaptureTab.SetFileName( s_strName + L"-CaptureTab.zip" );
		m_strCaptureTabFilePath = pathCaptureTab.get();

		// The NoDefaultsPrefs flag instructs the CEF UI Handler to ignore the settings
		// of the previous capture. Remove this flag to initialize new capture settings
		// with the previous capture's settings.
		m_nOptionsFlags = peekPluginHandlerUI_Zipped | peekPluginHandlerUI_NoDefaultPrefs;
		FilePath::Path	pathOptions( m_strPluginPath );
		pathOptions.SetFileName( s_strName + L"-Options.zip" );
		m_strOptionsFilePath = pathOptions.get();
	}

	// Create the single adapter if it does not exist.
	try {
		CAdapterManager	adptManager( m_EngineProxy.GetAdapterManager() );
		CAdapter		adpt = adptManager.GetAdapter( peekPluginAdapter, g_idAdapterObject, nullptr );
		if ( adpt.IsNotValid() ) {
			(void) CreateAdapter( g_idAdapterObject, s_strName );
		}
	}
	catch (...) {
		;
	}

	m_bInitialized = true;

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
COmniPlugin::OnUnload()
{
	// OnLoad is not called if the plugin is initialized.
	// There is no reference counting, don't uninitialize.
	// m_bInitialized = false;

	DeleteAllAdapters();

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

	adptManager.RemoveAdapter( adptExisting );
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
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
COmniPlugin::OnWritePrefs()
{
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		SetOptions
// -----------------------------------------------------------------------------

void
COmniPlugin::SetOptions(
	CPeekDataModeler&	inPrefs )
{
	(void)inPrefs;
}


#ifdef IMPLEMENT_PLUGINMESSAGE
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

	// Create the response.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Result: 0: Success, 1: Failure.
	pMessage->SetResult( 1 );

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

	try {
		// Check for an existing plugin adapter with the same name.
		CAdapterManager		adptManager( m_EngineProxy.GetAdapterManager() );
		CAdapter			adapter( adptManager.GetAdapter(
			peekPluginAdapter, pMessage->GetId(), nullptr ) );
		if ( adapter.IsValid() ) {
			CRemotePlugin*		pRemotePlugin( GetRemotePlugin() );
			if ( pRemotePlugin ) {
				CAdapterInfo	adptInfo( adapter.GetAdapterInfo() );
				adptInfo.SetDescription( pMessage->GetAdapterName() );
			}
		}
	}
	catch ( ... ) {
		;
	}

	// Create the response.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Result: 0: Success, 1: Failure.
	pMessage->SetResult( 0 );

	return PEEK_PLUGIN_SUCCESS;
}

#endif	// IMPLEMENT_PLUGINMESSAGE
