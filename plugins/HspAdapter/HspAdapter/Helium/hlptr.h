// =============================================================================
//	hlptr.h
// =============================================================================
//	Copyright (c) 2005-2012 WildPackets, Inc. All rights reserved.

#ifndef HLPTR_H
#define HLPTR_H

#include "heunk.h"
#include "hlruntime.h"
#include "hlstrconv.h"

namespace HeLib
{

inline Helium::IHeUnknown*
HePtrAssign(
	Helium::IHeUnknown**	pp,
	Helium::IHeUnknown*		p )
{
	if ( pp == NULL ) return NULL;
		
	if ( p != NULL )
	{
		p->AddRef();
	}

	if ( *pp )
	{
		(*pp)->Release();
	}

	*pp = p;
	
	return p;
}

inline Helium::IHeUnknown*
HeQIPtrAssign(
	Helium::IHeUnknown**	pp,
	Helium::IHeUnknown*		p,
	const Helium::HeIID&	iid )
{
	if ( pp == NULL ) return NULL;

	Helium::IHeUnknown*	pTemp = *pp;

	*pp = NULL;

	if ( p != NULL )
	{
		p->QueryInterface( iid, reinterpret_cast<void**>(pp) );
	}

	if ( pTemp != NULL )
	{
		pTemp->Release();
	}

	return *pp;
}

template <class T>
class _NoAddRefReleaseOnCHePtr : public T
{
private:
	HE_IMETHOD_(UInt32) AddRef() = 0;
	HE_IMETHOD_(UInt32) Release() = 0;
};

/// \class CHePtr
/// \brief Helium smart pointer. Like CComPtr.
template <class T>
class CHePtr
{
public:
	T*	p;

	CHePtr() throw() : p( NULL )
	{
	}

	CHePtr( T* pT ) throw() : p( pT )
	{
		if ( p != NULL )
		{
			p->AddRef();
		}
	}

	CHePtr( const CHePtr<T>& lp ) throw() : p( lp.p )
	{
		if ( p != NULL )
		{
			p->AddRef();
		}
	}

#ifdef HE_HAS_RVALUE_REFS
	CHePtr( CHePtr<T>&& lp ) throw()
	{
		p = lp.p;
		lp.p = NULL;
	}
#endif /* HE_HAS_RVALUE_REFS */

	~CHePtr() throw()
	{
		if ( p != NULL )
		{
			p->Release();
		}
	}

	T* operator=( T* pT ) throw()
	{
		if ( pT != NULL )
		{
			pT->AddRef();
		}

		if ( p != NULL )
		{
			p->Release();
		}

		p = pT;

		return pT;
	}

	T* operator=( const CHePtr<T>& lp ) throw()
	{
		return operator=( lp.p );
	}

#ifdef HE_HAS_RVALUE_REFS
	T* operator=( CHePtr<T>&& lp ) throw()
	{
		if ( *this != lp )
		{
			if ( p != NULL )
			{
				p->Release();
			}

			p = lp.p;
			lp.p = NULL;
		}
		return p;
	}
#endif /* HE_HAS_RVALUE_REFS */

	template <typename Q>
	T* operator=( const CHePtr<Q>& q ) throw()
	{
		return static_cast<T*>(HeQIPtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&p), q, T::GetIID() ));
	}

	operator T*() const throw()
	{
		return p;
	}

	T& operator*() const throw()
	{
		return *p;
	}

	// No! Never overload operator&
	// See, e.g., http://www.artima.com/cppsource/YourAddress.html
//	T** operator&() throw()
//	{
//		return &p;
//	}

	_NoAddRefReleaseOnCHePtr<T>* operator->() const throw()
	{
		return reinterpret_cast<_NoAddRefReleaseOnCHePtr<T>*>(p);
	}

	bool operator!() const throw()
	{
		return (p == NULL);
	}

	bool operator<( T* pT ) const throw()
	{
		return p < pT;
	}

	bool operator==( T* pT ) const throw()
	{
		return p == pT;
	}

	T* get() const throw()
	{
		return p;
	}

	T** get_address_of() throw()
	{
		return &p;
	}

	void Release() throw()
	{
		T*	pT = p;
		if ( pT != NULL )
		{
			p = NULL;
			pT->Release();
		}
	}

	bool IsEqualObject( Helium::IHeUnknown* pOther ) throw()
	{
		if ( (p == NULL) && (pOther == NULL) )
		{
			return true;	// They are both NULL objects.
		}

		if ( (p == NULL) || (pOther == NULL) )
		{
			return false;	// One is NULL the other is not.
		}

		CHePtr<Helium::IHeUnknown>	pUnk1;
		CHePtr<Helium::IHeUnknown>	pUnk2;
		p->QueryInterface( Helium::IHeUnknown::GetIID(), reinterpret_cast<void**>(&pUnk1) );
		pOther->QueryInterface( Helium::IHeUnknown::GetIID(), reinterpret_cast<void**>(&pUnk2) );
		return (pUnk1 == pUnk2);
	}

	// Attach to an existing interface (does not AddRef).
	void Attach( T* pT ) throw()
	{
		if ( p != NULL )
		{
			p->Release();
		}
		p = pT;
	}

	// Detach the interface (does not Release).
	T* Detach() throw()
	{
		T* pT = p;
		p = NULL;
		return pT;
	}

	HeResult CopyTo( T** ppT ) throw()
	{
		if ( ppT == NULL ) return HE_E_NULL_POINTER;
		*ppT = p;
		if ( p != NULL )
		{
			p->AddRef();
		}
		return HE_S_OK;
	}

	template <class Q>
	HeResult QueryInterface( Q** ppQ ) const throw()
	{
		if ( p == NULL ) return HE_E_NULL_POINTER;
		return p->QueryInterface( Q::GetIID(), reinterpret_cast<void**>(ppQ) );
	}

	template <class Q>
	HeResult QueryInterface( CHePtr<Q>* pspQ ) const throw()
	{
		if ( p == NULL ) return HE_E_NULL_POINTER;
		return p->QueryInterface( Q::GetIID(), reinterpret_cast<void**>(&(pspQ->p)) );
	}

	HeResult CreateInstance( const Helium::HeCID& cid ) throw()
	{
#if defined(HE_COM_RUNTIME) && defined(HE_WIN32)
		return ::CoCreateInstance( reinterpret_cast<const CLSID&>(cid), NULL,
			CLSCTX_ALL, reinterpret_cast<const IID&>(T::GetIID()),
			reinterpret_cast<void**>(&p) );
#else
		return HeCreateInstance( cid, T::GetIID(), reinterpret_cast<void**>(&p) );
#endif
	}

	HeResult CreateInstance( const char* pszProgID ) throw()
	{
#if defined(HE_COM_RUNTIME) && defined(HE_WIN32)
		CLSID	clsid;
		HRESULT hr = CLSIDFromProgID( HeLib::CA2W(pszProgID), &clsid );
		if ( SUCCEEDED(hr) )
		{
			hr = ::CoCreateInstance( clsid, NULL,
				CLSCTX_ALL, reinterpret_cast<const IID&>(T::GetIID()),
				reinterpret_cast<void**>(&p) );
		}
		return (HeResult) hr;
#else
		Helium::HeCID	cid;
		HeResult		hr = HeCIDFromProgID( pszProgID, &cid );
		if ( HE_SUCCEEDED(hr) )
		{
			hr = CreateInstance( cid );
		}

		return hr;
#endif
	}
};

/// \class CHeQIPtr
/// \brief Helium smart pointer. Like CComQIPtr.
template <class T>
class CHeQIPtr : public CHePtr<T>
{
public:
	CHeQIPtr() throw()
	{
	}

	CHeQIPtr( T* pT ) throw() :
		CHePtr<T>( pT )
	{
	}

	CHeQIPtr( const CHeQIPtr<T>& t ) throw() :
		CHePtr<T>( t.p )
	{
	}

	CHeQIPtr( Helium::IHeUnknown* pUnk ) throw()
	{
		if ( pUnk != NULL )
		{
			pUnk->QueryInterface( T::GetIID(), reinterpret_cast<void**>(&(this->p)) );
		}
	}

	T* operator=( T* pT ) throw()
	{
		return static_cast<T*>(HePtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), pT ));
	}

	T* operator=( const CHeQIPtr<T>& t ) throw()
	{
		return static_cast<T*>(HePtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), t.p ));
	}

	T* operator=( Helium::IHeUnknown* pUnk ) throw()
	{
		return static_cast<T*>(HeQIPtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), pUnk, T::GetIID() ));
	}
};


// Specialization of CHeQIPtr<> for Helium::IHeUnknown to avoid duplicate
// copy constructor and operator=.
template<>
class CHeQIPtr<Helium::IHeUnknown> :
	public CHePtr<Helium::IHeUnknown>
{
public:
	CHeQIPtr() throw()
	{
	}
	
	CHeQIPtr( Helium::IHeUnknown* pUnk ) throw()
	{
		if ( pUnk != NULL )
		{
			pUnk->QueryInterface( Helium::IHeUnknown::GetIID(),
				reinterpret_cast<void**>(&(this->p)) );
		}
	}

	CHeQIPtr( const CHeQIPtr<Helium::IHeUnknown>& t ) throw() :
		CHePtr<Helium::IHeUnknown>( t )
	{
	}

	Helium::IHeUnknown* operator=( Helium::IHeUnknown* pUnk ) throw()
	{
		if ( *this != pUnk )
		{
			return HePtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), pUnk );
		}
		return *this;
	}

	Helium::IHeUnknown* operator=( const CHeQIPtr<Helium::IHeUnknown>& t ) throw()
	{
		if ( *this != t )
		{
			return HePtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), t.p );
		}
		return *this;
	}
};

} /* namespace HeLib */

#endif /* HLPTR_H */
