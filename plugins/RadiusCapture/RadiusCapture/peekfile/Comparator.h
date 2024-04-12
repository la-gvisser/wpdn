// ============================================================================
//	Comparator.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2006. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "WPTypes.h"
#include <string.h>			///< for SInt32 and friends
#include <functional>		// for std::less

// ============================================================================
//	IComparator
// ============================================================================

class IComparator
{
public:
	virtual ~IComparator() {}
	
	virtual SInt32			Compare( const void* inItem1, const void* inItem2,
								UInt32 inSize1, UInt32 inSize2 ) const = 0;
	virtual bool			IsEqualTo( const void* inItem1, const void* inItem2,
								UInt32 inSize1, UInt32 inSize2 ) const = 0;
	virtual	SInt32			CompareToKey( const void* inItem,
								UInt32 inSize, const void* inKey ) const = 0;
	virtual	bool			IsEqualToKey( const void* inItem,
								UInt32 inSize, const void* inKey ) const = 0;
};


// ============================================================================
//	CComparator
// ============================================================================

class CComparator : public IComparator
{
public:
	virtual ~CComparator() {}
	virtual CComparator*	Clone() const
	{
		return new CComparator;
	}

	virtual SInt32			Compare( const void* inItem1, const void* inItem2,
								UInt32 inSize1, UInt32 inSize2 ) const
	{
		size_t	nSize = (inSize1 < inSize2) ? (size_t) inSize1 : (size_t) inSize2;
		return (SInt32) memcmp( inItem1, inItem2, nSize );
	}

	virtual bool			IsEqualTo( const void* inItem1, const void* inItem2,
								UInt32 inSize1, UInt32 inSize2 ) const
	{
		return (Compare( inItem1, inItem2, inSize1, inSize2 ) == 0);
	}

	virtual	SInt32			CompareToKey( const void* /*inItem*/,
								UInt32 /*inSize*/, const void* /*inKey*/ ) const
	{
		return 1;
	}

	virtual	bool			IsEqualToKey( const void* inItem,
								UInt32 inSize, const void* inKey ) const
	{
		return (CompareToKey( inItem, inSize, inKey ) == 0);
	}

	static CComparator*		GetComparator()
	{
		static CComparator	s_Comparator;
		return &s_Comparator;
	}
};


// ============================================================================
//	TComparator
// ============================================================================
///	A comparator that orders elements using only operator <, or the Less<> that you supply.

template < typename T, typename Less = std::less< T > >
class TComparator : public CComparator
{
	typedef TComparator			ThisType;
public:
	virtual TComparator*	Clone() const
	{
		return new TComparator();
	}

	virtual SInt32		
	Compare( const void* inItemOne, const void* inItemTwo,
		UInt32 inSizeOne, UInt32 inSizeTwo ) const
	{
		inSizeOne = inSizeOne;		// not used
		inSizeTwo = inSizeTwo;		// not used
		ASSERT(inSizeOne == sizeof(T) && inSizeTwo == sizeof(T));
		const T *pOne = static_cast< const T* >( inItemOne );
		const T *pTwo = static_cast< const T* >( inItemTwo );
		if ( Less()( *pOne, *pTwo ) ) return -1;
		if ( Less()( *pTwo, *pOne ) ) return +1;
		return 0;
	}

	virtual int			
	QCompare(const void *elem1, const void *elem2 )
	{
		return Compare(elem1, elem2, sizeof(T), sizeof(T));
	}

	virtual bool
	IsEqualTo( const void* inItemOne, const void* inItemTwo,
		UInt32 inSizeOne, UInt32 inSizeTwo ) const
	{
		return Compare(inItemOne, inItemTwo, inSizeOne, inSizeTwo) == 0;
	}

	static ThisType*
	GetComparator()
	{
		static ThisType s_Comp;
		return &s_Comp;
	}
};


// ============================================================================
//	TIntComparator
// ============================================================================
///	@brief A comparator that uses operator - and operator == to compare elements.
/// 
/// Use instead of TComparator<> for integer types because a single call to 
/// operator - or == on a pair of ints is probably fewer clock cycles
/// than two calls to operator <.

template < typename T >
class TIntComparator : public CComparator
{
	typedef TIntComparator			ThisType;
public:
	virtual TIntComparator*	Clone() const
	{
		return new TIntComparator();
	}

	virtual SInt32		
	Compare( const void* inItemOne, const void* inItemTwo,
		UInt32 inSizeOne, UInt32 inSizeTwo ) const
	{
		inSizeOne = inSizeOne;		// not used
		inSizeTwo = inSizeTwo;		// not used
		ASSERT(inSizeOne == sizeof(T) && inSizeTwo == sizeof(T));
		const T *pOne = static_cast< const T* >( inItemOne );
		const T *pTwo = static_cast< const T* >( inItemTwo );
		return static_cast< SInt32 >( *pOne - *pTwo );
	}

	virtual int			
	QCompare(const void *elem1, const void *elem2 )
	{
		return Compare(elem1, elem2, sizeof(T), sizeof(T));
	}

	virtual bool
	IsEqualTo( const void* inItemOne, const void* inItemTwo,
		UInt32 inSizeOne, UInt32 inSizeTwo ) const
	{
		inSizeOne = inSizeOne;		// not used
		inSizeTwo = inSizeTwo;		// not used
		ASSERT(inSizeOne == sizeof(T) && inSizeTwo == sizeof(T));
		const T *pOne = static_cast< const T* >( inItemOne );
		const T *pTwo = static_cast< const T* >( inItemTwo );
		return ( *pOne == *pTwo );
	}

	static ThisType*
	GetComparator()
	{
		static ThisType s_Comp;
		return &s_Comp;
	}
};

typedef TIntComparator< UInt32 >	CLongComparator;

/// 64-bit compare is large enough that it should use operator < instead of operators - and ==.
typedef TComparator< UInt64 >		CULongLongComparator;


#endif
