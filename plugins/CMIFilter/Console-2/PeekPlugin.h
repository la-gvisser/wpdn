// ============================================================================
//	PeekPlugin.h
//		interface for the CPeekPlugin class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "StdAfx.h"
#include "PeekPlug.h"
#include "PeekApp.h"
#include "Broadcaster.h"
#include "Listener.h"

using namespace PeekPlugin;

#define		kPeekPluginResourceFolder	_T( "PluginRes" )


// ============================================================================
//		CPeekPlugin
// ============================================================================

class CPeekPlugin
	:	public CBroadcaster
	,	public CListener
{
protected:
	UInt32				m_APIVersion;
	bool				m_bIsPeekUnicode;
	CPeekApp*			m_pPeekApp;
	HINSTANCE			m_hInstance;			// Calling app's instance handle.
	HINSTANCE			m_hResInstance;
	TCHAR*				m_szResourceFilename;
	bool				m_bResourceDllLoaded;
	UINT				m_nCodePage;			// Code page from User's default, for ASCII build

public:
	static const UInt32	kPluginAPIVersion10	= 10;
	static const UInt32	kPluginAPIVersion11	= 11;
	static const UInt32	kPluginAPIVersion12	= 12;

	bool			m_bFailure;

	;				CPeekPlugin();
	virtual			~CPeekPlugin();

	virtual int		HandleMessage( SInt16 inMessage, PluginParamBlock* ioParams );
	virtual BOOL	DllMain( HINSTANCE inInstance, DWORD inReason, LPVOID inReserved );

	//	CListener
	virtual void	ListenToMessage( CBLMessage& ioMessage );
	virtual void	NotifyAll( CString inMessage );

	UInt32			GetAPIVersion() const { return m_APIVersion; }
	virtual void	GetName( TCHAR* outName, int nBufSize ) const = 0;
	virtual void	GetID( PluginID* outID ) const = 0;
	virtual UInt16	GetAttributes() const = 0;
	virtual UInt64	GetAttributesEx() const = 0;
	virtual UInt16	GetSupportedActions() const = 0;
	virtual UInt16	GetDefaultActions() const = 0;
	virtual void	GetSupportedProtoSpecs( UInt32* outNumProtoSpecs, UInt32** outProtoSpecs ) const;

	//	Plugin Message Handlers
	virtual int		OnLoad( PluginLoadParam* ioParams );
	virtual int		OnUnload();
	virtual int		OnReadPrefs();
	virtual int		OnWritePrefs();
	virtual int		OnAbout();
	virtual int		OnOptions();
	virtual int		OnFilterOptions();
	virtual int		OnCreateContext( PluginCreateContextParam* ioParams );
	virtual int		OnDisposeContext( PluginDisposeContextParam* ioParams );
	// virtual int		OnSummaryDescr(); // <-- Deprecated
	virtual int		OnNameTableUpdate( PluginNameTableUpdateParam* ioParams );
	virtual int		OnCreateNewAdapter( PluginCreateNewAdapterParam* ioParams );
	virtual int		OnGetAdapterList( PluginGetAdapterListParam* ioParams );
	virtual int		OnDeleteAdapter( PluginDeleteAdapterParam* ioParams );
	virtual int		OnSetAdapterAttribs( PluginAdapterAttribsParam* ioParams );
	virtual int		OnGetAdapterAttribs( PluginAdapterAttribsParam* ioParams );
	virtual int		OnAdapterProperties( PluginAdapterPropertiesParam* ioParams );
	virtual int		OnUserDecode( PluginUserDecodeParam* ioParams );
	virtual int		OnDecodersLoaded();
	virtual int		OnIsMediaSupported( PluginMediaSupportedParam* ioParams );

	//	Context Specific Message Handlers
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

	// Utility routines.
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
							inMediaType, inMediaSubType, outIPHdrData, outIPHdrBytes,
							outIPData, outIPDataBytes, outDeliveryProtocol );
					}
	CWnd*			GetPeekWindow() const { return (m_pPeekApp != NULL) ? m_pPeekApp->GetPeekWindow() : NULL; }

	virtual HINSTANCE	GetInstanceHandle() { return m_hInstance; }
	virtual HINSTANCE	GetResourceHandle();

	// Wrappers for callback functions.
	int				DoGetProtocolName( UInt32 inProtocol, TCHAR* outString );
	int				DoGetProtocolLongName( UInt32 inProtocol, TCHAR* outString );
	int				DoGetProtocolHierName( UInt32 inProtocol, TCHAR* outString );
	int				DoGetProtocolParent( UInt32 inProtocol, UInt32* outProtocolParent );
	int				DoGetProtocolColor( UInt32 inProtocol, COLORREF* outColor );
	int				DoIsDescendentOf( UInt32 inSubProtocol, const UInt32* inParentArray,
						SInt32 inParentCount, UInt32* outMatchID );
	int				DoLookupName( const UInt8* inEntry, UInt16 inEntryType, TCHAR* outName, UInt8* outType );
	int				DoLookupName( PluginNameTableEntry* ioEntry );
	int				DoLookupEntry( const TCHAR* inName, UInt16 inEntryType, UInt8* outEntry );
	int				DoLookupEntry( PluginNameTableEntry* ioEntry );
	int				DoAddNameEntry( const TCHAR* inName, const TCHAR* inGroup, const UInt8* inEntry,
						UInt16 inEntryType, UInt16 inOptions );
	int				DoAddNameEntry( const PluginNameTableEntry* inEntry, UInt16 inOptions );
	int				DoSummaryGetEntry( PluginCaptureContext inCaptureContext, const TCHAR* inLabel,
						const TCHAR* inGroup, UInt32* outType, void** outData, UInt8* outSource );
	int				DoSummaryModifyEntry( PluginCaptureContext inCaptureContext, const TCHAR* inLabel,
						const TCHAR* inGroup, UInt32 inType, void* inData );
	const UInt8*	DoPacketGetLayer( UInt8 inLayerType, UInt8 inMediaType, UInt8 inMediaSubType,
						const PluginPacket* inPacket, const UInt8* inPacketData, UInt16* ioBytesLeft );
	int				DoPacketGetAddress( PluginCaptureContext inCaptureContext, UInt8 inAddressSelector,
						UInt8 inMediaType, UInt8 inMediaSubType, const PluginPacket* inPacket,
						const UInt8* inPacketData, UInt64 inPacketNumber, UInt8* outAddress, UInt16* outAddressType );
	int				DoPrefsGetValue( PluginCaptureContext inCaptureContext, const TCHAR* inName,
						const void* outData, UInt32* ioLength );
	int				DoPrefsSetValue( PluginCaptureContext inCaptureContext, const TCHAR* inName,
						const void* inData, UInt32 inLength );
	int				DoInsertPacket( PluginCaptureContext inCaptureContext, const PluginPacket* inPacket,
						const UInt8* inPacketData, UInt8 inMediaType, UInt8 inMediaSubType, UInt32 inReserved );
	int				DoSelectPackets( PluginCaptureContext inCaptureContext, UInt32 inNumPackets,
						UInt64* inPacketNumbers, UInt32 inFlags );
	int				DoNotify( PluginCaptureContext inCaptureContext, UInt64 inTimeStamp, UInt8 inSeverity,
						const TCHAR* inShortString, const TCHAR* inLongString );
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
	int				DoSelectPacketsEx( PluginCaptureContext inCaptureContext, const UInt8* inSrcAddr,
						UInt16 inSrcAddrType, const UInt8* inDestAddr, UInt16 inDestAddrType, UInt16 inSrcPort,
						UInt16 inDestPort, UInt16 inPortTypes, bool inBothDirections, UInt32 inFlags );
	int				DoGetPacketCount( PluginCaptureContext inCaptureContext, UInt32* outCount );
	int				DoGetFirstPacketIndex( PluginCaptureContext inCaptureContext, UInt32* outIndex );
	int				DoGetPacket( PluginCaptureContext inCaptureContext, UInt32 inIndex,
						UInt64* outPacketNumber, const PluginPacket** outPacket, const UInt8** outPacketData );
	int				DoClaimPacketString( PluginCaptureContext inCaptureContext, UInt64 inPacketNumber, bool inExpert );
	int				DoSetPacketListColumn( PluginCaptureContext inCaptureContext, const PluginID* inColumnID,
						const TCHAR* inColumnName, const TCHAR* inHeaderText, bool inDefaultVisibility,
						UInt16 inDefaultWidth, UInt8 inAlignment );
	int				DoDecodeOp( PluginContext inDecodeContext, PluginContext inDecodeState, int inOp,
						UInt32 inValue, UInt32 inGlobal, UInt8 inStyleValue, UInt8 inStyleLabel,
						const TCHAR* inString, const TCHAR* inSummary );
	int				DoAddUserDecode( const TCHAR* inFuncName );
	int				DoAddTab( PluginCaptureContext inCaptureContext, const TCHAR* inTabName,
						const TCHAR* inWindowClass, void** outTabWnd );
	int				DoAddTabWithGroup( PluginCaptureContext	inCaptureContext, const TCHAR* inTabName,
						const TCHAR* inGroupName, const TCHAR* inWindowClass, void** outTabWnd );
	int				DoSendPluginMessage( PluginCaptureContext inCaptureContext, UInt8* inParamData,
						UInt32 inParamDataLen );
	int				DoMakeFilter( const UInt8* inSrcAddr, UInt16 inSrcAddrType, const UInt8* inDestAddr,
						UInt16 inDestAddrType, UInt16 inSrcPort, UInt16 inDestPort, UInt16 inPortTypes,
						bool inBothDirections );
	int				DoGetAppResourcePath( TCHAR* outAppResourcePath );
	int				DoSaveContext( PluginCaptureContext	inCaptureContext );
};
