// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"


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
}
