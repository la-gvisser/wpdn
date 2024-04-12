// =============================================================================
//	CMIFilter.cpp
//		implementation of the CCMIFilterApp class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include <eh.h>
#include "CMIFilter.h"
#include "CMIFilterPlugin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CCMIFilterApp theApp;

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


// =============================================================================
//		CMIFilterApp
// =============================================================================

CCMIFilterApp::CCMIFilterApp()
{
	int	se_set = 0;
	try {
		_set_se_translator( SE_TranslationFunc );
		se_set = 1;
	}
	catch ( ... ) {
		// Note the error...
		se_set = -1;
	}
	ASSERT( se_set == 1 );
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCMIFilterApp, CWinApp)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		InitInstance
// ----------------------------------------------------------------------------

BOOL
CCMIFilterApp::InitInstance()
{
	m_Plugin.DllMain( m_hInstance, DLL_PROCESS_ATTACH, 0 );

	return CWinApp::InitInstance();
}


// ----------------------------------------------------------------------------
//		PlugInMain
// ----------------------------------------------------------------------------

#if defined(_MSC_VER) && !defined(_WIN64)
#pragma comment(linker, "/EXPORT:PlugInMain=_PlugInMain@8,PRIVATE")
#endif

extern "C"
#if (defined(_MSC_VER) && defined(_WIN64)) || defined(__GNUC__)
  __declspec(dllexport)
#endif
int WINAPI
PlugInMain(
	SInt16				inMessage,
	PluginParamBlock*	ioParams )
{
	CPeekPlugin& Plugin = theApp.GetPlugin();
	return Plugin.HandleMessage( inMessage, ioParams );
}
