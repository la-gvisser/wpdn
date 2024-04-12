// ============================================================================
//	FileUtil.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1998-2000. All rights reserved.

#pragma once

// ============================================================================
//	CFileName
// ============================================================================

class CFileName
{
public:
	CFileName();
	CFileName( const CString& inPath );
	
	CFileName&		operator=( const CString& inPath );

	void			SetPath( const CString& inPath );
	const CString&	GetPath() const { return m_Path; }
	
	void			SetDrive( const CString& inDrive );
	const CString&	GetDrive() const { return m_Drive; }
	
	void			SetDir( const CString& inDir );
	const CString&	GetDir() const { return m_Dir; }
	
	void			SetName( const CString& inName );
	const CString&	GetName() const { return m_Name; }
	
	void			SetExt( const CString& inExt );
	const CString&	GetExt() const { return m_Ext; }
	
protected:
	CString			m_Path;		// Full path.
	CString			m_Drive;	// Drive letter and colon (eg, c:).
	CString			m_Dir;		// Directories (eg, \dirA\dirB\dirC\).
	CString			m_Name;		// File name (eg, file).
	CString			m_Ext;		// File extension (eg, .txt).

	void			UpdateParts();
	void			UpdatePath();
};


// ============================================================================
//	StCurrentDirectory
// ============================================================================

class StCurrentDirectory
{
public:
	StCurrentDirectory();
	~StCurrentDirectory();

	void	Save();
	void	Restore() const;

private:
	TCHAR	m_szCurrDir[MAX_PATH];
	BOOL	m_bGood;
};


// ============================================================================
//	StFileIterator
// ============================================================================

class StFileIterator
{
public:
	StFileIterator( LPCTSTR inFileName );
	~StFileIterator();

	const WIN32_FIND_DATA&	Current() const { return m_FindData; }
	operator const TCHAR*() const { return m_bDone ? NULL : m_FindData.cFileName; }
	const WIN32_FIND_DATA&	operator ++();
	DWORD					Result() const { return m_dwResult; }

private:
	WIN32_FIND_DATA m_FindData;
	HANDLE			m_hFind;
	BOOL			m_bDone;
	DWORD			m_dwResult;
};


// ============================================================================
//	FileUtil
// ============================================================================

namespace FileUtil
{
	/// Attempt to destroy the directory in m_strPath upon destruction.
	class StDirectoryOfDoom
	{
	public :
		CString	m_strPath;
		~StDirectoryOfDoom();
		void Destroy( const CString & strPath );
	};

	void		LegalizeFilename( LPTSTR pszFileName, TCHAR chReplace, BOOL bForWeb = FALSE, LPCTSTR chAddIllegal = NULL );
	bool		MakePath( LPCTSTR inPath );
	bool		FileExists( LPCTSTR inFile );
	bool		DirectoryExists( LPCTSTR inDirectory );
	DWORD		GetFileSize( LPCTSTR inFile );
	DWORDLONG	GetFileSize64( LPCTSTR inFile );
	HRESULT		ExpandShortcut( const CString& strPath, CString& strExpandedPath );
	HRESULT		CompareFiles( LPCTSTR pszFile1, LPCTSTR pszFile2, int& nResult );
	bool		IsXMLFile( LPCTSTR pszFile );
	bool		AvoidCollision( CString & ioPath );
	void		SanitizeExtension( CString & ioExtension );
	CString		GetPayloadTempDirPath();
	HINSTANCE	OpenFileWithDefaultApplication( const CString & inPath );
} /* namespace FileUtil */
