// ============================================================================
//	VariantUtil.h
// ============================================================================
//	Copyright (c) 2003-2006 WildPackets, Inc. All rights reserved.

#pragma once

#include <atlcomcli.h>

namespace VariantUtil
{
	// Same signature as ::VariantClear() but handles VT_I8/VT_UI8.
	HRESULT Clear( VARIANTARG* pvarg );

	// Same signature as ::VariantCopy() but handles VT_I8/VT_UI8.
	HRESULT Copy( VARIANTARG* pvargDest, VARIANTARG* pvargSrc);

	// Same signature as ::VariantCopyInd() but handles VT_I8/VT_UI8.
	HRESULT	CopyInd( VARIANT* pvarDest, VARIANTARG* pvargSrc );

	// Same signature as ::VariantChangeType() but handles VT_I8/VT_UI8.
	HRESULT ChangeType( VARIANTARG* pvargDest, VARIANTARG* pvarSrc, USHORT wFlags, VARTYPE vt);

	// Shortcut for changing a VARIANT in place with default flags.
	inline HRESULT ChangeType( VARIANTARG* pVar, VARTYPE vt )
	{
		return ChangeType( pVar, pVar, 0, vt );
	}

	// Same signature as ::VarCmp() but handles VT_I8/VT_UI8.
	HRESULT	Compare( LPVARIANT pvarLeft, LPVARIANT pvarRight,
				LCID lcid = LOCALE_USER_DEFAULT, ULONG dwFlags = 0 );

	// Compare variants with a return code like memcmp().
	inline int	CompareCStyle( LPVARIANT pvarLeft, LPVARIANT pvarRight,
					LCID lcid = LOCALE_USER_DEFAULT, ULONG dwFlags = 0 )
	{
		// See comments in oleauto.h regarding values of VARCMP_LT etc.
		return Compare( pvarLeft, pvarRight, lcid, dwFlags ) - 1;
	}

	// Get the requested COM interface from a VARIANT.
	HRESULT GetInterface( VARIANTARG* pVar, REFIID iid, void** pp );

	// Shortcut for geting a COM interface from a VARIANT.
	template<class Q>
	HRESULT GetInterface( VARIANTARG* pVar, Q** pp )
	{
		return GetInterface( pVar, __uuidof(Q), (void**) pp );
	}

} /* namespace VariantUtil */



class CComVariantEx : public tagVARIANT
{
// Constructors
public:
	CComVariantEx() throw()
	{
		::VariantInit(this);
	}

	~CComVariantEx() throw()
	{
		Clear();
	}

	CComVariantEx(const VARIANT& varSrc)
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}

	CComVariantEx(const CComVariantEx& varSrc)
	{
		vt = VT_EMPTY;
		InternalCopy(&varSrc);
	}
	CComVariantEx(LPCOLESTR lpszSrc)
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}

	CComVariantEx(LPCSTR lpszSrc)
	{
		vt = VT_EMPTY;
		*this = lpszSrc;
	}

	CComVariantEx(bool bSrc)
	{
		vt = VT_BOOL;
		boolVal = bSrc ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	CComVariantEx(int nSrc, VARTYPE vtSrc = VT_I4) throw()
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_INT);
		vt = vtSrc;
		intVal = nSrc;
	}

	CComVariantEx(BYTE nSrc) throw()
	{
		vt = VT_UI1;
		bVal = nSrc;
	}

	CComVariantEx(short nSrc) throw()
	{
		vt = VT_I2;
		iVal = nSrc;
	}

	CComVariantEx(long nSrc, VARTYPE vtSrc = VT_I4) throw()
	{
		ATLASSERT(vtSrc == VT_I4 || vtSrc == VT_ERROR);
		vt = vtSrc;
		lVal = nSrc;
	}

	CComVariantEx(float fltSrc) throw()
	{
		vt = VT_R4;
		fltVal = fltSrc;
	}

	CComVariantEx(double dblSrc, VARTYPE vtSrc = VT_R8) throw()
	{
		ATLASSERT(vtSrc == VT_R8 || vtSrc == VT_DATE);
		vt = vtSrc;
		dblVal = dblSrc;
	}

	CComVariantEx(LONGLONG nSrc) throw()
	{
		vt = VT_I8;
		llVal = nSrc;
	}

	CComVariantEx(ULONGLONG nSrc) throw()
	{
		vt = VT_UI8;
		ullVal = nSrc;
	}

	CComVariantEx(CY cySrc) throw()
	{
		vt = VT_CY;
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
	}

	CComVariantEx(IDispatch* pSrc) throw()
	{
		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
	}

	CComVariantEx(IUnknown* pSrc) throw()
	{
		vt = VT_UNKNOWN;
		punkVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();
	}

	CComVariantEx(char cSrc) throw()
	{
		vt = VT_I1;
		cVal = cSrc;
	}

	CComVariantEx(unsigned short nSrc) throw()
	{
		vt = VT_UI2;
		uiVal = nSrc;
	}

	CComVariantEx(unsigned long nSrc) throw()
	{
		vt = VT_UI4;
		ulVal = nSrc;
	}

	CComVariantEx(unsigned int nSrc, VARTYPE vtSrc = VT_UI4) throw()
	{
		ATLASSERT(vtSrc == VT_UI4 || vtSrc == VT_UINT);
		vt = vtSrc;
		uintVal= nSrc;
	}

	CComVariantEx(const CComBSTR& bstrSrc)
	{
		vt = VT_EMPTY;
		*this = bstrSrc;
	}

	CComVariantEx(const SAFEARRAY *pSrc)
	{
		LPSAFEARRAY pCopy;
		if (pSrc != NULL)
		{
			HRESULT hRes = ::SafeArrayCopy((LPSAFEARRAY)pSrc, &pCopy);
			if (SUCCEEDED(hRes) && pCopy != NULL)
			{
				::SafeArrayGetVartype((LPSAFEARRAY)pSrc, &vt);
				vt |= VT_ARRAY;
				parray = pCopy;
			}
			else
			{
				vt = VT_ERROR;
				scode = hRes;
			}
		}
	}

	CComVariantEx(const BYTE* pData, size_t cbData )
	{
		this->HexEncode( pData, cbData );
	}

// Assignment Operators
public:
	CComVariantEx& operator=(const CComVariantEx& varSrc)
	{
		InternalCopy(&varSrc);
		return *this;
	}

	CComVariantEx& operator=(const VARIANT& varSrc)
	{
		InternalCopy(&varSrc);
		return *this;
	}

	CComVariantEx& operator=(const CComBSTR& bstrSrc)
	{
		Clear();
		vt = VT_BSTR;
		bstrVal = bstrSrc.Copy();
		if (bstrVal == NULL && bstrSrc.m_str != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
		}
		return *this;
	}

	CComVariantEx& operator=(LPCOLESTR lpszSrc)
	{
		Clear();
		vt = VT_BSTR;
		bstrVal = ::SysAllocString(lpszSrc);

		if (bstrVal == NULL && lpszSrc != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
		}
		return *this;
	}

	CComVariantEx& operator=(LPCSTR lpszSrc)
	{
		USES_CONVERSION_EX;
		Clear();
		vt = VT_BSTR;
		bstrVal = ::SysAllocString(A2COLE_EX(lpszSrc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD));

		if (bstrVal == NULL && lpszSrc != NULL)
		{
			vt = VT_ERROR;
			scode = E_OUTOFMEMORY;
		}
		return *this;
	}

	CComVariantEx& operator=(bool bSrc)
	{
		if (vt != VT_BOOL)
		{
			Clear();
			vt = VT_BOOL;
		}
		boolVal = bSrc ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return *this;
	}

	CComVariantEx& operator=(int nSrc) throw()
	{
		if (vt != VT_I4)
		{
			Clear();
			vt = VT_I4;
		}
		intVal = nSrc;

		return *this;
	}

	CComVariantEx& operator=(BYTE nSrc) throw()
	{
		if (vt != VT_UI1)
		{
			Clear();
			vt = VT_UI1;
		}
		bVal = nSrc;
		return *this;
	}

	CComVariantEx& operator=(short nSrc) throw()
	{
		if (vt != VT_I2)
		{
			Clear();
			vt = VT_I2;
		}
		iVal = nSrc;
		return *this;
	}

	CComVariantEx& operator=(long nSrc) throw()
	{
		if (vt != VT_I4)
		{
			Clear();
			vt = VT_I4;
		}
		lVal = nSrc;
		return *this;
	}

	CComVariantEx& operator=(float fltSrc) throw()
	{
		if (vt != VT_R4)
		{
			Clear();
			vt = VT_R4;
		}
		fltVal = fltSrc;
		return *this;
	}

	CComVariantEx& operator=(double dblSrc) throw()
	{
		if (vt != VT_R8)
		{
			Clear();
			vt = VT_R8;
		}
		dblVal = dblSrc;
		return *this;
	}

	CComVariantEx& operator=(CY cySrc) throw()
	{
		if (vt != VT_CY)
		{
			Clear();
			vt = VT_CY;
		}
		cyVal.Hi = cySrc.Hi;
		cyVal.Lo = cySrc.Lo;
		return *this;
	}

	CComVariantEx& operator=(IDispatch* pSrc) throw()
	{
		Clear();
		vt = VT_DISPATCH;
		pdispVal = pSrc;
		// Need to AddRef as VariantClear will Release
		if (pdispVal != NULL)
			pdispVal->AddRef();
		return *this;
	}

	CComVariantEx& operator=(IUnknown* pSrc) throw()
	{
		Clear();
		vt = VT_UNKNOWN;
		punkVal = pSrc;

		// Need to AddRef as VariantClear will Release
		if (punkVal != NULL)
			punkVal->AddRef();
		return *this;
	}

	CComVariantEx& operator=(char cSrc) throw()
	{
		if (vt != VT_I1)
		{
			Clear();
			vt = VT_I1;
		}
		cVal = cSrc;
		return *this;
	}

	CComVariantEx& operator=(unsigned short nSrc) throw()
	{
		if (vt != VT_UI2)
		{
			Clear();
			vt = VT_UI2;
		}
		uiVal = nSrc;
		return *this;
	}

	CComVariantEx& operator=(unsigned long nSrc) throw()
	{
		if (vt != VT_UI4)
		{
			Clear();
			vt = VT_UI4;
		}
		ulVal = nSrc;
		return *this;
	}

	CComVariantEx& operator=(unsigned int nSrc) throw()
	{
		if (vt != VT_UI4)
		{
			Clear();
			vt = VT_UI4;
		}
		uintVal= nSrc;
		return *this;
	}

	CComVariantEx& operator=(BYTE* pbSrc) throw()
	{
		if (vt != (VT_UI1|VT_BYREF))
		{
			Clear();
			vt = VT_UI1|VT_BYREF;
		}
		pbVal = pbSrc;
		return *this;
	}

	CComVariantEx& operator=(short* pnSrc) throw()
	{
		if (vt != (VT_I2|VT_BYREF))
		{
			Clear();
			vt = VT_I2|VT_BYREF;
		}
		piVal = pnSrc;
		return *this;
	}

#ifdef _NATIVE_WCHAR_T_DEFINED
	CComVariantEx& operator=(USHORT* pnSrc) throw()
	{
		if (vt != (VT_UI2|VT_BYREF))
		{
			Clear();
			vt = VT_UI2|VT_BYREF;
		}
		puiVal = pnSrc;
		return *this;
	}
#endif

	CComVariantEx& operator=(int* pnSrc) throw()
	{
		if (vt != (VT_I4|VT_BYREF))
		{
			Clear();
			vt = VT_I4|VT_BYREF;
		}
		pintVal = pnSrc;
		return *this;
	}

	CComVariantEx& operator=(UINT* pnSrc) throw()
	{
		if (vt != (VT_UI4|VT_BYREF))
		{
			Clear();
			vt = VT_UI4|VT_BYREF;
		}
		puintVal = pnSrc;
		return *this;
	}

	CComVariantEx& operator=(long* pnSrc) throw()
	{
		if (vt != (VT_I4|VT_BYREF))
		{
			Clear();
			vt = VT_I4|VT_BYREF;
		}
		plVal = pnSrc;
		return *this;
	}

	CComVariantEx& operator=(ULONG* pnSrc) throw()
	{
		if (vt != (VT_UI4|VT_BYREF))
		{
			Clear();
			vt = VT_UI4|VT_BYREF;
		}
		pulVal = pnSrc;
		return *this;
	}

	CComVariantEx& operator=(LONGLONG nSrc) throw()
	{
		if (vt != VT_I8)
		{
			Clear();
			vt = VT_I8;
		}
		llVal = nSrc;

		return *this;
	}

	CComVariantEx& operator=(LONGLONG* pnSrc) throw()
	{
		if (vt != (VT_I8|VT_BYREF))
		{
			Clear();
			vt = VT_I8|VT_BYREF;
		}
		pllVal = pnSrc;
		return *this;
	}

	CComVariantEx& operator=(ULONGLONG nSrc) throw()
	{
		if (vt != VT_UI8)
		{
			Clear();
			vt = VT_UI8;
		}
		ullVal = nSrc;

		return *this;
	}

	CComVariantEx& operator=(ULONGLONG* pnSrc) throw()
	{
		if (vt != (VT_UI8|VT_BYREF))
		{
			Clear();
			vt = VT_UI8|VT_BYREF;
		}
		pullVal = pnSrc;
		return *this;
	}

	CComVariantEx& operator=(float* pfSrc) throw()
	{
		if (vt != (VT_R4|VT_BYREF))
		{
			Clear();
			vt = VT_R4|VT_BYREF;
		}
		pfltVal = pfSrc;
		return *this;
	}

	CComVariantEx& operator=(double* pfSrc) throw()
	{
		if (vt != (VT_R8|VT_BYREF))
		{
			Clear();
			vt = VT_R8|VT_BYREF;
		}
		pdblVal = pfSrc;
		return *this;
	}

	CComVariantEx& operator=(const SAFEARRAY *pSrc)
	{
		Clear();
		LPSAFEARRAY pCopy;
		if (pSrc != NULL)
		{
			HRESULT hRes = ::SafeArrayCopy((LPSAFEARRAY)pSrc, &pCopy);
			if (SUCCEEDED(hRes) && pCopy != NULL)
			{
				::SafeArrayGetVartype((LPSAFEARRAY)pSrc, &vt);
				vt |= VT_ARRAY;
				parray = pCopy;
			}
			else
			{
				vt = VT_ERROR;
				scode = hRes;
			}
		}
		return *this;
	}

// Comparison Operators
public:
	bool operator==(const VARIANT& varSrc) const throw()
	{
		// For backwards compatibility
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return true;
		return VariantUtil::Compare((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0) == VARCMP_EQ;
	}

	bool operator!=(const VARIANT& varSrc) const throw()
	{
		return !operator==(varSrc);
	}

	bool operator<(const VARIANT& varSrc) const throw()
	{
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return false;
		return VariantUtil::Compare((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0)==VARCMP_LT;
	}

	bool operator>(const VARIANT& varSrc) const throw()
	{
		if (vt == VT_NULL && varSrc.vt == VT_NULL)
			return false;
		return VariantUtil::Compare((VARIANT*)this, (VARIANT*)&varSrc, LOCALE_USER_DEFAULT, 0)==VARCMP_GT;
	}

// Operations
public:
	HRESULT Clear() { return Clear_Impl(); }
	HRESULT Copy(const VARIANT* pSrc) { return Copy_Impl(pSrc); }
	HRESULT CopyInd(const VARIANT* pSrc) { return CopyInd_Impl(pSrc); }
	// copy VARIANT to BSTR
	HRESULT CopyTo(BSTR *pstrDest)
	{
		ATLASSERT(pstrDest != NULL && vt == VT_BSTR);
		HRESULT hRes = E_POINTER;
		if (pstrDest != NULL && vt == VT_BSTR)
		{
			*pstrDest = ::SysAllocStringByteLen((char*)bstrVal, ::SysStringByteLen(bstrVal));
			if (*pstrDest == NULL)
				hRes = E_OUTOFMEMORY;
			else
				hRes = S_OK;
		}
		else if (vt != VT_BSTR)
			hRes = DISP_E_TYPEMISMATCH;
		return hRes;
	}
	HRESULT Attach(VARIANT* pSrc)
	{
		if(pSrc == NULL)
			return E_INVALIDARG;
			
		// Clear out the variant
		HRESULT hr = Clear();
		if (!FAILED(hr))
		{
			// Copy the contents and give control to CComVariantEx
			memcpy(this, pSrc, sizeof(VARIANT));
			pSrc->vt = VT_EMPTY;
			hr = S_OK;
		}
		return hr;
	}

	HRESULT Detach(VARIANT* pDest)
	{
		ATLASSERT(pDest != NULL);
		if(pDest == NULL)
			return E_POINTER;
			
		// Clear out the variant
		HRESULT hr = VariantUtil::Clear(pDest);
		if (!FAILED(hr))
		{
			// Copy the contents and remove control from CComVariantEx
			memcpy(pDest, this, sizeof(VARIANT));
			vt = VT_EMPTY;
			hr = S_OK;
		}
		return hr;
	}

	HRESULT ChangeType(VARTYPE vtNew, const VARIANT* pSrc = NULL)
	{
		return ChangeType_Impl(vtNew,pSrc);
	}

	template< typename T >
	void SetByRef( T* pT ) throw()
	{
		Clear();
		vt = CVarTypeInfo< T >::VT|VT_BYREF;
		byref = pT;
	}

	template<class Q>
	HRESULT GetInterface( Q** pp)
	{
		return VariantUtil::GetInterface( this, __uuidof(Q), (void**) pp );
	}

	// Return the size in bytes of the current contents
	ULONG	GetSize() const;

	HRESULT WriteToStream(IStream* pStream);
	HRESULT ReadFromStream(IStream* pStream);

	void	HexEncode(const BYTE* pData, size_t cbData);
	void	HexDecode(BYTE* pData, size_t& cbData);

// Implementation
public:
	HRESULT InternalClear()
	{
		HRESULT hr = Clear();
		ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
		}
		return hr;
	}

	void InternalCopy(const VARIANT* pSrc)
	{
		HRESULT hr = Copy(pSrc);
		if (FAILED(hr))
		{
			vt = VT_ERROR;
			scode = hr;
		}
	}

private:
	HRESULT Clear_Impl()
	{
		return VariantUtil::Clear( this );
	}
	HRESULT Copy_Impl(const VARIANT* pSrc)
	{
		return VariantUtil::Copy( this, const_cast<VARIANT*>(pSrc) );
	}
	HRESULT CopyInd_Impl(const VARIANT* pSrc)
	{
		return VariantUtil::CopyInd( this, const_cast<VARIANT*>(pSrc) );
	}
	HRESULT ChangeType_Impl(VARTYPE vtNew, const VARIANT* pSrc = NULL)
	{
		VARIANT* pVar = const_cast<VARIANT*>(pSrc);
		// Convert in place if pSrc is NULL
		if (pVar == NULL)
		{
			// Do nothing if doing in place convert and vts not different
			if ( this->vt == vtNew ) return S_OK;
			pVar = this;
		}
		return VariantUtil::ChangeType(this, pVar, 0, vtNew);
	}
};


inline ULONG CComVariantEx::GetSize() const
{
	ULONG nSize = sizeof(VARTYPE);
	HRESULT hr;

	switch (vt)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			CComPtr<IPersistStream> spStream;
			if (punkVal != NULL)
			{
				hr = punkVal->QueryInterface(__uuidof(IPersistStream), (void**)&spStream);
				if (FAILED(hr))
				{
					hr = punkVal->QueryInterface(__uuidof(IPersistStreamInit), (void**)&spStream);
					if (FAILED(hr))
						break;
				}
			}
			if (spStream != NULL)
			{
				ULARGE_INTEGER nPersistSize;
				nPersistSize.QuadPart = 0;
				spStream->GetSizeMax(&nPersistSize);
				nSize += nPersistSize.LowPart + sizeof(CLSID);
			}
			else
				nSize += sizeof(CLSID);
		}
		break;
	case VT_UI1:
	case VT_I1:
		nSize += sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		nSize += sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
		nSize += sizeof(long);
		break;
	case VT_I8:
	case VT_UI8:
		nSize += sizeof(LONGLONG);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		nSize += sizeof(double);
		break;
	default:
		break;
	}
	if (nSize == sizeof(VARTYPE))
	{
		BSTR        bstr = NULL;
		CComVariantEx varBSTR;
		if (vt != VT_BSTR)
		{
			hr = VariantChangeType(&varBSTR, const_cast<VARIANT*>((const VARIANT*)this), VARIANT_NOVALUEPROP, VT_BSTR);
			if (SUCCEEDED(hr))
				bstr = varBSTR.bstrVal;
		}
		else
			bstr = bstrVal;

		// Add the size of the length, the string itself and the NULL character
		if (bstr != NULL)
			nSize += sizeof(ULONG) + SysStringByteLen(bstr) + sizeof(OLECHAR);
	}
	return nSize;
}
