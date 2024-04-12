// =============================================================================
//	DNSMessageOptions.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
//#include "PeekTime.h"
#include "PeekArray.h"
//#include "GlobalId.h"
#include "Domain.h"
#include "Options.h"
//#include "Packet.h"
//#include "PacketArray.h"
//#include "DnsEntry.h"

//class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// =============================================================================
//		CDNSMessageOptions
// =============================================================================

class CDNSMessageOptions
{

protected:
	COptions				m_Options;

	CDomainCountedList		m_DomainCountedList;
	CResolvedDomainList		m_ResolvedNames;
	CIpAddressList			m_ActiveIpAddressList;

protected:
	void		CheckDefaults() const {}
	void		Copy( const CDNSMessageOptions& inOther ) {
		//ptst???
		m_DomainCountedList = inOther.m_DomainCountedList;
		m_ResolvedNames = inOther.m_ResolvedNames;
//		m_ActiveIpAddressList = inOther.m_ActiveIpAddressList;
//		m_pOwningProcess = inOther.m_pOwningProcess;
		m_Options.Copy( inOther.m_Options );
	}

	bool		IsEqual( const CDNSMessageOptions& inOther ) {
		//ptst - ???
//		if ( m_pOwningProcess != inOther.m_pOwningProcess ) return false;
		return m_Options.IsEqual( inOther.m_Options );
	}

	bool		ModelCountedList( CPeekDataElement& ioElement );
	bool		ModelDomainList( CPeekDataElement& ioElement ) {
		return m_Options.ModelDomainList( ioElement );
	}
	bool		ModelResolvedList( CPeekDataElement& ioElement );

public:
	static CPeekString&	GetTagForensics() { return COptions::GetTagForensics(); }

public:
	;			CDNSMessageOptions() {} //: m_pOwningProcess( NULL ) {}
	;			CDNSMessageOptions( const CDNSMessageOptions& inOther ) { Copy( inOther ); }
	virtual		~CDNSMessageOptions() {}

	CDNSMessageOptions&	operator=( const CDNSMessageOptions& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool			operator==( const CDNSMessageOptions& inOther ) { return m_Options.operator==( inOther.m_Options ); }
	bool			operator!=( const CDNSMessageOptions& inOther ) { return m_Options.operator!=( inOther.m_Options ); }

	CIpAddressList&	GetActiveList() { return m_ActiveIpAddressList; }
	CDomainList&	GetDomainList() { return m_Options.GetDomainList(); }

	CDomainCountedList&		GetDomainCountedList() { return m_DomainCountedList; }
	CResolvedDomainList&	GetResolvedDomainList() { return m_ResolvedNames; }

	bool			IsTrackIP() const { return m_Options.IsTrackIP(); }

	bool			Model( CPeekDataModeler& ioPrefs, bool bSetDomainNames = false );
	bool			Model( CPeekDataElement& ioElement );

	void			Reset() {
		m_DomainCountedList.Reset();
		m_ResolvedNames.Reset();
		m_ActiveIpAddressList.Reset();
		m_Options.Reset();
	}

//	void		SetDomainNames() {
	void		SetDomainNames( COptions& inOptions ) {
		const CDomainList& theDomainList = inOptions.GetDomainList();
		m_DomainCountedList = theDomainList;
		m_ResolvedNames.PruneList( theDomainList );
		m_ResolvedNames.GetAddresses( m_ActiveIpAddressList );
	}

//	void			SetOwningProcess( IDNSProcess* inOwningProcess ) {
//		m_pOwningProcess = inOwningProcess;
//	}
	void			SetTrackIP( bool inTrackIP ) { m_Options.SetTrackIP( inTrackIP ); }
	void			SetVersion( CPeekString& inVersion ) { m_Options.SetVersion( inVersion ); }
};
