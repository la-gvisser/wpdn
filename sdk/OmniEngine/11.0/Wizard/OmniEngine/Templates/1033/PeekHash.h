// ============================================================================
//	PeekHash.h
//		interface to IP Header classes
// ============================================================================
//	Copyright (c) 2011-2017 Savvius, Inc. All rights reserved.

#pragma once


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Hash
//
//	A Peek Hash is a template used to add hashing to various data structures.
//	The template allows for multiple hashing algorithms, but the Framework
//	currently uses the default.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// ============================================================================
//		Constants and Data Structures
// ============================================================================

enum {
	kPeekHash_Default,
	kPeekHash_XOR,
	kPeekHash_FNV,
	kPeekHash_SoS,
	kPeekHash_Max
};


// ============================================================================
//		TPeekHash
// ============================================================================

template <class _Kyt, class _Rt>
class TPeekHash
{
public:
	static const UInt32	s_nOffset32 = 2166136261u;
	static const UInt64	s_nOffset64 = 14695981039346656037ull;
	static const UInt32	s_nPrime32 = 16777619u;
	static const UInt64	s_nPrime64 = 1099511628211ull;

public:
	static _Rt	hashXOR( const _Kyt& inKey, size_t inSize ) {
		_Rt				nHash = 0;
		const UInt8*	pFirst = reinterpret_cast<const UInt8*>( &inKey );
		const UInt8*	pLast = pFirst + inSize;

		for ( ; pFirst <= pLast; pFirst += sizeof( _Rt ) ) {
			nHash ^= *reinterpret_cast<const _Rt*>( pFirst );
		}
		if ( pFirst > pLast ) {
			pFirst -= sizeof( _Rt );
			_Rt	_n = 0;
			memcpy( &_n, pFirst, (pLast - pFirst) );
			nHash ^= _n;
		}
		return nHash;
	}

	static _Rt	hashFNV( const _Kyt& inKey, size_t inSize ) {
		const _Rt		nOffset = static_cast<_Rt>( (sizeof( _Rt ) <= 32) ? s_nOffset32 : s_nOffset64 );
		const _Rt		nPrime = static_cast<_Rt>( (sizeof( _Rt ) <= 32) ? s_nPrime32 : s_nPrime64 );
		_Rt				nHash = nOffset;
		const UInt8*	pFirst = reinterpret_cast<const UInt8*>( &inKey );
		const UInt8*	pLast = pFirst + inSize;

		for ( ; pFirst <= pLast; pFirst += sizeof( _Rt ) ) {
			nHash = (nPrime * nHash) ^ *reinterpret_cast<const _Rt*>( pFirst );
		}
		if ( pFirst > pLast ) {
			pFirst -= sizeof( _Rt );
			_Rt	_n = 0;
			memcpy( &_n, pFirst, (pLast - pFirst) );
			nHash = (nPrime * nHash) ^ _n;
		}
		return nHash;
	}

	static _Rt	hashSoS( const _Kyt& inKey, size_t inSize ) {
		_Rt				nHash = 0;
		const UInt8*	pFirst = reinterpret_cast<const UInt8*>( &inKey );
		const UInt8*	pLast = pFirst + inSize;

		for ( ; pFirst <= pLast; pFirst++ ) {
			nHash = (nHash * 2) + *pFirst;
		}
		return nHash;
	}

public:
	static _Rt	hash( int inAlg, const _Kyt& inKey, size_t inSize ) {
		switch( inAlg ) {
			case kPeekHash_XOR:
				return hashXOR( inKey, inSize );
			case kPeekHash_FNV:
				return hashFNV( inKey, inSize );
			case kPeekHash_SoS:
				return hashSoS( inKey, inSize );
		}
		return 0;
	}
};


#define CALLTYPEHASHXOR(t,f,r)	\
	TPeekHash<t,r>::hashXOR( *reinterpret_cast<const t*>( &(f) ), sizeof( t ) )

#define CALLTYPESIZEHASHXOR(t,s,f,r)	\
	TPeekHash<t,r>::hashXOR( *reinterpret_cast<const t*>( &(f) ), (s) )

#define CALLTYPEHASHFNV(t,f,r)	\
	TPeekHash<t,r>::hashFNV( *reinterpret_cast<const t*>( &(f) ), sizeof( t ) )

#define CALLTYPESIZEHASHFNV(t,s,f,r)	\
	TPeekHash<t,r>::hashFNV( *reinterpret_cast<const t*>( &(f) ), (s) )

#define CALLTYPEHASHSOS(t,f,r)	\
	TPeekHash<t,r>::hashSoS( *reinterpret_cast<const t*>( &(f) ), sizeof( t ) )

#define CALLTYPESIZEHASHSOS(t,s,f,r)	\
	TPeekHash<t,r>::hashSos( *reinterpret_cast<const t*>( &(f) ), (s) )


#define CALLTYPEHASH(a,t,f,r)	\
	TPeekHash<t,r>::hash( (a), *reinterpret_cast<const t*>( &(f) ), sizeof( t ) )

#define CALLTYPESIZEHASH(a,t,s,f,r)	\
	TPeekHash<t,r>::hash( (a), *reinterpret_cast<const t*>( &(f) ), (s) )



// ============================================================================
//		IPeekHash
// ============================================================================

class IPeekHash
{
public:
	virtual ~IPeekHash() {}
	virtual size_t	Hash() const = 0;
};
