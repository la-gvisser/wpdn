// ============================================================================
//	XString.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#pragma once

// required on the mac, if this doesn't compile on windows
// use ifdef TARGET_MAC_OS
#include <string.h>
#include "XObject.h"

#pragma warning(disable:4097)	// warning C4097: typedef-name 'XString8' used as synonym for class-name


// {555CCB9C-C31B-4bb6-AE12-AA2867BC588A}
X_DEFINE_GUID(XStringID,
0x555ccb9c, 0xc31b, 0x4bb6, 0xae, 0x12, 0xaa, 0x28, 0x67, 0xbc, 0x58, 0x8a);

template <class Y>
class DECLSPEC TString :
	virtual public XObject,
	virtual public XCompare,
	virtual public XCalcHash
{
// put the template members up front for clarity
protected:
	Y		m_nLength;
	Y		m_nCapacity;

public:
				TString();
				TString( TCHAR inChar );
				TString( const TCHAR* inStr );
				TString( const TCHAR* inStr, Y inLength );
				TString( const TString& inStr );
#if TARGET_OS_MAC
				TString( ConstStr255Param inPStr );
#endif

#if TARGET_OS_WIN32
				TString( int inResourceID );
	void		LoadResource( int inResourceID );
#endif
				~TString();

	X_BEGIN_ID_MAP(TString)
		X_ID_ENTRY(XStringID)
		X_CLASS_ENTRY(XCompare)
		X_CLASS_ENTRY(XCalcHash)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	// Assignment.
	TString&	operator=( TCHAR inChar )			{ return Assign( inChar ); }
	TString&	operator=( const TCHAR* inStr )		{ return Assign( inStr );  }
	TString&	operator=( const TString& inStr )	{ return Assign( inStr );  }
#if TARGET_OS_MAC
	TString&	operator=( ConstStr255Param inPStr ) { return Assign( inPStr ); }
	bool		CopyToPStr( UInt8* outPStr );
#endif

	// Size.
	Y			GetLength() const					{ return m_nLength; }
	void		Empty();
	bool		IsEmpty() const						{ return m_nLength == 0; }

	// Access.
	operator const TCHAR*() const;

	// Don't do this
	// - it causes "similar conversion error" because of the previous line
	//const TCHAR& operator[]( Y inIndex ) const;

	// Append.
	TString&	operator+=( TCHAR inChar )			{ return Append( inChar ); }
	TString&	operator+=( const TCHAR* inStr )	{ return Append( inStr );  }
	TString&	operator+=( const TString& inStr )	{ return Append( inStr );  }

	// Concatenate.
	friend TString	operator+( const TString& lhs, const TString& rhs )
		{ TString result = lhs; result += rhs; return result; }
	friend TString	operator+( const TCHAR* lhs, const TString& rhs )
		{ TString result = lhs; result += rhs; return result; }
	friend TString	operator+( TCHAR lhs, const TString& rhs )
		{ TString result(lhs); result += rhs; return result; }
	friend TString	operator+( const TString& lhs, const TCHAR* rhs )
		{ TString result = lhs; result += rhs; return result; }
	friend TString	operator+( const TString& lhs, TCHAR rhs )
		{ TString result = lhs; result += rhs; return result; }

	// Comparison.
	friend bool		operator==( TCHAR lhs, const TString& rhs )
		{ return rhs.GetLength() == 1 && rhs[0] == lhs; }
	friend bool		operator==( const TCHAR* lhs, const TString& rhs )
		{ return rhs.Compare( lhs ) == 0; }
	friend bool		operator==( const TString& lhs, TCHAR rhs )
		{ return lhs.GetLength() == 1 && lhs[0] == rhs; }
	friend bool		operator==( const TString& lhs, const TCHAR* rhs )
		{ return lhs.Compare( rhs ) == 0; }
	friend bool		operator==( const TString& lhs, const TString& rhs )
		{ return lhs.Compare( rhs ) == 0; }

	friend bool		operator!=( TCHAR lhs, const TString& rhs )
		{ return rhs.GetLength() != 1 || rhs[0] != lhs; }
	friend bool		operator!=( const TCHAR* lhs, const TString& rhs )
		{ return rhs.Compare( lhs ) != 0; }
	friend bool		operator!=( const TString& lhs, TCHAR rhs )
		{ return lhs.GetLength() != 1 || lhs[0] != rhs; }
	friend bool		operator!=( const TString& lhs, const TCHAR* rhs )
		{ return lhs.Compare( rhs ) != 0; }
	friend bool		operator!=( const TString& lhs, const TString& rhs )
		{ return lhs.Compare( rhs ) != 0; }

	friend 	bool	operator<( const TString& lhs, const TString& rhs )
		{ return lhs.Compare( rhs ) < 0; }
	friend 	bool	operator<=( const TString& lhs, const TString& rhs )
		{ return lhs.Compare( rhs ) <= 0; }
	friend 	bool	operator>( const TString& lhs, const TString& rhs )
		{ return lhs.Compare( rhs ) > 0; }
	friend 	bool	operator>=( const TString& lhs, const TString& rhs )
		{ return lhs.Compare( rhs ) >= 0; }

	virtual SInt16	Compare( const TCHAR* rhs ) const;
	virtual SInt16 Compare( const XCompare& rhs, int iFlag = 0) const;
	virtual int CalcHash(int iHashSize);

	// Misc.
	TString			Substring( Y inIndex, SInt32 inCount ) const;
	TString			Substring( Y inIndex ) const;

	// Find.
	bool			Find( const TString& inString, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), 0, false, outLocation ); }
	bool			Find( const TString& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), inStartPos, false, outLocation ); }
	bool			Find( const TCHAR* inString, Y& outLocation ) const
							{ return DoFind( inString, _tcslen( inString ), 0, false, outLocation ); }
	bool			Find( const TCHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, _tcslen( inString ), inStartPos, false, outLocation ); }

	bool			FindIgnoringCase( const TString& inString, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), 0, true, outLocation ); }
	bool			FindIgnoringCase( const TString& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), inStartPos, true, outLocation ); }
	bool			FindIgnoringCase( const TCHAR* inString, Y& outLocation ) const
							{ return DoFind( inString, _tcslen( inString ), 0, true, outLocation ); }
	bool			FindIgnoringCase( const TCHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, _tcslen( inString ), inStartPos, true, outLocation ); }

	bool			ReverseFind( const TString& inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), 0xFFFFFFFF, false, outLocation ); }
	bool			ReverseFind( const TString& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), inStartPos, false, outLocation ); }
	bool			ReverseFind( const TCHAR* inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, _tcslen( inString ), 0xFFFFFFFF, false, outLocation ); }
	bool			ReverseFind( const TCHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, _tcslen( inString ), inStartPos, false, outLocation ); }

	bool			ReverseFindIgnoringCase( const TString& inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), 0xFFFFFFFF, true, outLocation ); }
	bool			ReverseFindIgnoringCase( const TString& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), inStartPos, true, outLocation ); }
	bool			ReverseFindIgnoringCase( const TCHAR* inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, _tcslen( inString ), 0xFFFFFFFF, true, outLocation ); }
	bool			ReverseFindIgnoringCase( const TCHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, _tcslen( inString ), inStartPos, true, outLocation ); }

// ::vsnprintf() does not work on Mac OS X (either that, or Metrowerk's va_args() implementation
// is incompatible with the system ::vsnprintf() call)
#if TARGET_OS_WIN32
	// Formatting.
	int			Format( const TCHAR* inFormat, ... );
#endif // TARGET_OS_WIN32

protected:
	TCHAR*			m_pData;
	static TCHAR	m_sEmptyString[1];

	TString&	Assign( TCHAR inChar );
	TString&	Assign( const TCHAR* inStr );
	TString&	Assign( const TCHAR* inStr, Y inLength );
	TString&	Assign( const TString& inStr );
#if TARGET_OS_MAC
	TString&	Assign( ConstStr255Param inPStr );
#endif
	bool		DoFind( const TCHAR* inString, Y inStringLength,
						Y inStartPosition, bool inIgnoreCase, Y& outFoundLocation ) const;
	bool		DoFindPrevious( const TCHAR* inString, Y inStringLength,
						Y inStartPosition, bool inIgnoreCase, Y& outFoundLocation ) const;

	TString&	Append( TCHAR inChar );
	TString&	Append( const TCHAR* inStr );
	TString&	Append( const TString& inStr );

	bool			AdjustCapacity( Y inCharsNeeded );
	virtual Y		GetBlockSize( Y inCharsNeeded );
};

#include "XString.cpp"

typedef DECLSPEC TString<UInt32> XString;
typedef DECLSPEC TString<UInt8>  XString8;
typedef DECLSPEC TString<UInt16> XString16;
typedef DECLSPEC TString<UInt32> XString32;
