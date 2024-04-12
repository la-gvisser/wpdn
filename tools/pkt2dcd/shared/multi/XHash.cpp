

//------------------------------------------

template<class T>
XHash<T>::XHash() :
	m_ppTable		(NULL),
	m_iMaxQueues	(0),
	m_iTotal		(0),
	m_isClearing	(false)
{

}

//------------------------------------------

template<class T>
XHash<T>::~XHash()
{ 
	Clear();

	if (m_ppTable)
	{
		delete [] m_ppTable;
		m_ppTable = NULL;
	}
}

//------------------------------------------

template<class T>
void XHash<T>::SetHashSize(UInt32 iMaxQueues)
{ 
	if (m_iMaxQueues == iMaxQueues)
	{
		return;
	}

	Clear();

	m_iMaxQueues = iMaxQueues;

	if (m_ppTable)
	{
		delete [] m_ppTable;
		m_ppTable = NULL;
	}

	if (m_iMaxQueues)
	{

		m_ppTable = new XLink<T>*[ m_iMaxQueues ];	

		for (UInt32 i=0; i < m_iMaxQueues; i++)
		{
			m_ppTable[i] = NULL;
		}
	}
}


//--------------------------------------

template <class T>
XLink<T>* XHash<T>::CreateLink(T* pT)
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
int XHash<T>::SetChain(bool bChain)
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


//------------------------------

template<class T>
int	XHash<T>::Clear()
{
	m_isClearing = true;
	XLock lock(this);

	for (UInt32 iKey=0; iKey < m_iMaxQueues; iKey++)
	{
		m_ppTable[ iKey ] = NULL;
	}

	m_iTotal = 0;
	m_isClearing = false;
	

	return X_OK;
}


//------------------------------

template<class T>
T* XHash<T>::GetItem(XKey32* pKey)
{
	UInt32 iKey = pKey->GetKey();

	return GetItem(iKey);
}

//------------------------------

template<class T>
T* XHash<T>::GetItem(UInt32 iKey)
{
	_ASSERT(iKey < m_iMaxQueues);

	if (iKey >= m_iMaxQueues)
	{
		return NULL;
	}

	T* pT = NULL;

	XLock lock(this);

	XLink<T>* pLink =  m_ppTable[ iKey ];
	
	if (pLink)
	{
		pT = pLink->GetObject();
	}

	

	return pT;
}

//------------------------------

template<class T>
XLink<T>* XHash<T>::GetLink(UInt32 iKey)
{
	_ASSERT(iKey < m_iMaxQueues);

	if (iKey >= m_iMaxQueues)
	{
		return NULL;
	}

	XLock lock(this);

	XLink<T>* pLink =  m_ppTable[ iKey ];
	
	

	return pLink;
}

//------------------------------

template<class T>
int	XHash<T>::PutItem(XKey32* pKey, T* pT)
{
	UInt32 iKey = pKey->GetKey();
	return PutItem(iKey, pT);
}

//------------------------------

template<class T>
int	XHash<T>::PutItem(UInt32 iKey, T* pT)
{
	_ASSERT(iKey < m_iMaxQueues);

	if (iKey >= m_iMaxQueues)
	{
		return X_FAIL;
	}

#if 0
	if (m_isClearing)
	{
		XLock(this);
		
	}
#endif

	XLock lock(this);

	XLink<T>* pOldLink = m_ppTable[ iKey ];

	if (pOldLink && IsOwned())
	{
		// this is important because SAFE_DELETE sets to NULL
		T* pTemp = pOldLink->GetObject();
		X_SAFE_DELETE( pTemp );
		X_SAFE_DELETE( pOldLink );
	}

	XLink<T>* pLink = new XLink<T>;
	pLink->SetObject( pT );

	m_ppTable[ iKey ] = pLink;

	if (pOldLink && (pT == NULL))
	{
		m_iTotal--;
	}
	else
	if ((pOldLink == NULL) && pT)
	{
		m_iTotal++;
	}
	// otherwise it was a replace so leave the total is the same

	

	return X_OK;
}


//------------------------------

template<class T>
UInt32 XHash<T>::GetCount()
{
	return m_iTotal;
}

//------------------------------

template<class T>
UInt32 XHash<T>::GetMaxQueues()
{
	return m_iMaxQueues;
}

//------------------------------

#if 0

template<class T>
int XHash<T>::Write(FILE* fd)
{
	int n = 0;
	UInt32 i = 0;

	/*Write how many pairs there are*/
	n = fwrite(&(m_iTotal), sizeof(UInt32), 1, fd);
	if(n != 1) goto bad;

   /*Go through each hash entry and log the linked list*/
	for(i = 0; i < m_iMaxQueues; i++) 
	{
		XLink<T>* pLink = m_ppTable[i];

		if (pLink)
		{
			T* pT = pLink->GetObject();
			n = pT->Write(fd);
			if(n < 0) goto bad;
		}
	}

	return X_OK;
	bad:
	return X_FAIL;
}

#endif

//------------------------------

template<class T>
UInt32 XHash<T>::GetHashSize()
{
	return m_iMaxQueues;
}

//------------------------------

template<class T>
UInt32 XHash<T>::GetTotal()
{
	return m_iTotal;
}

//----------------------------------------

template <class T>
XLinkedHash<T>::~XLinkedHash()
{ 
	Clear();
}

//-------------------------------

template <class T>	
int	XLinkedHash<T>::Clear()
{
	m_isClearing = true;
	XLock lock(this);

	for (UInt32 iKey=0; (iKey < m_iMaxQueues) && m_iTotal; iKey++)
	{
		XLink<T>* pLink = m_ppTable[ iKey ];
		XLink<T>* pNext = NULL;

		while (pLink)
		{
			pNext = pLink->GetNext();

			if (IsOwned())
			{
				T* pT = pLink->GetObject();
				X_SAFE_RELEASE( pT );
			}

			m_iTotal--;
			X_SAFE_RELEASE( pLink );

			pLink = pNext;
		}
	}

	_ASSERT(m_iTotal == 0);

	XHash<T>::Clear();

	m_isClearing = false;
	
	

	return X_OK;
}

//-----------------------------------------------
// this method can be used to add a whole chain
// of objects.  It will increase the m_iTotal 
// correctly/

template <class T>	
int	XLinkedHash<T>::PutItem(UInt32 iKey, T* pT)
{ 
	_ASSERT(pT);
	_ASSERT(iKey < m_iMaxQueues);

	if (iKey >= m_iMaxQueues)
	{
		return X_FAIL;
	}

	XLock lock(this);

	XLink<T>* pLink = CreateLink( pT );
	if (pLink==NULL) return X_FAIL;

	pLink->SetObject( pT );

	/*
	XKey32* pKey = NULL;
	pT->QueryInterface(XKeyID,(void**)&pKey);
	_ASSERT(pKey);
	pKey->SetKey( iKey );
	*/

/*
	//in case pT is a list
	T* pNext = pT;
	XObject* pObject = NULL;
	XLink<T>* pLink = NULL;
	
	while (pNext)
	{
		pNext->QueryInterface(XLinkID,(void**)&pLink);
		_ASSERT(pLink);

		pNext = NULL;			
		pObject = pLink->GetNextObject();
		
		if (pObject)
		{
			_ASSERT(pObject != pT);
			m_iTotal++;
			pObject->QueryInterface(T::GetClassID(),(void**)&pNext);
		}
	}
*/

	XLink<T>* pHead = m_ppTable[ iKey ];
	
	if (pHead)
	{
		_ASSERT(pHead != pLink);

		pLink->InsertNext( pHead );
	}

	m_ppTable[ iKey ] = pLink;
	m_iTotal++;

	

	return X_OK;
}

//-----------------------------------

template <class T>	
int	XLinkedHash<T>::PutItem(T* pT)
{ 
	_ASSERT(pT);

	UInt32 iKey = 0;

	XCalcHash* pCalcHash = NULL;

	if (pT->QueryInterface(XCalcHashID,(void**)&pCalcHash)==X_FAIL)
	{
		return X_FAIL;
	}

	iKey = pCalcHash->CalcHash( m_iMaxQueues );

	return PutItem( iKey, pT );
}

//----------------------------------

template <class T>	
T* XLinkedHash<T>::GetItem(UInt32 iKey)
{
	_ASSERT(iKey < m_iMaxQueues);

	if (iKey >= m_iMaxQueues)
	{
		return NULL;
	}

	T* pT = NULL;

	XLock lock(this);

	XLink<T>* pLink =  m_ppTable[ iKey ];

	if (pLink)
	{
		pT = pLink->GetObject();
	}

	

	return pT;
}

//----------------------------------

template <class T>	
T* XLinkedHash<T>::GetItem(T& pClone)
{
	XLock lock(this);

	T*				pT				= NULL;
	UInt32			iKey			= 0;
	XCalcHash*	pCalcHash		= NULL;
	XCompare*		pCloneCompare	= NULL;
	XCompare*		pCompare		= NULL;

	if  (  (pClone.QueryInterface(XCalcHashID,(void**)&pCalcHash) == X_OK)
		&& (pClone.QueryInterface(XCompareID,(void**)&pCloneCompare) == X_OK) )
	{
		iKey = pCalcHash->CalcHash( m_iMaxQueues );

		_ASSERT(iKey < m_iMaxQueues);

		if (iKey < m_iMaxQueues)
		{		
			XLink<T>* pLink = m_ppTable[ iKey ];

			while (pLink)
			{
				pT = pLink->GetObject();

				if (pT->QueryInterface(XCompareID,(void**)&pCompare) == X_OK)
				{
					if (pCloneCompare->Compare( *pCompare ) == 0)
					{
						break;
					}
				}

				pT = NULL;
				pLink = pLink->GetNext();
			}
		}
	}

	

	return pT;
}


//------------------------------

template <class T>	
T* XLinkedHash<T>::Find(UInt32 iKey, T* pT)
{
	_ASSERT(pT);
	_ASSERT(iKey < m_iMaxQueues);
	
	XLock lock( this );

	XLink<T>* pLink = m_ppTable[ iKey ];
	if (pLink == NULL)
	{
		return NULL;
	}

	T* pTest = pLink->GetObject();

	while (pTest)
	{
		if (*pTest == *pT)
		{
			break;
		}

		pLink = pLink->GetNext();

		if (pLink == NULL)
		{
			return NULL;
		}

		pTest = pLink->GetObject();
	}

	return pTest;
}

//------------------------------

template <class T>	
int XLinkedHash<T>::Remove( UInt32 iKey, T* pT)
{
	_ASSERT(pT);
	_ASSERT(m_iTotal);
	_ASSERT(iKey < m_iMaxQueues);

	XLock lock( this );
		
/*
	// Set the key to invalid state
	XKey32* pKey = NULL;
	pT->QueryInterface(XKeyID,(void**)&pKey);
	_ASSERT(pKey);	
	pKey->SetKey( (UInt32)-1 );

	XLink<T>* pLink = NULL;
	pT->QueryInterface(XLinkID,(void**)&pLink);
	_ASSERT(pLink);	
	
	T* pPrev = pLink->GetPrevObject();
	T* pNext = pLink->GetNextObject();	

	_ASSERT(pPrev != pT);
	_ASSERT(pNext != pT);
*/

	XLink<T>* pLink = m_ppTable[ iKey ];
	
	int nResult = X_FAIL;

	while (pLink)
	{
		T* pTest = pLink->GetObject();
		if (pTest == pT)
		{
			XLink<T>* pPrev = pLink->GetPrev();
			XLink<T>* pNext = pLink->GetNext();	
			pLink->Remove();

			//if there is no prev then we are the head so
			//set the head to next even if it is NULL.
			if (pPrev == NULL)
			{
				m_ppTable[ iKey ] = pNext;
			}

			delete pLink;
			m_iTotal--;
			nResult = X_OK;
			break;
		}
		pLink = pLink->GetNext();
	}

	return nResult;
}


//------------------------------

template <class T>	
int XLinkedHash<T>::Remove( T* pT )
{
	_ASSERT(pT);
	_ASSERT(m_iTotal);

	XLock lock( this );
	
	for (UInt32 iKey = 0; iKey < m_iMaxQueues; iKey++ )
	{
		XLink<T>* pLink = m_ppTable[ iKey ];
		
		while (pLink)
		{
			T* pTest = pLink->GetObject();
			if (pTest == pT)
			{
				XLink<T>* pPrev = pLink->GetPrev();
				XLink<T>* pNext = pLink->GetNext();	
				pLink->Remove();

				//if there is no prev then we are the head so
				//set the head to next even if it is NULL.
				if (pPrev == NULL)
				{
					m_ppTable[ iKey ] = pNext;
				}

				delete pLink;
				m_iTotal--;
				return X_OK;
			}
			pLink = pLink->GetNext();
		}
	}

	return X_OK;
}

//------------------------------

#if 0

template <class T>	
int XLinkedHash<T>::Write(FILE* fd)
{
	T* pT = NULL;
	int n = 0;
	UInt32 i = 0;
	UInt32 iTotal = 0;

	XLock lock(this);

	//Write how many pairs there are
	n = fwrite(&(m_iTotal), sizeof(UInt32), 1, fd);
	if(n != 1) goto bad;

	//Go through each hash entry and log the linked list
	for(i = 0; i < m_iMaxQueues; i++) 
	{
		XLink<T>* pLink = m_ppTable[i];

		while(pLink != NULL) 
		{
			pT = pLink->GetObject();

			iTotal++;

			n = fwrite(&(i), sizeof(UInt32), 1, fd);
			if(n != 1) goto bad;

			n = pT->Write(fd);
			if(n < 0) goto bad;

			pLink = pLink->GetNext();
		}
	}

	_ASSERT(iTotal == m_iTotal);

	
	return X_OK;
  bad:
	
	return X_FAIL;
}

//------------------------------

template <class T>	
int XLinkedHash<T>::Read(FILE *fd)
{
	UInt32 i;
	int n;
	T* pT;
	UInt32 iKey = 0;

	XLock lock(this);

	if(m_iTotal != 0) 
	{
		return X_FAIL;
	}

	//Write how many pairs there are
	UInt32 iTotal;
	n = fread(&(iTotal), sizeof(UInt32), 1, fd);
	if(n != 1) goto bad;

	//We don't know the key generation function so
	//put everything in key=0
	for(i = 0; i < iTotal; i++)
	{
		n = fread(&(iKey), sizeof(UInt32), 1, fd);
		if(n != 1) goto bad;

		//We're pretty sure this casting is safe 
		//because of the static function call.
		//Compiler didn't like it though.
		
		int ret = T::Create( &pT ); 
		
		if (ret != X_OK || pT == NULL)
		{
			return X_FAIL;
		}

		n = pT->Read(fd);
		if(n != X_OK) goto bad;

		PutItem(iKey, pT);

	}

	_ASSERT(m_iTotal == iTotal);

	
	return X_OK;
  bad:

	_ASSERT(m_iTotal == iTotal);

	
	return X_FAIL;

}

#endif

//-------------------------------------

template <class Y, class T>
XLinkedHashIterator<Y,T>::XLinkedHashIterator(Y* pHash) :
	m_pCurrent	( NULL ),
	m_iCurrent	( 0 ),
	m_pHash		( pHash )
{
	
}

//-------------------------------------

template <class Y, class T>
T* XLinkedHashIterator<Y,T>::GetNext()
{
	T* pT = NULL;

	UInt32 iMax = m_pHash->GetMaxQueues();

	XLock lock(m_pHash);

	while (m_iCurrent < iMax)
	{ 
		if (m_pCurrent)
		{
			m_pCurrent = m_pCurrent->GetNext();
		}
		else
		{
			m_iCurrent++;

			if (m_iCurrent < iMax)
			{
				m_pCurrent = m_pHash->GetLink( m_iCurrent );
			}
		}

		if (m_pCurrent)
		{
			pT = m_pCurrent->GetObject();
			break;
		}
	}


	return pT;
}

//-------------------------------------

template <class Y, class T>
T* XLinkedHashIterator<Y,T>::GetFirst()
{
	T* pT = NULL;
	m_iCurrent = 0;
	m_pCurrent = NULL;

	UInt32 iMax = m_pHash->GetMaxQueues();

	XLock lock(m_pHash);

	while (m_iCurrent < iMax)
	{
		m_pCurrent = m_pHash->GetLink( m_iCurrent );

		if (m_pCurrent)
		{
			pT = m_pCurrent->GetObject();
			break;
		}
		else
		{
			m_iCurrent++;
		}
	}


	return pT;
}



//--------------------------------------

template <class Y, class T>
XLink<T>* XMap<Y,T>::CreateLink(T* pT)
{
	XLink<T>* pLink = NULL;

	if (m_flags.IsOn(X_LIST_CHAIN))
	{
		pT->QueryInterface(XLinkID,(void**)&pLink);
	}
	else
	{
		pLink = new XMapLink<Y,T>(pT);
	}

	return pLink;
}

//-----------------------------------------------
// this method can be used to add a whole chain
// of objects.  It will increase the m_iTotal 
// correctly/

template <class Y, class T>	
int	XMap<Y,T>::SetAt( const Y& key, T* pT)
{ 
	_ASSERT(pT);

	XLock lock(this);

	XLink<T>* pLink = CreateLink( pT );
	if (pLink==NULL) return X_FAIL;
	pLink->SetObject( pT );

	// special code for setting key
	XMapLink<Y,T>* pMapLink;
	pLink->QueryInterface( XMapLinkID, (void**)&pMapLink );
	ASSERT( pMapLink );
	pMapLink->SetKey( key );

	UInt32 iKey = key.CalcHash( m_iMaxQueues );
	_ASSERT(iKey < m_iMaxQueues);
	if (iKey >= m_iMaxQueues) return X_FAIL;

	XLink<T>* pHead = m_ppTable[ iKey ];
	
	if (pHead)
	{
		_ASSERT(pHead != pLink);
		pLink->InsertNext( pHead );
	}

	m_ppTable[ iKey ] = pLink;
	m_iTotal++;

	

	return X_OK;
}

//----------------------------------

template <class Y, class T>	
T* XMap<Y,T>::Lookup( const Y& key)
{
	XLock lock(this);

	T*				pT				= NULL;
	UInt32			iKey			= 0;
	XCalcHash*		pCalcHash		= NULL;
	XCompare*		pCloneCompare	= NULL;

	if  (  (key.QueryInterface(XCalcHashID,(void**)&pCalcHash) == X_OK)
		&& (key.QueryInterface(XCompareID,(void**)&pCloneCompare) == X_OK) )
	{
		iKey = pCalcHash->CalcHash( m_iMaxQueues );

		_ASSERT(iKey < m_iMaxQueues);

		if (iKey < m_iMaxQueues)
		{		
			XLink<T>* pLink = m_ppTable[ iKey ];

			while (pLink)
			{
				pT = pLink->GetObject();
				XMapLink<Y,T>* pMapLink = NULL;
				pLink->QueryInterface( XMapLinkID, (void**)&pMapLink );
				ASSERT( pMapLink );
				if (pMapLink == NULL) return NULL;
				XCompare::SmartPtr pCompare( pMapLink->GetKeyPtr() );
				ASSERT( pCompare );

				if (pCompare->Compare( *pCloneCompare ) == 0)
				{
					break;
				}

				pT = NULL;
				pLink = pLink->GetNext();
			}
		}
	}

	

	return pT;
}


//----------------------------------

template <class Y, class T>	
int XMap<Y,T>::RemoveKey( const Y& key)
{
	_ASSERT(m_iTotal);

	int nResult = X_FAIL;

	XLock lock( this );

	XCalcHash*	pCalcHash		= NULL;
	XCompare*	pCloneCompare	= NULL;

	if  (  (key.QueryInterface(XCalcHashID,(void**)&pCalcHash) == X_OK)
		&& (key.QueryInterface(XCompareID,(void**)&pCloneCompare) == X_OK) )
	{
		UInt32 iKey = pCalcHash->CalcHash( m_iMaxQueues );
		_ASSERT(iKey < m_iMaxQueues);

		XLink<T>* pLink = m_ppTable[ iKey ];
		
		while (pLink)
		{
			XMapLink<Y,T>* pMapLink = NULL;
			pLink->QueryInterface( XMapLinkID, (void**)&pMapLink );
			if (pMapLink == NULL) return X_FAIL;

			if (pCloneCompare->Compare( pMapLink->GetKey() ) == 0)
			{
				XLink<T>* pPrev = pLink->GetPrev();
				XLink<T>* pNext = pLink->GetNext();	
				pLink->Remove();

				//if there is no prev then we are the head so
				//set the head to next even if it is NULL.
				if (pPrev == NULL)
				{
					m_ppTable[ iKey ] = pNext;
				}

				delete pLink;
				m_iTotal--;
				nResult = X_OK;
				break;
			}
			pLink = pLink->GetNext();
		}
	}

	return nResult;
}

#ifdef _LINUX

//-----------------------------------------------

template <class T>	
int	XGUIDMap<T>::SetAt( const X_GUID& key, T* pT)
{ 
	XString xsKey;
	XGUIDUtil::XGUIDToXString( key, xsKey );
	return m_Map.SetAt( xsKey, pT );
}

//----------------------------------

template <class T>	
T* XGUIDMap<T>::Lookup( const X_GUID& key)
{
	XString xsKey;
	XGUIDUtil::XGUIDToXString( key, xsKey );
	T* pT = m_Map.Lookup( xsKey );
	return pT;
}

//----------------------------------

template <class T>	
int XGUIDMap<T>::RemoveKey( const X_GUID& key)
{
	XString xsKey;
	XGUIDUtil::XGUIDToXString( key, xsKey );
	return m_Map.RemoveKey( xsKey );
}

//----------------------------------

template <class T>	
int XGUIDMap<T>::RemoveAll()
{
	return m_Map.Clear();	
}

#endif