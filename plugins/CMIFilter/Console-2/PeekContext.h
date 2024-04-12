// ============================================================================
//	PeekContext.h
//		interface for the CPeekContext class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekApp.h"

using namespace PeekPlugin;


// ============================================================================
//		CPeekContext
// ============================================================================

class CPeekContext
	:	public CListener
{
protected:
	PluginCaptureContext	m_pCaptureContext;
	PluginContextFlags		m_ContextFlags;
	HWND					m_hCaptureWindow;
	CString					m_strCaptureTitle;

public:
	;				CPeekContext() : m_pCaptureContext( NULL ), m_ContextFlags( 0 ), m_hCaptureWindow( NULL ) {}
	virtual			~CPeekContext() {}

	virtual int		Init( PluginCreateContextParam* ioParams );
	virtual int		Term();
	virtual PluginCaptureContext	GetCaptureContext() { return m_pCaptureContext; }
	virtual	PluginContextFlags		GetContextType() { return (m_ContextFlags && 0xFF); }
	virtual HWND	GetCaptureWindow() { return m_hCaptureWindow; }
	virtual CString	GetCaptureTitle();

	// CListener
	virtual void	ListenToMessage( CBLMessage& inMessage );

	virtual int		OnReset( PluginResetParam* ioParams );
	virtual int		OnStartCapture( PluginStartCaptureParam* ioParams );
	virtual int		OnStopCapture( PluginStopCaptureParam* ioParams );
	virtual int		OnProcessPacket( PluginProcessPacketParam* ioParams );
	virtual int		OnGetPacketString( PluginGetPacketStringParam* ioParams );
	virtual int		OnApply( PluginApplyParam* ioParams );
	virtual int		OnSelect( PluginSelectParam* ioParams );
	virtual int		OnPacketsLoaded( PluginPacketsLoadedParam* ioParams );
	virtual int		OnSummary( PluginSummaryParam* ioParams );
	virtual int		OnFilter( PluginFilterParam* ioParams );
	virtual int		OnGetPacketAnalysis( PluginGetPacketStringParam* ioParams );
	virtual int		OnDecodePacket( PluginDecodePacketParam* ioParams );
	virtual int		OnProcessPluginMessage( PluginProcessPluginMessageParam* ioParams );
	virtual int		OnHandleNotify( PluginHandleNotifyParam* ioParams );
	virtual int		OnGetTextForPacketListCell( PluginGetTextForPacketListCellParam* ioParams );
	virtual int		OnMeasurePacketListCell( PluginMeasurePacketListCellParam* ioParams );
	virtual int		OnDrawPacketListCell( PluginDrawPacketListCellParam* ioParams );
	virtual int		OnContextOptions( PluginContextOptionsParam* ioParams );
	virtual int		OnSetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnGetContextPrefs( PluginContextPrefsParam* ioParams );
	virtual int		OnProcessTime( PluginProcessTimeParam* ioParams );

	// Utility functions.
	bool			LoadString( UInt32 inID, TCHAR* outString, int nBufSize ) const;
	bool			IsMACFrame( const UInt8* inPacketData ) { return CPeekApp::IsMACFrame( inPacketData ); }
	UInt16			GetSourceRouteInfoLength( const PluginPacket* inPacket, const UInt8* inPacketData ) {
						return CPeekApp::GetSourceRouteInfoLength( inPacket, inPacketData );
					}
	UInt16			GetPacketActualLength( const PluginPacket* inPacket ) {
						return CPeekApp::GetPacketActualLength( inPacket );
					}
	int				GetTCPUDPInfo( UInt32 inProtoSpecMatched, const PluginPacket* inPacket,
							const UInt8* inPacketData, UInt8 inMediaType, UInt8 inMediaSubType,
							const UInt8** outHdrData, UInt16* outHdrBytes, const UInt8** outData,
							UInt16* outDataBytes, UInt32* outDeliveryProtocol ) {
						return CPeekApp::GetTCPUDPInfo( inProtoSpecMatched, inPacket, inPacketData,
							inMediaType, inMediaSubType, outHdrData, outHdrBytes, outData,
							outDataBytes, outDeliveryProtocol );
					}
	int				GetIPInfo( UInt32 inProtoSpecMatched, const PluginPacket* inPacket,
							const UInt8* inPacketData, UInt8 inMediaType, UInt8 inMediaSubType,
							const UInt8** outIPHdrData, UInt16* outIPHdrBytes,
							const UInt8** outIPData, UInt16* outIPDataBytes,
							UInt32* outDeliveryProtocol ) {
						return CPeekApp::GetIPInfo( inProtoSpecMatched, inPacket, inPacketData,
							inMediaType, inMediaSubType, outIPHdrData, outIPHdrBytes, outIPData,
							outIPDataBytes, outDeliveryProtocol );
					}

	// Wrappers for callback functions.
	int				DoGetProtocolName( UInt32 inProtocol, TCHAR* outString );
	int				DoGetProtocolLongName( UInt32 inProtocol, TCHAR* outString );
	int				DoGetProtocolHierName( UInt32 inProtocol, TCHAR* outString );
	int				DoGetProtocolParent( UInt32 inProtocol, UInt32* outProtocolParent );
	int				DoGetProtocolColor( UInt32 inProtocol, COLORREF* outColor );
	int				DoIsDescendentOf( UInt32 inSubProtocol, const UInt32* inParentArray,
						SInt32 inParentCount, UInt32* outMatchID );
	int				DoLookupName( const UInt8* inEntry, SInt16 inEntryType, TCHAR* outName, UInt8* outType );
	int				DoLookupName( PluginNameTableEntry* ioEntry );
	int				DoLookupEntry( const TCHAR* inName, SInt16 inEntryType, UInt8* outEntry );
	int				DoLookupEntry( PluginNameTableEntry* ioEntry );
	int				DoAddNameEntry( const TCHAR* inName, const TCHAR* inGroup, const UInt8* inEntry,
						SInt16 inEntryType, UInt16 inOptions );
	int				DoAddNameEntry( const PluginNameTableEntry* inEntry, UInt16 inOptions );
	int				DoSummaryGetEntry( const TCHAR* inLabel, const TCHAR* inGroup, UInt32* outType,
						void** outData, UInt8* outSource );
	int				DoSummaryModifyEntry( const TCHAR* inLabel, const TCHAR* inGroup,
						UInt32 inType, void* inData );
	const UInt8*	DoPacketGetLayer( UInt8 inLayerType, UInt8 inMediaType, UInt8 inMediaSubType,
						const PluginPacket* inPacket, const UInt8* inPacketData, UInt16* ioBytesLeft );
	int				DoPacketGetAddress( UInt8 inAddressSelector, UInt8 inMediaType, UInt8 inMediaSubType,
						const PluginPacket* inPacket, const UInt8* inPacketData, UInt64 inPacketNumber,
						UInt8* outAddress, UInt16* outAddressType );
	int				DoPrefsGetValue( const TCHAR* inName, const void* outData, UInt32* ioLength );
	int				DoPrefsSetValue( const TCHAR* inName, const void* inData, UInt32 inLength );
	int				DoInsertPacket( const PluginPacket* inPacket, const UInt8* inPacketData,
						UInt8 inMediaType, UInt8 inMediaSubType, UInt32 inReserved );
	int				DoSelectPackets( UInt32 inNumPackets, UInt64* inPacketNumbers, UInt32 inFlags );
	int				DoNotify( UInt64 inTimeStamp, UInt8 inSeverity, const TCHAR* inShortString,
						const TCHAR* inLongString );
	int				DoSendPacket( const UInt8* inPacketData, UInt16 inPacketLength );
	int				DoInvokeNameEditDialog( PluginNameTableEntry* inEntry );
	int				DoResolveAddress( UInt8* inAddress, UInt16 inAddressType );
	const UInt8*	DoPacketGetDataLayer( UInt32 inProtoSpec, UInt8 inMediaType, UInt8 inMediaSubType,
						const PluginPacket*	inPacket, const UInt8* inPacketData, UInt16* ioBytesLeft,
						UInt32* outSourceProtoSpec );
	const UInt8*	DoPacketGetHeaderLayer( UInt32 inProtoSpec, UInt8 inMediaType, UInt8 inMediaSubType,
						const PluginPacket*	inPacket, const UInt8* inPacketData, UInt16* ioBytesLeft,
						UInt32* outSourceProtoSpec );
	int				DoPrefsGetPrefsPath( TCHAR* outString );
	int				DoSelectPacketsEx( const UInt8* inSrcAddr, UInt16 inSrcAddrType, const UInt8* inDestAddr,
						UInt16 inDestAddrType, UInt16 inSrcPort, UInt16 inDestPort, UInt16 inPortTypes,
						bool inBothDirections, UInt32 inFlags );
	int				DoGetPacketCount( UInt32* outCount );
	int				DoGetFirstPacketIndex( UInt32* outIndex );
	int				DoGetPacket( UInt32 inIndex, UInt64* outPacketNumber, const PluginPacket** outPacket,
						const UInt8** outPacketData );
	int				DoClaimPacketString( UInt64 inPacketNumber, bool inExpert );
	int				DoSetPacketListColumn( const PluginID* inColumnID,
						const TCHAR* inColumnName, const TCHAR* inHeaderText, bool inDefaultVisibility,
						UInt16 inDefaultWidth, UInt8 inAlignment );
	int				DoDecodeOp( PluginContext inDecodeContext, PluginContext inDecodeState, int inOp,
						UInt32 inValue, UInt32 inGlobal, UInt8 inStyleValue, UInt8 inStyleLabel,
						const TCHAR* inString, const TCHAR* inSummary );
	int				DoAddUserDecode( const TCHAR* inFuncName );
	int				DoAddTab( const TCHAR* inTabName, const TCHAR* inWindowClass, void** outTabWnd );
	int				DoAddTabWithGroup( const TCHAR* inTabName,
						const TCHAR* inGroupName, const TCHAR* inWindowClass, void** outTabWnd );
	int				DoSendPluginMessage( UInt8* inParamData, UInt32 inParamDataLen );
	int				DoMakeFilter( const UInt8* inSrcAddr, UInt16 inSrcAddrType, const UInt8* inDestAddr,
						UInt16 inDestAddrType, UInt16 inSrcPort, UInt16 inDestPort, UInt16 inPortTypes,
						bool inBothDirections );
	int				DoGetAppResourcePath( TCHAR* outAppResourcePath );
	int				DoSaveContext();
};
