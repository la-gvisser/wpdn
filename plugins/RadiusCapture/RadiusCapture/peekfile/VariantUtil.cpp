// ============================================================================
//	VariantUtil.cpp
// ============================================================================
//	Copyright (c) 2003-2006 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include <errno.h>
#include <atlenc.h>
#include "VariantUtil.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// ----------------------------------------------------------------------------
//		private namespace decls
// ----------------------------------------------------------------------------
namespace VariantUtil
{
	HRESULT ConvertToI8( VARIANTARG* pvargDest, VARIANTARG* pvarSrc, VARTYPE vt );
	HRESULT ConvertToBSTR( VARIANTARG* pvargDest, VARIANTARG* pvarSrc );
	HRESULT ConvertToI4( VARIANTARG* pvargDest, VARIANTARG* pvarSrc, VARTYPE vt );
	HRESULT ConvertToI1( VARIANTARG* pvargDest, VARIANTARG* pvarSrc, VARTYPE vt );
}

// ----------------------------------------------------------------------------
//		IsVT8Supported
// ----------------------------------------------------------------------------

static bool
IsVT8Supported()
{
	static int	s_Supported = -1;

	if ( s_Supported < 0 )
	{
		s_Supported = 0;

		VARIANT	varTest;
		::VariantInit( &varTest );
		V_VT(&varTest)  = VT_UI8;
		V_UI8(&varTest) = 72803UL;
		HRESULT	hr = ::VariantChangeType( &varTest, &varTest, 0, VT_BSTR );
		s_Supported = SUCCEEDED(hr) ? 1 : 0;
		::VariantClear( &varTest );
		ATLTRACE( _T("VT_I8/VT_UI8 supported by VariantChangeType = %u\n"), s_Supported );
	}

	return (s_Supported > 0);
}


// ----------------------------------------------------------------------------
//		Clear
// ----------------------------------------------------------------------------

HRESULT
VariantUtil::Clear(
	VARIANTARG*	pvarg )
{
	HRESULT		hr = S_OK;

	if ( IsVT8Supported() )
	{
		hr = ::VariantClear( pvarg );
	}
	else
	{
		// Sanity checks.
		if ( pvarg == NULL ) return E_INVALIDARG;

		if ( (pvarg->vt == VT_I8) || (pvarg->vt == VT_UI8) )
		{
			V_VT(pvarg) = VT_EMPTY;
		}
		else
		{
			hr = ::VariantClear( pvarg );
		}
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		Copy
// ----------------------------------------------------------------------------

HRESULT
VariantUtil::Copy(
	VARIANTARG*	pvargDest,
	VARIANTARG*	pvargSrc )
{
	HRESULT		hr = S_OK;

	if ( IsVT8Supported() )
	{
		hr = ::VariantCopy( pvargDest, pvargSrc );
	}
	else
	{
		// Sanity checks.
		if ( pvargDest == NULL ) return E_INVALIDARG;
		if ( pvargSrc == NULL ) return E_INVALIDARG;

		if ( pvargSrc->vt == VT_I8 )
		{
			hr = VariantUtil::Clear( pvargDest );
			if ( SUCCEEDED(hr) )
			{
				V_VT(pvargDest) = VT_I8;
				V_I8(pvargDest) = V_I8(pvargSrc);
				hr = S_OK;
			}
		}
		else if ( pvargSrc->vt == VT_UI8 )
		{
			hr = VariantUtil::Clear( pvargDest );
			if ( SUCCEEDED(hr) )
			{
				V_VT(pvargDest) = VT_UI8;
				V_UI8(pvargDest) = V_UI8(pvargSrc);
				hr = S_OK;
			}
		}
		else
		{
			hr = ::VariantCopy( pvargDest, pvargSrc );
		}
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		CopyInd
// ----------------------------------------------------------------------------

HRESULT
VariantUtil::CopyInd(
	VARIANT*	pvarDest,
	VARIANTARG*	pvargSrc )
{
	HRESULT		hr = S_OK;

	if ( IsVT8Supported() )
	{
		hr = ::VariantCopyInd( pvarDest, pvargSrc );
	}
	else
	{
		// Sanity checks.
		if ( pvarDest == NULL ) return E_INVALIDARG;
		if ( pvargSrc == NULL ) return E_INVALIDARG;

		if ( (pvargSrc->vt == VT_I8) || (pvargSrc->vt == VT_UI8) )
		{
			hr = VariantUtil::Copy( pvarDest, pvargSrc );
		}
		else if ( pvargSrc->vt == (VT_I8 | VT_BYREF) )
		{
			// TODO: handle VT_BYREF (after all, that's the whole point!)
		}
		else if ( pvargSrc->vt == (VT_UI8 | VT_BYREF) )
		{
			// TODO: handle VT_BYREF (after all, that's the whole point!)
		}
		else
		{
			hr = ::VariantCopyInd( pvarDest, pvargSrc );
		}
	}

	return hr;
}


// -----------------------------------------------------------------------------
//		ConvertToI8
// -----------------------------------------------------------------------------

HRESULT 
VariantUtil::ConvertToI8( 
						 VARIANTARG*	pvargDest,
						 VARIANTARG*	pvarSrc,
						 VARTYPE		vt )
{
	HRESULT hr = E_FAIL;
	
	// Sanity checks.
	if ( pvargDest == NULL ) return E_INVALIDARG;
	if ( pvarSrc == NULL ) return E_INVALIDARG;

	if ( pvarSrc->vt == VT_I8 )
	{
		if ( vt == VT_I8 )
		{
			hr = VariantUtil::Clear( pvargDest );
			if ( FAILED(hr) ) return hr;
			V_VT(pvargDest) = VT_I8;
			V_I8(pvargDest) = V_I8(pvarSrc);
			hr = S_OK;
		}
		else	// Must be vt == VT_UI8.
		{
			if ( V_I8(pvarSrc) < 0 ) return DISP_E_OVERFLOW;
			hr = VariantUtil::Clear( pvargDest );
			if ( FAILED(hr) ) return hr;
			V_VT(pvargDest) = VT_UI8;
			V_UI8(pvargDest) = (ULONGLONG) V_I8(pvarSrc);
			hr = S_OK;
		}
	}
	else if ( pvarSrc->vt == VT_UI8 )
	{
		if ( vt == VT_UI8 )
		{
			hr = VariantUtil::Clear( pvargDest );
			if ( FAILED(hr) ) return hr;
			V_VT(pvargDest) = VT_UI8;
			V_UI8(pvargDest) = V_UI8(pvarSrc);
			hr = S_OK;
		}
		else	// Must be vt == VT_I8.
		{
			if ( V_UI8(pvarSrc) > MAXLONGLONG ) return DISP_E_OVERFLOW;
			hr = VariantUtil::Clear( pvargDest );
			if ( FAILED(hr) ) return hr;
			V_VT(pvargDest) = VT_I8;
			V_I8(pvargDest) = (LONGLONG) V_UI8(pvarSrc);
			hr = S_OK;
		}
	}
	else if ( pvarSrc->vt == VT_BSTR )
	{
		if ( vt == VT_I8 )
		{
			wchar_t*	pSrc = V_BSTR(pvarSrc);
			wchar_t*	pEnd = NULL;
			errno = 0;
			LONGLONG	ll = _wcstoi64( pSrc, &pEnd, 10 );
			if ( (pEnd == pSrc) || (errno == ERANGE) )
			{
				errno = 0;
				hr = E_INVALIDARG;
			}
			else
			{
				hr = VariantUtil::Clear( pvargDest );
				if ( FAILED(hr) ) return hr;
				V_VT(pvargDest) = VT_I8;
				V_I8(pvargDest) = ll;
				hr = S_OK;
			}
		}
		else	// Must be vt == VT_UI8.
		{
			wchar_t*	pSrc = V_BSTR(pvarSrc);
			wchar_t*	pEnd = NULL;
			errno = 0;
			ULONGLONG	ull = _wcstoui64( pSrc, &pEnd, 10 );
			if ( (pEnd == pSrc) || (errno == ERANGE) )
			{
				errno = 0;
				return E_INVALIDARG;
			}
			else
			{
				hr = VariantUtil::Clear( pvargDest );
				if ( FAILED(hr) ) return hr;
				V_VT(pvargDest) = VT_UI8;
				V_UI8(pvargDest) = ull;
				hr = S_OK;
			}
		}
	}
	else
	{
		// Handle all simple integer promotions (where size is platform-independent).
		bool bIsSrcInteger = true;
		LONGLONG llSrc = 0;
		switch ( pvarSrc->vt )
		{
		case VT_I1:
			llSrc = V_I1(pvarSrc);
			break;
		case VT_I2:
			llSrc = V_I2(pvarSrc);
			break;
		case VT_I4:
			llSrc = V_I4(pvarSrc);
			break;
		case VT_UI1:
			llSrc = V_UI1(pvarSrc);
			break;
		case VT_UI2:
			llSrc = V_UI2(pvarSrc);
			break;
		case VT_UI4:
			llSrc = V_UI4(pvarSrc);
			break;
		default:
			bIsSrcInteger = false;
			break;
		}
		if ( bIsSrcInteger )
		{
			if ( vt == VT_UI8 )
			{
				if ( llSrc < 0 ) return DISP_E_OVERFLOW;
				hr = VariantUtil::Clear( pvargDest );
				if ( FAILED(hr) ) return hr;
				V_VT(pvargDest) = VT_UI8;
				V_UI8(pvargDest) = llSrc;
				hr = S_OK;
			}
			else	// Must be vt == VT_I8.
			{
				hr = VariantUtil::Clear( pvargDest );
				if ( FAILED(hr) ) return hr;
				V_VT(pvargDest) = VT_I8;
				V_I8(pvargDest) = llSrc;
				hr = S_OK;
			}
		}
	}

	return hr;
}

// -----------------------------------------------------------------------------
//		ConvertToBSTR
// -----------------------------------------------------------------------------

HRESULT 
VariantUtil::ConvertToBSTR( 
						   VARIANTARG*	pvargDest,
						   VARIANTARG*	pvarSrc )
{
	HRESULT hr = E_FAIL;

	// Sanity checks.
	if ( pvargDest == NULL ) return E_INVALIDARG;
	if ( pvarSrc == NULL ) return E_INVALIDARG;

	if ( pvarSrc->vt == VT_I8 )
	{
		wchar_t	str[256];
		str[0] = 0;
		_i64tow_s( V_I8(pvarSrc), str, 256, 10 );
		BSTR	bstr = ::SysAllocString( str );
		if ( bstr == NULL ) return E_OUTOFMEMORY;
		hr = VariantUtil::Clear( pvargDest );
		if ( FAILED(hr) ) return hr;
		V_VT(pvargDest) = VT_BSTR;
		V_BSTR(pvargDest) = bstr;
		hr = S_OK;
	}
	else if ( pvarSrc->vt == VT_UI8 )
	{
		wchar_t	str[256];
		str[0] = 0;
		_ui64tow_s( V_UI8(pvarSrc), str, 256, 10 );
		BSTR	bstr = ::SysAllocString( str );
		if ( bstr == NULL ) return E_OUTOFMEMORY;
		hr = VariantUtil::Clear( pvargDest );
		if ( FAILED(hr) )
		{
			::SysFreeString( bstr );
			return hr;
		}
		V_VT(pvargDest) = VT_BSTR;
		V_BSTR(pvargDest) = bstr;
		hr = S_OK;
	}

	return hr;
}

// -----------------------------------------------------------------------------
//		ConvertToI4
// -----------------------------------------------------------------------------

HRESULT 
VariantUtil::ConvertToI4( 
						 VARIANTARG*	pvargDest,
						 VARIANTARG*	pvarSrc,
						 VARTYPE		vt )
{
	HRESULT hr = E_FAIL;

	// sanity checks
	if ( pvargDest == NULL ) return E_INVALIDARG;
	if ( pvarSrc == NULL ) return E_INVALIDARG;

	// if we're converting from UI8 or I8, do it manually
	// otherwise, use the native ::VariantChangeType
	if ( pvarSrc->vt == VT_UI8 )
	{
		// cache
		ULONGLONG ull = V_UI8(pvarSrc);
		// clear
		::VariantClear(pvargDest);
		// set
		V_VT(pvargDest) = vt;
		if ( vt == VT_UI4 )
		{
			V_UI4(pvargDest) = (ULONG) ull;
		}
		else
		{
			V_I4(pvargDest) = (LONG) ull;
		}
		hr = S_OK;
	}
	else if (pvarSrc->vt == VT_I8)
	{
		// cache
		LONGLONG ll = V_I8(pvarSrc);
		// clear
		::VariantClear(pvargDest);
		// set
		V_VT(pvargDest) = vt;
		if ( vt == VT_UI4 )
		{
			V_UI4(pvargDest) = (ULONG) ll;
		}
		else
		{
			V_I4(pvargDest) = (LONG) ll;
		}
		hr = S_OK;
	}

	return hr;
}

// -----------------------------------------------------------------------------
//		ConvertToI1
// -----------------------------------------------------------------------------

HRESULT 
VariantUtil::ConvertToI1( 
						 VARIANTARG*	pvargDest,
						 VARIANTARG*	pvarSrc,
						 VARTYPE		vt )
{
	HRESULT hr = E_FAIL;

	// sanity checks
	if ( pvargDest == NULL ) return E_INVALIDARG;
	if ( pvarSrc == NULL ) return E_INVALIDARG;

	// I hate W2K.  This function only sort of works.  Basically it does string to type nothing.
	ASSERT( pvarSrc->vt == VT_BSTR );
	if (pvarSrc->vt != VT_BSTR)
		return hr;

	// convert
	CString str = (CString) V_BSTR(pvarSrc);
	// clear
	::VariantClear(pvargDest);
	// set
	V_VT(pvargDest) = vt;
	if ( vt == VT_UI1 )
	{
		V_UI1(pvargDest) = (BYTE) _tcstoul(str, NULL, 10);
	}
	else
	{
		V_I1(pvargDest) = (CHAR) _ttol(str);
	}

	hr = S_OK;

	return hr;
}

// ----------------------------------------------------------------------------
//		ChangeType
// ----------------------------------------------------------------------------

HRESULT
VariantUtil::ChangeType(
	VARIANTARG*	pvargDest,
	VARIANTARG*	pvarSrc,
	USHORT		wFlags,
	VARTYPE		vt )
{
	HRESULT		hr = S_OK;

#if defined(_DEBUG)
	VARIANT	varTemp;	// Use at the end to compare vs the calculated result.
	::VariantInit( &varTemp );
	HRESULT	hrTempConvertResult = ::VariantChangeType( &varTemp, pvarSrc, wFlags, vt );
	// Sometimes this will fail and that's OK. Cache the result of the temp
	// conversion and check against the real result.
#else
	// In release mode, use native conversion.
	// In debug mode, pretend like we're forced to use W2K style conversion.
	if ( IsVT8Supported() )
	{
		hr = ::VariantChangeType( pvargDest, pvarSrc, wFlags, vt );
	}
	else
#endif
	{
		// If any of the routines fail to convert (or we don't handle the
		// explicit conversion) then the native conversion routine will be used.
		hr = E_FAIL;

		// TODO: handle VT_BYREF (or not).
		switch ( vt )
		{
			case VT_I8:
			case VT_UI8:
				hr = VariantUtil::ConvertToI8( pvargDest, pvarSrc, vt );
				break;

			case VT_BSTR:
				hr = VariantUtil::ConvertToBSTR( pvargDest, pvarSrc );
				break;

			case VT_UI4:
			case VT_I4:
				hr = VariantUtil::ConvertToI4( pvargDest, pvarSrc, vt );
				break;

			case VT_I1:
				hr = VariantUtil::ConvertToI1( pvargDest, pvarSrc, vt );
				break;

			default:
				break;
		}

		if ( FAILED(hr) )
		{
			// If we're here and !IsVT8Supported() (W2K), but the src is VT8,
			// this function is going to fail on W2K, so ASSERT if that's the case.
			// If this ASSERTs, you must add another case for your conversion type
			// above, or you will totally break W2K support.
			ASSERT( (pvarSrc == NULL) || ((pvarSrc->vt != VT_UI8) && (pvarSrc->vt != VT_I8)) );
			hr = ::VariantChangeType( pvargDest, pvarSrc, wFlags, vt );
			// Sometimes this will fail and that's OK.
		}
	}

#if defined(_DEBUG)
	// Make sure that it "worked".
	if ( SUCCEEDED(hrTempConvertResult) )
	{
		// If you scroll down, you'll notice that the native VarCmp fails 
		// on many, many variant types. If this Compare function fails, and
		// you see that pvargDest and temp are really the same, then you must add
		// an if/else to the Compare() function.
		HRESULT	hrCmp = Compare( pvargDest, &varTemp, LOCALE_USER_DEFAULT, wFlags );
		ASSERT( hrCmp == VARCMP_EQ );
		::VariantClear( &varTemp );
	}
	else	// make sure they failed the same
	{
		ASSERT( hrTempConvertResult == hr );
	}
#endif

	return hr;
}


// ----------------------------------------------------------------------------
//		Compare
// ----------------------------------------------------------------------------

HRESULT
VariantUtil::Compare(
	LPVARIANT	pvarLeft,
	LPVARIANT	pvarRight,
	LCID		lcid,
	ULONG		dwFlags )
{
	// Sanity checks.
	if ( pvarLeft == NULL ) return E_INVALIDARG;
	if ( pvarRight == NULL ) return E_INVALIDARG;

	HRESULT	hr = S_OK;

	// Apparently stupid VarCmp doesn't handle these types!?
	VARTYPE	vtLeft = V_VT(pvarLeft);
	VARTYPE	vtRight = V_VT(pvarRight);
	if ( (vtLeft == VT_UI4) && (vtRight == VT_UI4) )
	{
		if ( V_UI4(pvarLeft) > V_UI4(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_UI4(pvarLeft) < V_UI4(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( (vtLeft == VT_UI8) && (vtRight == VT_UI8) )
	{
		if ( V_UI8(pvarLeft) > V_UI8(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_UI8(pvarLeft) < V_UI8(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( (vtLeft == VT_I8) && (vtRight == VT_I8) )
	{
		if ( V_I8(pvarLeft) > V_I8(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_I8(pvarLeft) < V_I8(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( (vtLeft == VT_UI2) && (vtRight == VT_UI2) )
	{
		if ( V_UI2(pvarLeft) > V_UI2(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_UI2(pvarLeft) < V_UI2(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( (vtLeft == VT_INT) && (vtRight == VT_INT) )
	{
		if ( V_INT(pvarLeft) > V_INT(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_INT(pvarLeft) < V_INT(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( (vtLeft == VT_UINT) && (vtRight == VT_UINT) )
	{
		if ( V_UINT(pvarLeft) > V_UINT(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_UINT(pvarLeft) < V_UINT(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( (vtLeft == VT_I1) && (vtRight == VT_I1))
	{
		if ( V_I1(pvarLeft) > V_I1(pvarRight) )
		{
			hr = VARCMP_GT;
		}
		else if ( V_I1(pvarLeft) < V_I1(pvarRight) )
		{
			hr = VARCMP_LT;
		}
		else
		{
			hr = VARCMP_EQ;
		}
	}
	else if ( ((vtRight == VT_EMPTY) || (vtRight == VT_NULL)) && (vtLeft != VT_EMPTY) )
	{
		hr = VARCMP_GT;		// Non-standard VarCmp behavior.
	}
	else if ( ((vtLeft == VT_EMPTY) || (vtLeft == VT_NULL)) && (vtRight != VT_EMPTY) )
	{
		hr = VARCMP_LT;		// Non-standard VarCmp behavior.
	}
	else
	{
		// Use VarCmp to compare the rest.
		hr = ::VarCmp( pvarLeft, pvarRight, lcid, dwFlags );
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		GetInterface
// ----------------------------------------------------------------------------

HRESULT
VariantUtil::GetInterface(
	VARIANTARG*	pVar,
	REFIID		iid,
	void**		pp )
{
	// Sanity checks.
	if ( pVar == NULL ) return E_INVALIDARG;
	if ( pp == NULL ) return E_INVALIDARG;

	HRESULT	hr = S_OK;

	// Check for variant type VT_UNKNOWN.
	if ( V_VT(pVar) != VT_UNKNOWN )
	{
		// Attempt to change type (will this work for VT_DISPATCH)?
		hr = ChangeType( pVar, VT_UNKNOWN );
		if ( FAILED(hr) ) return hr;
	}

	// Sanity checks.
	if ( V_UNKNOWN(pVar) == NULL ) return E_INVALIDARG;

	// Query for the requested interface.
	hr = V_UNKNOWN(pVar)->QueryInterface( iid, pp );

	return hr;
}


// ----------------------------------------------------------------------------
//		WriteToStream
// ----------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable: 4702)
HRESULT
CComVariantEx::WriteToStream(
	IStream*	pStream )
{
	if(pStream == NULL)
		return E_INVALIDARG;
		
	HRESULT hr = pStream->Write(&vt, sizeof(VARTYPE), NULL);
	if (FAILED(hr))
		return hr;

	const VARTYPE	elementVT = (VARTYPE)(vt & ~VT_ARRAY);
	const bool		bArray    = ((vt & VT_ARRAY) != 0);

	int		cbWrite = 0;
	switch ( elementVT )
	{
		case VT_NULL:
			return S_OK;
		case VT_UNKNOWN:
		case VT_DISPATCH:
		{
			ATLASSERT( !bArray );	// we cannot write arrays of non-primitive values
			if ( bArray ) return E_NOTIMPL;
			
			CComPtr<IPersistStream> spStream;
			if (punkVal != NULL)
			{
				hr = punkVal->QueryInterface(__uuidof(IPersistStream), (void**)&spStream);
				if (FAILED(hr))
				{
					hr = punkVal->QueryInterface(__uuidof(IPersistStreamInit), (void**)&spStream);
					if (FAILED(hr))
						return hr;
				}
			}
			if (spStream != NULL)
				return OleSaveToStream(spStream, pStream);
			return WriteClassStm(pStream, CLSID_NULL);
		}
		case VT_UI1:
		case VT_I1:
			cbWrite = sizeof(BYTE);
			break;
		case VT_I2:
		case VT_UI2:
		case VT_BOOL:
			cbWrite = sizeof(short);
			break;
		case VT_I4:
		case VT_UI4:
		case VT_R4:
		case VT_INT:
		case VT_UINT:
		case VT_ERROR:
			cbWrite = sizeof(long);
			break;
		case VT_I8:
		case VT_UI8:
			cbWrite = sizeof(LONGLONG);
			break;
		case VT_R8:
		case VT_CY:
		case VT_DATE:
			cbWrite = sizeof(double);
			break;
		default:
			break;
	}

	if ( bArray )
	{
		ATLASSERT( 0 != cbWrite );	// we can only write arrays of regular size elements
		if ( 0 == cbWrite ) return E_NOTIMPL;

		// we only handle 1-dimensional arrays, I don't need anything deeper
		LPSAFEARRAY		pArray = V_ARRAY( this );
		UINT			nDimCount = SafeArrayGetDim( pArray );
		ATLASSERT( 1 == nDimCount );
		if ( 1 != nDimCount ) return E_NOTIMPL;

		// write array bounds (usually [0,n-1], but could be [a,b-1], empty is [0,-1]
		LONG	nLowerBound	= 0;
		LONG	nUpperBound	= 0;
		hr = SafeArrayGetLBound( pArray, 1, &nLowerBound );
		if ( FAILED(hr) ) return hr;
		hr = SafeArrayGetUBound( pArray, 1, &nUpperBound );
		if ( FAILED(hr) ) return hr;
		hr = pStream->Write( &nLowerBound, sizeof(nLowerBound), NULL );
		if ( FAILED(hr) ) return hr;
		hr = pStream->Write( &nUpperBound, sizeof(nUpperBound), NULL );
		if ( FAILED(hr) ) return hr;

		// allocate a single-element buffer
		const size_t	MAX_ELEMENT_BYTE_COUNT = 16;
		ATLASSERT( cbWrite <= MAX_ELEMENT_BYTE_COUNT );
		if ( MAX_ELEMENT_BYTE_COUNT < cbWrite ) return E_NOTIMPL;

		// write each element
		char	buffer[MAX_ELEMENT_BYTE_COUNT];
		for ( LONG i = nLowerBound; i <= nUpperBound; ++i )
		{
			hr = SafeArrayGetElement( pArray, &i, buffer );
			if ( FAILED(hr) ) return hr;
			hr = pStream->Write( buffer, cbWrite, NULL );
			if ( FAILED(hr) ) return hr;
		}
	}
	else
	{
		// write out our one value
		if (cbWrite != 0)
			return pStream->Write((void*) &bVal, cbWrite, NULL);
	
		CComBSTR bstrWrite;
		CComVariantEx varBSTR;
		if (vt != VT_BSTR)
		{
			hr = VariantChangeType(&varBSTR, this, VARIANT_NOVALUEPROP, VT_BSTR);
			if (FAILED(hr))
				return hr;
			bstrWrite.Attach(varBSTR.bstrVal);
		}
		else
			bstrWrite.Attach(bstrVal);
	
		hr = bstrWrite.WriteToStream(pStream);
		bstrWrite.Detach();
	}

	return hr;
}
#pragma warning(pop)	// C4702


/// @class StSafeArray
/// utility class to make sure we don't leak in the face of errors
class StSafeArray
{
public:
	StSafeArray( SAFEARRAY* inArray ) : m_pArray( inArray ) { }
	~StSafeArray() { if ( NULL != m_pArray ) (void) SafeArrayDestroy( m_pArray ); }
	SAFEARRAY* release() { SAFEARRAY* pResult = m_pArray; m_pArray = NULL; return pResult; }
private:	// no OCF
	StSafeArray();
	StSafeArray( const StSafeArray& inOriginal );
	StSafeArray& operator=( const StSafeArray& inOriginal );
private:
	SAFEARRAY*	m_pArray;
};


// ----------------------------------------------------------------------------
//		ReadFromStream
// ----------------------------------------------------------------------------

HRESULT
CComVariantEx::ReadFromStream(
	IStream*	pStream )
{
	ATLASSERT(pStream != NULL);
	if(pStream == NULL)
		return E_INVALIDARG;
		
	HRESULT hr;
	hr = Clear();
	if (FAILED(hr))
		return hr;
	VARTYPE vtRead = VT_EMPTY;
	ULONG cbRead = 0;
	hr = pStream->Read(&vtRead, sizeof(VARTYPE), &cbRead);
	if (hr == S_FALSE || (cbRead != sizeof(VARTYPE) && hr == S_OK))
		hr = E_FAIL;
	if (FAILED(hr))
		return hr;
	vt = vtRead;

	const VARTYPE	elementVT = (VARTYPE)(vt & ~VT_ARRAY);
	const bool		bArray    = ((vt & VT_ARRAY) != 0);

	cbRead = 0;
	switch (elementVT)
	{
		case VT_NULL:
			return S_OK;
		case VT_UNKNOWN:
		case VT_DISPATCH:
		{
			ATLASSERT( !bArray );	// we cannot read arrays of non-primitive values
			if ( bArray ) return E_NOTIMPL;

			punkVal = NULL;
			hr = OleLoadFromStream(pStream,
				(vtRead == VT_UNKNOWN) ? __uuidof(IUnknown) : __uuidof(IDispatch),
				(void**)&punkVal);
			// If IPictureDisp or IFontDisp property is not set, 
			// OleLoadFromStream() will return REGDB_E_CLASSNOTREG.
			if (hr == REGDB_E_CLASSNOTREG)
				hr = S_OK;
			return hr;
		}
		case VT_UI1:
		case VT_I1:
			cbRead = sizeof(BYTE);
			break;
		case VT_I2:
		case VT_UI2:
		case VT_BOOL:
			cbRead = sizeof(short);
			break;
		case VT_I4:
		case VT_UI4:
		case VT_R4:
		case VT_INT:
		case VT_UINT:
		case VT_ERROR:
			cbRead = sizeof(long);
			break;
		case VT_I8:
		case VT_UI8:
			cbRead = sizeof(LONGLONG);
			break;
		case VT_R8:
		case VT_CY:
		case VT_DATE:
			cbRead = sizeof(double);
			break;
		default:
			break;
	}

	if ( bArray )	// read an array
	{
		ATLASSERT( 0 != cbRead );	// we can only read arrays of regular size elements
		if ( 0 == cbRead ) return E_NOTIMPL;

		// read array bounds (usually [0,n-1], but could be [a,b-1], empty is [0,-1]
		LONG	nLowerBound = 0;
		LONG	nUpperBound = 0;
		hr = pStream->Read( &nLowerBound, sizeof(nLowerBound), NULL );
		if ( FAILED(hr) )return hr;
		hr = pStream->Read( &nUpperBound, sizeof(nUpperBound), NULL );
		if ( FAILED(hr) ) return hr;
		
		// allocate the array
		LONG		nElementCount = nUpperBound - nLowerBound + 1;
		ATLASSERT( 0 <= nElementCount );
		if ( nElementCount < 0 ) return E_FAIL;		// negative count? Probably corrupt stream
		SAFEARRAY*	pArray = SafeArrayCreateVector( elementVT, nLowerBound, nElementCount );
		if ( NULL == pArray ) return E_FAIL;
		StSafeArray	arrayDisposer( pArray );	// dispose if we don't exit cleanly
		
		// read the elements
		const size_t	MAX_ELEMENT_BYTE_COUNT = 16;
		ATLASSERT( cbRead <= MAX_ELEMENT_BYTE_COUNT );
		if ( MAX_ELEMENT_BYTE_COUNT < cbRead ) return E_NOTIMPL;
		char 	buffer[ MAX_ELEMENT_BYTE_COUNT ];
		for ( LONG i = nLowerBound; i <= nUpperBound; ++i )
		{
			hr = pStream->Read( buffer, cbRead, NULL );
			if ( FAILED(hr) ) return hr;
			hr = SafeArrayPutElement( pArray, &i, buffer );
			if ( FAILED(hr) ) return hr;
		}

		// assume ownership of the array
		parray = arrayDisposer.release();
		return S_OK;
	}
	else	// read a single value
	{
		if (cbRead != 0)
		{
			hr = pStream->Read((void*) &bVal, cbRead, NULL);
			if (hr == S_FALSE)
				hr = E_FAIL;
			return hr;
		}
		CComBSTR bstrRead;
	
		hr = bstrRead.ReadFromStream(pStream);
		if (FAILED(hr))
		{
			// If CComBSTR::ReadFromStream failed, reset seek pointer to start of
			// variant type.
			LARGE_INTEGER nOffset;
			nOffset.QuadPart = -(static_cast<LONGLONG>(sizeof(VARTYPE)));
			pStream->Seek(nOffset, STREAM_SEEK_CUR, NULL);
			return hr;
		}
		vt = VT_BSTR;
		bstrVal = bstrRead.Detach();
		if (vtRead != VT_BSTR)
			hr = ChangeType(vtRead);
	}
	return hr;
}


// -----------------------------------------------------------------------------
//		HexEncode
// -----------------------------------------------------------------------------

void
CComVariantEx::HexEncode(
	const BYTE*		pData,
	size_t			nDataLen )
{
	this->Clear();

	CStringA	strData;

	if ( (pData != NULL) && (nDataLen > 0) )
	{
		int			nDataStrLen = AtlHexEncodeGetRequiredLength( (int) nDataLen );
		char*		pszData = strData.GetBuffer( nDataStrLen );
		if ( pszData == NULL ) AtlThrow( E_OUTOFMEMORY );
		if ( !AtlHexEncode( pData, (int) nDataLen, pszData, &nDataStrLen ) )
		{
			AtlThrow( E_FAIL );
		}
		pszData[nDataStrLen] = 0;	// Null-terminate the string.
		strData.ReleaseBuffer( nDataStrLen );
	}

	*this = strData;
}


// -----------------------------------------------------------------------------
//		HexDecode
// -----------------------------------------------------------------------------

void
CComVariantEx::HexDecode(
	BYTE*		pData,
	size_t&		nDataLen )
{
	// Sanity checks.
	if ( pData == NULL ) AtlThrow( E_POINTER );
	if ( nDataLen == 0 ) AtlThrow( E_INVALIDARG );
	if ( this->vt != VT_BSTR ) AtlThrow( E_INVALIDARG );

	CW2A	strData( this->bstrVal );
	LPCSTR	pSrcData = strData;
	int		nSrcLen = (int) strlen( pSrcData );
	int		nDestLen = AtlHexDecodeGetRequiredLength( nSrcLen );

	// HACK: stupid AtlHexDecodeGetRequiredLength algorithm is
	// (nSrcLen/2)+1 in ATL 7. Pretend there's one extra byte of data
	// if this algorithm is being used. We change the input value for
	// AtlHexDecode because it uses AtlHexDecodeGetRequiredLength to check.
	if ( AtlHexDecodeGetRequiredLength( 0 ) == 1 )
	{
		nDataLen = nDataLen + 1;
	}

	if ( (int) nDataLen < nDestLen )
	{
		HRESULT	hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		AtlThrow( hr );
	}

	if ( !AtlHexDecode( pSrcData, nSrcLen, pData, &nDestLen ) )
	{
		AtlThrow( E_FAIL );
	}

	nDataLen = nDestLen;
}

