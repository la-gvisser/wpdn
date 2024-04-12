// =============================================================================
//	AdapterManager.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "stdafx.h"
#include "ObjectWithSiteImpl.h"


// =============================================================================
//		CAdapterManager
// =============================================================================

class HE_NO_VTABLE CAdapterManager : 
	public CHeObjRoot<CHeMultiThreadModel>,
	public CHeClass<CAdapterManager>,	
	public IHeObjectWithSiteImpl<CAdapterManager>
{
public:
	HE_INTERFACE_MAP_BEGIN(CAdapterManager)		
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

protected:		
	CHePtr<IHeServiceProvider>	m_spServices;
	CHePtr<IAdapterManager>		m_spAdapterManager;

	CHePtr<IAdapter>			m_spAdapter;
	CHePtr<IHeUnknown>			m_spUnkSite;

protected:		
	HeResult AddAdapter( IAdapter* pAdapter );

public:
	;			CAdapterManager() {}
	virtual		~CAdapterManager() {}

	HeResult	FinalConstruct() { return HE_S_OK; }
	void		FinalRelease() {}

	HeResult	LoadSettings();

	// IObjectWithSiteImpl override
public:
	HE_IMETHOD	SetSite( IHeUnknown *pUnkSite );
};
