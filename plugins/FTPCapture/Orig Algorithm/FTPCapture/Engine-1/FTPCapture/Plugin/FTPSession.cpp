// =============================================================================
//	FtpSession.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#include "StdAfx.h"
#include "FtpSession.h"
#include "PeekTime.h"


// -----------------------------------------------------------------------------
//		Static
// -----------------------------------------------------------------------------

CPeekString	CFtpSession::g_strObscureName = _T( "******" );
CPeekString	CFtpSession::g_strObscureFile = _T( "******" );


// =============================================================================
//      CFtpSession
// =============================================================================

CFtpSession::CFtpSession(
	 const CIpAddressPortPair&	inIpPortPair,
	 const CPeekString&			inUserName )
	 :	m_State( kState_UserSubmitted )
	 ,	m_IpPortPair( inIpPortPair )
	 ,	m_strUserName( inUserName )
	 ,	m_nDataChannelState( kData_Closed )
	 ,	m_LastPacketTime( 0 )
	 ,	m_pFtpSessionList( NULL )
{
	m_LastPacketTime = GetTickCount64();
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

void
CFtpSession::Close() {
	if ( m_pFtpSessionList ) {
		m_pFtpSessionList->RemoveAll();
	}
}


// -----------------------------------------------------------------------------
//		GetClientIpAddressPort
// -----------------------------------------------------------------------------

CIpAddressPort
CFtpSession::GetClientIpAddressPort() const
{
	if ( static_cast<UInt16>( m_IpPortPair.GetSrcPort() ) == 21 ) {
		return m_IpPortPair.GetDestination();
	}
	if ( static_cast<UInt16>( m_IpPortPair.GetDstPort() ) == 21 ) {
		return m_IpPortPair.GetSource();
	}
	if ( m_IpPortPair.GetSrcPort() < m_IpPortPair.GetDstPort() ) {
		return m_IpPortPair.GetDestination();
	}

	return m_IpPortPair.GetSource();
}


// -----------------------------------------------------------------------------
//		GetServerIpAddressPort
// -----------------------------------------------------------------------------

CIpAddressPort
CFtpSession::GetServerIpAddressPort() const
{
	if ( static_cast<UInt16>( m_IpPortPair.GetSrcPort() ) == 21 ) {
		return m_IpPortPair.GetSource();
	}
	if ( static_cast<UInt16>( m_IpPortPair.GetDstPort() ) == 21 ) {
		return m_IpPortPair.GetDestination();
	}
	if ( m_IpPortPair.GetSrcPort() < m_IpPortPair.GetDstPort() ) {
		return m_IpPortPair.GetSource();
	}

	return m_IpPortPair.GetDestination();
}


// -----------------------------------------------------------------------------
//		IsCloseCmd
// -----------------------------------------------------------------------------

bool
CFtpSession::IsCloseCmd(
	UInt8	inTcpFlags )
{
	if ( (inTcpFlags & kTCPFlag_FIN) ||
		//(inTcpFlags & kTCPFlag_SYN) ||
		(inTcpFlags & kTCPFlag_RST) ) {
			return true;
	}
	return false;
}


// -----------------------------------------------------------------------------
//      IsUserCmd
// -----------------------------------------------------------------------------

bool
CFtpSession::IsUserCmd(
	CByteVectRef&	inFtpData )
{
	const char*	pData = (const char*) inFtpData.GetData( 4 );
	if ( pData ) {
		if ( _strnicmp( pData, "USER", 4 ) == 0 ) {
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//      IsPasswordCmd
// -----------------------------------------------------------------------------

bool
CFtpSession::IsPasswordCmd(
	CByteVectRef&	inFtpData )
{
	const char*	pData = (const char*) inFtpData.GetData( 4 );
	if ( pData ) {
		if ( _strnicmp( pData, "PASS", 4 ) == 0 ) {
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//      IsQuitCmd
// -----------------------------------------------------------------------------

bool
CFtpSession::IsQuitCmd(
	CByteVectRef&	inFtpData )
{
	const char*	pData = (const char*) inFtpData.GetData( 4 );
	if ( pData ) {
		if ( _strnicmp( pData, "QUIT", 4 ) == 0 ) {	// Logout
			return true;
		}
		if ( _strnicmp( pData, "REIN", 4 ) == 0 ) {	// Reinitialize
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//      ParseCmdArgument
// -----------------------------------------------------------------------------

bool
CFtpSession::ParseCmdArgument(
	CByteVectRef&	inFtpData,
	CByteVectRef&	outArgument )
{
	// check for: XXXX Argument<cr><lf>
	// i.e. minimum of 1 character argument.
	const UInt8*	pData = (const UInt8*) inFtpData.GetData( 8 );
	if ( pData == NULL ) return false;

	UInt16			nCount = static_cast<UInt16>(inFtpData.GetCount()) - 5;	// 4 char Cmd + 1 space.
	const UInt8*	pBegin = pData + 5;
	const UInt8*	pEnd = pData + 5;

	while ( nCount > 0 ) {
		if ( !((*pEnd >= 0x20) && (*pEnd <= 0x7E)) ) {
			break;
		}
		nCount--;
		pEnd++;
	}

	outArgument.Set( (pEnd - pBegin), pBegin );

	return true;
}


// -----------------------------------------------------------------------------
//      ParseFtpCmd
// -----------------------------------------------------------------------------

CFtpSession::Ftp_Cmd
CFtpSession::ParseFtpCmd(
	CByteVectRef&	inFtpData )
{
	const char*	pData = (const char*) inFtpData.GetData( 4 );
	if ( pData ) {
		if ( _strnicmp( pData, "RETR", 4 ) == 0 ) {	// Retrieve
			return kCmd_Retrieve;
		}
		if ( _strnicmp( pData, "STOR", 4 ) == 0 ) {	// Store
			return kCmd_Store;
		}
		if ( _strnicmp( pData, "STOU", 4 ) == 0 ) {	// Store Unique
			return kCmd_StoreUnique;
		}
		if ( _strnicmp( pData, "PORT", 4 ) == 0 ) {	// Data Port
			return kCmd_Port;
		}
	  #ifdef _ALGORITHM_EXTENDED
		if ( _strnicmp( pData, "EPRT", 4 ) == 0 ) {	// Passive
			return kCmd_EPort;
		}
	  #endif // _ALGORITHM_EXTENDED
		if ( _strnicmp( pData, "PASV", 4 ) == 0 ) {	// Passive
			return kCmd_Passive;
		}
		UInt32	nResponseCode;
		if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
			return kCmd_ResponseCode;
		}
	}

	return kCmd_Unknown;
}


// -----------------------------------------------------------------------------
//      ParsePortArgument
// -----------------------------------------------------------------------------

bool
CFtpSession::ParsePortArgument(
	CByteVectRef&	inArgument,
	CIpAddressPort&	outIpPort )
{
	CPeekString	str( inArgument.GetData( inArgument.GetCount() ) );
	size_t		nPos = 0;
	UInt32		nIpAddress = 0;
	for ( int i = 0; i < 4; i++ ) {
		CPeekString	strValue = str.Tokenize( L",", nPos );
		nIpAddress = (nIpAddress << 8) | ((UInt8) (_tstoi( strValue ) & 0xFF));
	}
	UInt16	nPort = 0;
	{
		CPeekString	strValue = str.Tokenize( L",", nPos );
		nPort = (UInt16) _tstoi( strValue );
		nPort <<= 8;
		strValue = str.Tokenize( L",", nPos );
		nPort |= (UInt8) _tstoi( strValue );
	}
	outIpPort.SetIpAddress( nIpAddress );
	outIpPort.SetPort( nPort );
	return true;
}


#ifdef _ALGORITHM_EXTENDED
// -----------------------------------------------------------------------------
//      ParsePortArgument
// -----------------------------------------------------------------------------

bool
CFtpSession::ParseEPortArgument(
	CByteVectRef&	inArgument,
	CIpAddressPort&	outIpPort )
{
	CPeekString	str( inArgument.GetData( inArgument.GetCount() ) );

	size_t	nPos = 0;
	UInt32	nIpType( 1 );
	UInt16	nPort( 0 );

	for ( int i = 0; i < 3; i++ ) {
		CPeekString	strValue = str.Tokenize( L"|", nPos );
		if ( strValue.GetLength() == 0 ) continue;

		if ( strValue.Find( L":" ) > 0 ) {
			CIpAddress EIpAddress( strValue );
			outIpPort.SetIpAddress( EIpAddress );
		}
		else {
			if ( i == 0 ) {
				nIpType = _tstoi( strValue );
			}
			else {
				nPort = static_cast<UInt16>( _tstoi( strValue ) );
				outIpPort.SetPort( nPort );
			}
		}
	}

	return true;
}
#endif // _ALGORITHM_EXTENDED


// -----------------------------------------------------------------------------
//      ParsePassiveResponse
// -----------------------------------------------------------------------------

bool
CFtpSession::ParsePassiveResponse(
	CByteVectRef&	inArgument,
	CIpAddressPort&	outIpPort )
{
	CPeekString	str( inArgument.GetData( inArgument.GetCount() ) );
	int			iPos = str.Find( _T( '(' ) );
	if ( iPos == -1 ) return false;

	size_t nPos( iPos );

	nPos++;		// skip the '('.

	UInt32		nIpAddress = 0;
	for ( int i = 0; i < 4; i++ ) {
		CPeekString	strValue = str.Tokenize( _T(","), nPos );
		nIpAddress = (nIpAddress << 8) | ((UInt8) (_tstoi( strValue ) & 0xFF));
	}
	UInt16	nPort = 0;
	{
		CPeekString	strValue = str.Tokenize( _T(","), nPos );
		nPort = (UInt16) _tstoi( strValue );
		nPort <<= 8;
		strValue = str.Tokenize( _T(","), nPos );
		nPort |= (UInt8) _tstoi( strValue );
	}
	outIpPort.SetIpAddress( nIpAddress );
	outIpPort.SetPort( nPort );
	return true;
}


// -----------------------------------------------------------------------------
//      ParseResponseCode
// -----------------------------------------------------------------------------

bool
CFtpSession::ParseResponseCode(
	CByteVectRef&	inFtpData,
	UInt32&			outResponseCode )
{
	const char*	pData = (const char*) inFtpData.GetData( 4 );
	if ( pData ) {
		if ( !((pData[0] >= '1') && (pData[0] <= '5')) ) return false;
		if ( !((pData[1] >= '0') && (pData[1] <= '5')) ) return false;
		if ( !isdigit( pData[2] ) ) return false;
		if ( !isspace( pData[3] ) ) return false;

		outResponseCode = atoi( pData );
		return true;
	}

	return false;
}


#ifndef _NEW_ALGORITHM_1
// -----------------------------------------------------------------------------
//      GetFtpData
// -----------------------------------------------------------------------------

CByteVectRef
CFtpSession::GetFtpData(
	CByteVectRef&	inIpHdr,
	CByteVectRef&	inTcpHdr )
{
	CByteVectRef	FtpData;

	tIpV4Header*	pIpHdr = (tIpV4Header*) inIpHdr.GetData( sizeof( tIpV4Header ) );
	if ( pIpHdr ) {
		tTcpHeader*	pTcpHdr = (tTcpHeader*) inTcpHdr.GetData( sizeof( tTcpHeader ) );
		if ( pTcpHdr ) {
			UInt16	nIpLength = NETWORKTOHOST16( pIpHdr->TotalLength );
			UInt16	nIpHdrLength = (pIpHdr->VersionLength & 0x0F) * 4;
			SInt16	nIpDataLength = nIpLength - nIpHdrLength;
			UInt16	nTcpHdrLength = (pTcpHdr->Offset & 0xF0) >> 2;

			SInt16	nFtpDataLength = nIpDataLength - nTcpHdrLength;
			if ( nFtpDataLength > 0 ) {
				const UInt8*	pFtpData = inTcpHdr.GetData( 1 ) + nTcpHdrLength;
				FtpData.Set( (UInt16) nFtpDataLength, pFtpData );
			}
		}
	}

	return FtpData;
}
#endif // _NEW_ALGORITHM_1


// -----------------------------------------------------------------------------
//      ProcessCmd
// -----------------------------------------------------------------------------

bool
CFtpSession::ProcessCmd(
	CByteVectRef&	/* inIpHdr */,
	CByteVectRef&	inTcpHdr,
	CByteVectRef&	inFtpData,
	bool			inObscure,
	CPeekString&	outMessage )
{
	m_LastPacketTime = GetTickCount64();

	tTcpHeader*	pTcpHdr = (tTcpHeader*) inTcpHdr.GetData( sizeof( tTcpHeader ) );
	if ( pTcpHdr ) {
		bool	bIsClose = IsCloseCmd( pTcpHdr->Flags );
		if ( bIsClose ) {
			m_State = kState_Closed;
		}
	}

	if ( inFtpData.GetCount() ) {
		switch ( m_State ) {
		case kState_UserSubmitted:		return StateUserSubmitted( inFtpData, inObscure );
		case kState_UserAccepted:		return StateUserAccepted( inFtpData, inObscure );
		case kState_PasswordSubmitted:	return StatePasswordSubmitted( inFtpData, inObscure, outMessage );
		case kState_Open:				return StateOpen( inFtpData, inObscure, outMessage );
		case kState_Quitting:			return StateQuitting( inFtpData, inObscure, outMessage );
		case kState_Closed:				return StateClosed( inFtpData, inObscure );
		}
	}
	return true;
}


// -----------------------------------------------------------------------------
//      StateUserSubmitted
// -----------------------------------------------------------------------------

bool
CFtpSession::StateUserSubmitted(
	CByteVectRef&	inFtpData,
	bool			/*inObscure*/ )
{
	// 230: User logged in, proceed.
	// 331: User name okay, need password.
	// 332: Need account for login.
	// 421: Service not available, closing control connection.
	// 500: Syntax error, command unrecognized.
	// 501: Syntax error in parameters or arguments.
	// 530: Not logged in.

	const char*	pData = (const char*) inFtpData.GetData( 3 );
	if ( pData ) {
		if ( strncmp( pData, "331", 3 ) == 0 ) {
			m_State = kState_UserAccepted;
		}
		else if ( strncmp( pData, "230", 3 ) == 0 ) {
			m_State = kState_Open;
		}
		//else {
		//	m_State = kState_Closed;
		//}
	}
	return true;
}


// -----------------------------------------------------------------------------
//      StateUserAccepted
// -----------------------------------------------------------------------------

bool
CFtpSession::StateUserAccepted(
	CByteVectRef&	inFtpData,
	bool			/*inObscure*/ )
{
	if ( IsPasswordCmd( inFtpData ) ) {
		m_State = kState_PasswordSubmitted;
	}
	//else {
	//	m_State = kState_Closed;
	//}
	return true;
}


// -----------------------------------------------------------------------------
//      StatePasswordSubmitted
// -----------------------------------------------------------------------------

bool
CFtpSession::StatePasswordSubmitted(
	CByteVectRef&	inFtpData,
	bool			inObscure,
	CPeekString&	outMessage )
{
	// 230: User logged in, proceed.
	// 331: User name ok, need password.
	// 332: Need account for login.
	// 421: Service not available, closing control connection.
	// 500: Syntax error, command unrecognized.
	// 501: Syntax error in parameters or arguments.
	// 530: Not logged in.

	const char*	pData = (const char*) inFtpData.GetData( 3 );
	if ( pData ) {
		if ( strncmp( pData, "230", 3 ) == 0 ) {
			m_State = kState_Open;

			CPeekOutString str;
			str << L"Logon - User " << GetUserName( inObscure ) << L", " << m_IpPortPair.GetSource().Format() 
				<< L" " << m_IpPortPair.GetDestination().Format();
			outMessage = str;
		}
		else if ( strncmp( pData, "331", 3 ) == 0 ) {
			m_State = kState_UserAccepted;
		}
		//else if ( strncmp( pData, "421", 3 ) == 0 ) {
		//	m_State = kState_Closed;
		//}
		//else {
		//	m_State = kState_Closed;
		//}
	}
	return true;
}


// -----------------------------------------------------------------------------
//      StateOpen
// -----------------------------------------------------------------------------

bool
CFtpSession::StateOpen(
	CByteVectRef&	inFtpData,
	bool			inObscure,
	CPeekString&	outMessage )
{
	CPeekOutString	strMessage;

	if ( IsQuitCmd( inFtpData ) ) {
		m_State = kState_Quitting;
		return true;
	}

	CByteVectRef	bvrArgument;

	Ftp_Cmd	nCmd = ParseFtpCmd( inFtpData );
	switch ( nCmd ) {
		case kCmd_Retrieve:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				if ( m_pFtpSessionList ) m_pFtpSessionList->IncrementStatValue( 0 );
				CString	strFileName = g_strObscureFile;
				if ( !inObscure ) {
					strFileName = bvrArgument.GetData( bvrArgument.GetCount() );
				}
				strMessage << L"Retrieve - User " << GetUserName( inObscure ) << L", file " << strFileName;
			}
			break;
		case kCmd_Store:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				if ( m_pFtpSessionList ) m_pFtpSessionList->IncrementStatValue( 1 );
				CString	strFileName = g_strObscureFile;
				if ( !inObscure ) {
					strFileName = bvrArgument.GetData( bvrArgument.GetCount() );  //petertest - improve this?
				}
				strMessage << L"Store - User " << GetUserName( inObscure ) << strFileName;
			}
			break;
		case kCmd_StoreUnique:
			strMessage << L"Store Unique - User " << GetUserName( inObscure );
			break;
		case kCmd_Port:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				CIpAddressPort	IpPort;
				if ( ParsePortArgument( bvrArgument, IpPort ) ) {
					m_nDataChannelState = kData_Issued;
					m_DataIpPort = IpPort;
				}
			}
			break;
	  #ifdef _ALGORITHM_EXTENDED
		case kCmd_EPort:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				CIpAddressPort	IpPort;
				if ( ParseEPortArgument( bvrArgument, IpPort ) ) {
					m_nDataChannelState = kData_Issued;
					m_DataIpPort = IpPort;
				}
			}
			break;
	  #endif // _ALGORITHM_EXTENDED
		case kCmd_Passive:
			{
				m_nDataChannelState = kData_Passive;
			}
			break;
		case kCmd_ResponseCode:
			if ( m_nDataChannelState == kData_Issued ) {
				UInt32	nResponseCode;
				if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
					if ( nResponseCode == 200 ) {
						strMessage << L"Open Port - User " << GetUserName( inObscure ) << L", " << m_DataIpPort.Format();
					  #ifdef _NEW_ALGORITHM_2
						m_nDataChannelState = kData_PortOpen;
					  #else // _NEW_ALGORITHM_2
						m_nDataChannelState = kData_Open;
					  #endif // _NEW_ALGORITHM_2
					}
				}
			}
			else if ( m_nDataChannelState == kData_Passive ) {
				UInt32	nResponseCode;
				if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
					if ( nResponseCode == 227 ) {
						if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
							CIpAddressPort	IpPort;
							if ( ParsePassiveResponse( bvrArgument, IpPort ) ) {
								m_nDataChannelState = kData_Issued;
								m_DataIpPort = IpPort;
								strMessage	<< L"Open Port - User " << GetUserName( inObscure ) 
											<< L", " << m_DataIpPort.Format();
							  #ifdef _NEW_ALGORITHM_2
								m_nDataChannelState = kData_PortOpen;
							  #else // _NEW_ALGORITHM_2
								m_nDataChannelState = kData_Open;
							  #endif // _NEW_ALGORITHM_2
							}
						}
					}
				}
			}
		  #ifdef _NEW_ALGORITHM_2
			else if ( m_nDataChannelState == kData_Open || m_nDataChannelState == kData_PortOpen ) {
				UInt32	nResponseCode;
				if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
					if ( nResponseCode == 226 ) {
						strMessage	<< L"Data channel closed - User " << GetUserName( inObscure ) 
							<< L", " << m_DataIpPort.Format();
						m_nDataChannelState = kData_Close_Issued;
					}
				}
			}
		  #endif // _NEW_ALGORITHM_2

			break;
	}

	outMessage = strMessage;

	return true;
}


// -----------------------------------------------------------------------------
//      StateQuitting
// -----------------------------------------------------------------------------

bool
CFtpSession::StateQuitting(
	CByteVectRef&	inFtpData,
	bool			inObscure,
	CPeekString&	outMessage )
{
	CPeekOutString	strMessage;

	// Quit
	// 221: Service closing control connection.
	//      Logged out if appropriate.
	// 500: Syntax error, command unrecognized. This may include errors 
	//      such as command line too long.
	// Reinititalize
	// 120: Service ready in nnn minutes.
	// 220: Service ready for new user.
	// 421: Service not available, closing control connection.
	// 500: Syntax error, command unrecognized.
	// 502: Command not implemented.

	const char*	pData = (const char*) inFtpData.GetData( 3 );
	if ( pData ) {
		if ( strncmp( pData, "221", 3 ) == 0 ) {
			m_State = kState_Closed;
		}
		else {
			m_State = kState_Closed; // Not sure what the state is?
		}
		strMessage	<< L"Logoff - User " << GetUserName( inObscure ) << L", " << m_IpPortPair.GetSource().Format()
					<< L" " << m_IpPortPair.GetDestination().Format();
	}

	outMessage = strMessage;

	return true;
}


// -----------------------------------------------------------------------------
//      
// -----------------------------------------------------------------------------

bool
CFtpSession::StateClosed(
	CByteVectRef&	/*inFtpData*/,
	bool			/*inObscure*/ )
{
  #ifdef _NEW_ALGORITHM_1
	return true;
  #else //  _NEW_ALGORITHM_1
	return false;
  #endif //  _NEW_ALGORITHM_1
}


////////////////////////////////////////////////////////////////////////////////
//      CFtpSessionList
////////////////////////////////////////////////////////////////////////////////

CFtpSessionList::CFtpSessionList()
{
	m_Map.InitHashTable( 257 );
	m_FileStats[0] = 0;
	m_FileStats[1] = 0;
}

CFtpSessionList::~CFtpSessionList()
{
	RemoveAll();
}


// -----------------------------------------------------------------------------
//		GetStatValue
// -----------------------------------------------------------------------------

UInt32
CFtpSessionList::GetStatValue(
	size_t inIndex )
{
	switch ( inIndex ) {
		case 1:	return 	m_FileStats[0];
		case 2:	return 	m_FileStats[1];
	}
	return m_Map.GetCount();
}


// -----------------------------------------------------------------------------
//		ResetStatValue
// -----------------------------------------------------------------------------

void
CFtpSessionList::ResetStatValue(
	size_t inIndex )
{
	switch ( inIndex ) {
		case 1:	m_FileStats[0] = 0;
		case 2:	m_FileStats[1] = 0;
	}
}


// -----------------------------------------------------------------------------
//		IncrementStatValue
// -----------------------------------------------------------------------------

void
CFtpSessionList::IncrementStatValue(
	size_t inIndex )
{
	if ( inIndex < 2 ) {
		m_FileStats[inIndex]++;
	}
}


#ifdef IMPLEMENT_MONITORING
// -----------------------------------------------------------------------------
//		Monitor
// -----------------------------------------------------------------------------

void
CFtpSessionList::Monitor(
	 DWORD			dwTimeOut,
	 bool			inObscure,
	 CArrayString&	outRemovedStreams )
{
	UInt64	nCurTime =  GetTickCount64();
	POSITION	pos = m_Map.GetStartPosition();
	while ( pos != NULL ) {
		CFtpSessionMap::CPair*	pCur = m_Map.GetNext( pos );

		CIpAddressPortPair	ippKey = pCur->m_key;
		CFtpSessionPtr		FtpSession( pCur->m_value );

		DWORD	dwDelta = static_cast<DWORD>( nCurTime - FtpSession->GetLastPacketTime() );
		bool	bRemove = (dwDelta > dwTimeOut);
		if ( bRemove ) {
			CPeekOutString	strRemoved;
			strRemoved << "User " << FtpSession->GetUserName( inObscure );
			outRemovedStreams.Add( strRemoved );
			m_Map.RemoveKey( ippKey );
		}
	}
	return;
}
#endif // IMPLEMENT_MONITORING


// -----------------------------------------------------------------------------
//		Find
// -----------------------------------------------------------------------------

CFtpSession*
CFtpSessionList::Find(
	const CIpAddressPortPair&	inIpPortPair )
{
	CFtpSessionMap::CPair*	pCur = m_Map.Lookup( inIpPortPair );
	if ( pCur ) {
		return pCur->m_value.get();
	}

	CIpAddressPortPair	invIpPortPair;
	inIpPortPair.Invert( invIpPortPair );
	pCur = m_Map.Lookup( invIpPortPair );
	if ( pCur ) {
		return pCur->m_value.get();
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

bool
CFtpSessionList::Add(
	CFtpSession*			inFtpSession )
{
	if ( inFtpSession == NULL ) return false;

	m_Map.SetAt( inFtpSession->Key(), CFtpSessionPtr( inFtpSession ) );
	inFtpSession->SetFtpSessionList( this );
	return true;
}
