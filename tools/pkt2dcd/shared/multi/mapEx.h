// ============================================================================
//	mapEx.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002-2004. All rights reserved.

#ifndef MAPEX_H
#define MAPEX_H

#ifdef _WIN32
#include <stddef.h>
#else
#include <stdint.h>
#endif
#include "AGTypes.h"
#include "ArrayEx.h"

typedef void* HASH_POSITION;

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class TMapEx 
{
public:
	TMapEx(SInt32 nHashSize = 257) 
	{	
		m_nHashSize = nHashSize; 
		m_bHashInit = false;
		m_ayHashKeys = NULL;
		m_nHashKeyCount = 0;
		m_ayHashTable = NULL;
		m_nHashTableCount = 0;
		m_bRendered = false;
	}
	
	virtual ~TMapEx()
	{
		Reset();
	}
	
	VALUE* Lookup(ARG_KEY key)
	{
		if (!m_bHashInit)
			return NULL;

		SInt32 nHashKey = HashKey(key) % GetHashSize();
		HashObj* pObj = m_ayHashKeys[nHashKey];

		while (pObj)
		{
			if (pObj->key == key)
				return &pObj->value;
			pObj = pObj->pNext;
		}

		return NULL;
	}

	VALUE* SetAt(ARG_KEY key, ARG_VALUE value)
	{
		ASSERT(!m_bRendered);

		if (!m_bHashInit)
			InitHashSize(m_nHashSize);

		SInt32 nHashKey = HashKey(key) % GetHashSize();
		HashObj* pObj = m_ayHashKeys[nHashKey];

		// replace the first key'd object with the input value
		while (pObj)
		{
			if (pObj->key == key)
			{
				pObj->value = value;
				return &pObj->value;
			}

			pObj = pObj->pNext;
		}

		return Add(key, value);
	}

	VALUE* Add(ARG_KEY key, ARG_VALUE value)
	{
		ASSERT(!m_bRendered);

		if (!m_bHashInit)
			InitHashSize(m_nHashSize);

		SInt32 nCount = GetCount();
		if (!m_ayHashTable)
			m_ayHashTable = (HashObj**) malloc(sizeof(HashObj*));
		else
			m_ayHashTable = (HashObj**) realloc(m_ayHashTable, (nCount + 1) * sizeof(HashObj*));

		m_nHashTableCount++;
		m_ayHashTable[nCount] = new HashObj;
		m_ayHashTable[nCount]->key = key;
		m_ayHashTable[nCount]->value = value;

		SInt32 nHashKey = HashKey(key) % GetHashSize();
		HashObj* pObj = m_ayHashKeys[nHashKey];
		if (pObj != NULL)
			m_ayHashTable[nCount]->pNext = pObj;
		else
			m_ayHashTable[nCount]->pNext = NULL;

		m_ayHashKeys[nHashKey] = m_ayHashTable[nCount];
		return &m_ayHashTable[nCount]->value;
	}

	VALUE* Replace( ARG_KEY keyReplace, VALUE* pReplace, ARG_KEY key, ARG_VALUE value )
	{	// first find it in the key chain
		ASSERT(!m_bRendered);

		SInt32 nHashKey = HashKey(keyReplace) % GetHashSize();
		HashObj* pObj = m_ayHashKeys[nHashKey];

		HashObj* pPrev = NULL;
		while (pObj && (&pObj->value) != pReplace)
		{
			pPrev = pObj;
			pObj = pObj->pNext;
		}
		// did we find it?
		if (!pObj) 
			return NULL;
		// ok, remove it from the chain
		// if it's the first one, remove it from the hash's key
		if ( pPrev == NULL )
			m_ayHashKeys[nHashKey] = pObj->pNext;
		else
			pPrev->pNext = pObj->pNext;
		// now insert pObj back into the hash
		nHashKey = HashKey(key) % GetHashSize();
		pObj->pNext = m_ayHashKeys[nHashKey];
		m_ayHashKeys[nHashKey] = pObj;
		pObj->key = key;
		pObj->value = value;

		return &pObj->value;
	}

	HASH_POSITION GetFirstPosition(const KEY& key)
	{
		if (!m_bHashInit) return NULL;

		SInt32 nHashKey = HashKey(key) % GetHashSize();
		HashObj* pObj = m_ayHashKeys[nHashKey];

		while (pObj && pObj->key != key)
			pObj = pObj->pNext;

		return pObj;
	}

	VALUE* GetNext(const KEY& key, HASH_POSITION& pos)
	{
		ASSERT(m_bHashInit);

		HashObj* pCur = (HashObj*) pos;
		VALUE* pRet = &pCur->value;
		pCur = pCur->pNext;
		while (pCur && pCur->key != key)
			pCur = pCur->pNext;
		pos = pCur;
		return pRet;		
	}

	// keyless walk is like an array access
	VALUE* operator[](SInt32 nIdx)
	{
		ASSERT(m_bHashInit);
		return &m_ayHashTable[nIdx]->value;
	}

	const VALUE* operator[](SInt32 nIdx) const
	{
		ASSERT(m_bHashInit);
		return &m_ayHashTable[nIdx]->value;
	}

	VALUE* GetAt(SInt32 nIdx)
	{
		ASSERT(m_bHashInit);
		return &m_ayHashTable[nIdx]->value;
	}

	const VALUE* GetAt(SInt32 nIdx) const
	{
		ASSERT(m_bHashInit);
		return &m_ayHashTable[nIdx]->value;
	}

	// keyless walk -- get key value
	KEY GetKeyAt(SInt32 nIdx)
	{
		ASSERT(m_bHashInit);
		return m_ayHashTable[nIdx]->key;	// always 1 too big
	}

	void InitHashSize(SInt32 nSize)
	{
		ASSERT(!m_bRendered);

		m_nHashSize = nSize;
		Reset();
		m_ayHashKeys = new HashObj*[nSize];
		memset(m_ayHashKeys, 0, sizeof(SInt32)*nSize);
		m_nHashKeyCount = nSize;
		m_bHashInit = true;
	}

	void Reset()
	{
		ASSERT(!m_bRendered);

		delete [] m_ayHashKeys;
		m_ayHashKeys = NULL;
		for (m_nHashTableCount--; m_nHashTableCount >= 0; m_nHashTableCount--)
			delete m_ayHashTable[m_nHashTableCount];
		if ( m_ayHashTable )
			free( m_ayHashTable );
		m_ayHashTable = NULL;
		m_nHashKeyCount = 0;
		m_nHashTableCount = 0;
		m_bHashInit = false;
	}

	SInt32 GetCount() const
	{
		return m_nHashTableCount;
	}

	SInt32 GetHashSize() const
	{
		return m_nHashKeyCount;
	}

	void SetHashSize(SInt32 nHashTableSize) 
	{
		ASSERT(!m_bRendered);

		Reset();
		m_nHashSize = nHashTableSize;
	}
	
	void GetHashDistribution(TArrayEx<SInt32> &ayCounts) const
	{
		ayCounts.SetCount(GetHashSize());
		for (SInt32 n = 0; n < GetHashSize(); n++)
		{
			const HashObj* pCur =  m_ayHashKeys[n];
			ayCounts[n] = 0;
			while (pCur)
			{
				ayCounts[n]++;
				pCur = pCur->pNext;
			}
		}
	}

	// render format is
	// [class][hash keys][hash obj ptrs][hash objects]
	UInt32 GetRenderSize() const
	{
		return sizeof(*this) +
			   sizeof(HashObj*) * GetHashSize() +
			   (sizeof(HashObj*) + sizeof(HashObj))* GetCount();
	}

	UInt8* Render(UInt8* pBuffer, UInt32 nBufferSize) const
	{
		ASSERT(!m_bRendered);

		UInt32 nSize = sizeof(*this);
		if (nBufferSize < nSize)
			return NULL;
		TMapEx *pNew = (TMapEx*) pBuffer;

#ifndef _LINUX
		pNew->TMapEx<KEY, ARG_KEY, VALUE, ARG_VALUE>::TMapEx(m_nHashSize);
#endif

		pBuffer += nSize;
		nBufferSize -= nSize;
		pNew->m_bHashInit = m_bHashInit;
		pNew->m_nHashKeyCount = m_nHashKeyCount;
		pNew->m_nHashTableCount = m_nHashTableCount;
		// hash keys
		// what we want to do is 
		//	1) lay down the hash key pointers as empty
		//	2) lay down the hash array pointers as empty
		//  3) lay down each object in the key's list in order, recording the start
		//     in the new rendered key map and array, correcting the pNext pointers
		//  so if the key is *A *B *C *D *E
		//  and the array is *B *C *A *D *E *F *H *I
		//  and the key order is A->F B->H C D->I E
		//  then the rendered memory looks like:
		//  keys:	*A *B *C *D *E
		//  array:	*A *F *B *H *C *D *I *E
		//  objects: A  F  B  H  C  D  I  E
		pNew->m_ayHashKeys = (HashObj**) pBuffer;
		nSize = sizeof(HashObj*) * GetHashSize();
		if (nBufferSize < nSize)
			return NULL;
		memset(pNew->m_ayHashKeys, 0, nSize);
		pBuffer += nSize;
		nBufferSize -= nSize;
		// now init the new array of pointers
		pNew->m_ayHashTable = (HashObj**) pBuffer;
		nSize = sizeof(HashObj*)*GetCount();
		if (nBufferSize < nSize)
			return NULL;
		memset(pNew->m_ayHashTable, 0, nSize);
		pBuffer += nSize;
		nBufferSize -= nSize;
		// now render each object from the first key to the last
		// check size first
		if (nBufferSize < GetCount()*sizeof(HashObj))
			return NULL;

		for (SInt32 nTableIdx = 0, nKey = 0; nKey < GetHashSize(); nKey++)
		{
			HashObj* pCur = m_ayHashKeys[nKey];
			pNew->m_ayHashKeys[nKey] = (HashObj*) pBuffer;
			// now follow the chain
			while (pCur)
			{
				HashObj* pCurRendered = (HashObj*) pBuffer;
				*pCurRendered = *pCur;
				pNew->m_ayHashTable[nTableIdx] = pCurRendered;
				nTableIdx++;
				pBuffer += sizeof(HashObj);
				if (pCur->pNext)
					pCurRendered->pNext = (HashObj*) pBuffer;
				else
					pCurRendered->pNext = NULL;

				pCur = pCur->pNext;
			}
		}

		pNew->m_bRendered = true;

		return pBuffer;
	}

	bool IsRendered() const	{	return m_bRendered;	}

protected:
	typedef struct tagHashObj
	{
		KEY key;
		VALUE value;
		tagHashObj* pNext;
	} HashObj;

	HashObj**		m_ayHashKeys;	// maps a hash # to a "first index" pointer
	SInt32			m_nHashKeyCount;
	HashObj**		m_ayHashTable;
	SInt32			m_nHashTableCount;

	SInt32			m_nHashSize;
	bool			m_bHashInit;
	bool			m_bRendered;

	UInt32 HashKey(ARG_KEY key)
	{
		// default identity hash - works for most primitive values
//		return (ULONG( ULONG_PTR(key)));
		return (UInt32)(uintptr_t)key;
	}
};

#endif
