// =============================================================================
//	RadiusPacketItem.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc.

#include "StdAfx.h"
//#include "Radius.h"
#include "RadiusPacketItem.h"
#include "Utils.h"

#define		ATTRIB_STATUS_NAME		0x01
#define		ATTRIB_STATUS_IPADDRESS	0x02
#define		ATTRIB_STATUS_STATUS	0x04
#define		ATTRIB_ACCESS_COMPLETE ATTRIB_STATUS_NAME
#define		ATTRIB_ACCOUNTING_COMPLETE ATTRIB_STATUS_NAME | ATTRIB_STATUS_IPADDRESS | ATTRIB_STATUS_STATUS

// =============================================================================
//		CRadiusPacketItem
// =============================================================================


// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

CRadiusPacketItem::CRadiusPacketItem()
	:	m_bIsRadius( false )
//	,	m_uSrcAddr( 0 )
//	,	m_uDstAddr( 0 )
	,	m_uRadiusPort( 0 )
	,	m_uFramedIPAddress( 0 )
	,	m_LastUpdateTime( 0 )
	,	m_uCode( 0 )
	,	m_uStatus( 0 )
	,	m_uMonitorting( MONITORING_NO_ACTION)
	,	m_uAccessIdentifier( 0 )
	,	m_uAccountingIdentifier( 0 )
//	,	m_bDataWasAccepted( false )
//	,	m_bProcessRadius( false )
{
}


// -----------------------------------------------------------------------------
//		GetIPAndRadiusInfo
// -----------------------------------------------------------------------------

bool
CRadiusPacketItem::GetIPAndRadiusInfo(
	CPacket&	inPacket )
//	const PluginPacket*		inPacket,
//	const UInt8*			inPacketData,
//	UInt8					inMediaType,
//	UInt8					inMediaSubType,
//	UInt32					inProtoSpecMatched,
//	COptions*				inOptions )
{
	inPacket;

	try {

		if ( inPacket.IsError() )					return false;

		// Retrieve the ethernet, Ip, and Udp layers.  If not any of these
		// then the packet is not of interest.
		CLayerEthernet layerEthernet;
		if ( !inPacket.GetLayer( layerEthernet ) )	return false;

		CLayerIP layerIP;
		if ( !inPacket.GetLayer( layerIP ) )		return false;

		CLayerUDP layerUDP;
		if( !inPacket.GetLayer(layerUDP) )			return false;

//		inPacket.Detach();

		CIpAddress SrcAddr = layerIP.GetSource();
		CIpAddress DstAddr = layerIP.GetDestination();

		// Get the IP Address
		m_uSrcAddr = layerIP.GetSource();
		m_uDstAddr = layerIP.GetDestination();

		// Get the port
		CPort  uSrcPort = layerUDP.GetSource();
		CPort  uDstPort = layerUDP.GetDestination();


/*
		const UInt8*	pIPHeader = NULL;
		UInt16			nIPHeaderSize = 0;
		const UInt8*	pIPData = NULL;
		UInt16			nIPDataSize = 0;
		UInt32			nDeliveryProtocol = 0;

		CPeekPlugin& thePlugin = theApp.GetPlugin();
		int	nResult = thePlugin.GetIPInfo(
			inProtoSpecMatched,
			inPacket,
			inPacketData,
			inMediaType,
			inMediaSubType,
			&pIPHeader,
			&nIPHeaderSize,
			&pIPData,
			&nIPDataSize,
			&nDeliveryProtocol );

		if ( nResult != PLUGIN_RESULT_SUCCESS ) {
			return false;
		}

		tIpHeader*	pIpHdr = (tIpHeader*) pIPHeader;
	
		// Get the IP Address
		m_uSrcAddr = NETWORKTOHOST32( pIpHdr->Source.word );
		m_uDstAddr = NETWORKTOHOST32( pIpHdr->Destination.word );

		if ( (m_uSrcAddr == 0) && (m_uDstAddr == 0) ) {
			return false;
		}

		// Get the port
		UInt16 uSrcPort = NETWORKTOHOST16( *((const UInt16*) pIPData) );
		UInt16 uDstPort = NETWORKTOHOST16( *((const UInt16*) (pIPData + 2)) );
		if ( (uSrcPort == 0) && (uDstPort == 0) ) {
			return false;
		}

		// if ( inOptions->IsRadiusPort( m_nSrcPort, m_nDstPort ) ) {
		m_uRadiusPort = inOptions->GetRadiusPort( uSrcPort, uDstPort );
		if ( m_uRadiusPort > 0 ) {
			if ( GetRadiusInfo( pIPData ) )	{
				 return true;
			}
		}
*/
	}
	catch( ... ) {
		ASSERT( 0 );
	}
/*
	catch( RediusErrorCode eCode ) {
  #ifdef _DEBUG
		CString strMsg;
		strMsg.Format( _T("Radius execption: %d"), eCode );
//		AfxMessageBox( strMsg );
  #else
		eCode;
  #endif
	}
*/
	return true;
}


// -----------------------------------------------------------------------------
//		GetRadiusInfo
// -----------------------------------------------------------------------------

bool
CRadiusPacketItem::GetRadiusInfo(
	const UInt8*	pIPData )
{
	pIPData;
/*
	UInt8	uAttribStatus = 0;

	// Check IP data
	ASSERT( pIPData != NULL );
	if( pIPData == NULL ) return false;

	// Reset
	m_bIsRadius = false;
	InitializeFramedIPAddress();

	// Do we find any RADIUS attributes
	// Beginning of Radius Info is offset 8 from the beginning of the IP data
	const UInt8*	pRadius = pIPData + 8;

	RadiusHeader Header;

	memcpy( &Header, pRadius, sizeof( RadiusHeader ) );
	Header.uLength = NETWORKTOHOST16( Header.uLength );
	if ( Header.uLength < sizeof( RadiusHeader ) ) {
		return false;
	}

	const RadiusAttribute*	pAttrib = (RadiusAttribute*) (pRadius + sizeof( RadiusHeader ));
	const RadiusAttribute*	pEnd = (RadiusAttribute*) (pRadius + Header.uLength); // or pAttrib + uLength??

	m_uCode = Header.uCode;	
	m_uIdentifier = Header.uIdentifier;

	if ( IsRadiusOfInterest() != true )
		return false;

	// Handle the different types of RADIUS attributes, fill out the RadiusInfo structure and return true
	while ( pAttrib < pEnd ) {

		// Sanity check
		if (pAttrib->uLength == 0) {
			break;
		}

		// This is so we don't get into an infinite loop
		if ( pAttrib->uAttribute == 0 ) {
			break;
		}

		switch ( pAttrib->uAttribute ) {
		case RADATT_USERNAME:  // Attribute: Username
			{
				// Length of user name.  Subtract 2 since it includes the first two octets of the attribute
				UInt8 uNameLength = pAttrib->uLength - 2;
				CStringA strName;
				strncpy( strName.GetBuffer( uNameLength ), (const char *)&pAttrib->uData, uNameLength );
				strName.ReleaseBuffer( uNameLength );
				m_strName = strName;
				uAttribStatus |= ATTRIB_STATUS_NAME;
			}
			break;

		case RADATT_FRMIPADDRESS:  // Attribute: IP Address
			{
				// Get the IP Address
				UInt32 uFramedIPAddress = NETWORKTOHOST32( *((const UInt32*) &pAttrib->uData) );

				// 0xFFFFFFFF signifies that the user selects an IP Address
				// oxFFFFFFFE signifies that the NAS selects an IP Address
				if ( (uFramedIPAddress != 0xFFFFFFFF) &&
					 (uFramedIPAddress != 0xFFFFFFFE) ) {

					// IP Address field is valid
					ASSERT( m_uFramedIPAddress == 0 );
					m_uFramedIPAddress = uFramedIPAddress;
					uAttribStatus |= ATTRIB_STATUS_IPADDRESS;
				}
			}
			break;

		case RADATT_STATUS:  // Attribute: Status
			{
				UInt32 uStatus = NETWORKTOHOST32( *((const UInt32*)&pAttrib->uData) );
				
				uAttribStatus |= ATTRIB_STATUS_STATUS;

				if ( m_uCode == RADCODE_ACCOUNTINGREQUEST ) {
					if ( uStatus == ACCOUNTING_STOP || uStatus == ACCOUNTING_OFF ) {
						SetMonitoring( MONITORING_STOP );
					}
					else if ( uStatus == ACCOUNTING_START ) {
						SetMonitoring( MONITORING_START );
					}
					else {
						SetMonitoring( MONITORING_START_NOREPORT );
					}
				}
			}
			break;

		default:
			break;
		}

		// Increment the offset by the length
		pAttrib = (RadiusAttribute*)( ((const UInt8*) pAttrib) + pAttrib->uLength );
	}

	UInt8 uAttribComplete;

	switch ( m_uCode ) {

	case RADCODE_ACCESSREQUEST:
		uAttribComplete = uAttribStatus & ATTRIB_ACCESS_COMPLETE;
		if ( (uAttribComplete) && (m_uIdentifier != 0) ) {
			m_bIsRadius = true;
			m_uStatus = RECEIVED_ACCESSREQUEST;
			m_uAccessIdentifier = m_uIdentifier;
		}
		break;

	case RADCODE_ACCESSACCEPT:
	case RADCODE_ACCESSREJECT:
	case RADCODE_ACCESSCHALLENGE:
		if ( m_uIdentifier != 0 ) {
			m_bIsRadius = true;
			m_uStatus = RECEIVED_ACCESSRESPONSE;
			m_uAccessIdentifier = m_uIdentifier;
		}
		break;

//	case RADCODE_ACCESSREJECT:
//		m_uStatus = RECEIVED_ACCESSREJECT;
//		m_uAccessIdentifier = m_uIdentifier;
//		break;

	case RADCODE_ACCOUNTINGREQUEST:
		uAttribComplete = uAttribStatus & ATTRIB_ACCOUNTING_COMPLETE;
		if ( (uAttribComplete) && (m_uIdentifier != 0) ) {
			m_bIsRadius = true;
			m_uStatus = RECEIVED_ACCOUNTINGREQUEST;
			m_uAccountingIdentifier = m_uIdentifier;

//			// For present we will accept any accounting
//			// request except accounting stop as a signal
//			// to start monitoring
//			if ( GetMonitoring() == MONITORING_NO_ACTION ) {
//				SetMonitoring( MONITORING_START_NOREPORT );
//			}

		}
		break;

	case RADCODE_ACCOUNTINGRESPONSE:
		if ( m_uIdentifier != 0 ) {
			m_bIsRadius = true;
			m_uStatus = RECEIVED_ACCOUNTINGRESPONSE;
			m_uAccountingIdentifier = m_uIdentifier;
		}
		break;

//	case RADCODE_ACCESSCHALLENGE:
//		m_uStatus = RECEIVED_ACCESSCHALLENGE;
//		m_uAccessIdentifier = m_uIdentifier;
//		break;

	default:
		break;
	}
*/
	// Return whether or not we found any Attributes that we are interested in.
	return m_bIsRadius;
}

/*
// -----------------------------------------------------------------------------
//		Update
// -----------------------------------------------------------------------------

void
CRadiusPacketItem::Update( CRadiusPacketItem& theRadiusPacketItem )
{
	ASSERT( m_bIsRadius && theRadiusPacketItem.m_bIsRadius );

	if ( m_uRadiusPort == 0 ) {
		ASSERT( theRadiusPacketItem.m_uRadiusPort > 0 );
		m_uRadiusPort = theRadiusPacketItem.m_uRadiusPort;
	}
	else {
		if ( m_uRadiusPort != theRadiusPacketItem.m_uRadiusPort ) {
			ASSERT( 0 );
			return;
		}
	}

	m_uStatus |= theRadiusPacketItem.m_uStatus;
	
	ASSERT( ( (!m_strName.IsEmpty() ) && (!theRadiusPacketItem.m_strName.IsEmpty())) ?
		(m_strName.CompareNoCase(theRadiusPacketItem.m_strName) == 0) : true );
	if ( m_strName.IsEmpty() && (!theRadiusPacketItem.m_strName.IsEmpty()) ) {
		m_strName = theRadiusPacketItem.m_strName;
	}

	ASSERT( (HasFramedIPAddr() && theRadiusPacketItem.HasFramedIPAddr()) ?
		(m_uFramedIPAddress == theRadiusPacketItem.m_uFramedIPAddress) : true );
	if ( !HasFramedIPAddr() && theRadiusPacketItem.HasFramedIPAddr() ) {
		m_uFramedIPAddress = theRadiusPacketItem.m_uFramedIPAddress;
	}

	ASSERT( m_uAccessIdentifier && theRadiusPacketItem.m_uAccessIdentifier ? 
		m_uAccessIdentifier == theRadiusPacketItem.m_uAccessIdentifier : true );
	if( (m_uAccessIdentifier == 0) && (theRadiusPacketItem.m_uAccessIdentifier != 0) ) {
		m_uAccessIdentifier = theRadiusPacketItem.m_uAccessIdentifier;
	}

	ASSERT( m_uAccountingIdentifier && theRadiusPacketItem.m_uAccountingIdentifier ?
		m_uAccountingIdentifier == theRadiusPacketItem.m_uAccountingIdentifier : true );
	if( (m_uAccountingIdentifier == 0) && (theRadiusPacketItem.m_uAccountingIdentifier != 0) ) {
		m_uAccountingIdentifier = theRadiusPacketItem.m_uAccountingIdentifier;
	}

	SetLastUpdateTime();
}

// -----------------------------------------------------------------------------
//		RadiusTransactionComplete
// -----------------------------------------------------------------------------

bool
CRadiusPacketItem::RadiusTransactionComplete( )
{

	const bool bAccessRequestStatus = (m_uStatus & RECEIVED_ACCESSREQUEST) == RECEIVED_ACCESSREQUEST;
	const bool bAccessAcceptStatus = (m_uStatus & RECEIVED_ACCESSRESPONSE) == RECEIVED_ACCESSRESPONSE;
	const bool bAccessComplete = bAccessRequestStatus && bAccessAcceptStatus;

	if ( bAccessComplete ) return true;

	const bool bAccountingRequestStatus = (m_uStatus & RECEIVED_ACCOUNTINGREQUEST) == RECEIVED_ACCOUNTINGREQUEST;
	const bool bAccountingResponseStatus = (m_uStatus & RECEIVED_ACCOUNTINGRESPONSE) == RECEIVED_ACCOUNTINGRESPONSE;
	const bool bAccountingComplete = bAccountingRequestStatus && bAccountingResponseStatus;

	if ( bAccountingComplete ) return true;

	return false;
}

// -----------------------------------------------------------------------------
//		IsMatch
// -----------------------------------------------------------------------------
bool
CRadiusPacketItem::IsMatch( CRadiusPacketItem& inRadiusPacket )
{
	const bool bAccessRequestReceived = (m_uStatus & RECEIVED_ACCESSREQUEST) == RECEIVED_ACCESSREQUEST;
	const bool bAccountingRequestReceived = (m_uStatus & RECEIVED_ACCOUNTINGREQUEST) == RECEIVED_ACCOUNTINGREQUEST;

	if ( IPMatch(inRadiusPacket) != true ) {
		return false;
	}

	ASSERT( inRadiusPacket.m_uRadiusPort > 0 );
	if ( m_uRadiusPort > 0 ) {
		if ( m_uRadiusPort != inRadiusPacket.m_uRadiusPort ) {
			return false;
		}
	}

	switch ( inRadiusPacket.m_uStatus ) {
	case RECEIVED_ACCESSREQUEST:
		if ( (inRadiusPacket.m_uAccessIdentifier == 0) || (inRadiusPacket.m_strName.IsEmpty()) ) {
			ASSERT( 0 );
			return false;
		}
		if ( m_uAccessIdentifier != inRadiusPacket.m_uAccessIdentifier ) {
			return false;
		}
		if ( m_strName.CompareNoCase(inRadiusPacket.m_strName) == 0 ) {
			return true;
		}
		ASSERT( 0 );
		break;

	case RECEIVED_ACCESSRESPONSE:
		if ( inRadiusPacket.m_uAccessIdentifier == 0 ) {
			ASSERT( 0 );
			return false;
		}
		if ( m_uAccessIdentifier == inRadiusPacket.m_uAccessIdentifier ) {
			ASSERT( m_uAccessIdentifier );
			if ( !bAccessRequestReceived ) {
				return false;
			}
			if ( m_strName.IsEmpty() ) {
				ASSERT( 0 );
				return false;
			}
			return true;
		}
		break;

	case RECEIVED_ACCOUNTINGREQUEST:
		if ( (inRadiusPacket.m_uAccountingIdentifier == 0) || (inRadiusPacket.m_strName.IsEmpty()) || (inRadiusPacket.m_uFramedIPAddress == 0) ) {
			ASSERT( 0 );
			return false;
		}

		if ( (m_uAccountingIdentifier != 0) && (m_uAccountingIdentifier != inRadiusPacket.m_uAccountingIdentifier) ) {
			return false;
		}

		ASSERT ( inRadiusPacket.m_uAccountingIdentifier != 0 );

		if ( m_strName.CompareNoCase(inRadiusPacket.m_strName) == 0 ) {
			if ( (m_uFramedIPAddress != 0) && (m_uFramedIPAddress != inRadiusPacket.m_uFramedIPAddress) ) {
				return false;
			}
			return true;
		}
		break;

	case RECEIVED_ACCOUNTINGRESPONSE:
		if ( inRadiusPacket.m_uAccountingIdentifier == 0 ) {
			ASSERT( 0 );
			return false;
		}
		if ( m_uAccountingIdentifier == inRadiusPacket.m_uAccountingIdentifier ) {
			if ( !bAccountingRequestReceived ) {
				return false;
			}
			if ( m_strName.IsEmpty() || m_uFramedIPAddress == 0 ) {
				ASSERT( 0 );
				return false;
			}
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

CRadiusPacketItem& 
CRadiusPacketItem::Copy(
	const CRadiusPacketItem& inOriginal )
{
	if ( this != &inOriginal ) {		
		m_uSrcAddr = inOriginal.m_uSrcAddr;
		m_uDstAddr = inOriginal.m_uDstAddr;
		m_bIsRadius = inOriginal.m_bIsRadius;
		m_strName = inOriginal.m_strName;
		m_uFramedIPAddress = inOriginal.m_uFramedIPAddress;
		m_LastUpdateTime = inOriginal.m_LastUpdateTime;
		m_uIdentifier = inOriginal.m_uIdentifier;
		m_uStatus = inOriginal.m_uStatus;
		m_uAccessIdentifier = inOriginal.m_uAccessIdentifier;
		m_uAccountingIdentifier = inOriginal.m_uAccountingIdentifier;
		m_uRadiusPort = inOriginal.m_uRadiusPort;
	}

	return *this;
}
*/
