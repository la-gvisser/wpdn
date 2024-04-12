// =============================================================================
//	PeekStream.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "ByteVectRef.h"
#include "hlcom.h"
#include "imemorystream.h"
#include <stdlib.h>

#ifndef __max
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef __min
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#endif


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

class CPeekStream
{
	friend class CPeekXml;
	friend class CPeekDataElement;
	friend class CPeekDataModeler;
	friend class CRemotePlugin;

private:
	void		CreateStream() {
		CHePtr<IMemoryStream>	spMemoryStream;
		// m_spStream.Release(); // uncomment if called from other than a constructor.
		Peek::ThrowIfFailed( spMemoryStream.CreateInstance( "PeekUtil.MemoryStream" ) );
		Peek::ThrowIfFailed( spMemoryStream.QueryInterface( &m_spStream.p ) );
	}

protected:
	CHePtr<Helium::IHeStream>	m_spStream;

	Helium::IHeStream*	GetIStreamPtr() const {
		return m_spStream;
	}

public:
	;			CPeekStream() { CreateStream(); }
	;			CPeekStream( CByteVectRef inData ) {
		CreateStream();
		Write( inData.GetCount(), inData.GetData( inData.GetCount() ) );
	}
	;			CPeekStream( Helium::IHeStream*	inStream )
		:	m_spStream( inStream )
	{
		if ( IsNotValid() ) {
			CreateStream();
		}
	}
	virtual		~CPeekStream() {}

	// Returns number of bytes read from otherStream and written into the internal stream.
	size_t		Append( CPeekStream& otherStream ) {
		size_t			nCount = otherStream.GetLength();
		const UInt8*	pData = otherStream.GetData();
		Seek( 0, Helium::IHeStream::STREAM_SEEK_END );
		size_t	nWritten = Write( nCount, pData );
		if ( nCount != nWritten ) Peek::Throw( HE_E_FAIL );
		return nWritten;
	}

	bool		CopyTo( CPeekStream& inDest, size_t inCount, size_t& outRead, size_t& outWrite ) const {
		size_t			nCount = __min( inCount, GetLength() );
		UInt64			nPosition = Position();
		const UInt8*	pData = GetData() + nPosition;
		size_t			nWritten = inDest.Write( nCount, pData );
		if ( nCount != nWritten ) Peek::Throw( HE_E_FAIL );
		outRead = nCount;
		outWrite = nWritten;
		return true;
	}
	bool		CopyTo( CPeekStream& inDest, UInt32 inCount ) const {
		size_t	nRead = 0;
		size_t	nWrite = 0;
		return CopyTo( inDest, inCount, nRead, nWrite );
	}

	// Returns pointer to internal data.
	UInt8*		GetData() const {
		CHePtr<IMemoryStream>	spMemoryStream;
		Peek::ThrowIfFailed( m_spStream.QueryInterface( &spMemoryStream.p ) );
		return spMemoryStream->GetData();
	}
	// Returns number of bytes set in the CByteVectRef.
	CByteVectRef	GetDataRef() const {
		CByteVectRef	bvData( GetLength(), GetData() );
		return bvData;
	}
	// Returns internal data length.
	size_t		GetLength() const {
		CHePtr<IMemoryStream>	spMemoryStream;
		Peek::ThrowIfFailed( m_spStream.QueryInterface( &spMemoryStream.p ) );
		return spMemoryStream->GetDataSize();
	}
	UInt32		GetLength32() const {
		CHePtr<IMemoryStream>	spMemoryStream;
		Peek::ThrowIfFailed( m_spStream.QueryInterface( &spMemoryStream.p ) );
		return (spMemoryStream->GetDataSize() & 0x0FFFFFFFF);
	}

	bool		IsEmpty() const { return (IsValid() && (GetLength() == 0)); }
	bool		IsNotEmpty() const { return (IsValid() && (GetLength() != 0)); }
	bool		IsNotValid() const { return (m_spStream == nullptr); }
	bool		IsValid() const { return (m_spStream != nullptr); }

	UInt64		Position() const { return Seek( 0, Helium::IHeStream::STREAM_SEEK_CUR ); }

	// Returns number of bytes returned in outData.
	UInt32		Read32( const UInt32 inBytes, void* outData ) const {
		UInt32	nBytesRead = 0;
		Peek::ThrowIfFailed( m_spStream->Read( outData, inBytes, &nBytesRead ) );
		return nBytesRead;
	}
	UInt64		Read64( UInt64 inBytes, void* inData ) {
		UInt64	nCount = inBytes;
		UInt8*	pData = static_cast<UInt8*>( inData );
		UInt64	nBytesRead = 0;
		while ( nCount > 0 ) {
			UInt32	nChunk = (nCount < kMaxUInt32) ? static_cast<UInt32>( nCount ) : kMaxUInt32;
			UInt32	nRead = Read32( nChunk, pData );
			if ( nRead == 0 ) break;
			nCount -= nRead;
			nBytesRead += nRead;
			pData += nRead;
		}
		return nBytesRead;
	}
#ifdef _WIN64
	UInt64		Read( UInt64 inBytes, void* inData ) {
		return Read64( inBytes, inData );
	}
#else
	UInt32		Read( UInt32 inBytes, void* inData ) {
		return Read32( inBytes, inData );
	}
#endif
	void		Reset() { SetSize( 0 ); }
	void		Rewind() const {
		Seek( 0, Helium::IHeStream::STREAM_SEEK_SET );
	}

	// Returns number of bytes moved inside the stream.
	//	inOrigin:
	//		STREAM_SEEK_SET = 0;	// From beginning of buffer.
	//		STREAM_SEEK_CUR = 1;	// From current location, inBytesToMove may be negative.
	//		STREAM_SEEK_END = 2;	// From the end of the buffer.
	UInt64		Seek( SInt64 inBytesToMove, UInt32 inOrigin ) const {
		UInt64	nNewPos = 0;
		Peek::ThrowIfFailed( m_spStream->Seek( inBytesToMove, inOrigin, &nNewPos ) );
		return nNewPos;
	}
	// Sets the internal data buffer size.
	void		SetSize( UInt64 inNewSize ) {
		Peek::ThrowIfFailed( m_spStream->SetSize( inNewSize ) );
	}

	// Returns number of bytes written into the internal stream.
	UInt32		Write32( UInt32 inBytes, const void* inData ) {
		UInt32	nBytesWritten = 0;
		Peek::ThrowIfFailed( m_spStream->Write( inData, inBytes, &nBytesWritten ) );
		_ASSERTE( nBytesWritten == inBytes );
		return nBytesWritten;
	}
	UInt64		Write64( UInt64 inBytes, const void* inData ) {
		UInt64			nCount = inBytes;
		const UInt8*	pData = static_cast<const UInt8*>( inData );
		UInt64			nBytesWritten = 0;
		while ( nCount > 0 ) {
			UInt32	nChunk = (nCount < kMaxUInt32) ? static_cast<UInt32>( nCount ) : kMaxUInt32;
			UInt32	nWritten = Write32( nChunk, pData );
			if ( nWritten == 0 ) break;
			nCount -= nWritten;
			nBytesWritten += nWritten;
			pData += nWritten;
		}
		return nBytesWritten;
	}

#ifdef _WIN64
	UInt64		Write( UInt64 inBytes, const void* inData ) {
		return Write64( inBytes, inData );
	}
#else
	UInt32		Write( UInt32 inBytes, const void* inData ) {
		return Write32( inBytes, inData );
	}
#endif

// Not implemented:
//	UInt64	CopyTo( CPeekStream& inStream, UInt64 cb, UInt64& cbRead );
//	void	Revert();
//	void	LockRegion( UInt64 offset, UInt64 cb, UInt32 lockType );
//	void	UnlockRegion( UInt64 offset, UInt64 cb, UInt32 lockType );
//	bool	Stat( HE_STATSTG* pStat, UInt32 grfStatFlag );
//	bool	Clone( CPeekStream& inStream );
};
