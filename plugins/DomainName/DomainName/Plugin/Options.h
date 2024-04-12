// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PersistedOptions.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// =============================================================================
//		COptions
// =============================================================================

class COptions
{

protected:
	CPersistedOptions		m_PersistedOptions;

	CDomainCountedList		m_DomainCountedList;
	CResolvedDomainList		m_ResolvedNames;
	CIpAddressList			m_ActiveIpAddressList;
	UInt32					m_nAction;

protected:
	void		CheckDefaults() const {}
	void		Copy( const COptions& inOther ) {
		m_DomainCountedList = inOther.m_DomainCountedList;
		m_ResolvedNames = inOther.m_ResolvedNames;
		m_PersistedOptions = inOther.m_PersistedOptions;
	}

	bool		IsEqual( const COptions& inOther ) {
		return ( m_PersistedOptions == inOther.m_PersistedOptions );
	}

	bool		ModelCountedList( CPeekDataElement& ioElement );
	bool		ModelDomainList( CPeekDataElement& ioElement ) {
		return m_PersistedOptions.ModelDomainList( ioElement );
	}
	bool		ModelResolvedList( CPeekDataElement& ioElement );

public:
	static CPeekString&	GetTagForensics() { return CPersistedOptions::GetTagForensics(); }

public:
	;			COptions() {}
	;			COptions( const COptions& inOther ) { Copy( inOther ); }
	virtual		~COptions() {}

	COptions&	operator=( const COptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool		operator==( const COptions& inOther ) { return m_PersistedOptions.operator==( inOther.m_PersistedOptions ); }
	bool		operator!=( const COptions& inOther ) { return m_PersistedOptions.operator!=( inOther.m_PersistedOptions ); }

	CIpAddressList&		 GetActiveList() { return m_ActiveIpAddressList; }
	CDomainList&		 GetDomainList() { return m_PersistedOptions.GetDomainList(); }
	CDomainCountedList&	 GetDomainCountedList() { return m_DomainCountedList; }
	CResolvedDomainList& GetResolvedDomainList() { return m_ResolvedNames; }

	bool		IsTrackIP() const { return m_PersistedOptions.IsTrackIP(); }

	bool		Model( CPeekDataModeler& ioPrefs );
	bool		Model( CPeekDataElement& ioElement );

	void		Reset() {
		m_DomainCountedList.Reset();
		m_ResolvedNames.Reset();
		m_ActiveIpAddressList.Reset();
		m_PersistedOptions.Reset();
	}

	void		SetDomainNames() {
		const CDomainList& theDomainList = GetDomainList();
		m_DomainCountedList = theDomainList;
		m_ResolvedNames.PruneList( theDomainList );
		m_ResolvedNames.GetAddresses( m_ActiveIpAddressList );
	}
	void		SetTrackIP( bool inTrackIP ) { m_PersistedOptions.SetTrackIP( inTrackIP ); }
	void		SetVersion( CPeekString& inVersion ) { m_PersistedOptions.SetVersion( inVersion ); }
};
