// =============================================================================
//	hlweakref.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef HLWEAKREF_H
#define HLWEAKREF_H

#include "heweakref.h"
#include "hlbase.h"

namespace HeLib
{

class IHeGetWeakReferenceImpl :
	public Helium::IHeGetWeakReference
{
public:
	IHeGetWeakReferenceImpl() : m_pProxy( nullptr ) {}

	virtual ~IHeGetWeakReferenceImpl()
	{
		ClearWeakReferences();
	}

	// IHeGetWeakReference
public:
	HE_IMETHOD GetWeakReference(Helium::IHeWeakReference** pp)
	{
		if ( pp == nullptr ) return HE_E_NULL_POINTER;

		if ( m_pProxy == nullptr )
		{
			try
			{
				m_pProxy = new HeLib::CHeObjNoLock<WeakReference>();
				m_pProxy->m_pReferent = this;
			}
			catch ( std::bad_alloc& )
			{
				return HE_E_OUT_OF_MEMORY;
			}
		}

		*pp = m_pProxy;
		(*pp)->AddRef();

		return HE_S_OK;
	}

public:
	void ClearWeakReferences()
	{
		if ( m_pProxy != nullptr )
		{
			m_pProxy->OnReferentDestruction();
			m_pProxy = nullptr;
		}
	}

	bool HasWeakReferences() const
	{
		return (m_pProxy != nullptr);
	}

private:
	void OnProxyDestruction()
	{
		m_pProxy = nullptr;
	}

private:
	class HE_NO_VTABLE WeakReference :
		public CHeObjRoot<CHeMultiThreadModelNoCS>,
		public Helium::IHeWeakReference
	{
		friend class IHeGetWeakReferenceImpl;
	public:
		HE_INTERFACE_MAP_BEGIN(WeakReference)
			HE_INTERFACE_ENTRY_IID(HE_IWEAKREFERENCE_IID,Helium::IHeWeakReference)
		HE_INTERFACE_MAP_END()

		WeakReference() : m_pReferent( nullptr ) {}

		virtual ~WeakReference()
		{
			if ( m_pReferent != nullptr )
			{
				m_pReferent->OnProxyDestruction();
			}
		}

		void OnReferentDestruction()
		{
			m_pReferent = nullptr;
		}

		// IHeWeakReference
	public:
		HE_IMETHOD QueryReferent(const Helium::HeIID& iid, void** ppv)
		{
			if ( m_pReferent == nullptr )
			{
				if ( ppv != nullptr ) *ppv = nullptr;
				return HE_E_NULL_POINTER;
			}
			return m_pReferent->QueryInterface( iid, ppv );
		}

	private:
		IHeGetWeakReferenceImpl*	m_pReferent;
	};

private:
	WeakReference*	m_pProxy;
};


} /* namespace HeLib */

#endif /* HLWEAKREF_H */
