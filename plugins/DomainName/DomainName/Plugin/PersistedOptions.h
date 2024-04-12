// =============================================================================
//	PersistedOptions.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"
#include "Domain.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	PersistedOptions
//
//	An Options holds the user preferences (settings) for the plugin and/or
//	individual capture contexts.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// =============================================================================
//		CPersistedOptions
// =============================================================================

class CPersistedOptions
{
	friend class COptions;

protected:
	static CPeekString	s_strTagForensics;

	bool				m_bTrackIP;
	CDomainList			m_ayDomainNames;
	CPeekString			m_strVersion;

protected:

	void		CheckDefaults() const {}

	void		Copy( const CPersistedOptions& inOther );

	bool		IsEqual( const CPersistedOptions& inOther );

	bool		ModelDomainList( CPeekDataElement& ioElement );

public:
	static CPeekString&	GetTagForensics() { return s_strTagForensics; }

public:
	;			CPersistedOptions();
	;			CPersistedOptions( const CPersistedOptions& inOther ) { Copy( inOther ); }
	virtual		~CPersistedOptions();

	CPersistedOptions&	operator=( const CPersistedOptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool		operator==( const CPersistedOptions& inOther ) { return IsEqual( inOther ); }
	bool		operator!=( const CPersistedOptions& inOther ) { return !IsEqual( inOther ); }

	CDomainList& GetDomainList() { return m_ayDomainNames; }

	bool		IsTrackIP() const { return m_bTrackIP; }

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		Reset() {
		m_bTrackIP = true;
		m_ayDomainNames.Reset();
	}


	void		SetVersion( CPeekString& inVersion ) { m_strVersion = inVersion; }
	void		SetTrackIP( bool inTrackIP ) { m_bTrackIP = inTrackIP; }
};
