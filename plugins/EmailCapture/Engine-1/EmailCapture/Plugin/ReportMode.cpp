// ============================================================================
//	ReportMode.cpp
//		implementation of the CReportMode class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "ReportMode.h"
#include "PeekTime.h"
#include "PeekDataModeler.h"
#include "GlobalTags.h"


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekString	kPen( L"Pen" );
	const CPeekString	kFull( L"Full" );
	const CPeekString	kMinutes( L"Minutes" );
	const CPeekString	kHours( L"Hours" );
	const CPeekString	kDays( L"Days" );
	const CPeekString	kKilobytes( L"Kilobytes" );
	const CPeekString	kMegabytes( L"Megabytes" );
	const CPeekString	kGigabytes( L"Gigabytes" );

	const CPeekStringA	kLogToA( " to " );
	const CPeekStringA	kLogEveryA( " every " );
	const CPeekStringA	kLogCaptureModeA( " 'Capture Mode' changed from " );
	const CPeekStringA	kLogFullEmailA( "  'Full Email' changed from " );
	const CPeekStringA	kLogFullPacketsA( "  'Full Packets' changed from " );
	const CPeekStringA	kLogPenEmailA( "  'Pen Email' changed from " );
	const CPeekStringA	kLogPenPerAddressA( "  'Pen Per Address' changed from " );
	const CPeekStringA	kLogPenSaveFileA( "  'Pen save' file:" );
	const CPeekStringA	kLogPenTextA( "  'Pen Text' changed from " );
	const CPeekStringA	kLogPenXmlA( "  'Pen XML' changed from " );
}


// ============================================================================
//		Defaults
// ============================================================================

namespace Defaults
{
	const bool			bAcceptPackets( true );
	const int			nMode( kMode_Pen );
	const bool			bPenEmail( true );
	const bool			bPenXml( true );
	const bool			bPenText( true );
	const bool			bPenPerAddress( false );
	const bool			bFullEmail( true );
	const bool			bFullPacket( true );
	const bool			bSaveAge( true );
	const CPeekTime		nSaveAgeValue( kSecondsInADay * WPTIME_SECONDS );
	const CPeekString	strAgeCount( L"1" );
	const CPeekString	strAgeUnits( Tags::kDays );
	const bool			bSaveSize( false );
	const UInt64		nSaveSizeValue( kBytesInMegabyte * 10 );
	const CPeekString	strSizeCount( L"10" );
	const CPeekString	strSizeUnits( Tags::kMegabytes );
}


// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	const CPeekString	kCaptureMode( L"CaptureMode" );
	const CPeekString	kPen( L"Pen" );
	const CPeekString	kFull( L"Full" );
	const CPeekString	kEmail( L"Email" );
	const CPeekString	kXml( L"XML" );
	const CPeekString	kText( L"Text" );
	const CPeekString	kPerAddress( L"PerAddress" );
	const CPeekString	kPacket( L"Packet" );
	const CPeekString	kSaveAge( L"SaveAge" );
	const CPeekString	kSaveSize( L"SaveSize" );
	const CPeekString	kValue( L"Value" );
	const CPeekString	kCount( L"Count" );
	const CPeekString	kUnits( L"Units" );
}


// ============================================================================
//		CReportMode
// ============================================================================

CReportMode::CReportMode()
	:	m_Mode( Defaults::nMode )
	,	m_bPenEmail( Defaults::bPenEmail )
	,	m_bPenXml( Defaults::bPenXml )
	,	m_bPenText( Defaults::bPenText )
	,	m_bPenPerAddress( Defaults::bPenPerAddress )
	,	m_bFullEmail( Defaults::bFullEmail )
	,	m_bFullPacket( Defaults::bFullPacket )
	,	m_bSaveAge( Defaults::bSaveAge )
	,	m_SaveAgeValue( Defaults::nSaveAgeValue )
	,	m_bSaveSize( Defaults::bSaveSize )
	,	m_nSaveSizeValue( Defaults::nSaveSizeValue )
{
}


// ----------------------------------------------------------------------------
//		BuildLogA		[static]
// ----------------------------------------------------------------------------

CPeekStringA
CReportMode::BuildLogA(
	const CPeekStringA& inPreAmble,
	const CPeekStringA& inMessage,
	const CPeekStringA& inValue )
{
#if (TOVERIFY)
	const CPeekStringA*	ayStrs[] = {
		&inPreAmble,
		&inMessage,
		&inValue,
		Tags::kxNewLine.RefA(),
		NULL
	};
	return FastCat( ayStrs );
#endif
}


// ----------------------------------------------------------------------------
//		FormatLongA
// ----------------------------------------------------------------------------

CPeekStringA
CReportMode::Format() const {
#if (TOVERIFY)
	CPeekOutStringA	ss;
	ss <<
		FormatMode() <<
		" - " <<
		((m_bPenEmail) ? "Pen Email, " : "") <<
		((m_bPenXml) ? "Pen Xml, " : "" ) <<
		((m_bPenText) ? "Pen Text, " : "" ) <<
		((m_bFullEmail) ? "Full Email, " : "" ) <<
		((m_bFullPacket) ? "Full Packet, " : "" );
#else
	CString	str;
	str.Format( "%s - %s%s%s%s%s%s%s",
		FormatMode(),
		((m_bPenEmail) ? "Pen Email, " : ""),
		((m_bPenXml) ? "Pen Xml, " : "" ),
		((m_bPenText) ? "Pen Text, " : "" ),
		((m_bFullEmail) ? "Full Email, " : "" ),
		((m_bFullPacket) ? "Full Packet, " : "" ) );
#endif

	return ss;
}


// ----------------------------------------------------------------------------
//		FormatLongA
// ----------------------------------------------------------------------------

CPeekStringA
CReportMode::FormatLongA() const
{
	CArrayStringA	aStrs;

	aStrs.Add( static_cast<PCSTR>( FormatMode() ) );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsPenEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsPenXml()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsPenText()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsPenPerAddress()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsFullEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsFullPacket()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsSaveAge()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	CPeekOutStringA	ssAge;
	ssAge << GetSaveAgeValue();
	aStrs.Add( ssAge );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (IsSaveSize()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	CPeekOutStringA	ssSize;
	ssSize << GetSaveSizeValue();
	aStrs.Add( ssSize );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	CPeekStringA	str = aStrs.ToString();
	return str;
}


// ----------------------------------------------------------------------------
//		GetSaveAgeValue
// ----------------------------------------------------------------------------

CPeekStringX
CReportMode::FormatMode() const {
	return ((m_Mode == kMode_Pen) ? Tags::kxPen : Tags::kxFull);
}


// ----------------------------------------------------------------------------
//		GetSaveAgeValue
// ----------------------------------------------------------------------------

void
CReportMode::GetSaveAgeValue(
	UInt32&		outCount,
	kAgeUnits&	outUnits ) const
{
	UInt32	nSeconds = static_cast<UInt32>( m_SaveAgeValue.Get( kSeconds ) );
	if ( (nSeconds % kSecondsInADay) == 0 ) {
		outCount = nSeconds / kSecondsInADay;
		outUnits = CReportMode::kAge_Days;
	}
	else if ( (nSeconds % kSecondsInAnHour) == 0 ) {
		outCount = nSeconds / kSecondsInAnHour;
		outUnits = CReportMode::kAge_Hours;
	}
	else {
		outCount = nSeconds / kSecondsInAMinute;
		outUnits = CReportMode::kAge_Minutes;
	}
}

void
CReportMode::GetSaveAgeValue(
	CPeekString&	outCount,
	CPeekString&	outUnits ) const
{
	UInt32	nSeconds = static_cast<UInt32>( m_SaveAgeValue.Get( kSeconds ) );
	int		nCount;

	if ( (nSeconds % kSecondsInADay) == 0 ) {
		nCount = nSeconds / kSecondsInADay;
		outUnits = Tags::kDays;
	}
	else if ( (nSeconds % kSecondsInAnHour) == 0 ) {
		nCount = nSeconds / kSecondsInAnHour;
		outUnits = Tags::kHours;
	}
	else {
		nCount = nSeconds / kSecondsInAMinute;
		outUnits = Tags::kMinutes;
	}
	CPeekOutString	ssCount;
	ssCount << nCount;
	outCount = ssCount;;
}


// ----------------------------------------------------------------------------
//		GetSaveSizeValue
// ----------------------------------------------------------------------------

void
CReportMode::GetSaveSizeValue(
	UInt32&		outCount,
	kSizeUnits&	outUnits ) const
{
	if ( (m_nSaveSizeValue % kBytesInGigabyte) == 0 ) {
		outCount = static_cast<UInt32>( m_nSaveSizeValue / kBytesInGigabyte );
		outUnits = CReportMode::kSize_Gigabytes;
	}
	else if ( (m_nSaveSizeValue % kBytesInMegabyte) == 0 ) {
		outCount = static_cast<UInt32>( m_nSaveSizeValue / kBytesInMegabyte );
		outUnits = CReportMode::kSize_Megabytes;
	}
	else {
		outCount = static_cast<UInt32>( m_nSaveSizeValue / kBytesInKilobyte );
		outUnits = CReportMode::kSize_Kilobytes;
	}
}

void
CReportMode::GetSaveSizeValue(
	CPeekString&	outCount,
	CPeekString&	outUnits ) const
{
	UInt32	nCount;

	if ( (m_nSaveSizeValue % kBytesInGigabyte) == 0 ) {
		nCount = static_cast<UInt32>( m_nSaveSizeValue / kBytesInGigabyte );
		outUnits = Tags::kGigabytes;
	}
	else if ( (m_nSaveSizeValue % kBytesInMegabyte) == 0 ) {
		nCount = static_cast<UInt32>( m_nSaveSizeValue / kBytesInMegabyte );
		outUnits = Tags::kMegabytes;
	}
	else {
		nCount = static_cast<UInt32>( m_nSaveSizeValue / kBytesInKilobyte );
		outUnits = Tags::kKilobytes;
	}
	CPeekOutString	ssCount;
	ssCount << nCount;
	outCount = ssCount;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CReportMode::IsMatch(
	const CReportMode&	inReportMode ) const
{
	if ( inReportMode.GetMode() != GetMode() ) return false;
	if ( inReportMode.IsPenEmail() != IsPenEmail() ) return false;
	if ( inReportMode.IsPenXml() != IsPenXml() ) return false;
	if ( inReportMode.IsPenText() != IsPenText() ) return false;
	if ( inReportMode.IsPenPerAddress() != IsPenPerAddress() ) return false;
	if ( inReportMode.GetSaveAge() != GetSaveAge() ) return false;
	if ( inReportMode.GetSaveAgeValue() != GetSaveAgeValue() ) return false;
	if ( inReportMode.GetSaveSize() != GetSaveSize() ) return false;
	if ( inReportMode.GetSaveSizeValue() != GetSaveSizeValue() ) return false;
	if ( inReportMode.IsFullEmail() != IsFullEmail() ) return false;
	if ( inReportMode.IsFullPacket() != IsFullPacket() ) return false;

	return true;
}


// ----------------------------------------------------------------------------
//		Log
// ----------------------------------------------------------------------------

bool
CReportMode::Log(
	const CPeekStringA&	inPreAmble,
	CFileEx&			inFile ) const
{
#if (TOVERIFY)
#endif

	try {
		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Capture Mode' set to ",
				((GetMode() == kMode_Pen) ? Tags::kxPen.GetA() : Tags::kxFull.GetA()) );
			inFile.Write( strMsg );
		}

		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Pen Email' set to ",
				((IsPenEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( strMsg );
		}

		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Pen XML' set to ",
				((IsPenXml()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( strMsg );
		}

		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Pen Text' set to ",
				((IsPenText()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( strMsg );
		}

		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Pen Per Address' set to ",
				((IsPenPerAddress()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( strMsg );
		}

		{
			CPeekOutStringA	ssMsg;
			ssMsg <<
				inPreAmble <<
				" 'Pen save' file:\r\n";
			inFile.Write( ssMsg );
		}

		{
			CPeekString	strCount;
			CPeekString	strUnits;
			GetSaveAgeValue( strCount, strUnits );

			CPeekOutStringA	ssMsg;
			ssMsg <<
				inPreAmble <<
				" - every " <<
				CPeekStringA( strCount ) <<
				" " <<
				CPeekStringA( strUnits ) <<
				" (" <<
				((IsSaveAge()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) <<
				")" <<
				Tags::kxNewLine.GetA();
			inFile.Write( ssMsg );
		}

		{
			CPeekString	strCount;
			CPeekString	strUnits;
			GetSaveSizeValue( strCount, strUnits );

			CPeekOutStringA	ssMsg;
			ssMsg <<
				inPreAmble <<
				" - every " <<
				CPeekStringA( strCount ) <<
				" " <<
				CPeekStringA( strUnits ) <<
				"(" <<
				((IsSaveSize()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) <<
				")" <<
				Tags::kxNewLine.GetA();
			inFile.Write( ssMsg );
		}

		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Full Email' set to ",
				((IsFullEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( strMsg );
		}

		{
			CPeekStringA	strMsg = BuildLogA(
				inPreAmble,
				" 'Full Packets' set to ",
				((IsFullPacket()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( strMsg );
		}
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		LogUpdate
// ----------------------------------------------------------------------------

bool
CReportMode::LogUpdate(
	const CPeekStringA&	inPreAmble,
	const CReportMode&	inReportMode,
	CFileEx&			inFile ) const
{
	try {
		CPeekStringA	strMsg;

		if ( inReportMode.GetMode() != GetMode() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogCaptureModeA,
				(GetMode() == kMode_Pen) ? Tags::kxPen.RefA() : Tags::kxFull.RefA(),
				&Tags::kLogToA,
				(inReportMode.GetMode() == kMode_Pen) ? Tags::kxPen.RefA() : Tags::kxFull.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Capture Mode' changed from %s to %s\r\n",
				inPreAmble,
				(GetMode() == kMode_Pen) ? Tags::kxPen.GetA() : Tags::kxFull.GetA(),
				(inReportMode.GetMode() == kMode_Pen) ? Tags::kxPen.GetA() : Tags::kxFull.GetA() );
			inFile.Write( strMsg );
#endif
		}
		if ( inReportMode.IsPenEmail() != IsPenEmail() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogPenEmailA,
				(IsPenEmail()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				&Tags::kLogToA,
				(inReportMode.IsPenEmail()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Pen Email' changed from %s to %s\r\n",
				inPreAmble,
				(IsPenEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inReportMode.IsPenEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( strMsg );
#endif
		}
		if ( inReportMode.IsPenXml() != IsPenXml() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogPenXmlA,
				(IsPenXml()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				&Tags::kLogToA,
				(inReportMode.IsPenXml()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Pen XML' changed from %s to %s\r\n",
				inPreAmble,
				(IsPenXml()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inReportMode.IsPenXml()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( strMsg );
#endif
		}
		if ( inReportMode.IsPenText() != IsPenText() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogPenTextA,
				(IsPenText()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				&Tags::kLogToA,
				(inReportMode.IsPenText()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Pen Text' changed from %s to %s\r\n",
				inPreAmble,
				(IsPenText()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inReportMode.IsPenText()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( strMsg );
#endif
		}
		if ( inReportMode.IsPenPerAddress() != IsPenPerAddress() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogPenPerAddressA,
				(IsPenPerAddress()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				&Tags::kLogToA,
				(inReportMode.IsPenPerAddress()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Pen Per Address' changed from %s to %s\r\n",
				inPreAmble,
				(IsPenPerAddress()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inReportMode.IsPenPerAddress()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( strMsg );
#endif
		}

		if ( (inReportMode.GetSaveAge() != GetSaveAge()) ||
			 (inReportMode.GetSaveSize() != GetSaveSize()) ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogPenSaveFileA,
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(	"%s 'Pen save' file:\r\n", inPreAmble );
			inFile.Write( strMsg );
#endif

			if ( inReportMode.GetSaveAge() != GetSaveAge() ){
				CPeekString	strCount;
				CPeekString	strUnits;
				inReportMode.GetSaveAgeValue( strCount, strUnits );

				CPeekStringA	strCountA( strCount );
				CPeekStringA	strUnitsA( strUnits );

#if (TOVERIFY)
				const CPeekStringA*	ayStrs[] = {
					&inPreAmble,
					&Tags::kLogEveryA,
					&strCountA,
					Tags::kxSpace.RefA(),
					&strUnitsA,
					Tags::kxSpace.RefA(),
					Tags::kxParenLeft.RefA(),
					(inReportMode.IsSaveAge()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
					Tags::kxParenRight.RefA(),
					Tags::kxNewLine.RefA(),
					NULL
				};
				inFile.Write( FastCat( ayStrs ) );
#else
				strMsg.Format(
					"%s - every %s %s (%s)\r\n",
					inPreAmble,
					CPeekStringA( strCount ),
					CPeekStringA( strUnits ),
					((inReportMode.IsSaveAge()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
				inFile.Write( strMsg );
#endif
			}

			if ( inReportMode.GetSaveSize() != GetSaveSize() ) {
				CPeekString	strCount;
				CPeekString	strUnits;
				inReportMode.GetSaveSizeValue( strCount, strUnits );

				CPeekStringA	strCountA( strCount );
				CPeekStringA	strUnitsA( strUnits );

#if (TOVERIFY)
				const CPeekStringA*	ayStrs[] = {
					&inPreAmble,
					&Tags::kLogEveryA,
					&strCountA,
					Tags::kxSpace.RefA(),
					&strUnitsA,
					Tags::kxSpace.RefA(),
					Tags::kxParenLeft.RefA(),
					(inReportMode.IsSaveSize()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
					Tags::kxParenRight.RefA(),
					Tags::kxNewLine.RefA(),
					NULL
				};
				inFile.Write( FastCat( ayStrs ) );
#else
				CPeekString	strCount;
				CPeekString	strUnits;
				inReportMode.GetSaveSizeValue( strCount, strUnits );
				strMsg.Format(
					"%s - every %s %s (%s)\r\n",
					inPreAmble,
					CPeekStringA( strCount ),
					CPeekStringA( strUnits ),
					((inReportMode.IsSaveSize()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
				inFile.Write( strMsg );
#endif
			}
		}

		if ( inReportMode.IsFullEmail() != IsFullEmail() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogFullEmailA,
				(IsFullEmail()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				&Tags::kLogToA,
				(inReportMode.IsFullEmail()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Full Email' changed from %s to %s\r\n",
				inPreAmble,
				(IsFullEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inReportMode.IsFullEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( strMsg );
#endif
		}
		if ( inReportMode.IsFullPacket() != IsFullPacket() ) {
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kLogFullPacketsA,
				(IsFullPacket()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				&Tags::kLogToA,
				(inReportMode.IsFullPacket()) ? Tags::kxEnabled.RefA() : Tags::kxDisabled.RefA(),
				Tags::kxNewLine.RefA(),
				NULL
			};
			inFile.Write( FastCat( ayStrs ) );
#else
			strMsg.Format(
				"%s 'Full Packets' changed from %s to %s\r\n",
				inPreAmble,
				(IsFullPacket()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inReportMode.IsFullPacket()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( strMsg );
#endif
		}
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CReportMode::Model(
	CPeekDataElement&	ioCase )
{
	static CArrayString	saLabels;
	if ( saLabels.IsEmpty() ) {
		saLabels.Add( ConfigTags::kPen );
		saLabels.Add( ConfigTags::kFull );
	}
	size_t	nIndex = m_Mode;
	ioCase.Attribute( ConfigTags::kCaptureMode, nIndex, saLabels );
	if ( ioCase.IsLoading() && (nIndex < saLabels.GetCount()) ) {
		m_Mode = static_cast<int>( nIndex );
	}

	CPeekDataElement	elemPen( ConfigTags::kPen, ioCase );
	;	CPeekDataElement	elemPenEmail( ConfigTags::kEmail, elemPen );
	;		elemPenEmail.Enabled( m_bPenEmail );
	;		elemPenEmail.End();
	;	CPeekDataElement	elemText( ConfigTags::kText, elemPen );
	;		elemText.Enabled( m_bPenText );
	;		elemText.End();
	;	CPeekDataElement	elemPerAddress( ConfigTags::kPerAddress, elemPen );
	;		elemPerAddress.Enabled( m_bPenPerAddress );
	;		elemPerAddress.End();
	;	CPeekDataElement	elemSaveAge( ConfigTags::kSaveAge, elemPen );
	;		elemSaveAge.Enabled( m_bSaveAge );
	;		elemSaveAge.Attribute( ConfigTags::kValue, m_nSaveSizeValue );
	;		elemSaveAge.End();
	;	CPeekDataElement	elemSaveSize( ConfigTags::kSaveSize, elemPen );
	;		elemSaveSize.Enabled( m_bSaveSize );
	;		elemSaveSize.Attribute( ConfigTags::kValue, m_nSaveSizeValue );
	;		elemSaveSize.End();
	;	elemPen.End();

	CPeekDataElement	elemFull( ConfigTags::kFull, ioCase );
	;	CPeekDataElement	elemFullEmail( ConfigTags::kEmail, elemFull );
	;		elemFullEmail.Enabled( m_bFullEmail );
	;		elemFullEmail.End();
	;	CPeekDataElement	elemPacket( ConfigTags::kPacket, elemFull );
	;		elemPacket.Enabled( m_bFullPacket );
	;		elemPacket.End();
	;	elemFull.End();

	return true;
}


// ----------------------------------------------------------------------------
//		SetSaveAgeValue
// ----------------------------------------------------------------------------

void
CReportMode::SetSaveAgeValue(
	UInt32		inCount,
	kAgeUnits	inUnits )
{
	UInt32	nSeconds = 0;
	switch ( inUnits ) {
	case CReportMode::kAge_Days:
		nSeconds = inCount * kSecondsInADay;
		break;
	case CReportMode::kAge_Hours:
		nSeconds = inCount * kSecondsInAnHour;
		break;
	default:	// kSave_Minutes
		nSeconds = inCount * kSecondsInAMinute;
		break;
	}
	m_SaveAgeValue.Set( kSeconds, nSeconds );
}

void
CReportMode::SetSaveAgeValue(
	const CPeekString&	inCount,
	const CPeekString&	inUnits )
{
	UInt32	nCount( _wtol( inCount ) );
	UInt32	nSeconds( 0 );

	if ( TagCompare( inUnits, Tags::kDays ) ) {
		nSeconds = nCount * kSecondsInADay;
	}
	else if ( TagCompare( inUnits, Tags::kHours ) ) {
		nSeconds = nCount * kSecondsInAnHour;
	}
	else{
		nSeconds = nCount * kSecondsInAMinute;
	}
	m_SaveAgeValue.Set( kSeconds, nSeconds );
}


// ----------------------------------------------------------------------------
//		SetSaveSizeValue
// ----------------------------------------------------------------------------

void
CReportMode::SetSaveSizeValue(
	UInt32		inCount,
	kSizeUnits	inUnits )
{
	switch ( inUnits ) {
	case CReportMode::kSize_Gigabytes:
		m_nSaveSizeValue = inCount * kBytesInGigabyte;
		break;
	case CReportMode::kSize_Megabytes:
		m_nSaveSizeValue = inCount * kBytesInMegabyte;
		break;
	default:	// kMax_Kilobytes
		m_nSaveSizeValue = inCount * kBytesInKilobyte;
		break;
	}
}

void
CReportMode::SetSaveSizeValue(
	const CPeekString&	inCount,
	const CPeekString&	inUnits )
{
	UInt32	nCount( _wtol( inCount ) );

	if ( TagCompare( inUnits, Tags::kGigabytes ) ) {
		m_nSaveSizeValue = nCount * kBytesInGigabyte;
	}
	else if ( TagCompare( inUnits, Tags::kMegabytes ) ) {
		m_nSaveSizeValue = nCount * kBytesInMegabyte;
	}
	else{
		m_nSaveSizeValue = nCount * kBytesInKilobyte;
	}
}
