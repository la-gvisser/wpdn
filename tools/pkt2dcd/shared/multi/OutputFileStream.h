// =============================================================================
//	OutputFileStream.h
// =============================================================================
//	Copyright (c) 1999-2012 WildPackets, Inc. All rights reserved.

#ifndef OUTPUTFILESTREAM_H
#define OUTPUTFILESTREAM_H

#include "AGTypes.h"
#include "ByteStream.h"
#include "XFile.h"

class DECLSPEC COutputFileStream : public CByteStream
{
public:
						COutputFileStream( UInt32 inChunkSize = 64 * 1024UL );
						COutputFileStream( const TFileSpec& inSpec,
							UInt32 inChunkSize = 64 * 1024UL );
	virtual				~COutputFileStream();

	virtual void		SetFileSpec( const TFileSpec& inSpec );
	virtual TFileSpec	GetFileSpec( ) const { return m_File.GetFileSpec(); }

	virtual TFileRef	GetFileRef() const { return m_File.GetFileRef(); }

	virtual bool		WriteBytes( const void* inData, UInt32 inSize );

	virtual bool		Write( const wchar_t* inString );
	virtual bool		Write( const char* inString );

	virtual bool		Flush( bool inForce = false );
	virtual void		Delete();

	bool				IsOpen() const { return m_File.IsOpen(); }

	XFile&				GetFile() { return m_File; }

protected:
	XFile				m_File;
};

#endif /* OUTPUTFILESTREAM_H */
