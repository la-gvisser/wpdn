// ============================================================================
//	ut_filepath.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

#include "StdAfx.h"

#define kWinPath	0
#define kNeutPath	1	// Linux

//#define _NEUT_TEST
#ifdef _NEUT_TEST

#ifdef _WIN32
#undef _WIN32
#include "FileEx.h"
#define kPathType	kNeutPath
#endif

#else
#include "FileEx.h"
#define kPathType	kWinPath
#endif //_NEUT_TEST


#define kWinSep		L"\\"
#define kNeutSep	L"/"

CPeekString	kSep[2] = { kWinSep, kNeutSep };
CPeekString	g_strDrive[2] = { L"C:\\", kNeutSep };
CPeekString	g_strFileName( L"MyFile.txt" );
CPeekString	g_strStem( L"MyFile" );
CPeekString	g_strExtension( L".txt" );
// CPeekString	g_strDirectory[2] = { L"Temp", L"Document" };

int
TestDrive(
	int	nType )
{
	int		nFailures( 0 );

	// "C:\", /
	FilePath::Path	path( g_strDrive[nType] );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( !bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( strDrive != g_strDrive[nType] ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != g_strDrive[nType] ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( !strFileName.empty() ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( !strStem.empty() ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( !strExtension.empty() ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( !strLastDir.empty() ) nFailures++;

	path.PopDir();
	if ( path.get() != g_strDrive[nType] ) nFailures++;

	return nFailures;
}


int
TestDriveFile(
	int	nType )
{
	int		nFailures( 0 );

	// "C:\MyFile.txt", "/MyFile.txt"
	FilePath::Path	path( g_strDrive[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( !bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( strDrive != g_strDrive[nType] ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != g_strDrive[nType] ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( !strLastDir.empty() ) nFailures++;

	path.PopDir();
	if ( path.get() != g_strDrive[nType] + L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestFile(
	int	nType )
{
	int		nFailures( 0 );

	// "MyFile.txt"
	FilePath::Path	path( L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( !strDrive.empty() ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( !strDirectory.empty() ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( !strLastDir.empty() ) nFailures++;

	path.PopDir();
	if ( path.get() != L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestRelativeFile(
	int	nType )
{
	int		nFailures( 0 );

	// "C:\MyFile.txt"
	FilePath::Path	path( L"." + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( !strDrive.empty() ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (L"." + kSep[nType]) ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"." ) nFailures++;

	path.PopDir();
	if ( path.get() != L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestDriveDirFile(
	int	nType )
{
	int		nFailures( 0 );

	// "C:\Temp\MyFile.txt"
	FilePath::Path	path( g_strDrive[nType] + L"Temp" + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( !bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( strDrive != g_strDrive[nType] ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (g_strDrive[nType] + L"Temp" + kSep[nType]) ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Temp" ) nFailures++;

	path.PopDir();
	if ( path.get() != (g_strDrive[nType] + L"MyFile.txt") ) nFailures++;

	return nFailures;
}


int
TestDirFile(
	int	nType )
{
	int		nFailures( 0 );

	// "Temp\MyFile.txt"
	FilePath::Path	path( L"Temp" + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( !strDrive.empty() ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (L"Temp" + kSep[nType]) ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Temp" ) nFailures++;

	path.PopDir();
	if ( path.get() != L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestRelativeDirFile(
	int	nType )
{
	int		nFailures( 0 );

	// ".\Temp\MyFile.txt"
	FilePath::Path	path( L"." + kSep[nType] + L"Temp" + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( !strDrive.empty() ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (L"." + kSep[nType] + L"Temp" + kSep[nType]) ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Temp" ) nFailures++;

	path.PopDir();
	if ( path.get() != L"." + kSep[nType] + L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestDriveDirDirFile(
	int	nType )
{
	int		nFailures( 0 );

	// "C:\Temp\Document\MyFile.txt"
	FilePath::Path	path( g_strDrive[nType] + L"Temp" + kSep[nType] + L"Document" + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( !bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( strDrive != g_strDrive[nType] ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (g_strDrive[nType] + L"Temp" + kSep[nType] + L"Document" + kSep[nType]) ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Document" ) nFailures++;

	path.PopDir();
	if ( path.get() != (g_strDrive[nType] + L"Temp" + kSep[nType] + L"MyFile.txt") ) nFailures++;

	return nFailures;
}


int
TestDirDirFile(
	int	nType )
{
	int		nFailures( 0 );

	// "Temp\Document\MyFile.txt"
	FilePath::Path	path( L"Temp" + kSep[nType] + L"Document" + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( !strDrive.empty() ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (L"Temp" + kSep[nType] + L"Document" + kSep[nType]) ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Document" ) nFailures++;

	path.PopDir();
	if ( path.get() != L"Temp" + kSep[nType] + L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestRelativeDirDirFile(
	int	nType )
{
	int		nFailures( 0 );

	// ".\Temp\Document\MyFile.txt"
	FilePath::Path	path( L"." + kSep[nType] + L"Temp" + kSep[nType] + L"Document" + kSep[nType] + L"MyFile.txt" );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( !strDrive.empty() ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (L"." + kSep[nType] + L"Temp" + kSep[nType] + L"Document" + kSep[nType])) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( strFileName != g_strFileName ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( strStem != g_strStem ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( strExtension != g_strExtension ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Document" ) nFailures++;

	path.PopDir();
	if ( path.get() != L"." + kSep[nType] + L"Temp" + kSep[nType] + L"MyFile.txt" ) nFailures++;

	return nFailures;
}


int
TestDriveDirDir(
	int	nType )
{
	int		nFailures( 0 );

	// "C:\Temp\Document\"
	FilePath::Path	path( g_strDrive[nType] + L"Temp" + kSep[nType] + L"Document" + kSep[nType] );
	bool			bAbsPath( path.IsAbsolutePath() );
	if ( !bAbsPath ) nFailures++;

	CPeekString		strDrive( path.GetDrive() );
	if ( strDrive != g_strDrive[nType] ) nFailures++;

	CPeekString		strDirectory( path.GetDir() );
	if ( strDirectory != (g_strDrive[nType] + L"Temp" + kSep[nType] + L"Document") ) nFailures++;

	CPeekString		strFileName( path.GetFileName() );
	if ( !strFileName.empty() ) nFailures++;

	CPeekString		strStem( path.GetFileStem() );
	if ( !strStem.empty() ) nFailures++;

	CPeekString		strExtension( path.GetExtension() );
	if ( !strExtension.empty() ) nFailures++;

	CPeekString		strLastDir( path.GetLastDir() );
	if ( strLastDir != L"Document" ) nFailures++;

	path.PopDir();
	if ( path.get() != (g_strDrive[nType] + L"Temp" + kSep[nType]) ) nFailures++;

	return nFailures;
}


int
UT_FilePath()
{
	int		nFailures( 0 );

	nFailures += TestDrive( kPathType );
	nFailures += TestDriveFile( kPathType );
	nFailures += TestFile( kPathType );
	nFailures += TestRelativeFile( kPathType );
	nFailures += TestDriveDirFile( kPathType );
	nFailures += TestDirFile( kPathType );
	nFailures += TestRelativeDirFile( kPathType );
	nFailures += TestDriveDirDirFile( kPathType );
	nFailures += TestDirDirFile( kPathType );
	nFailures += TestRelativeDirDirFile( kPathType );
	nFailures += TestDriveDirDir( kPathType );

	return nFailures;
}

#ifdef _NEUT_TEST
#define _WIN32
#endif
