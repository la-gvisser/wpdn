// =============================================================================
//	StreamOnCByteStream.h
// =============================================================================
//	Copyright (c) WildPackets, Inc.	2003-2004. All rights reserved.

#pragma once

#include "ByteStream.h"

// Our implementation of IStream on	top	of CByteStream.
class CStreamOnCByteStream :
	public IStream
{
public:
	CStreamOnCByteStream();
	virtual ~CStreamOnCByteStream();

	// Give const access to the underlying CByteStream plus
	// adapt access to a few CByteStream apis.
	const CByteStream& GetByteStream() const
	{
		return m_ByteStream;
	}
	CByteStream& GetByteStream()
	{
		return m_ByteStream;
	}

	char* GetRawDataPtr()
	{
		return (char*) m_ByteStream.GetData();
	}

	ULONG GetLength() const
	{
		return (ULONG) m_ByteStream.GetLength();
	}

	void SetChunkSize( UInt32 nChunkSize )
	{
		m_ByteStream.SetChunkSize( nChunkSize );
	}

	bool WriteToFile( LPCTSTR pszFullPath ) const
	{
		return m_ByteStream.WriteToFile( pszFullPath );
	}

	bool ReadFromFile( LPCTSTR pszFullPath )
	{
		return m_ByteStream.ReadFromFile( pszFullPath );
	}

	// IStream
public:
	HRESULT	STDMETHODCALLTYPE Seek(
		/* [in]	*/	LARGE_INTEGER dlibMove,
		/* [in]	*/	DWORD dwOrigin,
		/* [out] */	ULARGE_INTEGER *plibNewPosition);
	
	HRESULT	STDMETHODCALLTYPE SetSize( /* [in] */ ULARGE_INTEGER nNewLen );
	
	HRESULT	STDMETHODCALLTYPE CopyTo(
		/* [unique][in]	*/ IStream *,
		/* [in]	*/ ULARGE_INTEGER ,
		/* [out] */	ULARGE_INTEGER *,
		/* [out] */	ULARGE_INTEGER *)
	{
		return E_NOTIMPL;
	}
	
	HRESULT	STDMETHODCALLTYPE Commit( /* [in] */ DWORD )
	{
		return E_NOTIMPL;
	}
	
	HRESULT	STDMETHODCALLTYPE Revert( void )
	{
		return E_NOTIMPL;
	}
	
	HRESULT	STDMETHODCALLTYPE LockRegion(
		/* [in]	*/ ULARGE_INTEGER ,
		/* [in]	*/ ULARGE_INTEGER ,
		/* [in]	*/ DWORD )
	{
		return E_NOTIMPL;
	}
	
	HRESULT	STDMETHODCALLTYPE UnlockRegion(	
		/* [in]	*/ ULARGE_INTEGER ,
		/* [in]	*/ ULARGE_INTEGER ,
		/* [in]	*/ DWORD )
	{
		return E_NOTIMPL;
	}
	
	HRESULT	STDMETHODCALLTYPE Stat(	
		/* [out] */	STATSTG	*,
		/* [in]	*/ DWORD )
	{
		return E_NOTIMPL;
	}
	
	virtual	HRESULT	STDMETHODCALLTYPE Clone( /*	[out] */ IStream **)
	{
		return E_NOTIMPL;
	}
	
	// ISequentialStream
public:
	HRESULT	STDMETHODCALLTYPE Read(	
		/* [length_is][size_is][out] */	void *pv,
		/* [in]	*/ ULONG cb,
		/* [out] */	ULONG *pcbRead);
	
	HRESULT	STDMETHODCALLTYPE Write( 
		/* [size_is][in] */	const void *pv,
		/* [in]	*/ ULONG cb,
		/* [out] */	ULONG *pcbWritten);

	// IUnkonwn
public:
	HRESULT	STDMETHODCALLTYPE QueryInterface( REFIID riid, void** ppv )
	{
		if ( ppv ==	NULL ) return E_POINTER;

		if ( InlineIsEqualGUID(	riid, __uuidof(IStream)) )
		{
			*ppv = static_cast<IStream*>(this);
			AddRef();
			return S_OK;
		}
		if ( InlineIsEqualGUID(	riid, __uuidof(ISequentialStream)) )
		{
			*ppv = static_cast<ISequentialStream*>(this);
			AddRef();
			return S_OK;
		}
		if ( InlineIsEqualGUID(	riid, __uuidof(IUnknown) ) )
		{
			*ppv = this;
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE	AddRef()
	{
		return 1;
	}

	ULONG STDMETHODCALLTYPE	Release()
	{
		return 1;
	}

private:
	// Buffer used for storing actual data.
	CByteStream		m_ByteStream;
};
