// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "Peek.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "MediaTypes.h"
#include "PluginHandlersInterface.h"
#include "RemotePlugin.h"
#include "ContextManager.h"
#include "FileEx.h"

#ifdef _WIN32
#include <wtypes.h>
#endif // _WIN32


// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

GUID			g_PluginId = { 0x75AC6E1C, 0x55AD, 0x4A5A, { 0xA2, 0xD9, 0x49, 0x73, 0x52, 0x1E, 0xF8, 0x8F } };
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"EmailCaptureOE" );
CPeekString		COmniPlugin::s_strPublisher( L"Savvius, Inc." );
CVersion		COmniPlugin::s_Version( 2, 0, 0, 0 );

#ifdef _WIN32
int				COmniPlugin::s_nAboutId( IDR_ZIP_ABOUT );
int				COmniPlugin::s_nCaptureTabId( IDR_ZIP_CAPTURE_TAB );
int				COmniPlugin::s_nEngineTabId( IDR_ZIP_ENGINE_TAB );
int				COmniPlugin::s_nOptionsId( IDR_ZIP_OPTIONS );
#endif // _WIN32

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
		CRemotePlugin*	pRemotePlugin( GetRemotePlugin() );
		_ASSERTE( pRemotePlugin != nullptr );
		if ( pRemotePlugin != nullptr ) {
			CAppConfig	AppConfig( pRemotePlugin->GetPeekProxy().GetAppConfig() );
			if ( AppConfig.IsValid() ) {

#ifdef _DEBUG
				// TODO select the directory for Linux
				CPeekString	strConfig( AppConfig.GetConfigDir() );
				CPeekString	strLib( AppConfig.GetLibDir() );
				CPeekString	strResource( AppConfig.GetResourceDir() );
				CPeekString	strDoc( AppConfig.GetDocDir() );
				CPeekString	strLog( AppConfig.GetLogDir() );
				CPeekString	strData( AppConfig.GetDataDir() );
#endif
			
				CPeekString	strDirectory( AppConfig.GetLibDir() );
				if ( !strDirectory.empty() ) {
					FilePath::Path	path( strDirectory );
					path.Append( L"plugins", true );	// Lowercase 'p' for Linux.

					// This path exists in OmniEngine 8.0.1 and later.
					// CFileEx::MakePath( path.get() );

					m_strPluginPath = path.get();
				}
			}
		}
	}

	_ASSERTE( !m_strPluginPath.empty() );
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

	// The About, Options and Capture Tab web page/site are embedded as "ZIP" resources.
	// The following allows the plugin to look for alternate versions of the zip file.
	// The plugin will look for the files:
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\EmailCaptureOE-About.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\EmailCaptureOE-Options.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\EmailCaptureOE-Tab.zip
	// and will use them instead of the ZIP resource.
	// This is useful for the development of the web pages/sites.

	if ( !m_strPluginPath.empty() ) {
		m_nAboutFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathAbout( m_strPluginPath );
		pathAbout.SetFileName( s_strName + L"-About.zip" );
		m_strAboutFilePath = pathAbout.get();

		m_nCaptureTabFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathCaptureTab( m_strPluginPath );
		pathCaptureTab.SetFileName( s_strName + L"-CaptureTab.zip" );
		m_strCaptureTabFilePath = pathCaptureTab.get();

		m_nEngineTabFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathEngineTab( m_strPluginPath );
		pathEngineTab.SetFileName( s_strName + L"-EngineTab.zip" );
		m_strEngineTabFilePath = pathEngineTab.get();

		m_nOptionsFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathOptions( m_strPluginPath );
		pathOptions.SetFileName( s_strName + L"-Options.zip" );
		m_strOptionsFilePath = pathOptions.get();
	}

	m_Mimepp.Initialize();

	m_bInitialized = true;

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
COmniPlugin::OnUnload()
{
	m_bInitialized = false;

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
//		OnGetAboutData
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetAboutData(
	UInt32*			outFlags,
	CPeekStream&	outData )
{
	_ASSERTE( outFlags != NULL );
	if ( outFlags == NULL ) return PEEK_PLUGIN_FAILURE;

	CFileEx	fAboutPage( m_strAboutFilePath, CFileEx::modeRead );
	if ( fAboutPage.IsOpen() ) {
		UInt64	nFileSize( fAboutPage.GetLength() );
		if ( nFileSize < kMaxUInt32 ) {
			CPeekStream	psAboutPage;
			psAboutPage.SetSize( nFileSize );
			fAboutPage.Read( static_cast<UInt32>( nFileSize ), psAboutPage.GetData() );

			*outFlags = m_nAboutFlags;
			outData.Append( psAboutPage );
		}
	}
#ifdef _WIN32
	else {
		HMODULE	hModule( AfxGetResourceHandle() );
		HRSRC	hRsrc( ::FindResource( hModule, MAKEINTRESOURCE( s_nAboutId ), L"ZIP" ) );
		if ( hRsrc ){
			DWORD	dwSize( ::SizeofResource( hModule, hRsrc ) );
			HGLOBAL	hResource( ::LoadResource( hModule, hRsrc ) );
			if ( hResource ) {
				const char*	pResource( reinterpret_cast<const char*>( ::LockResource( hResource ) ) );

				CPeekStream	psAboutData;
				psAboutData.SetSize( dwSize );
				memcpy( psAboutData.GetData(), pResource, dwSize );

				*outFlags = m_nAboutFlags;
				outData.Append( psAboutData );
			}
		}
	}
#endif

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
	_ASSERTE( outFlags != NULL );
	if ( outFlags == NULL ) return PEEK_PLUGIN_FAILURE;

	CFileEx	fLibrary( m_strCaptureTabFilePath, CFileEx::modeRead );
	if ( fLibrary.IsOpen() ) {
		UInt64	nFileSize( fLibrary.GetLength() );
		if ( nFileSize < kMaxUInt32 ) {
			CPeekStream	psLibrary;
			psLibrary.SetSize( nFileSize );
			fLibrary.Read( static_cast<UInt32>( nFileSize ), psLibrary.GetData() );

			*outFlags = m_nCaptureTabFlags;
			outData.Append( psLibrary );
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

				CPeekStream	psLibraryData;
				psLibraryData.SetSize( dwSize );
				memcpy( psLibraryData.GetData(), pResource, dwSize );

				*outFlags = m_nCaptureTabFlags;
				outData.Append( psLibraryData );
			}
		}
	}
#endif	// _WIN32

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnGetEngineTabData
// -----------------------------------------------------------------------------

int
COmniPlugin::OnGetEngineTabData(
	UInt32*			outFlags,
	CPeekStream&	outData )
{
	_ASSERTE( outFlags != NULL );
	if ( outFlags == NULL ) return PEEK_PLUGIN_FAILURE;

	CFileEx	fLibrary( m_strEngineTabFilePath, CFileEx::modeRead );
	if ( fLibrary.IsOpen() ) {
		UInt64	nFileSize( fLibrary.GetLength() );
		if ( nFileSize < kMaxUInt32 ) {
			CPeekStream	psLibrary;
			psLibrary.SetSize( nFileSize );
			fLibrary.Read( static_cast<UInt32>( nFileSize ), psLibrary.GetData() );

			*outFlags = m_nEngineTabFlags;
			outData.Append( psLibrary );
		}
	}
#ifdef _WIN32
	else {
		HMODULE	hModule( AfxGetResourceHandle() );
		HRSRC	hRsrc( ::FindResource( hModule, MAKEINTRESOURCE( s_nEngineTabId ), L"ZIP" ) );
		if ( hRsrc ){
			DWORD	dwSize( ::SizeofResource( hModule, hRsrc ) );
			HGLOBAL	hResource( ::LoadResource( hModule, hRsrc ) );
			if ( hResource ) {
				const char*	pResource( reinterpret_cast<const char*>( ::LockResource( hResource ) ) );

				CPeekStream	psLibraryData;
				psLibraryData.SetSize( dwSize );
				memcpy( psLibraryData.GetData(), pResource, dwSize );

				*outFlags = m_nEngineTabFlags;
				outData.Append( psLibraryData );
			}
		}
	}
#endif	// _WIN32

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
	_ASSERTE( outFlags != NULL );
	if ( outFlags == NULL ) return PEEK_PLUGIN_FAILURE;

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
	m_Options.Model( inPrefs );

	OnWritePrefs();
}


// .............................................................................
//		Engine-Only Methods
// .............................................................................

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
			case CMessageGetAllOptions::s_nMessageType:
				nResult = ProcessGetAllOptions( spMessage.get() );
				break;
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			spMessage->StoreResponse( outResponse );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessGetAllOptions
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessGetAllOptions(
	CPeekMessage* ioMessage )
{
	CMessageGetAllOptions*	pMessage( dynamic_cast<CMessageGetAllOptions*>( ioMessage ) );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Build (set) the Options list.
	CMessageGetAllOptions::CNamedOptionsList&	ayNamedOptions = pMessage->GetNamedOptions();

	CContextManagerPtr	spContextManager( GetContextManager() );
	if ( spContextManager ) {
		size_t	nCount = spContextManager->GetCount();
		for ( size_t i = 0; i < nCount; i++ ) {
			CPeekContext*		pPeekContext( spContextManager->GetItem( i ) );
			CPeekEngineContext*	pContext( dynamic_cast<CPeekEngineContext*>( pPeekContext ) );
			if ( pContext ) {
				if ( pContext->GetId().IsNull() ) continue;
				CGlobalId	idContext( pContext->GetId() );
				CPeekString	strName( pContext->GetCaptureName() );
				if ( strName.IsEmpty() ) continue;

				COptions*	optContext = pContext->GetOptions();

				CMessageGetAllOptions::TNamedOptions	noContext = { idContext, strName, *optContext };
				ayNamedOptions.push_back( noContext );
			}
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}