// ============================================================================
//	FileEx.cpp
//		implementation of the CFileEx class.
// ============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "FileEx.h"
#include "PeekStrings.h"

#ifdef _WIN32
#define FILE_SEPERATOR_A	'\\'
#define FILE_SEP_STR_A		"\\"
#define FILE_SEPERATOR_W	L'\\'
#define FILE_SEP_STR_W		L"\\"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cwctype>
#define MAX_PATH	260
#define FILE_SEPERATOR_A	'/'
#define FILE_SEP_STR_A		"/"
#define FILE_SEPERATOR_W	L'/'
#define FILE_SEP_STR_W		L"/"
#endif // _WIN32


CPeekString	CFileEx::s_strMaxPreamble( L"\\\\?\\" );


// -----------------------------------------------------------------------------
//		MakeRegExMask
// -----------------------------------------------------------------------------

CPeekStringA
MakeRegExMask(
	const CPeekStringA& inMask )
{
	CPeekStringA strMask;

	static std::string	tokens( "\\.+$^/|()[]" );
	for ( auto itr = inMask.begin(); itr != inMask.end(); ++itr ) {
		const char	c( *itr );

		if ( c == '?' ) {
			strMask += '.';
			continue;
		}
		if ( c == '*' ) {
			strMask += '.';
			strMask += '*';
			continue;
		}

		if ( tokens.find_first_of( c ) != std::string::npos ) {
			strMask += '\\';
		}
		strMask += c;
	}

	return strMask;
}


#if (0)
// -----------------------------------------------------------------------------
//		RegExErrorToString
// -----------------------------------------------------------------------------

CPeekStringA
RegExErrorToString(
	std::regex_constants::error_type	inType )
{
	CPeekStringA strCode;

	switch ( inType ) {
	case std::regex_constants::error_collate:
		strCode = "error_collate";
		break;
	case  std::regex_constants::error_ctype:
		strCode = "errorregex_constants_ctype";
		break;
	case  std::regex_constants::error_escape:
		strCode = "error_escape";
		break;
	case  std::regex_constants::error_backref:
		strCode = "error_backref";
		break;
	case  std::regex_constants::error_brack:
		strCode = "error_brack";
		break;
	case  std::regex_constants::error_paren:
		strCode = "error_paren";
		break;
	case  std::regex_constants::error_brace:
		strCode = "error_brace";
		break;
	case  std::regex_constants::error_badbrace:
		strCode = "error_badbrace";
		break;
	case  std::regex_constants::error_range:
		strCode = "error_range";
		break;
	case  std::regex_constants::error_space:
		strCode = "error_space";
		break;
	case  std::regex_constants::error_badrepeat:
		strCode = "error_badrepeat";
		break;
	case  std::regex_constants::error_complexity:
		strCode = "error_complexity";
		break;
	case  std::regex_constants::error_stack:
		strCode = "error_stack";
		break;
	default:
		strCode = "Other";
	}

	return strCode;
}
#endif


// -----------------------------------------------------------------------------
//		IsMatchingFileName
// -----------------------------------------------------------------------------

bool
IsMatchingFileName(
	const CPeekString&	inFileName,
	const CPeekString&	inMask )
{
	CPeekStringA	strFileName( inFileName );
	CPeekStringA	strMask( inMask );

	return IsMatchingFileName( strFileName, strMask );
}

bool
IsMatchingFileName(
	const CPeekStringA&	inFileName,
	const CPeekStringA&	inMask )
{
	bool			bResult( false );
	CPeekStringA	strMask;

	try {
		strMask = MakeRegExMask( inMask );
		bResult = std::regex_match( inFileName.c_str(), std::regex( strMask.c_str() ) );
	}
	catch ( const std::regex_error& /*e*/ ) {
		// cout << "regex_error caught: " << e.what() << endl;
		// cout << RegExErrorToString(e.code()) << endl;
		bResult = false;
	}
	catch ( ... ) {
		// cout << "Failure" << endl;
		bResult = false;
	}

	return bResult;
}

bool
IsMatchingFileName(
	const CPeekString&	inFileName,
	std::regex	        inRegEx )
{
	CPeekStringA	strFileName( inFileName );

	return IsMatchingFileName( strFileName, inRegEx );
}


bool
IsMatchingFileName(
	const CPeekStringA&	inFileName,
	std::regex	        inRegEx )
{
	bool			bResult( false );
	CPeekStringA	strMask;

	try {
		std::smatch reMatch;
		bResult = std::regex_search( inFileName, reMatch, inRegEx );
		if ( bResult && (reMatch.size() > 1) ) {
		}
	}
	catch ( const std::regex_error& /*e*/ ) {
		// cout << "regex_error caught: " << e.what() << endl;
		// cout << RegExErrorToString(e.code()) << endl;
		bResult = false;
	}
	catch ( ... ) {
		// cout << "Failure" << endl;
		bResult = false;
	}

	return bResult;
}


// ============================================================================
//		CFileDetail Utils
// ============================================================================

// -----------------------------------------------------------------------------
//		GetFileDetail
// -----------------------------------------------------------------------------

bool
GetFileDetail(
	const wchar_t*	inFileName,
	CFileDetail&	outDetail )
{
#ifdef _WIN32
	WIN32_FIND_DATAW	wfd;
	const HANDLE		hFind = ::FindFirstFileW( inFileName, &wfd );
	if ( (hFind != nullptr) && (hFind != INVALID_HANDLE_VALUE) ) {
		return GetFileDetail( wfd, outDetail );
	}
#else
	(void) inFileName;
	(void) outDetail;
#endif

	return false;
}

bool
GetFileDetail(
	const FilePath::PathA&	inPath,
	struct stat64*			inStat,
	CFileDetail&			outDetail )
{
#ifdef _WIN32
	(void) inStat;
	CPeekString			strFileName( inPath.get() );
	WIN32_FIND_DATAW	wfd;
	const HANDLE		hFind = ::FindFirstFileW( strFileName.c_str(), &wfd );
	if ( (hFind != nullptr) && (hFind != INVALID_HANDLE_VALUE) ) {
		return GetFileDetail( wfd, outDetail );
	}

	return false;
#else
	struct stat64	st;
	struct stat64*	pStat( nullptr );
	const char*		pPath( inPath.get().c_str() );
	if ( inStat == nullptr ) {
		if ( stat64( pPath, &st ) != 0 ) return -1;
	}
	else {
		pStat = inStat;
	}

	if ( pStat == nullptr ) return -1;

	const char*		pFileName( inPath.GetFileName().c_str() );
	CPeekStringA	strFileNameA( pFileName );
	CPeekString		strFileName( strFileNameA );
#ifdef _DEBUG
	const wchar_t*	szFileName( strFileName.c_str() ); (void) szFileName;
#endif
	UInt32			nAttributes = 0;
	if ( S_ISDIR( st.st_mode ) ) {
		nAttributes |= CFileDetail::kAttribute_Directory;
	}
	CPeekTime		ptModified( kMilliseconds, pStat->st_mtime );

	outDetail.Set( strFileName, pStat->st_size, nAttributes, ptModified );
	return true;
#endif
}

#ifdef _WIN32
bool
GetFileDetail(
	const WIN32_FIND_DATAW&	inFindData,
	CFileDetail&			outDetail )
{
	ULARGE_INTEGER	nSize = {
		inFindData.nFileSizeLow,
		inFindData.nFileSizeHigh
	};

	FILETIME	ftModified = {
		inFindData.ftLastWriteTime.dwLowDateTime,
		inFindData.ftLastWriteTime.dwHighDateTime
	};
	CPeekTime	ptModified( ftModified );

	outDetail.Set( inFindData.cFileName, nSize.QuadPart,
		inFindData.dwFileAttributes, ptModified );

	return true;
}
#endif


// -----------------------------------------------------------------------------
//		GetFileDetailList
// -----------------------------------------------------------------------------

bool
GetFileDetailList(
	const std::wstring&	inPath,
	const std::wstring&	inMask,
	CFileDetailList&	outFileList )
{
	bool			bResult( false );

#ifdef _WIN32
	FilePath::Path	pathSearch( inPath.c_str() );
	pathSearch.Append( inMask.c_str() );

	WIN32_FIND_DATAW	wfd;
	const HANDLE		hFind = ::FindFirstFileW( pathSearch.get().c_str(), &wfd );
	if ( (hFind != nullptr) && (hFind != INVALID_HANDLE_VALUE) ) {
		bResult = true;
		for ( ;; ) {
			CFileDetail	fd;
			GetFileDetail( wfd, fd );

			outFileList.push_back( fd );

			if ( !::FindNextFileW( hFind, &wfd ) ) {
				bResult = (::GetLastError() == ERROR_NO_MORE_FILES);
				break;
			}
		}
		::FindClose( hFind );
	}
#else
	CPeekStringA	strPath( inPath );
	CPeekStringA	strMask( inMask );

#ifdef _DEBUG
	const char*		szPath( strPath.c_str() ); (void) szPath;
	const char*		szMask( strMask.c_str() ); (void) szMask;
#endif

	DIR*	pDir = opendir( strPath.c_str() );
	// Peek::ThrowIf( pDir == nullptr );
	if ( pDir == nullptr ) return false;

	bResult = true;
	struct dirent*	pEnt;
	while ( (pEnt = readdir( pDir )) != nullptr ) {
		const CPeekStringA	strName( pEnt->d_name );
		if ( (strName != ".") && (strName != "..") &&
			 IsMatchingFileName( strName, strMask ) ) {
			FilePath::PathA		path( strPath, strName );
			const char*			pPath( path.get().c_str() );
			struct stat64		st;
			if ( (stat64( pPath, &st ) == 0) && !S_ISDIR( st.st_mode ) ) {
				CFileDetail	fd;
				GetFileDetail( path, &st, fd );
				outFileList.push_back( fd );
			}
		}
	}

	closedir( pDir );
#endif

	return bResult;
}


// ============================================================================
//		CFileEx
// ============================================================================

CFileEx::CFileEx()
#ifdef PEEKFILE_WIN
	:	m_hFile( INVALID_HANDLE_VALUE )
#else
	:	m_pFile( nullptr )
#endif
{
}

CFileEx::CFileEx(
	const CPeekString&	inFileName,
	UInt32				inOpenFlags )
#ifdef PEEKFILE_WIN
	:	m_hFile( INVALID_HANDLE_VALUE )
#else
	:	m_pFile( nullptr )
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
		strFileName << inOutputDirectory << FILE_SEPERATOR_W << inLabel << L" " << strTimeStamp << inType;

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
	if ( m_pFile == nullptr ) {
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
//		Delete
// ----------------------------------------------------------------------------

void
CFileEx::Delete()
{
	Close();

	if ( !m_strFileName.IsEmpty() ) {
#ifdef PEEKFILE_WIN
		::DeleteFile( m_strFileName );
#else
		CPeekStringA	strFileName( m_strFileName );
		unlink( strFileName );
#endif
	}
	m_strFileName.Empty();
}


// ----------------------------------------------------------------------------
//		DeleteFile
// ----------------------------------------------------------------------------

bool
CFileEx::DeleteFile(
	const CPeekString&	inFileName )
{
	if ( inFileName.IsEmpty() ) return false;

#ifdef PEEKFILE_WIN
	return (::DeleteFile( inFileName.c_str() ) != FALSE);
#else
	CPeekStringA	strFileName( inFileName );
	return (unlink( strFileName.c_str() ) == 0);
#endif
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
	struct stat64	st;

	CPeekStringA	strFileName( (const CPeekString&)GetFileName() );
	if ( stat64( (const char*)strFileName, &st ) != 0 ) throw -1;

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
#ifdef WP_LINUX
	CPeekStringA	strFilePath( inFilePath );
	strFilePath.TrimRight( FILE_SEP_STR_A );

	struct stat	st;
	return (stat( strFilePath, &st ) == 0);
#else
	CPeekString	strFilePath = inFilePath;
	strFilePath.TrimRight( L"\\" );

	struct _stat	st;
	return (_wstat( strFilePath, &st ) == 0);
#endif
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

	return ((inFileName.Find( chUnprintable ) < 0) &&
		    (inFileName.Find( chIllegal ) < 0) );
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

	size_t	nOffset = ((isalpha( inFilePath[0] ) && (inFilePath[1] == L':'))) ? 2 : 0;

	return (inFilePath.Find( chIllegal, nOffset ) < 0);
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

#ifdef _WIN32
	strFilePath.Replace( L'/', L'\\' );
#endif

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

#ifdef WP_LINUX
	strRootPath.Replace( L'\\', L'/' );
#endif

	return strRootPath;
}


// ----------------------------------------------------------------------------
//		MakePath													[static]
// ----------------------------------------------------------------------------

bool
CFileEx::MakePath(
	const CPeekString&	inPath )
{
	CPeekStringA	tmpPath( inPath );
	_ASSERTE( !tmpPath.IsEmpty() );
	if ( tmpPath.IsEmpty() ) return false;

	bool bRetVal( false );

	// Make sure the directory name ends in a slash. PathAddBackslash()
	if ( tmpPath.Right( 1 ) != FILE_SEP_STR_A ) {
		tmpPath += FILE_SEPERATOR_A;
	}

	// Create each directory in the path.
	CPeekStringA	dirName;
	int				nIndex( 0 );
	bool			bDone( false );
	while ( !bDone ) {
		// Extract one directory.
		nIndex = tmpPath.Find( FILE_SEP_STR_A );
		if ( nIndex != -1 ) {
			dirName = dirName + tmpPath.Left( nIndex );
			tmpPath = tmpPath.Right( tmpPath.GetLength() - nIndex - 1 );

			// The first time through, we might have a drive name.
			if ( (dirName.GetLength() >= 1) && (dirName.Right( 1 ) != ":") ) {
#ifdef WP_LINUX
				int	nMode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
				mkdir( dirName, nMode );
#else
				bRetVal = (::CreateDirectoryA( dirName, nullptr ) != FALSE);
#endif
			}
			dirName += FILE_SEPERATOR_W;
		}
		else {
			bDone = true;
		}
	}
	// Return the last MakeDirectory() return value.
	return bRetVal;
}


// ----------------------------------------------------------------------------
//		Move														[static]
// ----------------------------------------------------------------------------

bool
CFileEx::MoveFile(
	const CPeekString&	inSource,
	const CPeekString&	inDestination )
{
	if ( inSource.IsEmpty() || inDestination.IsEmpty() ) return false;
#ifdef _WIN32
	return (::MoveFileW( inSource.c_str(), inDestination.c_str() ) != FALSE);
#else
	CPeekStringA	strSource( inSource );
	CPeekStringA	strDestination( inDestination );

	struct stat64	st;
	if ( stat64( strDestination.c_str(), &st ) == 0 ) return false;	// File exists.

	return (rename( strSource.c_str(), strDestination.c_str() ) == 0);
#endif
}


// ----------------------------------------------------------------------------
//		Open
// ----------------------------------------------------------------------------

static const char	s_szReadNoTruncate[] = "rb";
static const char	s_szReadWriteNoTruncate[] = "r+b";
static const char	s_szWriteCreate[] = "wb";
static const char	s_szReadWriteCreate[] = "w+b";

bool
CFileEx::Open(
	const CPeekString&	inFileName,
	int					inFlags )
{
	_ASSERTE( !IsOpen() );	// shouldn't open an already open file.

	if ( IsOpen() ) {
		Close();
	}

#ifdef PEEKFILE_WIN
	// Always binary and CreateFile does not need flag
	UINT	nFlags = static_cast<UINT>( inFlags & ~typeBinary );

	// map read/write mode
	_ASSERTE( (modeRead | modeWrite | modeReadWrite) == 3 );
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
		_ASSERTE( false );	// invalid share mode
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
		_ASSERTE( dwShareMode );	// invalid share mode?
	}

	// map modeNoInherit flag
	SECURITY_ATTRIBUTES	sa;
	sa.nLength = sizeof( sa );
	sa.lpSecurityDescriptor = nullptr;
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
	_ASSERTE(
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
		dwCreateFlag, dwFlags, nullptr ) );
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return false;
	}
	m_hFile = hFile;
	m_strFileName = inFileName;

	return true;
#else
	const char*	pszMode = nullptr;
	int			nFlags = inFlags & (modeReadWrite | modeNoTruncate | modeCreate);
	switch ( nFlags ) {
	case modeRead:
	case (modeRead | modeNoTruncate):
		pszMode = s_szReadNoTruncate;
		break;

	case (modeReadWrite | modeNoTruncate):
		pszMode = s_szReadWriteNoTruncate;
		break;

	case (modeWrite | modeCreate):
		pszMode = s_szWriteCreate;
		break;

	case (modeReadWrite | modeCreate):
		pszMode = s_szReadWriteCreate;
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
	_ASSERTE( !IsOpen() );	// shouldn't open an already open file.

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
	_ASSERTE( (inFrom == begin) || (inFrom == end) || (inFrom == current) );
	_ASSERTE( IsOpen() );

#ifdef PEEKFILE_WIN
	_ASSERTE( m_hFile != INVALID_HANDLE_VALUE );
	_ASSERTE( (begin == FILE_BEGIN) && (end == FILE_END) && (current == FILE_CURRENT) );

	LARGE_INTEGER	liOffset;

	liOffset.QuadPart = inOffset;
	liOffset.LowPart = ::SetFilePointer( m_hFile, liOffset.LowPart, &liOffset.HighPart, static_cast<DWORD>( inFrom ) );
	if ( liOffset.LowPart == static_cast<DWORD>( -1 ) ) {
		if ( ::GetLastError() != NO_ERROR ) throw -1;
	}

	return liOffset.QuadPart;
#else
	switch ( inFrom ) {
	case begin:
		{
			fseek( m_pFile, inOffset, SEEK_SET );
		}
		break;

	case end:
		{
			fseek( m_pFile, inOffset, SEEK_CUR );
		}
		break;

	case current:
		{
			fseek( m_pFile, inOffset, SEEK_END );
		}
		break;
	}

	return ftell( m_pFile );
#endif
}
