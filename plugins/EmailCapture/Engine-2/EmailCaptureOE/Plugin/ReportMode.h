// ============================================================================
//	ReportMode.h
//		interface for the CReportMode class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekStrings.h"
#include "FileEx.h"

class CModePage;
class CPeekDataModeler;
class CPeekDataElement;


// ============================================================================
//		CReportMode
// ============================================================================

class CReportMode
{
	friend class CModePage;

protected:
	int				m_Mode;
	BOOL			m_bPenEmail;
	BOOL			m_bPenXml;
	BOOL			m_bPenText;
	BOOL			m_bPenPerAddress;
	BOOL			m_bFullEmail;
	BOOL			m_bFullPacket;
	BOOL			m_bSaveAge;
	CPeekDuration	m_SaveAgeValue;
	BOOL			m_bSaveSize;
	UInt64			m_nSaveSizeValue;

public:
	static CPeekStringA BuildLogA( const CPeekStringA& inPreAmble,
			const CPeekStringA& inMessage, const CPeekStringA& inValue );

public:
	typedef enum {
		kAge_Minutes,
		kAge_Hours,
		kAge_Days
	} kAgeUnits;

	typedef enum {
		kSize_Kilobytes,
		kSize_Megabytes,
		kSize_Gigabytes
	} kSizeUnits;

	;		CReportMode();

	CPeekStringA	Format() const;
	CPeekStringA	FormatLongA() const;
	CPeekStringX	FormatMode() const;

	int		GetMode() const { return m_Mode; }
	UInt32	GetSaveAge() const { return (IsSaveAge()) ? GetSaveAgeValue() : WPTIME_INVALID; }
	UInt32	GetSaveAgeValue() const { return static_cast<UInt32>( m_SaveAgeValue ); }
	void	GetSaveAgeValue( UInt32& outCount, kAgeUnits& outUnits ) const;
	void	GetSaveAgeValue( CPeekString& outCount, CPeekString& outUnits ) const;
	UInt64	GetSaveSize() const { return (IsSaveSize()) ? GetSaveSizeValue() : static_cast<UInt64>( -1 ); }
	UInt64	GetSaveSizeValue() const { return m_nSaveSizeValue; }
	void	GetSaveSizeValue( UInt32& outCount, kSizeUnits& outUnits ) const;
	void	GetSaveSizeValue( CPeekString& outCount, CPeekString& outUnits ) const;

	bool	IsPenEmail() const { return (m_bPenEmail != FALSE); }
	bool	IsPenXml() const { return (m_bPenXml != FALSE); }
	bool	IsPenText() const { return (m_bPenText != FALSE); }
	bool	IsPenPerAddress() const { return (m_bPenPerAddress != FALSE); }
	bool	IsFullEmail() const { return (m_bFullEmail != FALSE); }
	bool	IsFullPacket() const { return (m_bFullPacket != FALSE); }
	bool	IsMatch( const CReportMode& inReportMode ) const;
	bool	IsSaveAge() const { return (m_bSaveAge != FALSE); }
	bool	IsSaveSize() const { return (m_bSaveSize != FALSE); }

	bool	Log( const CPeekStringA& inPreAmble, CFileEx& inFile ) const;
	bool	LogUpdate( const CPeekStringA& inPreAmble, const CReportMode& inReportMode, CFileEx& inFile ) const;

	bool	Model( CPeekDataElement& ioCase );

	void	SetMode( int inMode ) { m_Mode = inMode; }
	void	SetPenEmail( bool inEnable ) { m_bPenEmail = inEnable; }
	void	SetPenXml( bool inEnable ) { m_bPenXml = inEnable; }
	void	SetPenText( bool inEnable ) { m_bPenText = inEnable; }
	void	SetPenPerAddress( bool inEnable ) { m_bPenPerAddress = inEnable; }
	void	SetFullEmail( bool inEnable ) { m_bFullEmail = inEnable; }
	void	SetFullPacket( bool inEnable ) { m_bFullPacket = inEnable; }
	void	SetSaveAge( bool inEnable ) { m_bSaveAge = inEnable; }
	void	SetSaveAgeValue( UInt32 inValue ) { m_SaveAgeValue = inValue; }
	void	SetSaveAgeValue( UInt32 inCount, kAgeUnits inUnits );
	void	SetSaveAgeValue( const CPeekString& inCoutn, const CPeekString& inUnits );
	void	SetSaveSize( bool inEnable ) { m_bSaveSize = inEnable; }
	void	SetSaveSizeValue( UInt64 inValue ) { m_nSaveSizeValue = inValue; }
	void	SetSaveSizeValue( UInt32 inCount, kSizeUnits inUnits );
	void	SetSaveSizeValue( const CPeekString& inCount, const CPeekString& inUnits );
};
