// ============================================================================
//	RegExps.cpp
//		implementation of the CRegExps class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "RegExps.h"

//typedef	ATL::CAtlRegExp<ATL::CAtlRECharTraitsW>			CRegExpW;
typedef ATL::CAtlREMatchContext<ATL::CAtlRECharTraitsW>	CREMatchContextW;
typedef CREMatchContextW::MatchGroup					CREMatchGroupW;

//typedef	ATL::CAtlRegExp<ATL::CAtlRECharTraitsA>			CRegExpA;
typedef ATL::CAtlREMatchContext<ATL::CAtlRECharTraitsA>	CREMatchContextA;
typedef CREMatchContextA::MatchGroup					CREMatchGroupA;

// Both Regular expressions will match "..".
// Could add a test for ".." and fail the address.

CRegExps	g_RegExps;


// ============================================================================
//		CRegExps
// ============================================================================

CRegExps::CRegExps()
{
	// RFC 2822
	// atext : ALPHA DIGIT ! # $ % & ' * + - / = ? ^ _ ` { | } ~
	// ^[ atext \.]+@([ atext \.]+\.)+[A-Za-z]{2,4}$
	//
	// a-zA-Z0-9\\!#\\$%&'\\*\\+\\-/=\\?\\^_`{\\|}~\\.À-ÖØ-öø-ÿ

	REParseError		peResult;

	const CPeekString		strAddressW(
		L"[a-zA-Z0-9\\!#\\$%&'\\*\\+\\-/=\\?\\^_`{\\|}~\\.À-ÖØ-öø-ÿ]+@([a-zA-Z0-9\\!#\\$%&'\\*\\+\\-/=\\?\\^_`{\\|}~\\.À-ÖØ-öø-ÿ]+\\.)+[a-zA-Z0-9][a-zA-Z0-9][a-zA-Z0-9]?[a-zA-Z0-9]?" );
	// ^([atext \.]+\.)+[A-Za-z]{2,4}$
	const CPeekString		strDomainW(
		L"^([a-zA-Z0-9\\!#\\$%&'\\*\\+\\-/=\\?\\^_`{\\|}~\\.À-ÖØ-öø-ÿ]+\\.)+[a-zA-Z][a-zA-Z][a-zA-Z]?[a-zA-Z]?" );
	const CPeekString		strMailFromW( L"(m|M)(a|A)(i|I)(l|L) (f|F)(r|R)(o|O)(m|M):\\b*<{.*?}>\\n" );

	const CPeekStringA		strAddressA(
		"[a-zA-Z0-9\\!#\\$%&'\\*\\+\\-/=\\?\\^_`{\\|}~\\.À-ÖØ-öø-ÿ]+@([a-zA-Z0-9\\!#\\$%&'\\*\\+\\-/=\\?\\^_`{\\|}~\\.À-ÖØ-öø-ÿ]+\\.)+[a-zA-Z0-9][a-zA-Z0-9][a-zA-Z0-9]?[a-zA-Z0-9]?" );
	const CPeekStringA		strBdatA( "^(b|B)(d|D)(a|A)(t|T).*\r\n$" );
	const CPeekStringA		strBdatInfoA( "^(b|B)(d|D)(a|A)(t|T) {[0-9]+} {(l|L)(a|A)(s|S)(t|T)}.*\r\n$" );
	const CPeekStringA		strDataA( "^(d|D)(a|A)(t|T)(a|A).*\r\n$" );
	const CPeekStringA		strDateA( "(d|D)(a|A)(t|T)(e|E):{.*?}\\n" );
	const CPeekStringA		strMailFromA( "(m|M)(a|A)(i|I)(l|L) (f|F)(r|R)(o|O)(m|M):\\b*<{.*?}>" );
	const CPeekStringA		strPlusOKA( "^\\+(o|O)(k|K).*\r\n$" );
	const CPeekStringA		strQEncodedA( "^=\\?.+\\?[qQ]\\?{.+}\\?=$" );
	const CPeekStringA		strQuitA( "^(q|Q)(u|U)(i|I)(t|T).*\r\n$" );
	const CPeekStringA		strResetA( "^(r|R)(s|S)(e|E)(t|T).*\r\n$" );
	const CPeekStringA		strRecipientToA( "(r|R)(c|C)(p|P)(t|T) (t|T)(o|O):\\b*<{.*?}>" );
	const CPeekStringA		strResponseA( "^{[0-9]+}.*\r\n?" );
	const CPeekStringA		strXexch50A( "^(x|X)(e|E)(x|X)(c|C)(h|H)50.*\r\n$" );
	const CPeekStringA		strXexch50InfoA( "^(x|X)(e|E)(x|X)(c|C)(h|H)50 {[0-9]+} {[a-zA-Z0-9]+}.*\r\n$" );

#if USE_CDO_IMESSAGE
	// *? - non-greedy version of *.
	const CPeekStringA		strReceivedA = "{(r|R)(e|E)(c|C)(e|E)(i|I)(v|V)(e|E)(d|D):.*?}{\\n}{[a-zA-Z0-9\\-]+:}";
#endif

	peResult = m_reAddressW.Parse( strAddressW );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reDomainW.Parse( strDomainW );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reMailFromW.Parse( strMailFromW );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;


	peResult = m_reAddressA.Parse( strAddressA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reBdatA.Parse( strBdatA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reBdatInfoA.Parse( strBdatInfoA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reDataA.Parse( strDataA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reDateA.Parse( strDateA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reMailFromA.Parse( strMailFromA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_rePlusOKA.Parse( strPlusOKA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reQEncodedA.Parse( strQEncodedA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reQuitA.Parse( strQuitA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reRecipientToA.Parse( strRecipientToA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reResetA.Parse( strResetA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reResponseA.Parse( strResponseA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reXexch50A.Parse( strXexch50A );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

	peResult = m_reXexch50InfoA.Parse( strXexch50InfoA );
	ASSERT( peResult == REPARSE_ERROR_OK );
	//if ( peResult != REPARSE_ERROR_OK ) return;

#if USE_CDO_IMESSAGE
	peResult = m_reReceivedA.Parse( strReceivedA );
	ASSERT( peResult == REPARSE_ERROR_OK 
	//if ( peResult != REPARSE_ERROR_OK ) return;
#endif
}

// ----------------------------------------------------------------------------
//		GetRegExps
// ----------------------------------------------------------------------------

CRegExps&
CRegExps::GetRegExps()
{
	return g_RegExps;
}


// ----------------------------------------------------------------------------
//		GetAddress
// ----------------------------------------------------------------------------

CPeekString
CRegExps::GetAddress(
	const CPeekString&	inString )
{
	// Email address may contain international characters.
	// Must use an Unicode regular express.
	// ASCII RegEx will throw an exception.
	CPeekString	strAddress;

	CREMatchContextW	mc;
	BOOL				bResult;
	try {
		bResult = g_RegExps.m_reAddressW.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'MTG1';
	}
	catch ( ... ) {
		throw 'MTG2';
	}

	if ( bResult ) {
		ptrdiff_t	nAddressLen( mc.m_Match.szEnd - mc.m_Match.szStart );
		if ( nAddressLen < kMaxUInt32 ) {
			strAddress.assign( mc.m_Match.szStart, nAddressLen );
		}
	}
	return strAddress;
}


// ----------------------------------------------------------------------------
//		GetBdatInfo
// ----------------------------------------------------------------------------

bool
CRegExps::GetBdatInfo(
	const CPeekStringA&	inString,
	UInt32&				outSize,
	bool&				outLast )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reBdatInfoA.Match( inString, &mc );
		while ( bResult ) {
			CREMatchGroupA	mgGroup;
			{
				mc.GetMatch( 0, &mgGroup );
				int				nLen( static_cast<int>( mgGroup.szEnd - mgGroup.szStart ) );
				CPeekStringA	str( mgGroup.szStart, nLen );
				str.Trim();
				SInt32	nValue;
				StringToInteger( str, nValue );
				if ( (nValue > 0) && (nValue <= 1000000000) ) {	// less than 1,000,000,000 (1 billion)
					outSize = static_cast<UInt32>( nValue );
				}
			}
			{
				outLast = false;
				mc.GetMatch( 1, &mgGroup );
				int				nLen( static_cast<int>( mgGroup.szEnd - mgGroup.szStart ) );
				CPeekStringA	str( mgGroup.szStart, nLen );
				str.Trim();
				outLast = (str.CompareNoCase( "LAST" ) == 0);
				break;
			}
			bResult = g_RegExps.m_reBdatInfoA.Match( mgGroup.szEnd, &mc );
		}
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GBS1';
	}
	catch ( ... ) {
		throw 'GBS2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		GetDate
// ----------------------------------------------------------------------------

CPeekStringA
CRegExps::GetDate(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;
	CPeekStringA		strDate;

	try {
		bResult = g_RegExps.m_reDateA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GDT1';
	}
	catch ( ... ) {
		throw 'GDT2';
	}

	if ( bResult ) {
		CREMatchGroupA	mgGroup;
		mc.GetMatch( 0, &mgGroup );

		ptrdiff_t		nLength( mgGroup.szEnd - mgGroup.szStart );
		strDate.assign( mgGroup.szStart, nLength );
		strDate.Trim();
	}

	return strDate;
}


// ----------------------------------------------------------------------------
//		GetMailFrom
// ----------------------------------------------------------------------------

CPeekString
CRegExps::GetMailFrom(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextW	mc;
	CPeekString				strMailFrom;
	CPeekString				strW( inString );

	try {
		bResult = g_RegExps.m_reMailFromW.Match( strW, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GMF1';
	}
	catch ( ... ) {
		throw 'GMF2';
	}

	while ( bResult ) {
		CREMatchGroupW	mgGroup;
		mc.GetMatch( 0, &mgGroup );

		int			nLen( static_cast<int>( mgGroup.szEnd - mgGroup.szStart ) );
		CPeekString	str( mgGroup.szStart, nLen );
		str.Trim();
		str.Remove( '\t' );
		str.MakeLower();

		CPeekString	strAddress( GetAddress( str ) );
		strMailFrom = strAddress;
		try {
			bResult = g_RegExps.m_reMailFromW.Match( mgGroup.szEnd, &mc );
		}
		catch ( CException* pE ) {
			pE->Delete();
			throw 'GMF3';
		}
		catch ( ... ) {
			throw 'GMF4';
		}
	}
	return strMailFrom;
}


// ----------------------------------------------------------------------------
//		GetRecipientTo
// ----------------------------------------------------------------------------

bool
CRegExps::GetRecipientTo(
	const CPeekStringA&	inString,
	CArrayStringA&		outRecipientList )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reRecipientToA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GRT1';
	}
	catch ( ... ) {
		throw 'GRT2';
	}

	if ( !bResult ) return false;
	while ( bResult ) {
		CREMatchGroupA	mgGroup;
		mc.GetMatch( 0, &mgGroup );

		int			nLen( static_cast<int>( mgGroup.szEnd - mgGroup.szStart ) );
		CPeekStringA	str( mgGroup.szStart, nLen );
		str.Trim();
		str.Remove( '\t' );
		str.MakeLower();
		outRecipientList.Add( str );
		try {
			bResult = g_RegExps.m_reRecipientToA.Match( mgGroup.szEnd, &mc );
		}
		catch ( CException* pE ) {
			pE->Delete();
			throw 'GRT3';
		}
		catch ( ... ) {
			throw 'GRT4';
		}
	}
	return true;
}


// ----------------------------------------------------------------------------
//		GetQDecoded
// ----------------------------------------------------------------------------

CPeekStringA
CRegExps::GetQDecoded(
	const CPeekStringA&	inString )
{
	CPeekStringA		strDecoded( inString );
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reQEncodedA.Match( inString, &mc );
		if ( bResult == FALSE ) return strDecoded;
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GQD1';
	}
	catch ( ... ) {
		throw 'GQD2';
	}

	CREMatchGroupA	mgGroup = { 0 };
	mc.GetMatch( 0, &mgGroup );

#if (TOVERIFY)
	size_t	nLength( mgGroup.szEnd - mgGroup.szStart );
	strDecoded.assign( mgGroup.szStart, nLength );
	strDecoded.Trim();

	{
		std::string::const_iterator	pNext( strDecoded.begin() );
		std::string::iterator		pStr( strDecoded.begin() );

		while ( *pNext != 0 ) {
			if ( *pNext == '=' ) {
				pNext++;
				int		c;
				char	szHex[4];
				szHex[0] = *pNext++;
				szHex[1] = *pNext;
				szHex[2] = 0;
				sscanf_s( szHex, "%x", &c );
				*pStr = static_cast<char>( c );
			}
			else {
				*pStr = *pNext;
			}
			pNext++;
			pStr++;
		}
		strDecoded.resize( pStr - strDecoded.begin() );
	}
#endif

	return strDecoded;
}


// ----------------------------------------------------------------------------
//		GetResponse
// ----------------------------------------------------------------------------

bool
CRegExps::GetResponse(
	const CPeekStringA&	inString,
	UInt32&				outResponse )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reResponseA.Match( inString, &mc );
		while ( bResult ) {
			CREMatchGroupA	mgGroup;
			mc.GetMatch( 0, &mgGroup );

			int			nLen( static_cast<int>( mgGroup.szEnd - mgGroup.szStart ) );
			CPeekStringA	str( mgGroup.szStart, nLen );
			str.Trim();
			SInt32	nValue;
			StringToInteger( str, nValue );
			if ( (nValue > 0) && (nValue < 1000) ) {
				outResponse = static_cast<UInt32>( nValue );
				break;
			}
			bResult = g_RegExps.m_reResponseA.Match( mgGroup.szEnd, &mc );
		}
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISB1';
	}
	catch ( ... ) {
		throw 'ISB2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		GetXexch50Info
// ----------------------------------------------------------------------------

bool
CRegExps::GetXexch50Info(
	const CPeekStringA&	inString,
	UInt32&				outSize,
	int&				/*outCount*/ )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reXexch50InfoA.Match( inString, &mc );
		while ( bResult ) {
			CREMatchGroupA	mgGroup;
			mc.GetMatch( 0, &mgGroup );

			int			nLen( static_cast<int>( mgGroup.szEnd - mgGroup.szStart ) );
			CPeekStringA	str( mgGroup.szStart, nLen );
			str.Trim();
			SInt32	nValue;
			StringToInteger( str, nValue );
			if ( (nValue > 0) && (nValue <= 1000000000) ) { // less than 1,000,000,000 (1 billion)
				outSize = static_cast<UInt32>( nValue );
				break;
			}
			bResult = g_RegExps.m_reXexch50InfoA.Match( mgGroup.szEnd, &mc );
		}
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'GBS1';
	}
	catch ( ... ) {
		throw 'GBS2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		IsAddress
// ----------------------------------------------------------------------------

bool
CRegExps::IsAddress(
	const CPeekString&	inAddress )
{
	BOOL				bResult;
	CREMatchContextW	mcAddress;

	bResult = g_RegExps.m_reAddressW.Match( inAddress, &mcAddress );
	if ( bResult ) {
		size_t	nMatchLen( mcAddress.m_Match.szEnd - mcAddress.m_Match.szStart );
		size_t	nStrLen( inAddress.GetLength() );
		if ( nMatchLen == nStrLen ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsBdat
// ----------------------------------------------------------------------------

bool
CRegExps::IsBdat(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reBdatA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISB1';
	}
	catch ( ... ) {
		throw 'ISB2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		IsData
// ----------------------------------------------------------------------------

bool
CRegExps::IsData(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reDataA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISD1';
	}
	catch ( ... ) {
		throw 'ISD2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		IsDomain
// ----------------------------------------------------------------------------

bool
CRegExps::IsDomain(
	const CPeekString&	inAddress )
{
	BOOL				bResult;
	CREMatchContextW	mcDomain;

	bResult = g_RegExps.m_reDomainW.Match( inAddress, &mcDomain );
	if ( bResult ) {
		size_t	nMatchLen( mcDomain.m_Match.szEnd - mcDomain.m_Match.szStart );
		size_t	nStrLen( inAddress.GetLength() );
		if ( nMatchLen == nStrLen ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsPlusOK
// ----------------------------------------------------------------------------

bool
CRegExps::IsPlusOK(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_rePlusOKA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISP1';
	}
	catch ( ... ) {
		throw 'ISP2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		IsQuit
// ----------------------------------------------------------------------------

bool
CRegExps::IsQuit(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reQuitA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISQ1';
	}
	catch ( ... ) {
		throw 'ISQ2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		IsReset
// ----------------------------------------------------------------------------

bool
CRegExps::IsReset(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reResetA.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISR1';
	}
	catch ( ... ) {
		throw 'ISR2';
	}

	return (bResult != FALSE);
}


// ----------------------------------------------------------------------------
//		IsXexch50
// ----------------------------------------------------------------------------

bool
CRegExps::IsXexch50(
	const CPeekStringA&	inString )
{
	BOOL				bResult;
	CREMatchContextA	mc;

	try {
		bResult = g_RegExps.m_reXexch50A.Match( inString, &mc );
	}
	catch ( CException* pE ) {
		pE->Delete();
		throw 'ISE1';
	}
	catch ( ... ) {
		throw 'ISE2';
	}

	return (bResult != FALSE);
}
