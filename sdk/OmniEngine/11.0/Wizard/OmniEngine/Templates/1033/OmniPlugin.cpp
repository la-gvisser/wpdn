// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "Peek.h"
[!if !NOTIFY_UIHANDLER]
#include "AboutDialog.h"
[!if NOTIFY_TAB]
#include "TabHost.h"
#include "PluginTab.h"
[!endif]
[!endif]
#include "MemUtil.h"
#include "Packet.h"
#include "PeekMessage.h"
#include "Snapshot.h"
#include "MediaTypes.h"
[!if NOTIFY_UIHANDLER]
#include "PluginHandlersInterface.h"
[!else]
#include "OptionsDialog.h"
[!endif]
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

GUID			g_PluginId = [!output CLSID_STRUCT_FORMAT];
CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"[!output SAFE_PROJECT_NAME]" );
CPeekString		COmniPlugin::s_strPublisher( L"Savvius, Inc." );
CVersion		COmniPlugin::s_Version( 1, 0, 0, 0 );
[!if NOTIFY_UIHANDLER]

#ifdef _WIN32
int				COmniPlugin::s_nAboutId( IDR_ZIP_ABOUT );
int				COmniPlugin::s_nCaptureTabId( IDR_ZIP_CAPTURE_TAB );
int				COmniPlugin::s_nEngineTabId( IDR_ZIP_ENGINE_TAB );
int				COmniPlugin::s_nOptionsId( IDR_ZIP_OPTIONS );
#endif // _WIN32

[!if NOTIFY_CEFPREFS]
GUID			g_idUIHandler = CefPreferencesHandler_ID;
[!endif]
[!if NOTIFY_HTMLPREFS]
GUID			g_idUIHandler = HTMLPreferencesHandler_ID;
[!endif]
[!endif]


// =============================================================================
//		COmniPlugin
// =============================================================================

COmniPlugin::COmniPlugin()
	:	CPeekPlugin()
	,	m_bInitialized( false )
[!if !NOTIFY_UIHANDLER]
#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	,	m_MessageQueue( this )
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
[!endif]
{
}
[!if NOTIFY_UIHANDLER]


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
				CPeekString	strConfig( AppConfig.GetConfigurationDirectory() );
				CPeekString	strLib( AppConfig.GetLibraryDirectory() );
				CPeekString	strResource( AppConfig.GetResourceDirectory() );
				CPeekString	strDoc( AppConfig.GetDocumentDirectory() );
				CPeekString	strLog( AppConfig.GetLogDirectory() );
				CPeekString	strData( AppConfig.GetDataDirectory() );
#endif
			
				CPeekString	strDirectory( AppConfig.GetLibraryDirectory() );
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
[!endif]
[!if NOTIFY_TAB && !NOTIFY_UIHANDLER]


#if defined(_WIN32) && defined(IMPLEMENT_TAB)
// -----------------------------------------------------------------------------
//		CreateTab
// -----------------------------------------------------------------------------

void
COmniPlugin::CreateTab()
{
	if ( GetInstalledOnEngine() == CPeekPlugin::kOnEngine_Unknown ) return;

	if ( CPeekProxy::IsConsole() && IsInstalledOnEngine() && !m_TabHost.HasChild() ) {
		AFX_MANAGE_STATE( AfxGetStaticModuleState() );

		CPeekString	strCmd( COmniPlugin::GetClassId().Format() );
		CPeekString	strClass( L"STATIC" );
		HWND		hWnd( nullptr );

		_ASSERTE( m_pRemotePlugin != nullptr );
		if ( m_pRemotePlugin == nullptr ) return;

		CConsoleUI	ConsoleUI( m_pRemotePlugin->GetConsole() );
		hWnd = ConsoleUI.AddTabWindow( strCmd, strClass, s_strName );
		if ( hWnd ) {
			// Subclass the static window Peek created.
#undef SubclassWindow							// The MSVC compiler picks up this macro
			m_TabHost.SubclassWindow( hWnd );	// instead of this function.

			 //Create a dialog to be embedded in the Parent window. The Parent window will delete it.
			CPluginTab*	pTab = new CPluginTab( this );
			_ASSERTE( pTab != nullptr );
			if ( pTab != nullptr ) {
				CRect	rcTabHost;
				m_TabHost.GetClientRect( &rcTabHost );

				pTab->Create( IDD_PLUGIN_TAB, &m_TabHost );
				pTab->MoveWindow( &rcTabHost );
				m_TabHost.SetChild( pTab );
			}
		}
	}
}
#endif // _WIN32 && IMPLEMENT_TAB
[!endif]
[!if !NOTIFY_UIHANDLER]


#ifdef _WIN32
// -----------------------------------------------------------------------------
//		GetInstanceHandle
// -----------------------------------------------------------------------------

HINSTANCE
COmniPlugin::GetInstanceHandle() const
{
	// Can not move this function to the header file.
	// CRemotePlugin is not defined there.
	if ( m_pRemotePlugin != nullptr ) {
		return m_pRemotePlugin->GetInstanceHandle();
	}
	return nullptr;
}
#endif // _WIN32
[!endif]


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
//		OnCaptureCreated
// -----------------------------------------------------------------------------

int
COmniPlugin::OnCaptureCreated(
	CPeekCapture	inCapture )
{
[!if NOTIFY_CODEEXAMPLES]
	CPeekXml	xml( inCapture.GetCaptureTemplate() );
	CPeekStream	stream;
	xml.Save( stream );
[!else]
	(void) inCapture;
[!endif]
	// Returning anything other than PEEK_PLUGIN_SUCCESS will cause the
	// capture to be deleted.
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnCaptureDeleted
// -----------------------------------------------------------------------------

int
COmniPlugin::OnCaptureDeleted(
	CPeekCapture	/*inCapture*/ )
{
	return PEEK_PLUGIN_SUCCESS;
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

[!if NOTIFY_UIHANDLER]
	BuildPluginPath();

	// The About, Options and Capture Tab web page/site are embedded as "ZIP" resources.
	// The following allows the plugin to look for alternate versions of the zip file.
	// The plugin will look for the files:
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\[!output SAFE_PROJECT_NAME]-About.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\[!output SAFE_PROJECT_NAME]-Options.zip
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\[!output SAFE_PROJECT_NAME]-Tab.zip
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

[!if NOTIFY_CEFPREFS]
		// The NoDefaultsPrefs flag instructs the Cef UI Handler to ignore the settings
		// of the previous capture. Remove this flag to initialize new capture settings
		// with the previous capture's settings.
		m_nOptionsFlags = peekPluginHandlerUI_Zipped | peekPluginHandlerUI_NoDefaultPrefs;
[!else]
		m_nOptionsFlags = peekPluginHandlerUI_Zipped;
[!endif]
		FilePath::Path	pathOptions( m_strPluginPath );
		pathOptions.SetFileName( s_strName + L"-Options.zip" );
		m_strOptionsFilePath = pathOptions.get();
	}
[!else]
#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
	if ( HasRemotePlugin() && m_pRemotePlugin->GetPeekProxy().IsConsole() ) {
		CMessagePing*	pPing( new CMessagePing() );
		FnResponseProcessor	fnProcess = &CPeekMessageProcess::ProcessPluginResponse;
		PostPluginMessage( CPeekMessagePtr( pPing ), this, fnProcess );
	}
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
[!endif]

	m_bInitialized = true;

	return PEEK_PLUGIN_SUCCESS;
}
[!if !NOTIFY_UIHANDLER]


#ifdef _WIN32
// -----------------------------------------------------------------------------
//		OnLoadView
// -----------------------------------------------------------------------------

int
COmniPlugin::OnLoadView()
{
	int	nResult( CPeekPlugin::OnLoadView() );
	if ( nResult != PEEK_PLUGIN_SUCCESS ) {
		return nResult;
	}

[!if NOTIFY_TAB]
#ifdef IMPLEMENT_TAB
	CreateTab();
#endif

[!endif]
	return PEEK_PLUGIN_SUCCESS;
}
#endif
[!endif]


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
COmniPlugin::OnUnload()
{
	m_bInitialized = false;

	return PEEK_PLUGIN_SUCCESS;
}


[!if NOTIFY_UIHANDLER]
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
	_ASSERTE( outFlags != nullptr );
	if ( outFlags == nullptr ) return PEEK_PLUGIN_FAILURE;

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
	_ASSERTE( outFlags != nullptr );
	if ( outFlags == nullptr ) return PEEK_PLUGIN_FAILURE;

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
	_ASSERTE( outFlags != nullptr );
	if ( outFlags == nullptr ) return PEEK_PLUGIN_FAILURE;

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
[!else]
// -----------------------------------------------------------------------------
//		OnAbout
//		: Implements IPeekPluginAbout::ShowAbout
// -----------------------------------------------------------------------------

int
COmniPlugin::OnAbout()
{
#ifdef PEEK_UI
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CAboutDialog	dlgAbout( GetInstanceHandle() );
	dlgAbout.DoModal();
#endif	// PEEK_UI

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnOptions
// -----------------------------------------------------------------------------

bool
COmniPlugin::OnOptions()
{
#ifdef PEEK_UI
	COptionsDialog theOptionsDlg( m_Options );
	if ( theOptionsDlg.DoModal() == IDOK ) {
		m_Options = theOptionsDlg.GetOptions();
		return true;
	}
#endif	// PEEK_UI

	return false;
}
[!endif]


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
			case CMessageGetAllOptions::s_nMessageType:
				nResult = ProcessGetAllOptions( spMessage.get() );
				break;
[!if NOTIFY_CODEEXAMPLES && NOTIFY_TAB && NOTIFY_UIHANDLER]

			case CMessageTab::s_nMessageType:
				nResult = ExampleProcessTabMessage( spMessage.get() );
				break;

[!endif]
[!if !NOTIFY_UIHANDLER]
			case CMessagePing::s_nMessageType:
				nResult = ProcessPing( spMessage.get() );
				break;
[!endif]
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

				COptions&	optContext = pContext->GetOptions();

				CMessageGetAllOptions::TNamedOptions	noContext = { idContext, strName, optContext };
				ayNamedOptions.Add( noContext );
			}
		}
	}

	return PEEK_PLUGIN_SUCCESS;
}
[!if NOTIFY_CODEEXAMPLES && NOTIFY_TAB && NOTIFY_UIHANDLER]


// -----------------------------------------------------------------------------
//		ExampleProcessTabMessage
// -----------------------------------------------------------------------------

int
COmniPlugin::ExampleProcessTabMessage(
	CPeekMessage* ioMessage )
{
	CMessageTab*	pMessage = dynamic_cast<CMessageTab*>( ioMessage );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// Process the message string.
	CPeekString	strMessage( pMessage->GetMessage() );

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// No response content.
	CPeekString	strResponse( strMessage );
	strResponse.Reverse();

	pMessage->SetResponse( strResponse );

	return PEEK_PLUGIN_SUCCESS;
}
[!endif]
[!if !NOTIFY_UIHANDLER]


// -----------------------------------------------------------------------------
//		ProcessPing
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessPing(
	CPeekMessage* ioMessage )
{
	CMessagePing*	pMessage( dynamic_cast<CMessagePing*>(ioMessage) );
	_ASSERTE( pMessage != nullptr );
	if ( pMessage == nullptr ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// No response content.

	return PEEK_PLUGIN_SUCCESS;
}
[!endif]
#endif // IMPLEMENT_PLUGINMESSAGE
[!if !NOTIFY_UIHANDLER]


// .............................................................................
//		Console-Only Methods
// .............................................................................

#if defined(_WIN32) && defined(IMPLEMENT_PLUGINMESSAGE)
// -----------------------------------------------------------------------------
//		PostPluginMessage
// -----------------------------------------------------------------------------

bool
COmniPlugin::PostPluginMessage(
	CPeekMessagePtr			inMessage,
	CPeekMessageProcess*	inMsgProc,
	FnResponseProcessor		inResponseProc )
{
	return m_MessageQueue.PostPluginMessage( inMessage, inMsgProc, inResponseProc );
}


// -----------------------------------------------------------------------------
//		OnProcessPluginResponse
// -----------------------------------------------------------------------------

int
COmniPlugin::OnProcessPluginResponse(
	CPeekStream&	inMessage,
	HeResult		inMsgResult,
	UInt32			inTransId )
{
	int	nResult = PEEK_PLUGIN_FAILURE;

	CPeekMessageItem&	miMessage( m_MessageQueue.GetAt( inTransId ) );
	if ( miMessage.IsValid() ) {
		miMessage.CallResponseProcessor( inMsgResult, inMessage );
		nResult = PEEK_PLUGIN_SUCCESS;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessPluginResponse
// -----------------------------------------------------------------------------

void
COmniPlugin::ProcessPluginResponse(
	HeResult			inResult,
	CPeekMessagePtr		inMessage,
	const CPeekStream&	/*inResponse*/ )
{
	switch ( inMessage->GetType() ) {
		case CMessagePing::s_nMessageType:
			SetInstalledOnEngine( (HE_SUCCEEDED( inResult ))
				? kOnEngine_True
				: kOnEngine_False );
[!if NOTIFY_TAB]
#ifdef IMPLEMENT_TAB
			CreateTab();
#endif
[!endif]
			break;
	}
}
#endif // _WIN32 && IMPLEMENT_PLUGINMESSAGE
[!endif]
