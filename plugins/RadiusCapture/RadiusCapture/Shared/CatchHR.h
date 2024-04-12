// =============================================================================
//	CatchHR.h
// =============================================================================
//	Copyright (c) 2003-2005 WildPackets, Inc. All rights reserved.

#pragma once

// Use CATCH_HR to catch an HRESULT, most likely the result of AtlThrow.
// In an MFC app, AtlThrow is turned into a type of CException, whereas in ATL,
// it is a CAtlException. Many of the MFC/ATL shared classes use AtlThrow
// internally, so it's important to catch these exceptions.
//
// See the following code from atlexcept.h line 63:
// #ifdef _AFX
//     if( hr == E_OUTOFMEMORY )
//     {
//         AfxThrowMemoryException();
//     }
//     else
//     {
//         AfxThrowOleException( hr );
//     }
// #else
//     throw CAtlException( hr );
// #endif
//
// Typical usage:
// HRESULT hr = E_FAIL;
// try
// {
//     // Call a function that returns HRESULT and throw if failed:
//     hr = foo();
//     if ( FAILED(hr) ) AtlThrow( hr );
//     // Use an class that uses AtlThrow internally (CAtlArray):
//     const int item& = m_MyCAtlArray.GetAt( 1 );
// }
// CATCH_HR(hr)
// return hr;

#ifndef CATCH_HR
#ifdef _AFX
#define CATCH_HR(_hr) \
	catch ( ATL::CAtlException& e ) { \
		_hr = e; \
	} catch ( COleException* e ) { \
		_hr = e->m_sc; \
		e->Delete(); \
	} catch ( CMemoryException* e ) { \
		_hr = E_OUTOFMEMORY; \
		e->Delete(); \
	} catch ( CException* e ) { \
		_hr = E_UNEXPECTED; \
		e->Delete(); \
	} catch ( ... ) { \
		_hr = E_UNEXPECTED; \
	}
#else
#define CATCH_HR(_hr) \
	catch ( ATL::CAtlException& e ) { \
		_hr = e; \
	} catch ( ... ) { \
		_hr = E_UNEXPECTED; \
	}
#endif
#endif

inline void VERIFY_HR( const HRESULT hr )
{
	using namespace ATL;
	if ( FAILED(hr) ) AtlThrow( hr );
}

inline void VERIFY_OK( const HRESULT hr )
{
	using namespace ATL;
	if ( FAILED(hr) ) AtlThrow( hr );
	// Success codes other than S_OK become E_FAIL.
	if ( S_OK != hr ) AtlThrow( E_FAIL );
}
