// =============================================================================
//	PeekFile.cpp
//		implementation of the CPeekFile class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#include "StdAfx.h"
#include "PeekFile.h"


///////////////////////////////////////////////////////////////////////////////
//		CPeekFile
///////////////////////////////////////////////////////////////////////////////

CPeekFile::CPeekFile(
	tIntegrityOptions&	inOptions )
	:	m_Writer( inOptions )
{
}


CPeekFile::~CPeekFile()
{
	Close();
}


// -----------------------------------------------------------------------------
//		Open
// -----------------------------------------------------------------------------
bool
CPeekFile::Open(
	LPCTSTR	lpszFile )
{
	m_Writer.Open( lpszFile );

	DWORD	nResult = 0;

	// Version Block
	TCHAR 	szFilePath[MAX_PATH];
	::GetModuleFileName( NULL, szFilePath, MAX_PATH );

	ENV_VERSION	evFile;
	ENV_VERSION	evProduct;
	GetPeekVersion( evFile, evProduct );
	m_Version.SetCurrentVersion( evFile, evProduct );
	if ( !m_Version.Write( m_Writer ) ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	// Session Block
	m_Session.SetWriteTime( time( NULL ) );

	TIME_ZONE_INFORMATION	timeZone;
	::GetTimeZoneInformation( &timeZone );

	m_Session.SetTimeZoneBias( timeZone.Bias );
	m_Session.SetMediaType( kMediaType_802_3 );
	m_Session.SetMediaSubType( kMediaSubType_Native );
	m_Session.SetLinkSpeed( 100 );
	m_Session.SetPacketCount( 0 );

	// save the session block now
	if ( !m_Session.Write( m_Writer ) ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	// Capture Id
	m_Id.SetId();
	if ( !m_Id.Write( m_Writer ) ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	m_Packet.SetCompressed( false );	// compression is off
	m_Packet.SetAppend( true );			// no updating the block header,

	return true;
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------
void
CPeekFile::Close()
{
	m_Writer.Flush();
	m_Writer.FinishIntegrityFile();
	m_Writer.Close();
	m_Writer.WriteIntegrityFile();
}

#if 0
// -----------------------------------------------------------------------------
//		AddMsg
// -----------------------------------------------------------------------------
bool
CPeekFile::AddMsg(
	const CMsgItem*	inMsgItem )
{
	inMsgItem;

	__try
	{
		if ( !m_Writer.IsOpen() ) {
			return false;
		}
		const PeekPacket&	Packet = (const PeekPacket&) inMsgItem->GetPacket();

		// Calculate the actual length.
		UInt16	nActualLength = (Packet.fSliceLength > 0)
			? Packet.fSliceLength
			: Packet.fPacketLength;

		// write the packet
		DWORD	dwWritten = 0;
		m_Packet.SetPacket( &Packet, inMsgItem->GetPacketData(), nActualLength );
		m_Packet.Write( m_Writer, &dwWritten );
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		TRACE(_T("Caught Exception in %s\n"), __FUNCTION__ );
	}

	return true;
}

#endif
