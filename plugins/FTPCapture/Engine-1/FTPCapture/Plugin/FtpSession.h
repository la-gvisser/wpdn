// =============================================================================
//	FtpSession.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#pragma once

#include "IpHeaders.h"
#include "PluginStatistics.h"
#include "ByteVectRef.h"
#include <memory>
#include <std-tr1.h>
#include "Options.h"
#ifdef IMPLEMENT_DATA_CAPTURE
#include "PacketArray.h"
#include "PeekFileStream.h"
#include "FtpPacketArray.h"
#endif // IMPLEMENT_DATA_CAPTURE

class CFtpSessionList;
class IFtpProcess;

typedef	CIpAddressPortPair	SessionItemKey;


////////////////////////////////////////////////////////////////////////////////
//      CFtpSession
////////////////////////////////////////////////////////////////////////////////

class CFtpSession
{

public:
	static CPeekString	g_strObscureName;
	static CPeekString	g_strObscureFile;

	// Tcp flags (ack, fin, etc.)
	enum _TCPFlag {
		kTCPFlag_FIN = 0x01,
		kTCPFlag_SYN = 0x02,
		kTCPFlag_RST = 0x04,
		kTCPFlag_PSH = 0x08,
		kTCPFlag_ACK = 0x10,
		kTCPFlag_URG = 0x20
	};

	// Ftp session states
	typedef enum {
		kState_UserSubmitted,
		kState_UserAccepted,
		kState_PasswordSubmitted,
		kState_Open,
		kState_Quitting,
		kState_Maximum
	} FtpSession_State;

	// Data channel states
	typedef enum {
		kData_Closed,
		kData_Issued,
		kData_Passive,
		kData_Open,
	  #ifdef IMPLEMENT_DATA_CAPTURE
		kData_TransferReady,
		kData_FileTransfer,
	  #endif // IMPLEMENT_DATA_CAPTURE
	} DataChannelState;

  #ifdef IMPLEMENT_DATA_CAPTURE
	// Data capture states
	typedef enum {
		kDataCaptureState_None,
		kDataCaptureState_Request,
		kDataCaptureState_Capturing,
	} DataCaptureState;

	// Data capture types
	typedef enum {
		kDataCaptureType_None,
		kDataCaptureType_File,
		kDataCaptureType_List,
	} DataCaptureType;

  #endif // IMPLEMENT_DATA_CAPTURE

	// Ftp command references
	typedef enum {
		kCmd_Retrieve,
		kCmd_Store,
		kCmd_StoreUnique,
		kCmd_Port,
		kCmd_Passive,
		kCmd_ResponseCode,
		kCmd_EPort,
		kCmd_List,
		// kCmd_Maximum,
		kCmd_Unknown
	} Ftp_Cmd;

  #ifdef IMPLEMENT_DATA_CAPTURE
	// Help identify who is the file sender
	typedef enum {
		kFileSender_None,
		kFileSender_Client,
		kFileSender_Server
	} FileSenderType;
  #endif // IMPLEMENT_DATA_CAPTURE

protected:

  // Ftp Session Section:
	FtpSession_State		m_SessionState;
	CIpAddressPortPair		m_FtpSessionKey;
	CPeekString				m_strUserName;
	UInt64					m_LastFtpPacketTime;
	CFtpSessionList*		m_pFtpSessionList;
	IFtpProcess*			m_pFtpProcess;
	COptions&				m_Options;
  #ifdef IMPLEMENT_PASSWORD_LOGGING
	CPeekString				m_strPassword;
  #endif // IMPLEMENT_PASSWORD_LOGGING

 // Data Channel member variables:
	DataChannelState		m_nDataChannelState;
	CIpAddressPort			m_DataIpPort;
	CIpAddressPortPair		m_DataChannelKey;
	UInt64					m_LastDataPacketTime;
  #ifdef IMPLEMENT_DATA_CAPTURE
	CIpAddress				m_FileSenderAddress;
	CFtpPacketArray*		m_pPacketArray;
	DataCaptureState		m_DataCaptureState;
	DataCaptureType			m_DataCaptureType;
	CPeekString				m_strCaptureFileName;
	bool					m_bIsLoggingToFile;
	CPeekFileStream*		m_pLogStream;			

  // Internal Ftp processing functions:
	void		BeginDataCapture();
	void		BeginLogToFile();
	void		ConfigureForListRequest( CIpAddressPortPair& inIpPair );
	void		ConfigureForFileCaptureRequest( CByteVectRef& bvrArgument, CIpAddressPortPair& inIpPair, bool inServer );
	CPeekString	CreateTimeStringName( CPeekString inFileName );
	void		EndDataCapture();
	void		EndLogToFile();
	bool		LogToFile( CByteVectRef&	inFtpData );
	void		ProcessDataPacket( CPacket&	inPacket, CIpAddressPortPair& inAddrPortPair, CByteVectRef& inTcpData, bool inIsFin = false );
  #endif // IMPLEMENT_DATA_CAPTURE

	void	ProcessCloseDataChannel();
	void	ProcessCloseSession();
	void	ProcessOpenDataChannel( CIpAddressPortPair& inIpPair );
	void	ProcessResponseCode( CByteVectRef& inFtpData, CIpAddressPortPair& inIpPair );

	bool	StateUserSubmitted( CByteVectRef& inFtpData );
	bool	StateUserAccepted( CByteVectRef& inFtpData );
	bool	StatePasswordSubmitted( CByteVectRef& inFtpData );
	bool	StateOpen( CByteVectRef& inFtpData, CIpAddressPortPair& IpPair );
	bool	StateQuitting( CByteVectRef& inFtpData );

public:
	;				CFtpSession( const CIpAddressPortPair& inIpPortPair, const CPeekString& inUserName,
								IFtpProcess* inFtpProcess, COptions& inOptions ) ;
	;				virtual	~CFtpSession() {
	  #ifdef IMPLEMENT_DATA_CAPTURE
		delete	m_pLogStream;
		delete	m_pPacketArray;
	  #endif // IMPLEMENT_DATA_CAPTURE
	}

	// Static Ftp Session functions
	static bool			IsCloseCmd( UInt8 inTcpFlags );
	static bool			IsPasswordCmd( CByteVectRef& inFtpData );
	static bool			IsQuitCmd( CByteVectRef& inFtpData );
	static bool			IsUserCmd( CByteVectRef& inFtpData );

	static bool			ParseCmdArgument( CByteVectRef& inFtpData, CByteVectRef& outArgument );
	static Ftp_Cmd		ParseFtpCmd( CByteVectRef& inFtpData );
	static bool			ParsePortArgument( CByteVectRef& inArgument, CIpAddressPort& outIpPort );
	static bool			ParseEPortArgument( CByteVectRef& inArgument, CIpAddressPort& outIpPort );
	static bool			ParsePassiveResponse( CByteVectRef& inArgument, CIpAddressPort& outIpPort );
	static bool			ParseResponseCode( CByteVectRef& inFtpData, UInt32& outResponseCode );

	bool				ParseFileName( CByteVectRef& inArgument, CPeekString& outFileName );
  #ifdef IMPLEMENT_PASSWORD_LOGGING
	bool				ParsePassword( CByteVectRef& inArgument, CPeekString& outPassword );
  #endif // IMPLEMENT_PASSWORD_LOGGING

	// Ftp session functions
	void				CloseSessionList();

	const CIpAddressPortPair&	FtpSessionKey() const { return m_FtpSessionKey; }

	CIpAddressPort		GetClientIpAddressPort() const;
	CIpAddressPort		GetFtpServerIpAddressPort() const;
	CPeekString			GetDataConnectionString( bool bAddConnectText = true ) {
		CPeekOutString strConnect;
		strConnect	<< ( bAddConnectText ? L" - connection: " : L"" ) 
			<< m_DataChannelKey.GetSource().Format() << L" - " 
			<< m_DataChannelKey.GetDestination().Format();
		return strConnect;
	}
	CPeekString			GetFtpConnectionString( bool bAddConnectText = true ) {
		CPeekOutString strConnect;
		strConnect	<< ( bAddConnectText ? L" - connection: " : L"" ) 
			<< m_FtpSessionKey.GetSource().Format() << L" - " 
			<< m_FtpSessionKey.GetDestination().Format();
		return strConnect;
	}
	IFtpProcess*		GetFtpProcess() { return m_pFtpProcess; }
	UInt64				GetLastFtpPacketTime(){ return m_LastFtpPacketTime; }
	CPeekString			GetUserNameString( bool bAddUserText = true, bool bAddHyphen = true ) {
		CPeekOutString strUserNameString;
		strUserNameString 
			<< ( bAddUserText ? L"User " : L"" )
			<< ( m_Options.IsObscureNames() ? g_strObscureName : m_strUserName )
			<< ( bAddHyphen ? L" - " : L"");
		return strUserNameString;
	}

	bool				ProcessCmd( CByteVectRef& inTcpHdr, CByteVectRef& inFtpData, CIpAddressPortPair& IpPair );
	
	void				SetFtpSessionList( CFtpSessionList* inFtpSessionList ) {
		m_pFtpSessionList = inFtpSessionList;
	}

  // Data Channel functions:
	const CIpAddressPortPair&	GetDataChannelKey() const { return m_DataChannelKey; }
	CIpAddressPort		GetDataIpPort() { return m_DataIpPort; }
	UInt64				GetLastDataPacketTime(){ 
		return m_LastDataPacketTime; 
	}

	bool	IsLoggingToScreen() { return m_Options.IsLoggingToScreen(); }

  #ifdef IMPLEMENT_DATA_CAPTURE
	void	ProcessDataChannelData( CPacket& inPacket, CIpAddressPortPair& inAddrPortPair, CByteVectRef& inTcpHdr, CByteVectRef& FtpData );
	bool	IsDataChannelOpen()		{ return ( m_nDataChannelState == kData_Open ); }

	bool	IsDataTransferEnabled() { return m_Options.IsDataTransferEnabled(); }
	bool	IsCaptureToFileEnabled() { 
		ASSERT( m_Options.IsCaptureToFileEnabled() ? (m_Options.IsObscureNames() == false) : true );
		return( m_Options.IsCaptureToFileEnabled() ); 
	}
	bool	IsListResultsToFileEnabled() {
		ASSERT( m_Options.IsListResultsToFileEnabled() ? (m_Options.IsObscureNames() == false) : true );
		return( m_Options.IsListResultsToFileEnabled() ); 
	}
	bool	IsLogFtpCmds() {
		ASSERT( m_Options.IsLogFtpCmds() ? (m_Options.IsObscureNames() == false) : true );
		return( m_Options.IsLogFtpCmds() ); 
	}
  #else // IMPLEMENT_DATA_CAPTURE
	void	ProcessDataChannelData( CByteVectRef& inTcpHdr );
  #endif // IMPLEMENT_DATA_CAPTURE

	void	SetDataChannelKey( const CIpAddressPortPair& inDataPair ) {
		m_DataChannelKey = inDataPair; 
	}
};


////////////////////////////////////////////////////////////////////////////////
//      CFtpSessionMap
////////////////////////////////////////////////////////////////////////////////

typedef	CAtlMap<CIpAddressPortPair, CFtpSession*, CIpAddressPortPairTraits>	CFtpSessionMap;


////////////////////////////////////////////////////////////////////////////////
//      CFtpSessionList
////////////////////////////////////////////////////////////////////////////////

class CFtpSessionList
	:	public IStatistics
{
protected:
	CFtpSessionMap	m_SessionMap;

	UInt32			m_FileStats[2];	// 0 - Retrieve, 1 - Store

public:
	;				CFtpSessionList();
	virtual			~CFtpSessionList();

	void			Reset();

	// IStatistics
	size_t			GetStatValue( size_t inIndex );
	void			ResetStatValue( size_t inIndex );

	void			IncrementStatValue( size_t inIndex );

	size_t			GetCount() { return m_SessionMap.GetCount(); }

  #ifdef IMPLEMENT_MONITORING
	bool			Monitor( CArrayString& outRemovedStreams );
  #endif // IMPLEMENT_MONITORING

	CFtpSession*	Find( const CIpAddressPortPair& inIpPortPair );

	bool			Add( CFtpSession* pFtpSession );

	void			Remove( CFtpSession* inFtpSession ) {
		m_SessionMap.RemoveKey( inFtpSession->FtpSessionKey() );
	}
	void			RemoveAll() { 		m_SessionMap.RemoveAll(); }
};


// =============================================================================
//		CDataPortList
// =============================================================================

class CDataPortList
	:	public CAtlArray<UInt16>
{
public:
	;		CDataPortList() {}
	;		~CDataPortList() {}

	void	AddDataPorts( const UInt16 inPort1, const UInt16 inPort2 ) {
		if ( inPort1 > 0 ) {
			if ( !Find( inPort1 ) ) {
				Add( inPort1 );
			}
		}
		if ( inPort2 > 0 ) {
			if ( !Find( inPort2 ) ) {
				Add( inPort2 );
			}
		}
	}

	bool	IsDataPortOfInterest( const UInt16 inPort1, const UInt16  inPort2 ) const {
		return Find( inPort1, inPort2 );
	}

	bool	Find( const UInt16 inPort ) const {
		const size_t nCount( GetCount() );
		for ( size_t i = 0; i < nCount; i++ ) {
			UInt16 nPort( GetAt( i ) );
			if ( nPort == inPort ) return true;
		}
		return false;
	}

	bool	Find( UInt16 inPort1, UInt16 inPort2 ) const {
		const size_t nCount( GetCount() );
		for ( size_t i = 0; i < nCount; i++ ) {
			UInt16 nPort( GetAt( i ) );
			if ( nPort == inPort1 || nPort == inPort2) return true;
		}
		return false;
	}

	void	Remove( UInt16 inPort ) {
		const size_t nCount( GetCount() );
		for ( size_t i = 0; i < nCount; i++ ) {
			UInt16 nPort( GetAt( i ) );
			if ( nPort == inPort) {
				RemoveAt( i );
				return;
			}
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
//      CDataChannelMap
////////////////////////////////////////////////////////////////////////////////

typedef CAtlMap<CIpAddressPortPair, CFtpSession*, CIpAddressPortPairTraits>	CDataChannelMap;


////////////////////////////////////////////////////////////////////////////////
//      CDataChannelList
////////////////////////////////////////////////////////////////////////////////

class CDataChannelList
	:	public IStatistics
{

protected:

	CDataChannelMap	m_DataMap;
	CDataPortList	m_DataPortList;

	CFtpSession*	HalfFind( const SessionItemKey&	inIpPair );

public:
	;				CDataChannelList();
	virtual			~CDataChannelList();

	void			Reset() { RemoveAll(); }

	// IStatistics
	size_t			GetStatValue( size_t ) { return m_DataMap.GetCount(); }
	void			ResetStatValue( size_t ) {}

	size_t			GetCount() const { return m_DataMap.GetCount(); }

#ifdef IMPLEMENT_MONITORING
	bool			Monitor( CArrayString& outRemovedStreams );
#endif // IMPLEMENT_MONITORING

	CFtpSession*	Find( const SessionItemKey& inIpPair );

	bool			Add( CFtpSession* pDataChannel );

	bool			IsDataPortOfInterest( UInt16 inPort1, UInt16 inPort2 ) {
		return m_DataPortList.IsDataPortOfInterest( inPort1, inPort2 );
	}

	void			Remove( CFtpSession* inDataChannel );
	void			RemoveAll() {
		m_DataMap.RemoveAll();
		m_DataPortList.RemoveAll();
	}
};


// =============================================================================
//	IFtpProcess
// =============================================================================

class IFtpProcess
{
public:

	virtual void	AddDataChannel( CFtpSession* inFtpSession ) = 0;
	virtual void	AddLogMessage( const CPeekString& inMessage ) = 0;
	virtual void	AddLogMessage( const CPeekOutString& inMessage ) = 0;
	virtual void	AddSession( CFtpSession* inFtpSession ) = 0;

	virtual void	IncrementStats( size_t nStat ) = 0;

	virtual void	RemoveDataChannel(  CFtpSession* inFtpSession ) = 0;
	virtual	void	RemoveFtpSession(  CFtpSession* inFtpSession ) = 0;
};
