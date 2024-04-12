// =============================================================================
//	filepath.h
// =============================================================================
//	Copyright (c) 2005-2011 WildPackets, Inc. All rights reserved.

#ifndef FILEPATH_H
#define FILEPATH_H

#include <algorithm>
#include <string>

namespace FilePath
{

// POSIX & Windows cases: "", "/", "/foo", "foo", "foo/bar"
// Windows only cases: "c:", "c:\", "c:foo", "c:\foo",
//                     "prn:", "\\share", "\\share\", "\\share\foo"

template< typename ChType >
inline bool IsAbsolutePath( const ChType* psz )
{
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
inline ChType WindowsifySeparator( ChType ch )
{
	return (ch == '/') ? '\\' : ch;
}

template< class T >
class PathT
{
public:
	typedef T string_type;
	typedef typename T::value_type value_type;

#ifdef _WIN32
	static const typename T::value_type kSeparator = '\\';
#else
	static const typename T::value_type kSeparator = '/';
#endif

	PathT()
	{
	}

	PathT( const PathT<T>& path ) :
		m_strPath( path.m_strPath )
	{
	}

	explicit
	PathT( const typename T::value_type* pszPath, const bool bAddTrailingSeparator = false )
	{
		if ( pszPath != NULL )
		{
			m_strPath = pszPath;
		}
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	explicit
	PathT( const T& strPath, const bool bAddTrailingSeparator = false ) :
		m_strPath( strPath )
	{
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	PathT( const PathT<T>& pathBase, const PathT<T>& pathAppend, const bool bAddTrailingSeparator = false ) :
		m_strPath( pathBase.m_strPath )
	{
		if ( !pathAppend.empty() )
		{
			Append( pathAppend.get() );
		}
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	PathT( const PathT<T>& pathBase, const typename T::value_type* pszAppend, const bool bAddTrailingSeparator = false ) :
		m_strPath( pathBase.m_strPath )
	{
		if ( pszAppend != NULL )
		{
			Append( pszAppend );
		}
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	PathT( const PathT<T>& pathBase, const T& strAppend, const bool bAddTrailingSeparator = false ) :
		m_strPath( pathBase.m_strPath )
	{
		Append( strAppend );
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	PathT( const typename T::value_type* pszBase, const typename T::value_type* pszAppend, const bool bAddTrailingSeparator = false )
	{
		if ( pszBase != NULL )
		{
			m_strPath = pszBase;
		}
		if ( pszAppend != NULL )
		{
			Append( pszAppend );
		}
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	PathT( const T& strBase, const T& strAppend, const bool bAddTrailingSeparator = false ) :
		m_strPath( strBase )
	{
		Append( strAppend );
		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	PathT<T>& operator=( const T& strPath )
	{
		m_strPath = strPath;
		return *this;
	}

	PathT<T>& operator=( const typename T::value_type* psz )
	{
		if ( psz != NULL )
		{
			m_strPath = psz;
		}
		else
		{
			m_strPath.clear();
		}
		return *this;
	}

	const T& get() const throw()
	{
		return m_strPath;
	}

	bool empty() const
	{
		return m_strPath.empty();
	}

	void clear()
	{
		m_strPath.clear();
	}

	void swap( PathT<T>& other )
	{
		m_strPath.swap( other.m_strPath );
	}

	void Append( const T& str, const bool bAddTrailingSeparator = false )
	{
		if ( FilePath::IsAbsolutePath( str.c_str() ) )
		{
			m_strPath = str;
		}
		else
		{
			AddSeparator();
			m_strPath += str;
		}

		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	void Append( const typename T::value_type* psz, const bool bAddTrailingSeparator = false )
	{
		if ( psz != NULL )
		{
			if ( FilePath::IsAbsolutePath( psz ) )
			{
				m_strPath = psz;
			}
			else
			{
				AddSeparator();
				m_strPath += psz;
			}
		}

		if ( bAddTrailingSeparator )
		{
			AddSeparator();
		}
	}

	void AddSeparator()
	{
		if ( !m_strPath.empty() )
		{
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
	
	void RemoveSeparator()
	{
		if ( !m_strPath.empty() )
		{
			const typename T::value_type	ch = *m_strPath.rbegin();
			if ( (ch == '/')
#ifdef _WIN32
				|| (ch == '\\')
#endif
				)
			{
				m_strPath.erase( m_strPath.size() - 1 );
			}
		}
	}

	PathT<T>& Normalize()
	{
		if ( m_strPath.empty() ) return *this;

		std::transform( m_strPath.begin(), m_strPath.end(),
			m_strPath.begin(), NormalizeSeparator<value_type> );

		typename T::size_type	beg   = 0;
		typename T::size_type	start = 0;

#ifdef _WIN32
		if ( (m_strPath.size() > 2) && (m_strPath[1] == ':') && (m_strPath[2] == kSeparator) )
		{
			start = 2; // drive
		}
#endif

		T	strFind;
		strFind  = kSeparator;
		strFind += '.';
		strFind += '.';
		while ( (beg = m_strPath.find( strFind, beg ) ) != T::npos )
		{
			typename T::size_type	end = beg + 3;
			if ( (beg == 1 && m_strPath[0] == '.') ||
				(beg == 2 && m_strPath[0] == '.' && m_strPath[1] == '.') ||
				(beg > 2 && m_strPath[beg-3] == kSeparator &&
					m_strPath[beg-2] == '.' && m_strPath[beg-1] == '.') )
			{
				beg = end;
				continue;
			}
		
			if ( end < m_strPath.size() )
			{
				if ( m_strPath[end] == kSeparator )
				{
					++end;
				}
				else
				{
					beg = end;
					continue;	// Name starts with "..".
				}
			}

			while ( (beg > start) && (m_strPath[--beg] != kSeparator) ) {}
			if ( m_strPath[beg] == kSeparator ) ++beg;
			m_strPath.erase( beg, end - beg );
			if ( beg ) --beg;
		}

		if ( m_strPath.empty() )
		{
			m_strPath = '.';
		}
		else
		{
			// Remove trailing '/' if not root directory.
			typename T::size_type sz = m_strPath.size();

#ifdef _WIN32
			if ( start ) sz -= 2; // drive
#endif

			if ( sz > 1 && m_strPath[m_strPath.size()-1] == kSeparator )
			{
				m_strPath.erase( m_strPath.size()-1 );
			}
		}
		
		return *this;
	}

	PathT<T>& Neutralize()
	{
		if ( !m_strPath.empty() )
		{
			std::transform( m_strPath.begin(), m_strPath.end(),
				m_strPath.begin(), NeutralizeSeparator<value_type> );
		}
		return *this;
	}

	PathT<T>& Windowsify()
	{
		if ( !m_strPath.empty() )
		{
			std::transform( m_strPath.begin(), m_strPath.end(),
				m_strPath.begin(), WindowsifySeparator<value_type> );
		}
		return *this;
	}

	T GetDrive() const
	{
#ifdef _WIN32
		T	strDrive;

		if ( (m_strPath.size() > 2) && 
			((m_strPath[1] == ':') && ((m_strPath[2] == '\\') || (m_strPath[2] == '/'))) )
		{
			strDrive = m_strPath.substr( 0, 3 );
		}

		return strDrive;
#else
		return T();
#endif
	}
	
	void SetDrive( const T& str )
	{
#ifdef _WIN32
#ifdef ASSERT
		ASSERT( str.size() < 3 );
		ASSERT( str.size() > 0 );
		ASSERT( (str.size() == 2) ? ( str[1] == ':' ) : true );
#endif

		int	nFound = str.find( ":" );
		T	strDrive = str;
		if ( nFound <= 0 )
		{
			strDrive += ':';
		}

		nFound = m_strPath.find( ":" );
		// Previously set drive.
		if ( nFound <= 0 )
		{
			m_strPath.replace( 0, 2, strDrive ); 
		}
		// Drive not set.
		else
		{
			m_strPath.append( strDrive, 0, strDrive.size() );	
		}
#else
		// Nothing to do?
#endif
	}

	T GetDir() const
	{
		typename T::size_type	end = GetLeafPos( m_strPath, m_strPath.size() );

		// Skip a '/' unless it is a root directory.
		if ( end && (m_strPath[end-1] == '/') && !IsAbsoluteRoot( m_strPath, end ) )
		{
			--end;
		}
		return T( m_strPath.substr( 0, end ) );
	}
	
	void SetDir( const T& str )
	{
		// TODO
#ifdef ASSERT
		ASSERT( 0 );
#endif
	}

	bool IsAbsolutePath() const
	{
		return FilePath::IsAbsolutePath( m_strPath.c_str() );
	}

	T GetFileName() const
	{
		typename T::size_type	endpos = m_strPath.size();
		if ( endpos )
		{
			const typename T::value_type	ch = m_strPath[endpos-1];
			if ( (ch != '/') && (ch != '\\') )
			{
				typename T::size_type	pos = m_strPath.find_last_of( '/', endpos-1 );
#ifdef _WIN32
				if ( pos == T::npos ) pos = m_strPath.find_last_of( '\\', endpos-1 );
				if ( pos == T::npos ) pos = m_strPath.find_last_of( ':', endpos-1 );

				if ( (pos == 1) && (m_strPath[0] == '\\') )
				{
					pos = pos + 1;
				}
#endif

				if ( pos != T::npos )
				{
					return m_strPath.substr( pos + 1 );
				}
				else
				{
					return m_strPath;
				}
			}
		}

		return T();
	}

	void SetFileName( const T& strFileName )
	{
		m_strPath.erase( GetLeafPos( m_strPath, m_strPath.size() ) );
		m_strPath += strFileName;
	}

	T GetFileStem() const
	{
		T	str = GetFileName();
		return str.substr( 0, str.rfind( '.' ) );
	}

	void SetFileStem( const T& strFileStem )
	{
		T	strExt = GetExtension();
		m_strPath.erase( GetLeafPos( m_strPath, m_strPath.size() ) );
		m_strPath += strFileStem;
		m_strPath += strExt;
	}

	T GetExtension() const
	{
		T	str = this->GetFileName();
		if ( !str.empty() )
		{
			typename T::size_type	pos = str.find_last_of( '.', str.size() - 1 );
			if ( pos != T::npos )
			{
				str = str.substr( pos );
			}
			else
			{
				str.clear();
			}
		}

		return str;
	}
	
	void SetExtension( const T& strExt )
	{
		T	str = this->GetFileName();
		if ( !str.empty() )
		{
			typename T::size_type	pos = str.find_last_of( '.', str.size() - 1 );
			if ( pos != T::npos )
			{
				str.erase( pos );
			}

			str += strExt;

			this->SetFileName( str );
		}
	}

	bool IsRootPath() const
	{
		typename T::size_type	end = GetLeafPos( m_strPath, m_strPath.size() );
		return IsAbsoluteRoot( m_strPath, end );
	}

private:
	static typename T::size_type GetLeafPos( const T& str, typename T::size_type endpos )
	{
		if ( endpos && ((str[endpos-1] == '/') || str[endpos-1] == '\\') )
			return endpos-1;

		typename T::size_type	pos = str.find_last_of( '/', endpos-1 );
#ifdef _WIN32
		if ( pos == T::npos ) pos = str.find_last_of( '\\', endpos-1 );
		if ( pos == T::npos ) pos = str.find_last_of( ':', endpos-1 );
#endif

		return ((pos == T::npos)
#ifdef _WIN32
			|| ((pos == 1) && (str[0] == '\\'))
#endif
			) ? 0 : (pos + 1);
	}

	static bool IsAbsoluteRoot( const T& str, typename T::size_type len )
	{
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

private:
	// Operators that we don't support.
	bool operator==( const PathT<T>& path ) const;
	bool operator!=( const PathT<T>& path ) const;
	bool operator<( const PathT<T>& path ) const;
	bool operator>( const PathT<T>& path ) const;
	bool operator<=( const PathT<T>& path ) const;
	bool operator>=( const PathT<T>& path ) const;

private:
	T	m_strPath;
};

typedef PathT< std::string > PathA;
typedef PathT< std::wstring > PathW;
#if defined(_UNICODE) || defined(UNICODE)
typedef PathW Path;
#else
typedef PathA Path;
#endif

} /* namespace FilePath */

#endif /* FILEPATH_H */
