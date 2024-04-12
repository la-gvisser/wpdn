// ============================================================================
//	XObject.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.


#ifndef XOBJECT_H_
#define XOBJECT_H_

#include "XTypes.h"

//forwards
class XIObject;






//-----------------------------------------------------------

class DECLSPEC XException
{

};


//-----------------------------------------------------------

// {583EC3CD-E4AA-4ed8-9877-848BE524A669}
X_DEFINE_GUID(XSmartPtrID, 
0x583ec3cd, 0xe4aa, 0x4ed8, 0x98, 0x77, 0x84, 0x8b, 0xe5, 0x24, 0xa6, 0x69);


template <class Y>
class DECLSPEC XSmartPtr 
{
  public:
	  
	XSmartPtr(XIObject* pObject=NULL) : m_pPtr(NULL)
	{
		Attach(pObject);
	}
	
	virtual ~XSmartPtr()
	{
		Detach();
	}	
	
	X_BEGIN_ID_MAP(XSmartPtr)
		X_ID_ENTRY(XSmartPtrID)
		X_OBJECT_ENTRY(m_pPtr)
	X_END_ID_MAP
	
	void operator=(XIObject* pObject)
	{
		Attach(pObject);
	}
	
	Y* operator->() { return m_pPtr; }
	operator Y*()	{ return m_pPtr; }
	operator Y&()	{ return &m_pPtr; }

	// return a dereference to a NULL ptr
	// would result in an access violation
	// so I think that a throw would be 
	// better
	
	Y& operator*()	
	{  
		if (m_pPtr==NULL)
		{
#ifndef _LINUX
			throw XException;
#endif
		}
		return *m_pPtr; 
	}
	
	virtual X_RESULT Create( );

	virtual X_RESULT Attach( XIObject* pObject );

	// Detach is not really a good idea.  Try
	// to avoid it.  It could result in a throw

	virtual void Detach();

  protected:
	  
	Y* m_pPtr;	
};

#define X_DECLARE_SMART_PTR(Y) typedef XSmartPtr<Y> Y ## Ptr;

//-----------------------------------------------------------

// {FF028A0B-967A-4de1-9A46-49004C1DFDA5}
X_DEFINE_GUID(XIObjectID, 
0xff028a0b, 0x967a, 0x4de1, 0x9a, 0x46, 0x49, 0x0, 0x4c, 0x1d, 0xfd, 0xa5);


class DECLSPEC XIObject
{
  public:

	X_BEGIN_ID_MAP(XIObject)
		X_ID_ENTRY(XIObjectID)		
	X_END_ID_MAP
	
	virtual X_RESULT	AddRef()=0;
	virtual X_RESULT	Release()=0;
	virtual X_RESULT	Reset()=0;
	virtual X_RESULT	Init()=0;
};


//--------------------------------------

template <class Y>
X_RESULT XSmartPtr<Y>::Attach(XIObject* pObject)
{
	if (m_pPtr) Detach();
	
	if (pObject)
	{	
		X_RESULT nRet = pObject->QueryInterface( Y::GetClassID(), (void**)&m_pPtr );
		if (nRet==X_OK)
		{
			m_pPtr->AddRef();
			return X_OK;
		}
	}

	return X_FAIL;
}

// So why don't we return a pointer here?  Because
// it could have been deleted on the Release.

template <class Y>
void XSmartPtr<Y>::Detach()
{
	if (m_pPtr)
	{
		m_pPtr->Release();
		m_pPtr=NULL;
	}
}

//-----------------------------------------------------------

// {2BE2BF78-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XObjectID, 
0x2be2bf78, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);


class DECLSPEC XObject :
	virtual public XIObject
{
  public:

	XObject();
	virtual ~XObject();

	X_BEGIN_ID_MAP(XObject)
		X_ID_ENTRY(XObjectID)		
		X_CLASS_ENTRY(XIObject)		
	X_END_ID_MAP
	
	virtual X_RESULT	AddRef()		{ return X_OK; }
	virtual X_RESULT	PreRelease()	{ return X_OK; }
	virtual X_RESULT	Release()		{ if (PreRelease()==X_OK) {delete this; return X_OK; } return X_FAIL; }
	virtual X_RESULT	Reset()			{ return X_OK; };
	virtual X_RESULT	Init()			{ return X_OK; };
};


//
// Object creation macros.  Creates
// an object and calls Init()

#define kNoInit FALSE

// {23A47944-106D-4cca-8DB2-7C2E2D682B0C}
X_DEFINE_GUID(XFactoryID, 
0x23a47944, 0x106d, 0x4cca, 0x8d, 0xb2, 0x7c, 0x2e, 0x2d, 0x68, 0x2b, 0xc);


class XFactory : virtual public XObject
{

public:
	X_BEGIN_ID_MAP(XFactory)
		X_ID_ENTRY(XFactoryID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP


	//virtual X_GUID GetClassID() =0;
	virtual int Create(XObject** ppOut) =0;
	virtual XObject* CreateEx(BOOL bInit=TRUE) =0;
};



//--------------------------------------

// {DCA7F208-964A-48b7-86B2-49F7CEFDC492}
X_DEFINE_GUID(XDictionaryID, 
0xdca7f208, 0x964a, 0x48b7, 0x86, 0xb2, 0x49, 0xf7, 0xce, 0xfd, 0xc4, 0x92);

class DECLSPEC XDictionary : virtual public XObject
{
public:
	X_BEGIN_ID_MAP(XDictionary)
		X_ID_ENTRY(XDictionaryID)		
		X_CLASS_ENTRY(XIObject)		
	X_END_ID_MAP
	

	virtual X_RESULT LookupObject( const X_GUID& guid, XObject*& pObject)=0;
	virtual X_RESULT AddObject( const X_GUID& guid, XObject* pObject)=0;
};


extern XDictionary* s_pFactoryMap;
extern void SetFactoryMap( XDictionary* pDictionary );
#define X_ADD_FACTORY(y) y ## Factory y ## FactoryInstance;

//--------------------------------------

template <class Y>
X_RESULT XSmartPtr<Y>::Create()
{
	ASSERT( s_pFactoryMap );
	if (s_pFactoryMap == NULL) return X_FAIL;

	XObject* pFactoryObject = NULL;
	X_RESULT hr = s_pFactoryMap->LookupObject( Y::GetClassID(), pFactoryObject );
	if (X_FAILED(hr)) return hr;

	XFactory* pFactory = NULL;
	hr = pFactoryObject->QueryInterface( XFactoryID, (void**)pFactory );
	if (X_FAILED(hr)) return hr;

	XObject* pObject = NULL;
	hr = pFactory->Create(&pObject);
	if (X_FAILED(hr)) return hr;

	hr = Attach(pObject);
	if (X_FAILED(hr)) return hr;

	return X_OK;
}




#define X_DECLARE_CREATE(x) \
static int Create(x** ppOut) \
{ \
	*ppOut = NULL; \
	x* pNew = new x(); \
	_ASSERT(pNew); \
	int ret = X_FAIL; \
	if (pNew) ret = pNew->Init(); \
	if (ret == X_OK) *ppOut = pNew; \
	return ret; \
} \
static x* CreateEx(BOOL bInit=TRUE) \
{ \
	x* pNew = new x(); \
	_ASSERT(pNew); \
	if (pNew && bInit==TRUE) pNew->Init(); \
	return pNew; \
}

#define X_DECLARE_CREATE_AS_TYPE(x,y) \
static int CreateAs ## y(y** ppOut) \
{ \
	*ppOut = NULL; \
	x* pNew = new x(); \
	_ASSERT(pNew); \
	int ret = X_FAIL; \
	if (pNew) ret = pNew->Init(); \
	if (ret == X_OK) *ppOut = pNew; \
	return ret; \
} \
static y* CreateAsEx ## y(BOOL bInit=TRUE) \
{ \
	y* pNew = new x(); \
	_ASSERT(pNew); \
	if (pNew && bInit==TRUE) pNew->Init(); \
	return pNew; \
}
  






//-----------------------------------------------------------
//  XCompare

// {2BE2BF7E-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XCompareID, 
0x2be2bf7e, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

class DECLSPEC XCompare : virtual public XObject
{
  public:

	X_BEGIN_ID_MAP(XCompare)
		X_ID_ENTRY(XCompareID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	virtual SInt16 Compare( const XCompare& /*rhs*/, int /*iFlag*/ = 0) const 
	{ 
		return -1; 
	}
};

//-----------------------------------------------------------
//  XCalcHash

// {2BE2BF80-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XCalcHashID, 
0x2be2bf80, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

class DECLSPEC XCalcHash : virtual public XObject
{
  public:

	X_BEGIN_ID_MAP(XCalcHash)
		X_ID_ENTRY(XCalcHashID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	virtual X_RESULT CalcHash(int /*iHashSize*/) const { return -1; }
};

//-----------------------------------------------------
// XDirty

// {E897D9A1-44A4-11d4-8875-0050DA087DA3}
X_DEFINE_GUID(XDirtyID, 
0xe897d9a1, 0x44a4, 0x11d4, 0x88, 0x75, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

class XDirty :
	virtual public XObject
{
  public:

	XDirty() : m_bIsDirty(false) {}
	  
	X_BEGIN_ID_MAP(XDirty)
		X_ID_ENTRY(XDirtyID)
		X_CLASS_ENTRY(XObject)	
	X_END_ID_MAP
	  
	virtual bool IsDirty() {	return m_bIsDirty; }
	virtual void SetDirty(bool bIsDirty) { m_bIsDirty = bIsDirty; }
	  
  private:
	bool m_bIsDirty;
};

//-----------------------------------------------------------
//  XOwner

// {27191E54-81C1-475c-AC5F-7BCD57E88E69}
X_DEFINE_GUID(XOwnerID, 
0x27191e54, 0x81c1, 0x475c, 0xac, 0x5f, 0x7b, 0xcd, 0x57, 0xe8, 0x8e, 0x69);


class DECLSPEC XOwner : virtual public XObject
{
  public:

	XOwner() : m_bOwn(false) {}
	  
	X_BEGIN_ID_MAP(XOwner)
		X_ID_ENTRY(XOwnerID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	virtual X_RESULT SetOwner(bool bOwn) 
	{ 
		m_bOwn = bOwn; 
		return X_OK; 
	}

	virtual bool IsOwned() 
	{ 
		return m_bOwn; 
	}
	
  private:
	  
	bool	m_bOwn;
};


//--------------------------------------------------

// {3CA2F1E1-26D2-11d4-886F-0050DA087DA3}
X_DEFINE_GUID(XSwitchID, 
0x3ca2f1e1, 0x26d2, 0x11d4, 0x88, 0x6f, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

class DECLSPEC XSwitch : virtual public XObject
{
  public:
	XSwitch() : m_bOn(true) {}
	virtual ~XSwitch() {}

	X_BEGIN_ID_MAP(XSwitch)
		X_ID_ENTRY(XSwitchID)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	virtual X_RESULT	Reset()		{ m_bOn = true; return X_OK; }	
	virtual bool		IsOn()		{ return m_bOn; }
	virtual X_RESULT	TurnOn()	{ m_bOn = true; return X_OK; }
	virtual X_RESULT	TurnOff()	{ m_bOn = false; return X_OK; }

  protected:
	 bool m_bOn;
};



#endif
