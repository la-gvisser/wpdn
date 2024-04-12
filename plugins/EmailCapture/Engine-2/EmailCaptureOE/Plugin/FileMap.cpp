// ============================================================================
//	FileMap.cpp
//		implementation of the CFileMap class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "FileEx.h"
#include "FileMap.h"
#include "PenFileEx.h"
#include "GlobalTags.h"
#include <map>


// ============================================================================
//		Globals
// ============================================================================

const CPeekString	g_kFileType[kFileTypeMax] = {
	L".eml",
	L".pkt",
#ifdef _DEBUG
	L".pen",
#else
	L".txt",
#endif
	L".xml",
	L".pcap",
	L".pcapng",
	L".err"
};


// ============================================================================
//		CTypeExts
// ============================================================================

CTypeExts	CFileItem::s_ayTypeExt;

CTypeExts::CTypeExts()
{
	push_back( L".eml" );
	push_back( L".pkt" );
#ifdef _DEBUG
	push_back( L".pen" );
#else
	push_back( L".txt" );
#endif
	push_back( L".xml" );
	push_back( L".bdat" );
	push_back( L".pcap" );
	push_back( L".pcapng" );
	push_back( L".err" );
}


#if (0)
// Not currently used.

// ============================================================================
//		CFileMap
// ============================================================================

CFileMap::~CFileMap()
{
	std::map<CPeekString, CFileItem*>::iterator	item = begin();
	while ( item != end() ) {
		CFileItem*	pFile = item->second;
		if ( pFile ) {
			delete pFile;	// Will close an open file.
			item->second = NULL;
		}
		item++;
	}
	clear();
}


// ----------------------------------------------------------------------------
//		BuildCasePath		[static]
// ----------------------------------------------------------------------------

CPeekString
CFileMap::BuildCasePath(
	const CPeekString&	inCase,
	const CPeekString&	inOutputDirectory,
	tFileNameType		inType )
{
	CPeekString		strCase( CFileEx::LegalizeFileName( inCase ) );
#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&inOutputDirectory,
		Tags::kxBackSlash.RefW(),
		&strCase,
		Tags::kxBackSlash.RefW(),
		s_ayTypeExt.RefW( inType ),
		NULL
	};
	CPeekString	strPath( FastCat( ayStrs ) );
#else
	CPeekString	strPath;
	strPath.Format( L"%s\\%s\\%s", inOutputDirectory, strCase, inType );
#endif // TODO

	CPeekString	strLegalPath( CFileEx::LegalizeFilePath( strPath ) );

	CFileEx::MakePath( strLegalPath );

	return strLegalPath;
}


// ----------------------------------------------------------------------------
//		BuildUniqueName											[static]
// ----------------------------------------------------------------------------

#if (TOVERIFY)
CPeekString
CFileMap::BuildUniqueName(
	const CPeekString& inCase,
	const CPeekString& inOutputDirectory,
	const CPeekString& inName )
{
	CPeekString	strLegalFileName;
	CPeekString	strCase = CFileEx::LegalizeFileName( inCase );
	CPeekString	strName = CFileEx::LegalizeFileName( inName );

	const CPeekString*	ayPathStrs[] = {
		&inOutputDirectory,
		Tags::kxBackSlash.RefW(),
		&strCase,
		Tags::kxBackSlash.RefW(),
		NULL
	};
	CPeekString	strPath( FastCat( ayPathStrs ) );
	CPeekString	strLegalPath( CFileEx::LegalizeFilePath( strPath ) );

	CFileEx::MakePath( strLegalPath );

	int	x( 0 );
	do {
		if ( x++ > 1000 ) {
			strLegalFileName.Empty();
			break;
		}

		CPeekString		strTimeStamp = CPeekTime::GetTimeStringFileName( true );
		const CPeekString*	ayStrs[] = {
			&strLegalPath,
			&strTimeStamp,
			Tags::kxSpace.RefW(),
			&strCase,
			Tags::kxSpace.RefW(),
			&strName,
			NULL
		};
		CPeekString	strFileName( FastCat( ayStrs ) );

		strLegalFileName = CFileEx::LegalizeFilePath( strFileName );
		if ( strLegalFileName.GetLength() >= MAX_PATH ) {
			strLegalFileName.Insert( 0, CFileEx::s_strMaxPreamble );
		}
	} while ( !CFileEx::IsFilePath( strLegalFileName ) );

	return strLegalFileName;
}
#endif // TOVERIFY

#if (TOVERIFY)
CPeekString
CFileMap::BuildUniqueName(
	const CPeekString&	inCase,
	const CPeekString&	inOutputDirectory,
	const CPeekString&	inName,
	tFileNameType		inType )
{
	CPeekString	strLegalFileName;
	CPeekString	strLabel = CFileEx::LegalizeFileName( inCase );
	CPeekString	strName = CFileEx::LegalizeFileName( inName );
	CPeekString	strPath = BuildCasePath( strLabel, inOutputDirectory, inType );

	int	x( 0 );
	do {
		if ( x++ > 1000 ) {
			strLegalFileName.Empty();
			break;
		}

		CPeekString		strTimeStamp = CPeekTime::GetTimeStringFileName( true );
		const CPeekString*	ayStrs[] = {
			&strPath,					// has trailing back-slash
			&strTimeStamp,
			Tags::kxSpace.RefW(),
			&strLabel,
			Tags::kxSpace.RefW(),
			&strName,
			s_ayTypeExt.RefW( inType ),
			NULL
		};
		CPeekString	strFileName( FastCat( ayStrs ) );

		strLegalFileName = CFileEx::LegalizeFilePath( strFileName );
		if ( strLegalFileName.GetLength() >= MAX_PATH ) {
			strLegalFileName.Insert( 0, CFileEx::s_strMaxPreamble );
		}
	} while ( !CFileEx::IsFilePath( strLegalFileName ) );

	return strLegalFileName;
}
#endif // TOVERIFY


// ----------------------------------------------------------------------------
//		Clean
// ----------------------------------------------------------------------------

void
CFileMap::Clean()
{
#if (TOVERIFY)
	bool	bDirty( true );
	do {
		bDirty = false;
		std::map<CPeekString, CFileItem*>::iterator	item = begin();
		std::map<CPeekString, CFileItem*>::iterator	last = end();
		while ( item != last ) {
			if ( item->second == NULL ) {
				erase( item );
				bDirty = true;
				break;
			}
			item++;
		}
	} while ( bDirty );
#endif
}


// ----------------------------------------------------------------------------
//		FindFile
// ----------------------------------------------------------------------------

CFileItem*
CFileMap::FindFile(
	const CPeekString&	inPath,
	int					inType )
{
	CFileItem*		pFile( NULL );
#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&inPath,
		Tags::kxBackSlash.RefW(),
		&g_kFileType[inType],
		NULL
	};
	CPeekString	strPathType( FastCat( ayStrs ) );
#else
	CPeekString		strPathType.Format( L"%s\\%s", inPath, g_kFileType[inType] );
#endif

#if (TOVERIFY)
	std::map<CPeekString, CFileItem*>::iterator	item = find( strPathType );
	if ( item != end() ) {
		pFile = item->second;
	}
#else
	CPair*		pItem( Lookup( strPathType ) );
	if ( pItem != NULL ) {
		pFile = pItem->m_value;
	}
#endif // TOVERIFY

	return pFile;
}


// ----------------------------------------------------------------------------
//		GetPenFileType
// ----------------------------------------------------------------------------

tFileNameType
CFileMap::GetPenFileType(
	const CPeekString&	inFileName )
{
#if (TOVERIFY)
	CPeekString	strExt( ::PathFindExtension( inFileName ) );
	if ( !strExt.IsEmpty() ) {
		for ( int i = 0; i < kFileTypeMax; i++ ) {
			if ( strExt.CompareNoCase( s_ayTypeExt.GetW( i ) ) == 0 ) {
				return static_cast<tFileNameType>( i );
			}
		}
	}
#endif // TOVERIFY

	return kFileTypeMax;
}


// ----------------------------------------------------------------------------
//		PutFile
// ----------------------------------------------------------------------------

CFileItem*
CFileMap::PutFile(
	const CPeekString&	inPath,
	int					inType,
	CFileItem*			inFile )
{
	CFileItem*	pFile( NULL );

#if (TOVERIFY)
	const CPeekString*	ayStrs[] = {
		&inPath,
		Tags::kxBackSlash.RefW(),
		&g_kFileType[inType],
		NULL
	};
	CPeekString	strPathType( FastCat( ayStrs ) );
#else
	strPathType.Format( L"%s\\%s", inPath, g_kFileType[inType] );
#endif // TOVERIFY

#if (TOVERIFY)
	std::map<CPeekString, CFileItem*>::iterator	item = find( strPathType );
	if ( item == end() ) {
		insert( std::pair<CPeekString, CFileItem*>( strPathType, inFile ) );
	}
	else {
		if ( item->second != NULL ) {
			CFileItem*	pOldFile = item->second;
			delete pOldFile;
			item->second = NULL;
		}
		item->second = inFile;
	}
#else
	CPair*		pItem( Lookup( strPathType ) );
	if ( pItem != NULL ) {
		pFile = pItem->m_value;
		pItem->m_value = inFile;
	}
	else {
		SetAt( strPathType, inFile );
	}
#endif // TOVERIFY

	return pFile;
}


// ----------------------------------------------------------------------------
//		Monitor
// ----------------------------------------------------------------------------

void
CFileMap::Monitor()
{
#if (TOVERIFY)
	bool	bDirty( false );
	std::map<CPeekString, CFileItem*>::iterator	item = begin();
	std::map<CPeekString, CFileItem*>::iterator	last = end();
	while ( item != last ) {
		CFileItem*	pFile = item->second;
		if ( pFile ) {
			if ( pFile->DoMonitor() ) {
				delete pFile;
				item->second = NULL;
			}
		}
		item++;
	}

	if ( bDirty ) {
		Clean();
	}
#else
	POSITION	pos( GetStartPosition() );
	while ( pos != NULL ) {
		CPair*	pCur( GetNext( pos ) );
		if ( pCur != NULL ) {
			CPenFileEx*	pFile( dynamic_cast<CPenFileEx*>( pCur->m_value ) );
			if ( pFile != NULL ) {
				if ( pFile->DoMonitor() ) {
					delete pFile;
					pCur->m_value = NULL;
				}
			}
		}
	}
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		Reset
// ----------------------------------------------------------------------------

void
CFileMap::Reset()
{
#if (TOVERIFY)
	std::map<CPeekString, CFileItem*>::iterator	item = begin();
	std::map<CPeekString, CFileItem*>::iterator	last = end();
	while ( item != last ) {
		delete item->second;
		item->second = NULL;
		++item;
	}
	clear();
#else
	POSITION	pos( GetStartPosition() );
	while ( pos != NULL ) {
		CPair*	pCur( GetNext( pos ) );
		if ( pCur != NULL ) {
			CFileItem*	pFile( pCur->m_value );
			if ( pFile != NULL ) {
				delete pFile;
				pCur->m_value = NULL;
			}
		}
	}
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		UpdateSettings
// ----------------------------------------------------------------------------

void
CFileMap::UpdateSettings(
	COptions*	inOptions )
{
#if (TOVERIFY)
	bool	bDirty( false );
	std::map<CPeekString, CFileItem*>::iterator	item = begin();
	std::map<CPeekString, CFileItem*>::iterator	last = end();
	while ( item != last ) {
		CFileItem*	pFile = item->second;
		if ( pFile ) {
			CPenFileEx*	pPenFile( dynamic_cast<CPenFileEx*>( pFile ) );
			if ( pPenFile ) {
				if ( !pPenFile->Update( inOptions ) ) {
					delete pPenFile;
					item->second = NULL;
					bDirty = true;
				}
			}
		}
		++item;
	}

	if ( bDirty ) {
		Clean();
	}
#else
	POSITION	pos( GetStartPosition() );
	while ( pos != NULL ) {
		CPair*	pCur( GetNext( pos ) );
		if ( pCur != NULL ) {
			CFileItem*	pFile( pCur->m_value );
			if ( pFile != NULL ) {
				CPenFileEx*	pPenFile( dynamic_cast<CPenFileEx*>( pFile ) );
				if ( pPenFile != NULL ) {
					if ( !pPenFile->Update( inOptions ) ) {
						delete pPenFile;
						pCur->m_value = NULL;
					}
				}
			}
		}
	}
#endif // TOVERIFY
}

#endif
