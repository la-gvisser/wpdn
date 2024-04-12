// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Options
//
//	An Options holds the user preferences (settings) for the plugin and/or
//	individual capture contexts.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#if (0)
  <xml>
    <CaseList>
      <Case>Case 1</Case>
      <Case>Case 2</Case>
      <Case>Case 3</Case>
    </CaseList>
  </xml>
#endif


// =============================================================================
//		COptions
// =============================================================================

class COptions
{
	friend class COptionsDialog;

protected:
	CPeekString	m_strCaseName1;
	CPeekString	m_strCaseName2;
	CPeekString	m_strCaseName3;

protected:
	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther );

public:
	;			COptions();
	;			COptions( const COptions& inOther ) { Copy( inOther ); }
	virtual		~COptions();

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool		operator==( const COptions& inOther ) { return IsEqual( inOther ); }
	bool		operator!=( const COptions& inOther ) { return !IsEqual( inOther ); }

	const wchar_t*	GetCaseName( int inIndex ) const {
		switch ( inIndex ) {
		case 1: return m_strCaseName1.c_str();
		case 2: return m_strCaseName2.c_str();
		case 3: return m_strCaseName3.c_str();
		}
		return NULL;
	}

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		Reset();
};
