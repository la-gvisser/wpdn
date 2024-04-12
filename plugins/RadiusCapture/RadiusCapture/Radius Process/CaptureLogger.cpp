// ============================================================================
//	CaptureLogger.cpp 
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "CaptureLogger.h"


////////////////////////////////////////////////////////////////////////////////
//  CCaptureLogger
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

void
CCaptureLogger::Close()
{
	if ( IsOpen() ) {
		m_File.Close();
		ASSERT( m_strInProgressFileName.GetLength() > 0 );
		ASSERT( m_strClosedFileName.GetLength() > 0 );
		if ( (m_strInProgressFileName.GetLength() > 0) && (m_strClosedFileName.GetLength() > 0) ) {
			_wrename( (LPCTSTR)m_strInProgressFileName, (LPCTSTR)m_strClosedFileName );
		}
	}
}

// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

bool
CCaptureLogger::Write( 
	const CPeekString& strText )
{
	ASSERT( IsOpen() );
	if ( !IsOpen() ) return false;

	CPeekStringA strTextA( strText );
	const void * pBuffer = strTextA.Format();
	const UInt32 nLength = static_cast<UInt32>( strTextA.GetLength() );
	ASSERT( nLength > 0 );
	UInt32 nNumBytesWritten = m_File.Write( pBuffer, nLength );
	if ( nNumBytesWritten == 0 ) {
		ASSERT( 0 );
		return false;
	}
	return true;
}
