// =============================================================================
//	hlcom.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef HLCOM_H
#define HLCOM_H

#include "hlbase.h"

namespace HeLib
{

#ifdef __IUnknown_INTERFACE_DEFINED__

/// \method He2COM
/// \brief Convert IHeUnknown* to IUnknown*
inline IUnknown* He2COM( Helium::IHeUnknown* pUnk )
{
	return reinterpret_cast<IUnknown*>(pUnk);
}

/// \method COM2He
/// \brief Convert IUnknown* to IHeUnknown*
inline Helium::IHeUnknown* COM2He( IUnknown* pUnk )
{
	return reinterpret_cast<Helium::IHeUnknown*>(pUnk);
}

#endif /* __IUnknown_INTERFACE_DEFINED__ */

#ifdef GUID_DEFINED

/// \method GUID2HeID
/// \brief Convert COM GUID to HeID
inline const Helium::HeID& GUID2HeID(const GUID& guid)
{
	return reinterpret_cast<const Helium::HeID&>(guid);
}

/// \method HeID2GUID
/// \brief Convert HeID to COM GUID
inline const GUID& HeID2GUID(const Helium::HeID& id)
{
	return reinterpret_cast<const GUID&>(id);
}

/// \method IID2HeIID
/// \brief Convert COM IID to HeIID
inline const Helium::HeIID& IID2HeIID(const IID& iid)
{
	return reinterpret_cast<const Helium::HeIID&>(iid);
}

/// \method HeIID2IID
/// \brief Convert HeIID to COM IID
inline const IID& HeIID2IID(const Helium::HeIID& iid)
{
	return reinterpret_cast<const IID&>(iid);
}

/// \method CLSID2HeCID
/// \brief Convert COM CLSID to CID
inline const Helium::HeCID& CLSID2HeCID(const CLSID& clsid)
{
	return reinterpret_cast<const Helium::HeCID&>(clsid);
}

/// \method HeCID2CLSID
/// \brief Convert COM CLSID to CID
inline const CLSID& HeCID2CLSID(const Helium::HeCID& cid)
{
	return reinterpret_cast<const CLSID&>(cid);
}

#endif /* GUID_DEFINED */

} /* namespace HeLib */

#endif /* HLCOM_H */
