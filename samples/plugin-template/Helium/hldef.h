// =============================================================================
//	hldef.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef HLDEF_H
#define HLDEF_H

#include "hecore.h"
#include "hlstrconv.h"
#include <cstdarg>
#include <cstdio>

namespace HeLib
{

template <class T> inline void HeIgnoreUnused(T const&) {}
#ifndef HE_IGNORE_UNUSED
#define HE_IGNORE_UNUSED(x)	HeIgnoreUnused(x)
#endif /* HE_IGNORE_UNUSED */

#ifndef HE_OFFSETOFCLASS
#define HE_OFFSETOFCLASS(base,derived)	(reinterpret_cast<uintptr_t>(static_cast<base*>(reinterpret_cast<derived*>(8)))-8)
#endif /* HE_OFFSETOFCLASS */

#if !defined(HE_ASSERT_ENABLE) && (defined(_DEBUG) || defined(DEBUG))
#define HE_ASSERT_ENABLE
#endif

#ifdef HE_ASSERT_ENABLE

inline void
HeAssert(
	const char*		pszFileName,
	int				nLineNo,
	const char*		pszExpr )
{
#if defined(HE_WIN32) && defined(WINVER)	// Must have included <windows.h>
	char	sz[1024];
	std::sprintf( sz, "%s(%d) : ASSERT: %s\n", 
		pszFileName, nLineNo, pszExpr ? pszExpr : "nullptr" );
	OutputDebugStringA( sz );
	DebugBreak();
#else
	std::printf( "%s(%d) : ASSERT: %s\n", 
		pszFileName, nLineNo, pszExpr ? pszExpr : "nullptr" );
#endif
}

#else

inline void HeAssert(const char*, int, const char*) {}

#endif /* HE_ASSERT_ENABLE */

#ifdef HE_ASSERT_ENABLE

/// \def HE_ASSERT
#ifndef HE_ASSERT
#define HE_ASSERT(_p)	do { if (!(_p)) HeLib::HeAssert( __FILE__, __LINE__, # _p ); } while (0)
#endif

/// \def HE_VERIFY
#ifndef HE_VERIFY
#define HE_VERIFY(_p)	HE_ASSERT(_p)
#endif

#else

#ifndef HE_ASSERT
#define HE_ASSERT(_p)	((void)0)
#endif

#ifndef HE_VERIFY
#define HE_VERIFY(_p)	((void)(_p))
#endif

#endif /* HE_ASSERT_ENABLE */


#if !defined(HE_TRACE_ENABLE) && (defined(_DEBUG) || defined(DEBUG))
#define HE_TRACE_ENABLE
#endif

#ifdef HE_TRACE_ENABLE

inline void
HeTraceA(
	const char*		pszFileName,
	int				nLineNo,
	const char*		pszFmt,
	std::va_list	args )
{
#ifndef HE_TRACE_FILEANDLINENO
	HE_IGNORE_UNUSED( pszFileName );
	HE_IGNORE_UNUSED( nLineNo );
#endif
#if defined(HE_WIN32) && defined(WINVER)	// Must have included <windows.h>
	const std::size_t	cchMax = 1024;
	char				sz[cchMax];
	int					cch = 0;
#ifdef HE_TRACE_FILEANDLINENO
	cch = _snprintf( sz, cchMax, "%s(%d) : ", pszFileName, nLineNo );
	if ( cch < 0 ) cch = 0;
#endif
	_vsnprintf( sz + cch, cchMax - cch, pszFmt, args );
	sz[cchMax - 1] = 0;
	OutputDebugStringA( sz );
#else
#ifdef HE_TRACE_FILEANDLINENO
	std::printf( "%s(%d) : ", pszFileName, nLineNo );
#endif
	std::vprintf( pszFmt, args );
#endif
}

inline void
HeTraceW(
	const char*		pszFileName,
	int				nLineNo,
	const wchar_t*	pszFmt,
	std::va_list	args )
{
#ifndef HE_TRACE_FILEANDLINENO
	HE_IGNORE_UNUSED( pszFileName );
	HE_IGNORE_UNUSED( nLineNo );
#endif
	const std::size_t	cchMax = 1024;
	wchar_t				sz[cchMax];
	int					cch = 0;
#ifdef HE_TRACE_FILEANDLINENO
	cch = std::swprintf( sz, cchMax, L"%S(%d) : ", pszFileName, nLineNo );
	if ( cch < 0 ) cch = 0;
#endif
	std::vswprintf( sz + cch, cchMax - cch, pszFmt, args );
	sz[cchMax - 1] = 0;
#if defined(HE_WIN32) && defined(WINVER)	// Must have included <windows.h>
	OutputDebugStringW( sz );
#else
	std::printf( "%s", static_cast<const char*>(HeLib::CW2A( sz )) );
#endif
}

#else

inline void HeTraceA(const char*, int, const char*, va_list) {}
inline void HeTraceW(const char*, int, const wchar_t*, va_list) {}

#endif /* HE_TRACE_ENABLE */

class CHeTraceCaller
{
public:
	CHeTraceCaller(const char* pszFileName, int nLineNo) :
		m_pszFileName( pszFileName ),
		m_nLineNo( nLineNo )
	{
	}
	void operator()(const char* pszFmt, ...) const
	{
		std::va_list args;
		va_start( args, pszFmt );
		HeTraceA( m_pszFileName, m_nLineNo, pszFmt, args );
		va_end( args );
	}
	void operator()(const wchar_t* pszFmt, ...) const
	{
		std::va_list args;
		va_start( args, pszFmt );
		HeTraceW( m_pszFileName, m_nLineNo, pszFmt, args );
		va_end( args );
	}

private:
	CHeTraceCaller(const CHeTraceCaller&);
	CHeTraceCaller& operator=(const CHeTraceCaller&);

private:
	const char* const	m_pszFileName;
	const int			m_nLineNo;
};

class CHeNoopTraceCaller
{
public:
	CHeNoopTraceCaller() {}
	void operator()(const char*, ...) const {}
	void operator()(const wchar_t*, ...) const {}
};

#ifdef HE_TRACE_ENABLE

/// \def HE_TRACE
#ifndef HE_TRACE
#define HE_TRACE	HeLib::CHeTraceCaller(__FILE__, __LINE__)
#endif

#else

#ifndef HE_TRACE
#define HE_TRACE	HeLib::CHeNoopTraceCaller()
#endif

#endif /* HE_TRACE_ENABLE */


#define HE_DECL_PROP_VAL(type,var) \
	HE_IMETHOD_(type)			Get ## var() const = 0; \
	HE_IMETHOD_(void)			Set ## var( type in ## var ) = 0

#define HE_IMPL_PROP_VAL(type,var) \
protected: \
	type m_ ## var; \
public: \
	HE_IMETHOD_(type)			Get ## var() const { ObjectLock lock( this ); return m_ ## var; } \
	HE_IMETHOD_(void)			Set ## var( type in ## var ) { ObjectLock lock( this ); m_ ## var = in ## var; }

#define HE_DECL_PROP_PTR(type,var) \
	HE_IMETHOD_(const type*)	Get ## var() const = 0; \
	HE_IMETHOD_(void)			Set ## var( const type* p ## var ) = 0

#define HE_IMPL_PROP_PTR(type,var) \
protected: \
	type* m_p ## var; \
public: \
	HE_IMETHOD_(const type*)	Get ## var() const { ObjectLock lock( this ); return m_p ## var; } \
	HE_IMETHOD_(void)			Set ## var( const type* p ## var ) { ObjectLock lock( this ); m_p ## var = p ## var; }

#define HE_DECL_PROP_REF(type,var) \
	HE_IMETHOD_(const type&)	Get ## var() const = 0; \
	HE_IMETHOD_(void)			Set ## var( const type& in ## var ) = 0

#define HE_IMPL_PROP_REF(type,var) \
protected: \
	type m_ ## var; \
public: \
	HE_IMETHOD_(const type&)	Get ## var() const { ObjectLock lock( this ); return m_ ## var; } \
	HE_IMETHOD_(void)			Set ## var( const type& in ## var ) { ObjectLock lock( this ); m_ ## var = in ## var; }

#define HE_DECL_PROP_ITF(type,var) \
	HE_IMETHOD					Get ## var( type** pp ## var ) = 0; \
	HE_IMETHOD_(void)			Set ## var( type* p ## var ) = 0

#define HE_IMPL_PROP_ITF(type,var) \
protected: \
	HeLib::CHePtr<type> m_sp ## var; \
public: \
	HE_IMETHOD					Get ## var( type** pp ## var ) { ObjectLock lock( this ); return m_sp ## var.CopyTo( pp ## var ); } \
	HE_IMETHOD_(void)			Set ## var( type* p ## var ) { ObjectLock lock( this ); m_sp ## var = p ## var; }

} /* namespace HeLib */

#endif /* HLDEF_H */
