// =============================================================================
//	PeekFileStream.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

using namespace Helium;
using namespace HeLib;

#include "filestream.h"
#include "RefreshFile.h"
#include "FileEx.h"

#define	kMaxStringSize			2095
#define kMaxLineSize			500
#define THROW_ON_FAIL_DEFAULT	true
#define CREATE_PATH_DEFAULT		true


// =============================================================================
//		CPeekFileStream
// =============================================================================

/* 
	IMPORTANT NOTE: If you DO NOT implement the default functionality of throw- 
	on-fail, then right after construction you must either test GetLastError()
	for success or validate with the IsOpen() function.  Otherwise you will not
	know whether the file has properly been opened.  Implementing these functions
	within try-catch blocks will not require this additional validation.
*/

class CPeekFileStream
{
public:
	enum HE_STGM
	{
		MODE_READ				= 0x00000000L,
		MODE_WRITE				= 0x00000001L,
		MODE_READWRITE			= 0x00000002L,
		MODE_SHARE_DENY_NONE	= 0x00000040L,
		MODE_SHARE_DENY_READ	= 0x00000030L,
		MODE_SHARE_DENY_WRITE	= 0x00000020L,
		MODE_SHARE_EXCLUSIVE	= 0x00000010L,
		MODE_CREATE				= 0x00001000L,
		MODE_DELETEONRELEASE	= 0x04000000L
	};

protected:
	CHePtr<IHeStream>		m_spStream;
	CHePtr<IFileStream>		m_spFileStream;
	char					m_ReadBuffer[kMaxStringSize + 1];
	char*					m_pCurrentRead;
	UInt32					m_nReadAvailable;
	bool					m_bEOF;
	HeResult				m_HeLastError;
	bool					m_bThrowOnFail;
	CPeekString				m_strFileName;

/*
//	bool		CopyToStream( CPeekStream& inPeekStream ) {
//		Helium::IHeStream* pstm	= inPeekStream.GetIStreamPtr();
//	}
	bool		CopyTo( Helium::IHeStream* pstm, UInt64	cb, UInt64* pcbRead, UInt64* pcbWritten ) {
		m_HeLastError = m_spStream->CopyTo( pstm, cb, pcbRead, pcbWritten );
		if ( m_HeLastError == HE_S_OK )	return true;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return false;
	}
*/
	bool			CreateFileStream( CPeekString inFilePath, CPeekString inFileName, bool bCreatePath,
									UInt32 grfMode, UInt32 nAttributes );

	IHeStream*		GetPtr(	IFileStream* outFileStream = NULL )	 { 
		if ( outFileStream != NULL )	outFileStream = m_spFileStream;
		return m_spStream; 
	}

	void			Initialize() {
		m_spStream = NULL;
		m_spFileStream = NULL;
		m_HeLastError = HE_S_OK;
		m_pCurrentRead = NULL;
		m_nReadAvailable = 0;
		m_bEOF = false;
	}

	// Only invoke one version of SetPtr or the other, not both
	void			SetPtr( IHeStream* inStreamPtr ) {
		if ( !inStreamPtr )	return;
		Initialize();
		m_spStream = inStreamPtr;
		m_spStream.QueryInterface( &(m_spFileStream.p) );
		ASSERT( m_spFileStream!= NULL );
	}
	void			SetPtr( IFileStream* inFilePtr ) {
		if ( !inFilePtr )	return;
		Initialize();
		m_spFileStream = inFilePtr;
		m_spFileStream.QueryInterface( &(m_spStream.p) );
		ASSERT( m_spStream!= NULL );
	}

	void			ReleasePtr() {	Initialize(); }

/*
#ifdef	_DEBUG
	bool		Validate() { 
		if ( m_spStream == NULL || m_spFileStream == NULL ) {
			return false;
		}
		BOOL	bOpen;
		HeResult hr = m_spFileStream->IsOpen( &bOpen );
		if ( hr != HE_S_OK || !bOpen ) {
			return false;
		}
		return true;
	}
#endif // _DEBUG
*/

public:
	;			CPeekFileStream ( CPeekString inFilePath, CPeekString inFileName, UInt32 grfMode, UInt32 nAttributes, 
									bool bCreatePath = CREATE_PATH_DEFAULT, bool inThrowOnFail = THROW_ON_FAIL_DEFAULT );
	virtual		~CPeekFileStream() {}

	// Note: the setting of m_spStream to NULL by the Initialize() function will close the file.
	// Destruction also automatically closes the file, so it is not generally necessary to 
	// explicitely close it.
	void		Close()	{ Initialize(); }

	bool		EndOfFile() {
		return ( (m_nReadAvailable == 0) && m_bEOF );
	}

	HANDLE		GetFileHandle() {
		if ( m_spFileStream == NULL )	return NULL;
		HANDLE	h( NULL );
		m_HeLastError = m_spFileStream->GetHandle( &h );
		if ( m_HeLastError == HE_S_OK )	return h;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return NULL;
	}
	CPeekString	GetFileName()	{ return m_strFileName; }
	HeResult	GetLastError() { return m_HeLastError; }

	bool		IsNotValid() { return !IsValid(); }
	bool		IsOpen()		{ 
		return	IsValid();
	}
	bool		IsValid() { 
		if ( m_spStream == NULL ) return false;
		return true;
	}

	int			Read( void* outData, UInt32 inBytesToRead );
	int			ReadLine( CPeekString& outString );
	int			ReadString( CPeekString& outString, UInt32 inMaxBytesToRead = kMaxStringSize );
	bool		Rewind() {
		UInt64	nNewPos;
		bool	bReturn = Seek( 0, IHeStream::STREAM_SEEK_SET, &nNewPos );
		if ( bReturn )	m_bEOF = false;
		return bReturn;
	}

	bool		Seek( SInt64 move, UInt32 origin, UInt64*	pNewPos ) {
		m_HeLastError = m_spStream->Seek( move, origin, pNewPos );
		if ( m_HeLastError == HE_S_OK )	return true;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		if ( origin != IHeStream::STREAM_SEEK_END )	m_bEOF = false;
		return false;
	}
	bool		SetSize( UInt64	cbNewSize ) {
		m_HeLastError = m_spStream->SetSize( cbNewSize );
		if ( m_HeLastError == HE_S_OK )	return true;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return false;
	}
/*
	bool		Stat( IHeStream::HE_STATSTG* pstatstg, UInt32 grfStatFlag ) {
		m_HeLastError = m_spFileStream->Stat( pstatstg, grfStatFlag );
		if ( m_HeLastError == HE_S_OK )	return true;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return false;
	}
*/

	bool		UnlockRegion( UInt64 offset, UInt64	cb, UInt32 lockType ) {
		m_HeLastError = m_spStream->UnlockRegion( offset, cb, lockType );
		if ( m_HeLastError == HE_S_OK )	return true;
		if ( m_bThrowOnFail )	ASSERT( 0 ); Peek::Throw( m_HeLastError );
		return false;
	}

	bool		Write( const void* inData, UInt32 inBytesToWrite ) {
		UInt32	nNoOp;
		return	Write( inData, inBytesToWrite, nNoOp );
	}
	bool		Write( const void* inData, UInt32 inBytesToWrite, UInt32& outBytesWritten );
	bool		WriteLine( const CPeekString& inString ) {
		CPeekStringA	theString( inString );
		if ( !Write( theString.Format(), static_cast<UInt32>( theString.GetLength() ) ) )	return false;
		return( Write( "\r\n", 2) );
	}
	bool		WriteLine( const CPeekString& inString, UInt32& outBytesWritten ) {
		CPeekStringA	theString( inString );
		if ( !Write( theString.Format(), static_cast<UInt32>( theString.GetLength() ), outBytesWritten ) )	return false;
		if ( !Write( "\r\n", 2) )	return false;
		outBytesWritten += 2;
	}
	bool		WriteString( const CPeekString& inString ) {
		CPeekStringA	theString( inString );
		return ( Write(	theString.Format(), static_cast<UInt32>( theString.GetLength() ) ) );
	}
	bool		WriteString( const CPeekString& inString, UInt32& outBytesWritten ) {
		CPeekStringA	theString( inString );
		return ( Write(	theString.Format(), static_cast<UInt32>( theString.GetLength() ), outBytesWritten ) );
	}

};
