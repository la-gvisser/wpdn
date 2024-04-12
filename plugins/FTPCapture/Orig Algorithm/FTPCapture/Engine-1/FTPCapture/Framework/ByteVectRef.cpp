// ============================================================================
//	ByteVectRef.cpp
//		implements CByteVectRef class
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#include "StdAfx.h"
#include "ByteVectRef.h"


// ============================================================================
//      CByteVectRef
// ============================================================================

// ----------------------------------------------------------------------------
//		operator CPeekString
// ----------------------------------------------------------------------------

CByteVectRef::operator CPeekString()
{
	std::string	strTextA;
	strTextA.assign( reinterpret_cast<const char*>( m_pData ), m_nCount );

	return (CPeekString( strTextA ));
}
