// ============================================================================
//	Utils.cpp
//		implementation of the CUtils class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Utils.h"
#include "IpPenRegister.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CIpPenRegisterApp theApp;

typedef union _LONGTIME {
	UInt64		i;
	FILETIME	ft;
} LONGTIME;

LONGTIME	g_ltLastTime;
TCHAR*		g_strPeekNameList[5] = {
	_T( "\\OmniPeek.exe" ),
	_T( "\\Peek.exe" ),
	_T( "\\OPeek.exe" ),
	_T( "\\EPeek.exe" ),
	_T( "\\APeek.exe" )
};


// ============================================================================
//		Thread Name Utilities
// ============================================================================

// ----------------------------------------------------------------------------
//		SetThreadName
//
//	Usage: SetThreadName (-1, "MainThread");
// ----------------------------------------------------------------------------

void
SetThreadName(
	DWORD	dwThreadID,
	LPCSTR	szThreadName )
{
	THREADNAME_INFO info;
	{
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
	}

	__try {
		RaiseException( 0x406D1388, 0, (sizeof( info ) / sizeof( DWORD )),
			reinterpret_cast<const ULONG_PTR*>( &info ) );
	}
	__except ( EXCEPTION_CONTINUE_EXECUTION ) {
	}
}


// ============================================================================
//		Utilities
// ============================================================================

// ----------------------------------------------------------------------------
//		GetFileVersion
// ----------------------------------------------------------------------------

bool
GetFileVersion(
	CString			inFilePath,
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	bool	bResult = false;

	// Get the file version info size.
	DWORD	nIgnored;
	DWORD	nVersionSize = ::GetFileVersionInfoSize( inFilePath, &nIgnored );

	if ( nVersionSize > 0 ) {
		// Alloc the version info.
		void*	pVersionInfo = malloc( nVersionSize );
		ASSERT( pVersionInfo != NULL );

		if ( pVersionInfo != NULL ) {
			// Get the file version info.
			if ( ::GetFileVersionInfo( inFilePath, 0, nVersionSize, pVersionInfo ) ) {
				VS_FIXEDFILEINFO*	pFileInfo;
				UINT				nFileInfoLength = sizeof( VS_FIXEDFILEINFO );
				if ( ::VerQueryValue( pVersionInfo, _T( "\\" ), reinterpret_cast<void**>( &pFileInfo ), &nFileInfoLength ) ) {
					// Copy the file version info.
					outFileVersion.MajorVer = HIWORD( pFileInfo->dwFileVersionMS );
					outFileVersion.MinorVer = LOWORD( pFileInfo->dwFileVersionMS );
					outFileVersion.MajorRev = HIWORD( pFileInfo->dwFileVersionLS );
					outFileVersion.MinorRev = LOWORD( pFileInfo->dwFileVersionLS );

					// Copy the product version info.
					outProductVersion.MajorVer = HIWORD( pFileInfo->dwProductVersionMS );
					outProductVersion.MinorVer = LOWORD( pFileInfo->dwProductVersionMS );
					outProductVersion.MajorRev = HIWORD( pFileInfo->dwProductVersionLS );
					outProductVersion.MinorRev = LOWORD( pFileInfo->dwProductVersionLS );

					bResult = true;
				}
			}

			// Free the version info.
			free( pVersionInfo );
		}
	}

	return bResult;
}


// ----------------------------------------------------------------------------
//		GetPeekVersion
// ----------------------------------------------------------------------------

bool
GetPeekVersion(
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	TCHAR 	szFilePath[MAX_PATH];
	::GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );

	CString	strFilePath( szFilePath );
	int nPluginsFolder = strFilePath.ReverseFind( _T( '\\' ) );
	strFilePath.Truncate( nPluginsFolder );
	int nPeekFolder = strFilePath.ReverseFind( _T( '\\' ) );
	strFilePath.Truncate( nPeekFolder );

	for ( int i = 0; i < COUNTOF( g_strPeekNameList ); i++ ) {
		CString	strPeek = strFilePath + g_strPeekNameList[i];
		if ( GetFileVersion( strPeek, outFileVersion, outProductVersion ) ) {
			return true;
		}
	}

	return false;
}


// ----------------------------------------------------------------------------
//		GetPluginVersion
// ----------------------------------------------------------------------------

bool
GetPluginVersion(
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	TCHAR 	szFilePath[MAX_PATH];
	::GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );
	return GetFileVersion( szFilePath, outFileVersion, outProductVersion );
}


// ----------------------------------------------------------------------------
//		GetTimeStamp
// ----------------------------------------------------------------------------

UInt64
GetTimeStamp()
{
	SYSTEMTIME	theSystemTime;
	union {
		FILETIME	theFileTime;
		UInt64		thePeekTime;
	} uTime;

	::GetSystemTime( &theSystemTime );
	::SystemTimeToFileTime( &theSystemTime, &uTime.theFileTime );

	return uTime.thePeekTime;
}


// ----------------------------------------------------------------------------
//		FormatTimeStamp
// ----------------------------------------------------------------------------

CString
FormatTimeStamp(
	SYSTEMTIME*	inTime )
{
	CStringA				strTimeA;
	LONG					nTimeZone = 0;
	TIME_ZONE_INFORMATION	tzInfo;
	DWORD	dwResult = ::GetTimeZoneInformation( &tzInfo );

	if ( (dwResult == TIME_ZONE_ID_STANDARD) || (dwResult == TIME_ZONE_ID_DAYLIGHT) ) {
		nTimeZone = -tzInfo.Bias;		// Invert for display.
	}

	CStringA	strDate;
	::GetDateFormatA( LOCALE_SYSTEM_DEFAULT, 0, inTime, "dddd, MMMM dd, yyyy",
		strDate.GetBuffer( 32 ), 32 );
	strDate.ReleaseBuffer();

	CStringA	strTime;
	::GetTimeFormatA( LOCALE_SYSTEM_DEFAULT, 0, inTime, "hh:mm:ss tt",
		strTime.GetBuffer( 16 ), 16 );
	strTime.ReleaseBuffer();

	CStringA	strZone;
	strZone.Format( "%02d:%02d", (nTimeZone / 60), (abs( nTimeZone ) % 60) );

	strTimeA.Format( "%s %s %s", strDate, strTime, strZone );

	return CString( strTimeA );
}


// ----------------------------------------------------------------------------
//		GetTimeString
// ----------------------------------------------------------------------------

CString
GetTimeString()
{
	CString		strTime;
	SYSTEMTIME	theTime;

	::GetSystemTime( &theTime );

	// 24 characters: 'YYYY-MM-DD HH:MM:SS.mmmZ'
	strTime.Format(
		_T( "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u.%3.3uZ" ),
		theTime.wYear,
		theTime.wMonth,
		theTime.wDay,
		theTime.wHour,
		theTime.wMinute,
		theTime.wSecond,
		theTime.wMilliseconds );

	return strTime;
}


// ----------------------------------------------------------------------------
//		GetTimeStringFileName
// ----------------------------------------------------------------------------

CString
GetTimeStringFileName() throw( ... )
{
	CString		strFileName;
	LONGTIME	ltTime;

	::GetSystemTimeAsFileTime( &ltTime.ft );
	int x = 0;
	while ( ltTime.i == g_ltLastTime.i ) {
		Sleep( 1 );
		::GetSystemTimeAsFileTime( &ltTime.ft );
		if ( x++ > 1000 ) throw 0;
	}
	g_ltLastTime = ltTime;

	SYSTEMTIME	stTime;
	::FileTimeToSystemTime( &ltTime.ft, &stTime );
	// 25 characters: 'YYYY-MM-DDTHH.MM.SS.mmmZ'
	// As close to ISO 8601 as possible
	//    periods instead of colons
	strFileName.Format(
		_T( "%4.4u-%2.2u-%2.2uT%2.2u.%2.2u.%2.2u.%3.3uZ" ),
		stTime.wYear,
		stTime.wMonth,
		stTime.wDay,
		stTime.wHour,
		stTime.wMinute,
		stTime.wSecond,
		stTime.wMilliseconds );

	return strFileName;
}


// ----------------------------------------------------------------------------
//		MakePath
// ----------------------------------------------------------------------------

bool
MakePath(
	CString	inPath )
{
	CString	tmpPath( inPath );
	ASSERT( tmpPath.GetLength() > 0 );
	if ( tmpPath.GetLength() == 0 ) return false;

	bool bRetVal = false;

	// Make sure the directory name ends in a slash. PathAddBackslash()
	if ( tmpPath.Right( 1 ) != _T( "\\" ) ) {
		tmpPath += _T( '\\' );
	}

	// Create each directory in the path.
	CString	dirName;
	int		nIndex = 0;
	bool	bDone = false;
	while ( !bDone ) {
		// Extract one directory.
		nIndex = tmpPath.Find( _T( '\\' ) );
		if ( nIndex != -1 ) {
			dirName = dirName + tmpPath.Left( nIndex );
			tmpPath = tmpPath.Right( tmpPath.GetLength() - nIndex - 1 );

			// The first time through, we might have a drive name.
			if ( (dirName.GetLength() >= 1) && (dirName.Right( 1 ) != _T( ":" )) ) {
				bRetVal = (::CreateDirectory( dirName, NULL ) != FALSE);
			}
			dirName += _T( '\\' );
		}
		else {
			bDone = true;
		}
	}

	// Return the last MakeDirectory() return value.
	return bRetVal;
}


// ----------------------------------------------------------------------------
//		DirectoryExists
// ----------------------------------------------------------------------------

bool
DirectoryExists(
	CString	inDirectory )
{
	return (::PathIsDirectory( inDirectory ) != FALSE);
}


// ----------------------------------------------------------------------------
//		BuildFileName
// ----------------------------------------------------------------------------

CString
BuildFileName(
	CString	inFileName,
	CString	inOutputDirectory )
{
	CString	strFilePath;

	// Make sure the output directory name ends in a slash.
	CString	strOutputDirectory( inOutputDirectory );
	if ( strOutputDirectory.Right( 1 ) != _T( "\\" ) ) {
		strOutputDirectory += _T( '\\' );
	}

	MakePath( inOutputDirectory );

	SYSTEMTIME	theTime;
	::GetSystemTime( &theTime );

	strFilePath.Format(
		_T( "%s%4.4u-%2.2u-%2.2u %s" ),
		strOutputDirectory,
		theTime.wYear,
		theTime.wMonth,
		theTime.wDay,
		inFileName );

	return strFilePath;
}


// ----------------------------------------------------------------------------
//		BuildUniqueName
// ----------------------------------------------------------------------------

CString
BuildUniqueName(
	CString	inFileName,
	CString	inOutputDirectory ) throw( ... )
{
	CString	strFilePath;
	CString	strPath;

	// Make sure the output directory name ends in a slash.
	CString	strOutputDirectory( inOutputDirectory );
	if ( strOutputDirectory.Right( 1 ) != _T( "\\" ) ) {
		strOutputDirectory += _T( '\\' );
	}

	MakePath( strOutputDirectory );

	int	x = 0;
	do {
		CString	strTimeStamp = GetTimeStringFileName();

		strFilePath.Format(
			_T( "%s%s %s" ),
			strOutputDirectory,
			strTimeStamp,
			inFileName );
		if ( x++ > 100 ) throw 0;
	} while ( ::PathFileExists( strFilePath ) );

	return strFilePath;
}


// -----------------------------------------------------------------------------
//		Utf8ToUnicode
// -----------------------------------------------------------------------------

CString
Utf8ToUnicode(
	CString inUtf8 )
{
	CString		strUnicode;
	if ( inUtf8.IsEmpty() ) return strUnicode;

	CStringA	strUtf8( inUtf8 );
	size_t		nUtf8( strUtf8.GetLength() );
	int			nResult = ::MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		strUtf8.GetBuffer(),
		static_cast<int>( nUtf8 ),
		strUnicode.GetBufferSetLength( static_cast<int>( nUtf8 + 1 ) ),
		static_cast<int>( ((nUtf8 + 1) * sizeof( TCHAR )) ) );
	strUnicode.ReleaseBufferSetLength( nResult );
	if ( nResult == 0 ) {
#ifdef _DEBUG
		DWORD	dwError = ::GetLastError();
		dwError = 0;
#endif
		strUnicode.Empty();
	}
	return strUnicode;
}
