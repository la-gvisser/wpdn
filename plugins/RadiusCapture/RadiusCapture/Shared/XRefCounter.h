// ============================================================================
//	XRefCounter.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2001-2004. All rights reserved.

#ifndef XREFCOUNTER_H
#define XREFCOUNTER_H

#include "XObject.h"
#include "XMutex.h"


//--------------------------------------------------

// {2BE2BF79-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XRefCounterID, 
	0x2be2bf79, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);


class DECLSPEC XRefCounter : 
	virtual public XObject
{
  public:
	
	XRefCounter() : m_iRef(0), m_bOwner(FALSE) {}
	virtual ~XRefCounter() {}
	
	X_DECLARE_CREATE( XRefCounter );
	
	X_BEGIN_ID_MAP(XRefCounter)
		X_ID_ENTRY(XRefCounterID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP
		
	void SetOwner( BOOL bOwner ) { m_bOwner = bOwner; }

	virtual void Release()
	{
		ASSERT(m_iRef > 0);
		
		if (m_iRef <= 0)
		{
			return; // X_FAIL;
		}
		
		XLock theLock(&m_RefLock, FALSE );
		theLock.Lock();
		m_iRef--;
		theLock.Unlock();
		int iRef = m_iRef;
		
		if (iRef == 0 && m_bOwner == TRUE)
		{
			PreRelease();
			delete this;
		}

		return; // X_OK;
	}
	
	virtual void AddRef() 
	{ 
		XLock theLock(&m_RefLock, TRUE );
		m_iRef++; 
		return; // X_OK;
	}
	
  protected:
	
	int				m_iRef;
	BOOL			m_bOwner;
	XMutex			m_RefLock;
};



//--------------------------------------

// {3963C39D-3974-4d35-B61F-33251095C593}
X_DEFINE_GUID(XRefCountedObjectID, 
0x3963c39d, 0x3974, 0x4d35, 0xb6, 0x1f, 0x33, 0x25, 0x10, 0x95, 0xc5, 0x93);

template <class Y>
class DECLSPEC XRefCountedObject :
	virtual public Y,
	virtual public XRefCounter
{
	X_BEGIN_ID_MAP(XRefCountedObject)
		X_ID_ENTRY(XRefCountedObjectID)
		X_CLASS_ENTRY(XRefCounter)
		X_CLASS_ENTRY(Y)
	X_END_ID_MAP

	X_DECLARE_CREATE(Y)
};


#define X_DECLARE_REF_COUNTED_OBJECT(X,Y) \
	typedef XRefCountedObject< Y > X;


//--------------------------------------

// {C9D37A20-10B3-4347-8B6B-3BB267CC3C38}
X_DEFINE_GUID(XRefCountedPtrID, 
0xc9d37a20, 0x10b3, 0x4347, 0x8b, 0x6b, 0x3b, 0xb2, 0x67, 0xcc, 0x3c, 0x38);

template <class Y>
class DECLSPEC XRefCountedPtr :
	virtual public XRefCounter
{
  public:
	virtual ~XRefCountedPtr() { delete GetType(); }

	X_BEGIN_ID_MAP(XRefCountedPtr)
		X_ID_ENTRY(XRefCountedPtrID)
		X_CLASS_ENTRY(XRefCounter)
		X_OBJECT_ENTRY(m_pType)
	X_END_ID_MAP

	X_DECLARE_CREATE(XRefCountedPtr)
	X_DECLARE_PTR(Y,Type)
};


#define X_DECLARE_REF_COUNTED_PTR(X,Y) \
	typedef XRefCountedPtr< Y > X;


//#define TEST_REFCOUNTER 1
#ifdef TEST_REFCOUNTER

class XMemBlock : virtual public XObject
{
public:
	void DoThis();
};

X_DECLARE_REF_COUNTED_OBJECT( XCountedMemBlock , XMemBlock )
X_DECLARE_SMART_PTR( XCountedMemBlock )

void static TestRefCounter()
{
	// Create an XCountedMemBlock
	XCountedMemBlock* pMemBlock = new XCountedMemBlock();

	// Use the declared smart ptr
	XCountedMemBlockPtr pMemBlockPtr2( pMemBlock );

	// use the builtin smart ptr
	XCountedMemBlock::SmartPtr pMemBlockPtr( pMemBlock );

	// decr the ref count
	pMemBlock->Release();

	pMemBlockPtr->DoThis();
}

#endif

#endif

