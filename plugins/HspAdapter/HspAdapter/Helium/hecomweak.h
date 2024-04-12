// =============================================================================
//	hecomweak.h
// =============================================================================
//	Copyright (c) 2005-2009 WildPackets, Inc. All rights reserved.

#ifndef HECOMWEAK_H
#define HECOMWEAK_H

#include "hecom.h"
#include "heunk.h"
#include "hevariant.h"

namespace Helium
{

class HE_NO_VTABLE IHeSequentialStreamWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD Read(void* pv, UInt32 cb, UInt32* pcbRead) = 0;
	HE_IMETHOD Write(const void* pv, UInt32 cb, UInt32* pcbWritten) = 0;
};

class HE_NO_VTABLE IHeStreamWeak : public IHeSequentialStreamWeak
{
public:
	HE_IMETHOD Seek(SInt64 move, UInt32 origin, UInt64* pNewPos) = 0;
	HE_IMETHOD SetSize(UInt64 cbNewSize) = 0;
	HE_IMETHOD CopyTo(IHeStream* pStream, UInt64 cb,
					UInt64* pcbRead, UInt64* pcbWritten) = 0;
	HE_IMETHOD Revert() = 0;
	HE_IMETHOD LockRegion(UInt64 offset, UInt64 cb, UInt32 lockType) = 0;
	HE_IMETHOD UnlockRegion(UInt64 offset, UInt64 cb, UInt32 lockType) = 0;
	HE_IMETHOD Stat(IHeStream::HE_STATSTG* pStat, UInt32 grfStatFlag) = 0;
	HE_IMETHOD Clone(IHeStream** ppStream) = 0;
};

class HE_NO_VTABLE IHePersistWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD GetClassID(HeCID* pCID) = 0;
};

class HE_NO_VTABLE IHePersistStreamWeak : public IHePersistWeak
{
public:
	HE_IMETHOD IsDirty() = 0;
	HE_IMETHOD Load(IHeStream* pStream) = 0;
	HE_IMETHOD Save(IHeStream* pStream, int bClearDirty) = 0;
	HE_IMETHOD GetSizeMax(UInt64* pcbSize) = 0;
};

class HE_NO_VTABLE IHePersistFileWeak : public IHePersistWeak
{
public:
	HE_IMETHOD IsDirty() = 0;
	HE_IMETHOD Load(const wchar_t* pszFileName, UInt32 mode) = 0;
	HE_IMETHOD Save(const wchar_t* pszFileName, int bRemember) = 0;
	HE_IMETHOD SaveCompleted(const wchar_t* pszFileName) = 0;
	HE_IMETHOD GetCurFile(wchar_t** ppszFileName) = 0;
};

class HE_NO_VTABLE IHePersistXmlWeak : public IHePersistWeak
{
public:
	HE_IMETHOD Load(IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode) = 0;
	HE_IMETHOD Save(IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode, int bClearDirty) = 0;
};

class HE_NO_VTABLE IHePropertyBagWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD Read(const wchar_t* pszPropName, HEVARIANT* pVar, IHeUnknown* pErrorLog) = 0;
	HE_IMETHOD Write(const wchar_t* pszPropName, HEVARIANT* pVar) = 0;
};

class HE_NO_VTABLE IHePropertyBag2Weak : public IHeUnknownWeak
{
public:
	HE_IMETHOD Read(UInt32 cProperties, IHePropertyBag2::PROPBAG2* pPropBag,
					IHeUnknown* pErrLog, HEVARIANT* pvarValue,
					HeResult* phrError) = 0;
	HE_IMETHOD Write(UInt32 cProperties, IHePropertyBag2::PROPBAG2* pPropBag,
					HEVARIANT* pvarValue) = 0;
	HE_IMETHOD CountProperties(UInt32* pcProperties) = 0;
	HE_IMETHOD GetPropertyInfo(UInt32 iProperty, UInt32 cProperties,
					IHePropertyBag2::PROPBAG2* pPropBag, UInt32* pcProperties) = 0;
	HE_IMETHOD LoadObject(const wchar_t* pstrName, UInt32 dwHint,
					IHeUnknown*	pUnkObject, IHeUnknown* pErrLog) = 0;
};

class HE_NO_VTABLE IHePersistPropertyBagWeak : public IHePersistWeak
{
public:
	HE_IMETHOD InitNew() = 0;
	HE_IMETHOD Load(IHePropertyBag* pPropBag, IHeUnknown* pErrLog) = 0;
	HE_IMETHOD Save(IHePropertyBag* pPropBag, int bClearDirty, int bSaveAllProperties) = 0;
};

class HE_NO_VTABLE IHePersistPropertyBag2Weak : public IHePersistWeak
{
public:
	HE_IMETHOD InitNew() = 0;
	HE_IMETHOD Load(IHePropertyBag2* pPropBag, IHeUnknown* pErrLog) = 0;
	HE_IMETHOD Save(IHePropertyBag2* pPropBag, int bClearDirty, int bSaveAllProperties) = 0;
	HE_IMETHOD IsDirty() = 0;
};

class HE_NO_VTABLE IHeObjectWithSiteWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD SetSite(IHeUnknown* pUnkSite) = 0;
	HE_IMETHOD GetSite(const HeIID& iid, void** ppvSite) = 0;
};

class HE_NO_VTABLE IHeServiceProviderWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD QueryService(const HeID& sid, const HeIID& iid, void** ppvObject) = 0;
};

class IHeConnectionPointContainerWeak;
class IHeEnumConnectionPointsWeak;
class IHeConnectionPointWeak;
class IHeEnumConnectionsWeak;

class HE_NO_VTABLE IHeConnectionPointContainerWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD EnumConnectionPoints(IHeEnumConnectionPoints** ppEnum) = 0;
	HE_IMETHOD FindConnectionPoint(const HeIID& iid, IHeConnectionPoint** ppCP) = 0;
};

class HE_NO_VTABLE IHeEnumConnectionPointsWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD Next(UInt32 cConnections, IHeConnectionPoint** ppCP, UInt32* pcFetched) = 0;
	HE_IMETHOD Skip(UInt32 cConnections) = 0;
	HE_IMETHOD Reset() = 0;
	HE_IMETHOD Clone(IHeEnumConnectionPoints** ppEnum) = 0;
};

class HE_NO_VTABLE IHeConnectionPointWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD GetConnectionInterface(HeID* pIID) = 0;
	HE_IMETHOD GetConnectionPointContainer(IHeConnectionPointContainer** ppCPC) = 0;
	HE_IMETHOD Advise(IHeUnknown* pUnkSink, UInt32* pCookie) = 0;
	HE_IMETHOD Unadvise(UInt32 cookie) = 0;
	HE_IMETHOD EnumConnections(IHeEnumConnections** ppEnum) = 0;
};

class HE_NO_VTABLE IHeEnumConnectionsWeak : public IHeUnknownWeak
{
public:
	HE_IMETHOD Next(UInt32 cConnections, IHeEnumConnections::CONNECTDATA* pConnectData, UInt32* pcFetched) = 0;
	HE_IMETHOD Skip(UInt32 cConnections) = 0;
	HE_IMETHOD Reset() = 0;
	HE_IMETHOD Clone(IHeEnumConnections** ppEnum) = 0;
};

} /* namespace Helium */

#endif /* HECOMWEAK_H */
