// =============================================================================
//	FtpProcess.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniPlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#include "FileEx.h"
#include "FtpProcess.h"
#include "PeekDataModeler.h"


static CPeekString	g_strActions[CFtpProcess::kAction_Max] = {
	L"login",	// kAction_Logon
	L"RETR",	// kAction_Retrieve
	L"STOR"		// kAction_Store
};

static CPeekStringA g_strFtpCmdsA[CFtpProcess::kFtpCmd_Max] = {
	"RETR",		// kFtpCmd_Retrieve
	"STOR"		// kFtpCmd_Store
};

#ifdef IMPLEMENT_MONITORING
const UInt32	kMonitorCheckCount( 10 );
#endif // IMPLEMENT_MONITORING


// =============================================================================
//	CFtpProcess
// =============================================================================

// -----------------------------------------------------------------------------
//		FilterPacket
// -----------------------------------------------------------------------------

bool
CFtpProcess::FilterPacket(
	CPacket&	inPacket )
{
	try {

		if ( inPacket.IsError() )					return false;

		// Retrieve the ethernet, Ip, and Tcp layers.  If not any of these
		// then the packet is not of interest.
		CLayerEthernet layerEthernet;
		if ( !inPacket.GetLayer( layerEthernet ) )	return false;

		CLayerIP layerIP;
		if ( !inPacket.GetLayer( layerIP ) )		return false;

		CLayerTCP layerTCP;
		if( !inPacket.GetLayer(layerTCP) )			return false;

		// The address port pair is the addressing of our current packet. 
		// This will be investigated for possible Ftp of interest.
		CIpAddressPortPair IpPortPair;

		IpPortPair.SetSrcIpAddress( layerIP.GetSource() );
		IpPortPair.SetSrcPort( layerTCP.GetSource() );
		IpPortPair.SetDstIpAddress( layerIP.GetDestination() );
		IpPortPair.SetDstPort( layerTCP.GetDestination() );

		COptions&	theOptions = GetOptions();

		const bool	bFtpPortOfInterest = theOptions.IsFtpPortOfInterest( IpPortPair );
		const bool	bDataPortOfInterest = m_DataChannels.IsDataPortOfInterest( IpPortPair.GetSrcPort(), IpPortPair.GetDstPort() );

		if ( !bFtpPortOfInterest && !bDataPortOfInterest ) return false;

		CByteVectRef	TcpHdr( (static_cast<CLayer>(layerTCP)).GetHeaderRef() );
		CByteVectRef	FtpData( layerTCP.GetRemainingRef() );

		const bool bAlerting = theOptions.IsAlerting();

		if ( bFtpPortOfInterest ) {
			CFtpSession* pFtpSession( m_FtpSessions.Find( IpPortPair ) );
			if ( pFtpSession ) {
				bool	bResult = pFtpSession->ProcessCmd( TcpHdr, FtpData, IpPortPair );

				if ( bAlerting ) {
					const char*		pFtpData( reinterpret_cast<const char*>( FtpData.GetData( sizeof( UInt32 ) ) ) );
					if ( pFtpData ) {
						CPeekStringA	strCmd( pFtpData, sizeof( UInt32 ) );
						int				nAction( -1 );

						if		( strCmd.CompareNoCase( g_strFtpCmdsA[kFtpCmd_Retrieve] ) == 0 )	nAction = kAction_Retrieve;
						else if ( strCmd.CompareNoCase( g_strFtpCmdsA[kFtpCmd_Store] ) == 0	)		nAction = kAction_Store;

						if ( nAction >= 0 ) {
							CPeekStringA	strCmdA = static_cast<CPeekStringA>( FtpData );
							CPeekStringA	strFileNameA = strCmdA.Right( strCmdA.GetLength() - sizeof( UInt32 ) );
							CPeekString		strFileName( strFileNameA.Trim() );
							ReportAlert( nAction, pFtpSession, strFileName );
						}
					}
				}

				return bResult;
			}

			const bool	bIsUserCommand = CFtpSession::IsUserCmd( FtpData );
			if ( bIsUserCommand ) {
				bool			bResult;
				CByteVectRef	UserName;
				bResult = CFtpSession::ParseCmdArgument( FtpData, UserName );
				if ( bResult ) {
					CPeekString	strUserName( UserName );
					bool	bNameOfInterest = theOptions.IsNameOfInterest( strUserName );
					if ( bNameOfInterest ) {
						CFtpSession* pNewFtpSession( new CFtpSession( IpPortPair, strUserName, 
							reinterpret_cast<IFtpProcess*>( this ), m_Options ) );
						if ( !pNewFtpSession ) Peek::Throw( HE_E_OUT_OF_MEMORY );
						if ( bAlerting ) {
							CPeekString	strNoFileName;
							ReportAlert( kAction_Logon, pNewFtpSession, strNoFileName );
						}

						IncrementStats( CPluginStatistics::kStat_Logons );
						return true;
					}
				}
			}
		}

		if ( bDataPortOfInterest ) {
			CFtpSession* pDataChannel = m_DataChannels.Find( IpPortPair );
			if ( pDataChannel ) {
			  #ifdef IMPLEMENT_DATA_CAPTURE
				ASSERT( pDataChannel->IsDataChannelOpen() == true );
				pDataChannel->ProcessDataChannelData( inPacket, IpPortPair, TcpHdr, FtpData );
			  #else // IMPLEMENT_DATA_CAPTURE
				pDataChannel->ProcessDataChannelData( TcpHdr );
			  #endif // IMPLEMENT_DATA_CAPTURE
				return true;
			}
		}
	}
	catch(...) {
		ASSERT( 0 );
	}

	return false;
}


// -----------------------------------------------------------------------------
//		ReportAction
// -----------------------------------------------------------------------------

void
CFtpProcess::ReportAlert(
	const int			inAction,
	CFtpSession*		inSession,
	const CPeekString&	inFileName )
{
	bool			bObscuring = false;
	CPeekString		strSource;
	CPeekString		strFacility;
	CPeekString		strOutputDirectory;
	{
		COptions& theOptions( GetOptions() );

		bObscuring = theOptions.IsObscureNames();
		strSource = theOptions.GetAlerting().GetSource();
		strFacility = theOptions.GetAlerting().GetFacility();
		strOutputDirectory = theOptions.GetAlerting().GetOutputDirectory();
	}

	CPeekString		strAction = g_strActions[inAction];
	CPeekString		strTimeStamp = CPeekTime::GetTimeString();
	CPeekString		strTarget = inSession->GetUserNameString();
	CPeekString		strFileName = (bObscuring) ? CFtpSession::g_strObscureFile : inFileName;

	CPeekString		strClientIp;
	CPeekString		strClientPort;
	{
		CIpAddressPort	apClient = inSession->GetClientIpAddressPort();
		strClientIp = apClient.GetAddress().Format();
		strClientPort = apClient.GetPort().Format();
	}

	CPeekString		strServerIp;
	CPeekString		strServerPort;
	{
		CIpAddressPort	apServer = inSession->GetFtpServerIpAddressPort();
		strServerIp = apServer.GetAddress().Format();
		strServerPort = apServer.GetPort().Format();	
	}

	CPeekDataModeler	dmAlert( L"Alert", false, true );

	CPeekString			strXmlns( L"http://tempuri.org/RTNAlert.xsd" );
	dmAlert.Attribute( L"xmlns", strXmlns );

	CPeekDataElement	elemSource( L"Source", dmAlert );
	elemSource.Value( strSource );
	elemSource.End();
	CPeekDataElement	elemAlertDateTime( L"AlertDateTime", dmAlert );
	elemAlertDateTime.Value( strTimeStamp );
	elemAlertDateTime.End();
	CPeekDataElement	elemFacility( L"FacilityName", dmAlert );
	elemFacility.Value( strFacility );
	elemFacility.End();
	CPeekDataElement	elemIPAddress( L"IPAddress", dmAlert );
	elemIPAddress.Value( strClientIp );
	elemIPAddress.End();

	CPeekString strVariable;

	CPeekDataElement	elemVariables( L"Variables", dmAlert );

	CPeekDataElement	elemVarCommand( L"Variable", elemVariables );
	elemVarCommand.Value( strAction );
	strVariable = L"FTPCommand";
	elemVarCommand.Attribute( L"key", strVariable );
	elemVarCommand.End();
	CPeekDataElement	elemVarUser( L"Variable", elemVariables );
	elemVarUser.Value( strTarget );
	strVariable = L"FTPUser";
	elemVarUser.Attribute( L"key", strVariable );
	elemVarUser.End();

	if ( inAction != kAction_Logon ) {
		CPeekDataElement	elemFileName( L"Variable", elemVariables );
		elemFileName.Value( strTarget );
		strVariable = L"FTPFilename";
		elemFileName.Attribute( L"key", strVariable );
		elemVarUser.End();
	}

	CPeekDataElement	elemClientIP( L"Variable", elemVariables );
	elemClientIP.Value( strClientIp );
	strVariable = L"ClientIP";
	elemClientIP.Attribute( L"key", strVariable );
	elemClientIP.End();
	CPeekDataElement	elemClientPort( L"Variable", elemVariables );
	elemClientPort.Value( strClientPort );
	strVariable = L"ClientPort";
	elemClientPort.Attribute( L"key", strVariable );
	elemClientPort.End();
	CPeekDataElement	elemServerIp( L"Variable", elemVariables );
	elemServerIp.Value( strServerIp );
	strVariable = L"ServerIP";
	elemServerIp.Attribute( L"key", strVariable );
	elemServerIp.End();
	CPeekDataElement	elemServerPort( L"Variable", elemVariables );
	elemServerPort.Value( strServerPort );
	strVariable = L"ServerPort";
	elemServerPort.Attribute( L"key", strVariable );
	elemServerPort.End();

	elemVariables.End();

	CString		strXmlFileName = CFileEx::BuildUniqueName( strFacility, strOutputDirectory, L".rtnalert" );

	CPeekPersistFile	spFile = dmAlert.GetXml();
	if ( spFile ) spFile->Save( strXmlFileName, FALSE );

	dmAlert.End();

}


#ifdef IMPLEMENT_MONITORING
// -----------------------------------------------------------------------------
//		DoMonitor
// -----------------------------------------------------------------------------

void
CFtpProcess::DoMonitor(
	bool	bImmediate )
{
	if ( !bImmediate ) {
		if ( m_nMonitorCount++ < kMonitorCheckCount ) {
			return;
		}
		m_nMonitorCount = 0;
	}

	CArrayString	DataRemovedList;

	if ( m_DataChannels.Monitor( DataRemovedList ) ) {
		m_Stats.Increment( CPluginStatistics::kStat_TimedOutData, DataRemovedList.GetCount() );
		m_Log.AddTimedOutMessages( DataRemovedList );
	}

	CArrayString	FtpRemovedList;

	if ( m_FtpSessions.Monitor( FtpRemovedList ) ) {
		m_Stats.Increment( CPluginStatistics::kStat_TimedOutControl, FtpRemovedList.GetCount() );
		m_Log.AddTimedOutMessages( FtpRemovedList );
	}
}
#endif // IMPLEMENT_MONITORING


#ifdef IMPLEMENT_ALL_UPDATESUMMARY
// -----------------------------------------------------------------------------
//		ProcessSummary
// -----------------------------------------------------------------------------

void
CFtpProcess::ProcessSummary()
{
	m_Log.PostMessages();

	for ( size_t i = 0; i < CPluginStatistics::kStat_Maximum; i++ ) {
		size_t	nStat;
		CPeekString	strLabel;
		if ( m_Stats.Get( i, nStat, strLabel ) ) {
			UInt32	nStat32( static_cast<UInt32>( nStat ) );  // On behalf of 64 bit compile
			m_PeekContext->DoSummaryModifyEntry(
				strLabel,
				COmniPlugin::GetName(),
				(kSummaryType_OtherCount | kSummarySize_UInt32),
				(void *) &nStat32 );
		}
	}
}
#endif // IMPLEMENT_ALL_UPDATESUMMARY
