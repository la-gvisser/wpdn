// ============================================================================
//	Utils.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Utils.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
		RaiseException( 0x406D1388, 0, (sizeof( info ) / sizeof( DWORD )), (ULONG_PTR*) &info );
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
				UINT				nFileInfoLength = sizeof(VS_FIXEDFILEINFO);
				if ( ::VerQueryValue( pVersionInfo, _T( "\\" ), (void**) &pFileInfo, &nFileInfoLength ) ) {
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
	ENV_VERSION&	/*outFileVersion*/,
	ENV_VERSION&	/*outProductVersion*/ )
{
/*
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
*/
	return false;
}


//// ----------------------------------------------------------------------------
////		GetPluginVersion
//// ----------------------------------------------------------------------------
//
//bool
//GetPluginVersion(
//	ENV_VERSION&	/*outFileVersion*/,
//	ENV_VERSION&	/*outProductVersion*/ )
//{
//	TCHAR 	szFilePath[MAX_PATH];
//	::GetModuleFileName( theApp.m_hInstance, szFilePath, MAX_PATH );
//	return GetFileVersion( szFilePath, outFileVersion, outProductVersion );
//}


// -----------------------------------------------------------------------------
//		ParseTimeStamp
// -----------------------------------------------------------------------------

UInt64
ParseTimeStamp(
	CPeekString		inTime )
{
	SYSTEMTIME	stTime;
	memset( &stTime, 0, sizeof( SYSTEMTIME ) );

	TIME_ZONE_INFORMATION	tzInfo;
	memset( &tzInfo, 0, sizeof( tzInfo ) );

	UInt32	nYear = 0;
	UInt32	nMonth = 0;
	UInt32	nDay = 0;
	UInt32	nHour = 0;
	UInt32	nMinute = 0;
	UInt32	nSecond = 0;
	UInt32	nMilliseconds = 0;
//int		nBias = 0;
	_stscanf_s( inTime, _T( "%u/%u/%u %u:%u:%u:%u" ),
		&nYear, &nMonth, &nDay,
		&nHour, &nMinute, &nSecond, &nMilliseconds );

	stTime.wYear = (WORD) nYear;
	stTime.wMonth = (WORD) nMonth;
	stTime.wDay = (WORD) nDay;
	stTime.wHour = (WORD) nHour;
	stTime.wMinute = (WORD) nMinute;
	stTime.wSecond = (WORD) nSecond;
	stTime.wMilliseconds = (WORD) nMilliseconds;

	UInt64 nFileTime;
	::SystemTimeToFileTime( &stTime, (FILETIME*) &nFileTime );

	// Convert the local file time to UTC file time.
	UInt64 nUtcFileTime;
	::LocalFileTimeToFileTime( (FILETIME*) &nFileTime, (FILETIME*) &nUtcFileTime );

	return nUtcFileTime;
}


// ----------------------------------------------------------------------------
//		GetTimeStamp
// ----------------------------------------------------------------------------

UInt64
GetTimeStamp()
{
	UInt64	uTime;
	::GetSystemTimeAsFileTime( (FILETIME*) &uTime );	// 100-nanosecond units
	return uTime;
}


// -----------------------------------------------------------------------------
//		FormatTimeStamp
// -----------------------------------------------------------------------------

CPeekString
FormatTimeStamp(
	UInt64	inTime )
{
	LONG					nTimeZone = 0;
	TIME_ZONE_INFORMATION	tzInfo;
	DWORD	dwResult = ::GetTimeZoneInformation( &tzInfo );

	if ( (dwResult == TIME_ZONE_ID_STANDARD) || (dwResult == TIME_ZONE_ID_DAYLIGHT) ) {
		nTimeZone = -tzInfo.Bias;		// Invert for display.
	}

	SYSTEMTIME	stTime;
	TimeStampToTime( inTime, &stTime );

	CString					strTime;
	strTime.Format( _T( "%04u/%02u/%02u %02u:%02u:%02u:%04u" ),
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond, stTime.wMilliseconds );

	return (CPeekString)strTime;
}


// ----------------------------------------------------------------------------
//		FormatTimeStamp
// ----------------------------------------------------------------------------

CPeekString
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

	return CPeekString( (CPeekStringA)strTimeA );
}


// -----------------------------------------------------------------------------
//		FormatTimeStamp
// -----------------------------------------------------------------------------

CPeekStringA
FormatTimeStamp(
	SYSTEMTIME*	inTime,
	int			inFlags )
{
	CStringA				strTime;
	LONG					nTimeZone = 0;
	TIME_ZONE_INFORMATION	tzInfo;
	DWORD	dwResult = ::GetTimeZoneInformation( &tzInfo );

	if ( (dwResult == TIME_ZONE_ID_STANDARD) || (dwResult == TIME_ZONE_ID_DAYLIGHT) ) {
		nTimeZone = -tzInfo.Bias;		// Invert for display.
	}

	char	szDate[32];
	if ( inFlags && kUtil_Flag_No_Day ) {
		::GetDateFormatA( LOCALE_SYSTEM_DEFAULT, 0, inTime, "MMMM dd, yyyy", szDate, sizeof( szDate ) );
	}
	else {
		::GetDateFormatA( LOCALE_SYSTEM_DEFAULT, 0, inTime, "dddd, MMMM dd, yyyy", szDate, sizeof( szDate ) );
	}

	char	szTime[16];
	::GetTimeFormatA( LOCALE_SYSTEM_DEFAULT, 0, inTime, "hh:mm:ss tt", szTime, sizeof( szTime ) );

	if ( inFlags && kUtil_Flag_No_TimeZone ) {
		strTime.Format( "%s %s", szDate, szTime );
	}
	else {
		strTime.Format( "%s %s %02d:%02d", szDate, szTime, (nTimeZone / 60), (abs( nTimeZone ) % 60) );
	}
	return (CPeekStringA)strTime;
}

// ----------------------------------------------------------------------------
//		FormatTimeStampLocal
// ----------------------------------------------------------------------------

CPeekString
FormatTimeLocal(
	SYSTEMTIME*	inTime )
{
	CString					strTime;
	LONG					nTimeZone = 0;
	TIME_ZONE_INFORMATION	tzInfo;
	DWORD	dwResult = ::GetTimeZoneInformation( &tzInfo );

	if ( (dwResult == TIME_ZONE_ID_STANDARD) || (dwResult == TIME_ZONE_ID_DAYLIGHT) ) {
		nTimeZone = -tzInfo.Bias;		// Invert for display.
	}

	CString	strDate;
	::GetDateFormat( LOCALE_SYSTEM_DEFAULT, 0, inTime, _T("dddd, MMMM dd, yyyy"),
		strDate.GetBuffer( 32 ), 32 );
	strDate.ReleaseBuffer();

	CString	strTime1;
	::GetTimeFormat( LOCALE_SYSTEM_DEFAULT, 0, inTime, _T("hh:mm:ss tt"),
		strTime1.GetBuffer( 32 ), 32 );
	strTime1.ReleaseBuffer();

	CString	strZone;
	strZone.Format( _T("%02d:%02d"), (nTimeZone / 60), (abs( nTimeZone ) % 60) );

	strTime.Format( _T("%s %s %s"), strDate, strTime1, strZone );

	return (CPeekString)strTime;
}


// ----------------------------------------------------------------------------
//		FormatTimeGreenwich
// ----------------------------------------------------------------------------

CPeekString
FormatTimeGreenwich( SYSTEMTIME* inTime )
{
	ASSERT ( inTime );
	CString		strTime;

	// 24 characters: 'YYYY-MM-DD HH:MM:SS.mmmZ'
	strTime.Format(
		_T( "%4.4u/%2.2u/%2.2u %2.2u:%2.2u:%2.2u.%3.3uZ" ),
		inTime->wYear,
		inTime->wMonth,
		inTime->wDay,
		inTime->wHour,
		inTime->wMinute,
		inTime->wSecond,
		inTime->wMilliseconds );

	return (CPeekString)strTime;
}

CPeekString
FormatTimeGreenwich( UInt64 inPeekTime )
{
	ASSERT ( inPeekTime );
	CString	strTime;

	SYSTEMTIME	theSystemTime;
	union {
		FILETIME	theFileTime;
		UInt64		thePeekTime;
	} uTime;

	uTime.thePeekTime = inPeekTime;

	::FileTimeToSystemTime( &uTime.theFileTime, &theSystemTime );

	// 24 characters: 'YYYY-MM-DD HH:MM:SS.mmmZ'
	strTime.Format(
		_T( "%4.4u/%2.2u/%2.2u %2.2u:%2.2u:%2.2u.%3.3uZ" ),
		theSystemTime.wYear,
		theSystemTime.wMonth,
		theSystemTime.wDay,
		theSystemTime.wHour,
		theSystemTime.wMinute,
		theSystemTime.wSecond,
		theSystemTime.wMilliseconds );

	return (CPeekString)strTime;
}


// ----------------------------------------------------------------------------
//		GetTimeString
// ----------------------------------------------------------------------------

CPeekString
GetTimeStringGreenwich()
{
	CString		strTime;
	SYSTEMTIME	theTime;

	::GetSystemTime( &theTime );

	// 24 characters: 'YYYY/MM/DD HH:MM:SS.mmmZ'
	strTime.Format(
		_T( "%4.4u/%2.2u/%2.2u %2.2u:%2.2u:%2.2u.%3.3uZ" ),
		theTime.wYear,
		theTime.wMonth,
		theTime.wDay,
		theTime.wHour,
		theTime.wMinute,
		theTime.wSecond,
		theTime.wMilliseconds );

	return (CPeekString)strTime;
}


// ----------------------------------------------------------------------------
//		GetTimeString
// ----------------------------------------------------------------------------

CPeekString
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

	return (CPeekString)strTime;
}


// ----------------------------------------------------------------------------
//		GetTimeStringFileName
// ----------------------------------------------------------------------------

CPeekString
GetTimeStringFileName( 
	bool bRandomizeMilliseconds ) throw( ... )
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

	WORD wMilliseconds = 0;

	if ( bRandomizeMilliseconds ) {
		wMilliseconds = GetRandomizedMilliSec();
	}
	else {
		wMilliseconds = stTime.wMilliseconds;
	}

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
		wMilliseconds );

	return (CPeekString)strFileName;
}


// ----------------------------------------------------------------------------
//		GetRandMilliseconds
// ----------------------------------------------------------------------------

WORD
GetRandomizedMilliSec( bool bSeed ) throw( ... )
{
	static UInt64 uSeed = 0;

	if ( bSeed ) uSeed = time( NULL );

	srand( static_cast<UInt32>( time( NULL ) + uSeed ) );	

	int	nRandNumber;

	while( (nRandNumber = rand()) > 999 );

	return static_cast<WORD>( nRandNumber );
}


// -----------------------------------------------------------------------------
//		TimeStampToTime
// -----------------------------------------------------------------------------

bool
TimeStampToTime(
	UInt64		inTimeStamp,
	SYSTEMTIME*	outTime )
{
	ASSERT( outTime );
	if ( outTime == NULL ) return false;

	// Convert from UTC TimeStamp to file time.
	UInt64	nFileTime;
	::FileTimeToLocalFileTime( (FILETIME*) &inTimeStamp, (FILETIME*) &nFileTime );

	// Convert to local file time.
	::FileTimeToSystemTime( (FILETIME*) &nFileTime, outTime );

	return true;
}


// -----------------------------------------------------------------------------
//		TimeToTimeStamp
// -----------------------------------------------------------------------------

UInt64
TimeToTimeStamp(
	const SYSTEMTIME&	inDate,
	const SYSTEMTIME&	inTime )
{
	SYSTEMTIME	theDateTime;
	memset( &theDateTime, 0, sizeof( theDateTime ) );
	theDateTime.wYear	= inDate.wYear;
	theDateTime.wMonth	= inDate.wMonth;
	theDateTime.wDay	= inDate.wDay;
	theDateTime.wHour	= inTime.wHour;
	theDateTime.wMinute	= inTime.wMinute;
	theDateTime.wSecond	= inTime.wSecond;

	// Convert the time to local file time.
	UInt64	nFileTime;
	::SystemTimeToFileTime( &theDateTime, (FILETIME*) &nFileTime );

	// Convert the local file time to UTC file time.
	UInt64 nUtcFileTime;
	::LocalFileTimeToFileTime( (FILETIME*) &nFileTime, (FILETIME*) &nUtcFileTime );

	return nUtcFileTime;
}


// ----------------------------------------------------------------------------
//		LegalizeFilename
// ----------------------------------------------------------------------------

CPeekString
LegalizeFilename(
	CPeekString	inFileName,
	TCHAR	inReplace )
{
	if ( inFileName.IsEmpty() ) return inFileName;

	const TCHAR		chUnprintable[] = _T( "\r\n\t" );
	const size_t	nUnprintableCount = COUNTOF( chUnprintable );
	const TCHAR		chIllegal[] = _T("\\/:*?\"<>|");
	const size_t	nIllegalCount = COUNTOF( chIllegal );

	CPeekString	strFileName( inFileName );

	for ( size_t i = 0; i < nUnprintableCount; i++ ) {
		strFileName.Remove( chUnprintable[i] );
	}

	for ( size_t i = 0; i < nIllegalCount; i++ ) {
		strFileName.Replace( chIllegal[i], inReplace );
	}

	if ( strFileName.GetLength() > 64 ) {
		strFileName = strFileName.Right( 64 );
	}
	return strFileName;
}

// ----------------------------------------------------------------------------
//		IsPathValid
// ----------------------------------------------------------------------------

bool
IsPathValid(
	CPeekString	inPath )
{
	ASSERT( inPath.GetLength() > 0 );
	if ( inPath.GetLength() < 2 ) return false;

	if ( inPath.Mid(1,1) != CPeekString( L':' ) ) return false; 

	const int iLen = static_cast<int>( inPath.GetLength() );

	if ( iLen > MAX_PATH ) {
		return false;
	}

	const wchar_t* pszPath = inPath.Format();

	for ( int i = 2; i < iLen; i++ ) {
		wchar_t ch = pszPath[i];
		if ( wcschr( _T("<>:\"|?*"), ch ) ) {
			return false;
		}
	}

	if ( wcsstr( pszPath, _T("//") ) ) {
		return false;
	}
	if ( wcsstr( pszPath, _T("\\\\") ) ) {
		return false;
	}

	// The remote system may have drives that the local system does not.
#if (0)
	// Make sure we have a valid logical drive
	CString strDrive = inPath.Left(3);
	DWORD dwAttr = GetFileAttributes( strDrive );
	if( (dwAttr & FILE_ATTRIBUTE_DIRECTORY) && (dwAttr == 0xffffffff) )  {
		return false;
	}
#endif

	return true;
}

// ----------------------------------------------------------------------------
//		MakePath
// ----------------------------------------------------------------------------

bool
MakePath(
	CPeekString	inPath )
{
	if ( !IsPathValid( inPath ) ) {
		return false;
	}

	CString	tmpPath( inPath );

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
				if ( !bRetVal ) {
					DWORD dwError = GetLastError();
					if ( dwError != ERROR_ALREADY_EXISTS ) {
						return false;
					}
				}
			}
			dirName += _T( '\\' );
		}
		else {
			bDone = true;
		}
	}

	return true;
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

CPeekString
BuildFileName(
	CPeekString	inFileName,
	CPeekString	inOutputDirectory )
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
		(CString)strOutputDirectory,
		theTime.wYear,
		theTime.wMonth,
		theTime.wDay,
		(CString)inFileName );

	return (CPeekString)strFilePath;
}


// ----------------------------------------------------------------------------
//		IsWindowsType
// ----------------------------------------------------------------------------

bool
IsWindowsType(
	tWindowsType	inType )
{
	return (inType == GetWindowsType() );
}


// ----------------------------------------------------------------------------
//		GetWindowsType
// ----------------------------------------------------------------------------

tWindowsType
GetWindowsType()
{
	OSVERSIONINFOEX	osvi;
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
	if ( !GetVersionEx( (OSVERSIONINFO*)&osvi ) ) return kWindows_Unknown;

	if ( osvi.dwPlatformId != VER_PLATFORM_WIN32_NT ) return kWindows_Unknown;

	if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0) ) {
		return kWindows_Vista;
	}

	if ( osvi.dwMajorVersion == 5 ) {
		if ( osvi.dwMinorVersion >= 1 ) {
			return kWindows_XP;
		}
		if ( osvi.dwMinorVersion == 1 ) {
			return kWindows_2000;
		}
	}

	return kWindows_Unknown;
}


// -----------------------------------------------------------------------------
//		CopyToClipboard
// -----------------------------------------------------------------------------

bool
CopyToClipboard(
	CStringA	inString )
{
	CWnd*	pWnd = AfxGetMainWnd();
	HWND	hWnd = (pWnd != NULL) ? pWnd->m_hWnd : NULL;
	BOOL	bResult = OpenClipboard( hWnd );
	bool	bOk = false;
	if ( bResult ) {
		HANDLE	hglbCopy = GlobalAlloc( GMEM_MOVEABLE,  (inString.GetLength() + 1) ); 
		if ( hglbCopy != NULL ) { 
			PSTR	pszCopy = (PSTR) GlobalLock( hglbCopy ); 
			memcpy( pszCopy, inString.GetBuffer(), inString.GetLength() ); 
			pszCopy[ inString.GetLength() ] = 0;
			GlobalUnlock( hglbCopy );

			EmptyClipboard();
			bOk = (SetClipboardData( CF_TEXT, hglbCopy ) != NULL); 
		}
		CloseClipboard();
	}
	return bOk;
}


// ----------------------------------------------------------------------------
//		DumpBytes
// ----------------------------------------------------------------------------

void
DumpBytes(
	HANDLE		hFile,
	size_t		inLength,
	const byte*	inData )
{
	const byte*	pData = inData;
	const byte*	pEnd = inData + inLength;
	int			nCount = 0;
	CStringA	strLine;
	CStringA	strAscii;

	strLine.Format( "%04X: ", (pData - inData) );
	while ( pData < pEnd ) {
		strLine.AppendFormat( "%02X ", *pData );
		if ( isprint( *pData ) ) {
			strAscii.AppendChar( *pData );
		}
		else {
			strAscii.AppendChar( '.' );
		}
		if ( nCount > 14 ) {
			strLine.AppendFormat( "- %s\n%04X: ", strAscii, ((pData - inData) + 1) );
			DWORD	dwBytesWritten;
			::WriteFile( hFile, strLine.GetBuffer(),
				strLine.GetLength(), &dwBytesWritten, NULL );
			strLine.Empty();
			nCount = 0;
			strAscii.Empty();
		}
		else {
			nCount++;
		}
		pData++;
	}
	if ( nCount > 0 ) {
		int			nTabLen = (16 - nCount) * 3;
		CStringA	strTab( ' ', nTabLen );
		strLine.AppendFormat( "%s- %s\n", strTab, strAscii );
	}
	strLine.AppendFormat( "\n" );

	DWORD	dwBytesWritten;
	::WriteFile( hFile, strLine.GetBuffer(), strLine.GetLength(), &dwBytesWritten, NULL );
}

void
DumpBytes(
	HANDLE				hFile,
	const CByteArray&	inData )
{
	DumpBytes( hFile, inData.GetCount(), inData.GetData() );
}

void
DumpBytes(
	HANDLE			hFile,
	CByteVectRef	inData )
{
	DumpBytes( hFile, inData.GetCount(), inData.GetData( inData.GetCount() ) );
}

void
DumpBytes(
	PCTSTR		inPreAmble,
	size_t		inLength,
	const byte*	inData )
{
	TCHAR	szTempName[MAX_PATH];
	TCHAR	lpPathBuffer[BUFSIZE];
	DWORD	dwBufSize = BUFSIZE;

	GetTempPath( dwBufSize, lpPathBuffer );
	GetTempFileName( lpPathBuffer, inPreAmble, 0, szTempName );
	CFile	theFile( szTempName, (CFile::modeCreate | CFile::modeWrite) );
	DumpBytes( theFile.m_hFile, inLength, inData );
}

void
DumpBytes(
	PCTSTR				inPreAmble,
	const CByteArray&	inData )
{
	TCHAR	szTempName[MAX_PATH];
	TCHAR	lpPathBuffer[BUFSIZE];
	DWORD	dwBufSize = BUFSIZE;

	GetTempPath( dwBufSize, lpPathBuffer );
	GetTempFileName( lpPathBuffer, inPreAmble, 0, szTempName );
	CFile	theFile( szTempName, (CFile::modeCreate | CFile::modeWrite) );
	DumpBytes( theFile.m_hFile, inData.GetCount(), inData.GetData() );
}

// ----------------------------------------------------------------------------
//		ParseTimeStampGreenwich
// ----------------------------------------------------------------------------

UInt64
ParseTimeStampGreenwich(
	CPeekString	inTime )
{
	CString strTime( inTime );

	SYSTEMTIME	stTime;
	memset( &stTime, 0, sizeof( SYSTEMTIME ) );

	strTime.MakeLower();

	TIME_ZONE_INFORMATION	tzInfo;
	memset( &tzInfo, 0, sizeof( tzInfo ) );

	UInt32	nYear = 0;
	UInt32	nMonth = 0;
	UInt32	nDay = 0;
	UInt32	nHour = 0;
	UInt32	nMinute = 0;
	UInt32	nSecond = 0;
	UInt32	nMilliseconds = 0;
	//	int		nBias = 0;
	_stscanf_s( strTime, _T( "%u/%u/%u %u:%u:%u:%uz" ),
		&nYear, &nMonth, &nDay,
		&nHour, &nMinute, &nSecond, &nMilliseconds );

	stTime.wYear = (WORD) nYear;
	stTime.wMonth = (WORD) nMonth;
	stTime.wDay = (WORD) nDay;
	stTime.wHour = (WORD) nHour;
	stTime.wMinute = (WORD) nMinute;
	stTime.wSecond = (WORD) nSecond;
	stTime.wMilliseconds = (WORD) nMilliseconds;

	UInt64 nFileTime;
	::SystemTimeToFileTime( &stTime, (FILETIME*) &nFileTime );

	//	// Convert the local file time to UTC file time.
	//	UInt64 nUtcFileTime;
	//	::LocalFileTimeToFileTime( (FILETIME*) &nFileTime, (FILETIME*) &nUtcFileTime );
	//	return nUtcFileTime;

	return nFileTime;
}
