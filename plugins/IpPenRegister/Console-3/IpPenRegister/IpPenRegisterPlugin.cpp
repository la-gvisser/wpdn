// ============================================================================
//	IpPenRegisterPlugin.cpp
//		implementation of the CIpPenRegisterPlugin class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "IpPenRegisterPlugin.h"
#include "IpPenRegisterContext.h"
#include "AboutDlg.h"
#include "OptionsDlg.h"
#include "Utils.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Static class members.
UInt32		CIpPenRegisterPlugin::s_SupportedProtoSpecs[] =
{
	ProtoSpecDefs::IP,
	ProtoSpecDefs::IPv6
};

UInt32  CIpPenRegisterPlugin::s_SupportedProtoSpecCount =
	COUNTOF( s_SupportedProtoSpecs );

PluginID	CIpPenRegisterPlugin::s_ID =
	{ 0x63DA5F79, 0xADAB, 0x47FF, { 0x90, 0x0D, 0x8B, 0x31, 0x49, 0x70, 0xA7, 0xC2 } };


// ============================================================================
//		CIpPenRegisterPlugin
// ============================================================================

// ----------------------------------------------------------------------------
//		GetName
// ----------------------------------------------------------------------------

void
CIpPenRegisterPlugin::GetName(
	TCHAR*	outName,
	int		nBufSize ) const
{
	// Get the plugin name.
	LoadString( IDS_NAME, outName, nBufSize );
}


// ----------------------------------------------------------------------------
//		GetSupportedProtoSpecs
// ----------------------------------------------------------------------------

void
CIpPenRegisterPlugin::GetSupportedProtoSpecs(
	UInt32*		outNumProtoSpecs,
	UInt32**	outProtoSpecs ) const
{
	*outNumProtoSpecs = s_SupportedProtoSpecCount;
	if ( s_SupportedProtoSpecCount > 0 ) {
		*outProtoSpecs = s_SupportedProtoSpecs;
	}
	else {
		*outProtoSpecs = NULL;
	}
}


// ============================================================================
//		Plugin Message Handlers
// ============================================================================

// ----------------------------------------------------------------------------
//		OnLoad
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnLoad(
	PluginLoadParam*	ioParams )
{
	int	nResult = CPeekPlugin::OnLoad( ioParams );
	if ( nResult != PLUGIN_RESULT_SUCCESS ) {
		return nResult;
	}

    if ( !(ioParams->fAppCapabilities & kAppCapability_RemoteAgent) ) {
        return PLUGIN_RESULT_DISABLED;
    }

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_Options.Init( this );

	ENV_VERSION	evFile;
	ENV_VERSION	evProduct;
	if ( GetPluginVersion( evFile, evProduct) ) {
		CString	strVersionTemplate;
		strVersionTemplate.LoadString( IDS_VERSION_TEMPLATE );
		if ( strVersionTemplate.IsEmpty() ) {
			strVersionTemplate = _T( "%d.%d.%d.%d" );
		}

		CString	strVersion;
		strVersion.Format(
			strVersionTemplate,
			evFile.MajorVer,
			evFile.MinorVer,
			evFile.MajorRev,
			evFile.MinorRev );

		m_strPluginName.Format( _T( "%s v%s" ), ioParams->fName, strVersion );
	}
	else {
		m_strPluginName = ioParams->fName;
	}

	// Todo: allocate resources.

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnUnload
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnUnload()
{
	// Todo: release resources.

	CPeekPlugin::OnUnload();
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnReadPrefs
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnReadPrefs()
{
	//m_Options.Read();
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnWritePrefs
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnWritePrefs()
{
	//m_Options.Write();
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnAbout
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnAbout()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CAboutDlg	AboutDialog;
	AboutDialog.DoModal();
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnOptions
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnOptions()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	COptionsDlg	OptionsDialog( IDS_OPTIONS_TITLE );
	OptionsDialog.m_Options = m_Options;

	if ( OptionsDialog.DoModal() == IDOK ) {
		m_Options.SetOptions( OptionsDialog.m_Options );
		BroadcastMessage( CBLOptionsUpdate( &m_Options ) );
	}

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnFilterOptions
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnFilterOptions()
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnCreateContext
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnCreateContext(
	PluginCreateContextParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		new CIpPenRegisterContext( m_pPeekApp, this );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	if ( ioParams->fContextFlags == kContextType_Global ) {
		m_pGlobalContext = pContext;
	}

	if ( pContext != NULL ) {
		pContext->Init( ioParams );
		m_aContextList.Add( pContext );
		AddListener( pContext );
		*(ioParams->fContextPtr) = pContext;
	}

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnDisposeContext
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnDisposeContext(
	PluginDisposeContextParam*  ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	try {
		if ( pContext == m_pGlobalContext ) {
			m_pGlobalContext = NULL;
		}
		RemoveListener( pContext );
		pContext->Term();
		m_aContextList.Remove( pContext );
	}
	catch ( int e ) {
		if ( e == -1 ) {
			delete pContext;
		}
	}

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnNameTableUpdate
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnNameTableUpdate(
	PluginNameTableUpdateParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnCreateNewAdapter
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnCreateNewAdapter(
	PluginCreateNewAdapterParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetAdapterList
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnGetAdapterList(
	PluginGetAdapterListParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDeleteAdapter
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnDeleteAdapter(
	PluginDeleteAdapterParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSetAdapterAttribs
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnSetAdapterAttribs(
	PluginAdapterAttribsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetAdapterAttribs
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnGetAdapterAttribs(
	PluginAdapterAttribsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnAdapterProperties
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnAdapterProperties(
	PluginAdapterPropertiesParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnUserDecode
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnUserDecode(
	PluginUserDecodeParam*	/*ioParams*/ )
{
	// TODO: Add a C decoder here.
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnDecodersLoaded
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnDecodersLoaded()
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnIsMediaSupported
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnIsMediaSupported(
	PluginMediaSupportedParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ============================================================================
//		Context Specific Message Handlers
// ============================================================================

// ----------------------------------------------------------------------------
//		OnReset
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnReset(
	PluginResetParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnReset( ioParams );
}


// ----------------------------------------------------------------------------
//		OnStartCapture
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnStartCapture(
	PluginStartCaptureParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnStartCapture( ioParams );
}


// ----------------------------------------------------------------------------
//		OnStopCapture
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnStopCapture(
	PluginStopCaptureParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnStopCapture( ioParams );
}


// ----------------------------------------------------------------------------
//		OnProcessPacket
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnProcessPacket(
	PluginProcessPacketParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnProcessPacket( ioParams );
}


// ----------------------------------------------------------------------------
//		OnGetPacketString
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnGetPacketString(
	PluginGetPacketStringParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnGetPacketString( ioParams );
}


// ----------------------------------------------------------------------------
//		OnApply
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnApply(
	PluginApplyParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnApply( ioParams );
}


// ----------------------------------------------------------------------------
//		OnSelect
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnSelect(
	PluginSelectParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnSelect( ioParams );
}


// ----------------------------------------------------------------------------
//		OnPacketsLoaded
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnPacketsLoaded(
	PluginPacketsLoadedParam*	ioParams)
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnPacketsLoaded( ioParams );
}


// ----------------------------------------------------------------------------
//		OnSummary
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnSummary(
	PluginSummaryParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnSummary( ioParams );
}


// ----------------------------------------------------------------------------
//		OnFilter
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnFilter(
	PluginFilterParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnFilter( ioParams );
}


// ----------------------------------------------------------------------------
//		OnGetPacketAnalysis
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnGetPacketAnalysis(
	PluginGetPacketStringParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnGetPacketAnalysis( ioParams );
}


// ----------------------------------------------------------------------------
//		OnDecodePacket
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnDecodePacket(
	PluginDecodePacketParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnDecodePacket( ioParams );
}


// ----------------------------------------------------------------------------
//		OnProcessPluginMessage
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnProcessPluginMessage(
	PluginProcessPluginMessageParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnProcessPluginMessage( ioParams );
}


// ----------------------------------------------------------------------------
//		OnHandleNotify
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnHandleNotify(
	PluginHandleNotifyParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnHandleNotify( ioParams );
}


// ----------------------------------------------------------------------------
//		OnGetTextForPacketListCell
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnGetTextForPacketListCell(
	PluginGetTextForPacketListCellParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnGetTextForPacketListCell( ioParams );
}


// ----------------------------------------------------------------------------
//		OnMeasurePacketListCell
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnMeasurePacketListCell(
	PluginMeasurePacketListCellParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnMeasurePacketListCell( ioParams );
}


// ----------------------------------------------------------------------------
//		OnDrawPacketListCell
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnDrawPacketListCell(
	PluginDrawPacketListCellParam*	ioParams )
{
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnDrawPacketListCell( ioParams );
}


// ----------------------------------------------------------------------------
//		OnContextOptions
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnContextOptions(
	PluginContextOptionsParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnSetContextPrefs
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnSetContextPrefs(
	PluginContextPrefsParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetContextPrefs
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnGetContextPrefs(
	PluginContextPrefsParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessTime
// ----------------------------------------------------------------------------

int
CIpPenRegisterPlugin::OnProcessTime(
	PluginProcessTimeParam*	ioParams )
{
	// The flag kPluginAttr_HandlesProcessTime must be set in
	// kAttributesEx in IpPenRegisterPlugin.h
	// to receive this message.
	CIpPenRegisterContext* pContext =
		static_cast<CIpPenRegisterContext*>( ioParams->fContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnProcessTime( ioParams );
}
