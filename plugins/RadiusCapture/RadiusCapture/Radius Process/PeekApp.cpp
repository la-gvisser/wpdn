// ============================================================================
//	PeekApp.cpp
//		implementation of the CPeekApp class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2007. All rights reserved.

#include "StdAfx.h"
#include "PeekPSIDs.h"
#include "PeekApp.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Static members.
PluginAppContext			CPeekApp::s_AppContext					= NULL;
#if TARGET_OS_WIN32
CWnd*						CPeekApp::m_pPeekWnd					= NULL;
#endif

GetProtocolNameProc			CPeekApp::s_procGetProtocolName			= NULL;
GetProtocolLongNameProc		CPeekApp::s_procGetProtocolLongName		= NULL;
GetProtocolHierNameProc		CPeekApp::s_procGetProtocolHierName		= NULL;
GetProtocolParentProc		CPeekApp::s_procGetProtocolParent		= NULL;
GetProtocolColorProc		CPeekApp::s_procGetProtocolColor		= NULL;
IsDescendentOfProc			CPeekApp::s_procIsDescendentOf			= NULL;
LookupNameProc				CPeekApp::s_procLookupName				= NULL;
LookupEntryProc				CPeekApp::s_procLookupEntry				= NULL;
AddNameEntryProc			CPeekApp::s_procAddNameEntry			= NULL;
SummaryGetEntryProc			CPeekApp::s_procSummaryGetEntry			= NULL;
SummaryModifyEntryProc		CPeekApp::s_procSummaryModifyEntry		= NULL;
PacketGetLayerProc			CPeekApp::s_procPacketGetLayer			= NULL;
PacketGetAddressProc		CPeekApp::s_procPacketGetAddress		= NULL;
PrefsGetValueProc			CPeekApp::s_procPrefsGetValue			= NULL;
PrefsSetValueProc			CPeekApp::s_procPrefsSetValue			= NULL;
InsertPacketProc			CPeekApp::s_procInsertPacket			= NULL;
SelectPacketsProc			CPeekApp::s_procSelectPackets			= NULL;
NotifyProc					CPeekApp::s_procNotify					= NULL;
SendPacketProc				CPeekApp::s_procSendPacket				= NULL;
#if TARGET_OS_WIN32
InvokeNameEditDialogProc	CPeekApp::s_procInvokeNameEditDialog	= NULL;
ResolveAddressProc			CPeekApp::s_procResolveAddress			= NULL;
PacketGetDataLayerProc		CPeekApp::s_procPacketGetDataLayer		= NULL;
PacketGetHeaderLayerProc	CPeekApp::s_procPacketGetHeaderLayer	= NULL;
PrefsGetPrefsPathProc		CPeekApp::s_procPrefsGetPrefsPath		= NULL;
SelectPacketsExProc			CPeekApp::s_procSelectPacketsEx			= NULL;
GetPacketCountProc			CPeekApp::s_procGetPacketCount			= NULL;
GetFirstPacketIndexProc		CPeekApp::s_procGetFirstPacketIndex		= NULL;
GetPacketProc				CPeekApp::s_procGetPacket				= NULL;
ClaimPacketStringProc		CPeekApp::s_procClaimPacketString		= NULL;
SetPacketListColumnProc		CPeekApp::s_procSetPacketListColumn		= NULL;
DecodeOpProc				CPeekApp::s_procDecodeOp				= NULL;
AddUserDecodeProc			CPeekApp::s_procAddUserDecode			= NULL;
AddTabProc					CPeekApp::s_procAddTab					= NULL;
AddTabWithGroupProc			CPeekApp::s_procAddTabWithGroup			= NULL;
SendPluginMessageProc		CPeekApp::s_procSendPluginMessage		= NULL;
MakeFilterProc				CPeekApp::s_procMakeFilter				= NULL;
GetAppResourcePathProc		CPeekApp::s_procGetAppResourcePath		= NULL;
SaveContextProc				CPeekApp::s_procSaveContext				= NULL;
#elif TARGET_OS_MAC
EventDispatchProc			CPeekApp::s_procEventDispatch			= NULL;
#endif


#if TARGET_OS_WIN32
// ----------------------------------------------------------------------------
//      GetProcAddress2
// ----------------------------------------------------------------------------
static FARPROC
GetProcAddress2(
	HMODULE			inModule,
	const TCHAR*	inProcName )
{
	CT2A	szProcName( inProcName );
	FARPROC result = ::GetProcAddress( inModule, szProcName );
	if ( NULL == result ) throw PLUGIN_RESULT_ERROR;
	return result;
}
#endif // TARGET_OS_WIN32


// =============================================================================
//		CPeekApp
// =============================================================================

// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

int
CPeekApp::Init(
	PluginAppContext	AppContext,
	ClientAppDataPtr	ClientAppData )
{
	s_AppContext = AppContext;

	// Find all callbacks.
#if TARGET_OS_WIN32
	try {
		m_pPeekWnd = AfxGetMainWnd();

		HMODULE theModule = ::GetModuleHandle( static_cast<TCHAR*>( ClientAppData ) );
		if ( theModule == NULL ) return PLUGIN_RESULT_ERROR;

		// Get the proc addresses of all the callbacks.
		s_procGetProtocolName			= reinterpret_cast<GetProtocolNameProc>(		GetProcAddress2( theModule, kExportName_GetProtocolName ) );
		s_procGetProtocolLongName		= reinterpret_cast<GetProtocolLongNameProc>(	GetProcAddress2( theModule, kExportName_GetProtocolLongName ) );
		s_procGetProtocolHierName		= reinterpret_cast<GetProtocolHierNameProc>(	GetProcAddress2( theModule, kExportName_GetProtocolHierName ) );
		s_procGetProtocolParent			= reinterpret_cast<GetProtocolParentProc>(		GetProcAddress2( theModule, kExportName_GetProtocolParent ) );
		s_procGetProtocolColor			= reinterpret_cast<GetProtocolColorProc>(		GetProcAddress2( theModule, kExportName_GetProtocolColor ) );
		s_procIsDescendentOf			= reinterpret_cast<IsDescendentOfProc>(			GetProcAddress2( theModule, kExportName_IsDescendentOf ) );
		s_procLookupName				= reinterpret_cast<LookupNameProc>(				GetProcAddress2( theModule, kExportName_LookupName ) );
		s_procLookupEntry				= reinterpret_cast<LookupEntryProc>(			GetProcAddress2( theModule, kExportName_LookupEntry ) );
		s_procAddNameEntry				= reinterpret_cast<AddNameEntryProc>(			GetProcAddress2( theModule, kExportName_AddNameEntry ) );
		s_procSummaryGetEntry			= reinterpret_cast<SummaryGetEntryProc>(		GetProcAddress2( theModule, kExportName_SummaryGetEntry ) );
		s_procSummaryModifyEntry		= reinterpret_cast<SummaryModifyEntryProc>(		GetProcAddress2( theModule, kExportName_SummaryModifyEntry ) );
		s_procPacketGetLayer			= reinterpret_cast<PacketGetLayerProc>(			GetProcAddress2( theModule, kExportName_PacketGetLayer ) );
		s_procPacketGetAddress			= reinterpret_cast<PacketGetAddressProc>(		GetProcAddress2( theModule, kExportName_PacketGetAddress ) );
		s_procPrefsGetValue				= reinterpret_cast<PrefsGetValueProc>(			GetProcAddress2( theModule, kExportName_PrefsGetValue ) );
		s_procPrefsSetValue				= reinterpret_cast<PrefsSetValueProc>(			GetProcAddress2( theModule, kExportName_PrefsSetValue ) );
		s_procInsertPacket				= reinterpret_cast<InsertPacketProc>(			GetProcAddress2( theModule, kExportName_InsertPacket ) );
		s_procSelectPackets				= reinterpret_cast<SelectPacketsProc>(			GetProcAddress2( theModule, kExportName_SelectPackets) );
		s_procNotify					= reinterpret_cast<NotifyProc>(					GetProcAddress2( theModule, kExportName_Notify ) );
		s_procSendPacket				= reinterpret_cast<SendPacketProc>(				GetProcAddress2( theModule, kExportName_SendPacket ) );

		// Windows Only
		s_procInvokeNameEditDialog		= reinterpret_cast<InvokeNameEditDialogProc>(	GetProcAddress2( theModule, kExportName_InvokeNameEditDialog ) );
		s_procResolveAddress			= reinterpret_cast<ResolveAddressProc>(			GetProcAddress2( theModule, kExportName_ResolveAddress ) );
		s_procPacketGetDataLayer		= reinterpret_cast<PacketGetDataLayerProc>(		GetProcAddress2( theModule, kExportName_PacketGetDataLayer ) );
		s_procPacketGetHeaderLayer		= reinterpret_cast<PacketGetHeaderLayerProc>(	GetProcAddress2( theModule, kExportName_PacketGetHeaderLayer ) );
		s_procPrefsGetPrefsPath			= reinterpret_cast<PrefsGetPrefsPathProc>(		GetProcAddress2( theModule, kExportName_PrefsGetPrefsPath ) );
		s_procSelectPacketsEx			= reinterpret_cast<SelectPacketsExProc>(			GetProcAddress2( theModule, kExportName_SelectPacketsEx ) );
		s_procClaimPacketString			= reinterpret_cast<ClaimPacketStringProc>(		GetProcAddress2( theModule, kExportName_ClaimPacketString ) );
		s_procAddTab					= reinterpret_cast<AddTabProc>(					GetProcAddress2( theModule, kExportName_AddTab ) );
		s_procMakeFilter				= reinterpret_cast<MakeFilterProc>(				GetProcAddress2( theModule, kExportName_MakeFilter ) );
		s_procGetAppResourcePath		= reinterpret_cast<GetAppResourcePathProc>(		GetProcAddress2( theModule, kExportName_GetAppResourcePath ) );

		// New to API 10
		s_procDecodeOp					= reinterpret_cast<DecodeOpProc>(				GetProcAddress2( theModule, kExportName_DecodeOp ) );
		s_procAddUserDecode				= reinterpret_cast<AddUserDecodeProc>(			GetProcAddress2( theModule, kExportName_AddUserDecode ) );

		// New to API 11
		s_procGetPacketCount			= reinterpret_cast<GetPacketCountProc>(			GetProcAddress2( theModule, kExportName_GetPacketCount ) );
		s_procGetFirstPacketIndex		= reinterpret_cast<GetFirstPacketIndexProc>(		GetProcAddress2( theModule, kExportName_GetFirstPacketIndex ) );
		s_procGetPacket					= reinterpret_cast<GetPacketProc>(				GetProcAddress2( theModule, kExportName_GetPacket ) );
		s_procAddTabWithGroup			= reinterpret_cast<AddTabWithGroupProc>(			GetProcAddress2( theModule, kExportName_AddTabWithGroup ) );
		s_procSendPluginMessage			= reinterpret_cast<SendPluginMessageProc>(		GetProcAddress2( theModule, kExportName_SendPluginMessage ) );

		// New to API 12
		s_procSaveContext				= reinterpret_cast<SaveContextProc>(				GetProcAddress2( theModule, kExportName_SaveContext ) );
	}
	catch( ... ) {
		// This is a non-critical error.  The callback wrappers will fail if any particular
		// callback isn't present.  This is better than blindly failing here.
		// The callbacks are listed here in API version order, get the oldest first.
		// This allows a new plugin to support older versions of Peek.
	}
#elif TARGET_OS_MAC
	if ( ClientAppData == NULL ) return PLUGIN_RESULT_ERROR;
	PeekPlugCallbacks & callbacks	= ClientAppData->mCallbacks;
	mBundle							= ClientAppData->mPluginBundle;

	// record the callback addresses so considerately supplied
	// by our gracious EtherPeek application
	s_procGetProtocolName		= callbacks.mGetProtocolNameProc;
	s_procGetProtocolLongName	= callbacks.mGetProtocolLongNameProc;
	s_procGetProtocolHierName	= callbacks.mGetProtocolHierNameProc;
	s_procGetProtocolParent		= callbacks.mGetProtocolParentProc;
	s_procGetProtocolColor		= callbacks.mGetProtocolColorProc;
	s_procIsDescendentOf		= callbacks.mIsDescendentOfProc;
	s_procNotify				= callbacks.mNotifyProc;
	s_procLookupName			= callbacks.mLookupNameProc;
	s_procLookupEntry			= callbacks.mLookupEntryProc;
	s_procAddNameEntry			= callbacks.mAddNameEntryProc;
	s_procSendPacket			= callbacks.mSendPacketProc;
	s_procSummaryGetEntry		= callbacks.mSummaryGetEntryProc;
	s_procSummaryModifyEntry	= callbacks.mSummaryModifyEntryProc;
	s_procPacketGetLayer		= callbacks.mPacketGetLayerProc;
	s_procPacketGetAddress		= callbacks.mPacketGetAddressProc;
	s_procPrefsGetValue			= callbacks.mPrefsGetValueProc;
	s_procPrefsSetValue			= callbacks.mPrefsSetValueProc;
	s_procInsertPacket			= callbacks.mInsertPacketProc;
	s_procSelectPackets			= callbacks.mSelectPacketsProc;
	s_procEventDispatch			= callbacks.mEventDispatchProc;

	// ### mac no longer keeps the plugin's resource fork open.
	// ###      Use StUseBundleResFile across
	// ### ranges when you really do need plugin resources
#endif // TARGET_OS_WIN32

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		IsMACFrame
// ----------------------------------------------------------------------------
// Determine if a Token Ring packet is MAC frame.

bool
CPeekApp::IsMACFrame(
	const UInt8*	inPacketData )
{
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return false;

	return ((inPacketData[1] & 0xC0) == 0x00);
}


// ----------------------------------------------------------------------------
//		GetSourceRouteInfoLength
// ----------------------------------------------------------------------------
// Get the source routing info length for a Token Ring LLC frame.

UInt16
CPeekApp::GetSourceRouteInfoLength(
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData )
{
	PLUGIN_ASSERT( inPacket != NULL );
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( (inPacket == NULL) || (inPacketData == NULL) ) return 0;

	UInt16  nSourceRouteInfoLength = 0;

	if ( (inPacketData[1] & 0xC0) == 0x40 ) {
		if ( (inPacketData[8] & 0x80) != 0 ) {
			nSourceRouteInfoLength = static_cast<UInt16>( inPacketData[14] & 0x1F );
		}
	}

	return nSourceRouteInfoLength;
}


// ----------------------------------------------------------------------------
//		GetPacketActualLength
// ----------------------------------------------------------------------------
// Get the actual packet length.

UInt16
CPeekApp::GetPacketActualLength(
	const PluginPacket*	inPacket )
{
	PLUGIN_ASSERT( inPacket != NULL );
	if ( inPacket == NULL ) return 0;

	UInt16  nActualLength = inPacket->fSliceLength;
	if ( nActualLength == 0 ) {
		nActualLength = inPacket->fPacketLength;
	}

	return nActualLength;
}


// ----------------------------------------------------------------------------
//		GetTCPUDPInfo
// ----------------------------------------------------------------------------

int
CPeekApp::GetTCPUDPInfo(
	UInt32				inProtoSpecMatched,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const UInt8**		outHdrData,
	UInt16*				outHdrBytes,
	const UInt8**		outData,
	UInt16*				outDataBytes,
	UInt32*				outDeliveryProtocol )
{
	// Get the parent's instance (should be UDP or TCP) so we can get the port number
	int				nResult;
	UInt32			IPInstance;
	UInt32			DPInstance;
	UInt32			nTemp;
	const UInt8		*pIPHdrData = NULL;
	UInt16			nIPHdrBytes;

	// get the IP or IPv6 parent and header so we can calculate the exact TCP/UDP length
	UInt32 ayIPs[] = { ProtoSpecDefs::IP, ProtoSpecDefs::IPv6 };
	nResult = DoIsDescendentOf( inProtoSpecMatched, ayIPs, COUNTOF( ayIPs ), &IPInstance );
	if ( nResult != PLUGIN_RESULT_SUCCESS ) {
		return PLUGIN_RESULT_ERROR;
	}

	// get the IP or IPv6 header pointer
	pIPHdrData = DoPacketGetHeaderLayer( IPInstance, inMediaType,
		inMediaSubType, inPacket, inPacketData, &nIPHdrBytes, &nTemp );
	if ( pIPHdrData == NULL ) {
		return PLUGIN_RESULT_ERROR;
	}

	// the data length is:
	// IPTotalLength - IPHdrLength - DPHeaderLength
	// where
	//	IP:	IPTotalLength = IPHDR[2,3]
	//		IPHdrLength   = (IPHDR[0] & 0x0f) * 4
	//	IPv6: IPTotalLength = IPHDR[4,5]
	//		IPHdrLength   = 40
	//
	//	TCP: DPHdrLength = (TCPHdr[12] & 0xf0) >> 2
	//	UDP: DPHdrLength = 8

	// get the parent, either TCP or UDP
	UInt32 ayDeliveryProtocolss[] = { ProtoSpecDefs::TCP, ProtoSpecDefs::UDP };
	nResult = DoIsDescendentOf( inProtoSpecMatched, ayDeliveryProtocolss,
		COUNTOF( ayDeliveryProtocolss ), &DPInstance );
	if ( nResult != PLUGIN_RESULT_SUCCESS ) {
		return PLUGIN_RESULT_ERROR;
	}

	const UInt8*	pData;
	const UInt8*	pHdrData;
	UInt16			nDataBytes;
	UInt16			nHdrBytes;

	// for the payload
	pData = DoPacketGetDataLayer( DPInstance, inMediaType, inMediaSubType,
		inPacket, inPacketData, &nDataBytes, &nTemp );

	// for the src/dest ports and header size
	pHdrData = DoPacketGetHeaderLayer( DPInstance, inMediaType, inMediaSubType,
		inPacket, inPacketData, &nHdrBytes, &nTemp );
	if ( pData == NULL ) {
		return PLUGIN_RESULT_ERROR;
	}

	// calculate the length
	UInt16 nIPHdrLength, nIPTotalLength, nDPHdrLength;
	switch ( GET_PSID( IPInstance ) ) {
	case ProtoSpecDefs::IP:	// IP
		{
			nIPHdrLength = (pIPHdrData[0] & 0x0F) * 4;
			nIPTotalLength = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pIPHdrData + 2 ) ) );
		}
		break;
	case ProtoSpecDefs::IPv6:	// IPv6
		{
			nIPHdrLength = 40;
			nIPTotalLength = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( pIPHdrData + 4 )) ) + 40;
		}
		break;
	default:
		return PLUGIN_RESULT_ERROR;
	}

	switch ( GET_PSID( DPInstance ) ) {
	case ProtoSpecDefs::TCP:
		{
			if ( nHdrBytes < 12 ) {
				return PLUGIN_RESULT_ERROR;
			}
			nDPHdrLength = (pHdrData[12] & 0xF0) >> 2;
		}
		break;
	case ProtoSpecDefs::UDP:
		{
			nDPHdrLength = 8;
		}
		break;
	default:
		return PLUGIN_RESULT_ERROR;
	}

	// adjust for potential slicing
	nDataBytes = static_cast<UInt16>( min( (nIPTotalLength - nIPHdrLength - nDPHdrLength), nDataBytes ) );

	// what does the caller want?
	if ( outHdrData != NULL ) {
		ASSERT( outHdrBytes );
		*(outHdrData) = pHdrData;
		*(outHdrBytes) = nDPHdrLength;
	}

	if ( outData != NULL ) {
		ASSERT( outDataBytes );
		*(outData) = pData;
		*(outDataBytes) = nDataBytes;
	}

	if ( outDeliveryProtocol != NULL ) {
		*(outDeliveryProtocol) = DPInstance;
	}

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		GetIPInfo
// ----------------------------------------------------------------------------
//  just IP, not IPv6

int
CPeekApp::GetIPInfo(
	UInt32				inProtoSpecMatched,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const UInt8**		outIPHdrData,
	UInt16*				outIPHdrBytes,
	const UInt8**		outIPData,
	UInt16*				outIPDataBytes,
	UInt32*				outDeliveryProtocol )
{
	// get the IP header and data pointer and length
	UInt32	ayIPs[] = { ProtoSpecDefs::IP };
	UInt32	nTemp;
	int		nResult;

	nResult = DoIsDescendentOf( inProtoSpecMatched, ayIPs, COUNTOF( ayIPs ), outDeliveryProtocol );
	if ( nResult != PLUGIN_RESULT_SUCCESS ) {
		return PLUGIN_RESULT_ERROR;
	}

	*outIPHdrData = DoPacketGetHeaderLayer( *outDeliveryProtocol, inMediaType, inMediaSubType,
		inPacket, inPacketData, outIPHdrBytes, &nTemp );

	if ( *outIPHdrData == NULL ) {
		return PLUGIN_RESULT_ERROR;
	}

	// get the IP header length
	UInt16	nIPHdrLength = ((*outIPHdrData)[0] & 0x0F) * 4;
	UInt16	nIPTotalLength = NETWORKTOHOST16( *(reinterpret_cast<const UInt16*>( (*outIPHdrData) + 2) ) );

	// figure out the IP data start based on IP header length
	*outIPData = *outIPHdrData + nIPHdrLength;

	*outIPDataBytes = nIPTotalLength - nIPHdrLength;

	// adjust for potential slicing
	UInt16	nDataLength = (inPacket->fSliceLength > 0)
		? min( inPacket->fSliceLength, inPacket->fPacketLength )
		: inPacket->fPacketLength;

	const UInt8*	pEnd = inPacketData + nDataLength;
	if ( (*outIPData + *outIPDataBytes) > pEnd ) {
		*outIPDataBytes = static_cast<UInt16>( pEnd - *outIPData );
	}

	return PLUGIN_RESULT_SUCCESS;
}

// ////////////////////////////////////////////////////////////////////////////
//		Callbacks
// ////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
//		DoGetProtocolName
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetProtocolName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	PLUGIN_ASSERT( outString != NULL );
	if ( outString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetProtocolName != NULL );
	if ( s_procGetProtocolName == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procGetProtocolName)( inProtocol, outString );
#else
	wchar_t	szString[kPluginNameMaxLength];
	int		nResult = (s_procGetProtocolName)( inProtocol, szString );
	strcpy_s( outString, kPluginPacketStringMaxLength, CW2A( szString ) );
	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoGetProtocolLongName
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetProtocolLongName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	PLUGIN_ASSERT( outString != NULL );
	if ( outString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetProtocolLongName != NULL );
	if ( s_procGetProtocolLongName == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procGetProtocolLongName)( inProtocol, outString );
#else
	wchar_t	szString[kPluginNameMaxLength];
	int		nResult = (s_procGetProtocolLongName)( inProtocol, szString );
	strcpy_s( outString, kPluginPacketStringMaxLength, CW2A( szString ) );
	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoGetProtocolHierName
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetProtocolHierName(
	UInt32	inProtocol,
	TCHAR*	outString )
{
	PLUGIN_ASSERT( outString != NULL );
	if ( outString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetProtocolHierName != NULL );
	if ( s_procGetProtocolHierName == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procGetProtocolHierName)( inProtocol, outString );
#else
	wchar_t	szString[kPluginNameMaxLength];
	int		nResult = (s_procGetProtocolHierName)( inProtocol, szString );
	strcpy_s( outString, kPluginPacketStringMaxLength, CW2A( szString ) );
	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoGetProtocolParent
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetProtocolParent(
	UInt32	inProtocol,
	UInt32*	outProtocolParent )
{
	PLUGIN_ASSERT( outProtocolParent != NULL );
	if ( outProtocolParent == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetProtocolParent != NULL );
	if ( s_procGetProtocolParent == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procGetProtocolParent)( inProtocol, outProtocolParent );
}


// ----------------------------------------------------------------------------
//		DoGetProtocolColor
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetProtocolColor(
	UInt32		inProtocol,
	COLORREF*	outColor )
{
	PLUGIN_ASSERT( outColor != NULL );
	if ( outColor == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetProtocolColor != NULL );
	if ( s_procGetProtocolColor == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procGetProtocolColor)( inProtocol, outColor );
}


// ----------------------------------------------------------------------------
//		DoIsDescendentOf
// ----------------------------------------------------------------------------

int
CPeekApp::DoIsDescendentOf(
	UInt32			inSubProtocol,
	const UInt32*	inParentArray,
	SInt32			inParentCount,
	UInt32*			outMatchID )
{
	PLUGIN_ASSERT( inParentArray != NULL );
	if ( inParentArray == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inParentCount != 0 );
	if ( inParentCount == 0 ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outMatchID != NULL );
	if ( outMatchID == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procIsDescendentOf != NULL );
	if ( s_procIsDescendentOf == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procIsDescendentOf)(
		inSubProtocol,
		inParentArray,
		inParentCount,
		outMatchID );
}


// ----------------------------------------------------------------------------
//		DoLookupName
// ----------------------------------------------------------------------------

int
CPeekApp::DoLookupName(
	const UInt8*	inEntry,
	UInt16			inEntryType,
	TCHAR*			outName,
	UInt8*			outType )
{
	PLUGIN_ASSERT( inEntry != NULL );
	if ( inEntry == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outName != NULL );
	if ( outName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outType != NULL );
	if ( outType == NULL ) return PLUGIN_RESULT_ERROR;

	PluginNameTableEntry	theEntry = {
		inEntry,
		outName,
		NULL,
		PluginColor( 0 ),
		inEntryType,
		0
	};
	int nResult = DoLookupName( &theEntry );
	if ( outType != NULL ) {
		*outType = theEntry.fHostType;
	}
	return nResult;
}


// ----------------------------------------------------------------------------
//		DoLookupName
// ----------------------------------------------------------------------------

int
CPeekApp::DoLookupName(
	PluginNameTableEntry*	ioEntry )
{
	PLUGIN_ASSERT( ioEntry != NULL );
	if ( ioEntry == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procLookupName != NULL );
	if ( s_procLookupName == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procLookupName)( ioEntry );
#else
	PluginNameTableEntryW	EntryProxy;
	memcpy( &EntryProxy, ioEntry, sizeof( PluginNameTableEntry ) );
	CT2W	szName( ioEntry->fName );
	CT2W	szGroup( ioEntry->fName );
	if ( ioEntry->fName ) EntryProxy.fName = szName;
	if ( ioEntry->fGroup ) EntryProxy.fGroup = szGroup;

	int	nResult = (s_procLookupName)( &EntryProxy );

	ioEntry->fEntry = EntryProxy.fEntry;
	if ( ioEntry->fName ) strcpy_s( (char *) ioEntry->fName, kPluginPacketStringMaxLength, CW2A( szName ) );
	if ( ioEntry->fGroup ) strcpy_s( (char *) ioEntry->fGroup, kPluginPacketStringMaxLength, CW2A( szGroup ) );
	ioEntry->fColor = EntryProxy.fColor;
	ioEntry->fEntryType = EntryProxy.fEntryType;
	ioEntry->fHostType = EntryProxy.fHostType;

	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoLookupEntry
// ----------------------------------------------------------------------------

int
CPeekApp::DoLookupEntry(
	const TCHAR*	inName,
	UInt16			inEntryType,
	UInt8*			outEntry )
{
	PLUGIN_ASSERT( inName != NULL );
	if ( inName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outEntry != NULL );
	if ( outEntry == NULL ) return PLUGIN_RESULT_ERROR;

	PluginNameTableEntry	theEntry = {
		outEntry,
		inName,
		NULL,
		PluginColor( 0 ),
		inEntryType,
		0
	};
	return DoLookupEntry( &theEntry );
}


// ----------------------------------------------------------------------------
//		DoLookupEntry
// ----------------------------------------------------------------------------

int
CPeekApp::DoLookupEntry(
	PluginNameTableEntry*	ioEntry )
{
	PLUGIN_ASSERT( ioEntry != NULL );
	if ( ioEntry == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procLookupEntry != NULL );
	if ( s_procLookupEntry == NULL ) return PLUGIN_RESULT_ERROR;
#ifdef _UNICODE
	return (s_procLookupEntry)( ioEntry );
#else
	PluginNameTableEntryW	EntryProxy;
	memcpy( &EntryProxy, ioEntry, sizeof( PluginNameTableEntry ) );
	CT2W	szName( ioEntry->fName );
	CT2W	szGroup( ioEntry->fGroup );
	if ( ioEntry->fName ) EntryProxy.fName = szName;
	if ( ioEntry->fGroup ) EntryProxy.fGroup = szGroup;

	int nResult = (s_procLookupEntry)( &EntryProxy );

	ioEntry->fEntry = EntryProxy.fEntry;
	if ( ioEntry->fName ) strcpy_s( (char *) ioEntry->fName, kPluginPacketStringMaxLength, CW2A( szName ) );
	if ( ioEntry->fGroup ) strcpy_s( (char *) ioEntry->fGroup, kPluginPacketStringMaxLength, CW2A( szGroup ) );
	ioEntry->fColor = EntryProxy.fColor;
	ioEntry->fEntryType = EntryProxy.fEntryType;
	ioEntry->fHostType = EntryProxy.fHostType;

	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoAddNameEntry
// ----------------------------------------------------------------------------

int
CPeekApp::DoAddNameEntry(
	const TCHAR*	inName,
	const TCHAR*	inGroup,
	const UInt8*	inEntry,
	UInt16			inEntryType,
	UInt16			inOptions )
{
	PLUGIN_ASSERT( inName != NULL );
	if ( inName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inGroup != NULL );
	if ( inGroup == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inEntry != NULL );
	if ( inEntry == NULL ) return PLUGIN_RESULT_ERROR;

	PluginNameTableEntry	theEntry = {
		inEntry,
		inName,
		inGroup,
		PluginColor( 0 ),
		inEntryType,
		0
	};
	return DoAddNameEntry( &theEntry, inOptions );
}


// ----------------------------------------------------------------------------
//		DoAddNameEntry
// ----------------------------------------------------------------------------

int
CPeekApp::DoAddNameEntry(
	const PluginNameTableEntry*	inEntry,
	UInt16						inOptions )
{
	PLUGIN_ASSERT( inEntry != NULL );
	if ( inEntry == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procAddNameEntry != NULL );
	if ( s_procAddNameEntry == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procAddNameEntry)( inEntry, inOptions );
#else
	PluginNameTableEntryW	EntryProxy = {
		inEntry->fEntry,
		CT2W( inEntry->fName ),
		CT2W( inEntry->fGroup ),
		inEntry->fColor,
		inEntry->fEntryType,
		inEntry->fHostType
	};
	return (s_procAddNameEntry)( &EntryProxy, inOptions );
#endif
}


// ----------------------------------------------------------------------------
//		DoSummaryGetEntry
// ----------------------------------------------------------------------------

int
CPeekApp::DoSummaryGetEntry(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inLabel,
	const TCHAR*			inGroup,
	UInt32*					outType,
	void**					outData,
	UInt8*					outSource )
{
	PLUGIN_ASSERT( inLabel != NULL );
	if ( inLabel == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inGroup != NULL );
	if ( inGroup == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outType != NULL );
	if ( outType == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outData != NULL );
	if ( outData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outSource != NULL );
	if ( outSource == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSummaryGetEntry != NULL );
	if ( s_procSummaryGetEntry == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSummaryGetEntry)(
		s_AppContext,
		inCaptureContext,
		CT2W( inLabel ),
		CT2W( inGroup ),
		outType,
		outData,
		outSource );
}


// ----------------------------------------------------------------------------
//		DoSummaryModifyEntry
// ----------------------------------------------------------------------------

int
CPeekApp::DoSummaryModifyEntry(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inLabel,
	const TCHAR*			inGroup,
	UInt32					inType,
	void*					inData )
{
	PLUGIN_ASSERT( inLabel != NULL );
	if ( inLabel == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inGroup != NULL );
	if ( inGroup == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inData != NULL );
	if ( inData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSummaryModifyEntry != NULL );
	if ( s_procSummaryModifyEntry == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSummaryModifyEntry)(
		s_AppContext,
		inCaptureContext,
		CT2W( inLabel ),
		CT2W( inGroup ),
		inType,
		inData );
}


// ----------------------------------------------------------------------------
//		DoPacketGetLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekApp::DoPacketGetLayer(
	UInt8				inLayerType,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt16*				ioBytesLeft )
{
	PLUGIN_ASSERT( inPacket != NULL );
	if ( inPacket == NULL ) return NULL;
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return NULL;
	PLUGIN_ASSERT( ioBytesLeft != NULL );
	if ( ioBytesLeft == NULL ) return NULL;
	PLUGIN_ASSERT( s_procPacketGetLayer != NULL );
	if ( s_procPacketGetLayer == NULL ) return NULL;

	return (s_procPacketGetLayer)(
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
CPeekApp::DoPacketGetAddress(
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
	PLUGIN_ASSERT( inPacket != NULL );
	if ( inPacket == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outAddress != NULL );
	if ( outAddress == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outAddressType != NULL );
	if ( outAddressType == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procPacketGetAddress != NULL );
	if ( s_procPacketGetAddress == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procPacketGetAddress)(
		s_AppContext,
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
CPeekApp::DoPrefsGetValue(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inName,
	const void*				outData,
	UInt32*					ioLength )
{
	PLUGIN_ASSERT( inName != NULL );
	if ( inName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outData != NULL );
	if ( outData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procPrefsGetValue != NULL );
	if ( s_procPrefsGetValue == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procPrefsGetValue)(
		s_AppContext,
		inCaptureContext,
		CT2W( inName ),
		outData,
		ioLength );
}


// ----------------------------------------------------------------------------
//		DoPrefsSetValue
// ----------------------------------------------------------------------------

int
CPeekApp::DoPrefsSetValue(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inName,
	const void*				inData,
	UInt32					inLength )
{
	PLUGIN_ASSERT( inName != NULL );
	if ( inName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inData != NULL );
	if ( inData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procPrefsSetValue != NULL );
	if ( s_procPrefsSetValue == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procPrefsSetValue)(
		s_AppContext,
		inCaptureContext,
		CT2W( inName ),
		inData,
		inLength );
}


// ----------------------------------------------------------------------------
//		DoInsertPacket
// ----------------------------------------------------------------------------

int
CPeekApp::DoInsertPacket(
	PluginCaptureContext	inCaptureContext,
	const PluginPacket*		inPacket,
	const UInt8*			inPacketData,
	UInt8					inMediaType,
	UInt8					inMediaSubType,
	UInt32					inReserved )
{
	PLUGIN_ASSERT( inPacket != NULL );
	if ( inPacket == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procInsertPacket != NULL );
	if ( s_procInsertPacket == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procInsertPacket)(
		s_AppContext,
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
CPeekApp::DoSelectPackets(
	PluginCaptureContext	inCaptureContext,
	UInt32					inNumPackets,
	UInt64*					inPacketNumbers,
	UInt32					inFlags )
{
	PLUGIN_ASSERT( inPacketNumbers != NULL );
	if ( inPacketNumbers == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSelectPackets != NULL );
	if ( s_procSelectPackets == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSelectPackets)(
		s_AppContext,
		inCaptureContext,
		inNumPackets,
		inPacketNumbers,
		inFlags );
}


// ----------------------------------------------------------------------------
//		DoNotify
// ----------------------------------------------------------------------------

int
CPeekApp::DoNotify(
	PluginCaptureContext	inCaptureContext,
	UInt64					inTimeStamp,
	UInt8					inSeverity,
	const TCHAR*			inShortString,
	const TCHAR*			inLongString )
{
	PLUGIN_ASSERT( inShortString != NULL );
	if ( inShortString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inLongString != NULL );
	if ( inLongString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procNotify != NULL );
	if ( s_procNotify == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procNotify)(
		s_AppContext,
		inCaptureContext,
		inTimeStamp,
		inSeverity,
		CT2W( inShortString ),
		CT2W( inLongString ) );
}


// ----------------------------------------------------------------------------
//		DoSendPacket
// ----------------------------------------------------------------------------

int
CPeekApp::DoSendPacket(
	const UInt8*	inPacketData,
	UInt16			inPacketLength )
{
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSendPacket != NULL );
	if ( s_procSendPacket == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSendPacket)( inPacketData, inPacketLength );
}

#if TARGET_OS_WIN32
// ----------------------------------------------------------------------------
//		DoInvokeNameEditDialog
// ----------------------------------------------------------------------------

int
CPeekApp::DoInvokeNameEditDialog(
	PluginNameTableEntry*	inEntry )
{
	PLUGIN_ASSERT( inEntry != NULL );
	if ( inEntry == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procInvokeNameEditDialog != NULL );
	if ( s_procInvokeNameEditDialog == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procInvokeNameEditDialog)( inEntry );
#else
	PluginNameTableEntryW	EntryProxy = {
		inEntry->fEntry,
		CT2W( inEntry->fName ),
		CT2W( inEntry->fGroup ),
		inEntry->fColor,
		inEntry->fEntryType,
		inEntry->fHostType
	};
	return (s_procInvokeNameEditDialog)( &EntryProxy );
#endif
}


// ----------------------------------------------------------------------------
//		DoResolveAddress
// ----------------------------------------------------------------------------

int
CPeekApp::DoResolveAddress(
	UInt8*	inAddress,
	UInt16	inAddressType )
{
	PLUGIN_ASSERT( inAddress != NULL );
	if ( inAddress == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procResolveAddress != NULL );
	if ( s_procResolveAddress == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procResolveAddress)( inAddress, inAddressType );
}


// ----------------------------------------------------------------------------
//		DoPacketGetDataLayer
// ----------------------------------------------------------------------------

const UInt8*
CPeekApp::DoPacketGetDataLayer(
	UInt32				inProtoSpec,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt16*				ioBytesLeft,
	UInt32*				outSourceProtoSpec )
{
	PLUGIN_ASSERT( inPacket != NULL );
	if ( inPacket == NULL ) return NULL;
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return NULL;
	PLUGIN_ASSERT( ioBytesLeft != NULL );
	if ( ioBytesLeft == NULL ) return NULL;
	PLUGIN_ASSERT( outSourceProtoSpec != NULL );
	if ( outSourceProtoSpec == NULL ) return NULL;
	PLUGIN_ASSERT( s_procPacketGetDataLayer != NULL );
	if ( s_procPacketGetDataLayer == NULL ) return NULL;

	return (s_procPacketGetDataLayer)(
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
CPeekApp::DoPacketGetHeaderLayer(
	UInt32				inProtoSpec,
	UInt8				inMediaType,
	UInt8				inMediaSubType,
	const PluginPacket*	inPacket,
	const UInt8*		inPacketData,
	UInt16*				ioBytesLeft,
	UInt32*				outSourceProtoSpec )
{
	PLUGIN_ASSERT( inPacket != NULL );
	if ( inPacket == NULL ) return NULL;
	PLUGIN_ASSERT( inPacketData != NULL );
	if ( inPacketData == NULL ) return NULL;
	PLUGIN_ASSERT( ioBytesLeft != NULL );
	if ( ioBytesLeft == NULL ) return NULL;
	PLUGIN_ASSERT( outSourceProtoSpec != NULL );
	if ( outSourceProtoSpec == NULL ) return NULL;
	PLUGIN_ASSERT( s_procPacketGetHeaderLayer != NULL );
	if ( s_procPacketGetHeaderLayer == NULL ) return NULL;

	return (s_procPacketGetHeaderLayer)(
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
CPeekApp::DoPrefsGetPrefsPath(
	TCHAR*	outString )
{
	PLUGIN_ASSERT( outString != NULL );
	if ( outString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procPrefsGetPrefsPath != NULL );
	if ( s_procPrefsGetPrefsPath == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procPrefsGetPrefsPath)( outString );
#else
	wchar_t	szString[kPluginNameMaxLength];
	int		nResult = (s_procPrefsGetPrefsPath)( szString );
	_tcscpy_s( outString, kPluginPacketStringMaxLength, CW2T( szString ) );
	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoSelectPacketsEx
// ----------------------------------------------------------------------------

int
CPeekApp::DoSelectPacketsEx(
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
	PLUGIN_ASSERT( inSrcAddr != NULL );
	if ( inSrcAddr == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inDestAddr != NULL );
	if ( inDestAddr == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSelectPacketsEx != NULL );
	if ( s_procSelectPacketsEx == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSelectPacketsEx)(
		s_AppContext,
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
CPeekApp::DoGetPacketCount(
	PluginCaptureContext	inCaptureContext,
	UInt32*					outCount )
{
	PLUGIN_ASSERT( outCount != NULL );
	if ( outCount == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetPacketCount != NULL );
	if ( s_procGetPacketCount == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procGetPacketCount)(
		s_AppContext,
		inCaptureContext,
		outCount );
}


// ----------------------------------------------------------------------------
//		DoGetFirstPacketIndex
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetFirstPacketIndex(
	PluginCaptureContext	inCaptureContext,
	UInt32*					outIndex )
{
	PLUGIN_ASSERT( outIndex != NULL );
	if ( outIndex == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetFirstPacketIndex != NULL );
	if ( s_procGetFirstPacketIndex == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procGetFirstPacketIndex)(
		s_AppContext,
		inCaptureContext,
		outIndex );
}


// ----------------------------------------------------------------------------
//		DoGetPacket
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetPacket(
	PluginCaptureContext	inCaptureContext,
	UInt32					inIndex,
	UInt64*					outPacketNumber,
	const PluginPacket**	outPacket,
	const UInt8**			outPacketData )
{
	PLUGIN_ASSERT( outPacketNumber != NULL );
	if ( outPacketNumber == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outPacket != NULL );
	if ( outPacket == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( outPacketData != NULL );
	if ( outPacketData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetPacket != NULL );
	if ( s_procGetPacket == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procGetPacket)(
		s_AppContext,
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
CPeekApp::DoClaimPacketString(
	PluginCaptureContext	inCaptureContext,
	UInt64					inPacketNumber,
	bool					inExpert )
{
	PLUGIN_ASSERT( s_procClaimPacketString != NULL );
	if ( s_procClaimPacketString == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procClaimPacketString)(
		s_AppContext,
		inCaptureContext,
		inPacketNumber,
		inExpert );
}


// ----------------------------------------------------------------------------
//		DoSetPacketListColumn
// ----------------------------------------------------------------------------

int
CPeekApp::DoSetPacketListColumn(
	PluginCaptureContext	inCaptureContext,
	const PluginID*			inColumnID,
	const TCHAR*			inColumnName,
	const TCHAR*			inHeaderText,
	bool					inDefaultVisibility,
	UInt16					inDefaultWidth,
	UInt8					inAlignment )
{
	PLUGIN_ASSERT( inColumnID != NULL );
	if ( inColumnID == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inColumnName != NULL );
	if ( inColumnName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inHeaderText != NULL );
	if ( inHeaderText == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSetPacketListColumn != NULL );
	if ( s_procSetPacketListColumn == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSetPacketListColumn)(
		s_AppContext,
		inCaptureContext,
		inColumnID,
		CT2W( inColumnName ),
		CT2W( inHeaderText ),
		inDefaultVisibility,
		inDefaultWidth,
		inAlignment );
}


// ----------------------------------------------------------------------------
//		DoDecodeOp
// ----------------------------------------------------------------------------

int
CPeekApp::DoDecodeOp(
	PluginContext	inDecodeContext,
	PluginContext	inDecodeState,
	int				nOp,
	UInt32			nValue,
	UInt32			nGlobal,
	UInt8			nStyleValue,
	UInt8			nStyleLabel,
	TCHAR*			pszString,
	TCHAR*			pszSummary )
{
	PLUGIN_ASSERT( pszString != NULL );
	if ( pszString == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( pszSummary != NULL );
	if ( pszSummary == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procDecodeOp != NULL );
	if ( s_procDecodeOp == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procDecodeOp)(
		inDecodeContext,
		inDecodeState,
		nOp,
		nValue,
		nGlobal,
		nStyleValue,
		nStyleLabel,
		CT2W( pszString ),
		CT2W( pszSummary ) );
}


// ----------------------------------------------------------------------------
//		DoAddUserDecode
// ----------------------------------------------------------------------------

int
CPeekApp::DoAddUserDecode(
	const TCHAR*	inFuncName )
{
	PLUGIN_ASSERT( inFuncName != NULL );
	if ( inFuncName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procAddUserDecode != NULL );
	if ( s_procAddUserDecode == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procAddUserDecode)( CT2W( inFuncName ) );
}


// ----------------------------------------------------------------------------
//		DoAddTab
// ----------------------------------------------------------------------------

int
CPeekApp::DoAddTab(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inTabName,
	const TCHAR*			inWindowClass,
	void**					outTabWnd )
{
	PLUGIN_ASSERT( inTabName != NULL );
	if ( inTabName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inWindowClass != NULL );
	if ( inWindowClass == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inWindowClass != NULL );
	if ( inWindowClass == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procAddTab != NULL );
	if ( s_procAddTab == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procAddTab)(
		s_AppContext,
		inCaptureContext,
		CT2W( inTabName ),
		CT2W( inWindowClass ),
		outTabWnd );
}


// ----------------------------------------------------------------------------
//		DoAddTabWithGroup
// ----------------------------------------------------------------------------

int
CPeekApp::DoAddTabWithGroup(
	PluginCaptureContext	inCaptureContext,
	const TCHAR*			inTabName,
	const TCHAR*			inGroupName,
	const TCHAR*			inWindowClass,
	void**					outTabWnd )
{
	PLUGIN_ASSERT( inTabName != NULL );
	if ( inTabName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inGroupName != NULL );
	if ( inGroupName == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inWindowClass != NULL );
	if ( inWindowClass == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inWindowClass != NULL );
	if ( inWindowClass == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procAddTab != NULL );
	if ( s_procAddTab == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procAddTabWithGroup)(
		s_AppContext,
		inCaptureContext,
		CT2W( inTabName ),
		CT2W( inGroupName ),
		CT2W( inWindowClass ),
		outTabWnd );
}


// ----------------------------------------------------------------------------
//		DoSendPluginMessage
// ----------------------------------------------------------------------------

int
CPeekApp::DoSendPluginMessage(
	PluginCaptureContext	inCaptureContext,
	UInt8*					inParamData,
	UInt32					inParamDataLen )
{
	PLUGIN_ASSERT( inParamData != NULL );
	if ( inParamData == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procSendPluginMessage != NULL );
	if ( s_procSendPluginMessage == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSendPluginMessage)(
		s_AppContext,
		inCaptureContext,
		inParamData,
		inParamDataLen );
}


// ----------------------------------------------------------------------------
//		DoMakeFilter
// ----------------------------------------------------------------------------

int
CPeekApp::DoMakeFilter(
	const UInt8*	inSrcAddr,
	UInt16			inSrcAddrType,
	const UInt8*	inDestAddr,
	UInt16			inDestAddrType,
	UInt16			inSrcPort,
	UInt16			inDestPort,
	UInt16			inPortTypes,
	bool			inBothDirections )
{
	PLUGIN_ASSERT( inSrcAddr != NULL );
	if ( inSrcAddr == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( inDestAddr != NULL );
	if ( inDestAddr == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procMakeFilter != NULL );
	if ( s_procMakeFilter == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procMakeFilter)(
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
CPeekApp::DoGetAppResourcePath(
	TCHAR*	outAppResourcePath )
{
	PLUGIN_ASSERT( outAppResourcePath != NULL );
	if ( outAppResourcePath == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetAppResourcePath != NULL );
	if ( s_procGetAppResourcePath == NULL ) return PLUGIN_RESULT_ERROR;

#ifdef _UNICODE
	return (s_procGetAppResourcePath)( outAppResourcePath );
#else
	wchar_t	szString[kPluginNameMaxLength];
	int		nResult = (s_procGetAppResourcePath)( szString);
	_tcscpy_s( outAppResourcePath, kPluginNameMaxLength, CW2T( szString ) );
	return nResult;
#endif
}


// ----------------------------------------------------------------------------
//		DoSaveContext
// ----------------------------------------------------------------------------

int
CPeekApp::DoSaveContext(
	PluginCaptureContext	inCaptureContext )
{
	PLUGIN_ASSERT( s_procSaveContext != NULL );
	if ( s_procSaveContext == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procSaveContext)( s_AppContext, inCaptureContext );
}

#elif TARGET_OS_MAC

// ----------------------------------------------------------------------------
//		DoEventDispatch
// ----------------------------------------------------------------------------

void
CPeekApp::DoEventDispatch(
	const EventRecord*	inEvent )
{
	PLUGIN_ASSERT( inEvent != NULL );
	if ( inEvent == NULL ) return;
	PLUGIN_ASSERT( s_procEventDispatch != NULL );
	if ( s_procEventDispatch == NULL ) return;

	(s_procEventDispatch)( inEvent );
}



// ----------------------------------------------------------------------------
//		DoGetProtocolColor
// ----------------------------------------------------------------------------

int
CPeekApp::DoGetProtocolColor(
	UInt32		inProtocol,
	RGBColor*	outColor )
{
	PLUGIN_ASSERT( outColor != NULL );
	if ( outColor == NULL ) return PLUGIN_RESULT_ERROR;
	PLUGIN_ASSERT( s_procGetProtocolColor != NULL );
	if ( s_procGetProtocolColor == NULL ) return PLUGIN_RESULT_ERROR;

	return (s_procGetProtocolColor)( inProtocol, outColor );
}
#endif // TARGET_OS_xx
