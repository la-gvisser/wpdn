// ============================================================================
//	XMLDictionary.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.

#include "stdafx.h"
#include "XMLDictionary.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif


//#define _DEBUG_DICTIONARY

XXMLDictionary::XXMLDictionary()
{
	m_comp.SetArrays(&m_ayWords, &m_aySort);
	m_aySort.SetComparator(&m_comp, false);
	m_aySort.SetKeepSorted(true);	
}

XXMLDictionary::~XXMLDictionary()
{
	Reset();
}

UInt32 XXMLDictionary::FindWord(LPCTSTR lpszWord)
{	// search through the array (binary search)
	SInt32 nUpper = m_aySort.GetCount() - 1;
	SInt32 nLower = 0;

	while (nUpper >= nLower)
	{
		UInt32 nCur = (nUpper + nLower)/2;
		SInt32 nDiff = m_ayWords[m_aySort[nCur]]->Compare(lpszWord);
		if (nDiff == 0)
			return m_aySort[nCur];
		if (nDiff > 0)	
			nUpper = nCur - 1;
		else
			nLower = nCur + 1;
	}

#ifdef _DEBUG_DICTIONARY
	TRACE(_T("Could not find in XXMLDictionary 0x%08x %s\n"), this, lpszWord);	
#endif
	
	return (UInt32) CArrayEx::kArrayIndex_Invalid;
}

UInt32 XXMLDictionary::AddWord(LPCTSTR lpszWord)
{
	UInt32 nID = FindWord(lpszWord);
	if (nID == (UInt32)CArrayEx::kArrayIndex_Invalid)
	{	// add to the end of the array(s) and re-sort
		nID = m_ayWords.GetCount();
		m_ayWords.SetCount(nID + 1);
		m_aySort.SetCount(nID + 1);
		(*m_ayWords[nID]) = lpszWord;
		m_aySort[nID] = nID;
		m_aySort.InvalidateSort();
		m_aySort.Sort();
	}

	return nID;
}

void XXMLDictionary::Reset()
{
#ifdef _DEBUG_DICTIONARY
	TRACE(_T("XXMLDictionary 0x%08x:\n"), this);
	for (UInt32 nIdx = 0; nIdx < m_aySort.GetCount(); nIdx++)
		TRACE(_T("Word %3d is %s\n"), nIdx, (LPCTSTR) (*m_ayWords[m_aySort[nIdx]]));
#endif
	
	m_ayWords.RemoveAllItems();
	m_aySort.RemoveAllItems();
}
