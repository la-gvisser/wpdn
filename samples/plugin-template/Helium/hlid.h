// =============================================================================
//	hlid.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef HLID_H
#define HLID_H

#include "heid.h"
#include "hlexcept.h"
#include <functional>

namespace HeLib
{

template <typename T>
inline Helium::HeID
HeIDFromString( const T* psz )
{
	Helium::HeID	id;
	if ( psz == NULL )
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

	CHeIDToString( const Helium::HeID& id )
	{
		HeThrowIf( !id.ToString( m_sz, Helium::HeID::string_length ) );
	}

	operator const char_type*() const throw()
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

struct HashHeID : std::unary_function<Helium::HeID,std::size_t>
{
	std::size_t operator()(const Helium::HeID& id) const
	{
		return id.Hash();
	}
};

} /* namespace HeLib */

#endif /* HLID_H */
