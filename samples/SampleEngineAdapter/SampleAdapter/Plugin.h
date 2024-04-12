// =============================================================================
//	Plugin.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "MediaTypes.h"
#include <atlstr.h>

namespace Plugin
{

inline void ThrowIf( bool b )
{
	if ( b ) throw( E_FAIL );
}

inline void ThrowIfFailed( const HRESULT hr )
{
	if ( FAILED(hr) ) throw( hr );
}

#define kNameEntryLength		64

enum NameEntryType
{
	kNameEntryType_Unknown,
	kNameEntryType_Workstation,
	kNameEntryType_Server,
	kNameEntryType_Router,
	kNameEntryType_Switch,
	kNameEntryType_Repeater,
	kNameEntryType_Printer,
	kNameEntryType_AccessPoint
};

enum NameEntrySource
{
	kNameEntrySource_Unknown,		// Name source unknown.
	kNameEntrySource_Active,		// Name came from active resolver.
	kNameEntrySource_Passive,		// Name came from passive resolver.
	kNameEntrySource_User,			// Name came from user.
	kNameEntrySource_Plugin,		// Name came from a plug-in.
	kNameEntrySource_Wildcard		// Name came from wildcard.
};

enum NameEntryTrust
{
	kNameEntryTrust_Unknown,		// Trust is unknown.
	kNameEntryTrust_Known,			// Address is known.
	kNameEntryTrust_Trusted			// Address is trusted.
};

enum SummaryStatType
{
	kSummaryStatType_Null,
	kSummaryStatType_Date,
	kSummaryStatType_Time,
	kSummaryStatType_Duration,
	kSummaryStatType_Packets,
	kSummaryStatType_Bytes,
	kSummaryStatType_ValuePair,
	kSummaryStatType_Int,
	kSummaryStatType_Double
};

enum SummaryLayer
{
	kSummaryLayer_None,				// No layer.
	kSummaryLayer_Physical,			// Physical layer (Layer 1).
	kSummaryLayer_Link,				// Link layer (Layer 2).
	kSummaryLayer_Network,			// Network layer (Layer 3).
	kSummaryLayer_Transport,		// Transport layer (Layer 4).
	kSummaryLayer_Session,			// Session layer (Layer 5).
	kSummaryLayer_Presentation,		// Presentation layer (Layer 6).
	kSummaryLayer_Application		// Application layer (Layer 7).
};

#include "wppushpack.h"
struct NameEntry
{
	TMediaSpec				Spec;
	wchar_t					Name[kNameEntryLength];
	SInt32					Group;
	SInt32					Modified;
	SInt32					LastUsed;
	UInt32					Color;
	NameEntryType			Type;
	NameEntrySource			Source;
	NameEntryTrust			Trust;
};

struct SummaryStatValuePair
{
	UInt64	Packets;	// Packet count.
	UInt64	Bytes;		// Byte count.
};

union SummaryStatValue
{
	UInt64						Date;			// Date in nanoseconds since midnight 1/1/1601 UTC.
	UInt64						Time;			// Time in nanoseconds since midnight 1/1/1601 UTC.
	SInt64						Duration;		// Duration in nanoseconds since midnight 1/1/1601 UTC.
	UInt64						Packets;		// Packet count.
	UInt64						Bytes;			// Byte count.
	SummaryStatValuePair	ValuePair;		// Packet count/byte count pair.
	UInt64						IntValue;		// Integer value.
	double						DoubleValue;	// Double value.
};

struct SummaryStat
{
	SummaryStatType			ValueType;
	SummaryStatValue		Value;
};

struct SummaryStatEntry
{
	GUID						Id;
	SummaryStat				Stat;
};

#include "wppoppack.h"

};	// namespace Plugin


// =============================================================================
//		Peek Plugin Result Codes
// =============================================================================

#define	PEEK_PLUGIN_SUCCESS		0
#define PEEK_PLUGIN_FAILURE		-1
