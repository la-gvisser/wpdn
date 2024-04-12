// =============================================================================
//	XMLDictionary.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.

#ifndef XMLDICTIONARY_H
#define XMLDICTIONARY_H

#include "ArrayEx.h"
#include "XString.h"

class XXMLDictComparator: public CComparator
{
public:
	XXMLDictComparator() : m_payWords(NULL), m_paySort(NULL)	{}
	virtual					~XXMLDictComparator()
	{
		m_payWords = NULL;
		m_paySort = NULL;
	}

	virtual SInt32			Compare( const void* inItemOne, const void* inItemTwo,
								UInt32 /*inSizeOne*/, UInt32 /*inSizeTwo*/ ) const
	{
		return (*m_payWords)[*((UInt32*)inItemOne)]->Compare(*((*m_payWords)[*((UInt32*)inItemTwo)]));
	}

	virtual bool			IsEqualTo( const void* inItemOne, const void* inItemTwo,
								UInt32 /*inSizeOne*/, UInt32 /*inSizeTwo*/ ) const
	{
		return ((*m_payWords)[*((UInt32*)inItemOne)]->Compare(*((*m_payWords)[*((UInt32*)inItemTwo)])) == 0);
	}

	static CComparator*		GetComparator()	
	{
		static XXMLDictComparator	s_Comparator;
		return &s_Comparator;
	}

	void SetArrays(TObjArrayEx<XString> *payWords, TArrayEx<UInt32> *paySort)
	{
		m_payWords = payWords;
		m_paySort = paySort;
	}
	
protected:
	TObjArrayEx<XString>	*m_payWords;	// holds the words, new ones always go on the end
	TArrayEx<UInt32>	*m_paySort;	// hold the indexed sort, indicies are for m_ayWords
};

class XXMLDictionary
{
public:
	XXMLDictionary();
	virtual ~XXMLDictionary();

	UInt32 AddWord(LPCTSTR lpszWord);
	const XString& GetWord(UInt32 nID)	const	{	return (*m_ayWords[nID]);	}

	void Reset();

	UInt32 FindWord(LPCTSTR lpszWord);

	UInt32 GetFirstWordID()	const {	return (m_aySort.GetCount()) ? 0 : CArrayEx::kArrayIndex_Invalid;	}
	LPCTSTR GetNextWord(UInt32& nID) const
	{
		LPCTSTR lpszRet = (*m_ayWords[m_aySort[nID]]);
		nID++;
		if (m_aySort.GetCount() == nID)
			nID = (UInt32) CArrayEx::kArrayIndex_Invalid;

		return lpszRet;
	}

protected:
// data	
	TObjArrayEx<XString>	m_ayWords;	// holds the words, new ones always go on the end
	TArrayEx<UInt32>	m_aySort;	// hold the indexed sort, indicies are for m_ayWords

	XXMLDictComparator	m_comp;
};

#endif
