// ============================================================================
//	Utils.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "ByteVectRef.h"
#include "Environment.h"

#define kUtil_Flag_None			0x0000
#define kUtil_Flag_No_Day		0x0001
#define kUtil_Flag_No_TimeZone	0x0002

#define BUFSIZE					512

//typedef UInt32						PROTOSPEC_ID;			// id # (e.g. IP = 1000)
#define GET_PSID(inInstanceID)		((PROTOSPEC_ID) ( inInstanceID & 0x0000FFFF ))

typedef CAtlArray<CString>	_CArrayString;
typedef CAtlArray<CStringA>	_CArrayStringA;

typedef enum {
	kWindows_Unknown,
	kWindows_2000,
	kWindows_XP,
	kWindows_Vista
} tWindowsType;

//const	UInt32	kSecondsInAMinute = 60;
//const	UInt32	kSecondsInAnHour = 60 * kSecondsInAMinute;
//const	UInt32	kSecondsInADay = 24 * kSecondsInAnHour;
const	UInt32	kMinutesInAnHour = 60;
const	UInt32	kHoursInADay = 24;

const	UInt64	kKilobyte = 1024;
const	UInt64	kMegabyte = 1024 * 1024;
const	UInt64	kGigabyte = 1024 * 1024 * 1024;

#pragma comment(lib, "Version.lib")

#pragma pack(push,1)

typedef UInt8	tMacAddress[6];

typedef union _tIpAddress {
	UInt32	word;
	UInt8	byte[4];
} tIpAddress;


/*
typedef struct _tEthernetHeader {
	tMacAddress	Destination;
	tMacAddress	Source;
	UInt16		ProtocolType;
} tEthernetHeader;
*/

typedef struct _tIpHeader {
	UInt8		VersionLength;
	UInt8		TypeOfService;
	UInt16		TotalLength;
	UInt16		Identifier;
	UInt16		FlagsFragmentOffset;
	UInt8		TimeToLive;
	UInt8		Protocol;
	UInt16		HeaderChecksum;
	tIpAddress	Source;
	tIpAddress	Destination;
} tIpHeader;

/*
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

typedef struct _tLlcHeader {
	UInt8	DestinationSAP;
	UInt8	SourceSAP;
	UInt8	Command;
	UInt8	VendorId[3];
	UInt8	ProtocolType;
} tLlcHeader;

typedef struct _tLlcLongDDPHeader {
	UInt16	HopCountDataGramLength;
	UInt16	DDPChecksum;
	UInt16	DestinationNetwork;
	UInt16	SourceNetwork;
	UInt8	DestinationNode;
	UInt8	SourceNode;
	UInt8	DestinationSocket;
	UInt8	SourceSocket;
	UInt8	DDPType;
} tLlcLongDDPHeader;
*/
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
//		CFileStr
// ============================================================================

class CFileStr
	:	public CFile
{
public:
	void	Write( CStringA& inStr ) { CFile::Write( inStr.GetBuffer(), inStr.GetLength() ); };
	void	Write( CString& inStr ) { CFile::Write( inStr.GetBuffer(), inStr.GetLength() ); };
	void	WriteLineA() { CFile::Write( "\r\n", 2 ); };
	void	WriteLine() { CFile::Write( _T("\r\n"), (2 * sizeof( TCHAR )) ); };
};

/*
// ============================================================================
//		CFileStr
// ============================================================================

class CFileEx
	:	public CFile
{
public:
	void	Close() { if ( IsOpen() ) CFile::Close(); }
	bool	IsOpen() { return( m_hFile != hFileNull ); }
	UInt32	Write( CString in ) {
		try {
			CFile::Write( in.GetBuffer(), in.GetLength() );
		}
		catch ( ... ) {
			return 0;
		}
		return in.GetLength();
	}
};
*/

// ============================================================================
//		Utilities
// ============================================================================

bool		GetFileVersion( CString inFilePath,
				ENV_VERSION& outFileVersion, ENV_VERSION& outProductVersion );
bool		GetPeekVersion( ENV_VERSION& outFileVersion, ENV_VERSION& outProductVersion );
//bool		GetPluginVersion( ENV_VERSION& outFileVersion, ENV_VERSION& outProductVersion );
UInt64		ParseTimeStamp( CPeekString inTime );
CPeekString	FormatTimeGreenwich( SYSTEMTIME* inTime );
CPeekString	FormatTimeGreenwich( UInt64 inTime );
CPeekString	GetTimeStringGreenwich();
//CPeekStringA GetTimeStringGreenwich();
UInt64		GetTimeStamp();
CPeekString	FormatTimeLocal( SYSTEMTIME* inTime );
CPeekString	FormatTimeStamp( UInt64 inTime );
CPeekString	FormatTimeStamp( SYSTEMTIME* inTime );
CPeekStringA FormatTimeStamp( SYSTEMTIME* inTime, int inFlags );
CPeekString	GetTimeString();
CPeekString	GetTimeStringFileName( bool bRandomizeMilliseconds = false ) throw( ... );
bool		TimeStampToTime( UInt64 inTimeStamp, SYSTEMTIME* outTime );
UInt64		TimeToTimeStamp( const SYSTEMTIME& inDate, const SYSTEMTIME& inTime );
CPeekString		LegalizeFilename( CString inFileName, TCHAR inReplace = _T( '_' ) );
bool		IsPathValid( CPeekString inPath );
bool		MakePath( CPeekString inPath );
bool		DirectoryExists( CString inDirectory );
CPeekString	BuildFileName( CString inFileName, CString inOutputDirectory );
tWindowsType	GetWindowsType();
bool		IsWindowsType( tWindowsType inType );
bool		CopyToClipboard( CStringA inString );
void		DumpBytes( HANDLE hFile, size_t inLength, const byte* inData );
void		DumpBytes( HANDLE hFile, const CByteArray& inData );
void		DumpBytes( HANDLE hFile, CByteVectRef inData );
void		DumpBytes( PCTSTR inPreAmble, size_t inLength, const byte* inData );
void		DumpBytes( PCTSTR inPreAmble, const CByteArray& inData );
bool		ReadToByteArray( CString inFileName, CByteArray& outData );
UInt64		ParseTimeStampGreenwich( CPeekString	inTime );
WORD		GetRandomizedMilliSec( bool bSeed = false ) throw( ... );

