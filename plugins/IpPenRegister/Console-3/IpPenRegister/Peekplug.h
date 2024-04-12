// ============================================================================
//	PeekPlug.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#ifndef PEEKPLUG_H
#define PEEKPLUG_H

#include "WPTypes.h"
#include "PacketHeaders.h"

#ifdef __cplusplus
namespace PeekPlugin {
extern "C" {
#endif

#pragma pack(push,1)

// The API version.
#define kPluginAPIVersion	12


// ============================================================================
//		Type and Constant Definitions
// ============================================================================

// Plug-in identifier (same as GUID).
typedef struct PluginID
{
	UInt32	Data1;
	UInt16	Data2;
	UInt16	Data3;
	UInt8	Data4[8];
} PluginID;

// Opaque types used by application to store context.
typedef void*	PluginAppContext;
typedef void*	PluginCaptureContext;

// Opaque type used by plug-ins to store private data.
typedef void*	PluginContext;

// Other context data passed with CreateContext.
typedef void*	PluginContextData;
typedef UInt32	PluginContextFlags;

// Plug-in graphical types.
typedef HDC			PluginDrawingHandle;
typedef COLORREF	PluginColor;
typedef SIZE		PluginSize;
typedef RECT		PluginRect;

// Plug-in attributes.
// Passed as UInt16
enum
{
	kPluginAttr_ProcessPackets			= 0x0001,
	kPluginAttr_GetPacketString			= 0x0002,
	kPluginAttr_HandlesSelection		= 0x0004,
	kPluginAttr_HandlesSummaries		= 0x0008,
	kPluginAttr_HasOptions				= 0x0010,
	kPluginAttr_Filters					= 0x0020,
	kPluginAttr_HandlesErrorPackets		= 0x0040,
	//kPluginAttr_ProvidesSummaryDescr	= 0x0080,	<-- Deprecated
	kPluginAttr_HandlesNotify			= 0x0080,
	kPluginAttr_NameTableUpdates		= 0x0100,
	kPluginAttr_ProvidesAnalysis		= 0x0200,
	kPluginAttr_HandlesApply			= 0x0400,
	kPluginAttr_PluginAdapter			= 0x0800,
	kPluginAttr_DecodePackets			= 0x1000,
	kPluginAttr_UserDecode				= 0x2000,
	kPluginAttr_MediaSpecific			= 0x4000,
	kPluginAttr_ProcessPluginMessages	= 0x8000,
};

// Plug-in extended attributes.
// Passed as UInt64
enum
{
	kPluginAttr_HasContextOptions		= 0x00010000,
	kPluginAttr_HandlesProcessTime		= 0x00020000,
};

// Actions supported by plug-ins.
// Passed as UInt16
enum
{
	kPluginAction_None		= 0x0000,	// No actions
	kPluginAction_Display	= 0x0001,	// Show info in main list
	kPluginAction_Notify	= 0x0002	// Notify message
};

// Messages passed to the main routine of plug-ins.
// Passed as SInt16
enum
{
	kPluginMsg_Load,						//  0 PluginLoadParam
	kPluginMsg_Unload,						//  1 NULL
	kPluginMsg_ReadPrefs,					//  2 NULL
	kPluginMsg_WritePrefs,					//  3 NULL
	kPluginMsg_CreateContext,				//  4 PluginCreateContextParam
	kPluginMsg_DisposeContext,				//  5 PluginDisposeContextParam
	kPluginMsg_ProcessPacket,				//  6 PluginProcessPacketParam
	kPluginMsg_GetPacketString,				//  7 PluginGetPacketStringParam
	kPluginMsg_Apply,						//  8 PluginApplyParam
	kPluginMsg_Select,						//  9 PluginSelectParam
	kPluginMsg_Reset,						// 10 PluginResetParam
	kPluginMsg_StartCapture,				// 11 PluginStartCaptureParam
	kPluginMsg_StopCapture,					// 12 PluginStopCaptureParam
	kPluginMsg_PacketsLoaded,				// 13 PluginPacketsLoadedParam
	kPluginMsg_About,						// 14 NULL
	kPluginMsg_Options,						// 15 NULL
	kPluginMsg_Summary,						// 16 PluginSummaryParam
	kPluginMsg_Filter,						// 17 PluginFilterParam
	kPluginMsg_FilterOptions,				// 18 NULL
	kPluginMsg_SummaryDescr,				// 19 NULL
	kPluginMsg_NameTableUpdate,				// 20 PluginNameTableUpdateParam
	kPluginMsg_GetPacketAnalysis,			// 21 PluginGetPacketStringParam
	kPluginMsg_CreateNewAdapter,			// 22 PluginCreateNewAdapterParam
	kPluginMsg_GetAdapterList,				// 23 PluginGetAdapterListParam
	kPluginMsg_DeleteAdapter,				// 24 PluginDeleteAdapterParam
	kPluginMsg_SetAdapterAttribs,			// 25 PluginAdapterAttribsParam
	kPluginMsg_GetAdapterAttribs,			// 26 PluginAdapterAttribsParam
	kPluginMsg_DecodePacket,				// 27 PluginDecodePacketParam
	kPluginMsg_AdapterProperties,			// 28 PluginAdapterPropertiesParam
	kPluginMsg_UserDecode,					// 29 PluginUserDecodeParam
	kPluginMsg_DecodersLoaded,				// 30 NULL
	kPluginMsg_IsMediaSupported,			// 31 PluginMediaSupportedParam
	kPluginMsg_ProcessPluginMessage,		// 32 PluginProcessPluginMessageParam
	kPluginMsg_HandleNotify,				// 33 PluginHandleNotifyParam
	kPluginMsg_GetTextForPacketListCell,	// 34 PluginGetTextForPacketListCellParam
	kPluginMsg_MeasurePacketListCell,		// 35 PluginMeasurePacketListCellParam
	kPluginMsg_DrawPacketListCell,			// 36 PluginDrawPacketListCellParam
	kPluginMsg_ContextOptions,				// 37 PluginContextOptionsParam
	kPluginMsg_SetContextPrefs,				// 38 PluginContextPrefsParam
	kPluginMsg_GetContextPrefs,				// 39 PluginContextPrefsParam
	kPluginMsg_ProcessTime					// 40 PluginProcessTimeParam
};

// Commands for apply messages.
// Passed as UInt16
enum
{
	kApplyMsg_Start,
	kApplyMsg_End,
	kApplyMsg_Packet
};

// Commands for select messages.
// Passed as UInt16
enum
{
	kSelectMsg_Start,
	kSelectMsg_End,
	kSelectMsg_Packet
};

// Commands for name table update messages.
// Passed as UInt16
enum
{
	kNameTableUpdateMsg_Add,
	kNameTableUpdateMsg_Edit,
	kNameTableUpdateMsg_Delete
};

// Entry types used in the  AddNameEntry, LookupName, LookupEntry
// and MakeFilterProc callbacks.
// Passed as UInt16
enum
{
	kEntryType_EthernetAddr,			// 6-byte entry
	kEntryType_AppleTalkAddr,			// 4-byte entry (socket ignored)
	kEntryType_IPAddr,					// 4-byte entry
	kEntryType_DECnetAddr,				// 6-byte entry
	kEntryType_IPv6Addr,				// 16-byte entry
	kEntryType_IPPort,					// 2-byte entry
	kEntryType_ProtoSpec,				// 2-byte entry
	kEntryType_NWPort,					// 2-byte entry
	kEntryType_WirelessAddr,			// 6-byte entry
	kEntryType_IPXAddr,					// 10-byte entry
	kEntryType_WANDLCIAddr,				// 2-byte (10-bit) entry
	kEntryType_TokenRingAddr			// 6-byte entry
};

// Host types for name lookup.
// Passed as UInt8
enum
{
	kHostType_Unknown,
	kHostType_Workstation,
	kHostType_Server,
	kHostType_Router,
	kHostType_Switch,
	kHostType_Repeater,
	kHostType_Printer,
	kHostType_AccessPoint
};

// AddNameEntry callback options.
// Passed as UInt16
enum
{
	kSetEntryName_NoMatchAdd			= 0x0001,
	kSetEntryName_NoMatchSkip			= 0x0002,
	kSetEntryName_NameMatchAdd			= 0x0010,
	kSetEntryName_NameMatchReplace		= 0x0020,
	kSetEntryName_NameMatchSkip			= 0x0040,
	kSetEntryName_AddrMatchAdd			= 0x0100,
	kSetEntryName_AddrMatchReplace		= 0x0200,
	kSetEntryName_AddrMatchSkip			= 0x0400
};

// List for DecodeOps
// Passed as int
enum
{
	kDecodeOp_OFFS,
	kDecodeOp_LABL,
	kDecodeOp_HBIT,
	kDecodeOp_DBIT,
	kDecodeOp_BBIT,
	kDecodeOp_HBYT,
	kDecodeOp_DBYT,
	kDecodeOp_BBYT,
	kDecodeOp_HWRD,
	kDecodeOp_DWRD,
	kDecodeOp_BWRD,
	kDecodeOp_HLNG,
	kDecodeOp_DLNG,
	kDecodeOp_BLNG,
	kDecodeOp_D64B
};

// Used for NameTable calls.
#define kPluginEntryNameMaxLength	64
#define kPluginEntryGroupMaxLength	64
typedef struct PluginNameTableEntry
{
	const UInt8*	fEntry;
	wchar_t*		fName;
	wchar_t*		fGroup;
	PluginColor		fColor;
	UInt16			fEntryType;
	UInt8			fHostType;
} PluginNameTableEntry;

// Byte/count pair for summary statistics.
typedef struct ByteCountPair
{
	UInt64	fBytes;
	UInt64	fCount;
} ByteCountPair;

// Summary type sizes.
// Passed as UInt32 in combination with kPluginSummaryType and kPluginSummaryFlag
enum
{
	kPluginSummarySize_Mask				= 0x000000F0,
	kPluginSummarySize_UInt8			= 0x00000010,
	kPluginSummarySize_UInt16			= 0x00000020,
	kPluginSummarySize_UInt32			= 0x00000030,
	kPluginSummarySize_UInt64			= 0x00000040,
	kPluginSummarySize_ByteCountPair	= 0x00000050,
	kPluginSummarySize_Double			= 0x00000060
};

// Summary types.
// Passed as UInt32 in combination with kPluginSummarySize and kPluginSummaryFlag
enum
{
	kPluginSummaryType_Mask				= 0x0000000F,
	kPluginSummaryType_Date				= 0x00000001,
	kPluginSummaryType_Time				= 0x00000002,
	kPluginSummaryType_Duration			= 0x00000003,
	kPluginSummaryType_PacketCount		= 0x00000004,
	kPluginSummaryType_ByteCount		= 0x00000005,
	kPluginSummaryType_BothCount		= 0x00000006,
	kPluginSummaryType_OtherCount		= 0x00000007
};

// Summary flags.
// Passed as UInt32 in combination with kPluginSummarySize and kPluginSummaryType
enum
{
	kPluginSummaryFlag_Mask				= 0x00000F00,
	kPluginSummaryFlag_Ungraphable		= 0x00000100,
	kPluginSummaryFlag_IsOverTime		= 0x00000200,
	kPluginSummaryFlag_IsSampled		= 0x00000400
};

// Severity Levels for NotifyProc.
// Passed as UInt8
enum
{
	kNotifySeverity_Informational,
	kNotifySeverity_Minor,
	kNotifySeverity_Major,
	kNotifySeverity_Severe
};

// Media types.
// Passed as UInt8
enum
{
	kPluginPacketMediaType_802_3,			// Ethernet (802.3)
	kPluginPacketMediaType_802_5,			// Token Ring (802.5)
	kPluginPacketMediaType_Fddi,			// FDDI
	kPluginPacketMediaType_Wan,				// WAN
	kPluginPacketMediaType_LocalTalk,		// LocalTalk
	kPluginPacketMediaType_Dix,				// DEC/Intel/Xerox (DIX) Ethernet
	kPluginPacketMediaType_ArcnetRaw,		// ARCNET (raw)
	kPluginPacketMediaType_Arcnet878_2,		// ARCNET (878.2)
	kPluginPacketMediaType_Atm,				// ATM
	kPluginPacketMediaType_WirelessWan,		// Various wireless media
	kPluginPacketMediaType_Irda,			// Infrared (IrDA)
	kPluginPacketMediaType_Bpc,				// Broadcast Architecture
	kPluginPacketMediaType_CoWan,			// Connection-oriented WAN
	kPluginPacketMediaType_1394,			// IEEE 1394 (fire wire) bus
	kPluginPacketMediaType_Max				// Not a real type, defined as an upper-bound
};

// Media sub type (sort of like physical medium)
// Passed as UInt8
enum
{
	kPluginPacketMediaSubType_Native,			// Native media specified by the media type
	kPluginPacketMediaSubType_80211_b,			// Wireless LAN network using 802.11b
	kPluginPacketMediaSubType_80211_a,			// Wireless LAN network using 802.11a
	kPluginPacketMediaSubType_80211_gen,		// All possible 802.11 types, a, b, g, and combinations
	kPluginPacketMediaSubType_wan_ppp,			// PPP over a T1 or an E1
	kPluginPacketMediaSubType_wan_frameRelay,	// Frame Relay
	kPluginPacketMediaSubType_wan_x25,			// X.25
	kPluginPacketMediaSubType_wan_x25e,			// X.25mod128
	kPluginPacketMediaSubType_wan_ipars,		// IPARS
	kPluginPacketMediaSubType_wan_u200,			// U200
	kPluginPacketMediaSubType_wan_Q931,			// Q.931
	kPluginPacketMediaSubType_Max				// Not a real type, defined as an upper-bound
};

// Flags for PluginPacket.
// Passed as UInt32
enum
{
	kPluginPacketFlag_Control			= 0x0001,	// Control vs. data packet
	kPluginPacketFlag_CRC				= 0x0002,	// Checksum error
	kPluginPacketFlag_Frame				= 0x0004,	// Frame error
	kPluginPacketFlag_RouteInfo			= 0x0008,	// Has routing info (Token Ring)
	kPluginPacketFlag_Oversize			= 0x0010,	// Oversize error
	kPluginPacketFlag_Runt				= 0x0020,	// Runt error
	kPluginPacketFlag_Trigger			= 0x0040,	// Trigger packet
	kPluginPacketFlag_SNAP				= 0x0080,	// SNAP packet
	kPluginPacketFlag_DroppedPackets	= 0x0100,	// One or more packets were dropped prior to this packet
};

// Status flags for PluginPacket.
// Passed as UInt32
enum
{
	kPluginPacketStatus_Truncated		= 0x02,		// Truncated packet
	kPluginPacketStatus_Encrypted		= 0x04,		// Originally encrypted (e.g., WEP)
	kPluginPacketStatus_DecryptionError	= 0x08,		// Decryption error (e.g., WEP ICV)
	kPluginPacketStatus_Sliced			= 0x20,		// Sliced packet
	kPluginPacketStatus_ShortPreamble	= 0x40		// Short preamble
};

// Offset values for PacketGetLayer.
// Passed as UInt8
enum
{
	kPacketLayerType_Header				= 0x00,
	kPacketLayerType_Data				= 0x80,

	kPacketLayer_Physical				= 0x00,
	kPacketLayer_IP						= 0x01,
	kPacketLayer_UDP					= 0x02,
	kPacketLayer_TCP					= 0x03,
	kPacketLayer_ICMP					= 0x04
};

// Address types for PacketGetAddress.
// Passed as UInt8
enum
{
	kAddressType_SrcPhysical,
	kAddressType_DestPhysical,
	kAddressType_SrcLogical,
	kAddressType_DestLogical,
	kAddressType_BSSID,
	kAddressType_Receiver,
	kAddressType_Transmitter,
	kAddressType_Address1,
	kAddressType_Address2,
	kAddressType_Address3,
	kAddressType_Address4,
	kAddressType_WANDLCI
};

// Context types for CreateContext.
// Passed as UInt32
enum
{
	kContextType_Global					= 0,
	kContextType_CaptureWindow			= 1,
	kContextType_FileWindow				= 2,
	kContextType_RemoteCaptureWindow	= 3,
	kContextType_RemoteFileWindow		= 4
};

// Select Packets flags.
// Passed as UInt32 in combination with kSelectPacketsAction
enum
{
	kSelectPacketsFlag_ReplaceSelection	= 0x00,
	kSelectPacketsFlag_AddToSelection	= 0x01,
	kSelectPackets_FlagMask				= 0x0F
};

// Select Packets actions.
// Passed as UInt32 in combination with kSelectPacketsFlag
enum
{
	kSelectPacketsAction_PromptUser		= 0x00,
	kSelectPacketsAction_HideSelected	= 0x10,
	kSelectPacketsAction_HideUnselected	= 0x20,
	kSelectPacketsAction_SelectOnly		= 0x80,
	kSelectPackets_ActionMask			= 0xF0
};

// Drawing state flags.
// Passed as UInt32
enum
{
	kDrawingState_Selected	= 0x00000001,
	kDrawingState_Focused	= 0x00000002,
	kDrawingState_Italic	= 0x00000004
};

// A packet's metadata.
typedef struct PluginPacket
{
	UInt64			fTimeStamp;				// Timestamp in nanoseconds
	UInt32			fProtoSpec;				// ProtoSpec ID
	UInt32			fFlags;					// CRC, frame, runt, ...
	UInt32			fStatus;				// Slicing, ...
	UInt16			fPacketLength;			// Total length of packet
	UInt16			fSliceLength;			// Sliced length of packet or zero
	const UInt8*	fMediaSpecInfoBlock;	// Media Specific Info pointer (may be NULL)
} PluginPacket;

// A more complete packet metadata structure.
typedef struct PluginPacketMetadata
{
	UInt64			fTimeStamp;				// Timestamp in nanoseconds
	UInt32			fProtoSpec;				// ProtoSpec ID
	UInt32			fFlags;					// CRC, frame, runt, ...
	UInt32			fStatus;				// Slicing, ...
	UInt16			fPacketLength;			// Total length of packet
	UInt16			fSliceLength;			// Sliced length of packet or zero
	const UInt8*	fMediaSpecInfoBlock;	// Media Specific Info pointer (may be NULL)
	UInt64			fPacketNumber;
	UInt32			fMatchedProtoSpec;		// The ProtoSpec that is of interest to the plug-in; may differ from fProtoSpec.
	UInt8			fMediaType;
	UInt8			fMediaSubType;
} PluginPacketMetaData;

// All packet metadata and data required to process a packet.
typedef struct PluginPacketProcessingInfo
{
	PluginPacketMetadata	fPacketMetadata;
	const UInt8*			fPacketData;
} PluginPacketProcessingInfo;

// Bit-mask definitions for the capabilities field.
// Passed as UInt32
#define kAppCapability_Unicode		0x00000001
#define kAppCapability_Pro			0x00000002
#define kAppCapability_RemoteAgent	0x00000004
#define kAppCapability_Beta			0x00000008
#define kAppCapability_Demo			0x00000010
#define kAppCapability_Eval			0x00000020
#define kAppCapability_VoIP			0x00000040
#define kAppCapability_GigHardware	0x00000080
#define kAppCapability_WAN			0x00000100
#define kAppCapability_LocalCapture	0x00000200
#define kAppCapability_Expert		0x00000400
#define kAppCapability_Undef12		0x00000800
#define kAppCapability_Undef13		0x00001000
#define kAppCapability_Undef14		0x00002000
#define kAppCapability_Undef15		0x00004000
#define kAppCapability_Undef16		0x00008000
#define kAppCapability_Undef17		0x00010000
#define kAppCapability_Undef18		0x00020000
#define kAppCapability_Undef19		0x00040000
#define kAppCapability_Undef20		0x00080000
#define kAppCapability_Undef21		0x00100000
#define kAppCapability_Undef22		0x00200000
#define kAppCapability_Undef23		0x00400000
#define kAppCapability_Undef24		0x00800000
#define kAppCapability_Undef25		0x01000000
#define kAppCapability_Undef26		0x02000000
#define kAppCapability_Undef27		0x04000000
#define kAppCapability_Undef28		0x08000000
#define kAppCapability_Undef29		0x10000000
#define kAppCapability_Undef30		0x20000000
#define kAppCapability_Undef31		0x40000000
#define kAppCapability_Undef32		0x80000000

// Column alignments.
// Passed as UInt8
enum PluginColumnAlignment
{
	kPluginColumnAlignment_Left,
	kPluginColumnAlignment_Right,
	kPluginColumnAlignment_Center
};

// ============================================================================
//		XML Tags for Plug-in Adapters
// ============================================================================

#define kAdapterTag_StartCaptureText	_T( "StartCaptureText" )
#define kAdapterTag_StopCaptureText		_T( "StopCaptureText" )
#define kAdapterEle_Group				_T( "Group" )
#define kAdapterEle_Adapter				_T( "Adapter" )
#define kAdapterTag_Name				_T( "Name" )
#define kAdapterTag_ID					_T( "ID" )
#define kAdapterTag_Speed				_T( "LinkSpeed" )
#define kAdapterTag_DefaultSpeed		_T( "DefaultLinkSpeed" )
#define kAdapterTag_MediaType			_T( "MediaType" )
#define kAdapterTag_MediaSubType		_T( "MediaSubType" )
#define kAdapterTag_MediaDesc			_T( "MediaDesc" )
#define kAdapterTag_Deletable			_T( "Deletable" )
#define kAdapterVal_False				_T( "false" )
#define kAdapterVal_True				_T( "true" )
#define kAdapterTag_Address				_T( "Address" )
#define kAdapterTag_PluginID			_T( "PluginID" )
#define kAdapterTag_Device				_T( "Device" )			// A field in the adapter list
#define kAdapterTag_Properties			_T( "Properties" )		// Supports properties
#define kAdapterTag_Continuous			_T( "Continuous" )		// Supports continuous capture (can be used to monitor)

// Wireless adapter attributes.
#define kAdapterTag_CurrentSetting		_T( "Setting" )			// One of the adapter tags below
#define kAdapterTag_Channel				_T( "Channel" )			// Channel
#define kAdapterTag_ChannelNumber		_T( "ChannelNumber" )	// Channel #
#define kAdapterTag_ChannelFreq			_T( "ChannelFreq" )		// Channel frequency
#define kAdapterTag_ChannelBand			_T( "ChannelBand" )		// Channel band
#define kAdapterTag_ESSID				_T( "ESSID" )			// ESSID (name)
#define kAdapterTag_BSSID				_T( "BSSID" )			// BSSID (MAC address)
#define kAdapterTag_ChannelScan			_T( "Scanning" )		// Children are kAdapterTag_Channel with Enabled (=true/false) and Duration (in ms) attributes
#define kAdapterTag_Enabled				_T( "Enabled" )
#define kAdapterTag_Duration			_T( "Duration" )
#define kAdapterTag_WEPKeys				_T( "WEP" )				// (set/get) contains the name of the keyset as an attibute (kAdapterTag_Name)
																// (set) children are kAdapterTag_Key with attribute kAdapterTag_KeyNum and WEP key in hex as value
#define kAdapterTag_Key					_T( "Key" )
#define kAdapterTag_KeyNum				_T( "KeyNum" )

// ============================================================================
//		Callbacks
// ============================================================================

#if TARGET_OS_WIN32
	#define PeekPlugCallback_( inReturnType, inFunctionName )	inReturnType (WINAPI * inFunctionName )
#elif _LINUX
	#define PeekPlugCallback_( inReturnType, inFunctionName )	inReturnType (* inFunctionName )
#endif

// Protocol callbacks.
typedef PeekPlugCallback_( int,	GetProtocolNameProc )
	(
		UInt32		inProtocol,
		wchar_t*	outString
	);

typedef PeekPlugCallback_( int, GetProtocolLongNameProc	)
	(
		UInt32		inProtocol,
		wchar_t*	outString
	);

typedef PeekPlugCallback_( int,	GetProtocolHierNameProc	)
	(
		UInt32		inProtocol,
		wchar_t*	outString
	);

typedef PeekPlugCallback_( int,	GetProtocolParentProc )
	(
		UInt32		inProtocol,
		UInt32*		outProtocolParent
	);

typedef PeekPlugCallback_( int,	GetProtocolColorProc )
	(
		UInt32			inProtocol,
		PluginColor*	outColor
	);

typedef PeekPlugCallback_( int,	IsDescendentOfProc )
	(
		UInt32			inSubProtocol,
		const UInt32*	inParentArray,
		SInt32			inParentCount,
		UInt32			*outMatchID
	);

// Name table callbacks.
typedef PeekPlugCallback_( int,	LookupNameProc )
	(
		PluginNameTableEntry*	ioEntry
	);

typedef PeekPlugCallback_( int, LookupEntryProc )
	(
		PluginNameTableEntry*	ioEntry
	);

typedef PeekPlugCallback_( int, AddNameEntryProc )
	(
		const PluginNameTableEntry*	inEntry,
		UInt32						inOptions
	);

typedef PeekPlugCallback_( int, InvokeNameEditDialogProc )
	(
		const PluginNameTableEntry*	inEntry
	);

typedef PeekPlugCallback_( int, ResolveAddressProc )
	(
		UInt8*	inAddress,
		UInt16	inAddressType
	);

// Summary callbacks.
typedef PeekPlugCallback_( int, SummaryGetEntryProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const wchar_t*			inLabel,
		const wchar_t*			inGroup,
		UInt32*					outType,
		void**					outData,
		UInt8*					outSource
	);

typedef PeekPlugCallback_( int, SummaryModifyEntryProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const wchar_t*			inLabel,
		const wchar_t*			inGroup,
		UInt32					inType,
		void*					inData
	);

// Packet parsing callbacks.
typedef PeekPlugCallback_( UInt8*,	PacketGetLayerProc )
	(
		UInt8				inLayerType,
		UInt8				inMediaType,
		UInt8				inMediaSubType,
		const PluginPacket*	inPacket,
		const UInt8*		inPacketData,
		UInt16*				ioBytesLeft
	);

typedef PeekPlugCallback_( UInt8*,	PacketGetDataLayerProc	)
	(
		UInt32				inProtoSpec,
		UInt8				inMediaType,
		UInt8				inMediaSubType,
		const PluginPacket*	inPacket,
		const UInt8*		inPacketData,
		UInt16*				ioBytesLeft,
		UInt32*				outSourceProtoSpec
	);

typedef PeekPlugCallback_( UInt8*,	PacketGetHeaderLayerProc )
	(
		UInt32				inLayerType,
		UInt8				inMediaType,
		UInt8				inMediaSubType,
		const PluginPacket*	inPacket,
		const UInt8*		inPacketData,
		UInt16*				ioBytesLeft,
		UInt32*				outSourceProtoSpec
	);

typedef PeekPlugCallback_( int, PacketGetAddressProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt8					inAddressSelector,
		UInt8					inMediaType,
		UInt8					inMediaSubType,
		const PluginPacket*		inPacket,
		const UInt8*			inPacketData,
		UInt64					inPacketNumber,
		UInt8*					outAddress,
		UInt16*					outAddressType
	);

// Preferences callbacks.
typedef PeekPlugCallback_( int, PrefsGetValueProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const wchar_t*			inName,
		const void*				outData,
		UInt32*					ioLength
	);

typedef PeekPlugCallback_( int, PrefsSetValueProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const wchar_t*			inName,
		const void*				inData,
		UInt32					inLength
	);

typedef PeekPlugCallback_( int, PrefsGetPrefsPathProc )
	(
		wchar_t*				outString
	);

// Packet list callbacks.
typedef PeekPlugCallback_( int, InsertPacketProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const PluginPacket*		inPacket,
		const UInt8*			inPacketData,
		UInt8					inMediaType,
		UInt8					inMediaSubType,
		UInt32					inReserved
	);

typedef PeekPlugCallback_( int, SelectPacketsProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt32					inPacketCount,
		UInt64*					inPacketArray,
		UInt32					inFlags
	);

typedef PeekPlugCallback_( int, SelectPacketsExProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const UInt8*			inSrcAddr,
		UInt16					inSrcAddrType,
		const UInt8*			inDestAddr,
		UInt16					inDestAddrType,
		UInt16					inSrcPort,
		UInt16					inDestPort,
		UInt16					inPortTypes,
		bool					inBothDirections,
		UInt32					inFlags
	);

typedef PeekPlugCallback_( int, GetPacketCountProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt32*					outCount
	);

typedef PeekPlugCallback_( int, GetFirstPacketIndexProc	)
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt32*					outIndex
	);

typedef PeekPlugCallback_( int, GetPacketProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt32					inIndex,
		UInt64*					outPacketNumber,
		const PluginPacket**	outPacket,
		const UInt8**			outPacketData
	);

typedef PeekPlugCallback_( int, ClaimPacketStringProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt64					inPacketNumber,
		bool					inExpert
	);

typedef PeekPlugCallback_( int, SetPacketListColumnProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const PluginID*			inColumnID,
		const wchar_t*			inColumnName,
		const wchar_t*			inHeaderText,
		bool					inDefaultVisibility,
		UInt16					inDefaultWidth,
		UInt8					inAlignment
	);

// Decoder callbacks.
typedef PeekPlugCallback_( int, DecodeOpProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		int						inOp,
		UInt32					inValue,
		UInt32					inGlobal,
		UInt8					inStyleValue,
		UInt8					inStyleLabel,
		const wchar_t*			inString,
		const wchar_t*			inSummary
	 );

typedef PeekPlugCallback_( int, AddUserDecodeProc )
	(
		const wchar_t*			inFuncName
	);

// Plug-in UI callbacks.
typedef PeekPlugCallback_( int, AddTabProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const wchar_t*			inTabName,
		const wchar_t*			inWindowClass,
		void**					outTabWnd
	);

typedef PeekPlugCallback_( int, AddTabWithGroupProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		const wchar_t*			inTabName,
		const wchar_t*			inGroupName,
		const wchar_t*			inWindowClass,
		void**					outTabWnd
	);

// Misc. callbacks.
typedef PeekPlugCallback_( int, NotifyProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt64					inTimeStamp,
		UInt8					inSeverity,
		const wchar_t*			inShortString,
		const wchar_t*			inLongString
	);

typedef PeekPlugCallback_( int, SendPluginMessageProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext,
		UInt8*					inParamData,
		UInt32					inParamDataLength
	);

typedef PeekPlugCallback_( int, MakeFilterProc )
	(
		const UInt8*	inSrcAddr,
		UInt16			inSrcAddrType,
		const UInt8*	inDestAddr,
		UInt16			inDestAddrType,
		UInt16			inSrcPort,
		UInt16			inDestPort,
		UInt16			inPortTypes,
		bool			inBothDirections
	);

typedef PeekPlugCallback_( int, GetAppResourcePathProc )
	(
		wchar_t*		outAppResourcePath
	);

typedef PeekPlugCallback_( int, SendPacketProc )
	(
		const UInt8*	inPacketData,
		UInt16			inPacketLength
	);

typedef PeekPlugCallback_( int, SaveContextProc )
	(
		PluginAppContext		inAppContext,
		PluginCaptureContext	inCaptureContext
	);

	// Callback names for importing from Peek.
	#define kExportName_GetProtocolName			_T( "GetProtocolNameCallback" )
	#define kExportName_GetProtocolLongName		_T( "GetProtocolLongNameCallback" )
	#define kExportName_GetProtocolHierName		_T( "GetProtocolHierNameCallback" )
	#define kExportName_GetProtocolParent		_T( "GetProtocolParentCallback" )
	#define kExportName_GetProtocolColor		_T( "GetProtocolColorCallback" )
	#define kExportName_IsDescendentOf			_T( "IsDescendentOfCallback" )
	#define kExportName_LookupName				_T( "LookupNameCallback" )
	#define kExportName_LookupEntry				_T( "LookupEntryCallback" )
	#define kExportName_AddNameEntry			_T( "AddNameEntryCallback" )
	#define kExportName_InvokeNameEditDialog	_T( "InvokeNameEditDialogCallback" )
	#define kExportName_ResolveAddress			_T( "ResolveAddressCallback" )
	#define kExportName_SummaryGetEntry			_T( "SummaryGetEntryCallback" )
	#define kExportName_SummaryModifyEntry		_T( "SummaryModifyEntryCallback" )
	#define kExportName_PacketGetLayer			_T( "PacketGetLayerCallback" )
	#define kExportName_PacketGetDataLayer		_T( "PacketGetDataLayerCallback" )
	#define kExportName_PacketGetHeaderLayer	_T( "PacketGetHeaderLayerCallback" )
	#define kExportName_PacketGetAddress		_T( "PacketGetAddressCallback" )
	#define kExportName_PrefsGetValue			_T( "PrefsGetValueCallback" )
	#define kExportName_PrefsSetValue			_T( "PrefsSetValueCallback" )
	#define kExportName_PrefsGetPrefsPath		_T( "PrefsGetPrefsPathCallback" )
	#define kExportName_InsertPacket			_T( "InsertPacketCallback" )
	#define kExportName_SelectPackets			_T( "SelectPacketsCallback" )
	#define kExportName_SelectPacketsEx			_T( "SelectPacketsExCallback" )
	#define kExportName_GetPacketCount			_T( "GetPacketCountCallback" )
	#define kExportName_GetFirstPacketIndex		_T( "GetFirstPacketIndexCallback" )
	#define kExportName_GetPacket				_T( "GetPacketCallback" )
	#define kExportName_ClaimPacketString		_T( "ClaimPacketStringCallback" )
	#define kExportName_DecodeOp				_T( "DecodeOpCallback" )
	#define kExportName_CallUserDecodeCallback	_T( "CallUserDecodeCallback" )
	#define kExportName_AddUserDecode			_T( "AddUserDecodeCallback" )
	#define kExportName_AddTab					_T( "AddTabCallback" )
	#define kExportName_AddTabWithGroup			_T( "AddTabWithGroupCallback" )
	#define kExportName_Notify					_T( "NotifyCallback" )
	#define kExportName_SendPluginMessage		_T( "SendPluginMessageCallback" )
	#define kExportName_MakeFilter				_T( "MakeFilterCallback" )
	#define kExportName_GetAppResourcePath		_T( "GetAppResourcePathCallback" )
	#define kExportName_SendPacket				_T( "SendPacketCallback" )
	#define kExportName_SetPacketListColumn		_T( "SetPacketListColumnCallback" )
	#define kExportName_SaveContext				_T( "SaveContextCallback" )

	typedef void*	ClientAppDataPtr;


// ============================================================================
//		Message structures.
// ============================================================================

// kPluginMsg_Load parameters.
#define kPluginNameMaxLength				256
#define kPluginLoadErrorMessageMaxLength	1024
typedef struct PluginLoadParam
{
	UInt32				fAPIVersion;
	UInt32				fAppCapabilities;
	wchar_t*			fName;
	PluginID			fID;
	UInt16				fAttributes;
	UInt16				fSupportedActions;
	UInt16				fDefaultActions;
	UInt32				fSupportedCount;
	UInt32*				fSupportedProtoSpecs;
	ClientAppDataPtr	fClientAppData;
	PluginAppContext	fAppContextData;	// Plug-in retains to give back to callbacks
	wchar_t*			fLoadErrorMessage;
	LCID				fLocaleId;
	UInt64				fAttributesEx;		// Attributes = fAttributes | fAttributesEx
} PluginLoadParam;

// kPluginMsg_IsMediaSupported parameters.
typedef struct PluginMediaSupportedParam
{
	UInt8				fMediaType;
	UInt8				fMediaSubType;
} PluginMediaSupportedParam;

// kPluginMsg_ProcessPacket parameters.
typedef struct PluginProcessPacketParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt64				fPacketNumber;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	PluginContext		fContext;
} PluginProcessPacketParam;

// kPluginMsg_ProcessPluginMessage parameters.
typedef struct PluginProcessPluginMessageParam
{
	const UInt8*		fData;
	UInt32				fDataLen;
	UInt32				fResult;
	PluginContext		fContext;
} PluginProcessPluginMessageParam;

// kPluginMsg_DecodePacket parameters.
typedef struct PluginDecodePacketParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt64				fPacketNumber;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	PluginContext		fContext;
} PluginDecodePacketParam;

// kPluginMsg_UserDecode parameters.
#define kPluginFunctionNameMaxLength	256
typedef struct PluginUserDecodeParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt64				fPacketNumber;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	PluginContext		fContext;
	wchar_t*			fFunctionName;
	PluginAppContext	fDecodeContext;
	PluginAppContext	fDecodeState;
} PluginUserDecodeParam;

// kPluginMsg_GetPacketString parameters.
#define kPluginPacketStringMaxLength 1024
typedef struct PluginGetPacketStringParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt64				fPacketNumber;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	wchar_t*			fString;
	PluginColor			fColor;
	PluginContext		fContext;
} PluginGetPacketStringParam;

// kPluginMsg_Apply parameters.
typedef struct PluginApplyParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt64				fPacketNumber;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	UInt16				fCommand;
	UInt32				fPrivateData;
	PluginContext		fContext;
} PluginApplyParam;

// kPluginMsg_Select parameters.
typedef struct PluginSelectParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt64				fPacketNumber;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	UInt16				fCommand;
	UInt32				fPrivateData;
	PluginContext		fContext;
} PluginSelectParam;

// kPluginMsg_Reset parameters.
#define kPluginAdapterIDMaxLength 256
typedef struct PluginResetParam
{
	PluginContext		fContext;
	const wchar_t*		fPluginAdapterID;
} PluginResetParam;

// kPluginMsg_StartCapture parameters.
typedef struct PluginStartCaptureParam
{
	PluginContext	fContext;
} PluginStartCaptureParam;

// kPluginMsg_StopCapture parameters.
typedef struct PluginStopCaptureParam
{
	PluginContext	fContext;
} PluginStopCaptureParam;

// kPluginMsg_GetAdapterList parameters.
typedef struct PluginGetAdapterListParam
{
	const wchar_t*	fOutXMLAdapterList;
	UInt32			fOutXMLStreamLength;
} PluginGetAdapterListParam;

// kPluginMsg_DeleteAdapter parameters.
typedef struct PluginDeleteAdapterParam
{
	const wchar_t*	fInDeleteID;
} PluginDeleteAdapterParam;

// kPluginMsg_AdapterProperties parameters.
typedef struct PluginAdapterPropertiesParam
{
	const wchar_t*	fInAdapterID;
} PluginAdapterPropertiesParam;

// kPluginMsg_SetAdapterAttribs, kPluginMsg_GetAdapterAttribs parameters.
typedef struct PluginAdapterAttribsParam
{
	const wchar_t*	fAdapterID;
	const wchar_t*	fInParamTagStream;
	UInt32			fInParamTagStreamLen;
	const wchar_t*	fOutParamTagStream;
	UInt32			fOutParamTagStreamLen;
} PluginAdapterAttribsParam;

// kPluginMsg_PacketsLoaded parameters.
typedef struct PluginPacketsLoadedParam
{
	PluginContext	fContext;
} PluginPacketsLoadedParam;

// kPluginMsg_Summary parameters.
typedef struct PluginSummaryParam
{
	PluginContext	fContext;
} PluginSummaryParam;

// kPluginMsg_CreateContext parameters.
typedef struct PluginCreateContextParam
{
	PluginContext*			fContextPtr;
	PluginCaptureContext	fCaptureContext;
	PluginContextData		fContextData;
	PluginContextFlags		fContextFlags;
} PluginCreateContextParam;

// kPluginMsg_DisposeContext parameters.
typedef struct PluginDisposeContextParam
{
	PluginContext	fContext;
} PluginDisposeContextParam;

// kPluginMsg_Filter parameters.
typedef struct PluginFilterParam
{
	const PluginPacket*	fPacket;
	const UInt8*		fPacketData;
	UInt8				fMediaType;
	UInt8				fMediaSubType;
	UInt32				fProtoSpecMatched;
	SInt16				fBytesAccepted;
	PluginContext		fContext;
} PluginFilterParam;

// kPluginMsg_NameTableUpdate parameters.
typedef struct PluginNameTableUpdateParam
{
	PluginNameTableEntry*	fEntry;
	UInt16					fCommand;
} PluginNameTableUpdateParam;

// kPluginMsg_CreateNewAdapter parameters.
typedef struct PluginCreateNewAdapterParam
{
	wchar_t*	fOutSelectedAdapterTag;
	UInt16		fInMaxLength;
} PluginCreateNewAdapterParam;

// kPluginMsg_HandleNotify parameters.
#define kPluginNotifyShortMessageMaxLength	 256
#define kPluginNotifyLongMessageMaxLength	4096
typedef struct PluginHandleNotifyParam
{
	UInt64			fTimeStamp;
	UInt8			fSeverity;
	const wchar_t*	fShortMessage;
	const wchar_t*	fLongMessage;
	UInt32			fFlags;
	PluginContext	fContext;
} PluginHandleNotifyParam;

// kPluginMsg_GetTextForPacketListCell parameters.
typedef struct PluginGetTextForPacketListCellParam
{
	PluginContext						fContext;
	const PluginPacketProcessingInfo*	fInPacketProcessingInfo;
	const PluginID*						fInColumnID;
	wchar_t*							fOutCellText;
	UInt32								fInCellTextMaxLength;
} PluginGetTextForPacketListCellParam;

// kPluginMsg_MeasurePacketListCell parameters.
typedef struct PluginMeasurePacketListCellParam
{
	PluginContext						fContext;
	const PluginPacketProcessingInfo*	fInPacketProcessingInfo;
	const PluginID*						fInColumnID;
	TCHAR*								fOutCellText;
	UInt32								fInCellTextMaxLength;
	PluginDrawingHandle					fInDrawingHandle;
	UInt32								fIoDrawingStates;
	PluginSize							fOutCellContentSize;
	bool								fOutDefaultMeasure;
} PluginMeasurePacketListCellParam;

// kPluginMsg_DrawPacketListCell parameters.
typedef struct PluginPacketListCellDrawingInfo
{
	PluginRect		fInBounds;
	PluginRect		fInContentRect;
	PluginColor		fIoForeColor;
	PluginColor		fIoBackColor;
} PluginPacketListCellDrawingInfo;

typedef struct PluginDrawPacketListCellParam
{
	PluginContext						fContext;
	const PluginPacketProcessingInfo*	fInPacketProcessingInfo;
	const PluginID*						fInColumnID;
	wchar_t*							fOutCellText;
	UInt32								fInCellTextMaxLength;
	PluginDrawingHandle					fInDrawingHandle;
	UInt32								fIoDrawingStates;
	PluginPacketListCellDrawingInfo*	fInCellDrawingInfo;
	bool								fOutDefaultDraw;
} PluginDrawPacketListCellParam;

// kPluginMsg_ContextOptions parameters.
typedef struct PluginContextOptionsParam
{
	const wchar_t*	fInOldPrefs;
	UInt32			fInOldPrefsLength;
	wchar_t*		fOutNewPrefs;
	UInt32			fOutNewPrefsLength;
	PluginContext	fContext;
} PluginContextOptionsParam;

// kPluginMsg_SetContextPrefs, kPluginMsg_GetContextPrefs parameters.
typedef struct PluginContextPrefsParam
{
	UInt32			fFlags;
	wchar_t*		fIoPrefs;
	UInt32			fIoPrefsLength;
	PluginContext	fContext;
} PluginContextPrefsParam;

// kPluginMsg_ProcessTime parameters.
typedef struct PluginProcessTimeParam
{
	PluginContext	fContext;
} PluginProcessTimeParam;

typedef union PluginParamBlock
{
	PluginLoadParam						uLoad;						// kPluginMsg_Load
	PluginCreateContextParam			uCreateContext;				// kPluginMsg_CreateContext
	PluginDisposeContextParam			uDisposeContext;			// kPluginMsg_DisposeContext
	PluginProcessPacketParam			uProcessPacket;				// kPluginMsg_ProcessPacket
	PluginGetPacketStringParam			uGetPacketString;			// kPluginMsg_GetPacketString, kPluginMsg_GetPacketAnalysis
	PluginApplyParam					uApply;						// kPluginMsg_Apply
	PluginSelectParam					uSelect;					// kPluginMsg_Select
	PluginResetParam					uReset;						// kPluginMsg_Reset
	PluginStartCaptureParam				uStartCapture;				// kPluginMsg_StartCatpure
	PluginStopCaptureParam				uStopCapture;				// kPluginMsg_StopCapture
	PluginPacketsLoadedParam			uPacketsLoaded;				// kPluginMsg_PacketsLoaded
	PluginSummaryParam					uSummary;					// kPluginMsg_Summary
	PluginFilterParam					uFilter;					// kPluginMsg_Filter
	PluginNameTableUpdateParam			uNameTableUpdate;			// kPluginMsg_NameTableUpdate
	PluginGetAdapterListParam			uAdapterList;				// kPluginMsg_GetAdapterList
	PluginCreateNewAdapterParam 		uCreateAdapter;				// kPluginMsg_CreateNewAdapter
	PluginDeleteAdapterParam			uDeleteAdapter;				// kPluginMsg_DeleteAdapter
	PluginAdapterAttribsParam   		uAdapterAttribs;			// kPluginMsg_SetAdapterAttribs, kPluginMsg_GetAdapterAttribs
	PluginDecodePacketParam				uDecodePacket;				// kPluginMsg_DecodePacket
	PluginAdapterPropertiesParam		uAdapterProperties;			// kPluginMsg_AdapterProperties
	PluginUserDecodeParam				uUserDecode;				// kPluginMsg_UserDecode
	PluginMediaSupportedParam			uMediaSupported;			// kPluginMsg_MediaSupported
	PluginProcessPluginMessageParam		uProcessPluginMessage;		// kPluginMsg_ProcessPluginMessage
    PluginHandleNotifyParam				uHandleNotify;				// kPluginMsg_HandleNotify
	PluginGetTextForPacketListCellParam	uGetTextForPacketListCell;	// kPluginMsg_GetTextForPacketListCell
	PluginMeasurePacketListCellParam	uMeasurePacketListCell;		// kPluginMsg_MeasurePacketListCell
	PluginDrawPacketListCellParam		uDrawPacketListCell;		// kPluginMsg_DrawPacketListCell
	PluginContextOptionsParam			uContextOptions;			// kPluginMsg_ContextOptions
	PluginContextPrefsParam				uContextPrefs;				// kPluginMsg_SetContextPrefs, kPluginMsg_GetContextPrefs
	PluginProcessTimeParam				uProcessTime;				// kPluginMsg_ProcessTime
} PluginParamBlock;


// ============================================================================
//		Return Codes.
// ============================================================================

#define PLUGIN_RESULT_SUCCESS		0
#define PLUGIN_RESULT_ERROR			-1
#define PLUGIN_RESULT_DISABLED		-2

// Return when processing kPluginMsg_Apply and kPluginMsg_Select during the
// kApplyMsg_Start/kSelectMsg_Start phase to indicate whether to continue the
// operation.
#define PLUGIN_START_CONTINUE		0
#define PLUGIN_START_CANCEL			-1

// Return when processing kPluginMsg_Apply, kPluginMsg_Filter, and
// kPluginMsg_Select to indicate whether to include a packet in the operation.
#define PLUGIN_PACKET_ACCEPT		0
#define PLUGIN_PACKET_REJECT		-1

// When processing kPluginMsg_Filter, assign to
// PluginFilterParam.fBytesAccepted to indicate the entire length of the
// packet should be accepted. In other words, no filter slicing should take
// place.
#define PLUGIN_ACCEPT_WHOLE_PACKET	0

// Entry point names to export from the plug-in module.
#define kPluginMainProc				"PlugInMain"
#define kPluginInitProc				"PlugInInitialize"

// The main entry point of a plug-in through which all messages are received.
typedef int (WINAPI *PluginProc)( SInt16 inMessage, PluginParamBlock* ioParams );

#define CallPluginProc(userRoutine, inMessage, ioParams)		\
		((PluginProc)(userRoutine))((inMessage), (ioParams))

#pragma pack(pop)

#ifdef __cplusplus
}	// end extern "C"
}	// end namespace PeekPlugin
#endif

#endif	// end PEEKPLUG_H
