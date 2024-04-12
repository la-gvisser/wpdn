// =============================================================================
//	peekvoipcall.h
// =============================================================================
//	Copyright (c) 2007-2011 WildPackets, Inc. All rights reserved.

#ifndef PEEKVOIPCALL_H
#define PEEKVOIPCALL_H

#include "heunk.h"
#include "hevariant.h"
#include "heid.h"
#include "hecom.h"
#include "PeekPacket.h"
#include "peekcore.h"
#include "peekfilters.h"
#include "peekflow.h"

// Forward declarations.
class ICall;
class ICallEventReceiver;
class ICallManager;
class ICallFlow;
class ICallFlowSignaling;
class ICallFlowMedia;
class ICallFlowMediaControl;
class ICallStats;
struct CallFlowKey;
struct CallFlowMediaControlPacketInfo;
struct CallFlowMediaPacketInfo;
struct CallFlowSignalingPacketInfo;
struct MediaPacketInfo;
struct AddressPortPair;

/// Integer call ID, assigned by ICallManager as it creates a new call.
/// Unique, monotonically incremented, and not rollover-safe.
typedef UInt64 CallID;
static const CallID kCallID_Invalid = 0;

/// Integer flow index within a single call.
/// Starts at 0, goes up to n-1 where n is total number of flows
/// ever associated with this call.
typedef UInt32 CallFlowIndex;
static const CallFlowIndex kCallFlowIndex_Invalid = static_cast<CallFlowIndex>(-1);

// Sync Source ID
typedef UInt32 SourceID;
typedef UInt64* TelHandle;

/// \enum CallStatus
/// \brief The call status - open, closed, etc.
enum CallStatus
{
	callStatus_Unknown,
	callStatus_SettingUp,
	callStatus_Open,
	callStatus_Idle,
	callStatus_Closing,
	callStatus_Closed
};

/// \enum CallRole
/// \brief The role of a node within a call.
/// 
/// Roles are bits, just in case we ever want to support "this node fulfills 
/// multiple roles".
enum CallRole
{
	callRole_None				= 0x0000,	///< Role unknown/not yet assigned.
	callRole_Caller				= 0x0001,	///< Node initiated the call.
	callRole_Callee				= 0x0002,	///< Node received the call.
	callRole_Gateway			= 0x0004	///< Proxy/gateway helped set up the call.
};

/// \enum CallFlowType
/// \brief What kind of flow is this?
/// 
/// Values are bitfields so that you can or them together when requesting 
/// multiple flow types.
enum CallFlowType
{
	callFlowType_None			= 0x0000,
	callFlowType_Signaling		= 0x0001,
	callFlowType_Media			= 0x0002,
	callFlowType_MediaControl	= 0x0004,
	callFlowType_All			= 0xFFFF
};

/// \enum CallFlowStatus
/// \brief Is the flow open or closed?
enum CallFlowStatus
{
	callFlowStatus_Unknown,
	callFlowStatus_Open,
	callFlowStatus_Closed
};

/// \enum CallFlowMediaType
/// \brief What kind of media flow is this?
/// 
/// Values are bitfields so that you can or them together when requesting 
/// multiple media types.
/// Must fit entirely into a UInt16, since MediaPacketInfo stores 
/// CallFlowMediaType as a UInt16.
enum CallFlowMediaType
{
	callFlowMediaType_None		= 0x0000,
	callFlowMediaType_Audio		= 0x0001,	///< Audio portion of a video call, when not intermingled as part of video flow
	callFlowMediaType_Video		= 0x0002,	///< Video portion of a video call
	callFlowMediaType_Voice		= 0x0004,	///< VoIP/Voice over IP flows
	callFlowMediaType_All		= 0x0007
};

/// \enum CallFlowSignalingType
/// \brief What kind of signaling flow is this?
/// 
enum CallFlowSignalingType
{
	callFlowSignalingType_None,
	callFlowSignalingType_SIP,
	callFlowSignalingType_MGCP,
	callFlowSignalingType_SCCP,
	callFlowSignalingType_RTSP,
	callFlowSignalingType_H323,
	callFlowSignalingType_CCMS,
	callFlowSignalingType_H245
};

/// \interface ICall
/// \brief A single VoIP call, including signaling packets/flows, control 
/// flows, and media flows.
#define ICall_IID \
{ 0xCAC773CC, 0xA9DE, 0x4572, {0x81, 0xC9, 0x32, 0x44, 0x9E, 0x59, 0xB4, 0x73} }

class HE_NO_VTABLE ICall : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICall_IID);

	/// Get the call ID. Unique identifier.
	HE_IMETHOD GetCallID(/*out*/ CallID* pCallID) = 0;
	/// Number of ICallFlow objects associated with this ICall.
	HE_IMETHOD GetCallFlowCount(/*out*/ UInt32* pnCount) = 0;
	/// Number of ICallFlow(type) objects associated with this ICall.
	HE_IMETHOD GetCallFlowCountOfType(/*in*/ CallFlowType callFlowype, /*out*/ UInt32* pnCount) = 0;
	/// All the ICallFlow objects associated with his call, index 0..n-1 where 
	/// n is CallFlowCount().
	HE_IMETHOD GetCallFlowByIndex(/*in*/ CallFlowIndex index, /*out*/ ICallFlow** ppCallFlow) = 0;
	/// The ICallFlow object associated with his SSRC.
	HE_IMETHOD GetCallFlowBySSRC(/*in*/	SourceID sourceID, /*out*/ ICallFlow** ppCallFlow) = 0;
	/// Get the call status.
	HE_IMETHOD GetCallStatus(/*out*/ CallStatus* pCallStatus) = 0;
	/// Is this call done? No longer receiving packets.
	/// Closed calls can be recycled to make room for new calls.
	HE_IMETHOD GetClosed(/*out*/ BOOL* pbClosed) = 0;
	/// Is this call closing?
	HE_IMETHOD GetClosing(/*out*/ BOOL* pbClosing) = 0;
	/// When was the last time we saw a packet for this call?
	HE_IMETHOD GetTouchedTime(/*out*/ UInt64* pTouchedTime) = 0;
	/// Who initiated this call?
	HE_IMETHOD GetCaller(/*out*/ AddressPortPair* pCaller) = 0;
	/// Who received this call?
	HE_IMETHOD GetCallee(/*out*/ AddressPortPair* pCallee) = 0;
	/// Who helped set up this call?
	HE_IMETHOD GetGatekeeper(/*out*/ AddressPortPair* pGatekeeper) = 0;
	/// Where to expect auxiliary signaling flow if any?
	HE_IMETHOD GetAuxSignalingEndpoint(/*out*/ AddressPortPair* pAddrPort) = 0;
	/// Get the address for a particular role (shorthand for a switch across the
	/// above 3 XXXAddress properties.
	HE_IMETHOD GetRoleAddress(/*in*/ CallRole role, /*out*/ AddressPortPair* pAddress) = 0;
	/// Lookup the codec name by supplied value and media spec.
	HE_IMETHOD GetLookupCodec(/*in*/ IPacket* pPacket, /*out*/ Helium::HEBSTR* pbstrCodec,
		/*out*/ UInt32* pSpeed) = 0;
	HE_IMETHOD GetSetupDuration(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetPostDialDelay(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetSignalingType(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetFrom(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetFromAbbrev(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetTo(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetToAbbrev(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetCallIDString(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetContext(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetEndCause(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetStartTime(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetEndTime(/*out*/ UInt64* pVal) = 0;
	/// Calculate the lowest MOS score for all media flows.
	HE_IMETHOD GetMOSLow(/*out*/ UInt64* pVal) = 0;
	/// Create and return a new ICall object suitable for nothing more
	/// than holding a list of ICallMedia and ICallMediaControl objects
	/// for stat calculations. This ICall clone cannot itself process packets.
	/// 
	/// Used internally to calculate stats for 
	/// ICallManager::CreateMediaPacketInfoArray().
	HE_IMETHOD CreateStatCalculatorClone(/*in*/ SInt32 nTelchemyInterfaceIndex,
		/*out*/ ICall** ppVal) = 0;
	/// Save the media keys for later retrieval
	HE_IMETHOD GetMediaKey(/*in*/ UInt32 index, /*out*/ UInt64* pKey) = 0;
	/// Packet closes call in half-closed state, pbNewCall (optional) indicates 
	/// packet is for a new call.
	HE_IMETHOD PacketIsForNewCall(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbNewCall) = 0;
	/// Given a Video Media flow, try an find a matching Audio/Voice flow to make
	/// this a Multimedia flow.
	HE_IMETHOD FindMatchingAudioForVideo(/*in*/ ICallFlow* pVideoFlow,
		/*out*/ TelHandle* pAudioHandle, /*out*/ BOOL* pbFound) = 0;
	/// Weak back-pointer to creating/owning ICallManager.
	HE_IMETHOD GetCallManager(/*out*/ ICallManager** ppCallManager) = 0;
};

#include "hepushpack.h"
struct CallSummaryStats
{
	UInt32				TotalCalls;				///< Total number of calls seen including open and closed.
	UInt32				CurrentCalls;			///< Number of calls being tracked (open and closed).
	UInt32				RecycledCalls;			///< Number of calls that have been recycled.
	UInt32				OpenCalls;				///< Number of active calls.
	UInt64				MaxCallsTime;			///< Time when the maximum number of open calls was reached (PEEK time format).
	UInt64				TotalPacketsReceived;	///< Total packets received.
	UInt64				TotalPacketsLost;		///< Total packets lost.
	UInt64				TotalPacketsDiscarded;	///< Total packets discarded.

	// Voice
	UInt64				VoicePacketsReceived;	///< Voice packets received.
	UInt64				VoicePacketsLost;		///< Voice packets lost.
	UInt64				VoicePacketsDiscarded;	///< Voice packets discarded.
	UInt32				VoiceScoreElements;		///< Number of flows the voice stats are based on.
	UInt16				AvgMOS_LQ;				///< Average MOS LQ (scaled by 100; zero is invalid).
	UInt16				MinMOS_LQ;				///< Minimum MOS LQ (scaled by 100; zero is invalid).
	UInt16				MaxMOS_LQ;				///< Maximum MOS LQ (scaled by 100; zero is invalid).
	UInt16				AvgMOS_CQ;				///< Average MOS CQ (scaled by 100; zero is invalid).
	UInt16				MinMOS_CQ;				///< Minimum MOS CQ (scaled by 100; zero is invalid).
	UInt16				MaxMOS_CQ;				///< Maximum MOS CQ (scaled by 100; zero is invalid).
	UInt16				AvgMOS_PQ;				///< Average MOS PQ (scaled by 100; zero is invalid).
	UInt16				MinMOS_PQ;				///< Minimum MOS PQ (scaled by 100; zero is invalid).
	UInt16				MaxMOS_PQ;				///< Maximum MOS PQ (scaled by 100; zero is invalid).
	UInt8				AvgRF_Listen;			///< Average R-Factor Listen (0-100).
	UInt8				MinRF_Listen;			///< Minimum R-Factor Listen (0-100).
	UInt8				MaxRF_Listen;			///< Maximum R-Factor Listen (0-100).
	UInt8				AvgRF_Conversational;	///< Average R-Factor Conversational (0-100).
	UInt8				MinRF_Conversational;	///< Minimum R-Factor Conversational (0-100).
	UInt8				MaxRF_Conversational;	///< Maximum R-Factor Conversational (0-100).
	UInt8				AvgRF_G107;				///< Average R-Factor G107 (0-100).
	UInt8				MinRF_G107;				///< Minimum R-Factor G107 (0-100).
	UInt8				MaxRF_G107;				///< Maximum R-Factor G107 (0-100).
	UInt8				Unused1;
	
	// Audio
	UInt64				AudioPacketsReceived;	///< Audio packets received.
	UInt64				AudioPacketsLost;		///< Audio packets lost.
	UInt64				AudioPacketsDiscarded;	///< Audio packets discarded.
	UInt32				AudioScoreElements;		///< Number of flows the audio stats are based on.
	UInt16				AvgMOS_A;				///< Average Audio Quality MOS (scaled by 100; zero is invalid).
	UInt16				MinMOS_A;				///< Minimum Audio Quality MOS (scaled by 100; zero is invalid).
	UInt16				MaxMOS_A;				///< Maximum Audio Quality MOS (scaled by 100; zero is invalid).
	UInt8				AvgVSAQ;				///< Average Video Service Audio Quality (scaled by 2).
	UInt8				MinVSAQ;				///< Minimum Video Service Audio Quality (scaled by 2).
	UInt8				MaxVSAQ;				///< Maximum Video Service Audio Quality (scaled by 2).
	UInt8				Unused2;

	// Video
	UInt64				VideoPacketsReceived;	///< Video packets received.
	UInt64				VideoPacketsLost;		///< Video packets lost.
	UInt64				VideoPacketsDiscarded;	///< Video packets discarded.
	UInt32				VideoScoreElements;		///< Number of flows the video stats are based on.
	UInt16				AvgMOS_AV;				///< Average Multimedia Quality MOS (scaled by 100; zero is invalid).
	UInt16				MinMOS_AV;				///< Minimum Multimedia Quality MOS (scaled by 100; zero is invalid).
	UInt16				MaxMOS_AV;				///< Maximum Multimedia Quality MOS (scaled by 100; zero is invalid).
	UInt16				AvgMOS_V;				///< Average Picture Quality MOS (scaled by 100; zero is invalid).
	UInt16				MinMOS_V;				///< Minimum Picture Quality MOS (scaled by 100; zero is invalid).
	UInt16				MaxMOS_V;				///< Maximum Picture Quality MOS (scaled by 100; zero is invalid).
	UInt8				AvgVSPQ;				///< Average Video Service Picture Quality (scaled by 2).
	UInt8				MinVSPQ;				///< Minimum Video Service Picture Quality (scaled by 2).
	UInt8				MaxVSPQ;				///< Maximum Video Service Picture Quality (scaled by 2).
	UInt8				AvgVSMQ;				///< Average Video Service Multimedia Quality (scaled by 2).
	UInt8				MinVSMQ;				///< Minimum Video Service Multimedia Quality (scaled by 2).
	UInt8				MaxVSMQ;				///< Maximum Video Service Multimedia Quality (scaled by 2).
	UInt8				AvgVSTQ;				///< Average Video Service Transmission Quality (scaled by 2).
	UInt8				MinVSTQ;				///< Minimum Video Service Transmission Quality (scaled by 2).
	UInt8				MaxVSTQ;				///< Maximum Video Service Transmission Quality (scaled by 2).
	UInt8				Unused3;
	UInt16				Unused4;
	UInt32				Unused5;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \interface ICallManager
/// \brief Keeper of all ICall objects.
///
/// Processes packets to detect VoIP calls, create ICall objects for them, update them.
/// \see ICallEventReceiver and PeekFlow.IEventReceiverList for event receiver interfaces.
#define ICallManager_IID \
{ 0x1E15FE94, 0xADA0, 0x4627, {0xBD, 0x1D, 0xD5, 0xCA, 0x00, 0x5F, 0x19, 0x6A} }

class HE_NO_VTABLE ICallManager : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallManager_IID);

	/// Sharing our IFlowTracker 
	///
	/// If you have a capture-wide IFlowTracker, let us know so that we can share it.
	/// Otherwise we'll create our own (which you can use and share).
	/// Get causes ICallManager to lazily create its own PeekFlow.FlowTracker if one has
	/// not yet been assigned via SetFlowTracker(). Set lets us use the same IFlowTracker
	/// used by other capture objects (for example, the Expert). If you're going to call Set,
	/// do so before you start processing packets. Otherwise bad things will happen when the
	/// old FlowTracker's FlowID space doesn't match the new FlowTracker's FlowID space.
	HE_IMETHOD GetFlowTracker(/*out*/ IFlowTracker** ppFlowTracker) = 0;
	HE_IMETHOD SetFlowTracker(/*in*/ IFlowTracker* pFlowTracker) = 0;
	/// Access to the first IPacketBuffer that came through as the buffer for 
	/// the first packet through ProcessPacket().
	HE_IMETHOD GetPacketBuffer(/*out*/ IPacketBuffer** ppPacketBuffer) = 0;
	/// Forget all calls, but retain max call count, event listeners, flow tracker.
	HE_IMETHOD Reset() = 0;
	/// If we have such a call, return HE_S_OK and output the call. If not, 
	// return HE_S_FALSE and output NULL.
	HE_IMETHOD GetCall(/*in*/ CallID callID, /*out*/ ICall** ppCall) = 0;
	/// Shortcut for GetCall() + ICall::CallFlowByIndex().
	HE_IMETHOD GetCallFlow(/*in*/ CallID callID, /*in*/ CallFlowIndex callFlowIndex, 
		/*out*/ ICallFlow** ppCallFlow) = 0;
	/// Access to ALL our calls, for your iterating pleasure.
	/// Outputs an array of CallIDs.
	/// Caller must HeTaskMemFree() the array.
	HE_IMETHOD GetAllCallIDs(/*out*/ UInt32* pnCount,
		/*out*/ CallID** payCallIDCallerOwnsAndReleases) = 0;
	/// Access to ALL media flows, for your iterating pleasure.
	/// Outputs an array of <call id, flow index> key structures.
	/// Caller must HeTaskMemFree() the array.
	/// \deprecated, use GetAllFlowKeys()
	HE_IMETHOD GetAllCallFlowMediaKeys(/*out*/ UInt32* pnCount,
		/*out*/ CallFlowKey** payCallFlowKey) = 0;
	/// Access to ALL media flows, for your iterating pleasure.
	/// Outputs an array of <call id, flow index> key structures.
	/// Caller must HeTaskMemFree() the array.
	/// \param inWantCallFlowType: specify what kind of flows you want, pass 
	///			callFlowType_None or callFlowType_All to get all flows.
	HE_IMETHOD GetAllCallFlowKeys(/*in*/ UInt32 nWantCallFlowType,
		/*out*/ UInt32* pCount, /*out*/ CallFlowKey** payCallFlowKey) = 0;
	/// How many calls are we permitted to manage before recycling old, closed, 
	/// calls to make room for new calls?
	HE_IMETHOD GetMaxCallCount(/*out*/ UInt32* pnMaxCallCount) = 0;
	HE_IMETHOD SetMaxCallCount(/*in*/ UInt32 nMaxCallCount) = 0;
	/// How long must an open call be idle before it can be recycled to make 
	/// room for new calls?
	HE_IMETHOD GetTimeout(/*out*/ UInt64* pnTimeout) = 0;
	HE_IMETHOD SetTimeout(/*in*/ UInt64 nTimeout) = 0;
	/// How many total calls have we seen?
	HE_IMETHOD GetTotalCallCount(/*out*/ UInt64* pCallCount) = 0;
	/// How many calls do we know about?
	HE_IMETHOD GetCallCount(/*out*/ UInt32* pCallCount) = 0;
	/// Get the count of media flows in all current calls.
	HE_IMETHOD GetMediaFlowCount(/*out*/ UInt32* pMediaFlowCount) = 0;
	/// Map a single packet to the VoIP call to which it belongs.
	/// \return HE_S_OK if call found, HE_S_FALSE if not (leave output parameters untouched).
	HE_IMETHOD FindCallIDForPacket(/*in*/ IPacket* pPacket,
		/*out*/ CallID* outCallID, /*out*/ CallFlowIndex* pCallFlowIndex) = 0;	///< optional, NULL ok
	/// Get a list of regions supported by this call manager's analysis module 
	/// when calculating quality scores? Regions are identified by a UInt32. 
	/// \see RegionName() to get user-visible region name like "North America".
	/// \see Region() to get/set region.
	HE_IMETHOD GetAvailableRegionArray(/*out*/ UInt32* pRegionCount,
		/*out*/ UInt32** payRegionCallerOwnsAndReleases) = 0;
	/// User-visible region name. Hard coded to US English.
	HE_IMETHOD GetRegionName(/*in*/ UInt32 nRegion, /*out*/ Helium::HEBSTR* pVal) = 0;
	/// What region is assumed when analyzing quality scores?
	HE_IMETHOD GetRegion(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetRegion(/*in*/ UInt32 nVal) = 0;
	/// Maximum number of simultaneous CallManager instances permitted.
	/// Can only change before first CallManager processes a packet: once it 
	/// starts analysis, change does no good.
	HE_IMETHOD GetMaxCaptureCount(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetMaxCaptureCount(/*in*/ UInt32 nVal) = 0;
	/// Find a media flow with specified IP/port belonging to a call with no signaling.
	/// Attach the media flow to a given call and remove original empty call.
	HE_IMETHOD AttachMediaFlow(/*in*/ ICall* pCall, /*in*/ const TMediaSpec* IP, /*in*/ UInt16 port) = 0;
	/// Analyze the given RTP and RTCP call flows, and produce a HUGE array of
	/// MediaPacketInfo objects, one for each RTP or RTCP packet in the given call flows.
	/// This analysis can take a long time and memory.
	///
	/// Individual packets with analysis errors are skipped over and not included in
	/// outayMediaPacketInfoCallerReleases, rather than abort analysis.
	/// Bigger errors like "no such call flow" result in HE_E_FAIL and untouched MediaPacketInfo array.
	///
	/// \param inayCallFlowKey
	/// 	array of call flows for which you want analysis. keys for nonexistent 
	///		or signaling call flows are skipped over. Cannot be NULL.
	/// \param inCallFlowKeyCount	size of inayCallFlowKey. Must be greater than zero.
	/// \param outayMediaPacketInfoCallerReleases
	///		output as a pointer to a buffer of MediaPacketInfo elements, one for
	///		each packet in any of the flows of inayCallFlowKey.
	///		Caller must release each individual element with MediaPacketInfo_Release(),
	///		and release entire array with CoTaskMemFree().
	///		Array is ordered by call flow key, so that each call flow's packets are
	///		contiguous. Within a call flow key's packets, elements are ordered by
	///		packet number. Use outCallFlowBegin[i] to find the start of inayCallFlowKey[i]'s
	///		packets, and outCallFlowBegin[i+1] to find the end. Cannot be NULL.
	/// \param outMediaPacketInfoCount	size of outayMediaPacketInfoCallerReleases. Cannot be NULL.
	/// \param outCallFlowBegin
	///		output as pointers into outayMediaPacketInfoCallerReleases, one pointer for each
	///		call flow in inayCallFlowKey. Each pointer points to the first MediaPacketInfo
	///		element for that call flow key. Last element of output array points one past
	///		the last valid element in outayMediaPacketInfoCallerReleases (use as end() pointer).
	///		Cannot be NULL. Array is created by caller, no CreateMediaPacketInfoArray().
	///		Array Must be inCallFlowKeyCount+1 elements long.
	HE_IMETHOD CreateMediaPacketInfoArray(/*in*/ CallFlowKey* ayCallFlowKey,
		/*in*/ UInt32 nCallFlowKeyCount, /*out*/ MediaPacketInfo** payMediaPacketInfoCallerReleases,
		/*out*/ UInt32* pMediaPacketInfoCount, /*in*/ MediaPacketInfo** ppCallFlowBegin) = 0;
	/// Combining calls
	/// Calls can be combined together under certain circumstances, use 
	/// CallsCanBeCombined() to determine this. Once they are combined using 
	/// CombineCalls() the old calls are then destroyed.
	///
	/// \param nCallCount
	///		the count of calls to be expected in the payCalls array
	/// \param payCalls
	///		caller allocated array of CallIDs
	HE_IMETHOD CallsCanBeCombined(/*in*/ UInt32 nCallCount, /*in*/ const CallID* payCalls) = 0;
	HE_IMETHOD CombineCalls(/*in*/ UInt32 nCallCount, /*in*/ const CallID* payCalls) = 0;
	HE_IMETHOD GetActivationBits(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD NotifyOfPacketBufferRelease() = 0;
	/// Get call summary statistics.
	HE_IMETHOD GetCallSummaryStats(/*out*/ CallSummaryStats* pStats) = 0;
	/// Enable/disable call statistics.
	HE_IMETHOD EnableCallStats(/*in*/ BOOL bEnable) = 0;
	/// Get call statistics.
	HE_IMETHOD GetCallStats(/*out*/ ICallStats** ppCallStats) = 0;
};

/// \interface ICallManagerEvents
/// \brief How you learn about new and closed calls.
#define ICallManagerEvents_IID \
{ 0xA8B2FEFF, 0x9EC0, 0x42C6, {0xAE, 0xD1, 0x1D, 0x37, 0xE0, 0x82, 0x4A, 0xE0} }

class HE_NO_VTABLE ICallManagerEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallManagerEvents_IID);

	/// Analysis stopped.
	HE_IMETHOD OnAnalysisStopped() = 0;
	/// The max call limit has been exceeded.
	HE_IMETHOD OnOverMaxCallLimit(/*in*/ UInt64 time) = 0;
	/// The number of calls has fallen below the (previously exceeded) max call limit.
	HE_IMETHOD OnUnderMaxCallLimit(/*in*/ UInt64 time) = 0;
};

/// \interface ICallManagerCallEvents
/// \brief How you learn about new and closed calls.
#define ICallManagerCallEvents_IID \
{ 0xFFE40B89, 0x70D6, 0x473D, {0x9F, 0x1F, 0x49, 0x2E, 0x88, 0x2E, 0xAD, 0x03} }

class HE_NO_VTABLE ICallManagerCallEvents : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallManagerCallEvents_IID);

	/// A new call has been opened.
	HE_IMETHOD OnCallOpened(/*in*/ ICall* pCall) = 0;
	/// A call was closed.
	HE_IMETHOD OnCallClosed(/*in*/ ICall* pCall) = 0;
};

/// \interface ICallFlow
/// 
/// A single "flow" associated with a call. For media and media control flows, this
/// flow will correspond to a PeekFlow.IFlow flow. For signaling "flows", these are
/// usually random UDP packets scattered across multiple PeekFlow.IFlows.
/// 
/// All ICallFlow objects also implement exactly one of ICallFlowSignaling, 
/// ICallFlowMedia, or ICallFlowMediaControl.
#define ICallFlow_IID \
{ 0x9A62E032, 0x0F6E, 0x4ec9, {0xA6, 0xDA, 0x0D, 0x83, 0x69, 0x18, 0x5A, 0xF1} }

class HE_NO_VTABLE ICallFlow : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallFlow_IID);

	HE_IMETHOD Initialize(/*in*/ UInt32 nFlowID, /*in*/ UInt16 nPSID,
		/*in*/ ICall* pICall, /*in*/ ICallFlow * pICallFlowParent, /*in*/ int nTelchemyIndex) = 0;

	/// Set an owner of this call flow
	HE_IMETHOD SetOwner(/*in*/ ICall* pICall) = 0;

	/// Does pPacket belong to this VoIP call? (regardless of protocol)
	HE_IMETHOD PacketMatchesCall(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbMatches) = 0;
	/// Does pPacket belong to this VoIP call flow? (protocol checked)
	HE_IMETHOD PacketMatchesFlow(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbMatches) = 0;
	/// Does pPacket source address match the Callee of this VoIP call flow?
	HE_IMETHOD PacketSrcMatchesCallee(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbMatches) = 0;
	/// Does pPacket source address match the Caller of this VoIP call flow?
	HE_IMETHOD PacketSrcMatchesCaller(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbMatches) = 0;
	HE_IMETHOD GetFlowID(/*out*/ UInt32* pFlowID) = 0;
	HE_IMETHOD GetProtoSpec(/*out*/ UInt32* pProtoSpec) = 0;
	HE_IMETHOD GetCallFlowType(/*out*/ UInt32* pCallFlowType) = 0;
	HE_IMETHOD GetCallFlowStatus(/*out*/ CallFlowStatus* pStatus) = 0;
	HE_IMETHOD GetSourceAddress(/*out*/ TMediaSpec* pAddress) = 0;
	HE_IMETHOD GetSourcePort(/*out*/ UInt16* pPort) = 0;
	HE_IMETHOD GetDestAddress(/*out*/ TMediaSpec* pAddress) = 0;
	HE_IMETHOD GetDestPort(/*out*/ UInt16* pPort) = 0;
	HE_IMETHOD GetTotalPackets(/*out*/ UInt64* pullTotalPackets) = 0;
	HE_IMETHOD GetTotalBytes(/*out*/ UInt64* pullTotalBytes) = 0;
	HE_IMETHOD GetStartTime(/*out*/ UInt64* pStartTime) = 0;
	HE_IMETHOD GetEndTime(/*out*/ UInt64* pEndTime) = 0;
	HE_IMETHOD GetStatus(/*out*/ Helium::HEBSTR* pbstrStatus) = 0;
	/// If this call flow's list packets can be approximated with an IPacketFilter
	/// object, create one and output it. Otherwise return HE_E_FAIL.
	HE_IMETHOD CreateFilter(/*out*/ IPacketFilter** ppFilter) = 0;
	HE_IMETHOD GetFirstPacketNumber(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetLastPacketNumber(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD NotifyOfPacketBufferRelease() = 0;
	HE_IMETHOD GetSource(/*out*/ AddressPortPair* pSource) = 0;
	HE_IMETHOD GetDest(/*out*/ AddressPortPair* pDest) = 0;
};

/// \interface ICallFlowSignaling
/// 
/// A single "flow" of one or more signaling packets. Usually these packets
/// come from random UDP ports, so they correspond to more than one
/// port pair-based PeekFlow.IFlow.
#define ICallFlowSignaling_IID \
{ 0xC746E3A2, 0xF58A, 0x46C2, {0xB4, 0xE9, 0x3D, 0x79, 0x2F, 0xB4, 0x39, 0x1A} }

class HE_NO_VTABLE ICallFlowSignaling : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallFlowSignaling_IID);

	/// Get signaling type as a string.
	HE_IMETHOD GetSignalingType(/*out*/ CallFlowSignalingType* pVal ) = 0;
	/// Get signaling type as a string.
	HE_IMETHOD GetSignalingName(/*out*/ Helium::HEBSTR* pVal ) = 0;
	/// Who initiated this call?
	HE_IMETHOD GetCaller(/*out*/ AddressPortPair* pCaller) = 0;
	/// Who received this call?
	HE_IMETHOD GetCallee(/*out*/ AddressPortPair* pCallee) = 0;
	/// Who helped set up this call?
	HE_IMETHOD GetGatekeeper(/*out*/ AddressPortPair* pGatekeeper) = 0;
	/// Where to expect auxiliary signaling flow if any?
	HE_IMETHOD GetAuxSignalingEndpoint(/*out*/ AddressPortPair* pAddrPort) = 0;
	/// Where to expect media control flow?
	HE_IMETHOD GetMediaEndpointsList(/*out*/ UInt32 * pnCount, 
		/*out*/ AddressPortPair ** payAddrPort) = 0;
	// Dynamic RTP maps
	HE_IMETHOD GetRTPCodec(/*in*/ IPacket* pPacket,
		/*out*/ Helium::HEBSTR* pbstrCodec, /*out*/ UInt32* pSpeed) = 0;
	/// Is this call closing?
	HE_IMETHOD GetClosing(/*out*/ BOOL* pbClosing) = 0;
	HE_IMETHOD GetSetupDuration(/*out*/ UInt64 * pVal) = 0;
	HE_IMETHOD GetPostDialDelay(/*out*/ UInt64 * pVal) = 0;
	HE_IMETHOD GetFrom(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetFromAbbrev(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetTo(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetToAbbrev(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetCallIDString(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetContext(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetEndCause(/*out*/ Helium::HEBSTR* pVal) = 0;
	/// Output an array of info elements, one per packet in this signaling 
	/// flow, ordered by packet number.
	HE_IMETHOD CreateSignalingPacketInfo(/*out*/ UInt32* pnCount,
		/*out*/ CallFlowSignalingPacketInfo** payInfo) = 0;
	/// Receive the first Media packet (for timing).
	HE_IMETHOD SetFirstMediaPacket(/*in*/ IPacket* pPacket) = 0;
	/// Receive the first Media Control packet (for caller/callee identity).
	HE_IMETHOD SetFirstMediaControlPacket(/*in*/ IPacket* pPacket) = 0;
	/// Packet closes call in half-closed state, pbNewCall (optional) indicates 
	/// packet is for a new call.
	HE_IMETHOD PacketIsForNewCall(/*in*/ IPacket* pPacket, /*out*/ BOOL* pbNewCall) = 0;
	/// Returns true if the call start has been detected within signaling
	HE_IMETHOD IsCallStarted( /*out*/ BOOL * pbCallStarted ) = 0;
	/// Returns true if this flow is an auxiliary signaling flow
	HE_IMETHOD IsAuxSignaling( /*out*/ BOOL * bAuxSignaling ) = 0;
	/// Set the actual caller address as determined by other flow (more authoritative).
	HE_IMETHOD SetCallerEndpoint(/*in*/ AddressPortPair* pCaller) = 0;
	/// Set the actual callee address as determined by other flow (more authoritative).
	HE_IMETHOD SetCalleeEndpoint(/*in*/ AddressPortPair* pCallee) = 0;
};

/// \interface ICallFlowMedia
/// \brief A single media flow. This is usually a one-way UDP flow of RTP packets.
#define ICallFlowMedia_IID \
{ 0xF7F7DBB5, 0x80F6, 0x406F, {0xAA, 0x12, 0x15, 0x3E, 0x19, 0x50, 0x44, 0xB9} }

class HE_NO_VTABLE ICallFlowMedia : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallFlowMedia_IID);

	/// Get the SSRC for this media
	HE_IMETHOD GetSSRC(/*out*/ SourceID * pSourceID) = 0;
	/// Get the Telchemy handle for this media (Does not find CLOSED streams!)
	HE_IMETHOD GetMediaStreamBySSRC(/*in*/ SourceID inSourceID, /*out*/ TelHandle * outVoiceStream) = 0;
	/// Tell the media to do periodic processing
	HE_IMETHOD RequestProcessing() = 0;
	/// Is this a VoIP flow? Video? Audio-portion-of-video?
	HE_IMETHOD GetMediaType(/*out*/ CallFlowMediaType* pType) = 0;
	/// Return an opaque block of quality statistics for a VoIP flow. 
	/// \param cbStatBuffer input is size of inStatBuffer, output is actual 
	/// size required, even if input size was not large enough to hold all statistics.
	/// \param inStatBuffer opaque block of statistics.
	/// \return HE_E_INVALID_ARG if cbStatBuffer too small. HE_E_FAIL if 
	/// MediaType is not callFlowMediaType_Voice.
	/// \internal only useful for WildPackets engineers who can understand 
	/// the stat buffer structure.
	HE_IMETHOD GetVoiceStats(/*in, out*/ UInt32* pcbStatBuffer, /*out*/ UInt8* pStatBuffer) = 0;
	/// Return an opaque block of quality statistics for an audio-portion-of-a-video flow. 
	/// \param cbStatBuffer input is size of inStatBuffer, output is actual 
	/// size required, even if input size was not large enough to hold all statistics.
	/// \param inStatBuffer opaque block of statistics.
	/// \return HE_E_INVALID_ARG if cbStatBuffer too small. HE_E_FAIL if 
	/// MediaType is not callFlowMediaType_Audio.
	/// \internal only useful for WildPackets engineers who can understand 
	/// the stat buffer structure.
	HE_IMETHOD GetAudioStats(/*in, out*/ UInt32* pcbStatBuffer, /*out*/ UInt8* pStatBuffer) = 0;
	/// Return an opaque block of quality statistics for a video flow. 
	/// \param cbStatBuffer input is size of inStatBuffer, output is actual 
	/// size required, even if input size was not large enough to hold all statistics.
	/// \param inStatBuffer opaque block of statistics.
	/// \return HE_E_INVALID_ARG if cbStatBuffer too small. HE_E_FAIL if 
	/// MediaType is not callFlowMediaType_Voice.
	/// \internal only useful for WildPackets engineers who can understand 
	/// the stat buffer structure.
	HE_IMETHOD GetVideoStats(/*in, out*/ UInt32* pcbStatBuffer, /*out*/ UInt8* pStatBuffer) = 0;
	/// Indicates whether Telchemy is updating the stats for a open call.
	HE_IMETHOD StatsAreUpdating(/*out*/ BOOL* pbUpdating) = 0;
	// Notify the media that the call is closed.
	HE_IMETHOD SetStreamClosed() = 0;
	/// Number of audio or video "frames". For audio, this is usually some 
	/// codec-defined multiple of packet count (such as G.711's 20 frames per RTP 
	/// packet.
	HE_IMETHOD GetMediaFrameCount(/*out*/ UInt64* pVal) = 0;
	/// Get the codec type. VQMON vocoder type.
	HE_IMETHOD GetCodec(/*out*/ UInt32* pVal) = 0;
	/// Get the code speed, if known from the signaling flow.
	HE_IMETHOD GetCodecSpeed(/*out*/ UInt32* pVal) = 0;
	/// Get the user-visible codec name like "G711".
	HE_IMETHOD GetCodecName(/*out*/ Helium::HEBSTR* pVal) = 0;
	/// Indicates whether the codec has been identified.
	HE_IMETHOD IsCodecKnown(/*out*/ BOOL* pbVal) = 0;
	/// Create and return a new ICallFlowMedia object suitable for processing 
	/// packets and generating voice stats, but not part of the usual 
	/// ICallManager's list of call flow objects. This object does not consume a 
	/// slot on the ICallManager's limited list of MaxCallCount calls, is not part 
	/// of a call, and has no effect on this original ICallFlowMedia or its owning 
	/// ICallManager.
	/// 
	/// Used internally to calculate stats for 
	/// ICallManager::CreateMediaPacketInfoArray().
	/// 
	/// Call the returned object's ProcessPacketWithFlowID()
	/// to add packets to its data, call ProcessTime() to update stats, and
	/// GetVoiceStats() to get the actual stats.
	HE_IMETHOD CreateStatCalculatorClone(/*in*/ ICall* pStatsCalculatorCall,
		/*out*/ ICallFlowMedia** ppVal) = 0;
};

/// \interface ICallFlowMediaControl
/// \brief A single media control flow. This is usually an RTCP flow.
#define ICallFlowMediaControl_IID \
{ 0x1DC2AA5D, 0x86FC, 0x4A84, {0xB5, 0x2D, 0x23, 0x81, 0xF9, 0xB1, 0x7F, 0x40} }

class HE_NO_VTABLE ICallFlowMediaControl : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallFlowMediaControl_IID);

	/// Create and return a new ICallFlowMediaControl object suitable for processing 
	/// packets and generating voice stats, but not part of the usual 
	/// ICallManager's list of call flow objects. This object does not consume a 
	/// slot on the ICallManager's limited list of MaxCallCount calls, is not part 
	/// of a call, and has no effect on this original ICallFlowMedia or its owning 
	/// ICallManager.
	/// 
	/// Used internally to calculate stats for 
	/// ICallManager::CreateMediaPacketInfoArray().
	/// 
	/// Call the returned object's ProcessPacketWithFlowID()
	/// to add packets to its data, call ProcessTime() to update stats, and
	/// GetVoiceStats() to get the actual stats.
	HE_IMETHOD CreateStatCalculatorClone(/*in*/ ICall* pStatsCalculatorCall,
		/*out*/ ICallFlowMediaControl** ppVal) = 0;
	/// Get the SSRC for this media control (source).
	HE_IMETHOD GetSSRC(/*out*/ SourceID* pSourceID) = 0;
	/// Get the source CNAME string (from SDES) if available.
	HE_IMETHOD GetSourceCName(/*out*/ Helium::HEBSTR* pVal) = 0;
	/// Get the source NAME string (from SDES) if available.
	HE_IMETHOD GetSourcePhone(/*out*/ Helium::HEBSTR* pVal) = 0;
};

/// \interface IVoIPPacket
/// \brief Representation of packet as VoIP protocol.
#define IVoIPPacket_IID \
{ 0x4329B396, 0xBFC0, 0x4923, {0xB8, 0xDF, 0x35, 0x7A, 0xDA, 0x57, 0x2E, 0x8F} }

class HE_NO_VTABLE IVoIPPacket : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IVoIPPacket_IID);

	HE_IMETHOD Initialize(/*in*/ IPacket* pPacket) = 0;
};

/// \interface IMediaFileCreator
/// \brief An object that can render itself as a media file.
#define IMediaFileCreator_IID \
{ 0xA5D447EA, 0x029B, 0x40A0, {0xAB, 0x98, 0x6D, 0x7F, 0x50, 0x49, 0xDB, 0xD2} }
class HE_NO_VTABLE IMediaFileCreator : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMediaFileCreator_IID);

	/// Can we create a media file from our current data?
	/// Returns HE_S_OK if possible, HE_S_FALSE if not.
	/// If returning HE_S_FALSE, pbstrError (if not NULL) is filled 
	/// with a description like "codec 'foo' unsupported" or "extension
	/// 'bar' not supported"
	HE_IMETHOD CanCreateMediaFile(/*in*/ Helium::HEBSTR bstrFileExt,
		/*out*/ Helium::HEBSTR* pbstrError) = 0;
	/// Save our media content to the given filename.
	/// E_FAIL if file extension not supported.
	/// Supported file extensions:
	///     ICall=WAV
	///     ICallFlowMedia=RTP
	HE_IMETHOD CreateMediaFile(/*in*/ Helium::HEBSTR bstFilePath, UInt32 ulJitterBufferSize) = 0;
};

/// \struct CallFlowKey
/// \brief Everything you need to uniquely identify a single CallFlow.
#include "hepushpack.h"
struct CallFlowKey
{
	CallID			m_nCallID;
	CallFlowIndex	m_nCallFlowIndex;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct CallFlowSignalingPacketInfo
/// \brief Enough information about a single signaling packet to show in a signaling view.
/// Probably a little bit too SIP-specific.
#include "hepushpack.h"
struct CallFlowSignalingPacketInfo
{
	UInt64				m_nPacketNumber;
	UInt32				m_nMessageIndex;
	UInt64				m_nPacketTime;
	UInt64				m_nOtherPacketNumber;	///< Response to this request, or request to which this is a response. 
	Helium::HEBSTR		m_bstrSummary;			///< "INVITE SDP( pcmu pcma telephone-event )", "180 Ringing", and so on.
	UInt16				m_nFlags;				///< \see CallFlowSignalingPacketInfoFlag
	UInt16				m_nResponseCode;		///< 180 of "180 Ringing"
	Helium::HEBSTR 		m_bstrResponseText;		///< "Trying" of "180 Ringing"
	UInt32				m_nSequence;			///< 101 of "101 INVITE" or "101 ACK"
	Helium::HEBSTR		m_bstrSequence;			///< "INVITE" of "101 INVITE"
	TMediaSpec			m_SourceAddress;		///< Who sent this packet?
	TMediaSpec			m_DestAddress;			///< To whom was this packet sent?
	UInt16				m_nSourcePort;			///< Who sent this packet?
	UInt16				m_nDestPort;			///< To whom was this packet sent?
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct AddressPortPair
/// \brief IP address, port pair.
#include "hepushpack.h"
struct AddressPortPair
{
	TMediaSpec		Address;		///< IP address.
	UInt16			Port;			///< UDP/TCP port.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \enum CallFlowSignalingPacketInfo
/// \brief Values for CallFlowSignalingPacketInfo.m_nFlags.
enum CallFlowSignalingPacketInfoFlag
{
	kSignalingFlag_None		= 0x0000,	///< A more verbose way to say 0.
	kSignalingFlag_Request	= 0x0001,	///< A SIP request packet lacks a result code.
	kSignalingFlag_Response	= 0x0002,	///< A SIP response packet starts with a numeric result code.
	kSignalingFlag_Ack		= 0x0004,	///< SIP ACK packets are actually request packets.
	kSignalingFlag_Invite	= 0x0008,	///< SIP INVITE packets
	kSignalingFlag_Bye		= 0x0010,	///< SIP BYE packets
	kSignalingFlag_Error	= 0x0020,	///< SIP error code
	kSignalingFlag_Warning	= 0x0040	///< SIP warning
};

/// \enum MediaPacketInfo_QualityIndex
/// \brief Indices into MediaPacketInfo::m_ayQuality
///
/// MediaPacketInfo elements contain several quality scores, but never 
/// *all* of these. So we either end up with a lot of wasted memory (and 
/// network bandwidth when remote), use a union and switch on the media 
/// type. Fake a union by making our own then by reusing array indices.
enum MediaPacketInfo_QualityIndex
{
	// m_RTPMediaType == callFlowMediaType_Voice
	mediaPacketInfo_index_RFactorListening		= 0,	///< [0-120]
	mediaPacketInfo_index_RFactorConversational	= 1,	///< [0-120]
	mediaPacketInfo_index_RFactorG107			= 2,	///< [0-120]
	mediaPacketInfo_index_reserved3				= 3,
	mediaPacketInfo_index_MOSListeningX100		= 4,	///< [0.00-5.00 * 100 = 0-500] 
	mediaPacketInfo_index_MOSConversationalX100	= 5,	///< [0.00-5.00 * 100 = 0-500] 
	mediaPacketInfo_index_MOSPESQListeningX100	= 6,	///< [0.00-5.00 * 100 = 0-500] 
	
	// m_RTPMediaType == callFlowMediaType_Audio portion of video call
	// Indices intentionally do not overlay callFlowMediaType_Video's indices, to allow future releases to populate both sets of stats.
	mediaPacketInfo_index_VSAQ					= 0,	///< [0-50]
	mediaPacketInfo_index_MOSAX100				= 4,	///< [0.00-5.00 * 100 = 0-500]
	
	// m_RTPMediaType == callFlowMediaType_Video
	// Indices intentionally do not overlay callFlowMediaType_Audio's indices, to allow future releases to populate both sets of stats.
	mediaPacketInfo_index_VSMQ					= 1,	///< [0-50]
	mediaPacketInfo_index_VSPQ					= 2,	///< [0-50]
	mediaPacketInfo_index_VSTQ					= 3,	///< [0-50]
	mediaPacketInfo_index_MOSAVX100				= 5,	///< [0.00-5.00 * 100 = 0-500]
	mediaPacketInfo_index_MOSVX100				= 6		///< [0.00-5.00 * 100 = 0-500]
};

/// \struct MediaPacketInfo
/// \brief Analysis results for a single RTP or RTCP packet.
/// 
/// Created by ICallManager::CreateMediaPacketInfoArray().
#include "hepushpack.h"
struct MediaPacketInfo
{
	UInt64		m_nPacketNumber;
	UInt64		m_nPacketTime;
	UInt16		m_nFlags;					///< \see MediaPacketInfoFlag

	// RTP fields. Not used for RTCP packets (left zero/blank)
	UInt16		m_nRTPMediaType;			///< If RTP, this is what kind of media. Otherwise, callFlowMediaType_None. CallFlowMediaType cast to UInt16.
	UInt16		m_nRTPSequenceNumber;		///< RTP sequence number.
	UInt16		m_nRTPPayloadByteCount;		///< Count of RTP payload bytes.
	
	// Quality metrics: Value of given quality metric after processing this RTP packet.
	UInt64		m_nRTPAverageJitter;		///< Average jitter across lifetime of call, up to this packet (nanoseconds).
	
	/// R-Factor/MOS/Quality scores, meaning changes depending on m_nRTPMediaType. 
	/// \see MediaPacketInfo_QualityIndex
	UInt16		m_ayRTPQ[7];
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \enum MediaPacketInfoFlag
/// \brief A few bits of information about a single RTP or RTCP packet.
enum MediaPacketInfoFlag
{
	mediaPacketInfoFlag_None				= 0x0000,
	mediaPacketInfoFlag_RTP					= 0x0001,
	mediaPacketInfoFlag_RTCP				= 0x0002,
	mediaPacketInfoFlag_OutOfSequence		= 0x0004,	///< current.sequence < prev.sequence
	mediaPacketInfoFlag_FollowsMissing		= 0x0008,	///< at least 1 packet between prev and current is missing, and does not appear elsewhere out of sequence.
	mediaPacketInfoFlag_CodecUnsupported	= 0x0010,	///< Jitter, R-Factor, and MOS scores not valid.
	mediaPacketInfoFlag_QualityValid		= 0x0020	///< R-Factor and MOS scores valid.
};

/// \struct CallQualityStat
/// \brief A single call quality stats containing counts for good, fair, poor, and bad.
#include "hepushpack.h"
struct CallQualityStat
{
	UInt32		Good;
	UInt32		Fair;
	UInt32		Poor;
	UInt32		Bad;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct CallQualityDistributionStats
/// \brief Call quality stats for all calls and open calls.
#include "hepushpack.h"
struct CallQualityDistributionStats
{
	CallQualityStat		AllCalls;
	CallQualityStat		OpenCalls;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct CallCodecQualityStat
/// \brief Call quality stats per-codec.
#include "hepushpack.h"
struct CallCodecQualityStat
{
	UInt32		Calls;			///< Number of open calls.
	UInt32		Flows;			///< Number of media flows (used to calculate average MOS).
	UInt16		AvgMOS;			///< Average MOS (scaled by 100; zero is invalid).
	UInt16		MinMOS;			///< Minimum MOS (scaled by 100; zero is invalid).
	UInt16		MaxMOS;			///< Maximum MOS (scaled by 100; zero is invalid).
	UInt16		Unused;			///< Unused.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface ICallCodecQualityHistoryStats
/// \brief Voice and video call quality statistics per codec.
#define ICallCodecQualityHistoryStats_IID \
{ 0x422534A5, 0xCF5F, 0x449A, {0xBC, 0xD7, 0x7F, 0x9A, 0xA2, 0xB6, 0xD3, 0xAB} }

class HE_NO_VTABLE ICallCodecQualityHistoryStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallCodecQualityHistoryStats_IID)

	/// Get the sample interval in seconds.
	HE_IMETHOD GetSampleInterval(/*out*/ UInt32* pnSampleInterval) = 0;
	/// Set the sample interval in seconds. Do this before processing.
	HE_IMETHOD SetSampleInterval(/*in*/ UInt32 nSampleInterval) = 0;
	/// Get the sample start time.
	HE_IMETHOD GetSampleStartTime(/*out*/ UInt64* pStartTime) = 0;
	/// Get the sample count.
	HE_IMETHOD GetSampleCount(/*out*/ UInt32* pnSamples) = 0;
	/// Get history data.
	HE_IMETHOD GetSamples(/*out*/ CallCodecQualityStat** ppSamples, /*out*/ UInt32* pnSamples) = 0;
};

/// \internal
/// \interface ICallCodecQualityStats
/// \brief Voice and video call quality statistics per codec.
#define ICallCodecQualityStats_IID \
{ 0x3A2A8078, 0xDE55, 0x4EAA, {0xA5, 0x5C, 0xB9, 0x53, 0xC2, 0xD2, 0x47, 0xEC} }

class HE_NO_VTABLE ICallCodecQualityStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallCodecQualityStats_IID)

	/// Get the number of stats.
	HE_IMETHOD GetCount(/*out*/ SInt32* pCount) = 0;
	/// Get call quality history stats.
	HE_IMETHOD GetCallHistoryStats(/*in*/ SInt32 nIndex, /*out*/ SInt32* pCodec,
		/*out*/ Helium::HEBSTR* pbstrCodecName,
		/*out*/ ICallCodecQualityHistoryStats** ppHistoryStats) = 0;
};

/// \internal
/// \interface ICallUtilizationStats
/// \brief Call utilization statistics.
#define ICallUtilizationStats_IID \
{ 0x90EACF54, 0xDEFA, 0x4DFF, {0xB7, 0xFD, 0x57, 0xBB, 0x40, 0xBE, 0xD7, 0xB6} }

class HE_NO_VTABLE ICallUtilizationStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallUtilizationStats_IID)

	/// Get the sample interval in seconds.
	HE_IMETHOD GetSampleInterval(/*out*/ UInt32* pnSampleInterval) = 0;
	/// Set the sample interval in seconds. Do this before processing.
	HE_IMETHOD SetSampleInterval(/*in*/ UInt32 nSampleInterval) = 0;
	/// Get the sample start time.
	HE_IMETHOD GetSampleStartTime(/*out*/ UInt64* pStartTime) = 0;
	/// Get the sample count.
	HE_IMETHOD GetSampleCount(/*out*/ UInt32* pnSamples) = 0;
	/// Get history data.
	HE_IMETHOD GetSamples(/*out*/ double** ppSamples, /*out*/ UInt32* pnSamples) = 0;
};

/// \internal
/// \interface ICallStats
/// \brief Voice and video call statistics.
#define ICallStats_IID \
{ 0xA5393576, 0x25A1, 0x462B, {0xB1, 0x53, 0x3D, 0x4A, 0xA9, 0x93, 0x3C, 0xBF} }

class HE_NO_VTABLE ICallStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICallStats_IID)

	/// Get call quality distribution stats.
	HE_IMETHOD GetCallQualityDistributionStats(/*out*/ CallQualityDistributionStats* pStats) = 0;
	/// Get call quality stats per codec.
	HE_IMETHOD GetCallCodecQualityStats(/*out*/ ICallCodecQualityStats** ppCallCodecQualityStats) = 0;
	/// Get call utilization stats.
	HE_IMETHOD GetCallUtilizationStats(/*out*/ ICallUtilizationStats** ppCallUtilizationStats) = 0;
};


#define Call_CID \
{ 0x8160F7B2, 0xB783, 0x42B7, {0x9A, 0xCC, 0xD6, 0xED, 0xF5, 0x6F, 0x3C, 0x47} }

#define CallManager_CID \
{ 0x0E6A736F, 0x5B28, 0x40B5, {0x8F, 0x49, 0xFF, 0x15, 0x76, 0xD1, 0xB2, 0x80} }

#define CallFlowSignaling_CID \
{ 0x2C1FBFE8, 0x3C87, 0x4A72, {0xA0, 0xEC, 0x31, 0xAE, 0xCA, 0x83, 0xDD, 0x22} }

#define CallFlowMedia_CID \
{ 0x0FE299BC, 0xB748, 0x4CF6, {0xAB, 0x26, 0x15, 0x12, 0xDC, 0xAA, 0x9F, 0x76} }

#define CallFlowMediaControl_CID \
{ 0xB57EBB6D, 0xD791, 0x44A2, {0x93, 0x64, 0x48, 0xC4, 0xA4, 0x63, 0x54, 0xCF} }

#define VoIPLayer_CID \
{ 0x2B5ADF2A, 0x4A93, 0x4EE6, {0xA3, 0xB3, 0xFB, 0x21, 0xC5, 0x32, 0xF1, 0x69} }

#define CallStats_CID \
{ 0x2C98A67E, 0x7C8B, 0x4A26, {0xBF, 0x0E, 0xFB, 0x6E, 0x59, 0xEF, 0x32, 0x8D} }

#endif /* PEEKVOIPCALL_H */
