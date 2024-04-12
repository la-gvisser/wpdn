// ============================================================================
//	Version.cpp
// ============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Version.h"
#include <cwchar>

#ifdef WP_LINUX
extern int swscanf (__const wchar_t *__restrict __s,
		    __const wchar_t *__restrict __format, ...)
     __THROW /* __attribute__ ((__format__ (__wscanf__, 2, 3))) */;
#endif


// ============================================================================
//		CVersion
// ============================================================================

// -----------------------------------------------------------------------------
//		Format
// -----------------------------------------------------------------------------

CPeekString
CVersion::Format() const
{
	CPeekOutString	str;
	str << m_nMajorVersion << L"." << m_nMinorVersion << L"."
		<< m_nMajorRevision << L"." << m_nMinorRevision;
	return str;
}


// -----------------------------------------------------------------------------
//		Parse
// -----------------------------------------------------------------------------

bool
CVersion::Parse(
	const CPeekString&	inVersion )
{
	UInt32	nMajorVersion = 0;
	UInt32	nMinorVersion = 0;
	UInt32	nMajorRevision = 0;
	UInt32	nMinorRevision = 0;

#ifdef _WIN32
	bool	bMatch( (4 != swscanf_s( inVersion, L"%u.%u.%u.%u",
						&nMajorVersion, &nMinorVersion,
						&nMajorRevision, &nMinorRevision )) &&
					(3 != swscanf_s( inVersion, L"%u.%u.%u",
						&nMajorVersion, &nMinorVersion,
						&nMajorRevision )) &&
					(2 != swscanf_s( inVersion, L"%u.%u",
						&nMajorVersion, &nMinorVersion )) &&
					(1 != swscanf_s( inVersion, L"%u", &nMajorVersion )) );
#else
	bool	bMatch( (4 != swscanf( inVersion, L"%u.%u.%u.%u",
						&nMajorVersion, &nMinorVersion,
						&nMajorRevision, &nMinorRevision )) &&
					(3 != swscanf( inVersion, L"%u.%u.%u",
						&nMajorVersion, &nMinorVersion,
						&nMajorRevision )) &&
					(2 != swscanf( inVersion, L"%u.%u",
						&nMajorVersion, &nMinorVersion )) &&
					(1 != swscanf( inVersion, L"%u", &nMajorVersion )) );
#endif

	if ( bMatch ) {
		return false;
	}
	Set( nMajorVersion, nMinorVersion, nMajorRevision, nMinorRevision );

	return true;
}

// ============================================================================
//		Environment
// ============================================================================

namespace Environment
{
#ifdef IMP_GETOSVERSION
// ----------------------------------------------------------------------------
//		GetOSVersion
// ----------------------------------------------------------------------------
// Adapted from the MSDN sample code here:
// http://msdn.microsoft.com/en-us/library/ms724429(VS.85).aspx
// Another interesting, but different, article on this:
// http://vbnet.mvps.org/index.html?code/helpers/iswinversion.htm

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

CPeekString
GetOSVersion()
{
	CPeekString		strOSVersion( L"Windows" );

	SYSTEM_INFO	si;
	ZeroMemory( &si, sizeof( SYSTEM_INFO ) );

	OSVERSIONINFOEX	osvi;
	ZeroMemory( &osvi, sizeof( OSVERSIONINFOEX ) );
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );

#pragma warning (disable : 4996)
	// GetVersionEx has been deprecated.
	if( !::GetVersionEx( (OSVERSIONINFO*) &osvi ) ){
		return strOSVersion;	// Default to Windows.
	}
#pragma warning (default : 4996)

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	PGNSI	pGNSI = (PGNSI)GetProcAddress(
		GetModuleHandle( L"kernel32.dll" ), "GetNativeSystemInfo" );
	if ( pGNSI != nullptr ) {
		pGNSI( &si );
	}
	else {
		GetSystemInfo( &si );
	}

	if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
		// Test for the specific product.
		if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 3) ) {
			// Windows 8.1 or Server 2012 r2.
			strOSVersion += (osvi.wProductType == VER_NT_WORKSTATION)
				? L" 8.1"
				: L" Server 2012 R2";
		}
		else if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 2) ) {
			// Windows 8 or Server 2012.
			strOSVersion += (osvi.wProductType == VER_NT_WORKSTATION)
				? L" 8"
				: L" Server 2012";
		}
		else if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1) ) {
			// Windows 7 or Server 2008 R2.
			strOSVersion += (osvi.wProductType == VER_NT_WORKSTATION)
				? L" 7"
				: L" Server 2008 R2";
		}
		else if ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0) ) {
			// Windows Vista or 2008 Server.
			strOSVersion += (osvi.wProductType == VER_NT_WORKSTATION)
				? L" Vista"
				: L" Server 2008";

			PGPI	pGPI = (PGPI)GetProcAddress( GetModuleHandle( L"kernel32.dll" ), "GetProductInfo" );

			// Get OS type.
			DWORD	dwType;
			pGPI( 6, 0, 0, 0, &dwType );
			switch( dwType ) {
			case PRODUCT_ULTIMATE:
				strOSVersion += L" Ultimate Edition";
				break;
			case PRODUCT_HOME_PREMIUM:
				strOSVersion += L" Home Premium Edition";
				break;
			case PRODUCT_HOME_BASIC:
				strOSVersion += L" Home Basic Edition";
				break;
			case PRODUCT_ENTERPRISE:
				strOSVersion += L" Enterprise Edition";
				break;
			case PRODUCT_BUSINESS:
				strOSVersion += L" Business Edition";
				break;
			case PRODUCT_STARTER:
				strOSVersion += L" Starter Edition";
				break;
			case PRODUCT_CLUSTER_SERVER:
				strOSVersion += L" Cluster Server Edition";
				break;
			case PRODUCT_DATACENTER_SERVER:
				strOSVersion += L" Datacenter Edition";
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				strOSVersion += L" Datacenter Edition (core installation)";
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				strOSVersion += L" Enterprise Edition";
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				strOSVersion += L" Enterprise Edition (core installation)";
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				strOSVersion += L" Enterprise Edition for Itanium-based Systems";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				strOSVersion += L" Small Business Server";
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				strOSVersion += L" Small Business Server Premium Edition";
				break;
			case PRODUCT_STANDARD_SERVER:
				strOSVersion += L" Standard Edition";
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				strOSVersion += L" Standard Edition (core installation)";
				break;
			case PRODUCT_WEB_SERVER:
				strOSVersion += L" Web Server Edition";
				break;
			}

			if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) {
				strOSVersion += L", 64-bit";
			}
			else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ) {
				strOSVersion += L", 32-bit";
			}
		}
		else if ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 2) ) {
			// Windows Server 2003 or XP Pro x64.
			if ( GetSystemMetrics( SM_SERVERR2 ) ) {
				strOSVersion += L" Server 2003 R2";
			}
			else if ( osvi.wSuiteMask == VER_SUITE_STORAGE_SERVER ) {
				strOSVersion += L" Storage Server 2003";
			}
			else if ( (osvi.wProductType == VER_NT_WORKSTATION) &&
						(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ) {
				strOSVersion += L" XP Professional x64 Edition";
			}
			else {
				strOSVersion += L" Server 2003";
			}

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION ) {
				if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ) {
					strOSVersion += L" for Itanium";
				}
				else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) {
					strOSVersion += L" x64 Edition";
				}
			}
		}
		else if ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 1) ) {
			// Windows XP.
			strOSVersion += L" XP";
			strOSVersion += (osvi.wSuiteMask & VER_SUITE_PERSONAL)
				? L" Home Edition"
				: L" Professional" ;
		}
		else if ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0) ) {
			// Windows 2000.
			strOSVersion += L" 2000";
			if ( osvi.wProductType == VER_NT_WORKSTATION ){
				strOSVersion += L" Professional";
			}
			else {
				if ( osvi.wSuiteMask & VER_SUITE_DATACENTER ) {
					strOSVersion += L" Datacenter Server";
				}
				else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) {
					strOSVersion += L" Advanced Server";
				}
				else {
					strOSVersion += L" Server";
				}
			}
		}
		else {
			// Default to Windows NT.
			strOSVersion += L" NT";
		}
	}
	else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
		// Win9X: build number is in low word.
		DWORD dwBuildNumber = LOWORD( osvi.dwBuildNumber );
		if ( osvi.dwMinorVersion <= 10 ) {
			switch( dwBuildNumber ) {
			case 2222:
				strOSVersion += L" 98 SE";
				break;
			case 1998:
				strOSVersion += L" 98";
				break;
			case 950:
				strOSVersion += L" 95 OSR1";
				break;
			default:
				strOSVersion += L" 95 OSR2";
				break;
			}
		}
		else {
			strOSVersion += L" Me";
		}
	}

	// Include service pack (if any).
	if ( osvi.szCSDVersion[0] != 0 ) {
		strOSVersion += osvi.szCSDVersion;
	}

	// Include build number if any.
	if ( osvi.dwBuildNumber != 0 ) {
		CPeekOutString	strBuildNumber;
		strBuildNumber << L" (build " << osvi.dwBuildNumber << L")";
		strOSVersion += strBuildNumber;
	}

	return strOSVersion;
}
#endif // IMP_GETOSVERSION


#ifdef _WIN32
// ----------------------------------------------------------------------------
//		GetModuleVersion
// ----------------------------------------------------------------------------

bool
GetModuleVersion(
	CVersion&	outFile,
	CVersion&	outProduct,
	HMODULE		inModule )
{
#ifdef PEEK_UI
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
#endif

	wchar_t szModulePath[MAX_PATH];
	::GetModuleFileName( inModule, szModulePath, MAX_PATH );

	CPeekString	strModulePath( szModulePath );
	return GetFileVersion( strModulePath, outFile, outProduct );
}
#endif // _WIN32


// ----------------------------------------------------------------------------
//		GetFileVersion
// ----------------------------------------------------------------------------

bool
GetFileVersion(
	const wchar_t*	inFilePath,
	CVersion&		outFile,
	CVersion&		outProduct )
{
	bool	bResult = false;

	// Copy the file path.
	// Why are the string parameters to the functions not const?
	CPeekString		strFilePath( inFilePath );

	// Get the file version info size.
#ifdef _WIN32
	UInt32	nIgnored;
	UInt32	nVersionSize = ::GetFileVersionInfoSize( strFilePath, &nIgnored );

	if ( nVersionSize > 0 ) {
		// Alloc the version info.
		void*	pVersionInfo = malloc( nVersionSize );
		_ASSERTE( pVersionInfo != nullptr );

		if ( pVersionInfo != nullptr ) {
			// Get the file version info.
			if ( ::GetFileVersionInfo( strFilePath, 0, nVersionSize, pVersionInfo ) ) {
				VS_FIXEDFILEINFO*	pFileInfo;
				UINT				nFileInfoLength = sizeof( VS_FIXEDFILEINFO );
				bool				bQueryResult =
					(::VerQueryValue( pVersionInfo, L"\\",
						reinterpret_cast<void**>( &pFileInfo ), &nFileInfoLength ) != FALSE);
				if ( bQueryResult ) {
					// Copy the file version info.
					outFile.Set( pFileInfo->dwFileVersionMS, pFileInfo->dwFileVersionLS );

					// Copy the product version info.
					outProduct.Set( pFileInfo->dwProductVersionMS, pFileInfo->dwProductVersionLS );

					bResult = true;
				}
			}

			// Free the version info.
			free( pVersionInfo );
		}
	}
#else
	// TODO
#endif // _WIN32

	return bResult;
}

}	// namespace Environment
