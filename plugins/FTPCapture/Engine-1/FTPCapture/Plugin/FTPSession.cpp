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

#ifdef IMPLEMENT_MONITORING
static const UInt64 kResolutionPerSecond = 1000000000ull;		// nanoseconds
static const UInt64	kMonitorTime( 5 * kSecondsInAMinute * kResolutionPerSecond ); 	// 5 minute timeout in wp time
#endif // IMPLEMENT_MONITORING


// =============================================================================
//      CFtpSession
// =============================================================================

CFtpSession::CFtpSession(
	 const CIpAddressPortPair&	inIpPortPair,
	 const CPeekString&			inUserName,
	 IFtpProcess*				inFtpProcess,
	 COptions&					inOptions )
	 :	m_SessionState( kState_UserSubmitted )
	 ,	m_FtpSessionKey( inIpPortPair )
	 ,	m_strUserName( inUserName )
	 ,	m_nDataChannelState( kData_Closed )
	 ,	m_LastFtpPacketTime( 0 )
	 ,	m_pFtpSessionList( NULL )
	 ,	m_pFtpProcess( inFtpProcess )
	 ,	m_Options( inOptions )
   #ifdef IMPLEMENT_PASSWORD_LOGGING
	 ,	m_strPassword( L"***")
   #endif // IMPLEMENT_PASSWORD_LOGGING
   #ifdef IMPLEMENT_DATA_CAPTURE
	 ,	m_pPacketArray( NULL )
	 ,	m_DataCaptureState( kDataCaptureState_None )
	 ,	m_DataCaptureType( kDataCaptureType_None )
	 ,	m_bIsLoggingToFile( false )
	 ,	m_pLogStream( NULL )
   #endif // IMPLEMENT_DATA_CAPTURE
{
	m_LastFtpPacketTime = CPeekTime::Now().i; // GetTickCount64();

	m_pFtpProcess->AddSession( this );
}


// -----------------------------------------------------------------------------
//		CloseSessionList
// -----------------------------------------------------------------------------

void
CFtpSession::CloseSessionList() {
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
	if ( static_cast<UInt16>( m_FtpSessionKey.GetSrcPort() ) == 21 ) {
		return m_FtpSessionKey.GetDestination();
	}
	if ( static_cast<UInt16>( m_FtpSessionKey.GetDstPort() ) == 21 ) {
		return m_FtpSessionKey.GetSource();
	}
	if ( m_FtpSessionKey.GetSrcPort() < m_FtpSessionKey.GetDstPort() ) {
		return m_FtpSessionKey.GetDestination();
	}

	return m_FtpSessionKey.GetSource();
}


// -----------------------------------------------------------------------------
//		GetServerIpAddressPort
// -----------------------------------------------------------------------------

CIpAddressPort
CFtpSession::GetFtpServerIpAddressPort() const
{
	if ( static_cast<UInt16>( m_FtpSessionKey.GetSrcPort() ) == 21 ) {
		return m_FtpSessionKey.GetSource();
	}
	if ( static_cast<UInt16>( m_FtpSessionKey.GetDstPort() ) == 21 ) {
		return m_FtpSessionKey.GetDestination();
	}
	if ( m_FtpSessionKey.GetSrcPort() < m_FtpSessionKey.GetDstPort() ) {
		return m_FtpSessionKey.GetSource();
	}

	return m_FtpSessionKey.GetDestination();
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


const UInt32	kMaxFilenameLen( 50 );

// -----------------------------------------------------------------------------
//      ParseFileName
// -----------------------------------------------------------------------------

bool
CFtpSession::ParseFileName(
	CByteVectRef&	inArgument,
	CPeekString&	outFileName )
{
	if ( m_Options.IsObscureNames() ) {
		outFileName = g_strObscureFile;
		return true;
	}

	outFileName = L"Unknown";

	const size_t	nSize = inArgument.GetCount();
	const UInt8*	pData = (const UInt8*) inArgument.GetData( 1 );

	if ( pData == NULL ) return false;

	const UInt8*	pBegin( pData );
	const UInt8*	pEnd( pBegin + nSize );
	UInt8*			pCurrent( const_cast<UInt8*>( pEnd ) );

	// Walk past any possible unacceptable characters
	while ( pCurrent >= pBegin ) {
		if ( (*(pCurrent) >= 0x20) && (*pCurrent <= 0x7E) ) {
			break;
		}
		pCurrent--;
	}

	if ( pCurrent <= pBegin ) return false;

	ASSERT( pCurrent >= pBegin && pCurrent <= pEnd );

	size_t			nCount( 0 );

	while ( pCurrent >= pBegin && nCount < kMaxFilenameLen ) {

		// Retrieve the final set of characters that are valid file
		// names.  This really serves to make sure we don't have any
		// Path name in the string.
		// Look for "*/:<>?\|
		if ( !FilePath::IsValidFileNameChar( static_cast<char>( *pCurrent ) ) ) {
				pCurrent++;
				break;
		}

		nCount++;

		if ( pCurrent <= pBegin || nCount >= kMaxFilenameLen ) {
			break;
		}

		pCurrent--;
	}

	if ( pCurrent < pBegin || pCurrent > pEnd ) return false;

	// Trim any leading spaces
	UInt8* pSpCur( pCurrent );
	while( *pSpCur == 0x20 ) {
		pSpCur++;
		nCount--;
	}
	pCurrent = pSpCur;

	if ( pCurrent < pBegin || pCurrent > pEnd ) return false;

	ASSERT( nCount < kMaxFilenameLen );
	ASSERT( nCount > 0 );

	// Null terminate the string
	UInt8* pTerm( pCurrent < pEnd ? (pCurrent + nCount ) : const_cast<UInt8*>( pEnd ) );
	*pTerm = 0;

	CPeekString strFileName( pCurrent );

	ASSERT( strFileName.GetLength() > 0 );
	ASSERT( nCount == strFileName.GetLength() );

	outFileName = strFileName;

	return true;
}


#ifdef IMPLEMENT_PASSWORD_LOGGING
// -----------------------------------------------------------------------------
//      ParsePassword
// -----------------------------------------------------------------------------

bool
CFtpSession::ParsePassword(
	CByteVectRef&	inFtpData,
	CPeekString&	outPassword )
{
	const char*		kPasswordKey( "PASS" );
	const size_t	kKeywordSize( strlen( kPasswordKey ) );
	const size_t	kMaxPasswordLen( 50 );

	outPassword = L"***";

	if ( m_Options.IsObscureNames() ) {
		return true;
	}

	outPassword = L"PwdUnknown";

	size_t	nSize = inFtpData.GetCount();
	UInt8*	pData = const_cast<UInt8*>( inFtpData.GetData( kKeywordSize ) );
	if ( pData ) {
		if ( _strnicmp( reinterpret_cast<char*>( pData ), kPasswordKey, kKeywordSize ) != 0 ) {
			return false;
		}
	}

	nSize -= kKeywordSize;
	pData += kKeywordSize;

	if ( pData == NULL ) return false;

	// Trim any leading spaces
	while( *pData == 0x20 && nSize > 0 ) {
		if ( nSize == 0 )	return false;
		pData++;
		nSize--;
	}

	size_t	nCount( 0 );

	const UInt8*	pBegin( pData );
	const UInt8*	pEnd( pBegin + nSize );
	UInt8*			pCurrent( const_cast<UInt8*>( pEnd ) );

	// Walk past any possible unacceptable characters
	while ( pCurrent >= pBegin ) {
		if ( (*(pCurrent) >= 0x20) && (*pCurrent <= 0x7E) ) {
			break;
		}
		pCurrent--;
	}

	if ( pCurrent <= pBegin ) return false;

	while ( pCurrent >= pBegin && nCount < kMaxPasswordLen ) {

		if ( (*(pCurrent) < 0x20) || (*pCurrent >= 0x7E) ) {
			break;
		}

		nCount++;

		if ( pCurrent <= pBegin || nCount >= kMaxPasswordLen ) {
			break;
		}

		pCurrent--;

	}

	if ( pCurrent < pBegin || pCurrent > pEnd ) return false;

	ASSERT( nCount < kMaxPasswordLen );
	ASSERT( nCount > 0 );

	// Null terminate the string
	UInt8* pTerm( pCurrent < pEnd ? (pCurrent + nCount ) : const_cast<UInt8*>( pEnd ) );
	*pTerm = 0;

	CPeekString strPassword( pCurrent );

	ASSERT( strPassword.GetLength() > 0 );
	ASSERT( nCount == strPassword.GetLength() );

	outPassword = strPassword;

	return true;
}
#endif // IMPLEMENT_PASSWORD_LOGGING


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

	outArgument.Set( (pEnd - pBegin), const_cast<UInt8*>( pBegin ) );

	return true;
}


// -----------------------------------------------------------------------------
//      ParseFtpCmd
// -----------------------------------------------------------------------------

CFtpSession::Ftp_Cmd
CFtpSession::ParseFtpCmd(
	CByteVectRef&	inFtpData )
{
	Ftp_Cmd	theCmd( kCmd_Unknown );

	const char*	pData = (const char*) inFtpData.GetData( 4 );
	if ( pData ) {
		if ( _strnicmp( pData, "RETR", 4 ) == 0 ) {			// Retrieve
			theCmd = kCmd_Retrieve;
		}
		else if ( _strnicmp( pData, "STOR", 4 ) == 0 ) {	// Store
			theCmd = kCmd_Store;
		}
		else if ( _strnicmp( pData, "STOU", 4 ) == 0 ) {	// Store Unique
			theCmd = kCmd_StoreUnique;
		}
		else if ( _strnicmp( pData, "PORT", 4 ) == 0 ) {	// Data Port
			theCmd = kCmd_Port;
		}
		else if ( _strnicmp( pData, "EPRT", 4 ) == 0 ) {	// Extended Data Port
			theCmd = kCmd_EPort;
		}
		else if ( _strnicmp( pData, "PASV", 4 ) == 0 ) {	// Passive Mode
			theCmd = kCmd_Passive;
		}
		else if ( _strnicmp( pData, "LIST", 4 ) == 0 ) {	// List files
			theCmd = kCmd_List;
		}
		else if ( _strnicmp( pData, "MLSD", 4 ) == 0 ) {	// List files
			theCmd = kCmd_List;
		}
		else {
			UInt32	nResponseCode;
			if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
				theCmd = kCmd_ResponseCode;
			}
			else {
				theCmd = kCmd_Unknown;
			}
		}
	}

	return theCmd;
}


#ifdef IMPLEMENT_DATA_CAPTURE
// -----------------------------------------------------------------------------
//      LogToFile
// -----------------------------------------------------------------------------

bool
CFtpSession::LogToFile(
	CByteVectRef&	inFtpData )
{
	ASSERT( m_SessionState == kState_Open );
	ASSERT( m_bIsLoggingToFile );
	ASSERT( m_pLogStream != NULL );

	if ( m_pLogStream == NULL ) {
		m_bIsLoggingToFile = false;
		return false;
	}

	const size_t nCount( inFtpData.GetCount() );
	const char*	pData = (const char*) inFtpData.GetData( nCount );

	UInt32 i( 0 );
	for ( ; i < nCount; i++ ) {
		if ( pData[i] < 0x0020 || pData[i] > 0x0126 ) break;
	}

	if ( i < 3 )	return false;

	char*	pString( NULL );

	try
	{
		pString = (char*)malloc( i + 2 );
		memcpy( pString, pData, i );
		pString[i++] = '\r';
		pString[i++] = '\n';

		m_pLogStream->Write( pString, i );
	}
	catch (...)
	{
		if ( IsLoggingToScreen() ){
			CPeekOutString	strMsg;
			strMsg	<< GetUserNameString() 
					<< L"Attempt to write to log file was unsuccessful" << GetFtpConnectionString();
			m_pFtpProcess->AddLogMessage( strMsg );
		}
	}

	if ( pString ) delete pString;

	return true;
}
#endif // IMPLEMENT_DATA_CAPTURE


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


// -----------------------------------------------------------------------------
//      ParseEPortArgument
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

	CPeekString	strValue = str.Tokenize( L"|", nPos );
	if ( strValue.GetLength() > 0 ) {

		// Ip type (1 == type 4, 2 == type 6)
		nIpType = _tstoi( strValue );

		// Ip address
		CPeekString	strValue = str.Tokenize( L"|", nPos );
		if ( strValue.GetLength() > 0 ) {
			if ( strValue.Find( L"." ) > 0) {
				ASSERT( nIpType == 1  );  // IpV4
				CIpAddress EIpAddress( strValue );
				outIpPort.SetIpAddress( EIpAddress );
			}
			else if ( strValue.Find( L":" ) > 0 ) {
				ASSERT( nIpType == 2 );  // IpV6
				CIpAddress EIpAddress( strValue );
				outIpPort.SetIpAddress( EIpAddress );
			}

			// Port number
			CPeekString	strValue = str.Tokenize( L"|", nPos );
			if ( strValue.GetLength() > 0 ) {
				nPort = static_cast<UInt16>( _tstoi( strValue ) );
				outIpPort.SetPort( nPort );
			}
		}
	}

	return true;
}


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


// -----------------------------------------------------------------------------
//      ProcessCmd
// -----------------------------------------------------------------------------

bool
CFtpSession::ProcessCmd(
	CByteVectRef&		inTcpHdr,
	CByteVectRef&		inFtpData,
	CIpAddressPortPair& IpPair )
{
	m_LastFtpPacketTime = CPeekTime::Now().i; // GetTickCount64();

	tTcpHeader*	pTcpHdr = (tTcpHeader*) inTcpHdr.GetData( sizeof( tTcpHeader ) );
	if ( pTcpHdr ) {
		bool	bIsClose = IsCloseCmd( pTcpHdr->Flags );
		if ( bIsClose ) {
			ProcessCloseSession();
			return true;
		}
	}

	if ( inFtpData.GetCount() ) {
		switch ( m_SessionState ) {
		case kState_UserSubmitted:		return StateUserSubmitted( inFtpData );
		case kState_UserAccepted:		return StateUserAccepted( inFtpData );
		case kState_PasswordSubmitted:	return StatePasswordSubmitted( inFtpData );
		case kState_Open:				return StateOpen( inFtpData, IpPair );
		case kState_Quitting:			return StateQuitting( inFtpData );
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//      StateUserSubmitted
// -----------------------------------------------------------------------------

bool
CFtpSession::StateUserSubmitted(
	CByteVectRef&	inFtpData )
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
			m_SessionState = kState_UserAccepted;
		}
		else if ( strncmp( pData, "230", 3 ) == 0 ) {
			m_SessionState = kState_Open;
		  #ifdef IMPLEMENT_DATA_CAPTURE
			if ( IsLogFtpCmds() ) {
				BeginLogToFile();
			}
		  #endif // IMPLEMENT_DATA_CAPTURE

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
	CByteVectRef&	inFtpData )
{
	if ( IsPasswordCmd( inFtpData ) ) {

	  #ifdef IMPLEMENT_PASSWORD_LOGGING
		CPeekString	strPassword;
		if ( ParsePassword( inFtpData, strPassword ) ) {
			m_strPassword = strPassword;
		}
	  #endif // IMPLEMENT_PASSWORD_LOGGING
		m_SessionState = kState_PasswordSubmitted;
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
	CByteVectRef&	inFtpData )
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
			m_SessionState = kState_Open;

			if ( IsLoggingToScreen() ) {
				CPeekOutString str;
			  #ifdef IMPLEMENT_PASSWORD_LOGGING
				if ( m_Options.IsIncludePassword() && m_strPassword.GetLength() > 0 ) {
					str << L"User \"" << GetUserNameString( false, false ) << L" @ " << m_strPassword << L"\"  logon" << GetFtpConnectionString(); 
				}
				else {
					str << GetUserNameString() << L"logon" << GetFtpConnectionString(); 
				}
			  #else // IMPLEMENT_PASSWORD_LOGGING
				str << GetUserNameString() << L"logon" << GetFtpConnectionString(); 
			  #endif // IMPLEMENT_PASSWORD_LOGGING

				m_pFtpProcess->AddLogMessage( str );
			}

		}
		else if ( strncmp( pData, "331", 3 ) == 0 ) {
			m_SessionState = kState_UserAccepted;
		}
		//else if ( strncmp( pData, "421", 3 ) == 0 ) {
		//	m_State = kState_Closed;
		//}
		//else {
		//	m_State = kState_Closed;
		//}
	}

  #ifdef IMPLEMENT_DATA_CAPTURE
	if ( m_SessionState == kState_Open ) {
		if ( IsLogFtpCmds() ) {
			BeginLogToFile();
		}
	}
  #endif // IMPLEMENT_DATA_CAPTURE

	return true;
}


// -----------------------------------------------------------------------------
//      StateOpen
// -----------------------------------------------------------------------------

bool
CFtpSession::StateOpen(
	CByteVectRef&		inFtpData,
	CIpAddressPortPair& inIpPair )
{
	CPeekOutString	strMessage;

	if ( IsQuitCmd( inFtpData ) ) {
		m_SessionState = kState_Quitting;
		return true;
	}

	CByteVectRef	bvrArgument;

  #ifdef IMPLEMENT_DATA_CAPTURE
	if ( m_SessionState == kState_Open && m_bIsLoggingToFile && m_pLogStream != NULL ) {
		LogToFile( inFtpData );
	}
  #endif // IMPLEMENT_DATA_CAPTURE


	Ftp_Cmd	nCmd = ParseFtpCmd( inFtpData );

	switch ( nCmd ) {
		case kCmd_List:
			{
				if ( IsLoggingToScreen() ) {
					strMessage << GetUserNameString() << L"show directory (LIST) command received" << GetFtpConnectionString();
					m_pFtpProcess->AddLogMessage( strMessage );
				}
			  #ifdef IMPLEMENT_DATA_CAPTURE
				if ( IsListResultsToFileEnabled() ) {
					ConfigureForListRequest( inIpPair );
				}
			  #endif // IMPLEMENT_DATA_CAPTURE
			}
			break;
		case kCmd_Retrieve:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				if ( m_pFtpSessionList ) m_pFtpSessionList->IncrementStatValue( 0 );
				CPeekString	strFileName;
				if ( ParseFileName( bvrArgument, strFileName ) == true ) {
					if ( IsLoggingToScreen() ) {
						strMessage << GetUserNameString() << L"retrieve file (RETR) command received - file \"" << strFileName 
							<< L"\"" << GetFtpConnectionString();
						m_pFtpProcess->AddLogMessage( strMessage );
					}
				}

			  #ifdef IMPLEMENT_DATA_CAPTURE
				if ( IsCaptureToFileEnabled() ) {
					ConfigureForFileCaptureRequest( bvrArgument, inIpPair, true );
				}
			  #endif // IMPLEMENT_DATA_CAPTURE
			}
			break;
		case kCmd_Store:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				if ( m_pFtpSessionList ) m_pFtpSessionList->IncrementStatValue( 1 );
				CPeekString	strFileName;
				if ( ParseFileName( bvrArgument, strFileName ) == true ) {
					if ( IsLoggingToScreen() ) {
						strMessage	<< GetUserNameString() << L"send file (STOR) command received - file \"" 
							<< strFileName << L"\"" << GetFtpConnectionString();
						m_pFtpProcess->AddLogMessage( strMessage );
					}
				}
			  #ifdef IMPLEMENT_DATA_CAPTURE
				if ( IsCaptureToFileEnabled() ) {
					ConfigureForFileCaptureRequest( bvrArgument, inIpPair, false );
				}
			  #endif // IMPLEMENT_DATA_CAPTURE
			}
			break;
		case kCmd_StoreUnique:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				if ( m_pFtpSessionList ) m_pFtpSessionList->IncrementStatValue( 1 );
				CPeekString	strFileName;
				if ( ParseFileName( bvrArgument, strFileName ) == true ) {
					if ( IsLoggingToScreen() ) {
						strMessage	<< GetUserNameString() << L"send file unique (STOU) command received - file \"" 
							<< strFileName << L"\"" << GetFtpConnectionString();
						m_pFtpProcess->AddLogMessage( strMessage );
					}
				}
			}
			break;
		case kCmd_Port:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
//				if ( m_pFtpSessionList ) m_pFtpSessionList->IncrementStatValue( 1 );
				if ( IsLoggingToScreen() ) {
					strMessage	<< GetUserNameString() << L"PORT command received" << GetFtpConnectionString();
					m_pFtpProcess->AddLogMessage( strMessage );
				}
				CIpAddressPort	IpPort;
				if ( ParsePortArgument( bvrArgument, IpPort ) ) {
					m_nDataChannelState = kData_Issued;
					m_DataIpPort = IpPort;
				}
			}
			break;
		case kCmd_EPort:
			if ( ParseCmdArgument( inFtpData, bvrArgument ) ) {
				if ( IsLoggingToScreen() ) {
					strMessage	<< GetUserNameString() << L"EPRT extended port command received" << GetFtpConnectionString();
					m_pFtpProcess->AddLogMessage( strMessage );
				}
				CIpAddressPort	IpPort;
				if ( ParseEPortArgument( bvrArgument, IpPort ) ) {
					m_nDataChannelState = kData_Issued;
					m_DataIpPort = IpPort;
				}
			}
			break;
		case kCmd_Passive:
			{
				m_nDataChannelState = kData_Passive;
			}
			break;
		case kCmd_ResponseCode:
			ProcessResponseCode( inFtpData, inIpPair );


		case kCmd_Unknown:
		default:
			break;
	}

	return true;
}


// -----------------------------------------------------------------------------
//      StateQuitting
// -----------------------------------------------------------------------------

bool
CFtpSession::StateQuitting(
	CByteVectRef&	inFtpData )
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
			ProcessCloseSession();
		}
		else {
			ProcessCloseSession(); // Not sure what the state is?
		}
	}

	return true;
}


#ifdef IMPLEMENT_DATA_CAPTURE
// -----------------------------------------------------------------------------
//      BeginDataCapture
// -----------------------------------------------------------------------------

void
CFtpSession::BeginDataCapture()
{
	ASSERT( m_nDataChannelState == kData_Open );
	if ( m_nDataChannelState != kData_Open ) return;

	ASSERT( m_DataCaptureState == kDataCaptureState_Request );
	if ( m_DataCaptureState != kDataCaptureState_Request ) return;

	CPeekString	strSpecificMessage;

	if ( IsLoggingToScreen() ) {
		if ( m_DataCaptureType == kDataCaptureType_File ) {
			ASSERT( IsCaptureToFileEnabled() );
			strSpecificMessage = L"begin capturing FTP data for file write ";
		}
		else if ( m_DataCaptureType == kDataCaptureType_List ) {
			ASSERT( IsListResultsToFileEnabled() );
			strSpecificMessage = L"begin capturing directory command responses ";
		}
		else {
			ASSERT ( 0 );
			return;
		}
	}

	ASSERT( m_pPacketArray == NULL );
	if ( m_pPacketArray != NULL ) {
		m_pPacketArray->RemoveAll();

		delete m_pPacketArray;
		m_pPacketArray = NULL;
	}

	CFtpPacketArray* pPacketArray( new CFtpPacketArray );
	ASSERT( pPacketArray );
	if ( ! pPacketArray ) Peek::Throw( HE_E_OUT_OF_MEMORY );

	if ( IsLoggingToScreen() ) {
		CPeekOutString	strCompleteMessage;
		strCompleteMessage	<< GetUserNameString() << strSpecificMessage << GetDataConnectionString();

		m_pFtpProcess->AddLogMessage( strCompleteMessage );
	}

	m_pPacketArray = pPacketArray;

	m_DataCaptureState = kDataCaptureState_Capturing;
}


// -----------------------------------------------------------------------------
//      CreateTimeStringName
// -----------------------------------------------------------------------------

CPeekString
CFtpSession::CreateTimeStringName( CPeekString inFileName )
{
	ASSERT( m_Options.IsObscureNames() == false );

	CPeekString strUserName( GetUserNameString( false, false ) );
	CPeekString strTimeString( CPeekTime::GetTimeStringFileName( true ) );
	CPeekString strLine( L"_" );

	CPeekString	theName( strUserName + strLine + strTimeString + strLine + inFileName );

	return theName;
}


// -----------------------------------------------------------------------------
//      EndDataCapture
// -----------------------------------------------------------------------------

void
CFtpSession::EndDataCapture()
{
	ASSERT( m_DataCaptureState == kDataCaptureState_Capturing );
	if ( m_DataCaptureState != kDataCaptureState_Capturing )	return;

	ASSERT( !m_Options.IsObscureNames() );
	if ( m_Options.IsObscureNames() ) {
		m_pPacketArray->RemoveAll();
		delete m_pPacketArray;
		m_pPacketArray = NULL;
		return;
	}

	m_DataCaptureState = kDataCaptureState_None;

	CPeekString		strDirectory;
	CDataTransfer&	DataTransfer( m_Options.GetDataTransfer() );

	if ( m_DataCaptureType == kDataCaptureType_File ) {
		strDirectory = DataTransfer.GetFileCaptureDir();
	}
	else if ( m_DataCaptureType == kDataCaptureType_List ) {
		strDirectory = DataTransfer.GetListResultsToFileDir();
	}
	else {
		ASSERT( 0 );
		return;
	}

	if ( m_pPacketArray == NULL )	return;

	if ( (m_strCaptureFileName.GetLength() == 0) || (m_pPacketArray->GetCount() == 0) ) {
		delete m_pPacketArray;
		m_pPacketArray = NULL;
		return;
	}

	try
	{
		m_pPacketArray->Sort();

		const UInt32 nCount( static_cast<UInt32>( m_pPacketArray->GetCount() ) );
		if ( nCount == 0 ) return;

		const bool bVerified( m_pPacketArray->Verify() );

		CPeekString strCaptureFileName;

		if ( bVerified ) {
			strCaptureFileName = m_strCaptureFileName;
		} 
		else {
			strCaptureFileName = L"X_" + m_strCaptureFileName;
		}

		CPeekFileStream		FileStream( 
			strDirectory, strCaptureFileName, CPeekFileStream::MODE_WRITE | 
			CPeekFileStream::MODE_SHARE_EXCLUSIVE | CPeekFileStream::MODE_CREATE, 0 );

		if ( m_DataCaptureType == kDataCaptureType_List ) {
			CPeekOutString strOutMessage;
			strOutMessage << L"Directory text for user " << GetUserNameString( false, false ) 
				<< L" on connection " << GetDataConnectionString( false );

			FileStream.WriteLine( strOutMessage );

			if ( bVerified == false ) {
				strOutMessage << L"Some data is missing.";
				FileStream.WriteLine( strOutMessage );
			}

			FileStream.WriteLine( L"" );
		}

		for ( UInt32 i = 0; i < nCount; i++ ) {
			FtpPacketPtr& theFtpPacketPtr( m_pPacketArray->GetAt( i ) );
			CByteVectRef& theDataRef( theFtpPacketPtr->DataRef );

			if ( (theFtpPacketPtr->bDeleted == false) && (theDataRef.GetCount() > 0) ) {
				FileStream.Write( theDataRef.GetData( static_cast<UInt32>( theDataRef.GetCount() ) ), static_cast<UInt32>( theDataRef.GetCount() ) );
			}
		}

		if ( IsLoggingToScreen() ) {
			CPeekOutString	strMsg;

			if ( bVerified ) {
				strMsg	<< GetUserNameString() << L"file \"" << strCaptureFileName << L"\" on connection "
					<< GetDataConnectionString( false ) << L" successfully written.";
			}
			else {
				strMsg	<< GetUserNameString() << L"file \"" << strCaptureFileName << L"\" on connection "
					<< GetDataConnectionString( false ) << L" successfully written.  NOTE: some data was missing, this may corrupt the file!";
			}

			m_pFtpProcess->AddLogMessage( strMsg );
		}
	}
	catch (...)
	{
		if ( IsLoggingToScreen() ) {
			CPeekOutString	strMsg;
			strMsg	<< GetUserNameString() 
				<< L"attempt to write FTP data to file was unsuccessful" << GetDataConnectionString();
			m_pFtpProcess->AddLogMessage( strMsg );
		}
	}

	m_pPacketArray->RemoveAll();

	delete m_pPacketArray;
	m_pPacketArray = NULL;

	m_strCaptureFileName = L"";
}


// -----------------------------------------------------------------------------
//      BeginLogToFile
// -----------------------------------------------------------------------------

void
CFtpSession::BeginLogToFile()
{
	ASSERT( m_SessionState == kState_Open );

	if ( m_Options.IsObscureNames() ) return;

	CPeekString		strDirectory;
	CDataTransfer&	DataTransfer( m_Options.GetDataTransfer() );

	ASSERT( !m_bIsLoggingToFile );
	m_bIsLoggingToFile = false;

	ASSERT( m_pLogStream == NULL );
	delete( m_pLogStream );
	m_pLogStream = NULL;

	if ( !IsLogFtpCmds() )	return;

	strDirectory = DataTransfer.GetLogFtpCmdsDir();
	CPeekString	strCaptureFileName = CreateTimeStringName( kLoggingFileName );

	try
	{
		m_pLogStream = new CPeekFileStream( strDirectory, strCaptureFileName,
			CPeekFileStream::MODE_WRITE | CPeekFileStream::MODE_SHARE_EXCLUSIVE | 
			CPeekFileStream::MODE_CREATE, 0 );

		ASSERT( m_pLogStream );
		if ( !m_pLogStream )	Peek::Throw( HE_E_OUT_OF_MEMORY );

		if ( IsLoggingToScreen() ) {
			CPeekOutString	strMsg;
			strMsg	<< GetUserNameString() << L"begin logging to file \"" 
				<< m_pLogStream->GetFileName() << L"\"" << GetFtpConnectionString();
			m_pFtpProcess->AddLogMessage( strMsg );
		}

		m_bIsLoggingToFile = true;

	  #ifdef IMPLEMENT_PASSWORD_LOGGING
		CPeekOutString strUserNameString;
		if ( m_Options.IsIncludePassword() && m_strPassword.GetLength() > 0 ) {
			strUserNameString << L"\"" << GetUserNameString( false, false ) << L" @ " << m_strPassword << L"\"";
		}
		else {
			strUserNameString << GetUserNameString( false, false ); 
		}
	  #else // IMPLEMENT_PASSWORD_LOGGING
		CPeekString	strUserNameString( GetUserNameString( false, false ) ); 
	  #endif // IMPLEMENT_PASSWORD_LOGGING

		CPeekOutString strOutMessage;
		strOutMessage	<< L"Logging for user " << (CPeekString)strUserNameString << L" on connection " << GetFtpConnectionString( false );
	
		CPeekStringA	strMessage( strOutMessage );
		m_pLogStream->WriteLine( strOutMessage );
		m_pLogStream->WriteLine( L"" );
	}
	catch (...)
	{
		if ( IsLoggingToScreen() ) {
			CPeekOutString	strMsg;

			strMsg	<< GetUserNameString() << L"attempt to open logging file \"" 
				<< L"\" was unsuccessful" << GetFtpConnectionString();
			m_pFtpProcess->AddLogMessage( strMsg );
		}
	}
}


// -----------------------------------------------------------------------------
//      EndLogToFile
// -----------------------------------------------------------------------------

void
CFtpSession::EndLogToFile()
{
	if ( m_Options.IsObscureNames() )		return;

	m_bIsLoggingToFile = false;
	if ( m_pLogStream == NULL )	return;

	delete m_pLogStream;
	m_pLogStream = NULL;

	if ( IsLoggingToScreen() ) {
		CPeekOutString	strMsg;

		strMsg	<< GetUserNameString() << L"log file closed" << GetFtpConnectionString();
		m_pFtpProcess->AddLogMessage( strMsg );
	}
}
#endif IMPLEMENT_DATA_CAPTURE


// -----------------------------------------------------------------------------
//      ProcessOpenDataChannel
// -----------------------------------------------------------------------------

void
CFtpSession::ProcessOpenDataChannel( 
	CIpAddressPortPair& inIpPair )
{
	ASSERT( m_pFtpProcess );

	CIpAddressPort		IpAddressPort( GetDataIpPort() );
	CPeekString			strUserName( GetUserNameString() );
	CPort				PortOfInterest( IpAddressPort.GetPort() );
	CIpAddressPort		Addr1( inIpPair.GetSrcAddress(), PortOfInterest );
	CIpAddressPort		Addr2( inIpPair.GetDstAddress(), PortOfInterest );
	CIpAddressPortPair	IpPair( Addr1, Addr2 );

	SetDataChannelKey( IpPair );

	m_pFtpProcess->AddDataChannel( this );

//	if ( IsLoggingToScreen() ) {
//		CPeekOutString strMsg;
//		strMsg << GetUserNameString() << L"data channel opened" << GetDataConnectionString();
//		m_pFtpProcess->AddLogMessage( strMsg );
//	}

  #ifdef IMPLEMENT_DATA_CAPTURE
	m_DataCaptureState = kDataCaptureState_None;
  #endif // IMPLEMENT_DATA_CAPT

	m_nDataChannelState = kData_Open;
}


// -----------------------------------------------------------------------------
//      ProcessCloseDataChannel
// -----------------------------------------------------------------------------

void
CFtpSession::ProcessCloseDataChannel()
{
	ASSERT( m_pFtpProcess );

  #ifdef IMPLEMENT_DATA_CAPTURE
	if ( m_DataCaptureState == kDataCaptureState_Capturing ) {
		EndDataCapture();
	}
  #endif // IMPLEMENT_DATA_CAPTURE

	m_pFtpProcess->RemoveDataChannel( this );

	if ( IsLoggingToScreen() ) {
		CPeekOutString	strMsg;

		strMsg	<< GetUserNameString() << L"data channel closed" << GetDataConnectionString();
		m_pFtpProcess->AddLogMessage( strMsg );
	}

	m_nDataChannelState = kData_Closed;
}


// -----------------------------------------------------------------------------
//      ProcessCloseSession
// -----------------------------------------------------------------------------

void
CFtpSession::ProcessCloseSession()
{
	ASSERT( m_pFtpProcess );

  #ifdef IMPLEMENT_DATA_CAPTURE
	EndLogToFile();
  #endif // IMPLEMENT_DATA_CAPTURE

	if ( IsLoggingToScreen() ) {
		CPeekOutString	strMsg;

		strMsg << GetUserNameString() << L"monitoring stopped" << GetFtpConnectionString();
		m_pFtpProcess->AddLogMessage( strMsg );
	}

	// Remove from data channel map (if it still exists there) and remove from session map:
	m_pFtpProcess->RemoveDataChannel( this );
	m_pFtpProcess->RemoveFtpSession( this );

	m_pFtpProcess->IncrementStats( CPluginStatistics::kStat_Logoffs );

	// We're done!
	delete this;
}


// -----------------------------------------------------------------------------
//      ProcessDataChannelData
// -----------------------------------------------------------------------------

void
CFtpSession::ProcessDataChannelData(
  #ifdef IMPLEMENT_DATA_CAPTURE
	CPacket&			inPacket,
	CIpAddressPortPair&	inAddrPortPair,
	CByteVectRef&		inTcpHdr,
	CByteVectRef&		FtpData )
  #else // IMPLEMENT_DATA_CAPTURE
	CByteVectRef&		inTcpHdr )
  #endif // IMPLEMENT_DATA_CAPTURE
{
  #ifdef IMPLEMENT_DATA_CAPTURE
	ASSERT( IsDataChannelOpen() == true );
  #endif // IMPLEMENT_DATA_CAPTURE

	m_LastDataPacketTime = CPeekTime::Now().i; // GetTickCount64();

	tTcpHeader*	pTcpHdr = (tTcpHeader*) inTcpHdr.GetData( sizeof( tTcpHeader ) );
	if ( pTcpHdr ) {
		bool	bIsClose = IsCloseCmd( pTcpHdr->Flags );
		if ( bIsClose ) {
		  #ifdef IMPLEMENT_DATA_CAPTURE
			if ( m_DataCaptureState == kDataCaptureState_Capturing ) {
				ProcessDataPacket( inPacket, inAddrPortPair, FtpData, true );
			}
		  #endif // IMPLEMENT_DATA_CAPTURE

			ProcessCloseDataChannel();
			return;
		}
	}

  #ifdef IMPLEMENT_DATA_CAPTURE
	if ( m_DataCaptureState == kDataCaptureState_Capturing ) {
		ProcessDataPacket( inPacket, inAddrPortPair, FtpData );
	}
  #endif // IMPLEMENT_DATA_CAPTURE
}


#ifdef IMPLEMENT_DATA_CAPTURE
// -----------------------------------------------------------------------------
//      ProcessDataPacket
// -----------------------------------------------------------------------------

void
CFtpSession::ProcessDataPacket(
	CPacket&			inPacket,
	CIpAddressPortPair&	inAddrPortPair,
	CByteVectRef&		inTcpData,
	bool				inIsFin )
{
	ASSERT( m_pPacketArray != NULL );
	if ( m_pPacketArray == NULL )	return;
	ASSERT( m_DataCaptureState == kDataCaptureState_Capturing );

	if ( !inPacket.IsValid() ) return;
	if ( m_FileSenderAddress != inAddrPortPair.GetSrcAddress() ) return;

	m_pPacketArray->Add( inPacket, inTcpData, inIsFin );
}


// -----------------------------------------------------------------------------
//		ConfigureForFileCaptureRequest
// -----------------------------------------------------------------------------

void
CFtpSession::ConfigureForFileCaptureRequest(
	CByteVectRef&		bvrArgument,
	CIpAddressPortPair& inIpPair,
	bool				inServer )
{
	const CPeekStringA	strFileName = reinterpret_cast<const char*>( bvrArgument.GetData( bvrArgument.GetCount() ) );

	CPeekString	strParsedFileName;
	if ( ParseFileName( bvrArgument, strParsedFileName ) == true ) {
		CPeekString strFullFileName( CreateTimeStringName( strParsedFileName ) );
		ASSERT( strParsedFileName.GetLength() > 0 );
		m_strCaptureFileName = strFullFileName;
	}

	m_DataCaptureState = kDataCaptureState_Request;
	m_DataCaptureType = kDataCaptureType_File;
	if ( inServer == true ) {
		m_FileSenderAddress = inIpPair.GetDstAddress();
	}
	else {
		m_FileSenderAddress = inIpPair.GetSrcAddress();
	}

	BeginDataCapture();
}


// -----------------------------------------------------------------------------
//		ConfigureForListRequest
// -----------------------------------------------------------------------------

void
CFtpSession::ConfigureForListRequest(
	CIpAddressPortPair& inIpPair )
{
	m_strCaptureFileName = CreateTimeStringName( kDirectoryFileName );
	m_DataCaptureState = kDataCaptureState_Request;
	m_DataCaptureType = kDataCaptureType_List;
	m_FileSenderAddress = inIpPair.GetDstAddress();

	BeginDataCapture();
}
#endif // IMPLEMENT_DATA_CAPTURE


// -----------------------------------------------------------------------------
//		ProcessResponseCode
// -----------------------------------------------------------------------------

void
CFtpSession::ProcessResponseCode(
	CByteVectRef&	inFtpData,
	CIpAddressPortPair& inIpPair )
{
	CPeekOutString	strMessage;
	CByteVectRef	bvrArgument;

	if ( m_nDataChannelState == kData_Issued ) {
		UInt32	nResponseCode;
		if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
			if ( nResponseCode == 200 ) {
				if ( IsLoggingToScreen() ) {
					strMessage << GetUserNameString() << L"open port: " << m_DataIpPort.Format();
					m_pFtpProcess->AddLogMessage( strMessage );
				}

				ProcessOpenDataChannel( inIpPair );
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
						if ( IsLoggingToScreen() ) {
							strMessage << GetUserNameString() << L"open port: " << m_DataIpPort.Format();
							m_pFtpProcess->AddLogMessage( strMessage );
						}

						ProcessOpenDataChannel( inIpPair );
					}
				}
			}
		}
	}
/*
  #ifdef IMPLEMENT_DATA_CAPTURE
	else if ( m_nDataChannelState == kData_Open ) {
		if ( m_DataCaptureState == kDataCaptureState_Request ) {

		  #ifdef _DEBUG
			if ( m_DataCaptureType == kDataCaptureType_File ) ASSERT( IsCaptureToFileEnabled() ); 
			else if ( m_DataCaptureType == kDataCaptureType_List ) ASSERT( IsListResultsToFileEnabled() );
			else ASSERT( 0 );
		  #endif // _DEBUG

			UInt32	nResponseCode;
			if ( ParseResponseCode( inFtpData, nResponseCode ) ) {
				if ( nResponseCode == 150 ) {	// Accepted
					BeginDataCapture();
				}
				else if ( nResponseCode == 550 ) {	// Permission denied
					m_DataCaptureState = kDataCaptureState_None;
				}
			}
		}
	}
  #endif // IMPLEMENT_DATA_CAPTURE
*/
}


////////////////////////////////////////////////////////////////////////////////
//      CFtpSessionList
////////////////////////////////////////////////////////////////////////////////

CFtpSessionList::CFtpSessionList()
{
	m_SessionMap.InitHashTable( 257 );
	m_FileStats[0] = 0;
	m_FileStats[1] = 0;
}

CFtpSessionList::~CFtpSessionList()
{
	Reset();
}

// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
CFtpSessionList::Reset() {

	POSITION	pos = m_SessionMap.GetStartPosition();
	while ( pos != NULL ) {
		CFtpSessionMap::CPair*	pCur = m_SessionMap.GetNext( pos );
		
		CFtpSession* pFtpSession = pCur->m_value;
		if ( pFtpSession ) delete pFtpSession;
	}

	RemoveAll();
}


// -----------------------------------------------------------------------------
//		GetStatValue
// -----------------------------------------------------------------------------

size_t
CFtpSessionList::GetStatValue(
	size_t inIndex )
{
	switch ( inIndex ) {
		case 1:	return 	m_FileStats[0];
		case 2:	return 	m_FileStats[1];
	}

	return m_SessionMap.GetCount();
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

bool
CFtpSessionList::Monitor(
	 CArrayString&	outRemovedStreams )
{
	bool		bRemoved( false );
	UInt64		nCurTime =  CPeekTime::Now().i;
	POSITION	pos = m_SessionMap.GetStartPosition();
	
	while ( pos != NULL ) {
		CFtpSessionMap::CPair*	pCur = m_SessionMap.GetNext( pos );
		CFtpSession*			pFtpSession( pCur->m_value );

		ASSERT( pFtpSession );

		UInt64	nDelta = nCurTime - pFtpSession->GetLastFtpPacketTime();
		bool	bRemove = (nDelta > kMonitorTime);
		if ( bRemove ) {
			CPeekOutString	strRemoved;

			strRemoved	<< pFtpSession->GetUserNameString() << L"the Ftp connection " << pFtpSession->GetFtpConnectionString( false );

			outRemovedStreams.Add( strRemoved );

			Remove( pFtpSession );
			delete pFtpSession;

			bRemoved = true;
		}
	}

	return bRemoved;
}
#endif // IMPLEMENT_MONITORING


// -----------------------------------------------------------------------------
//		Find
// -----------------------------------------------------------------------------

CFtpSession*
CFtpSessionList::Find(
	const CIpAddressPortPair&	inIpPortPair )
{
	CFtpSessionMap::CPair*	pCur = m_SessionMap.Lookup( inIpPortPair );
	if ( pCur ) {
		return pCur->m_value;
	}

	CIpAddressPortPair	invIpPortPair;
	inIpPortPair.Invert( invIpPortPair );
	pCur = m_SessionMap.Lookup( invIpPortPair );
	if ( pCur ) {
		return pCur->m_value;
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

bool
CFtpSessionList::Add(
	CFtpSession*	inFtpSession )
{
	if ( inFtpSession == NULL ) return false;

	m_SessionMap.SetAt( inFtpSession->FtpSessionKey(), inFtpSession );
	inFtpSession->SetFtpSessionList( this );
	return true;
}



////////////////////////////////////////////////////////////////////////////////
//      CDataChannelList
////////////////////////////////////////////////////////////////////////////////

CDataChannelList::CDataChannelList()
{
	m_DataMap.InitHashTable( 257 );
}

CDataChannelList::~CDataChannelList()
{
	RemoveAll();
}


// -----------------------------------------------------------------------------
//		HalfFind
// -----------------------------------------------------------------------------

CFtpSession*
CDataChannelList::HalfFind(
	const SessionItemKey&	inItemKey )
{
	CFtpSession*		pFoundItemValue( NULL );
	SessionItemKey		FoundItemKey;

	POSITION			pos = m_DataMap.GetStartPosition();
	while ( pos != NULL ) {
		CDataChannelMap::CPair*	pCur = m_DataMap.GetNext( pos );
		CFtpSession*			pCurItemValue( pCur->m_value );
		const SessionItemKey&	CurItemKey( pCur->m_key );

		// Assertion break may indicate a problem with the data such as second data channel
		// for a given session.  Currently we'll let that pass, but in the future we may 
		// want to deal with this.
		ASSERT( CurItemKey == pCurItemValue->GetDataChannelKey() );

		// If the item's source and destination port are the same, we have 
		// a "half channel".  This indicates that this is the first packet 
		// received on a recently added data channel, whose key must now be 
		// adjusted.  If we have a half channel map entry, then determine 
		// whether it's the one we want, i.e. it has the correct address 
		// pair with the new data port that defined the half channel.
		if ( CurItemKey.GetSrcPort() == CurItemKey.GetDstPort() ) {
			CIpAddressPair	Pair1( CurItemKey.GetSrcAddress(), CurItemKey.GetDstAddress() );
			CIpAddressPair	Pair2( inItemKey.GetSrcAddress(), inItemKey.GetDstAddress() );
			CPort			CurPort( CurItemKey.GetSrcPort() );  // Same as CurItemKey.GetDstPort()
			CPort			inPort1( inItemKey.GetSrcPort() );
			CPort			inPort2( inItemKey.GetDstPort() );

			if ( CurPort == inPort1 || CurPort == inPort2 ) {
				if ( Pair1 == Pair2 || Pair1.IsInverse( Pair2 ) ) {
					pFoundItemValue = pCurItemValue ;
					FoundItemKey = CurItemKey;
					break;
				}
			}
		}
	}

	// If a half channel (placeholder) item was found in the map, then correct the key to the 
	// found item.  This is done by removing the half channel item from the map, correcting 
	// its key, then adding it back to the map.
	if ( pFoundItemValue != NULL ) {
		m_DataMap.RemoveKey( FoundItemKey );
		pFoundItemValue->SetDataChannelKey( inItemKey );
		m_DataMap.SetAt( pFoundItemValue->GetDataChannelKey(), pFoundItemValue );
		m_DataPortList.AddDataPorts( inItemKey.GetDstPort(), inItemKey.GetSrcPort() );

		IFtpProcess*	pFtpProcess( pFoundItemValue->GetFtpProcess() );
		ASSERT( pFtpProcess );
		if ( pFtpProcess ) {
			if ( pFoundItemValue->IsLoggingToScreen() ) {
				CPeekOutString strMsg;
				strMsg << pFoundItemValue->GetUserNameString() << L"data channel opened" << pFoundItemValue->GetDataConnectionString();
				pFoundItemValue->GetFtpProcess()->AddLogMessage( strMsg );
			}
		}
	}

	return pFoundItemValue;
}


#ifdef IMPLEMENT_MONITORING
// -----------------------------------------------------------------------------
//		Monitor
// -----------------------------------------------------------------------------

bool
CDataChannelList::Monitor(
	CArrayString&	outRemovedStreams )
{
	bool	bRemoved( false );
 
	UInt64		nCurTime = CPeekTime::Now().i;
	POSITION	pos = m_DataMap.GetStartPosition();

	while ( pos != NULL ) {
		CDataChannelMap::CPair*	pCur = m_DataMap.GetNext( pos );
		CFtpSession*			pDataChannel( pCur->m_value );

		UInt64	nDelta = nCurTime - pDataChannel->GetLastDataPacketTime();
		bool	bRemove = (nDelta > kMonitorTime);
		if ( bRemove ) {
			CPeekOutString	strRemoved;

			strRemoved	<< pDataChannel->GetUserNameString() << L"the data connection " << pDataChannel->GetDataConnectionString( false );

			outRemovedStreams.Add( strRemoved );

			Remove( pDataChannel );

			// Note: the object is only deleted when removed from the session map

			bRemoved = true;
		}
	}

	return bRemoved;
}
#endif // IMPLEMENT_MONITORING


// -----------------------------------------------------------------------------
//		Find
// -----------------------------------------------------------------------------

CFtpSession*
CDataChannelList::Find(
	const SessionItemKey&	inItemKey )
{
	CDataChannelMap::CPair*	pCur = m_DataMap.Lookup( inItemKey );
	if ( pCur ) {
		return pCur->m_value;
	}

	SessionItemKey	invItemKey;
	inItemKey.Invert( invItemKey );
	pCur = m_DataMap.Lookup( invItemKey );
	if ( pCur ) {
		return pCur->m_value;
	}

	return	HalfFind( inItemKey );
}


// -----------------------------------------------------------------------------
//		Remove
// -----------------------------------------------------------------------------

void
CDataChannelList::Remove( 
	CFtpSession* inDataChannel ) 
{
	CPort PortToRemove1 = inDataChannel->GetDataChannelKey().GetSrcPort();
	CPort PortToRemove2 = inDataChannel->GetDataChannelKey().GetDstPort();

	m_DataMap.RemoveKey( inDataChannel->GetDataChannelKey() );

	if ( PortToRemove1 == PortToRemove2 ) {
		PortToRemove2 = (CPort)0;
	}

	bool	bDataPort1StillInUse( false );
	bool	bDataPort2StillInUse( false );

	POSITION		pos = m_DataMap.GetStartPosition();
	while ( pos != NULL ) {
		CDataChannelMap::CPair*	pCur = m_DataMap.GetNext( pos );

		CFtpSession*	pCurDataChannel( pCur->m_value );
		CPort Port1 = pCurDataChannel->GetDataChannelKey().GetSrcPort();
		CPort Port2 = pCurDataChannel->GetDataChannelKey().GetDstPort();
		if ( !bDataPort1StillInUse && PortToRemove1 > 0 && Port1 == PortToRemove1 ) {
			bDataPort1StillInUse = true;
		}
		if ( !bDataPort2StillInUse && PortToRemove2 > 0 && Port2 == PortToRemove2 ) {
			bDataPort2StillInUse = true;
		}
		if ( bDataPort1StillInUse && bDataPort2StillInUse ) {
			break;
		}
	}

	if ( bDataPort1StillInUse == false ) {
		m_DataPortList.Remove( PortToRemove1 );
	}
	if ( bDataPort2StillInUse == false ) {
		m_DataPortList.Remove( PortToRemove2 );
	}
}


// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

bool
CDataChannelList::Add(
	CFtpSession*	inDataChannel )
{
	if ( inDataChannel == NULL ) return false;

	m_DataMap.SetAt( inDataChannel->GetDataChannelKey(), inDataChannel );

	m_DataPortList.AddDataPorts( inDataChannel->GetDataIpPort().GetPort(), 0 );

	return true;
}
