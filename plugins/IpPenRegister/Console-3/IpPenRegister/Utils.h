// ============================================================================
//	Utils.h
//		interface for various utility functions.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

typedef CAtlArray<CString>	CArrayString;
typedef CAtlArray<CStringA>	CArrayStringA;

#pragma comment(lib, "Version.lib")

#pragma pack(push,1)

struct ENV_VERSION
{
	UInt16	MajorVer;
	UInt16	MinorVer;
	UInt16	MajorRev;
	UInt16	MinorRev;
};

typedef UInt8	tMacAddress[6];

typedef union _tIPv4Address {
	UInt32	word;
	UInt8	bytes[4];
} tIPv4Address;

typedef union _tIPv6Address {
	UInt64	longs[2];
	UInt32	words[4];
	UInt16	shorts[8];
	UInt8	bytes[16];
} tIPv6Address;

typedef struct _tEthernetHeader {
	tMacAddress	Destination;
	tMacAddress	Source;
	UInt16		ProtocolType;
} tEthernetHeader;

typedef struct _t80211Header {
	UInt16		VersionTypeSubType;
	UInt16		Duration;
	tMacAddress	BSSID;
	tMacAddress	Source;
	tMacAddress	Destination;
	UInt16		ProtocolType;
} t80211Header;

typedef struct _tIPv4Header {
	UInt8			VersionLength;
	UInt8			TypeOfService;
	UInt16			TotalLength;
	UInt16			Identifier;
	UInt16			FlagsFragmentOffset;
	UInt8			TimeToLive;
	UInt8			Protocol;
	UInt16			HeaderChecksum;
	tIPv4Address	Source;
	tIPv4Address	Destination;
} tIPv4Header;

typedef struct _tIPv6Header {
	UInt32			VersionClassLabel;
	UInt16			PayloadLength;
	UInt8			NextHeader;
	UInt8			HopLimit;
	tIPv6Address	Source;
	tIPv6Address	Destination;
} tIPv6Header;

typedef struct _tTcpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt32	SequenceNumber;
	UInt32	AckNumber;
	UInt8	Offset;
	UInt8	Flags;
	UInt16	aWindowSize;
	UInt16	aChecksum;
	UInt16	UrgentPointer;
} tTcpHeader;

typedef struct _tUdpHeader {
	UInt16	SourcePort;
	UInt16	DestinationPort;
	UInt16	Length;
	UInt16	Checksum;
} tUdpHeader;

typedef struct _tDnsHeader {
	unsigned short	id;			// query identification number //

	unsigned short	rd:		1;	// recursion desired //
	unsigned short	tc:		1;	// truncated message //
	unsigned short	aa:		1;	// authoritive answer //
	unsigned short	opcode: 4;	// purpose of message //
	unsigned short	qr:     1;	// response flag //

	unsigned short	rcode:  4;	// response code //
	unsigned short	unused: 1;	// unused bits  //
	unsigned short	ad:		1;	// authentic data from named //
	unsigned short	cd:		1;	// checking disabled by resolver //
	unsigned short	ra:		1;	// recursion available //

	unsigned short	qdcount;	// number of question entries //
	unsigned short	ancount;	// number of answer entries //
	unsigned short	nscount;	// number of authority entries //
	unsigned short	arcount;	// number of resource entries //
} tDnsHeader;

#pragma pack(pop)


// ============================================================================
//		Thread Name Utilities
// ============================================================================

typedef struct tagTHREADNAME_INFO {
	DWORD	dwType;			// must be 0x1000
	LPCSTR	szName;			// pointer to name (in user addr space)
	DWORD	dwThreadID;		// thread ID (-1=caller thread)
	DWORD	dwFlags;		// reserved for future use, must be zero
} THREADNAME_INFO;

void		SetThreadName( DWORD dwThreadID, LPCSTR szThreadName );


// ============================================================================
//		Utilities
// ============================================================================

bool		GetFileVersion( CString inFilePath,
				ENV_VERSION& outFileVersion, ENV_VERSION& outProductVersion );
bool		GetPeekVersion( ENV_VERSION& outFileVersion, ENV_VERSION& outProductVersion );
bool		GetPluginVersion( ENV_VERSION& outFileVersion, ENV_VERSION& outProductVersion );
UInt64		GetTimeStamp();
CString		FormatTimeStamp( SYSTEMTIME* inTime );
CString		GetTimeString();
CString		GetTimeStringFileName() throw( ... );
bool		MakePath( CString inPath );
bool		DirectoryExists( CString inDirectory );
CString		BuildFileName( CString inFileName, CString inOutputDirectory );
CString		BuildUniqueName( CString inFileName, CString inOutputDirectory ) throw( ... );
CString		Utf8ToUnicode( CString inUtf8 );
