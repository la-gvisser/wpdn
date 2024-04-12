// ============================================================================
//	CaptureLogger.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "XFile.h"


// =============================================================================
//		CCaptureLogger
// =============================================================================

class CCaptureLogger
{

public:
	CCaptureLogger() {}
	~CCaptureLogger() { 
		Close(); 
	}

	void Close();
	bool Write( const CPeekString& strText );

	bool Open( const CPeekString& strFileName ) {
		m_File.SetFileSpec( strFileName );
		return( Open() );
	}
	bool Open() {
		ASSERT( (m_strInProgressFileName.GetLength() > 0) && m_strClosedFileName.GetLength() > 0  );
		if ( m_strInProgressFileName.GetLength() == 0 ) return false;

		m_File.SetFileSpec( m_strInProgressFileName );
		bool bReturn = ( (m_File.Open( kWritePerm, kFile_CreateNew )) == TRUE ) ? true : false;
		return bReturn;
	}
	bool IsOpen() {
		return m_File.IsOpen();
	}

protected:
	void	SetFileNames( const CPeekString& strInProgressFileName, const CPeekString& strClosedFileName ) {
		ASSERT ( strInProgressFileName.GetLength() > 0 );
		ASSERT ( strClosedFileName.GetLength() > 0 );
		m_strInProgressFileName = strInProgressFileName;
		m_strClosedFileName = strClosedFileName;
	}

protected:
	XFile		m_File;
	CPeekString	m_strInProgressFileName;
	CPeekString	m_strClosedFileName;
};

