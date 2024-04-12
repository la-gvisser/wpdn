// ============================================================================
//	DualWriter.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PktFile.h"
#include "CaptureLogger.h"
#include "Options.h"
#include "Packet.h"

typedef void (CALLBACK * FileRefreshCallbackProc)( void* pUserArg, bool bAboutToRestart );


// ============================================================================
//	CDualWriter
// ============================================================================

class CDualWriter 
	: public 
	    CPktFile,
	    CCaptureLogger
{
	enum {
		kSaveFlag_None = 0x0000,
		kSaveFlag_Age  = 0x0001,
		kSaveFlag_Size = 0x0002
	};

public:

	CDualWriter( bool bIsLogMessages = false );
	~CDualWriter() {
		Close();
	}

	bool	StartCapture( const CPeekString& strOutputDirectory, UInt32 nSaveAgeSeconds = 0, UInt64 nSaveSizeBytes = 0,
							const CPeekString& strCaptureStartString = _T("") );
	bool	AddPacket( CPacket& inPacket, const bool bRadius );
	void	Monitor( COptions * pOptions );
	void	ResetFileRefreshValues( UInt32 nSaveAgeSeconds, UInt64 nSaveSizeBytes );
	void	SetPreAmble( const CPeekString& inPreAmble, bool bUseFileNumInFileName );

	bool	IsOpen() {
		#ifdef _DEBUG
		  if ( IsLogMessages() ) {
		  	ASSERT( CCaptureLogger::IsOpen() );
		  }
		#endif
		return m_Writer.IsOpen();
	}
	UInt32	GetTotalNumPacketsWritten() {
		return m_nTotalPacketsWritten;
	}
	UInt32	GetNumPacketsWritten() {
		return m_nPacketsWrittenSinceRefresh;
	}
	bool	IsLogMessages() {
		return m_bIsLogMessages;
	}
	void	SetIsLogMessages( bool bIsLogMessages ) {
		m_bIsLogMessages = bIsLogMessages;
	}
	bool	WriteLog( const CPeekString& strMessage ) {
		if ( m_bIsLogMessages ) {
			return CCaptureLogger::Write( strMessage );
		}
		return false;
	}
	void	StopCapture( const CPeekString& strCaptureStopString ) {
		if ( (m_bIsLogMessages) && (strCaptureStopString.GetLength() > 0) ) {
			CCaptureLogger::Write( strCaptureStopString );
		}
		StopCapture();
	}
	void	StopCapture() {
		Close();
	}
	void	RegisterFileRefreshCallback( FileRefreshCallbackProc inFileRefreshCallbackProc, void* pUserArg ) {
		m_FileRefreshCallbackProc = inFileRefreshCallbackProc;
		m_pCallbackUserArg = pUserArg;
	}
	UInt32	GetFileNumber() {
		return m_nFileNumber;
	}
	CPeekString GetPreAmble() {
		return CPktFile::m_strPreAmble;
	}
	CPeekString GetPostAmble() {
		CPeekString strReturnPostAmble;
		size_t	iLen = CPktFile::m_strPostAmble.GetLength();
		if ( iLen < 5 ) return L"";
		if ( CPktFile::m_strPostAmble.Right(4).CompareNoCase( L".pkt" ) == 0 ) {
			strReturnPostAmble = CPktFile::m_strPostAmble.Left( iLen - 4 );
		}
		return strReturnPostAmble;
	}

#ifdef _DEBUG
	void DebugValidateCaptureSettings( void* pUserArg ) {
		ASSERT( GetFileNumber() > 0 );
		ASSERT( pUserArg == m_pCallbackUserArg );
	}
#endif

protected:
	void SetLoggerStartStopFiles();
	bool RestartWriters();
	void ResetPreAmble();

	void IncrementPacketsWritten() {
		m_nPacketsWrittenSinceRefresh++;
		m_nTotalPacketsWritten++;
	}
	void RefreshPacketsWritten() {
		m_nPacketsWrittenSinceRefresh = 0;
	}
	void Close() {
		CPktFile::Close();
		CCaptureLogger::Close();
	}
	void InformCallerOfRestart( bool bAboutToRestart ) {
		if ( m_FileRefreshCallbackProc ) {
			(m_FileRefreshCallbackProc)( m_pCallbackUserArg, bAboutToRestart );
		}
	}
	void IncrementFileNumber() {
		m_nFileNumber++;
	}

private:
	// Disallow copy constructor
	CDualWriter( const CDualWriter& inDualWriter ) {
		ASSERT( 0 );
		inDualWriter;
	}
	CDualWriter& operator=( const CDualWriter& inDualWriter ) {
		ASSERT( 0 );
		inDualWriter;
		return *this;
	}

protected:
	bool	m_bIsLogMessages;
	UInt32	m_nPacketsWrittenSinceRefresh;
	UInt32	m_nTotalPacketsWritten;
	CPeekString m_strPrefix;
	UInt32	m_nFileNumber;
	bool	m_bUseFileNumInFileName;

	FileRefreshCallbackProc m_FileRefreshCallbackProc;
	void*					m_pCallbackUserArg;
};
