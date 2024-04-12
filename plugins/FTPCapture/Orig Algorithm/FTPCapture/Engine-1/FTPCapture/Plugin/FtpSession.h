// =============================================================================
//	FtpSession.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc.

#pragma once

//#include "IpAddress.h"
#include "IpHeaders.h"
#include "PluginStatistics.h"
#include "ByteVectRef.h"
#include <memory>
#include <std-tr1.h>

class CFtpSessionList;

////////////////////////////////////////////////////////////////////////////////
//      CFtpSession
////////////////////////////////////////////////////////////////////////////////

class CFtpSession
{
public:
	static CPeekString	g_strObscureName;
	static CPeekString	g_strObscureFile;

protected:
	enum _TCPFlag {
		kTCPFlag_FIN = 0x01,
		kTCPFlag_SYN = 0x02,
		kTCPFlag_RST = 0x04,
		kTCPFlag_PSH = 0x08,
		kTCPFlag_ACK = 0x10,
		kTCPFlag_URG = 0x20
	};

	typedef enum {
		kState_UserSubmitted,
		kState_UserAccepted,
		kState_PasswordSubmitted,
		kState_Open,
		kState_Quitting,
		kState_Closed,
		kState_Maximum
	} FtpSession_State;

	typedef enum {
		kCmd_Retrieve,
		kCmd_Store,
		kCmd_StoreUnique,
		kCmd_Port,
		kCmd_Passive,
		kCmd_ResponseCode,
		kCmd_Unknown,
	  #ifdef _ALGORITHM_EXTENDED
		kCmd_EPort,
	  #endif // _ALGORITHM_EXTENDED
		kCmd_Maximum
	} Ftp_Cmd;

	typedef enum {
		kData_Closed,
		kData_Issued,
		kData_Passive,
		kData_Open
#ifdef _NEW_ALGORITHM_1
		,
		kData_Close_Issued,
		kData_PortOpen
#endif // _NEW_ALGORITHM_1

//		kData_Maximum
	} DataChannelState;

	FtpSession_State	m_State;
	CIpAddressPortPair	m_IpPortPair;
	CPeekString			m_strUserName;
	UInt64				m_LastPacketTime;
	DataChannelState	m_nDataChannelState;
	CIpAddressPort		m_DataIpPort;
	CFtpSessionList*	m_pFtpSessionList;
  #ifdef _NEW_ALGORITHM_2
	CIpAddressPortPair	m_DataIpPortPair;
  #endif // _NEW_ALGORITHM_2

	bool	StateUserSubmitted( CByteVectRef& inFtpData, bool inObscure );
	bool	StateUserAccepted( CByteVectRef& inFtpData, bool inObscure );
	bool	StatePasswordSubmitted( CByteVectRef& inFtpData, bool inObscure, CPeekString& outMessage );
	bool	StateOpen( CByteVectRef& inFtpData, bool inObscure, CPeekString& outMessage );
	bool	StateQuitting( CByteVectRef& inFtpData, bool inObscure, CPeekString& outMessage );
	bool	StateClosed( CByteVectRef& inFtpData, bool inObscure );

public:
	;		CFtpSession() {}
	;		CFtpSession( const CIpAddressPortPair& inIpPortPair, const CPeekString& inUserName );
	;		~CFtpSession() {}

	CFtpSession&	operator=( const CFtpSession& in ) {
		m_State = in.m_State;
		m_IpPortPair = in.m_IpPortPair;
		m_strUserName = in.m_strUserName;
		m_nDataChannelState = in.m_nDataChannelState;
		m_DataIpPort = in.m_DataIpPort;
	  #ifdef _NEW_ALGORITHM_2
		m_DataIpPortPair = in.m_DataIpPortPair;
	  #endif // _NEW_ALGORITHM_2
		m_pFtpSessionList = NULL;		// don't want to copy the ptr.
		return *this;
	}

	void			Close();

	static bool		IsCloseCmd( UInt8 inTcpFlags );
	static bool		IsUserCmd( CByteVectRef& inFtpData );
	static bool		IsPasswordCmd( CByteVectRef& inFtpData );
	static bool		IsQuitCmd( CByteVectRef& inFtpData );
	static bool		ParseCmdArgument( CByteVectRef& inFtpData, CByteVectRef& outArgument );
	static Ftp_Cmd	ParseFtpCmd( CByteVectRef& inFtpData );
	static bool		ParsePortArgument( CByteVectRef& inArgument, CIpAddressPort& outIpPort );
  #ifdef _ALGORITHM_EXTENDED
	static bool		ParseEPortArgument( CByteVectRef& inArgument, CIpAddressPort& outIpPort );
  #endif // _ALGORITHM_EXTENDED
	static bool		ParsePassiveResponse( CByteVectRef& inArgument, CIpAddressPort& outIpPort );
	static bool		ParseResponseCode( CByteVectRef& inFtpData, UInt32& outResponseCode );

  #ifndef  _NEW_ALGORITHM_1
	static CByteVectRef GetFtpData( CByteVectRef& inIpHdr, CByteVectRef& inTcpHdr );
  #endif //  _NEW_ALGORITHM_1

	CIpAddressPort		GetClientIpAddressPort() const;
	UInt64				GetLastPacketTime(){ return m_LastPacketTime; }
	static CPeekString	GetObscureName() { return g_strObscureName; }
	CPeekString			GetUserName( bool inObscureName ) {	
		return (inObscureName) ? GetObscureName() : m_strUserName;
	}

	bool			GetDataChannelInfo( bool inObscure, CIpAddressPort& outIpPort, CPeekString& outUserName ) {
		outIpPort = m_DataIpPort;
		outUserName = GetUserName( inObscure );
	  #ifdef _NEW_ALGORITHM_2
//		m_nDataChannelState = kData_Open;
	  #else // _NEW_ALGORITHM_2
		m_nDataChannelState = kData_Closed;
	  #endif // _NEW_ALGORITHM_2

		return true;
	}

	CIpAddressPort	GetServerIpAddressPort() const;

	const CIpAddressPortPair&	Key() const { return m_IpPortPair; }

	bool	IsSessionClosed() { return m_State == kState_Closed; }

  #ifdef _NEW_ALGORITHM_2
	bool	GetDataIpAddressPort( CIpAddressPortPair& outIpPair ) const {
		outIpPair = m_DataIpPortPair;
		return true;
	}
	bool	HasDataChannelClosed() {
		if ( m_nDataChannelState == kData_Close_Issued ) {
			m_nDataChannelState = kData_Closed;
			return true;
		}
		return false; 
	}
	bool	IsDataChannelPortOpen() { 
		if ( m_nDataChannelState == kData_PortOpen ) {
			m_nDataChannelState = kData_Open;
			return true;
		}
		return false; 
	}
	void	SetDataIpPortPair( CIpAddressPortPair& inAddressPortPair ) { m_DataIpPortPair = inAddressPortPair; }
  #endif // _NEW_ALGORITHM_2

	bool	IsDataChannelOpen() { return (m_nDataChannelState == kData_Open); }
	bool	ProcessCmd( CByteVectRef& inIpHdr, CByteVectRef& inTcpHdr,
		CByteVectRef& inFtpData, bool inObscure, CPeekString& outMessage );
	void	SetFtpSessionList( CFtpSessionList* inFtpSessionList ) {
		m_pFtpSessionList = inFtpSessionList;
	}
};

typedef NSTR1::shared_ptr<CFtpSession>	CFtpSessionPtr;

////////////////////////////////////////////////////////////////////////////////
//      CFtpSessionMap
////////////////////////////////////////////////////////////////////////////////

typedef	CAtlMap<CIpAddressPortPair, CFtpSessionPtr, CIpAddressPortPairTraits>	CFtpSessionMap;


////////////////////////////////////////////////////////////////////////////////
//      CFtpSessionList
////////////////////////////////////////////////////////////////////////////////

class CFtpSessionList
	:	public IStatistics
{
protected:
	CFtpSessionMap	m_Map;

	UInt32			m_FileStats[2];	// 0 - Retrieve, 1 - Store

public:
	;				CFtpSessionList();
	virtual			~CFtpSessionList();

	void			Close() {
		RemoveAll();
	}

	// IStatistics
	UInt32			GetStatValue( size_t inIndex );
	void			ResetStatValue( size_t inIndex );

	void			IncrementStatValue( size_t inIndex );

	UInt32			GetCount() { return m_Map.GetCount(); }
  #ifdef IMPLEMENT_MONITORING
	void			Monitor( DWORD dwTimeOut, bool inObscure, CArrayString& outRemovedStreams );
  #endif // IMPLEMENT_MONITORING

	CFtpSession*	Find( const CIpAddressPortPair& inIpPortPair );
	bool			Add( CFtpSession* pFtpSession );
	void			RemoveAndDelete( CFtpSession* inFtpSession ) {
		m_Map.RemoveKey( inFtpSession->Key() );
	}
	void			RemoveAll() { 		m_Map.RemoveAll(); }
};
