// ============================================================================
//	ByteVectRef.h
//		interface to CByteVectRef
// ============================================================================
//	Copyright (c) 2009-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekStrings.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Byte Vector Reference
//
//	A Byte Vector Reference contains a pointer to an array (a vector) of bytes 
//	and a count of how many bytes are in the buffer. The inMin argument to the
//	GetData member helps ensure that the array contains at least the number of
//	bytes needed.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// ============================================================================
//      CByteVectRef
// ============================================================================

class CByteVectRef
{
protected:
	UInt8*		m_pData;
	size_t		m_nCount;

public:
	;		CByteVectRef( size_t inCount = 0, UInt8* inData = nullptr )
		:	m_pData( inData )
		,	m_nCount( inCount )
		{}
	;		CByteVectRef( size_t inCount, const UInt8* inData )
		:	m_pData( const_cast<UInt8*>( inData ) )
		,	m_nCount( inCount )
		{}

	;				operator CPeekString();		// Returns a copy of the data as a CPeekString.
	;				operator CPeekStringA();	// Returns a copy of the data as a CPeekStringA.

	size_t			GetCount() const { return m_nCount; }
	UInt8*			GetData( UInt64 inMin ) const { return (inMin <= m_nCount) ? m_pData : nullptr; }

	CByteVectRef	Left( size_t inCount ) {
		if ( inCount > m_nCount ) return *this;
		return CByteVectRef( inCount, m_pData );
	}

	bool			IsEmpty() const { return ((m_pData == nullptr) || (m_nCount == 0));}

	void			Reset() { Set( 0, nullptr ); }
	CByteVectRef	Right( size_t inCount ) {
		if ( inCount > m_nCount ) return *this;
		return CByteVectRef( inCount, (m_pData + (m_nCount - inCount)) );
	}

	CByteVectRef	Seek( size_t inCount ) {
		if ( inCount > m_nCount ) return *this;
		return CByteVectRef( (m_nCount - inCount), (m_pData + inCount) );
	}
	void			Set( size_t inCount, UInt8* inData ) {
		m_nCount = inCount;
		m_pData = inData;
	}

	void			Test( size_t inMax = static_cast<size_t>( -1 ) ) {
		if ( (m_pData == nullptr) || (m_nCount == 0) || (m_nCount > inMax) ) {
			m_pData = nullptr;
			m_nCount = 0;
		}
	}
};