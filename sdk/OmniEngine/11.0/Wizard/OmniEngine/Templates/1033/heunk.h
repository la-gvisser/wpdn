// =============================================================================
//	heunk.h
// =============================================================================
//	Copyright (c) 2005-2017 Savvius, Inc. All rights reserved.

#ifndef HEUNK_H
#define HEUNK_H

#include "hecore.h"
#include "heid.h"
#include "hestdint.h"

namespace Helium
{

/// \def HE_IHEUNKNOWN_IID
/// \brief Interface ID for IHeUnknown (same as IID_Unknown).
#define HE_IHEUNKNOWN_IID \
{ 0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

/// \interface IHeUnknown
/// \brief Equivalent to IUnknown or nsISupports
class HE_NO_VTABLE IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEUNKNOWN_IID);

	/// Returns a pointer to a specified interface on an object to which a
	/// client currently holds an interface pointer. This function must call
	/// AddRef() on the pointer it returns.
	/// \param iid [in] Identifier of the interface being requested. 
	/// \param ppv [out] Address of pointer variable that receives the interface
	/// pointer requested in riid. Upon successful return, *ppv contains the
	/// requested interface pointer to the object. If the object does not
	/// support the interface specified in iid, *ppv is set to NULL.
	/// \retval HE_S_OK if the interface is supported, HE_E_NO_INTERFACE if not.
	HE_IMETHOD QueryInterface(/*in*/ const HeIID& iid, /*out*/ void** ppv) = 0;

	/// Increments the reference count for an interface on an object. It should
	/// be called for every new copy of a pointer to an interface on a given
	/// object.
	/// \retval Returns an integer from 1 to n, the value of the new reference
	/// count. This information is meant to be used for diagnostic/testing
	/// purposes only, because, in certain situations, the value may be
	/// unstable.
	HE_IMETHOD_(UInt32) AddRef() = 0;
	
	/// Decrements the reference count for the calling interface on a object.
	/// If the reference count on the object falls to 0, the object is freed
	/// from memory.
	/// \retval Returns the resulting value of the reference count, which is
	/// used for diagnostic/testing purposes only.
	HE_IMETHOD_(UInt32) Release() = 0;

	template<class Q>
	HE_IMETHODIMP QueryInterface(Q** pp)
	{
		return QueryInterface(Q::GetIID(), reinterpret_cast<void**>(pp));
	}
};

/// \interface IHeUnknownWeak
/// \brief "Weak" version of IHeUnkown
class HE_NO_VTABLE IHeUnknownWeak
{
public:
	HE_IMETHOD			WeakQueryInterface(const HeIID& iid, void** ppv) = 0;
	HE_IMETHOD_(UInt32)	WeakAddRef() = 0;
	HE_IMETHOD_(UInt32)	WeakRelease() = 0;
};

} /* namespace Helium */

#endif /* HEUNK_H */
