// =============================================================================
//	encodehex.h
// =============================================================================
//	Copyright (c) 2003-2011 WildPackets, Inc. All rights reserved.

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

inline void
HexEncode(
	const UInt8*		pData,
	size_t				nDataLen,
	HeLib::CHeVariant&	varVal )
{
	std::string	strData;

	if ( (pData != NULL) && (nDataLen > 0) )
	{
		size_t	nDataStrLen = HexEncodeGetRequiredLength( nDataLen );
		strData.resize( nDataStrLen );
		char*	pszData = &strData[0];
		if ( pszData == NULL ) HeThrow( HE_E_OUT_OF_MEMORY );
		if ( !HexDataEncode( pData, nDataLen, pszData, &nDataStrLen ) )
		{
			HeThrow( HE_E_FAIL );
		}
		pszData[nDataStrLen] = 0;	// Null-terminate the string.
	}

	varVal = strData.c_str();
}


inline void
HexDecode(
	UInt8*						pData,
	size_t&						nDataLen,
	const HeLib::CHeVariant&	varVal )
{
	// Sanity checks.
	if ( pData == NULL ) HeThrow( HE_E_NULL_POINTER );
	if ( nDataLen == 0 ) HeThrow( HE_E_INVALID_ARG );
	if ( varVal.vt != HE_VT_BSTR ) HeThrow( HE_E_INVALID_ARG );

	if ( varVal.bstrVal == NULL )
	{
		nDataLen = 0;
		return;
	}

	HeLib::CW2A	strData( varVal.bstrVal );
	const char*	pSrcData = strData;
	size_t		nSrcLen = strlen( pSrcData );
	size_t		nDestLen = HexDecodeGetRequiredLength( nSrcLen );

	if ( nDataLen < nDestLen )
	{
		HeThrow( HE_E_INSUFFICIENT_BUFFER );
	}

	if ( !HexDataDecode( pSrcData, nSrcLen, pData, &nDestLen ) )
	{
		HeThrow( HE_E_FAIL );
	}

	nDataLen = nDestLen;
}

}	/* namespace EncodeHex */

#endif /* ENCODEHEX_H */
