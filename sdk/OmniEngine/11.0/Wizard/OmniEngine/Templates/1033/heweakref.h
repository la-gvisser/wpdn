// =============================================================================
//	heweakref.h
// =============================================================================
//	Copyright (c) 2005-2017 Savvius, Inc. All rights reserved.

#ifndef HEWEAKREF_H
#define HEWEAKREF_H

#include "heunk.h"

namespace Helium
{

/// \def HE_IWEAKREFERENCE_IID
/// \brief Interface ID for IHeWeakReference.
#define HE_IWEAKREFERENCE_IID \
{ 0x9188BC85, 0xF92E, 0x11D2, {0x81, 0xEF, 0x00, 0x60, 0x08, 0x3A, 0x0B, 0xCF} }

/// \interface IHeWeakReference
/// \brief Equivalent to nsIWeakReference
class HE_NO_VTABLE IHeWeakReference : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IWEAKREFERENCE_IID);

	/// Queries the referent, if it exists.
	/// \param iid [in] Identifier of the interface being requested. 
	/// \param ppv [out] Address of pointer variable that receives the interface
	/// pointer requested in riid. Upon successful return, *ppv contains the
	/// requested interface pointer to the object. If the object does not
	/// support the interface specified in iid, *ppv is set to NULL.
	/// \retval HE_S_OK If the interface is supported.
	/// \retval HE_E_NO_INTERFACE If the interface is not supported.
	/// \retval HE_E_NULL_POINTER The object no longer exists.
	HE_IMETHOD QueryReferent(/*in*/ const HeIID& iid, /*out*/ void** ppv) = 0;

	template<class Q>
	HE_IMETHODIMP QueryReferent(Q** pp)
	{
		return QueryReferent(Q::GetIID(), reinterpret_cast<void**>(pp));
	}
};

/// \def HE_IGETWEAKREFERENCE_IID
/// \brief Interface ID for IHeGetWeakReference.
#define HE_IGETWEAKREFERENCE_IID \
{ 0x9188BC86, 0xF92E, 0x11D2, {0x81, 0xEF, 0x00, 0x60, 0x08, 0x3A, 0x0B, 0xCF} }

/// \interface IHeGetWeakReference
/// \brief Equivalent to nsISupportsWeakReference
class HE_NO_VTABLE IHeGetWeakReference : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IGETWEAKREFERENCE_IID);

	/// Returns an instance of IHeWeakReference.
	/// \param pp [out] Address of pointer variable that receives the interface
	/// pointer.
	/// \retval HE_S_OK The method was successful.
	/// \retval HE_E_FAIL An unspecified error occurred.
	HE_IMETHOD GetWeakReference(/*out*/ IHeWeakReference** pp) = 0;
};

class HE_NO_VTABLE IHeGetWeakReferenceWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD GetWeakReference(/*out*/ IHeWeakReference** pp) = 0;
};

} /* namespace Helium */

#endif /* HEWEAKREF_H */
