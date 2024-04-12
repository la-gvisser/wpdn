// =============================================================================
//	Environment.cpp
// =============================================================================
//	Copyright (c) 2000-2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Environment.h"
#include "CatchHR.h"
#if (_ATL_VER >= 0x0700) && (_ATL_VER <= 0x0800)
#include <atlcrypt.h>	// Part of ATL Server, deprecated in VS 2008.
#endif

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

static LPCTSTR	kIEVersionPath    = _T("SOFTWARE\\Microsoft\\Internet Explorer");
static LPCTSTR	kIEVersionKey     = _T("Version");
static LPCTSTR	kDefaultIEVersion = _T("Unknown (Pre-IE4)");

// -----------------------------------------------------------------------------
//		GetOSVersion
// -----------------------------------------------------------------------------

/* From CodeProject.com:
         dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
95             1              4               0             950
95 SP1         1              4               0        >950 && <=1080
95 OSR2        1              4             <10           >1080
98             1              4              10            1998
98 SP1         1              4              10       >1998 && <2183
98 SE          1              4              10          >=2183
ME             1              4              90            3000

NT 3.51        2              3              51            1057
NT 4           2              4               0            1381
2000           2              5               0            2195
XP             2              5               1            2600
2003 Server    2              5               2
Vista		   2              6               0
*/

void
CEnvironment::GetOSVersion(
	CString& outString )
{
	CString			strOSVersion;
	CString			strOSBuildNumber;
	OSVERSIONINFO	theVersion;
	theVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if ( ::GetVersionEx( &theVersion ) )
	{
		switch ( theVersion.dwPlatformId )
		{
			case VER_PLATFORM_WIN32_WINDOWS:
			{
				if( theVersion.dwMinorVersion <= 10 )
				{
					switch( LOWORD( theVersion.dwBuildNumber ) )
					{
						case 2222:
							strOSVersion = _T("Windows 98 SE");
						break;

						case 1998:
							strOSVersion = _T("Windows 98");
						break;

						case 950:
							strOSVersion = _T("Windows 95 OSR1");
						break;

						default:
							strOSVersion = _T("Windows 95 OSR2");
						break;
					}
				}
				else
				{
					strOSVersion = _T("Windows Me");
				}

				// Win9x: build number is in low word.
				strOSBuildNumber.Format( _T("%d"), LOWORD(theVersion.dwBuildNumber) );
			}
			break;

			case VER_PLATFORM_WIN32_NT:
			{
				strOSVersion = _T("Windows NT");

				if ( theVersion.dwMajorVersion == 5 )
				{
					if( theVersion.dwMinorVersion == 0 )
					{
						strOSVersion = _T("Windows 2000");
					}
					else if( theVersion.dwMinorVersion == 1 )
					{
						strOSVersion = _T("Windows XP");
					}
					else if( theVersion.dwMinorVersion == 2 )
					{
						strOSVersion = _T("Windows 2003 Server");
					}
				}
				else if( theVersion.dwMajorVersion == 6 )
				{
					strOSVersion = _T("Windows Vista");
				}

				// WinNT/2K: build number is entire field.
				strOSBuildNumber.Format( _T("%d"), theVersion.dwBuildNumber );
			}
			break;
		}

		TCHAR	szVersion[256];
		_stprintf( szVersion, _T("(%d.%d"), theVersion.dwMajorVersion, theVersion.dwMinorVersion );

		strOSVersion += _T(" ");
		strOSVersion += szVersion;

		if ( theVersion.szCSDVersion[0] != 0 )
		{
			strOSVersion += _T(": ");
			strOSVersion += theVersion.szCSDVersion;
		}

		strOSVersion += _T(')');
	}

	outString = strOSVersion;
}


// -----------------------------------------------------------------------------
//		GetIEVersion
// -----------------------------------------------------------------------------
// Get the Internet Explorer version from the registry.
// It lives here: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Internet Explorer
// Note versions of IE prior to 4 used a different key.  On those setups,
// this key will not exist.  We don't care, because we're required version 6
// or higher, anyway.

void
CEnvironment::GetIEVersion(
	CString& outString )
{
	outString = kDefaultIEVersion;

	// Open the key.
	ATL::CRegKey theKey;
	if ( !theKey.Open( HKEY_LOCAL_MACHINE, kIEVersionPath, KEY_READ ) )
	{
		CString	strFilePath;
		ULONG	nPathLen = MAX_PATH;
		if ( !theKey.QueryStringValue( kIEVersionKey, 
				strFilePath.GetBuffer( nPathLen ), &nPathLen ) )
		{
			outString = strFilePath;
		}
		strFilePath.ReleaseBuffer();
	}
}


// -----------------------------------------------------------------------------
//		GetModuleVersion
// -----------------------------------------------------------------------------

bool
CEnvironment::GetModuleVersion(
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	TCHAR 	szModulePath[MAX_PATH];
	DWORD	cch = ::GetModuleFileName( NULL, szModulePath, MAX_PATH );
	if ( (cch == 0) || (cch == MAX_PATH) ) return false;
	return GetFileVersion( CString( szModulePath ), outFileVersion, outProductVersion );
}


// -----------------------------------------------------------------------------
//		GetFileVersion
// -----------------------------------------------------------------------------

bool
CEnvironment::GetFileVersion(
	const CString&	inFilePath,
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	bool	bResult = false;
	
	// Copy the file path.
	// Why are the string parameters to the functions not const?
	TCHAR	szFilePath[MAX_PATH];
	_tcscpy( szFilePath, inFilePath );

	// Get the file version info size.
	DWORD	nIgnored;
	DWORD	nVersionSize = ::GetFileVersionInfoSize( szFilePath, &nIgnored );

	if ( nVersionSize > 0 )
	{
		// Alloc the version info.
		void*	pVersionInfo = malloc( nVersionSize );
		ASSERT( pVersionInfo != NULL );

		if ( pVersionInfo != NULL )
		{
			// Get the file version info.
			if ( ::GetFileVersionInfo( szFilePath, 0, nVersionSize, pVersionInfo ) )
			{
				VS_FIXEDFILEINFO*	pFileInfo;
				UINT				nFileInfoLength = sizeof(VS_FIXEDFILEINFO);
				if ( ::VerQueryValue( pVersionInfo, _T("\\"), (void**) &pFileInfo, &nFileInfoLength ) )
				{
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


// -----------------------------------------------------------------------------
//		FormatVersion
// -----------------------------------------------------------------------------

void
CEnvironment::FormatVersion( 
	const ENV_VERSION&	inVersion,
	CString&			outVersionString )
{
	outVersionString.Format(
		_T("%u.%u.%u.%u"),
		inVersion.MajorVer,
		inVersion.MinorVer,
		inVersion.MajorRev,
		inVersion.MinorRev );
}


#if (_ATL_VER >= 0x0700) && (_ATL_VER <= 0x0800)

// -----------------------------------------------------------------------------
//		GetFileHash
// -----------------------------------------------------------------------------

bool
CEnvironment::GetFileHash( 
	const CString&	inFilePath,
	CByteStream&	outHash )
{
	HRESULT	hr = S_OK;

	try
	{
		// Init crypto.
		CCryptProv	theProv;
		hr = theProv.Initialize( PROV_RSA_FULL, NULL, MS_DEF_PROV, CRYPT_VERIFYCONTEXT );
		ATLASSERT( SUCCEEDED( hr ) );
		VERIFY_HR( hr );

		// Load file.
		CByteStream	fileStream;
		if( fileStream.ReadFromFile( inFilePath ) && fileStream.GetLength() > 0 )
		{
			CCryptMD5Hash	theHash;
			hr = theHash.Initialize( theProv );
			ATLASSERT( SUCCEEDED( hr ) );
			VERIFY_HR( hr );

			hr = theHash.AddData( (BYTE*) fileStream.GetData(), fileStream.GetLength() );
			ATLASSERT( SUCCEEDED( hr ) );
			VERIFY_HR( hr );

			DWORD	nHashSize = 0;
			hr = theHash.GetSize( &nHashSize );
			ATLASSERT( SUCCEEDED( hr ) );
			VERIFY_HR( hr );

			outHash.SetLength( nHashSize );
			if( outHash.GetLength() >= nHashSize )
			{
				hr = theHash.GetValue( (BYTE*) outHash.GetData(), &nHashSize );
				ATLASSERT( SUCCEEDED( hr ) );
				VERIFY_HR( hr );
			}
		}
	}
	CATCH_HR(hr)

	return SUCCEEDED( hr );
}

#endif /* #if (_ATL_VER >= 0x0700) && (_ATL_VER <= 0x0800) */

