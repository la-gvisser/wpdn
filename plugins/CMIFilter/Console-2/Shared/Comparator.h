// ============================================================================
//	Comparator.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "AGTypes.h"
#include <string.h>

// ============================================================================
//	IComparator
// ============================================================================

class IComparator
{
public:
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
//	CLongComparator
// ============================================================================

class CLongComparator : public CComparator
{
public:
	virtual CComparator*	Clone() const
	{
		return new CLongComparator;
	}

	virtual SInt32			Compare( const void* inItem1, const void* inItem2,
								UInt32 /*inSize1*/, UInt32 /*inSize2*/ ) const
	{
		return ((*(SInt32*)inItem1) - (*(SInt32*)inItem2));
	}

	virtual bool			IsEqualTo( const void* inItem1, const void* inItem2,
								UInt32 /*inSize1*/, UInt32 /*inSize2*/ ) const
	{
		return ((*(SInt32*)inItem1) == (*(SInt32*)inItem2));
	}

	static CLongComparator*		GetComparator()
	{
		static CLongComparator	s_Comparator;
		return &s_Comparator;
	}
};


// ============================================================================
//	CULongLongComparator
// ============================================================================

class CULongLongComparator : public CComparator
{
public:
	virtual CComparator*	Clone() const
	{
		return new CULongLongComparator;
	}

	virtual SInt32			Compare( const void* inItem1, const void* inItem2,
								UInt32 /*inSize1*/, UInt32 /*inSize2*/ ) const
	{
		if ((*(UInt64*)inItem1) > (*(UInt64*)inItem2))
			return 1;
		else if ((*(UInt64*)inItem1) < (*(UInt64*)inItem2))
			return -1;
		else
			return 0;
	}

	virtual bool			IsEqualTo( const void* inItem1, const void* inItem2,
								UInt32 /*inSize1*/, UInt32 /*inSize2*/ ) const
	{
		return ((*(UInt64*)inItem1) == (*(UInt64*)inItem2));
	}

	virtual int				QCompare(const void *elem1, const void *elem2 )
	{
		return Compare(elem1, elem2, sizeof(UInt64*), sizeof(UInt64*));
	}

	static CULongLongComparator*	GetComparator()
	{
		static CULongLongComparator	s_Comparator;
		return &s_Comparator;
	}
};

#endif
