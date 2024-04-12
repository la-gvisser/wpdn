// =============================================================================
//	ByteStream.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include "AGTypes.h"

class DECLSPEC CByteStream
{
public:
	enum TMarkerPos
	{
		kMarker_Begin,
		kMarker_Current,
		kMarker_End
	};

					CByteStream();
					CByteStream( void* inData, UInt32 nSize, bool inOwnsData );
					CByteStream( const CByteStream& inOriginal );
	virtual			~CByteStream();

	CByteStream &	operator=( const CByteStream& inOriginal );
#if TARGET_OS_MAC
	CByteStream &	operator=( Handle inHandle );
#endif

	bool			IsEmpty() const { return (m_nDataStored == 0); }
	UInt32			GetCount() const { return m_nDataStored; }

	UInt32			GetMarker() const { return m_nMarker; }
	void			SetMarker( UInt32 inOffset, TMarkerPos inMarkerPos ) const;

	UInt32			GetChunkSize() const { return m_nChunkSize; }
	void			SetChunkSize( UInt32 inChunkSize ) { m_nChunkSize = inChunkSize; }

	void*			GetData() const { return m_pData; }
	void*			AcquireData();
	static void		FreeData( void* pData );

	void*			GetPtrAtMarker() const { return (void*)((UInt8*)m_pData + m_nMarker); }

	void			SetLength( UInt32 inLength );
	UInt32			GetLength() const { return m_nDataStored; }

	virtual bool	WriteBytes( const void* inData, UInt32 inSize );
	virtual bool	ReadBytes( void* outData, UInt32 inSize ) const;

	bool			Write( LPCTSTR inString );
#ifdef _UNICODE
	bool			Write( const char* inString );
#endif
#if TARGET_OS_MAC
	bool			Write( StringPtr inPString );
#endif

	virtual	LPCTSTR	ReadString() const;
	bool			WriteString( LPCTSTR inString );

	virtual bool	AtEnd() const { return m_nMarker >= m_nDataStored; };

	bool			WriteToFile( LPCTSTR inFullPath ) const;
	bool			ReadFromFile( LPCTSTR inFullPath );

protected:
	mutable UInt32	m_nMarker;
	UInt32			m_nChunkSize;
	UInt32			m_nDataStored;
	UInt32			m_nDataAllocated;
	void*			m_pData;
	bool			m_bOwnsData;

	bool			CheckAllocation( UInt32 inBytesNeeded );
};

#endif
