// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"
[!if NOTIFY_CODEEXAMPLES]
[!if !NOTIFY_UIHANDLER]

#ifdef IMPLEMENT_CODEEXAMPLES

  #define EXAMPLE_COMPUTER_COUNT	3
  #define EXAMPLE_USER_COUNT		5

static GUID	gSoftwareId = {
	0xC1A19CAC, 0x4FA3, 0x4223, { 0x82, 0x3F, 0xEC, 0x50, 0x54, 0xF3, 0x33, 0xA7 }
};

CGlobalId	COptions::m_idSoftware( gSoftwareId );

#endif // IMPLEMENT_CODEEXAMPLES
[!else]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
//
// -----------------------------------------------------------------------------
//		ReplaceAll
// -----------------------------------------------------------------------------

void
ReplaceAll(
	CPeekString&	inValue,
	wchar_t			inFind,
	const wchar_t*	inReplace )
{
	size_t	pos = inValue.Find( inFind );
	while ( pos != std::wstring::npos ) {
		inValue.replace( pos, 1, inReplace );
		pos = inValue.find( inFind );
	}
}


// -----------------------------------------------------------------------------
//		xmlEncode
// -----------------------------------------------------------------------------

CPeekString
xmlEncode(
	const CPeekString&	inValue,
	bool				inEncode )
{
	CPeekString	strValue( inValue );

	static const std::wstring strTab( L"&#0009;" );
	static const std::wstring strNewLine( L"&#0010;" );
	static const std::wstring strReturn( L"&#0013;" );

	if ( inEncode ) {
		ReplaceAll( strValue, L'\t', strTab.c_str() );
		ReplaceAll( strValue, L'\n', strNewLine.c_str() );
		ReplaceAll( strValue, L'\r', strReturn.c_str() );
	}

	return strValue;
}
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
[!endif]


// =============================================================================
//		COptions
// =============================================================================

COptions::COptions()
{
[!if NOTIFY_CODEEXAMPLES && NOTIFY_UIHANDLER]
#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
	m_ayOptions.resize( s_nCount );
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	Reset();
}

COptions::~COptions()
{
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	(void)inOther;
[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
[!if NOTIFY_NOTIFICATIONS]
  #ifdef IMPLEMENT_NOTIFICATIONS
	m_bReceiveNotifies = inOther.m_bReceiveNotifies;
  #endif // IMPLEMENT_NOTIFICATIONS

[!endif]
[!if NOTIFY_UIHANDLER]
	m_ayOptions.resize( inOther.m_ayOptions.size() );
	auto itr = m_ayOptions.begin();
	for ( auto itrOther = inOther.m_ayOptions.begin(); itrOther != inOther.m_ayOptions.end(); ++itrOther ) {
		*itr = *itrOther;
		++itr;
	}
	m_strNotes = inOther.m_strNotes;
[!else]
	m_bEnabledExample = inOther.m_bEnabledExample;
	m_strLabelExample = inOther.m_strLabelExample;
	m_EthernetExample = inOther.m_EthernetExample;
	m_IpAddressExample = inOther.m_IpAddressExample;
	m_nPortExample = inOther.m_nPortExample;
	m_TimeExample = inOther.m_TimeExample;
[!endif]
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther )
{
	(void)inOther;
[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
[!if NOTIFY_NOTIFICATIONS]
  #ifdef IMPLEMENT_NOTIFICATIONS
	m_bReceiveNotifies = inOther.m_bReceiveNotifies;
  #endif // IMPLEMENT_NOTIFICATIONS

[!endif]
[!if NOTIFY_UIHANDLER]
	if ( m_ayOptions.size() != inOther.m_ayOptions.size() ) return false;

	auto itrOther = inOther.m_ayOptions.begin();
	for ( auto itr = m_ayOptions.begin(); itr != m_ayOptions.end(); ++itr ) {
		if ( *itr != *itrOther ) return false;
	}
	if ( m_strNotes != inOther.m_strNotes ) return false;
[!else]
	if ( m_bEnabledExample != inOther.m_bEnabledExample ) return false;
	if ( m_strLabelExample != inOther.m_strLabelExample ) return false;
	if ( m_IpAddressExample != inOther.m_IpAddressExample ) return false;
	if ( m_nPortExample != inOther.m_nPortExample ) return false;
	if ( m_TimeExample != inOther.m_TimeExample ) return false;
[!endif]
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------
[!if NOTIFY_NOTIFICATIONS && NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
  #ifdef IMPLEMENT_NOTIFICATIONS
	#define kOptionsReceiveNotifies	L"ReceiveNotifies"
  #endif // IMPLEMENT_NOTIFICATIONS
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elem( ioPrefs );
	return Model( elem );
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	(void)ioElement;
[!if NOTIFY_CODEEXAMPLES]

#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
//
#ifdef _DEBUG
	// Get the initial Options XML as a string.
	CPeekStream streamInXml;
	ioElement.Store( streamInXml );
	CPeekStringA strInXml( (const char*)streamInXml.GetData(), streamInXml.GetLength() );
#endif // _DEBUG

[!if NOTIFY_NOTIFICATIONS]
  #ifdef IMPLEMENT_NOTIFICATIONS
	CPeekDataElement	elemNotifyOptions( L"NotifyOptions", ioElement );
		elemNotifyOptions.Attribute( kOptionsReceiveNotifies, m_bReceiveNotifies );
	elemNotifyOptions.End();
  #endif // IMPLEMENT_NOTIFICATIONS

[!endif]
[!if NOTIFY_UIHANDLER]
	CPeekDataElement elemOptions( L"Options", ioElement );
	;	UInt32	nIndex = 0;
	;	for ( auto itr = m_ayOptions.begin(); itr != m_ayOptions.end(); ++itr ) {
	;		ioElement.ChildValue( L"Case", *itr, nIndex++ );
	;	}

	;	CPeekDataElement	elemNotes( L"Notes", ioElement );
	;		CPeekString	strNotes = xmlEncode( m_strNotes, ioElement.IsStoring() );
	;		elemNotes.Value( (ioElement.IsStoring()) ? strNotes : m_strNotes );
[!else]
	CPeekDataElement	elemObjExamples( L"Examples", ioElement );
	if ( elemObjExamples ) {
		ObjectExamples( elemObjExamples );
		elemObjExamples.End();
	}

	CPeekDataElement	elemRepeatingExample( L"RepeatingExample", ioElement );
	if ( elemRepeatingExample ) {
		RepeatingExample( elemRepeatingExample );
		elemRepeatingExample.End();
	}
[!endif]

#ifdef _DEBUG
	// Get the initial Options XML as a string.
	CPeekStream streamOutXml;
	ioElement.Store( streamOutXml );
	CPeekStringA strOutXml( (const char*)streamOutXml.GetData(), streamOutXml.GetLength() );
#endif
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES

[!endif]
	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset()
{
[!if NOTIFY_CODEEXAMPLES]
#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage
[!if NOTIFY_NOTIFICATIONS]
#ifdef IMPLEMENT_NOTIFICATIONS
	m_bReceiveNotifies = false;
#endif // IMPLEMENT_NOTIFICATIONS

[!endif]
[!if NOTIFY_UIHANDLER]
	for ( auto itr = m_ayOptions.begin(); itr != m_ayOptions.end(); ++itr ) {
		itr->clear();
	}
	m_strNotes.Empty();
[!else]
	m_bEnabledExample = FALSE;
	m_strLabelExample = L"User One";
	m_EthernetExample.Reset();
	m_IpAddressExample.Reset();
	m_nPortExample.Reset();
	m_TimeExample.SetNow();
	m_ayComputers.RemoveAll();
[!endif]
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
}
[!if NOTIFY_CODEEXAMPLES && !NOTIFY_UIHANDLER]


#ifdef IMPLEMENT_CODEEXAMPLES
////////////////////////////////////////////////////////////////////////
// Example usage

// -----------------------------------------------------------------------------
//		ObjectExamples
// -----------------------------------------------------------------------------

void
COptions::ObjectExamples(
	CPeekDataElement&	inParentElement )
{
	CPeekDataElement	elemObject( L"Object", inParentElement );
	if ( elemObject ) {
		elemObject.Enabled( m_bEnabledExample );
		elemObject.ChildValue( L"Label", m_strLabelExample );
		elemObject.ChildValue( L"Ethernet", m_EthernetExample );
		elemObject.ChildValue( L"Address", m_IpAddressExample );
		elemObject.ChildValue( L"Port", m_nPortExample );
		elemObject.ChildValue( L"Time", m_TimeExample );

		elemObject.End();
	}

	return;
}


// -----------------------------------------------------------------------------
//		RepeatingExample
//		Create demo lists of computers and users
// -----------------------------------------------------------------------------

void
COptions::RepeatingExample(
	CPeekDataElement&	inParentElement )
{
	bool bIsLoading = inParentElement.IsLoading();

	tComputerExample	aComputerList[EXAMPLE_COMPUTER_COUNT];
	if ( !bIsLoading ) {
		for ( UInt32 i = 0; i < EXAMPLE_COMPUTER_COUNT; i++ ) {
			tComputerExample&	comp = aComputerList[i];
			CPeekOutString		strName;
			strName << L"Computer " << i;
			comp.fName = strName;
			comp.fIpAddress = 0xA099480 + ( i * 100 );
			comp.fPort = static_cast<UInt16>( i );
			comp.fId = m_idSoftware;
			for ( UInt32 nUserIndex = 0; nUserIndex < EXAMPLE_USER_COUNT; nUserIndex++ ) {
				CPeekOutString	strUser;
				strUser << L"User " << (nUserIndex + 1);
				comp.fUserNames.Add( strUser );
			}
		}
	}

	CPeekDataElement	elemComputerList( L"ComputerList", inParentElement );
	UInt32				nComputerCount;
	if ( bIsLoading ) {
		// If the list was an CPeekArray: aComputerList.RemoveAll();
		nComputerCount = elemComputerList.GetChildCount();
	}
	else {
		nComputerCount = EXAMPLE_COMPUTER_COUNT;	// aComputerList.GetCount();
	}

	for ( UInt32 nComputer = 0; nComputer < nComputerCount; nComputer++ ) {
		CPeekDataElement	elemComputer( L"Computer", elemComputerList, nComputer );
		if ( elemComputer.IsValid() ) {
			if ( bIsLoading ) {
				// If the list was an CPeekArray: aComputerList.Add();
			}
			tComputerExample&	comp = aComputerList[nComputer];
			RepeatComputerExample( elemComputer, comp );
		}
	}

  #ifdef _DEBUG
	if ( bIsLoading && (nComputerCount > 0) ) {
		for ( UInt32 i = 0; i < EXAMPLE_COMPUTER_COUNT; i++ ) {
			tComputerExample&	compExample = aComputerList[i];

			CPeekOutString	strName;
			strName << L"Computer " << i;
			_ASSERTE( compExample.fName.Compare( strName ) == 0 );

			CIpAddress	IpAddress( 168400000 + ( i * 100 ) );
			_ASSERTE( compExample.fIpAddress == IpAddress );

			CPort	Port( static_cast<UInt16>( i ) );
			_ASSERTE( compExample.fPort == Port );

			_ASSERTE( compExample.fId == m_idSoftware );

			for ( UInt32 nUserIndex = 0; nUserIndex < EXAMPLE_USER_COUNT; nUserIndex++ ) {
				CPeekOutString	strUser;
				strUser << L"User " << (nUserIndex + 1);
				_ASSERTE( compExample.fUserNames[nUserIndex].Compare( strUser ) == 0 );
			}
		}
	}
  #endif // _DEBUG
}


// -----------------------------------------------------------------------------
//		RepeatComputerExample
// -----------------------------------------------------------------------------

void
COptions::RepeatComputerExample(
	CPeekDataElement&	ioComputerElement,
	tComputerExample&	ioComputer )
{
	bool bIsLoading = ioComputerElement.IsLoading();

	// Model the ComputerObject;
	ioComputerElement.Attribute( L"Name", ioComputer.fName );
	ioComputerElement.ChildValue( L"IPAddress", ioComputer.fIpAddress );
	ioComputerElement.ChildValue( L"Port", ioComputer.fPort );
	ioComputerElement.ChildValue( L"Id", ioComputer.fId );

	CPeekDataElement	elemUserList( L"UserList", ioComputerElement );
	UInt32	nUserCount;
	if ( bIsLoading ) {
		ioComputer.fUserNames.RemoveAll();
		nUserCount = elemUserList.GetChildCount();
	}
	else {
		nUserCount = EXAMPLE_USER_COUNT;
	}
	for ( UInt32 nUser = 0; nUser < nUserCount; nUser++ ) {
		if ( bIsLoading ) {
			ioComputer.fUserNames.Add( L"" );
		}
		CPeekString&	strUserName = ioComputer.fUserNames[nUser];
		elemUserList.ChildValue( L"User", strUserName, nUser );
	}
}
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
[!endif]
