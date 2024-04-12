// =============================================================================
//	CatchHR.h
// =============================================================================
//	Copyright (c) 2003-2006 WildPackets, Inc. All rights reserved.

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
// HRESULT hr = S_OK;
// try
// {
//     // Call a function that returns HRESULT and throw if failed:
//     ThrowIfFailed( foo() );
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

/// \brief Helper function for testing an HRESULT and throwing a
/// CAtlException if it indicates failure.
/// \param hr The error code you want to check/throw.
inline void ThrowIfFailed( const HRESULT hr )
{
	using namespace ATL;
	if ( FAILED(hr) ) AtlThrow( hr );
}

/// \brief Helper function for testing an HRESULT and throwing a
/// CAtlException if it is not S_OK. Success codes other than S_OK
/// are translated to E_FAIL.
/// \param hr The error code you want to check/throw.
inline void ThrowIfNotOK( const HRESULT hr )
{
	using namespace ATL;
	if ( FAILED(hr) ) AtlThrow( hr );
	// Success codes other than S_OK become E_FAIL.
	if ( S_OK != hr ) AtlThrow( E_FAIL );
}

/// \brief Helper function for testing an boolean expression and throwing a
/// CAtlException if it is true.
/// \param b A boolean expression.
/// \param hr The error you want to throw if the expression is true.
inline void ThrowIf( const bool b, const HRESULT hr = E_FAIL )
{
	using namespace ATL;
	if ( b ) AtlThrow( hr );
}

/// \brief Helper function for testing a pointer and throwing a
/// CAtlException if it is NULL.
/// \param p The pointer you want to check.
/// \param hr The error you want to throw if the pointer is NULL.
inline void ThrowIfNull( const void* p, const HRESULT hr = E_POINTER )
{
	using namespace ATL;
	if ( p == NULL ) AtlThrow( hr );
}

/// \brief Helper function for testing a Win32 result code for ERROR_SUCCESS
/// and throwing a CAtlException.
/// \param x The return code you want to check.
inline void ThrowIfError( const DWORD x )
{
	using namespace ATL;
	if ( x != ERROR_SUCCESS ) AtlThrow( HRESULT_FROM_WIN32(x) );
}

inline void VERIFY_HR( const HRESULT hr )
{
	ThrowIfFailed( hr );
}

inline void VERIFY_OK( const HRESULT hr )
{
	ThrowIfNotOK( hr );
}

#define RETURN_IF_HR( x ) if (FAILED(hr)){ return hr; }

