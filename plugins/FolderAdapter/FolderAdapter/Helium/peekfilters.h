// =============================================================================
//	peekfilters.h
// =============================================================================
//	Copyright (c) 2003-2015 WildPackets, Inc. All rights reserved.

#ifndef PEEKFILTERS_H
#define PEEKFILTERS_H

#include "MediaTypes.h"
#include "peekcore.h"

/// \enum FilterNodeType
/// \brief Types of filter nodes.
enum FilterNodeType
{
	filterNodeTypeNull,
	filterNodeTypeAddress,
	filterNodeTypeProtocol,
	filterNodeTypePort,
	filterNodeTypeValue,
	filterNodeTypeError,
	filterNodeTypePattern,
	filterNodeTypeLength,
	filterNodeTypePlugin,
	filterNodeTypeWireless,
	filterNodeTypeChannel,
	filterNodeTypeDirection,
	filterNodeTypeLogical,
	filterNodeTypeBpf,
	filterNodeTypeVlan,
	filterNodeTypeApplication,
	filterNodeTypeTimeRange,
	filterNodeTypeCountry
};

/// \interface IFilterNode
/// \ingroup Filter
/// \brief A filter node
#define IFilterNode_IID \
{ 0xB4298A64, 0x5A40, 0x4F5F, {0xAB, 0xCD, 0xB1, 0x4B, 0xA0, 0xF8, 0xD9, 0xEB} }

class HE_NO_VTABLE IFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterNode_IID)

	/// Get the filter type.
	HE_IMETHOD GetNodeType(/*out*/ FilterNodeType* pNodeType) = 0;
	/// Get the parent node.
	HE_IMETHOD GetParentNode(/*out*/ IFilterNode** ppParentNode) = 0;
	/// Set the parent node.
	HE_IMETHOD SetParentNode(/*in*/ IFilterNode* pParentNode) = 0;
	/// Get the 'and' node.
	HE_IMETHOD GetAndNode(/*out*/ IFilterNode** ppAndNode) = 0;
	/// Set the 'and' node.
	HE_IMETHOD SetAndNode(/*in*/ IFilterNode* pAndNode) = 0;
	/// Get the 'or' node.
	HE_IMETHOD GetOrNode(/*out*/ IFilterNode** ppOrNode) = 0;
	/// Set the 'or' node.
	HE_IMETHOD SetOrNode(/*in*/ IFilterNode* pOrNode) = 0;
	/// Get the filter 'not' flag.
	HE_IMETHOD GetInverted(/*out*/ BOOL* pbInverted) = 0;
	/// Set the filter 'not' flag.
	HE_IMETHOD SetInverted(/*in*/ BOOL bInverted) = 0;
	/// Get the filter node comment.
	HE_IMETHOD GetComment(/*out*/ Helium::HEBSTR* pbstrComment) = 0;
	/// Get the filter node comment.
	HE_IMETHOD SetComment(/*in*/ Helium::HEBSTR bstrComment) = 0;
};

/// \interface IPacketFilter
/// \ingroup Filter
/// \brief A filter
/// \see IPacketFilter
#define IFilter_IID \
{ 0x2FA2507F, 0x1687, 0x4441, {0x8A, 0x70, 0x13, 0xEC, 0xB2, 0x56, 0x36, 0xEF} }

class HE_NO_VTABLE IPacketFilter : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilter_IID)

	/// Get the root filter node.
	HE_IMETHOD GetRootNode(/*out*/ IFilterNode** ppRootNode) = 0;
	/// Get the root filter node.
	HE_IMETHOD SetRootNode(/*in*/ IFilterNode* pRootNode) = 0;
	/// Get the filter id.
	HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	/// Set the filter id.
	HE_IMETHOD SetID(/*in*/ const Helium::HeID& id) = 0;
	/// Get the filter name.
	HE_IMETHOD GetName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	/// Set the filter name.
	HE_IMETHOD SetName(/*in*/ Helium::HEBSTR bstrName) = 0;
	/// Get the filter comment.
	HE_IMETHOD GetComment(/*out*/ Helium::HEBSTR* pbstrComment) = 0;
	/// Set the filter comment.
	HE_IMETHOD SetComment(/*in*/ Helium::HEBSTR bstrComment) = 0;
	/// Get the filter color.
	HE_IMETHOD GetColor(/*out*/ UInt32* pColor) = 0;
	/// Set the filter color.
	HE_IMETHOD SetColor(/*in*/ UInt32 color) = 0;
	/// Get the time the filter was created (PEEK format).
	HE_IMETHOD GetCreated(/*out*/ UInt64* pullTimeStamp) = 0;
	/// Set the time the filter was created (PEEK format).
	HE_IMETHOD SetCreated(/*in*/ UInt64 ullTimeStamp) = 0;
	/// Get the time the filter was last modified (PEEK format).
	HE_IMETHOD GetModified(/*out*/ UInt64* pullTimeStamp) = 0;
	/// Set the time the filter was last modified (PEEK format).
	HE_IMETHOD SetModified(/*in*/ UInt64 ullTimeStamp) = 0;
	/// Get the filter group.
	HE_IMETHOD GetGroup(/*out*/ Helium::HEBSTR* pbstrGroup) = 0;
	/// Set the filter group.
	HE_IMETHOD SetGroup(/*in*/ Helium::HEBSTR bstrGroup) = 0;
};

#define PEEK_E_FILTER_IN_USE			((HeResult)	0x80040230L)	///< Filter is in use.

/// \interface IFilterCollection
/// \ingroup Filter
/// \brief A collection of filters
/// \see coclass FilterCollection
#define IFilterCollection_IID \
{ 0xF14A6908, 0xC7F9, 0x4370, {0x82, 0x9D, 0x2A, 0xB5, 0x84, 0xA3, 0x34, 0xF4} }

class HE_NO_VTABLE IFilterCollection : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterCollection_IID)

	/// Get a filter by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ IPacketFilter** ppFilter) = 0;
	/// Get the count of filters in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Get a filter by id.
	HE_IMETHOD ItemFromID(/*in*/ const Helium::HeID& id, /*out*/ IPacketFilter** ppFilter) = 0;
	/// Add a filter.
	HE_IMETHOD Add(/*in*/ IPacketFilter* pFilter) = 0;
	/// Remove a filter.
	HE_IMETHOD Remove(/*in*/ IPacketFilter* pFilter) = 0;
	/// Remove all filters.
	HE_IMETHOD RemoveAll() = 0;
};

/// \interface IFilterPack
/// \ingroup Filter
/// \brief A package of filters
/// \see coclass FilterPack
#define IFilterPack_IID \
{ 0x4A97BBB6, 0xAD7D, 0x435B, {0xAE, 0x63, 0x70, 0x86, 0x4E, 0xA3, 0x92, 0x1E} }

class HE_NO_VTABLE IFilterPack : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IFilterPack_IID)

	#define PEEK_E_FILTERS_MISSING		((HeResult)	0x80040200L)

	/// Get the filter collection.
	HE_IMETHOD GetFilters(/*out*/ IFilterCollection** ppFilters) = 0;
	/// Set the filter collection.
	HE_IMETHOD SetFilters(/*in*/ IFilterCollection* pFilters) = 0;
	/// Get a filter id by index.
	HE_IMETHOD GetItem(/*in*/ SInt32 lIndex, /*out*/ Helium::HeID* pID) = 0;
	/// Get the count of ids in the list.
	HE_IMETHOD GetCount(/*out*/ SInt32* plCount) = 0;
	/// Add a filter id.
	HE_IMETHOD Add(/*in*/ const Helium::HeID& id) = 0;
	/// Remove a filter id.
	HE_IMETHOD Remove(/*in*/ const Helium::HeID& id) = 0;
	/// Remove all filter ids.
	HE_IMETHOD RemoveAll() = 0;
	/// Find a filter id.
	HE_IMETHOD Find(/*in*/ const Helium::HeID& id, /*out*/ SInt32* plIndex) = 0;
	/// Compile the filters.
	/// Get the media type.
	HE_IMETHOD GetMediaType(/*out*/ TMediaType* pMediaType) = 0;
	/// Set the media type.
	HE_IMETHOD SetMediaType(/*in*/ TMediaType MediaType) = 0;
	/// Get the media sub type.
	HE_IMETHOD GetMediaSubType(/*out*/ TMediaSubType* pMediaSubType) = 0;
	/// Set the media sub type.
	HE_IMETHOD SetMediaSubType(/*in*/ TMediaSubType MediaSubType) = 0;
	HE_IMETHOD Compile() = 0;
	/// Compile the filters with filter mode and return IDs of missing filters.
	HE_IMETHOD Compile(/*in*/ PeekFilterMode inMode, /*out, optional*/ Helium::HeID** ppMissingIDs,
		/*out, optional*/ UInt32* pnMissingIDs) = 0;
};

/// \interface ITimeRangeFilterNode
/// \ingroup Filter
/// \brief A time range filter for packet timestamps
/// \see IFilterNode, coclass TimeRangeFilterNode
#define ITimeRangeFilterNode_IID \
{ 0x406164F6, 0xC753, 0x45FF, {0xA7, 0x9C, 0x87, 0xA8, 0x29, 0xC9, 0x77, 0xE7} }

class HE_NO_VTABLE ITimeRangeFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ITimeRangeFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ UInt64* pStartTime, /*out*/ UInt64* pEndTime) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ UInt64 nStartTime, /*in*/ UInt64 nEndTime) = 0;
};

/// \interface IAddressFilterNode
/// \ingroup Filter
/// \brief An address filter node
/// \see IFilterNode, coclass AddressFilterNode
#define IAddressFilterNode_IID \
{ 0x2D2D9B91, 0x08BF, 0x44CF, {0xB2, 0x40, 0xF0, 0xBB, 0xAD, 0xBF, 0x21, 0xB5} }

class HE_NO_VTABLE IAddressFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAddressFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ TMediaSpec* pAddr1, /*out*/ TMediaSpec* pAddr2,
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const TMediaSpec* pAddr1, /*in*/ const TMediaSpec* pAddr2,
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
};

/// \interface IAddressFilterNode2
/// \ingroup Filter
/// \brief An address filter node that supports multiple addresses
/// \see IFilterNode, coclass PortFilterNode
#define IAddressFilterNode2_IID \
{ 0x8BE8A231, 0x1ADA, 0x4BBE, {0x9E, 0xD7, 0xF2, 0x51, 0x01, 0x97, 0x26, 0xC6} }

class HE_NO_VTABLE IAddressFilterNode2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IAddressFilterNode2_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ TMediaSpecType* pType,
		/*out*/ Helium::HEBSTR* pbstrAddr1, /*out*/ Helium::HEBSTR* pbstrAddr2,
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ TMediaSpecType type,
		/*in*/ Helium::HEBSTR bstrAddr1, /*in*/ Helium::HEBSTR bstrAddr2,
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
};

/// \interface IErrorFilterNode
/// \ingroup Filter
/// \brief An error filter node
/// \see IFilterNode, coclass ErrorFilterNode
#define IErrorFilterNode_IID \
{ 0xD0BDFB3F, 0xF72F, 0x4AEF, {0x8E, 0x17, 0xB1, 0x6D, 0x4D, 0x35, 0x43, 0xFF} }

class HE_NO_VTABLE IErrorFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IErrorFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ UInt8* pucErrorFlags) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ UInt8 ucErrorFlags) = 0;
};

/// \interface ILengthFilterNode
/// \ingroup Filter
/// \brief A length filter node
/// \see IFilterNode, coclass LengthFilterNode
#define ILengthFilterNode_IID \
{ 0xCF190294, 0xC869, 0x4D67, {0x93, 0xF2, 0x9A, 0x53, 0xFD, 0xFA, 0xE7, 0x7D} }

class HE_NO_VTABLE ILengthFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ILengthFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ UInt16* pusMinLength, /*out*/ UInt16* pusMaxLength) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ UInt16 usMinLength, /*in*/ UInt16 usMaxLength) = 0;
};

/// \enum PatternFilterType
/// \brief Pattern filter types for IPatternFilterNode.
enum PatternFilterType
{
	patternFilterTypeAscii,
	patternFilterTypeUnicode,
	patternFilterTypeRawData,
	patternFilterTypeEbcdic,
	patternFilterTypeRegex
};

/// \enum PatternFilterFlags
/// \brief Flags for a pattern filter.
/// \see IPatternFilterNode
enum PatternFilterFlags
{
	patternFilterFlagRelativeToHeader	= 0x01,
	patternFilterFlagRelativeToPayload	= 0x02
};

/// \interface IPatternFilterNode
/// \ingroup Filter
/// \brief A pattern (string) filter node
/// \see IFilterNode, coclass PatternFilterNode
#define IPatternFilterNode_IID \
{ 0x47D49D7C, 0x8219, 0x40D5, {0x9E, 0x5D, 0x8A, 0xDE, 0xAA, 0xCC, 0x64, 0x4D} }

class HE_NO_VTABLE IPatternFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPatternFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ PatternFilterType* ppft,
		/*out*/ UInt8** ppData, /*out*/ UInt32* pulDataBytes,
		/*out*/ UInt16* pusStartOffset, /*out*/ UInt16* pusEndOffset,
		/*out*/ BOOL* pbCaseSensitive) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ PatternFilterType pft,
		/*in*/ const UInt8* pData, /*in*/ UInt32 ulDataBytes,
		/*in*/ UInt16 usStartOffset, /*in*/ UInt16 usEndOffset,
		/*in*/ BOOL bCaseSensitive) = 0;
};

/// \interface IPatternFilterNode2
/// \ingroup Filter
/// \brief A pattern (string) filter node supporting relative offsets
/// \see IFilterNode, coclass PatternFilterNode
#define IPatternFilterNode2_IID \
{ 0x25D861AF, 0xB4B4, 0x4D06, {0xAC, 0x22, 0xEB, 0x1F, 0xFB, 0x2D, 0x86, 0x19} }

class HE_NO_VTABLE IPatternFilterNode2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPatternFilterNode2_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ PatternFilterType* ppft,
		/*out*/ UInt8** ppData, /*out*/ UInt32* pulDataBytes,
		/*out*/ UInt16* pusStartOffset, /*out*/ UInt16* pusEndOffset,
		/*out*/ UInt16* pusPspec, /*out*/ BOOL* pbCaseSensitive,
		/*out*/ UInt8* pucFlags) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ PatternFilterType pft,
		/*in*/ const UInt8* pData, /*in*/ UInt32 ulDataBytes,
		/*in*/ UInt16 usStartOffset, /*in*/ UInt16 usEndOffset,
		/*in*/ UInt16 usPspec, /*in*/ BOOL bCaseSensitive,
		/*in*/ UInt8 ucFlags) = 0;
};

/// \interface IPatternFilterNode3
/// \ingroup Filter
/// \brief A pattern (string) filter node supporting relative offsets
/// \see IFilterNode, coclass PatternFilterNode
#define IPatternFilterNode3_IID \
{ 0x6A38C9E7, 0x4280, 0x45F9, {0x8A, 0xF8, 0x05, 0x57, 0x3A, 0x32, 0xF4, 0x00} }

class HE_NO_VTABLE IPatternFilterNode3 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPatternFilterNode3_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ PatternFilterType* ppft,
		/*out*/ UInt8** ppData, /*out*/ UInt32* pulDataBytes,
		/*out*/ UInt16* pusCodePage, /*out*/ BOOL* pbCaseSensitive,
		/*out*/ UInt16* pusStartOffset, /*out*/ UInt16* pusEndOffset,
		/*out*/ UInt16* pusPspec, /*out*/ UInt8* pucFlags) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ PatternFilterType pft,
		/*in*/ const UInt8* pData, /*in*/ UInt32 ulDataBytes,
		/*in*/ UInt16 usCodePage, /*in*/ BOOL bCaseSensitive,
		/*in*/ UInt16 usStartOffset, /*in*/ UInt16 usEndOffset,
		/*in*/ UInt16 usPspec, /*in*/ UInt8 ucFlags) = 0;
};

/// \interface IPluginFilterNode
/// \ingroup Filter
/// \brief A plugin filter node
/// \see IFilterNode, coclass PluginFilterNode
#define IPluginFilterNode_IID \
{ 0xD0329C21, 0x8B27, 0x489F, {0x84, 0xD7, 0xC7, 0xB7, 0x83, 0x63, 0x4A, 0x6A} }

class HE_NO_VTABLE IPluginFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPluginFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ Helium::HeID* pPluginId) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const Helium::HeID& PluginId) = 0;
};

/// \interface IPortFilterNode
/// \ingroup Filter
/// \brief A port filter node
/// \see IFilterNode, coclass PortFilterNode
#define IPortFilterNode_IID \
{ 0x297D404D, 0x3610, 0x4A18, {0x95, 0xA2, 0x22, 0x76, 0x8B, 0x55, 0x4B, 0xED} }

class HE_NO_VTABLE IPortFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPortFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ TMediaSpec* pPort1, /*out*/ TMediaSpec* pPort2,
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const TMediaSpec* pPort1, /*in*/ const TMediaSpec* pPort2,
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
};

/// \interface IPortFilterNode2
/// \ingroup Filter
/// \brief A port filter node
/// \see IFilterNode, coclass PortFilterNode
#define IPortFilterNode2_IID \
{ 0x52D958BB, 0x5269, 0x4912, {0x88, 0x76, 0xA9, 0xDF, 0x62, 0x38, 0x67, 0xC4} }

class HE_NO_VTABLE IPortFilterNode2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IPortFilterNode2_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ TMediaSpecType* pType,
		/*out*/ Helium::HEBSTR* pbstrPort1, /*out*/ Helium::HEBSTR* pbstrPort2,
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ TMediaSpecType type,
		/*in*/ Helium::HEBSTR bstrPort1, /*in*/ Helium::HEBSTR bstrPort2,
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
};

/// \interface IProtocolFilterNode
/// \ingroup Filter
/// \brief A protocol filter node
/// \see IFilterNode, coclass ProtocolFilterNode
#define IProtocolFilterNode_IID \
{ 0xF4342DAD, 0x4A56, 0x4ABA, {0x94, 0x36, 0x6E, 0x3C, 0x30, 0xDA, 0xB1, 0xC8} }

class HE_NO_VTABLE IProtocolFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IProtocolFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ TMediaSpec* pProtocol, /*out*/ BOOL* pbSliceToHeader) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const TMediaSpec* pProtocol, /*in*/ BOOL bSliceToHeader) = 0;
	/// Get the node's path data.
	HE_IMETHOD GetProtoSpecPath(/*out*/ UInt16* pProtospecPath, /*out, in*/ UInt32* pulPathLength ) = 0;
	/// Set the node's path data.
	HE_IMETHOD SetProtoSpecPath(/*in*/ const UInt16* pProtospecPath, /*in*/ UInt32 ulPathLength ) = 0;
};

/// \enum ValueFilterType
/// \brief Types for a value filter.
/// \see IValueFilterNode, ValueFilterValue, ValueFilterValueData
enum ValueFilterType
{
	valueFilterType1ByteUnsigned,
	valueFilterType1ByteSigned,
	valueFilterType2BytesUnsigned,
	valueFilterType2BytesSigned,
	valueFilterType4BytesUnsigned,
	valueFilterType4BytesSigned,
	valueFilterType8BytesUnsigned,
	valueFilterType8BytesSigned
};

/// \union ValueFilterValueData
/// \brief Data union inside a ValueFilterValue.
/// \see IValueFilterNode, ValueFilterValue
union ValueFilterValueData
{
	UInt8	m_UInt8;
	SInt8	m_SInt8;
	UInt16	m_UInt16;
	SInt16	m_SInt16;
	UInt32	m_UInt32;
	SInt32	m_SInt32;
	UInt64	m_UInt64;
	SInt64	m_SInt64;
};

/// \struct ValueFilterValue
/// \brief Values to be used in a value filter.
/// \see IValueFilterNode, ValueFilterType, ValueFilterValueData
#include "hepushpack.h"
struct ValueFilterValue
{
	ValueFilterType				ValueType;
	union ValueFilterValueData	Value;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \enum ValueFilterOp
/// \brief Ops for a value filter.
/// \see IValueFilterNode
enum ValueFilterOp
{
	valueFilterOpEqual				= 0x01,
	valueFilterOpGreaterThan		= 0x02,
	valueFilterOpGreaterThanOrEqual	= valueFilterOpEqual | valueFilterOpGreaterThan,
	valueFilterOpLessThan			= 0x04,
	valueFilterOpLessThanOrEqual	= valueFilterOpEqual | valueFilterOpLessThan,
	valueFilterOpNotEqual			= valueFilterOpGreaterThan | valueFilterOpLessThan
};

/// \enum ValueFilterFlags
/// \brief Flags for a value filter.
/// \see IValueFilterNode
enum ValueFilterFlags
{
	valueFilterFlagNetworkByteOrder		= 0x01,
	valueFilterFlagSigned				= 0x02,
	valueFilterFlagHex					= 0x04,
	valueFilterFlagRelativeToHeader		= 0x10,
	valueFilterFlagRelativeToPayload	= 0x20
};

/// \interface IValueFilterNode
/// \ingroup Filter
/// \brief A value filter node
/// \see IFilterNode, coclass ValueFilterNode
#define IValueFilterNode_IID \
{ 0x838F0E57, 0x0D9F, 0x4095, {0x9C, 0x12, 0xF1, 0x04, 0x0C, 0x84, 0xE4, 0x28} }

class HE_NO_VTABLE IValueFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IValueFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData( /*out*/ ValueFilterValue* pValue, /*out*/ ValueFilterValue* pMask,
		/*out*/ UInt16* pusOffset, /*out*/ ValueFilterOp* pOp, /*out*/ UInt32* pulFlags ) = 0;
	/// Set the node data.
	HE_IMETHOD SetData( /*in*/ ValueFilterValue value, ValueFilterValue mask,
		/*in*/ UInt16 usOffset, /*in*/ ValueFilterOp op, /*in*/ UInt32 ulFlags ) = 0;
};

/// \interface IValueFilterNode2
/// \ingroup Filter
/// \brief A value filter node which can search relative to packet layer
/// \see IFilterNode, coclass ValueFilterNode2
#define IValueFilterNode2_IID \
{ 0xE216190A, 0xE63D, 0x4126, {0xBE, 0xD8, 0x0B, 0x1B, 0xB2, 0xD2, 0xB5, 0xD0} }

class HE_NO_VTABLE IValueFilterNode2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR( IValueFilterNode2_IID )

	/// Get the node data.
	HE_IMETHOD GetData( /*out*/ ValueFilterValue* pValue, /*out*/ ValueFilterValue* pMask,
		/*out*/ UInt16* pusOffset, /*out*/ UInt16* pusPspec, /*out*/ ValueFilterOp* pOp,
		/*out*/ UInt32* pulFlags ) = 0;
	/// Set the node data.
	HE_IMETHOD SetData( /*in*/ ValueFilterValue value, ValueFilterValue mask,
		/*in*/ UInt16 usOffset, /*in*/ UInt16 usPspec, /*in*/ ValueFilterOp op,
		/*in*/ UInt32 ulFlags ) = 0;
};

/// \enum WirelessFilterNodeDataMask
/// \brief Mask for which entries in WirelessFilterNodeData are valid.
/// \see WirelessFilterNodeData
enum WirelessFilterNodeDataMask
{
	wirelessFilterMaskChannel			= 0x00000001,
	wirelessFilterMaskDataRate			= 0x00000002,
	wirelessFilterMaskSignal			= 0x00000004,
	wirelessFilterMaskSignaldBm			= 0x00000008,
	wirelessFilterMaskNoise				= 0x00000010,
	wirelessFilterMaskNoisedBm			= 0x00000020,
	wirelessFilterMaskEncryption		= 0x00000040,
	wirelessFilterMaskDecryptionError	= 0x00000080,
	wirelessFilterMaskBSSID				= 0x00000100,
	wirelessFilterMaskSignaldBm1		= 0x00000200,
	wirelessFilterMaskSignaldBm2		= 0x00000400,
	wirelessFilterMaskSignaldBm3		= 0x00000800,
	wirelessFilterMaskNoisedBm1			= 0x00001000,
	wirelessFilterMaskNoisedBm2			= 0x00002000,
	wirelessFilterMaskNoisedBm3			= 0x00004000,
	wirelessFilterMaskFlagsN			= 0x00008000,
	wirelessFilterMaskSourceAP			= 0x00010000
};

/// \struct WirelessFilterNodeData
/// \brief Values to be used in a wireless filter.
/// \see IWirelessFilterNode
#include "hepushpack.h"
struct WirelessFilterNodeData
{
	UInt32			DataMask;
	WirelessChannel	Channel;
	UInt32			DataRate;
	UInt8			MinSignal;
	UInt8			MaxSignal;
	SInt8			MinSignaldBm;
	SInt8			MaxSignaldBm;
	SInt8			MinSignaldBm1;
	SInt8			MaxSignaldBm1;
	SInt8			MinSignaldBm2;
	SInt8			MaxSignaldBm2;
	SInt8			MinSignaldBm3;
	SInt8			MaxSignaldBm3;
	UInt8			MinNoise;
	UInt8			MaxNoise;
	SInt8			MinNoisedBm;
	SInt8			MaxNoisedBm;
	SInt8			MinNoisedBm1;
	SInt8			MaxNoisedBm1;
	SInt8			MinNoisedBm2;
	SInt8			MaxNoisedBm2;
	SInt8			MinNoisedBm3;
	SInt8			MaxNoisedBm3;
	BOOL			Encryption;
	BOOL			DecryptionError;
	TMediaSpec		BSSID;
	UInt32			FlagsN;
	TMediaSpec		SourceAP;
	UInt16			RateScaleFactor;
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \interface IWirelessFilterNode
/// \ingroup Filter
/// \brief A wireless filter node
/// \see IFilterNode, coclass WirelessFilterNode
#define IWirelessFilterNode_IID \
{ 0x899E11AD, 0x1849, 0x40BA, {0x94, 0x54, 0x9F, 0x03, 0x79, 0x8B, 0x3A, 0x6C} }

class HE_NO_VTABLE IWirelessFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IWirelessFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ WirelessFilterNodeData* pData) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const WirelessFilterNodeData* pData) = 0;
};

/// \interface IChannelFilterNode
/// \ingroup Filter
/// \brief A full duplex channel filter node
/// \see IFilterNode, coclass ChannelFilterNode
#define IChannelFilterNode_IID \
{ 0x6E8DAF74, 0xAF0D, 0x4CD3, {0x86, 0x5D, 0xD5, 0x59, 0xA5, 0x79, 0x8C, 0x5B} }

class HE_NO_VTABLE IChannelFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IChannelFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ UInt32* pulChannel) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ UInt32 ulChannel) = 0;
};

/// \interface IDirectionFilterNode
/// \ingroup Filter
/// \brief WAN direction filter node
/// \see IFilterNode, coclass DirectionFilterNode
#define IDirectionFilterNode_IID \
{ 0x7ED9CACB, 0x36CB, 0x40C7, {0xB4, 0xCB, 0xF5, 0x0C, 0xAB, 0x5B, 0xA3, 0x15} }

class HE_NO_VTABLE IDirectionFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IDirectionFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ UInt8* pucDirection) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ UInt8 ulDirection) = 0;
};

/// \enum LogicalFilterOp
/// \brief Ops for a logical filter.
/// \see ILogicalFilterNode
enum LogicalFilterOp
{
	logicalFilterOpAnd,
	logicalFilterOpOr,
	logicalFilterOpNot
};

/// \interface ILogicalFilterNode
/// \ingroup Filter
/// \brief A full duplex Logical filter node
/// \see IFilterNode, coclass LogicalFilterNode
#define ILogicalFilterNode_IID \
{ 0x37F09024, 0x9398, 0x44E2, {0x9C, 0xA7, 0x77, 0x85, 0x94, 0x0B, 0x4C, 0x7D} }

class HE_NO_VTABLE ILogicalFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ILogicalFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ LogicalFilterOp* pOp,
		/*out*/ IFilterNode** ppLeft, /*out*/ IFilterNode** ppRight) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ LogicalFilterOp op,
		/*in*/ IFilterNode* pLeft, /*in*/ IFilterNode* pRight) = 0;
};

/// \interface IBpfFilterNode
/// \ingroup Filter
/// \brief A BPF filter node
/// \see IFilterNode, coclass BpfFilterNode
#define IBpfFilterNode_IID \
{ 0x3A1BB429, 0x681C, 0x429C, {0xB2, 0x68, 0x2F, 0xCC, 0x8E, 0xB6, 0xE2, 0xEA} }

class HE_NO_VTABLE IBpfFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IBpfFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ Helium::HEBSTR* pbstrData) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ Helium::HEBSTR bstrData) = 0;
};

/// \interface IVlanFilterNode
/// \ingroup Filter
/// \brief A VLAN filter node
/// \see IFilterNode, coclass VlanFilterNode
#define IVlanFilterNode_IID \
{ 0x1999CC65, 0x01DA, 0x4256, {0x81, 0xB4, 0xC3, 0x03, 0xBD, 0xE8, 0x8B, 0xDA} }

class HE_NO_VTABLE IVlanFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IVlanFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ Helium::HEBSTR* pbstrIDs, /*out*/ Helium::HEBSTR* pbstrLabels) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ Helium::HEBSTR bstrIDs, /*in*/ Helium::HEBSTR bstrLabels) = 0;
};

/// \interface IApplicationFilterNode
/// \ingroup Filter
/// \brief An application filter node
/// \see IFilterNode, coclass ApplicationFilterNode
#define IApplicationFilterNode_IID \
{ 0x313FC251, 0x1B0B, 0x4D52, {0x90, 0x30, 0x25, 0xE8, 0x72, 0x87, 0x69, 0x57} }

class HE_NO_VTABLE IApplicationFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IApplicationFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ TMediaSpec* pApplication ) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const TMediaSpec* pApplication ) = 0;
};

/// \interface ICountryFilterNode
/// \ingroup Filter
/// \brief A country filter node
/// \see IFilterNode, coclass CountryFilterNode
#define ICountryFilterNode_IID \
{ 0xF5D1D851, 0x7542, 0x40FC, {0xAA, 0x97, 0xE0, 0x51, 0x69, 0xBA, 0x7F, 0xD0} }

class HE_NO_VTABLE ICountryFilterNode : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICountryFilterNode_IID)

	/// Get the node data.
	HE_IMETHOD GetData(/*out*/ char country1[2], /*out*/ char country2[2],
		/*out*/ BOOL* pbAccept1To2, /*out*/ BOOL* pbAccept2To1) = 0;
	/// Set the node data.
	HE_IMETHOD SetData(/*in*/ const char country1[2], /*in*/ const char country2[2],
		/*in*/ BOOL bAccept1To2, /*in*/ BOOL bAccept2To1) = 0;
};

/// \interface IConvertFilterExpression
/// \ingroup Filter
/// \brief Convert or validate a filter expression
/// \see IFilter, coclass FilterExpressionConverter
#define IConvertFilterExpression_IID \
{ 0xA06DDC36, 0xE9A2, 0x4DD1, {0x9C, 0xD9, 0x3D, 0x18, 0xA5, 0xBC, 0x3B, 0x2D} }

class HE_NO_VTABLE IConvertFilterExpression : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IConvertFilterExpression_IID)

	/// Convert a filter expression string to a filter or just validate it.
	/// \param pszFilterExpression Filter expression string
	/// \param pResults Receives the conversion and parsing results in a
	/// JSON value (see IJSONDOMValue)
	/// \param ppFilter Receives the converted filter (optional, may be NULL)
	HE_IMETHOD ConvertFilterExpression(/*in*/ const wchar_t* pszFilterExpression,
		/*in*/ Helium::IHeUnknown* pResults, /*out*/ IPacketFilter** ppFilter) = 0;
};

#define TimeRangeFilterNode_CID \
{ 0x046D9C7D, 0x0D9E, 0x4409, {0x9C, 0x2C, 0xD3, 0x83, 0x18, 0x8F, 0x1A, 0x95} }

#define AddressFilterNode_CID \
{ 0xD2ED5346, 0x496C, 0x4EA0, {0x94, 0x8E, 0x21, 0xCD, 0xDA, 0x1E, 0xD7, 0x23} }

#define ErrorFilterNode_CID \
{ 0xF4818ADB, 0x0AA8, 0x428B, {0xA6, 0x11, 0xD3, 0x16, 0x85, 0xE2, 0x3D, 0xE8} }

#define Filter_CID \
{ 0x22353029, 0xA733, 0x4FCC, {0x8A, 0xC0, 0x78, 0x2D, 0xA3, 0x3F, 0xA4, 0x64} }

#define FilterCollection_CID \
{ 0x9A914678, 0xE8B2, 0x43d6, {0xBF, 0xDC, 0xEB, 0x14, 0x7B, 0xBF, 0xCC, 0xD1} }

#define FilterExpressionConverter_CID \
{ 0x09E0B8DC, 0x5F11, 0x47B8, {0xA0, 0xE4, 0x67, 0x20, 0x99, 0x58, 0xE8, 0xED} }

#define ProtocolFilterNode_CID \
{ 0xA43DDCC0, 0xCDD2, 0x46B4, {0x81, 0x14, 0x68, 0xE5, 0xFA, 0xF3, 0x51, 0x12} }

#define PortFilterNode_CID \
{ 0xB3279AE9, 0x91E1, 0x4D0A, {0x8A, 0xBD, 0x6D, 0x1B, 0xDC, 0x54, 0x71, 0xA9} }

#define ValueFilterNode_CID \
{ 0x019D6856, 0x7380, 0x4FE6, {0x99, 0xE7, 0x04, 0xEC, 0x78, 0x73, 0x2A, 0xFB} }

#define PatternFilterNode_CID \
{ 0xD64E6090, 0x9351, 0x4915, {0x97, 0xE3, 0x06, 0x72, 0x51, 0xD7, 0x54, 0x6A} }

#define LengthFilterNode_CID \
{ 0x76D9F2C2, 0x20D3, 0x463B, {0xA3, 0xD2, 0x50, 0x42, 0x9D, 0xF3, 0xD2, 0x3D} }

#define FilterPack_CID \
{ 0x0B5343D2, 0xF8E2, 0x44E4, {0x9C, 0xC5, 0x15, 0xD9, 0xB3, 0xA8, 0xA7, 0x88} }

#define PluginFilterNode_CID \
{ 0xE461CF99, 0x9A0A, 0x4DCF, {0x96, 0x91, 0x89, 0xEC, 0xED, 0x1D, 0x43, 0x74} }

#define WirelessFilterNode_CID \
{ 0x5C1006D4, 0xBA1E, 0x4F28, {0x88, 0xE6, 0x76, 0x5B, 0xE7, 0x1D, 0x2C, 0xD9} }

#define ChannelFilterNode_CID \
{ 0x0FCC314A, 0xDF4F, 0x4535, {0xA6, 0x9A, 0x5D, 0x42, 0xB2, 0x9D, 0x72, 0x67} }

#define DirectionFilterNode_CID \
{ 0xFD4B3916, 0xEEFC, 0x452E, {0xAF, 0x7E, 0x34, 0x1B, 0x6E, 0xC1, 0x37, 0x2A} }

#define LogicalFilterNode_CID \
{ 0x3D84D09E, 0x8AB5, 0x4336, {0xA8, 0xA0, 0x38, 0x3B, 0x02, 0xD0, 0x30, 0x08} }

#define BpfFilterNode_CID \
{ 0xE1D41D24, 0xCF70, 0x43D7, {0xA5, 0x79, 0xF7, 0x81, 0x77, 0x77, 0xE1, 0x38} }

#define VlanFilterNode_CID \
{ 0x04208487, 0x8370, 0x4B63, {0x98, 0xD6, 0xB9, 0xAA, 0x68, 0xC8, 0xDF, 0x7D} }

#define ApplicationFilterNode_CID \
{ 0xB588E248, 0xAB43, 0x4AB8, {0xAA, 0xFB, 0xE2, 0xE1, 0xBD, 0x2E, 0xC4, 0x28} }

#define CountryFilterNode_CID \
{ 0x0866D6FD, 0x0DBC, 0x47A9, {0xAD, 0x9E, 0x92, 0x7A, 0x64, 0x89, 0xD0, 0x1C} }

#endif /*PEEKFILTERS_H */
