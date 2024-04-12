// =============================================================================
//	objectwithsiteimpl.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef OBJECTWITHSITEIMPL_H
#define OBJECTWITHSITEIMPL_H

#include "hecom.h"
#include "hlptr.h"

namespace ObjectWithSiteUtil
{

inline HeResult
SetChildSite(
	Helium::IHeUnknown*	pUnkChild,
	Helium::IHeUnknown*	pUnkParent )
{
	using namespace Helium;
	using namespace HeLib;

	if ( pUnkChild == NULL ) return HE_E_NULL_POINTER;

	CHeQIPtr<IHeObjectWithSite>	spChildSite( pUnkChild );
	if ( spChildSite == NULL ) return HE_E_NO_INTERFACE;

	return spChildSite->SetSite( pUnkParent );
}

inline HeResult
SetSameSite(
	Helium::IHeUnknown*	pSource,
	Helium::IHeUnknown*	pDest )
{
	using namespace Helium;
	using namespace HeLib;

	if ( pSource == NULL ) return HE_E_NULL_POINTER;
	if ( pDest == NULL ) return HE_E_NULL_POINTER;

	CHeQIPtr<IHeObjectWithSite>	spSource( pSource );
	if ( spSource == NULL ) return HE_E_NO_INTERFACE;
	CHeQIPtr<IHeObjectWithSite>	spDest( pDest );
	if ( spDest == NULL ) return HE_E_NO_INTERFACE;

	CHePtr<IHeUnknown>	spSite;
	HeResult			hr = spSource->GetSite( &spSite.p );
	if ( HE_SUCCEEDED(hr) )
	{
		hr = spDest->SetSite( spSite );
	}

	return hr;
}

template <class Q>
inline HeResult
GetSite(
	Helium::IHeUnknown*	pObj,
	Q**					ppSite )
{
	using namespace Helium;
	using namespace HeLib;

	if ( ppSite == NULL ) return HE_E_NULL_POINTER;
	*ppSite = NULL;

	if ( pObj == NULL ) return HE_E_NULL_POINTER;

	CHeQIPtr<IHeObjectWithSite>	spObjWithSite( pObj );
	if ( spObjWithSite == NULL ) return HE_E_NO_INTERFACE;

	return spObjWithSite->GetSite( Q::GetIID(), reinterpret_cast<void**>(ppSite) );
}

} /* namespace ObjectWithSiteUtil */


template <class T>
class HE_NO_VTABLE IHeObjectWithSiteImpl :
	public Helium::IHeObjectWithSite
{
public:
				IHeObjectWithSiteImpl() {}
	virtual		~IHeObjectWithSiteImpl() {}

public:
	HeResult
	SetChildSite(
		Helium::IHeUnknown*	pUnkChild )
	{
		return ObjectWithSiteUtil::SetChildSite( pUnkChild, this );
	}

	// IHeObjectWithSite
public:
	HE_IMETHOD
	SetSite(
		Helium::IHeUnknown*		pUnkSite )
	{
		T*	pT = (T*) this;
		typename T::ObjectLock lock( pT );

		this->m_spUnkSite = pUnkSite;

		return HE_S_OK;
	}

	HE_IMETHOD
	GetSite(
		const Helium::HeIID&	iid,
		void**					ppvSite )
	{
		if ( ppvSite == NULL ) return HE_E_NULL_POINTER;

		T*	pT = (T*) this;
		typename T::ObjectLock lock( pT );

		if ( this->m_spUnkSite != NULL )
		{
			return this->m_spUnkSite->QueryInterface( iid, ppvSite );
		}

		return HE_E_FAIL;
	}

protected:
	HeLib::CHePtr<Helium::IHeUnknown>	m_spUnkSite;
};

#endif /* OBJECTWITHSITEIMPL_H */
