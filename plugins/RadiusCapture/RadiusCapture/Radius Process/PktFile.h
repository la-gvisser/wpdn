// ============================================================================
//	PktFile.h
//		interface for the CPktFile class.
// ============================================================================	
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "IntegrityFileWriter.h"
#include "Utils.h"

class CPktFile
{
	enum {
		kSaveFlag_None = 0x0000,
		kSaveFlag_Age  = 0x0001,
		kSaveFlag_Size = 0x0002
	};

protected:
	CPeekString		m_strPreAmble;
	CPeekString		m_strPostAmble;
	CPeekString		m_strOutputDirectory;

	CPeekString		m_strOpenFilePath;
	CPeekString		m_strCloseFilePath;
	UInt64			m_nCreateTime;
	UInt64			m_nCloseTime;
	UInt64			m_nBytesWritten;
	UInt32			m_nPacketCount;

	int				m_bSaveFlags;
	UInt64			m_nSaveAge;		// 100-nanosecond units
	UInt64			m_nSaveSize;	// bytes

	CCaptureFileVersionBlock	m_Version;
	CCaptureFileSessionBlock	m_Session;
	CCaptureFileIdBlock			m_Id;
	CCaptureFilePacketBlock		m_Packet;
	CIntegrityFileWriter		m_Writer;

	bool	CheckClose() const {
		if ( m_Writer.IsOpen() && m_bSaveFlags ) {
			if ( m_bSaveFlags & kSaveFlag_Age ) {
				UInt64	nNow = GetTimeStamp();
				if ( nNow >= m_nCloseTime ) {
					return true;
				}
			}
			if ( m_bSaveFlags & kSaveFlag_Size ) {
				if ( m_nBytesWritten >= m_nSaveSize ) {
					return true;
				}
			}
		}
		return false;
	}
	void	Close();
	void	NextFilePath();
	bool	Open();

public:
	;		CPktFile();
	virtual	~CPktFile();

	bool	AddPacket( const PeekPacket& inPacket, 	UInt32 inDataSize, const UInt8* inPacketData );
	CPeekString	GetCurrentFileName() const {
		return (m_Writer.IsOpen()) ? m_strOpenFilePath : m_strCloseFilePath;
	}
	CPeekString	GetFileName() const { return m_strCloseFilePath; }
	UInt32	GetPacketCount() const { return m_nPacketCount; }
	void	Monitor();
	bool	StartCapture( LPCTSTR lpszPreAmble, LPCTSTR lpszPostAmble, LPCTSTR lpszOutputDirectory,
		UInt32 nSeconds = 0, UInt64 nBytes = 0 );
	void	StopCapture();
};
