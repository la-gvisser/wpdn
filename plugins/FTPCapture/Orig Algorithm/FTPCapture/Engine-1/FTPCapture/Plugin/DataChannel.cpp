// =============================================================================
//	DataChannel.cpp
//  implements CDataChannel class
// =============================================================================
//	Copyright (c) 2005 WildPackets, Inc.

#include "StdAfx.h"
#include "DataChannel.h"
#include "PeekTime.h"

// -----------------------------------------------------------------------------
//		Static
// -----------------------------------------------------------------------------

CPeekString	CDataChannel::g_strObscureName = _T( "******" );


////////////////////////////////////////////////////////////////////////////////
//      CDataChannel
////////////////////////////////////////////////////////////////////////////////

CDataChannel::CDataChannel(
		const CIpAddressPort&		inIpPort
	,	const CIpAddressPortPair&	inIpPair
	,	const CPeekString&			inUserName
  #ifdef _NEW_ALGORITHM_2
	,	CFtpSession* inMyFtpSession
  #endif
	)
	:	m_bOpen( true )
	,	m_IpPort( inIpPort )
	,	m_IpPair( inIpPair )
	,	m_strUserName( inUserName )
	,	m_LastPacketTime( 0 )
  #ifdef _NEW_ALGORITHM_2
	,	m_pFtpMySession( inMyFtpSession )
  #endif
{
	m_LastPacketTime = GetTickCount64();
}


#ifndef _NEW_ALGORITHM_2
// -----------------------------------------------------------------------------
//		IsCloseCmd
// -----------------------------------------------------------------------------

bool
CDataChannel::IsCloseCmd(
	UInt8	inTcpFlags )
{
	if ( (inTcpFlags & kTCPFlag_FIN) ||
		//(inTcpFlags & kTCPFlag_SYN) ||
		(inTcpFlags & kTCPFlag_RST) ) {
			return true;
	}
	return false;
}
#endif // !_NEW_ALGORITHM_2


#ifndef _NEW_ALGORITHM_2
// -----------------------------------------------------------------------------
//      ProcessData
// -----------------------------------------------------------------------------

bool
CDataChannel::ProcessData(
	CByteVectRef&	inTcpHdr )
{
	if ( m_bOpen ) {
		m_LastPacketTime = GetTickCount64();

		tTcpHeader*	pTcpHdr = (tTcpHeader*) inTcpHdr.GetData( sizeof( tTcpHeader ) );
		if ( pTcpHdr ) {
			bool	bIsClose = IsCloseCmd( pTcpHdr->Flags );
			if ( bIsClose ) {
				m_bOpen = false;
			}
		}
		return true;
	}
	return false;
}
#endif // !_NEW_ALGORITHM_2


////////////////////////////////////////////////////////////////////////////////
//      CDataChannelList
////////////////////////////////////////////////////////////////////////////////

CDataChannelList::CDataChannelList()
{
	m_Map.InitHashTable( 257 );
}

CDataChannelList::~CDataChannelList()
{
	RemoveAll();
}


// -----------------------------------------------------------------------------
//		HalfFind
// -----------------------------------------------------------------------------

CDataChannel*
CDataChannelList::HalfFind(
	const CIpAddressPortPair&	inIpPair )
{
	CDataChannel*		ippResult = NULL;

	POSITION			pos = m_Map.GetStartPosition();
	while ( pos != NULL ) {
		CDataChannelMap::CPair*	pCur = m_Map.GetNext( pos );

		CIpAddressPortPair	ippKey = pCur->m_key;
		CDataChannelPtr		DataChannel = pCur->m_value;

		if ( DataChannel->IsHalfChannel() ) {
		  #ifdef _NEW_ALGORITHM_2
			CIpAddressPair Pair1( ippKey.GetSrcAddress(), ippKey.GetDstAddress() );
			CIpAddressPair Pair2( inIpPair.GetSrcAddress(), inIpPair.GetDstAddress() );

			if ( Pair1 == Pair2 ) {
				ippResult = DataChannel.get();
				break;
			}
			Pair1.Invert();
			if ( Pair1 == Pair2 ) {
				ippResult = DataChannel.get();
				break;
			}
		  #else // _NEW_ALGORITHM_2
			if ( ippKey.GetSource() == inIpPair.GetSource() ) {
				ippResult = DataChannel.get();
				break;
			}
			if ( ippKey.GetDestination() == inIpPair.GetDestination() ) {
				ippResult = DataChannel.get();
				break;
			}
		  #endif // _NEW_ALGORITHM_2
		}
	}

	// The found item's source and destination address-port pair are the same.
	// Remove it from the list and add a new entry from the input.
	if ( ippResult != NULL ) {
	  #ifdef _NEW_ALGORITHM_2
		CDataChannelPtr	pDataChannel( new CDataChannel( ippResult->GetIpPort(), inIpPair,
										ippResult->GetUserName( false ), ippResult->GetSession() ) );
	  #else // _NEW_ALGORITHM_2
		CDataChannelPtr	pDataChannel( new CDataChannel( ippResult->GetIpPort(), inIpPair,
			ippResult->GetUserName( false ) ) );
	  #endif // _NEW_ALGORITHM_2

		m_Map.RemoveKey( ippResult->Key() );

		POSITION	pos = m_Map.SetAt( pDataChannel->Key(), pDataChannel );
		ippResult = m_Map.GetValueAt( pos ).get();
	}

	return ippResult;
}


#ifdef IMPLEMENT_MONITORING
// -----------------------------------------------------------------------------
//		Monitor
// -----------------------------------------------------------------------------

void
CDataChannelList::Monitor(
	DWORD			dwTimeOut,
	bool			inObscure,
	CArrayString&	outRemovedStreams )
{
	UInt64	nCurTime = GetTickCount64();
	POSITION	pos = m_Map.GetStartPosition();
	while ( pos != NULL ) {
		CDataChannelMap::CPair*	pCur = m_Map.GetNext( pos );
		CIpAddressPortPair	ippKey = pCur->m_key;
		CDataChannelPtr		DataChannel( pCur->m_value );

		DWORD	dwDelta = static_cast<DWORD>( nCurTime - DataChannel->GetLastPacketTime() );
		bool	bRemove = (dwDelta > dwTimeOut);
		if ( bRemove ) {
			CPeekOutString	strRemoved;
			strRemoved	<< L"User " << DataChannel->GetUserName( inObscure ) << L" Data connection " 
						<< DataChannel->GetIpPort().Format();
			outRemovedStreams.Add( strRemoved );
			m_Map.RemoveKey( ippKey );
		}
	}
}
#endif // IMPLEMENT_MONITORING


// -----------------------------------------------------------------------------
//		Find
// -----------------------------------------------------------------------------

CDataChannel*
CDataChannelList::Find(
	const CIpAddressPortPair&	inIpPair
  #ifdef _NEW_ALGORITHM_2
	,	bool& outIsHalfFind
  #endif
	)
{
	CDataChannelMap::CPair*	pCur = m_Map.Lookup( inIpPair );
	if ( pCur ) {
		return pCur->m_value.get();
	}

	CIpAddressPortPair	invIpPair;
	inIpPair.Invert( invIpPair );
	pCur = m_Map.Lookup( invIpPair );
	if ( pCur ) {
		return pCur->m_value.get();
	}

	CDataChannel* pDataChannel( HalfFind( inIpPair ) );

  #ifdef _NEW_ALGORITHM_2
	outIsHalfFind = false;
	if ( pDataChannel ) outIsHalfFind = true;
  #endif

	return pDataChannel;
}


#ifdef _NEW_ALGORITHM_2
// -----------------------------------------------------------------------------
//		RemoveAndDelete
// -----------------------------------------------------------------------------

void
CDataChannelList::RemoveAndDelete( 
	CDataChannel* inDataChannel ) {

	CPort PortToRemove( inDataChannel->GetIpPort().GetPort() );

	m_Map.RemoveKey( inDataChannel->Key() );

	bool	bDataPortStillInUse( false );

	POSITION			pos = m_Map.GetStartPosition();
	while ( pos != NULL ) {
		CDataChannelMap::CPair*	pCur = m_Map.GetNext( pos );

		CDataChannelPtr		DataChannel( pCur->m_value );
		CPort thePort = DataChannel->GetIpPort().GetPort();
		if ( thePort == PortToRemove ) {
			bDataPortStillInUse = true;
			break;
		}
	}

	if ( bDataPortStillInUse == false ) {
		m_DataPortList.Remove( PortToRemove );
	}
}
#endif // _NEW_ALGORITHM_2


// -----------------------------------------------------------------------------
//		Add
// -----------------------------------------------------------------------------

bool
CDataChannelList::Add(
	CDataChannel*			inDataChannel )
{
	if ( inDataChannel == NULL ) return false;

	m_Map.SetAt( inDataChannel->Key(), CDataChannelPtr( inDataChannel ) );

  #ifdef _NEW_ALGORITHM_2
	m_DataPortList.AddDataPort( inDataChannel->GetIpPort().GetPort() );
  #endif

	return true;
}
