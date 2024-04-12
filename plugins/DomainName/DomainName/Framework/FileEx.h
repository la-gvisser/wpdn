// ============================================================================
//	FileEx.h
//		interface for the CFileEx class.
// ============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "PeekStrings.h"
#include "PeekTime.h"
#include "ByteVectRef.h"

#ifdef PEEKFILE_WIN
  #define	PEEKFILE_LEN	UInt32
#else
  #define	PEEKFILE_LEN	size_t

  #include <stdio.h>
  #include <iostream>
  #include <fstream>
#endif


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	File Ex and File Path
//
//	An enhanced File object. Providing Write Line and String writing methods.
//	And filename normalization, path creation, unique and In Progress (open)
//	filename support.
//
//	A File Path manages file paths. Providing access to the components of a
//	fully qualified file name. And additional servics such as name normalization
//	to created valid file names (once which does not contain invalid
//	characters).
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


namespace FilePath
{
	// POSIX & Windows cases: "", "/", "/foo", "foo", "foo/bar"
	// Windows only cases: "c:", "c:\", "c:foo", "c:\foo",
	//                     "prn:", "\\share", "\\share\", "\\share\foo"

	template< typename ChType >
	inline bool IsAbsolutePath( const ChType* psz ) {
#ifdef _WIN32
		return ( (psz != 0) &&
			(psz[0] != 0) && (psz[1] != 0) && (psz[2] != 0) &&	// len > 2
			( ((psz[1] == ':')  && ((psz[2] == '\\') || (psz[2] == '/')) ) ||	// C:\ or C:/
			((psz[0] == '\\') && (psz[1] == '\\')) ||	// \\share
			((psz[0] == '/')  && (psz[1] == '/')) ));	// //share
#else
		return (psz != 0) && (psz[0] == '/');
#endif
	}

	// Exclude "*:<>?|
	static	bool	IsValidPathChar( char c ) {
		if ( c < ' ' || c > '~' )		return false;
		if ( (c == '\"') || (c == '*') || (c == '<')
			|| (c == '>') || (c == '?') || (c == '|') )	return false;
		return true;
	}
	static	bool	IsValidPathChar( wchar_t c ) {
		if ( c < L' ' || c > L'~' )		return false;
		if ( (c == L'\"') || (c == L'*') || (c == L'<') ||
			(c == L'>') || (c == L'?') || (c == L'|') )		return false;
		return true;
	}

	// Exclude "*/:<>?\|
	static	bool	IsValidFileNameChar( char c ) {
		if ( !IsValidPathChar( c ) )		return false;
		if ( (c == '/') || (c == '\\') )	return false;
		return true;
	}
	static	bool	IsValidFileNameChar( wchar_t c ) {
		if ( !IsValidPathChar( c ) )		return false;
		if ( (c == L'/') || (c == L'\\') )	return false;
		return true;
	}

	template< typename ChType >
	inline ChType NormalizeSeparator( ChType ch )
	{
#ifdef _WIN32
		return (ch == '/') ? '\\' : ch;
#else
		return (ch == '\\') ? '/' : ch;
#endif
	}

	template< typename ChType >
	inline ChType NeutralizeSeparator( ChType ch )
	{
		return (ch == '\\') ? '/' : ch;
	}

	template< typename ChType >
	inline ChType WindowsifySeparator( ChType ch ) {
		return (ch == '/') ? '\\' : ch;
	}

	template< class T >
	class PathT
	{
	private:
		T	m_strPath;

	private:
		// Operators that are not supported.
		bool operator==( const PathT<T>& path ) const;
		bool operator!=( const PathT<T>& path ) const;
		bool operator<( const PathT<T>& path ) const;
		bool operator>( const PathT<T>& path ) const;
		bool operator<=( const PathT<T>& path ) const;
		bool operator>=( const PathT<T>& path ) const;

	public:
		typedef T string_type;
		typedef typename T::value_type value_type;

#ifdef _WIN32
		static const typename T::value_type kSeparator = '\\';
#else
		static const typename T::value_type kSeparator = '/';
#endif

		PathT() {}
		PathT( const PathT<T>& path ) : m_strPath( path.m_strPath ) {}

		explicit
		PathT( const typename T::value_type* pszPath, const bool bAddTrailingSeparator = false )
		{
			if ( pszPath != NULL ) {
				m_strPath = pszPath;
			}
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		explicit
		PathT( const T& strPath, const bool bAddTrailingSeparator = false )
			: m_strPath( strPath )
		{
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		PathT( const PathT<T>& pathBase, const PathT<T>& pathAppend, const bool bAddTrailingSeparator = false )
			: m_strPath( pathBase.m_strPath )
		{
			if ( !pathAppend.empty() ) {
				Append( pathAppend.get() );
			}
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		PathT( const PathT<T>& pathBase, const typename T::value_type* pszAppend, const bool bAddTrailingSeparator = false )
			: m_strPath( pathBase.m_strPath )
		{
			if ( pszAppend != NULL ) {
				Append( pszAppend );
			}
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		PathT( const PathT<T>& pathBase, const T& strAppend, const bool bAddTrailingSeparator = false )
			: m_strPath( pathBase.m_strPath )
		{
			Append( strAppend );
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		PathT( const typename T::value_type* pszBase, const typename T::value_type* pszAppend, const bool bAddTrailingSeparator = false )
		{
			if ( pszBase != NULL ) {
				m_strPath = pszBase;
			}
			if ( pszAppend != NULL ) {
				Append( pszAppend );
			}
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		PathT( const T& strBase, const T& strAppend, const bool bAddTrailingSeparator = false )
			: m_strPath( strBase )
		{
			Append( strAppend );
			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		PathT<T>& operator=( const T& strPath ) {
			m_strPath = strPath;
			return *this;
		}

		PathT<T>& operator=( const typename T::value_type* psz ) {
			if ( psz != NULL ) {
				m_strPath = psz;
			}
			else {
				m_strPath.clear();
			}
			return *this;
		}

		const T& get() const throw() { return m_strPath; }

		bool empty() const { return m_strPath.empty(); }

		void clear() { return m_strPath.clear(); }

		void Append( const T& str, const bool bAddTrailingSeparator = false ) {
			if ( FilePath::IsAbsolutePath( str.c_str() ) ) {
				m_strPath = str;
			}
			else {
				AddSeparator();
				m_strPath += str;
			}

			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		void Append( const typename T::value_type* psz, const bool bAddTrailingSeparator = false ) {
			if ( psz != NULL ) {
				if ( FilePath::IsAbsolutePath( psz ) ) {
					m_strPath = psz;
				}
				else {
					AddSeparator();
					m_strPath += psz;
				}
			}

			if ( bAddTrailingSeparator ) {
				AddSeparator();
			}
		}

		void AddSeparator() {
			if ( !m_strPath.empty() ) {
				const typename T::value_type	ch = *m_strPath.rbegin();
				if ( ch == '/' ) return;
#ifdef _WIN32
				if ( ch == '\\' ) return;
				m_strPath += '\\';
#else
				m_strPath += '/';
#endif
			}
		}

		void RemoveSeparator() {
			if ( !m_strPath.empty() ) {
				const typename T::value_type	ch = *m_strPath.rbegin();
#ifdef _WIN32
				if ( (ch == '/') || (ch == '\\') ) {
#else
				if ( (ch == '/') ) {
#endif
					m_strPath.erase( m_strPath.size() - 1 );
				}
			}
		}

		PathT<T>& Normalize() {
			if ( m_strPath.empty() ) return *this;

			std::transform( m_strPath.begin(), m_strPath.end(),
				m_strPath.begin(), NormalizeSeparator<value_type> );

			typename T::size_type	beg   = 0;
			typename T::size_type	start = 0;

#ifdef _WIN32
			if ( (m_strPath.size() > 2) && (m_strPath[1] == ':') && (m_strPath[2] == kSeparator) ) {
				start = 2; // drive
			}
#endif

			T	strFind;
			strFind = kSeparator;
			strFind += '.';
			strFind += '.';
			while ( (beg = m_strPath.find( strFind, beg ) ) != T::npos ) {
				typename T::size_type	end = beg + 3;
				if ( (beg == 1 && m_strPath[0] == '.') ||
					 (beg == 2 && m_strPath[0] == '.' && m_strPath[1] == '.') ||
					 (beg > 2 && m_strPath[beg-3] == kSeparator &&
					m_strPath[beg-2] == '.' && m_strPath[beg-1] == '.') ) {
					beg = end;
					continue;
				}

				if ( end < m_strPath.size() ) {
					if ( m_strPath[end] == kSeparator ) {
						++end;
					}
					else {
						beg = end;
						continue;	// Name starts with "..".
					}
				}

				while ( (beg > start) && (m_strPath[--beg] != kSeparator) ) {}
				if ( m_strPath[beg] == kSeparator ) ++beg;
				m_strPath.erase( beg, end - beg );
				if ( beg ) --beg;
			}

			if ( m_strPath.empty() ) {
				m_strPath = '.';
			}
			else {
				// Remove trailing '/' if not root directory.
				typename T::size_type sz = m_strPath.size();

#ifdef _WIN32
				if ( start ) sz -= 2; // drive
#endif

				if ( (sz > 1) && (m_strPath[m_strPath.size()-1] == kSeparator) ) {
					m_strPath.erase( m_strPath.size() - 1 );
				}
			}
			return *this;
		}

		PathT<T>& Neutralize() {
			if ( !m_strPath.empty() ) {
				std::transform( m_strPath.begin(), m_strPath.end(),
					m_strPath.begin(), NeutralizeSeparator<value_type> );
			}
			return *this;
		}

		PathT<T>& Windowsify() {
			if ( !m_strPath.empty() ) {
				std::transform( m_strPath.begin(), m_strPath.end(),
					m_strPath.begin(), WindowsifySeparator<value_type> );
			}
			return *this;
		}

		T GetDrive() const {
#ifdef _WIN32
			T	strDrive;

			if ( (m_strPath.size() > 2) &&
				 ((m_strPath[1] == ':') && ((m_strPath[2] == '\\') || (m_strPath[2] == '/'))) ) {
				strDrive = m_strPath.substr( 0, 3 );
			}
			return strDrive;
#else
			return T();
#endif
		}

		void SetDrive( const T& str ) {
#ifdef _WIN32
#ifndef NDEBUG
			_ASSERTE( str.size() < 3 );
			_ASSERTE( str.size() > 0 );
			_ASSERTE( (str.size() == 2) ? ( str[1] == ':' ) : true );
#endif

			int	nFound = str.find( ":" );
			T	strDrive = str;
			if ( nFound <= 0 ) {
				strDrive += ':';
			}

			nFound = m_strPath.find( ":" );
			// Previously set drive.
			if ( nFound <= 0 ) {
				m_strPath.replace( 0, 2, strDrive );
			}
			// Drive not set.
			else {
				m_strPath.append( strDrive, 0, strDrive.size() );
			}
#else
			// Nothing to do?
#endif
		}

		T GetDir() const {
			typename T::size_type	end = GetLeafPos( m_strPath, m_strPath.size() );

			// Skip a '/' unless it is a root directory.
			if ( end && (m_strPath[end-1] == '/') && !IsAbsoluteRoot( m_strPath, end ) ) {
				--end;
			}
			return T( m_strPath.substr( 0, end ) );
		}

		void SetDir( const T& str ) {
			// TODO
#ifndef NDEBUG
			_ASSERTE( 0 );
#endif
		}

		bool IsAbsolutePath() const {
			return FilePath::IsAbsolutePath( m_strPath.c_str() );
		}

		T GetFileName() const {
			return m_strPath.substr( GetLeafPos( m_strPath, m_strPath.size() ) );
		}

		void SetFileName( const T& strFileName ) {
			m_strPath.erase( GetLeafPos( m_strPath, m_strPath.size() ) );
			m_strPath += strFileName;
		}

		T GetFileStem() const {
			T	str = GetFileName();
			return str.substr( 0, str.rfind( '.' ) );
		}

		void SetFileStem( const T& strFileStem ) {
			T	strExt = GetExtension();
			m_strPath.erase( GetLeafPos( m_strPath, m_strPath.size() ) );
			m_strPath += strFileStem;
			m_strPath += strExt;
		}

		T GetExtension() const {
			T	str = this->GetFileName();
			if ( !str.empty() ) {
				typename T::size_type	pos = str.find_last_of( '.', str.size() - 1 );
				if ( pos != T::npos ) {
					str = str.substr( pos );
				}
				else {
					str.clear();
				}
			}
			return str;
		}

		void SetExtension( const T& strExt ) {
			T	str = this->GetFileName();
			if ( !str.empty() ) {
				typename T::size_type	pos = str.find_last_of( '.', str.size() - 1 );
				if ( pos != T::npos ) {
					str.erase( pos );
				}
				str += strExt;
				this->SetFileName( str );
			}
		}

	private:
		static typename T::size_type GetLeafPos( const T& str, typename T::size_type endpos ) {
			if ( endpos && ((str[endpos-1] == '/') || (str[endpos-1] == '\\')) ) {
				return (endpos - 1);
			}

			typename T::size_type	pos = str.find_last_of( '/', endpos-1 );
#ifdef _WIN32
			if ( pos == T::npos ) pos = str.find_last_of( '\\', endpos-1 );
			if ( pos == T::npos ) pos = str.find_last_of( ':', endpos-1 );
#endif

#ifdef _WIN32
			return ((pos == T::npos) || ((pos == 1) && (str[0] == '\\'))) ? 0 : (pos + 1);
#else
			return (pos == T::npos) ? 0 : (pos + 1);
#endif
		}

		static bool IsAbsoluteRoot( const T& str, typename T::size_type len ) {
			return (len > 0) && (str[len-1] == '/') &&
				(len == 1 // "/"
#ifdef _WIN32
				|| (len > 1
				&& (str[len-2] == ':'	// drive or device
				|| (str[0] == '/'		// share
				&& str[1] == '/'
				&& str.find( '/', 2 ) == len-1)))
#endif
				);
		}
	};

	typedef PathT< std::string > PathA;
	typedef PathT< std::wstring > PathW;
#if defined(_UNICODE) || defined(UNICODE)
	typedef PathW Path;
#else
	typedef PathA Path;
#endif

} /* namespace FilePath */


// ============================================================================
//		CFileEx
// ============================================================================

class CFileEx
{
public:
	enum OpenFlags {
		modeRead =         0x00001,	// read only
		modeWrite =        0x00002,	// read and write
		modeReadWrite =    0x00003,	// modeRead | modeWrite
		modeNew =          0x00800,	// fail if exist
		modeCreate =       0x01000,	// create if does not exist
		modeNoTruncate =   0x02000,	// do not truncate if exist
#ifdef PEEKFILE_WIN
		shareCompat =      0x00000,
		shareExclusive =   0x00010,
		shareDenyWrite =   0x00020,
		shareDenyRead =    0x00030,
		shareDenyNone =    0x00040,
		modeNoInherit =    0x00080,
		typeText =         0x04000, // typeText and typeBinary are
		typeBinary =       0x08000, // used in derived classes only
		osNoBuffer =       0x10000,
		osWriteThrough =   0x20000,
		osRandomAccess =   0x40000,
		osSequentialScan = 0x80000,
#endif
	};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
	};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

protected:
#ifdef PEEKFILE_WIN
	HANDLE			m_hFile;
#else
	FILE*			m_pFile;
#endif
	CPeekString		m_strFileName;
	CPeekString		m_strBaseName;

public:
	static CPeekString	s_strMaxPreamble;

public:
	static CPeekString	BuildUniqueName( const CPeekString& inLabel, const CPeekString& inOutputDirectory, const CPeekString& inType );
	static bool			IsFilePath( const CPeekString& inPathPath );
	static bool			IsLegalFileName( const CPeekString& inFileName );
	static bool			IsLegalFilePath( const CPeekString& inFilePath );
	static CPeekString	LegalizeFileName( const CPeekString& inFileName, wchar_t inReplace = L'_' );	// Without drive letter.
	static CPeekString	LegalizeFilePath( const CPeekString& inFilePath );	// Fully qualified path.
	static bool			MakePath( const CPeekString& inPath );

public:
	;			CFileEx();
	;			CFileEx( const CPeekString& inFileName, UINT nOpenFlags );
	virtual		~CFileEx();

	void		Close();

	void		Flush() {
		if ( !IsOpen() ) return;
#ifdef PEEKFILE_WIN
		if ( !::FlushFileBuffers( m_hFile ) ) throw -1;
#else
		if ( fflush( m_pFile ) == EOF ) throw -1;
#endif
	}

	UInt64		GetPosition() {
#ifdef PEEKFILE_WIN
#else
		fpos_t nPos = 0;
		if ( IsOpen() ) fgetpos( m_pFile, &nPos );
		return nPos;
#endif
	}
	CPeekString	GetFileName() const { return m_strFileName; }
	CPeekString	GetInProgressName() const;
	virtual UInt64	GetLength();

	bool		IsOpen() const {
#ifdef PEEKFILE_WIN
		return (m_hFile != INVALID_HANDLE_VALUE);
#else
		return (m_pFile != NULL);
#endif
	}

	bool		Open( const CPeekString& inFileName, int inFlags );
	void		OpenInProgress( const CPeekString& inFileName, int inFlags );

	size_t		Read( PEEKFILE_LEN inCount, void* outData ) {
		_ASSERTE( IsOpen() );
		_ASSERTE( (inCount != 0) ? (outData != NULL) : true );
#ifdef PEEKFILE_WIN
		if ( inCount == 0 ) return 0;	// avoid Win32 "null-read"
		DWORD	dwRead;
		if ( !::ReadFile( m_hFile, outData, inCount, &dwRead, NULL ) ) throw -1;
		return dwRead;
#else
		return fread( static_cast<char *>( outData ), 1, inCount, m_pFile );
#endif
	}
	void		Reset() { Close(); }
	void		Rewind() { Seek( 0, begin ); }

	virtual UInt64	Seek( UInt64 inOffset, int inFrom );
	UInt64			SeekToEnd() { return Seek( 0, end ); }

	size_t		Write( PEEKFILE_LEN inCount, const void* inData ) {
		_ASSERTE( IsOpen() );
		_ASSERTE( (inCount != 0) ? (inData != NULL) : true );
#ifdef PEEKFILE_WIN
		if ( inCount == 0 ) { return 0; }	// avoid Win32 "null-read"
		DWORD	dwWritten;
		if ( !::WriteFile( m_hFile, inData, inCount, &dwWritten, NULL ) ) throw -1;
		return dwWritten;
#else
		return fwrite( static_cast<const char *>( inData ), 1, inCount, m_pFile );
#endif
	}
	void		Write( const CPeekString& inStr ) {
		CPeekStringA	strA( inStr );
		Write( strA );
	}
	void		Write( const CPeekStringA& inStr ) {
#ifdef PEEKFILE_WIN
		Write( inStr.GetLength32(), inStr  );
#else
		Write( inStr.GetLength(), inStr  );
#endif
	}
	void		Write( const CByteVectRef& inRange ) {
		if ( inRange.GetCount() > kMaxUInt32 ) Peek::Throw( -1 );
		Write( static_cast<UInt32>( inRange.GetCount() ),
			static_cast<const void*>( inRange.GetData( inRange.GetCount() ) ) );
	}
};
