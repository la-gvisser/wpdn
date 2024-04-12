// =============================================================================
//	PeekFileStream.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekFileStream.h"


// =============================================================================
//	CPeekFileStream
// =============================================================================

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

CPeekFileStream::CPeekFileStream(
	CPeekString		inFilePath,
	CPeekString		inFileName,
	UInt32			grfMode,
	UInt32			nAttributes,
	bool			bCreatePath,
	bool			inThrowOnFail ) 
	: m_bThrowOnFail( inThrowOnFail )
{
	Initialize();

	CreateFileStream( inFilePath, inFileName, bCreatePath, grfMode, nAttributes );
}


// -----------------------------------------------------------------------------
//		CreateFileStream
// -----------------------------------------------------------------------------

bool	
CPeekFileStream::CreateFileStream(
	 CPeekString	inFilePath,
	 CPeekString	inFileName,
	 bool			bCreatePath,
	 UInt32			grfMode,
	 UInt32			nAttributes ) 
{
	m_HeLastError = HE_E_PATH_NOT_FOUND;

	if ( !CRefreshFile::IsPathValid( inFilePath ) ) {
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( HE_E_FAIL );
		return false;
	}

	if ( !CRefreshFile::DirectoryExists( inFilePath ) ) {
		if ( !bCreatePath ) {
			if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( HE_E_FAIL );
			return false;
		}

		if ( !CRefreshFile::MakePath( inFilePath ) ) {
			if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( HE_E_FAIL );
			return false;
		}
	}

	CPeekString	strDirectory( inFilePath );
	if ( strDirectory.Right( 1 ) != L"\\" && strDirectory.Right( 1 ) != L"/" ) {
		strDirectory += L'\\';
	}

	CPeekString	strFilePathComplete( strDirectory + inFileName );

	m_strFileName = strFilePathComplete;
	m_HeLastError = FileStream<>::CreateStreamOnFile( m_strFileName.Format(), grfMode, 
													nAttributes, TRUE, &m_spStream.p );
	if ( m_HeLastError != HE_S_OK )	{
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( HE_E_FAIL );
		return false;
	}

	m_spStream.QueryInterface( &(m_spFileStream.p) );
	ASSERT( m_spFileStream!= NULL );

	return true;
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

int		
CPeekFileStream::Read( 
	void*	outData, 
	UInt32	inBytesToRead ) 
{
	m_HeLastError = HE_E_FAIL;

	ASSERT( m_spStream != NULL );
	if ( m_spStream == NULL ) {
		m_HeLastError = HE_E_NULL_POINTER;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return -1;  
	}

	m_bEOF = false;
	UInt32	nBytesRead( 0 );
	m_HeLastError = m_spStream->Read( outData, inBytesToRead, &nBytesRead );

	if ( m_HeLastError == HE_S_OK ) return static_cast<int>( nBytesRead );

	if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );

	return -1;
}


// -----------------------------------------------------------------------------
//		ReadLine
// -----------------------------------------------------------------------------

int		
CPeekFileStream::ReadLine( 
	CPeekString& outString ) 
{
	outString = L"";

	UInt32	nLineEndPos = 0;
	char	LineBuffer[kMaxLineSize + 1];

	if ( m_nReadAvailable == 0 ) {
		m_pCurrentRead = NULL;
	}

	while( (m_nReadAvailable > 0) || !m_bEOF ) 
	{
		if ( m_nReadAvailable == 0 && !m_bEOF ) {
			 m_pCurrentRead = NULL;

			UInt32 nBytesRead = Read( m_ReadBuffer, kMaxStringSize );
			if ( nBytesRead <= 0 ) {
				m_bEOF = true;
				if ( m_nReadAvailable = 0 || m_pCurrentRead == NULL ) {
					m_pCurrentRead = NULL;
					m_nReadAvailable = 0;
					return 0;
				}
			}

			ASSERT( m_ReadBuffer != NULL );

			if ( nBytesRead < kMaxStringSize ) {
				m_bEOF = true;
			}

			m_pCurrentRead = m_ReadBuffer;
			m_nReadAvailable = nBytesRead;
		}

		LineBuffer[nLineEndPos] = *m_pCurrentRead;

		if ( LineBuffer[nLineEndPos] == '\n' || nLineEndPos == kMaxLineSize ) {
			m_pCurrentRead++;
			m_nReadAvailable--;
			break;
		}

		nLineEndPos++;
		m_pCurrentRead++;
		m_nReadAvailable--;
	}

	if ( nLineEndPos == 0 )	return 0;

	if ( LineBuffer[nLineEndPos] == '\n' ) nLineEndPos--;
	if ( nLineEndPos == 0 )	return 0;

	if ( LineBuffer[nLineEndPos] == '\r' ) nLineEndPos--;
	if ( nLineEndPos == 0 )	return 0;

	CPeekStringA	strLine( LineBuffer, nLineEndPos + 1 );
	outString = (CPeekString)strLine;

	return static_cast<int>( nLineEndPos );
}


// -----------------------------------------------------------------------------
//		ReadString
// -----------------------------------------------------------------------------

int		
CPeekFileStream::ReadString( 
	CPeekString& outString, 
	UInt32 inMaxBytesToRead ) 
{
	if ( inMaxBytesToRead > kMaxStringSize )		return -1;

	m_pCurrentRead = NULL;
	m_nReadAvailable = 0;

	int nBytesRead = Read( m_ReadBuffer, inMaxBytesToRead );
	if ( nBytesRead <= 0 )	return nBytesRead;

	const char*	pData( static_cast<const char*>( m_ReadBuffer ) );
	CPeekStringA	strLineA( pData, nBytesRead );
	CPeekString		strLine( strLineA );
	outString = strLine;

	return nBytesRead;
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool	
CPeekFileStream::Write( 
	const void* inData, 
	UInt32 inBytesToWrite, 
	UInt32& outBytesWritten ) 
{
//	ASSERT( Validate() );
	outBytesWritten = 0;
	m_HeLastError = HE_E_FAIL;
	ASSERT( m_spStream != NULL );
	if ( m_spStream == NULL ) {
		m_HeLastError = HE_E_NULL_POINTER;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return false;  
	}

	m_HeLastError = m_spStream->Write( inData, inBytesToWrite, &outBytesWritten );
	if ( m_HeLastError == HE_S_OK ) return true;
	if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
	return false;
}
