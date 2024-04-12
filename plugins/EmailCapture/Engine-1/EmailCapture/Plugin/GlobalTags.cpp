// ============================================================================
//	GlobalTags.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#include "StdAfx.h"
#include "PeekStrings.h"
#include "GlobalTags.h"


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	extern const CPeekStringX	kxAt( "@" );
	extern const CPeekStringX	kxAngleLeft( "<" );
	extern const CPeekStringX	kxAngleRight( ">" );
	extern const CPeekStringX	kxBackSlash( "\\" );
	extern const CPeekStringX	kxCommaSpace( ", " );
	extern const CPeekStringX	kxColon( ":" );
	extern const CPeekStringX	kxColonSpace( ": " );
	extern const CPeekStringX	kxCurlyLeft( "{" );
	extern const CPeekStringX	kxCurlyRight( "}");
	extern const CPeekStringX	kxDouble( "\"" );
	extern const CPeekStringX	kxEmpty( "" );
	extern const CPeekStringX	kxNewLine( "\r\n" );
	extern const CPeekStringX	kxParenLeft( "(" );
	extern const CPeekStringX	kxParenRight( ")" );
	extern const CPeekStringX	kxQuote( "'" );
	extern const CPeekStringX	kxSemiSpace( "; " );
	extern const CPeekStringX	kxSingle( "\'" );
	extern const CPeekStringX	kxSpace( " " );
	extern const CPeekStringX	kxSpacedAnd( " & " );
	extern const CPeekStringX	kxSpaceDash( " -" );
	extern const CPeekStringX	kxSpaceDouble( " \"" );
	extern const CPeekStringX	kxSquareLeft( "[" );
	extern const CPeekStringX	kxSpaceQuote( " '" );
	extern const CPeekStringX	kxSquareRight( "]" );
	extern const CPeekStringX	kxTab( "\t" );
	extern const CPeekStringX	kxUnderbar( "_" );

	extern const CPeekStringX	kxCaptureAll( "Capture All" );
	extern const CPeekStringX	kxCase( "Case" );
	extern const CPeekStringX	kxCaptures( "Captures" );
	extern const CPeekStringX	kxDefaultOptions( "Default Options" );
	extern const CPeekStringX	kxDefaults( "Defaults" );
	extern const CPeekStringX	kxDisabled( "Disabled" );
	extern const CPeekStringX	kxEnabled( "Enabled" );
	extern const CPeekStringX	kxFull( "Full" );
	extern const CPeekStringX	kxIntegrity( "Integrity" );
	extern const CPeekStringX	kxInvalid( "Invalid" );
	extern const CPeekStringX	kxMessageId( "Message-ID" );
	extern const CPeekStringX	kxPen( "Pen" );
	extern const CPeekStringX	kxPenMail( "PenMail" );
	extern const CPeekStringX	kxPop3( "POP3" );
	extern const CPeekStringX	kxSmtp( "SMTP" );
	extern const CPeekStringX	kxTypesEml[4] = { "From", "To", "Cc", "Bcc" };
	extern const CPeekStringX	kxTypesProtocol[3] = { "SMTP", "POP3", "Invalid" };
	extern const CPeekStringX	kxTypesTxt[4] = { "FROM", "TO", "CC", "BCC" };
	extern const CPeekStringX	kxXMailFrom( "X-MAIL-FROM: " );
	extern const CPeekStringX	kxXRcptTo( "X-RCPT-TO: " );
}


// ============================================================================
//		FormatEnabled
// ============================================================================

const CPeekString&
FormatEnabled( bool inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.GetW()
		: Tags::kxDisabled.GetW();
}

const CPeekStringA&
FormatEnabledA( bool inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.GetA()
		: Tags::kxDisabled.GetA();
}

const CPeekString*
FormatEnabledRef( bool inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.RefW()
		: Tags::kxDisabled.RefW();
}

const CPeekStringA*
FormatEnabledRefA( bool inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.RefA()
		: Tags::kxDisabled.RefA();
}

const CPeekString&
FormatEnabled( BOOL inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.GetW()
		: Tags::kxDisabled.GetW();
}

const CPeekStringA&
FormatEnabledA( BOOL inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.GetA()
		: Tags::kxDisabled.GetA();
}

const CPeekString*
FormatEnabledRef( BOOL inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.RefW()
		: Tags::kxDisabled.RefW();
}

const CPeekStringA*
FormatEnabledRefA( BOOL inEnabled ) {
	return (inEnabled)
		? Tags::kxEnabled.RefA()
		: Tags::kxDisabled.RefA();
}
