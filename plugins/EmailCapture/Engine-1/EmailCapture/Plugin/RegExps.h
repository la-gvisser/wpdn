// ============================================================================
//	RegExps.h
//		interface for the CRegExps class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekStrings.h"
#include <atlrx.h>


// ============================================================================
//		Data Structures
// ============================================================================

typedef	ATL::CAtlRegExp<ATL::CAtlRECharTraitsW>			CRegExpW;

typedef	ATL::CAtlRegExp<ATL::CAtlRECharTraitsA>			CRegExpA;


// ============================================================================
//		CRegExps
// ============================================================================

class	CRegExps
{
public:
	CRegExpW	m_reAddressW;
	CRegExpW	m_reDomainW;
	CRegExpW	m_reMailFromW;

	CRegExpA	m_reAddressA;
	CRegExpA	m_reBdatA;
	CRegExpA	m_reBdatInfoA;
	CRegExpA	m_reDataA;
	CRegExpA	m_reDateA;
	CRegExpA	m_reMailFromA;
	CRegExpA	m_rePlusOKA;
	CRegExpA	m_reQEncodedA;
	CRegExpA	m_reQuitA;
	CRegExpA	m_reRecipientToA;
	CRegExpA	m_reResetA;
	CRegExpA	m_reResponseA;
	CRegExpA	m_reXexch50A;
	CRegExpA	m_reXexch50InfoA;

#if USE_CDO_IMESSAGE
	CRegExpA	m_reReceivedA;
#endif

	;			CRegExps();

	static	CRegExps&	GetRegExps();

	CPeekString		GetAddress( const CPeekString& inStr );
	bool			GetBdatInfo( const CPeekStringA& inString, UInt32& outSize, bool& outLast );
	CPeekStringA	GetDate( const CPeekStringA& inString );
	CPeekString		GetMailFrom( const CPeekStringA& inString );
	CPeekStringA	GetQDecoded( const CPeekStringA& inString );
	bool			GetRecipientTo( const CPeekStringA& inString, CArrayStringA& outRecipientList );
	bool			GetResponse( const CPeekStringA& inString, UInt32& outResponse );
	bool			GetXexch50Info( const CPeekStringA& inString, UInt32& outSize, int& outCount );

	bool		IsDomain( const CPeekString& inAddress );
	bool		IsAddress( const CPeekString& inAddress );
	bool		IsReset( const CPeekStringA& inString );
	bool		IsQuit( const CPeekStringA& inString );
	bool		IsPlusOK( const CPeekStringA& inString );
	bool		IsData( const CPeekStringA& inString );
	bool		IsBdat( const CPeekStringA& inString );
	bool		IsXexch50( const CPeekStringA& inString );
};
