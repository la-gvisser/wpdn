// =============================================================================
//	hlbase.h
// =============================================================================
//	Copyright (c) 2005-2013 WildPackets, Inc. All rights reserved.

#ifndef HLBASE_H
#define HLBASE_H

#include "hecom.h"
#include "hecore.h"
#include "heid.h"
#include "hlatomic.h"
#include "hldef.h"
#include "hlexcept.h"
#include "hlsync.h"
#include <climits>
#ifdef HE_WIN32
#include <cstddef>
#else
#include <stdint.h>
#endif

namespace HeLib
{

/// \class CHeSingleThreadModel
/// \brief Corresponds to ATL::CComSingleThreadModel
class CHeSingleThreadModel
{
public:
	typedef CHeFakeCriticalSection AutoCriticalSection;
	typedef CHeFakeCriticalSection CriticalSection;

	static UInt32 Increment(SInt32* p)
	{
		return static_cast<UInt32>(++(*p));
	}
	static UInt32 Decrement(SInt32* p)
	{
		return static_cast<UInt32>(--(*p));
	}
};

/// \class CHeMultiThreadModel
/// \brief Corresponds to ATL::CComMultiThreadModel
class CHeMultiThreadModel
{
public:
	typedef CHeAutoCriticalSection AutoCriticalSection;
	typedef CHeCriticalSection CriticalSection;

	static UInt32 Increment(SInt32* p)
	{
		return static_cast<UInt32>(HeAtomicIncrement32( p ));
	}
	static UInt32 Decrement(SInt32* p)
	{
		return static_cast<UInt32>(HeAtomicDecrement32( p ));
	}
};

/// \class CHeMultiThreadModelNoCS
/// \brief Corresponds to ATL::CComMultiThreadModelNoCS
class CHeMultiThreadModelNoCS
{
public:
	typedef CHeFakeCriticalSection AutoCriticalSection;
	typedef CHeFakeCriticalSection CriticalSection;

	static UInt32 Increment(SInt32* p)
	{
		return static_cast<UInt32>(HeAtomicIncrement32( p ));
	}
	static UInt32 Decrement(SInt32* p)
	{
		return static_cast<UInt32>(HeAtomicDecrement32( p ));
	}
};

/// \class CHeGlobalsThreadModel
/// \brief Corresponds to ATL::CComGlobalsThreadModel
typedef CHeMultiThreadModel CHeGlobalsThreadModel;

#include "hepushpack.h"
struct HE_MODULEVERSION
{
	UInt32		nMajorVersion;
	UInt32		nMinorVersion;
	UInt32		nRevisionNumber;
	UInt32		nBuildNumber;
	wchar_t		szPublisher[256];
	wchar_t		szProductName[256];
} HE_PACK_STRUCT;
#include "hepoppack.h"

typedef HeResult (HE_CREATORFUNC)(void* pv, const Helium::HeIID& iid, void** ppv);
typedef HeResult (HE_GETCLASSOBJECTFUNC)(const Helium::HeCID& cid,
										const Helium::HeIID& iid, void** ppv);
typedef HeResult (HE_CANUNLOADNOWFUNC)(void);
typedef HeResult (HE_GERVERSIONFUNC)(HE_MODULEVERSION* pVersion);

#define HE_SYM_GETCLASSOBJECT	"HeModuleGetClassObject"
#define HE_SYM_CANUNLOADNOW		"HeModuleCanUnloadNow"
#define HE_SYM_GETVERSION		"HeModuleGetVersion"

struct _HE_INTERFACE_MAP_ENTRY
{
	Helium::HeIID	iid;
	uintptr_t		offset;
};

#define HE_INTERFACE_MAP_BEGIN(x) \
public: \
	typedef x _InterfaceMapClass; \
	Helium::IHeUnknown* GetUnknown() throw() \
	{ return reinterpret_cast<Helium::IHeUnknown*>(reinterpret_cast<uintptr_t>(this) +_GetEntries()->offset); } \
	HeResult _InternalQueryInterface(const Helium::HeIID& iid, void** ppv) \
	{ return this->InternalQueryInterface( this, _GetEntries(), iid, ppv ); } \
	static const HeLib::_HE_INTERFACE_MAP_ENTRY* _GetEntries() { \
	static const HeLib::_HE_INTERFACE_MAP_ENTRY s_Entries[] = {

#define HE_INTERFACE_ENTRY_IID(iid, x) \
	{iid, \
	reinterpret_cast<uintptr_t>(static_cast<x*>( \
		reinterpret_cast<_InterfaceMapClass*>(8)))-8},

#define HE_INTERFACE_ENTRY2_IID(iid, x, x2) \
	{iid, \
	reinterpret_cast<uintptr_t>(reinterpret_cast<x*>(static_cast<x2*>( \
		reinterpret_cast<_InterfaceMapClass*>(8))))-8},

#define HE_INTERFACE_MAP_END() \
	{HE_NULL_ID, static_cast<uintptr_t>(-1)} }; return s_Entries; }

#define HE_WEAK_INTERFACE_MAP_BEGIN(x) \
public: \
	Helium::IHeUnknown* GetUnknownWeak() throw() \
	{ return reinterpret_cast<Helium::IHeUnknown*>(reinterpret_cast<uintptr_t>(this) + _GetEntriesWeak()->offset); } \
	HeResult _WeakInternalQueryInterface(const Helium::HeIID& iid, void** ppv) \
	{ return InternalQueryInterface( this, _GetEntriesWeak(), iid, ppv ); } \
	static const HeLib::_HE_INTERFACE_MAP_ENTRY* _GetEntriesWeak() { \
	static const HeLib::_HE_INTERFACE_MAP_ENTRY s_Entries[] = {

#define HE_WEAK_INTERFACE_MAP_END() \
	{HE_NULL_ID, static_cast<uintptr_t>(-1)} }; return s_Entries; } \
	HE_IMETHOD WeakQueryInterface(const Helium::HeIID&, void**) = 0; \
	HE_IMETHOD_(UInt32) WeakAddRef() = 0; \
	HE_IMETHOD_(UInt32) WeakRelease() = 0;

inline HeResult
HeInternalQueryInterface(
	void*							pThis,
	const _HE_INTERFACE_MAP_ENTRY*	pEntries,
	const Helium::HeIID&			iid,
	void**							ppv )
{
	HE_ASSERT( pThis != NULL );
	HE_ASSERT( pEntries != NULL );
	HE_ASSERT( ppv != NULL );
	if ( (pThis == NULL) || (pEntries == NULL) || (ppv == NULL) )
	{
		return HE_E_NULL_POINTER;
	}

	*ppv = NULL;

	const _HE_INTERFACE_MAP_ENTRY*	pEntry = pEntries;
	while ( pEntry->offset != static_cast<uintptr_t>(-1) )
	{
		if ( pEntry->iid == iid )
		{
			Helium::IHeUnknown*	pUnk = reinterpret_cast<Helium::IHeUnknown*>(
									reinterpret_cast<uintptr_t>(pThis) + pEntry->offset);
			pUnk->AddRef();
			*ppv = pUnk;
			return HE_S_OK;
		}
		++pEntry;
	}

	if ( (iid == Helium::IHeUnknown::GetIID()) && 
		(pEntries->offset != static_cast<uintptr_t>(-1)) )
	{
		// Use the first interface.
		Helium::IHeUnknown*	pUnk = reinterpret_cast<Helium::IHeUnknown*>(
								reinterpret_cast<uintptr_t>(pThis) + pEntries->offset);
		pUnk->AddRef();
		*ppv = pUnk;
		return HE_S_OK;
	}

	return HE_E_NO_INTERFACE;
}

/// \class CHeModule
/// \brief Corresponds to CAtlModule
class CHeModule
{
public:
	CHeModule() :
		m_bInitialized( false ),
		m_nLockCount( 0 )
	{
		m_bInitialized = true;
	}

	virtual ~CHeModule() throw()
	{
		m_bInitialized = false;
	}

	virtual SInt32 Lock()
	{
		return static_cast<SInt32>(CHeGlobalsThreadModel::Increment( &m_nLockCount ));
	}

	virtual SInt32 Unlock()
	{
		return static_cast<SInt32>(CHeGlobalsThreadModel::Decrement( &m_nLockCount ));
	}

	virtual SInt32 GetLockCount() const
	{
		return m_nLockCount;
	}

	HeResult CanUnloadNow() throw()
	{
		return (GetLockCount() == 0) ? HE_S_OK : HE_S_FALSE;
	}

protected:
	bool	m_bInitialized;
	SInt32	m_nLockCount;
};

extern CHeModule* _pModule;

template <class Base>
class CHeObject : public Base
{
public:
	typedef Base _BaseClass;

	CHeObject(void* = 0) throw()
	{
		_pModule->Lock();
	}

	virtual ~CHeObject() throw()
	{
		// Set refcount to -(INT_MAX / 2) to protect destruction and 
		// also catch mismatched Release() in debug builds.
		this->m_cRef = -(INT_MAX / 2);
		this->FinalRelease();
		_pModule->Unlock();
	}

	HE_IMETHOD QueryInterface(const Helium::HeIID& iid, void** ppv)
	{
		return this->_InternalQueryInterface( iid, ppv );
	}

	HE_IMETHOD_(UInt32) AddRef()
	{
		return this->InternalAddRef();
	}

	HE_IMETHOD_(UInt32) Release()
	{
		const UInt32	cRef = this->InternalRelease();
		if ( cRef == 0 )
		{
			delete this;
		}

		return cRef;
	}

	static HeResult CreateInstance( CHeObject<Base>** ppv ) throw()
	{
		if ( ppv == NULL ) return HE_E_NULL_POINTER;

		HeResult	hr = HE_E_OUT_OF_MEMORY;

		try
		{
			*ppv = NULL;

			CHeObject<Base>*	p = new CHeObject<Base>();
			if ( p != NULL )
			{
				p->SetVoid( NULL );
				p->InternalAddRef();
				hr = p->FinalConstruct();
				p->InternalRelease();
				if ( hr != HE_S_OK )
				{
					delete p;
					p = NULL;
				}
				else
				{
					*ppv = p;
				}
			}
		}
		HE_CATCH(hr)

		return hr;
	}
};

template <class Base>
class CHeObjectSI : public Base
{
public:
	typedef Base _BaseClass;

	CHeObjectSI(void* = NULL) throw()
	{
		_pModule->Lock();
	}

	virtual ~CHeObjectSI() throw()
	{
		// Set refcount to -(INT_MAX / 2) to protect destruction and 
		// also catch mismatched Release() in debug builds.
		this->m_cRef = -(INT_MAX / 2);
		_pModule->Unlock();
	}

	void BeginDestroy()
	{
		// Set refcount to -(INT_MAX / 2) to protect destruction and 
		// also catch mismatched Release() in debug builds.
		this->m_cRef = -(INT_MAX / 2);
		this->FinalRelease();
		this->m_cRef = 0;
	}

	HE_IMETHOD QueryInterface(const Helium::HeIID& iid, void** ppv)
	{
		return this->_InternalQueryInterface( iid, ppv );
	}

	HE_IMETHOD_(UInt32) AddRef()
	{
		return this->InternalAddRef();
	}

	HE_IMETHOD_(UInt32) Release()
	{
		const UInt32	cRef = this->InternalRelease();
		if ( cRef == 0 )
		{
			WeakAddRef();
			BeginDestroy();
			WeakRelease();
		}

		return cRef;
	}

	HE_IMETHOD WeakQueryInterface(const Helium::HeIID& iid, void** ppv) 
	{
		return this->_WeakInternalQueryInterface( iid, ppv );
	}

	HE_IMETHOD_(UInt32) WeakAddRef()
	{
		return this->InternalWeakAddRef();
	}

	HE_IMETHOD_(UInt32) WeakRelease()
	{
		const UInt32	cRef = this->InternalWeakRelease();
		if ( (cRef == 0) && (this->m_cRef == 0) )
		{
			delete this;
		}

		return cRef;
	}

	static HeResult CreateInstance( CHeObjectSI<Base>** ppv ) throw()
	{
		if ( ppv == NULL ) return HE_E_NULL_POINTER;

		HeResult	hr = HE_E_OUT_OF_MEMORY;

		try
		{
			*ppv = NULL;

			CHeObjectSI<Base>*	p = new CHeObjectSI<Base>();
			if ( p != NULL )
			{
				p->SetVoid( NULL );
				p->InternalAddRef();
				hr = p->FinalConstruct();
				p->InternalRelease();
				if ( hr != HE_S_OK )
				{
					delete p;
					p = NULL;
				}
				else
				{
					*ppv = p;
				}
			}
		}
		HE_CATCH(hr)

		return hr;
	}
};

template <class Base>
class CHeObjNoLock : public Base
{
public:
	typedef Base _BaseClass;

	CHeObjNoLock() {}

	virtual ~CHeObjNoLock() throw()
	{
		// Set refcount to -(INT_MAX / 2) to protect destruction and 
		// also catch mismatched Release() in debug builds.
		this->m_cRef = -(INT_MAX / 2);
		this->FinalRelease();
	}

	HE_IMETHOD QueryInterface(const Helium::HeIID& iid, void** ppv)
	{
		return this->_InternalQueryInterface( iid, ppv );
	}

	HE_IMETHOD_(UInt32) AddRef()
	{
		return this->InternalAddRef();
	}

	HE_IMETHOD_(UInt32) Release()
	{
		const UInt32	cRef = this->InternalRelease();
		if ( cRef == 0 )
		{
			delete this;
		}

		return cRef;
	}

	static HeResult CreateInstance( CHeObjNoLock<Base>** ppv ) throw()
	{
		if ( ppv == NULL ) return HE_E_NULL_POINTER;

		HeResult	hr = HE_E_OUT_OF_MEMORY;

		try
		{
			*ppv = NULL;

			CHeObjNoLock<Base>*	p = new CHeObjNoLock<Base>();
			if ( p != NULL )
			{
				p->SetVoid( NULL );
				p->InternalAddRef();
				hr = p->FinalConstruct();
				p->InternalRelease();
				if ( hr != HE_S_OK )
				{
					delete p;
					p = NULL;
				}
				else
				{
					*ppv = p;
				}
			}
		}
		HE_CATCH(hr)

		return hr;
	}
};

template <class Base>
class CHeObjStack : public Base
{
public:
	typedef Base _BaseClass;

	CHeObjStack(void* = NULL)
	{
		m_hrFinalConstruct = this->FinalConstruct();
	}

	virtual ~CHeObjStack() throw()
	{
		this->FinalRelease();
	}

	HE_IMETHOD QueryInterface(const Helium::HeIID& iid, void** ppv)
	{
		return this->_InternalQueryInterface( iid, ppv );
	}

	HE_IMETHOD_(UInt32) AddRef()
	{
		return 0;
	}

	HE_IMETHOD_(UInt32) Release()
	{
		return 0;
	}

	HeResult	m_hrFinalConstruct;
};

class CHeObjRootBase
{
public:
	SInt32	m_cRef;

	CHeObjRootBase() : m_cRef( 0 ) {}

	HeResult FinalConstruct()
	{
		return HE_S_OK;
	}

	void FinalRelease() {}

	static HeResult InternalQueryInterface(void* pThis,
		const _HE_INTERFACE_MAP_ENTRY* pEntries,
		const Helium::HeIID& iid, void** ppv)
	{
		HE_ASSERT( pThis != NULL );
		HE_ASSERT( ppv != NULL );
		return HeInternalQueryInterface( pThis, pEntries, iid, ppv );
	}

	void SetVoid(void*) {}
};

template <class T>
class CHeObjLockT
{
public:
	CHeObjLockT( const T* p ) : m_p( p )
	{
		if ( m_p != NULL )
		{
			m_p->Lock();
		}
	}

	~CHeObjLockT() throw()
	{
		if ( m_p != NULL )
		{
			m_p->Unlock();
		}
	}

public:
	const T*	m_p;
};

template <class T>
class CHeObjUnlockT
{
public:
	CHeObjUnlockT( const T* p ) : m_p( p )
	{
		if ( m_p != NULL )
		{
			m_p->Unlock();
		}
	}

	~CHeObjUnlockT() throw()
	{
		if ( m_p != NULL )
		{
			m_p->Lock();
		}
	}

public:
	const T*	m_p;
};

template <class ThreadModel>
class CHeObjRoot : public CHeObjRootBase
{
public:
	typedef ThreadModel _ThreadModel;
	typedef typename _ThreadModel::AutoCriticalSection _CritSec;
	typedef CHeObjLockT< CHeObjRoot<_ThreadModel> > ObjectLock;
	typedef CHeObjUnlockT< CHeObjRoot<_ThreadModel> > ObjectUnlock;

public:
	UInt32 InternalAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cRef );
	}

	UInt32 InternalRelease()
	{
#ifdef _DEBUG
		const UInt32	cRef = _ThreadModel::Decrement( &m_cRef );
		if ( static_cast<SInt32>(cRef) < -(INT_MAX / 2) )
		{
			HE_ASSERT( 0 && "Release called on a pointer that's already been released\n" );
		}
		return cRef;
#else
		return _ThreadModel::Decrement( &m_cRef );
#endif
	}

	void Lock() const { m_cs.Lock(); }
	void Unlock() const { m_cs.Unlock(); }

private:
	mutable _CritSec	m_cs;
};

template <>
class CHeObjRoot<CHeSingleThreadModel> : public CHeObjRootBase
{
public:
	typedef CHeSingleThreadModel _ThreadModel;
	typedef _ThreadModel::AutoCriticalSection _CritSec;
	typedef CHeObjLockT< CHeObjRoot<_ThreadModel> > ObjectLock;
	typedef CHeObjUnlockT< CHeObjRoot<_ThreadModel> > ObjectUnlock;

public:
	UInt32 InternalAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cRef );
	}

	UInt32 InternalRelease()
	{
#ifdef _DEBUG
		const UInt32	cRef = _ThreadModel::Decrement( &m_cRef );
		if ( static_cast<SInt32>(cRef) < -(INT_MAX / 2) )
		{
			HE_ASSERT( 0 && "Release called on a pointer that's already been released\n" );
		}
		return cRef;
#else
		return _ThreadModel::Decrement( &m_cRef );
#endif
	}

	void Lock() const {}
	void Unlock() const {}
};

template <>
class CHeObjLockT< CHeObjRoot<CHeSingleThreadModel> >
{
public:
	CHeObjLockT( const CHeObjRoot<CHeSingleThreadModel>* ) {}
	~CHeObjLockT() throw() {}
};

template <>
class CHeObjUnlockT< CHeObjRoot<CHeSingleThreadModel> >
{
public:
	CHeObjUnlockT( const CHeObjRoot<CHeSingleThreadModel>* ) {}
	~CHeObjUnlockT() throw() {}
};

template <>
class CHeObjRoot<CHeMultiThreadModelNoCS> : public CHeObjRootBase
{
public:
	typedef CHeMultiThreadModelNoCS _ThreadModel;
	typedef _ThreadModel::AutoCriticalSection _CritSec;
	typedef CHeObjLockT< CHeObjRoot<_ThreadModel> > ObjectLock;
	typedef CHeObjUnlockT< CHeObjRoot<_ThreadModel> > ObjectUnlock;

public:
	UInt32 InternalAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cRef );
	}

	UInt32 InternalRelease()
	{
#ifdef _DEBUG
		const UInt32	cRef = _ThreadModel::Decrement( &m_cRef );
		if ( static_cast<SInt32>(cRef) < -(INT_MAX / 2) )
		{
			HE_ASSERT( 0 && "Release called on a pointer that's already been released\n" );
		}
		return cRef;
#else
		return _ThreadModel::Decrement( &m_cRef );
#endif
	}

	void Lock() const {}
	void Unlock() const {}
};

template <>
class CHeObjLockT< CHeObjRoot<CHeMultiThreadModelNoCS> >
{
public:
	CHeObjLockT( const CHeObjRoot<CHeMultiThreadModelNoCS>* ) {}
	~CHeObjLockT() throw() {}
};

template <>
class CHeObjUnlockT< CHeObjRoot<CHeMultiThreadModelNoCS> >
{
public:
	CHeObjUnlockT( const CHeObjRoot<CHeMultiThreadModelNoCS>* ) {}
	~CHeObjUnlockT() throw() {}
};

template <class ThreadModel>
class CHeObjRootSI : public CHeObjRootBase
{
public:
	typedef ThreadModel _ThreadModel;
	typedef typename _ThreadModel::AutoCriticalSection _CritSec;
	typedef CHeObjLockT< CHeObjRootSI<_ThreadModel> > ObjectLock;
	typedef CHeObjUnlockT< CHeObjRootSI<_ThreadModel> > ObjectUnlock;

public:
	SInt32	m_cWeakRef;

	CHeObjRootSI() : m_cWeakRef( 0 ) {}

	UInt32 InternalAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cRef );
	}

	UInt32 InternalRelease()
	{
#ifdef _DEBUG
		const UInt32	cRef = _ThreadModel::Decrement( &m_cRef );
		if ( static_cast<SInt32>(cRef) < -(INT_MAX / 2) )
		{
			HE_ASSERT( 0 && "Release called on a pointer that's already been released\n" );
		}
		return cRef;
#else
		return _ThreadModel::Decrement( &m_cRef );
#endif
	}

	UInt32 InternalWeakAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cWeakRef );
	}

	UInt32 InternalWeakRelease()
	{
		return _ThreadModel::Decrement( &m_cWeakRef );
	}

	void Lock() const { m_cs.Lock(); }
	void Unlock() const { m_cs.Unlock(); }

private:
	mutable _CritSec	m_cs;
};

template <>
class CHeObjRootSI<CHeSingleThreadModel> : public CHeObjRootBase
{
public:
	typedef CHeSingleThreadModel _ThreadModel;
	typedef _ThreadModel::AutoCriticalSection _CritSec;
	typedef CHeObjLockT< CHeObjRootSI<_ThreadModel> > ObjectLock;
	typedef CHeObjUnlockT< CHeObjRootSI<_ThreadModel> > ObjectUnlock;

public:
	UInt32 InternalAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cRef );
	}

	UInt32 InternalRelease()
	{
#ifdef _DEBUG
		const UInt32	cRef = _ThreadModel::Decrement( &m_cRef );
		if ( static_cast<SInt32>(cRef) < -(INT_MAX / 2) )
		{
			HE_ASSERT( 0 && "Release called on a pointer that's already been released\n" );
		}
		return cRef;
#else
		return _ThreadModel::Decrement( &m_cRef );
#endif
	}

	void Lock() const {}
	void Unlock() const {}
};

template <>
class CHeObjLockT< CHeObjRootSI<CHeSingleThreadModel> >
{
public:
	CHeObjLockT( const CHeObjRootSI<CHeSingleThreadModel>* ) {}
	~CHeObjLockT() throw() {}
};

template <>
class CHeObjUnlockT< CHeObjRootSI<CHeSingleThreadModel> >
{
public:
	CHeObjUnlockT( const CHeObjRootSI<CHeSingleThreadModel>* ) {}
	~CHeObjUnlockT() throw() {}
};

template <>
class CHeObjRootSI<CHeMultiThreadModelNoCS> : public CHeObjRootBase
{
public:
	typedef CHeMultiThreadModelNoCS _ThreadModel;
	typedef _ThreadModel::AutoCriticalSection _CritSec;
	typedef CHeObjLockT< CHeObjRootSI<_ThreadModel> > ObjectLock;
	typedef CHeObjUnlockT< CHeObjRootSI<_ThreadModel> > ObjectUnlock;

public:
	UInt32 InternalAddRef()
	{
		HE_ASSERT( m_cRef != -1 );
		return _ThreadModel::Increment( &m_cRef );
	}

	UInt32 InternalRelease()
	{
#ifdef _DEBUG
		const UInt32	cRef = _ThreadModel::Decrement( &m_cRef );
		if ( static_cast<SInt32>(cRef) < -(INT_MAX / 2) )
		{
			HE_ASSERT( 0 && "Release called on a pointer that's already been released\n" );
		}
		return cRef;
#else
		return _ThreadModel::Decrement( &m_cRef );
#endif
	}

	void Lock() const {}
	void Unlock() const {}
};

template <>
class CHeObjLockT< CHeObjRootSI<CHeMultiThreadModelNoCS> >
{
public:
	CHeObjLockT( const CHeObjRootSI<CHeMultiThreadModelNoCS>* ) {}
	~CHeObjLockT() throw() {}
};

template <>
class CHeObjUnlockT< CHeObjRootSI<CHeMultiThreadModelNoCS> >
{
public:
	CHeObjUnlockT( const CHeObjRootSI<CHeMultiThreadModelNoCS>* ) {}
	~CHeObjUnlockT() throw() {}
};

/// \class CHeFactory
/// \brief Corresponds to ATL::CComClassFactory
class CHeFactory :
	public CHeObjRoot<CHeGlobalsThreadModel>,
	public Helium::IHeFactory
{
public:
	HE_INTERFACE_MAP_BEGIN(CHeFactory)
		HE_INTERFACE_ENTRY_IID(HE_IHEFACTORY_IID,Helium::IHeFactory)
	HE_INTERFACE_MAP_END()

	CHeFactory() : m_pfnCreateInstance( NULL ) {}

	// IHeFactory
public:
	HE_IMETHOD CreateInstance(Helium::IHeUnknown* pOuter, const Helium::HeIID& iid, void** ppv)
	{
		if ( ppv == NULL ) return HE_E_NULL_POINTER;

		*ppv = NULL;

		LockFactory( true );

		HeResult	hr = HE_E_FAIL;

		if ( m_pfnCreateInstance != NULL )
		{
			hr = (*m_pfnCreateInstance)( pOuter, iid, ppv );
		}

		LockFactory( false );

		return hr;
	}

	HE_IMETHOD LockFactory(SInt32 bLock)
	{
		if ( _pModule != NULL )
		{
			if ( bLock )
			{
				_pModule->Lock();
			}
			else
			{
				_pModule->Unlock();
			}
		}

		return HE_S_OK;
	}

	void SetVoid(void* pv)
	{
		m_pfnCreateInstance = reinterpret_cast<HE_CREATORFUNC*>(pv);
	}

protected:
	virtual ~CHeFactory() {}

protected:
	HE_CREATORFUNC*	m_pfnCreateInstance;
};

template <class T>
class CHeCreator
{
public:
	static HeResult CreateInstance(void* pv, const Helium::HeIID& iid, void** ppv)
	{
		if ( ppv == NULL ) return HE_E_NULL_POINTER;
		*ppv = NULL;

		HeResult	hr = HE_E_OUT_OF_MEMORY;
		T*			pT = NULL;

		try
		{
			pT = new T();
		}
		catch(...)
		{
		}
		
		if ( pT != NULL )
		{
			pT->SetVoid( pv );
			pT->InternalAddRef();
			hr = pT->FinalConstruct();
			pT->InternalRelease();
			if ( hr == HE_S_OK )
			{
				hr = pT->QueryInterface( iid, ppv );
			}
			if ( hr != HE_S_OK )
			{
				delete pT;
			}
		}

		return hr;
	}
};

#define HE_DECLARE_CLASS_FACTORY(cf) 	typedef CHeCreator< cf > _ClassFactoryCreatorClass;
#define HE_DECLARE_CREATOR_CLASS(cc) 	typedef CHeCreator< cc > _CreatorClass;

/// \class CHeClass
/// \brief Corresponds to ATL::CComCoClass
template <class T>
class CHeClass
{
public:
	HE_DECLARE_CLASS_FACTORY(CHeObjNoLock< CHeFactory >)
	HE_DECLARE_CREATOR_CLASS(CHeObject< T >)

	template <class Q>
	static HeResult CreateInstance( Q** ppQ )
	{
		return T::_CreatorClass::CreateInstance( NULL, Q::GetIID(), reinterpret_cast<void**>(ppQ) );
	}
};

} /* namespace HeLib */

#endif /* HLBASE_H */
