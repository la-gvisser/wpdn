// ====================================================================
//	THashMap
// ====================================================================
//	Copyright (c) WildPackets, Inc. 2003. All rights reserved.
#pragma once

#include <list>
#include <utility>	// for std::pair
#include <vector>

// --------------------------------------------------------------------
///  @class Hashor
///
///	Default hash function for your THashMap if you're trivially 
/// convertible to int (or if your Key has operator int), you
/// don't need to supply a hash function.
// --------------------------------------------------------------------
template < class T >
class THashor
{
public :
	int
	hashcode( const T & inT ) const
	{
		return (int) inT;
	}
};

typedef std::vector< size_t > HashDistVector;

// --------------------------------------------------------------------
///		@class THashMap
///
///	A hash map, map[ key ] = value.
///
///	NOT a real STL container, lacks iterators and references. I'll
/// add 'em if I really really need em. No references, so there's
/// no reference & operator [ Key ] for put.
///
///	Works as a single map (one-key-to-one-value) or multi-map
/// (one-key-to-many-values). Use get() for single maps, and
/// getall() for multimaps.
// --------------------------------------------------------------------

template < class Key, class T, class HashFunc = THashor< Key > >
class THashMap
{
public :
	enum { DEFAULT_BUCKET_COUNT = 4 * 1024 - 1 };
	enum
	{
		kIsMultiMap	= true,
		kIsNotMultiMap	= false
	};
	
	typedef THashMap< Key, T, HashFunc >	ThisType;
	typedef std::pair< Key, T >				Element;
	typedef std::list< Element >			Bucket;
	typedef std::vector< Bucket >			BucketVector;
	
//	typedef std::pair< typename Bucket, typename Bucket::const_iterator > const_iterator;

	/// @class find_iterator -- to iterate through all search hits in 
	///							a multimap
	class find_iterator
	{
	public :
		find_iterator()
		:	m_Key(),
			m_pBucket( NULL ),
			m_BucketIterator()
		{
		}
		
		find_iterator( const find_iterator & inOriginal )
		:	m_Key(				inOriginal.m_Key ),
			m_pBucket(			inOriginal.m_pBucket	),
			m_BucketIterator(	inOriginal.m_BucketIterator )
		{
		}
		
		find_iterator &
		operator = ( const find_iterator & inOriginal )
		{
			m_Key				= inOriginal.m_Key;
			m_pBucket			= inOriginal.m_pBucket;
			m_BucketIterator	= inOriginal.m_BucketIterator;
			return *this;
		}
		
		find_iterator(
			const Key &								inKey,
			const typename Bucket *					inBucket,
			const typename Bucket::const_iterator &	inBucketIterator )
		:	m_Key(				inKey ),
			m_pBucket(			inBucket ),
			m_BucketIterator(	inBucketIterator )
		{
		}
		
		~find_iterator()
		{
		}
		
		bool	isvalid() const
		{
			if ( NULL == m_pBucket ) return false;
			if ( m_BucketIterator == m_pBucket->end() ) return false;
			return true;
		}
		
		const T & operator * () const
		{
			return (*m_BucketIterator).second;
		}
		
		find_iterator& operator++()
		{
			if ( !this->isvalid() ) return *this;
			for ( ++m_BucketIterator ; m_BucketIterator != m_pBucket->end();
					++m_BucketIterator )
			{
				if ((*m_BucketIterator).first == m_Key ) break;
			}
			return *this;
		}

	private :
		Key										m_Key;
		const Bucket *							m_pBucket;
		typename Bucket::const_iterator			m_BucketIterator;
		
	};	// class find_iterator
	
	class const_iterator
	{
	public :
		typedef std::forward_iterator_tag iterator_category;
		typedef T value_type;
		typedef typename std::allocator< T >::difference_type	difference_type;
		typedef typename std::allocator< T >::pointer			pointer;
		typedef typename std::allocator< T >::reference			reference;


		// end()maker
		const_iterator(
			const typename Bucket &							inBucketBack,
			const typename BucketVector::const_iterator	&	inBucketEnd )
		:	m_BucketCurrent(	inBucketEnd ),
			m_BucketEnd(		inBucketEnd	),
			m_ListCurrent(		inBucketBack.end() ),
			m_ListEnd(			inBucketBack.end() )
		{
		}
		explicit
		const_iterator(
			const typename BucketVector & inBucketVector )
		:	m_BucketCurrent(	inBucketVector.begin() ),
			m_BucketEnd(		inBucketVector.end()	),
			m_ListCurrent(),
			m_ListEnd()
		{
			if ( m_BucketCurrent != m_BucketEnd )
			{
				m_ListCurrent		= m_BucketCurrent->begin();
				m_ListEnd			= m_BucketCurrent->end();
				if ( m_ListCurrent == m_ListEnd )
				{
					this->advanceToNextValid();
				}
			}
		}
		const_iterator( const const_iterator & inOriginal )
		:	m_BucketCurrent(	inOriginal.m_BucketCurrent	),
			m_BucketEnd(		inOriginal.m_BucketEnd		),
			m_ListCurrent(		inOriginal.m_ListCurrent	),
			m_ListEnd(			inOriginal.m_ListEnd		)
		{
		}
		const_iterator()
		:	m_BucketCurrent(),
			m_BucketEnd(),
			m_ListCurrent(),
			m_ListEnd()
		{
		}
		~const_iterator()
		{
		}
		const_iterator & operator = ( const const_iterator & inOriginal )
		{
			m_BucketCurrent	= inOriginal.m_BucketCurrent;
			m_BucketEnd		= inOriginal.m_BucketEnd;
			m_ListCurrent		= inOriginal.m_ListCurrent;
			m_ListEnd		= inOriginal.m_ListEnd;
			return *this;
		}
		bool operator == ( const const_iterator & inB ) const
		{
			return (( m_BucketCurrent == inB.m_BucketCurrent )
				&&  ( m_BucketEnd	  == inB.m_BucketEnd	 )
				&&	( m_ListCurrent	  == inB.m_ListCurrent	 )
				&&	( m_ListEnd		  == inB.m_ListEnd		 ));
		}
		bool operator != ( const const_iterator & inB ) const
		{
			return (( m_BucketCurrent != inB.m_BucketCurrent )
				||  ( m_BucketEnd	  != inB.m_BucketEnd	 )
				||	( m_ListCurrent	  != inB.m_ListCurrent	 )
				||	( m_ListEnd		  != inB.m_ListEnd		 ));
		}
		const_iterator & operator ++ ()	// preincrement
		{
			this->advanceToNextValid();
			return *this;
		}
		const_iterator operator ++ ( int )	// postincrement
		{
			const_iterator	result	= *this;
			this->advanceToNextValid();
			return result;
		}
		const T &
		operator * () const
		{
			return m_ListCurrent->second;
		}
		const T *
		operator -> () const
		{
			return & (m_ListCurrent->second);
		}
	private :
		void advanceToNextValid()
		{
			// if current list has more, use it
			if ( m_ListCurrent != m_ListEnd )
			{
				++m_ListCurrent;
			}
			
			// if current list done, find next non-empty list
			for ( ; ( m_ListCurrent == m_ListEnd ) && ( ++m_BucketCurrent != m_BucketEnd ); ) 
			{	
				m_ListCurrent	= m_BucketCurrent->begin();
				m_ListEnd		= m_BucketCurrent->end();
			}
		}
	protected :
		typename BucketVector::const_iterator	m_BucketCurrent;
		typename BucketVector::const_iterator	m_BucketEnd;
		typename Bucket::const_iterator			m_ListCurrent;
		typename Bucket::const_iterator			m_ListEnd;
	};	// end class const_iterator
	
	
	class iterator
	:	public const_iterator
	{
	private :
		typedef const_iterator inherited;
	public :
		typedef std::forward_iterator_tag iterator_category;
		typedef T value_type;
		typedef typename std::allocator< T >::difference_type	difference_type;
		typedef typename std::allocator< T >::pointer			pointer;
		typedef typename std::allocator< T >::reference			reference;

		// end()maker
		iterator(
			typename Bucket &				   		inBucketBack,
			const typename BucketVector::iterator &	inBucketEnd )
		:	inherited( inBucketBack, inBucketEnd )
		{
		}
		explicit
		iterator(
			typename BucketVector & inBucketVector )
		:	inherited( inBucketVector )
		{
		}
		iterator( const iterator & inOriginal )
		:	inherited( inOriginal )
		{
		}
		iterator()
		:	inherited()
		{
		}
		~iterator()
		{
		}
		iterator & operator = ( const iterator & inOriginal )
		{
			(void) inherited::operator = ( inOriginal );
			return *this;
		}
		iterator & operator ++ ()	// preincrement
		{
			(void) inherited::operator ++ ();
			return *this;
		}
		iterator operator ++ ( int )	// postincrement
		{
			iterator	result	= *this;
			(void*) inherited::operator++( 0 );
			return result;
		}
		T &
		operator * () const
		{
			return (T&) m_ListCurrent->second;
		}
		T *
		operator -> () const
		{
			return & (T&) (m_ListCurrent->second);
		}
	};	// end class iterator
	
	// begin THashMap	
	explicit
	THashMap(
		const size_t	inBucketCount	= DEFAULT_BUCKET_COUNT,
		const bool		inIsMultiMap	= kIsNotMultiMap )
	:	m_BucketVector( inBucketCount ),
		m_Hashor(),
		m_bIsMultiMap( inIsMultiMap ),
		m_nSize( 0 ),
		m_nBucketSizeHighWaterMark( 0 )
	{
	}
	
	THashMap( const THashMap & inOriginal )
	:	m_BucketVector( inOriginal.m_BucketVector ),
		m_Hashor(),
		m_bIsMultiMap( inOriginal.m_bIsMultiMap ),
		m_nSize( inOriginal.m_nSize ),
		m_nBucketSizeHighWaterMark( inOriginal.m_nBucketSizeHighWaterMark )
	{
	}
	
	~THashMap()
	{
	}
	
	THashMap & operator = ( const THashMap & inOriginal )
	{
		m_BucketVector	= inOriginal.m_BucketVector;
		m_bIsMultiMap	= inOriginal.m_bIsMultiMap;
		m_nSize			= inOriginal.m_nSize;
		m_nBucketSizeHighWaterMark	= inOriginal.m_nBucketSizeHighWaterMark;
		return *this;
	}
	
	T *
	put( const Key & inKey, const T & inValue )
	{
		const int		nHash			= m_Hashor.hashcode( inKey );
		const size_t	nBucketCount	= m_BucketVector.size();
		const size_t	nBucketIndex	= nHash % nBucketCount;
		Bucket &		bucket = m_BucketVector[ nBucketIndex ];
		if ( false == m_bIsMultiMap )
		{
			// if not multimap, remove any previous entry for this key
			for ( Bucket::iterator iter = bucket.begin();
					bucket.end() != iter; ++iter )
			{
				if ( inKey == iter->first )
				{
					bucket.erase( iter );
					--m_nSize;
					break;
				}
			} 
		}
		bucket.push_back( Element( inKey, inValue ) );
		if ( m_nBucketSizeHighWaterMark < bucket.size() )
		{
			m_nBucketSizeHighWaterMark = bucket.size();
		}
		++m_nSize;
		return &( bucket.back().second );
	}

	const T *
	get( const Key & inKey ) const
	{
		const int		nHash			= m_Hashor.hashcode( inKey );
		const size_t	nBucketIndex	= nHash % m_BucketVector.size();
		const Bucket &	bucket			= m_BucketVector[ nBucketIndex ];
		for ( Bucket::const_iterator iter = bucket.begin();
					bucket.end() != iter; ++iter )
		{
			if ( inKey == iter->first )
			{
				return &( iter->second );
			}
		}
		return NULL;
	}
	T *
	get( const Key & inKey )
	{
		const ThisType *	constThis	= this;
		const T *			constResult	= constThis->get( inKey );
		return const_cast< T * >( constResult );
	}
	
	std::vector< T >
	getall( const Key & inKey ) const
	{
		const int		nHash			= m_Hashor.hashcode( inKey );
		const int		nBucketIndex	= nHash % m_BucketVector.size();
		const Bucket &	bucket			= m_BucketVector[ nBucketIndex ];
		std::vector< T >	result;
		for ( Bucket::const_iterator iter = bucket.begin();
					bucket.end() != iter; ++iter )
		{
			if ( inKey == iter->first )
			{
				result.push_back( iter->second );
			}
		}
		return result;
	}
	
	size_t	size() const
	{
		return m_nSize;
	}
	
	void
	remove( const Key & inKey )
	{
		const int		nHash			= m_Hashor.hashcode( inKey );
		const size_t	nBucketIndex	= nHash % m_BucketVector.size();
		Bucket &		bucket			= m_BucketVector[ nBucketIndex ];
		for ( Bucket::iterator iter = bucket.begin();
					bucket.end() != iter; ++iter )
		{
			if ( inKey == iter->first )
			{
				bucket.erase( iter );
				--m_nSize;
				break;
			}
		}
	}
	
	void
	clear()
	{
		for ( BucketVector::iterator iter = m_BucketVector.begin();
			m_BucketVector.end() != iter; ++iter )
		{
			iter->clear();
		}		
		m_nSize	= 0;
		m_nBucketSizeHighWaterMark	= 0;
	}
	
	find_iterator
	find( const Key & inKey ) const
	{
		const int		nHash			= m_Hashor.hashcode( inKey );
		const size_t	nBucketIndex	= nHash % m_BucketVector.size();
		const Bucket &	bucket			= m_BucketVector[ nBucketIndex ];
		for ( Bucket::const_iterator iter = bucket.begin();
					bucket.end() != iter; ++iter )
		{
			if ( inKey == iter->first )
			{
				return find_iterator( inKey, &bucket, iter );
			}
		}
		return find_iterator();
	}
	
	const_iterator	begin() const
	{
		if ( 0 == m_BucketVector.size() ) return const_iterator();
		return const_iterator( m_BucketVector );
	}
	iterator	begin()
	{
		if ( 0 == m_BucketVector.size() ) return iterator();
		return iterator( m_BucketVector );
	}
	
	const_iterator	end() const
	{
		if ( 0 == m_BucketVector.size() ) return const_iterator();
		const typename Bucket &							back = m_BucketVector.back();
		const typename BucketVector::const_iterator	&	end	 = m_BucketVector.end();
		return const_iterator( back, end );
	}
	iterator	end()
	{
		if ( 0 == m_BucketVector.size() ) return iterator();
		typename Bucket &					back = m_BucketVector.back();
		typename BucketVector::iterator		end	 = m_BucketVector.end();
		return iterator( back, end );
	}
	
	/// cheesy instrumentation hook
	size_t getBucketSizeHighWaterMark() const
	{
		return m_nBucketSizeHighWaterMark;
	}
	
	UInt64
	GetMemoryUsed() const
	{
		const size_t	nElementByteCount	= sizeof( Element );
		const size_t	nElementCount		= m_nSize;
		const size_t	nBucketByteCount	= sizeof( Bucket );
		const size_t	nBucketCount		= m_BucketVector.size();
		const size_t	nResult
			= nElementByteCount * nElementCount
			+ nBucketByteCount * nBucketCount;
		return (UInt64) nResult;
	}

	HashDistVector
	GetHashDistVector() const
	{
		HashDistVector result;

		BucketVector::const_iterator iter = m_BucketVector.begin();
		const BucketVector::const_iterator end = m_BucketVector.end();
		for ( ; iter != end; ++iter )
		{
			const Bucket bucket = *iter;
			const size_t nBucketSizeIndex = bucket.size();

			// Resize the result vector if necessary.
			const size_t nOldSize = result.size();
			if ( nBucketSizeIndex >= nOldSize )
			{
				const size_t nNewSize = nBucketSizeIndex + 1;
				result.resize( nNewSize );

				// Initialize all new items to zero.
				for ( size_t i = nOldSize; i < nNewSize; i++ )
				{
					result[ i ] = 0;
				}
			}

			// Add this bucket's size to the distribution.
			size_t & nBucketSizeFrequency = result[ nBucketSizeIndex ];
			nBucketSizeFrequency++;
		}

		return result;
	}
	
private :	
	BucketVector		m_BucketVector;
	HashFunc			m_Hashor;
	bool				m_bIsMultiMap;
	size_t				m_nSize;
	size_t				m_nBucketSizeHighWaterMark;	///< because I'm curious
};
