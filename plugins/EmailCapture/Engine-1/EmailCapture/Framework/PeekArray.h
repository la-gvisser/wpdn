// =============================================================================
//	PeekArray.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include <algorithm>
#include <vector>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Array
//
//	A Peek Array is template of a Standard Template Library Vector (std::vector)
//	with the semantics of an MFC CAtlArray.
//
//	Peek Array Deprecated
//	The CPeekArrayDep template is identical to CAtlArray template but without
//	Traits. Private data members have been made protected in order to allow
//	modifying the array behavior.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPeekArray
// =============================================================================

template< typename T >
class CPeekArray
	:	public std::vector<T>
{
public:
	;		CPeekArray() {}
	;		CPeekArray( size_t inSize )
		:	std::vector<T>( inSize )
	{
	}

	size_t		Add() { push_back( T() ); return (size() - 1); }
	size_t		Add( const T& inItem ) { push_back( inItem ); return (size() - 1); }
	size_t		AddUnique( const T& inItem ) {
		std::vector<T>::iterator	itr = std::find( begin(), end(), inItem );
		if ( itr != end() ) {
			return std::distance( begin(), itr );
		}
		return Add( inItem );
	}

	const CPeekArray<T>&	Copy( const CPeekArray<T>& inOther ) {
		assign( inOther.begin(), inOther.end() );
		return *this;
	}

	size_t		Find( const T& inItem ) { return find( inItem ); }

	const T&	GetAt( size_t inIndex ) const { return at( inIndex ); }
	T&			GetAt( size_t inIndex ) { return at( inIndex ); }
	size_t		GetCount() const { return size(); }

	void		InsertAt( size_t inIndex, const T& inItem ) { insert( (begin() + inIndex), inItem ); }
	bool		IsEmpty() const { return empty(); }

	void		Remove( const T& inItem ) {
		std::vector<T>::iterator	itr = std::find( begin(), end(), inItem );
		if ( itr != end() ) {
			erase( itr );
		}
	}
	void		RemoveAll() {
#ifdef _DEBUG
		size_t	nCapacity = capacity();
#endif
		clear();
#ifdef _DEBUG
		_ASSERTE( nCapacity == capacity() );
#endif
	}
	void		RemoveAt( size_t inIndex ) { erase( begin() + inIndex ); }
	void		Reserve( size_t inCount ) { reserve( inCount ); }

	void		SetCount( size_t inCount ) { resize( inCount ); }
};


#if (0)
// =============================================================================
//		CPeekArrayDep
// =============================================================================

template< typename E >
class CPeekArrayDep
{
public:
	typedef const E& INARGTYPE;
	typedef E& OUTARGTYPE;

protected:
	E*		m_pData;
	size_t	m_nSize;
	size_t	m_nMaxSize;
	int		m_nGrowBy;

public:
	CPeekArrayDep() throw()
		: m_pData( NULL )
		, m_nSize( 0 )
		, m_nMaxSize( 0 )
		, m_nGrowBy( 0 ) {}

	~CPeekArrayDep() throw() {
		if( m_pData != NULL )
		{
			CallDestructors( m_pData, m_nSize );
			free( m_pData );
		}
	}

	size_t GetCount() const throw();
	bool IsEmpty() const throw();
	bool SetCount( size_t nNewSize, int nGrowBy = -1 );

	void FreeExtra() throw();
	void RemoveAll() throw();

	const E& GetAt( size_t iElement ) const;
	void SetAt( size_t iElement, INARGTYPE element );
	E& GetAt( size_t iElement );

	const E* GetData() const throw();
	E* GetData() throw();

	void SetAtGrow( size_t iElement, INARGTYPE element );
	// Add an empty element to the end of the array
	size_t Add();
	// Add an element to the end of the array
	size_t Add( INARGTYPE element );
	size_t Append( const CPeekArrayDep< E >& aSrc );
	void Copy( const CPeekArrayDep< E >& aSrc );

	const E& operator[]( size_t iElement ) const;
	E& operator[]( size_t iElement );

	void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
	void InsertArrayAt( size_t iStart, const CPeekArrayDep< E >* paNew );
	void RemoveAt( size_t iElement, size_t nCount = 1 );

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

protected:
	static void CallConstructors( E* pElements, size_t nElements );
	static void CallDestructors( E* pElements, size_t nElements ) throw();
	static void CopyElements( E* pDest, const E* pSrc, size_t nElements ) {
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}
	static void RelocateElements( E* pDest, E* pSrc, size_t nElements ) {
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		Checked::memmove_s( pDest, nElements*sizeof( E ), pSrc, nElements*sizeof( E ));
	}

	bool GrowBuffer( size_t nNewSize );

private:
	// Private to prevent use
	CPeekArrayDep( const CPeekArrayDep& ) throw();
	CPeekArrayDep& operator=( const CPeekArrayDep& ) throw();
};


template< typename E >
inline size_t CPeekArrayDep< E >::GetCount() const throw()
{
	return( m_nSize );
}


template< typename E >
inline bool CPeekArrayDep< E >::IsEmpty() const throw()
{
	return( m_nSize == 0 );
}

template< typename E >
inline void CPeekArrayDep< E >::RemoveAll() throw()
{
	SetCount( 0, -1 );
}

template< typename E >
inline const E& CPeekArrayDep< E >::GetAt( size_t iElement ) const
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E >
inline void CPeekArrayDep< E >::SetAt( size_t iElement, INARGTYPE element )
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	m_pData[iElement] = element;
}

template< typename E >
inline E& CPeekArrayDep< E >::GetAt( size_t iElement )
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E >
inline const E* CPeekArrayDep< E >::GetData() const throw()
{
	return( m_pData );
}

template< typename E >
inline E* CPeekArrayDep< E >::GetData() throw()
{
	return( m_pData );
}

template< typename E >
inline size_t CPeekArrayDep< E >::Add()
{
	size_t iElement;

	iElement = m_nSize;
	bool bSuccess=SetCount( m_nSize+1 );
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	return( iElement );
}

#pragma push_macro("new")
#undef new

template< typename E >
inline size_t CPeekArrayDep< E >::Add( INARGTYPE element )
{
	size_t iElement;

	iElement = m_nSize;
	if( iElement >= m_nMaxSize )
	{
		bool bSuccess = GrowBuffer( iElement+1 );
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}
	::new( m_pData+iElement ) E( element );
	m_nSize++;

	return( iElement );
}

#pragma pop_macro("new")

template< typename E >
inline const E& CPeekArrayDep< E >::operator[]( size_t iElement ) const
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E >
inline E& CPeekArrayDep< E >::operator[]( size_t iElement )
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E >
bool CPeekArrayDep< E >::GrowBuffer( size_t nNewSize )
{
	if( nNewSize > m_nMaxSize )
	{
		if( m_pData == NULL )
		{
			size_t nAllocSize =  size_t( m_nGrowBy ) > nNewSize ? size_t( m_nGrowBy ) : nNewSize ;
			m_pData = static_cast< E* >( calloc( nAllocSize,sizeof( E ) ) );
			if( m_pData == NULL )
			{
				return( false );
			}
			m_nMaxSize = nAllocSize;
		}
		else
		{
			// otherwise, grow array
			size_t nGrowBy = m_nGrowBy;
			if( nGrowBy == 0 )
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = m_nSize/8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
			}
			size_t nNewMax;
			if( nNewSize < (m_nMaxSize+nGrowBy) )
				nNewMax = m_nMaxSize+nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush

			ATLASSERT( nNewMax >= m_nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
			ATLASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
			E* pNewData = static_cast< E* >( calloc( nNewMax,sizeof( E ) ) );
			if( pNewData == NULL )
			{
				return false;
			}

			// copy new data from old
			RelocateElements( pNewData, m_pData, m_nSize );

			// get rid of old stuff (note: no destructors called)
			free( m_pData );
			m_pData = pNewData;
			m_nMaxSize = nNewMax;
		}
	}

	return true;
}

template< typename E >
bool CPeekArrayDep< E >::SetCount( size_t nNewSize, int nGrowBy )
{
	ATLASSERT_VALID(this);

	if( nGrowBy != -1 )
	{
		m_nGrowBy = nGrowBy;  // set new size
	}

	if( nNewSize == 0 )
	{
		// shrink to nothing
		if( m_pData != NULL )
		{
			CallDestructors( m_pData, m_nSize );
			free( m_pData );
			m_pData = NULL;
		}
		m_nSize = 0;
		m_nMaxSize = 0;
	}
	else if( nNewSize <= m_nMaxSize )
	{
		// it fits
		if( nNewSize > m_nSize )
		{
			// initialize the new elements
			CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );
		}
		else if( m_nSize > nNewSize )
		{
			// destroy the old elements
			CallDestructors( m_pData+nNewSize, m_nSize-nNewSize );
		}
		m_nSize = nNewSize;
	}
	else
	{
		bool bSuccess;

		bSuccess = GrowBuffer( nNewSize );
		if( !bSuccess )
		{
			return( false );
		}

		// construct new elements
		ATLASSERT( nNewSize > m_nSize );
		CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );

		m_nSize = nNewSize;
	}

	return true;
}


template< typename E >
//size_t CPeekArrayDep< E >::Append( const CPeekArrayDep< E >& aSrc )
size_t CPeekArrayDep< E >::Append( const CPeekArrayDep< E >& aSrc )
{
	ATLASSERT_VALID(this);
	ATLASSERT( this != &aSrc );   // cannot append to itself

	size_t nOldSize = m_nSize;
	bool bSuccess=SetCount( m_nSize+aSrc.m_nSize );
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	CopyElements( m_pData+nOldSize, aSrc.m_pData, aSrc.m_nSize );

	return( nOldSize );
}

template< typename E >
void CPeekArrayDep< E >::Copy( const CPeekArrayDep< E >& aSrc )
{
	ATLASSERT_VALID(this);
	ATLASSERT( this != &aSrc );   // cannot append to itself

	bool bSuccess=SetCount( aSrc.m_nSize );
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	CopyElements( m_pData, aSrc.m_pData, aSrc.m_nSize );
}

template< typename E >
void CPeekArrayDep< E >::FreeExtra() throw()
{
	ATLASSERT_VALID(this);

	if( m_nSize != m_nMaxSize )
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ATLASSUME( m_nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
		E* pNewData = NULL;
		if( m_nSize != 0 )
		{
			pNewData = (E*)calloc( m_nSize,sizeof( E ) );
			if( pNewData == NULL )
			{
				return;
			}

			// copy new data from old
			RelocateElements( pNewData, m_pData, m_nSize );
		}

		// get rid of old stuff (note: no destructors called)
		free( m_pData );
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template< typename E >
void CPeekArrayDep< E >::SetAtGrow( size_t iElement, INARGTYPE element )
{
	ATLASSERT_VALID(this);
	size_t nOldSize;

	nOldSize = m_nSize;
	if( iElement >= m_nSize )
	{
		bool bSuccess=SetCount( iElement+1, -1 );
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}

	_ATLTRY
	{
		m_pData[iElement] = element;
	}
	_ATLCATCHALL()
	{
		if( m_nSize != nOldSize )
		{
			SetCount( nOldSize, -1 );
		}
		_ATLRETHROW;
	}
}

template< typename E >
void CPeekArrayDep< E >::InsertAt( size_t iElement, INARGTYPE element, size_t nElements /*=1*/)
{
	ATLASSERT_VALID(this);
	ATLASSERT( nElements > 0 );     // zero size not allowed

	if( iElement >= m_nSize )
	{
		// adding after the end of the array
		bool bSuccess=SetCount( iElement+nElements, -1 );   // grow so nIndex is valid
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}
	else
	{
		// inserting in the middle of the array
		size_t nOldSize = m_nSize;
		bool bSuccess=SetCount( m_nSize+nElements, -1 );  // grow it to new size
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
		// destroy intial data before copying over it
		CallDestructors( m_pData+nOldSize, nElements );
		// shift old data up to fill gap
		RelocateElements( m_pData+(iElement+nElements), m_pData+iElement,
			nOldSize-iElement );

		_ATLTRY
		{
			// re-init slots we copied from
			CallConstructors( m_pData+iElement, nElements );
		}
		_ATLCATCHALL()
		{
			RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
				nOldSize-iElement );
			SetCount( nOldSize, -1 );
			_ATLRETHROW;
		}
	}

	// insert new value in the gap
	ATLASSERT( (iElement+nElements) <= m_nSize );
	for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
	{
		m_pData[iNewElement] = element;
	}
}

template< typename E >
void CPeekArrayDep< E >::RemoveAt( size_t iElement, size_t nElements )
{
	ATLASSERT_VALID(this);
	ATLASSERT( (iElement+nElements) <= m_nSize );

	size_t newCount = iElement+nElements;
	if ((newCount < iElement) || (newCount < nElements) || (newCount > m_nSize))
		AtlThrow(E_INVALIDARG);

	// just remove a range
	size_t nMoveCount = m_nSize-(newCount);
	CallDestructors( m_pData+iElement, nElements );
	if( nMoveCount > 0 )
	{
		RelocateElements( m_pData+iElement, m_pData+(newCount),
			nMoveCount );
	}
	m_nSize -= nElements;
}

template< typename E >
void CPeekArrayDep< E >::InsertArrayAt( size_t iStartElement,
	const CPeekArrayDep< E >* paNew )
{
	ATLASSERT_VALID( this );
	ATLENSURE( paNew != NULL );
	ATLASSERT_VALID( paNew );

	if( paNew->GetCount() > 0 )
	{
		InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
		for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
		{
			SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
		}
	}
}

#ifdef _DEBUG
template< typename E >
void CPeekArrayDep< E >::AssertValid() const
{
	if( m_pData == NULL )
	{
		ATLASSUME( m_nSize == 0 );
		ATLASSUME( m_nMaxSize == 0 );
	}
	else
	{
		ATLASSUME( m_nSize <= m_nMaxSize );
		ATLASSERT( AtlIsValidAddress( m_pData, m_nMaxSize * sizeof( E ) ) );
	}
}
#endif

#pragma push_macro("new")
#undef new

template< typename E >
void CPeekArrayDep< E >::CallConstructors( E* pElements, size_t nElements )
{
	size_t iElement = 0;

	_ATLTRY
	{
		for( iElement = 0; iElement < nElements; iElement++ )
		{
			::new( pElements+iElement ) E;
		}
	}
	_ATLCATCHALL()
	{
		while( iElement > 0 )
		{
			iElement--;
			pElements[iElement].~E();
		}

		_ATLRETHROW;
	}
}

#pragma pop_macro("new")

template< typename E >
void CPeekArrayDep< E >::CallDestructors( E* pElements, size_t nElements ) throw()
{
	(void)pElements;

	for( size_t iElement = 0; iElement < nElements; iElement++ )
	{
		pElements[iElement].~E();
	}
}


// =============================================================================
//		class CPeekArrayEx
//
//  This class supports array use whereby the internal array buffer does not
//  shrink to zero, but where the starting position may be repeatedly reset
//  to zero.  This can give efficiency gains in certain situations.
//
//	To use the array in this manner, adds should be performed using the AddRT()
//  function, and the resets to zero should be performed with the Reset()
//  function.  The default behavior of CPeekArrayDep is otherwise maintained.
// =============================================================================

template< typename E >
class CPeekArrayEx
	: public CPeekArrayDep< E >
{
public:

	CPeekArrayEx() {}
	CPeekArrayEx( size_t inInitialSize, size_t inGrowAmount ) {
		Initialize( inInitialSize, inGrowAmount );
	}
	~CPeekArrayEx() {}

	// Adds to the buffer from current position.  Will dynamically grow the buffer
	// if necessary by the grow amount.
	size_t AddRT( E& inElement ) {
		CheckGrow();

		size_t nPosition = m_nSize++;
		SetAt( nPosition, inElement );

		return( nPosition );
	}

	// After calling Reset, calls to AddRT will start adding from position 0.
	// Note that this does not shrink the buffer.  To shrink the buffer to 0,
	// RemoveAll() should be called.
	void	Reset() {
		m_nSize  = 0;
	}

	// A value of 0 for inGrowAmount will heuristically determine the grow
	// amount based on the existing size of the buffer.
	// Note: this is an alternative to the SetCount() function.  A subsequent
	// call to SetCount will override any initialization done here.
	void	Initialize( size_t inInitialSize, size_t inGrowAmount ) {
		ASSERT( m_nSize == 0 );  // Do not initialize after any other business has been done.
		ASSERT( m_nGrowBy == 0 );
		m_nGrowBy = int( inGrowAmount );
		m_nSize = inInitialSize;

		if ( inInitialSize > 0 ) GrowBuffer( m_nSize );
	}

protected:
	void	CheckGrow() {
		if ( m_nSize >= m_nMaxSize ) {
			ASSERT( m_nSize == m_nMaxSize );
			size_t nGrowBy = m_nGrowBy;
			if( nGrowBy == 0 ) {
				nGrowBy = m_nSize/8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
			}
			GrowBuffer( m_nSize + nGrowBy );
		}
	}
};
#endif
