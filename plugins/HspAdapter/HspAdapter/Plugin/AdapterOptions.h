// =============================================================================
//	AdapterOptions.h
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	AdapterOptions
//
//	An Options holds the adapter settings.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CAdapterOptions
// =============================================================================

class CAdapterOptions
{
protected:
	CGlobalId	m_Id;
	CPeekString	m_AdapterName;

protected:
	void		Copy( const CAdapterOptions& inOther );
	bool		IsEqual( const CAdapterOptions& inOther ) const;

public:
	;			CAdapterOptions();
	;			CAdapterOptions( const CAdapterOptions& inOther ) { Copy( inOther ); }
	virtual		~CAdapterOptions();

	CAdapterOptions&	operator=( const CAdapterOptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool	operator==( const CAdapterOptions& inOther ) { return IsEqual( inOther ); }
	bool	operator!=( const CAdapterOptions& inOther ) { return !IsEqual( inOther ); }

	const CGlobalId&	GetId() const { return m_Id; }
	const CPeekString&	GetAdapterName() const { return m_AdapterName; }

	bool	IsEmpty() const;

	bool	Model( CPeekDataModeler& ioPrefs );
	bool	Model( CPeekDataElement& ioElement );

	void	Reset();

	void	SetId( const CGlobalId& inId ) { m_Id = inId; }
	void	SetName( const CPeekString& inName ) { m_AdapterName = inName; }
};
