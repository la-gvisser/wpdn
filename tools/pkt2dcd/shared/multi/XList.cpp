// =============================================================================
//	XList.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2001. All rights reserved.



template <class T>
XLink<T>::XLink(T* t) :
	m_pNext	(NULL),
	m_pPrev (NULL),
	m_pT	(t)
{
		
}

//--------------------------------------

template <class T>
XLink<T>::~XLink()
{

}

//--------------------------------------

template <class T>
T* XLink<T>::GetObject() 
{ 
	T* pT = NULL;
	m_pT->QueryInterface(T::GetClassID(),(void**)&pT);
	return pT; 
}

//--------------------------------------

template <class T>
void XLink<T>::SetObject(T* pObject) 
{ 
	m_pT = pObject; 
}

//--------------------------------------

template <class T>
X_RESULT XLink<T>::Init()
{
	m_pNext = NULL;
	m_pPrev = NULL;
	return X_OK;
}

//--------------------------------------

template <class T>
int XLink<T>::Unlink() 
{ 
	m_pNext = NULL;
	m_pPrev = NULL;
	return X_OK;
}

//--------------------------------------

template <class T>
XLink<T>* XLink<T>::GetNext() 
{ 
	return m_pNext; 
}

//--------------------------------------

template <class T>
XLink<T>* XLink<T>::GetPrev() 
{ 
	return m_pPrev; 
}

//--------------------------------------

template <class T>
T* XLink<T>::GetNextObject() 
{ 
	return (m_pNext) ? m_pNext->GetObject() : NULL; 
}

//--------------------------------------

template <class T>
T* XLink<T>::GetPrevObject() 
{ 
	return (m_pPrev) ? m_pPrev->GetObject() : NULL; 
}

//--------------------------------------

template <class T>
int XLink<T>::InsertNext(XLink<T>* pLink) 
{ 
	_ASSERT( pLink );
	//_ASSERT( pLink->GetPrev() == NULL );
	//if( (pLink->GetNext() != NULL) && (GetNext() != NULL) ) _ASSERT(0);

	if (m_pNext)
	{
		pLink->SetNext( m_pNext );
		m_pNext->SetPrev( pLink );
	}

	pLink->SetPrev( this );
	SetNext(pLink);
	return X_OK;
}

//--------------------------------------

template <class T>
int XLink<T>::SetNext(XLink<T>* pLink) 
{ 
	m_pNext = pLink;
	return X_OK;
}

//--------------------------------------

template <class T>
int XLink<T>::InsertPrev(XLink<T>* pLink) 
{ 
	_ASSERT( pLink );
	_ASSERT( pLink->GetNext() == NULL );
	//if( (pLink->GetPrev() != NULL) && (GetPrev() != NULL) ) _ASSERT(0);

	if (m_pPrev)
	{
		pLink->SetPrev( m_pPrev );
		m_pPrev->SetNext( pLink );
	}

	pLink->SetNext( this );
	SetPrev(pLink);
	return X_OK;
}

//--------------------------------------

template <class T>
int XLink<T>::SetPrev(XLink<T>* pLink) 
{ 
	m_pPrev = pLink;
	return X_OK;
}

//--------------------------------------

template <class T>
int XLink<T>::Clear()
{
	XLink<T>* pLink = m_pNext;
	XLink<T>* pNext = NULL;

	while (pLink)
	{
		pNext = pLink->GetNext();
		X_SAFE_RELEASE(pLink);
		pLink = pNext;
	}

	Release();
	return X_OK;
}

//--------------------------------------

/**
 * Remove the current link
 */

template <class T>
int XLink<T>::Remove()
{
	XLink<T>* pNext = NULL;
	XLink<T>* pPrev = NULL;
	
	pPrev = GetPrev();
	pNext = GetNext();

	if (pPrev)
	{
		if (pNext)
		{
			pPrev->SetNext( pNext );
			pNext->SetPrev( pPrev );
		}
		else
		{
			pPrev->SetNext( NULL );
		}
	}
	else
	{
		if (pNext)
		{
			pNext->SetPrev( NULL );
		}
	}

	//Unlink the current link
	Unlink();
	
	return X_OK;
}

//--------------------------------------

template <class T>
UInt32 XLink<T>::GetCount()
{
	UInt32 iCount = 1;
	XLink<T>* pNext = m_pNext;

	while (pNext)
	{
		pNext = pNext->GetNext();
		iCount++;
	}
	return iCount;
}

//--------------------------------------

template <class T>
SInt16 XLink<T>::Compare(const XCompare& rhs, int iFlag) const
{
	XCompare* pCompare1 = NULL;
	XCompare* pCompare2 = NULL;
	XLink<T>* pLink = NULL;

	if (m_pT->QueryInterface(XCompareID, (void**)&pCompare1) == X_OK)
	{
		XCompare* pOther = &(XCompare&)rhs;
		if (pOther->QueryInterface(XLinkID, (void**)&pLink) == X_OK)
		{
			T * pObject = pLink->GetObject();
			if(pObject->QueryInterface(XCompareID, (void**)&pCompare2) == X_OK)
			{
				return pCompare1->Compare( *pCompare2, iFlag );
			}
		}
	}
	return X_FAIL;
}

//--------------------------------------


template <class T>
XList<T>::XList() : 
	XLock( &m_Mutex ),
	m_pHead			(NULL), 
	m_pTail			(NULL), 
	m_pCurrent		(NULL),
	m_iCount		(0),
	m_pUnusedQ		(NULL)
{
}

//--------------------------------------

template <class T>
XList<T>::~XList() 
{
	m_flags.TurnOff(X_LIST_KEEP_UNUSED);

	Clear();

	while (m_pUnusedQ)
	{
		XLink<T>* pNext = m_pUnusedQ->GetNext();
		delete m_pUnusedQ;
		m_pUnusedQ = pNext;
	}
}

//--------------------------------------

template <class T>	
int XList<T>::KeepUnused(bool bKeepUnused)
{
	if (bKeepUnused)
	{
		m_flags.TurnOn(X_LIST_KEEP_UNUSED);
	}
	else
	{
		m_flags.TurnOff(X_LIST_KEEP_UNUSED);
	}
	
	return X_OK;
}


//--------------------------------------

template <class T>
bool XList<T>:: HasNext() 
{
	Lock();
	bool res = (m_pCurrent && m_pCurrent->GetNext()) ? true : false;
	Unlock();
	return res;
}

//--------------------------------------

template <class T>
bool XList<T>:: HasPrev() 
{
	Lock();
	bool res = (m_pCurrent && m_pCurrent->GetPrev()) ? true : false;
	Unlock();
	return res;
}

//--------------------------------------

template <class T>
int	XList<T>::GetCount()
{
	int i=0;

	Lock();
	i = m_iCount;
	Unlock();

	return i;
}

//--------------------------------------

template <class T>
int XList<T>::SetChain(bool bChain)
{
	if (bChain)
	{
		m_flags.TurnOn(X_LIST_CHAIN);
	}
	else
	{
		m_flags.TurnOff(X_LIST_CHAIN);
	}
			
	return X_OK;
}

//--------------------------------------

template <class T>
int XList<T>::SetOwner(bool bOwner)
{
	if (bOwner)
	{
		m_flags.TurnOn(X_LIST_OWNER);
	}
	else
	{
		m_flags.TurnOff(X_LIST_OWNER);
	}

	return X_OK;
}

//--------------------------------------

template <class T>
XLink<T>* XList<T>::GetLink()
{
	return m_pCurrent;
}

//--------------------------------------

template <class T>
int XList<T>::SetLink(XLink<T>* pCurrent)
{
	m_pCurrent = pCurrent;
	return X_OK;
}

//--------------------------------------

template <class T>
T* XList<T>::GetFirst() 
{ 
	Lock();

	m_pCurrent = m_pHead;

	T* pT = GetCurrent();

	Unlock();

	return pT;
}

//--------------------------------------

template <class T>
T* XList<T>::GetLast() 
{ 
	Lock();

	m_pCurrent = m_pTail;

	T* pT = GetCurrent();

	Unlock();

	return pT;
}

//--------------------------------------

template <class T>
T* XList<T>::GetNext() 
{ 
	T* pT = NULL;

	Lock();

	if (m_pCurrent)
	{
		XLink<T>* pNext = m_pCurrent->GetNext();

		if (pNext)
		{
			m_pCurrent = pNext;
			pT = GetObject();
		}
	}

	Unlock();

	return pT;
}

//--------------------------------------

template <class T>
T* XList<T>::GetPrev() 
{ 
	T* pT = NULL;

	Lock();

	if (m_pCurrent)
	{
		XLink<T>* pPrev = m_pCurrent->GetPrev();

		if (pPrev)
		{
			m_pCurrent = pPrev;
			pT = GetObject();
		}
	}

	Unlock();

	return pT;
}

//--------------------------------------

template <class T>
T* XList<T>::GetCurrent() 
{ 
	return GetObject();
}

//--------------------------------------

template <class T>
T* XList<T>::GetObject() 
{ 
	T* pT = NULL;

	Lock();

	if (m_pCurrent)
	{
		pT = m_pCurrent->GetObject();
		_ASSERT(pT);
	}

	Unlock();

	return pT;
}

//--------------------------------------

/**
 * Add an object to the end of the list
 */

template <class T>
int XList<T>::Add(T* pT)
{
	GetLast();
	InsertAfter(pT);
	
	return X_OK;
}

template <class T>
int XList<T>::AddTail(T* pT)
{
	GetLast();
	InsertAfter(pT);
	
	return X_OK;
}


template <class T>
int	XList<T>::AddHead(T* pT)
{
	GetFirst();
	Insert(pT);

	return X_OK;
}

//--------------------------------------

/**
 * Insert a link before the current one.
 */

template <class T>
int XList<T>::Insert(T* t)
{
	Lock();

	XLink<T>* pLink = CreateLink(t);

	if (m_pCurrent)
	{
		m_pCurrent->InsertPrev(pLink);
		if (m_pHead == m_pCurrent)
		{
			m_pHead = pLink;
		}

	}
	else
	{
		m_pHead = m_pCurrent = m_pTail = pLink;
	}

	m_iCount++;
	m_flags.TurnOff(X_LIST_SORTED);
	Unlock();
	return X_OK;
}

//--------------------------------------

/**
 * Insert a link before the current one.
 */

template <class T>
int XList<T>::InsertAfter(T* t)
{
	Lock();

	XLink<T>* pLink = CreateLink(t);

	if (m_pCurrent)
	{
		m_pCurrent->InsertNext(pLink);
		if (m_pTail == m_pCurrent)
		{
			m_pTail = pLink;
		}

	}
	else
	{
		m_pHead = m_pCurrent = m_pTail = pLink;
	}

	m_iCount++;
	m_flags.TurnOff(X_LIST_SORTED);
	Unlock();
	return X_OK;
}

//--------------------------------------

/**
 * Remove a specific link.
 */

template <class T>
int XList<T>::Remove(T* pT1)
{
	int err = X_OK;
	Lock();

	T* pT2 = GetCurrent();

	if (pT2 && (pT1 == pT2))
	{
		Remove();
	}
	else
	{
		pT2 = GetFirst();

		while (pT2)
		{
			if (pT1 == pT2)
			{
				Remove();
				pT2 = NULL;
			}
			else
			{
				pT2 = GetNext();
			}
		}
	}

	Unlock();
	return err;
}

//--------------------------------------

/**
 * Remove the fist link
 */

template <class T>
T* XList<T>::RemoveHead()
{
	Lock();
	T* pT = GetFirst();
	{

		Remove();

	}
	Unlock();

	return pT;
}

/**
 * Remove the current link
 */

template <class T>
int XList<T>::Remove()
{
	Lock();

	_ASSERT( m_iCount );

	XLink<T>* pLink = m_pCurrent;
	XLink<T>* pNext = NULL;
	XLink<T>* pPrev = NULL;
	
	if (pLink)
	{
		pPrev = pLink->GetPrev();
		pNext = pLink->GetNext();

		if (pPrev)
		{
			if (pNext)
			{
				pPrev->SetNext( pNext );
				pNext->SetPrev( pPrev );
			}
			else
			{
				pPrev->SetNext( NULL );
			}
		}
		else
		{
			if (pNext)
			{
				pNext->SetPrev( NULL );
			}
		}

		if (m_pTail == pLink)
		{
			m_pTail = pPrev;
		}

		if (m_pHead == pLink)
		{
			m_pHead = pNext;
		}

		m_pCurrent = pNext; 

		if (m_flags.IsOn(X_LIST_OWNER))
		{
			if (m_flags.IsOn(X_LIST_CHAIN))
			{
				X_SAFE_RELEASE(pLink)
			}
			else
			{
				T* pT = pLink->GetObject();
				X_SAFE_RELEASE(pT);
			}
		}

		//if the chain is made of internally Created
		//links then figure out what to do with this one
		if (m_flags.IsOff(X_LIST_CHAIN))
		{
			//pLink->SetObject( NULL );

			if (m_flags.IsOn(X_LIST_KEEP_UNUSED) && (m_flags.IsOff(X_LIST_CLEARING)))
			{
				pLink->Unlink();

				if (m_pUnusedQ)
				{
					pLink->InsertNext( m_pUnusedQ );
				}

				m_pUnusedQ = pLink;
			}
			else
			{
				X_SAFE_RELEASE(pLink);
			}
		}
	}

	m_iCount--;
	
	_ASSERT( m_iCount >= 0 );
	//if( (m_iCount > 0) && (m_pCurrent == NULL) ) _ASSERT(0);

	Unlock();
	return X_OK;
}

//--------------------------------------

template <class T>
int XList<T>::LocalSort(void * list, int i)
{
	XTools::Sort(list, i);
	return X_OK;
}

//--------------------------------------

template <class T>
int XList<T>::Sort()
{
	Lock();

	if (m_flags.IsOn(X_LIST_SORTED))
	{
		Unlock();
		return X_OK;
	}
	
	int iCount = 0;
	XLink<T> * pLink = m_pHead;
	while (pLink)
	{
		iCount++;
		pLink = pLink->GetNext();
	}

	if (iCount < 2)
	{
		Unlock();
		return X_OK;
	}

	XObject ** ppObjectArray = new XObject*[iCount];
	pLink = m_pHead;
	int i = 0;

	while (pLink)
	{
		XObject* pObject = NULL;
		pLink->QueryInterface(XObjectID, (void**)&pObject);
				
		ppObjectArray[i++] = pObject;
		pLink = pLink->GetNext();
	}

	
	LocalSort(ppObjectArray, i);
	

	//reSet the m_pPrev and m_pNext in each link
	for (int j = 0; j < i - 1; j++)
	{
		XLink<T> * pLink1 = NULL;
		XLink<T> * pLink2 = NULL;

		ppObjectArray[j]->QueryInterface(XLinkID, (void**)&pLink1);
		ppObjectArray[j+1]->QueryInterface(XLinkID, (void**)&pLink2);
					
		if (j == 0)
		{
			pLink1->SetPrev(NULL);
			m_pCurrent = pLink1;
			m_pHead = pLink1;
		}
		else
		{
			pLink1->SetPrev(m_pCurrent);
		}

		if (j == i - 2)
		{
			pLink2->SetNext(NULL);
			pLink2->SetPrev(pLink1);
			m_pTail = pLink2;
		}	
		
		pLink1->SetNext(pLink2);
					
		m_pCurrent = pLink1;									
	}
	
	m_pCurrent = m_pHead;

	delete [] ppObjectArray;

	m_flags.TurnOn(X_LIST_SORTED);

	Unlock();

	return X_OK;
}

//--------------------------------------

template <class T>
T* XList<T>::GetAt(int iIndex)
{		
	Lock();

	if ( GetFirst() ) do 
	{
		iIndex--;
	}
	while (iIndex && GetNext());

	Unlock();

	return GetCurrent();
}


//--------------------------------------

template <class T>
int XList<T>::Clear()
{
	T* pT = NULL;
	
	Lock();

	m_flags.TurnOn(X_LIST_CLEARING);

	while ( GetFirst() )
	{
		Remove();
	} 

	m_flags.TurnOff(X_LIST_CLEARING);

	Unlock();

	return X_OK;
}

//--------------------------------------

//
// match on object
//

template <class T>
T* XList<T>::FindObject(T* pOuterObject)
{
	Lock();

	T* pInnerObject = NULL;

	if (GetFirst()) do
	{
		pInnerObject = GetObject();

		if ( pInnerObject == pOuterObject )
		{
			Unlock();
			return pInnerObject;
		}
	}
	while (GetNext());

	Unlock();
	return NULL;
}


//--------------------------------------

//
// match on object
//

template <class T>
int XList<T>::FindNextObjectByID(const X_GUID& rGUID, void** pp, const X_GUID* rGUIDout)
{
	Lock();

	T* pInnerObject = NULL;

	while (GetNext())
	{
		pInnerObject = GetObject();
		
		if (pInnerObject->QueryInterface(rGUID,pp)==X_OK)
		{
			if (rGUIDout)
			{
				if (pInnerObject->QueryInterface(*rGUIDout,pp)==X_OK)
				{
					Unlock();
					return X_OK;							
				}				
			}
			else
			{
				Unlock();
				return X_OK;				
			}
		}
	}

	Unlock();
	return X_FAIL;
}


//--------------------------------------
// match on class ID
//

template <class T>
int XList<T>::FindObjectByID(const X_GUID& rGUID, void** pp, const X_GUID* rGUIDout)
{
	Lock();

	T* pInnerObject = NULL;

	if (GetFirst()) do
	{			
		pInnerObject = GetObject();

		if (pInnerObject->QueryInterface(rGUID,pp)==X_OK)
		{
			if (rGUIDout)
			{
				if (pInnerObject->QueryInterface(*rGUIDout,pp)==X_OK)
				{
					Unlock();
					return X_OK;							
				}				
			}
			else
			{
				Unlock();
				return X_OK;				
			}
		}
	}
	while (GetNext());

	Unlock();
	return X_FAIL;
}


//--------------------------------------
// match on class ID and Remove
//

template <class T>
int XList<T>::Remove(const X_GUID& rGUID)
{
	Lock();

	T* pInnerObject = NULL;
	T* pOuterObject = NULL;

	if (GetFirst()) do
	{			
		pInnerObject = GetObject();

		if (pInnerObject->QueryInterface(rGUID,(void**)&pOuterObject)==X_OK)
		{
			Remove();
			Unlock();
			return X_OK;
		}
	}
	while (GetNext());

	Unlock();
	return X_FAIL;
}

//--------------------------------------

template <class T>
XLink<T>* XList<T>::CreateLink(T* pT)
{
	XLink<T>* pLink = NULL;

	if (m_flags.IsOn(X_LIST_CHAIN))
	{
		pT->QueryInterface(XLinkID,(void**)&pLink);
	}
	else
	{
		pLink = new XLink<T>(pT);
	}

	return pLink;
}

//--------------------------------------

template <class T>
X_RESULT XList<T>::Reset()
{
	Lock();

	T* pT = GetFirst();

	while (pT)
	{
		pT->Reset();
		pT = GetNext();
	}

	Unlock();
	
	return X_OK;
}

template <class T>
int XList<T>::ReleaseAll()	// deletes all elements in the list
{
	Lock();

	T* pT = GetFirst();

	while (pT)
	{
		Remove(pT);
		X_SAFE_RELEASE(pT);
		pT = GetFirst();
	}
	 
	Unlock();

	return X_OK;
}
