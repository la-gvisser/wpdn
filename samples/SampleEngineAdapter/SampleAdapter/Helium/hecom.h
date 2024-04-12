// =============================================================================
//	hecom.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef HECOM_H
#define HECOM_H

#include "heunk.h"
#include "hevariant.h"
#include <cstddef>

namespace Helium
{

/// \interface IHeDispatch
/// \brief Equivalent to IDispatch
#define HE_IHEDISPATCH_IID \
{ 0x00020400, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHeDispatch : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEDISPATCH_IID)

	HE_IMETHOD GetTypeInfoCount(/*out*/ UInt32* pctinfo) = 0;
	HE_IMETHOD GetTypeInfo(/*in*/ UInt32 iTInfo, /*in*/ UInt32 lcid, /*out*/ void** ppTInfo) = 0;
	HE_IMETHOD GetIDsOfNames(/*in*/ const HeIID& iid, /* [size_is]in */ const wchar_t* rgszNames,
		/*in*/ UInt32 cNames, /*in*/ UInt32 lcid, /* [size_is]out */ SInt32* rgDispId) = 0;
	HE_IMETHOD Invoke(/*in*/ SInt32 dispIdMember, /*in*/ const HeIID& iid,
		/*in*/ UInt32 lcid, /*in*/ UInt16 wFlags, /* outin */ void* pDispParams,
		/*out*/ HEVARIANT* pVarResult, /*out*/ void* pExcepInfo,
		/*out*/ UInt32* puArgErr) = 0;
};

/// \interface IHeFactory
/// \brief Equivalent to IClassFactory or nsIFactory
#define HE_IHEFACTORY_IID \
{ 0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHeFactory : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEFACTORY_IID);

	HE_IMETHOD CreateInstance(IHeUnknown* pOuter, const HeIID& iid, void** ppv) = 0;
	HE_IMETHOD LockFactory(SInt32 bLock) = 0;
};

/// \interface IHeMalloc
/// \brief Equivalent to IMalloc
#define HE_IHEMALLOC_IID \
{ 0x00000002, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHeMalloc : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEMALLOC_IID);

	HE_IMETHOD_(void*)			Alloc(std::size_t cb) = 0;
    HE_IMETHOD_(void*)			Realloc(void* pv, std::size_t cb) = 0;
    HE_IMETHOD_(void)			Free(void* pv) = 0;
    HE_IMETHOD_(std::size_t)	GetSize(void* pv) = 0;
    HE_IMETHOD_(int)			DidAlloc(void* pv) = 0;
    HE_IMETHOD_(void)			HeapMinimize() = 0;
};

#define HE_IHESEQUENTIALSTREAM_IID \
{ 0x0C733A30, 0x2A1C, 0x11CE, {0xAD, 0xE5, 0x00, 0xAA, 0x00, 0x44, 0x77, 0x3D} }

class HE_NO_VTABLE IHeSequentialStream : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHESEQUENTIALSTREAM_IID);

	HE_IMETHOD Read(void* pv, UInt32 cb, UInt32* pcbRead) = 0;
	HE_IMETHOD Write(const void* pv, UInt32 cb, UInt32* pcbWritten) = 0;
};

#define HE_IHESTREAM_IID \
{ 0x0000000C, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHeStream : public IHeSequentialStream
{
public:
	// TODO: check alignment with the Win32 STATSTG - beware ULARGE_INTEGER alignment
#include "hepushpack.h"
	struct HE_STATSTG
	{
		wchar_t*	pwcsName;
		UInt32		type;
		UInt64		cbSize;
		UInt64		mtime;
		UInt64		ctime;
		UInt64		atime;
		UInt32		grfMode;
		UInt32		grfLocksSupported;
		HeCID		clsid;
		UInt32		grfStateBits;
		UInt32		reserved;
	} HE_PACK_STRUCT;
#include "hepoppack.h"

	// Corresponds to STGM_ constants.
	enum HE_STGM
	{
		MODE_READ				= 0x00000000L,
		MODE_WRITE				= 0x00000001L,
		MODE_READWRITE			= 0x00000002L,
		MODE_SHARE_DENY_NONE	= 0x00000040L,
		MODE_SHARE_DENY_READ	= 0x00000030L,
		MODE_SHARE_DENY_WRITE	= 0x00000020L,
		MODE_SHARE_EXCLUSIVE	= 0x00000010L,
		MODE_CREATE				= 0x00001000L,
		MODE_DELETEONRELEASE	= 0x04000000L
	};

	enum HE_STGTY
	{
		STGTY_STORAGE		= 1,
		STGTY_STREAM		= 2,
		STGTY_LOCKBYTES		= 3,
		STGTY_PROPERTY		= 4
	};

	enum HE_STREAM_SEEK
	{
		STREAM_SEEK_SET		= 0,
		STREAM_SEEK_CUR		= 1,
		STREAM_SEEK_END		= 2
	};

	enum HE_LOCKTYPE
	{
		LOCK_WRITE			= 1,
		LOCK_EXCLUSIVE		= 2,
		LOCK_ONLYONCE		= 4
	};

    enum HE_STATFLAG
	{
		STATFLAG_DEFAULT	= 0,
		STATFLAG_NONAME		= 1,
		STATFLAG_NOOPEN		= 2
    };

public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHESTREAM_IID);

	HE_IMETHOD Seek(SInt64 move, UInt32 origin, UInt64* pNewPos) = 0;
	HE_IMETHOD SetSize(UInt64 cbNewSize) = 0;
	HE_IMETHOD CopyTo(IHeStream* pStream, UInt64 cb,
					UInt64* pcbRead, UInt64* pcbWritten) = 0;
	HE_IMETHOD Revert() = 0;
	HE_IMETHOD LockRegion(UInt64 offset, UInt64 cb, UInt32 lockType) = 0;
	HE_IMETHOD UnlockRegion(UInt64 offset, UInt64 cb, UInt32 lockType) = 0;
	HE_IMETHOD Stat(HE_STATSTG* pStat, UInt32 grfStatFlag) = 0;
	HE_IMETHOD Clone(IHeStream** ppStream) = 0;
};

#define HE_IHEPERSIST_IID \
{ 0x0000010C, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHePersist : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEPERSIST_IID);

	HE_IMETHOD GetClassID(HeCID* pCID) = 0;
};

#define HE_IHEPERSISTSTREAM_IID \
{ 0x00000109, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHePersistStream : public IHePersist
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEPERSISTSTREAM_IID);

	HE_IMETHOD IsDirty() = 0;
	HE_IMETHOD Load(IHeStream* pStream) = 0;
	HE_IMETHOD Save(IHeStream* pStream, int bClearDirty) = 0;
	HE_IMETHOD GetSizeMax(UInt64* pcbSize) = 0;
};

#define HE_IHEPERSISTFILE_IID \
{ 0x0000010B, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46} }

class HE_NO_VTABLE IHePersistFile : public IHePersist
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEPERSISTFILE_IID);

	HE_IMETHOD IsDirty() = 0;
	HE_IMETHOD Load(const wchar_t* pszFileName, UInt32 mode) = 0;
	HE_IMETHOD Save(const wchar_t* pszFileName, int bRemember) = 0;
	HE_IMETHOD SaveCompleted(const wchar_t* pszFileName) = 0;
	HE_IMETHOD GetCurFile(wchar_t** ppszFileName) = 0;
};

#define HE_IHEPERSISTXML_IID \
{ 0x83FE0429, 0xCBE9, 0x47A2, {0x98, 0xE5, 0x4B, 0x85, 0x84, 0x64, 0xAF, 0x9C} }

class HE_NO_VTABLE IHePersistXml : public IHePersist
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEPERSISTXML_IID);

	HE_IMETHOD Load(IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode) = 0;
	HE_IMETHOD Save(IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode, int bClearDirty) = 0;
};

#define	IHEPROPERTYBAG_IID \
{ 0x55272A00, 0x42CB, 0x11CE, {0x81, 0x35, 0x00, 0xAA, 0x00, 0x4B, 0xB8, 0x51} }

class HE_NO_VTABLE IHePropertyBag :	public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IHEPROPERTYBAG_IID);

	HE_IMETHOD Read( const wchar_t* pszPropName, HEVARIANT* pVar, IHeUnknown* pErrorLog ) = 0;
	HE_IMETHOD Write( const wchar_t* pszPropName, HEVARIANT* pVar ) = 0;
};

#define	IHEPROPERTYBAG2_IID	\
{ 0x22F55882, 0x280B, 0x11d0, {0xA8, 0xA9, 0x00, 0xA0, 0xC9, 0x0C, 0x20, 0x04} }

class HE_NO_VTABLE IHePropertyBag2 : public	IHeUnknown
{
public:

	enum PROPBAG2_TYPE
	{
		PROPBAG2_TYPE_UNDEFINED,
		PROPBAG2_TYPE_DATA,			// Value is	simple data
		PROPBAG2_TYPE_URL,			// Value is	a URL reference
		PROPBAG2_TYPE_OBJECT,		// Value is	an object
		PROPBAG2_TYPE_STREAM,		// Value is	a stream
		PROPBAG2_TYPE_STORAGE,		// Value is	a storage
		PROPBAG2_TYPE_MONIKER		// Value is	a moniker
	};

#include "hepushpack.h"
	struct PROPBAG2
	{
		UInt32			dwType;		// Property	type (from PROPBAG2_TYPE)
		UInt16			vt;			// VARIANT property	type
		UInt16			cfType;		// Clipboard format	(aka MIME-type)
		UInt32			dwHint;		// Property	name hint
		wchar_t*		pstrName;	// Property	name
		Helium::HeCID	clsid;		// CLSID (for HEPROPBAG2_TYPE_OBJECT)
	} HE_PACK_STRUCT;
#include "hepoppack.h"

	HE_IMETHOD	Read( UInt32 cProperties, PROPBAG2* pPropBag,
					Helium::IHeUnknown* pErrLog, HEVARIANT* pvarValue,
					HeResult* phrError ) = 0;
	HE_IMETHOD	Write( UInt32 cProperties, PROPBAG2* pPropBag,
					HEVARIANT* pvarValue ) = 0;
	HE_IMETHOD	CountProperties( UInt32* pcProperties ) = 0;
	HE_IMETHOD	GetPropertyInfo( UInt32 iProperty, UInt32 cProperties,
					PROPBAG2* pPropBag, UInt32* pcProperties ) = 0;
	HE_IMETHOD	LoadObject( const wchar_t* pstrName, UInt32 dwHint,
					IHeUnknown*	pUnkObject, IHeUnknown* pErrLog ) = 0;
};

#define HE_IHEPERSISTPROPERTYBAG_IID \
{ 0x37D84F60, 0x42CB, 0x11CE, {0x81, 0x35, 0x00, 0xAA, 0x00, 0x4B, 0xB8, 0x51} }

class HE_NO_VTABLE IHePersistPropertyBag : public IHePersist
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEPERSISTPROPERTYBAG_IID);

	HE_IMETHOD	InitNew() = 0;
	HE_IMETHOD	Load(IHePropertyBag* pPropBag, IHeUnknown* pErrLog) = 0;
	HE_IMETHOD	Save(IHePropertyBag* pPropBag, int bClearDirty, int bSaveAllProperties) = 0;
};

#define HE_IHEPERSISTPROPERTYBAG2_IID \
{ 0x22F55881, 0x280B, 0x11d0, {0xA8, 0xA9, 0x00, 0xA0, 0xC9, 0x0C, 0x20, 0x04} }

class HE_NO_VTABLE IHePersistPropertyBag2 : public IHePersist
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEPERSISTPROPERTYBAG2_IID);

	HE_IMETHOD	InitNew() = 0;
	HE_IMETHOD	Load(IHePropertyBag2* pPropBag, IHeUnknown* pErrLog) = 0;
	HE_IMETHOD	Save(IHePropertyBag2* pPropBag, int bClearDirty, int bSaveAllProperties) = 0;
	HE_IMETHOD	IsDirty() = 0;
};

#define HE_IHEOBJECTWITHSITE_IID \
{ 0xFC4801A3, 0x2BA9, 0x11CF, {0xA2, 0x29, 0x00, 0xAA, 0x00, 0x3D, 0x73, 0x52} }

class HE_NO_VTABLE IHeObjectWithSite : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHEOBJECTWITHSITE_IID);

	HE_IMETHOD	SetSite(IHeUnknown* pUnkSite) = 0;
	HE_IMETHOD	GetSite(const HeIID& iid, void** ppvSite) = 0;

	template <class Q>
	HE_IMETHODIMP GetSite(Q** pp)
	{
		return GetSite(Q::GetIID(), (void**) pp);
	}
};

#define HE_IHESERVICEPROVIDER_IID \
{ 0x6D5140C1, 0x7436, 0x11CE, {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA} }

class HE_NO_VTABLE IHeServiceProvider : public IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(HE_IHESERVICEPROVIDER_IID);

	HE_IMETHOD	QueryService(const HeID& sid, const HeIID& iid, void** ppvObject) = 0;

	template <class Q>
	HE_IMETHODIMP QueryService(const HeID& sid, Q** pp)
	{
		return QueryService(sid, Q::GetIID(), (void**) pp);
	}
};

} /* namespace Helium */

#endif /* HECOM_H */
