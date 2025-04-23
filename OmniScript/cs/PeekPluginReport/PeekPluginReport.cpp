#include <Windows.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using std::string;
using std::wstring;

#define kPluginNameMaxLength				 256
#define kPluginLoadErrorMessageMaxLength	1024

typedef int (CALLBACK* pPluginProc)( int, void* );

typedef unsigned short		UInt16;
typedef unsigned int		UInt32;
typedef unsigned long long	UInt64;
typedef GUID				PluginID;

typedef struct PluginLoadParam {
	UInt32					fAPIVersion;
	UInt32					fAppCapabilities;
	wchar_t*				fName;
	PluginID				fID;
	UInt16					fAttributes;
	UInt16					fSupportedActions;
	UInt16					fDefaultActions;
	UInt32					fSupportedCount;
	UInt32*					fSupportedProtoSpecs;
	void*					fClientAppData;
	void*					fAppContextData;	// Plug-in retains to give back to callbacks
	wchar_t*				fLoadErrorMessage;
	LCID					fLocaleId;
	UInt64					fAttributesEx;		// Attributes = fAttributes | fAttributesEx
} PluginLoadParamW;

std::string
Convert(
	const wstring&	inOther )
{
	string str;

	if ( !inOther.empty() ) {
		size_t	nSize( inOther.size() + 1 );
		std::auto_ptr<char>	spBuf( new char[nSize] );
		if ( spBuf.get() == NULL ) return str;

		size_t	nNewSize( 0 );
		errno_t nErr( wcstombs_s( &nNewSize, spBuf.get(),
						nSize, inOther.c_str(), inOther.size() ) );
		if ( nErr == 0 ) {
			str.assign( spBuf.get(), (nNewSize - 1) );
		}
	}

	return str;
}


class CPluginInfo
{
protected:
	wstring	m_Name;
	GUID	m_Id;

public:
	;		CPluginInfo( wchar_t* inName, GUID inId )
		:	m_Name( inName )
		,	m_Id( inId )
	{
	}

	string	Format() const {
		OLECHAR*	bstrId;
		StringFromCLSID( m_Id, &bstrId );

		wstring		str( m_Name );
		str.append( L" : " );
		str.append( bstrId );

		return Convert( str );
	}
};


// ============================================================================
//		SE_Exception
// ============================================================================

class SE_Exception
{
private:
	unsigned int	m_nSE;

public:
	;				SE_Exception() {}
	;				SE_Exception( unsigned int n ) : m_nSE( n ) {}
	;				~SE_Exception() {}
	unsigned int	GetSeNumber() { return m_nSE; }
};


#pragma comment(linker, "/EXPORT:GetProtocolNameCallback=_GetProtocolNameCallback@0,PRIVATE")
extern "C" int WINAPI GetProtocolNameCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetProtocolLongNameCallback=_GetProtocolLongNameCallback@0,PRIVATE")
extern "C" int WINAPI GetProtocolLongNameCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetProtocolHierNameCallback=_GetProtocolHierNameCallback@0,PRIVATE")
extern "C" int WINAPI GetProtocolHierNameCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetProtocolParentCallback=_GetProtocolParentCallback@0,PRIVATE")
extern "C" int WINAPI GetProtocolParentCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetProtocolColorCallback=_GetProtocolColorCallback@0,PRIVATE")
extern "C" int WINAPI GetProtocolColorCallback() { return -1; }

#pragma comment(linker, "/EXPORT:IsDescendentOfCallback=_IsDescendentOfCallback@0,PRIVATE")
extern "C" int WINAPI IsDescendentOfCallback() { return -1; }

#pragma comment(linker, "/EXPORT:LookupNameCallback=_LookupNameCallback@0,PRIVATE")
extern "C" int WINAPI LookupNameCallback() { return -1; }

#pragma comment(linker, "/EXPORT:LookupEntryCallback=_LookupEntryCallback@0,PRIVATE")
extern "C" int WINAPI LookupEntryCallback() { return -1; }

#pragma comment(linker, "/EXPORT:AddNameEntryCallback=_AddNameEntryCallback@0,PRIVATE")
extern "C" int WINAPI AddNameEntryCallback() { return -1; }

#pragma comment(linker, "/EXPORT:InvokeNameEditDialogCallback=_InvokeNameEditDialogCallback@0,PRIVATE")
extern "C" int WINAPI InvokeNameEditDialogCallback() { return -1; }

#pragma comment(linker, "/EXPORT:LookupNameCallbackEx=_LookupNameCallbackEx@0,PRIVATE")
extern "C" int WINAPI LookupNameCallbackEx() { return -1; }

#pragma comment(linker, "/EXPORT:LookupEntryCallbackEx=_LookupEntryCallbackEx@0,PRIVATE")
extern "C" int WINAPI LookupEntryCallbackEx() { return -1; }

#pragma comment(linker, "/EXPORT:AddNameEntryCallbackEx=_AddNameEntryCallbackEx@0,PRIVATE")
extern "C" int WINAPI AddNameEntryCallbackEx() { return -1; }

#pragma comment(linker, "/EXPORT:InvokeNameEditDialogCallbackEx=_InvokeNameEditDialogCallbackEx@0,PRIVATE")
extern "C" int WINAPI InvokeNameEditDialogCallbackEx() { return -1; }

#pragma comment(linker, "/EXPORT:ResolveAddressCallback=_ResolveAddressCallback@0,PRIVATE")
extern "C" int WINAPI ResolveAddressCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SummaryGetEntryCallback=_SummaryGetEntryCallback@0,PRIVATE")
extern "C" int WINAPI SummaryGetEntryCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SummaryModifyEntryCallback=_SummaryModifyEntryCallback@0,PRIVATE")
extern "C" int WINAPI SummaryModifyEntryCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PacketGetLayerCallback=_PacketGetLayerCallback@0,PRIVATE")
extern "C" int WINAPI PacketGetLayerCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PacketGetDataLayerCallback=_PacketGetDataLayerCallback@0,PRIVATE")
extern "C" int WINAPI PacketGetDataLayerCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PacketGetHeaderLayerCallback=_PacketGetHeaderLayerCallback@0,PRIVATE")
extern "C" int WINAPI PacketGetHeaderLayerCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PacketGetAddressCallback=_PacketGetAddressCallback@0,PRIVATE")
extern "C" int WINAPI PacketGetAddressCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PacketGetGPSInfoCallback=_PacketGetGPSInfoCallback@0,PRIVATE")
extern "C" int WINAPI PacketGetGPSInfoCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PrefsGetValueCallback=_PrefsGetValueCallback@0,PRIVATE")
extern "C" int WINAPI PrefsGetValueCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PrefsSetValueCallback=_PrefsSetValueCallback@0,PRIVATE")
extern "C" int WINAPI PrefsSetValueCallback() { return -1; }

#pragma comment(linker, "/EXPORT:PrefsGetPrefsPathCallback=_PrefsGetPrefsPathCallback@0,PRIVATE")
extern "C" int WINAPI PrefsGetPrefsPathCallback() { return -1; }

#pragma comment(linker, "/EXPORT:InsertPacketCallback=_InsertPacketCallback@0,PRIVATE")
extern "C" int WINAPI InsertPacketCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SelectPacketsCallback=_SelectPacketsCallback@0,PRIVATE")
extern "C" int WINAPI SelectPacketsCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SelectPacketsExCallback=_SelectPacketsExCallback@0,PRIVATE")
extern "C" int WINAPI SelectPacketsExCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetPacketCountCallback=_GetPacketCountCallback@0,PRIVATE")
extern "C" int WINAPI GetPacketCountCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetFirstPacketIndexCallback=_GetFirstPacketIndexCallback@0,PRIVATE")
extern "C" int WINAPI GetFirstPacketIndexCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetPacketCallback=_GetPacketCallback@0,PRIVATE")
extern "C" int WINAPI GetPacketCallback() { return -1; }

#pragma comment(linker, "/EXPORT:ClaimPacketStringCallback=_ClaimPacketStringCallback@0,PRIVATE")
extern "C" int WINAPI ClaimPacketStringCallback() { return -1; }

#pragma comment(linker, "/EXPORT:DecodeOpCallback=_DecodeOpCallback@0,PRIVATE")
extern "C" int WINAPI DecodeOpCallback() { return -1; }

#pragma comment(linker, "/EXPORT:CallUserDecodeCallback=_CallUserDecodeCallback@0,PRIVATE")
extern "C" int WINAPI CallUserDecodeCallback() { return -1; }

#pragma comment(linker, "/EXPORT:AddUserDecodeCallback=_AddUserDecodeCallback@0,PRIVATE")
extern "C" int WINAPI AddUserDecodeCallback() { return -1; }

#pragma comment(linker, "/EXPORT:AddTabCallback=_AddTabCallback@0,PRIVATE")
extern "C" int WINAPI AddTabCallback() { return -1; }

#pragma comment(linker, "/EXPORT:AddTabWithGroupCallback=_AddTabWithGroupCallback@0,PRIVATE")
extern "C" int WINAPI AddTabWithGroupCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetThemeColorsCallback=_GetThemeColorsCallback@0,PRIVATE")
extern "C" int WINAPI GetThemeColorsCallback() { return -1; }

#pragma comment(linker, "/EXPORT:NotifyCallback=_NotifyCallback@0,PRIVATE")
extern "C" int WINAPI NotifyCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SendPluginMessageCallback=_SendPluginMessageCallback@0,PRIVATE")
extern "C" int WINAPI SendPluginMessageCallback() { return -1; }

#pragma comment(linker, "/EXPORT:MakeFilterCallback=_MakeFilterCallback@0,PRIVATE")
extern "C" int WINAPI MakeFilterCallback() { return -1; }

#pragma comment(linker, "/EXPORT:GetAppResourcePathCallback=_GetAppResourcePathCallback@4,PRIVATE")
extern "C" int WINAPI GetAppResourcePathCallback( wchar_t* ) { return -1; }

#pragma comment(linker, "/EXPORT:SendPacketCallback=_SendPacketCallback@0,PRIVATE")
extern "C" int WINAPI SendPacketCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SetPacketListColumnCallback=_SetPacketListColumnCallback@0,PRIVATE")
extern "C" int WINAPI SetPacketListColumnCallback() { return -1; }

#pragma comment(linker, "/EXPORT:SaveContextCallback=_SaveContextCallback@0,PRIVATE")
extern "C" int WINAPI SaveContextCallback() { return -1; }


// ============================================================================
//		SE_TranslationFunc
// ============================================================================

void
SE_TranslationFunc(
	unsigned int		u,
	EXCEPTION_POINTERS*	pExp )
{
	throw SE_Exception();
}

// ============================================================================
//		GetDllPaths
// ============================================================================

void
GetDllPaths( string inPath, vector<string>& outPaths )
{
	string strPath( inPath );
	strPath.append( "\\Plugins" );

	string strFind( strPath );
	strFind.append( "\\*.dll" );

	WIN32_FIND_DATA stFindData;
	HANDLE hFind = ::FindFirstFile( strFind.c_str(), &stFindData );
	if( hFind != INVALID_HANDLE_VALUE ) {
		do {
			string strPlugin( strPath );
			strPlugin.append( "\\" );
			strPlugin.append( stFindData.cFileName );

			outPaths.push_back( strPlugin );
		} while ( FindNextFile( hFind, &stFindData ) != 0 );
	}
}


// ============================================================================
//		Logger
// ============================================================================

class Logger
{
protected:
	int		m_nLevel;
	HANDLE	m_hFile;

public:
	;		Logger() : m_nLevel( 0 ), m_hFile( NULL ) {}
	;		~Logger() {
		if ( m_hFile ) {
			::CloseHandle( m_hFile );
		}
	}

	void Log( char* inMsg ) {
		if ( m_hFile == NULL ) return;

		DWORD	dwWritten( 0 );
		char*	pMsg((inMsg != NULL) ? inMsg : "\n");

		size_t	nLength( strlen( pMsg ) );
		::WriteFile( m_hFile, pMsg, nLength, &dwWritten, NULL );
	}

	void SetLevel( int inLevel ) { m_nLevel = inLevel; }
};


// ============================================================================
//		main
// ============================================================================

int
main(
	int		argc,
	char**	argv )
{
	_set_se_translator( SE_TranslationFunc );

	wchar_t*	pValue( NULL );
	size_t		nLength( 0 );
	errno_t		nError( _wdupenv_s( &pValue, &nLength, L"APPDATA" ) );

	string	strPath( "C:\\Program Files (x86)\\WildPackets\\OmniPeek" );
	string	strReport( "PluginReport.txt" );
	Logger	log;
	int		nVerbose( 0 );
	HANDLE	hLog( NULL );

#ifdef _DEBUG
	nVerbose = 1;
#endif

	if ( nVerbose > 0 ){
		hLog = ::CreateFile( "PeekPluginLog.txt", GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	}

	for (int i = 0; i < argc; ++i ) {
		if ( argv[i][0] == '-' ) {
			switch ( argv[i][1] ) {
			case 'p':
				++i;
				strPath = argv[i];
				break;

			case 'o':
				++i;
				strReport = argv[i];
				break;

			case 'v':
				++i;
				nVerbose = atoi(argv[i]);
				if ( nVerbose < 0 ) {
					nVerbose = 0;
				}
				break;
			}
		}
	}

	if ( nVerbose > 0 ) {
		printf( "PeekPluginReport:\n" );
		printf( "  OmniPeek Path: %s\n", strPath.c_str() );
		printf( "  Report:        %s\n\n", strReport.c_str() );
	}

	vector<string>	ayDllPaths;
	GetDllPaths( strPath, ayDllPaths );

	vector<CPluginInfo>	ayPlugins;
	vector<string>::const_iterator pItem = ayDllPaths.begin();
	vector<string>::const_iterator pEnd = ayDllPaths.end();

	char szCurrentDir[MAX_PATH];
	::GetCurrentDirectoryA( MAX_PATH, szCurrentDir );

	string strPeek( strPath + "\\Peek.dll" );
	HMODULE hPeek( ::LoadLibraryA( strPeek.c_str() ) );
	if ( nVerbose > 0 ) {
		printf( "Loaded: %s\n", strPeek.c_str() );
	}

	strPath.append( "\\Plugins" );
	::SetCurrentDirectoryA( strPath.c_str() );
	if ( nVerbose > 0 ) {
		printf( "Set current directory: %s\n\n", strPath.c_str() );
	}

	for ( ; pItem != pEnd; pItem++ ) {
		HMODULE hPlugin = ::LoadLibraryA( pItem->c_str() );
		if ( hPlugin != INVALID_HANDLE_VALUE ) {
			if ( nVerbose > 0 ) {
				printf( "Loaded: %s\n", pItem->c_str() );
			}
			pPluginProc pPluginMain = (pPluginProc)::GetProcAddress( hPlugin, "PlugInMain" );
			if ( pPluginMain != NULL ) {
				wchar_t			szName[256] = { 0 };
				wchar_t			szError[1024] = { 0 };
				PluginLoadParam	lp;
				memset( &lp, 0, sizeof( lp ) );
				lp.fAPIVersion = 12;
				lp.fAppCapabilities = 1991;
				lp.fName = szName;
				lp.fLoadErrorMessage = szError;
				lp.fLocaleId = 0x0409;

				try {
					int nResult( (*pPluginMain)( 0, &lp ) );
					if ( wcslen( szName ) > 0 ) {
						nResult = 0;
						ayPlugins.push_back( CPluginInfo( szName, lp.fID ) );
					}
				}
				catch ( ... ) {
					;
				}
			}
			else {
				if ( nVerbose > 0 ) {
					printf( "*** Failed to load: %s\n", pItem->c_str() );
				}
			}
			::FreeLibrary( hPlugin );
		}
	}
	if ( nVerbose > 0 ) {
		printf( "\n" );
	}

	if ( hPeek != INVALID_HANDLE_VALUE ) {
		::FreeLibrary( hPeek );
		hPeek = reinterpret_cast<HMODULE>( INVALID_HANDLE_VALUE );
	}

	::SetCurrentDirectoryA( szCurrentDir );
	if ( nVerbose > 0 ) {
		printf( "Set current directory: %s\n\n", szCurrentDir );
	}

	HANDLE hFile( ::CreateFile( strReport.c_str(), GENERIC_WRITE, 0, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) );
	if ( hFile != INVALID_HANDLE_VALUE ) {
		if ( nVerbose > 0 ) {
			printf( "Report opened: %s\n", strReport.c_str() );
		}
		size_t nCount = ayPlugins.size();
		for ( size_t i = 0; i < nCount; ++i ) {
			const CPluginInfo&	info( ayPlugins[i] );
			string str( info.Format() );
			str.append( "\r\n" );
			DWORD dwWritten( 0 );
			::WriteFile( hFile, str.c_str(), str.size(), &dwWritten, NULL );
			if ( nVerbose > 0 ) {
				printf( "  %s", str.c_str() );
			}
		}
		::CloseHandle( hFile );
	}
	else {
		if ( nVerbose > 0 ) {
			printf( "Failed to open report: %s\n", strReport.c_str() );
		}
	}

	return 0;
}
