// ============================================================================
//	FileUtil.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#include "StdAfx.h"
#include <shlobj.h>
#include <atlfile.h>
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
//		StCurrentDirectory
// ----------------------------------------------------------------------------

StCurrentDirectory::StCurrentDirectory(
	LPCTSTR inDirectory )
		: m_bGood( FALSE )
{
	Save();

	::SetCurrentDirectory( inDirectory );
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


// ----------------------------------------------------------------------------
//		LegalizeFilename
// ----------------------------------------------------------------------------

void
FileUtil::LegalizeFilename(
	LPTSTR	pszFileName,
	TCHAR	chReplace )
{
	// Sanity checks.
	ASSERT( pszFileName != NULL );
	if ( pszFileName == NULL ) return;

	const TCHAR		chIllegal[] = _T("\\/:*?\"<>|");
	const size_t	nIllegal = sizeof(chIllegal)/sizeof(chIllegal[0]);

	TCHAR*	pch = pszFileName;
	while ( *pch != 0 )
	{
		TCHAR	ch = *pch;
		for ( size_t i = 0; i < nIllegal; i++ )
		{
			if ( ch == chIllegal[i] )
			{
				*pch = chReplace;
				break;
			}
		}

		pch++;
	}
}


// ----------------------------------------------------------------------------
//		MakePath
// ----------------------------------------------------------------------------

bool 
FileUtil::MakePath( 
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
FileUtil::FileExists( 
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
FileUtil::DirectoryExists( 
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
FileUtil::GetFileSize(
	LPCTSTR	inFile )
{
	WIN32_FIND_DATA	findFileData;
	HANDLE			hFind = ::FindFirstFile( (LPTSTR) inFile, &findFileData );
	if ( hFind == INVALID_HANDLE_VALUE ) return 0;
	VERIFY( ::FindClose( hFind ) );
	
	ASSERT( findFileData.nFileSizeHigh == 0 );
	return findFileData.nFileSizeLow;
}


// ----------------------------------------------------------------------------
//		ExpandShortcut
// ----------------------------------------------------------------------------

HRESULT
FileUtil::ExpandShortcut(
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
            hr = pPersistFile->Load( CT2OLE(strPath), STGM_READ );
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
FileUtil::CompareFiles(
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
FileUtil::IsXMLFile(
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
	if ( FAILED(hr) ) return false;
	file.Close();

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
