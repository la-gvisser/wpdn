// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekArray.h"
#include "PeekStrings.h"
#include "AdapterOptions.h"

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
public:
	typedef CPeekArray<CAdapterOptions>	CAdapterList;

protected:
	CAdapterList	m_ayAdapters;

protected:
	void		Copy( const COptions& inOther );
	bool		IsEqual( const COptions& inOther ) const;

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
	bool	operator==( const COptions& inOther ) { return IsEqual( inOther ); }
	bool	operator!=( const COptions& inOther ) { return !IsEqual( inOther ); }

	void	Add( const CAdapterOptions& inOptions ) {
		m_ayAdapters.push_back( inOptions );
	}

	bool	Delete( CGlobalId inId );

	bool	Find( CGlobalId inId, CAdapterOptions& outAdapterOptions ) const;

	const CAdapterList&	GetAdapterList() const { return m_ayAdapters; }

	bool	IsEmpty() const;

	bool	Model( CPeekDataModeler& ioPrefs );
	bool	Model( CPeekDataElement& ioElement );

	void	Reset();

	CPeekString	ToString() const;

	void	Update( const CAdapterOptions& inAdapterOptions );
};
