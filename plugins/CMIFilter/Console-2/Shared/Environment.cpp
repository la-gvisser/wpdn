// ============================================================================
//	Environment.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Environment.h"

#if defined(_DEBUG) && defined(_AFX)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// ----------------------------------------------------------------------------
//		GetOSVersion [static]
// ----------------------------------------------------------------------------

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
XP             2              5               1
*/

void
CEnvironment::GetOSVersion(
	CString& outString )
{
	CString			strOSVersion;
	CString			strOSBuildNumber;
	OSVERSIONINFO	theVersion;
	theVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if ( ::GetVersionEx( &theVersion ) ) {
		switch ( theVersion.dwPlatformId ) 		{
		case VER_PLATFORM_WIN32_WINDOWS:
			{
				if ( theVersion.dwMinorVersion <= 10 ) {
					switch( LOWORD( theVersion.dwBuildNumber ) ) {
						case 2222:
							strOSVersion = _T( "Windows 98 SE" );
						break;

						case 1998:
							strOSVersion = _T( "Windows 98" );
						break;

						case 950:
							strOSVersion = _T( "Windows 95 OSR1" );
						break;

						default:
							strOSVersion = _T( "Windows 95 OSR2" );
						break;

					}
				}
				else {
					strOSVersion = _T( "Windows Me" );
				}

				// Win9x: build number is in low word.
				strOSBuildNumber.Format( _T( "%d" ), LOWORD( theVersion.dwBuildNumber ) );
			}
			break;

		case VER_PLATFORM_WIN32_NT:
			{
				strOSVersion = _T( "Windows NT" );

				if ( theVersion.dwMajorVersion == 5 ) {
					if( theVersion.dwMinorVersion == 0 ) {
						strOSVersion = _T( "Windows 2000" );
					}
					else if( theVersion.dwMinorVersion == 1 ) {
						strOSVersion = _T( "Windows XP" );
					}
				}

				// WinNT/2K: build number is entire field.
				strOSBuildNumber.Format( _T( "%d" ), theVersion.dwBuildNumber );
			}
			break;
		}

		CString	strVersion;
		strVersion.Format( _T( " (%d.%d" ), theVersion.dwMajorVersion, theVersion.dwMinorVersion );
		strOSVersion += strVersion;

		if ( theVersion.szCSDVersion[0] != 0 ) {
			strOSVersion += _T( ": " );
			strOSVersion += theVersion.szCSDVersion;
		}

		strOSVersion += _T( ')' );
	}

	outString = strOSVersion;
}


// ----------------------------------------------------------------------------
//		GetModuleVersion [static]
// ----------------------------------------------------------------------------

bool
CEnvironment::GetModuleVersion(
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	TCHAR 	szModulePath[MAX_PATH];
	::GetModuleFileName( NULL, szModulePath, MAX_PATH );

	CString	strModulePath(szModulePath);
	return GetFileVersion( strModulePath, outFileVersion, outProductVersion );
}


// ----------------------------------------------------------------------------
//		GetFileVersion [static]
// ----------------------------------------------------------------------------

bool
CEnvironment::GetFileVersion(
	const CString&	inFilePath,
	ENV_VERSION&	outFileVersion,
	ENV_VERSION&	outProductVersion )
{
	bool	bResult = false;

	// Copy the file path.
	// Why are the string parameters to the functions not const?
	CString	strFilePath( inFilePath );
	TCHAR*	pszFilePath = strFilePath.GetBufferSetLength( MAX_PATH );

	// Get the file version info size.
	DWORD	nIgnored;
	DWORD	nVersionSize = ::GetFileVersionInfoSize( pszFilePath, &nIgnored );

	if ( nVersionSize > 0 ) {
		// Alloc the version info.
		void*	pVersionInfo = malloc( nVersionSize );
		ASSERT( pVersionInfo != NULL );

		if ( pVersionInfo != NULL ) {
			// Get the file version info.
			if ( ::GetFileVersionInfo( pszFilePath, 0, nVersionSize, pVersionInfo ) ) {
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
	strFilePath.ReleaseBuffer();

	return bResult;
}


// ----------------------------------------------------------------------------
//		FormatVersion [static]
// ----------------------------------------------------------------------------

void
CEnvironment::FormatVersion(
	const ENV_VERSION&	inVersion,
	CString&			outVersionString )
{
	outVersionString.Format( _T( "%u.%u.%u.%u" ),
		inVersion.MajorVer, inVersion.MinorVer,
		inVersion.MajorRev, inVersion.MinorRev );
}
