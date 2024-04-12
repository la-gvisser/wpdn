// ============================================================================
//	GlobalTags.cpp
//		implementation of the CReportMode class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#include "PeekStrings.h"


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	extern const CPeekStringX	kxAt;
	extern const CPeekStringX	kxAngleLeft;
	extern const CPeekStringX	kxAngleRight;
	extern const CPeekStringX	kxBackSlash;
	extern const CPeekStringX	kxCommaSpace;
	extern const CPeekStringX	kxColon;
	extern const CPeekStringX	kxColonSpace;
	extern const CPeekStringX	kxCurlyLeft;
	extern const CPeekStringX	kxCurlyRight;
	extern const CPeekStringX	kxDouble;
	extern const CPeekStringX	kxEmpty;
	extern const CPeekStringX	kxNewLine;
	extern const CPeekStringX	kxParenLeft;
	extern const CPeekStringX	kxParenRight;
	extern const CPeekStringX	kxQuote;
	extern const CPeekStringX	kxSemiSpace;
	extern const CPeekStringX	kxSingle;
	extern const CPeekStringX	kxSpace;
	extern const CPeekStringX	kxSpacedAnd;
	extern const CPeekStringX	kxSpaceDash;
	extern const CPeekStringX	kxSpaceDouble;
	extern const CPeekStringX	kxSquareLeft;
	extern const CPeekStringX	kxSpaceQuote;
	extern const CPeekStringX	kxSquareRight;
	extern const CPeekStringX	kxTab;
	extern const CPeekStringX	kxUnderbar;

	extern const CPeekStringX	kxCaptureAll;
	extern const CPeekStringX	kxCase;
	extern const CPeekStringX	kxDefaultOptions;
	extern const CPeekStringX	kxDefaults;
	extern const CPeekStringX	kxDisabled;
	extern const CPeekStringX	kxEnabled;
	extern const CPeekStringX	kxFull;
	extern const CPeekStringX	kxIntegrity;
	extern const CPeekStringX	kxInvalid;
	extern const CPeekStringX	kxMessageId;
	extern const CPeekStringX	kxPen;
	extern const CPeekStringX	kxPenMail;
	extern const CPeekStringX	kxPop3;
	extern const CPeekStringX	kxXMailFrom;
	extern const CPeekStringX	kxXRcptTo;
	extern const CPeekStringX	kxSmtp;
	extern const CPeekStringX	kxTypesEml[];
	extern const CPeekStringX	kxTypesProtocol[];
	extern const CPeekStringX	kxTypesTxt[];
	extern const CPeekStringX	kxXMailFrom;
	extern const CPeekStringX	kxXRcptTo;
}


// ============================================================================
//		FormatEnabled
// ============================================================================

const CPeekString&	FormatEnabled( bool inEnabled );
const CPeekStringA&	FormatEnabledA( bool inEnabled );
const CPeekString*	FormatEnabledRef( bool inEnabled );
const CPeekStringA*	FormatEnabledRefA( bool inEnabled );

const CPeekString&	FormatEnabled( BOOL inEnabled );
const CPeekStringA&	FormatEnabledA( BOOL inEnabled );
const CPeekString*	FormatEnabledRef( BOOL inEnabled );
const CPeekStringA*	FormatEnabledRefA( BOOL inEnabled );
