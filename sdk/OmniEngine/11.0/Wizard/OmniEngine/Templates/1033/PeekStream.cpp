// =============================================================================
//	PeekStream.cpp
// =============================================================================
//	Copyright (c) 2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekStream.h"
#include "ByteVectRef.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek Stream
//
//	A Peek Stream is a wrapper for the core Omni stream class. The Peek Stream
//	class adds some additional services to Omni's stream class.
//	A Stream is an expandable memory buffer with file i/o semantics: read, seek,
//	write.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CPeekStream
// =============================================================================

CPeekStream::CPeekStream(
	CByteVectRef inData )
{
	CreateStream();
	Write( inData.GetCount(), inData.GetData( inData.GetCount() ) );
}


// ----------------------------------------------------------------------------
//		GetDataRef
// ----------------------------------------------------------------------------

CByteVectRef
CPeekStream::GetDataRef() const
{
	CByteVectRef	bvData( GetLength(), GetData() );
	return bvData;
}
