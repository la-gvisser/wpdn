// ============================================================================
//	loadso.cpp
//	g++ -o loadso -std=c++11 loadso.cpp -ldl
// ============================================================================

#include <stdio.h>

#if defined(__GNUC__)
#include <dlfcn.h>
#else
#define RTLD_LAZY	0
int		dlclose( void* );
int		dlerror();
void*	dlopen( char*, int );
int		dlsym( void*, char* );
#endif

typedef struct
#if defined(__GNUC__)
__attribute__ ((packed))
#endif
_GUID
{
	unsigned int	Data1;
	unsigned short	Data2;
	unsigned short	Data3;
	unsigned char	Data4[8];
} GUID;

typedef struct
#if defined(__GNUC__)
__attribute__ ((packed))
#endif
_MODULEVERSION
{
	unsigned int	nMajorVersion;
	unsigned int	nMinorVersion;
	unsigned int	nRevisionNumber;
	unsigned int	nBuildNumber;
	wchar_t			szPublisher[256];
	wchar_t			szProductName[256];
} MODULEVERSION;


typedef int (CREATORFUNC)(void* pv, const GUID& iid, void** ppv );
typedef int (GETCLASSOBJECTFUNC)( const GUID& cid, const GUID& iid, void** ppv );
typedef int (CANUNLOADNOWFUNC)( void );
typedef int (GERVERSIONFUNC)( MODULEVERSION* pVersion );


int
main(
	int		argc,
	char*	argv[] )
{
	printf( "Begin\n" );

	if ( argc < 2 ) {
		printf( "File name must be specified.\n" );
		return 1;
	}

	printf( "File: %s\n", argv[1] );

#if defined(__GNUC__)
	void* hLibrary = dlopen( argv[1], RTLD_LAZY );
	if ( hLibrary == nullptr ) {
		printf( "Failed to open the library: %s.", dlerror() );
		return 1;
	}

	printf( "Library opened\n" );

	GETCLASSOBJECTFUNC*	pGetClassObjectFunc = reinterpret_cast<GETCLASSOBJECTFUNC*>( dlsym( hLibrary, "HeModuleGetClassObject" ) );
	if ( pGetClassObjectFunc == nullptr ) {
		printf( "Failed to resolve GetClassObject function" );
	}

	CANUNLOADNOWFUNC*	pCanUnloadNowFunc = reinterpret_cast<CANUNLOADNOWFUNC*>( dlsym( hLibrary, "HeModuleCanUnloadNow" ) );
	if ( pCanUnloadNowFunc == nullptr ) {
		printf( "Failed to resolve CanUnloadNow function" );
	}

	GERVERSIONFUNC*		pGetVersionFunc = reinterpret_cast<GERVERSIONFUNC*>( dlsym( hLibrary, "HeModuleGetVersion" ) );
	if ( pGetVersionFunc == nullptr ) {
		printf( "Failed to resolve GetVersion function" );
	}
	else {
		MODULEVERSION	version;
		int				nResult = (*pGetVersionFunc)( &version );
		if ( nResult == 0 ) {
			printf( "Version: %d.%d.%d.%d\n", version.nMajorVersion, version.nMinorVersion,
				version.nRevisionNumber, version.nBuildNumber );
		}
		else {
			printf( "Call to GetVersion failed: %d", nResult );
		}
	}

	dlclose( hLibrary );
	printf( "Library closed\n" );
#else
	printf( "This utility doesn't do anything under Windows.\n" );
#endif

	printf( "Done\n" );

	return 0;
}
