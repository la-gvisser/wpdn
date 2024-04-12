// ============================================================================
//	MsgText.cpp
//		implementation of the CMsgText class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "MsgText.h"
#include "RegExps.h"
#include "GlobalTags.h"


// ============================================================================
//		CMsgText
// ============================================================================

// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

bool
CMsgText::Find(
	const char*	pstrPattern,
	size_t&		ioOffset )
{
	size_t	nBegin( ioOffset );
	int		nOffset = CPeekStringA::Find( pstrPattern, nBegin );
	if ( nOffset < 0 ) return false;
	ioOffset = nOffset;
	return true;
}


// ----------------------------------------------------------------------------
//		FindLast
// ----------------------------------------------------------------------------

int
CMsgText::FindLast(
	const char*	pstrPattern ) const
{
	int nLast( -1 );
	int nNext( CPeekStringA::Find( pstrPattern, 0 ) );
	while ( nNext >= 0 ) {
		nLast = nNext;
		nNext = CPeekStringA::Find( pstrPattern, (nNext + 1) );
	}
	return nLast;
}


// ----------------------------------------------------------------------------
//		FindNoCase
// ----------------------------------------------------------------------------

bool
CMsgText::FindNoCase(
	const char*	pstrPattern,
	size_t&		ioOffset )
{
	if ( pstrPattern == NULL ) return false;
	if ( IsEmpty() ) return false;
	if ( ioOffset > static_cast<UInt32>( GetLength() ) ) return false;

	CPeekStringA	strPattern( pstrPattern );
	strPattern.MakeLower();
	const char*	pPattern( strPattern.c_str() );
	const char*	pNext( c_str() + ioOffset );
	// size_t		nPatternLength( strPattern.GetLength() );

	while ( *pNext != 0 ) {
		if ( tolower( *pNext ) == *pPattern ) {
			if ( /*_strnicmp( pstrPattern, pNext, nPatternLength ) == */ 0 ) {
				ptrdiff_t	nOffset( pNext - c_str() );
				if ( nOffset > kMaxUInt32 ) return false;
				ioOffset = nOffset;
				return true;
			}
		}
		pNext++;
	}
	return false;
}


// ----------------------------------------------------------------------------
//		GetAddress
// ----------------------------------------------------------------------------

CPeekString
CMsgText::GetAddress() const
{
#if (0)
	return CRegExps::GetRegExps().GetAddress( GetString() );
#else
	CPeekString strReturn;
	return strReturn;
#endif
}


// ----------------------------------------------------------------------------
//		GetAddressA
// ----------------------------------------------------------------------------

CPeekStringA
CMsgText::GetAddressA() const
{
#if (0)
	return CPeekStringA( CRegExps::GetRegExps().GetAddress( GetString() ) );
#else
	CPeekStringA strReturn;
	return strReturn;
#endif
}


// ----------------------------------------------------------------------------
//		GetDate
// ----------------------------------------------------------------------------

CPeekStringA
CMsgText::GetDate() const
{
#if (0)
	return CRegExps::GetRegExps().GetDate( GetStringA() );
#else
	CPeekStringA strReturn;
	return strReturn;
#endif
}

// ----------------------------------------------------------------------------
//		GetMailFrom
// ----------------------------------------------------------------------------

CPeekString
CMsgText::GetMailFrom() const
{
#if (0)
	return CRegExps::GetRegExps().GetMailFrom( GetStringA() );
#else
	CPeekString strReturn;
	return strReturn;
#endif
}


// ----------------------------------------------------------------------------
//		GetQDecoded
// ----------------------------------------------------------------------------

CPeekStringA
CMsgText::GetQDecoded() const
{
#if (0)
	return CRegExps::GetRegExps().GetQDecoded( GetStringA() );
#else
	CPeekStringA strReturn;
	return strReturn;
#endif
}


// ----------------------------------------------------------------------------
//		GetRecipientTo
// ----------------------------------------------------------------------------

bool
CMsgText::GetRecipientTo(
	CArrayStringA&	outRecipientList ) const
{
#if (0)
	return CRegExps::GetRegExps().GetRecipientTo( GetStringA(), outRecipientList );
#else
	return false;
#endif
}


#if USE_CDO_IMESSAGE
The CDO iMessage interface only returns one Receive field.
The mimepp interface returns them all.
This function is no longer used.
// ----------------------------------------------------------------------------
//		GetReceivedList
// ----------------------------------------------------------------------------

bool
CMsgText::GetReceivedList(
	CArrayStringA&	outReceivedList ) const
{
#if (0)
	BOOL				bResult = true;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExpAs.m_reReceivedA.Match( GetString(), &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GRL1';
	}
	catch ( ... ) {
		throw 'GRL2';
	}
	while ( bResult ) {
		CREMatchGroupA	mgGroup;
		mc.GetMatch( 0, &mgGroup );

		int			nLen = (int)(mgGroup.szEnd - mgGroup.szStart);
		CPeekStringA	str( mgGroup.szStart, nLen );
		str.Remove( '\t' );
		str.Replace( "\r\n", " " );
		str.Trim();

		int	nCount;
		int nMax = str.GetLength();
		do {
			nCount = str.Replace( "  ", " " );
			nMax--;
		} while ( (nCount > 0) && (nMax > 0) );
		
		outReceivedList.Add( str );
		try {
			bResult = g_RegExpAs.m_reReceivedA.Match( mgGroup.szEnd, &mc );
		}
		catch ( CException* pE ) {
			pE->Delete();
			throw 'GRL3';
		}
		catch ( ... ) {
			throw 'GRL4';
		}
	}
	return true;
#else
	(void) outReceivedList;
	return false;
#endif
}
#endif

// ----------------------------------------------------------------------------
//		GetBdatInfo
// ----------------------------------------------------------------------------

bool
CMsgText::GetBdatInfo(
	UInt32&	outSize,
	bool&	outLast ) const
{
#if (0)
	return CRegExps::GetRegExps().GetBdatInfo( GetStringA(), outSize, outLast );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		GetResponse
// ----------------------------------------------------------------------------

bool
CMsgText::GetResponse(
	UInt32&	outResponse ) const
{
#if (0)
	return CRegExps::GetRegExps().GetResponse( GetStringA(), outResponse );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		GetXexch50Info
// ----------------------------------------------------------------------------

bool
CMsgText::GetXexch50Info(
	UInt32&	outSize,
	int&	outCount ) const
{
#if (0)
	return CRegExps::GetRegExps().GetXexch50Info( GetStringA(), outSize, outCount );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		IsBdat
// ----------------------------------------------------------------------------

bool
CMsgText::IsBdat() const
{
#if (0)
	return CRegExps::GetRegExps().IsBdat( GetStringA() );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		IsCrLf
// ----------------------------------------------------------------------------

bool
CMsgText::IsCrLf(
	ptrdiff_t	inOffset ) const
{
#if (TOVERIFY)
	if ( inOffset < 0 ) return false;
	if ( inOffset > (static_cast<ptrdiff_t>( GetLength() ) - 1) ) return false;
	return ((operator[](inOffset) == '\r') && (operator[](inOffset + 1) == '\n'));
#endif // TOVERIFY
}


// ----------------------------------------------------------------------------
//		IsData
// ----------------------------------------------------------------------------

bool
CMsgText::IsData() const
{
#if (0)
	return CRegExps::GetRegExps().IsData( GetStringA() );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		IsEndOfMsg
// ----------------------------------------------------------------------------

bool
CMsgText::IsEndOfMsg() const
{
	//CPeekString	str( GetString() );
	//return (str.Right( 5 ) == "\r\n.\r\n" );

	const char*	pResult( strstr( c_str(), "\r\n.\r\n" ) );
	if ( pResult != NULL ) {
		return true;
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsError
// ----------------------------------------------------------------------------

bool
CMsgText::IsError() const
{
	size_t	nLength = GetLength();
	if ( nLength < 6 ) return false;

	int	nResult = 0; // _strnicmp( c_str(), "-ERR", 4 );
	if ( nResult == 0 ) {
		return IsCrLf( nLength - 2 );
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsMailFrom
// ----------------------------------------------------------------------------

bool
CMsgText::IsMailFrom() const
{
	const char*	pString = c_str();
	if ( pString == NULL ) return false;

	size_t	nLength = GetLength();
	if ( nLength < 12 ) return false;

	int	nResult = 0; // _strnicmp( pString, "MAIL FROM:", 10 );
	if ( nResult == 0 ) {
		return IsCrLf( nLength - 2 );
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsPlusOK
// ----------------------------------------------------------------------------

bool
CMsgText::IsPlusOK() const
{
#if (0)
	return CRegExps::GetRegExps().IsPlusOK( GetStringA() );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		IsQuit
// ----------------------------------------------------------------------------

bool
CMsgText::IsQuit() const
{
#if (0)
	return CRegExps::GetRegExps().IsQuit( GetStringA() );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		IsRcptTo
// ----------------------------------------------------------------------------

bool
CMsgText::IsRcptTo() const
{
	const char*	pString = c_str();
	if ( pString == NULL ) return false;

	size_t	nLength = GetLength();
	if ( nLength < 10 ) return false;

	int	nResult = 0; // _strnicmp( pString, "RCPT TO:", 8 );
	if ( nResult == 0 ) {
		return IsCrLf( nLength - 2 );
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsReset
// ----------------------------------------------------------------------------

bool
CMsgText::IsReset() const
{
#if (0)
	return CRegExps::GetRegExps().IsReset( GetStringA() );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		IsRetrieve
// ----------------------------------------------------------------------------

bool
CMsgText::IsRetrieve() const
{
	size_t	nLength = GetLength();
	if ( nLength < 7 ) return false;

	int	nResult = 0; // _strnicmp( c_str(), "RETR ", 5 );
	if ( nResult == 0 ) {
		return IsCrLf( nLength - 2 );
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsXexch50
// ----------------------------------------------------------------------------

bool
CMsgText::IsXexch50() const
{
#if (0)
	return CRegExps::GetRegExps().IsXexch50( GetStringA() );
#else
	return false;
#endif
}


// ----------------------------------------------------------------------------
//		Set
// ----------------------------------------------------------------------------

void
CMsgText::Set(
	CByteVectRef	inData )
{
	if ( inData.IsEmpty() ) {
		clear();
	}
	else {
		const size_t	nCount( inData.GetCount() );
		const char*		pData( reinterpret_cast<const char*>( inData.GetData( nCount ) ) );
		ASSERT( pData );
		if ( pData ) assign( pData, nCount );
	}
}


// ----------------------------------------------------------------------------
//		Split
// ----------------------------------------------------------------------------
// Like Tokenize, but returns the tokens as part of the string.

CPeekStringA
CMsgText::Split(
	const char*	inTokens,
	size_t&		ioOffset ) const
{
#if (TOVERIFY)
	if ( inTokens == NULL ) {
		return( Tags::kxEmpty.GetA() );
	}

	if ( ioOffset != std::string::npos ) {
		const size_t	nTokensLength = strlen( inTokens );
		const char*		pszPlace( c_str() + ioOffset );
		const char*		pszEnd( c_str() + GetLength() );	// end()?
		if ( pszPlace < pszEnd ) {
			const char*	pszFind( strstr( pszPlace, inTokens ) );
			if ( pszFind ) {
				pszFind += nTokensLength;

				ptrdiff_t	nBegin( ioOffset );
				ptrdiff_t	nLength( pszFind - pszPlace );
				ioOffset += nLength;
				return( Mid( nBegin, nLength ) );
			}
		}

		// return empty string, done tokenizing
		ioOffset = std::string::npos;
	}

#endif // TOVERIFY
	return( Tags::kxEmpty.GetA() );
}


// ============================================================================
//		CMsgTextList
// ============================================================================

// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

size_t
CMsgTextList::Parse(
	const CMsgText&	inMsgText )
{
	size_t	nBodyLength( inMsgText.GetLength() );
	if ( nBodyLength == 0 ) {
		return 0;
	}

	size_t	nCurPos( 0 );
	while ( (nCurPos < nBodyLength) && (nCurPos != std::string::npos ) ) {
		CPeekStringA	strSplit( inMsgText.Split( "\r\n", nCurPos ) );
		if ( !strSplit.IsEmpty() ) {
			push_back( strSplit );
		}
	}
	return size();
}
