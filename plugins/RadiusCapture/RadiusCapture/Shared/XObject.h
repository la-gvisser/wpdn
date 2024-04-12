// ============================================================================
//	XObject.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#pragma once

#include "XTypes.h"
#include "ThreadUtil.h"

class XIObject;

//-----------------------------------------------------------

class DECLSPEC XException
{

};


//-----------------------------------------------------------

// {583EC3CD-E4AA-4ed8-9877-848BE524A669}
X_DEFINE_GUID(XSmartPtrID,
0x583ec3cd, 0xe4aa, 0x4ed8, 0x98, 0x77, 0x84, 0x8b, 0xe5, 0x24, 0xa6, 0x69);

class XIObject;

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

	// return a dereference to a NULL ptr
	// would result in an access violation
	// so I think that a throw would be
	// better

	Y& operator*()
	{
		if (m_pPtr==NULL)
		{
			throw XException;
		}
		return *m_pPtr;
	}

	virtual int Attach( XIObject* pObject );

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

	virtual void	AddRef()=0;
	virtual void	Release()=0;
	virtual int		Reset()=0;
	virtual int		Init()=0;
};

//--------------------------------------

template <class Y>
int XSmartPtr<Y>::Attach(XIObject* pObject)
{
	m_pPtr = NULL;

	if (pObject)
	{
		int nRet = pObject->QueryInterface( Y::GetClassID(), (void**)&m_pPtr );
		if (nRet==X_OK)
		{
			m_pPtr->AddRef();
			return X_OK;
		}
	}
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

	X_DECLARE_CREATE( XObject );

	X_BEGIN_ID_MAP(XObject)
		X_ID_ENTRY(XObjectID)
		X_CLASS_ENTRY(XIObject)
	X_END_ID_MAP

	virtual void	AddRef()		{}
	virtual int		PreRelease()	{ return X_OK; }
	virtual void	Release()		{ if (PreRelease()==X_OK) {delete this;}}
	virtual int		Reset()			{ return X_OK; };
	virtual int		Init()			{ return X_OK; };
};


//-----------------------------------------------------------

// {2B26E061-17BC-11d4-886C-0050DA087DA3}
X_DEFINE_GUID(XIFactoryID,
0x2b26e061, 0x17bc, 0x11d4, 0x88, 0x6c, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

class DECLSPEC XIFactory :
	virtual public XIObject
{
  public:

	X_BEGIN_ID_MAP(XIFactory)
		X_ID_ENTRY(XIFactoryID)
		X_CLASS_ENTRY(XIObject)
	X_END_ID_MAP

	virtual XObject* createObject()=0;
};



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

	virtual int CalcHash(int /*iHashSize*/) { return -1; }
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

	X_DECLARE_CREATE(XDirty);

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

	virtual int	SetOwner(bool bOwn)
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

	virtual int			Reset()		{ m_bOn = true; return X_OK; }
	virtual bool		IsOn()		{ return m_bOn; }
	virtual int			TurnOn()	{ m_bOn = true; return X_OK; }
	virtual int			TurnOff()	{ m_bOn = false; return X_OK; }

  protected:
	 bool m_bOn;
};
