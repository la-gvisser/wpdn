// =============================================================================
//	encodehex.h
// =============================================================================
//	Copyright (c) 2018-2024 LiveAction, Inc. All rights reserved.
//	Copyright (c) 2015-2018 Savvius, Inc. All rights reserved.
//	Copyright (c) 2003-2015 WildPackets, Inc. All rights reserved.

#ifndef ENCODEHEX_H
#define ENCODEHEX_H

#include "hlvariant.h"
#include <string>

namespace EncodeHex
{

inline size_t
HexEncodeGetRequiredLength(
	size_t	nSrcLen ) throw()
{
	return (2 * nSrcLen) + 1;
}

static inline size_t
HexDecodeGetRequiredLength(
	size_t	nSrcLen ) throw()
{
	return (nSrcLen / 2);
}

inline bool
HexDataEncode(
	const UInt8*	pbSrcData,
	size_t			nSrcLen,
	char*			szDest,
	size_t*			pnDestLen ) throw()
{
	static const char s_chHexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
										  'A', 'B', 'C', 'D', 'E', 'F'};

	if ( !pbSrcData || !szDest || !pnDestLen )
	{
		return false;
	}

	if ( *pnDestLen < HexEncodeGetRequiredLength( nSrcLen ) )
	{
		return false;
	}

	size_t	nRead = 0;
	size_t	nWritten = 0;
	UInt8	ch;
	while ( nRead < nSrcLen )
	{
		ch = *pbSrcData++;
		nRead++;
		*szDest++ = s_chHexChars[(ch >> 4) & 0x0F];
		*szDest++ = s_chHexChars[ch & 0x0F];
		nWritten += 2;
	}

	*pnDestLen = nWritten;

	return true;
}

inline char
GetHexValue( char ch ) throw()
{
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (ch - 'a' + 10);
	return -1;
}

inline bool
HexDataDecode(
	const char*	pSrcData,
	size_t		nSrcLen,
	UInt8*		pbDest,
	size_t*		pnDestLen ) throw()
{
	if ( !pSrcData || !pbDest || !pnDestLen )
	{
		return false;
	}

	if ( *pnDestLen < HexDecodeGetRequiredLength( nSrcLen ) )
	{
		return false;
	}

	size_t	nRead = 0;
	size_t	nWritten = 0;
	while ( nRead < nSrcLen )
	{
		char	ch1 = GetHexValue( (char) *pSrcData++ );
		char	ch2 = GetHexValue( (char) *pSrcData++ );
		if ( (ch1 < 0) || (ch2 < 0) )
		{
			return false;
		}
		*pbDest++ = (UInt8)((16 * ch1) + ch2);
		nWritten++;
		nRead += 2;
	}

	*pnDestLen = nWritten;

	return true;
}

inline std::string
HexEncode(
	const UInt8*		pData,
	size_t				nDataLen )
{
	std::string	strData;

	if ( (pData != nullptr) && (nDataLen > 0) )
	{
		// Get the string length including terminator.
		size_t	nDataStrLen = HexEncodeGetRequiredLength( nDataLen );
		strData.resize( nDataStrLen - 1 );	// Ignore terminator.
		if ( !HexDataEncode( pData, nDataLen, &strData[0], &nDataStrLen ) )
		{
			HeLib::HeThrow( HE_E_FAIL );
		}
	}

	return strData;
}

inline void
HexEncode(
	const UInt8*		pData,
	size_t				nDataLen,
	HeLib::CHeVariant&	varVal )
{
	varVal = HexEncode( pData, nDataLen );
}

inline void
HexDecode(
	UInt8*				pData,
	size_t&				nDataLen,
	const char*			pSrcData )
{
	// Sanity checks.
	if ( pData == nullptr ) HeLib::HeThrow( HE_E_NULL_POINTER );
	if ( nDataLen == 0 ) HeLib::HeThrow( HE_E_INVALID_ARG );

	if ( pSrcData == nullptr )
	{
		nDataLen = 0;
		return;
	}

	const size_t	nSrcLen  = strlen( pSrcData );
	size_t			nDestLen = HexDecodeGetRequiredLength( nSrcLen );

	if ( nDataLen < nDestLen )
	{
		HeLib::HeThrow( HE_E_INSUFFICIENT_BUFFER );
	}

	if ( !HexDataDecode( pSrcData, nSrcLen, pData, &nDestLen ) )
	{
		HeLib::HeThrow( HE_E_FAIL );
	}

	nDataLen = nDestLen;
}

inline void
HexDecode(
	UInt8*						pData,
	size_t&						nDataLen,
	const HeLib::CHeVariant&	varVal )
{
	// Sanity checks.
	if ( varVal.vt != Helium::HE_VT_BSTR ) HeLib::HeThrow( HE_E_INVALID_ARG );

	if ( varVal.bstrVal == nullptr )
	{
		nDataLen = 0;
		return;
	}

	HexDecode( pData, nDataLen, HeLib::CW2A( varVal.bstrVal ) );
}

}	/* namespace EncodeHex */

#endif /* ENCODEHEX_H */
