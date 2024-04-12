// ============================================================================
//	XList.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002-2004. All rights reserved.

#ifndef XLIST_H
#define XLIST_H

#include <string.h>

#include "XObject.h"
#include "XTools.h"
#include "XBitfield.h"
#include "XMutex.h"


// {2BE2BF9B-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XLinkID, 
0x2be2bf9b, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

template<class T>
class DECLSPEC XLink : virtual public XCompare
{
#ifndef _LINUX
	friend XLink;
#endif

  public:
	  
	XLink(T* t=NULL);
	virtual ~XLink();

	X_BEGIN_ID_MAP(XLink)
		X_ID_ENTRY(XLinkID)
		X_CLASS_ENTRY(XCompare)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP
	
	virtual T*			GetObject() ;
	virtual void		SetObject(T* pObject);
	virtual X_RESULT	Init();
	virtual int			Unlink() ;
	virtual XLink<T>*	GetNext();
	virtual XLink<T>*	GetPrev();
	virtual T*			GetNextObject();
	virtual T*			GetPrevObject();
	virtual int			InsertNext(XLink<T>* pLink) ;
	virtual int			SetNext(XLink<T>* pLink) ;
	virtual int			InsertPrev(XLink<T>* pLink) ;
	virtual int			SetPrev(XLink<T>* pLink) ;
	virtual int			Clear();
	virtual int			Remove();
	virtual UInt32		GetCount();
	virtual SInt16		Compare( const XCompare& rhs, int iFlag = 0) const;

  protected:

	XLink<T>*	m_pNext;
	XLink<T>*	m_pPrev;
	T*	m_pT;
};


//------------------------------------
// This macro makes it easy to make any
// class a linked class.

#define X_DECLARE_LINKED_CLASS(X,Y) \
template<class Y> \
class X : virtual public XLink<Y> { };


//-----------------------------------------------------------
//		XList
//-----------------------------------------------------------

// {2BE2BF9D-5F2A-11d3-8849-0050DA087DA3}
X_DEFINE_GUID(XListID, 
0x2be2bf9d, 0x5f2a, 0x11d3, 0x88, 0x49, 0x0, 0x50, 0xda, 0x8, 0x7d, 0xa3);

#define X_LIST_OWNER		0x1	
#define X_LIST_KEEP_UNUSED	0x2	
#define X_LIST_CLEARING		0x4
#define X_LIST_CHAIN		0x8
#define X_LIST_SORTED		0x20

template<class T>
class DECLSPEC XList : virtual public XLock
{
  public:
	  
	XList();
	virtual ~XList();

	X_BEGIN_ID_MAP(XList)
		X_ID_ENTRY(XListID)
		X_CLASS_ENTRY(XLock)		
		X_CLASS_ENTRY(XObject)		
	X_END_ID_MAP

	//DECLARE_PRINT
	
	virtual X_RESULT	Reset();	
	virtual int			KeepUnused(bool bKeepUnused);
	virtual bool		HasNext();
	virtual bool		HasPrev();
	virtual int			GetCount();
	virtual int			SetChain(bool bChain);
	virtual int			SetOwner(bool bOwner);
	virtual XLink<T>*	GetLink();
	virtual int			SetLink(XLink<T>* pCurrent);
	virtual T*			GetFirst();
	virtual T*			GetLast();
	virtual T*			GetNext();
	virtual T*			GetPrev();
	virtual T*			GetCurrent();
	virtual T*			GetObject();
	virtual T*			GetAt(int iIndex);
	virtual int			Add(T* pT);			// adds to the tail
	virtual int			AddTail(T* pT);		// adds to the tail
	virtual int			AddHead(T* pT);		// adds to the head
	virtual int			Insert(T* t);
	virtual int			InsertAfter(T* t);
	virtual int			Remove(const X_GUID& rGUID);	
	virtual int			Remove(T* pT1);
	virtual int			Remove();
	virtual T*			RemoveHead();
	virtual int			LocalSort(void * list, int i);
	virtual int			Sort();
	virtual int			Clear();
	virtual T*			FindObject(T* pOuterObject);
	virtual int			FindNextObjectByID(const X_GUID& rGUID, void** pp, const X_GUID* rGUIDout=NULL);
	virtual int			FindObjectByID(const X_GUID& rGUID, void** pp, const X_GUID* rGUIDout=NULL);

	virtual int			ReleaseAll();	// deletes all elements in the list

  protected:

	virtual XLink<T>*	CreateLink(T* pT);
	
	XLink<T>*		m_pHead;	
	XLink<T>*		m_pTail;	
	XLink<T>*		m_pCurrent;	
	int				m_iCount;
	XLink<T>*		m_pUnusedQ;
	XBitField64		m_flags;
	XMutex			m_Mutex;
};

typedef XList<XObject> XObjectList;

#include "XList.cpp"

//-----------------------------------------------------------
//	class XCounterLink
//-----------------------------------------------------------

template<class T>
class DECLSPEC XCounterLink : public XLink<T>
{
  public:

	XCounterLink(T* t=NULL) : XLink<T>(t), m_iCounter(1) {}
	virtual ~XCounterLink() {}
	
	virtual int Incr(const int iValue=1);
	virtual int Decr(const int iValue=1);
	virtual int GetCounter();

  protected:

	int m_iCounter;
};

//-----------------------------------------------------------

template<class T>
int XCounterLink<T>::Incr(const int iValue)
{
	m_iCounter += iValue;
	return 0;
}

template<class T>
int XCounterLink<T>::Decr(const int iValue)
{
	m_iCounter -= iValue;
	return 0;
}

template<class T>
int XCounterLink<T>::GetCounter()
{
	return m_iCounter;
}

//-----------------------------------------------------------
//	class XCounterList
//-----------------------------------------------------------

template<class T>
class DECLSPEC XCounterList : public XList<T>
{
  public:

	XCounterList()  {}
	virtual ~XCounterList() {}

	virtual int Add(T* pT);
	virtual int GetCounter();
	virtual XLink<T>* CreateLink(T* pT);
};

//-----------------------------------------------------------

template<class T>
int XCounterList<T>::Add(T* pT)
{
	Lock();
	T* pInnerObject = findObject(pT);

	if (pInnerObject)
	{
		XCounterLink<T>* pLink = (XCounterLink<T>*)m_pCurrent;
		pLink->incr();
	}
	else
	{
		XList<T>::add( pT );
	}

	Unlock();

	return X_OK;
}

template<class T>
int XCounterList<T>::GetCounter()
{
	Lock();

	int iCounter = X_FAIL;

	if (m_pCurrent)
	{
		XCounterLink<T>* pLink = (XCounterLink<T>*)m_pCurrent;
		iCounter = pLink->GetCounter();
	}

	Unlock();

	return iCounter;
}

template<class T>
XLink<T>* XCounterList<T>::CreateLink(T* pT)
{
	return new XCounterLink<T>(pT);
}

//---------------------------------------------------------------

template<class T>
class DECLSPEC XListener : virtual public XObject
{
  public:
	virtual int Notify(T* pT)=0;
};


//---------------------------------------------------------------

template<class T>
class DECLSPEC XListenerManager : virtual public XObject
{
  public:

	XListenerManager() :
		m_pListeners( NULL )
	{

	}

	virtual ~XListenerManager()
	{
		X_SAFE_DELETE( m_pListeners );
	}

	virtual int AddListener( XListener<T>* pListener )
	{
		if (m_pListeners == NULL)
		{
			m_pListeners = new XList< XListener<T> >;
		}

		m_pListeners->Add( pListener );
		return X_OK;
	}

	virtual int removeListener( XListener<T>* pListener )
	{
		_ASSERT( m_pListeners );
		m_pListeners->Remove( pListener );
		return X_OK;
	}

	virtual int Notify(T* pT)
	{
		int err = X_OK;

		if ( m_pListeners )
		{
			XListener<T>* pListener = m_pListeners->GetFirst();

			while (pListener)
			{
				err = pListener->Notify( pT );

				if (err == X_FAIL)
				{
					
				}
				pListener = m_pListeners->GetNext();
			}
		}

		return err;
	}

  protected:

	XList< XListener<T> >* m_pListeners;
};


#endif
