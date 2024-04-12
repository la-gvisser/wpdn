// =============================================================================
//	OutputFileStream.cpp
// =============================================================================
//	Copyright (c) 1999-2012 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include "OutputFileStream.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif


// -----------------------------------------------------------------------------
//		COutputFileStream(UInt32)
// -----------------------------------------------------------------------------

COutputFileStream::COutputFileStream( 
	UInt32				inChunkSize )
{
	SetChunkSize( inChunkSize );
}


// -----------------------------------------------------------------------------
//		COutputFileStream(const TFileSpec&,UInt32)
// -----------------------------------------------------------------------------

COutputFileStream::COutputFileStream( 
	const TFileSpec&	inSpec ,
	UInt32				inChunkSize )
	: m_File( inSpec )
{
	SetChunkSize( inChunkSize );
	m_File.Open( kWritePerm, kFile_CreateNew );
}


// -----------------------------------------------------------------------------
//		~COutputFileStream
// -----------------------------------------------------------------------------

COutputFileStream::~COutputFileStream()
{
	if ( m_File.IsOpen() )
	{
		Flush( true );
		m_File.Close();
	}
}


// -----------------------------------------------------------------------------
//		SetFileSpec
// -----------------------------------------------------------------------------

void
COutputFileStream::SetFileSpec(
	const TFileSpec&	inSpec )
{
	Flush( true );
	m_File.Close();
	m_File.SetFileSpec( inSpec );
	m_File.Open( kWritePerm, kFile_CreateNew );
}


// -----------------------------------------------------------------------------
//		WriteBytes
// -----------------------------------------------------------------------------

bool	
COutputFileStream::WriteBytes( 
	const void*	inData, 
	UInt32		inSize )
{
	if ( !CByteStream::WriteBytes( inData, inSize ) )
		return false;

	return Flush();
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool	
COutputFileStream::Write( 
	const wchar_t*	inString )
{
	if ( !CByteStream::Write( inString ) )
		return false;

	return Flush();
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
COutputFileStream::Write( 
	const char*	inString )
{
	if ( !CByteStream::Write( inString ) )
		return false;

	return Flush();
}


// -----------------------------------------------------------------------------
//		Flush
// -----------------------------------------------------------------------------

bool
COutputFileStream::Flush(
	bool	inForce /* = false */ )
{
	UInt32	nMarker    = GetMarker();
	UInt32	nChunkSize = GetChunkSize();

	if ( (nMarker >= nChunkSize) || inForce )
	{
		void*	pData = GetData();
		
		if ( (pData != NULL) && (nMarker > 0) )
		{
			if ( nMarker != m_File.Write( pData, nMarker ) )
			{
				return false;
			}
		}
		
		SetMarker( 0, CByteStream::kMarker_Begin );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Delete
// -----------------------------------------------------------------------------

void
COutputFileStream::Delete()
{
	m_File.Delete();
}
