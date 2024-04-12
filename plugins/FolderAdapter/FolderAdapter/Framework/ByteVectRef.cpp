// ============================================================================
//	ByteVectRef.cpp
//		implements CByteVectRef class
// ============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "ByteVectRef.h"
#include <string>


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


// ----------------------------------------------------------------------------
//		operator CPeekStringA
// ----------------------------------------------------------------------------

CByteVectRef::operator CPeekStringA()
{
	std::string	strTextA;
	strTextA.assign( reinterpret_cast<const char*>( m_pData ), m_nCount );

	return strTextA;
}
