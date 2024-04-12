// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "OmniPlugin.h"
#include "GlobalTags.h"
#include "PeekDataModeler.h"


// ============================================================================
//		Globals
// ============================================================================

extern const CCase::tCaseId		gFirstCaseId;


// ============================================================================
//		Defaults
// ============================================================================

const UInt32	g_nMaxAddresses( 1000 );

namespace Defaults
{
	const CSize		sizeOptions( 533, 1144 );	// inits to ( 0, 0 )
	const int		nSizeCX( 533 );
	const int		nSizeCY( 1144 );
	const bool		bIntegrity( false );
	const bool		bDisplayPackets( true );
	const bool		bDisplaySummary( true );
	const bool		bDisplayMessages( true );
	const bool		bGlobalContext( false );
	const bool		bProcessPackets( false );

	// Column Widths
	const UInt32	nCaseIdWidth( 0 );
	const UInt32	nNameWidth( 100 );
	const UInt32	nAddressWidth( 200 );
	const UInt32	nCountWidth( 50 );
	const UInt32	nProtocolWidth( 75 );
	const UInt32	nModeWidth( 75 );
	const UInt32	nStateWidth( 55 );
	const UInt32	nTriggersWidth( 175 );
	const UInt32	kOutputWidth( 200 );
}


// ============================================================================
//		Global Tags
// ============================================================================

namespace Tags
{
	const CPeekString	kWPOmniPeekPlugins( L"WildPackets\\OmniPeek\\Plugins" );
	const CPeekString	kAppData( L"APPDATA" );
	const CPeekString	kConfigFileName( L"Config.xml" );
	const CPeekString	kChangeLogFileName( L"ChangeLog.txt" );
}

#if (0)
// Used to initialize COptionsView::m_List
// Column Labels
#define kTag_Id				L"Id"
#define	kTag_Name			L"Name"
#define	kTag_Address		L"Address"
#define	kTag_Count			L"Count"
#define kTag_Protocol		L"Protocol"
#define	kTag_Mode			L"Mode"
#define	kTag_State			L"State"
#define	kTag_Triggers		L"Triggers"
#define	kTag_Output			L"Output"

// Column Configuration Tags
#define kConfigTag_Id		L"Id"
#define	kConfigTag_Name		L"Name"
#define	kConfigTag_Address	L"Address"
#define	kConfigTag_Count	L"Count"
#define	kConfigTag_Protocol	L"Protocol"
#define kConfigTag_Mode		L"Mode"
#define	kConfigTag_State	L"State"
#define	kConfigTag_Triggers	L"Triggers"
#define	kConfigTag_Output	L"Output"
#endif


// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	const CPeekString	kEmailCapture( L"EmailCapture" );
	const CPeekString	kVersion( L"Version" );
	const CPeekString	kValue( L"Value" );
	const CPeekString	kHeight( L"Height" );
	const CPeekString	kWidth( L"Width" );

	// View
	const CPeekString	kView( L"View" );
	const CPeekString	kSize( L"Size" );
	const CPeekString	kIntegrity( L"DisplayIntegrityOptions" );

	// General Options
	const CPeekString	kGeneral( L"General" );
	const CPeekString	kDisplayPackets( L"DisplayPackets" );
	const CPeekString	kDisplaySummary( L"DisplaySummary" );
	const CPeekString	kDisplayMessages( L"DisplayMessages" );
	const CPeekString	kGlobalContext( L"GlobalContext" );
	const CPeekString	kProcessPackets( L"ProcessPackets" );

	// Target List
	const CPeekString	kCaseList( L"CaseList" );

	// Registry Keys
	const CPeekString	kKeyName_Capture( L"Capture" );
	const CPeekString	kKeyValue_Integrity( L"Integrity" );
	const CPeekString	kKeyValue_HashAlgorithm( L"HashAlgorithm" );
	const CPeekString	kKeyValue_KeyContainer( L"KeyContainer" );
}

CPeekString	COptions::s_strDefault( L"defaults" );

#if (0)
// ============================================================================
//		COptionsView
// ============================================================================

COptionsView::COptionsView()
	:	m_Size( Defaults::nSizeCX, Defaults::nSizeCY )
	,	m_bIntegrity( Defaults::bIntegrity )
{
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Id, kTag_Id, kConfigTag_Id, Defaults::nCaseIdWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Name, kTag_Name, kConfigTag_Name, Defaults::nNameWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Address, kTag_Address, kConfigTag_Address, Defaults::nAddressWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Count, kTag_Count, kConfigTag_Count, Defaults::nCountWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Protocol, kTag_Protocol, kConfigTag_Protocol, Defaults::nProtocolWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Mode, kTag_Mode, kConfigTag_Mode, Defaults::nModeWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_State, kTag_State, kConfigTag_State, Defaults::nStateWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Triggers, kTag_Triggers, kConfigTag_Triggers, Defaults::nTriggersWidth ) );
	m_ColumnPropList.Add( CColumnProp( CCase::kColumnId_Folder, kTag_Output, kConfigTag_Output, Defaults::kOutputWidth ) );
}
#endif


// ============================================================================
//		COptions::COptionsCases
// ============================================================================

// ----------------------------------------------------------------------------
//		Add
// ----------------------------------------------------------------------------

bool
COptions::COptionsCases::Add(
	const CCase&	inCase )
{
	m_CaseList.push_back( inCase );
	return true;
}


// ----------------------------------------------------------------------------
//		Update
// ----------------------------------------------------------------------------

bool
COptions::COptionsCases::Update(
	int				inIndex,
	const CCase&	inCase )
{
	if ( (inIndex < 0) || (inIndex >= static_cast<int>( m_CaseList.size() )) ) return false;
	m_CaseList[inIndex] = inCase;
	return true;
}


// =============================================================================
//		COptions
// =============================================================================

// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	m_Version = inOther.m_Version;
	m_XmlVersion = inOther.m_XmlVersion;
	m_Cases = inOther.m_Cases;
#if (0)
	m_View = inOther.m_View;
#endif
}


// -----------------------------------------------------------------------------
//		Log
// -----------------------------------------------------------------------------

bool
COptions::Log(
	const CPeekStringA&	inLabel,
	const CPeekString&	inChangeLogFileName )
{
	CPeekStringA	strCase;
	if ( inLabel != Tags::kxDefaultOptions.GetA() ) {
		strCase = Tags::kxCase.GetA();
		strCase += Tags::kxSpace.GetA();
	}

	CPeekStringA	strPreAmble;
	CPeekTime		ptNow( true );
	CPeekStringA	strNow( ptNow.FormatA( kMilliseconds ) );
	const CPeekStringA*	ayStrs[] = {
		&strNow,
		Tags::kxSpaceDash.RefA(),
		Tags::kxSpace.RefA(),
		&strCase,
		&inLabel,
		Tags::kxSpaceDash.RefA(),
		NULL
	};
	strPreAmble = FastCat( ayStrs );

	bool	bResult( true );
	try {
		CFileEx	feLog;
		int		nFlags( CFileEx::modeCreate | CFileEx::modeNoTruncate | CFileEx::modeWrite );
		feLog.Open( inChangeLogFileName, nFlags );
		if ( feLog.IsOpen() ) {
			feLog.SeekToEnd();
			m_Cases.Log( strPreAmble, feLog );
		}
	}
	catch ( ... ) {
		bResult = false;
	}

	return bResult;
}


// -----------------------------------------------------------------------------
//		LogUpdate
// -----------------------------------------------------------------------------

bool
COptions::LogUpdate(
	const CPeekStringA&	inLabel,
	const COptions&		inOptions,
	const CPeekString&	inChangeLogFileName )
{
	CPeekStringA	strCase;
	if ( inLabel != Tags::kxDefaultOptions.GetA() ) {
		strCase = Tags::kxCase.GetA();
		strCase += Tags::kxSpace.GetA();
	}

	CPeekStringA	strPreAmble;
	CPeekTime		ptNow( true );
	CPeekStringA	strNow( ptNow.FormatA( kMilliseconds ) );
	const CPeekStringA*	ayStrs[] = {
		&strNow,
		Tags::kxSpaceDash.RefA(),
		Tags::kxSpace.RefA(),
		&strCase,
		&inLabel,
		Tags::kxSpaceDash.RefA(),
		NULL
	};
	strPreAmble = FastCat( ayStrs );

	bool	bResult( true );
	try {
		CFileEx	feLog;
		int		nFlags( CFileEx::modeCreate | CFileEx::modeNoTruncate | CFileEx::modeWrite );
		feLog.Open( inChangeLogFileName, nFlags );
		if ( feLog.IsOpen() ) {
			feLog.SeekToEnd();
			m_Cases.LogUpdate( strPreAmble, inOptions.GetCases(), feLog );
		}
	}
	catch ( ... ) {
		bResult = false;
	}

	return bResult;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::Model(
	CPeekDataElement& ioPreferences )
{
	try {
		CPeekDataElement	elemRoot( ConfigTags::kEmailCapture, ioPreferences );
		;	elemRoot.Attribute( ConfigTags::kVersion, m_XmlVersion );

		;	CPeekDataElement	elemCaseList( ConfigTags::kCaseList, elemRoot );
		;		m_Cases.Model( elemCaseList );
		;		elemCaseList.End();

#if (0)
		;	CPeekDataElement	elemView( ConfigTags::kView, elemRoot );
		;		CPeekDataElement	elemSize( L"Size", elemView );
		;			CSize	sSize = m_View.GetSize();
		;			elemSize.Attribute( L"Width", sSize.cx );
		;			elemSize.Attribute( L"Height", sSize.cy );
		;			elemSize.End();
		;		elemView.End();
#endif

		elemRoot.End();
	}
	catch ( ... ) {
		Reset();
	}

	return true;
}

bool
COptions::Model(
	CPeekDataModeler& ioPreferences )
{
	try {
		CPeekDataElement	elemRoot( ConfigTags::kEmailCapture, ioPreferences );
		;	elemRoot.Attribute( ConfigTags::kVersion, m_XmlVersion );

		;	CPeekDataElement	elemCaseList( ConfigTags::kCaseList, elemRoot );
		;		m_Cases.Model( elemCaseList );
		;		elemCaseList.End();

#if (0)
		;	CPeekDataElement	elemView( ConfigTags::kView, elemRoot );
		;		CPeekDataElement	elemSize( L"Size", elemView );
		;			CSize	sSize = m_View.GetSize();
		;			elemSize.Attribute( L"Width", sSize.cx );
		;			elemSize.Attribute( L"Height", sSize.cy );
		;			elemSize.End();
		;		elemView.End();
#endif

		elemRoot.End();
	}
	catch ( ... ) {
		Reset();
	}

	return true;
}
