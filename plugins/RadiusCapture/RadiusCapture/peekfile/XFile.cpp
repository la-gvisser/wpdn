// =============================================================================
//	XFile.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2003. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#include "StdAfx.h"
#include "XFile.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif


// =============================================================================
//	XFile
// =============================================================================

// -----------------------------------------------------------------------------
//		XFile													   [constructor]
// -----------------------------------------------------------------------------

XFile::XFile()
	: m_FileRef( kFileRef_Invalid )
{
}


// -----------------------------------------------------------------------------
//		XFile(const TFileSpec&)									   [constructor]
// -----------------------------------------------------------------------------

XFile::XFile( const TFileSpec& inSpec )
	: m_FileSpec( inSpec ),
		m_FileRef( kFileRef_Invalid )
{
}


// -----------------------------------------------------------------------------
//		~XFile														[destructor]
// -----------------------------------------------------------------------------

XFile::~XFile()
{
}


// -----------------------------------------------------------------------------
//		SetFileSpec
// -----------------------------------------------------------------------------

void
XFile::SetFileSpec(
	const TFileSpec&	inFileSpec )
{
	if ( IsOpen() )
	{
		Close();
	}

	m_FileSpec = inFileSpec;
}


// -----------------------------------------------------------------------------
//		Open
// -----------------------------------------------------------------------------

bool	
XFile::Open( 
	EFilePerms	inPerms,
	EOpenMethod inOpenMethod,
	UInt32		/*inType*/,		/* '????' */ 
	UInt32		/*inCreator*/	/* '????' */ )
{
	ASSERT( !IsOpen() );

	DWORD	nOpenMethod	= 0;
	if ( inOpenMethod == kFile_OpenExisting )
	{
		nOpenMethod = OPEN_ALWAYS;
	}
	else if ( inOpenMethod == kFile_CreateNew )
	{
		nOpenMethod = CREATE_ALWAYS;
	}
	ASSERT( nOpenMethod != 0 );

	m_FileRef = ::CreateFile( m_FileSpec, inPerms, 0, NULL,
		nOpenMethod, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( m_FileRef == kFileRef_Invalid )
	{
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

bool	
XFile::Close()
{
	if ( IsOpen() )
	{
		::CloseHandle( m_FileRef );
		m_FileRef = kFileRef_Invalid;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

UInt32
XFile::Read( 
	void*	outBuffer, 
	UInt32	inSize ) const
{
	ASSERT( outBuffer != NULL );
	ASSERT( inSize > 0 );
	ASSERT( IsOpen() );

	UInt32	nBytesRead	= 0;
	if ( (outBuffer != NULL) && (inSize > 0) )
	{
		::ReadFile( m_FileRef, outBuffer, inSize, &nBytesRead, NULL );
	}

	return nBytesRead;
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

UInt32	
XFile::Write( 
	const void*	inBuffer, 
	UInt32		inSize )
{
	ASSERT( inBuffer != NULL );
	ASSERT( inSize > 0 );
	ASSERT( IsOpen() );
	
	UInt32	nBytesWritten = 0;
	if ( (inBuffer != NULL) && (inSize > 0) )
	{
		::WriteFile( m_FileRef, inBuffer, inSize, &nBytesWritten, NULL );
	}
	
	return nBytesWritten;
}


// -----------------------------------------------------------------------------
//		SetPosition
// -----------------------------------------------------------------------------

bool
XFile::SetPosition( 
	SInt64		inOffset,
	ESeekType	inSeekType ) const
{
	ASSERT( IsOpen() );

	SInt32	hiOffset = (UInt32) (inOffset >> 32);
	UInt32	loOffset = (UInt32) inOffset;

	DWORD nResult = ::SetFilePointer( m_FileRef, loOffset, &hiOffset, inSeekType );
	return (nResult != (DWORD) -1);
}


// -----------------------------------------------------------------------------
//		GetPosition
// -----------------------------------------------------------------------------

UInt64	
XFile::GetPosition() const
{
	ASSERT( IsOpen() );

	// SetFilePointer actually RETURNS the current file position if you pass
	// zero for offset, and FILE_CURRENT. Love that Win32 hackery.
	UInt32	nPosLo = 0;
	SInt32	nPosHi = 0;
	nPosLo = ::SetFilePointer( m_FileRef, 0L, &nPosHi, FILE_CURRENT );

	UInt64	nPos = nPosHi;
	nPos = (nPos << 32) + nPosLo;

	return nPos;
}


// -----------------------------------------------------------------------------
//		GetSize
// -----------------------------------------------------------------------------

UInt64
XFile::GetSize() const
{
	ASSERT( IsOpen() );

	UInt32	nSizeLo;
	UInt32	nSizeHi = 0;
	nSizeLo = ::GetFileSize( m_FileRef, &nSizeHi );

	UInt64	nSize = nSizeHi;
	nSize = (nSize << 32) + nSizeLo;

	return nSize;
}


// -----------------------------------------------------------------------------
//		SetSize
// -----------------------------------------------------------------------------

bool	
XFile::SetSize( 
	UInt64	inSize )
{
	ASSERT( IsOpen() );

	if ( SetPosition( inSize, kFromStart ) )
	{
		return (::SetEndOfFile( m_FileRef ) == TRUE);
	}

	return false;
}


// -----------------------------------------------------------------------------
//		Delete
// -----------------------------------------------------------------------------

bool
XFile::Delete()
{
	Close();

	return (::DeleteFile( m_FileSpec ) == TRUE);
}

