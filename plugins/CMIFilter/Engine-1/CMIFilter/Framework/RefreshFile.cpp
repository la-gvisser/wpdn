// =============================================================================
//	RefreshFile.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "RefreshFile.h"
#include "FileEx.h"
#include <sys/stat.h>


#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY	0x0010
#endif

#ifndef MAX_PATH
#define MAX_PATH		260
#endif


// =============================================================================
//	CRefreshFile
// =============================================================================

// -----------------------------------------------------------------------------
//		CRefreshFile Construction
// -----------------------------------------------------------------------------

CRefreshFile::CRefreshFile()
{
	Reset();
}

CRefreshFile::CRefreshFile(
	const CPeekString&	inFilePath,
	const CPeekString&	inFileBaseName,
	UInt32				inRefreshFileType,
	bool				inAppendTimestring )
{
	Reset();

	SetRefreshFilePath( inFilePath, inAppendTimestring );
	SetRefreshFileBaseName( inFileBaseName );
	SetRefreshFileType( inRefreshFileType );
}


// -----------------------------------------------------------------------------
//		Activate
// -----------------------------------------------------------------------------

bool
CRefreshFile::Activate()
{
	if ( m_nRefreshFileType != kRefreshFileTypeNone ) {
		if ( (m_nRefreshSize < kMinRefreshSize) || (m_nRefreshSize > kMaxRefreshSize) ) {
			Deactivate();
			return false;
		}
	}

	Monitor();

	m_nTimeActivatedMS = Now();
	m_nTimeIntervalBeginMS = Now();
	m_nUniqueNameId = 0;
	m_bActivated = true;

	Monitor();

	return true;
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

bool
CRefreshFile::Copy(
	const CRefreshFile& inRefreshFile )
{
	m_strFilePath = inRefreshFile.m_strFilePath;
	m_strFileBaseName = inRefreshFile.m_strFileBaseName;
	m_nRefreshFileType = inRefreshFile.m_nRefreshFileType;
	m_nTimeIntervalBeginMS = inRefreshFile.m_nTimeIntervalBeginMS;
	m_nRefreshTimeIntervalMS = inRefreshFile.m_nRefreshTimeIntervalMS;
	m_nUniqueNameId = inRefreshFile.m_nUniqueNameId;
	m_bActivated = inRefreshFile.m_bActivated;
	m_nRefreshSize = inRefreshFile.m_nRefreshSize;
	m_nRefreshTriggerSize = inRefreshFile.m_nRefreshTriggerSize;
	m_nTimeActivatedMS = inRefreshFile.m_nTimeActivatedMS;

	return true;
}


// -----------------------------------------------------------------------------
//		CreateNextSaveName
// -----------------------------------------------------------------------------

CPeekString
CRefreshFile::CreateNextSaveName()
{
	_ASSERTE( m_nRefreshFileType != kRefreshFileTypeNone );
	_ASSERTE( !m_strFilePath.IsEmpty() );

	CPeekString	strNext;
	if ( m_strFilePath.IsEmpty() ) {
		return strNext;
	}

	CPeekOutString	strNextFileName;
	CPeekString strTimeString;
	switch ( m_nRefreshFileType ) {
		case kRefreshFileTypeNone:
			break;

		case kRefreshFileTypeFileNumber:
			strNextFileName << m_strFilePath << m_strFileBaseName << L"_"
							<< m_nUniqueNameId++ << kFileType_OmniPeekPacketFile;
			break;

		case kRefreshFileTypeOverwriteExisting:
			strNextFileName << m_strFilePath << m_strFileBaseName
							<< kFileType_OmniPeekPacketFile;
			break;

		case kRefreshFileTypeTimeStampWithNumber:
			strTimeString = CPeekTime::GetTimeStringFileName();
			strNextFileName << m_strFilePath << m_strFileBaseName << strTimeString << L"_"
							<< m_nUniqueNameId++ << kFileType_OmniPeekPacketFile;
			break;

		case kRefreshFileTypeTimeStamp:
			strTimeString = CPeekTime::GetTimeStringFileName();
			strNextFileName << m_strFilePath << m_strFileBaseName << strTimeString
							<< kFileType_OmniPeekPacketFile;
			break;

		default:
			break;
	}
	strNext = strNextFileName;
	return strNext;
}


// -----------------------------------------------------------------------------
//		Deactivate
// -----------------------------------------------------------------------------

void
CRefreshFile::Deactivate()
{
	m_bActivated = false;
	Refresh();
	m_nTimeActivatedMS = 0;
	m_nTimeIntervalBeginMS = 0;
	m_nUniqueNameId = 0;
}


// ----------------------------------------------------------------------------
//		DirectoryExists
// ----------------------------------------------------------------------------

bool
CRefreshFile::DirectoryExists(
	const CPeekString&	inDirectory )
{
	bool bReturn = false;

	try {

		// Remove trailing backslash:
		wchar_t	cLast( *inDirectory.Right( 1 ) );
		if ( (cLast == L'\\') || (cLast == L'/') ) {
			const CPeekString strDirectory( inDirectory.Left( inDirectory.GetLength() - 1 ) );
			bReturn = CFileEx::IsFilePath( strDirectory );
		}
		else {
		bReturn = CFileEx::IsFilePath( inDirectory );
	}
	}
	catch (...) {
		bReturn = false;
	}
	return bReturn;
}


// ----------------------------------------------------------------------------
//		IsPathValid
// ----------------------------------------------------------------------------

bool
CRefreshFile::IsPathValid(
	const CPeekString&	inPath )
{
	bool	bReturn = false;

	try {
		_ASSERTE( !inPath.IsEmpty() );
		size_t	nLength = inPath.GetLength();

		if ( (nLength < 2) || (nLength > MAX_PATH) ) return false;
		if ( inPath.Mid( 1 ) != L':' ) return false;

		if ( inPath.FindOneOf( L"<>:\"|?*", 2 ) >= 0 ) return false;

		//	if ( wcsstr( pszPath, L".." ) ) {
		//		inPath.ReleaseBuffer();
		//		return false;
		//	}
		if ( inPath.Find( L"\\\\" ) >= 0 ) return false;
		if ( inPath.Find( L"//" ) >= 0 ) return false;

#ifdef _WIN32
		// Make sure we have a valid logical drive
		CPeekString	strDrive = inPath.Left( 3 );
		UInt32		dwAttr = ::GetFileAttributes( strDrive );
		if( (dwAttr & FILE_ATTRIBUTE_DIRECTORY) && (dwAttr == 0xFFFFFFFF) ) {
			return false;
		}
#endif

		bReturn = true;
	}
	catch (...) {
		bReturn = false;
	}

	return bReturn;
}


// ----------------------------------------------------------------------------
//		MakePath
// ----------------------------------------------------------------------------

bool
CRefreshFile::MakePath(
	const CPeekString&	inPath )
{
	bool	bReturn = false;

	CPeekString	tmpPath( inPath );
	if ( DirectoryExists( tmpPath ) ) return true;
	if ( !IsPathValid( tmpPath ) )  return false;

	try {

		// Make sure the directory name ends in a slash. PathAddBackslash()
		if ( tmpPath.Right( 1 ) != L"\\" ) {
			tmpPath += L'\\';
		}

		// Create each directory in the path.
		CPeekString	dirName;
		int			nIndex = 0;
		bool		bDone = false;
		while ( !bDone ) {
			// Extract one directory.
			nIndex = tmpPath.Find( L'\\' );
			if ( nIndex != -1 ) {
				dirName = dirName + tmpPath.Left( nIndex );
				tmpPath = tmpPath.Right( tmpPath.GetLength() - nIndex - 1 );

				// The first time through, we might have a drive name.
				if ( (dirName.GetLength() >= 1) && (dirName.Right( 1 ) != L":") ) {
#ifdef _WIN32
					bool	bRetVal = (::CreateDirectory( dirName, nullptr ) != FALSE);
					if ( !bRetVal ) {
						UInt32 dwError = GetLastError();
						if ( dwError != ERROR_ALREADY_EXISTS ) {
							return false;
						}
					}
#else
					CPeekStringA	strDirNameA( dirName );
					int	nMode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
					if ( mkdir( strDirNameA.c_str(), nMode ) != 0 ) {
						return false;
					}
#endif
				}
				dirName += L'\\';
			}
			else {
				bDone = true;
			}
			bReturn = true;
		}
	}
	catch(...) {
		bReturn = false;
	}

	return  bReturn;
}


// ----------------------------------------------------------------------------
//		Reset
// ----------------------------------------------------------------------------

void
CRefreshFile::Reset()
{
	m_strFilePath.Empty();
	m_strFileBaseName.Empty();
	m_nRefreshFileType = kRefreshFileTypeNone;
	m_nTimeIntervalBeginMS = 0;
	m_nRefreshTimeIntervalMS = kRefreshTimeNotUsed;
	m_nUniqueNameId = 0;
	m_bActivated = false;
	m_nRefreshSize = 0;
	m_nRefreshTriggerSize = 0;
	m_nTimeActivatedMS = 0;
}


// ----------------------------------------------------------------------------
//		SetRefreshFilePath
// ----------------------------------------------------------------------------

bool
CRefreshFile::SetRefreshFilePath(
	const CPeekString&	inFilePath,
	bool				inAppendTimestring )
{
	_ASSERTE( !inFilePath.IsEmpty() );
	if ( inFilePath.IsEmpty() ) return false;

	CPeekString	strFilePath = inFilePath;
	wchar_t		cLast( *strFilePath.Right( 1 ) );
	if ( (cLast != L'\\') || (cLast != L'/') ) {
		strFilePath += L'\\';
	}
	if ( m_nRefreshFileType == kRefreshFileTypeNone ) {
		m_nRefreshFileType = kRefreshFileTypeDefault;
	}

	if ( inAppendTimestring ) {
		CPeekTime	ptNow( true );
		strFilePath += ptNow.Format() + L"\\";
	}

	bool bHavePath = MakePath( strFilePath );
	if ( bHavePath ) m_strFilePath = strFilePath;

	return bHavePath;
}
