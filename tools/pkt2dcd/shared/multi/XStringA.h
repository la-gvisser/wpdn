// ============================================================================
//	XStringA.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#ifndef XSTRINGA_H
#define XSTRINGA_H

// required on the mac, if this doesn't compile on windows
// use ifdef TARGET_MAC_OS
#include <string.h>
#include "XObject.h"
 
#pragma warning(disable:4097)	// warning C4097: typedef-name 'XString8' used as synonym for class-name 


// {555CCB9C-C31B-4bb6-AE12-AA2867BC588A}
X_DEFINE_GUID(TStringAID, 
0x555ccb9c, 0xc31b, 0x4bb6, 0xae, 0x12, 0xaa, 0x28, 0x67, 0xbc, 0x58, 0x8a);

template <class Y>
class DECLSPEC TStringA :
	virtual public XObject,
	virtual public XCompare,
	virtual public XCalcHash
{
// put the template members up front for clarity
protected:
	Y		m_nLength;
	Y		m_nCapacity;

public:
				TStringA();
				TStringA( CHAR inChar );
				TStringA( const CHAR* inStr );
				TStringA( const CHAR* inStr, Y inLength );
				TStringA( const TStringA& inStr );
#if TARGET_OS_MAC
				TStringA( ConstStr255Param inPStr );
#endif

#if TARGET_OS_WIN32
				TStringA( int inResourceID );
	void		LoadResource( int inResourceID );
#endif
				~TStringA();

	X_BEGIN_ID_MAP(TStringA)
		X_ID_ENTRY(TStringAID)
		X_CLASS_ENTRY(XCompare)
		X_CLASS_ENTRY(XCalcHash)
		X_CLASS_ENTRY(XObject)
	X_END_ID_MAP

	// Assignment.
	TStringA&	operator=( CHAR inChar )			{ return Assign( inChar ); }
	TStringA&	operator=( const CHAR* inStr )		{ return Assign( inStr );  }
	TStringA&	operator=( const TStringA& inStr )	{ return Assign( inStr );  }
#if TARGET_OS_MAC
	TStringA&	operator=( ConstStr255Param inPStr ) { return Assign( inPStr ); }
	bool		CopyToPStr( UInt8* outPStr );
#endif
		
	// Size.
	Y			GetLength() const					{ return m_nLength; }
	void		Empty();
	bool		IsEmpty() const						{ return m_nLength == 0; }
	
	// Access.
	operator const CHAR*() const;

	// Don't do this
	// - it causes "similar conversion error" because of the previous line
	//const TCHAR& operator[]( Y inIndex ) const;
	
	// Append.
	TStringA&	operator+=( CHAR inChar )			{ return Append( inChar ); }
	TStringA&	operator+=( const CHAR* inStr )	{ return Append( inStr );  }
	TStringA&	operator+=( const TStringA& inStr )	{ return Append( inStr );  }
	
	// Concatenate.
	friend TStringA	operator+( const TStringA& lhs, const TStringA& rhs )
		{ TStringA result = lhs; result += rhs; return result; }
	friend TStringA	operator+( const CHAR* lhs, const TStringA& rhs )
		{ TStringA result = lhs; result += rhs; return result; }
	friend TStringA	operator+( CHAR lhs, const TStringA& rhs )
		{ TStringA result(lhs); result += rhs; return result; }
	friend TStringA	operator+( const TStringA& lhs, const CHAR* rhs )
		{ TStringA result = lhs; result += rhs; return result; }
	friend TStringA	operator+( const TStringA& lhs, CHAR rhs )
		{ TStringA result = lhs; result += rhs; return result; }
	
	// Comparison.	
	friend bool		operator==( CHAR lhs, const TStringA& rhs )
		{ return rhs.GetLength() == 1 && rhs[0] == lhs; }
	friend bool		operator==( const CHAR* lhs, const TStringA& rhs )
		{ return rhs.Compare( lhs ) == 0; }
	friend bool		operator==( const TStringA& lhs, CHAR rhs )
		{ return lhs.GetLength() == 1 && lhs[0] == rhs; }
	friend bool		operator==( const TStringA& lhs, const CHAR* rhs )
		{ return lhs.Compare( rhs ) == 0; }
	friend bool		operator==( const TStringA& lhs, const TStringA& rhs )
		{ return lhs.Compare( rhs ) == 0; }
		
	friend bool		operator!=( CHAR lhs, const TStringA& rhs )
		{ return rhs.GetLength() != 1 || rhs[0] != lhs; }
	friend bool		operator!=( const CHAR* lhs, const TStringA& rhs )
		{ return rhs.Compare( lhs ) != 0; }
	friend bool		operator!=( const TStringA& lhs, CHAR rhs )
		{ return lhs.GetLength() != 1 || lhs[0] != rhs; }
	friend bool		operator!=( const TStringA& lhs, const CHAR* rhs )
		{ return lhs.Compare( rhs ) != 0; }
	friend bool		operator!=( const TStringA& lhs, const TStringA& rhs )
		{ return lhs.Compare( rhs ) != 0; }
		
	friend 	bool	operator<( const TStringA& lhs, const TStringA& rhs )
		{ return lhs.Compare( rhs ) < 0; }
	friend 	bool	operator<=( const TStringA& lhs, const TStringA& rhs )
		{ return lhs.Compare( rhs ) <= 0; }
	friend 	bool	operator>( const TStringA& lhs, const TStringA& rhs )
		{ return lhs.Compare( rhs ) > 0; }
	friend 	bool	operator>=( const TStringA& lhs, const TStringA& rhs )
		{ return lhs.Compare( rhs ) >= 0; }
	
	virtual SInt16	Compare( const CHAR* rhs ) const;
	virtual SInt16 Compare( const XCompare& rhs, int iFlag = 0) const;
	virtual X_RESULT CalcHash(int iHashSize) const;

	// Misc.
	TStringA			Substring( Y inIndex, SInt32 inCount ) const;
	TStringA			Substring( Y inIndex ) const;
	
	// Find.
	bool			Find( const TStringA& inString, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), 0, false, outLocation ); }
	bool			Find( const TStringA& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), inStartPos, false, outLocation ); }
	bool			Find( const CHAR* inString, Y& outLocation ) const
							{ return DoFind( inString, strlen( inString ), 0, false, outLocation ); }
	bool			Find( const CHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, strlen( inString ), inStartPos, false, outLocation ); }
	
	bool			FindIgnoringCase( const TStringA& inString, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), 0, true, outLocation ); }
	bool			FindIgnoringCase( const TStringA& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, inString.GetLength(), inStartPos, true, outLocation ); }
	bool			FindIgnoringCase( const CHAR* inString, Y& outLocation ) const
							{ return DoFind( inString, strlen( inString ), 0, true, outLocation ); }
	bool			FindIgnoringCase( const CHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFind( inString, strlen( inString ), inStartPos, true, outLocation ); }
	
	bool			ReverseFind( const TStringA& inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), 0xFFFFFFFF, false, outLocation ); }
	bool			ReverseFind( const TStringA& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), inStartPos, false, outLocation ); }
	bool			ReverseFind( const CHAR* inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, strlen( inString ), 0xFFFFFFFF, false, outLocation ); }
	bool			ReverseFind( const CHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, strlen( inString ), inStartPos, false, outLocation ); }
	
	bool			ReverseFindIgnoringCase( const TStringA& inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), 0xFFFFFFFF, true, outLocation ); }
	bool			ReverseFindIgnoringCase( const TStringA& inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, inString.GetLength(), inStartPos, true, outLocation ); }
	bool			ReverseFindIgnoringCase( const CHAR* inString, Y& outLocation ) const
							{ return DoFindPrevious( inString, strlen( inString ), 0xFFFFFFFF, true, outLocation ); }
	bool			ReverseFindIgnoringCase( const CHAR* inString, Y inStartPos, Y& outLocation ) const
							{ return DoFindPrevious( inString, strlen( inString ), inStartPos, true, outLocation ); }
	
// ::vsnprintf() does not work on Mac OS X (either that, or Metrowerk's va_args() implementation
// is incompatible with the system ::vsnprintf() call)
#if TARGET_OS_WIN32
	// Formatting.
	int			Format( const CHAR* inFormat, ... );
#endif // TARGET_OS_WIN32

protected:
	CHAR*			m_pData;
	static CHAR		m_sEmptyString[1];

	TStringA&	Assign( CHAR inChar );
	TStringA&	Assign( const CHAR* inStr );
	TStringA&	Assign( const CHAR* inStr, Y inLength );
	TStringA&	Assign( const TStringA& inStr );
#if TARGET_OS_MAC
	TStringA&	Assign( ConstStr255Param inPStr );
#endif
	bool		DoFind( const CHAR* inString, Y inStringLength,
						Y inStartPosition, bool inIgnoreCase, Y& outFoundLocation ) const;
	bool		DoFindPrevious( const CHAR* inString, Y inStringLength,
						Y inStartPosition, bool inIgnoreCase, Y& outFoundLocation ) const;
	
	TStringA&	Append( CHAR inChar );
	TStringA&	Append( const CHAR* inStr );
	TStringA&	Append( const TStringA& inStr );
	
	bool			AdjustCapacity( Y inCharsNeeded );
	virtual Y		GetBlockSize( Y inCharsNeeded );
};

#include "XStringA.cpp"

typedef DECLSPEC TStringA<UInt32> XStringA;
typedef DECLSPEC TStringA<UInt8>  XStringA8;
typedef DECLSPEC TStringA<UInt16> XStringA16;
typedef DECLSPEC TStringA<UInt32> XStringA32;

#endif /* _XSTRING_H_ */
