// =============================================================================
//	DNSProcess.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "IpHeaders.h"
#include "PeekTime.h"
#include "PeekArray.h"
#include "GlobalId.h"
#include "Options.h"
#include "Packet.h"
#include "PacketArray.h"

class CPeekContext;
class CPeekDataModeler;
class CPeekDataElement;


// ============================================================================
//		IDNSProcess Interface
// ============================================================================

class IDNSProcess
{
public:
	virtual void	LogMessage( const CPeekString& inMessage ) = 0;
};


// ============================================================================
//		OmniDNS
// ============================================================================
namespace OmniDNS
{
	enum
	{
		kActionUnknown = 0,
		kActionParsePacket,
		kActionDNSProcess,
		kActionFindAddress,
		kActionParseDNS,
		kActionQuestionName,
		kActionGetResolved,
		kActionResolveNames,
		kActionGetAddresses
	};
}

// =============================================================================
//		CDNSProcess
// =============================================================================

class CDNSProcess
{

protected:
	COptions		m_Options;

	UInt32			m_nAction;
	IDNSProcess*	m_pOwningProcess;

protected:
	void			CheckDefaults() const {}
	void			Copy( const CDNSProcess& inOther ) {
		m_Options = inOther.m_Options;
	}

	bool			IsEqual( const CDNSProcess& inOther ) {
		return ( m_Options == inOther.m_Options );
	}

	bool			ProcessDnsPacket( CLayerUDP& inLayerUDP );

public:
	static CPeekString&	GetTagForensics() { return COptions::GetTagForensics(); }

public:
	;				CDNSProcess() : m_pOwningProcess( NULL ) {}
	;				CDNSProcess( const CDNSProcess& inOther ) { Copy( inOther ); }
	virtual			~CDNSProcess() {}

	CDNSProcess&	operator=( const CDNSProcess& inOther ) {
		if ( this != &inOther ) {
			Copy( inOther );
		}
		return *this;
	}
	bool			operator==( const CDNSProcess& inOther ) { return m_Options.operator==( inOther.m_Options ); }
	bool			operator!=( const CDNSProcess& inOther ) { return m_Options.operator!=( inOther.m_Options ); }

	CIpAddressList&	GetActiveList() { return m_Options.GetActiveList(); }
	CDomainList&	GetDomainList() { return m_Options.GetDomainList(); }
	COptions&		GetOptions() { return m_Options; }

	bool			IsTrackIP() const { return m_Options.IsTrackIP(); }

	void			LogError( const CPeekString& inMessage );

	int				ProcessPacket( CPacket&	inPacket );

	void			Reset() { m_Options.Reset(); }

	void			SetOptions( const COptions& inOptions ) { m_Options = inOptions; }
	void			SetOwningProcess( IDNSProcess* inOwningProcess ) { m_pOwningProcess = inOwningProcess; }
	void			SetTrackIP( bool inTrackIP ) { m_Options.SetTrackIP( inTrackIP ); }
	void			SetVersion( CPeekString& inVersion ) { m_Options.SetVersion( inVersion ); }
};
