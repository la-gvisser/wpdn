// =============================================================================
//	objectwithsiteimpl.h
// =============================================================================
//	Copyright (c) 2005-2011 WildPackets, Inc. All rights reserved.

#ifndef OBJECTWITHSITEIMPL_H
#define OBJECTWITHSITEIMPL_H

#include "hecom.h"
#include "hlptr.h"

template <class T>
class HE_NO_VTABLE IHeObjectWithSiteImpl :
	public Helium::IHeObjectWithSite
{
public:
				IHeObjectWithSiteImpl() {}
	virtual		~IHeObjectWithSiteImpl() {}

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
		if ( ppvSite == nullptr ) return HE_E_NULL_POINTER;

		T*	pT = (T*) this;
		typename T::ObjectLock lock( pT );

		if ( this->m_spUnkSite.get() != nullptr )
		{
			return this->m_spUnkSite->QueryInterface( iid, ppvSite );
		}

		return HE_E_FAIL;
	}

protected:
	HeLib::CHePtr<Helium::IHeUnknown>	m_spUnkSite;
};

#endif /* OBJECTWITHSITEIMPL_H */
