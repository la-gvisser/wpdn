// ============================================================================
//	FileMap.h
//		interface for the CFileMap class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekStrings.h"
#include "FileEx.h"
#include "LockedPtr.h"
#include <vector>
#include <map>


// ============================================================================
//		Globals
// ============================================================================

typedef enum {
	kFileTypeEml,
	kFileTypePkt,
	kFileTypeTxt,
	kFileTypeXml,
	kFileTypeBdat,
	kFileTypePcap,
	kFileTypePcapNG,
	kFileTypeMax
} tFileNameType ;

#define DEFAULT_TIMEOUT		500

class COptions;

// ============================================================================
//		CTypeExts
// ============================================================================

class CTypeExts
	:	public std::vector<CPeekString>
{
public:
	;	CTypeExts();

	const CPeekString&	GetW( size_t inIndex ) const {
		return (*this)[inIndex];
	}

	const CPeekString*	RefW( size_t inIndex ) const {
		return &((*this)[inIndex]);
	}
};

// ============================================================================
//		CFileItem
// ============================================================================

class CFileItem
	:	public CFileEx
{
public:
	static CTypeExts	s_ayTypeExt;

public:
	;		CFileItem() {}
	virtual	~CFileItem() {}

	virtual bool	DoMonitor() = 0;
};


#if (0)
// Not currently used.

// ============================================================================
//		CFileMap
// ============================================================================

class CFileMap
	:	public std::map<CPeekString, CFileItem*>
	,	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
{
public:
	typedef std::map<CPeekString, CFileItem*>::iterator	CMapIter;

public:
	static CPeekString	BuildCasePath( const CPeekString& inCase,
		const CPeekString& inOutputDirectory, tFileNameType inType );
	static CPeekString	BuildUniqueName( const CPeekString& inCase,
		const CPeekString& inOutputDirectory, const CPeekString& inType );
	static CPeekString	BuildUniqueName( const CPeekString& inCase,
		const CPeekString& inOutputDirectory, const CPeekString& inName,
		tFileNameType inType );

	static tFileNameType	GetPenFileType( const CPeekString& inFileName );

public:
	;			CFileMap() {}
	virtual		~CFileMap();

	void		Clean();
	CFileItem*	FindFile( const CPeekString& inPath, int inType );
	CFileItem*	PutFile( const CPeekString& inPath, int inType, CFileItem* inFile );
	void		Monitor();
	void		Reset();
	void		UpdateSettings( COptions* inOptions );
};


// ============================================================================
//		CFileMapPtr
// ============================================================================

typedef class TLockedPtr< CFileMap >	CFileMapPtr;


// ============================================================================
//		CSafeFileMap
// ============================================================================

class CSafeFileMap
{
protected:
	CFileMap	m_FileMap;

public:
	;		CSafeFileMap() {}

	CFileMapPtr	Get() { return CFileMapPtr( &m_FileMap ); }
};

#endif
