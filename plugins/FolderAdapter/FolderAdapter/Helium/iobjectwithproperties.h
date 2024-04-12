// =============================================================================
//	iobjectwithproperties.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef	IOBJECTWITHPROPERTIES_H
#define	IOBJECTWITHPROPERTIES_H

#include "heunk.h"

#define IOBJECTWITHPROPERTIES_IID \
{ 0x544BAF0A, 0xB673, 0x4DF8, {0xBD, 0x51, 0xB5, 0x7B, 0x3C, 0xFA, 0x3B, 0x55} }

/// \interface IObjectWithProperties
class HE_NO_VTABLE IObjectWithProperties : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IOBJECTWITHPROPERTIES_IID);

	/// Get a property.
	/// \param pszPropName The name of the property being requested. This
	/// argument cannot be NULL.
	/// \param HEVARIANT that receives the property value.
	/// The function sets both type and value fields in the HEVARIANT before
	/// returning. If the caller initialized the pVar->vt field on entry,
	/// an attempt is made to change its corresponding value into this type. If
	/// the caller sets pVar->vt to HE_VT_EMPTY, the returned type is whatever is
	/// convenient. The caller is responsible for freeing any allocations
	/// contained in pVar. This argument cannot be NULL.
	/// \retval HE_S_OK The property was read successfully.
	/// \retval HE_E_POINTER A required argument was not valid (NULL).
	/// \retval HE_E_INVALID_ARG The property does not exist.
	/// \retval HE_E_FAIL The property was not successfully read.
	HE_IMETHOD GetProperty(/*in*/ const wchar_t* pszPropName, /*in, out*/ Helium::HEVARIANT* pVar) = 0;

	/// Set a property.
	/// \param pszPropName The name of the property to set. This argument cannot
	/// be NULL.
	/// \param HEVARIANT that contains the property value. The caller owns this
	/// this HEVARIANT and is responsible for its allocations.
	/// This argument cannot be NULL.
	/// \retval HE_S_OK The property was set successfully.
	/// \retval HE_E_POINTER A required argument was not valid (NULL).
	/// \retval HE_E_INVALIDARG The property does not exist.
	/// \retval HE_E_FAIL The property was not successfully set.
	HE_IMETHOD SetProperty(/*in*/ const wchar_t* pszPropName, /*in*/ const Helium::HEVARIANT* pVar) = 0;
};

class HE_NO_VTABLE IObjectWithPropertiesWeak : public Helium::IHeUnknownWeak
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IOBJECTWITHPROPERTIES_IID);

	HE_IMETHOD GetProperty(/*in*/ const wchar_t* pszPropName, /*in, out*/ Helium::HEVARIANT* pVar) = 0;
	HE_IMETHOD SetProperty(/*in*/ const wchar_t* pszPropName, /*in*/ const Helium::HEVARIANT* pVar) = 0;
};


#define IOBJECTWITHPROPERTIES2_IID \
{ 0x22DD49AF, 0x9CB8, 0x42E9, {0x9D, 0x82, 0xAE, 0xA9, 0x83, 0x2F, 0x07, 0x58} }

/// \interface IObjectWithProperties2
class HE_NO_VTABLE IObjectWithProperties2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IOBJECTWITHPROPERTIES2_IID);

	struct ObjPropParam
	{
		const wchar_t*		pszPropName;	///< Property name.
		Helium::HEVARTYPE	vt;				///< HEVARIANT property type
	};

	/// Get one or more properties.
	/// \param cProperties Number of properties to get. This argument specifies
	/// the number of elements in the arrays pParams and pVars.
	/// \param pParams Array of ObjPropParam structures that specify the properties
	/// being requested. The pstrName and vt members of these structures must be
	/// filled in before calling this method. There must be at least cProperties
	/// elements in this array. This argument cannot be NULL.
	/// \param Array of HEVARIANT structures that receive the property values.
	/// The caller does not need to initialize these structures before calling
	/// IObjectWithProperties2::GetProperties. The IObjectWithProperties2::GetProperties
	/// method will fill both type and value fields in these structures before
	/// returning. There must be at least cProperties elements in this array.
	/// The caller is responsible for freeing any allocations that are contained
	/// in these structures. This argument cannot be NULL.
	/// \retval HE_S_OK All of the properties were successfully read.
	/// \retval HE_E_POINTER A required argument was not valid (NULL).
	/// \retval HE_E_FAIL One or more of the properties were not successfully read.
	HE_IMETHOD GetProperties(/*in*/ UInt32 cProperties,
		/*in*/ const ObjPropParam params[], /*out*/ Helium::HEVARIANT vars[]) = 0;

	/// Sets one or more properties.
	/// \param cProperties Number of properties to set. This argument specifies
	/// the number of elements in the arrays pParams and pVars.
	/// \param pParams Array of ObjPropParam structures that specify the properties
	/// being set. The pstrName member of these structures must be
	/// filled in before calling this method. There must be at least cProperties
	/// elements in this array. This argument cannot be NULL.
	/// \param Array of VARIANT structures that contain the property values to be set.
	/// There must be at least cProperties elements in this array. This argument
	/// cannot be NULL.
	/// \retval HE_S_OK All of the properties were successfully set.
	/// \retval HE_E_INVALID_POINTER A required argument was not valid (NULL).
	/// \retval HE_E_FAIL One or more of the properties could not be set.
	HE_IMETHOD SetProperties(/*in*/ UInt32 cProperties,
		/*in*/ const ObjPropParam params[], /*in*/ const Helium::HEVARIANT vars[]) = 0;
};

class HE_NO_VTABLE IObjectWithProperties2Weak : public Helium::IHeUnknownWeak
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IOBJECTWITHPROPERTIES2_IID);

	HE_IMETHOD GetProperties(/*in*/ UInt32 cProperties,
		/*in*/ const IObjectWithProperties2::ObjPropParam params[],
		/*out*/ Helium::HEVARIANT vars[]) = 0;
	HE_IMETHOD SetProperties(/*in*/ UInt32 cProperties,
		/*in*/ const IObjectWithProperties2::ObjPropParam params[],
		/*in*/ const Helium::HEVARIANT vars[]) = 0;
};

#endif
