// =============================================================================
//	heid.h
// =============================================================================
//	Copyright (c) 2005-2011 WildPackets, Inc. All rights reserved.

#ifndef HEID_H
#define HEID_H

#include "hecore.h"
#include "heinttypes.h"
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>

namespace Helium
{

/// \def HE_NULL_ID
/// \brief Same as GUID_NULL
#define HE_NULL_ID \
{0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}

/// \struct HeID
/// \brief Same as GUID or nsID
#include "hepushpack.h"

struct HeIDHashHelper
{
	UInt32	Data1;
	UInt32	Data2;
	UInt32	Data3;
	UInt32	Data4;
} HE_PACK_STRUCT;

struct HeID
{
	UInt32	Data1;
	UInt16	Data2;
	UInt16	Data3;
	UInt8	Data4[8];

	/// Length of an ID as string including terminator.
	static const std::size_t	string_length = 39;

	/// Returns a zeroed out HeID (unlike HE_NULL_ID, this can be passed as a
	/// parameter and used in initializer lists).
	static inline const HeID& Null()
	{
		static const HeID idNull = HE_NULL_ID;
		return idNull;
	}

	inline bool operator==( const HeID& other ) const throw()
	{
		return Equals( other );
	}

	inline bool operator!=( const HeID& other ) const throw()
	{
		return !operator==( other );
	}

	inline bool operator<( const HeID& other ) const throw()
	{
		return (std::memcmp( this, &other, sizeof(HeID) ) < 0);
	}

	inline bool operator>( const HeID& other ) const throw()
	{
		return (std::memcmp( this, &other, sizeof(HeID) ) > 0);
	}

	/// Compare this id with another.
	inline bool Equals( const HeID& other ) const throw()
	{
		return (std::memcmp( this, &other, sizeof(HeID) ) == 0);
	}

	/// Determine if this id is equal to HE_NULL_ID.
	inline bool IsNull() const throw()
	{
		return (*this == Null());
	}

	/// Set this id to HE_NULL_ID.
	inline void SetNull() throw()
	{
		std::memset( this, 0, sizeof(HeID) );
	}

	/// Converts an id into a string of printable characters of the form
	/// {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}.
	inline bool ToString( char* sz, std::size_t cchMax, bool bBraces = true ) const
	{
		if ( sz == nullptr ) return false;
		if ( cchMax < string_length ) return false;

		*sz = 0;

		const char*		pszFmt = bBraces ?
			"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}" :
			"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";

		std::sprintf( sz, pszFmt,
			static_cast<unsigned int>(Data1),
			static_cast<unsigned int>(Data2),
			static_cast<unsigned int>(Data3),
			static_cast<unsigned int>(Data4[0]),
			static_cast<unsigned int>(Data4[1]),
			static_cast<unsigned int>(Data4[2]),
			static_cast<unsigned int>(Data4[3]),
			static_cast<unsigned int>(Data4[4]),
			static_cast<unsigned int>(Data4[5]),
			static_cast<unsigned int>(Data4[6]),
			static_cast<unsigned int>(Data4[7]) );

		return true;
	}

	/// Converts an id into a string of printable characters of the form
	/// {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}.
	inline bool ToString( wchar_t* sz, std::size_t cchMax, bool bBraces = true ) const
	{
		if ( sz == nullptr ) return false;
		if ( cchMax < string_length ) return false;

		*sz = 0;

		const wchar_t*	pszFmt = bBraces ?
			L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}" :
			L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";

		std::swprintf( sz, cchMax, pszFmt,
			static_cast<unsigned int>(Data1),
			static_cast<unsigned int>(Data2),
			static_cast<unsigned int>(Data3),
			static_cast<unsigned int>(Data4[0]),
			static_cast<unsigned int>(Data4[1]),
			static_cast<unsigned int>(Data4[2]),
			static_cast<unsigned int>(Data4[3]),
			static_cast<unsigned int>(Data4[4]),
			static_cast<unsigned int>(Data4[5]),
			static_cast<unsigned int>(Data4[6]),
			static_cast<unsigned int>(Data4[7]) );

		return true;
	}

	/// Parses a string of the form {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
	/// into an id.
	inline bool Parse( const char* psz )
	{
		if ( psz == nullptr ) return false;
		if ( *psz == '{' ) ++psz;

		unsigned int	d1;
		unsigned int	d2;
		unsigned int	d3;
		unsigned int	d4[8];
		if ( std::sscanf( psz,
			"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			&d1, &d2, &d3,
			&d4[0], &d4[1], &d4[2], &d4[3],
			&d4[4], &d4[5], &d4[6], &d4[7] ) != 11 )
		{
			return false;
		}

		Data1 = static_cast<UInt32>(d1);
		Data2 = static_cast<UInt16>(d2);
		Data3 = static_cast<UInt16>(d3);
		for ( int i = 0; i < 8; ++i )
		{
			Data4[i] = static_cast<UInt8>(d4[i]);
		}

		return true;
	}

	/// Parses a string of the form {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
	/// into an id.
	inline bool Parse( const wchar_t* psz )
	{
		if ( psz == nullptr ) return false;
		if ( *psz == L'{' ) ++psz;

		unsigned int	d1;
		unsigned int	d2;
		unsigned int	d3;
		unsigned int	d4[8];
		if ( std::swscanf( psz,
			L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			&d1, &d2, &d3,
			&d4[0], &d4[1], &d4[2], &d4[3],
			&d4[4], &d4[5], &d4[6], &d4[7] ) != 11 )
		{
			return false;
		}

		Data1 = static_cast<UInt32>(d1);
		Data2 = static_cast<UInt16>(d2);
		Data3 = static_cast<UInt16>(d3);
		for ( int i = 0; i < 8; ++i )
		{
			Data4[i] = static_cast<UInt8>(d4[i]);
		}

		return true;
	}

	inline std::size_t Hash() const throw()
	{
		const HeIDHashHelper* p = reinterpret_cast<const HeIDHashHelper*>(this);
		#if SIZE_MAX == 0xffffffffffffffff
			const std::size_t	h1 = p->Data1 ^ p->Data2;
			const std::size_t	h2 = p->Data3 ^ p->Data4;
			return (h1 << 32) | h2;
		#else
			return static_cast<std::size_t>(p->Data1 ^ p->Data2 ^ p->Data3 ^ p->Data4);
		#endif
	}
} HE_PACK_STRUCT;
#include "hepoppack.h"

/// \typedef HeIID
/// \brief Same as IID or nsIID
typedef HeID HeIID;

/// \typedef HeCID
/// \brief Same as CLSID or nsCID
typedef HeID HeCID;

/// \def HE_DEFINE_STATIC_IID_ACCESSOR
/// \brief Standard interface accessor for IID
#define HE_DEFINE_STATIC_IID_ACCESSOR(_iid) \
	static const Helium::HeID& GetIID() throw() \
	{ \
		static const Helium::HeIID iid = _iid; \
		return iid; \
	}

/// \def HE_DEFINE_STATIC_CID_ACCESSOR
/// \brief Standard class accessor for CID
#define HE_DEFINE_STATIC_CID_ACCESSOR(_cid) \
	static const Helium::HeCID& GetCID() throw() \
	{ \
		static const Helium::HeCID cid = _cid; \
		return cid; \
	}

} /* namespace Helium */

#endif /* HEID_H */
