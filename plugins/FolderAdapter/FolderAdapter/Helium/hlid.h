// =============================================================================
//	hlid.h
// =============================================================================
//	Copyright (c) 2005-2011 WildPackets, Inc. All rights reserved.

#ifndef HLID_H
#define HLID_H

#include "heid.h"
#include "hlexcept.h"

namespace HeLib
{

template <typename T>
inline Helium::HeID
HeIDFromString( const T* psz )
{
	Helium::HeID	id;
	if ( psz == nullptr )
	{
		id.SetNull();
	}
	else
	{
		HeThrowIf( !id.Parse( psz ) );
	}
	return id;
}

template <typename T>
class CHeIDToString
{
public:
	typedef T char_type;

	char_type	m_sz[Helium::HeID::string_length];

	CHeIDToString( const Helium::HeID& id, bool braces = true )
	{
		HeThrowIf( !id.ToString( m_sz, Helium::HeID::string_length, braces ) );
	}

	operator const char_type*() const throw()
	{
		return m_sz;
	}

	const char_type* c_str() const throw()
	{
		return m_sz;
	}

	const char_type* data() const throw()
	{
		return m_sz;
	}

	char_type* data() throw()
	{
		return m_sz;
	}

private:
	CHeIDToString();
	CHeIDToString( const CHeIDToString& );
	CHeIDToString& operator=( const CHeIDToString& );
};

typedef CHeIDToString<char> CHeIDToStringA;
typedef CHeIDToString<wchar_t> CHeIDToStringW;

struct HashHeID
{
	std::size_t operator()(const Helium::HeID& id) const
	{
		return id.Hash();
	}
};

} /* namespace HeLib */

#endif /* HLID_H */
