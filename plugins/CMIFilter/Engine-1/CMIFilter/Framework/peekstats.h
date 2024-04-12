// =============================================================================
//	peekstats.h
// =============================================================================
//	Copyright (c) 2001-2011 WildPackets, Inc. All rights reserved.

#include "peekcore.h"

#ifndef PEEKSTATS_H
#define PEEKSTATS_H

class IPeekStatsContext;
class IEntityTable;
class ITupleTable;
class IPeekStatsServer;
class IStatsOutputMethod;

/// \internal
/// \interface ISaveStreamDeltas
/// \brief Persistence interface for saving to a stream with deltas.
#define ISaveStreamDeltas_IID \
{ 0xB29B4A65, 0xCD92, 0x4994, {0xAC, 0x41, 0xC7, 0x86, 0x47, 0x5E, 0xC6, 0x44} }

class HE_NO_VTABLE ISaveStreamDeltas : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISaveStreamDeltas_IID)

	/// Save deltas to a stream.
	/// \remarks The timestamp is nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD Save(/*in*/ Helium::IHeStream* pStream, /*in,out*/ UInt64* pullTimeStamp) = 0;
};

/// \internal
/// \interface IPersistXmlDeltas
/// \brief Persistence interface for saving to XML with deltas.
#define IPersistXmlDeltas_IID \
{ 0xCC58331B, 0xEE84, 0x4100, {0xA4, 0x46, 0x3A, 0x1E, 0x06, 0x0C, 0x7F, 0x1D} }

class HE_NO_VTABLE IPersistXmlDeltas : public Helium::IHePersist
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPersistXmlDeltas_IID)

	/// Load from XML.
	HE_IMETHOD Load(/*in*/ Helium::IHeUnknown* pXmlDoc, Helium::IHeUnknown* pXmlNode) = 0;
	/// Save to XML.
	HE_IMETHOD Save(/*in*/ Helium::IHeUnknown* pXmlDoc, /*in*/ Helium::IHeUnknown* pXmlNode,
		/*in,out*/ UInt64* pullTimeStamp) = 0;
};

/// \internal
/// \interface IUpdateStats
/// \brief Objects that can update internal stats.
#define IUpdateStats_IID \
{ 0x6370BF0C, 0xDD31, 0x4751, {0xBD, 0xF7, 0x39, 0x21, 0x1F, 0xF9, 0xB4, 0x96} }

class HE_NO_VTABLE IUpdateStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IUpdateStats_IID)

	/// Update statistics.
	HE_IMETHOD UpdateStats() = 0;
};

/// \internal
/// \interface IResetStats
/// \brief Objects that can reset just their statistics without a complete Reset().
#define IResetStats_IID \
{ 0x2A045D28, 0x6790, 0x420A, {0x9E, 0xA8, 0xAD, 0x0A, 0x95, 0x91, 0x3F, 0x97} }

class HE_NO_VTABLE IResetStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IResetStats_IID)

	/// Reset the statistics without completely resetting everything.
	HE_IMETHOD ResetStats() = 0;
};

/// \internal
/// \interface IPeekStats
/// \brief Basic stats interface.
#define IPeekStats_IID \
{ 0xC777A3B6, 0x7B37, 0x49DF, {0x8C, 0x1D, 0x96, 0xFB, 0x9D, 0xD7, 0xAA, 0x8E} }

class HE_NO_VTABLE IPeekStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStats_IID)

	/// Get the stats context.
	HE_IMETHOD GetStatsContext(/*out*/ IPeekStatsContext** ppStatsContext) = 0;
	/// Set the stats context. Clients should keep a weak reference.
	HE_IMETHOD SetStatsContext(/*in*/ IPeekStatsContext* pStatsContext) = 0;
	/// Reset the stats. Stats objects expect to have reset called before they
	/// do any processing.
	HE_IMETHOD Reset() = 0;
};

/// \internal
/// \interface IPeekStatsLimitSettings
/// \brief Maintains basic configuration of stat objects with limits
#define IPeekStatsLimitSettings_IID \
{ 0x291E5E81, 0xD453, 0x4C62, {0x8B, 0x6F, 0x5A, 0xB8, 0xF2, 0xD0, 0xC1, 0x82} }

class HE_NO_VTABLE IPeekStatsLimitSettings : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsLimitSettings_IID)

	/// Set/Get enabled state.
	HE_IMETHOD GetDoLimit(/*out*/ BOOL *pbDoLimit) = 0;
	HE_IMETHOD SetDoLimit(/*in*/ BOOL bDoLimit ) = 0;
	/// Set/Get the stats limit.
	HE_IMETHOD GetLimit(/*out*/ UInt32 *pnLimit) = 0;
	HE_IMETHOD SetLimit(/*in*/ UInt32 nLimit ) = 0;
	/// Set/Get the actions to take.
	HE_IMETHOD GetNotify(/*out*/ BOOL* pbNotify, /*out*/ PeekSeverity* pSeverity ) = 0;
	HE_IMETHOD PutNotify(/*in*/ BOOL bNotify, /*in*/ PeekSeverity severity ) = 0;
	/// It's either set to "reset" or "stop" (default).
	HE_IMETHOD GetReset(/*out*/ BOOL* pbReset) = 0;
	HE_IMETHOD SetReset(/*in*/ BOOL bReset) = 0;
};

/// \internal
/// \interface IPeekStatsLimit
/// \brief Objects that support stats limits support this interface
#define IPeekStatsLimit_IID \
{ 0xB1BEE578, 0xCE95, 0x4C1F, {0x98, 0xE0, 0x6D, 0x3D, 0xF2, 0x51, 0xC7, 0x07} }

class HE_NO_VTABLE IPeekStatsLimit : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsLimit_IID)

	/// Set/Get the stats limit and needed runtime stuff
	HE_IMETHOD GetLimit(/*out*/ IPeekStatsLimitSettings** ppSettings ) = 0;
	HE_IMETHOD SetLimit(/*in*/ IPeekStatsLimitSettings* pSettings ) = 0;
	HE_IMETHOD GetCaptureID(/*out*/ Helium::HeID* pCaptureID ) = 0;
	HE_IMETHOD SetCaptureID(/*in*/ Helium::HeID captureID ) = 0;
	HE_IMETHOD GetNotify(/*out*/ INotify** ppNotify ) = 0;
	HE_IMETHOD SetNotify(/*in*/ INotify* pNotify ) = 0;
	/// Gets the current value of the reset counter, which should be checked to determine
	/// whether a reset has occurred since the last time this object was accessed.
	HE_IMETHOD GetResetCount(/*out*/ UInt32* pulResetCount ) = 0;
	/// Set any associated stats object that should be reset or stopped, keeps a weak reference
	/// Resets will be issued through the IPeekStats interface, Halts through this interface
	HE_IMETHOD AddStatObject(/*in*/ Helium::IHeUnknown* pStatsObject) = 0;
	/// Halt further processing until Reset()
	HE_IMETHOD Halt() = 0;
	/// Reset the stats. Stats objects expect to have reset called before they
	/// do any processing.
	HE_IMETHOD Reset() = 0;
	/// Get the time the limit was reached. Zero if not reached.
	HE_IMETHOD GetTimeLimitReached(/*out*/ UInt64* pnTimeLimitReached) = 0;
	/// Get the time the limit was reached. Zero if not reached.
	HE_IMETHOD SetTimeLimitReached(/*in*/ UInt64 nTimeLimitReached) = 0;
};

/// \internal
/// \interface IStatsUpdater
/// \brief An object that knows how to read specific stats out of various
/// stats implementations and write those stats into an ISummaryStats.
/// \see IPeekStatsContext
#define IStatsUpdater_IID \
{ 0x35C177BA, 0x15C3, 0x448F, {0xB3, 0x49, 0xE3, 0x85, 0x16, 0x08, 0x08, 0x0B} }

class HE_NO_VTABLE IStatsUpdater : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IStatsUpdater_IID)

	/// Called to update stats in a stats context before saving.
	HE_IMETHOD Update(/*in*/ IPeekStatsContext* pStatsContext) = 0;
};

/// \internal
/// \interface IPeekStatsContext
/// \brief Processor and holder for stats objects.
/// \see StatsContext
#define IPeekStatsContext_IID \
{ 0x11F27A5A, 0x5503, 0x4502, {0xA4, 0xB7, 0xA7, 0xC1, 0x79, 0xBC, 0x5A, 0xE9} }

class HE_NO_VTABLE IPeekStatsContext : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsContext_IID)

	/// Get a stat object by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IHeUnknown** ppPeekStats) = 0;
	/// Get the count of items.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get a stat object by ID.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& guidID,
		/*in*/ const Helium::HeIID& iid, void** ppvObject) = 0;
	template<class T>
	HE_IMETHODIMP ItemFromID(const Helium::HeID& guidID, T** pp)
	{
		return ItemFromID(guidID, T::GetIID(), reinterpret_cast<void**>(pp) );
	}
	/// Add a stat object.
	HE_IMETHOD Add(/*in*/ const Helium::HeID& guidID, /*in*/ IHeUnknown* pPeekStats) = 0;
	/// Remove a stat object.
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& guidID) = 0;
	/// Remove all stat objects.
	HE_IMETHOD RemoveAll() = 0;
	/// Reset the stats context and all contained stat objects.
	HE_IMETHOD Reset() = 0;
	/// Get the media type.
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pMediaType) = 0;
	/// Set the media type.
	HE_IMETHOD SetMediaType(/*in*/ TMediaType MediaType) = 0;
	/// Get the media sub type.
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pMediaSubType) = 0;
	/// Set the media sub type.
	HE_IMETHOD SetMediaSubType(/*in*/ TMediaSubType MediaSubType) = 0;
	/// Get the link speed in bits/second units.
	HE_IMETHOD GetLinkSpeed(/*out*/ UInt64* pnLinkSpeed) = 0;
	/// Set the link speed in bits/second units.
	HE_IMETHOD SetLinkSpeed(/*in*/ UInt64 nLinkSpeed) = 0;
	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Get the tuple table.
	HE_IMETHOD GetTupleTable(/*out*/ ITupleTable** ppTupleTable) = 0;
	/// Get the date-time started in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetStartDateTime(/*out*/ UInt64* pullStartDateTime) = 0;
	/// Set the date-time started in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD SetStartDateTime(/*in*/ UInt64 ullStartDateTime) = 0;
	/// Get the date-time stopped in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetStopDateTime(/*out*/ UInt64* pullStopDateTime) = 0;
	/// Set the date-time stopped in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD SetStopDateTime(/*in*/ UInt64 ullStopDateTime) = 0;
	/// Get the duration in microseconds.
	HE_IMETHOD GetDuration(/*out*/ SInt64* pullDuration) = 0;
	/// Total packets seen.
	HE_IMETHOD GetTotalPackets(/*out*/ UInt64* pullTotalPackets) = 0;
	/// Total bytes seen.
	HE_IMETHOD GetTotalBytes(/*out*/ UInt64* pullTotalBytes) = 0;
	/// Get the stats updater.
	HE_IMETHOD GetStatsUpdater(/*out*/ Helium::IHeUnknown** ppStatsUpdater) = 0;
	/// Set the stats updater.
	HE_IMETHOD SetStatsUpdater(/*in*/ Helium::IHeUnknown* pStatsUpdater) = 0;
	/// Update stats now.
	HE_IMETHOD UpdateStats() = 0;
	/// Get the media info.
	HE_IMETHOD GetMediaInfo(/*out*/ IMediaInfo** ppMediaInfo) = 0;
	/// Set the media info.
	HE_IMETHOD SetMediaInfo(/*in*/ IMediaInfo* pMediaInfo) = 0;
	/// Require that stat object A sees packets before stat object B").
	HE_IMETHOD AddSequenceDependency(/*in*/ const Helium::HeID& idBefore, /*in*/ const Helium::HeID& idAfter) = 0;
};

/// \internal
/// \interface IPeekStatsTracker
/// \brief Generic interface for tracking stats over time, driven by the stats
/// server's Process thread. Individual Stats objects (NodeStats) create
/// objects that support this interface.
#define IPeekStatsTracker_IID \
{ 0x0AA72DD8, 0xD74B, 0x4845, {0xA8, 0x9E, 0x5B, 0x1B, 0x4D, 0x64, 0xC6, 0xFC} }

class HE_NO_VTABLE IPeekStatsTracker : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsTracker_IID)

	/// For generic configuration, users of these objects get the unit count and labels.
	HE_IMETHOD GetUnitsCount(/*out*/ SInt32* plCount) = 0;
	HE_IMETHOD IndexToUnitID(/*in*/ SInt32 lIndex, /*out*/ Helium::HEVARIANT* ppvtID) = 0;
	HE_IMETHOD UnitIDToLabel(/*in*/ Helium::HEVARIANT pvtUnitID, /*out*/ Helium::HEBSTR* pbstrLabel) = 0;
	/// Once the unit ID is decided, this is how to get/set it.
	HE_IMETHOD GetUnitID(/*out*/ Helium::HEVARIANT* pvtUnitID) = 0;
	HE_IMETHOD SetUnitID(/*in*/ Helium::HEVARIANT vtUnitID) = 0;
	/// Update the history of the stat.
	HE_IMETHOD Update(UInt64 ullTimestamp) = 0;
	/// Reset the history of the stat.
	HE_IMETHOD Reset() = 0;
	/// Get the total value for this stat.
	HE_IMETHOD GetTotal(/*in*/ UInt64 nTimeStamp, /*out*/ double* pdTotal) = 0;
	/// Get the difference for this stat from one timestamp to the next.
	HE_IMETHOD GetDifference(/*in*/ UInt64 nTimeStamp0, /*in*/ UInt64 nTimeStamp1, /*out*/ double* pdDiff) = 0;
	/// Get the average per second from timestamp to timestamp.
	HE_IMETHOD GetDifferencePerSecond(/*in*/ UInt64 nTimeStamp, /*out*/ double* pdPerSecRate) = 0;
	/// Get the maximum history this stat can store in seconds.
	HE_IMETHOD GetMaxHistory(/*in*/ UInt32* pulMaxHistory) = 0;
	/// Get the history this stat can store in seconds.
	HE_IMETHOD GetHistoryLength(/*out*/ UInt32* pulHistoryLength) = 0;
	/// Set the history this stat can store in seconds.
	HE_IMETHOD SetHistoryLength(/*in*/ UInt32 ulHistoryLength) = 0;
	/// Get the current history boundary timestamps.
	HE_IMETHOD GetHistoryBounds(/*out*/ UInt64* pnTimeStamp0, /*out*/ UInt64* pnTimeStamp1) = 0;
	/// Get the last update timestamp in nanoseconds since midnight 1/1/1601 UTC.
	HE_IMETHOD GetLastUpdate(/*out*/ UInt64* pnTimeStamp) = 0;
	/// Set the stats context.
	HE_IMETHOD SetContext(/*in*/ IPeekStatsContext* pContext) = 0;
	/// Get the stats context.
	HE_IMETHOD GetContext(/*out*/ IPeekStatsContext** ppContext) = 0;
};

/// \internal
/// \interface IPeekStatsTrackerFactory
/// \brief Generic interface used by IPeekStats objects for creating stats trackers.
/// \see StatsServer
#define IPeekStatsTrackerFactory_IID \
{ 0xB9093FFD, 0x2F50, 0x47FD, {0xB5, 0x07, 0x39, 0xE7, 0x59, 0x7B, 0x18, 0x73} }

class HE_NO_VTABLE IPeekStatsTrackerFactory : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsTrackerFactory_IID)

	HE_IMETHOD CreateStatsTracker(/*in*/ Helium::HEVARIANT pvtItem,
		/*out*/ IPeekStatsTracker** ppStatsTracker) = 0;
};

/// \internal
/// \interface IEntityTable
/// \brief Table of network entities based on TMediaSpec.
/// \see coclass EntityTable
#define IEntityTable_IID \
{ 0x02184323, 0x03C1, 0x4C92, {0x86, 0x6D, 0xCC, 0xDF, 0x44, 0xAF, 0xF7, 0x52} }

class HE_NO_VTABLE IEntityTable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IEntityTable_IID)

	static const UInt32 EntityIndex_Invalid = static_cast<UInt32>(-1);

	/// Reset the entity table.
	HE_IMETHOD Reset() = 0;
	/// Get the count of items of a particular type (kMediaSpecType_Null for all).
	HE_IMETHOD GetCount(/*in*/ TMediaSpecType type, /*out*/ UInt32* pulCount) = 0;
	/// Look up the index for an entity in the table and optionally add it if
	/// not found.
	/// \see IndexToEntity
	HE_IMETHOD EntityToIndex(/*in*/ const TMediaSpec* pMediaSpec,
		/*in*/ BOOL bAddIfNotFound, /*in*/ UInt64 nTimeStamp, /*out*/ UInt32* pulIndex) = 0;
	/// Retrieve an entity from the table by index and optionally the time it
	/// was added to the table.
	/// \see EntityToIndex
	HE_IMETHOD IndexToEntity(/*in*/ UInt32 ulIndex, /*out*/ TMediaSpec* pMediaSpec,
		/*out*/ UInt64* pullTimeStamp) = 0;
	/// Get the first index for a given type of entity.
	/// \see GetNext
	HE_IMETHOD GetFirst(/*in*/ TMediaSpecType mst, /*out*/ UInt32* pulIndex, /*out*/ BOOL* pbResult) = 0;
	/// Get the next index for an entity.
	/// \see GetFirst
	HE_IMETHOD GetNext(/*in*/ UInt32 ulIndex, /*out*/ UInt32* pulNextIndex, /*out*/ BOOL* pbResult) = 0;
	/// Check if a protocol is a subprotocol of another protocol.
	HE_IMETHOD IsChildProtocol(/*in*/ UInt32 ulProtocolIndex, /*in*/ UInt32 ulParentIndex, /*out*/ BOOL* pbResult) = 0;
	/// Get the media spec type and class for an entry in the table.
	HE_IMETHOD IndexToClassAndType(/*in*/ UInt32 ulIndex, /*out*/ TMediaSpecClass* pClass,
		/*out*/ TMediaSpecType* pType) = 0;
};

/// \internal
/// \struct PeekTupleEntry
/// \brief An entry in the tuple table.
/// \see ITupleTable
#include "hepushpack.h"
struct PeekTupleEntry
{
	UInt64		TimeStamp;	///< Time the entry was added to the table in nanoseconds since midnight 1/1/1601 UTC.
	UInt32		SrcAddr;	///< Entity index for the source address.
	UInt32		DestAddr;	///< Entity index for the destination address.
	UInt32		Protocol;	///< Entity index for the protocol.
	UInt32		NextIndex;	///< Next index in the tuple table (used internally).
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface ITupleTable
/// \brief Table of network entities based on TMediaSpec.
/// \see coclass TupleTable
#define ITupleTable_IID \
{ 0x3C6209BE, 0x566D, 0x48D9, {0x94, 0x1E, 0x96, 0xFE, 0x4A, 0x30, 0x79, 0xE6} }

class HE_NO_VTABLE ITupleTable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITupleTable_IID)

	static const UInt32 TupleIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Set the entity table.
	HE_IMETHOD SetEntityTable(/*in*/ IEntityTable* pEntityTable) = 0;
	/// Reset the tuple table.
	HE_IMETHOD Reset() = 0;
	/// Get the count of items of a particular type (kMediaSpecType_Null for all).
	HE_IMETHOD GetCount(/*in*/ TMediaSpecType type, /*out*/ UInt32* pulCount) = 0;
	/// Look up the index for a tuple in the table and optionally add it if
	/// not found.
	/// \see IndexToTuple
	HE_IMETHOD TupleToIndex(/*in*/ const PeekTupleEntry* pTuple,
		/*in*/ BOOL bAddIfNotFound, /*out*/ UInt32* pulIndex) = 0;
	/// Retrieve a tuple from the table by index.
	/// \see TupleToIndex
	HE_IMETHOD IndexToTuple(/*in*/ UInt32 ulIndex, /*out*/ PeekTupleEntry* pTuple) = 0;
	/// Find the next tuple.
	HE_IMETHOD FindNextTuple(/*in*/ UInt32 ulSrcAddr, /*in*/ UInt32 ulDestAddr,
		/*in*/ UInt32 ulProtocol, /*in*/ BOOL bBothDirections,
		/*in*/ UInt32 ulIndex, /*out*/ UInt32* pulIndex, /*out*/ BOOL* pbResult ) = 0;
	/// Get the first index for a given type of tuple.
	/// \see GetNext
	HE_IMETHOD GetFirst(/*in*/ TMediaSpecType mst, /*out*/ UInt32* pulIndex, /*out*/ BOOL* pbResult) = 0;
	/// Get the next index for a tuple.
	/// \see GetFirst
	HE_IMETHOD GetNext(/*in*/ UInt32 ulIndex, /*out*/ UInt32* pulNextIndex, /*out*/ BOOL* pbResult) = 0;
};

/// \internal
/// \struct PeekConversationStat
/// \brief Basic conversation stats.
/// \see IConversationStats
#include "hepushpack.h"
struct PeekConversationStat
{
	UInt64		FirstTime;		///< First time this conversation was seen in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		LastTime;		///< Last time this conversation was seen in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		Packets;		///< Total packets in this conversation.
	UInt64		Bytes;			///< Total bytes in this conversation.
	UInt16		MinPacketSize;	///< Minimum size packet in this conversation in bytes.
	UInt16		MaxPacketSize;	///< Maximum size packet in this conversation in bytes.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekConversationStatEntry
/// \brief An entry in the conversation stats.
/// \see IConversationStats
#include "hepushpack.h"
struct PeekConversationStatEntry
{
	UInt32					TupleIndex;		///< Tuple table index for the conversation.
	UInt32					NextIndex;		///< Next stat index (used internally).
	PeekConversationStat	Stat;			///< Statistics for this conversation.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IConversationStats
/// \brief Conversation statistics.
/// \see ConversationStats
#define IConversationStats_IID \
{ 0xE51EE005, 0x2871, 0x4546, {0x90, 0xEF, 0xB5, 0xCA, 0x71, 0xD8, 0xBE, 0xF9} }

class HE_NO_VTABLE IConversationStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IConversationStats_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the conversation count.
	HE_IMETHOD GetCount(/*out*/ UInt32* pulCount) = 0;
	/// Find a conversation stat.
	HE_IMETHOD Find(/*in*/ UInt32 ulTupleIndex, /*out*/ UInt32* pulStatIndex, /*out*/ BOOL* pbFound) = 0;
	/// Get a conversation stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 ulStatIndex, /*out*/ PeekConversationStatEntry* pStatEntry) = 0;
};

/// \internal
/// \interface IErrorStats
/// \brief Error statistics.
/// \see ErrorStats
#define IErrorStats_IID \
{ 0xBB8224AB, 0xD316, 0x4EF7, {0x88, 0xFB, 0xA7, 0xC4, 0x32, 0x83, 0x48, 0x23} }

class HE_NO_VTABLE IErrorStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IErrorStats_IID)

	/// Get the number of stats available.
	HE_IMETHOD GetCount(/*out*/ UInt32* pulCount) = 0;
	/// Get stats.
	HE_IMETHOD GetStats(/*in*/ UInt32 ulCount, /*out*/ UInt64 stats[]) = 0;
	/// Get the current errors/second.
	HE_IMETHOD GetErrorsPerSecond(/*out*/ double* pErrorsPerSec) = 0;
};

/// \internal
/// \struct PeekNetworkStats
/// \brief Basic network statistics.
/// \see INetworkStats
#include "hepushpack.h"
struct PeekNetworkStats
{
	UInt64		TotalPackets;		///< Total packets.
	UInt64		TotalBytes;			///< Total bytes.
	UInt64		BroadcastPackets;	///< Broadcast packets.
	UInt64		BroadcastBytes;		///< Broadcast bytes.
	UInt64		MulticastPackets;	///< Multicast packets.
	UInt64		MulticastBytes;		///< Multicast bytes.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekCurrentNetworkStats
/// \brief Current network statistics per second.
/// \see INetworkStats
#include "hepushpack.h"
struct PeekCurrentNetworkStats
{
	double		TotalPacketsPerSecond;		///< Total packets per second.
	double		TotalUtilization;			///< Total bits per second.
	double		BroadcastPacketsPerSecond;	///< Broadcast packets per second.
	double		BroadcastUtilization;		///< Broadcast bits per second.
	double		MulticastPacketsPerSecond;	///< Multicast packets per second.
	double		MulticastUtilization;		///< Multicast bits per second.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekMaximumNetworkStats
/// \brief Maximum network statistics per second.
/// \see INetworkStats
#include "hepushpack.h"
struct PeekMaximumNetworkStats
{
	double		TotalUtilization;			///< Total bits per second.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface INetworkStats
/// \brief Basic network statistics
/// \see coclass NetworkStats
#define INetworkStats_IID \
{ 0x90DDFF35, 0xDC02, 0x406D, {0xB1, 0xB8, 0x65, 0x6E, 0xBF, 0x4C, 0x30, 0x8C} }

class HE_NO_VTABLE INetworkStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INetworkStats_IID)

	/// Get the network stats.
	HE_IMETHOD GetStats(/*out*/ PeekNetworkStats* pStats) = 0;
	/// Get the current network stats in per second format.
	HE_IMETHOD GetCurrentStats(/*out*/ PeekCurrentNetworkStats* pStats) = 0;
	/// Get the maximum network stats in per second format.
	HE_IMETHOD GetMaximumStats(/*out*/ PeekMaximumNetworkStats* pStats) = 0;
};

/// \internal
/// \struct PeekChannelStats
/// \brief Per-channel basic network statistics.
/// \see IChannelStats
#include "hepushpack.h"
struct PeekChannelStats
{
	UInt32				ChannelNumber;
	PeekNetworkStats	NetworkStats;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IChannelStats
/// \brief Per-channel basic network statistics
/// \see coclass NetworkStats
#define IChannelStats_IID \
{ 0xE0592785, 0x82CD, 0x4CAF, {0x89, 0x31, 0x79, 0x2D, 0xC3, 0x0E, 0x49, 0x6E} }

class HE_NO_VTABLE IChannelStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IChannelStats_IID)

	/// Get the channel count.
	HE_IMETHOD GetCount(/*out*/ UInt32* pnChannelCount) = 0;
	/// Get channel stats by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 nChannelIndex, /*out*/ PeekChannelStats* pChannelStats) = 0;
	/// Get the current network stats in per second format for the given channel.
	HE_IMETHOD GetCurrentStats(/*in*/ UInt32 nChannelIndex, /*out*/ PeekCurrentNetworkStats* pStats) = 0;
};

/// \internal
/// \interface IHistoryStats
/// \brief History statistics.
#define IHistoryStats_IID \
{ 0x35D910BB, 0xD613, 0x4845, {0xAF, 0xFA, 0x23, 0x2E, 0xC7, 0x97, 0x0A, 0xFB} }

class HE_NO_VTABLE IHistoryStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IHistoryStats_IID)

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
/// \interface IDashboardStats
/// \brief History statistics.
#define IDashboardStats_IID \
{ 0x7D71D3DF, 0x597C, 0x412E, {0x9B, 0x27, 0xEB, 0x68, 0xAA, 0x87, 0x82, 0x0B} }

class HE_NO_VTABLE IDashboardStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDashboardStats_IID)

	/// Get the current history stats.
	HE_IMETHOD GetHistoryStats(/*out*/ IHistoryStats** ppHistoryStats) = 0;
};

/// \internal
/// \struct PeekNodeStat
/// \brief Basic node stats
/// \see INodeStats
#include "hepushpack.h"
struct PeekNodeStat
{
	UInt64		FirstTimeSent;			///< First time this node sent packets in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		LastTimeSent;			///< Last time this node sent packets in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		FirstTimeReceived;		///< First time this node received packets in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		LastTimeReceived;		///< Last time this node received packets in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		PacketsSent;			///< Total packets sent by this node.
	UInt64		BytesSent;				///< Total bytes sent by this node.
	UInt64		PacketsReceived;		///< Total packets received by this node.
	UInt64		BytesReceived;			///< Total bytes received by this node.
	UInt64		BroadcastPackets;		///< Broadcast packets sent by this node.
	UInt64		BroadcastBytes;			///< Broadcast bytes sent by this node.
	UInt64		MulticastPackets;		///< Multicast packets sent by this node.
	UInt64		MulticastBytes;			///< Multicast bytes sent by this node.
	UInt16		MinPacketSizeSent;		///< Minimum size packet sent by this node in bytes.
	UInt16		MaxPacketSizeSent;		///< Maximum size packet sent by this node in bytes.
	UInt16		MinPacketSizeReceived;	///< Minimum size packet received by this node in bytes.
	UInt16		MaxPacketSizeReceived;	///< Maximum size packet received by this node in bytes.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekNodeStatEntry
/// \brief An entry in the node stats
/// \see INodeStats
#include "hepushpack.h"
struct PeekNodeStatEntry
{
	UInt32			EntityIndex;	///< Entity table index for the node.
	UInt32			NextIndex;		///< Next stat index (used internally).
	PeekNodeStat	Stat;			///< Statistics for this node.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface INodeStats
/// \brief Node statistics.
/// \see coclass NodeStats
#define INodeStats_IID \
{ 0x4B03564E, 0x978F, 0x4FA2, {0xB3, 0x82, 0xE3, 0x5B, 0x3F, 0x43, 0x6E, 0xFC} }

class HE_NO_VTABLE INodeStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INodeStats_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Get the node count.
	HE_IMETHOD GetCount(/*out*/ UInt32* pulCount) = 0;
	/// Get the number of physical addresses in the table.
	HE_IMETHOD GetPhysicalAddresses(/*out*/ UInt32* pulPhysicalAddresses) = 0;
	/// Get the number of logical addresses in the table.
	HE_IMETHOD GetLogicalAddresses(/*out*/ UInt32* pulLogicalAddresses) = 0;
	/// Find a node stat.
	HE_IMETHOD Find(/*in*/ UInt32 ulEntityIndex, /*out*/ UInt32* pulStatIndex, /*out*/ BOOL* pbFound) = 0;
	/// Get a node stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 ulStatIndex, /*out*/ PeekNodeStatEntry* pStatEntry) = 0;
};

/// \internal
/// \struct PeekNodeStatHierEntry
/// \brief An entry in the node stats hierarchy
/// \see INodeStatsHierarchy
#include "hepushpack.h"
struct PeekNodeStatHierEntry
{
	UInt32			PhysicalIndex;	///< Entity table index for the physical address.
	UInt32			LogicalIndex;	///< Entity table index for the logical address.
	UInt32			NextIndex;		///< Next stat index (used internally).
	PeekNodeStat	Stat;			///< Statistics for this node.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface INodeStatsHierarchy
/// \brief Node statistics hierarchy.
/// \see coclass NodeStats
#define INodeStatsHierarchy_IID \
{ 0xF24092F0, 0x9E70, 0x46C1, {0xA3, 0x6D, 0x5A, 0x53, 0x74, 0x3E, 0xBF, 0x9D} }

class HE_NO_VTABLE INodeStatsHierarchy : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INodeStatsHierarchy_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Get the node count.
	HE_IMETHOD GetCount(/*out*/ UInt32* pulCount) = 0;
	/// Get the number of physical addresses in the table.
	HE_IMETHOD GetPhysicalAddresses(/*out*/ UInt32* pulPhysicalAddresses) = 0;
	/// Get the number of logical addresses in the table.
	HE_IMETHOD GetLogicalAddresses(/*out*/ UInt32* pulLogicalAddresses) = 0;
	/// Find a node stat.
	HE_IMETHOD Find(/*in*/ UInt32 ulPhysicalIndex, /*in*/ UInt32 ulLogicalIndex,
		/*out*/ UInt32* pulStatIndex, /*out*/ BOOL* pbFound) = 0;
	/// Get a node stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 ulStatIndex, /*out*/ PeekNodeStatHierEntry* pStatEntry) = 0;
};

/// \internal
/// \interface IRollingNodeStats
/// \brief Rolling node statistics.
/// \see coclass RollingNodeStats
#define IRollingNodeStats_IID \
{ 0x75866A5C, 0x23A0, 0x43DF, {0xA8, 0x25, 0x50, 0x1F, 0x74, 0xCF, 0x48, 0x19} }

class HE_NO_VTABLE IRollingNodeStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IRollingNodeStats_IID)

	/// Get the bucket count.
	HE_IMETHOD GetBucketCount(/*out*/ UInt32* pnBucketCount) = 0;
	/// Set the bucket count.
	HE_IMETHOD SetBucketCount(/*in*/ UInt32 nBucketCount) = 0;
	/// Get the sample size in seconds.
	HE_IMETHOD GetSampleSize(/*out*/ UInt32* pnSampleSize) = 0;
	/// Set the sample size in seconds.
	HE_IMETHOD SetSampleSize(/*in*/ UInt32 nSampleSizeSeconds) = 0;
	/// Compile the stats
	HE_IMETHOD CompileStats() = 0;
};

/// \internal
/// \struct PeekProtocolStat
/// \brief Basic protocol stats.
/// \see IProtocolStats
#include "hepushpack.h"
struct PeekProtocolStat
{
	UInt64		FirstTime;	///< First time this protocol was seen in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		LastTime;	///< Last time this protocol was seen in nanoseconds since midnight 1/1/1601 UTC.
	UInt64		Packets;	///< Total packets using this protocol.
	UInt64		Bytes;		///< Total bytes using this protocol.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekProtocolStatEntry
/// \brief An entry in the protocol stats.
/// \see IProtocolStats
#include "hepushpack.h"
struct PeekProtocolStatEntry
{
	UInt32				EntityIndex;	///< Entity table index for the protocol.
	UInt32				NextIndex;		///< Next stat index (used internally).
	PeekProtocolStat	Stat;			///< Statistics for this protocol.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IProtocolStats
/// \brief Protocol statistics.
/// \see coclass ProtocolStats
#define IProtocolStats_IID \
{ 0xA4B887DE, 0x66E8, 0x4F1E, {0xA8, 0x97, 0xFB, 0x75, 0x8F, 0xC3, 0x79, 0x63} }

class HE_NO_VTABLE IProtocolStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProtocolStats_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Get the protocol count.
	HE_IMETHOD GetCount(/*out*/ UInt32* pulCount) = 0;
	/// Find a protocol stat.
	HE_IMETHOD Find(/*in*/ UInt32 ulEntityIndex, /*out*/ UInt32* pulStatIndex) = 0;
	/// Get a protocol stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 ulStatIndex, /*out*/ PeekProtocolStatEntry* pStatEntry) = 0;
};

/// \internal
/// \enum SizeStatsBucketSetID
/// \brief Identifier for packets size distribution statistics.
enum SizeStatsBucketSetID
{
	kSizeStatsBucketSetID_Undefined,	///< No packets yet seen, so we have no idea which buckets to use.
	kSizeStatsBucketSetID_Ethernet,		///< <= 64, 65-127, 128-255, 256-511, 512-1023, 1024-1517, >= 1518
	kSizeStatsBucketSetID_TokenRing,	///<  < 64, 64-127, 128-255, 256-511, 512-1023, 1024-2047, 2048-4095, 4096-8191, 8192-17999, >= 18000
	kSizeStatsBucketSetID_Wireless,		///<  < 64, 64-127, 128-255, 256-511, 512-1023, 1024-2047, 2048-2346, >= 2347
	kSizeStatsBucketSetID_GigEthernet,	///< <= 64, 65-127, 128-255, 256-511, 512-1023, 1024-1518, 1519-2047, 2048-4095, 4096-8191, 8192-9017, 9018-9022, >= 9023
	kSizeStatsBucketSetID_WAN			///< <= 64, 65-127, 128-255, 256-511, 512-1023, 1024-1517, >= 1518
};

/// \internal
/// \interface ISizeStats
/// \brief Packet size distribution statistics.
/// Ranges for Ethernet (7):<br>
/// <= 64, 65-127, 128-255, 256-511, 512-1023, 1024-1517, >= 1518<br>
/// Ranges for TokenRing (10):<br>
/// < 64, 64-127, 128-255, 256-511, 512-1023, 1024-2047, 2048-4095, 4096-8191, 8192-17999, >= 18000<br>
/// Ranges for Wireless (7):<br>
/// < 64, 64-127, 128-255, 256-511, 512-1023, 1024-2047, 2048-2346, >= 2347<br>
/// Ranges for Gigabit:<br>
/// <= 64, 65-127, 128-255, 256-511, 512-1023, 1024-1518, 1519-2047, 2048-4095, 4096-8191, 8192-9017, 9018-9022, >= 9023<br>
/// Ranges for WAN:<br>
/// <= 64, 65-127, 128-255, 256-511, 512-1023, 1024-1517, >= 1518
/// \see SizeStats
#define ISizeStats_IID \
{ 0x876BCD98, 0x0C50, 0x461A, {0xB1, 0xCD, 0x5D, 0x02, 0x03, 0x94, 0x37, 0xBE} }

class HE_NO_VTABLE ISizeStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISizeStats_IID)

	/// Get the number of stats available.
	HE_IMETHOD GetCount(/*out*/ UInt32* pulCount) = 0;
	/// Get stats.
	HE_IMETHOD GetStats(/*in*/ UInt32 ulCount, /*out*/ UInt64 stats[]) = 0;
	/// Get the bucket set ID.
	HE_IMETHOD GetBucketSetID(/*out*/ SizeStatsBucketSetID* pbsid) = 0;
};

/// \internal
/// \struct PeekWirelessChannelDataRateStat
/// \brief TODO.
/// \see IWirelessChannelStats
#include "hepushpack.h"
struct PeekWirelessChannelDataRateStat
{
	UInt64			nPackets;
	UInt64			nBytes;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekWirelessChannelStatEntry
/// \brief TODO.
/// \see IWirelessChannelStats
#include "hepushpack.h"
struct PeekWirelessChannelStatEntry
{
	UInt64			nFirstTime;
	UInt64			nLastTime;
	UInt64			nPackets;
	UInt64			nBytes;
	UInt64			nDataPackets;
	UInt64			nDataBytes;
	UInt64			nManagementPackets;
	UInt64			nManagementBytes;
	UInt64			nControlPackets;
	UInt64			nControlBytes;
	UInt64			nLocalPackets;
	UInt64			nLocalBytes;
	UInt64			nFromDSPackets;
	UInt64			nFromDSBytes;
	UInt64			nToDSPackets;
	UInt64			nToDSBytes;
	UInt64			nDS2DSPackets;
	UInt64			nDS2DSBytes;
	UInt64			nRetryPackets;
	UInt64			nRetryBytes;
	UInt64			nWEPPackets;
	UInt64			nWEPBytes;
	UInt64			nOrderPackets;
	UInt64			nOrderBytes;
	UInt64			nPLCPShortPackets;
	UInt64			nPLCPShortBytes;
	UInt64			nPLCPLongPackets;
	UInt64			nPLCPLongBytes;
	UInt64			nCRCErrorPackets;
	UInt64			nWEPICVErrorPackets;
	UInt64			nSignalAccumulator;
	UInt8			nSignalMin;
	UInt8			nSignalMax;
	UInt8			nSignalCurrent;
	double			nSignaldBmAccumulator;
	SInt16			nSignaldBmMin;
	SInt16			nSignaldBmMax;
	SInt16			nSignaldBmCurrent;
	UInt64			nNoiseAccumulator;
	UInt8			nNoiseMin;
	UInt8			nNoiseMax;
	UInt8			nNoiseCurrent;
	double			nNoisedBmAccumulator;
	SInt16			nNoisedBmMin;
	SInt16			nNoisedBmMax;
	SInt16			nNoisedBmCurrent;
	WirelessChannel	Channel;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IWirelessChannelStats
/// \brief Wireless Channel statistics.
/// \see coclass WirelessChannelStats
#define IWirelessChannelStats_IID \
{ 0x97B9706E, 0x8C40, 0x4D0C, {0x9F, 0x05, 0x63, 0xCB, 0x0F, 0x27, 0x4C, 0xA2} }

class HE_NO_VTABLE IWirelessChannelStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessChannelStats_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the number of stats available.
	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	/// Get the number of channel changes.
	HE_IMETHOD GetChannelChanges(/*out*/ UInt32* pnChanges) = 0;
	/// Get the number of date rate changes.
	HE_IMETHOD GetDataRateChanges(/*out*/ UInt32* pnChanges) = 0;
	/// Get the array of data rates.
	HE_IMETHOD GetDataRates(/*out*/ UInt32** ppRates, /*out*/ UInt32* pnRates) = 0;
	/// Get a stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 nStatIndex, /*out*/ PeekWirelessChannelStatEntry* pStatEntry) = 0;
	/// Get a stat entry by index.
	HE_IMETHOD GetDataRateStats(/*in*/ WirelessChannel Channel,
		/*out*/ PeekWirelessChannelDataRateStat** ppStats, /*out*/ UInt32* pnStats) = 0;
};

/// \struct PeekWirelessStrength
/// \brief Current, average, min, max signal (or noise) strength, in both percent and dBm.
/// \see PeekWirelessChannelStatEntry2, PeekWirelessStrengthSet
#include "hepushpack.h"
struct PeekWirelessStrength
{
	BOOL			bHasFirstPacket;
	UInt8			nPercentCurrent;
	UInt8			nPercentMin;
	UInt8			nPercentMax;
	UInt64			nPercentAccumulator;
	double			nmWAccumulator;
	SInt16			ndBmCurrent;
	SInt16			ndBmMin;
	SInt16			ndBmMax;
} HE_PACK_STRUCT;
#include "hepoppack.h"

///\struct PeekWirelessStrengthSet
///\brief Current and recent signal, noise, and signal-to-noise data.
///\see PeekWirelessChannelStatEntry2
#include "hepushpack.h"
struct PeekWirelessStrengthSet
{
	PeekWirelessStrength	Signal;					///< Signal levels, built up since capture started.
	PeekWirelessStrength	Noise;					///< Noise levels, build up since capture started.
	PeekWirelessStrength	SignalToNoise;			///< Signal-to-noise ratios, all values really in dB (not % or dBm).
	PeekWirelessStrength	RecentSignal;			///< Signal levels, reset every 10 seconds or so.
	PeekWirelessStrength	RecentNoise;			///< Noise levels, reset every 10 seconds or so.
	PeekWirelessStrength	RecentSignalToNoise;	///< Signal to noise ratio, all values really in dB (not % or dBm).
	PeekWirelessStrength	PrevSignal;				///< RecentSignal's value before ResetAging().
	PeekWirelessStrength	PrevNoise;				///< RecentNoise's value before ResetAging().
	PeekWirelessStrength	PrevSignalToNoise;		///< RecentSignalToNoise's value before ResetAging().
	UInt64					RecentPacketCount;		///< How many packets have we seen since our last "reset every 10 seconds or so".
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct PeekWirelssChannelStatEntry2
/// \brief  data for a single 802.11 wireless channel
/// \see IWirelessChannelStats2
/// Includes more data about signal strength than PeekWirelessChannelStatEntry.
#include "hepushpack.h"
struct PeekWirelessChannelStatEntry2
{
	UInt64						nFirstTime;
	UInt64						nLastTime;
	UInt64						nPackets;
	UInt64						nBytes;
	UInt64						nDataPackets;
	UInt64						nDataBytes;
	UInt64						nManagementPackets;
	UInt64						nManagementBytes;
	UInt64						nControlPackets;
	UInt64						nControlBytes;
	UInt64						nLocalPackets;
	UInt64						nLocalBytes;
	UInt64						nFromDSPackets;
	UInt64						nFromDSBytes;
	UInt64						nToDSPackets;
	UInt64						nToDSBytes;
	UInt64						nDS2DSPackets;
	UInt64						nDS2DSBytes;
	UInt64						nRetryPackets;
	UInt64						nRetryBytes;
	UInt64						nWEPPackets;
	UInt64						nWEPBytes;
	UInt64						nOrderPackets;
	UInt64						nOrderBytes;
	UInt64						nPLCPShortPackets;
	UInt64						nPLCPShortBytes;
	UInt64						nPLCPLongPackets;
	UInt64						nPLCPLongBytes;
	UInt64						nCRCErrorPackets;
	UInt64						nWEPICVErrorPackets;
	WirelessChannel				Channel;
	UInt64						nPrevResetTime;
	PeekWirelessStrengthSet		All;
	PeekWirelessStrengthSet		FromDS;
	PeekWirelessStrengthSet		ToDS;
	PeekWirelessStrengthSet		DS2DS;
	PeekWirelessStrengthSet		STA2STA;
} HE_PACK_STRUCT;
#include "hepoppack.h"

///\interface IWirelessChannelStats2
///\brief A set of signal, noise, byte, and packet statistics, greater than IWirelessChannelStats.
///\see coclass WirelessChannelStats2.
#define IWirelessChannelStats2_IID \
{ 0x602CD241, 0x0F66, 0x4AC3, {0x81, 0xF8, 0xFC, 0x4A, 0x74, 0x78, 0x06, 0x7D} }

class HE_NO_VTABLE IWirelessChannelStats2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessChannelStats2_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the number of stats available.
	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	/// Get the number of channel changes.
	HE_IMETHOD GetChannelChanges(/*out*/ UInt32* pnChanges) = 0;
	HE_IMETHOD AddAPToChannel(/*in*/ WirelessChannel channel, /*in*/ TMediaSpec* pBSSID) = 0;
	HE_IMETHOD RemoveAPFromChannel(/*in*/ WirelessChannel channel, /*in*/ TMediaSpec* pBSSID) = 0;
	/// Get a stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 nStatIndex, /*out*/ PeekWirelessChannelStatEntry2* pStatEntry) = 0;
	HE_IMETHOD GetAPCount(/*in*/ WirelessChannel channel, /*out*/ UInt32* pnCount) = 0;
	HE_IMETHOD FindAPIndex(/*in*/ WirelessChannel channel, /*in*/ const TMediaSpec* pBSSID, /*out*/ SInt32* pnIndex) = 0;
	HE_IMETHOD CalcMaxAPCount(/*out*/ UInt32* pnCount) = 0;
	/// Get the array of data rates.
	HE_IMETHOD GetDataRates(/*out*/ UInt32** ppRates, /*out*/ UInt32* pnRates) = 0;
	HE_IMETHOD GetDataRateStats(/*in*/ WirelessChannel Channel,
		/*out*/ PeekWirelessChannelDataRateStat** ppStats, /*out*/ UInt32* pnStats) = 0;
};

/// \internal
/// \enum PeekWirelessNodeType
/// \brief TODO.
/// \see PeekWirelessNodeStat, IWirelessNodeStats
enum PeekWirelessNodeType
{
	peekWirelessNodeTypeUnknown,
	peekWirelessNodeTypeAdministrative,
	peekWirelessNodeTypeAccessPoint,
	peekWirelessNodeTypeESSID,
	peekWirelessNodeTypeStation,
	peekWirelessNodeTypeAdHocBaseStation
};

/// \internal
/// \enum PeekWirelessEncryptionType
/// \brief TODO.
/// \see PeekWirelessNodeStat, IWirelessNodeStats
enum PeekWirelessEncryptionType
{
	peekWirelessEncryptionTypeNone,
	peekWirelessEncryptionTypeWEP,
	peekWirelessEncryptionTypeCKIP,
	peekWirelessEncryptionTypeTKIP,
	peekWirelessEncryptionTypeCCMP
};

/// \internal
/// \enum PeekWirelessAuthenticationType
/// \brief TODO.
/// \see PeekWirelessNodeStat, IWirelessNodeStats
enum PeekWirelessAuthenticationType
{
	peekWirelessAuthenticationTypeNone,
	peekWirelessAuthenticationTypeEAP,
	peekWirelessAuthenticationTypeLEAP,
	peekWirelessAuthenticationTypePEAP,
	peekWirelessAuthenticationTypeEAPTLS
};

/// \internal
/// \struct PeekWirelessNodeStat
/// \brief TODO.
/// \see IWirelessNodeStats
#include "hepushpack.h"
struct PeekWirelessNodeStat
{
	UInt64					nFirstTimeSent;
	UInt64					nLastTimeSent;
	UInt64					nFirstTimeReceived;
	UInt64					nLastTimeReceived;
	UInt64					nPacketsSent;
	UInt64					nPacketsReceived;
	UInt64					nBytesSent;
	UInt64					nBytesReceived;
	UInt64					nBroadcastPackets;
	UInt64					nBroadcastBytes;
	UInt64					nMulticastPackets;
	UInt64					nMulticastBytes;
	UInt16					nMinPacketSizeSent;
	UInt16					nMaxPacketSizeSent;
	UInt16					nMinPacketSizeReceived;
	UInt16					nMaxPacketSizeReceived;
	UInt64					nRetryPackets;
	UInt64					nWEPPackets;
	UInt64					nBeaconPackets;
	UInt64					nWEPICVErrors;
	UInt8					ESSID[32];
	UInt8					nESSIDSize;			
	UInt8					nType;
	UInt8					nEncryptionType;
	UInt8					nAuthenticationType;
	WirelessChannel			DSChannel;
	WirelessChannel			APChannel;
	UInt8					nWEPKey;
	UInt8					nTrust;
	UInt8					nSignalMin;
	UInt8					nSignalMax;
	UInt8					nSignalCurrent;
	UInt8					nNoiseMin;
	UInt8					nNoiseMax;
	UInt8					nNoiseCurrent;
	SInt16					nSignaldBmMin;
	SInt16					nSignaldBmMax;
	SInt16					nSignaldBmCurrent;
	SInt16					nNoisedBmMin;
	SInt16					nNoisedBmMax;
	SInt16					nNoisedBmCurrent;
	UInt8					bBeaconsContainESSID;
	UInt8					bAdHocBaseStation;
	UInt8					bPrivacyFlag;
	UInt8					bPowerSaveFlag;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekWirelessNodeStat2
/// \brief TODO.
/// \see IWirelessNodeStats
#include "hepushpack.h"
struct PeekWirelessNodeStat2
{
	UInt64					nFirstTimeSent;
	UInt64					nLastTimeSent;
	UInt64					nFirstTimeReceived;
	UInt64					nLastTimeReceived;
	UInt64					nPacketsSent;
	UInt64					nPacketsReceived;
	UInt64					nBytesSent;
	UInt64					nBytesReceived;
	UInt64					nBroadcastPackets;
	UInt64					nBroadcastBytes;
	UInt64					nMulticastPackets;
	UInt64					nMulticastBytes;
	UInt16					nMinPacketSizeSent;
	UInt16					nMaxPacketSizeSent;
	UInt16					nMinPacketSizeReceived;
	UInt16					nMaxPacketSizeReceived;
	UInt64					nRetryPackets;
	UInt64					nWEPPackets;
	UInt64					nBeaconPackets;
	UInt64					nWEPICVErrors;
	UInt8					ESSID[32];
	UInt8					nESSIDSize;			
	UInt8					nType;
	UInt8					nEncryptionType;
	UInt8					nAuthenticationType;
	WirelessChannel			DSChannel;
	WirelessChannel			APChannel;
	UInt8					nWEPKey;
	UInt8					nTrust;
	UInt8					bBeaconsContainESSID;
	UInt8					bAdHocBaseStation;
	UInt8					bPrivacyFlag;
	UInt8					bPowerSaveFlag;
	PeekWirelessStrength	Signal;				
	PeekWirelessStrength	Noise;				
	PeekWirelessStrength	Recent;				
	PeekWirelessStrength	Prev;				
	PeekWirelessStrength	RecentSignal;		
	PeekWirelessStrength	PrevSignal;			
	PeekWirelessStrength	RecentNoise;		
	PeekWirelessStrength	PrevNoise;			
	UInt64					nPrevResetTime;		
	UInt64					nRecentPacketCount;	
	UInt64					nRoamTimeAccumulator;
	UInt32					nRoamTimeSampleCount;
	UInt32					nEntityIndex;
	UInt32					nRecentBSSIDStatIndex;		///< Used internally.
	UInt32					nRecentESSIDStatIndex;		///< Used internally.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekWirelessNodeDataRateStat
/// \brief TODO.
/// \see IWirelessNodeStats
#include "hepushpack.h"
struct PeekWirelessNodeDataRateStat
{
	UInt32			nRate;
	UInt64			nPackets;
	UInt64			nBytes;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \struct PeekWirelessNodeStatEntry
/// \brief An entry in the wireless node stats
/// \see IWirelessNodeStats
#include "hepushpack.h"
struct PeekWirelessNodeStatEntry
{
	UInt32					nEntityIndex;				///< Entity table index for the node.
	UInt32					nNextIndex;					///< Used internally.
	UInt32					nRecentBSSIDStatIndex;		///< Used internally.
	UInt32					nRecentESSIDStatIndex;		///< Used internally.
	PeekWirelessNodeStat	Stat;						///< Statistics for this node.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \enum PeekWirelessAssociationStrength
/// \brief TODO.
/// \see PeekWirelessNodeStat, IWirelessNodeStats
enum PeekWirelessAssociationStrength
{
	peekWirelessAssociationStrengthUnknown,
	peekWirelessAssociationStrengthWeak,
	peekWirelessAssociationStrengthStrong
};

/// \internal
/// \struct PeekWirelessNodeAssociation
/// \brief An entry in the wireless node association table.
/// \see IWirelessNodeAssociationTable
#include "hepushpack.h"
struct PeekWirelessNodeAssociation
{
	UInt32		nESSIDIndex;	///< Entity table index for the ESSID.
	UInt32		nBSSIDIndex;	///< Entity table index for the BSSID.
	UInt32		nNodeIndex;		///< IWirelessNodeStats2 stat index for the node.
	UInt32		nStrength;		///< A PeekWirelessAssociationStrength enum value.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IWirelessNodeAssociationTable
/// \brief Wireless node association table.
/// \see coclass IWirelessNodeStats
#define IWirelessNodeAssociationTable_IID \
{ 0x4E510F8B, 0xBE17, 0x422B, {0xB5, 0xBE, 0x8B, 0xB8, 0x34, 0xE7, 0x34, 0x02} }

class HE_NO_VTABLE IWirelessNodeAssociationTable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessNodeAssociationTable_IID)

	/// Get the number of entries in the table.
	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	/// Get an item in the table by index.
	HE_IMETHOD Item(/*in*/ UInt32 nIndex, /*out*/ PeekWirelessNodeAssociation* pItem) = 0;
};

/// \internal
/// \struct PeekWirelessNodeStatTotalCounts
/// \brief Total counts for wireless node stats.
/// \see IWirelessNodeStats
#include "hepushpack.h"
struct PeekWirelessNodeStatTotals
{
	UInt32		nBSSNetworks;
	UInt32		nWeakBSSNetworks;
	UInt32		nAdHocNetworks;
	UInt32		nAPs;
	UInt32		nClients;
	UInt32		nBSSIDTrusted;
	UInt32		nBSSIDKnown;
	UInt32		nBSSIDUnknown;
	UInt32		nAdHocBaseStationTrusted;
	UInt32		nAdHocBaseStationKnown;
	UInt32		nAdHocBaseStationUnknown;
	UInt32		nClientTrusted;
	UInt32		nClientKnown;
	UInt32		nClientUnknown;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \internal
/// \interface IWirelessNodeStats
/// \brief Wireless Node statistics.
/// \see coclass WirelessNodeStats
#define IWirelessNodeStats_IID \
{ 0x48FB1569, 0x2FE1, 0x47A3, {0x9E, 0x5E, 0xD0, 0x8E, 0xCB, 0x47, 0x36, 0xAC} }

class HE_NO_VTABLE IWirelessNodeStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessNodeStats_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Get the wireless node association table.
	HE_IMETHOD GetNodeAssociationTable(/*out*/ IWirelessNodeAssociationTable** ppNodeAssociationTable) = 0;
	/// Get the number of stats available.
	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	/// Get the number of date rate changes.
	HE_IMETHOD GetDataRateChanges(/*out*/ UInt32* pnChanges) = 0;
	/// Get the index of the 'unknown' BSSID entry.
	HE_IMETHOD GetBSSIDUnknownIndex(/*out*/ UInt32* pnBSSIDUnknownIndex ) = 0;
	/// Get the index of the 'unknown' ESSID entry.
	HE_IMETHOD GetESSIDUnknownIndex(/*out*/ UInt32* pnESSIDUnknownIndex ) = 0;
	/// Find a wireless node stat.
	HE_IMETHOD Find(/*in*/ UInt32 nEntityIndex, /*out*/ UInt32* pnStatIndex, /*out*/ BOOL* pbFound) = 0;
	/// Get a stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 nStatIndex, /*out*/ PeekWirelessNodeStatEntry* pStatEntry) = 0;
	/// Get general wireless stats.
	HE_IMETHOD GetTotals(/*out*/ PeekWirelessNodeStatTotals* pTotals) = 0;
	/// Get the array of data rates.
	HE_IMETHOD GetDataRates(/*out*/ UInt32** ppRates, /*out*/ UInt32* pnRates) = 0;
	/// Get an array of data rates for a specific index.
	HE_IMETHOD IndexToDataRateStats(/*in*/ UInt32 nStatIndex,
		/*out*/ PeekWirelessNodeDataRateStat** ppDataRates, /*out*/ UInt32* pnRateCount) = 0;
};

/// \internal
/// \interface IWirelessNodeStats2
/// \brief Wireless Node statistics. Includes more signal data than IWirelessNodeStats.
/// \see coclass WirelessNodeStats2
#define IWirelessNodeStats2_IID \
{ 0x9CD33EEB, 0x603A, 0x4D1F, {0x92, 0x49, 0x8C, 0x4E, 0x31, 0xDE, 0xD0, 0xF7} }

class HE_NO_VTABLE IWirelessNodeStats2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessNodeStats2_IID)

	static const UInt32 StatsIndex_Invalid = static_cast<UInt32>(-1);

	/// Get the entity table.
	HE_IMETHOD GetEntityTable(/*out*/ IEntityTable** ppEntityTable) = 0;
	/// Get the wireless node association table.
	HE_IMETHOD GetNodeAssociationTable(/*out*/ IWirelessNodeAssociationTable** ppNodeAssociationTable) = 0;
	/// Get the number of stats available.
	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	/// Get the number of date rate changes.
	HE_IMETHOD GetDataRateChanges(/*out*/ UInt32* pnChanges) = 0;
	/// Get the index of the 'unknown' BSSID entry.
	HE_IMETHOD GetBSSIDUnknownIndex(/*out*/ UInt32* pnIndex) = 0;
	/// Get the index of the 'unknown' ESSID entry.
	HE_IMETHOD GetESSIDUnknownIndex(/*out*/ UInt32* pnIndex) = 0;
	/// Find a wireless node stat.
	HE_IMETHOD Find(/*in*/ UInt32 nEntityIndex, /*out*/ UInt32* pnStatIndex, /*out*/ BOOL* pbFound) = 0;
	/// Get a stat entry by index.
	HE_IMETHOD IndexToStats(/*in*/ UInt32 nStatIndex, /*out*/ PeekWirelessNodeStat2* pStatEntry) = 0;
	/// Get general wireless stats.
	HE_IMETHOD GetTotals(/*out*/ PeekWirelessNodeStatTotals* pTotals) = 0;
	/// Get the array of data rates.
	HE_IMETHOD GetDataRates(/*out*/ UInt32** ppRates, /*out*/ UInt32* pnRates) = 0;
	/// Get an array of data rates for a specific index.
	HE_IMETHOD IndexToDataRateStats(/*in*/ UInt32 nStatIndex,
		/*out*/ PeekWirelessNodeDataRateStat** ppDataRates, /*out*/ UInt32* pnRateCount) = 0;
};

/// \enum PeekSummaryStatType
/// \brief Summary stats value types.
/// \see PeekSummaryStat
enum PeekSummaryStatType
{
	peekSummaryStatTypeNull,
	peekSummaryStatTypeDate,
	peekSummaryStatTypeTime,
	peekSummaryStatTypeDuration,
	peekSummaryStatTypePackets,
	peekSummaryStatTypeBytes,
	peekSummaryStatTypeValuePair,
	peekSummaryStatTypeInt,
	peekSummaryStatTypeDouble
};

/// \enum PeekSummaryStatFlags
/// \brief Flags for summary stats items.
/// \see PeekSummaryStat2
enum PeekSummaryStatFlags
{
	peekSummaryStatFlagUngraphable = 0x00000001,	///< This item should not be graphed.
	peekSummaryStatFlagOverTime    = 0x00000002,	///< This item is an average over time.
	peekSummaryStatFlagSampled     = 0x00000004,	///< This item is a sample per second.
	peekSummaryStatsSortSubItems   = 0x00000008,	///< If this item has subitems, sort them alphabetically.
	peekSummaryStatFlagCounter     = 0x00000010		///< This item is a counter that should be reduced using differences.
};

/// \struct PeekSummaryStatValuePair
/// \brief Summary statistic value pair.
///
/// This structure defines a summary statistic value pair which consists
/// of a packet and byte count.
/// \see PeekSummaryStat
#include "hepushpack.h"
struct PeekSummaryStatValuePair
{
	UInt64	Packets;	///< Packet count.
	UInt64	Bytes;		///< Byte count.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \union PeekSummaryStatValue
/// \brief Summary statistic value.
///
/// This union defines the possible summary statistic value formats.
/// \see PeekSummaryStat
#include "hepushpack.h"
union PeekSummaryStatValue
{
	UInt64						Date;			///< Date in nanoseconds since midnight 1/1/1601 UTC.
	UInt64						Time;			///< Time in nanoseconds since midnight 1/1/1601 UTC.
	SInt64						Duration;		///< Duration in nanoseconds since midnight 1/1/1601 UTC.
	UInt64						Packets;		///< Packet count.
	UInt64						Bytes;			///< Byte count.
	PeekSummaryStatValuePair	ValuePair;		///< Packet count/byte count pair.
	UInt64						IntValue;		///< Integer value.
	double						DoubleValue;	///< Double value.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct PeekSummaryStat
/// \brief Summary statistic.
///
/// This structure defines the format of a summary statistic.
/// \see ISummaryStatsSnapshot
#include "hepushpack.h"
struct PeekSummaryStat
{
	PeekSummaryStatType		ValueType;	///< Type of data contained in Value.
	PeekSummaryStatValue	Value;		///< The statistic value.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \struct PeekSummaryStat2
/// \brief Summary statistic.
///
/// This structure defines the format of a summary statistic.
/// \see ISummaryStatsSnapshot2
#include "hepushpack.h"
struct PeekSummaryStat2
{
	PeekSummaryStatType		Type;	///< Type of data contained in Value.
	UInt32					Flags;	///< Item flags. \see PeekSummaryStatFlags
	PeekSummaryStatValue	Value;	///< The statistic value.
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \interface ISummaryStatsItem
/// \brief Summary statistics item.
#define ISummaryStatsItem_IID \
{ 0x02D2E8B9, 0xAA0E, 0x42E9, {0xBA, 0xD8, 0xB9, 0xD3, 0xA2, 0x84, 0xC2, 0x8B} }

class HE_NO_VTABLE ISummaryStatsItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStatsItem_IID)

	/// Get the item ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Get the item parent ID.
	HE_IMETHOD GetParent(/*out*/ Helium::HeID* pID) = 0;
	/// Get the item label.
	HE_IMETHOD GetLabel(/*out*/ Helium::HEBSTR* pbstrLabel) = 0;
	/// Set the item label.
	HE_IMETHOD SetLabel(/*in*/ Helium::HEBSTR bstrLabel) = 0;
	/// Get the item value.
	HE_IMETHOD GetValue(/*out*/ PeekSummaryStat* pValue) = 0;
	/// Set the item value.
	HE_IMETHOD SetValue(/*in*/ const PeekSummaryStat* pValue) = 0;
};

/// \interface ISummaryStatsItem2
/// \brief Summary statistics item.
#define ISummaryStatsItem2_IID \
{ 0xDD8D92E1, 0xF7A5, 0x4EA9, {0xAC, 0x28, 0x9F, 0xD0, 0x68, 0x27, 0x44, 0x8C} }

class HE_NO_VTABLE ISummaryStatsItem2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStatsItem2_IID)

	/// Get the item ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Get the item parent ID.
	HE_IMETHOD GetParent(/*out*/ Helium::HeID* pID) = 0;
	/// Get the item label.
	HE_IMETHOD GetLabel(/*out*/ Helium::HEBSTR* pbstrLabel) = 0;
	/// Set the item label.
	HE_IMETHOD SetLabel(/*in*/ Helium::HEBSTR bstrLabel) = 0;
	/// Get the item value.
	HE_IMETHOD GetValue(/*out*/ PeekSummaryStat2* pValue) = 0;
	/// Set the item value.
	HE_IMETHOD SetValue(/*in*/ const PeekSummaryStat2* pValue) = 0;
};

/// \interface ISummaryStatsSnapshot
/// \brief Summary statistics snapshot.
///
/// This interface is implemented by the Omni Engine and allows items to be added to
/// the summary statistics display.
#define ISummaryStatsSnapshot_IID \
{ 0xA6783DCF, 0x6C6A, 0x49B3, {0x91, 0x96, 0x15, 0x8F, 0xD9, 0xAE, 0xAD, 0x4B} }

class HE_NO_VTABLE ISummaryStatsSnapshot : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStatsSnapshot_IID)

	/// Get a summary statistics item by index.
	/// \param lIndex Index of the item
	/// \param pID Pointer to a variable that receives the globally unique identifier assigned to the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG The index is out of bounds.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID* pID) = 0;
	/// Get the count of summary statistics items.
	/// \param plCount Pointer to a variable that receives the count of items.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get the summary statistics snapshot name.
	/// \param pbstrName Pointer to a variable that receives a Helium::HEBSTR containing the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the summary statistics snapshot name.
	/// \param bstrName Helium::HEBSTR containing the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the summary statistics snapshot ID.
	/// \param pID Pointer to a variable that receives the globally unique identifier assigned to the snapshot.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the summary statistics snapshot ID. Not normally used by a client.
	/// \param id The globally unique identifier to assign to the snapshot.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& id) = 0;
	/// Test if a summary statistics item exists.
	/// \param id Globally unique identifier of the item.
	/// \param pbResult TRUE if the item exists.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD HasItem(/*in*/ const Helium::HeID& id, /*out*/ BOOL* pbResult) = 0;
	/// Add a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param parent Globally unique identifier of the parent of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD AddItem(/*in*/ const Helium::HeID& id, /*in*/ const Helium::HeID& parent) = 0;
	/// Remove a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD RemoveItem(/*in*/ const Helium::HeID& id) = 0;
	/// Set the properties for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param parent Parent ID of the item.
	/// \param bstrLabel Label for the item. May be NULL.
	/// \param pValue Pointer to the value. May be NULL.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Adds the item if it doesn't already exist.
	HE_IMETHOD SetItem(/*in*/ const Helium::HeID& id, /*in*/ const Helium::HeID& parent,
		/*in*/ Helium::HEBSTR bstrLabel, /*in*/ const PeekSummaryStat* pValue) = 0;
	/// Get the parent ID for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pID Pointer to a variable that receives the globally unique identifier of
	/// the parent of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItemParent(/*in*/ const Helium::HeID& id, /*out*/ Helium::HeID* pID) = 0;
	/// Set the label for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param bstrLabel Helium::HEBSTR containing the label of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetItemLabel(/*in*/ const Helium::HeID& id, /*in*/ Helium::HEBSTR bstrLabel) = 0;
	/// Get the label for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pbstrLabel Pointer to a variable that receives the Helium::HEBSTR containing the label.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItemLabel(/*in*/ const Helium::HeID& id, /*out*/ Helium::HEBSTR* pbstrLabel) = 0;
	/// Set the value for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pValue Pointer to the value.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD SetItemValue(/*in*/ const Helium::HeID& id, /*in*/ const PeekSummaryStat* pValue) = 0;
	/// Get the value for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pValue Pointer to a variable that receives the value.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItemValue(/*in*/ const Helium::HeID& id, /*out*/ PeekSummaryStat* pValue) = 0;
	/// Remove all items.
	HE_IMETHOD Reset() = 0;
};

/// \interface ISummaryStatsSnapshot2
/// \brief Summary statistics snapshot.
///
/// This interface is implemented by the Omni Engine and allows items to be added to
/// the summary statistics display.
#define ISummaryStatsSnapshot2_IID \
{ 0xA1954F91, 0xEB32, 0x4E00, {0x9D, 0x17, 0xEB, 0x8F, 0x32, 0xCE, 0x86, 0xFD} }

class HE_NO_VTABLE ISummaryStatsSnapshot2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStatsSnapshot2_IID)

	/// Get the summary statistics snapshot name.
	/// \param pbstrName Pointer to a variable that receives a Helium::HEBSTR containing the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the summary statistics snapshot name.
	/// \param bstrName Helium::HEBSTR containing the name.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the summary statistics snapshot ID.
	/// \param pID Pointer to a variable that receives the globally unique identifier assigned to the snapshot.
	/// \retval HE_S_OK Success
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the summary statistics snapshot ID. Not normally used by a client.
	/// \param id The globally unique identifier to assign to the snapshot.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& id) = 0;
	/// Get the count of summary statistics items.
	/// \param plCount Pointer to a variable that receives the count of items.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get a summary statistics item by index.
	/// \param id Identifier of the item.
	/// \param ppStatsItem Pointer to a ISummaryStatsItem interface pointer.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG The index is out of bounds.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetStatsItem(/*in*/ const Helium::HeID& id, /*out*/ ISummaryStatsItem2** ppStatsItem) = 0;
	/// Get a summary statistics item by index.
	/// \param lIndex Index of the item
	/// \param pID Pointer to a variable that receives the globally unique identifier assigned to the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_ARG The index is out of bounds.
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID* pID) = 0;
	/// Test if a summary statistics item exists.
	/// \param id Globally unique identifier of the item.
	/// \param pbResult TRUE if the item exists.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD HasItem(/*in*/ const Helium::HeID& id, /*out*/ BOOL* pbResult) = 0;
	/// Add a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param parent Globally unique identifier of the parent of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD AddItem(/*in*/ const Helium::HeID& id, /*in*/ const Helium::HeID& parent) = 0;
	/// Remove a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD RemoveItem(/*in*/ const Helium::HeID& id) = 0;
	/// Set the properties for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param parent Parent ID of the item.
	/// \param bstrLabel Label for the item. May be NULL.
	/// \param pValue Pointer to the value. May be NULL.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks Adds the item if it doesn't already exist.
	HE_IMETHOD SetItem(/*in*/ const Helium::HeID& id, /*in*/ const Helium::HeID& parent,
		/*in*/ Helium::HEBSTR bstrLabel, /*in*/ const PeekSummaryStat2* pValue) = 0;
	/// Get the parent ID for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pID Pointer to a variable that receives the globally unique identifier of
	/// the parent of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItemParent(/*in*/ const Helium::HeID& id, /*out*/ Helium::HeID* pID) = 0;
	/// Set the label for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param bstrLabel Helium::HEBSTR containing the label of the item.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \remarks None
	HE_IMETHOD SetItemLabel(/*in*/ const Helium::HeID& id, /*in*/ Helium::HEBSTR bstrLabel) = 0;
	/// Get the label for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pbstrLabel Pointer to a variable that receives the Helium::HEBSTR containing the label.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItemLabel(/*in*/ const Helium::HeID& id, /*out*/ Helium::HEBSTR* pbstrLabel) = 0;
	/// Set the value for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pValue Pointer to the value.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD SetItemValue(/*in*/ const Helium::HeID& id, /*in*/ const PeekSummaryStat2* pValue) = 0;
	/// Get the value for a summary statistics item.
	/// \param id Globally unique identifier of the item.
	/// \param pValue Pointer to a variable that receives the value.
	/// \retval HE_S_OK Success
	/// \retval HE_E_FAIL Error
	/// \retval HE_E_INVALID_POINTER The pointer is invalid.
	/// \remarks None
	HE_IMETHOD GetItemValue(/*in*/ const Helium::HeID& id, /*out*/ PeekSummaryStat2* pValue) = 0;
	/// Remove all items.
	HE_IMETHOD Reset() = 0;
};

/// \interface ISummaryStats
/// \brief Summary statistics.
/// \see SummaryStats
#define ISummaryStats_IID \
{ 0xBE043620, 0x056A, 0x4529, {0x80, 0x7F, 0x14, 0x5C, 0xBF, 0xB2, 0xF5, 0x0B} }

class HE_NO_VTABLE ISummaryStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStats_IID)

	/// Get a snapshot by index. Note: the current snapshot is always index zero.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ ISummaryStatsSnapshot** ppSnapshot) = 0;
	/// Get the count of snapshots.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get a snapshot by ID.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& guidID, /*out*/ ISummaryStatsSnapshot** ppSnapshot) = 0;
	/// Add a snapshot.
	HE_IMETHOD Add(/*in*/ ISummaryStatsSnapshot* pSnapshot) = 0;
	/// Remove a snapshot.
	HE_IMETHOD Remove(/*in*/ ISummaryStatsSnapshot* pSnapshot) = 0;
};

/// \interface ISummaryStats2
/// \brief Summary statistics.
/// \see SummaryStats
#define ISummaryStats2_IID \
{ 0x3EBE40E4, 0xEBC4, 0x4FC9, {0xBC, 0xD9, 0x4F, 0xDA, 0x4A, 0x6C, 0x0F, 0xB4} }

class HE_NO_VTABLE ISummaryStats2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStats2_IID)

	/// Get a snapshot by index. Note: the current snapshot is always index zero.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ ISummaryStatsSnapshot2** ppSnapshot) = 0;
	/// Get the count of snapshots.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get a snapshot by ID.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& guidID, /*out*/ ISummaryStatsSnapshot2** ppSnapshot) = 0;
	/// Add a snapshot.
	HE_IMETHOD Add(/*in*/ ISummaryStatsSnapshot2* pSnapshot) = 0;
	/// Remove a snapshot.
	HE_IMETHOD Remove(/*in*/ ISummaryStatsSnapshot2* pSnapshot) = 0;
};

/// \internal
/// \interface ISummaryStatsTracker
/// \brief Summary statistics tracking item.
#define ISummaryStatsTracker_IID \
{ 0x191C72A3, 0x290C, 0x410A, {0xA8, 0xFA, 0x5A, 0x28, 0xDF, 0xB7, 0x03, 0x11} }

class HE_NO_VTABLE ISummaryStatsTracker : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStatsTracker_IID)

	/// Get the item ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the item ID.
	HE_IMETHOD SetID(/*in*/ Helium::HeID ID) = 0;
	/// Get the stat type.
	HE_IMETHOD GetType(/*out*/ PeekSummaryStatType* pType) = 0;
	/// Set the stat type.
	HE_IMETHOD SetType(/*in*/ PeekSummaryStatType type) = 0;
	/// Get the stat flags.
	HE_IMETHOD GetFlags(/*out*/ UInt32* pFlags) = 0;
	/// Set the stat flags.
	HE_IMETHOD SetFlags(/*in*/ UInt32 flags) = 0;
};

/// \internal
/// \enum PeekNodeStatType
/// \brief Node stats value types.
/// \see PeekNodeStat
enum PeekNodeStatType
{
	peekNodeStatTypeNull,
	peekNodeStatTypeTotalBytes,					///< Calculated.
	peekNodeStatTypeTotalPackets,				///< Calculated.
	peekNodeStatTypePacketsSent,				///< Total packets sent by this node.
	peekNodeStatTypeBytesSent,					///< Total bytes sent by this node.
	peekNodeStatTypePacketsReceived,			///< Total packets received by this node.
	peekNodeStatTypeBytesReceived,				///< Total bytes received by this node.
	peekNodeStatTypeBroadcastPackets,			///< Broadcast packets sent by this node.
	peekNodeStatTypeBroadcastBytes,				///< Broadcast bytes sent by this node.
	peekNodeStatTypeMulticastPackets,			///< Multicast packets sent by this node.
	peekNodeStatTypeMulticastBytes,				///< Multicast bytes sent by this node.
	peekNodeStatTypeBroadcastMulticastPackets,	///< Calculated.
	peekNodeStatTypeBroadcastMulticastBytes,	///< Calculated.
	peekNodeStatTypeMinPacketSizeSent,			///< Minimum size packet sent by this node in bytes.
	peekNodeStatTypeMaxPacketSizeSent,			///< Maximum size packet sent by this node in bytes.
	peekNodeStatTypeMinPacketSizeReceived,		///< Minimum size packet received by this node in bytes.
	peekNodeStatTypeMaxPacketSizeReceived		///< Maximum size packet received by this node in bytes.
};

/// \internal
/// \interface INodeStatsTracker
/// \brief Node statistics tracking item.
#define INodeStatsTracker_IID \
{ 0xB76C08C6, 0x0E7F, 0x4fc8, {0x9C, 0xB6, 0xC8, 0x9D, 0x11, 0x21, 0x16, 0x18} }

class HE_NO_VTABLE INodeStatsTracker : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INodeStatsTracker_IID)

	/// Get the item's media spec.
	HE_IMETHOD GetMediaSpec(/*out*/ TMediaSpec* pSpec) = 0;
	/// Set the item media spec.
	HE_IMETHOD SetMediaSpec(/*in*/ TMediaSpec MediaSpec) = 0;
	/// Get the stat type.
	HE_IMETHOD GetType(/*out*/ PeekNodeStatType* pType) = 0;
	/// Set the stat type.
	HE_IMETHOD SetType(/*in*/ PeekNodeStatType type) = 0;
};

/// \internal
/// \enum PeekProtocolStatType
/// \brief Protocol stats value types.
/// \see PeekProtocolStat
enum PeekProtocolStatType
{
	peekProtocolStatTypeNull,		///< None
	peekProtocolStatTypePackets,	///< Total packets using this protocol.
	peekProtocolStatTypeBytes		///< Total bytes using this protocol.
};

/// \internal
/// \interface IProtocolStatsTracker
/// \brief Protocol statistics tracking item.
#define IProtocolStatsTracker_IID \
{ 0x7F5E5B64, 0x2E8A, 0x45c2, {0xB6, 0x8B, 0x23, 0x28, 0x45, 0xFE, 0xD1, 0x2A} }

class HE_NO_VTABLE IProtocolStatsTracker : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProtocolStatsTracker_IID)

	/// Get the item's media spec.
	HE_IMETHOD GetMediaSpec(/*out*/ TMediaSpec* pSpec) = 0;
	/// Set the item media spec.
	HE_IMETHOD SetMediaSpec(/*in*/ TMediaSpec MediaSpec) = 0;
	/// Get the stat type.
	HE_IMETHOD GetType(/*out*/ PeekProtocolStatType* pType) = 0;
	/// Set the stat type.
	HE_IMETHOD SetType(/*in*/ PeekProtocolStatType type) = 0;
};

/// \internal
/// \enum PeekWirelessChannelStatType
/// \brief PeekWirelessChannelStatType value types.
/// \see PeekWirelessChannelStat
enum PeekWirelessChannelStatType
{
	peekWirelessChannelStatTypeNull,
	peekWirelessChannelStatTypeAPs,
	peekWirelessChannelStatTypeTotalPackets,
	peekWirelessChannelStatTypeTotalBytes,
	peekWirelessChannelStatTypeDataPackets,
	peekWirelessChannelStatTypeDataBytes,
	peekWirelessChannelStatTypeMgmtPackets,
	peekWirelessChannelStatTypeMgmtBytes,
	peekWirelessChannelStatTypeCtrlPackets,
	peekWirelessChannelStatTypeCtrlBytes,
	peekWirelessChannelStatTypeLocalPackets,
	peekWirelessChannelStatTypeLocalBytes,
	peekWirelessChannelStatTypeFromDSPackets,
	peekWirelessChannelStatTypeFromDSBytes,
	peekWirelessChannelStatTypeToDSPackets,
	peekWirelessChannelStatTypeToDSBytes,
	peekWirelessChannelStatTypeDStoDSPackets,
	peekWirelessChannelStatTypeDStoDSBytes,
	peekWirelessChannelStatTypeRetryPackets,
	peekWirelessChannelStatTypeRetryBytes,
	peekWirelessChannelStatTypeProtectedPackets,
	peekWirelessChannelStatTypeProtectedBytes,
	peekWirelessChannelStatTypeOrderPackets,
	peekWirelessChannelStatTypeOrderBytes,
	peekWirelessChannelStatTypeCRCErrorsPackets,
	peekWirelessChannelStatTypeCRCErrorsBytes,
	peekWirelessChannelStatTypeWEPICVPackets,
	peekWirelessChannelStatTypeWEPICVBytes,
	peekWirelessChannelStatTypeMinSignal,
	peekWirelessChannelStatTypeMaxSignal,
	peekWirelessChannelStatTypeMinSignaldBm,
	peekWirelessChannelStatTypeMaxSignaldBm,
	peekWirelessChannelStatTypeMinNoise,
	peekWirelessChannelStatTypeMaxNoise,
	peekWirelessChannelStatTypeMinNoisedBm,
	peekWirelessChannelStatTypeMaxNoisedBm,
	peekWirelessChannelStatCount,
	// anything else might be rates in bytes or packets
	peekWirelessChannelStatTypeRateBytes = 0x80000000,
	peekWirelessChannelStatTypeRatePackets = 0x40000000
};

/// \internal
/// \interface IWirelessChannelStatsTracker
/// \brief Wireless channel statistics tracking item.
#define IWirelessChannelStatsTracker_IID \
{ 0x1EBB8EF6, 0x9998, 0x4881, {0xB5, 0xBD, 0x2C, 0xDE, 0xA3, 0x4D, 0x2B, 0xEB} }

class HE_NO_VTABLE IWirelessChannelStatsTracker : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessChannelStatsTracker_IID)

	/// Get the item's channel
	HE_IMETHOD GetChannel(/*out*/ WirelessChannel* pChannel) = 0;
	/// Set the item WirelessChannel.
	HE_IMETHOD SetChannel(/*in*/ WirelessChannel channel) = 0;
	/// Get the stat type.
	HE_IMETHOD GetType(/*out*/ PeekWirelessChannelStatType* pType) = 0;
	/// Set the stat type.
	HE_IMETHOD SetType(/*in*/ PeekWirelessChannelStatType type) = 0;
};

enum AlarmConditionType
{
	kAlarmConditionType_Undefined,			///< Unknown
	kAlarmConditionType_Suspect,			///< Suspect condition
	kAlarmConditionType_Problem,			///< Problem condition
	kAlarmConditionType_Resolve				///< Resolve condition
};

enum AlarmComparisonType
{
	kAlarmComparisonType_Undefined,			///< Unknown
	kAlarmComparisonType_LessThan,			///< Less than
	kAlarmComparisonType_LessThanEqaul,		///< Less than or equal to
	kAlarmComparisonType_GreaterThan,		///< Greater than
	kAlarmComparisonType_GreaterThanEqual,	///< Greater than or equal to
	kAlarmComparisonType_Equal,				///< Equal to
	kAlarmComparisonType_NotEqual			///< Not equal to
};

enum AlarmTrackType
{
	kAlarmTrackType_Undefined,				///< Unknown
	kAlarmTrackType_Total,					///< Total
	kAlarmTrackType_Difference,				///< Difference
	kAlarmTrackType_DifferencePerSec		///< Difference per Second
};

/// \internal
/// \interface IAlarmStates
/// \brief An array-like object that you can put/get alarm states
///
/// The alarm states object can get/put the Alarm ID, current state, and severity
#define IAlarmStates_IID \
{ 0x7A10C95A, 0x9CA0, 0x490F, {0x88, 0xF2, 0x44, 0xAE, 0x9B, 0x3B, 0xC2, 0xF4} }

class HE_NO_VTABLE IAlarmStates : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAlarmStates_IID)

	/// Add an alarm state.
	HE_IMETHOD Add(/*in*/ Helium::HeID id, /*in*/ AlarmConditionType curState,
		/*in*/ PeekSeverity severity ) = 0;
	/// Get the count of things here.
	HE_IMETHOD GetCount(/*out*/ SInt32 *plCount ) = 0;
	/// Get the a specific alarm state.
	HE_IMETHOD GetState(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID *pId,
		/*out*/ AlarmConditionType *pCurState, /*out*/ PeekSeverity *pSeverity ) = 0;
};

/// \internal
/// \interface IAlarm
/// \brief An object that watches summary stats through IPeekStatsTracker(s) and notifies on them.
///
/// An alarm contains several conditions, and a stat tracker and action to take for each condition.
#define IAlarm_IID \
{ 0xF8E5C1C3, 0xF2FB, 0x426F, {0xA8, 0x5F, 0xE3, 0xF2, 0x3F, 0xC1, 0x6C, 0xA3} }

class HE_NO_VTABLE IAlarm : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAlarm_IID)

	/// Reset the alarm's state
	HE_IMETHOD Reset() = 0;
	/// Update the alarm's state
	HE_IMETHOD Update(/*in*/ UInt64 ullTimeStamp, /*out*/ BOOL *pbStateChange,
		/*out*/ AlarmConditionType *pCondType) = 0;
	/// Get the alarm ID.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the alarm ID.
	HE_IMETHOD SetID(/*in*/ Helium::HeID id) = 0;
	/// Get creation time
	HE_IMETHOD GetCreated(/*out*/ UInt64 *pullTimestamp) = 0;
	/// Set the creation time
	HE_IMETHOD SetCreated(/*in*/ UInt64 ullTimestamp) = 0;
	/// Get modified time
	HE_IMETHOD GetModified(/*out*/ UInt64 *pullTimestamp) = 0;
	/// Set the modified time
	HE_IMETHOD SetModified(/*in*/ UInt64 ullTimestamp) = 0;
	/// Get the capture context ID
	HE_IMETHOD GetCaptureID(/*out*/ Helium::HeID *pguidCaptureID) = 0;
	/// Set the capture context ID
	HE_IMETHOD SetCaptureID(/*in*/ Helium::HeID guidCaptureID) = 0;
	/// Get the name
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR *pbstrName) = 0;
	/// Set the name
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the tracking type
	HE_IMETHOD GetTrackType(/*out*/ AlarmTrackType *pTrackType) = 0;
	/// Set the tracking type
	HE_IMETHOD SetTrackType(/*in*/ AlarmTrackType trackType) = 0;
	/// Get the StatsTracker
	HE_IMETHOD GetStatsTracker(/*out*/ IPeekStatsTracker **ppStatsTracker) = 0;
	/// Set the StatsTracker
	HE_IMETHOD SetStatsTracker(/*in*/ IPeekStatsTracker* pStatsTracker) = 0;
	/// Get the notifier
	HE_IMETHOD GetNotify(/*out*/ INotify **ppNotify) = 0;
	/// Set the notifier
	HE_IMETHOD SetNotify(/*in*/ INotify *pNotify) = 0;
	/// Get Enable/Disable condition state
	HE_IMETHOD GetEnabledCondition(/*in*/ AlarmConditionType condType, /*out*/ BOOL *pbEnabled) = 0;
	/// Set Enable/Disable condition
	HE_IMETHOD PutEnabledCondition(/*in*/ AlarmConditionType condType, /*in*/ BOOL bEnabled) = 0;
	/// Set the condition
	HE_IMETHOD PutCondition(/*in*/ AlarmConditionType condType,  
		/*in*/ int iDurationSecs,	/*in*/ AlarmComparisonType compType, 
		/*in*/ double dValue,	/*in*/ PeekSeverity severity ) = 0;
	/// Get the condition
	HE_IMETHOD GetCondition(/*in*/ AlarmConditionType condType,
		/*out*/ int *piDurationSecs, /*out*/ AlarmComparisonType *pCompType, 
		/*out*/ double *pdValue, /*out*/ PeekSeverity *pSeverity ) = 0;
	/// Get the state
	HE_IMETHOD GetState(/*out*/ AlarmConditionType *pCondType, /*out*/ PeekSeverity *pSeverity ) = 0;
	/// Get the time this alarm last changed states
	HE_IMETHOD GetLastChange(/*out*/ UInt64 *pullTimestamp ) = 0;
};

/// \internal
/// \interface IAlarmCollection
/// \brief A collection of alarms
/// \see coclass AlarmCollection
#define IAlarmCollection_IID \
{ 0x6674AFB0, 0x76EB, 0x4109, {0x9C, 0x60, 0xB5, 0x79, 0x0E, 0x79, 0x84, 0x1F} }

class HE_NO_VTABLE IAlarmCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAlarmCollection_IID)

	/// Get an alarm by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IAlarm** ppAlarm) = 0;
	/// Get the count of alarms in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get an alarm by ID.
	HE_IMETHOD ItemFromID(/*in*/ Helium::HeID id, /*out*/ IAlarm** ppAlarm) = 0;
	/// Add an alarm.
	HE_IMETHOD Add(/*in*/ IAlarm* pAlarm) = 0;
	/// Remove an alarm.
	HE_IMETHOD Remove(/*in*/ IAlarm* pAlarm) = 0;
	/// Remove all alarms.
	HE_IMETHOD RemoveAll() = 0;
};

/// \internal
/// \interface IPeekStatsServer
/// \brief Server object for regularly getting stats diffs and distributing.
/// \see StatsServer
#define IPeekStatsServer_IID \
{ 0x01A26BEF, 0x5D78, 0x43A6, {0xAF, 0xBE, 0xCF, 0xBE, 0x21, 0xE4, 0xD2, 0xC5} }

class HE_NO_VTABLE IPeekStatsServer : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsServer_IID)

	/// Context (for updating the summary stats)
	HE_IMETHOD SetContext(/*in*/ IPeekStatsContext* pContext) = 0;
	/// Add a stats tracker
	HE_IMETHOD AddTracker(/*in*/ IPeekStatsTracker* pTracker) = 0;
	HE_IMETHOD RemoveTracker(/*in*/ IPeekStatsTracker* pTracker) = 0;
	HE_IMETHOD GetAlarmConfig(/*out*/ IAlarmConfig** ppConfig) = 0;
	HE_IMETHOD RemoveAllAlarms() = 0;
	HE_IMETHOD AddAlarm(/*in*/ IAlarm* pAlarm) = 0;
	HE_IMETHOD GetAlarmStates(/*in*/ IAlarmStates** ppAlarmStates, /*in, out*/ UInt64* pullTimestamp) = 0;
};

#define IPeekStatsContextCollection_IID \
{ 0x5CC63745, 0x00E4, 0x4504, {0xA3, 0xAF, 0xB9, 0x99, 0x76, 0x8B, 0x15, 0x57} }

class HE_NO_VTABLE IPeekStatsContextCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPeekStatsContextCollection_IID)

	HE_IMETHOD GetCount(/*out*/ SInt32* pVal) = 0;
	HE_IMETHOD GetItem(/*in*/ SInt32 Index, /*out*/ IPeekStatsContext** pVal) = 0;
	HE_IMETHOD Add(/*in*/ IPeekStatsContext* StatsContext) = 0;
	HE_IMETHOD RemoveAll() = 0;
};

enum ExpertTableID
{
	EXPERT_TABLE_ID_NONE							=   0,
	EXPERT_TABLE_ID_CLIENT_SERVER					=   1,	///< client-server pairs, stream children, problem children
	EXPERT_TABLE_ID_NODE_PAIRx						=   2,	///< node pairs, stream children, problem children
	EXPERT_TABLE_ID_STREAM							=   3,	///< streams, flat, no children
	EXPERT_TABLE_ID_STREAM_HIERARCHY				=   4,	///< streams, problem children
	EXPERT_TABLE_ID_PROBLEM							=   5,	///< problems, flat, no children
	EXPERT_TABLE_ID_EVENT_LOG						=   6,
	EXPERT_TABLE_ID_EVENT_DESCRIPTIONS				=   9,
	EXPERT_TABLE_ID_EVENT_SETTINGS_GROUPS			=  10,
	EXPERT_TABLE_ID_EVENT_SETTINGS					=  11,
	EXPERT_TABLE_ID_EVENT_COUNTS					=  12,
	EXPERT_TABLE_ID_POLICY_SETTINGS					=  13,
	EXPERT_TABLE_ID_POLICY_ITEMS					=  14,
	EXPERT_TABLE_ID_APPLICATION_HIERARCHY			=  15,
	EXPERT_TABLE_ID_APPLICATION_SERVER_HIERARCHY	=  16,
	EXPERT_TABLE_ID_APPLICATION_CLIENT_HIERARCHY	=  17,
	EXPERT_TABLE_ID_APPLICATION_DESCRIPTIONS		=  18,
	EXPERT_TABLE_ID_SYSTABLES						= 100,
	EXPERT_TABLE_ID_SYSCOLUMNS						= 101,
	EXPERT_TABLE_ID_HEADER_COUNTERS					= 102,
	EXPERT_TABLE_ID_VOIP_MEDIA_FLAT					= 150,
	EXPERT_TABLE_ID_VOIP_CALL						= 151,
	EXPERT_TABLE_ID_VOIP_MEDIA						= 152,
	EXPERT_TABLE_ID_VOIP_CALL_FLOW					= 153,
	EXPERT_TABLE_ID_VOIP_FROM						= 154,
	EXPERT_TABLE_ID_VOIP_SIGNALING					= 155
};

#define IExpertTable_IID \
{ 0x28E57618, 0x8030, 0x49DA, {0x8A, 0x82, 0x6A, 0xE6, 0x63, 0x87, 0x9F, 0x5E} }

class HE_NO_VTABLE IExpertTable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IExpertTable_IID);

	HE_IMETHOD GetColumnCount(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD GetColumnName(/*in*/ UInt32 columnIndex, /*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetColumnUIName(/*in*/ UInt32 columnIndex, /*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetRowCount(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD GetCellString(/*in*/ UInt32 rowIndex, /*in*/ UInt32 columnIndex, /*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD GetCell(/*in*/ UInt32 rowIndex, /*in*/ UInt32 columnIndex, /*out*/ Helium::HEVARIANT* pVal) = 0;
	HE_IMETHOD ColumnIDFromIndex(/*in*/ UInt32 columnIndex, /*out*/ UInt32* pColumnID) = 0;
	HE_IMETHOD ColumnIndexFromID(/*in*/ UInt32 columnID, /*out*/ UInt32* pColumnIndex) = 0;
};

enum ColumnSet
{
	COLUMN_SET_ALL,				///< Every possible column for this table. Can be HUGE and contain all sorts of strange columns you won't understand.
	COLUMN_SET_UI_DEFAULT		///< Approximately the same columns as what would appear in the console if every single UI column was enabled.
};

#define IExpertTableCreator_IID \
{ 0x090D8838, 0x3973, 0x4BE9, {0x9A, 0xB9, 0xE3, 0x7E, 0xA5, 0xB1, 0x3F, 0x46} }

class IExpertTableCreator : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IExpertTableCreator_IID);

	HE_IMETHOD CreateTable(/*in*/ ExpertTableID tableID,
		/*in*/ ColumnSet columnSet, /*out*/ IExpertTable** ppVal) = 0;
};


#define Alarm_CID \
{ 0xE1B48467, 0xCC9F, 0x4C91, {0xA0, 0x30, 0x6C, 0xEA, 0xE1, 0xFD, 0x93, 0x4D} }

#define AlarmCollection_CID \
{ 0x046D5A97, 0x1154, 0x40F1, {0x8A, 0x91, 0x5E, 0x7B, 0x5F, 0x1A, 0xF2, 0xB1} }

#define AlarmStates_CID \
{ 0x72C75BEF, 0x10A9, 0x41C4, {0x88, 0x1E, 0x53, 0xC2, 0x00, 0x1F, 0x5B, 0xC5} }

#define ChannelStats_CID \
{ 0x9E6F756E, 0x4BE7, 0x4E14, {0xAD, 0x2A, 0x72, 0xC8, 0x1C, 0x8D, 0xDC, 0x6C} }

#define ConversationStats_CID \
{ 0x5C91C7DE, 0xD4EB, 0x4AC2, {0x84, 0xF5, 0x84, 0x82, 0x9A, 0x88, 0x37, 0xBF} }

#define DashboardStats_CID \
{ 0x5F520656, 0x9C9D, 0x4897, {0x96, 0x4D, 0xAE, 0x7E, 0x19, 0x96, 0x04, 0xDE} }

#define EntityTable_CID \
{ 0x9817510F, 0xBBE3, 0x4B2B, {0xB3, 0xC7, 0x5B, 0x2B, 0x60, 0xCC, 0x60, 0x66} }

#define ErrorStats_CID \
{ 0xDEA0A0A2, 0x05DF, 0x41F2, {0x8E, 0x87, 0xF9, 0x73, 0x03, 0xD7, 0xD8, 0x49} }

#define HistoryStats_CID \
{ 0x8CAF98A1, 0x69FB, 0x42DA, {0x9D, 0x9A, 0xE8, 0x58, 0xB9, 0xF6, 0xE9, 0x73} }

#define NetworkStats_CID \
{ 0x81A6B979, 0xD9E2, 0x45ED, {0x8E, 0xF7, 0x92, 0x8D, 0x61, 0xC5, 0x33, 0x56} }

#define NodeStats_CID \
{ 0x7FABA337, 0x7DB5, 0x4029, {0xB4, 0x00, 0x40, 0x0D, 0x39, 0xDE, 0xE2, 0xEC} }

#define NodeStatsHierarchy_CID \
{ 0x1338335F, 0x33D7, 0x475C, {0xB2, 0x22, 0xA7, 0xC8, 0xED, 0xAE, 0x1F, 0xCE} }

#define NodeStatsTracker_CID \
{ 0x0EC1390E, 0x3A7D, 0x45B3, {0x98, 0x03, 0x84, 0xD7, 0x4B, 0x5D, 0x5B, 0x80} }

#define PeekStatsContextCollection_CID \
{ 0x96E479E3, 0x364F, 0x41EC, {0xA2, 0x5E, 0x68, 0x96, 0x8D, 0x20, 0x31, 0x5D} }

#define PeekStatsLimitSettings_CID \
{ 0xF7FD1F47, 0xF210, 0x4A95, {0xA9, 0xA9, 0xFC, 0x04, 0x14, 0x14, 0xD3, 0xC2} }

#define ProtocolStats_CID \
{ 0x2D33B8EB, 0xCCFB, 0x4AF8, {0xB0, 0x4D, 0x39, 0xAB, 0x35, 0xEF, 0xB6, 0x77} }

#define ProtocolStatsByID_CID \
{ 0x135C247F, 0x539C, 0x49BB, {0x93, 0x95, 0xCA, 0xB3, 0x23, 0x04, 0x94, 0x4F} }

#define ProtocolStatsTracker_CID \
{ 0xCDC6DD80, 0xC8AC, 0x4968, {0x8A, 0x1C, 0xCC, 0x30, 0x93, 0x2E, 0x2E, 0x84} }

#define RollingNodeStats_CID \
{ 0x1A61492D, 0xA721, 0x4038, {0xAD, 0x4B, 0x3F, 0xA7, 0xA9, 0x91, 0xC9, 0xBC} }

#define SizeStats_CID \
{ 0x9BF9BAF3, 0x85A2, 0x497B, {0x82, 0xB7, 0xBA, 0x36, 0xD6, 0x50, 0xB9, 0x76} }

#define StatsContext_CID \
{ 0xA584414D, 0x9320, 0x4CEE, {0xB4, 0x2E, 0xB5, 0xF5, 0xA1, 0xD2, 0x44, 0x5B} }

#define StatsServer_CID \
{ 0xB8DF30C9, 0x74A3, 0x4377, {0x8B, 0xB1, 0xAF, 0x1B, 0x5F, 0xD8, 0xA9, 0x81} }

#define SummaryStats_CID \
{ 0x54A9726D, 0xEA8F, 0x434A, {0x91, 0x1F, 0x97, 0x50, 0xE3, 0x22, 0x9D, 0xAF} }

#define SummaryStatsSnapshot_CID \
{ 0x09A1C9B3, 0xE44C, 0x495d, {0x90, 0xBF, 0x45, 0xF5, 0x40, 0x03, 0xAF, 0x35} }

#define SummaryStatsTracker_CID \
{ 0x705C4632, 0xEFC9, 0x4395, {0xA6, 0x6A, 0x71, 0xA7, 0xF5, 0x61, 0x8C, 0xC6} }

#define SummaryStatsUpdater_CID \
{ 0xC0D3B240, 0x9937, 0x4FC1, {0x98, 0xA6, 0x1F, 0x37, 0x14, 0x37, 0x3C, 0x5C} }

#define TupleTable_CID \
{ 0x849DC78B, 0x49CB, 0x4E70, {0x81, 0x28, 0x34, 0x9D, 0xC9, 0x51, 0xD7, 0x6B} }

#define WirelessChannelStats_CID \
{ 0x7536F351, 0x57A7, 0x47CF, {0xB1, 0x23, 0x46, 0xFD, 0x9B, 0x91, 0xE2, 0xC2} }

#define WirelessChannelStats2_CID \
{ 0x24A07384, 0x12FF, 0x49B7, {0x9A, 0xD2, 0x42, 0x02, 0x01, 0x68, 0x0C, 0x16} }

#define WirelessChannelStatsTracker_CID \
{ 0x1B352D25, 0xBE6C, 0x408E, {0xAC, 0xB8, 0x0F, 0x08, 0x0F, 0xA0, 0x5B, 0x2A} }

#define WirelessNodeAssociationTable_CID \
{ 0xEF298A48, 0x48CF, 0x43AD, {0x85, 0x3F, 0xF1, 0x60, 0xD1, 0xDD, 0x8A, 0xAB} }

#define WirelessNodeStats_CID \
{ 0x9650A486, 0x60DB, 0x4A84, {0xAF, 0xCC, 0x5F, 0x66, 0xDA, 0x8C, 0xBC, 0xD0} }

#define WirelessNodeStats2_CID \
{ 0x804D54CF, 0x3035, 0x4D0F, {0xAD, 0x6E, 0x9A, 0x8C, 0xCE, 0x78, 0xD6, 0x8C} }

#endif /* PEEKSTATS_H */
