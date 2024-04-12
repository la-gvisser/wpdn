// ============================================================================
//	MsgText.h
//		interface for the CMsgText class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekStrings.h"
#include "ByteVectRef.h"


// ============================================================================
//		CMsgText
// ============================================================================

class CMsgText
	:	public CPeekStringA
{
protected:
	CPeekString		GetString() const { return CPeekString( *this ); }
	CPeekStringA	GetStringA() const { return *this; }

public:
	;			CMsgText() {}
	;			CMsgText( const CPeekStringA& inBody ) { assign( inBody ); }
	;			CMsgText( const CPeekString& inBody ) { Convert( inBody ); }

	bool		Find( const char* pstrPattern, size_t& ioOffset );
	int			FindLast( const char* pstrPattern ) const ;
	bool		FindNoCase( const char* pstrPattern, size_t& ioOffset );

	CPeekStringA	GetAddressA() const;
	CPeekString		GetAddress() const;
	CPeekStringA	GetDate() const;
	CPeekString		GetMailFrom() const;
	CPeekStringA	GetQDecoded() const;
	bool		GetRecipientTo( CArrayStringA& outRecipientList ) const;
#if USE_CDO_IMESSAGE
	bool		GetReceivedList( CArrayStringA& outReceivedList ) const;
#endif
	bool		GetBdatInfo( UInt32& outSize, bool& outLast ) const;
	bool		GetResponse( UInt32& outResponse ) const;
	bool		GetXexch50Info( UInt32& outSize, int& outCount ) const;

	bool		IsBdat() const;
	bool		IsCrLf( ptrdiff_t inOffset ) const;
	bool		IsData() const;
	bool		IsEndOfMsg() const;
	bool		IsError() const;
	bool		IsMailFrom() const;
	bool		IsPlusOK() const;
	bool		IsQuit() const;
	bool		IsRcptTo() const;
	bool		IsReset() const;
	bool		IsRetrieve() const;
	bool		IsXexch50() const;

	void			Set( CByteVectRef inData );
	CPeekStringA	Split( const char* pszTokens, size_t& ioOffset ) const;
};


// ============================================================================
//		CMsgTextList
// ============================================================================

class CMsgTextList
	: public CAtlArray<CMsgText>
{
public:
	size_t	Parse( const CMsgText& inMsgText );
};
