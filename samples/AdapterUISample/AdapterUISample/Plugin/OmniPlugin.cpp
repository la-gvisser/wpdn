// =============================================================================
//	OmniPlugin.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "Peek.h"
#include "GlobalId.h"
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
#include <algorithm>


// =============================================================================
//		Global Data
// =============================================================================

extern CContextManagerPtr	GetContextManager();

// #pragma message("Create a new GUID and update the .he file.")
// {F5CC15F5-BFBF-4DA2-8405-4612D975C559}
GUID			g_PluginId = { 0xF5CC15F5, 0xBFBF, 0x4DA2, { 0x84, 0x05, 0x46, 0x12, 0xD9, 0x75, 0xC5, 0x59 } };

// {BC399CB2-CC7A-4BBB-9C21-9B3917DC89E6}
GUID			g_AdapterId = { 0xBC399CB2, 0xCC7A, 0x4BBB, { 0x9C, 0x21, 0x9B, 0x39, 0x17, 0xDC, 0x89, 0xE6 } };

CGlobalId		COmniPlugin::s_idClass( g_PluginId );
CPeekString		COmniPlugin::s_strName( L"AdapterUISample" );
CPeekString		COmniPlugin::s_strPublisher( L"Savvius, Inc." );
CVersion		COmniPlugin::s_Version( 1, 0, 0, 0 );

#ifdef _WIN32
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

COmniPlugin::~COmniPlugin()
{
	DeleteAdapter( g_PluginId );
}


// -----------------------------------------------------------------------------
//		AddAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::AddAdapter(
	CAdapter	inAdapter )
{
	if ( !HasRemotePlugin() ) return PEEK_PLUGIN_FAILURE;

	CAdapterInfo	aiAdapterInfo( inAdapter.GetAdapterInfo() );
	UInt64			nLinkSpeed = aiAdapterInfo.GetLinkSpeed();

	CAdapterConfig acConfig;
	acConfig.Create();
	acConfig.SetDefaultLinkSpeed( nLinkSpeed );
	acConfig.SetLinkSpeed( nLinkSpeed );
	acConfig.SetRingBufferSize( 0 );

	CAdapterManager	manager( m_pRemotePlugin->GetPeekProxy().GetAdapterManager() );
	manager.AddAdapter( inAdapter, aiAdapterInfo, acConfig );

	return PEEK_PLUGIN_SUCCESS;
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
//		CreateAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::CreateAdapter(
	const CPeekString&	inName )
{
	CRemotePlugin*	pRemotePlugin( GetRemotePlugin() );
	CGlobalId		idAdapter( g_AdapterId );
	CAdapter		adapter( pRemotePlugin, idAdapter, inName );

	AddAdapter( adapter );
	m_AdapterList.push_back( adapter );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		DeleteAdapter
// -----------------------------------------------------------------------------

int
COmniPlugin::DeleteAdapter(
	const CGlobalId&	inId )
{
	if ( !HasRemotePlugin() ) return PEEK_PLUGIN_FAILURE;

	try {
		auto adapter = find_if( 
			m_AdapterList.begin(),
			m_AdapterList.end(),
			[inId] (CAdapter& a) -> bool {
				CAdapterInfo	info( a.GetAdapterInfo() );
				return (CGlobalId( info.GetId() ) == inId);
			} );
		if ( adapter == m_AdapterList.end() ) {
			return PEEK_PLUGIN_SUCCESS;
		}

		CAdapterManager	manager( m_pRemotePlugin->GetPeekProxy().GetAdapterManager() );
		if ( manager.RemoveAdapter( *adapter ) ) {
			m_AdapterList.erase( adapter );
		}
	}
	catch( ... ) {
		return PEEK_PLUGIN_FAILURE;
	}

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		GetInstanceHandle
// -----------------------------------------------------------------------------

HINSTANCE
COmniPlugin::GetInstanceHandle() const
{
	// Can not move this function to the header file.
	// CRemotePlugin is not defined there.
#ifdef _WIN32
	CRemotePlugin*	pRemotePlugin( GetRemotePlugin() );
	if ( pRemotePlugin != NULL ) {
		return pRemotePlugin->GetInstanceHandle();
	}
#endif // _WIN32
	return NULL;
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
	//   %ALLUSERSPROFILE\Application Data\Savvius\Plugins\AdapterUISample-Options.zip
	// and will use them instead of the ZIP resource.
	// This is useful for the development of the web pages/sites.

	if ( !m_strPluginPath.empty() ) {
		m_nOptionsFlags = peekPluginHandlerUI_Zipped;
		FilePath::Path	pathOptions( m_strPluginPath );
		pathOptions.SetFileName( s_strName + L"-Options.zip" );
		m_strOptionsFilePath = pathOptions.get();
	}

	// Create persistent adapter(s)
	if (m_AdapterList.empty()) {
		// Delete the adapter in Destructor.
		CreateAdapter( L"My_Adapter" );
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

	(void) outData;

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

	(void) outData;

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

	(void) outData;

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
			case CMessageCreate::s_nMessageType:
				nResult = ProcessCreate( spMessage.get() );
				break;
			case CMessageList::s_nMessageType:
				nResult = ProcessList( spMessage.get() );
				break;
		}

		if ( nResult == PEEK_PLUGIN_SUCCESS ) {
			spMessage->StoreResponse( outResponse );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		ProcessCreate
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessCreate(
	CPeekMessage* ioMessage )
{
	CMessageCreate*	pMessage = dynamic_cast<CMessageCreate*>( ioMessage );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// Process the message: Get the name for the new adapter.
	CPeekString	strName( pMessage->GetAdapterName() );

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	/* Single Instance of Adapter Only - Already Created in OnLoad

	// Response is creation result as a string: Ok, or Failed.
	int	nResult( CreateAdapter( strName ) );
	CPeekString	strResult( (nResult == PEEK_PLUGIN_SUCCESS) ? L"Ok" : L"Failed" );
	pMessage->SetResponse( strResult );

	*/

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ProcessList
// -----------------------------------------------------------------------------

int
COmniPlugin::ProcessList(
	CPeekMessage* ioMessage )
{
	CMessageList*	pMessage = dynamic_cast<CMessageList*>( ioMessage );
	_ASSERTE( pMessage != NULL );
	if ( pMessage == NULL ) return PEEK_PLUGIN_FAILURE;

	// No input to process.

	// Create the response modeler.
	if ( !pMessage->StartResponse() ) return PEEK_PLUGIN_FAILURE;

	// Build the Adapter list.
	CMessageList::CIdNameList&	ayAdapterList( pMessage->GetAdapterList() );
	
	for ( auto i = m_AdapterList.begin(); i != m_AdapterList.end(); ++i ) {
		CAdapterInfo	info( i->GetAdapterInfo() );
		CMessageList::TIdName	adapter = { CGlobalId( info.GetId() ), info.GetDescription() };
		ayAdapterList.Add( adapter );
	}

	return PEEK_PLUGIN_SUCCESS;
}
#endif // IMPLEMENT_PLUGINMESSAGE
