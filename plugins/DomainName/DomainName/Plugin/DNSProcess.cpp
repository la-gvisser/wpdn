// =============================================================================
//	DNSProcess.cpp
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "DNSProcess.h"
#include "PeekDataModeler.h"
#include "FileEx.h"
#include "OmniEngineContext.h"
#include "DnsEntry.h"


// ============================================================================
//		Data Modeling Tags
// ============================================================================

namespace DNSProcessTags
{
	const CPeekString	kElem_Domain( L"Name" );

	const CPeekString	kElem_ResolvedNameList( L"ResolvedNameList" );
	const CPeekString	kElem_ResolvedName( L"ResolvedName" );
	const CPeekString	kElem_AddressList( L"AddressList" );

	const CPeekString	kElem_DomainCountedList( L"DomainCountedList" );
	const CPeekString	kElem_DomainCounted( L"DomainCounted" );
	const CPeekString	kElem_QueryCount( L"QueryCount" );
	const CPeekString	kElem_ResponseCount( L"ResponseCount" );
}

// =============================================================================
//		CDNSProcess
// =============================================================================

// -----------------------------------------------------------------------------
//		ProcessPacket
// -----------------------------------------------------------------------------

int
CDNSProcess::ProcessPacket(
	 CPacket&	inPacket )
{
	try {
		m_nAction = OmniDNS::kActionParsePacket;

		CLayerIP		layerIP;

		if ( inPacket.IsError() )					return PEEK_PLUGIN_FAILURE;
		if ( !inPacket.GetLayer( layerIP ) )		return PEEK_PLUGIN_FAILURE;

		// For now we codify that only IP4 is supported:
		if ( layerIP.GetVersion() != kIpVersion_4 ) return PEEK_PLUGIN_FAILURE;

		if ( inPacket.GetProtoSpecId() == ProtoSpecDefs::DNS ) {
			m_nAction = OmniDNS::kActionDNSProcess;

			CLayerUDP layerUDP;
			if( inPacket.GetLayer(layerUDP) ) {
				if ( ProcessDnsPacket( layerUDP ) ) {
					return PEEK_PLUGIN_SUCCESS;
				}
			}
		}

		if ( IsTrackIP() ) {
			m_nAction = OmniDNS::kActionFindAddress;

			CIpAddress	theSrcAddress( layerIP.GetSource() );
			CIpAddress	theDstAddress( layerIP.GetDestination() );
			size_t		nIndex( 0 );

			const CIpAddressList& theActiveList( GetActiveList() );
			if ( theActiveList.Find( theSrcAddress, nIndex ) ||
				theActiveList.Find( theDstAddress, nIndex ) ) {
					return PEEK_PLUGIN_SUCCESS;
			}
		}
	}
	catch ( std::exception& e ) {
		LogError( (CPeekString)(CPeekStringA)e.what() );

		return PEEK_PLUGIN_FAILURE;
	}
	catch ( ... ) {
		CPeekString strErrorMessage;

		switch( m_nAction )
		{
		case OmniDNS::kActionParsePacket:
			strErrorMessage = L"Failure in OnFilter attempting to parse input packet.";
			break;
		case OmniDNS::kActionDNSProcess:
			strErrorMessage = L"Failure in OnFilter processing DNS packet.";
			break;
		case OmniDNS::kActionFindAddress:
			strErrorMessage = L"Failure in OnFilter attempting to find IP address in stored list.";
			break;
		case OmniDNS::kActionQuestionName:
			strErrorMessage = L"Failure in DNS process getting the question name.";
			break;
		case OmniDNS::kActionGetResolved:
			strErrorMessage = L"Failure in DNS process attempting to resolve the name entry.";
			break;
		case OmniDNS::kActionResolveNames:
			strErrorMessage = L"Failure in DNS process attempting to resolve the domain name.";
			break;
		case OmniDNS::kActionGetAddresses:
			strErrorMessage = L"Failure in DNS process attempting to get the DNS address.";
			break;
		case OmniDNS::kActionUnknown:
		default:
			strErrorMessage = L"Unknown failure processing packet for domain name analysis.";
			break;
		}

		LogError( strErrorMessage );

		return PEEK_PLUGIN_FAILURE;
	}

	return PEEK_PLUGIN_FAILURE;
}


// ----------------------------------------------------------------------------
//		ProcessDnsPacket
// ----------------------------------------------------------------------------

bool
CDNSProcess::ProcessDnsPacket(
	CLayerUDP&		inLayerUDP )
{
	CByteVectRef	UdpData( inLayerUDP.GetRemainingRef() );
	const UInt8*	pData = UdpData.GetData( sizeof( tDnsHeader ) );
	if ( !pData ) return false;

	m_nAction = OmniDNS::kActionDNSProcess;

	ASSERT( UdpData.GetCount() >= sizeof( tDnsHeader ) );

	const tDnsHeader*	pDnsHdr = reinterpret_cast<const tDnsHeader*>( pData );

	if ( pDnsHdr->opcode != kDNS_OpCode_Query ) return false;	// Not a standard query.

#if (0)
	if ( pDnsHdr->qr ) {
		// This is a Response.
		//if ( pDnsHdr->rcode != kDNS_RCode_NoError ) return false;
		//if ( pDnsHdr->aa ) return false;	// non-Authoritative Answer.
		//if ( pDnsHdr->tc ) return false;	// The response is truncated.
	}
#endif

	CDnsEntryList		ayEntryList( *this );
	CResolvedDomainList	ayResolvedList;
	const UInt16		nDataLen = static_cast<UInt16>( UdpData.GetCount() );

	CDomainCountedList&	ayDomainCountedList( m_Options.GetDomainCountedList() );
	CResolvedDomainList& ayResolvedDomainList( m_Options.GetResolvedDomainList() );

	m_nAction = OmniDNS::kActionParseDNS;
	if ( ayEntryList.Parse( nDataLen, pDnsHdr ) <= 0 ) {
		return false;
	}

	// DNS Query
	if ( !pDnsHdr->qr ) {
		m_nAction = OmniDNS::kActionQuestionName;
		CPeekString	strQuestion = ayEntryList.GetQuestionName();
		return ayDomainCountedList.IsMatch( strQuestion, CDomainCounted::kType_Query );
	}

	// DNS Response
	m_nAction = OmniDNS::kActionGetResolved;
	ayEntryList.GetResolved( ayResolvedList );

	bool	bAccept = false;

	m_nAction = OmniDNS::kActionResolveNames;
	const size_t	nCount = ayResolvedList.GetCount();
	for ( size_t i = 0; i < nCount; i++ ) {
		CResolvedDomain&	theResolvedDomain( ayResolvedList.GetAt( i ) );
		CPeekString			strName = theResolvedDomain.GetName();

		if ( ayDomainCountedList.IsMatch( strName, CDomainCounted::kType_Response ) ) {
			if ( IsTrackIP() ) {
				ayResolvedDomainList.UpdateAddUnique( theResolvedDomain );
			}
			bAccept = true;
		}
	}

	if ( !bAccept )	return false;

	CIpAddressList& theActiveAddressList( GetActiveList() );

	if ( IsTrackIP() ) {
		m_nAction = OmniDNS::kActionGetAddresses;
		ayResolvedDomainList.GetAddresses( theActiveAddressList );
	}

	return true;
}


// ----------------------------------------------------------------------------
//		LogError
// ----------------------------------------------------------------------------

void
CDNSProcess::LogError( 
	const CPeekString& inMessage ) {
	if ( m_pOwningProcess != NULL ) {
		(reinterpret_cast<COmniEngineContext*>(m_pOwningProcess))->LogMessage( L"DomineNameOE error: " + inMessage );
	}
}
