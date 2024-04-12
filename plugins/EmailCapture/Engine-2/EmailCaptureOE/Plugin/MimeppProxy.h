// =============================================================================
//	MimeppProxy.h
// =============================================================================
//	Copyright (c) 2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_LINUX

#if defined(__GNUC__)
#include <dlfcn.h>
#else
#define RTLD_LAZY	0
int		dlclose( void* );
int		dlerror();
void*	dlopen( char*, int );
int		dlsym( void*, char* );
#endif

typedef void*		LIB_TYPE;

#else

#include "DelayImp.h"

typedef HMODULE		LIB_TYPE;

int		ExpFilter( EXCEPTION_POINTERS*	pPtrs, const CArrayString&	inPathList );
bool	LoadAll( LPCSTR inDllName, const CArrayString&	inPathList );

#endif // HE_LINUX

typedef bool (INITFUNC)( void );
typedef void (FINALFUNC)( void );


class CMimeppProxy
{
protected:
	LIB_TYPE	m_hLibrary;
	INITFUNC*	m_pInitialize;
	FINALFUNC*	m_pFinalize;

public:
	CMimeppProxy() 
		:	m_hLibrary( nullptr )
		,	m_pInitialize( nullptr )
		,	m_pFinalize( nullptr )
	{
#ifdef HE_LINUX
		m_hLibrary = dlopen( "/usr/lib/omni/plugins/libmimepp.so", RTLD_NOW | RTLD_GLOBAL );
#else // HE_LINUX

	CArrayString	saPathList;
	{
		CPeekString strPath;
		strPath.GetEnvironmentVariable( L"CommonProgramFiles" );
		strPath.Append( L"\\Savvius" );
		saPathList.push_back( strPath );
	}

#ifdef WIN64
		LoadAll( "mimepp64.dll", saPathList );
#else
		LoadAll( "mimepp.dll", saPathList );
#endif // WIN64

#endif
	}
	~CMimeppProxy()
	{
		if ( m_hLibrary != nullptr ) {
#ifdef HE_LINUX
			dlclose( m_hLibrary );
#else
			::CloseHandle( m_hLibrary );
#endif
			m_hLibrary = nullptr;
		}
	}

	bool	Initialize() {
		if ( m_hLibrary != nullptr ) {
#ifdef HE_LINUX
			m_pInitialize = reinterpret_cast<INITFUNC*>( dlsym( m_hLibrary, "_ZN6mimepp10InitializeEv" ) );
			m_pFinalize = reinterpret_cast<FINALFUNC*>( dlsym( m_hLibrary, "Finalize" ) );
#else
			m_pInitialize = reinterpret_cast<INITFUNC*>( ::GetProcAddress( m_hLibrary, "Initialize" ) );
			m_pFinalize = reinterpret_cast<FINALFUNC*>( ::GetProcAddress( m_hLibrary, "Finalize" ) );
#endif

			if ( m_pInitialize != nullptr ) {
				return (*m_pInitialize)();
			}
		}
		return false;
	}

	void	Finalize() {
		if ( m_pFinalize ) {
			(*m_pFinalize)();
		}
	}
};
