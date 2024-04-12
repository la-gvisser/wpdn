// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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


// =============================================================================
//		COptions
// =============================================================================

class COptions
{
	friend class COptionsDialog;

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

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		Reset();
};
