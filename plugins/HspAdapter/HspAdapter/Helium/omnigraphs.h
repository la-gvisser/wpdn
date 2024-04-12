// =============================================================================
//	omnigraphs.h
// =============================================================================
//	Copyright (c) 2004-2015 WildPackets, Inc. All rights reserved.

#ifndef OMNIGRAPHS_H
#define OMNIGRAPHS_H

#include "heunk.h"
#include "peekcore.h"
#include "peekstats.h"

enum GraphItemUnitType
{
	TypeUnknown,
	TypeBytes,
	TypePackets
};

enum GraphItemType
{
	kGraphItemUnknown,
	kGraphItemProtocol,
	kGraphItemNode,
	kGraphItemSummary,
	kGraphItemApplication
};

enum PeekNodeStatsItemType
{
	TypeStatItemUnknown,
	TypeSent,					///< Total packets/Bytes sent by this node.
	TypeReceived,				///< Total packets/bytes received by this node.
	TypeBroadcast,				///< Broadcast packets sent by this node.
	TypeMulticast,				///< Multicast packets sent by this node.
	TypeTotal					///< Total packets/bytes sent/received by this node.
};

#define IGraphItemUnit_IID \
{ 0x69C76F8A, 0xCD6F, 0x4a67, {0x9E, 0xB8, 0xAD, 0x85, 0xB4, 0x00, 0xAB, 0x73} }

class HE_NO_VTABLE IGraphItemUnit : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItemUnit_IID);

	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	HE_IMETHOD GetType(/*out*/ GraphItemUnitType* pType) = 0;
};

#define IGraphItemUnitColl_IID \
{ 0x47A048DE, 0x061F, 0x4DED, {0xB1, 0x1C, 0xD1, 0x3C, 0x61, 0x58, 0x50, 0x86} }

class HE_NO_VTABLE IGraphItemUnitColl : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItemUnitColl_IID);

	HE_IMETHOD GetCount(/*out*/ UInt32* pnCount) = 0;
	HE_IMETHOD Item(/*in*/ UInt32 iItem, /*out*/ IGraphItemUnit** ppItem) = 0;
	HE_IMETHOD Add(/*in*/ IGraphItemUnit* pItem) = 0;
};

#define IGraphItem_IID \
{ 0x3CA86B54, 0xFE0F, 0x48c3, {0xBE, 0xB9, 0x31, 0xA6, 0xD5, 0x0C, 0x44, 0x38} }

class HE_NO_VTABLE IGraphItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItem_IID);

	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetDescription(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetDescription(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetUnits(/*out*/ IGraphItemUnitColl** pVal) = 0;
	HE_IMETHOD GetData(/*in*/ UInt64 StartTime, /*in*/ UInt32* NumSamples, 
		/*in*/ UInt32 SamplesToDrop, /*out*/ double GraphData[], /*out*/ UInt64 TimeStampData[]) = 0;
	HE_IMETHOD AppendData(/*in*/ UInt32 NumSamples, 
		/*in*/ double GraphData[], /*in*/ UInt64 TSData[]) = 0;
	HE_IMETHOD Update(/*in*/ UInt64 TimeStamp, /*in*/ IPeekStatsContext* StatsCtx) = 0;
	HE_IMETHOD GetStartTime(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetSampleInterval(/*out*/ UInt16* pVal) = 0;
	HE_IMETHOD Reset() = 0;
	HE_IMETHOD GetUnitType(/*out*/ GraphItemUnitType* pVal) = 0;
	HE_IMETHOD SetUnitType(/*in*/ GraphItemUnitType newVal) = 0;
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pVal) = 0;
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& newVal) = 0;
	HE_IMETHOD GetType(/*out*/ GraphItemType* pVal) = 0;
	HE_IMETHOD ResetToStartTime(/*in*/ UInt64 ullNewStartTime) = 0;
};

#define IGraphItemData_IID \
{ 0xB5B361A6, 0x7010, 0x41DE, {0x95, 0xB5, 0xDA, 0x7B, 0x46, 0xB1, 0x39, 0xF6} }

class HE_NO_VTABLE IGraphItemData : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItemData_IID);

	HE_IMETHOD GetDataSliced(/*in*/ UInt64 StartTime, /*in*/ UInt32* NumSamples, 
		/*in*/ UInt32 SamplesToDrop, /*out*/ double GraphData[], /*out*/ UInt64 TimeStampData[]) = 0;
	HE_IMETHOD GetDataSampled(/*in*/ UInt64 StartTime, /*in*/ UInt32* NumSamples, 
		/*in*/ UInt32 SamplesToDrop, /*out*/ double GraphData[], /*out*/ UInt64 TimeStampData[]) = 0;
};

#define IProtocolGraphItem_IID \
{ 0xE5EA2D1B, 0x5195, 0x4085, {0xA7, 0x2E, 0x2F, 0xBC, 0x2B, 0xC9, 0x9F, 0x11} }

class HE_NO_VTABLE IProtocolGraphItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProtocolGraphItem_IID);

	HE_IMETHOD Init(/*in*/ GraphItemUnitType unitType,
		/*in*/ UInt32 ProtoSpecInstId) = 0;
	HE_IMETHOD GetProtoSpecInstID(/*out*/ UInt32* pVal) = 0;
};

#define INodeStatGraphItem_IID \
{ 0xC67C6211, 0x116D, 0x4C81, {0xA0, 0xF6, 0xA6, 0xDD, 0x88, 0x9D, 0xD7, 0x98} }

class HE_NO_VTABLE INodeStatGraphItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(INodeStatGraphItem_IID);

	HE_IMETHOD Init(/*in*/ TMediaSpecType mst, /*in*/ const TMediaSpec* pTrackingNode, 
		/*in*/ PeekNodeStatsItemType NodeStatType, /*in*/ GraphItemUnitType UnitType) = 0;
	HE_IMETHOD GetDataType(/*out*/ PeekNodeStatsItemType* pVal) = 0;
	HE_IMETHOD GetMediaSpecType(/*out*/ TMediaSpecType* pVal) = 0;
	HE_IMETHOD GetMediaSpec(/*out*/ TMediaSpec* pVal) = 0;
};

#define ISummaryStatGraphItem_IID \
{ 0xB43EFA26, 0xDF2F, 0x4A9F, {0xA5, 0x21, 0x82, 0xED, 0x3E, 0x62, 0xEF, 0x56} }

class HE_NO_VTABLE ISummaryStatGraphItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISummaryStatGraphItem_IID);

	HE_IMETHOD Init(/*in*/ const Helium::HeID& StatItemId,
		/*in*/ PeekSummaryStatType StatType, /*in*/ UInt32 StatFlags, 
		/*in*/ GraphItemUnitType unitType) = 0;
	HE_IMETHOD GetStatID(/*out*/ Helium::HeID* pVal) = 0;
	HE_IMETHOD GetStatType(/*out*/ PeekSummaryStatType* pVal) = 0;
	HE_IMETHOD GetStatFlags(/*out*/ UInt32* pVal) = 0;
};

#define IApplicationGraphItem_IID \
{ 0x26B7482D, 0x61D, 0x465D, {0x98, 0x8C, 0x60, 0x2F, 0xD5, 0xB8, 0x81, 0x83 }}

class HE_NO_VTABLE IApplicationGraphItem : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IApplicationGraphItem_IID);

	HE_IMETHOD Init(/*in*/ const TMediaSpec* pMediaSpec, 
		/*in*/ GraphItemUnitType UnitType) = 0;
	HE_IMETHOD GetMediaSpec(/*out*/ TMediaSpec* pMediaSpec) = 0;
};

#define IGraphItemUnitPackets_IID \
{ 0xE87F249B, 0x1AB4, 0x4019, {0xBB, 0xB2, 0xEB, 0x62, 0xF4, 0xF4, 0x59, 0x10} }

class HE_NO_VTABLE IGraphItemUnitPackets : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItemUnitPackets_IID);
};

#define IGraphItemUnitBytes_IID \
{ 0xD2314980, 0x25EB, 0x4E2E, {0x80, 0x25, 0xB1, 0xDA, 0x94, 0xDB, 0x8B, 0x50} }

class HE_NO_VTABLE IGraphItemUnitBytes : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItemUnitBytes_IID);
};

#define IGraphItemColl_IID \
{ 0x96763A21, 0xAC65, 0x460A, {0x9D, 0xE4, 0xB1, 0x5E, 0xD9, 0xD9, 0xBE, 0xEE} }

class HE_NO_VTABLE IGraphItemColl : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphItemColl_IID);

	HE_IMETHOD GetCount(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD Add(/*in*/ IGraphItem* pGraphItem) = 0;
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& id) = 0;
	HE_IMETHOD Item(/*in*/ UInt32 iIndex, /*out*/ IGraphItem** ppGraphItem) = 0;
	HE_IMETHOD RemoveAll() = 0;
};

#define ISimpleGraph_IID \
{ 0xC0B544FC, 0xBC2A, 0x4572, {0xB5, 0xFD, 0x0C, 0x30, 0x54, 0x2A, 0x35, 0x0B} }

class HE_NO_VTABLE ISimpleGraph : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISimpleGraph_IID);

	HE_IMETHOD GetTitle(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetTitle(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetDescription(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetDescription(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetTemplateID(/*out*/ Helium::HeID* pVal) = 0;
	HE_IMETHOD SetTemplateID(/*in*/ const Helium::HeID& pVal) = 0;
	HE_IMETHOD AddItem(/*in*/ IGraphItem* NewItem) = 0;
	HE_IMETHOD RemoveItemByID(/*in*/ const Helium::HeID& Id) = 0;
	HE_IMETHOD GetItemColl(/*out*/ IGraphItemColl** ppGraphItemColl) = 0;
	HE_IMETHOD PutItemColl(/*in*/ IGraphItemColl* pGraphItemColl) = 0;
	HE_IMETHOD NewGraph(/*in*/ ISimpleGraph** ppObject) = 0;
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pVal) = 0;
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& pVal) = 0;
	HE_IMETHOD Update(/*in*/ UInt64 CurrentTimeStamp, /*in*/ IPeekStatsContext* StatsCtx) = 0;
	HE_IMETHOD Reset() = 0;
	HE_IMETHOD GetStartTime(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD SetStartTime(/*in*/ UInt64 newVal) = 0;
	HE_IMETHOD GetSamplingInterval(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetSamplingInterval(/*in*/ UInt32 newVal) = 0;
	HE_IMETHOD GetTotalSamples(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetTotalSamples(/*in*/ UInt32 newVal) = 0;
	HE_IMETHOD GetDataCollectionInProgress(/*out*/ Helium::HEVARBOOL* pVal) = 0;
};

#define IStatFileReader_IID \
{ 0x70726219, 0x0470, 0x433C, {0xA4, 0x2D, 0x19, 0x1C, 0x95, 0x1C, 0x57, 0x5D} }

class HE_NO_VTABLE IStatFileReader : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IStatFileReader_IID);

	HE_IMETHOD GetFilenames(/*out*/ Helium::HEBSTR* pVal1, /*out*/ Helium::HEBSTR *pVal2) = 0;
	HE_IMETHOD SetFilenames(/*in*/ Helium::HEBSTR newVal1, /*in*/ Helium::HEBSTR newVal2) = 0;
	HE_IMETHOD Update() = 0;
	HE_IMETHOD Start(/*in*/ UInt64 StartTime, /*in*/ UInt32 SampleInterval) = 0;
	HE_IMETHOD Reset() = 0;
	HE_IMETHOD AddGraph(/*in*/ ISimpleGraph* NewGraph) = 0;
	HE_IMETHOD GetGraphIds(/*in*/ UInt32 Count, /*out*/ Helium::HeID IdList[]) = 0;
	HE_IMETHOD FindGraphById(/*in*/ Helium::HeID Id, /*out*/ ISimpleGraph** Graph, /*out*/ BOOL* Found) = 0;
	HE_IMETHOD DeleteGraphById(/*in*/ Helium::HeID Id, /*out*/ BOOL* Found) = 0;
	HE_IMETHOD SwapFiles(/*in*/ UInt32 ulActiveFile) = 0;
};

#define IGraphCollection_IID \
{ 0x4CF53E64, 0xE4F7, 0x44F2, {0xB9, 0xC4, 0xD1, 0xA1, 0x65, 0xDE, 0x89, 0xFE} }

class HE_NO_VTABLE IGraphCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphCollection_IID);

	HE_IMETHOD Add(/*in*/ ISimpleGraph* pGraph, /*out*/ Helium::HEVARBOOL* pbResult) = 0;
	HE_IMETHOD Delete(/*in*/ ISimpleGraph* pGraph, /*out*/ Helium::HEVARBOOL* pbResult) = 0;
	HE_IMETHOD Item(/*in*/ UInt32 iIndex, /*out*/ ISimpleGraph** ppGraph) = 0;
	HE_IMETHOD GetCount(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD FindByID(/*in*/ const Helium::HeID& GraphId, 
		/*out*/ ISimpleGraph** GraphFound, /*out*/ Helium::HEVARBOOL* pbResult) = 0;	
	HE_IMETHOD GetIsDirty(/*out*/ Helium::HEVARBOOL* pVal) = 0;
	HE_IMETHOD RemoveAll() = 0;
	HE_IMETHOD DeleteByID(/*in*/ const Helium::HeID& idGraph, /*out*/ BOOL* pbResult) = 0;
	HE_IMETHOD FindByTemplateID(/*in*/ const Helium::HeID& idGraph, 
		/*out*/ ISimpleGraph** ppGraph, /*out*/ Helium::HEVARBOOL* pbResult) = 0;	
	HE_IMETHOD DeleteByTemplateID(/*in*/ const Helium::HeID& idGraph, /*out*/ BOOL* pbResult) = 0;
};

#define IGraphTemplateMgr_IID \
{ 0xF7751AEC, 0x98E4, 0x4C60, {0xBB, 0x39, 0xC0, 0xBA, 0x01, 0x7E, 0x95, 0xCC} }

class HE_NO_VTABLE IGraphTemplateMgr : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphTemplateMgr_IID);

	HE_IMETHOD GetTemplateList(/*out*/ IGraphCollection** ppTemplateColl) = 0;
	HE_IMETHOD LoadSupportedStats(/*in*/ Helium::IHeServiceProvider* pServiceProvider) = 0;
	HE_IMETHOD GetDefaultStatsContext(/*out*/ IPeekStatsContext** ppStatsContext) = 0;
};

/// This interface provides settings for STS files
#define ISTSSettings_IID \
{ 0xE42CA11F, 0xB337, 0x4556, {0xA0, 0x52, 0xDC, 0xCF, 0xAD, 0x5C, 0xA1, 0x86} }

class HE_NO_VTABLE ISTSSettings : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISTSSettings_IID);

	HE_IMETHOD GetDataSetName(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetDataSetName(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetDataStoreLocation(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetDataStoreLocation(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetCollectionInterval(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetCollectionInterval(/*in*/ UInt32 newVal) = 0;
	HE_IMETHOD GetKeepFilesDuration(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetKeepFilesDuration(/*in*/ UInt32 newVal) = 0;
	HE_IMETHOD GetKeepAllFiles(/*out*/ BOOL* pVal) = 0;
	HE_IMETHOD SetKeepAllFiles(/*in*/ BOOL newVal) = 0;
};

#define ISTSFileIterator_IID \
{ 0x5D014541, 0xA666, 0x43F2, {0xAE, 0xAD, 0x43, 0x57, 0x46, 0xAA, 0xA4, 0x07} }

class HE_NO_VTABLE ISTSFileIterator : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISTSFileIterator_IID);

	HE_IMETHOD Init(/*in*/ UInt64 StartTime, /*in*/ UInt64 EndTime, 
		/*in*/ Helium::HEBSTR STSFiles[], /*in*/ UInt32 FileCount) = 0;
	HE_IMETHOD GetFirst(/*out*/ IPeekStatsContext** StatsCtx) = 0;
	HE_IMETHOD GetLast(/*out*/ IPeekStatsContext** StatsCtx) = 0;
	HE_IMETHOD Next(/*out*/ UInt64* TimeStamp, /*out*/ IPeekStatsContext** StatsCtx) = 0;
	HE_IMETHOD GetStatItemCount(/*out*/ UInt32* pVal) = 0;
};

/// This interface provides STS file management facilities
#define ISTSFileManager_IID \
{ 0x925F4B48, 0xE86D, 0x4B00, {0x96, 0xB2, 0xA3, 0xBE, 0xBA, 0x86, 0x87, 0x8A} }

class HE_NO_VTABLE ISTSFileManager : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ISTSFileManager_IID);

	HE_IMETHOD Init(/*in*/ ISTSSettings* Settings) = 0;
	HE_IMETHOD GetSettings(/*out*/ ISTSSettings** pVal) = 0;
	HE_IMETHOD ReadDataStore(/*in*/ ISTSSettings* Settings) = 0;
	HE_IMETHOD SaveStats(/*in*/ UInt64 Timestamp, /*in*/ IPeekStatsContext* StatsCtx) = 0;
	HE_IMETHOD ClearDataStore() = 0;
	HE_IMETHOD GetStatsCtxCollection(/*in*/ UInt64* StartTime, /*in*/ UInt64* EndTime, 
		/*out*/ IPeekStatsContextCollection** StatsCtxColl) = 0;
	HE_IMETHOD CloseDataStore() = 0;
	HE_IMETHOD GetStatsCtx(/*in*/ UInt64* Timestamp, /*out*/ IPeekStatsContext** StatsCtx) = 0;
	HE_IMETHOD PurgeOldStats(/*in*/ UInt64 Timestamp) = 0;
	HE_IMETHOD GetStartTime(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetEndTime(/*out*/ UInt64* pVal) = 0;
	HE_IMETHOD GetIterator(/*in*/ UInt64* StartTime, /*in*/ UInt64* EndTime, 
		/*out*/ ISTSFileIterator** FileIterator) = 0;
};

enum GraphDataIntervalUnit
{
	kGraphDataIntervalUnit_Undefined,
	kGraphDataIntervalUnit_Seconds,
	kGraphDataIntervalUnit_Minutes,
	kGraphDataIntervalUnit_Hours,
	kGraphDataIntervalUnit_Days
};

#define IGraphSettings_IID \
{ 0x279C3B07, 0xE4B4, 0x4E68, {0xBD, 0x5F, 0xE1, 0xB9, 0xB0, 0xD4, 0x1B, 0x53} }

class HE_NO_VTABLE IGraphSettings : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphSettings_IID);

	HE_IMETHOD GetEnabled(/*out*/ BOOL* pbEnabled) = 0;
	HE_IMETHOD SetEnabled(/*in*/ BOOL bEnabled) = 0;
	HE_IMETHOD GetWorkingDir(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetWorkingDir(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetFileName(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetFileName(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetOutputInterval(/*out*/ UInt32* pulTime, /*out*/ GraphDataIntervalUnit* pUnit) = 0;
	HE_IMETHOD SetOutputInterval(/*in*/ UInt32 ulTime, /*in*/ GraphDataIntervalUnit unit) = 0;
	HE_IMETHOD ConvertIntervalToSeconds(/*in*/ UInt32 ulInterval,
		/*in*/ GraphDataIntervalUnit unitType, /*out*/ UInt32 *pulIntervalInSeconds) = 0;
	HE_IMETHOD GetIntervalAsSeconds(/*out*/ UInt32 *pulIntervalInSeconds) = 0;
	HE_IMETHOD GetFileCnt(/*out*/ UInt32* pulFileCnt) = 0;
	HE_IMETHOD SetFileCnt(/*in*/ UInt32 ulFileCnt) = 0;
	HE_IMETHOD GetFileBufferSize(/*out*/ UInt32* pulFileBufferSize) = 0;
	HE_IMETHOD SetFileBufferSize(/*in*/ UInt32 ulFileBufferSize) = 0;
	HE_IMETHOD GetPreserveFiles(/*out*/ BOOL* pbPreserveFiles) = 0;
	HE_IMETHOD SetPreserveFiles(/*in*/ BOOL bPreserveFiles) = 0;
	HE_IMETHOD GetItem(/*in*/ SInt32 iIndex, /*out*/ Helium::HeID* pID) = 0;
	HE_IMETHOD GetCount(/*out*/ SInt32* pnCount) = 0;
	HE_IMETHOD Add(/*in*/ const Helium::HeID& tid) = 0;
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& tid) = 0;
	HE_IMETHOD GetMemory(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD SetMemory(/*in*/ UInt32 newVal) = 0;
};

#define IGraphsContext_IID \
{ 0x22AE9384, 0x96C1, 0x4FB2, {0xA4, 0x51, 0x1E, 0x37, 0xF6, 0x60, 0xD5, 0x69} }

class HE_NO_VTABLE IGraphsContext : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphsContext_IID);

	HE_IMETHOD Init(/*in*/ Helium::IHeServiceProvider* EngineServiceProvider, 
		/*in*/ const Helium::HeID& captureId, /*in*/ IGraphSettings* pSettings, 
		/*in*/ IGraphTemplateMgr* pTemplateMgr, /*in*/ IMediaInfo* pMediaInfo) = 0;
	HE_IMETHOD GetGraphs(/*out*/ IGraphCollection** pVal) = 0;
	HE_IMETHOD AddGraph(/*in*/ ISimpleGraph *spGraph, /*out*/ Helium::HEVARBOOL* pResult) = 0;
	HE_IMETHOD DeleteGraphByID(/*in*/ Helium::HeID grphId, /*out*/ Helium::HEVARBOOL* pResult) = 0;
	HE_IMETHOD GetStatsContext(/*in*/ UInt64* StartTime, /*in*/ UInt64* EndTime, 
		/*out*/ IPeekStatsContextCollection** StatsCollection) = 0;
};

#define IGraphableStats_IID \
{ 0x664CA9EB, 0xED64, 0x4256, {0xBC, 0x9A, 0x79, 0x61, 0x8C, 0x79, 0xFE, 0x3F} }

class HE_NO_VTABLE IGraphableStats : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphableStats_IID);

	HE_IMETHOD GetEnabled(/*out*/ BOOL* pVal) = 0;
	HE_IMETHOD SetEnabled(/*in*/ BOOL newVal) = 0;
	HE_IMETHOD GetDescription(/*out*/ Helium::HEBSTR* pVal) = 0;
	HE_IMETHOD SetDescription(/*in*/ Helium::HEBSTR newVal) = 0;
	HE_IMETHOD GetPrimGuid(/*out*/ Helium::HeID* pGuid) = 0;
	HE_IMETHOD SetPrimGuid(/*in*/ const Helium::HeID& objGuid) = 0;
	HE_IMETHOD GetSecGuid(/*out*/ Helium::HeID* pGuid) = 0;
	HE_IMETHOD SetSecGuid(/*in*/ const Helium::HeID& objGuid) = 0;
};

#define IGraphableStatsColl_IID \
{ 0xCD78CE0E, 0xC230, 0x4137, {0xA2, 0x10, 0x65, 0x3B, 0x7B, 0x57, 0xF2, 0x9A} }

class HE_NO_VTABLE IGraphableStatsColl : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IGraphableStatsColl_IID);

	HE_IMETHOD GetCount(/*out*/ UInt32* pVal) = 0;
	HE_IMETHOD Add(/*in*/ IGraphableStats* NewItem) = 0;
	HE_IMETHOD Item(/*in*/ UInt32 ItemIndex, /*out*/ IGraphableStats** ItemData) = 0;
	HE_IMETHOD RemoveAll() = 0;
};

#define StatFileReader_CID \
{ 0x863C1667, 0x37D1, 0x432E, {0xBC, 0x9D, 0xB6, 0x48, 0xC3, 0x5D, 0x45, 0x73} }

#define ProtocolGraphItem_CID \
{ 0xA541981C, 0xE2E4, 0x4BA5, {0x84, 0x29, 0xC7, 0x12, 0x07, 0xEF, 0x9E, 0x6B} }

#define GraphItemUnitPackets_CID \
{ 0xE8A52F20, 0x7A7D, 0x4B66, {0x93, 0x94, 0x74, 0x6F, 0x79, 0x78, 0x96, 0x45} }

#define GraphItemUnitBytes_CID \
{ 0x39A38294, 0x6D1F, 0x4094, {0xB8, 0x4C, 0x8E, 0x29, 0xB7, 0xBB, 0xA1, 0xF2} }

#define GraphItemUnitColl_CID \
{ 0x4C961874, 0xDB5E, 0x417B, {0x90, 0x86, 0x29, 0xE6, 0x61, 0x44, 0x7E, 0x9E} }

#define SimpleGraph_CID \
{ 0x5D41B4CA, 0xDDA7, 0x4733, {0xAF, 0xA8, 0x54, 0x5D, 0xE3, 0xD9, 0x9E, 0x50} }

#define GraphItemColl_CID \
{ 0x244623C9, 0x8CEF, 0x4AEF, {0xBA, 0x5D, 0xAA, 0x20, 0x6A, 0x11, 0xCB, 0xB2} }

#define GraphCollection_CID \
{ 0xCEC24644, 0x35A4, 0x487F, {0x92, 0xB3, 0x71, 0x16, 0x0A, 0x91, 0x3D, 0xF5} }

#define GraphTemplateMgr_CID \
{ 0x5B0E7861, 0x0D62, 0x473C, {0xA6, 0x82, 0x60, 0x8E, 0xA8, 0xB4, 0xF5, 0x1C} }

#define NodeStatGraphItem_CID \
{ 0xB1DAFDE2, 0x68F2, 0x4D35, {0xAA, 0xA7, 0x58, 0x61, 0xB3, 0x5A, 0x3E, 0x9F} }

#define SummaryStatGraphItem_CID \
{ 0x0BDD4628, 0x5AFD, 0x4D26, {0x89, 0x09, 0xEC, 0x37, 0x17, 0x24, 0xF5, 0x1E} }

#define ApplicationGraphItem_CID \
{ 0x588BBDEF, 0x59E8, 0x428E, {0x92, 0x9B, 0x14, 0x22, 0x52, 0x0A, 0x0F, 0x8B} }

#define STSFileManager_CID \
{ 0x4704C282, 0x0444, 0x4BFB, {0x8B, 0xA8, 0x35, 0x3F, 0xA1, 0x00, 0x7C, 0x84} }

#define STSSettings_CID \
{ 0xC2DC3170, 0xD8A8, 0x4408, {0x84, 0xF0, 0xA8, 0x85, 0x5B, 0xD1, 0xE8, 0xA2} }

#define StatItemQuery_CID \
{ 0x2059CC8B, 0x12B1, 0x49BA, {0x90, 0xB0, 0xAF, 0x40, 0xB0, 0xF7, 0x6B, 0x1C} }

#define STSFileIterator_CID \
{ 0xAD528F5F, 0x0866, 0x4E8E, {0xA2, 0xAD, 0xBD, 0x69, 0x0C, 0xD0, 0xF4, 0xC8} }

#define GraphsContext_CID \
{ 0xDA57309A, 0x3C64, 0x4C39, {0xBA, 0x94, 0x8B, 0xA4, 0x3B, 0x23, 0x4D, 0xF9} }

#define GraphSettings_CID \
{ 0x3E3D84CE, 0x563C, 0x486C, {0x8E, 0x6E, 0xAA, 0xDD, 0xD2, 0x97, 0x86, 0xF8} }

#define GraphableStats_CID \
{ 0x4C26454F, 0x5BD4, 0x446E, {0x98, 0x7B, 0x79, 0x95, 0xAD, 0xE5, 0x59, 0x7A} }

#define GraphableStatsColl_CID \
{ 0x42781CCF, 0x0086, 0x4F88, {0x80, 0x2B, 0xCA, 0x1B, 0xE1, 0xD6, 0x4A, 0xA7} }

#endif /* OMNIGRAPHS_H */
