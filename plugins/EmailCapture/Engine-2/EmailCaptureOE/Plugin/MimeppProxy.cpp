// =============================================================================
//	MimeppProxy.cpp
// =============================================================================
//	Copyright (c) 2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "MimeppProxy.h"

#ifndef HE_LINUX

#include "DelayImp.h"

// ============================================================================
//		ExpFilter
// ============================================================================

int
ExpFilter(
	EXCEPTION_POINTERS*	pPtrs,
	const CArrayString&	inPathList )
{
	if ( (pPtrs == NULL) || (pPtrs->ExceptionRecord->NumberParameters == 0) ) {
		return EXCEPTION_EXECUTE_HANDLER;
	}

	PDelayLoadInfo	pInfo( reinterpret_cast<PDelayLoadInfo>( pPtrs->ExceptionRecord->ExceptionInformation[0] ) );
	if ( pInfo == NULL ) return EXCEPTION_EXECUTE_HANDLER;

	CPeekStringA	strNameA( pInfo->szDll );
	CPeekString		strName( strNameA );
	for ( size_t i = 0; i < inPathList.size(); i++ ) {
		CPeekString	strPath( inPathList[i] );
		if ( strPath.Right( 1 ) != L"\\" ) {
			strPath += L"\\";
		}
		strPath += strName;

		HMODULE	hLib( LoadLibrary( strPath ) );
		if ( hLib != NULL ) {
			pInfo->pfnCur = GetProcAddress( hLib, pInfo->dlp.szProcName );
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}


// ============================================================================
//		LoadAll
// ============================================================================

bool
LoadAll(
	LPCSTR				inDllName,
	const CArrayString&	inPathList  )
{
	__try { 
		HRESULT	hr( __HrLoadAllImportsForDll( inDllName ) );
		if ( FAILED( hr ) ) return false;
	} 
	__except( ExpFilter( GetExceptionInformation(), inPathList ) ) {
		return false;
	}
	return true;
}

#endif // !HE_LINUX

