// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"
#include <algorithm>


// -----------------------------------------------------------------------------
//		ReplaceAll
// -----------------------------------------------------------------------------

void
ReplaceAll(
	CPeekString&	inValue,
	wchar_t			inFind,
	const wchar_t*	inReplace )
{
	size_t	pos = inValue.Find( inFind );
	while ( pos != std::wstring::npos ) {
		inValue.replace( pos, 1, inReplace );
		pos = inValue.find( inFind );
	}
}


// -----------------------------------------------------------------------------
//		xmlEncode
// -----------------------------------------------------------------------------

CPeekString
xmlEncode(
	const CPeekString&	inValue,
	bool				inEncode )
{
	CPeekString	strValue( inValue );

	static const std::wstring strTab( L"&#0009;" );
	static const std::wstring strNewLine( L"&#0010;" );
	static const std::wstring strReturn( L"&#0013;" );

	if ( inEncode ) {
		ReplaceAll( strValue, L'\t', strTab.c_str() );
		ReplaceAll( strValue, L'\n', strNewLine.c_str() );
		ReplaceAll( strValue, L'\r', strReturn.c_str() );
	}

	return strValue;
}


// =============================================================================
//		COptions
// =============================================================================

COptions::COptions()
{
	Reset();
}

COptions::~COptions()
{
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	m_strCaseName1 = inOther.m_strCaseName1;
	m_strCaseName2 = inOther.m_strCaseName2;
	m_strCaseName3 = inOther.m_strCaseName3;

	m_strNotes = inOther.m_strNotes;
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther )
{
	if ( m_strCaseName1 != inOther.m_strCaseName1 ) return false;
	if ( m_strCaseName2 != inOther.m_strCaseName2 ) return false;
	if ( m_strCaseName3 != inOther.m_strCaseName3 ) return false;

	if ( m_strNotes != inOther.m_strNotes ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elem( ioPrefs );
	return Model( elem );
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	CPeekDataElement	elemCase1( L"Case", ioElement, 0 );
	;	elemCase1.Value( m_strCaseName1 );
	CPeekDataElement	elemCase2( L"Case", ioElement, 1 );
	;	elemCase2.Value( m_strCaseName2 );
	CPeekDataElement	elemCase3( L"Case", ioElement, 2 );
	;	elemCase3.Value( m_strCaseName3 );

	CPeekDataElement	elemNotes( L"Notes", ioElement );
	;	CPeekString	strNotes = xmlEncode( m_strNotes, ioElement.IsStoring() );
	;	elemNotes.Value( (ioElement.IsStoring()) ? strNotes : m_strNotes );

	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset()
{
	m_strCaseName1.Empty();
	m_strCaseName2.Empty();
	m_strCaseName3.Empty();

	m_strNotes.Empty();
}
