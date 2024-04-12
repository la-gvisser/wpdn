// ============================================================================
//	Case.cpp
//		implementation of the CCase class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "Case.h"
#include "GlobalTags.h"
#include "MsgStream.h"
#include "Options.h"
#include "PeekDataModeler.h"

extern const CCase::tCaseId	gFirstCaseId( 1024 * 1024 );
CCase::tCaseId				gNextCaseId( gFirstCaseId + 1 );
size_t						CCaseList::s_nMaxAddresses( 10000 );


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekString	strCaseName( L"Case" );

	const CPeekString	kCapture( L"Cap" );
	const CPeekString	kDisk( L"Disk" );
	const CPeekString	kLongNameOn( L"Include Case in file names." );
	const CPeekString	kLongNameOff( L"Do not include Case in file names." );

	// formating
	const CPeekStringA	kAcceptPackets( "Accept Packets: " );
	const CPeekStringA	kWritePackets( "Write Packets: " );
	const CPeekStringA	kSummaryStats( "Display Summary: " );
	const CPeekStringA	kLogMessages( "Log Messages: " );
	const CPeekStringA	kOutputDirectory( "Output Directory: " );
	const CPeekStringA	kLongName( "Case in file name: " );
	const CPeekStringA	kProtocols( "Protocols: " );
	const CPeekStringA	kStartTime( "Start Time: " );
	const CPeekStringA	kStopTime( "Stop Time: " );
	const CPeekStringA	kTargetIs( "target is " );
	const CPeekStringA	kAddressList( "Address List: " );
	const CPeekStringA	kCaseDeleteA( " - The case " );
	const CPeekStringA	kWasDeletedA( " was deleted." );
	const CPeekStringA	kHasBeenAdded( "' has been added." );
	const CPeekStringA	kCheckedSet( " Checked (enabled) set to " );
}


// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	// CCaseList
	const CPeekString	kCaseList( L"CaseList" );

	// CCase
	const CPeekString	kCase( L"Case" );
	const CPeekString	kCaptureMode( L"CaptureMode" );
	const CPeekString	kDisplayIntegrity( L"DisplayIngegrity" );
	const CPeekString	kCaptureAll( L"CaptureAll" );
	const CPeekString	kFacility( L"Facility" );
	const CPeekString	kName( L"Name" );
	const CPeekString	kAddressList( L"AddressList" );
	const CPeekString	kAddressInfo( L"AddressInfo" );
	const CPeekString	kAddress( L"Address" );
	const CPeekString	kAcceptPackets( L"DisplayPackets" );
	const CPeekString	kWritePackets( L"WritePackets" );
	const CPeekString	kSummaryStats( L"DisplayStats" );
	const CPeekString	kLogMessages( L"LogMessages" );
	const CPeekString	kFileNamePrefix( L"FileNamePrefix" );
	const CPeekString	kAppendEmail( L"AppendEmail" );
	const CPeekString	kOutputDirectory( L"OutputDirectory" );
	const CPeekString	kLongName( L"CaseInFileName" );
	const CPeekString	kProtocols( L"Protocols" );
	const CPeekString	kPen( L"Pen" );
	const CPeekString	kFull( L"Full" );
	const CPeekString	kStartTime( L"StartTime" );
	const CPeekString	kStopTime( L"StopTime" );

#if SAVE_ATTACHMENTS
	const CPeekString	kSaveAttachments( L"SaveAttachments" );
	const CPeekString	kAttachmentDirectory( L"AttachmentDirectory" );
#endif
}


// ============================================================================
//		Default Values
// ============================================================================

namespace Defaults
{
	const bool			bEnabled( TRUE );
	const bool			bDisplayIntegrity( FALSE );
	const CPeekString	strName( L"Case" );
	const bool			bAcceptPackets( true );
	const bool			bWritePackets( false );
	const tFileNameType	nPacketFileFormat( kFileTypePcap );
	const bool			bSummaryStats( true );
	const bool			bLogMessages( true );
	const CPeekStringA	strFileNamePrefix( "Prefix_" );
	const bool			bAppendEmail( false );
	const CPeekStringA	strDirectory( "C:\\EmailCapture" );
	const bool			bSMTP( true );
	const bool			bPOP3( true );

	extern const bool			bCaptureAll;
	extern const CPeekString	strFacility;
#if SAVE_ATTACHMENTS
	const BOOL			bSaveAttachments( FALSE );
	const CPeekStringA	strAttachmentDirectory( "C:\\EmailCapture\\Attachments" );
#endif
}


// ============================================================================
//		CCaseSettings
// ============================================================================

CCaseSettings::CCaseSettings()
	:	m_bAcceptPackets( Defaults::bAcceptPackets )
	,	m_bWritePackets( Defaults::bWritePackets )
	,	m_nPacketFileFormat( Defaults::nPacketFileFormat )
	,	m_bSummaryStats( Defaults::bSummaryStats )
	,	m_bLogMessages( Defaults::bLogMessages )
	,	m_strFileNamePrefix( Defaults::strFileNamePrefix )
	,	m_bAppendEmail( Defaults::bAppendEmail )
	,	m_strOutputDirectory( Defaults::strDirectory )
	,	m_StartTime( true )
	,	m_StopTime( true )
#if SAVE_ATTACHMENTS
	,	m_bSaveAttachments( Defaults::bSaveAttachments )
	,	m_strAttachmentDirectory( Defaults::strAttachmentDirectory )
#endif
{
	m_Protocols[kProtocol_SMTP] = Defaults::bSMTP;
	m_Protocols[kProtocol_POP3] = Defaults::bPOP3;
}


// ----------------------------------------------------------------------------
//		FormatA
// ----------------------------------------------------------------------------

CPeekStringA
CCaseSettings::FormatA() const
{
#if (TOVERIFY)
	CPeekStringA	strStart( FormatStartTimeA() );
	CPeekStringA	strStop( FormatStopTimeA() );

	const CPeekStringA*	ayStrs[] = {
		&Tags::kAcceptPackets,
		FormatEnabledRefA( m_bAcceptPackets ),
		Tags::kxCommaSpace.RefA(),

		&Tags::kWritePackets,
		FormatEnabledRefA( m_bWritePackets ),
		Tags::kxCommaSpace.RefA(),

		&Tags::kSummaryStats,
		FormatEnabledRefA( m_bSummaryStats ),
		Tags::kxCommaSpace.RefA(),

		&Tags::kLogMessages,
		FormatEnabledRefA( m_bLogMessages ),
		Tags::kxCommaSpace.RefA(),

		&Tags::kOutputDirectory,
		m_strOutputDirectory.RefA(),
		Tags::kxCommaSpace.RefA(),

		&Tags::kOutputDirectory,
		&Tags::kProtocols,
		((m_Protocols[kProtocol_SMTP]) ? Tags::kxSmtp.RefA() : Tags::kxEmpty.RefA() ),
		((m_Protocols[kProtocol_POP3]) ? Tags::kxPop3.RefA() : Tags::kxEmpty.RefA() ),
		Tags::kxCommaSpace.RefA(),

		&Tags::kStartTime,
		&strStart,
		Tags::kxCommaSpace.RefA(),
	
		&Tags::kStopTime,
		&strStop,
		Tags::kxCommaSpace.RefA(),
	};

	CPeekStringA	str = FastCat( ayStrs );
#else
	CArrayStringA	aStrs;

	aStrs.Add( Tags::kAcceptPackets );
	aStrs.Add( (m_bAcceptPackets) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( kxCommaSpace.GetA() );

	aStrs.Add( Tags::kWritePackets );
	aStrs.Add( (m_bWritePackets) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( kxCommaSpace.GetA() );

	aStrs.Add( Tags::kSummaryStats );
	aStrs.Add( (m_bSummaryStats) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( kxCommaSpace.GetA() );

	aStrs.Add( Tags::kLogMessages );
	aStrs.Add( (m_bLogMessages) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( kxCommaSpace.GetA() );

	aStrs.Add( Tags::kOutputDirectory );
	aStrs.Add( static_cast<PCSTR>( m_strOutputDirectory ) );
	aStrs.Add( kxCommaSpace.GetA() );

	aStrs.Add( Tags::kLogMessages );
	aStrs.Add( (m_bLongName) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( kxCommaSpace.GetA() );

	aStrs.Add( Tags::kOutputDirectory );
	aStrs.Add( Tags::kProtocols );
	if ( m_Protocols[kProtocol_SMTP] ) aStrs.Add( Tags::kSMTP );
	if ( m_Protocols[kProtocol_POP3] ) aStrs.Add( Tags::kPOP3 );
	aStrs.Add( kxCommaSpace.GetA() );

	CPeekStringA	strStart( FormatStartTimeA() );
	aStrs.Add( Tags::kStartTime );
	aStrs.Add( strStart );
	aStrs.Add( kxCommaSpace.GetA() );
	
	CPeekStringA	strStop( FormatStopTimeA() );
	aStrs.Add( Tags::kStopTime );
	aStrs.Add( strStop );
	aStrs.Add( kxCommaSpace.GetA() );

	CPeekStringA	str = aStrs.ToString();
#endif
	return str;
}


// ----------------------------------------------------------------------------
//		FormatLongA
// ----------------------------------------------------------------------------

CPeekStringA
CCaseSettings::FormatLongA() const
{
	CArrayStringA	aStrs;

	aStrs.Add( (m_bAcceptPackets) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (m_bWritePackets) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (m_bSummaryStats) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (m_bLogMessages) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( Tags::kxCommaSpace.GetA() );

	if ( m_Protocols[kProtocol_SMTP] ) aStrs.Add( Tags::kxSmtp.GetA() );
	if ( m_Protocols[kProtocol_POP3] ) aStrs.Add( Tags::kxPop3.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( m_Mode.FormatLongA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( FormatStartTimeA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );
	
	aStrs.Add( FormatStopTimeA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	CPeekStringA	str = aStrs.ToString();
	return str;
}


// ----------------------------------------------------------------------------
//		FormatMode
// ----------------------------------------------------------------------------

CPeekString
CCaseSettings::FormatMode() const
{
	return (IsMode( kMode_Pen ))
		? Tags::kxPen.GetW()
		: Tags::kxFull.GetW();
}


// ----------------------------------------------------------------------------
//		FormatPackets
// ----------------------------------------------------------------------------

CPeekString
CCaseSettings::FormatPackets() const
{
	CPeekString	strPackets;

	if ( IsAcceptPackets() ) {
		strPackets.Append( Tags::kCapture );
	}
	if ( IsWritePackets() ) {
		if ( !strPackets.IsEmpty() ) {
			strPackets.Append( Tags::kxSpacedAnd.GetW() );
		}
		strPackets.Append( Tags::kDisk );	// Wingdings 60
	}
	return strPackets;
}


// ----------------------------------------------------------------------------
//		FormatProtocol
// ----------------------------------------------------------------------------

CPeekString
CCaseSettings::FormatProtocol() const
{
	CPeekString	strProtocol;
	if ( m_Protocols[kProtocol_SMTP] ) {
		strProtocol = CProtocolType::Format( kProtocol_SMTP );
	}
	if ( m_Protocols[kProtocol_POP3] ) {
		if ( !strProtocol.IsEmpty() ) {
			strProtocol.Append( Tags::kxCommaSpace.GetW() );
		}
		strProtocol.Append( CProtocolType::Format( kProtocol_POP3 ) );
	}

	return strProtocol;
}


// ----------------------------------------------------------------------------
//		FormatTriggers
// ----------------------------------------------------------------------------

CPeekString
CCaseSettings::FormatTriggers() const
{
#if (TOVERIFY)
	CPeekOutString	ssTriggers;
	ssTriggers <<
		FormatStartTime() <<
		Tags::kxNewLine.GetA() <<
		FormatStopTime();
#else
	CString	strTriggers;
	strTriggers.Format( L"%s\r\n%s", FormatStartTime(), FormatStopTime() );
#endif

	return ssTriggers;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CCaseSettings::IsMatch(
	const CCaseSettings&	inSettings ) const
{
	if ( inSettings.IsAcceptPackets() != IsAcceptPackets() ) return false;
	if ( inSettings.IsWritePackets() != IsWritePackets() ) return false;
	if ( inSettings.IsSummaryStats() != IsSummaryStats() ) return false;
	if ( inSettings.IsLogMessages() != IsLogMessages() ) return false;
	if ( inSettings.IsProtocolOfInterest( kProtocol_SMTP ) ^ IsProtocolOfInterest( kProtocol_SMTP ) ) return false;
	if ( inSettings.IsProtocolOfInterest( kProtocol_POP3 ) != IsProtocolOfInterest( kProtocol_POP3 ) ) return false;
	if ( inSettings.GetFileNamePrefix() != GetFileNamePrefix() ) return false;
	if ( inSettings.IsAppendEmail() != IsAppendEmail() ) return false;
	if ( inSettings.GetOutputDirectory() != GetOutputDirectory() ) return false;
	if ( inSettings.GetStartTime() != GetStartTime() ) return false;
	if ( inSettings.GetStopTime() != GetStopTime() ) return false;

	return GetReportMode().IsMatch( inSettings.GetReportMode() );
}


// ----------------------------------------------------------------------------
//		Log
// ----------------------------------------------------------------------------

bool
CCaseSettings::Log(
	const CPeekStringA&	inPreAmble,
	CFileEx&			inFile ) const
{

	try {
#if (TOVERIFY)
		CPeekStringA	strMsg;

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Accept/Capture Packets' set to ",
			((IsAcceptPackets()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Save Packets to Disk' set to ",
			((IsWritePackets()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Display Summary Statistics' set to ",
			((IsSummaryStats()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Post messages to the Log' set to ",
			((IsLogMessages()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Monitor SMTP' set to ",
			((IsProtocolOfInterest( kProtocol_SMTP )) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Monitor POP3' set to ",
			((IsProtocolOfInterest( kProtocol_POP3 )) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Filename Prefix' set to ",
			GetFileNamePrefix().GetA() );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Append email address' set to ",
			((IsAppendEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Output Directory' set to ",
			GetOutputDirectory().GetA() );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Start Time' set to ",
			FormatStartTimeA() );
		inFile.Write( strMsg );

		strMsg = CReportMode::BuildLogA(
			inPreAmble,
			" 'Stop Time' set to ",
			FormatStopTimeA() );
		inFile.Write( strMsg );
#endif
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
CCaseSettings::LogUpdate(
	const CPeekStringA&		inPreAmble,
	const CCaseSettings&	inSettings,
	CFileEx&				inFile ) const
{
	CBoolResultTrue	bResult;

	try {
#if (TOVERIFY)
		CPeekStringA	str;

		// Consider reserving Max length.

		if ( inSettings.IsAcceptPackets() != IsAcceptPackets() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Accept/Capture Packets' changed from ",
				((IsAcceptPackets()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsAcceptPackets()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}
		if ( inSettings.IsWritePackets() != IsWritePackets() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Save Packets to Disk' changed from ",
				((IsWritePackets()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsWritePackets()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}
		if ( inSettings.IsSummaryStats() != IsSummaryStats() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Display Summary Statistics' changed from ",
				((IsSummaryStats()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsSummaryStats()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}
		if ( inSettings.IsLogMessages() != IsLogMessages() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Post messages to the Log' changed from ",
				((IsLogMessages()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsLogMessages()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}
		if ( inSettings.IsProtocolOfInterest( kProtocol_SMTP ) ^ IsProtocolOfInterest( kProtocol_SMTP ) ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Monitor SMTP' changed from ",
				((IsProtocolOfInterest( kProtocol_SMTP )) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsProtocolOfInterest( kProtocol_SMTP )) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}
		if ( inSettings.IsProtocolOfInterest( kProtocol_POP3 ) != IsProtocolOfInterest( kProtocol_POP3 ) ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Monitor POP3' changed from ",
				((IsProtocolOfInterest( kProtocol_POP3 )) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsProtocolOfInterest( kProtocol_POP3 )) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}

		if ( inSettings.GetFileNamePrefix() != GetFileNamePrefix() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Filename Prefix' changed from ",
				GetFileNamePrefix().GetA(),
				inSettings.GetFileNamePrefix().GetA() );
			inFile.Write( str );
		}

		if ( inSettings.IsAppendEmail() != IsAppendEmail() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Append email address' changed from ",
				((IsAppendEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()),
				((inSettings.IsAppendEmail()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) );
			inFile.Write( str );
		}

		if ( inSettings.GetOutputDirectory() != GetOutputDirectory() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Output Directory' changed from ",
				GetOutputDirectory().GetA(),
				inSettings.GetOutputDirectory().GetA() );
			inFile.Write( str );
		}

		bResult = GetReportMode().LogUpdate( inPreAmble, inSettings.GetReportMode(), inFile );

		if ( inSettings.GetStartTime() != GetStartTime() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Start Time' changed from ",
				FormatStartTimeA(),
				inSettings.FormatStartTimeA() );
			inFile.Write( str );
		}
		if ( inSettings.GetStopTime() != GetStopTime() ) {
			str = BuildModifyLog(
				inPreAmble,
				" 'Stop Time' changed from ",
				FormatStopTimeA(),
				inSettings.FormatStopTimeA() );
			inFile.Write( str );
		}
#endif // TOVERIFY
	}
	catch ( ... ) {
		return false;
	}

	return bResult;
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CCaseSettings::Model(
	CPeekDataElement&	ioCase )
{
	CPeekDataElement	elemFileNamePrefix( ConfigTags::kFileNamePrefix, ioCase );
	;	elemFileNamePrefix.Attribute( ConfigTags::kAppendEmail, m_bAppendEmail );
	{
		if ( !elemFileNamePrefix.Value( m_strFileNamePrefix ) && ioCase.IsLoading() ) {
			m_strFileNamePrefix.Empty();
		}
	}
	;	elemFileNamePrefix.End();

	CPeekDataElement	elemDirectory( ConfigTags::kOutputDirectory, ioCase );
	;	elemDirectory.Value( m_strOutputDirectory );
	;	elemDirectory.End();

	CPeekDataElement	elemProtocols( ConfigTags::kProtocols, ioCase );
	;	const CPeekString&	strSmtp( Tags::kxSmtp.GetW() );
	;	const CPeekString&	strPop3( Tags::kxPop3.GetW() );
	;	CPeekDataElement	elemSMTP( strSmtp, elemProtocols );
	;		elemSMTP.Enabled( m_Protocols[kProtocol_SMTP] );
	;		elemSMTP.End();
	;	CPeekDataElement	elemPOP3( strPop3, elemProtocols );
	;		elemPOP3.Enabled( m_Protocols[kProtocol_POP3] );
	;		elemPOP3.End();
	;	elemProtocols.End();

	CPeekDataElement	elemAcceptPackets( ConfigTags::kAcceptPackets, ioCase );
	;	elemAcceptPackets.Enabled( m_bAcceptPackets );
	;	elemAcceptPackets.End();

	CPeekDataElement	elemWritePackets( ConfigTags::kWritePackets, ioCase );
	;	elemWritePackets.Enabled( m_bWritePackets );
	;	elemWritePackets.End();

	CPeekDataElement	elemSummaryStats( ConfigTags::kSummaryStats, ioCase );
	;	elemSummaryStats.Enabled( m_bSummaryStats );
	;	elemSummaryStats.End();

	CPeekDataElement	elemLogMessages( ConfigTags::kLogMessages, ioCase );
	;	elemLogMessages.Enabled( m_bLogMessages );
	;	elemLogMessages.End();

	m_Mode.Model( ioCase );

	CPeekDataElement	elemStart( ConfigTags::kStartTime, ioCase );
	;	elemStart.Value( m_StartTime );
	;	elemStart.End();

	CPeekDataElement	elemStop( ConfigTags::kStopTime, ioCase );
	;	elemStop.Value( m_StopTime );
	;	elemStop.End();

	return true;
}


// ============================================================================
//		CCase
// ============================================================================

CCase::CCase()
	:	m_nId( gNextCaseId++ )
	,	m_bEnabled( Defaults::bEnabled )
	,	m_bDisplayIntegrity( Defaults::bDisplayIntegrity )
	,	m_strName( Defaults::strName )
{
}

CCase::CCase(
	size_t	nIndex )
	:	m_nId( gNextCaseId++ )
	,	m_bEnabled( Defaults::bEnabled )
	,	m_bDisplayIntegrity( Defaults::bDisplayIntegrity )
{
	CPeekOutString	posName;
	posName << Defaults::strName << L" " << nIndex;
	m_strName = posName;
}

CCase::CCase(
	const CCase&	inCase )
	:	m_nId( inCase.m_nId )
	,	m_bEnabled( inCase.m_bEnabled)
	,	m_bDisplayIntegrity( inCase.m_bDisplayIntegrity )
	,	m_strName( inCase.m_strName )
	,	m_AddressList( inCase.m_AddressList )
	,	m_Settings( inCase.m_Settings )
{
}


// ----------------------------------------------------------------------------
//		Analyze
// ----------------------------------------------------------------------------

bool
CCase::Analyze(
	CMsgStream*	inMsgStream )
{
	ASSERT( inMsgStream != NULL );
	if ( inMsgStream == NULL ) return false;

	if ( !IsActive() ) return false;

	tProtocolTypes	ProtocolType( inMsgStream->GetProtocolType() );
	if ( !IsProtocolOfInterest( ProtocolType ) ) return false;

	if ( m_AddressList.IsCaptureAll() ) {
		inMsgStream->Analyze( *this );
#if SAVE_ATTACHMENTS
		SaveAttachments( inMsgStream );
#endif
		return true;
	}

	bool			bResult( false );
	CTargetAddress	Address;
	size_t			nPos( kIndex_Invalid );
	bool			bContinue( m_AddressList.GetFirst( Address, nPos ) );
	while ( bContinue ) {
		if ( inMsgStream->HasTarget( Address ) ) {
			inMsgStream->Analyze( *this, Address );
#if SAVE_ATTACHMENTS
			SaveAttachments( inMsgStream );
#endif
			bResult = true;
		}
		bContinue = m_AddressList.GetNext( Address, nPos );
	}
	return bResult;
}


// ----------------------------------------------------------------------------
//		FormatA
// ----------------------------------------------------------------------------

CPeekStringA
CCase::FormatA() const
{
	CArrayStringA	aStrs;

	aStrs.Add( m_strName );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( Tags::kTargetIs );
	aStrs.Add( (m_bEnabled) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( Tags::kAddressList );
	aStrs.Add( m_AddressList.FormatLongA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( m_Settings.FormatA() );

	CPeekStringA	str = aStrs.ToString();
	return str;
}


// ----------------------------------------------------------------------------
//		FormatLongA
// ----------------------------------------------------------------------------

CPeekStringA
CCase::FormatLongA() const
{
	CArrayStringA	aStrs;

	aStrs.Add( m_strName );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( (m_bEnabled) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( m_AddressList.FormatLongA() );
	aStrs.Add( Tags::kxCommaSpace.GetA() );

	aStrs.Add( m_Settings.FormatLongA() );

	CPeekStringA	str = aStrs.ToString();
	return str;
}


// ----------------------------------------------------------------------------
//		GetColumnText
// ----------------------------------------------------------------------------

CPeekString
CCase::GetColumnText(
	int	inId ) const
{
	CPeekString		strText;

	switch ( inId ) {
	case kColumnId_Id:
		{
			CPeekOutString	ssText;
			ssText << GetId();
			strText = ssText;
		}
		break;

	case kColumnId_Name:
		strText = GetName().GetW();
		break;

	case kColumnId_Address:
		strText = GetAddressList().Format();
		break;

	case kColumnId_Count:
		strText = GetAddressList().FormatCount();
		break;

	case kColumnId_Protocol:
		strText = GetSettings().FormatProtocol();
		break;

	case kColumnId_Mode:
#if (TOVERIFY)
		{
			CPeekOutString	ssText;
			ssText <<
				GetSettings().GetReportMode().FormatMode() <<
				Tags::kxNewLine.GetW() <<
				GetSettings().FormatPackets();
			strText = ssText;
		}
#else
		strText.Format(
			L"%s\r\n%s",
			static_cast<PCTSTR>( GetSettings().GetReportMode().FormatMode() ),
			GetSettings().FormatPackets() );
#endif // TOVERIFY
		break;

	case kColumnId_State:
		strText = GetSettings().FormatActive();
		break;

	case kColumnId_Triggers:
#if (TOVERIFY)
		{
			CPeekOutString	ssText;
			ssText <<
				GetSettings().FormatStartTime() <<
				Tags::kxNewLine.GetW() <<
				GetSettings().FormatStopTime();
			strText = ssText;
		}
#else
		strText.Format(
			L"%s\r\n%s",
			GetSettings().FormatStartTime(),
			GetSettings().FormatStopTime() );
#endif // TOVERIFY
		break;

	case kColumnId_Folder:
#if (TOVERIFY)
		{
			CPeekOutString	ssText;
			ssText <<
				GetSettings().GetFileNamePrefixAppend() <<
				Tags::kxNewLine.GetW() <<
				GetSettings().GetOutputDirectory();
			strText = ssText;
		}
#else
		strText.Format(
			L"%s\r\n%s",
			static_cast<PCTSTR>( GetSettings().GetOutputDirectory() ),
			GetSettings().FormatLongName() );
#endif // TOVERIFY
		break;
	}

	return strText;
}


// ----------------------------------------------------------------------------
//		GetDefaultName		[static]
// ----------------------------------------------------------------------------

const CPeekString&
CCase::GetDefaultName()
{
	return Tags::strCaseName;
}


// ----------------------------------------------------------------------------
//		IsMatch
// ----------------------------------------------------------------------------

bool
CCase::IsMatch(
	const CCase& inCase ) const
{
	if ( IsEnabled() != inCase.IsEnabled() ) return false;
	if ( GetName().CompareNoCase( CPeekStringA( inCase.GetName() ) ) != 0 ) return false;
	if ( !GetAddressList().IsMatch( inCase.GetAddressList() ) ) return false;
	if ( !GetSettings().IsMatch( inCase.GetSettings() ) ) return false;

	return true;
}


// ----------------------------------------------------------------------------
//		Log
// ----------------------------------------------------------------------------

bool
CCase::Log(
	const CPeekStringA&	inPreAmble, 
	CFileEx&			inFile ) const
{
	try {
		{
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				Tags::kxSpace.RefA(),
				Tags::kxCase.RefA(),
				Tags::kxSpaceQuote.RefA(),
				m_strName.RefA(),
				Tags::kxQuote.RefA(),
				&Tags::kHasBeenAdded,
				Tags::kxNewLine.RefA(),
				NULL
			};
			CPeekStringA	strNameAdded( FastCat( ayStrs ) );
			inFile.Write( strNameAdded );
		}

		// Add an additional indent.
		CPeekStringA	strPreAmble( inPreAmble );
		strPreAmble += Tags::kxSpaceDash.GetA();

		{
			const CPeekStringA*	ayStrs[] = {
				&strPreAmble,
				&Tags::kCheckedSet,
				FormatEnabledRefA( IsEnabled() ),
				Tags::kxNewLine.RefA(),
				NULL
			};
			CPeekStringA	strChecked( FastCat( ayStrs ) );
			inFile.Write( strChecked );
		}

		m_AddressList.Log( strPreAmble, inFile );
		m_Settings.Log( strPreAmble, inFile );
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
CCase::LogUpdate(
	const CPeekStringA&	inPreAmble,
	const CCase&		inCase,
	CFileEx&			inFile ) const
{
	try {
		if ( IsMatch( inCase ) ) return true;

		CPeekStringA	str;

		if ( inCase.m_strName.CompareNoCase( static_cast<CPeekStringA>( m_strName ) ) != 0 ) {
#if (TOVERIFY)
			CPeekOutStringA	ss;
			ss <<
				inPreAmble <<
				" The case " <<
				m_strName.GetA() <<
				" renamed to " <<
				inCase.m_strName.GetA() <<
				Tags::kxNewLine.GetA();
			inFile.Write( ss );
#else
			str.Format(
				"%s The case %s renamed to %s.\r\n",
				inPreAmble,
				CPeekStringA( m_strName ),
				CPeekStringA( inCase.m_strName ) );
			inFile.Write( str );
#endif //  TOVERIFY
		}
		else {
#if (TOVERIFY)
			CPeekOutStringA	ss;
			ss <<
				inPreAmble <<
				" The case '" <<
				m_strName.GetA() <<
				"' has been modified." <<
				Tags::kxNewLine.GetA();
			inFile.Write( ss );
#else
			str.Format(
				"%s The case '%s' has been modified.\r\n",
				inPreAmble,
				static_cast<PCSTR>( m_strName ) );
			inFile.Write( str );
#endif // TOVERIFY
			inFile.Write( str );
		}

		CPeekStringA	strPreAmble;
#if (TOVERIFY)
		strPreAmble = inPreAmble + " -";
#else
		strPreAmble.Format( "%s -", inPreAmble );
#endif // TOVERIFY

		if ( inCase.IsEnabled() != IsEnabled() ) {
#if (TOVERIFY)
			CPeekOutStringA	ss;
			ss <<
				inPreAmble <<
				" Checked (enabled) changed from " <<
				((IsEnabled()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) <<
				" to " <<
				((inCase.IsEnabled()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA()) <<
				Tags::kxNewLine.GetA();
			inFile.Write( ss );
#else
			str.Format(
				"%s Checked (enabled) changed from %s to %s.\r\n",
				strPreAmble,
				(IsEnabled()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA(),
				(inCase.IsEnabled()) ? Tags::kxEnabled.GetA() : Tags::kxDisabled.GetA() );
			inFile.Write( str );
#endif // TOVERIFY
			inFile.Write( str );
		}

		m_AddressList.LogUpdate( strPreAmble, inCase.GetAddressList(), inFile );
		m_Settings.LogUpdate( strPreAmble, inCase.GetSettings(), inFile );
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
CCase::Model(
	CPeekDataElement&	ioCase )
{
	ioCase.Enabled( m_bEnabled );

	CPeekDataElement	elemName( L"Name", ioCase );
	;	elemName.Value( m_strName );
	;	elemName.End();

	CPeekDataElement	elemAddressList( ConfigTags::kAddressList, ioCase );
	;	m_AddressList.Model( elemAddressList );
	;	elemAddressList.End();

	m_Settings.Model( ioCase );

	return true;
}


#if SAVE_ATTACHMENTS
// ----------------------------------------------------------------------------
//		SaveAttachments
// ----------------------------------------------------------------------------

void
CCase::SaveAttachments(
	CMsgStream*	inMsgStream )
{
	if ( !m_Settings.m_bSaveAttachments ) return;

	CPeekString		strTarget( GetName() );
	bool			bResult;
	int				nPos( 0 );
	CPeekStringA	strAttach;
	do {
		bResult = inMsgStream->GetAttachmentText( nPos, strAttach );
		if ( bResult ) {
			CMsgAttachment	Attachment( strAttach );
			Attachment.WriteFile( m_Settings.m_strAttachmentDirectory, strTarget );
		}
	} while ( bResult );
}
#endif


// ============================================================================
//		CCaseList
// ============================================================================

// ----------------------------------------------------------------------------
//		Analyze
// ----------------------------------------------------------------------------

bool
CCaseList::Analyze(
	CMsgStream*	inMsgStream )
{
	ASSERT( inMsgStream != NULL );
	if ( inMsgStream == NULL ) return false;

	bool	bResult( inMsgStream->IsValid() );
	if ( !bResult ) return false;

	for ( size_t i = 0; i < GetCount(); i++ ) {
		CCase&	Case( GetAt( i ) );
		if ( Case.IsActive() ) {
			if ( Case.Analyze( inMsgStream ) ) {
				bResult = true;
			}
		}
	}
	return bResult;
}


// ----------------------------------------------------------------------------
//		Find
// ----------------------------------------------------------------------------

bool
CCaseList::Find(
	CCase::tCaseId	inId,
	size_t&			outIndex ) const
{
	ASSERT( inId > gFirstCaseId );
	for ( size_t i = 0; i < GetCount(); i++ ) {
		const CCase&	Case( GetAt( i ) );
		if ( Case.GetId() == inId ) {
			outIndex = i;
			return true;
		}
	}
	return false;
}

bool
CCaseList::Find(
	const CPeekString&					inName,
	std::vector<CCase>::const_iterator&	outIterator ) const
{
#if (TOVERIFY)
	std::vector<CCase>::const_iterator	item( begin() );
	std::vector<CCase>::const_iterator	last( end() );
	for ( ; item != last; ++item ) {
		if ( item->GetName() == inName ) {
			outIterator = item;
			return true;
		}
	}
#endif

	return false;
}


// ----------------------------------------------------------------------------
//		IsAcceptPackets
// ----------------------------------------------------------------------------

bool
CCaseList::IsAcceptPackets() const
{
#if (TOVERIFY)
	std::vector<CCase>::const_iterator	item( begin() );
	std::vector<CCase>::const_iterator	last( end() );
	for ( ; item != last; ++item ) {
		if ( item->GetSettings().IsAcceptPackets() ) {
			return true;
		}
	}
#endif

	return false;
}


// ----------------------------------------------------------------------------
//		IsAddressOfInterest
// ----------------------------------------------------------------------------

bool
CCaseList::IsAddressOfInterest(
	const CPeekString&	inAddress,
	size_t&				outIndex ) const
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		const CCase&	Case( GetAt( i ) );
		if ( Case.IsActive() ) {
			if ( Case.IsAddressOfInterest( inAddress ) ) {
				if ( i < kMaxUInt32 ) {
					outIndex = i;
					return true;
				}
				break;	// return false;
			}
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsLogMessages
// ----------------------------------------------------------------------------

bool
CCaseList::IsLogMessages() const
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		const CCase&	Case( GetAt( i ) );
		if ( Case.GetSettings().IsLogMessages() ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsProtocolOfInterest
// ----------------------------------------------------------------------------

bool
CCaseList::IsProtocolOfInterest(
	CProtocolType	inProtocolType ) const
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		const CCase&	Case( GetAt( i ) );
		if ( Case.IsActive() ) {
			if ( Case.IsProtocolOfInterest( inProtocolType ) ) {
				return true;
			}
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsSlicePackets
// ----------------------------------------------------------------------------

bool
CCaseList::IsSlicePackets() const
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		const CCase&	Case( GetAt( i ) );
		if ( Case.IsActive() &&
			 (Case.GetSettings().GetReportMode().GetMode() == kMode_Pen) ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		IsSummaryStats
// ----------------------------------------------------------------------------

bool
CCaseList::IsSummaryStats() const
{
	for ( size_t i = 0; i < GetCount(); i++ ) {
		const CCase&	Case( GetAt( i ) );
		if ( Case.GetSettings().IsSummaryStats() ) {
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------------------------
//		Log
// ----------------------------------------------------------------------------

bool
CCaseList::Log(
	const CPeekStringA&	inPreAmble,
	CFileEx&			inFile ) const
{
	try {
		if ( GetCount() == 0 ) {
			CPeekStringA	str;

#if (TOVERIFY)
			str = inPreAmble + " Has no cases." + Tags::kxNewLine.GetA();
#else
			str.Format(
				"%s Has no cases.\r\n",
				inPreAmble );
#endif // TOVERIFY
			inFile.Write( str );
		}
		else {
			for ( size_t i = 0; i < GetCount(); i++ ) {
				const CCase&	Case = GetAt( i );
				Case.Log( inPreAmble, inFile );
			}
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
CCaseList::LogUpdate(
	const CPeekStringA&	inPreAmble,
	const CCaseList&	inCaseList,
	CFileEx&			inFile ) const
{
	class CBool {
	protected:
		bool	m_Value;

	public:
		;	CBool() : m_Value( false ) {}

		bool	operator=( bool inValue ) {
			m_Value = inValue;
			return m_Value;
		}
		;		operator bool() const {
			return m_Value;
		}
	};


	typedef std::vector<CCase>::const_iterator ciCase;

	CBoolResultTrue	bResult;
	try {
		CPeekArray< ciCase >	ayCasesToRemove;
		const size_t			nCaseCount( inCaseList.GetCount() );
		CPeekArray<CBool>		ayUpdatedCases( nCaseCount );

		for ( const_iterator i( begin() ); i != end(); i++ ) {
			ciCase	ciCaseItem;
			if ( inCaseList.Find( i->GetName(), ciCaseItem ) ) {
				ayUpdatedCases[ciCaseItem - inCaseList.begin()] = true;	// ciCaseItem to index.
				bResult = i->LogUpdate( inPreAmble, *ciCaseItem, inFile );
			}
			else {
				ayCasesToRemove.Add( i );
			}
		}

		for ( size_t i( 0 ); i < nCaseCount; i++ ) {
			if ( !ayUpdatedCases[i] ) {
				const CCase&	InCase( inCaseList[i] );
				bResult = InCase.Log( inPreAmble, inFile );
			}
		}

		typedef CPeekArray< ciCase >::reverse_iterator	ri;
		ri	last( ayCasesToRemove.rend() );
		for ( ri i( ayCasesToRemove.rbegin() ); i != last; ++i ) {
			const CCase&	Case( **i );
#if (TOVERIFY)
			const CPeekStringA*	ayStrs[] = {
				&inPreAmble,
				&Tags::kCaseDeleteA,
				Case.GetName().RefA(),
				&Tags::kWasDeletedA,
				Tags::kxNewLine.RefA(),
				NULL
			};
			CPeekStringA	strDelete( FastCat( ayStrs ) );
#else
			CPeekStringA	strDelete;
			strDelete.Format( "%s - The case %s was deleted.\r\n", inPreAmble, static_cast<PCSTR>( Case.GetName() ) );
#endif // TOVERIFY
			inFile.Write( strDelete );
		} 
	}
	catch ( ... ) {
		return false;
	}

	return bResult;
}


// ----------------------------------------------------------------------------
//		Model
// ----------------------------------------------------------------------------

bool
CCaseList::Model(
	CPeekDataElement&	ioCaseList )
{
	bool	bLoading = ioCaseList.IsLoading();
	size_t	nCount = 0;
	
	if ( bLoading ) {
		RemoveAll();
		nCount = ioCaseList.GetChildCount( ConfigTags::kCase );
	}
	else {
		nCount = GetCount();
	}

	for ( size_t i = 0; i < nCount; i++ ) {
		CPeekDataElement	elemCase( ConfigTags::kCase, ioCaseList, i );
		if ( elemCase.IsValid() ) {
			if ( bLoading ) {
				Add();
			}
			CCase&	cCase = GetAt( i );
			cCase.Model( elemCase );
		}
	}
	return true;
}
