// =============================================================================
//	PktFile.cpp
//		implementation of the CPktFile class.
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PktFile.h"
#include "Environment.h"


///////////////////////////////////////////////////////////////////////////////
//		CPktFile
///////////////////////////////////////////////////////////////////////////////

CPktFile::CPktFile()
	:	m_nCreateTime( 0 )
	,	m_nCloseTime( 0 )
	,	m_nBytesWritten( 0 )
	,	m_nPacketCount( 0 )
	,	m_bSaveFlags( kSaveFlag_None )
	,	m_nSaveAge( 0 )
	,	m_nSaveSize( 0 )
{
	m_Id.Enable( true );
}


CPktFile::~CPktFile()
{
	StopCapture();
}


// -----------------------------------------------------------------------------
//		AddPacket
// -----------------------------------------------------------------------------

bool
CPktFile::AddPacket(
	const PeekPacket&	inPacket,
	UInt32				inDataSize,
	const UInt8*		inPacketData )
{
	try {
		if ( !m_Writer.IsOpen() ) {
			if ( !Open() ) {
				return false;
			}
		}

		// Calculate the actual length.
		UInt16	nActualLength = (inPacket.fSliceLength > 0)
			? inPacket.fSliceLength
			: inPacket.fPacketLength;
		if ( nActualLength < static_cast<UInt16>(inDataSize) ) {
			nActualLength = static_cast<UInt16>(inDataSize);
		}

		// write the packet
		DWORD	dwWritten = 0;
		m_Packet.SetPacket( &inPacket, inPacketData, nActualLength );
		m_Packet.Write( m_Writer, &dwWritten );
		m_nBytesWritten += dwWritten;

		m_nPacketCount++;

		if ( CheckClose() ) {
			Close();
		}
	}
	catch ( ... ) {
		TRACE( L"Caught Exception in %s\n", __FUNCTION__ );
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Close
// -----------------------------------------------------------------------------

void
CPktFile::Close()
{
	if ( !m_Writer.IsOpen() ) return;
	m_Writer.Flush();

	// Update the Session Block
	m_Session.SetPacketCount( GetPacketCount() );
	m_Session.SetSessionEndTime( time( NULL ) );
	m_Session.Write( m_Writer );

	m_Packet.UpdateHeader( m_Writer, true );

	m_Writer.Close();
	::MoveFile( m_strOpenFilePath, m_strCloseFilePath );
	m_Writer.WriteIntegrityFile( m_strCloseFilePath );
}


// -----------------------------------------------------------------------------
//		Monitor
// -----------------------------------------------------------------------------

void
CPktFile::Monitor()
{
	if ( CheckClose() ) {
		Close();
	}
}


// -----------------------------------------------------------------------------
//		NextFilePath
// -----------------------------------------------------------------------------

void
CPktFile::NextFilePath()
{
//	CPeekString	strFilePath;
//	CPeekString	strPath;

	MakePath( m_strOutputDirectory );

	CPeekString	strTimeStamp;
	int	x = 0;
	do {
		strTimeStamp = GetTimeStringFileName( true );

		CPeekString strPreAmble = m_strPreAmble;

		if (strPreAmble.GetLength() > 0 ) strPreAmble += L"_";

		CPeekOutString strOpenFilePath, strCloseFilePath;

		strOpenFilePath << m_strOutputDirectory << L"(In Progress) " << strPreAmble << strTimeStamp << m_strPostAmble;
		m_strOpenFilePath = strOpenFilePath;

		strCloseFilePath << m_strOutputDirectory << strPreAmble << strTimeStamp << m_strPostAmble;
		m_strCloseFilePath = strCloseFilePath;

		if ( x++ > 100 ) throw 0;
	} while ( ::PathFileExists( m_strOpenFilePath ) || ::PathFileExists( m_strCloseFilePath ) );
}

// -----------------------------------------------------------------------------
//		Open
// -----------------------------------------------------------------------------

bool
CPktFile::Open()
{
	DWORD	nResult = 0;

	NextFilePath();
	bool	bResult = m_Writer.Open( m_strOpenFilePath );
	if ( !bResult ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	m_nCreateTime = GetTimeStamp();
	m_nCloseTime = m_nCreateTime + m_nSaveAge;
	m_nBytesWritten = 0;
	m_nPacketCount = 0;

	// Version Block
	bResult = m_Version.Write( m_Writer );
	if ( !bResult ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	// Session Block
	m_Session.SetWriteTime( time( NULL ) );
	m_Session.SetPacketCount( 0 );
	m_Session.SetSessionStartTime( GetTimeStamp() );
	m_Session.SetSessionEndTime( 0 );
	m_Session.SetFileIndex( m_Id.GetIndex() + 1 );
	m_Session.Reset();
	bResult = m_Session.Write( m_Writer );
	if ( !bResult ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	// Capture Id
	bResult = m_Id.Write( m_Writer );
	if ( !bResult ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	// Packet
	m_Packet.Reset();
	bResult = m_Packet.WriteHeader( m_Writer );
	if ( !bResult ) {
		nResult = m_Writer.GetLastError();
		return false;
	}

	m_Writer.Flush();
	m_nBytesWritten = m_Writer.GetFilePosition();

	return true;
}


// -----------------------------------------------------------------------------
//		StartCapture
// -----------------------------------------------------------------------------

bool
CPktFile::StartCapture(
	LPCTSTR lpszPreAmble,
	LPCTSTR	lpszPostAmble,
	LPCTSTR	lpszOutputDirectory,
	UInt32	nSeconds /* = 0 */,
	UInt64	nBytes /* = 0 */ )
{
	m_strPreAmble = lpszPreAmble;

	m_strPostAmble = lpszPostAmble;
	
	if ( (m_strPostAmble.GetLength() < 5) || (m_strPostAmble.Right( 4 ).CompareNoCase( L".pkt" ) ) != 0 ) {
		m_strPostAmble.Append( L".pkt" );
	}

	m_strOutputDirectory = lpszOutputDirectory;
	if ( m_strOutputDirectory.Right( 1 ).Compare( L"\\" ) != 0 ) {
		m_strOutputDirectory.Append( L"\\" );
	}

	m_bSaveFlags = kSaveFlag_None;
	if ( nSeconds > 0 ) {
		m_bSaveFlags |= kSaveFlag_Age;
		m_nSaveAge = static_cast<UInt64>(nSeconds) * 10000000;	// convert seconds to 100-nanoseconds
	}
	if ( nBytes > 0 ) {
		m_bSaveFlags |= kSaveFlag_Size;
		m_nSaveSize = nBytes;
	}

	// Version Block

//	CVersion	outFile,
//	CVersion	outProduct,
//	
//		GetModuleVersion(
//		CVersion&	outFile,
//		CVersion&	outProduct,
//		HMODULE		inModule )
	

	ENV_VERSION	evFile;
	ENV_VERSION	evProduct;

//	bool bResult = GetPeekVersion( evFile, evProduct );
	CEnvironment env;
	bool bResult = 	env.GetModuleVersion( evFile, evProduct );
	if ( !bResult ) {
		return false;
	}
	m_Version.SetCurrentVersion( evFile, evProduct );

	// Session Block
	TIME_ZONE_INFORMATION	timeZone;
	::GetTimeZoneInformation( &timeZone );

	m_Session.SetTimeZoneBias( timeZone.Bias );
	m_Session.SetMediaType( kMediaType_802_3 );
	m_Session.SetMediaSubType( kMediaSubType_Native );
	m_Session.SetLinkSpeed( 100 );
	m_Session.SetPacketCount( 0 );

	// Capture Id
	m_Id.SetId();

	// Open the first file now, timestamp matches Start Capture.
	Open();

	return true;
}


// -----------------------------------------------------------------------------
//		StopCapture
// -----------------------------------------------------------------------------

void
CPktFile::StopCapture()
{
	Close();
}
