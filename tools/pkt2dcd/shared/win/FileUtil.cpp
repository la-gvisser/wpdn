// ============================================================================
//	FileUtil.cpp
// ============================================================================
//	Copyright (c) 1998-2012 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include <shellapi.h>
#include <shlobj.h>
#include <atlfile.h>
#include <atlpath.h>
#include <vector>
#include "FileUtil.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// ============================================================================
//	CFileName
// ============================================================================

// ----------------------------------------------------------------------------
//		CFileName
// ----------------------------------------------------------------------------

CFileName::CFileName()
{
}


// ----------------------------------------------------------------------------
//		CFileName(const CString&)
// ----------------------------------------------------------------------------

CFileName::CFileName(
	const CString&	inPath )
{
	SetPath( inPath );
}


// ----------------------------------------------------------------------------
//		operator=(const CString&)
// ----------------------------------------------------------------------------

CFileName&
CFileName::operator=(
	const CString&	inPath )
{
	SetPath( inPath );
	return *this;
}


// ----------------------------------------------------------------------------
//		SetPath
// ----------------------------------------------------------------------------

void
CFileName::SetPath(
	const CString&	inPath )
{
	m_Path = inPath;
	UpdateParts();
}


// ----------------------------------------------------------------------------
//		SetDrive
// ----------------------------------------------------------------------------

void
CFileName::SetDrive(
	const CString&	inDrive )
{
	m_Drive = inDrive;
	UpdatePath();
}


// ----------------------------------------------------------------------------
//		SetDir
// ----------------------------------------------------------------------------

void
CFileName::SetDir(
	const CString&	inDir )
{
	m_Dir = inDir;
	UpdatePath();
}


// ----------------------------------------------------------------------------
//		SetName
// ----------------------------------------------------------------------------

void
CFileName::SetName(
	const CString&	inName )
{
	m_Name = inName;
	UpdatePath();
}


// ----------------------------------------------------------------------------
//		SetExt
// ----------------------------------------------------------------------------

void
CFileName::SetExt(
	const CString&	inExt )
{
	m_Ext = inExt;
	UpdatePath();
}


// ----------------------------------------------------------------------------
//		UpdateParts
// ----------------------------------------------------------------------------

void
CFileName::UpdateParts()
{
	if ( m_Path.IsEmpty() )
	{
		m_Drive.Empty();
		m_Dir.Empty();
		m_Name.Empty();
		m_Ext.Empty();
	}
	else
	{
		_tsplitpath(
			m_Path,
			m_Drive.GetBuffer( _MAX_DRIVE ),
			m_Dir.GetBuffer( _MAX_DIR ),
			m_Name.GetBuffer( _MAX_FNAME ),
			m_Ext.GetBuffer( _MAX_EXT ) );

		m_Drive.ReleaseBuffer();
		m_Dir.ReleaseBuffer();
		m_Name.ReleaseBuffer();
		m_Ext.ReleaseBuffer();
	}
}


// ----------------------------------------------------------------------------
//		UpdatePath
// ----------------------------------------------------------------------------

void
CFileName::UpdatePath()
{
	m_Path = m_Drive + m_Dir + m_Name + m_Ext;
}


// ============================================================================
//	StCurrentDirectory
// ============================================================================

// ----------------------------------------------------------------------------
//		StCurrentDirectory
// ----------------------------------------------------------------------------

StCurrentDirectory::StCurrentDirectory()
	: m_bGood( FALSE )
{
	Save();
}


// ----------------------------------------------------------------------------
//		~StCurrentDirectory
// ----------------------------------------------------------------------------

StCurrentDirectory::~StCurrentDirectory()
{
	Restore();
}


// ----------------------------------------------------------------------------
//		Save
// ----------------------------------------------------------------------------

void
StCurrentDirectory::Save()
{
	m_bGood = (::GetCurrentDirectory( MAX_PATH, m_szCurrDir ) > 0);
}


// ----------------------------------------------------------------------------
//		Restore
// ----------------------------------------------------------------------------

void
StCurrentDirectory::Restore() const
{
	if ( m_bGood )
	{
		::SetCurrentDirectory( m_szCurrDir );
	}
}


// ============================================================================
//	StFileIterator
// ============================================================================

// ----------------------------------------------------------------------------
//		StFileIterator(LPCTSTR)
// ----------------------------------------------------------------------------

StFileIterator::StFileIterator(
	LPCTSTR	inFileName )
{
	m_hFind = ::FindFirstFile( inFileName, &m_FindData );
	m_bDone = (m_hFind == INVALID_HANDLE_VALUE) || (m_hFind == NULL);
	m_dwResult = ::GetLastError();
}


// ----------------------------------------------------------------------------
//		~StFileIterator
// ----------------------------------------------------------------------------

StFileIterator::~StFileIterator()
{
	if ( (m_hFind != INVALID_HANDLE_VALUE) && (m_hFind != NULL) )
	{
		::FindClose( m_hFind );
	}
}


// ----------------------------------------------------------------------------
//		operator ++
// ----------------------------------------------------------------------------

const WIN32_FIND_DATA&
StFileIterator::operator ++()
{
	if ( (m_hFind != INVALID_HANDLE_VALUE) && (m_hFind != NULL) )
	{
		if ( !::FindNextFile( m_hFind, &m_FindData ) )
		{
			// Done searching.
			m_bDone = TRUE;

			// Get the last error code.
			m_dwResult = ::GetLastError();

			// ERROR_NO_MORE_FILES is the expected/normal result.
			if ( m_dwResult == ERROR_NO_MORE_FILES )
			{
				m_dwResult = ERROR_SUCCESS;
			}
		}
	}
	else
	{
		m_bDone = FALSE;
	}

	return m_FindData;
}


// ============================================================================
//	FileUtil
// ============================================================================
namespace FileUtil {


StDirectoryOfDoom::~StDirectoryOfDoom()
{
	try
	{
		if ( !m_strPath.IsEmpty() )
		{
			Destroy( m_strPath );
		}
	}
	catch( ... )	// Never throw from a destructor.
	{
	}
}

void
StDirectoryOfDoom::Destroy( const CString & strPath )
{
	// double-null-terminate SHFILEOPSTRUCT.pFrom.
	// I probably could copy into a MAX_PATH+1-sized array, or I could use a dynamic vector of whatever size necessary.
	LPCTSTR	pPath	= strPath;
	std::vector< TCHAR >	vPath( pPath, pPath + strPath.GetLength() );
	vPath.push_back( TCHAR(0) );
	vPath.push_back( TCHAR(0) );

	SHFILEOPSTRUCT	op;
	::memset( &op, 0, sizeof( op ) );
	op.wFunc	= FO_DELETE;
	op.pFrom 	= &vPath.front();
	op.fFlags	= FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
//	op.lpszProgressTitle	= _T("#Removing temporary web files");
	(void) ::SHFileOperation( &op );
}


// ----------------------------------------------------------------------------
//		LegalizeFilename
// ----------------------------------------------------------------------------

void
LegalizeFilename(
	LPTSTR	pszFileName,
	TCHAR	chReplace,
	BOOL	bForWeb, /* = FALSE */
	LPCTSTR chAddIllegal /* = NULL */ )
{
	// Sanity checks.
	ASSERT( pszFileName != NULL );
	if ( pszFileName == NULL ) return;

	const TCHAR		chIllegalWeb[] = _T("\\/:*?\"<>|%#");
	const TCHAR		chIllegal[]    = _T("\\/:*?\"<>|");
	CString			illegal = (bForWeb) ? chIllegalWeb : chIllegal;

	if ( chAddIllegal != NULL )
		illegal += chAddIllegal;

	TCHAR*	pch = pszFileName;
	while ( *pch != 0 )
	{
		TCHAR	ch = *pch;
		const TCHAR *pchIllegal = illegal;

		while ( *pchIllegal != 0 )
		{
			if ( ch == *pchIllegal )
			{
				*pch = chReplace;
				break;
			}

			pchIllegal++;
		}

		pch++;
	}
}


// ----------------------------------------------------------------------------
//		MakePath
// ----------------------------------------------------------------------------

bool 
MakePath( 
	LPCTSTR inPath )
{
	ASSERT( inPath != NULL );
	if ( inPath == NULL ) return false;

	CString	tmpPath( inPath );
	ASSERT( tmpPath.GetLength() > 0 );
	if ( tmpPath.GetLength() == 0 ) return false;

	bool bRetVal = false;
	
	// Make sure the directory name ends in a slash.
	if ( tmpPath[tmpPath.GetLength() - 1] != _T('\\') )
	{
		tmpPath += _T('\\');
	}

	// Create each directory in the path.
	CString	dirName;
	int		nIndex = 0;
	bool	bDone = false;
	while ( !bDone )
	{
		// Extract one directory.
		nIndex = tmpPath.Find( _T('\\') );
		if ( nIndex != -1 )
		{
			dirName = dirName + tmpPath.Left( nIndex );
			tmpPath = tmpPath.Right( tmpPath.GetLength() - nIndex - 1 );

			// The first time through, we might have a drive name.
			if ( (dirName.GetLength() >= 1) && (dirName[dirName.GetLength() - 1] != _T(':')) )
			{
				bRetVal = (::CreateDirectory( dirName, NULL ) == TRUE);
			}
			dirName += _T('\\');
		}
		else
		{
			// We're finished
			bDone = true;
		}
	}

	// Return the last MakeDirectory() return value.
	return bRetVal;
}


// ----------------------------------------------------------------------------
//		FileExists
// ----------------------------------------------------------------------------

bool 
FileExists( 
	LPCTSTR	inFile )
{
	ASSERT( inFile != NULL );
	if ( inFile == NULL ) return false;

	DWORD	dwAttributes = ::GetFileAttributes( inFile );
	return (dwAttributes != INVALID_FILE_ATTRIBUTES);
}


// ----------------------------------------------------------------------------
//		DirectoryExists
// ----------------------------------------------------------------------------

bool 
DirectoryExists( 
	LPCTSTR	inDirectory )
{
	ASSERT( inDirectory != NULL );
	if ( inDirectory == NULL ) return false;

	DWORD	dwAttributes = ::GetFileAttributes( inDirectory );
	if ( dwAttributes == INVALID_FILE_ATTRIBUTES ) return false;
	
	return ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}


// ----------------------------------------------------------------------------
//		GetFileSize
// ----------------------------------------------------------------------------

DWORD
GetFileSize(
	LPCTSTR	inFile )
{
	WIN32_FIND_DATA	fd;
	HANDLE			hFind = ::FindFirstFile( (LPTSTR) inFile, &fd );
	if ( hFind == INVALID_HANDLE_VALUE ) return 0;
	VERIFY( ::FindClose( hFind ) );
	
	ASSERT( fd.nFileSizeHigh == 0 );
	return fd.nFileSizeLow;
}


// ----------------------------------------------------------------------------
//		GetFileSize64
// ----------------------------------------------------------------------------

DWORDLONG
GetFileSize64(
	LPCTSTR	inFile )
{
	ULARGE_INTEGER	cb;

	WIN32_FIND_DATA	fd;
	HANDLE			hFind = ::FindFirstFile( (LPTSTR) inFile, &fd );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		VERIFY( ::FindClose( hFind ) );
		cb.u.HighPart = fd.nFileSizeHigh;
		cb.u.LowPart = fd.nFileSizeLow;
	}
	else
	{
		cb.u.HighPart = 0;
		cb.u.LowPart = 0;
	}

	return cb.QuadPart;
}


// ----------------------------------------------------------------------------
//		ExpandShortcut
// ----------------------------------------------------------------------------

HRESULT
ExpandShortcut(
	const CString&	strPath,
	CString&		strExpandedPath )
{
    // Sanity checks.
    ASSERT( !strPath.IsEmpty() );
	if ( strPath.IsEmpty() ) return E_INVALIDARG;

    // Create instance for shell link.
    HRESULT		hr;
    IShellLink*	pShellLink = NULL;
    hr = ::CoCreateInstance( CLSID_ShellLink, NULL,
		CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &pShellLink );
    if ( SUCCEEDED( hr ) )
    {
        // Get the persist file interface.
        IPersistFile*	pPersistFile = NULL;
        hr = pShellLink->QueryInterface( IID_IPersistFile, (LPVOID*) &pPersistFile );
        if ( SUCCEEDED( hr ) )
        {
            // Load shortcut.
#if defined(_UNICODE) || defined(UNICODE)
			hr = pPersistFile->Load( strPath, STGM_READ );
#else
			hr = pPersistFile->Load( ATL::CA2W( strPath ), STGM_READ );
#endif
			if ( SUCCEEDED( hr ) )
			{
				// Find the path from that.
				hr = pShellLink->GetPath( strExpandedPath.GetBuffer( MAX_PATH ),
					MAX_PATH, NULL, SLGP_UNCPRIORITY );
				strExpandedPath.ReleaseBuffer();
				if ( !SUCCEEDED( hr ) )
				{
					// Clear the result.
					strExpandedPath.Empty();
				}
            }
            pPersistFile->Release();
        }
        pShellLink->Release();
    }

    return hr;
}


// ----------------------------------------------------------------------------
//		CompareFiles
// ----------------------------------------------------------------------------

HRESULT
CompareFiles(
	LPCTSTR	pszFile1,
	LPCTSTR	pszFile2,
	int&	nResult )
{
	// Sanity checks.
	if ( pszFile1 == NULL ) return E_POINTER;
	if ( pszFile2 == NULL ) return E_POINTER;

	HRESULT		hr;
	CAtlFile	file1;
	hr = file1.Create( pszFile1, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
	if ( FAILED(hr) ) return hr;

	CAtlFileMapping<char>	fileMap1;
	hr = fileMap1.MapFile( file1 );
	if ( FAILED(hr) ) return hr;

	CAtlFile	file2;
	hr = file2.Create( pszFile2, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
	if ( FAILED(hr) ) return hr;

	CAtlFileMapping<char>	fileMap2;
	hr = fileMap2.MapFile( file2 );
	if ( FAILED(hr) ) return hr;

	if ( fileMap1.GetMappingSize() == fileMap2.GetMappingSize() )
	{
		nResult = memcmp( fileMap1.GetData(), fileMap2.GetData(), fileMap1.GetMappingSize() );
	}
	else if ( fileMap1.GetMappingSize() > fileMap2.GetMappingSize() )
	{
		nResult = 1;
	}
	else
	{
		nResult = -1;
	}

	return S_OK;
}


// ----------------------------------------------------------------------------
//		IsXMLFile
// ----------------------------------------------------------------------------

bool
IsXMLFile(
	LPCTSTR	pszFile )
{
	// Sanity checks.
	if ( pszFile == NULL ) return false;

	HRESULT		hr;
	CAtlFile	file;
	hr = file.Create( pszFile, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
	if ( FAILED(hr) ) return false;

	// Read the first 12 bytes.
	BYTE	buffer[12];
	hr = file.Read( buffer, 12 );
	file.Close();
	if ( FAILED(hr) ) return false;

	static const BYTE	ayXmlAscii[] = { '<', '?', 'x', 'm', 'l' };
	static const BYTE	ayXmlUtf8[] = { 0xef, 0xbb, 0xbf, '<', '?', 'x', 'm', 'l' };
	static const BYTE	ayXmlUtf16le[] = { 0xff, 0xfe, 0x3C, 0x00, 0x3F, 0x00, 0x78, 0x00, 0x6D, 0x00, 0x6C, 0x00 };
	static const BYTE	ayXmlUtf16be[] = { 0xfe, 0xff, 0x00, 0x3C, 0x00, 0x3F, 0x00, 0x78, 0x00, 0x6D, 0x00, 0x6C };
	static struct XmlChecks
	{
		const BYTE*	ayXmlCheck;
		size_t		cbXmlCheck;
	} checks[] = 
	{
		{ ayXmlAscii,	sizeof(ayXmlAscii) },
		{ ayXmlUtf8,	sizeof(ayXmlUtf8) },
		{ ayXmlUtf16le,	sizeof(ayXmlUtf16le) },
		{ ayXmlUtf16be,	sizeof(ayXmlUtf16be) }
	};

	const size_t	nChecks = (sizeof(checks)/sizeof(checks[0]));
	for ( size_t i = 0; i < nChecks; i++ )
	{
		ASSERT( sizeof(buffer) >= checks[i].cbXmlCheck );
		if ( memcmp( buffer, checks[i].ayXmlCheck, checks[i].cbXmlCheck ) == 0 )
		{
			return true;
		}
	}

	return false;
}


// ----------------------------------------------------------------------------
//		AvoidCollision
// ----------------------------------------------------------------------------
/// If no file exists at ioPath, return false and output ioPath unchanged.
/// If a file at ioPath exists, append a number (starting with " 2") and keep
/// increasing the number as necessary until you find a clear spot. Return true.

bool
AvoidCollision( CString & ioPath )
{
	// If no collision, there's nothing more to do.
	static const bool	NO_COLLISION_SO_PATH_UNCHANGED		= false;
	static const bool	COLLISION_AVOIDED_PATH_RENUMBERED	= true;
	bool	bFileExists	= FileExists( ioPath );
	if ( !bFileExists ) return NO_COLLISION_SO_PATH_UNCHANGED;

	CFileName		filename( ioPath );
	const CString	strOrigFilename	= filename.GetName();
	CString			strFilename;
	static const int	INFINITE_LOOP	= 100000;	// for small values of "infinite"
	for ( int i = 2; i < INFINITE_LOOP ; ++i )
	{
		// Renumber. If still collides, loop around and try again with the next number.
		strFilename.Format( _T("%s %d"), strOrigFilename.GetString(), i );
		filename.SetName( strFilename );
		bFileExists	= FileExists( filename.GetPath() );
		if ( bFileExists ) continue;
		
		// Doesn't exist. We found a safe spot.
		ioPath = filename.GetPath();
		return COLLISION_AVOIDED_PATH_RENUMBERED;
	}
	// If you got here, you either have 100,000 numbered files in a folder,
	// or there's a bug in the above loop.
	ASSERT( !INFINITE_LOOP );
	return NO_COLLISION_SO_PATH_UNCHANGED;	// Lie.
}

//---------------------------------------------------------------------
//		SanitizeExtension()
//---------------------------------------------------------------------
/// lowercase, trim leading/trailing whitespace, and force a leading dot.
void
SanitizeExtension( CString & ioExtension )
{
	ioExtension.MakeLower();
	ioExtension.TrimLeft();
	ioExtension.TrimRight();
	if ( ioExtension.GetAt(0) == TCHAR('.') )
	{
		CString	str2 = ioExtension.Mid( 1 );
		ioExtension = str2;
	}
}

//---------------------------------------------------------------------
//		GetPayloadTempDirPath()
//---------------------------------------------------------------------
/// @return a full path to $TEMP/OmniPeek_Payloads" where you can
///			write temp payload files that you want deleted on
///			application exit.
CString
GetPayloadTempDirPath()
{
	static StDirectoryOfDoom	s_Cleanup;
	static CString				s_strWPTempPath;
	
	if ( s_strWPTempPath.IsEmpty() )
	{
		// Find temp directory, forcing it into existence.
		CString strTempPath;
		GetTempPath( _MAX_PATH, strTempPath.GetBuffer( _MAX_PATH ) );
		strTempPath.ReleaseBuffer();
		::CreateDirectory( strTempPath, NULL );
		// Create a WildPackets-controlled directory within the temp directory, just for our payloads.
		s_strWPTempPath = strTempPath + _T("\\OmniPeek_Payloads");
		::CreateDirectory( s_strWPTempPath, NULL );
		// OK if temp folder find/create failed. We'll die later in AvoidCollision() or SavePayload().

		// Register temp folder for eventual deletion upon application exit.
		s_Cleanup.m_strPath = s_strWPTempPath;
	}
	
	return s_strWPTempPath;
}

//---------------------------------------------------------------------
//		OpenFileWithDefaultApplication()
//---------------------------------------------------------------------
///	Launch the given file with the user's preferred application, same
/// as double-clicking the file from the Windows Explorer.
HINSTANCE
OpenFileWithDefaultApplication( const CString & inPath )
{
	CPath	pathDir( inPath );
	(void) pathDir.RemoveFileSpec();
	CString	strPathDir	= pathDir;
		
	// Open it.
	static const HWND		kHWNDNone		= NULL;
	static const LPCTSTR	kDefaultOpen	= NULL;
	static const LPCTSTR	kNoParameters	= NULL;
#pragma warning( disable : 4311 )
	HINSTANCE	nResult = ::ShellExecute( kHWNDNone, kDefaultOpen, inPath,
		kNoParameters, strPathDir, SW_SHOWNORMAL );
	return nResult;
}


}	// namespace FileUtil
