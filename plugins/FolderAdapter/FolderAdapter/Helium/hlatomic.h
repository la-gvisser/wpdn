// =============================================================================
//	hlatomic.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.
//
//	See: http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html

#ifndef HLATOMIC_H
#define HLATOMIC_H

#include "hecore.h"
#ifdef HE_WIN32
#include <cstddef>
#include <Windows.h>
#else
#include <cstdint>
#include <pthread.h>
#endif

#ifdef HE_OPT_ATOMICINTERLOCKED
#include <intrin.h>
#pragma intrinsic( _InterlockedIncrement )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedExchange )
#pragma intrinsic( _InterlockedCompareExchange )
#endif

namespace HeLib
{

inline SInt32
HeAtomicIncrement32( SInt32 volatile* p )
{
#if defined(HE_WIN32)
#ifdef HE_OPT_ATOMICINTERLOCKED
	return _InterlockedIncrement( p );
#else
	return InterlockedIncrement( p );
#endif
#elif defined(__GNUC__)
	return __atomic_add_fetch( p, 1, __ATOMIC_SEQ_CST );
#else
#pragma warning HeAtomicIncrement32 not implemented
#endif
}

inline SInt32
HeAtomicDecrement32( SInt32 volatile* p )
{
#if defined(HE_WIN32)
#ifdef HE_OPT_ATOMICINTERLOCKED
	return _InterlockedDecrement( p );
#else
	return InterlockedDecrement( p );
#endif
#elif defined(__GNUC__)
	return __atomic_sub_fetch( p, 1, __ATOMIC_SEQ_CST );
#else
#pragma warning HeAtomicDecrement32 not implemented
#endif
}

inline SInt32
HeAtomicExchange32( SInt32 volatile* p, SInt32 v )
{
#if defined(HE_WIN32)
#ifdef HE_OPT_ATOMICINTERLOCKED
	return _InterlockedExchange( p, v );
#else
	return InterlockedExchange( p, v );
#endif
#elif defined(__GNUC__)
	return __atomic_exchange_n( p, v, __ATOMIC_SEQ_CST );
#else
#pragma warning HeAtomicExchange32 not implemented
#endif
}

inline SInt32
HeAtomicCompareExchange32( SInt32 volatile* p, SInt32 v, SInt32 c )
{
#if defined(HE_WIN32)
#ifdef HE_OPT_ATOMICINTERLOCKED
	return _InterlockedCompareExchange( p, v, c );
#else
	return InterlockedCompareExchange( p, v, c );
#endif
#elif defined(__GNUC__)
	SInt32	r = *p;
	SInt32	e = c;
	__atomic_compare_exchange_n( p, &e, v, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
	return r;
#else
#pragma warning HeAtomicCompareExchange32 not implemented
#endif
}

inline void*
HeAtomicExchangePointer( void* volatile* pp, void* v )
{
#if defined(HE_WIN32)
#ifdef _M_IX86
#ifdef HE_OPT_ATOMICINTERLOCKED
	// InterlockedExchangePointer defined in the system headers has issues.
	return reinterpret_cast<void*>(
		static_cast<LONG_PTR>(
			_InterlockedExchange(
				reinterpret_cast<LONG volatile*>(pp),
				static_cast<LONG>(reinterpret_cast<LONG_PTR>(v)))));
#else
	// InterlockedExchangePointer defined in the system headers has issues.
	return reinterpret_cast<void*>(
		static_cast<LONG_PTR>(
			InterlockedExchange(
				reinterpret_cast<LONG volatile*>(pp),
				static_cast<LONG>(reinterpret_cast<LONG_PTR>(v)))));
#endif
#else
	return InterlockedExchangePointer( pp, v );
#endif
#elif defined(__GNUC__)
	return __atomic_exchange_n( pp, v, __ATOMIC_SEQ_CST );
#else
#pragma warning HeAtomicExchangePointer not implemented
#endif
}

} /* namespace HeLib */

#endif /* HLATOMIC_H */
