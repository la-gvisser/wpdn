// ============================================================================
// IpPenRegisterContext.cpp:
//		implementation of the CIpPenRegisterContext class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "IpPenRegisterContext.h"
#include "IpPenRegisterPlugin.h"
#include "OptionsDlg.h"
#include "Memutil.h"
#include "IpAddress.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int	MIN_UDPHDR_LEN( 8 );
const int	MIN_TCPHDR_LEN( 20 );
const int	MIN_ICMPHDR_LEN( 8 );
const int	MIN_ICMPv6HDR_LEN( 10 );
const int	MIN_DNSHDR_LEN( 10 );


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CString	kPluginName( _T( "IpPenRegister" ) );
	const CString	kLogFilename( _T( "MessageLog.txt" ) );
	const CString	kUnhandledException( _T( "IpPenRegister: Caught an unhandled exception." ) );
	const CString	kKnownPorts( _T( "Connections on known Ports" ) );
	const CString	kUnknownPorts( _T( "Connections on unknown Ports" ) );
}


// ============================================================================
//		CIpPenRegisterContext
// ============================================================================

CIpPenRegisterContext::CIpPenRegisterContext(
	CPeekApp*				/*pPeekApp*/,
	CIpPenRegisterPlugin*	pPlugin )
	:	m_pPlugin( pPlugin )
{
}


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::Init(
	PluginCreateContextParam*	ioParams )
{
	CPeekContext::Init( ioParams );

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		Term
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::Term( )
{
	CPeekContext::Term();

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		ListenToMessage
// ----------------------------------------------------------------------------

void
CIpPenRegisterContext::ListenToMessage(
	const CBLMessage&	ioMessage )
{
	switch ( ioMessage.GetId() ) {
	case kBLM_NameTableUpdate:
		{
			const CBLNameTableUpdate& NTU = dynamic_cast<const CBLNameTableUpdate&>( ioMessage );
			DoNotify( 0, kNotifySeverity_Informational, NTU.GetMessage(), NTU.GetMessage() );
		}
		break;

	case kBLM_OptionsUpdate:
		{
			const CBLOptionsUpdate& OU = dynamic_cast<const CBLOptionsUpdate&>( ioMessage );
			COptions*	pOptions = OU.GetMessage();
			if ( pOptions != NULL ) {
				;
			}
		}
		break;

	default:
		CPeekContext::ListenToMessage( ioMessage );
		break;
	}
}


// ----------------------------------------------------------------------------
//		OnReset
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnReset(
	PluginResetParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnStartCapture
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnStartCapture(
	PluginStartCaptureParam*	/*ioParams*/ )
{
	// TODO
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnStopCapture
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnStopCapture(
	PluginStopCaptureParam*	/*ioParams*/ )
{
	// TODO
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessPacket
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnProcessPacket(
	PluginProcessPacketParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//      OnGetPacketString
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnGetPacketString(
	PluginGetPacketStringParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnApply
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnApply(
	PluginApplyParam*	ioParams )
{
	if ( ioParams->fCommand == kApplyMsg_Start ) return PLUGIN_RESULT_SUCCESS;
	if ( ioParams->fCommand == kApplyMsg_End ) return PLUGIN_RESULT_SUCCESS;
	if ( ioParams->fCommand != kApplyMsg_Packet ) return -1;
	if ( ioParams->fPacket == NULL ) return -1;
    return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnSelect
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnSelect(
	PluginSelectParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnPacketsLoaded
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnPacketsLoaded(
	PluginPacketsLoadedParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnSummary
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnSummary(
	PluginSummaryParam*	/*ioParams*/ )
{
	if ( m_pPlugin && m_pPlugin->m_bFailure ) {
		m_pPlugin->m_bFailure = false;
		DoNotify( 0, kNotifySeverity_Major, Tags::kUnhandledException, Tags::kUnhandledException );
	}

	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnFilter
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnFilter(
	PluginFilterParam*	ioParams )
{
	int				nResult;
	const UInt8*	pHdrData;
	UInt16			nHdrBytes;
	const UInt8*	pData;
	UInt16			nDataBytes;
	UInt32			nDeliveryProtocol;

	CAutoPtr<CIPHeader>	pIpHeader;
	if ( GET_PSID( ioParams->fProtoSpecMatched ) == ProtoSpecDefs::IP ) {
		nResult = GetPlugin()->GetIPInfo(
			ioParams->fProtoSpecMatched,
			ioParams->fPacket,
			ioParams->fPacketData,
			ioParams->fMediaType,
			ioParams->fMediaSubType,
			&pHdrData,
			&nHdrBytes,
			&pData,
			&nDataBytes,
			&nDeliveryProtocol );
		if ( nResult < 0 ) return PLUGIN_PACKET_REJECT;

		pIpHeader.Attach( new CIPv4Header( nHdrBytes, reinterpret_cast<const tIPv4Header*>( pHdrData ) ) );
	}
	else if ( GET_PSID( ioParams->fProtoSpecMatched ) == ProtoSpecDefs::IPv6 ) {
		nResult = GetPlugin()->GetIPv6Info(
			ioParams->fProtoSpecMatched,
			ioParams->fPacket,
			ioParams->fPacketData,
			ioParams->fMediaType,
			ioParams->fMediaSubType,
			&pHdrData,
			&nHdrBytes,
			&pData,
			&nDataBytes,
			&nDeliveryProtocol );
		if ( nResult < 0 ) return PLUGIN_PACKET_REJECT;

		pIpHeader.Attach( new CIPv6Header( nHdrBytes, reinterpret_cast<const tIPv6Header*>( pHdrData ) ) );
	}
	else {
		return PLUGIN_PACKET_REJECT;
	}

	if ( (pIpHeader == NULL) || !pIpHeader->IsValid() ) {
		return PLUGIN_PACKET_REJECT;
	}

	UInt16	nOffset( sizeof( tEthernetHeader ) );
	UInt16	nIPOffset( static_cast<UInt16>( pHdrData - ioParams->fPacketData ) );
	if ( nIPOffset > sizeof( tEthernetHeader ) ) {
		// Packet is 802.3/802.5.
		nOffset = static_cast<int>( nIPOffset );

		if ( ioParams->fMediaType == kPluginPacketMediaType_802_5 ) {
			// Skip over Token Ring source routing info.
			nOffset += GetPlugin()->GetSourceRouteInfoLength( ioParams->fPacket, ioParams->fPacketData );
		}
	}

	// Reject IP fragments
	if ( pIpHeader->IsFragment() ) return PLUGIN_PACKET_REJECT;

	SInt16	nSliceLength( nOffset + pIpHeader->GetLength() );

	switch ( pIpHeader->GetPayloadProtocol() ) {
	case kProtocol_ICMP:
		nSliceLength += MIN_ICMPHDR_LEN;
		break;

	case kProtocol_ICMPv6:
		nSliceLength += MIN_ICMPv6HDR_LEN;
		break;

	case kProtocol_TCP:
		{
			const tTcpHeader*	pTCP( reinterpret_cast<const tTcpHeader*>( pIpHeader->GetPayload() ) );
			int					nTCPHeaderLength( ((pTCP->Offset & 0xF0) >> 4) * 4 );
			nSliceLength += static_cast<UInt16>( nTCPHeaderLength );
		}
		break;

	case kProtocol_UDP:
		nSliceLength += MIN_UDPHDR_LEN;
		break;

	default:  // IP (Other)
		break;
	}

	ioParams->fBytesAccepted = nSliceLength;
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetPacketAnalysis
// ----------------------------------------------------------------------------
// Called when a packet needs to be displayed in the main packet list.  Plug-ins
// will only get packets which correspond the the ProtoSpecs they supplied
// during Load (in GetSupportedProtoSpecs).

int
CIpPenRegisterContext::OnGetPacketAnalysis(
	PluginGetPacketStringParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnDecodePacket
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnDecodePacket(
	PluginDecodePacketParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessPluginMessage
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnProcessPluginMessage(
	PluginProcessPluginMessageParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnHandleNotify
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnHandleNotify(
	PluginHandleNotifyParam*	/* ioParams */ )
{
	// TODO: Add code here to handle notifications.
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetTextForPacketListCell
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnGetTextForPacketListCell(
	PluginGetTextForPacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnMeasurePacketListCell
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnMeasurePacketListCell(
	PluginMeasurePacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnDrawPacketListCell
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnDrawPacketListCell(
	PluginDrawPacketListCellParam*	/* ioParams */ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnContextOptions
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnContextOptions(
	PluginContextOptionsParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_ERROR;
}


// ----------------------------------------------------------------------------
//		OnSetContextPrefs
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnSetContextPrefs(
	PluginContextPrefsParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnGetContextPrefs
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnGetContextPrefs(
	PluginContextPrefsParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ----------------------------------------------------------------------------
//		OnProcessTime
// ----------------------------------------------------------------------------

int
CIpPenRegisterContext::OnProcessTime(
	PluginProcessTimeParam*	/*ioParams*/ )
{
	return PLUGIN_RESULT_SUCCESS;
}


// ============================================================================
//		CIpPenRegisterContextList
// ============================================================================

// ----------------------------------------------------------------------------
//		Remove
// ----------------------------------------------------------------------------

void
CIpPenRegisterContextList::Remove(
	CIpPenRegisterContext*	inContext )
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		CPeekContext*	pContext = GetAt( i );
		if ( pContext == inContext ) {
			RemoveAt( i );
			delete pContext;
			return;
		}
	}
	throw -1;
}
