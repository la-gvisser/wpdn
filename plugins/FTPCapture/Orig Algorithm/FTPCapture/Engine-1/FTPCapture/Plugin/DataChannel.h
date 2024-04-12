// =============================================================================
//	DataChannel.h
//  interface to CDataChannel
// =============================================================================
//	Copyright (c) 2005 WildPackets, Inc.

#pragma once

#include "IpHeaders.h"
#include "PluginStatistics.h"
#include "ByteVectRef.h"
#include <memory>
#include <std-tr1.h>
#include "FtpSession.h"


////////////////////////////////////////////////////////////////////////////////
//      CDataChannel
////////////////////////////////////////////////////////////////////////////////

class CDataChannel
{
public:
	static CPeekString		g_strObscureName;

protected:
	enum _TCPFlag {
		kTCPFlag_FIN = 0x01,
		kTCPFlag_SYN = 0x02,
		kTCPFlag_RST = 0x04,
		kTCPFlag_PSH = 0x08,
		kTCPFlag_ACK = 0x10,
		kTCPFlag_URG = 0x20
	};

	bool				m_bOpen;
	CIpAddressPort		m_IpPort;
	CIpAddressPortPair	m_IpPair;
	CPeekString			m_strUserName;
	UInt64				m_LastPacketTime;
#ifdef _NEW_ALGORITHM_2
	CFtpSession*		m_pFtpMySession;
#endif // _NEW_ALGORITHM_2

public:
	;		CDataChannel() {};
  #ifdef _NEW_ALGORITHM_2
	;		CDataChannel( const CIpAddressPort& inIpPort, const CIpAddressPortPair& inIpPortPair, 
							const CPeekString& inUserName, CFtpSession* inMyFtpSession );
  #else // _NEW_ALGORITHM_2
	;		CDataChannel( const CIpAddressPort& inIpPort, const CIpAddressPortPair& inIpPortPair, 
							const CPeekString& inUserName );
  #endif // _NEW_ALGORITHM_2
	;		~CDataChannel() {}

  #ifdef _NEW_ALGORITHM_2
	CFtpSession*	GetSession() { return m_pFtpMySession; }
	bool			IsHalfChannel() { return (m_IpPair.GetDstPort() == m_IpPair.GetSrcPort()); }
	bool			ProcessData() { 
		if ( m_bOpen )	m_LastPacketTime = GetTickCount64();
		return m_bOpen;
	}
	bool			IsChannelOpen() { return m_bOpen; }
  #else // _NEW_ALGORITHM_2
	static bool		IsCloseCmd( UInt8 inTcpFlags );
	bool			IsHalfChannel() { return (m_IpPair.GetSource() == m_IpPair.GetDestination()); }
	bool			ProcessData( CByteVectRef& inTcpHdr );
  #endif // _NEW_ALGORITHM_2

	UInt64			GetLastPacketTime(){ return m_LastPacketTime; }

	CIpAddressPort	GetIpPort() { return m_IpPort; }
	CPeekString		GetUserName( bool inObscure ) {
		return (inObscure) ? g_strObscureName : m_strUserName;
	}
	const CIpAddressPortPair&	Key() const { return m_IpPair; }

	bool	IsChannelClosed() { return !m_bOpen; }
};

typedef NSTR1::shared_ptr<CDataChannel>	CDataChannelPtr;

#ifdef _NEW_ALGORITHM_2
// =============================================================================
//		CDataPortList
// =============================================================================

class CDataPortList
	:	public CAtlArray<UInt16>
{
public:
	;		CDataPortList() {}
	;		~CDataPortList() {}

	void	AddDataPort( const UInt16 inPort ) {
		if ( !Find( inPort ) ) {
			Add( inPort );
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
			}
		}
	}
};
#endif // _NEW_ALGORITHM_2


////////////////////////////////////////////////////////////////////////////////
//      CDataChannelMap
////////////////////////////////////////////////////////////////////////////////
typedef CAtlMap<CIpAddressPortPair, CDataChannelPtr, CIpAddressPortPairTraits>	CDataChannelMap;

////////////////////////////////////////////////////////////////////////////////
//      CDataChannelList
////////////////////////////////////////////////////////////////////////////////

class CDataChannelList
	:	public IStatistics
{

protected:

	CDataChannelMap	m_Map;
  #ifdef _NEW_ALGORITHM_2
	CDataPortList	m_DataPortList;
  #endif

	CDataChannel*	HalfFind( const CIpAddressPortPair& inIpPair );

public:
	;				CDataChannelList();
	virtual			~CDataChannelList();

	void			Close() {
		RemoveAll();
	}

	// IStatistics
	UInt32			GetStatValue( size_t ) { return m_Map.GetCount(); }
	void			ResetStatValue( size_t ) {}

	UInt32			GetCount() const { return m_Map.GetCount(); }

  #ifdef IMPLEMENT_MONITORING
	void			Monitor( DWORD dwTimeOut, bool inObscure, CArrayString& outRemovedStreams );
  #endif // IMPLEMENT_MONITORING

  #ifdef _NEW_ALGORITHM_2
	CDataChannel*	Find( const CIpAddressPortPair& inIpPortPair, bool& outIsHalfFind );
  #else // _NEW_ALGORITHM_2
	CDataChannel*	Find( const CIpAddressPortPair& inIpPortPair );
  #endif // _NEW_ALGORITHM_2

	bool			Add( CDataChannel* pDataChannel );

  #ifdef _NEW_ALGORITHM_2
	bool			IsDataPortOfInterest( UInt16 inPort1, UInt16 inPort2 ) {
		return m_DataPortList.IsDataPortOfInterest( inPort1, inPort2 );
	}

	void			RemoveAndDelete( CDataChannel* inDataChannel );
  #else //  _NEW_ALGORITHM_2
	void			RemoveAndDelete( CDataChannel* inDataChannel ) {
		m_Map.RemoveKey( inDataChannel->Key() );
	}
  #endif //  _NEW_ALGORITHM_2

	void			RemoveAll() {
		m_Map.RemoveAll();
	  #ifdef _NEW_ALGORITHM_2
		m_DataPortList.RemoveAll();
	  #endif // _NEW_ALGORITHM_2
	}

};
