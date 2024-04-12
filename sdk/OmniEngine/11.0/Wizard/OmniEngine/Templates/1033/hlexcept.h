// =============================================================================
//	hlexcept.h
// =============================================================================
//	Copyright (c) 2005-2017 Savvius, Inc. All rights reserved.

#ifndef HLEXCEPT_H
#define HLEXCEPT_H

#include "hecore.h"
#include "heresult.h"
#include <exception>
#include <new>

namespace HeLib
{

/// \class CHeLibException
/// \brief Basic exception class
/// 
/// Same as ATL::CAtlException. A light-weight exception class that carries
/// only an error code. If you want to add a string message, derive from this.
class CHeLibException
{
public:
	CHeLibException() throw() :
		m_hr( HE_E_FAIL )
	{
	}

	CHeLibException( HeResult hr ) throw() :
		m_hr( hr )
	{
	}

	operator HeResult() const throw()
	{
		return m_hr;
	}

public:
	HeResult	m_hr;
};

/// \brief Helper function for throwing an CHeLibException.
/// \param hr The error code you want to throw.
HE_NOINLINE inline void HE_NORETURN HeThrow( const HeResult hr )
{
	throw HeLib::CHeLibException( hr );
}

/// \def HE_TRY
/// \brief Start a HE_TRY/HE_CATCH block (just maps to 'try').
#ifndef HE_TRY
#define HE_TRY try
#endif

/// \def HE_CATCH
/// \brief Catch various exception types and map to HeResult.
#ifndef HE_CATCH

#if defined(_AFX)

// MFC/ATL version
#define HE_CATCH(_hr) \
	catch ( HeLib::CHeLibException& e ) { \
		_hr = e; \
	} catch ( std::bad_alloc& ) { \
		_hr = HE_E_OUT_OF_MEMORY; \
	} catch ( std::exception& ) { \
		_hr = HE_E_FAIL; \
	} catch ( ATL::CAtlException& e ) { \
		_hr = (HeResult) e.m_hr; \
	} catch ( COleException* e ) { \
		_hr = e->m_sc; \
		e->Delete(); \
	} catch ( CMemoryException* e ) { \
		_hr = HE_E_OUT_OF_MEMORY; \
		e->Delete(); \
	} catch ( CException* e ) { \
		_hr = HE_E_UNEXPECTED; \
		e->Delete(); \
	} catch ( ... ) { \
		_hr = HE_E_UNEXPECTED; \
	}

#elif defined(_ATL)

// ATL-only version
#define HE_CATCH(_hr) \
	catch ( HeLib::CHeLibException& e ) { \
		_hr = e; \
	} catch ( std::bad_alloc& ) { \
		_hr = HE_E_OUT_OF_MEMORY; \
	} catch ( std::exception& ) { \
		_hr = HE_E_FAIL; \
	} catch ( ATL::CAtlException& e ) { \
		_hr = (HeResult) e.m_hr; \
	} catch ( ... ) { \
		_hr = HE_E_UNEXPECTED; \
	}

#else

// HeLib-only version
#define HE_CATCH(_hr) \
	catch ( HeLib::CHeLibException& e ) { \
		_hr = e; \
	} catch ( std::bad_alloc& ) { \
		_hr = HE_E_OUT_OF_MEMORY; \
	} catch ( std::exception& ) { \
		_hr = HE_E_FAIL; \
	} catch ( ... ) { \
		_hr = HE_E_UNEXPECTED; \
	}

#endif

#endif

/// \brief Helper function for testing an HeResult and throwing a
/// CHeLibException if it indicates failure.
/// \param hr The error code you want to check/throw.
inline void HeThrowIfFailed( const HeResult hr )
{
	if ( HE_FAILED(hr) ) HeLib::HeThrow( hr );
}

/// \brief Helper function for testing an HeResult and throwing a
/// CHeLibException if it is not HE_S_OK. Success codes other than HE_S_OK
/// are translated to HE_E_FAIL.
/// \param hr The error code you want to check/throw.
inline void HeThrowIfNotOK( const HeResult hr )
{
	if ( HE_FAILED(hr) ) HeLib::HeThrow( hr );
	// Success codes other than HE_S_OK become HE_E_FAIL.
	if ( HE_S_OK != hr ) HeLib::HeThrow( HE_E_FAIL );
}

/// \brief Helper function for testing an boolean expression and throwing a
/// CHeLibException if it is true.
/// \param b A boolean expression.
/// \param hr The error you want to throw if the expression is true.
inline void HeThrowIf( const bool b, const HeResult hr = HE_E_FAIL )
{
	if ( b ) HeLib::HeThrow( hr );
}

/// \brief Helper function for testing a pointer and throwing a
/// CHeLibException if it is NULL.
/// \param p The pointer you want to check.
inline void HeThrowIfNull( const void* p, const HeResult hr = HE_E_NULL_POINTER )
{
	if ( p == NULL ) HeLib::HeThrow( hr );
}

/// \brief Helper function for testing a result code for non-zero and throwing
/// a CHeLibException.
/// \param x The return code you want to check.
inline void HeThrowIfError( const int x )
{
	if ( x != 0 ) HeLib::HeThrow( Helium::HeResultFromErrorCode( x ) );
}

/// \brief Helper function for testing a result code for EOF and throwing
/// a CHeLibException.
/// \param x The return code you want to check.
inline void HeThrowIfEOF( const int x )
{
	// EOF is always -1, right?
	if ( x == -1 ) HeLib::HeThrow( HE_E_EOF );
}

#if defined(HE_WIN32) && defined(WINVER)	// Must have included <windows.h>

/// \brief Throw a Win32 error.
inline void HE_NORETURN HeThrowWin32( const DWORD dwWin32Err )
{
	HeLib::HeThrow( Helium::HeResultFromWin32( dwWin32Err ) );
}

/// \brief Throw the result of GetLastError().
inline void HE_NORETURN HeThrowLastWin32()
{
	HeThrowWin32( ::GetLastError() );
}

#endif

} /* namespace HeLib */

#endif /* HLEXCEPT_H */
