// ============================================================================
//	ut_peekstring.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2010. All rights reserved.

#include "StdAfx.h"
#include "PeekStrings.h"
#include "stdlib.h"
#include <iostream>
#include <iomanip>

#if (0)
#include <math.h>
using namespace std;
#define leftprec(x,y) (x - (((int)floor(x) - (((int)floor(x))% ((int)(pow((double)10,(double)y)))))))
extern ostream cout;

void
Example_PeekOutString()
{
	string 			str;
	ostringstream 	strstrm;

	strstrm << boolalpha << true << " is a bool value" << noboolalpha
		    << ", so is " << false;
	cout << strstrm.str() << endl;
	strstrm.str( "" );

	strstrm << "100 decimal = " << dec << 100 << ", octal = " << 100
		    << ", hex = " << hex << 100 << endl;
	cout << strstrm.str();
	strstrm.str( "" );

	float f = 199.9273f;
	strstrm << "A float: " << f << ", scientific notation " << f 
			<< " " << scientific << f << " fixed notation " << fixed 
			<< f << endl;
	cout << strstrm.str();
	strstrm.str( "" );

	strstrm << "Formatted to two decimal places " << setprecision(2) 
	        << f << ", formatted to %2.5f " << setprecision(5)
			<< leftprec(f, 2) << endl;
	cout << strstrm.str();
}

// Output of Example_PeekOutString
// -------------------------------------------------------------------------------------
// true is a bool value, so is 0
// 100 decimal = 100, octal = 144, hex = 64
// A float: 199.927, scientific notation 199.927 1.999273e+002 fixed notation 199.927307
// Formatted to two decimal places 199.93, formatted to %2.5f 99.92731
#endif

int
PeekStrings()
{
	int		nFailures( 0 );

	std::string			strDate( "2011-05-13T01:00:17.122818Z" );	//122818600Z
	std::istringstream	istrDate( strDate );
	char				cSeparators[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	SYSTEMTIME			stTime;
	UInt32				nMilliseconds = 0;
	memset( &stTime, 0, sizeof( stTime ) );

	istrDate >> stTime.wYear >> cSeparators[0] >>
		stTime.wMonth >> cSeparators[1] >>
		stTime.wDay >> cSeparators[2] >>
		stTime.wHour >> cSeparators[3] >>
		stTime.wMinute >> cSeparators[4] >>
		stTime.wSecond >> cSeparators[5] >>
		nMilliseconds >> cSeparators[6];
	stTime.wMilliseconds = static_cast<unsigned short>( nMilliseconds );


	std::string	strStd( "Hello World" );
	CString		strMFC( "World Hello" );

	CPeekString	strA;
	CPeekString	strB( L"Beta" );
	CPeekString	strC( strStd );
	CPeekString	strD( strMFC );

	wchar_t*		pNULL( nullptr );
	CPeekString		strNull( pNULL );
	CPeekString		strNullLen( pNULL, 0 );
	char*			pNULLA( nullptr );
	CPeekStringA	strNullA( pNULLA );
	CPeekStringA	strNullLenA( pNULLA, 0 );

	ASSERT( strA.IsEmpty() );
	if ( !strA.IsEmpty() ) nFailures++;
	strA = L"Hello ";
	strA += L"World";

	if ( strA != L"Hello World" ) nFailures++;
	if ( !(strA.Compare( L"Hello World" ) == 0) ) nFailures++;
	if ( !(strA.CompareNoCase( L"hello WORLD" ) == 0) ) nFailures++;

	ASSERT( strA == strC );
	if ( strA != strC ) nFailures++;

	strA.Empty();
	ASSERT( strA.IsEmpty() );
	if ( !strA.IsEmpty() ) nFailures++;

	size_t	nLenB = strB.GetLength();
	ASSERT( nLenB == 4 );
	if ( nLenB != 4 ) nFailures++;

	strA.Format( L"%s = %d", L"Four score and seven", 87 );
	ASSERT( strA.GetLength() == 25 );
	if ( strA.GetLength() != 25 ) nFailures++;
	ASSERT( strA.GetAt( 22 - 1 ) == L'=' );
	if ( strA.GetAt( 22 - 1 ) != L'=' ) nFailures++;

	strB = strA.Left( 10 );
	ASSERT( strB == L"Four score" );
	if ( strB != L"Four score" ) nFailures++;

	strB.Remove( L'o' );
	ASSERT( strB == L"Fur scre" );
	if ( strB != L"Fur scre" ) nFailures++;

	strB = strA.Left( 10 );
	ASSERT( strB == L"Four score" );
	strB.Replace( L'o', L'*' );
	ASSERT( strB == L"F*ur sc*re" );
	if ( strB != L"F*ur sc*re" ) nFailures++;

	strC = strA.Right( 2 );
	ASSERT( strC == L"87" );
	if ( strC != L"87" ) nFailures++;

	// Tokenize: simple
	const int nTokenCount( 7 );	// CString throws on 8, but not CPeekString
	CString	strTest = "Four score and seven = 87";
	CString	strToks[nTokenCount];
	int		nTestOffset = 0;
	int		nTestOffs[nTokenCount] = { 0 };
	for ( int i = 0; i < nTokenCount; i++ ) {
		strToks[i] = strTest.Tokenize( L" ", nTestOffset );
		nTestOffs[i] = nTestOffset;
	}

	size_t		nTokenOffset = 0;
	CPeekString	strTokens[nTokenCount];
	strA = strTest.GetString();
	for ( int i = 0; i < nTokenCount; i++ ) {
		strTokens[i] = strA.Tokenize( L" ", nTokenOffset );
		ASSERT( strTokens[i] == strToks[i].GetString() );
		if ( strTokens[i] != strToks[i].GetString() ) nFailures++;
		ASSERT( nTokenOffset == nTestOffs[i] );
		if ( nTokenOffset != nTestOffs[i] ) nFailures++;
	}

	// Tokenize: complex
	strTest = " Four  score  and  seven =        87 ";
	nTestOffset = 0;
	for ( int i = 0; i < nTokenCount; i++ ) {
		nTestOffs[i] = 0;
		strToks[i].Empty();
		strTokens[i].Empty();
	}

	for ( int i = 0; i < nTokenCount; i++ ) {
		strToks[i] = strTest.Tokenize( L" ", nTestOffset );
		nTestOffs[i] = nTestOffset;
	}

	strA = strTest.GetString();
	nTokenOffset = 0;
	for ( int i = 0; i < nTokenCount; i++ ) {
		strTokens[i] = strA.Tokenize( L" ", nTokenOffset );
		ASSERT( strTokens[i] == strToks[i].GetString() );
		if ( strTokens[i] != strToks[i].GetString() ) nFailures++;
		ASSERT( nTokenOffset == nTestOffs[i] );
		if ( nTokenOffset != nTestOffs[i] ) nFailures++;
	}

	// Trimming
	CString	strTestTrim( L"  Trim Test  \t\n" );
	strTestTrim.Trim();

	strA = L"  Trim Test  \t\n";
	strB = strA.TrimLeft();
	ASSERT( strA == L"Trim Test  \t\n" );
	if ( strA != L"Trim Test  \t\n" ) nFailures++;
	ASSERT( strB == L"Trim Test  \t\n" );
	if ( strB != L"Trim Test  \t\n" ) nFailures++;

	strA = L"  Trim Test  \t\n";
	CPeekString&	strRef = strA.TrimRight();
	ASSERT( strA == L"  Trim Test" );
	if ( strA != L"  Trim Test" ) nFailures++;
	ASSERT( strRef == L"  Trim Test" );
	if ( strRef != L"  Trim Test" ) nFailures++;

	strA = L"  Trim Test  \t\n";
	strRef = strA.Trim();
	ASSERT( strA == L"Trim Test" );
	if ( strA != L"Trim Test" ) nFailures++;
	ASSERT( strRef == L"Trim Test" );
	if ( strRef != L"Trim Test" ) nFailures++;

	bool			bTrue = true;
	UInt32			nIpValue = 0x01020304;
	CPeekOutString	strOut1;
	CPeekOutString	strOut2;
	CPeekString		strValue;

	// boolalpha - format bools as true or false.
	// noboolalpha - format bools as 1 or 0. (default)
	// sticky setting.
	strOut1 << std::boolalpha << bTrue << L" " << !bTrue << std::endl;
	strValue = strOut1;

	strOut2 << bTrue << std::endl;
	strValue = strOut2;
	strOut1.str( L"" );

	strOut1 << bTrue << std::endl;
	strValue = strOut1;
	strOut1.str( L"" );

	std::streamsize nWidth = strOut1.width();

	// Set Width to 8, base 16 (hex), fill with 0 (zer0).
	// Show Base - std::setiosflags( std::ios_base::showbase ) - overrides width and fill.
	// Set Width only applies to next value to be formatted.
	strOut1 << std::hex << std::setfill( L'0' );
	strOut1 << std::setw( 8 ) << nIpValue << L" " << nIpValue << L" " << 0x0F << L" ";
	strOut1 << std::dec << std::setw( 8 ) << 0x0F << L" " << 0x0F << std::endl;
	strOut1.str( L"" );	// 01020304 00000001
	// Want to reset the width.., turn on showing base (leading 0x).
	strOut1 << std::setw( 0 ) << std::setiosflags( std::ios_base::showbase );
	strOut1 << nIpValue << L" " << std::endl;
	strOut1 << 1 << L" " << std::endl;
	strOut1.str( L"" ); // 0x1020304 0x1

	{
#include "wppushpack.h"
		typedef union {
			long	nValue;
			wchar_t	nUnicode[2];
		} longchar_t;
#include "wppoppack.h"

		errno_t		ayErrNo[1024];
		char		szDst[1024];

		_locale_t	locUTF8 = _create_locale( LC_ALL, "american" );


		for ( int i = 0; i < 1024; ++i ) {
			size_t	nNewSize = 0;
			wchar_t	szChar = i;
			ayErrNo[i] = _wcstombs_s_l( &nNewSize, szDst, 1024, &szChar, 1, locUTF8 );
		}

		int	szChar = 0;
		if ( ayErrNo[0] == 0 ) {
			szChar++;
		}
	}

	return nFailures;
}


int
PeekStringsA()
{
	int		nFailures( 0 );

	std::string			strDate( "2011-05-13T01:00:17.122818Z" );	//122818600Z
	std::istringstream	istrDate( strDate );
	char				cSeparators[7] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	SYSTEMTIME			stTime;
	UInt32				nMilliseconds = 0;
	memset( &stTime, 0, sizeof( stTime ) );

	istrDate >> stTime.wYear >> cSeparators[0] >>
		stTime.wMonth >> cSeparators[1] >>
		stTime.wDay >> cSeparators[2] >>
		stTime.wHour >> cSeparators[3] >>
		stTime.wMinute >> cSeparators[4] >>
		stTime.wSecond >> cSeparators[5] >>
		nMilliseconds >> cSeparators[6];
	stTime.wMilliseconds = static_cast<unsigned short>( nMilliseconds );

	std::string	strStd( "Hello World" );
	CStringA	strMFC( "World Hello" );

	CPeekStringA	strA;
	CPeekStringA	strB( "Beta" );
	CPeekStringA	strC( strStd );
	CPeekStringA	strD( strMFC );

	wchar_t*		pNULL( nullptr );
	CPeekString		strNull( pNULL );
	CPeekString		strNullLen( pNULL, 0 );
	char*			pNULLA( nullptr );
	CPeekStringA	strNullA( pNULLA );
	CPeekStringA	strNullLenA( pNULLA, 0 );

	ASSERT( strA.IsEmpty() );
	if ( !strA.IsEmpty() ) nFailures++;
	strA = "Hello ";
	strA += "World";

	if ( strA != "Hello World" ) nFailures++;
	if ( !(strA.Compare( "Hello World" ) == 0) ) nFailures++;
	if ( !(strA.CompareNoCase( "hello WORLD" ) == 0) ) nFailures++;

	ASSERT( strA == strC );
	if ( strA != strC ) nFailures++;

	strA.Empty();
	ASSERT( strA.IsEmpty() );
	if ( !strA.IsEmpty() ) nFailures++;

	size_t	nLenB = strB.GetLength();
	ASSERT( nLenB == 4 );
	if ( nLenB != 4 ) nFailures++;

	strA.Format( "%s = %d", "Four score and seven", 87 );
	ASSERT( strA.GetLength() == 25 );
	if ( strA.GetLength() != 25 ) nFailures++;
	ASSERT( strA.GetAt( 22 - 1 ) == '=' );
	if ( strA.GetAt( 22 - 1 ) != '=' ) nFailures++;

	strB = strA.Left( 10 );
	ASSERT( strB == "Four score" );
	if ( strB != "Four score" ) nFailures++;

	strB.Remove( 'o' );
	ASSERT( strB == "Fur scre" );
	if ( strB != "Fur scre" ) nFailures++;

	strB = strA.Left( 10 );
	ASSERT( strB == "Four score" );
	strB.Replace( 'o', '*' );
	ASSERT( strB == "F*ur sc*re" );
	if ( strB != "F*ur sc*re" ) nFailures++;

	strC = strA.Right( 2 );
	ASSERT( strC == "87" );
	if ( strC != "87" ) nFailures++;

	// Tokenize: simple
	const int nTokenCount( 7 );	// CString throws on 8, but not CPeekString
	CStringA	strTest = "Four score and seven = 87";
	CStringA	strToks[nTokenCount];
	int			nTestOffset = 0;
	int			nTestOffs[nTokenCount] = { 0 };
	for ( int i = 0; i < nTokenCount; i++ ) {
		strToks[i] = strTest.Tokenize( " ", nTestOffset );
		nTestOffs[i] = nTestOffset;
	}

	size_t			nTokenOffset = 0;
	CPeekStringA	strTokens[nTokenCount];
	strA = strTest.GetString();
	for ( int i = 0; i < nTokenCount; i++ ) {
		strTokens[i] = strA.Tokenize( " ", nTokenOffset );
		ASSERT( strTokens[i] == strToks[i].GetString() );
		if ( strTokens[i] != strToks[i].GetString() ) nFailures++;
		ASSERT( nTokenOffset == nTestOffs[i] );
		if ( nTokenOffset != nTestOffs[i] ) nFailures++;
	}

	// Tokenize: complex
	strTest = " Four  score  and  seven =        87 ";
	nTestOffset = 0;
	for ( int i = 0; i < nTokenCount; i++ ) {
		nTestOffs[i] = 0;
		strToks[i].Empty();
		strTokens[i].Empty();
	}

	for ( int i = 0; i < nTokenCount; i++ ) {
		strToks[i] = strTest.Tokenize( " ", nTestOffset );
		nTestOffs[i] = nTestOffset;
	}

	strA = strTest.GetString();
	nTokenOffset = 0;
	for ( int i = 0; i < nTokenCount; i++ ) {
		strTokens[i] = strA.Tokenize( " ", nTokenOffset );
		ASSERT( strTokens[i] == strToks[i].GetString() );
		if ( strTokens[i] != strToks[i].GetString() ) nFailures++;
		ASSERT( nTokenOffset == nTestOffs[i] );
		if ( nTokenOffset != nTestOffs[i] ) nFailures++;
	}

	// Trimming
	CStringA	strTestTrim( "  Trim Test  \t\n" );
	strTestTrim.Trim();

	strA = "  Trim Test  \t\n";
	strB = strA.TrimLeft();
	ASSERT( strA == "Trim Test  \t\n" );
	if ( strA != "Trim Test  \t\n" ) nFailures++;
	ASSERT( strB == "Trim Test  \t\n" );
	if ( strB != "Trim Test  \t\n" ) nFailures++;

	strA = "  Trim Test  \t\n";
	CPeekStringA&	strRef = strA.TrimRight();
	ASSERT( strA == "  Trim Test" );
	if ( strA != "  Trim Test" ) nFailures++;
	ASSERT( strRef == "  Trim Test" );
	if ( strRef != "  Trim Test" ) nFailures++;

	strA = "  Trim Test  \t\n";
	strRef = strA.Trim();
	ASSERT( strA == "Trim Test" );
	if ( strA != "Trim Test" ) nFailures++;
	ASSERT( strRef == "Trim Test" );
	if ( strRef != "Trim Test" ) nFailures++;

	bool			bTrue = true;
	UInt32			nIpValue = 0x01020304;
	CPeekOutStringA	strOut1;
	CPeekOutStringA	strOut2;
	CPeekStringA	strValue;

	// boolalpha - format bools as true or false.
	// noboolalpha - format bools as 1 or 0. (default)
	// sticky setting.
	strOut1 << std::boolalpha << bTrue << " " << !bTrue << std::endl;
	strValue = strOut1;

	strOut2 << bTrue << std::endl;
	strValue = strOut2;
	strOut1.str( "" );

	strOut1 << bTrue << std::endl;
	strValue = strOut1;
	strOut1.str( "" );

	std::streamsize nWidth = strOut1.width();

	// Set Width to 8, base 16 (hex), fill with 0 (zer0).
	// Show Base - std::setiosflags( std::ios_base::showbase ) - overrides width and fill.
	// Set Width only applies to next value to be formatted.
	strOut1 << std::hex << std::setfill( '0' );
	strOut1 << std::setw( 8 ) << nIpValue << " " << nIpValue << " " << 0x0F << " ";
	strOut1 << std::dec << std::setw( 8 ) << 0x0F << " " << 0x0F << std::endl;
	strOut1.str( "" );	// 01020304 00000001
	// Want to reset the width.., turn on showing base (leading 0x).
	strOut1 << std::setw( 0 ) << std::setiosflags( std::ios_base::showbase );
	strOut1 << nIpValue << " " << std::endl;
	strOut1 << 1 << " " << std::endl;
	strOut1.str( "" ); // 0x1020304 0x1

	{
#include "wppushpack.h"
		typedef union {
			long	nValue;
			char	nChars[2];
		} longchar_t;
#include "wppoppack.h"

		errno_t		ayErrNo[1024];
		wchar_t		szDst[1024];

		_locale_t	locUTF8 = _create_locale( LC_ALL, "american" );

		for ( int i = 0; i < 1024; ++i ) {
			size_t	nNewSize = 0;
			char	szChar = i;
			ayErrNo[i] = _mbstowcs_s_l( &nNewSize, szDst, 1024, &szChar, 1, locUTF8 );
		}

		int	szChar = 0;
		if ( ayErrNo[0] == 0 ) {
			szChar++;
		}
	}

	return nFailures;
}


int
UT_PeekStrings()
{
	int		nFailures( 0 );

	nFailures += PeekStrings();
	nFailures += PeekStringsA();

	return nFailures;
}