// ============================================================================
//	PeekContext.cpp
//		implementation of the CPeekContext class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "Memutil.h"
#include "PeekContext.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ============================================================================
//		CPeekContext
// ============================================================================

// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

int
CPeekContext::Init(
	PluginCreateContextParam*	ioParams )
{
	m_pCaptureContext = ioParams->fCaptureContext;
	m_ContextFlags = ioParams->fContextFlags;

	// Don't attach or create a CWnd for this HWND.
	// Call CWnd::FromHandle() or CWnd::FromHandlePermanent() later.
	m_hCaptureWindow = static_cast<HWND>( ioParams->fContextData );

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		Term
// ----------------------------------------------------------------------------

int
CPeekContext::Term()
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		GetCaptureTitle
// ----------------------------------------------------------------------------

CString
CPeekContext::GetCaptureTitle()
{
	if ( m_strCaptureTitle.IsEmpty() && ::IsWindow( m_hCaptureWindow ) ) {
		int	nLength = ::GetWindowTextLength( m_hCaptureWindow );
		if ( nLength > 0 ) {
			CString	strTitle;

			nLength++;		// add one for the NULL terminator.
			::GetWindowText( m_hCaptureWindow,
				strTitle.GetBufferSetLength( nLength ), nLength );
			strTitle.ReleaseBuffer();
			// Don't save Peek's title template: Capture %1
			if ( strTitle.CompareNoCase( _T( "Capture %1" ) ) != 0 ) {
				m_strCaptureTitle = strTitle;
			}
		}
	}

	return m_strCaptureTitle;
}


// ----------------------------------------------------------------------------
//		ListenToMessage
// ----------------------------------------------------------------------------

void
CPeekContext::ListenToMessage(
	CBLMessage&	ioMessage )
{
	CListener::ListenToMessage( ioMessage );
}


// ----------------------------------------------------------------------------
//		OnReset
// ----------------------------------------------------------------------------

int
CPeekContext::OnReset(
	PluginResetParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnStartCapture
// ----------------------------------------------------------------------------

int
CPeekContext::OnStartCapture(
	PluginStartCaptureParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnStopCapture
// ----------------------------------------------------------------------------

int
CPeekContext::OnStopCapture(
	PluginStopCaptureParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessPacket
// ----------------------------------------------------------------------------

int
CPeekContext::OnProcessPacket(
	PluginProcessPacketParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetPacketString
// ----------------------------------------------------------------------------

int
CPeekContext::OnGetPacketString(
	PluginGetPacketStringParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnApply
// ----------------------------------------------------------------------------

int
CPeekContext::OnApply(
	PluginApplyParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnSelect
// ----------------------------------------------------------------------------

int
CPeekContext::OnSelect(
	PluginSelectParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnPacketsLoaded
// ----------------------------------------------------------------------------

int
CPeekContext::OnPacketsLoaded(
	PluginPacketsLoadedParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnSummary
// ----------------------------------------------------------------------------

int
CPeekContext::OnSummary(
	PluginSummaryParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnFilter
// ----------------------------------------------------------------------------

int
CPeekContext::OnFilter(
	PluginFilterParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetPacketAnalysis
// ----------------------------------------------------------------------------

int
CPeekContext::OnGetPacketAnalysis(
	PluginGetPacketStringParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnDecodePacket
// ----------------------------------------------------------------------------

int
CPeekContext::OnDecodePacket(
	PluginDecodePacketParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessPluginMessage
// ----------------------------------------------------------------------------

int
CPeekContext::OnProcessPluginMessage(
	PluginProcessPluginMessageParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnHandleNotify
// ----------------------------------------------------------------------------

int
CPeekContext::OnHandleNotify(
	PluginHandleNotifyParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetTextForPacketListCell
// ----------------------------------------------------------------------------

int
CPeekContext::OnGetTextForPacketListCell(
	PluginGetTextForPacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnMeasurePacketListCell
// ----------------------------------------------------------------------------

int
CPeekContext::OnMeasurePacketListCell(
	PluginMeasurePacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDrawPacketListCell
// ----------------------------------------------------------------------------

int
CPeekContext::OnDrawPacketListCell(
	PluginDrawPacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnContextOptions
// ----------------------------------------------------------------------------

int
CPeekContext::OnContextOptions(
	PluginContextOptionsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSetContextPrefs
// ----------------------------------------------------------------------------

int
CPeekContext::OnSetContextPrefs(
	PluginContextPrefsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnGetContextPrefs
// ----------------------------------------------------------------------------

int
CPeekContext::OnGetContextPrefs(
	PluginContextPrefsParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnProcessTime
// ----------------------------------------------------------------------------

int
CPeekContext::OnProcessTime(
	PluginProcessTimeParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// =============================================================================
//		Utility Routines
// =============================================================================

// ----------------------------------------------------------------------------
//		DoGetProtocolName
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetProtocolName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	return CPeekApp::DoGetProtocolName( inProtocol, outString );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolLongName
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetProtocolLongName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	return CPeekApp::DoGetProtocolLongName( inProtocol, outString );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolHierName
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetProtocolHierName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	return CPeekApp::DoGetProtocolHierName( inProtocol, outString );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolParent
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetProtocolParent(
	UInt32	inProtocol,
	UInt32*	outProtocolParent )
{
	return CPeekApp::DoGetProtocolParent( inProtocol, outProtocolParent );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolColor
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetProtocolColor(
	UInt32		inProtocol,
	COLORREF*	outColor )
{
	return CPeekApp::DoGetProtocolColor( inProtocol, outColor );
}


// ----------------------------------------------------------------------------
//		DoIsDescendentOf
// ----------------------------------------------------------------------------

int
CPeekContext::DoIsDescendentOf(
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
CPeekContext::DoLookupName(
	const UInt8*	inEntry,
	SInt16			inEntryType,
	TCHAR*			outName,
	UInt8*			outType )
{
	return CPeekApp::DoLookupName( inEntry, inEntryType, outName, outType );
}


// ----------------------------------------------------------------------------
//		DoLookupName
// ----------------------------------------------------------------------------

int
CPeekContext::DoLookupName(
	PluginNameTableEntry*	ioEntry )
{
	return CPeekApp::DoLookupName( ioEntry );
}


// ----------------------------------------------------------------------------
//		DoLookupEntry
// ----------------------------------------------------------------------------

int
CPeekContext::DoLookupEntry(
	const TCHAR*	inName,
	SInt16			inEntryType,
	UInt8*			outEntry )
{
	return CPeekApp::DoLookupEntry( inName, inEntryType, outEntry );
}


// ----------------------------------------------------------------------------
//		DoLookupEntry
// ----------------------------------------------------------------------------

int
CPeekContext::DoLookupEntry(
	PluginNameTableEntry*	inEntry )
{
	return CPeekApp::DoLookupEntry( inEntry );
}


// ----------------------------------------------------------------------------
//		DoAddNameEntry
// ----------------------------------------------------------------------------

int
CPeekContext::DoAddNameEntry(
	const TCHAR*	inName,
	const TCHAR*	inGroup,
	const UInt8*	inEntry,
	SInt16			inEntryType,
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
CPeekContext::DoAddNameEntry(
	const PluginNameTableEntry*	inEntry,
	UInt16						inOptions )
{
	return CPeekApp::DoAddNameEntry( inEntry, inOptions );
}


// ----------------------------------------------------------------------------
//		DoSummaryGetEntry
// ----------------------------------------------------------------------------

int
CPeekContext::DoSummaryGetEntry(
	const TCHAR*	inLabel,
	const TCHAR*	inGroup,
	UInt32*			outType,
	void**			outData,
	UInt8*			outSource )
{
	return CPeekApp::DoSummaryGetEntry(
		m_pCaptureContext,
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
CPeekContext::DoSummaryModifyEntry(
	const TCHAR*	inLabel,
	const TCHAR*	inGroup,
	UInt32			inType,
	void*			inData )
{
	return CPeekApp::DoSummaryModifyEntry(
		m_pCaptureContext,
		inLabel,
		inGroup,
		inType,
		inData );
}


// ----------------------------------------------------------------------------
//		DoPacketGetLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekContext::DoPacketGetLayer(
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
CPeekContext::DoPacketGetAddress(
	UInt8				inAddressSelector,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt64				inPacketNumber,
	UInt8*				outAddress,
	UInt16*				outAddressType )
{
	return CPeekApp::DoPacketGetAddress(
		m_pCaptureContext,
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
CPeekContext::DoPrefsGetValue(
	const TCHAR*	inName,
	const void*		outData,
	UInt32*			ioLength )
{
	return CPeekApp::DoPrefsGetValue(
		m_pCaptureContext,
		inName,
		outData,
		ioLength );
}


// ----------------------------------------------------------------------------
//		DoPrefsSetValue
// ----------------------------------------------------------------------------

int
CPeekContext::DoPrefsSetValue(
	const TCHAR*	inName,
	const void*		inData,
	UInt32			inLength )
{
	return CPeekApp::DoPrefsSetValue(
		m_pCaptureContext,
		inName,
		inData,
		inLength );
}


// ----------------------------------------------------------------------------
//		DoInsertPacket
// ----------------------------------------------------------------------------

int
CPeekContext::DoInsertPacket(
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	UInt32				inReserved )
{
	return CPeekApp::DoInsertPacket(
		m_pCaptureContext,
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
CPeekContext::DoSelectPackets(
	UInt32	inNumPackets,
	UInt64*	inPacketNumbers,
	UInt32	inFlags )
{
	return CPeekApp::DoSelectPackets(
		m_pCaptureContext,
		inNumPackets,
		inPacketNumbers,
		inFlags );
}


// ----------------------------------------------------------------------------
//		DoNotify
// ----------------------------------------------------------------------------

int
CPeekContext::DoNotify(
	UInt64			inTimeStamp,
	UInt8			inSeverity,
	const TCHAR*	inShortString,
	const TCHAR*	inLongString )
{
	return CPeekApp::DoNotify(
		m_pCaptureContext,
		inTimeStamp,
		inSeverity,
		inShortString,
		inLongString );
}


// ----------------------------------------------------------------------------
//		DoSendPacket
// ----------------------------------------------------------------------------

int
CPeekContext::DoSendPacket(
	const UInt8*	inPacketData,
	UInt16			inPacketLength )
{
	return CPeekApp::DoSendPacket( inPacketData, inPacketLength );
}


// ----------------------------------------------------------------------------
//		DoInvokeNameEditDialog
// ----------------------------------------------------------------------------

int
CPeekContext::DoInvokeNameEditDialog(
	PluginNameTableEntry*	inEntry )
{
	return CPeekApp::DoInvokeNameEditDialog( inEntry );
}


// ----------------------------------------------------------------------------
//		DoResolveAddress
// ----------------------------------------------------------------------------

int
CPeekContext::DoResolveAddress(
	UInt8*	inAddress,
	UInt16	inAddressType )
{
	return CPeekApp::DoResolveAddress( inAddress, inAddressType );
}


// ----------------------------------------------------------------------------
//		DoPacketGetDataLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekContext::DoPacketGetDataLayer(
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
CPeekContext::DoPacketGetHeaderLayer(
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
CPeekContext::DoPrefsGetPrefsPath(
	TCHAR*	outString )
{
	return CPeekApp::DoPrefsGetPrefsPath( outString );
}


// ----------------------------------------------------------------------------
//		DoSelectPacketsEx
// ----------------------------------------------------------------------------

int
CPeekContext::DoSelectPacketsEx(
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
		m_pCaptureContext,
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
CPeekContext::DoGetPacketCount(
	UInt32*	outCount )
{
	return CPeekApp::DoGetPacketCount( m_pCaptureContext, outCount );
}


// ----------------------------------------------------------------------------
//		DoGetFirstPacketIndex
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetFirstPacketIndex(
	UInt32*	outIndex )
{
	return CPeekApp::DoGetFirstPacketIndex( m_pCaptureContext, outIndex );
}


// ----------------------------------------------------------------------------
//		DoGetPacket
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetPacket(
	UInt32					inIndex,
	UInt64*					outPacketNumber,
	const PluginPacket**	outPacket,
	const UInt8**			outPacketData )
{
	return CPeekApp::DoGetPacket(
		m_pCaptureContext,
		inIndex,
		outPacketNumber,
		outPacket,
		outPacketData );
}


// ----------------------------------------------------------------------------
//		DoClaimPacketString
// ----------------------------------------------------------------------------

int
CPeekContext::DoClaimPacketString(
	UInt64	inPacketNumber,
	bool	inExpert )
{
	return CPeekApp::DoClaimPacketString(
		m_pCaptureContext,
		inPacketNumber,
		inExpert );
}


// ----------------------------------------------------------------------------
//		DoSetPacketListColumn
// ----------------------------------------------------------------------------

int
CPeekContext::DoSetPacketListColumn(
	const PluginID*	inColumnID,
	const TCHAR*	inColumnName,
	const TCHAR*	inHeaderText,
	bool			inDefaultVisibility,
	UInt16			inDefaultWidth,
	UInt8			inAlignment )
{
	return CPeekApp::DoSetPacketListColumn(
		m_pCaptureContext,
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
CPeekContext::DoDecodeOp(
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
CPeekContext::DoAddUserDecode(
	const TCHAR*	inFuncName )
{
	return CPeekApp::DoAddUserDecode( inFuncName );
}


// ----------------------------------------------------------------------------
//		DoAddTab
// ----------------------------------------------------------------------------

int
CPeekContext::DoAddTab(
	const TCHAR*	inTabName,
	const TCHAR*	inWindowClass,
	void**			outTabWnd )
{
	return CPeekApp::DoAddTab(
		m_pCaptureContext,
		inTabName,
		inWindowClass,
		outTabWnd );
}


// ----------------------------------------------------------------------------
//		DoAddTabWithGroup
// ----------------------------------------------------------------------------

int
CPeekContext::DoAddTabWithGroup(
	const TCHAR*	inTabName,
	const TCHAR*	inGroupName,
	const TCHAR*	inWindowClass,
	void**			outTabWnd )
{
	return CPeekApp::DoAddTabWithGroup(
		m_pCaptureContext,
		inTabName,
		inGroupName,
		inWindowClass,
		outTabWnd );
}


// ----------------------------------------------------------------------------
//		DoSendPluginMessage
// ----------------------------------------------------------------------------

int
CPeekContext::DoSendPluginMessage(
	UInt8*	inParamData,
	UInt32	inParamDataLen )
{
	return CPeekApp::DoSendPluginMessage(
		m_pCaptureContext,
		inParamData,
		inParamDataLen );
}


// ----------------------------------------------------------------------------
//		DoMakeFilter
// ----------------------------------------------------------------------------

int
CPeekContext::DoMakeFilter(
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
//		DoPrefsGetPrefsPath
// ----------------------------------------------------------------------------

int
CPeekContext::DoGetAppResourcePath(
	TCHAR*	outAppResourcePath )
{
	return CPeekApp::DoGetAppResourcePath( outAppResourcePath );
}


// ----------------------------------------------------------------------------
//		DoSaveContext
// ----------------------------------------------------------------------------

int
CPeekContext::DoSaveContext()
{
	return CPeekApp::DoSaveContext( m_pCaptureContext );
}
