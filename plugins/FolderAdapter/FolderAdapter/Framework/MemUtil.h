// =============================================================================
//	MemUtil.h
// =============================================================================
//	Copyright (c) 2000-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"

#ifdef WP_LINUX

#include <endian.h>		// For __BYTE_ORDER
#include <byteswap.h>	// For bswap_16, bswap_32 and bswap_64

#else	// WP_LINUX

// Set up __BYTE_ORDER
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN	1234
#endif
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN	4321
#endif
#ifndef __PDP_ENDIAN
#define __PDP_ENDIAN	3412
#endif
#define __BYTE_ORDER	__LITTLE_ENDIAN

// Set up bswap_16, bswap_32 and bswap_64
#include <stdlib.h>
#ifndef bswap_16
#define bswap_16(x)		_byteswap_ushort(x)
#endif
#ifndef bswap_32
#define bswap_32(x)		_byteswap_ulong(x)
#endif
#ifndef bswap_64
#define bswap_64(x)		_byteswap_uint64(x)
#endif

#pragma intrinsic(_byteswap_ushort,_byteswap_ulong,_byteswap_uint64)

#endif // WP_LINUX

#if (0)
UInt16 _byteswap_ushort( UInt16 x ) {
	return ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8);
}

UInt32 _byteswap_ulong( UInt32 x ) {
	return (((x & 0x000000FF) << 24) |
	        ((x & 0x0000FF00) <<  8) |
		    ((x & 0x00FF0000) >>  8) |
		    ((x & 0xFF000000) >> 24));
}
#endif


namespace MemUtil
{

inline UInt16 Swap16( UInt16 x )
{
	return bswap_16( x );
}

inline SInt16 Swap16( SInt16 x )
{
	return bswap_16( x );
}

inline UInt32 Swap32( UInt32 x )
{
	return bswap_32( x );
}

inline SInt32 Swap32( SInt32 x )
{
	return bswap_32( x );
}

inline UInt64 Swap64( UInt64 x )
{
	return bswap_64( x );
}

inline SInt64 Swap64( SInt64 x )
{
	return bswap_64( x );
}

inline float SwapFloat( float x )
{
	char*	pData     = (char*) &x;
	float	returnVar = 0;
	char*	pCharData = (char*) &returnVar;
	for ( int i = sizeof(returnVar) - 1, j = 0; i >= 0; i--, j++ )
	{
		pCharData[j] = pData[i];
	}
	return returnVar;
}

inline double SwapDouble( double x )
{
	char*	pData     = (char*) &x;
	double	returnVar = 0;
	char*	pCharData = (char*) &returnVar;
	for ( int i = sizeof(returnVar) - 1, j = 0; i >= 0; i--, j++ )
	{
		pCharData[j] = pData[i];
	}
	return returnVar;
}

inline UInt8 Reverse8( UInt8 x )
{
#ifdef WP_LINUX
	#define _byteswap_ushort(x)	__builtin_bswap16( x )
    #define _byteswap_ulong(x)	__builtin_bswap32( x )
#endif

	UInt8	r = 0;
#define S(n)	if ( x & (1 << n ) ) { r |= 0x80 >> n; }
	S(0); S(1); S(2); S(3);
	S(4); S(5); S(6); S(7);
#undef S

	return r;
}

inline UInt16 GetShort( const UInt8* pData, int bo = __BYTE_ORDER )
{
	if ( bo == __BIG_ENDIAN ) {
		return ((pData[0] << 8) | pData[1]);
	}
	else {
		return ((pData[1] << 8) | pData[0]);
	}
}

inline UInt32 GetLong( const UInt8* pData, int bo = __BYTE_ORDER )
{
	if ( bo == __BIG_ENDIAN ) {
		return ((pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | pData[3]);
	}
	else {
		return ((pData[3] << 24) | (pData[2] << 16) | (pData[1] << 8) | pData[0]);
	}
}

inline UInt64 GetLongLong( const UInt8* pData, int bo = __BYTE_ORDER )
{
	if ( bo == __BIG_ENDIAN ) {
		UInt64	t;
		UInt64	v = 0;
		t = pData[0]; v |= (t << 56);
		t = pData[1]; v |= (t << 48);
		t = pData[2]; v |= (t << 40);
		t = pData[3]; v |= (t << 32);
		t = pData[4]; v |= (t << 24);
		t = pData[5]; v |= (t << 16);
		t = pData[6]; v |= (t << 8);
		t = pData[7]; v |= t;
		return v;
	}
	else {
		UInt64	t;
		UInt64	v = 0;
		t = pData[7]; v |= (t << 56);
		t = pData[6]; v |= (t << 48);
		t = pData[5]; v |= (t << 40);
		t = pData[4]; v |= (t << 32);
		t = pData[3]; v |= (t << 24);
		t = pData[2]; v |= (t << 16);
		t = pData[1]; v |= (t << 8);
		t = pData[0]; v |= t;
		return v;
	}
}

} /* namespace MemUtil */

#if __BYTE_ORDER == __BIG_ENDIAN
#define BIGTOHOST16(x)			(x)
#define HOSTTOBIG16(x)			(x)
#define BIGTOHOST32(x)			(x)
#define HOSTTOBIG32(x)			(x)
#define BIGTOHOST64(x)			(x)
#define HOSTTOBIG64(x)			(x)
#define BIGTOHOSTFLOAT(x)		(x)
#define HOSTTOBIGFLOAT(x)		(x)
#define BIGTOHOSTDOUBLE(x)		(x)
#define HOSTTOBIGDOUBLE(x)		(x)
#define LITTLETOHOST16(x)		(MemUtil::Swap16(x))
#define HOSTTOLITTLE16(x)		(MemUtil::Swap16(x))
#define LITTLETOHOST32(x)		(MemUtil::Swap32(x))
#define HOSTTOLITTLE32(x)		(MemUtil::Swap32(x))
#define LITTLETOHOST64(x)		(MemUtil::Swap64(x))
#define HOSTTOLITTLE64(x)		(MemUtil::Swap64(x))
#define LITTLETOHOSTFLOAT(x)	(MemUtil::SwapFloat(x))
#define HOSTTOLITTLEFLOAT(x)	(MemUtil::SwapFloat(x))
#define LITTLETOHOSTDOUBLE(x)	(MemUtil::SwapDouble(x))
#define HOSTTOLITTLEDOUBLE(x)	(MemUtil::SwapDouble(x))
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define BIGTOHOST16(x)			(MemUtil::Swap16(x))
#define HOSTTOBIG16(x)			(MemUtil::Swap16(x))
#define BIGTOHOST32(x)			(MemUtil::Swap32(x))
#define HOSTTOBIG32(x)			(MemUtil::Swap32(x))
#define BIGTOHOST64(x)			(MemUtil::Swap64(x))
#define HOSTTOBIG64(x)			(MemUtil::Swap64(x))
#define BIGTOHOSTFLOAT(x)		(MemUtil::SwapFloat(x))
#define HOSTTOBIGFLOAT(x)		(MemUtil::SwapFloat(x))
#define BIGTOHOSTDOUBLE(x)		(MemUtil::SwapDouble(x))
#define HOSTTOBIGDOUBLE(x)		(MemUtil::SwapDouble(x))
#define LITTLETOHOST16(x)		(x)
#define HOSTTOLITTLE16(x)		(x)
#define LITTLETOHOST32(x)		(x)
#define HOSTTOLITTLE32(x)		(x)
#define LITTLETOHOST64(x)		(x)
#define HOSTTOLITTLE64(x)		(x)
#define LITTLETOHOSTFLOAT(x)	(x)
#define HOSTTOLITTLEFLOAT(x)	(x)
#define LITTLETOHOSTDOUBLE(x)	(x)
#define HOSTTOLITTLEDOUBLE(x)	(x)
#endif

#define NETWORKTOHOST16(x)	BIGTOHOST16(x)
#define HOSTTONETWORK16(x)	HOSTTOBIG16(x)
#define NETWORKTOHOST32(x)	BIGTOHOST32(x)
#define HOSTTONETWORK32(x)	HOSTTOBIG32(x)
#define NETWORKTOHOST64(x)	BIGTOHOST64(x)
#define HOSTTONETWORK64(x)	HOSTTOBIG64(x)

// Conversions for bytes to/from KB/MB/GB.
#define BYTES_PER_KILOBYTE	1024
#define BYTES_PER_MEGABYTE	1048576			// 1024 * 1024
#define BYTES_PER_GIGABYTE	1073741824		// 1024 * 1024 * 1024

// Inline versions to make it possible to call byte swapping
// functions from a template.
inline UInt8	NetworkToHost( const UInt8  inN ){	return inN;	}
inline SInt8	NetworkToHost( const SInt8  inN ){	return inN;	}
inline UInt16	NetworkToHost( const UInt16 inN ){	return NETWORKTOHOST16( inN );	}
inline SInt16	NetworkToHost( const SInt16 inN ){	return NETWORKTOHOST16( inN );	}
inline UInt32	NetworkToHost( const UInt32 inN ){	return NETWORKTOHOST32( inN );	}
inline SInt32	NetworkToHost( const SInt32 inN ){	return NETWORKTOHOST32( inN );	}

inline UInt8	HostToNetwork( const UInt8  inN ){	return inN;	}
inline SInt8	HostToNetwork( const SInt8  inN ){	return inN;	}
inline UInt16	HostToNetwork( const UInt16 inN ){	return HOSTTONETWORK16( inN );	}
inline SInt16	HostToNetwork( const SInt16 inN ){	return HOSTTONETWORK16( inN );	}
inline UInt32	HostToNetwork( const UInt32 inN ){	return HOSTTONETWORK32( inN );	}
inline SInt32	HostToNetwork( const SInt32 inN ){	return HOSTTONETWORK32( inN );	}

inline UInt8	LittleToHost( const UInt8  inN ){	return inN;	}
inline SInt8	LittleToHost( const SInt8  inN ){	return inN;	}
inline UInt16	LittleToHost( const UInt16 inN ){	return LITTLETOHOST16( inN );	}
inline SInt16	LittleToHost( const SInt16 inN ){	return LITTLETOHOST16( inN );	}
inline UInt32	LittleToHost( const UInt32 inN ){	return LITTLETOHOST32( inN );	}
inline SInt32	LittleToHost( const SInt32 inN ){	return LITTLETOHOST32( inN );	}

inline UInt8	HostToLittle( const UInt8  inN ){	return inN;	}
inline SInt8	HostToLittle( const SInt8  inN ){	return inN;	}
inline UInt16	HostToLittle( const UInt16 inN ){	return HOSTTOLITTLE16( inN );	}
inline SInt16	HostToLittle( const SInt16 inN ){	return HOSTTOLITTLE16( inN );	}
inline UInt32	HostToLittle( const UInt32 inN ){	return HOSTTOLITTLE32( inN );	}
inline SInt32	HostToLittle( const SInt32 inN ){	return HOSTTOLITTLE32( inN );	}

inline UInt8	BigToHost( const UInt8  inN ){	return inN;	}
inline SInt8	BigToHost( const SInt8  inN ){	return inN;	}
inline UInt16	BigToHost( const UInt16 inN ){	return BIGTOHOST16( inN );	}
inline SInt16	BigToHost( const SInt16 inN ){	return BIGTOHOST16( inN );	}
inline UInt32	BigToHost( const UInt32 inN ){	return BIGTOHOST32( inN );	}
inline SInt32	BigToHost( const SInt32 inN ){	return BIGTOHOST32( inN );	}

inline UInt8	HostToBig( const UInt8  inN ){	return inN;	}
inline SInt8	HostToBig( const SInt8  inN ){	return inN;	}
inline UInt16	HostToBig( const UInt16 inN ){	return HOSTTOBIG16( inN );	}
inline SInt16	HostToBig( const SInt16 inN ){	return HOSTTOBIG16( inN );	}
inline UInt32	HostToBig( const UInt32 inN ){	return HOSTTOBIG32( inN );	}
inline SInt32	HostToBig( const SInt32 inN ){	return HOSTTOBIG32( inN );	}
