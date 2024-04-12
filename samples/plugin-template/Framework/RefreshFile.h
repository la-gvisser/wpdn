// =============================================================================
//	RefreshFile.h
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "PeekTime.h"
#include "Packet.h"

#define kFileType_OmniPeekPacketFile	L".pkt"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Refresh File
//
//	A Refresh File automates storing data, usually Capture Buffers, based on
//	time (age) and size. The current file is closed after the configured amount
//	of time has elapsed. Or the file is closed when it exceeds the configured
//	size threshold. A new file is created when data is added to the file.
//
//	A Refresh File is joined with a Capture Buffer in a File Capture Buffer and
//	is used when a stream of packets is continuously streamed to disk. All the
//	files will be of a maximum size, but will remain open only for a configured
//	amount of time.
//
//	Use a Capture Buffer to efficiently manage a large number packets in memory.
//	Use an array of Packets to manage a small number of packets.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		Time and size units and conversions
// =============================================================================

const	UInt64	kMinRefreshSize = kBytesInKilobyte;
const	UInt32	kRefreshSizeAdjust = 6000;
const	UInt64	kRefreshTimeNotUsed = kMaxUInt64;
const	UInt32	kMinRefreshTimeMS = 30 * kMillisecondsInASecond;
const	UInt32	kMaxRefreshTimeMS = 30 * kSecondsInADay * kMillisecondsInASecond;
const	UInt32	kMaxRefreshSize = kMaxUInt32  - 1;

enum {
	kTimeUnitMilliseconds = 0,
	kTimeUnitSeconds,
	kTimeUnitMinutes,
	kTimeUnitHours,
	kTimeUnitDays
};

enum {
	kRefreshFileTypeNone = 0,
	kRefreshFileTypeOverwriteExisting,
	kRefreshFileTypeFileNumber,
	kRefreshFileTypeTimeStampWithNumber,
	kRefreshFileTypeTimeStamp,
	kRefreshFileTypeDefault = kRefreshFileTypeTimeStamp
};


// =============================================================================
//	CRefreshFile
// =============================================================================

class CRefreshFile
{
protected:
	CPeekString	m_strFilePath;
	CPeekString	m_strFileBaseName;
	UInt32		m_nRefreshFileType;
	UInt64		m_nTimeIntervalBeginMS;
	UInt64		m_nRefreshTimeIntervalMS;
	UInt32		m_nUniqueNameId;
	bool		m_bActivated;
	UInt64		m_nRefreshSize;
	UInt64		m_nRefreshTriggerSize;
	UInt64		m_nTimeActivatedMS;

protected:
	virtual bool	Refresh( bool bWriteIfEmpty = false ) = 0;
	virtual bool	Monitor( bool inWriteIfEmpty = true ) = 0;
	void			ResetTimeInterval() {
		m_nTimeIntervalBeginMS = 0;
		if ( m_nRefreshTimeIntervalMS != kRefreshTimeNotUsed ) {
			m_nTimeIntervalBeginMS = Now();
		}
	}

public:
	;				CRefreshFile();
	;				CRefreshFile( const CPeekString& inFilePath, const CPeekString& inFileBaseName = CPeekString(),
						UInt32 inRefreshFileType = kRefreshFileTypeNone, bool inAppendTimestring = false );
	virtual			~CRefreshFile() {}

	bool			Activate();

	virtual bool	CheckRefresh( const CPacket& inPacket ) = 0;
	bool			Copy( const CRefreshFile& inRefreshFile );
	CPeekString		CreateNextSaveName();

	void			Deactivate();
	static bool		DirectoryExists( const CPeekString& inDirectory );

	static bool		IsPathValid( const CPeekString& inPath );

	static bool		MakePath( const CPeekString& inPath );

	static UInt64	Now() {
#ifdef _WIN32
		return ::GetTickCount();
#else
		// TODO
		return 0;
#endif
	}

	static UInt64	ToMilliseconds( UInt32 nTime, UInt32 nUnits ) {
		UInt64 nMilliseconds = kMillisecondsInASecond;
		switch ( nUnits ) {
			case kTimeUnitMinutes:
				nMilliseconds *= kSecondsInAMinute;
				break;
			case kTimeUnitHours:
				nMilliseconds *= kSecondsInAnHour;
				break;
			case kTimeUnitDays:
				nMilliseconds *= kSecondsInADay;
				break;
			case kTimeUnitSeconds:
			default:
				break;
		}
		return (nMilliseconds * nTime);
	}

	bool		IsTimeExpired() {
		if ( (m_nRefreshTimeIntervalMS != kRefreshTimeNotUsed) &&
			 m_nTimeActivatedMS &&
			 m_nTimeIntervalBeginMS &&
			 (Now() > (m_nTimeIntervalBeginMS + m_nRefreshTimeIntervalMS)) ) {
			return true;
		}
		return false;
	}

	CPeekString	GetRefreshFileBaseName() const { return m_strFileBaseName; }
	CPeekString	GetRefreshFilePath() const { return m_strFilePath; }
	UInt32		GetRefreshFileType() const { return m_nRefreshFileType; }
	UInt64		GetRefreshSize() const { return m_nRefreshSize; }
	UInt64		GetRefreshTimeInterval() const { return m_nRefreshTimeIntervalMS; }
	UInt64		GetTimeRemainingUntilRefresh() const {
		return (Now() - (m_nTimeIntervalBeginMS + m_nRefreshTimeIntervalMS));
	}
	UInt64		GetTimeSinceActivated() const { return (Now() - m_nTimeActivatedMS); }

	virtual void	Reset();

	void			SetRefreshFileBaseName( const CPeekString& strFileBaseName ) {
		m_strFileBaseName = strFileBaseName;  // Note: a zero length base name is acceptable
	}
	bool			SetRefreshFilePath( const CPeekString& inFilePath, bool inAppendTimestring = false );
	void			SetRefreshFileType( UInt32 inRefreshFileType ) { m_nRefreshFileType = inRefreshFileType; }
	virtual bool	SetRefreshSize( const UInt32 inRefreshSize, const UInt32 inUnits ) = 0;
	bool			SetRefreshTimeInterval( UInt32 inTimeInterval, UInt32 inTimeUnits ) {
		return SetRefreshTimeInterval( ToMilliseconds( inTimeInterval, inTimeUnits ) );
	}
	bool			SetRefreshTimeInterval( UInt64 inRefreshTimeIntervalMS ) {
		if ( (inRefreshTimeIntervalMS < kMinRefreshTimeMS) ||
			 (inRefreshTimeIntervalMS > kMaxRefreshTimeMS) ) return false;
		m_nRefreshTimeIntervalMS = inRefreshTimeIntervalMS;
		return true;
	}
};
