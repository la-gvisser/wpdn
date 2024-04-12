// =============================================================================
//	DualWriter.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "DualWriter.h"


// ============================================================================
//	CDualWriter
// ============================================================================

// -----------------------------------------------------------------------------
//		Construction
// -----------------------------------------------------------------------------

CDualWriter::CDualWriter( 
	bool bIsLogMessages ) 
	: m_bIsLogMessages( bIsLogMessages )
	, m_nPacketsWrittenSinceRefresh( 0 )
	, m_nTotalPacketsWritten( 0 )
	, m_FileRefreshCallbackProc( NULL )
	, m_pCallbackUserArg( NULL )
	, m_nFileNumber( 0 )
	, m_bUseFileNumInFileName( false )
{

}

// -----------------------------------------------------------------------------
//		StartCapture
// -----------------------------------------------------------------------------

bool
CDualWriter::StartCapture( 
	const CPeekString&  strOutputDirectory,
	UInt32				nSeconds /*= 0*/,
	UInt64				nBytes /*= 0*/,
	const CPeekString&	strCaptureStartString /* = L""*/ )
{
	CPeekString strPreAmble = GetPreAmble();
	CPeekString strPostAmble = GetPostAmble();

	bool bPktReturn = CPktFile::StartCapture( strPreAmble, strPostAmble, strOutputDirectory, nSeconds, nBytes );
	if ( !bPktReturn ) return false;

	ASSERT( m_nFileNumber == 1 );

	if ( m_bIsLogMessages ) {
		SetLoggerStartStopFiles();

		bool bLoggerResult = CCaptureLogger::Open();
		ASSERT( bLoggerResult );
		if ( !bLoggerResult ) {
			CPktFile::StopCapture();
			return false;
		}

		CCaptureLogger::Write( strCaptureStartString );
	}

	return true;
}

// -----------------------------------------------------------------------------
//		AddPacket
//  !!! This function is a re-write of CPktFile::AddPacket() !!!
//  !!! If CPktFile::AddPacket() changes it must be updated here!!!
//  Note that the section to be modified is identified below with !!!
// -----------------------------------------------------------------------------

bool
CDualWriter::AddPacket(
	CPacket&			inPacket,
	const bool			bRadius )
{
	try {

	// !!! Begin custom changes to CPktFile::AddPacket()
	// Replace this code:
		//if ( !m_Writer.IsOpen() ) {
		//	if ( !Open() ) {
		//		return false;
		//	}
		//}
	// With the following code:
		if ( !CPktFile::m_Writer.IsOpen() ) {
			ResetPreAmble();
			if ( !CPktFile::Open() ) {
				return false;
			}
		}

	#ifndef TEST_NO_CHECKCLOSE
		if ( CPktFile::CheckClose() ) {
			if ( !bRadius ) { // We will let queued Radius packets write before performing the refresh
				if ( !RestartWriters() ) {
					return false;
				}
			}
		}
	#else
		bRadius;
	#endif
	// !!! End custom changes to CPktFile::AddPacket

		const UInt8*	pPacketData( NULL );
		UInt16			nDataLen = inPacket.GetPacketData( pPacketData );
		ASSERT( nDataLen > 0 );
		if ( nDataLen == 0 ) return false;

		const PeekPacket	fPacket = inPacket.GetPeekPacket();

		UInt16	nPacketLength = inPacket.GetPacketLength();

		ASSERT( nPacketLength >= nDataLen + 4 ); // GetPacketData return does not include FCS or extra bytes.

		// Adjust if the above assertion triggers (should never happen).
		if ( nPacketLength < nDataLen + 4 ) {
			nPacketLength = nDataLen + 4;
		}

		// Get the slice length, if any.
		UInt16	nSliceLength = inPacket.GetSliceLength();

		// Test - can remove if the assertion ever triggers.
		// I don't believe sliced packets ever come on the filter thread.
		ASSERT( nSliceLength == 0 );

		// Calculate the actual length in case there is a slice length.
		UInt16	nActualLength = (nSliceLength > 0) ? nSliceLength : nPacketLength;

		// write the packet
		DWORD	dwWritten = 0;
		m_Packet.SetPacket( &fPacket, pPacketData, nActualLength );
		m_Packet.Write( m_Writer, &dwWritten );
		m_nBytesWritten += dwWritten;

		m_nPacketCount++;

// !!! Begin custom changes to CPktFile::AddPacket()
// Replace this code:
//		if ( CheckClose() ) {
//			Close();
//		}
// With the following code:
		IncrementPacketsWritten();
// !!! End custom changes to CPktFile::AddPacket

	}
	catch ( ... ) {
		TRACE( _T( "Caught Exception in %s\n" ), __FUNCTION__ );
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------------
//		RestartWriters
// -----------------------------------------------------------------------------

bool
CDualWriter::RestartWriters()
{
	InformCallerOfRestart( true );

	CPktFile::Close();

	ResetPreAmble();

	if ( !CPktFile::Open() ) {
		return false;
	}

	RefreshPacketsWritten();

	if ( m_bIsLogMessages ) {
		CCaptureLogger::Close();
		SetLoggerStartStopFiles();
		if ( !CCaptureLogger::Open() ) {
			CPktFile::Close();
			return false;
		}
	}

	InformCallerOfRestart( false );

	return true;
}

// -----------------------------------------------------------------------------
//		SetLoggerStartStopFiles
// -----------------------------------------------------------------------------

void
CDualWriter::SetLoggerStartStopFiles()
{
	if ( m_bIsLogMessages ) {
		CPeekString strOpenFilePath = CPktFile::m_strOpenFilePath;
		CPeekString strOpenLoggerPath = strOpenFilePath;
		if ( ( strOpenFilePath.GetLength() > 5 ) && ( strOpenFilePath.Right( 4 ).CompareNoCase( kFileTypePkt ) == 0 ) ) {
			strOpenLoggerPath = (strOpenFilePath.Left( strOpenFilePath.GetLength() - 4) );
		}
		strOpenLoggerPath += kFileTypeLog;

		CPeekString strCloseFilePath = CPktFile::m_strCloseFilePath;
		CPeekString strCloseLoggerPath = strCloseFilePath;
		if ( ( strCloseFilePath.GetLength() > 5 ) && ( strCloseFilePath.Right( 4 ).CompareNoCase( kFileTypePkt ) == 0 ) ) {
			strCloseLoggerPath = ( strCloseFilePath.Left( strCloseFilePath.GetLength() - 4) );
		}
		strCloseLoggerPath += kFileTypeLog;

		CCaptureLogger::SetFileNames( strOpenLoggerPath, strCloseLoggerPath );
	}
}

// -----------------------------------------------------------------------------
//		Monitor
// -----------------------------------------------------------------------------

void
CDualWriter::Monitor( COptions * pOptions )
{
	ASSERT( pOptions->IsSaveAgeSelected() );
	if ( !pOptions->IsSaveAgeSelected() ) {
		return;
	}
#ifndef TEST_NO_CHECKCLOSE
	if ( CPktFile::CheckClose() ) {
		if ( !RestartWriters() ) {
			return;
		}
	}
#endif
}

// -----------------------------------------------------------------------------
//		SetPreAmble
// -----------------------------------------------------------------------------

void
CDualWriter::SetPreAmble( 
	const CPeekString&	inPrefix,
	bool				bUseFileNumInFileName ) 
{
	m_strPrefix = inPrefix;
	m_bUseFileNumInFileName = bUseFileNumInFileName;

	ResetPreAmble();
}

// -----------------------------------------------------------------------------
//		ResetPreAmble
// -----------------------------------------------------------------------------

void
CDualWriter::ResetPreAmble() {

	IncrementFileNumber();

	CPeekOutString strPrefix;

	if ( m_bUseFileNumInFileName ) {
		if ( m_strPrefix.GetLength() > 0 ) {
			strPrefix << m_strPrefix << L"_" << m_nFileNumber;
		}
		else {
			strPrefix << m_nFileNumber;
		}
	}
	else {
		strPrefix << m_strPrefix;
	}

	CPktFile::m_strPreAmble = strPrefix;
}

// -----------------------------------------------------------------------------
//		ResetFileRefreshValues
// -----------------------------------------------------------------------------

void
CDualWriter::ResetFileRefreshValues( UInt32 nSaveAgeSeconds, UInt64 nSaveSizeBytes )
{
	CPktFile::m_bSaveFlags = kSaveFlag_None;

	CPktFile::m_nSaveAge = 0;
	CPktFile::m_nCloseTime = 0;
	CPktFile::m_nSaveSize = 0;

	if ( nSaveAgeSeconds > 0 ) {
		CPktFile::m_bSaveFlags |= kSaveFlag_Age;
		CPktFile::m_nSaveAge = static_cast<UInt64>(nSaveAgeSeconds) * 10000000;	// convert seconds to 100-nanoseconds
		CPktFile::m_nCloseTime = CPktFile::m_nCreateTime + CPktFile::m_nSaveAge;
	}
	if ( nSaveSizeBytes > 0 ) {
		CPktFile::m_bSaveFlags |= kSaveFlag_Size;
		CPktFile::m_nSaveSize = nSaveSizeBytes;
	}
}
