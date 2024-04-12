// ============================================================================
//	PeekPlugin.cpp
//		implementation of the CPeekPlugin class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PeekPlugin.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// =============================================================================
//		CPeekPlugin
// =============================================================================

CPeekPlugin::CPeekPlugin()
	:	m_APIVersion( kPluginAPIVersion )
	,	m_bIsPeekUnicode( true )
	,	m_pPeekApp( NULL )
	,	m_hInstance( NULL )
	,	m_hResInstance( NULL )
	,	m_szResourceFilename( NULL )
	,	m_bResourceDllLoaded( false )
	,	m_nCodePage( 1033 )
	,	m_bFailure( false )
{
	m_pPeekApp = new CPeekApp();
}

CPeekPlugin::~CPeekPlugin()
{
	if ( m_bResourceDllLoaded ) {
		::FreeLibrary( m_hResInstance );
	}
	delete m_pPeekApp;
}


// ----------------------------------------------------------------------------
//		HandleMessage
// ----------------------------------------------------------------------------

int
CPeekPlugin::HandleMessage(
	SInt16				inMessage,
	PluginParamBlock*	ioParams )
{
	try {
		switch ( inMessage ) {
			case kPluginMsg_Load:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				m_bIsPeekUnicode =
					((reinterpret_cast<PluginLoadParam*>( ioParams ))->fAppCapabilities & kAppCapability_Unicode);
				return OnLoad( reinterpret_cast<PluginLoadParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_Unload:
			{
				return OnUnload();
			}
			break;

			case kPluginMsg_ReadPrefs:
			{
				return OnReadPrefs();
			}
			break;

			case kPluginMsg_WritePrefs:
			{
				return OnWritePrefs();
			}
			break;

			case kPluginMsg_CreateContext:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnCreateContext( reinterpret_cast<PluginCreateContextParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_DisposeContext:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnDisposeContext( reinterpret_cast<PluginDisposeContextParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_ProcessPacket:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnProcessPacket( reinterpret_cast<PluginProcessPacketParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_GetPacketString:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnGetPacketString( reinterpret_cast<PluginGetPacketStringParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_Apply:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnApply( (PluginApplyParam*) ioParams );
			}
			break;

			case kPluginMsg_Select:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnSelect( (PluginSelectParam*) ioParams );
			}
			break;

			case kPluginMsg_Reset:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnReset( (PluginResetParam*) ioParams );
			}
			break;

			case kPluginMsg_StartCapture:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnStartCapture( reinterpret_cast<PluginStartCaptureParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_StopCapture:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnStopCapture( reinterpret_cast<PluginStopCaptureParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_PacketsLoaded:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnPacketsLoaded( reinterpret_cast<PluginPacketsLoadedParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_About:
			{
				return OnAbout();
			}
			break;

			case kPluginMsg_Options:
			{
				return OnOptions();
			}
			break;

			case kPluginMsg_Summary:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnSummary( reinterpret_cast<PluginSummaryParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_Filter:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnFilter( reinterpret_cast<PluginFilterParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_FilterOptions:
			{
				return OnFilterOptions();
			}
			break;

			//	case kPluginMsg_SummaryDescr:	// <-- Deprecated
			//	{
			//		return OnSummaryDescr();
			//	}
			//	break;

			case kPluginMsg_NameTableUpdate:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnNameTableUpdate( reinterpret_cast<PluginNameTableUpdateParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_GetPacketAnalysis:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnGetPacketAnalysis( reinterpret_cast<PluginGetPacketStringParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_CreateNewAdapter:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnCreateNewAdapter( reinterpret_cast<PluginCreateNewAdapterParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_GetAdapterList:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnGetAdapterList( reinterpret_cast<PluginGetAdapterListParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_DeleteAdapter:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnDeleteAdapter( reinterpret_cast<PluginDeleteAdapterParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_SetAdapterAttribs:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnSetAdapterAttribs( reinterpret_cast<PluginAdapterAttribsParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_GetAdapterAttribs:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnGetAdapterAttribs( reinterpret_cast<PluginAdapterAttribsParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_DecodePacket:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnDecodePacket( reinterpret_cast<PluginDecodePacketParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_AdapterProperties:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnAdapterProperties( reinterpret_cast<PluginAdapterPropertiesParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_UserDecode:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnUserDecode( reinterpret_cast<PluginUserDecodeParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_DecodersLoaded:
			{
				return OnDecodersLoaded();
			}
			break;

			case kPluginMsg_IsMediaSupported:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnIsMediaSupported( reinterpret_cast<PluginMediaSupportedParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_ProcessPluginMessage:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnProcessPluginMessage( reinterpret_cast<PluginProcessPluginMessageParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_HandleNotify:
			{
				return OnHandleNotify( reinterpret_cast<PluginHandleNotifyParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_GetTextForPacketListCell:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnGetTextForPacketListCell( reinterpret_cast<PluginGetTextForPacketListCellParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_MeasurePacketListCell:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnMeasurePacketListCell( reinterpret_cast<PluginMeasurePacketListCellParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_DrawPacketListCell:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnDrawPacketListCell( reinterpret_cast<PluginDrawPacketListCellParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_ContextOptions:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnContextOptions( reinterpret_cast<PluginContextOptionsParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_SetContextPrefs:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnSetContextPrefs( reinterpret_cast<PluginContextPrefsParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_GetContextPrefs:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnGetContextPrefs( reinterpret_cast<PluginContextPrefsParam*>( ioParams ) );
			}
			break;

			case kPluginMsg_ProcessTime:
			{
				PLUGIN_ASSERT( ioParams != NULL );
				return OnProcessTime( reinterpret_cast<PluginProcessTimeParam*>( ioParams ) );
			}
			break;

			default:
			{
				PLUGIN_DEBUGSTR( _T( "Unknown message received in CPlugin::HandleMessage!" ) );
			}
			break;
		}
	}
	catch ( ... ) {
		m_bFailure = true;
	}

	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		DllMain
// ----------------------------------------------------------------------------

BOOL
CPeekPlugin::DllMain(
	HINSTANCE	inInstance,
	DWORD		inReason,
	LPVOID		/* inReserved */ )
{
	if ( inReason == DLL_PROCESS_ATTACH ) {
		m_hInstance = inInstance;
		m_hResInstance = inInstance;
	}

	return TRUE;
}


// ----------------------------------------------------------------------------
//		ListenToMessage
// ----------------------------------------------------------------------------

void
CPeekPlugin::ListenToMessage(
	const CBLMessage&	ioMessage )
{
	switch ( ioMessage.GetId() ) {
	case -1:
		break;

	default:
		CListener::ListenToMessage( ioMessage );
		break;
	}
}


// ----------------------------------------------------------------------------
//		NotifyAll
// ----------------------------------------------------------------------------

void 
CPeekPlugin::NotifyAll(
	CString	inMessage )
{
	BroadcastMessage( CBLNotify( inMessage ) );
}


// ----------------------------------------------------------------------------
//		GetSupportedProtoSpecs
// ----------------------------------------------------------------------------

void
CPeekPlugin::GetSupportedProtoSpecs(
	UInt32*		outNumProtoSpecs,
	UInt32**	outProtoSpecs ) const
{
	*outNumProtoSpecs = 0;
	*outProtoSpecs = NULL;
}


// ============================================================================
//		Plugin Message Handlers
// ============================================================================

// ----------------------------------------------------------------------------
//		OnLoad
// ----------------------------------------------------------------------------
int
CPeekPlugin::OnLoad(
	PluginLoadParam*	ioParams )
{
	// Return the name of the plugin.
	ioParams->fName[0] = 0;
	GetName( ioParams->fName, kPluginNameMaxLength );

	// Return the id of the plugin.
	GetID( &ioParams->fID );

	// Check the API version.
	if ( ioParams->fAPIVersion == kPluginAPIVersion ) {
		m_APIVersion = kPluginAPIVersion;
	}
	else {
		return PLUGIN_RESULT_ERROR;
	}

	// Return the API version used by the plugin.
	ioParams->fAPIVersion = m_APIVersion;

	// Resolve the callbacks to the Peek Application.
	m_pPeekApp->Init( ioParams->fAppContextData, ioParams->fClientAppData );

	// Return the plugin attributes.
	ioParams->fAttributes = GetAttributes();
	if ( m_APIVersion >= kPluginAPIVersion12 ) {
		ioParams->fAttributesEx = GetAttributesEx();
	}

	// Return the supported actions.
	ioParams->fSupportedActions = GetSupportedActions();

	// Return the default actions.
	ioParams->fDefaultActions = GetDefaultActions();

	// Return the supported ProtoSpecs.
	UInt32	nNumProtoSpecs = 0;
	UInt32*	pProtoSpecs = nullptr;
	GetSupportedProtoSpecs( &nNumProtoSpecs, &pProtoSpecs );

	ioParams->fSupportedCount = nNumProtoSpecs;
	ioParams->fSupportedProtoSpecs = pProtoSpecs;

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnUnload
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnUnload()
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnReadPrefs
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnReadPrefs()
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnWritePrefs
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnWritePrefs()
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnAbout
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnAbout()
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnOptions
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnOptions()
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnFilterOptions
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnFilterOptions()
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnCreateContext
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnCreateContext(
	PluginCreateContextParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDisposeContext
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnDisposeContext(
	PluginDisposeContextParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSummaryDescr		<-- Deprecated
// ----------------------------------------------------------------------------

//	int
//	CPeekPlugin::OnSummaryDescr()
//	{
//		return PLUGIN_RESULT_ERROR;
//	}


// ----------------------------------------------------------------------------
//		OnNameTableUpdate
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnNameTableUpdate(
	PluginNameTableUpdateParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnCreateNewAdapter
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnCreateNewAdapter(
	PluginCreateNewAdapterParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetAdapterList
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnGetAdapterList(
	PluginGetAdapterListParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDeleteAdapter
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnDeleteAdapter(
	PluginDeleteAdapterParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSetAdapterAttribs
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnSetAdapterAttribs(
	PluginAdapterAttribsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetAdapterAttribs
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnGetAdapterAttribs(
	PluginAdapterAttribsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnAdapterProperties
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnAdapterProperties(
	PluginAdapterPropertiesParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnUserDecode
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnUserDecode(
	PluginUserDecodeParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDecodersLoaded
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnDecodersLoaded()
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnIsMediaSupported
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnIsMediaSupported(
	PluginMediaSupportedParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// =============================================================================
//		Context Specific Message Handlers
// =============================================================================

// ----------------------------------------------------------------------------
//		OnReset
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnReset(
	PluginResetParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnStartCapture
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnStartCapture(
	PluginStartCaptureParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnStopCapture
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnStopCapture(
	PluginStopCaptureParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessPacket
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnProcessPacket(
	PluginProcessPacketParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetPacketString
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnGetPacketString(
	PluginGetPacketStringParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnApply
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnApply(
	PluginApplyParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSelect
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnSelect(
	PluginSelectParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnPacketsLoaded
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnPacketsLoaded(
	PluginPacketsLoadedParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnSummary
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnSummary(
	PluginSummaryParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnFilter
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnFilter(
	PluginFilterParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetPacketAnalysis
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnGetPacketAnalysis(
	PluginGetPacketStringParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDecodePacket
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnDecodePacket(
	PluginDecodePacketParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnProcessPluginMessage
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnProcessPluginMessage(
	PluginProcessPluginMessageParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnHandleNotify
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnHandleNotify(
	PluginHandleNotifyParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetTextForPacketListCell
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnGetTextForPacketListCell(
	PluginGetTextForPacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnMeasurePacketListCell
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnMeasurePacketListCell(
	PluginMeasurePacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDrawPacketListCell
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnDrawPacketListCell(
	PluginDrawPacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnContextOptions
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnContextOptions(
	PluginContextOptionsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSetContextPrefs
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnSetContextPrefs(
	PluginContextPrefsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetContextPrefs
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnGetContextPrefs(
	PluginContextPrefsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnProcessTime
// ----------------------------------------------------------------------------

int
CPeekPlugin::OnProcessTime(
	PluginProcessTimeParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// =============================================================================
//		Utility Routines
// =============================================================================

// ----------------------------------------------------------------------------
//		LoadString
// ----------------------------------------------------------------------------
// Load string from resource.

bool
CPeekPlugin::LoadString(
	UInt32	inID,
	TCHAR*	outString,
	int		nBufSize ) const
{
	// Sanity check.
	PLUGIN_ASSERT( outString != NULL );
	if ( outString == NULL ) return false;

	// Start with an empty string.
	outString[0] = 0;

	bool bRet = true;
	if ( m_hResInstance == NULL ) {
		// This should never be the case. This means the Plugin specified a
		//  Resource DLL but that DLL was never loaded.
		ASSERT(0);
		if ( ::LoadString( m_hInstance, inID, outString, nBufSize ) == 0 ) {
			bRet = false;
		}
	}
	else {
		if ( ::LoadString( m_hResInstance, inID, outString, nBufSize ) == 0 ) {
			bRet = false;
		}
		else {
			// Use proper null termination.
			outString[nBufSize-1] = 0;
		}
	}

	return bRet;
}


// ----------------------------------------------------------------------------
//		GetResourceHandle
// ----------------------------------------------------------------------------

HINSTANCE
CPeekPlugin::GetResourceHandle()
{
	if ( m_szResourceFilename == NULL ) {
		return m_hInstance;
	}

	if ( !m_bResourceDllLoaded ) {

		// Otherwise ....
		TCHAR	szAppResPath[MAX_PATH];
		int		nResult= CPeekApp::DoGetAppResourcePath( szAppResPath );
		if ( nResult == PLUGIN_RESULT_SUCCESS ) {
			// Try to open the Resource file
			CString theResoucePath;
			theResoucePath.Format( _T( "%s\\%s\\%s" ), szAppResPath,
				kPeekPluginResourceFolder, m_szResourceFilename );
			m_hResInstance = ::LoadLibrary( theResoucePath );
			ASSERT( m_hResInstance != NULL );
			if ( m_hResInstance != NULL ) {
				m_bResourceDllLoaded = true;
			}
		}
	}

	return m_hResInstance;
}


// ============================================================================
//  Callback Routines
// ============================================================================

// ----------------------------------------------------------------------------
//		DoGetProtocolName
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetProtocolName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	return CPeekApp::DoGetProtocolName( inProtocol, outString );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolLongName
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetProtocolLongName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	return CPeekApp::DoGetProtocolLongName( inProtocol, outString );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolHierName
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetProtocolHierName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	return CPeekApp::DoGetProtocolHierName( inProtocol, outString );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolParent
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetProtocolParent(
	UInt32	inProtocol,
	UInt32*	outProtocolParent )
{
	return CPeekApp::DoGetProtocolParent( inProtocol, outProtocolParent );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolColor
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetProtocolColor(
	UInt32		inProtocol,
	COLORREF*	outColor )
{
	return CPeekApp::DoGetProtocolColor( inProtocol, outColor );
}


// ----------------------------------------------------------------------------
//		DoIsDescendentOf
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoIsDescendentOf(
	UInt32			inSubProtocol,
	const UInt32*	inParentArray,
	SInt32			inParentCount,
	UInt32*			outMatchID )
{
	return CPeekApp::DoIsDescendentOf(
		inSubProtocol,
		inParentArray,
		inParentCount,
		outMatchID );
}


// ----------------------------------------------------------------------------
//		DoLookupName
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoLookupName(
	const UInt8*	inEntry,
	UInt16			inEntryType,
	TCHAR*			outName,
	UInt8*			outType )
{
	return CPeekApp::DoLookupName(
		inEntry,
		inEntryType,
		outName,
		outType );
}


// ----------------------------------------------------------------------------
//		DoLookupName
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoLookupName(
	PluginNameTableEntry*	ioEntry )
{
	return CPeekApp::DoLookupName( ioEntry );
}


// ----------------------------------------------------------------------------
//		DoLookupEntry
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoLookupEntry(
	const TCHAR*	inName,
	UInt16			inEntryType,
	UInt8*			outEntry )
{
	return CPeekApp::DoLookupEntry( inName, inEntryType, outEntry );
}


// ----------------------------------------------------------------------------
//		DoLookupEntry
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoLookupEntry(
	PluginNameTableEntry*	inEntry )
{
	return CPeekApp::DoLookupEntry( inEntry );
}


// ----------------------------------------------------------------------------
//		DoAddNameEntry
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoAddNameEntry(
	const TCHAR*	inName,
	const TCHAR*	inGroup,
	const UInt8*	inEntry,
	UInt16			inEntryType,
	UInt16			inOptions )
{
	return CPeekApp::DoAddNameEntry(
		inName,
		inGroup,
		inEntry,
		inEntryType,
		inOptions );
}


// ----------------------------------------------------------------------------
//		DoAddNameEntry
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoAddNameEntry(
	const PluginNameTableEntry*	inEntry,
	UInt16						inOptions )
{
	return CPeekApp::DoAddNameEntry( inEntry, inOptions );
}


// ----------------------------------------------------------------------------
//		DoSummaryGetEntry
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSummaryGetEntry(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inLabel,
	const TCHAR*			inGroup,
	UInt32*					outType,
	void**					outData,
	UInt8*					outSource )
{
	return CPeekApp::DoSummaryGetEntry(
		inCaptureContext,
		inLabel,
		inGroup,
		outType,
		outData,
		outSource );
}


// ----------------------------------------------------------------------------
//		DoSummaryModifyEntry
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSummaryModifyEntry(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inLabel,
	const TCHAR*			inGroup,
	UInt32					inType,
	void*					inData )
{
	return CPeekApp::DoSummaryModifyEntry(
		inCaptureContext,
		inLabel,
		inGroup,
		inType,
		inData );
}


// ----------------------------------------------------------------------------
//		DoPacketGetLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekPlugin::DoPacketGetLayer(
	UInt8				inLayerType,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt16*				ioBytesLeft )
{
	return CPeekApp::DoPacketGetLayer(
		inLayerType,
		inMediaType,
		inMediaSubType,
		inPacket,
		inPacketData,
		ioBytesLeft );
}


// ----------------------------------------------------------------------------
//		DoPacketGetAddress
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoPacketGetAddress(
	PluginCaptureContext	inCaptureContext,
	UInt8					inAddressSelector,
	UInt8					inMediaType,
	UInt8					inMediaSubType,
	const PluginPacket*		inPacket,
	const UInt8*			inPacketData,
	UInt64					inPacketNumber,
	UInt8*					outAddress,
	UInt16*					outAddressType )
{
	return CPeekApp::DoPacketGetAddress(
		inCaptureContext,
		inAddressSelector,
		inMediaType,
		inMediaSubType,
		inPacket,
		inPacketData,
		inPacketNumber,
		outAddress,
		outAddressType );
}


// ----------------------------------------------------------------------------
//		DoPrefsGetValue
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoPrefsGetValue(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inName,
	const void*				outData,
	UInt32*					ioLength )
{
	return CPeekApp::DoPrefsGetValue(
		inCaptureContext,
		inName,
		outData,
		ioLength );
}


// ----------------------------------------------------------------------------
//		DoPrefsSetValue
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoPrefsSetValue(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inName,
	const void*				inData,
	UInt32					inLength )
{
	return CPeekApp::DoPrefsSetValue(
		inCaptureContext,
		inName,
		inData,
		inLength );
}


// ----------------------------------------------------------------------------
//		DoInsertPacket
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoInsertPacket(
	PluginCaptureContext	inCaptureContext,
	const PluginPacket*		inPacket,
	const UInt8*			inPacketData,
	UInt8					inMediaType,
	UInt8					inMediaSubType,
	UInt32					inReserved )
{
	return CPeekApp::DoInsertPacket(
		inCaptureContext,
		inPacket,
		inPacketData,
		inMediaType,
		inMediaSubType,
		inReserved );
}


// ----------------------------------------------------------------------------
//		DoSelectPackets
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSelectPackets(
	PluginCaptureContext	inCaptureContext,
	UInt32					inNumPackets,
	UInt64*					inPacketNumbers,
	UInt32					inFlags )
{
	return CPeekApp::DoSelectPackets(
		inCaptureContext,
		inNumPackets,
		inPacketNumbers,
		inFlags );
}


// ----------------------------------------------------------------------------
//		DoNotify
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoNotify(
	PluginCaptureContext	inCaptureContext,
	UInt64					inTimeStamp,
	UInt8					inSeverity,
	const TCHAR*			inShortString,
	const TCHAR*			inLongString )
{
	return CPeekApp::DoNotify(
		inCaptureContext,
		inTimeStamp,
		inSeverity,
		inShortString,
		inLongString );
}


// ----------------------------------------------------------------------------
//		DoSendPacket
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSendPacket(
	const UInt8*	inPacketData,
	UInt16			inPacketLength )
{
	return CPeekApp::DoSendPacket( inPacketData, inPacketLength );
}


// ----------------------------------------------------------------------------
//		DoInvokeNameEditDialog
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoInvokeNameEditDialog(
	PluginNameTableEntry*	inEntry )
{
	return CPeekApp::DoInvokeNameEditDialog( inEntry );
}


// ----------------------------------------------------------------------------
//		DoResolveAddress
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoResolveAddress(
	UInt8*	inAddress,
	UInt16	inAddressType )
{
	return CPeekApp::DoResolveAddress( inAddress, inAddressType );
}


// ----------------------------------------------------------------------------
//		DoPacketGetDataLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekPlugin::DoPacketGetDataLayer(
	UInt32				inProtoSpec,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt16*				ioBytesLeft,
	UInt32*				outSourceProtoSpec )
{
	return CPeekApp::DoPacketGetDataLayer(
		inProtoSpec,
		inMediaType,
		inMediaSubType,
		inPacket,
		inPacketData,
		ioBytesLeft,
		outSourceProtoSpec );
}


// ----------------------------------------------------------------------------
//		DoPacketGetHeaderLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekPlugin::DoPacketGetHeaderLayer(
	UInt32				inProtoSpec,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt16*				ioBytesLeft,
	UInt32*				outSourceProtoSpec )
{
	return CPeekApp::DoPacketGetHeaderLayer(
		inProtoSpec,
		inMediaType,
		inMediaSubType,
		inPacket,
		inPacketData,
		ioBytesLeft,
		outSourceProtoSpec );
}


// ----------------------------------------------------------------------------
//		DoPrefsGetPrefsPath
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoPrefsGetPrefsPath(
	TCHAR*	outString )
{
	return CPeekApp::DoPrefsGetPrefsPath( outString );
}


// ----------------------------------------------------------------------------
//		DoSelectPackets
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSelectPacketsEx(
	PluginCaptureContext	inCaptureContext,
	const UInt8*			inSrcAddr,
	UInt16					inSrcAddrType,
	const UInt8*			inDestAddr,
	UInt16					inDestAddrType,
	UInt16					inSrcPort,
	UInt16					inDestPort,
	UInt16					inPortTypes,
	bool					inBothDirections,
	UInt32					inFlags )
{
	return CPeekApp::DoSelectPacketsEx(
		inCaptureContext,
		inSrcAddr,
		inSrcAddrType,
		inDestAddr,
		inDestAddrType,
		inSrcPort,
		inDestPort,
		inPortTypes,
		inBothDirections,
		inFlags );
}


// ----------------------------------------------------------------------------
//		DoGetPacketCount
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetPacketCount(
	PluginCaptureContext	inCaptureContext,
	UInt32*					outCount )
{
	return CPeekApp::DoGetPacketCount( inCaptureContext, outCount );
}


// ----------------------------------------------------------------------------
//		DoGetFirstPacketIndex
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetFirstPacketIndex(
	PluginCaptureContext	inCaptureContext,
	UInt32*					outIndex )
{
	return CPeekApp::DoGetFirstPacketIndex( inCaptureContext, outIndex );
}


// ----------------------------------------------------------------------------
//		DoGetPacket
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetPacket(
	PluginCaptureContext	inCaptureContext,
	UInt32					inIndex,
	UInt64*					outPacketNumber,
	const PluginPacket**	outPacket,
	const UInt8**			outPacketData )
{
	return CPeekApp::DoGetPacket(
		inCaptureContext,
		inIndex,
		outPacketNumber,
		outPacket,
		outPacketData );
}


// ----------------------------------------------------------------------------
//		DoClaimPacketString
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoClaimPacketString(
	PluginCaptureContext	inCaptureContext,
	UInt64					inPacketNumber,
	bool					inExpert )
{
	return CPeekApp::DoClaimPacketString(
		inCaptureContext,
		inPacketNumber,
		inExpert );
}


// ----------------------------------------------------------------------------
//		DoSetPacketListColumn
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSetPacketListColumn(
	PluginCaptureContext	inCaptureContext,
	const PluginID*			inColumnID,
	const TCHAR*			inColumnName,
	const TCHAR*			inHeaderText,
	bool					inDefaultVisibility,
	UInt16					inDefaultWidth,
	UInt8					inAlignment )
{
	return CPeekApp::DoSetPacketListColumn(
		inCaptureContext,
		inColumnID,
		inColumnName,
		inHeaderText,
		inDefaultVisibility,
		inDefaultWidth,
		inAlignment );
}


// ----------------------------------------------------------------------------
//		DoDecodeOp
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoDecodeOp(
	PluginContext	inDecodeContext,
	PluginContext	inDecodeState,
	int				inOp,
	UInt32			inValue,
	UInt32			inGlobal,
	UInt8			inStyleValue,
	UInt8			inStyleLabel,
	const TCHAR*	inString,
	const TCHAR*	inSummary )
{
	return CPeekApp::DoDecodeOp(
		inDecodeContext,
		inDecodeState,
		inOp,
		inValue,
		inGlobal,
		inStyleValue,
		inStyleLabel,
		inString,
		inSummary );
}


// ----------------------------------------------------------------------------
//		DoAddUserDecode
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoAddUserDecode(
	const TCHAR*	inFuncName )
{
	return CPeekApp::DoAddUserDecode( inFuncName );
}


// ----------------------------------------------------------------------------
//		DoAddTab
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoAddTab(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inTabName,
	const TCHAR*			inWindowClass,
	void**					outTabWnd )
{
	return CPeekApp::DoAddTab(
		inCaptureContext,
		inTabName,
		inWindowClass,
		outTabWnd );
}


// ----------------------------------------------------------------------------
//		DoAddTabWithGroup
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoAddTabWithGroup(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inTabName,
	const TCHAR*			inGroupName,
	const TCHAR*			inWindowClass,
	void**					outTabWnd )
{
	return CPeekApp::DoAddTabWithGroup(
		inCaptureContext,
		inTabName,
		inGroupName,
		inWindowClass,
		outTabWnd );
}


// ----------------------------------------------------------------------------
//		DoSendPluginMessage
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSendPluginMessage(
	PluginCaptureContext	inCaptureContext,
	UInt8*					inParamData,
	UInt32					inParamDataLen )
{
	return CPeekApp::DoSendPluginMessage(
		inCaptureContext,
		inParamData,
		inParamDataLen );
}


// ----------------------------------------------------------------------------
//		DoMakeFilter
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoMakeFilter(
	const UInt8*	inSrcAddr,
	UInt16			inSrcAddrType,
	const UInt8*	inDestAddr,
	UInt16			inDestAddrType,
	UInt16			inSrcPort,
	UInt16			inDestPort,
	UInt16			inPortTypes,
	bool			inBothDirections )
{
	return CPeekApp::DoMakeFilter(
		inSrcAddr,
		inSrcAddrType,
		inDestAddr,
		inDestAddrType,
		inSrcPort,
		inDestPort,
		inPortTypes,
		inBothDirections );
}


// ----------------------------------------------------------------------------
//		DoGetAppResourcePath
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoGetAppResourcePath(
	TCHAR* outAppResourcePath )
{
	return CPeekApp::DoGetAppResourcePath( outAppResourcePath );
}


// ----------------------------------------------------------------------------
//		DoSaveContext
// ----------------------------------------------------------------------------

int
CPeekPlugin::DoSaveContext(
	PluginCaptureContext	inCaptureContext )
{
	return CPeekApp::DoSaveContext( inCaptureContext );
}
