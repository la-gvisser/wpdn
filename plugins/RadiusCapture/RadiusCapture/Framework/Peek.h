// =============================================================================
//	Peek.h
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "MediaTypes.h"

class CPeekString;


namespace Peek
{

inline bool Throw( const HRESULT hr )
{
	throw hr;
}

inline bool ThrowIf( bool b )
{
	_ASSERTE( !b );
	if ( b ) throw( E_FAIL );
	return b;
}

inline HRESULT ThrowIfFailed( const HRESULT hr )
{
	_ASSERTE( SUCCEEDED( hr ) );
	if ( FAILED( hr ) ) throw( hr );
	return hr;
}

#define kNameEntryLength	64

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
	UInt64					Date;			// Date in nanoseconds since midnight 1/1/1601 UTC.
	UInt64					Time;			// Time in nanoseconds since midnight 1/1/1601 UTC.
	SInt64					Duration;		// Duration in nanoseconds since midnight 1/1/1601 UTC.
	UInt64					Packets;		// Packet count.
	UInt64					Bytes;			// Byte count.
	SummaryStatValuePair	ValuePair;		// Packet count/byte count pair.
	UInt64					IntValue;		// Integer value.
	double					DoubleValue;	// Double value.
};

struct SummaryStat
{
	SummaryStatType			ValueType;
	SummaryStatValue		Value;
};

struct SummaryStatEntry
{
	GUID					Id;
	SummaryStat				Stat;
};

#include "wppoppack.h"

};	// namespace Peek


// =============================================================================
//		COmniModeledData
// =============================================================================

class COmniModeledData
{
public:
	virtual CPeekString	ModeledDataGet() const = 0;
	virtual void		ModeledDataSet( const CPeekString& inValue ) = 0;
};


// =============================================================================
//		Peek Plugin Result Codes
// =============================================================================

#define	PEEK_PLUGIN_SUCCESS		0
#define PEEK_PLUGIN_FAILURE		-1

//#pragma message( "Fix Console constant references." )

// Return when processing kPluginMsg_Apply and kPluginMsg_Select during the
// kApplyMsg_Start/kSelectMsg_Start phase to indicate whether to continue the
// operation.
#define PLUGIN_START_CONTINUE	 0
#define PLUGIN_START_CANCEL		-1

// Return when processing kPluginMsg_Apply, kPluginMsg_Filter, and
// kPluginMsg_Select to indicate whether to include a packet in the operation.
#define PLUGIN_PACKET_ACCEPT	 0
#define PLUGIN_PACKET_REJECT	-1

// When processing kPluginMsg_Filter, assign to
// PluginFilterParam.fBytesAccepted to indicate the entire length of the
// packet should be accepted. In other words, no filter slicing should take
// place.
#define PLUGIN_ACCEPT_WHOLE_PACKET	0

