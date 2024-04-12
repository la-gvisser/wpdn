// =============================================================================
//	Packet.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Packet.h"


// =============================================================================
//		CPacketLayers
// =============================================================================

CPacketLayers::CPacketLayers(
	void*	p /*= NULL*/ )
	:	m_pvLayers( NULL )
{
	SetPtr( p );
}

CPacketLayers::~CPacketLayers()
{
	ReleasePtr();
}


// -----------------------------------------------------------------------------
//		GetHeaderLayer
// -----------------------------------------------------------------------------

int
CPacketLayers::GetHeaderLayer(
	UInt16	ProtoSpecId,
	UInt32* pSourceProtoSpec,
	const	UInt8** ppData,
	UInt16*	pBytesRemaining  )
{
	IPacketLayers*	pLayers( reinterpret_cast<IPacketLayers*>( m_pvLayers ) );
	if ( pLayers == NULL ) return PEEK_PLUGIN_FAILURE;

	return pLayers->GetHeaderLayer( ProtoSpecId, pSourceProtoSpec, ppData, pBytesRemaining );
}


// -----------------------------------------------------------------------------
//		ReleasePtr
// -----------------------------------------------------------------------------

void
CPacketLayers::ReleasePtr()
{
	IPacketLayers*	pLayers( reinterpret_cast<IPacketLayers*>( m_pvLayers ) );
	if ( pLayers ) {
		pLayers->Release();
		m_pvLayers = NULL;
	}
}


// -----------------------------------------------------------------------------
//		SetPtr
// -----------------------------------------------------------------------------

void
CPacketLayers::SetPtr(
	void* p )
{
	IPacketLayers*	pLayers( reinterpret_cast<IPacketLayers*>( p ) );
	if ( pLayers ) {
		m_pvLayers = p;
		pLayers->AddRef();
	}
}


// =============================================================================
//		CPacket
// =============================================================================

CPacket::CPacket(
	void*	p /*= NULL*/ )
	:	m_pvPacket( NULL )
{
	SetPtr( p );
}

CPacket::~CPacket()
{
	ReleasePtr();
}


// -----------------------------------------------------------------------------
//		ClearFlag
// -----------------------------------------------------------------------------

int
CPacket::ClearFlag(
	UInt32	nFlag )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->ClearFlag( nFlag );
}


// -----------------------------------------------------------------------------
//		ClearStatus
// -----------------------------------------------------------------------------

int
CPacket::ClearStatus(
	UInt32	nStatus )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->ClearStatus( nStatus );
}


// -----------------------------------------------------------------------------
//		GetPacketLayers
// -----------------------------------------------------------------------------

bool
CPacket::ExtractText(
	const unsigned char*	pData,
	size_t					nDataLen,
	bool					bTerm,
	unsigned char			ucTerm,
	char					cNonPrint,
	char*					pText,
	size_t					nTextLen )
{
	// ToDo: modify to fill use a CStringA.
	// ToDo: modify to pass in an offset within the packet.
	// ToDo: then reallocate a string of length 'rest of packet'.
	// Sanity check.
	if ( pData == NULL ) return false;
	if ( pText == NULL ) return false;
	if ( nTextLen < 1 ) return false;
	
	size_t	nMaxLen = nTextLen - 1;
	size_t	nLen = 0;

	while ( (nLen < nDataLen) && (nLen < nMaxLen) ) {
		// Check for terminator.
		if ( bTerm && (*pData == ucTerm ) ) break;

		// Copy the data and increment the length.
		// Only copy data if its a char, otherwise replace with cNonPrint.
		unsigned char	c = *pData++;
		*pText++ = ((c >= 0x20) && (c <= 0x7E)) ? (char) c : cNonPrint;
		nLen++;
	}

	// Null terminate.
	*pText = 0;

	return true;
}


// -----------------------------------------------------------------------------
//		GetActualLength
// -----------------------------------------------------------------------------

int
CPacket::GetActualLength(
	UInt16&	nLength )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetActualLength( &nLength );
}


// -----------------------------------------------------------------------------
//		GetApplicationLayer
// -----------------------------------------------------------------------------

int
CPacket::GetApplicationLayer(
	void** pLayer )
{
	pLayer;
#if (0)
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;
#endif

	return PEEK_PLUGIN_FAILURE;;
}


// -----------------------------------------------------------------------------
//		GetFlags
// -----------------------------------------------------------------------------

int
CPacket::GetFlags(
	UInt32&	nFlags )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetFlags( &nFlags );
}


// -----------------------------------------------------------------------------
//		GetFlowId
// -----------------------------------------------------------------------------

int
CPacket::GetFlowId(
	UInt32&	nId )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetFlowID( &nId );
}


// -----------------------------------------------------------------------------
//		GetIPFragmentBits
// -----------------------------------------------------------------------------

int
CPacket::GetIPFragmentBits(
	UInt16&	nBits )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetIPFragmentBits( &nBits );
}


// -----------------------------------------------------------------------------
//		GetIPFragmentOffset
// -----------------------------------------------------------------------------

int
CPacket::GetIPFragmentOffset(
	UInt16&	nOffset )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetIPFragmentOffset( &nOffset );
}


// -----------------------------------------------------------------------------
//		GetIPId
// -----------------------------------------------------------------------------

int
CPacket::GetIPId(
	UInt16&	nId )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetIPID( &nId );
}


// -----------------------------------------------------------------------------
//		GetIPLength
// -----------------------------------------------------------------------------

int
CPacket::GetIPLength(
	UInt16&	nLength )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetIPLength( &nLength );
}


// -----------------------------------------------------------------------------
//		GetMediaSubType
// -----------------------------------------------------------------------------

int
CPacket::GetMediaSubType(
	TMediaSubType&	tMediaSubType )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetMediaSubType( &tMediaSubType );
}


// -----------------------------------------------------------------------------
//		GetMediaType
// -----------------------------------------------------------------------------

int
CPacket::GetMediaType(
	TMediaType&	tMediaType )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetMediaType( &tMediaType );
}


// -----------------------------------------------------------------------------
//		GetPacketData
// -----------------------------------------------------------------------------

int
CPacket::GetPacketData(
	UInt16&			nLength,
	const UInt8*&	pData )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	if ( HE_FAILED( pPacket->GetPacketData( &pData ) ) ) return PEEK_PLUGIN_FAILURE;
	const UInt8*	pEnd = NULL;
	if ( HE_FAILED( pPacket->GetPacketDataEnd( &pEnd ) ) ) return PEEK_PLUGIN_FAILURE;
	nLength = static_cast<UInt16>( pEnd - pData );
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		GetPacketLayers
// -----------------------------------------------------------------------------

int
CPacket::GetPacketLayers(
	CPacketLayers&	inLayers )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	CHeQIPtr<IPacketLayers>	spLayers( pPacket );
	if ( spLayers == NULL ) return PEEK_PLUGIN_FAILURE;
	inLayers.SetPtr( spLayers.p );
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		GetPacketLength
// -----------------------------------------------------------------------------

int
CPacket::GetPacketLength(
	UInt16&	nLength )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetPacketLength( &nLength );
}


// -----------------------------------------------------------------------------
//		GetPacketNumber
// -----------------------------------------------------------------------------

int
CPacket::GetPacketNumber(
	UInt64&	nPacketNumber )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetPacketNumber( &nPacketNumber );
}


// -----------------------------------------------------------------------------
//		GetProtoSpec
// -----------------------------------------------------------------------------

int
CPacket::GetProtoSpec(
	UInt32&	nProtoSpec )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetProtoSpec( &nProtoSpec );
}


// -----------------------------------------------------------------------------
//		GetProtoSpecId
// -----------------------------------------------------------------------------

int
CPacket::GetProtoSpecId(
	UInt16&	nProtoSpecId )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetProtoSpecID( &nProtoSpecId );
}


// -----------------------------------------------------------------------------
//		GetSliceLength
// -----------------------------------------------------------------------------

int
CPacket::GetSliceLength(
	UInt16&	nLength )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetSliceLength( &nLength );
}


// -----------------------------------------------------------------------------
//		GetStatus
// -----------------------------------------------------------------------------

int
CPacket::GetStatus(
	UInt32&	nStatus )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetStatus( &nStatus );
}


// -----------------------------------------------------------------------------
//		GetTCPUDPInfo
// -----------------------------------------------------------------------------

int
CPacket::GetTCPUDPInfo(
	UInt8**	/*ppHeader*/,
	UInt16*	/*pusHeaderBytes*/,
	UInt8**	/*ppPayload*/,
	UInt16*	/*pusPayloadBytes*/,
	UInt16*	/*pusTransportID*/ )
{
#if (0)
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;
#endif

	return PEEK_PLUGIN_FAILURE;
}


// -----------------------------------------------------------------------------
//		GetTimeStamp
// -----------------------------------------------------------------------------

int
CPacket::GetTimeStamp(
	UInt64&	nTimeStamp )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetTimeStamp( &nTimeStamp );
}


// -----------------------------------------------------------------------------
//		GetVoIPCallFlowIndex
// -----------------------------------------------------------------------------

int
CPacket::GetVoIPCallFlowIndex(
	UInt32&	nIndex )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetVoIPCallFlowIndex( &nIndex );
}


// -----------------------------------------------------------------------------
//		GetVoIPCallId
// -----------------------------------------------------------------------------

int
CPacket::GetVoIPCallId(
	UInt64&	nId )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->GetVoIPCallID( &nId );
}


// -----------------------------------------------------------------------------
//		IsBroadcast
// -----------------------------------------------------------------------------

int
CPacket::IsBroadcast(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsBroadcast( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsDescendentOf
// -----------------------------------------------------------------------------

int
CPacket::IsDescendentOf(
	const UInt16	payParent[],
	SInt32			nCount,
	UInt32*			pMatchInstanceId,
	SInt32*			pMatchIndex,
	bool&			bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsDescendentOf( payParent, nCount, pMatchInstanceId, pMatchIndex, &b ) ) ) {
		return PEEK_PLUGIN_FAILURE;
	}

	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsError
// -----------------------------------------------------------------------------

int
CPacket::IsError(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL		b = FALSE;
	if ( HE_FAILED( pPacket->IsError( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsFragment
// -----------------------------------------------------------------------------

int
CPacket::IsFragment(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsFragment( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsFullDuplex
// -----------------------------------------------------------------------------

int
CPacket::IsFullDuplex(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsFullDuplexPacket( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsMultiCaset
// -----------------------------------------------------------------------------

int
CPacket::IsMultiCaset(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsMulticast( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsWAN
// -----------------------------------------------------------------------------

int
CPacket::IsWAN(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsWanPacket( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		IsWireless
// -----------------------------------------------------------------------------

int
CPacket::IsWireless(
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = FALSE;
	if ( HE_FAILED( pPacket->IsWirelessPacket( &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		ReleasePtr
// -----------------------------------------------------------------------------

void
CPacket::ReleasePtr()
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket ) {
		pPacket->Release();
		m_pvPacket = NULL;
	}
}


// -----------------------------------------------------------------------------
//		SetFlag
// -----------------------------------------------------------------------------

int
CPacket::SetFlag(
	UInt32	nFlag )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->SetFlag( nFlag );
}


// -----------------------------------------------------------------------------
//		SetPtr
// -----------------------------------------------------------------------------

void
CPacket::SetPtr(
	void* p )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( p ) );
	if ( pPacket ) {
		m_pvPacket = p;
		pPacket->AddRef();
	}
}


// -----------------------------------------------------------------------------
//		GetPtr
// -----------------------------------------------------------------------------

IPacket* CPacket::GetPtr() const
{
	IPacket*	pPacket = 0;
	pPacket = reinterpret_cast<IPacket*>( m_pvPacket );
	if ( pPacket ) {		
		pPacket->AddRef();
	}
	return pPacket;
}


// -----------------------------------------------------------------------------
//		SetStatus
// -----------------------------------------------------------------------------

int
CPacket::SetStatus(
	UInt32	nStatus )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	return pPacket->SetStatus( nStatus );
}


// -----------------------------------------------------------------------------
//		TestFlag
// -----------------------------------------------------------------------------

int
CPacket::TestFlag(
	UInt32	nFlag,
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = false;
	if ( HE_FAILED( pPacket->TestFlag( nFlag, &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}


// -----------------------------------------------------------------------------
//		TestStatus
// -----------------------------------------------------------------------------

int
CPacket::TestStatus(
	UInt32	nStatus,
	bool&	bResult )
{
	IPacket*	pPacket( reinterpret_cast<IPacket*>( m_pvPacket ) );
	if ( pPacket == NULL ) return PEEK_PLUGIN_FAILURE;

	BOOL	b = false;
	if ( HE_FAILED( pPacket->TestStatus( nStatus, &b ) ) ) return PEEK_PLUGIN_FAILURE;
	bResult = (b != FALSE);
	return PEEK_PLUGIN_SUCCESS;
}
