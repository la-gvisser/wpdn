// ============================================================================
//	ComInit.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002. All rights reserved.

#pragma once

#include <objbase.h>

class CComInitilize
{
public:
	HRESULT Init( DWORD dwCoInit ) throw()
	{
		HRESULT hr = S_OK;
		__try
		{
			hr = ::CoInitializeEx( NULL, dwCoInit );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			if ( STATUS_NO_MEMORY == GetExceptionCode() )
			{
				hr = E_OUTOFMEMORY;
			}
			else
			{
				hr = E_FAIL;
			}
		}
		return hr;
	}

	HRESULT Term() throw()
	{
		::CoUninitialize();
		return S_OK;
	}
};


class CComAutoInitialize : public CComInitilize
{
public:
	CComAutoInitialize( DWORD dwCoInit )
	{
		HRESULT hr = CComInitilize::Init( dwCoInit );
		if ( FAILED(hr) ) AtlThrow( hr );
	}
	~CComAutoInitialize() throw()
	{
		CComInitilize::Term();
	}
private :
	HRESULT Init();	// Not implemented. CComInitilize::Init should never be called.
	HRESULT Term();	// Not implemented. CComInitilize::Term should never be called.
};
