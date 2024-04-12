// =============================================================================
//	hlvariant.h
// =============================================================================
//	Copyright (c) 2005-2013 WildPackets, Inc. All rights reserved.

#ifndef HLVARIANT_H
#define HLVARIANT_H

#include "hecom.h"
#include "hevariant.h"
#include "hlptr.h"
#include "hlruntime.h"
#include "hlstr.h"
#include "hlstrconv.h"
#include <cstring>
#include <string>

namespace HeLib
{

inline bool
FromHEVARBOOL( Helium::HEVARBOOL b )
{
	return (b != HE_VAR_FALSE);
}

inline Helium::HEVARBOOL
ToHEVARBOOL( bool b )
{
	return b ? HE_VAR_TRUE : HE_VAR_FALSE;
}

class CHeVariant : public Helium::HEVARIANT
{
public:
	CHeVariant() throw()
	{
		vt = Helium::HE_VT_EMPTY;
	}

	~CHeVariant() throw()
	{
		Clear();
	}

	CHeVariant( const Helium::HEVARIANT& varSrc )
	{
		vt = Helium::HE_VT_EMPTY;
		InternalCopy( &varSrc );
	}

	CHeVariant( const CHeVariant& varSrc )
	{
		vt = Helium::HE_VT_EMPTY;
		InternalCopy( &varSrc );
	}

	CHeVariant( const CHeBSTR& bstrSrc )
	{
		vt = Helium::HE_VT_EMPTY;
		*this = bstrSrc;
	}

	CHeVariant( const wchar_t* pszSrc )
	{
		vt = Helium::HE_VT_EMPTY;
		*this = pszSrc;
	}

	CHeVariant( const char* pszSrc )
	{
		vt = Helium::HE_VT_EMPTY;
		*this = pszSrc;
	}

	CHeVariant( const std::wstring& str )
	{
		vt = Helium::HE_VT_EMPTY;
		*this = str;
	}

	CHeVariant( const std::string& str )
	{
		vt = Helium::HE_VT_EMPTY;
		*this = str;
	}

	CHeVariant( bool bSrc ) throw()
	{
		vt = Helium::HE_VT_BOOL;
		boolVal = ToHEVARBOOL( bSrc );
	}

	CHeVariant( SInt8 cSrc ) throw()
	{
		vt = Helium::HE_VT_I1;
		cVal = cSrc;
	}

	CHeVariant( SInt16 nSrc ) throw()
	{
		vt = Helium::HE_VT_I2;
		iVal = nSrc;
	}

	CHeVariant( SInt32 nSrc ) throw()
	{
		vt = Helium::HE_VT_I4;
		lVal = nSrc;
	}

	CHeVariant( SInt64 nSrc ) throw()
	{
		vt = Helium::HE_VT_I8;
		llVal = nSrc;
	}

	CHeVariant( UInt8 nSrc ) throw()
	{
		vt = Helium::HE_VT_UI1;
		bVal = nSrc;
	}

	CHeVariant( UInt16 nSrc ) throw()
	{
		vt = Helium::HE_VT_UI2;
		uiVal = nSrc;
	}

	CHeVariant( UInt32 nSrc ) throw()
	{
		vt = Helium::HE_VT_UI4;
		ulVal = nSrc;
	}

	CHeVariant( UInt64 nSrc ) throw()
	{
		vt = Helium::HE_VT_UI8;
		ullVal = nSrc;
	}

	CHeVariant( float fltSrc ) throw()
	{
		vt = Helium::HE_VT_R4;
		fltVal = fltSrc;
	}

	CHeVariant( double dblSrc ) throw()
	{
		vt = Helium::HE_VT_R8;
		dblVal = dblSrc;
	}

#ifndef OPT_64BITINTTYPES
	CHeVariant( int nSrc ) throw()
	{
		vt = Helium::HE_VT_INT;
		intVal = nSrc;
	}

	CHeVariant( unsigned int nSrc ) throw()
	{
		vt = Helium::HE_VT_UINT;
		uintVal = nSrc;
	}
#endif /* OPT_64BITINTTYPES */

	CHeVariant( Helium::IHeUnknown* pSrc ) throw()
	{
		vt = Helium::HE_VT_UNKNOWN;
		punkVal = pSrc;
		// Need to AddRef as HeVariantClear will Release
		if ( punkVal != 0 )
		{
			punkVal->AddRef();
		}
	}

	CHeVariant( const Helium::HeID& id )
	{
		vt = Helium::HE_VT_EMPTY;
		*this = id;
	}

public:
	CHeVariant& operator=( const CHeVariant& varSrc )
	{
		InternalCopy( &varSrc );
		return *this;
	}

	CHeVariant& operator=( const HEVARIANT& varSrc )
	{
		InternalCopy( &varSrc );
		return *this;
	}

	CHeVariant& operator=( const CHeBSTR& bstrSrc )
	{
		using namespace Helium;
		Clear();
		vt = HE_VT_BSTR;
		bstrVal = bstrSrc.Copy();
		if ( (bstrVal == NULL) && (bstrSrc.m_str != NULL) )
		{
			vt    = HE_VT_ERROR;
			hrVal = HE_E_OUT_OF_MEMORY;
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
		return *this;
	}

	CHeVariant& operator=( const wchar_t* pszSrc )
	{
		using namespace Helium;
		Clear();
		vt = HE_VT_BSTR;
		bstrVal = HeAllocString( pszSrc );
		if ( (bstrVal == NULL) && (pszSrc != NULL) )
		{
			vt    = HE_VT_ERROR;
			hrVal = HE_E_OUT_OF_MEMORY;
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
		return *this;
	}

	CHeVariant& operator=( const char* pszSrc )
	{
		using namespace Helium;
		Clear();
		vt = HE_VT_BSTR;
		HeLib::CA2W	s( pszSrc );
		bstrVal = HeAllocStringLen( s, s.size() );
		if ( (bstrVal == NULL) && (pszSrc != NULL) )
		{
			vt    = HE_VT_ERROR;
			hrVal = HE_E_OUT_OF_MEMORY;
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
		return *this;
	}

	CHeVariant& operator=( const std::wstring& str )
	{
		using namespace Helium;
		Clear();
		vt = HE_VT_BSTR;
		bstrVal = HeAllocStringLen( str.c_str(), str.size() );
		if ( (bstrVal == NULL) && !str.empty() )
		{
			vt    = HE_VT_ERROR;
			hrVal = HE_E_OUT_OF_MEMORY;
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
		return *this;
	}

	CHeVariant& operator=( const std::string& str )
	{
		return operator=( str.c_str() );
	}

	CHeVariant& operator=( bool bSrc )
	{
		using namespace Helium;
		if ( vt != HE_VT_BOOL )
		{
			Clear();
			vt = HE_VT_BOOL;
		}
		boolVal = bSrc ? -1 : 0;
		return *this;
	}

	CHeVariant& operator=( SInt8 cSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_I1 )
		{
			Clear();
			vt = HE_VT_I1;
		}
		cVal = cSrc;
		return *this;
	}

	CHeVariant& operator=( SInt16 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_I2 )
		{
			Clear();
			vt = HE_VT_I2;
		}
		iVal = nSrc;
		return *this;
	}

	CHeVariant& operator=( SInt32 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_I4 )
		{
			Clear();
			vt = HE_VT_I4;
		}
		lVal = nSrc;
		return *this;
	}

	CHeVariant& operator=( SInt64 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_I8 )
		{
			Clear();
			vt = HE_VT_I8;
		}
		llVal = nSrc;

		return *this;
	}

	CHeVariant& operator=( UInt8 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_UI1 )
		{
			Clear();
			vt = HE_VT_UI1;
		}
		bVal = nSrc;
		return *this;
	}

	CHeVariant& operator=( UInt16 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_UI2 )
		{
			Clear();
			vt = HE_VT_UI2;
		}
		uiVal = nSrc;
		return *this;
	}

	CHeVariant& operator=( UInt32 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_UI4 )
		{
			Clear();
			vt = HE_VT_UI4;
		}
		ulVal = nSrc;
		return *this;
	}

	CHeVariant& operator=( UInt64 nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_UI8 )
		{
			Clear();
			vt = HE_VT_UI8;
		}
		ullVal = nSrc;

		return *this;
	}

	CHeVariant& operator=( float fltSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_R4 )
		{
			Clear();
			vt = HE_VT_R4;
		}
		fltVal = fltSrc;
		return *this;
	}

	CHeVariant& operator=( double dblSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_R8 )
		{
			Clear();
			vt = HE_VT_R8;
		}
		dblVal = dblSrc;
		return *this;
	}

#ifndef OPT_64BITINTTYPES
	CHeVariant& operator=( int nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_I4 )
		{
			Clear();
			vt = HE_VT_I4;
		}
		intVal = nSrc;

		return *this;
	}

	CHeVariant& operator=( unsigned int nSrc ) throw()
	{
		using namespace Helium;
		if ( vt != HE_VT_UINT )
		{
			Clear();
			vt = HE_VT_UINT;
		}
		uintVal= nSrc;
		return *this;
	}
#endif /* OPT_64BITINTTYPES */

	CHeVariant& operator=( Helium::IHeUnknown* pSrc ) throw()
	{
		using namespace Helium;
		Clear();
		vt = HE_VT_UNKNOWN;
		punkVal = pSrc;

		// Need to AddRef as HeVarClear will Release.
		if ( punkVal != NULL )
		{
			punkVal->AddRef();
		}
		return *this;
	}

	CHeVariant& operator=( const Helium::HeID& id )
	{
		using namespace Helium;
		Clear();
		wchar_t	szID[HeID::string_length];
		HeThrowIf( !id.ToString( szID, HeID::string_length ) );
		bstrVal = HeAllocString( szID );
		if ( bstrVal == NULL )
		{
			HeThrow( HE_E_OUT_OF_MEMORY );
		}
		vt = HE_VT_BSTR;
		return *this;
	}

	CHeVariant& operator=( UInt8* pbSrc ) throw()
	{
		using namespace Helium;
		if (vt != (HE_VT_UI1|HE_VT_BYREF))
		{
			Clear();
			vt = HE_VT_UI1|HE_VT_BYREF;
		}
		pbVal = pbSrc;
		return *this;
	}

	CHeVariant& operator=( SInt16* pnSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_I2|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_I2|HE_VT_BYREF;
		}
		piVal = pnSrc;
		return *this;
	}

	CHeVariant& operator=( UInt16* pnSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_UI2|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_UI2|HE_VT_BYREF;
		}
		puiVal = pnSrc;
		return *this;
	}

	CHeVariant& operator=( SInt32* pnSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_I4|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_I4|HE_VT_BYREF;
		}
		plVal = pnSrc;
		return *this;
	}

	CHeVariant& operator=( UInt32* pnSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_UI4|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_UI4|HE_VT_BYREF;
		}
		pulVal = pnSrc;
		return *this;
	}

	CHeVariant& operator=( SInt64* pnSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_I8|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_I8|HE_VT_BYREF;
		}
		pllVal = pnSrc;
		return *this;
	}

	CHeVariant& operator=( UInt64* pnSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_UI8|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_UI8|HE_VT_BYREF;
		}
		pullVal = pnSrc;
		return *this;
	}

	CHeVariant& operator=( float* pfSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_R4|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_R4|HE_VT_BYREF;
		}
		pfltVal = pfSrc;
		return *this;
	}

	CHeVariant& operator=( double* pfSrc ) throw()
	{
		using namespace Helium;
		if ( vt != (HE_VT_R8|HE_VT_BYREF) )
		{
			Clear();
			vt = HE_VT_R8|HE_VT_BYREF;
		}
		pdblVal = pfSrc;
		return *this;
	}

public:
	bool operator==( const HEVARIANT& varSrc ) const throw()
	{
		using namespace Helium;
		// For backwards compatibility
		if ( (vt == HE_VT_NULL) && (varSrc.vt == HE_VT_NULL) )
			return true;
		return (HeVarCmp( this, &varSrc, 0 ) == HE_VARCMP_EQ);
	}

	bool operator!=( const HEVARIANT& varSrc ) const throw()
	{
		return !operator==( varSrc );
	}

	bool operator<( const HEVARIANT& varSrc ) const throw()
	{
		using namespace Helium;
		if ( (vt == HE_VT_NULL) && (varSrc.vt == HE_VT_NULL) )
			return false;
		return (HeVarCmp( this, &varSrc, 0 ) == HE_VARCMP_LT);
	}

	bool operator>( const HEVARIANT& varSrc ) const throw()
	{
		using namespace Helium;
		if ( (vt == HE_VT_NULL) && (varSrc.vt == HE_VT_NULL) )
			return false;
		return (HeVarCmp( this, &varSrc, 0 ) == HE_VARCMP_GT);
	}

	int Compare( const HEVARIANT& var, UInt32 flags = 0 ) const throw()
	{
		return static_cast<int>(HeVarCmp( this, &var, flags ) - 1);
	}

	int CompareNoCase( const HEVARIANT& var ) const throw()
	{
		return static_cast<int>(HeVarCmp( this, &var, HE_VARCMP_IGNORECASE ) - 1);
	}

public:
	HeResult Clear()
	{
		return HeVarClear( this );
	}
	
	HeResult Copy( const Helium::HEVARIANT* pSrc )
	{
		return HeVarCopy( this, pSrc );
	}
	
	HeResult CopyInd( const Helium::HEVARIANT* pSrc )
	{
		return HeVarCopyInd( this, pSrc );
	}
	
	HeResult CopyTo( Helium::HEBSTR* pbstrDest )
	{
		if ( pbstrDest == NULL ) return HE_E_NULL_POINTER;
		if ( vt != Helium::HE_VT_BSTR ) return HE_E_FAIL;

		HE_ASSERT( (pbstrDest != NULL) && (vt == Helium::HE_VT_BSTR) );

		*pbstrDest = HeLib::HeAllocStringByteLen( 
			reinterpret_cast<char*>(bstrVal), HeStringByteLen( bstrVal ) );

		return (*pbstrDest != NULL) ? HE_S_OK : HE_E_OUT_OF_MEMORY;
	}

	HeResult Attach( Helium::HEVARIANT* pSrc)
	{
		using namespace Helium;

		if ( pSrc == NULL ) return HE_E_NULL_POINTER;
			
		// Clear out the variant
		HeResult	hr = Clear();
		if ( HE_SUCCEEDED(hr) )
		{
			// Copy the contents and give control to CHeVariant.
			std::memcpy( this, pSrc, sizeof(HEVARIANT) );
			pSrc->vt = HE_VT_EMPTY;
			hr = HE_S_OK;
		}
		return hr;
	}

	HeResult Detach( Helium::HEVARIANT* pDest )
	{
		using namespace Helium;

		if ( pDest == NULL ) return HE_E_NULL_POINTER;
			
		// Clear out the variant.
		HeResult	hr = HeVarClear( pDest );
		if ( HE_SUCCEEDED(hr) )
		{
			// Copy the contents and remove control from CHeVariant.
			std::memcpy( pDest, this, sizeof(HEVARIANT) );
			vt = HE_VT_EMPTY;
			hr = HE_S_OK;
		}
		return hr;
	}

	HeResult ChangeType( Helium::HEVARTYPE vtNew, const Helium::HEVARIANT* pSrc = NULL )
	{
		Helium::HEVARIANT*	pVar = const_cast<Helium::HEVARIANT*>(pSrc);
		// Convert in place if pSrc is NULL
		if ( pVar == NULL )
		{
			// Do nothing if doing in place convert and vt not different.
			if ( this->vt == vtNew ) return HE_S_OK;
			pVar = this;
		}
		return HeVarChangeType( this, pVar, 0, vtNew );
	}

	template<class Q>
	HeResult GetInterface( Q** pp)
	{
		// Sanity checks.
		if ( pp == NULL ) return HE_E_NULL_POINTER;

		HeResult	hr = HE_S_OK;

		// Check for variant type HE_VT_UNKNOWN.
		if ( this->vt != Helium::HE_VT_UNKNOWN )
		{
			// Attempt to change type.
			hr = this->ChangeType( Helium::HE_VT_UNKNOWN );
			if ( HE_FAILED(hr) ) return hr;
		}

		// Sanity checks.
		if ( this->punkVal == NULL ) return HE_E_INVALID_ARG;

		// Query for the requested interface.
		hr = this->punkVal->QueryInterface( Q::GetIID(), reinterpret_cast<void**>(pp) );

		return hr;
	}

	HeResult	WriteToStream( Helium::IHeStream* pStream ) const;
	HeResult	ReadFromStream( Helium::IHeStream* pStream );

	// Return the size in bytes of the current contents
	UInt32		GetSize() const;

private:
	void InternalCopy( const Helium::HEVARIANT* pSrc )
	{
		HeResult	hr = Copy( pSrc );
		if ( HE_FAILED(hr) )
		{
			vt = Helium::HE_VT_ERROR;
			hrVal = hr;
		}
	}
};

inline HeResult CHeVariant::WriteToStream( Helium::IHeStream* pStream ) const
{
	using namespace Helium;

	if ( pStream == NULL ) return HE_E_NULL_POINTER;
		
	HeResult	hr = pStream->Write( &vt, sizeof(HEVARTYPE), NULL );
	if ( HE_FAILED(hr) ) return hr;

	int		cbWrite = 0;
	switch ( vt )
	{
		case HE_VT_NULL:
			return HE_S_OK;
		case HE_VT_UNKNOWN:
		case HE_VT_DISPATCH:
		{
			HeLib::CHePtr<IHePersistStream>	spStream;
			if ( punkVal != NULL )
			{
				hr = punkVal->QueryInterface( IHePersistStream::GetIID(),
					reinterpret_cast<void**>(&spStream.p) );
				if ( HE_FAILED(hr) ) return hr;
			}
			if ( spStream != NULL )
			{
				HeCID	cid;
				hr = spStream->GetClassID( &cid );
				if ( HE_FAILED(hr) ) return hr;
				UInt32	cb;
				hr = pStream->Write( &cid, sizeof(cid), &cb );
				if ( HE_FAILED(hr) ) return hr;
				return spStream->Save( pStream, 1 );
			}
			else
			{
				HeCID	cid;
				cid.SetNull();
				UInt32	cb;
				return pStream->Write( &cid, sizeof(cid), &cb );
			}
		}
		case HE_VT_UI1:
		case HE_VT_I1:
			cbWrite = sizeof(UInt8);
			break;
		case HE_VT_I2:
		case HE_VT_UI2:
		case HE_VT_BOOL:
			cbWrite = sizeof(UInt16);
			break;
		case HE_VT_I4:
		case HE_VT_UI4:
		case HE_VT_R4:
		case HE_VT_INT:
		case HE_VT_UINT:
		case HE_VT_ERROR:
			cbWrite = sizeof(UInt32);
			break;
		case HE_VT_I8:
		case HE_VT_UI8:
			cbWrite = sizeof(UInt64);
			break;
		case HE_VT_R8:
		case HE_VT_CY:
		case HE_VT_DATE:
			cbWrite = sizeof(double);
			break;
		default:
			break;
	}

	if ( cbWrite != 0 )
	{
		return pStream->Write( &bVal, static_cast<UInt32>(cbWrite), NULL );
	}

	CHeBSTR		bstrWrite;
	CHeVariant	varBSTR;
	if ( vt != HE_VT_BSTR )
	{
		hr = HeVarChangeType( &varBSTR, this, 0, HE_VT_BSTR );
		if ( HE_FAILED(hr) ) return hr;
		bstrWrite.Attach( varBSTR.bstrVal );
	}
	else
	{
		bstrWrite.Attach( bstrVal );
	}

	hr = bstrWrite.WriteToStream( pStream );
	bstrWrite.Detach();

	return hr;
}

inline HeResult CHeVariant::ReadFromStream( Helium::IHeStream* pStream )
{
	using namespace Helium;

	if ( pStream == NULL ) return HE_E_NULL_POINTER;
		
	HeResult	hr = HeVarClear( this );
	if ( HE_FAILED(hr) ) return hr;
	HEVARTYPE	vtRead = HE_VT_EMPTY;
	UInt32		cbRead = 0;
	hr = pStream->Read( &vtRead, sizeof(HEVARTYPE), &cbRead );
	if ( HE_FAILED(hr) ) return hr;
	if ( (hr != HE_S_OK) || (cbRead != sizeof(HEVARTYPE)) ) return HE_E_FAIL;

	vt     = vtRead;
	cbRead = 0;
	switch ( vtRead )
	{
		case HE_VT_NULL:
			return HE_S_OK;
		case HE_VT_UNKNOWN:
		case HE_VT_DISPATCH:
		{
			punkVal = NULL;

			HeCID	cid;
			UInt32	cb;
			hr = pStream->Read( &cid, sizeof(cid), &cb );
			if ( HE_FAILED(hr) ) return hr;
			if ( cb != sizeof(cid) ) return HE_E_FAIL;
			CHePtr<IHePersistStream>	spObj;
			hr = spObj.CreateInstance( cid );
			if ( hr == HE_E_FACTORY_NOT_REGISTERED )
			{
				// Just be NULL.
				return HE_S_OK;
			}
			if ( HE_FAILED(hr) ) return hr;
			hr = spObj->Load( pStream );
			if ( HE_FAILED(hr) ) return hr;

			if ( vtRead == HE_VT_UNKNOWN )
			{
				hr = spObj->QueryInterface( IHeUnknown::GetIID(),
					reinterpret_cast<void**>(&punkVal) );
			}
			else
			{
				hr = spObj->QueryInterface( IHeDispatch::GetIID(),
					reinterpret_cast<void**>(&punkVal) );
			}
			return hr;
		}
		case HE_VT_UI1:
		case HE_VT_I1:
			cbRead = sizeof(UInt8);
			break;
		case HE_VT_I2:
		case HE_VT_UI2:
		case HE_VT_BOOL:
			cbRead = sizeof(UInt16);
			break;
		case HE_VT_I4:
		case HE_VT_UI4:
		case HE_VT_R4:
		case HE_VT_INT:
		case HE_VT_UINT:
		case HE_VT_ERROR:
			cbRead = sizeof(UInt32);
			break;
		case HE_VT_I8:
		case HE_VT_UI8:
			cbRead = sizeof(UInt64);
			break;
		case HE_VT_R8:
		case HE_VT_CY:
		case HE_VT_DATE:
			cbRead = sizeof(double);
			break;
		default:
			break;
	}
	if ( cbRead != 0 )
	{
		hr = pStream->Read( &bVal, cbRead, NULL );
		if ( hr == HE_S_FALSE ) hr = HE_E_FAIL;
		return hr;
	}

	CHeBSTR	bstrRead;
	hr = bstrRead.ReadFromStream( pStream );
	if ( HE_FAILED(hr) )
	{
		// Reset seek pointer to start of variant type.
		SInt64	nOffset = -(static_cast<SInt64>(sizeof(HEVARTYPE)));
		pStream->Seek( nOffset, IHeStream::STREAM_SEEK_CUR, NULL );
		return hr;
	}
	vt = HE_VT_BSTR;
	bstrVal = bstrRead.Detach();
	if ( vtRead != HE_VT_BSTR )
	{
		hr = ChangeType( vtRead );
	}
	return hr;
}

inline UInt32 CHeVariant::GetSize() const
{
	using namespace Helium;

	std::size_t	nSize = sizeof(HEVARTYPE);
	HeResult	hr;

	switch ( vt )
	{
		case HE_VT_UNKNOWN:
		case HE_VT_DISPATCH:
		{
			CHePtr<IHePersistStream>	spStream;
			if ( punkVal != NULL )
			{
				hr = punkVal->QueryInterface( IHePersistStream::GetIID(),
					reinterpret_cast<void**>(&spStream.p) );
				if ( HE_FAILED(hr) )
					break;
			}
			if ( spStream != NULL )
			{
				UInt64	cbSizeMax = 0;
				spStream->GetSizeMax( &cbSizeMax );
				nSize += static_cast<std::size_t>(cbSizeMax + sizeof(HeCID));
			}
			else
			{
				nSize += sizeof(HeCID);
			}
		}
		break;
		case HE_VT_UI1:
		case HE_VT_I1:
			nSize += sizeof(UInt8);
			break;
		case HE_VT_I2:
		case HE_VT_UI2:
		case HE_VT_BOOL:
			nSize += sizeof(UInt16);
			break;
		case HE_VT_I4:
		case HE_VT_UI4:
		case HE_VT_R4:
		case HE_VT_INT:
		case HE_VT_UINT:
		case HE_VT_ERROR:
			nSize += sizeof(UInt32);
			break;
		case HE_VT_I8:
		case HE_VT_UI8:
			nSize += sizeof(UInt64);
			break;
		case HE_VT_R8:
		case HE_VT_CY:
		case HE_VT_DATE:
			nSize += sizeof(double);
			break;
		default:
			break;
	}
	if ( nSize == sizeof(HEVARTYPE) )
	{
		HEVARTYPE	vtTemp = vt;
		HEBSTR		bstr   = NULL;
		CHeVariant	varBSTR;
		if ( vt != HE_VT_BSTR )
		{
			hr = HeVarChangeType( &varBSTR, this, 0, HE_VT_BSTR );
			if ( HE_SUCCEEDED(hr) )
			{
				bstr   = varBSTR.bstrVal;
				vtTemp = HE_VT_BSTR;
			}
		}
		else
		{
			bstr = bstrVal;
		}

		// Add the size of the length, the string itself and the NULL character
		if ( vtTemp == HE_VT_BSTR )
		{
			nSize += sizeof(UInt32) + HeStringByteLen( bstr ) + sizeof(wchar_t);
		}
	}
	return static_cast<UInt32>(nSize);
}


template<typename T>
class CHeVarTypeInfo
{
//	static const Helium::HEVARTYPE      vt;     // HEVARTYPE corresponding to type T
//	static T Helium::HEVARIANT::* const pField; // Pointer-to-member of corresponding field in HEVARIANT struct
};

template<>
class CHeVarTypeInfo<char>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I1;
	static char Helium::HEVARIANT::* const pField;
};
HE_SELECTANY char Helium::HEVARIANT::* const CHeVarTypeInfo<char>::pField = &Helium::HEVARIANT::cVal;

template<>
class CHeVarTypeInfo<UInt8>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI1;
	static UInt8 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt8 Helium::HEVARIANT::* const CHeVarTypeInfo<UInt8>::pField = &Helium::HEVARIANT::bVal;

template<>
class CHeVarTypeInfo<char*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I1 | Helium::HE_VT_BYREF;
	static char* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY char* Helium::HEVARIANT::* const CHeVarTypeInfo<char*>::pField = &Helium::HEVARIANT::pcVal;

template<>
class CHeVarTypeInfo<UInt8*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI1 | Helium::HE_VT_BYREF;
	static UInt8* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt8* Helium::HEVARIANT::* const CHeVarTypeInfo<UInt8*>::pField = &Helium::HEVARIANT::pbVal;

template<>
class CHeVarTypeInfo<SInt16>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I2;
	static SInt16 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY SInt16 Helium::HEVARIANT::* const CHeVarTypeInfo<SInt16>::pField = &Helium::HEVARIANT::iVal;

template<>
class CHeVarTypeInfo<SInt16*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I2 | Helium::HE_VT_BYREF;
	static SInt16* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY SInt16* Helium::HEVARIANT::* const CHeVarTypeInfo<SInt16*>::pField = &Helium::HEVARIANT::piVal;

template<>
class CHeVarTypeInfo<UInt16>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI2;
	static UInt16 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt16 Helium::HEVARIANT::* const CHeVarTypeInfo<UInt16>::pField = &Helium::HEVARIANT::uiVal;

template<>
class CHeVarTypeInfo<UInt16*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI2 | Helium::HE_VT_BYREF;
	static UInt16* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt16* Helium::HEVARIANT::* const CHeVarTypeInfo<UInt16*>::pField = &Helium::HEVARIANT::puiVal;

#ifndef OPT_64BITINTTYPES

template<>
class CHeVarTypeInfo<int>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_INT;
	static int Helium::HEVARIANT::* const pField;
};
HE_SELECTANY int Helium::HEVARIANT::* const CHeVarTypeInfo<int>::pField = &Helium::HEVARIANT::intVal;

template<>
class CHeVarTypeInfo<int*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_INT | Helium::HE_VT_BYREF;
	static int* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY int* Helium::HEVARIANT::* const CHeVarTypeInfo<int*>::pField = &Helium::HEVARIANT::pintVal;

template<>
class CHeVarTypeInfo<unsigned int>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI4;
	static unsigned int Helium::HEVARIANT::* const pField;
};
HE_SELECTANY unsigned int Helium::HEVARIANT::* const CHeVarTypeInfo<unsigned int>::pField = &Helium::HEVARIANT::uintVal;

template<>
class CHeVarTypeInfo<unsigned int*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI4 | Helium::HE_VT_BYREF;
	static unsigned int* Helium::HEVARIANT::* const	pField;
};
HE_SELECTANY unsigned int* Helium::HEVARIANT::* const CHeVarTypeInfo<unsigned int*>::pField = &Helium::HEVARIANT::puintVal;

#endif /* OPT_64BITINTTYPES */

template<>
class CHeVarTypeInfo<SInt32>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I4;
	static SInt32 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY SInt32 Helium::HEVARIANT::* const CHeVarTypeInfo<SInt32>::pField = &Helium::HEVARIANT::lVal;

template<>
class CHeVarTypeInfo<SInt32*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I4 | Helium::HE_VT_BYREF;
	static SInt32* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY SInt32* Helium::HEVARIANT::* const CHeVarTypeInfo<SInt32*>::pField = &Helium::HEVARIANT::plVal;

template<>
class CHeVarTypeInfo<UInt32>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI4;
	static UInt32 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt32 Helium::HEVARIANT::* const CHeVarTypeInfo<UInt32>::pField = &Helium::HEVARIANT::ulVal;

template<>
class CHeVarTypeInfo<UInt32*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI4 | Helium::HE_VT_BYREF;
	static UInt32* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt32* Helium::HEVARIANT::* const CHeVarTypeInfo<UInt32*>::pField = &Helium::HEVARIANT::pulVal;

template<>
class CHeVarTypeInfo<SInt64>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I8;
	static SInt64 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY SInt64 Helium::HEVARIANT::* const CHeVarTypeInfo<SInt64>::pField = &Helium::HEVARIANT::llVal;

template<>
class CHeVarTypeInfo<SInt64*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_I8 | Helium::HE_VT_BYREF;
	static SInt64* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY SInt64* Helium::HEVARIANT::* const CHeVarTypeInfo<SInt64*>::pField = &Helium::HEVARIANT::pllVal;

template<>
class CHeVarTypeInfo<UInt64>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI8;
	static UInt64 Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt64 Helium::HEVARIANT::* const CHeVarTypeInfo<UInt64>::pField = &Helium::HEVARIANT::ullVal;

template<>
class CHeVarTypeInfo<UInt64*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UI8 | Helium::HE_VT_BYREF;
	static UInt64* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY UInt64* Helium::HEVARIANT::* const CHeVarTypeInfo<UInt64*>::pField = &Helium::HEVARIANT::pullVal;

template<>
class CHeVarTypeInfo<float>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_R4;
	static float Helium::HEVARIANT::* const pField;
};
HE_SELECTANY float Helium::HEVARIANT::* const CHeVarTypeInfo<float>::pField = &Helium::HEVARIANT::fltVal;

template<>
class CHeVarTypeInfo<float*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_R4 | Helium::HE_VT_BYREF;
	static float* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY float* Helium::HEVARIANT::* const CHeVarTypeInfo<float*>::pField = &Helium::HEVARIANT::pfltVal;

template<>
class CHeVarTypeInfo<double>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_R8;
	static double Helium::HEVARIANT::* const pField;
};
HE_SELECTANY double Helium::HEVARIANT::* const CHeVarTypeInfo<double>::pField = &Helium::HEVARIANT::dblVal;

template<>
class CHeVarTypeInfo<double*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_R8 | Helium::HE_VT_BYREF;
	static double* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY double* Helium::HEVARIANT::* const CHeVarTypeInfo<double*>::pField = &Helium::HEVARIANT::pdblVal;

template<>
class CHeVarTypeInfo<Helium::HEVARIANT>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_VARIANT;
};

template<>
class CHeVarTypeInfo<Helium::HEBSTR>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_BSTR;
	static Helium::HEBSTR Helium::HEVARIANT::* const pField;
};
HE_SELECTANY Helium::HEBSTR Helium::HEVARIANT::* const CHeVarTypeInfo<Helium::HEBSTR>::pField = &Helium::HEVARIANT::bstrVal;

template<>
class CHeVarTypeInfo<Helium::HEBSTR*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_BSTR | Helium::HE_VT_BYREF;
	static Helium::HEBSTR* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY Helium::HEBSTR* Helium::HEVARIANT::* const CHeVarTypeInfo<Helium::HEBSTR*>::pField = &Helium::HEVARIANT::pbstrVal;

template<>
class CHeVarTypeInfo<Helium::IHeUnknown*>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UNKNOWN;
	static Helium::IHeUnknown* Helium::HEVARIANT::* const pField;
};
HE_SELECTANY Helium::IHeUnknown* Helium::HEVARIANT::* const CHeVarTypeInfo<Helium::IHeUnknown*>::pField = &Helium::HEVARIANT::punkVal;

template<>
class CHeVarTypeInfo<Helium::IHeUnknown**>
{
public:
	static const Helium::HEVARTYPE vt = Helium::HE_VT_UNKNOWN | Helium::HE_VT_BYREF;
	static Helium::IHeUnknown** Helium::HEVARIANT::* const pField;
};
HE_SELECTANY Helium::IHeUnknown** Helium::HEVARIANT::* const CHeVarTypeInfo<Helium::IHeUnknown**>::pField = &Helium::HEVARIANT::ppunkVal;


} /* namespace HeLib */

#endif /* HLHEVARIANT_H */
