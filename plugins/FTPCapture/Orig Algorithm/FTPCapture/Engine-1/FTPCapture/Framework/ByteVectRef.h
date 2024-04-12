// ============================================================================
//	ByteVectRef.h
//		interface to CByteVectRef
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#pragma once

#include "PeekStrings.h"


// ============================================================================
//      CByteVectRef
// ============================================================================

class CByteVectRef
{
protected:
	const UInt8*	m_pData;
	size_t			m_nCount;

public:
	;		CByteVectRef( size_t inCount = 0, const UInt8* inData = NULL )
		:	m_nCount( inCount )
		,	m_pData( inData )
		{}

	;				operator CPeekString();

	size_t			GetCount() const { return m_nCount; }
	const UInt8*	GetData( UInt64 inMin ) const { return (inMin <= m_nCount) ? m_pData : NULL; }

	CByteVectRef	Left( size_t inCount ) {
		if ( inCount > m_nCount ) return *this;
		return CByteVectRef( inCount, m_pData );
	}

	bool			IsEmpty() const { return ((m_pData == NULL) && (m_nCount == 0));}

	void			Reset() { Set( 0, NULL ); }
	CByteVectRef	Right( size_t inCount ) {
		if ( inCount > m_nCount ) return *this;
		return CByteVectRef( inCount, (m_pData + (m_nCount - inCount)) );
	}

	CByteVectRef	Seek( size_t inCount ) {
		if ( inCount > m_nCount ) return *this;
		return CByteVectRef( (m_nCount - inCount), (m_pData + inCount) );
	}
	void			Set( size_t inCount, const UInt8* inData ) {
		m_nCount = inCount;
		m_pData = inData;
	}

	void			Test( size_t inMax = static_cast<size_t>( -1 ) ) {
		if ( (m_pData == NULL) || (m_nCount == 0) || (m_nCount > inMax) ) {
			m_pData = NULL;
			m_nCount = 0;
		}
	}
};
