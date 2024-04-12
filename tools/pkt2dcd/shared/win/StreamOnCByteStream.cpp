// ============================================================================
//	StreamOnCByteStream.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2003-2005. All rights reserved.

#include "StdAfx.h"
#include "StreamOnCByteStream.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// ----------------------------------------------------------------------------
//		CStreamOnCByteStream
// ----------------------------------------------------------------------------

CStreamOnCByteStream::CStreamOnCByteStream()
{
	m_ByteStream.SetChunkSize( 64 * 1024L );	// 64 KB chunk size by default.
}


// ----------------------------------------------------------------------------
//		~CStreamOnCByteStream
// ----------------------------------------------------------------------------

CStreamOnCByteStream::~CStreamOnCByteStream()
{
}


// ----------------------------------------------------------------------------
//		Seek
// ----------------------------------------------------------------------------

HRESULT
CStreamOnCByteStream::Seek(
	LARGE_INTEGER	dlibMove,  
	DWORD			dwOrigin,  
	ULARGE_INTEGER*	plibNewPosition )
{
	HRESULT	hr = S_OK;

	try
	{
		switch ( dwOrigin )
		{
			case STREAM_SEEK_SET:
			{
				if ( dlibMove.QuadPart < 0 )
				{
					// It's an error to seek before the beginning of the stream.
					return E_INVALIDARG;
				}
				else // if ( dlibMove.QuadPart >= 0 )
				{
					// Now set the marker.
					UInt32	nOffset = (UInt32) dlibMove.QuadPart;
					m_ByteStream.SetMarker( nOffset, CByteStream::kMarker_Begin );

					// TODO: expected behavior for IStream is to extend the
					// stream size when seeking beyond the end of the stream,
					// but that's not what CByteStream does!
				}
			}
			break;

			case STREAM_SEEK_CUR:
			{
				const UInt32	nMarker = m_ByteStream.GetMarker();

				if ( dlibMove.QuadPart < 0 )
				{
					// Seeking backwards from the current marker.
					const UInt32	nMove = (UInt32) -dlibMove.QuadPart;
					if ( nMove > nMarker )
					{
						// It's an error to seek before the beginning of the stream.
						return E_INVALIDARG;
					}
					else
					{
						// Now set the marker.
						UInt32	nOffset = nMarker - nMove;
						m_ByteStream.SetMarker( nOffset, CByteStream::kMarker_Begin );
					}
				}
				else
				{
					// Now set the marker.
					UInt32	nOffset = (UInt32) dlibMove.QuadPart;
					m_ByteStream.SetMarker( nOffset, CByteStream::kMarker_Current );

					// TODO: expected behavior for IStream is to extend the
					// stream size when seeking beyond the end of the stream,
					// but that's not what CByteStream does!
				}
			}
			break;

			case STREAM_SEEK_END:
			{
				UInt32	nOffset;
				if ( dlibMove.QuadPart < 0 )
				{
					nOffset = (UInt32) -dlibMove.QuadPart;
				}
				else
				{
					nOffset = (UInt32) dlibMove.QuadPart;
				}

				if ( nOffset > m_ByteStream.GetLength() )
				{
					// It's an error to seek before the beginning of the stream.
					return E_INVALIDARG;
				}

				// Now set the marker.
				m_ByteStream.SetMarker( nOffset, CByteStream::kMarker_End );
			}
			break;

			default:
			{
				// Invalid value for dwOrigin.
				return STG_E_INVALIDFUNCTION;
			}
			break;
		}
		
		// Now get the new position, if necessary.
		if ( plibNewPosition != NULL )
		{
			plibNewPosition->QuadPart = (ULONGLONG) m_ByteStream.GetMarker();
		}
	}
	catch ( ... )
	{
		hr = E_FAIL;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		SetSize
// ----------------------------------------------------------------------------

HRESULT
CStreamOnCByteStream::SetSize(
	ULARGE_INTEGER	nNewLen )
{	
	HRESULT	hr = S_OK;

	try
	{
		// Check overflow.
		if ( nNewLen.QuadPart > ULONG_MAX ) return E_INVALIDARG;

		// Set the stream length.
		m_ByteStream.SetLength( (UInt32) nNewLen.QuadPart );
	}
	catch ( ... )
	{
		// CByteStream throws -1 due to OutOfMem
		hr = STG_E_MEDIUMFULL;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		Read
// ----------------------------------------------------------------------------

HRESULT
CStreamOnCByteStream::Read(
	void*	pv,
	ULONG	cb,
	ULONG*	pcbRead )
{
	// Sanity checks
	if ( pv == NULL ) return E_POINTER;

	HRESULT	hr = S_OK;

	try
	{
		// Get the current marker.
		UInt32	nMarker = m_ByteStream.GetMarker();

		// Find out the length of our buffer.
		ULONG	nDataLen = m_ByteStream.GetLength() - nMarker;
		if ( cb > nDataLen )
		{
			cb = nDataLen;
		}

		// Copy the data.
		memcpy( pv, (char*) m_ByteStream.GetData() + nMarker, cb );

		// Advance the marker.
		m_ByteStream.SetMarker( cb, CByteStream::kMarker_Current );

		// Note that according to spec of ISequentialStream, pcbRead may be NULL.
		if ( pcbRead != NULL )
		{
			*pcbRead = cb;
		}
	}
	catch ( ... )
	{
		hr = E_FAIL;
	}

	return hr;
}


// ----------------------------------------------------------------------------
//		Write
// ----------------------------------------------------------------------------

HRESULT
CStreamOnCByteStream::Write(
	const void*	pv,
	ULONG		cb,
	ULONG*		pcbWritten )
{
	// Sanity checks
	if ( pv == NULL ) return E_POINTER;

	HRESULT	hr = S_OK;

	try
	{
		if ( pcbWritten != NULL )
		{
			*pcbWritten = 0;
		}

		if ( cb == 0 ) return S_OK;

		UInt32	nCurMarker = m_ByteStream.GetMarker();
		UInt32	nRemainingLen = m_ByteStream.GetLength() - nCurMarker;

		if ( nRemainingLen < cb )
		{
			try
			{
				// Need to allocate space for the buffer. Handled by the 
				// CByteStream automatically. We just need to SetLength()
				UInt32 nNewSize = m_ByteStream.GetLength() + (cb - nRemainingLen);
				m_ByteStream.SetLength( nNewSize );
			}
			catch ( ... )
			{
				// CByteStream throws -1 due to OutOfMem
				return STG_E_MEDIUMFULL;
			}
		}

		// Now write.
		memcpy( (char*) m_ByteStream.GetData() + nCurMarker, (char*) pv, cb );

		// Update the marker.
		m_ByteStream.SetMarker( nCurMarker + cb, CByteStream::kMarker_Begin );

		// The following maybe NULL according to the spec.
		if ( pcbWritten != NULL )
		{
			*pcbWritten = cb;
		}
	}
	catch ( ... )
	{
		// CByteStream throws -1 due to OutOfMem
		hr = STG_E_MEDIUMFULL;
	}

	return hr;
}
