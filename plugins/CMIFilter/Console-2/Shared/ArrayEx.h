// =============================================================================
//	ArrayEx.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#ifndef ARRAYEX_H
#define ARRAYEX_H

#include "AGTypes.h"
#include "Comparator.h"

typedef int (*QCompareFunc) (const void*, const void*);

class DECLSPEC CArrayEx
{
public:
	enum
	{
		kArrayIndex_Invalid		= -1,
		kArrayIndex_First		= 0,
		kArrayIndex_Last		= 0x7FFFFFFF
	};

					CArrayEx();
					CArrayEx( UInt32 inItemSize,
						CComparator* inComparator = NULL,
						bool inKeepSorted = false );
					CArrayEx( UInt32 inItemCount,
						UInt32 inItemSize,
						CComparator* inComparator = NULL,
						bool inKeepSorted = false );
					CArrayEx( const CArrayEx& inOriginal );
	virtual			~CArrayEx();

	CArrayEx&		operator=( const CArrayEx& inArray );

	bool			IsValidIndex( SInt32 inIndex ) const
						{ return ( (inIndex >= 0) && (inIndex < (SInt32) m_nCount) ); }

	UInt32			GetCount() const { return m_nCount; }
	virtual void	SetCount( UInt32 inCount );

	virtual UInt32	GetItemSize( SInt32 /*inIndex*/ ) const { return m_nItemSize; }

	virtual void*	GetData() const { return m_pData; }

	virtual bool	GetItem( SInt32 inIndex, void* outItem ) const;
	virtual bool	GetItem( SInt32 inIndex, void* outItem, UInt32& ioItemSize ) const;
	virtual void*	GetItemPtr( SInt32 inIndex ) const;

	virtual SInt32	AddItem( const void* inItem, UInt32 inItemSize = 0 );
	virtual SInt32	InsertItems( UInt32 inCount, SInt32 inIndex,
						const void* inItem = NULL, UInt32 inItemSize = 0 );

	virtual void	RemoveAllItems();
	virtual void	RemoveItems( UInt32 inCount, SInt32 inIndex );
	virtual void	RemoveItem( const void* inValue, UInt32 inItemSize = 0 );

	virtual SInt32	SetItems( UInt32 inCount, SInt32 inIndex,
						const void* inValue, UInt32 inItemSize = 0 );

	virtual void	SwapItems( SInt32 inIndex1, SInt32 inIndex2 );
	virtual void	MoveItem( SInt32 inFromIndex, SInt32 inToIndex );

	virtual void	Sort();
	virtual void	QSort();
	bool			IsSorted() const { return m_bIsSorted; }
	void			InvalidateSort() { m_bIsSorted = false; }
	bool			IsKeptSorted() const { return m_bIsKeptSorted; }
	void			SetKeepSorted( bool inKeepSorted );
	void			SetComparator( CComparator* inComparator, bool inTakeOwnership = true );
	CComparator*	GetComparator() const { return m_pComparator; }

	void			SetQComparator( QCompareFunc pFunc ) { m_pQCompareFunc = pFunc; }
	QCompareFunc	GetQComparator() const { return m_pQCompareFunc; }

	virtual SInt32	GetIndexOf( const void* inItem, UInt32 inItemSize = 0 ) const;
	virtual SInt32	GetIndexOfKey( const void* inKey ) const;
	virtual SInt32	GetInsertIndexOf( const void* inItem, UInt32 inItemSize = 0 ) const;
	virtual SInt32	GetInsertIndexOfKey( const void* inKey ) const;

	virtual void	AdjustAllocation( UInt32 inExtraItems, UInt32 inExtraData = 0 );

protected:
	UInt32			m_nItemSize;
	UInt32			m_nCount;
	SInt32			m_nDataStored;
	SInt32			m_nDataAllocated;
	bool			m_bIsKeptSorted;
	bool			m_bIsSorted;
	bool			m_bOwnsComparator;
	CComparator*	m_pComparator;
	QCompareFunc	m_pQCompareFunc;
	void*			m_pData;

	virtual void	PeekItem( SInt32 inIndex, void* outItem ) const;
	virtual void	PokeItem( SInt32 inIndex, const void* inItem, UInt32 inItemSize );

	virtual void	InternalAdjustAllocation( UInt32 inItemAllocation, UInt32 inDataAllocation );
	virtual void	AdjustStorage( SInt32 inDeltaItems, SInt32 inDeltaData );

	virtual void	ShiftItems( SInt32 inStartIndex, SInt32 inEndIndex,
						SInt32 inIndexShift, SInt32 inDataShift );
	virtual void	InternalSwapItems( SInt32 inIndexA, SInt32 inIndexB, void* inBuffer );
	virtual void	InternalMoveItem( SInt32 inFromIndex, SInt32 inToIndex, void* inBuffer );
	virtual void	InternalCopyItem( SInt32 inSourceIndex, SInt32 inDestIndex );

	SInt32			LinearSearch( const void* inItem, UInt32 inItemSize ) const;
	SInt32			LinearSearchByKey( const void* inKey ) const;
	SInt32			BinarySearch( const void* inItem, UInt32 inItemSize ) const;
	SInt32			BinarySearchByKey( const void* inKey ) const;

	virtual UInt32	GrabItemSize( SInt32 /*inIndex*/ ) const { return m_nItemSize; }
	virtual UInt32	GrabItemRangeSize( SInt32 inStartIndex, SInt32 inEndIndex ) const
						{
							return (inEndIndex - inStartIndex + 1) * m_nItemSize;
						}

private:
	void			InitArray( UInt32 inItemSize, CComparator* inComparator,
						bool inIsSorted, bool inKeepSorted );
	void			DestroyArray();
	void			CopyArray( const CArrayEx& inOriginal );
};


template <class T> class TArrayEx : public CArrayEx
{
public:
					TArrayEx() : CArrayEx( sizeof(T), NULL, false ) {}
					TArrayEx( CComparator* inComparator, bool inKeepSorted = false )
						: CArrayEx( sizeof(T), inComparator, inKeepSorted ) {}
					TArrayEx( UInt32 inItemCount, CComparator* inComparator = NULL,
						bool inKeepSorted = false )
							: CArrayEx( inItemCount, sizeof(T),
								inComparator, inKeepSorted ) {}
	virtual			~TArrayEx() {}

	T&				operator[]( SInt32 inIndex )
						{
							ASSERT(IsValidIndex(inIndex));
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return *(T*) (pData + (inIndex * sizeof(T)));
						}

	const T&		operator[]( SInt32 inIndex ) const
						{
							ASSERT(IsValidIndex(inIndex));
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return *(T*) (pData + (inIndex * sizeof(T)));
						}

	const T&		GetAt( SInt32 inIndex ) const
						{
							ASSERT(IsValidIndex(inIndex));
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return *(T*) (pData + (inIndex * sizeof(T)));
						}

	bool			GetItem( SInt32 inIndex, T& outItem ) const
						{
							return CArrayEx::GetItem( inIndex, &outItem );
						}

	T*				GetItem( SInt32 inIndex ) const
						{
							if ( m_pData == NULL ) return NULL;
							if ( !IsValidIndex( inIndex ) ) return NULL;
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return (T*) (pData + (inIndex * sizeof(T)));
						}

	SInt32			AddItem( const T& inItem )
						{
							return CArrayEx::AddItem( &inItem, sizeof(T) );
						}

	SInt32			InsertItems( UInt32 inCount, SInt32 inIndex, const T& inItem )
						{
							return CArrayEx::InsertItems( inCount, inIndex, &inItem, sizeof(T) );
						}

	void			RemoveItem( const T& inItem )
						{
							CArrayEx::RemoveItem( &inItem, sizeof(T) );
						}

	SInt32			SetItems( UInt32 inCount, SInt32 inIndex, const T& inItem )
						{
							return CArrayEx::SetItems( inCount, inIndex, &inItem );
						}

	SInt32			GetIndexOf( const T& inItem ) const
						{
							return CArrayEx::GetIndexOf( &inItem );
						}

	SInt32			GetInsertIndexOf( const T& inItem ) const
						{
							return CArrayEx::GetInsertIndexOf( &inItem, sizeof(T) );
						}

#if TARGET_OS_MAC
	// zz 2001-06-27 don't hide publicly-inherited CArrayEx API.
	// Triggers "hides inherited virtual function" warnings in CodeWarrior 6.
	// In this case, you sort of *want* to hide the type-unsafe inherited API,
	// but the hiding isn't reallly sufficient since you can cast to a
	// CArrayEx and then stuff random void* gibberish in the array.]
	// For now,  I'll squelch the warnings with "using" below.
	// For longer-term, use CArrayEx as a private data member (delegate, has-a)
	// instead of a public base class (is-a).
	using CArrayEx::GetItem;
	using CArrayEx::AddItem;
	using CArrayEx::InsertItems;
	using CArrayEx::RemoveItem;
	using CArrayEx::SetItems;
	using CArrayEx::GetIndexOf;
	using CArrayEx::GetInsertIndexOf;
#endif // TARGET_OS_MAC
};



template <class T> class TPtrArrayEx : public CArrayEx
{
public:
					TPtrArrayEx() : CArrayEx( sizeof(T*), NULL, false ) {}
					TPtrArrayEx( CComparator* inComparator, bool inKeepSorted = false )
						: CArrayEx( sizeof(T*), inComparator, inKeepSorted ) {}
					TPtrArrayEx( UInt32 inItemCount, CComparator* inComparator = NULL,
						bool inKeepSorted = false )
							: CArrayEx( inItemCount, sizeof(T*),
								inComparator, inKeepSorted ) {}
	virtual			~TPtrArrayEx() {}

	T*				operator[]( SInt32 inIndex)
						{
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return *(T**) (pData + (inIndex * sizeof(T*)));
						}

	const T*		operator[]( SInt32 inIndex) const
						{
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return *(T**) (pData + (inIndex * sizeof(T*)));
						}

	bool			GetItem( SInt32 inIndex, T*& outItem ) const
						{
							return CArrayEx::GetItem( inIndex, &outItem );
						}

	T*				GetItemPtr( SInt32 inIndex )	// Non-const to differ from inherited.
						{
							if ( m_pData == NULL ) return NULL;
							if ( !IsValidIndex( inIndex ) ) return NULL;
							UInt8*	pData = static_cast<UInt8*>(m_pData);
							return *(T**) (pData + (inIndex * sizeof(T*)));
						}

	SInt32			AddItem( const T* inItem )
						{
							return CArrayEx::AddItem( &inItem, sizeof(T*) );
						}

	SInt32			InsertItems( UInt32 inCount, SInt32 inIndex, const T* inItem )
						{
							return CArrayEx::InsertItems( inCount, inIndex, &inItem, sizeof(T*) );
						}

	void			RemoveItem( const T* inItem )
						{
							CArrayEx::RemoveItem( &inItem, sizeof(T*) );
						}

	SInt32			SetItems( UInt32 inCount, SInt32 inIndex, const T* inItem )
						{
							return CArrayEx::SetItems( inCount, inIndex, &inItem );
						}

	SInt32			GetIndexOf( const T* inItem ) const
						{
							return CArrayEx::GetIndexOf( &inItem );
						}

	SInt32			GetInsertIndexOf( const T* inItem ) const
						{
							return CArrayEx::GetInsertIndexOf( &inItem, sizeof(T*) );
						}

#if TARGET_OS_MAC
	// zz 2001-06-27 don't hide publicly inherited virtual API
	using CArrayEx::GetItem;
	using CArrayEx::GetItemPtr;
	using CArrayEx::AddItem;
	using CArrayEx::InsertItems;
	using CArrayEx::RemoveItem;
	using CArrayEx::SetItems;
	using CArrayEx::GetIndexOf;
	using CArrayEx::GetInsertIndexOf;
#endif // TARGET_OS_MAC
};


template <class T> class TStackEx : public TArrayEx<T>
{
public:
//					TStackEx() : TArrayEx<T>( sizeof(T), NULL, false ) {}
//					TStackEx( CComparator* inComparator, bool inKeepSorted = false )
//						: TArrayEx<T>( sizeof(T), inComparator, inKeepSorted ) {}
					TStackEx( UInt32 inItemCount = 0)
							: TArrayEx<T>( inItemCount, NULL, false ) {}
	virtual			~TStackEx() {}

	void			Push( const T& inItem ) { AddItem( inItem ); }
	const T*		Peek( ) const			{ return GetItem(GetCount() - 1); }
	T				Pop( )
	{
		T t;
		GetItem(GetCount() - 1, t);
		SetCount(GetCount() - 1);
		return t;
	}

	bool			IsEmpty() const	{	return (GetCount() == 0);	}
	void			Reset()			{	SetCount(0);				}
};

// =============================================================================
//		TQueueEx
// =============================================================================

template <class T>
class TQueueEx : public TArrayEx<T>
{
public:
				TQueueEx() {}
				~TQueueEx(){}

	void		Put( const T& inItem )
					{ AddItem( inItem ); }

	bool		Get( T& outItem );
};


// -----------------------------------------------------------------------------
//		Get
// -----------------------------------------------------------------------------

template <class T>
bool
TQueueEx<T>::Get(
	T&	outItem )
{
	bool	bGotItem = false;

	if( GetItem( CArrayEx::kArrayIndex_First, outItem ) )
	{
		bGotItem = true;
		RemoveItems( 1, CArrayEx::kArrayIndex_First );
	}

	return bGotItem;
}

// CComparator and CLongComparator moved to Comparator.h/cpp

// -----------------------------------------------------------------------------
//		TObjArrayEx, a template array that can hold class object pointers (that it creates and owns)
// -----------------------------------------------------------------------------
template <class T>
class TObjArrayEx : public TArrayEx<T*>
{
public:
	TObjArrayEx()	{}
	virtual ~TObjArrayEx()
	{	// walk through and delete them all
		for (UInt32 nIdx = 0; nIdx < GetCount(); nIdx++)
			delete (*this)[nIdx];
	}

	virtual void SetCount(UInt32 nCount)
	{	// make new entries and add them in
		UInt32 nCurCount = GetCount();
		// delete the end ones
		while (nCurCount > nCount)
			delete (*this)[--nCurCount];
		TArrayEx<T*>::SetCount(nCount);
		while (nCurCount < nCount)
			(*this)[nCurCount++] = new T();
	}

	// I'm extreme programming here, so just keep anyone from using these
	// inherited functions without writing support code
	virtual void RemoveAllItems()
	{
		SetCount(0);
		TArrayEx<T*>::RemoveAllItems();
	}

	virtual SInt32	AddItem( const void* inItem, UInt32 inItemSize = 0 )
	{
#ifndef _LINUX
		inItem; inItemSize;
#endif
		ASSERT(0);
		return kArrayIndex_Invalid;
	}

	virtual SInt32	InsertItems( UInt32 inCount, SInt32 inIndex,
						const void* inItem = NULL, UInt32 inItemSize = 0 )
	{
#ifndef _LINUX
		inCount; inIndex; inItem; inItemSize;
#endif
		ASSERT(0);
		return kArrayIndex_Invalid;
	}

	virtual void	RemoveItems( UInt32 inCount, SInt32 inIndex )
	{
#ifndef _LINUX
		inCount; inIndex;
#endif
		ASSERT(0);
	}

	virtual void	RemoveItem( const void* inValue, UInt32 inItemSize = 0 )
	{
#ifndef _LINUX
		inValue; inItemSize;
#endif
		ASSERT(0);
	}

	virtual SInt32	SetItems( UInt32 inCount, SInt32 inIndex,
						const void* inValue, UInt32 inItemSize = 0 )
	{
#ifndef _LINUX
		inCount; inIndex; inValue; inItemSize;
#endif
		ASSERT(0);
		return kArrayIndex_Invalid;
	}

#if TARGET_OS_MAC
	// zz 2001-06-27 don't hide publicly-inherited CArrayEx API.
	using TArrayEx<T*>::AddItem;
	using TArrayEx<T*>::InsertItems;
	using TArrayEx<T*>::RemoveItems;
	using TArrayEx<T*>::RemoveItem;
	using TArrayEx<T*>::SetItems;
#endif // TARGET_OS_MAC
};

#endif
