// =============================================================================
//	ArrayEx.cpp
// =============================================================================
//	Copyright (c) AG Group, Inc. 1998-2005. All rights reserved.
//	Copyright (c) WildPackets, Inc. 2000-2001. All rights reserved.

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ArrayEx.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------
//		CArrayEx
// -----------------------------------------------------------------------------

CArrayEx::CArrayEx()
{
	InitArray( sizeof(void*), NULL, true, false );
}


// -----------------------------------------------------------------------------
//		CArrayEx(UInt32,CComparator*,bool)
// -----------------------------------------------------------------------------

CArrayEx::CArrayEx(
	UInt32			inItemSize,
	CComparator*	inComparator,
	bool			inKeepSorted )
{
	InitArray( inItemSize, inComparator, true, inKeepSorted );
}


// -----------------------------------------------------------------------------
//		CArrayEx(UInt32,UInt32,CComparator*,bool)
// -----------------------------------------------------------------------------

CArrayEx::CArrayEx(
	UInt32			inItemCount,
	UInt32			inItemSize,
	CComparator*	inComparator,
	bool			inKeepSorted )
{
	InitArray( inItemSize, inComparator, true, inKeepSorted );
	AdjustAllocation( inItemCount );
}


// -----------------------------------------------------------------------------
//		CArrayEx(const CArrayEx&)
// -----------------------------------------------------------------------------

CArrayEx::CArrayEx(
	const CArrayEx&	inOriginal )
{
	// In case called for copy with same object on both sides of = sign.
	if ( &inOriginal == this )
	{
		return;
	}

	CopyArray( inOriginal );
}


// -----------------------------------------------------------------------------
//		~CArrayEx
// -----------------------------------------------------------------------------

CArrayEx::~CArrayEx()
{
	DestroyArray();
}


// -----------------------------------------------------------------------------
//		operator=
// -----------------------------------------------------------------------------

CArrayEx&
CArrayEx::operator=(
	const CArrayEx&	inArray )
{
	if ( this != &inArray )
	{
		// Destroy the current array storage.
		DestroyArray();

		// Copy the array.
		CopyArray( inArray );
	}

	return *this;
}


// -----------------------------------------------------------------------------
//		InitArray
// -----------------------------------------------------------------------------

void
CArrayEx::InitArray(
	UInt32			inItemSize,
	CComparator*	inComparator,
	bool			inIsSorted,
	bool			inKeepSorted )
{
	m_nItemSize = inItemSize;
	m_nCount = 0;
	m_nDataStored = 0;
	m_nDataAllocated = 0;
	m_bIsSorted = inIsSorted;
	m_bIsKeptSorted = inKeepSorted;
	m_bOwnsComparator = true;
	m_pComparator = inComparator;
	m_pQCompareFunc = NULL;
	m_pData = NULL;

	if ( inComparator == NULL )
	{
		m_bOwnsComparator = false;
		if ( inItemSize == sizeof(UInt32) )
		{
			m_pComparator = CLongComparator::GetComparator();
		}
		else
		{
			m_pComparator = CComparator::GetComparator();
		}
	}
}


// -----------------------------------------------------------------------------
//		DestroyArray
// -----------------------------------------------------------------------------

void
CArrayEx::DestroyArray()
{
	if ( m_bOwnsComparator )
	{
		// Delete comparator object.
		delete m_pComparator;
		m_pComparator = NULL;
	}

	if ( m_pData != NULL )
	{
		// Free the array storage.
		free( m_pData );
		m_pData = NULL;
	}
}


// -----------------------------------------------------------------------------
//		CopyArray
// -----------------------------------------------------------------------------

void
CArrayEx::CopyArray(
	const CArrayEx&	inOriginal )
{
	// Duplicate original's items. Original may have more space
	// allocated than needed to store its items. To save memory,
	// our copy allocates only what it needs to store the data
	// and not any excess space.
	m_nDataStored = inOriginal.m_nDataStored;
	m_nDataAllocated = m_nDataStored;
	m_pData = NULL;

	if ( m_nDataStored > 0 )
	{
		// Allocate space.
		m_pData = malloc( m_nDataStored );
		if ( m_pData == NULL ) throw -1;

		// Copy the array data.
		memcpy( m_pData, inOriginal.m_pData, m_nDataStored );
	}

	// Copy comparator.
	m_bOwnsComparator = inOriginal.m_bOwnsComparator;
	if ( m_bOwnsComparator && (m_pComparator != NULL) )
	{
		// Original owns its comparator.
		try
		{
			// So we must clone our own copy.
			m_pComparator = inOriginal.m_pComparator->Clone();
		}
		catch ( ... )
		{
			// Clone failed.
			if ( m_pData != NULL )
			{
				free( m_pData );
				m_pData = NULL;
			}

			// Rethrow the exception.
			throw -1;
		}
	}
	else
	{
		// Original shares the comparator, so
		// we can just copy the pointer.
		m_pComparator = inOriginal.m_pComparator;
	}

	// Copy original's data members
	m_nItemSize = inOriginal.m_nItemSize;
	m_nCount = inOriginal.m_nCount;
	m_bIsSorted = inOriginal.m_bIsSorted;
	m_bIsKeptSorted = inOriginal.m_bIsKeptSorted;
}


// -----------------------------------------------------------------------------
//		SetCount
// -----------------------------------------------------------------------------

void
CArrayEx::SetCount(
	UInt32	inCount )
{
	// Set the new count.
	m_nCount = inCount;
	// Adjust the allocation.
	AdjustAllocation( 0 );
	// the user forced the count up, so make sure we track that as "stored" data
	m_nDataStored = m_nItemSize*m_nCount;
}


// -----------------------------------------------------------------------------
//		GetItem
// -----------------------------------------------------------------------------

bool
CArrayEx::GetItem(
	SInt32	inIndex,
	void*	outItem ) const
{
	// Sanity check.
	if ( m_pData == NULL ) return false;
	if ( !IsValidIndex( inIndex ) ) return false;

	// Copy the item from the array data.
	PeekItem( inIndex, outItem );

	return true;
}


// -----------------------------------------------------------------------------
//		GetItem
// -----------------------------------------------------------------------------

bool
CArrayEx::GetItem(
	SInt32	inIndex,
	void*	outItem,
	UInt32&	ioItemSize ) const
{
	// Sanity check.
	if ( m_pData == NULL ) return false;
	if ( !IsValidIndex( inIndex ) ) return false;

	// Get the item size.
	UInt32	nItemSize = GrabItemSize( inIndex );

	if ( nItemSize == ioItemSize )
	{
		// Copy the item from the array data.
		PeekItem( inIndex, outItem );
	}
	else
	{
		// Contrain to the size given.
		if ( nItemSize > ioItemSize )
		{
			nItemSize = ioItemSize;
		}

		// Copy the item.
		memcpy( outItem, GetItemPtr( inIndex ), nItemSize );

		// Return the size copied.
		ioItemSize = nItemSize;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		AddItem
// -----------------------------------------------------------------------------

SInt32
CArrayEx::AddItem(
	const void*	inItem,
	UInt32		inItemSize )
{
	if ( IsKeptSorted() )
	{
		return InsertItems( 1, kArrayIndex_Last, inItem, inItemSize );
	}

	if ( inItemSize == 0 )
	{
		inItemSize = m_nItemSize;
	}

	// Add item at end.
	SInt32	nIndex = (SInt32) m_nCount;

	// Array is no longer sorted.
	InvalidateSort();

	AdjustStorage( 1, (SInt32) inItemSize );
	PokeItem( nIndex, inItem, inItemSize );

	return nIndex;
}


// -----------------------------------------------------------------------------
//		InsertItems
// -----------------------------------------------------------------------------

SInt32
CArrayEx::InsertItems(
	UInt32		inCount,
	SInt32		inIndex,
	const void*	inItem,
	UInt32		inItemSize )
{
	// Sanity check.
	if ( inCount < 1 ) return kArrayIndex_Invalid;

	// Can't insert NULL item into a sorted array.
	if ( IsKeptSorted() && (inItem == NULL) ) return kArrayIndex_Invalid;

	// Enforce upper bound.
	if ( inIndex > (SInt32) m_nCount ) inIndex = (SInt32) m_nCount;

	// Enforce lower bound.
	if ( inIndex < 0 ) inIndex = 0;

	// Check for default item size.
	if ( inItemSize == 0 )
	{
		inItemSize = m_nItemSize;
	}

	// Adjust the insertion index for sorted arrays.
	if ( IsKeptSorted() )
	{
		if ( !IsSorted() )
		{
			// Update sort.
			Sort();
		}

		// Get the proper insertion index.
		inIndex = GetInsertIndexOf( inItem, inItemSize );
	}

	// Check upper and lower bound.
	if ( inIndex > (SInt32) m_nCount )
	{
		// Index too big, insert at end.
		inIndex = (SInt32) m_nCount;
	}
	else if ( inIndex < 0 )
	{
		// Index too small, insert at start.
		inIndex = 0;
	}

	UInt32	nSavedCount = m_nCount;
	AdjustStorage( (SInt32) inCount, (SInt32) (inCount * inItemSize) );

	if ( (inIndex < (SInt32) nSavedCount) && (nSavedCount > 0) )
	{
		// Move existing items to make room for new ones.
		ShiftItems( inIndex, nSavedCount - 1,
			(SInt32) inCount, (SInt32)(inCount * inItemSize) );
	}

	m_bIsSorted = m_bIsKeptSorted;

	if ( inItem != NULL )
	{
		// Inserted items are all set to the same value.
		SInt32	nLastIndex = inIndex + inCount - 1;
		for ( SInt32 i = inIndex; i <= nLastIndex; i++ )
		{
			PokeItem( i, inItem, inItemSize );
		}
	}

	return inIndex;
}


// -----------------------------------------------------------------------------
//		RemoveAllItems
// -----------------------------------------------------------------------------

void
CArrayEx::RemoveAllItems()
{
	// Reset the count.
	m_nCount = 0;

	// Reset storage counters.
	m_nDataStored = 0;
	m_nDataAllocated = 0;

	if ( m_pData != NULL )
	{
		// Free the array storage.
		free( m_pData );
		m_pData = NULL;
	}
}


// -----------------------------------------------------------------------------
//		RemoveItems
// -----------------------------------------------------------------------------

void
CArrayEx::RemoveItems(
	UInt32	inCount,
	SInt32	inIndex )
{
	// Sanity check.
	if ( m_pData == NULL ) return;
	if ( m_nCount == 0 ) return;
	if ( inCount < 1 ) return;

	// Enforce upper bound.
	if ( inIndex == kArrayIndex_Last )
	{
		inIndex = m_nCount - 1;
	}

	// Validate the index.
	if ( !IsValidIndex( inIndex ) ) return;

	// Limit inCount to the number of items from
	// inIndex to the end of the array.
	if ( (inIndex + inCount) >= m_nCount )
	{
		inCount = m_nCount - inIndex;
	}

	// Calculate the number of bytes to remove.
	SInt32	nBytesRemoved = GrabItemRangeSize( inIndex, inIndex + (SInt32) inCount - 1 );

	if ( (inIndex + inCount) <= m_nCount - 1 )
	{
		// Removing items from the middle. Shift down items
		// that are above the one being removed.
		ShiftItems( inIndex + (SInt32) inCount, (SInt32) m_nCount - 1,
			-((SInt32)inCount), -nBytesRemoved );
	}

	AdjustStorage( -((SInt32) inCount), -nBytesRemoved );
}


// -----------------------------------------------------------------------------
//		RemoveItem
// -----------------------------------------------------------------------------

void
CArrayEx::RemoveItem(
	const void*	inValue,
	UInt32		inItemSize )
{
	// Find the item in the array.
	SInt32	nIndex = GetIndexOf( inValue, inItemSize );
	
	if ( nIndex != kArrayIndex_Invalid )
	{
		// Remove the item.
		RemoveItems( 1, nIndex );
	}
}


// -----------------------------------------------------------------------------
//		SetItems
// -----------------------------------------------------------------------------

SInt32
CArrayEx::SetItems(
	UInt32		inCount,
	SInt32		inIndex,
	const void*	inValue,
	UInt32		inItemSize )
{
	// Sanity check.
	if ( m_pData == NULL ) return kArrayIndex_Invalid;
	if ( inCount < 1 ) return kArrayIndex_Invalid;

	// Enforce upper bound.
	if ( inIndex == kArrayIndex_Last )
	{
		inIndex = m_nCount - 1;
	}

	// Validate the index.
	if ( !IsValidIndex( inIndex ) ) return kArrayIndex_Invalid;

	if ( IsKeptSorted() )
	{
		RemoveItems( inCount, inIndex );
		inIndex = InsertItems( inCount, inIndex, inValue, inItemSize );
	}
	else
	{
		SInt32	nLastIndex = inIndex + (SInt32) inCount - 1;
		if ( nLastIndex > (SInt32) m_nCount )
		{
			// Don't go past end of array.
			nLastIndex = (SInt32) m_nCount - 1;
		}

		if ( inItemSize == 0 )
		{
			inItemSize = m_nItemSize;
		}

		for ( SInt32 i = inIndex; i <= nLastIndex; i++ )
		{
			PokeItem( i, inValue, inItemSize );
		}

		// Changing items invalidates sort.
		InvalidateSort();
	}

	return inIndex;
}


// -----------------------------------------------------------------------------
//		SwapItems
// -----------------------------------------------------------------------------

void
CArrayEx::SwapItems(
	SInt32	inIndex1,
	SInt32	inIndex2 )
{
	// Can't swap in a sorted array.
	if ( IsKeptSorted() ) return;

	// Enforce upper bounds.
	if ( inIndex1 == kArrayIndex_Last ) inIndex1 = (SInt32) m_nCount - 1;
	if ( inIndex2 == kArrayIndex_Last ) inIndex2 = (SInt32) m_nCount - 1;

	// Don't do anything if the indexes are the same.
	if ( inIndex1 == inIndex2 ) return;

	// Validate the indexes.
	if ( !IsValidIndex( inIndex1 ) ) return;
	if ( !IsValidIndex( inIndex2 ) ) return;

	char*	pBuffer = new char[GrabItemSize( inIndex1 )];
	InternalSwapItems( inIndex1, inIndex2, pBuffer );
	delete [] pBuffer;

	// Swap invalidates sort.
	InvalidateSort();
}


// -----------------------------------------------------------------------------
//		MoveItem
// -----------------------------------------------------------------------------

void
CArrayEx::MoveItem(
	SInt32	inFromIndex,
	SInt32	inToIndex )
{
	// Can't swap in a sorted array.
	if ( IsKeptSorted() ) return;

	// Enforce upper bounds.
	if ( inFromIndex == kArrayIndex_Last ) inFromIndex = (SInt32) m_nCount - 1;
	if ( inToIndex == kArrayIndex_Last ) inToIndex = (SInt32) m_nCount - 1;

	// Don't do anything if the indexes are the same.
	if ( inFromIndex == inToIndex ) return;

	// Validate the indexes.
	if ( !IsValidIndex( inFromIndex ) ) return;
	if ( !IsValidIndex( inToIndex ) ) return;

	char*	pBuffer = new char[GrabItemSize( inFromIndex )];
	InternalMoveItem( inFromIndex, inToIndex, pBuffer );
	delete [] pBuffer;

	// Move invalidates sort.
	InvalidateSort();
}

// -----------------------------------------------------------------------------
//		QSort
// -----------------------------------------------------------------------------

void
CArrayEx::QSort()
{
	if ( !m_bIsSorted && (m_nCount > 1) )
	{
		qsort(
			m_pData,
			m_nCount,
			m_nItemSize,
			m_pQCompareFunc
		);

		m_bIsSorted = true;
	}
}

// -----------------------------------------------------------------------------
//		Sort
// -----------------------------------------------------------------------------

void
CArrayEx::Sort()
{
	if ( !m_bIsSorted && (m_nCount > 1) )
	{
		// Shellsort Algorithm.
		// Find starting "h" value.
		SInt32	h = 1;
		while ( h <= (SInt32)(m_nCount / 9) )
		{
			h = (3 * h) + 1;
		}

		char*	pBuffer = new char[m_nItemSize];
		for ( ; h > 0; h /= 3 )
		{
			for ( SInt32 i = h; i < (SInt32) m_nCount; i++ )
			{
				PeekItem( i, pBuffer );
				SInt32	j;
				for ( j = i - h; j >= 0; j -= h )
				{
					if ( m_pComparator->Compare( pBuffer,
						GetItemPtr( j ), m_nItemSize, m_nItemSize ) >= 0 )
					{
						break;
					}
					PokeItem( j + h, GetItemPtr( j ), m_nItemSize );
				}
				PokeItem( j + h, pBuffer, m_nItemSize );
			}
		}
		delete [] pBuffer;
	}

	m_bIsSorted = true;
}


// -----------------------------------------------------------------------------
//		SetKeepSorted
// -----------------------------------------------------------------------------

void
CArrayEx::SetKeepSorted(
	bool	inKeepSorted )
{
	m_bIsKeptSorted = inKeepSorted;

	if ( inKeepSorted )
	{
		Sort();
	}
}


// -----------------------------------------------------------------------------
//		SetComparator
// -----------------------------------------------------------------------------

void
CArrayEx::SetComparator(
	CComparator*	inComparator,
	bool			inTakeOwnership )
{
	if ( m_bOwnsComparator )
	{
		delete m_pComparator;
	}

	m_pComparator = inComparator;
	m_bOwnsComparator = inTakeOwnership;

	if ( GetCount() > 1 )
	{
		// Array is no longer sorted when Comparator changes
		InvalidateSort();
	}

	if ( IsKeptSorted() )
	{
		// Re-sort.
		Sort();
	}
}


// -----------------------------------------------------------------------------
//		GetIndexOf
// -----------------------------------------------------------------------------

SInt32
CArrayEx::GetIndexOf(
	const void*	inItem,
	UInt32		inItemSize ) const
{
	SInt32	nIndex = kArrayIndex_Invalid;

	if ( GetCount() > 0 )
	{
		if ( inItemSize == 0 )
		{
			inItemSize = m_nItemSize;
		}

		if ( IsSorted() )
		{
			// Sorted data can use a faster binary search.
			nIndex = BinarySearch( inItem, inItemSize );
		}
		else
		{
			// Unsorted data is searched linearly.
			nIndex = LinearSearch( inItem, inItemSize );
		}
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		GetIndexOfKey
// -----------------------------------------------------------------------------

SInt32
CArrayEx::GetIndexOfKey(
	const void*	inKey ) const
{
	SInt32	nIndex = kArrayIndex_Invalid;

	if ( GetCount() > 0 )
	{
		if ( IsSorted() )
		{
			// Sorted data can use a faster binary search.
			nIndex = BinarySearchByKey( inKey );
		}
		else
		{
			// Unsorted data is searched linearly.
			nIndex = LinearSearchByKey( inKey );
		}
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		GetInsertIndexOf
// -----------------------------------------------------------------------------

SInt32
CArrayEx::GetInsertIndexOf(
	const void*	inItem,
	UInt32		inItemSize ) const
{
	SInt32	nIndex = kArrayIndex_Last;

	if ( IsSorted() && (inItem != NULL) &&
		(GetCount() > 0) && (m_pComparator != NULL) )
	{
		if ( inItemSize == 0 )
		{
			inItemSize = m_nItemSize;
		}

		SInt32	nLeft = 0;
		SInt32	nRight = (SInt32) m_nCount - 1;
		SInt32	v = 0;

		nIndex = 0;

		while ( nLeft <= nRight )
		{
			nIndex = (nLeft + nRight) / 2;

			v = m_pComparator->Compare( GetItemPtr( nIndex ),
					inItem, GrabItemSize( nIndex ), inItemSize );

			if ( v == 0 )
			{
				break;
			}
			else if ( v > 0 )
			{
				nRight = nIndex - 1;
			}
			else
			{
				nLeft = nIndex + 1;
			}
		}

		if ( v < 0 )
		{
			nIndex += 1;
		}
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		GetInsertIndexOfKey
// -----------------------------------------------------------------------------

SInt32
CArrayEx::GetInsertIndexOfKey(
	const void*	inKey ) const
{
	SInt32	nIndex = kArrayIndex_Last;

	if ( IsSorted() && (GetCount() > 0) && (m_pComparator != NULL) )
	{
		SInt32	nLeft = 0;
		SInt32	nRight = (SInt32) m_nCount - 1;
		SInt32	v = 0;

		nIndex = 0;

		while ( nLeft <= nRight )
		{
			nIndex = (nLeft + nRight) / 2;

			v = m_pComparator->CompareToKey( GetItemPtr( nIndex ),
					GrabItemSize( nIndex ), inKey );

			if ( v == 0 )
			{
				break;
			}
			else if ( v > 0 )
			{
				nRight = nIndex - 1;
			}
			else
			{
				nLeft = nIndex + 1;
			}
		}

		if ( v < 0 )
		{
			nIndex += 1;
		}
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		AdjustAllocation
// -----------------------------------------------------------------------------

void
CArrayEx::AdjustAllocation(
	UInt32	inExtraItems,
	UInt32	/*inExtraData*/ )
{
	InternalAdjustAllocation( 0, (m_nCount + inExtraItems) * m_nItemSize );
}


// -----------------------------------------------------------------------------
//		PeekItem
// -----------------------------------------------------------------------------

void
CArrayEx::PeekItem(
	SInt32	inIndex,
	void*	outItem ) const
{
	UInt32	nItemSize = GrabItemSize( inIndex );

	if ( nItemSize == sizeof(UInt32) )
	{
		// Specialization: be fast for 4-byte item size.
		*(UInt32*) outItem = *(UInt32*) GetItemPtr( inIndex );
	}
	else
	{
		// Copy the item from the array.
		memcpy( outItem, GetItemPtr( inIndex ), nItemSize );
	}
}


// -----------------------------------------------------------------------------
//		PokeItem
// -----------------------------------------------------------------------------

void
CArrayEx::PokeItem(
	SInt32		inIndex,
	const void*	inItem,
	UInt32		inItemSize )
{
	if ( inItemSize == sizeof(UInt32) )
	{
		// Specialization: be fast for 4-byte item size.
		*(UInt32*) GetItemPtr( inIndex ) = *(UInt32*) inItem;
	}
	else
	{
		// Copy the item to the array.
		memcpy( GetItemPtr( inIndex ), inItem, inItemSize);
	}
}


// -----------------------------------------------------------------------------
//		InternalAdjustAllocation
// -----------------------------------------------------------------------------

void
CArrayEx::InternalAdjustAllocation(
	UInt32	/*inItemAllocation*/,
	UInt32	inDataAllocation )
{
	if ( inDataAllocation != (UInt32) m_nDataAllocated )
	{
		void*	pNewData;
		if ( m_pData == NULL )
		{
			// First allocation.
			pNewData = malloc( inDataAllocation );
		}
		else
		{
			// Resize existing storage.
			pNewData = realloc( m_pData, inDataAllocation );
		}

#if defined(_DEBUG) && defined(_AFX)
//		TRACE1( "CArrayEx malloc/realloc, size = %d\n", inDataAllocation );
#endif

		// Throw if memory error.
		if ( (pNewData == NULL) && (inDataAllocation != 0) ) throw -1;

		// Update the storage.
		m_pData = pNewData;
		m_nDataAllocated = (SInt32) inDataAllocation;
	}
}


// -----------------------------------------------------------------------------
//		AdjustStorage
// -----------------------------------------------------------------------------

void
CArrayEx::AdjustStorage(
	SInt32	inDeltaItems,
	SInt32	/*inDeltaData*/ )
{
	UInt32	nNewItemCount = m_nCount + inDeltaItems;
	UInt32	nNewDataStored = nNewItemCount * m_nItemSize;

	if ( nNewDataStored > (UInt32) m_nDataAllocated )
	{
		// Adding items.
		// Allocate space for new items plus extra space to reduce
		// memory management overhead on subsequent additions.
		UInt32	nDeltaAlloc = inDeltaItems * m_nItemSize;
		if ( nDeltaAlloc < (UInt32) m_nDataAllocated )
		{
			nDeltaAlloc = (UInt32) m_nDataAllocated;
		}

		try
		{
			InternalAdjustAllocation( 0, m_nDataAllocated + nDeltaAlloc );
		}
		catch ( ... )
		{
			// Probably not enough memory. Try allocating
			// just the amount of space that we need.
			InternalAdjustAllocation( 0, nNewDataStored );
		}
	}
	else if ( inDeltaItems < 0 )
	{
		// Removing items.
		SInt32	nCurrentExtra = m_nDataAllocated - m_nDataStored;
		InternalAdjustAllocation( 0, nNewDataStored + nCurrentExtra );
	}

	m_nCount = nNewItemCount;
	m_nDataStored = (SInt32) nNewDataStored;
}


// -----------------------------------------------------------------------------
//		ShiftItems
// -----------------------------------------------------------------------------

void
CArrayEx::ShiftItems(
	SInt32	inStartIndex,
	SInt32	inEndIndex,
	SInt32	inIndexShift,
	SInt32	/*inDataShift*/ )
{
	memmove(
		GetItemPtr( inStartIndex + inIndexShift ),
		GetItemPtr( inStartIndex ),
		GrabItemRangeSize( inStartIndex, inEndIndex ) );
}


// -----------------------------------------------------------------------------
//		InternalSwapItems
// -----------------------------------------------------------------------------

void
CArrayEx::InternalSwapItems(
	SInt32	inIndex1,
	SInt32	inIndex2,
	void*	inBuffer )
{
	// Store a copy of Item 1.
	PeekItem( inIndex1, inBuffer );
	UInt32	nItemSize = GrabItemSize( inIndex1 );

	// Item 1 = Item 2.
	InternalCopyItem( inIndex2, inIndex1 );

	// Item 2 = copy of Item 1.
	PokeItem( inIndex2, inBuffer, nItemSize );
}


// -----------------------------------------------------------------------------
//		InternalMoveItem
// -----------------------------------------------------------------------------

void
CArrayEx::InternalMoveItem(
	SInt32	inFromIndex,
	SInt32	inToIndex,
	void*	inBuffer )
{
	// Store copy of item to move.
	UInt32	nItemSize = GrabItemSize( inFromIndex );
	PeekItem( inFromIndex, inBuffer );

	if ( inFromIndex < inToIndex )
	{
		// Move item to a higher index. Shift items
		// between "from" and "to" down one spot.
		ShiftItems( inFromIndex + 1, inToIndex, -1, -(SInt32) nItemSize );
	}
	else
	{
		// Move item to a lower index. Shift items
		// between "to" and "from" up one spot.
		ShiftItems( inToIndex, inFromIndex - 1, 1, (SInt32) nItemSize );
	}

	// Store item at new position
	PokeItem( inToIndex, inBuffer, nItemSize );
}


// -----------------------------------------------------------------------------
//		InternalCopyItem
// -----------------------------------------------------------------------------

void
CArrayEx::InternalCopyItem(
	SInt32	inSourceIndex,
	SInt32	inDestIndex )
{
	PokeItem( inDestIndex, GetItemPtr( inSourceIndex ), m_nItemSize );
}


// -----------------------------------------------------------------------------
//		LinearSearch
// -----------------------------------------------------------------------------

SInt32
CArrayEx::LinearSearch(
	const void*	inItem,
	UInt32		inItemSize ) const
{
	SInt32	nIndex = 0;
	if ( m_pData == NULL ) return nIndex;
	UInt8*	pItem = static_cast<UInt8*>(m_pData);
	while ( nIndex < (SInt32) m_nCount )
	{
		UInt32	nItemSize = GrabItemSize( nIndex );
		if ( m_pComparator->IsEqualTo( inItem, pItem, inItemSize, nItemSize ) )
		{
			break;
		}
		nIndex++;
		pItem += nItemSize;
	}

	if ( nIndex >= (SInt32) m_nCount )
	{
		// Search stopped because we reached the
		// end without finding the item.
		nIndex = kArrayIndex_Invalid;
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		LinearSearchByKey
// -----------------------------------------------------------------------------

SInt32
CArrayEx::LinearSearchByKey(
	const void*	inKey ) const
{
	SInt32	nIndex = 0;
	UInt8*	pItem = static_cast<UInt8*>(m_pData);
	while ( nIndex < (SInt32) m_nCount )
	{
		UInt32	nItemSize = GrabItemSize( nIndex );
		if ( m_pComparator->IsEqualToKey( pItem, nItemSize, inKey ) )
		{
			break;
		}
		nIndex++;
		pItem += nItemSize;
	}

	if ( nIndex >= (SInt32) m_nCount )
	{
		// Search stopped because we reached the
		// end without finding the item.
		nIndex = kArrayIndex_Invalid;
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		BinarySearch
// -----------------------------------------------------------------------------

SInt32
CArrayEx::BinarySearch(
	const void*	inItem,
	UInt32		inItemSize ) const
{
	SInt32	nLeft = 0;
	SInt32	nRight = m_nCount - 1;
	SInt32	nIndex = kArrayIndex_Invalid;

	while ( nLeft <= nRight )
	{
		SInt32	nCurrIndex = (nLeft + nRight) / 2;
		SInt32	v = m_pComparator->Compare( GetItemPtr( nCurrIndex ),
						inItem, GrabItemSize( nCurrIndex ), inItemSize );

		if ( v == 0 )
		{
			nIndex = nCurrIndex;
			break;
		}
		else if ( v > 0 )
		{
			nRight = nCurrIndex - 1;
		}
		else
		{
			nLeft = nCurrIndex + 1;
		}
	}

	return nIndex;
}


// -----------------------------------------------------------------------------
//		BinarySearchByKey
// -----------------------------------------------------------------------------

SInt32
CArrayEx::BinarySearchByKey(
	const void*	inKey ) const
{
	SInt32	nLeft = 0;
	SInt32	nRight = m_nCount - 1;
	SInt32	nIndex = kArrayIndex_Invalid;

	while ( nLeft <= nRight )
	{
		SInt32	nCurrIndex = (nLeft + nRight) / 2;
		SInt32	v = m_pComparator->CompareToKey( GetItemPtr( nCurrIndex ),
						GrabItemSize( nCurrIndex ), inKey );

		if ( v == 0 )
		{
			nIndex = nCurrIndex;
			break;
		}
		else if ( v > 0 )
		{
			nRight = nCurrIndex - 1;
		}
		else
		{
			nLeft = nCurrIndex + 1;
		}
	}

	return nIndex;
}

// CComparator and CLongComparator moved to Comparator.h/cpp

