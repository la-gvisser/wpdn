// ============================================================================
//	FileEx.cpp
//		implementation of the CFileEx class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "FileEx.h"
#include "PeekStrings.h"

CPeekString	CFileEx::s_strMaxPreamble( L"\\\\?\\" );


// ============================================================================
//		CFileEx
// ============================================================================

CFileEx::CFileEx()
#ifdef PEEKFILE_WIN
	:	m_hFile( INVALID_HANDLE_VALUE )
#else
	:	m_pFile( NULL )
#endif
{
}

CFileEx::CFileEx(
	const CPeekString&	inFileName,
	UINT				inOpenFlags )
#ifdef PEEKFILE_WIN
	:	m_hFile( INVALID_HANDLE_VALUE )
#else
	:	m_pFile( NULL )
#endif
{
	Open( inFileName, inOpenFlags );
}

CFileEx::~CFileEx()
{
	Close();
}


// ----------------------------------------------------------------------------
//		BuildUniqueName											[static]
// ----------------------------------------------------------------------------

CPeekString
CFileEx::BuildUniqueName(
	const CPeekString& inLabel,
	const CPeekString& inOutputDirectory,
	const CPeekString& inType )
{
	CPeekString	strLegalFileName;
	CPeekString	strLabel = LegalizeFileName( inLabel );

	MakePath( inOutputDirectory );

	int	x( 0 );
	do {
		if ( x++ > 1000 ) {
			strLegalFileName.Empty();
			break;
		}

		CPeekString		strTimeStamp = CPeekTime::GetTimeStringFileName( true );
		CPeekOutString	strFileName;
		strFileName << inOutputDirectory << L"\\" << inLabel << L" " << strTimeStamp << inType;

		strLegalFileName = LegalizeFilePath( strFileName );
		if ( strLegalFileName.GetLength() >= MAX_PATH ) {
			strLegalFileName.Insert( 0, CFileEx::s_strMaxPreamble );
		}
	} while ( IsFilePath( strLegalFileName ) );

	return strLegalFileName;
}


// ----------------------------------------------------------------------------
//		Close
// ----------------------------------------------------------------------------

void
CFileEx::Close()
{
#ifdef PEEKFILE_WIN
	if ( m_hFile != INVALID_HANDLE_VALUE ) {
		if ( !::CloseHandle( m_hFile ) ) throw -1;
		m_hFile = INVALID_HANDLE_VALUE;
	}

	if ( !m_strBaseName.IsEmpty() ) {
		CPeekString	strInProgressName = GetInProgressName();
		::MoveFile( strInProgressName, m_strBaseName );
		m_strBaseName.Empty();
	}
#else
	if ( m_pFile == NULL ) {
		return;
	}

	int	nError = fclose( m_pFile );
	if ( nError == EOF ) throw -1;

	if ( !m_strBaseName.IsEmpty() ) {
		CPeekStringA	strInProgressName( GetInProgressName() );
		CPeekStringA	strBaseName( m_strBaseName );
		rename( strInProgressName, strBaseName );
		m_strBaseName.Empty();
	}
#endif
	m_strFileName.Empty();
}


// ----------------------------------------------------------------------------
//		GetInProgressName
// ----------------------------------------------------------------------------

CPeekString
CFileEx::GetInProgressName() const
{
	FilePath::PathW	pathBase( m_strBaseName );
	CPeekString		strFileName( pathBase.GetFileName() );
	ptrdiff_t		nPathLen = m_strBaseName.GetLength() - strFileName.GetLength();
	if ( (nPathLen > 0) && (nPathLen < MAX_PATH)) {	// MAX_PATH = 260 : 0x104
		CPeekOutString	strInProgressName;
		strInProgressName << m_strBaseName.Left( nPathLen ) << L"(In Progress) " << strFileName;
		return strInProgressName;
	}

	return m_strBaseName;
}


// ----------------------------------------------------------------------------
//		GetLength
// ----------------------------------------------------------------------------

UInt64
CFileEx::GetLength()
{
#ifdef PEEKFILE_WIN
	ULARGE_INTEGER liSize;

	liSize.LowPart = ::GetFileSize( m_hFile, &liSize.HighPart );
	if ( liSize.LowPart == static_cast<DWORD>( -1 ) ) {
		if ( ::GetLastError() != NO_ERROR ) throw -1;
	}

	return liSize.QuadPart;
#else
	struct _stat64	st;

	if ( _wstat64( GetFileName(), &st ) != 0 ) throw -1;

	return static_cast<UInt64>(st.st_size);
#endif
}


// ----------------------------------------------------------------------------
//		IsFilePath													[static]
// ----------------------------------------------------------------------------

bool
CFileEx::IsFilePath(
	const CPeekString&	inFilePath )
{
	CPeekString	strFilePath = inFilePath;
	strFilePath.TrimRight( L"\\" );

	struct _stat	st;
	return (_wstat( strFilePath, &st ) == 0);
}


// ----------------------------------------------------------------------------
//		IsLegalFilename												[static]
// ----------------------------------------------------------------------------

bool
CFileEx::IsLegalFileName(
	const CPeekString&	inFileName )
{
	const wchar_t	chUnprintable[] = L"\r\n\t";
	const wchar_t	chIllegal[] = L"\\/:*?\"<>|";

	return ((inFileName.Find( chUnprintable ) >= 0) ||
		    (inFileName.Find( chIllegal ) >= 0) );
}


// ----------------------------------------------------------------------------
//		IsLegalFilePath												[static]
// ----------------------------------------------------------------------------

bool
CFileEx::IsLegalFilePath(
	const CPeekString&	inFilePath )
{
	const wchar_t	chUnprintable[] = L"\r\n\t";
	const wchar_t	chIllegal[] = L":*?\"<>|";

	if ( inFilePath.Find( chUnprintable ) >= 0 ) {
		return false;
	}

	size_t	nOffset = ((iswalpha( inFilePath[0] ) && (inFilePath[1] == L':'))) ? 2 : 0;

	return (inFilePath.Find( chIllegal, nOffset ) >= 0);
}


// ----------------------------------------------------------------------------
//		LegalizeFileName											[static]
// ----------------------------------------------------------------------------

CPeekString
CFileEx::LegalizeFileName(
	const CPeekString&	inFileName,
	wchar_t				inReplace /*= L'_'*/ )
{
	if ( inFileName.IsEmpty() ) return inFileName;

	const wchar_t	chUnprintable[] = L"\r\n\t";
	const size_t	nUnprintableCount( COUNTOF( chUnprintable ) );

	const wchar_t	chIllegal[] = L"\\/:*?\"<>|";
	const size_t	nIllegalCount( COUNTOF( chIllegal ) );

	CPeekString	strFileName = inFileName;
	strFileName.Trim();

	for ( size_t i = 0; i < nUnprintableCount; i++ ) {
		strFileName.Remove( chUnprintable[i] );
	}

	for ( size_t i = 0; i < nIllegalCount; i++ ) {
		strFileName.Replace( chIllegal[i], inReplace );
	}

	return strFileName;
}


// ----------------------------------------------------------------------------
//		LegalizeFilePath											[static]
// ----------------------------------------------------------------------------

CPeekString
CFileEx::LegalizeFilePath(
	const CPeekString&	inFilePath )
{
	if ( inFilePath.IsEmpty() ) return inFilePath;

	const wchar_t	chUnprintable[] = L"\r\n\t";
	const size_t	nUnprintableCount( COUNTOF( chUnprintable ) );
	const wchar_t	chIllegal[] = L":*?\"<>|";
	const size_t	nIllegalCount( COUNTOF( chIllegal ) );

	CPeekString strRootPath;
	CPeekString	strFilePath;

	if ( isalpha( inFilePath[0] ) && (inFilePath[1] == L':') ) {
		strRootPath = inFilePath.Left( 2 );
		strFilePath = inFilePath.Right( inFilePath.GetLength() - 2 );
	}
	else {
		strFilePath = inFilePath;
	}

	for ( size_t i = 0; i < nUnprintableCount; i++ ) {
		strFilePath.Remove( chUnprintable[i] );
	}

	strFilePath.Replace( L'/', L'\\' );

	for ( size_t i = 0; i < nIllegalCount; i++ ) {
		strFilePath.Replace( chIllegal[i], L'_' );
	}

	size_t		nOffset = 0;
	CPeekString	strLegalFilePath;
	CPeekString	strFolder( strFilePath.Tokenize( L"\\", nOffset ) );
	while ( nOffset != std::wstring::npos ) {
		CPeekString	strLegalFolder( LegalizeFileName( strFolder ) );
		strLegalFilePath += CPeekString( L"\\" ) + strLegalFolder;
		strFolder = strFilePath.Tokenize( L"\\", nOffset );
	}
	strRootPath.Append( strLegalFilePath );

	return strRootPath;
}


// ----------------------------------------------------------------------------
//		MakePath													[static]
// ----------------------------------------------------------------------------

bool
CFileEx::MakePath(
	const CPeekString&	inPath )
{
	CPeekString	tmpPath( inPath );
	ASSERT( !tmpPath.IsEmpty() );
	if ( tmpPath.IsEmpty() ) return false;

	bool bRetVal( false );

	// Make sure the directory name ends in a slash. PathAddBackslash()
	if ( tmpPath.Right( 1 ) != L"\\" ) {
		tmpPath += L"\\";
	}

	// Create each directory in the path.
	CPeekString	dirName;
	int		nIndex( 0 );
	bool	bDone( false );
	while ( !bDone ) {
		// Extract one directory.
		nIndex = tmpPath.Find( L"\\" );
		if ( nIndex != -1 ) {
			dirName = dirName + tmpPath.Left( nIndex );
			tmpPath = tmpPath.Right( tmpPath.GetLength() - nIndex - 1 );

			// The first time through, we might have a drive name.
			if ( (dirName.GetLength() >= 1) && (dirName.Right( 1 ) != L":") ) {
				bRetVal = (::CreateDirectory( dirName, NULL ) != FALSE);
			}
			dirName += L'\\';
		}
		else {
			bDone = true;
		}
	}
	// Return the last MakeDirectory() return value.
	return bRetVal;
}


// ----------------------------------------------------------------------------
//		Open
// ----------------------------------------------------------------------------

bool
CFileEx::Open(
	const CPeekString&	inFileName,
	int					inFlags )
{
	ASSERT( !IsOpen() );	// shouldn't open an already open file.

	if ( IsOpen() ) {
		Close();
	}

#ifdef PEEKFILE_WIN
	// Always binary and CreateFile does not need flag
	UINT	nFlags = static_cast<UINT>( inFlags & ~typeBinary );

	// map read/write mode
	ASSERT( (modeRead | modeWrite | modeReadWrite) == 3 );
	DWORD	dwAccess( 0 );
	switch ( nFlags & 3 ) {
	case modeRead:
		dwAccess = GENERIC_READ;
		break;
	case modeWrite:
		dwAccess = GENERIC_WRITE;
		break;
	case modeReadWrite:
		dwAccess = (GENERIC_READ | GENERIC_WRITE);
		break;
	default:
		ASSERT( false );	// invalid share mode
	}

	// map share mode
	DWORD	dwShareMode( 0 );
	switch ( nFlags & 0x70 ) {	// map compatibility mode to exclusive
	case shareCompat:
	case shareExclusive:
		dwShareMode = 0;
		break;
	case shareDenyWrite:
		dwShareMode = FILE_SHARE_READ;
		break;
	case shareDenyRead:
		dwShareMode = FILE_SHARE_WRITE;
		break;
	case shareDenyNone:
		dwShareMode = (FILE_SHARE_WRITE | FILE_SHARE_READ);
		break;
	default:
		ASSERT( false );	// invalid share mode?
	}

	// map modeNoInherit flag
	SECURITY_ATTRIBUTES	sa;
	sa.nLength = sizeof( sa );
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = ((nFlags & modeNoInherit) == 0);

	// map creation flags
	DWORD dwCreateFlag;
	if ( nFlags & modeNew ) {
		dwCreateFlag = CREATE_NEW;
	}
	else if ( nFlags & modeCreate ) {
		if ( nFlags & modeNoTruncate ) {
			dwCreateFlag = OPEN_ALWAYS;
		}
		else {
			dwCreateFlag = CREATE_ALWAYS;
		}
	}
	else {
		dwCreateFlag = OPEN_EXISTING;
	}

	// special system-level access flags

	// Random access and sequential scan should be mutually exclusive
	ASSERT( 
		(nFlags & (osRandomAccess | osSequentialScan)) !=
		(osRandomAccess | osSequentialScan) );

	DWORD	dwFlags( FILE_ATTRIBUTE_NORMAL );
	if ( nFlags & osNoBuffer ) {
		dwFlags |= FILE_FLAG_NO_BUFFERING;
	}
	if ( nFlags & osWriteThrough ) {
		dwFlags |= FILE_FLAG_WRITE_THROUGH;
	}
	if ( nFlags & osRandomAccess ) {
		dwFlags |= FILE_FLAG_RANDOM_ACCESS;
	}
	if ( nFlags & osSequentialScan ) {
		dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
	}

	// attempt file creation
	HANDLE	hFile( ::CreateFile( inFileName, dwAccess, dwShareMode, &sa,
		dwCreateFlag, dwFlags, NULL ) );
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return false;
	}
	m_hFile = hFile;
	m_strFileName = inFileName;

	return true;
#else
	char*	pszMode = NULL;
	int		nFlags = inFlags & (modeReadWrite | modeNoTruncate | modeCreate);
	switch ( nFlags ) {
	case (modeRead | modeNoTruncate):
		pszMode = "rb";
		break;

	case (modeReadWrite | modeNoTruncate):
		pszMode = "r+b";
		break;

	case (modeWrite | modeCreate):
		pszMode = "wb";
		break;

	case (modeReadWrite | modeCreate):
		pszMode = "w+b";
		break;

	default:
		return false;
	}

	CPeekStringA	strFileName( inFileName );
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	errno_t	nErr = fopen_s( &m_pFile, strFileName, pszMode );
	if ( nErr != 0 ) return false; 
#else
	m_pFile = fopen( strFileName, pszMode );
#endif
	if ( !IsOpen() ) return false;

	m_strFileName = inFileName;

	return true;
#endif
}


// ----------------------------------------------------------------------------
//		OpenInProgress
// ----------------------------------------------------------------------------

void
CFileEx::OpenInProgress(
	const CPeekString&	inFileName,
	int					inFlags )
{
	ASSERT( !IsOpen() );	// shouldn't open an already open file.

	if ( IsOpen() ) {
		Close();
	}

	m_strBaseName = inFileName;
	Open( GetInProgressName(), inFlags );
}


// ----------------------------------------------------------------------------
//		Seek
// ----------------------------------------------------------------------------

UInt64
CFileEx::Seek(
	UInt64	inOffset,
	int		inFrom )
{
	ASSERT( (inFrom == begin) || (inFrom == end) || (inFrom == current) );
	ASSERT( IsOpen() );

#ifdef PEEKFILE_WIN
	ASSERT( m_hFile != INVALID_HANDLE_VALUE );
	ASSERT( (begin == FILE_BEGIN) && (end == FILE_END) && (current == FILE_CURRENT) );

	LARGE_INTEGER	liOffset;

	liOffset.QuadPart = inOffset;
	liOffset.LowPart = ::SetFilePointer( m_hFile, liOffset.LowPart, &liOffset.HighPart, static_cast<DWORD>( inFrom ) );
	if ( liOffset.LowPart == static_cast<DWORD>( -1 ) ) {
		if ( ::GetLastError() != NO_ERROR ) throw -1;
	}

	return liOffset.QuadPart;
#else
	fpos_t	nPos = 0;
	switch ( inFrom ) {
	case begin:
		nPos = inOffset;
		fsetpos( m_pFile, &nPos );
		break;

	case end:
		nPos = GetLength() - inOffset;
		fsetpos( m_pFile, &nPos );
		break;

	case current:
		fgetpos( m_pFile, &nPos );
		nPos += inOffset;
		fsetpos( m_pFile, &nPos );
		break;
	}

	return nPos;
#endif
}
