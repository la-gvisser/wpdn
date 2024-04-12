// =============================================================================
//	XTools.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2001-2004. All rights reserved.


#include "stdafx.h"
#include "XTools.h"
#include "XObject.h"



static int compare(const void *elem1, const void *elem2 )
{
	XObject * pObject1 = *(XObject**)elem1;
	XObject * pObject2 = *(XObject**)elem2;	

	XCompare* pCompare1 = NULL;	
	XCompare* pCompare2 = NULL;

	int ret1 = pObject1->QueryInterface(XCompareID, (void**)&pCompare1);
	int ret2 = pObject2->QueryInterface(XCompareID, (void**)&pCompare2);
	
	if ((ret1 == X_OK) && (ret2 == X_OK))
	{
		int result = pCompare1->Compare( *pCompare2 );
		return result;
	}

	return X_FAIL;
}


void XTools::Sort(void* list, int size, compareFunc pFunc)
{
	if (pFunc == NULL)
	{
		pFunc = compare;
	}

	qsort(
		list,
		size,
		sizeof(XObject*),
		pFunc
	);
}

