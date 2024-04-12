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
	if ( pp == nullptr ) return nullptr;

	if ( p != nullptr )
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
	if ( pp == nullptr ) return nullptr;

	Helium::IHeUnknown*	pTemp = *pp;

	*pp = nullptr;

	if ( p != nullptr )
	{
		p->QueryInterface( iid, reinterpret_cast<void**>(pp) );
	}

	if ( pTemp != nullptr )
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

	CHePtr() throw() : p( nullptr )
	{
	}

	CHePtr( T* pT ) throw() : p( pT )
	{
		if ( p != nullptr )
		{
			p->AddRef();
		}
	}

	CHePtr( const CHePtr<T>& lp ) throw() : p( lp.p )
	{
		if ( p != nullptr )
		{
			p->AddRef();
		}
	}

#ifdef HE_HAS_RVALUE_REFS
	CHePtr( CHePtr<T>&& lp ) throw()
	{
		p = lp.p;
		lp.p = nullptr;
	}
#endif /* HE_HAS_RVALUE_REFS */

	~CHePtr() throw()
	{
		if ( p != nullptr )
		{
			p->Release();
		}
	}

	T* operator=( T* pT ) throw()
	{
		if ( pT != nullptr )
		{
			pT->AddRef();
		}

		if ( p != nullptr )
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
		if ( p != lp.p )
		{
			if ( p != nullptr )
			{
				p->Release();
			}

			p = lp.p;
			lp.p = nullptr;
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
		return (p == nullptr);
	}

	bool operator<( T* pT ) const throw()
	{
		return p < pT;
	}

	bool operator==( T* pT ) const throw()
	{
		return p == pT;
	}

	bool operator!=( T* pT ) const throw()
	{
		return !operator==( pT );
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
		if ( pT != nullptr )
		{
			p = nullptr;
			pT->Release();
		}
	}

	bool IsEqualObject( Helium::IHeUnknown* pOther ) throw()
	{
		if ( (p == nullptr) && (pOther == nullptr) )
		{
			return true;	// They are both nullptr objects.
		}

		if ( (p == nullptr) || (pOther == nullptr) )
		{
			return false;	// One is nullptr the other is not.
		}

		CHePtr<Helium::IHeUnknown>	pUnk1;
		CHePtr<Helium::IHeUnknown>	pUnk2;
		p->QueryInterface( Helium::IHeUnknown::GetIID(), reinterpret_cast<void**>(&pUnk1) );
		pOther->QueryInterface( Helium::IHeUnknown::GetIID(), reinterpret_cast<void**>(&pUnk2) );
		return (pUnk1.p == pUnk2.p);
	}

	// Attach to an existing interface (does not AddRef).
	void Attach( T* pT ) throw()
	{
		if ( p != nullptr )
		{
			p->Release();
		}
		p = pT;
	}

	// Detach the interface (does not Release).
	T* Detach() throw()
	{
		T* pT = p;
		p = nullptr;
		return pT;
	}

	HeResult CopyTo( T** ppT ) throw()
	{
		if ( ppT == nullptr ) return HE_E_NULL_POINTER;
		*ppT = p;
		if ( p != nullptr )
		{
			p->AddRef();
		}
		return HE_S_OK;
	}

	template <class Q>
	HeResult QueryInterface( Q** ppQ ) const throw()
	{
		if ( p == nullptr ) return HE_E_NULL_POINTER;
		return p->QueryInterface( Q::GetIID(), reinterpret_cast<void**>(ppQ) );
	}

	template <class Q>
	HeResult QueryInterface( CHePtr<Q>* pspQ ) const throw()
	{
		if ( p == nullptr ) return HE_E_NULL_POINTER;
		return p->QueryInterface( Q::GetIID(), reinterpret_cast<void**>(&(pspQ->p)) );
	}

	HeResult CreateInstance( const Helium::HeCID& cid ) throw()
	{
		return HeCreateInstance( cid, T::GetIID(), reinterpret_cast<void**>(&p) );
	}

	HeResult CreateInstance( const char* pszProgID ) throw()
	{
		Helium::HeCID	cid;
		HeResult		hr = HeCIDFromProgID( pszProgID, &cid );
		if ( HE_SUCCEEDED(hr) )
		{
			hr = CreateInstance( cid );
		}
		return hr;
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
		if ( pUnk != nullptr )
		{
			pUnk->QueryInterface( T::GetIID(), reinterpret_cast<void**>(&(this->p)) );
		}
	}

	template <class Q>
	CHeQIPtr( const CHePtr<Q>& lp ) throw() : CHePtr<T>()
	{
		if ( lp.p != nullptr )
		{
			lp->QueryInterface( T::GetIID(), reinterpret_cast<void**>(&(this->p)) );
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
		if ( pUnk != nullptr )
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
		if ( p != pUnk )
		{
			return HePtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), pUnk );
		}
		return *this;
	}

	Helium::IHeUnknown* operator=( const CHeQIPtr<Helium::IHeUnknown>& t ) throw()
	{
		if ( p != t.p )
		{
			return HePtrAssign( reinterpret_cast<Helium::IHeUnknown**>(&(this->p)), t.p );
		}
		return *this;
	}
};

} /* namespace HeLib */

#endif /* HLPTR_H */
