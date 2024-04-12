// ============================================================================
//	PeekApp.h
//		interface for the CPeekApp class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekPlug.h"

using namespace PeekPlugin;


// ============================================================================
//		CPeekApp
// ============================================================================

class CPeekApp
{
protected:
	static PluginAppContext				s_AppContext;
	static CWnd*						m_pPeekWnd;

	static GetProtocolNameProc			s_procGetProtocolName;
	static GetProtocolLongNameProc		s_procGetProtocolLongName;
	static GetProtocolHierNameProc		s_procGetProtocolHierName;
	static GetProtocolParentProc		s_procGetProtocolParent;
	static GetProtocolColorProc			s_procGetProtocolColor;
	static IsDescendentOfProc			s_procIsDescendentOf;
	static LookupNameProc				s_procLookupName;
	static LookupEntryProc				s_procLookupEntry;
	static AddNameEntryProc				s_procAddNameEntry;
	static SummaryGetEntryProc			s_procSummaryGetEntry;
	static SummaryModifyEntryProc		s_procSummaryModifyEntry;
	static PacketGetLayerProc			s_procPacketGetLayer;
	static PacketGetAddressProc			s_procPacketGetAddress;
	static PrefsGetValueProc			s_procPrefsGetValue;
	static PrefsSetValueProc			s_procPrefsSetValue;
	static InsertPacketProc				s_procInsertPacket;
	static SelectPacketsProc			s_procSelectPackets;
	static NotifyProc					s_procNotify;
	static SendPacketProc				s_procSendPacket;
	static InvokeNameEditDialogProc		s_procInvokeNameEditDialog;
	static ResolveAddressProc			s_procResolveAddress;
	static PacketGetDataLayerProc		s_procPacketGetDataLayer;
	static PacketGetHeaderLayerProc		s_procPacketGetHeaderLayer;
	static PrefsGetPrefsPathProc		s_procPrefsGetPrefsPath;
	static SelectPacketsExProc			s_procSelectPacketsEx;
	static GetPacketCountProc			s_procGetPacketCount;
	static GetFirstPacketIndexProc		s_procGetFirstPacketIndex;
	static GetPacketProc				s_procGetPacket;
	static ClaimPacketStringProc		s_procClaimPacketString;
	static SetPacketListColumnProc		s_procSetPacketListColumn;
	static DecodeOpProc					s_procDecodeOp;
	static AddUserDecodeProc			s_procAddUserDecode;
	static AddTabProc					s_procAddTab;
	static AddTabWithGroupProc			s_procAddTabWithGroup;
	static SendPluginMessageProc		s_procSendPluginMessage;
	static MakeFilterProc				s_procMakeFilter;
	static GetAppResourcePathProc		s_procGetAppResourcePath;
	static SaveContextProc				s_procSaveContext;

public:
	;				CPeekApp() {}
	;				~CPeekApp() {}

	int				Init( PluginAppContext AppContext, ClientAppDataPtr ClientAppData );

	// Utility Functions
	static bool		IsMACFrame( const UInt8* inPacketData );
	static UInt16	GetSourceRouteInfoLength( const PluginPacket* inPacket,
						const UInt8* inPacketData );
	static UInt16	GetPacketActualLength( const PluginPacket* inPacket );
	static int		GetTCPUDPInfo( UInt32 inProtoSpecMatched, const PluginPacket* inPacket,
						const UInt8* inPacketData, UInt8 inMediaType, UInt8 inMediaSubType,
						const UInt8** outHdrData, UInt16* outHdrBytes, const UInt8** outData,
						UInt16* outDataBytes, UInt32* outDeliveryProtocol );
	static int		GetIPInfo( UInt32 inProtoSpecMatched, const PluginPacket* inPacket,
						const UInt8* inPacketData, UInt8 inMediaType, UInt8 inMediaSubType,
						const UInt8** outIPHdrData, UInt16* outIPHdrBytes, const UInt8** outIPData,
						UInt16* outIPDataBytes, UInt32* outDeliveryProtocol );
	CWnd*			GetPeekWindow() const { return m_pPeekWnd; }

	// Wrappers for callback functions.
	static int		DoGetProtocolName( UInt32 inProtocol, TCHAR* outString );
	static int		DoGetProtocolLongName( UInt32 inProtocol, TCHAR* outString );
	static int		DoGetProtocolHierName( UInt32 inProtocol, TCHAR* outString );
	static int		DoGetProtocolParent( UInt32 inProtocol, UInt32* outProtocolParent );
	static int		DoGetProtocolColor( UInt32 inProtocol, COLORREF* outColor );
	static int		DoIsDescendentOf( UInt32 inSubProtocol, const UInt32* inParentArray,
						SInt32 inParentCount, UInt32* outMatchID );
	static int		DoLookupName( const UInt8* inEntry, UInt16 inEntryType,
						TCHAR* outName, UInt8* outType );
	static int		DoLookupName( PluginNameTableEntry* ioEntry );
	static int		DoLookupEntry( const TCHAR* inName, UInt16 inEntryType, UInt8* outEntry );
	static int		DoLookupEntry( PluginNameTableEntry* ioEntry );
	static int		DoAddNameEntry( const TCHAR* inName, const TCHAR* inGroup, const UInt8* inEntry,
						UInt16 inEntryType, UInt16 inOptions );
	static int		DoAddNameEntry( const PluginNameTableEntry* inEntry, UInt16 inOptions );
	static int		DoSummaryGetEntry( PluginCaptureContext inCaptureContext, const TCHAR* inLabel,
						const TCHAR* inGroup, UInt32* outType, void** outData, UInt8* outSource );
	static int		DoSummaryModifyEntry( PluginCaptureContext inCaptureContext, const TCHAR* inLabel,
						const TCHAR* inGroup, UInt32 inType, void* inData );
	static const UInt8*		DoPacketGetLayer( UInt8 inLayerType, UInt8 inMediaType,
								UInt8 inMediaSubType, const PluginPacket* inPacket,
								const UInt8* inPacketData, UInt16* ioBytesLeft );
	static int		DoPacketGetAddress( PluginCaptureContext inCaptureContext,
						UInt8 inAddressSelector, UInt8 inMediaType, UInt8 inMediaSubType,
						const PluginPacket* inPacket, const UInt8* inPacketData,
						UInt64 inPacketNumber, UInt8* outAddress, UInt16* outAddressType );
	static int		DoPrefsGetValue( PluginCaptureContext inCaptureContext, const TCHAR* inName,
						const void* outData, UInt32* ioLength );
	static int		DoPrefsSetValue( PluginCaptureContext inCaptureContext, const TCHAR* inName,
						const void* inData, UInt32 inLength );
	static int		DoInsertPacket( PluginCaptureContext inCaptureContext,
						const PluginPacket* inPacket, const UInt8* inPacketData,
						UInt8 inMediaType, UInt8 inMediaSubType, UInt32 inReserved );
	static int		DoSelectPackets( PluginCaptureContext inCaptureContext, UInt32 inNumPackets,
						UInt64* inPacketNumbers, UInt32 inFlags );
	static int		DoNotify( PluginCaptureContext inCaptureContext, UInt64 inTimeStamp,
						UInt8 inSeverity, const TCHAR* inShortString, const TCHAR* inLongString );
	static int		DoSendPacket( const UInt8* inPacketData, UInt16 inPacketLength );

	static int		DoInvokeNameEditDialog( PluginNameTableEntry* inEntry );
	static int		DoResolveAddress( UInt8* inAddress, UInt16 inAddressType );
	static const UInt8*		DoPacketGetDataLayer( UInt32 inProtoSpec, UInt8 inMediaType,
								UInt8 inMediaSubType, const PluginPacket* inPacket,
								const UInt8* inPacketData, UInt16* ioBytesLeft,
								UInt32* outSourceProtoSpec );
	static const UInt8*		DoPacketGetHeaderLayer( UInt32 inProtoSpec, UInt8 inMediaType,
								UInt8 inMediaSubType, const PluginPacket* inPacket,
								const UInt8* inPacketData, UInt16* ioBytesLeft,
								UInt32* outSourceProtoSpec );
	static int		DoPrefsGetPrefsPath( TCHAR* outString );
	static int		DoSelectPacketsEx( PluginCaptureContext inCaptureContext, const UInt8* inSrcAddr,
						UInt16 inSrcAddrType, const UInt8* inDestAddr, UInt16 inDestAddrType,
						UInt16 inSrcPort, UInt16 inDestPort, UInt16 inPortTypes,
						bool inBothDirections, UInt32 inFlags );
	static int		DoGetPacketCount( PluginCaptureContext inCaptureContext, UInt32* outCount );
	static int		DoGetFirstPacketIndex( PluginCaptureContext inCaptureContext, UInt32* outIndex );
	static int		DoGetPacket( PluginCaptureContext inCaptureContext, UInt32 inIndex,
						UInt64* outPacketNumber, const PluginPacket** outPacket,
						const UInt8** outPacketData );
	static int		DoClaimPacketString( PluginCaptureContext inCaptureContext,
						UInt64 inPacketNumber, bool inExpert );
	static int		DoSetPacketListColumn( PluginCaptureContext inCaptureContext, const PluginID* inColumnID,
						const TCHAR* inColumnName, const TCHAR* inHeaderText, bool inDefaultVisibility,
						UInt16 inDefaultWidth, UInt8 inAlignment );
	static int		DoDecodeOp( PluginContext inDecodeContext, PluginContext inDecodeState, int inOp,
						UInt32 inValue, UInt32 inGlobal, UInt8 inStyleValue, UInt8 inStyleLabel,
						const TCHAR* inString, const TCHAR* inSummary );
	static int		DoAddUserDecode( const TCHAR* inFuncName );
	static int		DoAddTab( PluginCaptureContext inCaptureContext, const TCHAR* inTabName,
						const TCHAR* inWindowClass, void** outTabWnd );
	static int		DoAddTabWithGroup( PluginCaptureContext	inCaptureContext, const TCHAR* inTabName,
						const TCHAR* inGroupName, const TCHAR* inWindowClass, void** outTabWnd );
	static int		DoSendPluginMessage( PluginCaptureContext inCaptureContext, UInt8* inParamData,
						UInt32 inParamDataLen );
	static int		DoMakeFilter( const UInt8* inSrcAddr, UInt16 inSrcAddrType, const UInt8* inDestAddr,
						UInt16 inDestAddrType, UInt16 inSrcPort, UInt16 inDestPort, UInt16 inPortTypes,
						bool inBothDirections );
	static int		DoGetAppResourcePath( TCHAR* outAppResourcePath );
	static int		DoSaveContext( PluginCaptureContext	inCaptureContext );
};
