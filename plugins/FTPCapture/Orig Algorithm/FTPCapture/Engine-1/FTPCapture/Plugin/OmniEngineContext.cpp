// =============================================================================
//	OmniEngineContext.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OmniEngineContext.h"
#include "RemotePlugin.h"
#include "MemUtil.h"
#include "Packet.h"
#include "PacketArray.h"
#include "PeekMessage.h"
#include "FileEx.h"


class CRemotePlugin;

static CPeekString	s_strPacketsProcessed( L"Packets Processed" );

static CPeekString	g_strActions[COmniEngineContext::kAction_Max] = {
	L"login",	// kAction_Logon
	L"RETR",	// kAction_Retrieve
	L"STOR"		// kAction_Store
};

static CPeekStringA g_strFtpCmdsA[COmniEngineContext::kFtpCmd_Max] = {
	"RETR",		// kFtpCmd_Retrieve
	"STOR"		// kFtpCmd_Store
};

const UInt32	kMonitorTime( kMillisecondsInASecond * kSecondsInAMinute * 5 ); 	// 5 minute timeout


// =============================================================================
//		COmniEngineContext
// =============================================================================

COmniEngineContext::COmniEngineContext(
	CGlobalId&		inId,
	CRemotePlugin*	/*inPlugin*/ )
	:	CPeekEngineContext( inId )
{
}

COmniEngineContext::~COmniEngineContext()
{
}


// -----------------------------------------------------------------------------
//		OnCreateContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnCreateContext()
{
	int	nResult = CPeekEngineContext::OnCreateContext();
	// IsGlobalContext() return true if this is NOT a capture or file context.

	InitFtp();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnDisposeContext
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnDisposeContext()
{
	int	nResult = CPeekEngineContext::OnDisposeContext();

	m_Stats.Reset();

	Clean();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnFilter
//		: Implements IFilterPacket::FilterPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnFilter(
	CPacket&	inPacket,
	UInt16*		/*outBytesToAccept*/,
	CGlobalId&	/*outFilterId*/ )
{
	try {
		if ( inPacket.IsError() ) {
			return PEEK_PLUGIN_FAILURE;
		}

		CLayerEthernet layerEthernet;
		if ( !inPacket.GetLayer( layerEthernet ) ) {
			return PEEK_PLUGIN_FAILURE;
		}

		CLayerIP layerIP;
		if ( !inPacket.GetLayer( layerIP ) ) {
			return PEEK_PLUGIN_FAILURE;
		}

		CLayerTCP layerTCP;
		if( !inPacket.GetLayer(layerTCP) ) {
			return PEEK_PLUGIN_FAILURE;
		}

		CIpAddressPortPair IpPortPair;

		IpPortPair.SetSrcIpAddress( layerIP.GetSource() );
		IpPortPair.SetSrcPort( layerTCP.GetSource() );
		IpPortPair.SetDstIpAddress( layerIP.GetDestination() );
		IpPortPair.SetDstPort( layerTCP.GetDestination() );

	  #define NO_WPDNX
	  #ifdef _DEBUG
		#ifdef NO_WPDNX
			// petertest - for internal testing, rule out WPDNX packets:
			if ( (UInt16)IpPortPair.GetSrcPort() == 6367 || (UInt16)IpPortPair.GetDstPort() == 6367 ) {
				return PEEK_PLUGIN_FAILURE;
			}
		#endif
	  #endif

		CByteVectRef	IpHdr( layerIP.GetHeaderRef() );
		CByteVectRef	TcpHdr( layerTCP.GetHeaderRef() );

	  #ifdef _NEW_ALGORITHM_1
		CByteVectRef	FtpData( layerTCP.GetDataRef() );
	  #else // _NEW_ALGORITHM_1
		CByteVectRef	FtpData = CFtpSession::GetFtpData( IpHdr, TcpHdr );
	  #endif // _NEW_ALGORITHM_1

		COptions& theOptions = GetOptions();

		const bool bAlerting = theOptions.IsAlerting();
		const bool bObscuring = theOptions.IsObscuringNames();

  #ifdef _NEW_ALGORITHM_2
		const bool	bFtpPortOfInterest = theOptions.IsFtpPortOfInterest( IpPortPair );

		if ( bFtpPortOfInterest ) {
			CFtpSession*		pFtpSession = m_FtpSessions.Find( IpPortPair );

			if ( pFtpSession != NULL ) {
				CPeekString	strMessage;
				bool		bResult( false );

				bResult = pFtpSession->ProcessCmd( IpHdr, TcpHdr, FtpData, bObscuring, strMessage );
				m_Log.AddMessage( strMessage );

				{
					const char*		pFtpData( reinterpret_cast<const char*>( FtpData.GetData( sizeof( UInt32 ) ) ) );

					if ( pFtpData ) {
						CPeekStringA	strCmd( pFtpData, sizeof( UInt32 ) );
						if ( bAlerting ) {
							if ( strCmd.CompareNoCase( g_strFtpCmdsA[kFtpCmd_Retrieve] ) == 0 ) {
								CPeekStringA	strCmdA = static_cast<CPeekStringA>( FtpData );
								CPeekStringA	strFileNameA = strCmdA.Right( strCmdA.GetLength() - sizeof( UInt32 ) );
								CPeekString		strFileName( strFileNameA.Trim() );
								ReportAlert( kAction_Retrieve, pFtpSession, strFileName );
							}
							else if ( strCmd.CompareNoCase( g_strFtpCmdsA[kFtpCmd_Store] ) == 0 ) {
								CPeekStringA	strCmdA = static_cast<CPeekStringA>( FtpData );
								CPeekStringA	strFileNameA = strCmdA.Right( strCmdA.GetLength() - sizeof( UInt32 ) );
								CPeekString		strFileName( strFileNameA.Trim() );
								ReportAlert( kAction_Store, pFtpSession, strFileName );
							}
						}
					}
				}

				if ( pFtpSession->IsDataChannelPortOpen() ) {
					CIpAddressPort	IpAddressPort;
					CPeekString		strUserName;
					pFtpSession->GetDataChannelInfo( bObscuring, IpAddressPort, strUserName );

					CPort	PortOfInterest( IpAddressPort.GetPort() );
					CIpAddressPort	Addr1( IpPortPair.GetSrcAddress(), PortOfInterest );
					CIpAddressPort	Addr2( IpPortPair.GetDstAddress(), PortOfInterest );
					CIpAddressPortPair		IpPair( Addr1, Addr2 );

					m_DataChannels.Add( new CDataChannel( IpAddressPort, IpPair, strUserName, pFtpSession ) );
					pFtpSession->SetDataIpPortPair( IpPair );

					CPeekOutString strMsg;
					strMsg << L"User " << strUserName << L" - Data channel opened " << IpAddressPort.Format();
					m_Log.AddMessage( strMsg );
				}

				if ( pFtpSession->HasDataChannelClosed() ) {
					CIpAddressPortPair IpPair;
					if ( pFtpSession->GetDataIpAddressPort( IpPair ) ) {
						bool bDummy( false );
						CDataChannel*	pDataChannel = m_DataChannels.Find( IpPair, bDummy );
						if ( pDataChannel != NULL ) {
							CPeekOutString	strMsg;
							strMsg	<< "User " << pDataChannel->GetUserName( bObscuring ) 
								<< L" - Data channel closed " << pDataChannel->GetIpPort().Format();
							m_Log.AddMessage( strMsg );
							m_DataChannels.RemoveAndDelete( pDataChannel );
						}
					}
				}

				if ( pFtpSession->IsSessionClosed() ) {
					CPeekOutString	strMsg;
					strMsg << L"Stopped Monitoring " << pFtpSession->GetUserName( bObscuring );
					m_Log.AddMessage( strMsg );

					m_FtpSessions.RemoveAndDelete( pFtpSession );

					m_Stats.Increment( CPluginStatistics::kStat_Logoffs );
				}

				return ( ( bResult == true ) ? PEEK_PLUGIN_SUCCESS : PEEK_PLUGIN_FAILURE );
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
						pFtpSession = new CFtpSession( IpPortPair, strUserName );
						ASSERT( pFtpSession );
						if ( !pFtpSession ) Peek::Throw( HE_E_OUT_OF_MEMORY );
						m_FtpSessions.Add( pFtpSession );

						if ( bAlerting ) {
							CPeekString	strNoFileName;
							ReportAlert( kAction_Logon, pFtpSession, strNoFileName );
						}
						m_Stats.Increment( CPluginStatistics::kStat_Logons );
						return PEEK_PLUGIN_SUCCESS;
					}
				}
			}
		}
		
		const bool	bDataPortOfInterest = m_DataChannels.IsDataPortOfInterest( IpPortPair.GetSrcPort(), IpPortPair.GetDstPort() );
		if ( bDataPortOfInterest ) {
			bool bIsHalfFind( false );
			CDataChannel*			pDataChannel = m_DataChannels.Find( IpPortPair, bIsHalfFind );
			if ( pDataChannel != NULL ) {
				bool bResult = pDataChannel->ProcessData();

				if ( bIsHalfFind ) {
					CFtpSession*	pFtpSession = pDataChannel->GetSession();
					ASSERT( pFtpSession );
					if ( pFtpSession ) {
						pFtpSession->SetDataIpPortPair( IpPortPair );
					}
				}

				ASSERT( pDataChannel->IsChannelOpen() == true );
				return ( ( bResult == true ) ? PEEK_PLUGIN_SUCCESS : PEEK_PLUGIN_FAILURE );
			}
		}
  #else // _NEW_ALGORITHM_2
		CFtpSession*		pFtpSession = m_FtpSessions.Find( IpPortPair );

		if ( pFtpSession != NULL ) {
			CPeekString	strMessage;
			bool		bResult( false );

			bResult = pFtpSession->ProcessCmd( IpHdr, TcpHdr, FtpData, bObscuring, strMessage );
			m_Log.AddMessage( strMessage );

			{
				const char*		pFtpData( reinterpret_cast<const char*>( FtpData.GetData( sizeof( UInt32 ) ) ) );

				if ( pFtpData ) {
					CPeekStringA	strCmd( pFtpData, sizeof( UInt32 ) );
					if ( bAlerting ) {
						if ( strCmd.CompareNoCase( g_strFtpCmdsA[kFtpCmd_Retrieve] ) == 0 ) {
							CPeekStringA	strCmdA = static_cast<CPeekStringA>( FtpData );
							CPeekStringA	strFileNameA = strCmdA.Right( strCmdA.GetLength() - sizeof( UInt32 ) );
							CPeekString		strFileName( strFileNameA.Trim() );
							ReportAlert( kAction_Retrieve, pFtpSession, strFileName );
						}
						else if ( strCmd.CompareNoCase( g_strFtpCmdsA[kFtpCmd_Store] ) == 0 ) {
							CPeekStringA	strCmdA = static_cast<CPeekStringA>( FtpData );
							CPeekStringA	strFileNameA = strCmdA.Right( strCmdA.GetLength() - sizeof( UInt32 ) );
							CPeekString		strFileName( strFileNameA.Trim() );
							ReportAlert( kAction_Store, pFtpSession, strFileName );
						}
					}
				}
			}

			if ( pFtpSession->IsDataChannelOpen() ) {
				CIpAddressPort	IpAddressPort;
				CPeekString		strUserName;
				pFtpSession->GetDataChannelInfo( bObscuring, IpAddressPort, strUserName );

				CIpAddressPortPair		IpPair( IpAddressPort, IpAddressPort );
				m_DataChannels.Add( new CDataChannel( IpAddressPort, IpPair, strUserName ) );

				CPeekOutString strMsg;
				strMsg << L"User " << strUserName << L" - Data channel opened " << IpAddressPort.Format();
				m_Log.AddMessage( strMsg );
			}

			if ( pFtpSession->IsSessionClosed() ) {
				CPeekOutString	strMsg;
				strMsg << L"Stopped Monitoring " << pFtpSession->GetUserName( bObscuring );
				m_Log.AddMessage( strMsg );

				m_FtpSessions.RemoveAndDelete( pFtpSession );

				m_Stats.Increment( CPluginStatistics::kStat_Logoffs );
			}

			return ( ( bResult == true ) ? PEEK_PLUGIN_SUCCESS : PEEK_PLUGIN_FAILURE );
		}

		CDataChannel*			pDataChannel = m_DataChannels.Find( IpPortPair );
		if ( pDataChannel != NULL ) {
			CPeekString	strMessage;
			bool		bResult;

			bResult = pDataChannel->ProcessData( TcpHdr );
			m_Log.AddMessage( strMessage );

			if ( pDataChannel->IsChannelClosed() ) {
				CPeekOutString	strMsg;
				strMsg	<< "User " << pDataChannel->GetUserName( bObscuring ) 
					<< L" - Data channel closed " << pDataChannel->GetIpPort().Format();
				m_Log.AddMessage( strMsg );

				m_DataChannels.RemoveAndDelete( pDataChannel );
			}

			return ( ( bResult == true ) ? PEEK_PLUGIN_SUCCESS : PEEK_PLUGIN_FAILURE );
		}

		bool	bPortOfInterest = theOptions.IsFtpPortOfInterest( IpPortPair );
		if ( bPortOfInterest ) {
			bool	bIsUserCommand = CFtpSession::IsUserCmd( FtpData );
			if ( bIsUserCommand ) {
				bool			bResult;
				CByteVectRef	UserName;
				bResult = CFtpSession::ParseCmdArgument( FtpData, UserName );
				if ( bResult ) {
					CPeekString	strUserName( UserName );
					bool	bNameOfInterest = theOptions.IsNameOfInterest( strUserName );
					if ( bNameOfInterest ) {
						pFtpSession = new CFtpSession( IpPortPair, strUserName );
						ASSERT( pFtpSession );
						if ( pFtpSession ) m_FtpSessions.Add( pFtpSession );
						if ( bAlerting ) {
							CPeekString	strNoFileName;
							ReportAlert( kAction_Logon, pFtpSession, strNoFileName );
						}
						m_Stats.Increment( CPluginStatistics::kStat_Logons );
						return PEEK_PLUGIN_SUCCESS;
					}
				}
			}
		}
  #endif // _NEW_ALGORITHM_2

	}
	catch(...) {
		ASSERT( 0 );
	}

	return PEEK_PLUGIN_FAILURE;
}


// -----------------------------------------------------------------------------
//		OnNotify
//		: Implements INotify::Notify
// ----------------------------------------------------------------------------- 

int
COmniEngineContext::OnNotify(
	 const CGlobalId&	inContextId, 
	 const CGlobalId&	inSourceId,
	 UInt64				inTimeStamp,
	 PeekSeverity		inSeverity,
	 const CPeekString& inShortMessage,
	 const CPeekString& inLongMessage )
{
	inContextId;
	inSourceId;
	inTimeStamp;
	inSeverity;
	inShortMessage;
	inLongMessage;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPacket
//		: Implements IProcessPacket::ProcessPacket
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPacket(
	CPacket&	inPacket )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inPacket;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessPluginMessage
//		: Implements IProcessPluginMessage::ProcessPluginMessage
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessPluginMessage(
	CPeekStream& inMessage,
	CPeekStream& outResponse )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	inMessage;
	outResponse;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnProcessTime
//		: Implements IProcessTime::ProcessTime
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnProcessTime(
	UInt64	/*inCurrentTime*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

  #ifdef IMPLEMENT_MONITORING
	DoMonitor( kMonitorTime );
  #endif 

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnReset
//		: Implements IResetProcessing::ResetProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnReset()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnResetCapture
//		: Implements IPacketProcessorEvents::OnResetPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnResetCapture()
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStartCapture
//		: Implements IPacketProcessorEvents::OnStartPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStartCapture(
	UInt64	/*inStartTime*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	m_Stats.Reset();

	return nResult;
}


// -----------------------------------------------------------------------------
//		OnStopCapture
//		: Implements IPacketProcessorEvents::OnStopPacketProcessing
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnStopCapture(
	UInt64	/*inStopTime*/ )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	Clean();

	return nResult;
}


#ifdef IMPLEMENT_ALL_UPDATESUMMARY
// -----------------------------------------------------------------------------
//		OnSummary
//		: Implements IUpdateSummaryStats::UpdateSummaryStats
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnSummary()
{
	int	nResult	= PEEK_PLUGIN_SUCCESS;

	m_Log.PostMessages();

	const CPeekString& strCaptureName = GetCaptureName();

	for ( size_t i = 0; i < CPluginStatistics::kStat_Maximum; i++ ) {
		UInt32	nStat;
		CPeekString	strLabel;
		if ( m_Stats.Get( i, nStat, strLabel ) ) {
			DoSummaryModifyEntry(
				strLabel,
				strCaptureName,
				(kSummaryType_OtherCount | kSummarySize_UInt32),
				(void *) &nStat );
		}
	}

	return nResult;
}
#endif // IMPLEMENT_ALL_UPDATESUMMARY


// -----------------------------------------------------------------------------
//		OnContextPrefs
// -----------------------------------------------------------------------------

int
COmniEngineContext::OnContextPrefs(
	CPeekDataModeler& ioPrefs )
{
	CPeekContext::OnContextPrefs( ioPrefs );

	m_Options.Model( ioPrefs );

	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ReportAction
// -----------------------------------------------------------------------------

void
COmniEngineContext::ReportAlert(
	int				inAction,
	CFtpSession*	inSession,
	CPeekString		inFileName )
{
	bool		bObscuring = false;
	CPeekString		strSource;
	CPeekString		strFacility;
	CPeekString		strOutputDirectory;
	{
		COptions& theOptions( GetOptions() );

		bObscuring = theOptions.IsObscuringNames();
		strSource = theOptions.GetAlerting().GetSource();
		strFacility = theOptions.GetAlerting().GetFacility();
		strOutputDirectory = theOptions.GetAlerting().GetOutputDirectory();
	}

	CPeekString		strAction = g_strActions[inAction];
	CPeekString		strTimeStamp = CPeekTime::GetTimeString();
	CPeekString		strTarget = inSession->GetUserName( bObscuring );
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
		CIpAddressPort	apServer = inSession->GetServerIpAddressPort();
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
COmniEngineContext::DoMonitor(
	DWORD dwTimeOut )
{
	const bool		bObscuring = GetOptions().IsObscuringNames();

	CArrayString	FtpRemovedList;
	CArrayString	DataRemovedList;

	m_FtpSessions.Monitor( dwTimeOut, bObscuring, FtpRemovedList );
	m_Stats.Increment( CPluginStatistics::kStat_TimedOutControl, FtpRemovedList.GetCount() );
	m_Log.AddTimedOutMessages( FtpRemovedList );

	m_DataChannels.Monitor( dwTimeOut, bObscuring, DataRemovedList );
	m_Stats.Increment( CPluginStatistics::kStat_TimedOutData, DataRemovedList.GetCount() );
	m_Log.AddTimedOutMessages( DataRemovedList );
}
#endif // IMPLEMENT_MONITORING
