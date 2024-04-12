// =============================================================================
//	XFile.h
// =============================================================================
//	Copyright (c) 1998-2007 WildPackets, Inc. All rights reserved.

#ifndef XFILE_H
#define XFILE_H

#include "AGTypes.h"

#ifdef _LINUX
#include <stdio.h>
#include "XString.h"
#endif

typedef enum
{
#if TARGET_OS_WIN32
	kFromStart		= FILE_BEGIN,
	kFromCurrent	= FILE_CURRENT,
	kFromEnd		= FILE_END
#elif TARGET_OS_MAC
	kFromStart		= fsFromStart,
	kFromCurrent	= fsFromMark,
	kFromEnd		= fsFromLEOF
#elif _LINUX
	kFromStart		= 0,
	kFromCurrent	= 1,
	kFromEnd		= 2
#endif
} ESeekType;

typedef enum
{
#if TARGET_OS_WIN32
	kReadPerm		= GENERIC_READ,
	kWritePerm		= GENERIC_WRITE,
	kReadWritePerm	= GENERIC_READ | GENERIC_WRITE
#elif TARGET_OS_MAC
	kReadPerm		= fsRdPerm,
	kWritePerm		= fsWrPerm,
	kReadWritePerm	= fsRdWrPerm
#elif _LINUX
	kReadPerm		= (0x80000000L),
	kWritePerm		= (0x40000000L),
	kReadWritePerm	= kReadPerm | kWritePerm
#endif
} EFilePerms;

#if TARGET_OS_WIN32
	typedef HANDLE	TFileRef;
	typedef CString	TFileSpec;
#elif TARGET_OS_MAC
	typedef SInt16	TFileRef;
	typedef FSSpec	TFileSpec;
#elif _LINUX
	typedef FILE*	TFileRef;
	typedef XString	TFileSpec;
#endif

#if TARGET_OS_WIN32 || _LINUX
#define AGFOUR_CHAR_CODE(ch0, ch1, ch2, ch3)			\
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |	\
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#else
#define AGFOUR_CHAR_CODE(ch0, ch1, ch2, ch3)			\
	((UInt32)((UInt8)(ch3)) | ((UInt32)((UInt8)(ch2)) << 8) |	\
	((UInt32)((UInt8)(ch1)) << 16) | ((UInt32)((UInt8)(ch0)) << 24 ))
#endif

typedef enum
{
	kFile_OpenExisting,
	kFile_CreateNew
} EOpenMethod;

#ifdef _LINUX
const TFileRef	kFileRef_Invalid	= (TFileRef) NULL;
#else
const TFileRef	kFileRef_Invalid	= (TFileRef) -1;
#endif

class DECLSPEC XFile
{
public:
					XFile();
					XFile( const TFileSpec& inFileSpec );
	virtual			~XFile();

	virtual void				SetFileSpec( const TFileSpec& inFileSpec );
	virtual const TFileSpec&	GetFileSpec() const { return m_FileSpec; }

	virtual TFileRef			GetFileRef() const { return m_FileRef; }

	virtual bool	Open( EFilePerms inFilePerms, EOpenMethod inOpenMethod,
						UInt32 inType = AGFOUR_CHAR_CODE('?', '?', '?', '?'),
						UInt32 inCreator = AGFOUR_CHAR_CODE('?', '?', '?', '?') );
	virtual bool	Close();

	virtual bool	IsOpen() const { return (m_FileRef != kFileRef_Invalid); }

	virtual UInt32	Read( void* outBuffer, UInt32 inSize ) const;
	virtual UInt32	Write( const void* inBuffer, UInt32 inSize );

	virtual bool	SetPosition( SInt64 inOffset, ESeekType inSeekType ) const;
	virtual UInt64	GetPosition() const;

	virtual UInt64	GetSize() const;
	virtual bool	SetSize( UInt64 inSize );

	virtual bool	Delete();

protected:
	TFileSpec		m_FileSpec;
	TFileRef		m_FileRef;
};

#endif /* XFILE_H */

