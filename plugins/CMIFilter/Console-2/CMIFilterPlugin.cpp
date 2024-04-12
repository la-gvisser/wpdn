// =============================================================================
//	CMIFilterPlugin.cpp
//		implementation of the CCMIFilterPlugin class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "CMIFilterPlugin.h"
#include "CMIFilterContext.h"
#include "CMIFilterAbout.h"
#include "OptionsDlg.h"
#include "ByteStream.h"
#include "XString.h"
#include "FileUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
		    

// Static class members.
UInt32		CCMIFilterPlugin::s_ParentArray[] =
{
	ProtoSpecDefs::IP
};

UInt32  CCMIFilterPlugin::s_ParentCount = COUNTOF( s_ParentArray );

// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CString	kWildPacketsOmniPeekPlugins( _T( "WildPackets\\OmniPeek\\Plugins" ) );
	const CString	kConfigFileName( _T( "CMIFilterPrefs.xml" ) );
	const CString	kChangeLogFileName( _T( "CMIFilterPrefsLog.txt" ) );
}


// =============================================================================
//		CCMIFilterPlugin
// =============================================================================

CCMIFilterPlugin::CCMIFilterPlugin()
{
	HRESULT hr = CLSIDFromString(
		L"{1E680486-DA6B-4145-84D3-A6EF7A116AC1}", reinterpret_cast<LPCLSID>( &s_ID ) );
	ASSERT( SUCCEEDED( hr ) );
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

void
CCMIFilterPlugin::GetName(
	TCHAR*	outName,
	int		nBufSize ) const
{
	// Get the plugin name.
	LoadString( IDS_NAME, outName, nBufSize );
}


// -----------------------------------------------------------------------------
//		GetSupportedProtoSpecs
// -----------------------------------------------------------------------------

void
CCMIFilterPlugin::GetSupportedProtoSpecs(
	UInt32*		outNumProtoSpecs,
	UInt32**	outProtoSpecs ) const
{
	*outNumProtoSpecs = 0; //s_ParentCount;
	*outProtoSpecs = NULL; //s_ParentArray;
}


// -----------------------------------------------------------------------------
//		OnLoad
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnLoad(
	PluginLoadParam*	ioParams )
{
	int	nResult = CPeekPlugin::OnLoad( ioParams );
	if ( nResult != PLUGIN_RESULT_SUCCESS ) {
		return nResult;
	}

    // Only load if Professional or Enterprise.
    if ( !(ioParams->fAppCapabilities & kAppCapability_RemoteAgent) ) {
        return PLUGIN_RESULT_DISABLED;
    }

	CString	strPathName;
	BOOL bResult( SHGetSpecialFolderPath(
		NULL, strPathName.GetBufferSetLength( MAX_PATH ), CSIDL_APPDATA, FALSE ) );
	strPathName.ReleaseBuffer();
	if ( !strPathName.IsEmpty() ) {
		CString	strPath;
		strPath.Format(
			_T( "%s\\%s" ),
			strPathName,
			Tags::kWildPacketsOmniPeekPlugins );
		FileUtil::MakePath( strPath );

		CString	strConfigPathName;
		strConfigPathName.Format(
			_T( "%s\\%s" ),
			strPath,
			Tags::kConfigFileName );
		m_strConfigPathName = strConfigPathName;

		CString	strChageLogPathName;
		strChageLogPathName.Format(
			_T( "%s\\%s" ),
			strPath,
			Tags::kChangeLogFileName );
		m_strChangeLogPathName = strChageLogPathName;
	}

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnUnload
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnUnload()
{
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReadPrefs
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnReadPrefs()
{
	COptionsPtr	pOptions( GetOptions() );
	ASSERT( pOptions );
	if ( pOptions == NULL ) return 0;

	X_RESULT hr = S_OK;
	try {
		// Read the file into a stream
		ULONG	nFileSize = static_cast<ULONG>( FileUtil::GetFileSize( m_strConfigPathName ) );
		if ( nFileSize > 0 ) {
			CFile	theFile( m_strConfigPathName, CFile::modeRead );
			CStringA	strXml;
			PSTR		pszXml( strXml.GetBufferSetLength( nFileSize ) );
			ULONG		nBytesRead( theFile.Read( pszXml, nFileSize ) );
			strXml.ReleaseBuffer();
			if ( nBytesRead == nFileSize ) {
				bool	bResult( pOptions->SetContextPrefs( strXml ) );
				if ( !bResult ) {
					return PLUGIN_RESULT_ERROR;
				}
			}
		}
		else {
			WritePrefs();
		}
	}
	X_CATCH( hr )

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnWritePrefs
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnWritePrefs()
{
	WritePrefs();
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnAbout
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnAbout()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCMIFilterAbout AboutDialog;
	AboutDialog.DoModal();
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnOptions
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnOptions()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	COptionsPtr	pOptions( GetOptions() );
	ASSERT( pOptions );
	if ( pOptions == NULL ) return 0;

	COptionsDialog theDialog;
	theDialog.SetOptions( *pOptions );
	if ( theDialog.DoModal() == IDOK ) {
		SetOptions( theDialog.GetOptions() );
		WritePrefs();
	}

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnCreateContext(
	PluginCreateContextParam*	ioParams )
{
	// Only create CMI context for capture windows
	if ( (ioParams->fContextFlags & kContextType_CaptureWindow) == 0) {
		*(ioParams->fContextPtr) = NULL;
		return PLUGIN_RESULT_SUCCESS;
	}

	CCMIFilterContext* pContext = new CCMIFilterContext( this );
	ASSERT( pContext );
	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	if ( pContext ) {
		pContext->Init( ioParams );
		*(ioParams->fContextPtr) = pContext;
	}

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnDisposeContext(
	PluginDisposeContextParam*  ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	pContext->Term();
	delete pContext;

	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnReset
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnReset(
	PluginResetParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnReset( ioParams );
}


// -----------------------------------------------------------------------------
//		OnStartCapture
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnStartCapture(
	PluginStartCaptureParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnStartCapture( ioParams );
}


// -----------------------------------------------------------------------------
//		OnStopCapture
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnStopCapture(
	PluginStopCaptureParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnStopCapture( ioParams );
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnProcessPacket(
	PluginProcessPacketParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnProcessPacket( ioParams );
}


// -----------------------------------------------------------------------------
//		OnGetPacketString
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnGetPacketString(
	PluginGetPacketStringParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnGetPacketString( ioParams );
}


// -----------------------------------------------------------------------------
//		OnApply
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnApply(
	PluginApplyParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnApply( ioParams );
}


// -----------------------------------------------------------------------------
//		OnSelect
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnSelect(
	PluginSelectParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnSelect( ioParams );
}


// -----------------------------------------------------------------------------
//		OnPacketsLoaded
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnPacketsLoaded(
	PluginPacketsLoadedParam*	ioParams)
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnPacketsLoaded( ioParams );
}


// -----------------------------------------------------------------------------
//		OnSummary
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnSummary(
	PluginSummaryParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnSummary( ioParams );
}


// -----------------------------------------------------------------------------
//		OnFilter
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnFilter(
	PluginFilterParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnFilter( ioParams );
}


// -----------------------------------------------------------------------------
//		OnGetPacketAnalysis
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnGetPacketAnalysis(
	PluginGetPacketStringParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnGetPacketAnalysis( ioParams );
}


// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnProcessPluginMessage(
	PluginProcessPluginMessageParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnProcessPluginMessage( ioParams );
}


// -----------------------------------------------------------------------------
//		OnContextOptions
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnContextOptions(
	PluginContextOptionsParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext ) {
		return pContext->OnContextOptions( ioParams );
	}

	COptionsDialog	theDialog( NULL );
	if ( ioParams->fInOldPrefs && (ioParams->fInOldPrefsLength != 0) ) {
		CStringA	strXml( ioParams->fInOldPrefs );
		theDialog.SetPrefs( strXml );
	}
	else {
		COptionsPtr	pOptions( GetOptions() );
		ASSERT( pOptions );
		if ( pOptions == NULL ) return 0;
		theDialog.SetOptions( *pOptions );
	}

	if ( theDialog.DoModal() == IDOK ) {
		m_strPrefs = theDialog.GetPrefs();
		if ( !m_strPrefs.IsEmpty() ) {
			ioParams->fOutNewPrefs = m_strPrefs.GetBuffer();
			ioParams->fOutNewPrefsLength = (m_strPrefs.GetLength() + 1) * sizeof( *(m_strPrefs.GetBuffer()) );
		}
	}
	return PLUGIN_RESULT_SUCCESS;
}


// -----------------------------------------------------------------------------
//		OnSetContextPrefs
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnSetContextPrefs(
	PluginContextPrefsParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnSetContextPrefs( ioParams );
}


// -----------------------------------------------------------------------------
//		OnGetContextPrefs
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnGetContextPrefs(
	PluginContextPrefsParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnGetContextPrefs( ioParams );
}


// -----------------------------------------------------------------------------
//		OnProcessTime
// -----------------------------------------------------------------------------

int
CCMIFilterPlugin::OnProcessTime(
	PluginProcessTimeParam*	ioParams )
{
	CCMIFilterContext* pContext = reinterpret_cast<CCMIFilterContext*>( ioParams->fContext );

	if ( pContext == NULL ) return PLUGIN_RESULT_ERROR;

	return pContext->OnProcessTime( ioParams );
}


// -----------------------------------------------------------------------------
//		WritePrefs
// -----------------------------------------------------------------------------

void
CCMIFilterPlugin::WritePrefs()
{
	X_RESULT hr = S_OK;

	COptionsPtr	pOptions( GetOptions() );
	ASSERT( pOptions );
	if ( pOptions == NULL ) return;

	try {
		// Save Options
		CStringA	strPrefs( pOptions->GetContextPrefs() );
		if ( !strPrefs.IsEmpty() ) {
			// Write the XML to a file
			CFile		theFile( m_strConfigPathName, (CFile::modeCreate | CFile::modeWrite) );
			if ( theFile.m_hFile != INVALID_HANDLE_VALUE ) {
				theFile.Write( strPrefs.GetBuffer(), strPrefs.GetLength() );
			}
		}
	}
	X_CATCH( hr )
}
