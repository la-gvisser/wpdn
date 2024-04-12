// ============================================================================
//	ProtocolTableParser.cpp
// ============================================================================
//	Copyright (c) 2002-2007 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "ProtocolTableParser.h"
#include "MediaSpecUtil.h"
#include <comdef.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

static const SIDNameMapEntry
s_MediaSpecNames[] =
{
	{ kMediaSpecType_EthernetProto,			_T("EtherProto") },
	{ kMediaSpecType_LSAP,					_T("LSAP") },
	{ kMediaSpecType_SNAP,					_T("SNAP") },
	{ kMediaSpecType_WAN_PPP_Proto,			_T("WANPPPProto") },
	{ kMediaSpecType_WAN_FrameRelay_Proto,	_T("WANFrameRelayProto") },
	{ kMediaSpecType_WAN_X25_Proto,			_T("WANX25Proto") },
	{ kMediaSpecType_WAN_X25e_Proto,		_T("WANX25eProto") },
	{ kMediaSpecType_WAN_U200_Proto,		_T("WANU200Proto") },
	{ kMediaSpecType_WAN_Ipars_Proto,		_T("WANIparsProto") },
	{ kMediaSpecType_WAN_Q931_Proto,		_T("WANQ931Proto") }
};
#define kNumMediaSpecNameEntries	((sizeof(s_MediaSpecNames))/sizeof(SIDNameMapEntry))

CProtocolTableParser::CProtocolTableParser( CNameTableParser* pParserController )
	: m_pNameTableParser( pParserController)
	, m_nCurMediaSpecType( kMediaSpecType_Null )
{
}

CProtocolTableParser::~CProtocolTableParser()
{
}

void
CProtocolTableParser::Init()
{
	memset( &m_NewNTEntry, 0, sizeof(m_NewNTEntry) );
	m_NewNTEntry.fDateModified	= time( NULL );
	m_NewNTEntry.fDateLastUsed	= m_NewNTEntry.fDateModified;
	m_NewNTEntry.fType			= kNameEntryType_Unknown;
	m_NewNTEntry.fSource 		= kNameEntrySource_TypeUnknown | kNameEntrySource_ResolveUnknown;
	m_NewNTEntry.fFlags 		= 0;
	m_NewNTEntry.fGroup 		= kGroupID_None;
	m_NewNTEntry.fColor			= RGB(0,0,0);		// Default is black.

	m_strCurProtocol.Empty();
	m_strCurGroup.Empty();
}

void
CProtocolTableParser::BeginElement( const wchar_t* pwchName, int cchChars, ISAXAttributes *pAttributes )
{
	wchar_t*	szBuffer = m_strCurElemName.GetBuffer( cchChars+1 );
	wcsncpy( szBuffer, pwchName, cchChars );
	szBuffer[cchChars] = '\0';
	m_strCurElemName.ReleaseBuffer();
	if ( !m_strCurElemName.Compare(L"Protocol") )
	{
		try
		{
			wchar_t*	szAddrType;
			int		nLen = 0;
			HRESULT hr = pAttributes->getValueFromQName( L"Type", 4, (const wchar_t**)&szAddrType, &nLen );
			if ( hr == E_FAIL )
			{
				_com_issue_error(hr);
			}
			if ( hr == S_OK )
			{	
				bool bResult = GetIdFromName( (SIDNameMapEntry*) s_MediaSpecNames,
					kNumMediaSpecNameEntries, szAddrType, nLen, (UInt32*) &m_nCurMediaSpecType );
				if ( bResult == false )
				{
					// Wrong typename!!
					TRACE( _T("Unsupported Media Type: %s\t"), CStringW( szAddrType, nLen ) );
					m_pNameTableParser->EntryParsingAbort();
				}
			}
			else
			{
				// Must define Address Type!! Wrong format... so skip
				TRACE( _T("Media Type not defined!\n") );
				m_pNameTableParser->EntryParsingAbort();
			}
		}
		catch( _com_error& err )
		{
			err;
			TRACE( _T("COM Error: %s\n"), err.ErrorMessage() );
		}	
	}
}

void
CProtocolTableParser::EndElement( const wchar_t* pwchName, int cchChars )
{
	if ( !wcsncmp(pwchName, L"Entry", cchChars ) )
	{
		// This is the last element
		//TRACE( _T("AddrTbl End of Entry\n") );

		// Get the medis spec 
		UMediaSpecUtils::StringToSpec( m_strCurProtocol, m_nCurMediaSpecType, m_NewNTEntry.fSpec );

		// Process the group
		if (!m_strCurGroup.IsEmpty())
		{
			// finish processing the group
			CNameSubTable*	pSubTable = m_pNameTableParser->GetNameTable()->ClassToSubTable( m_NewNTEntry.fSpec.GetClass() );
			if ( pSubTable != NULL )
			{
				m_NewNTEntry.fGroup = pSubTable->GroupNameToIndex( m_strCurGroup );
				if ( m_NewNTEntry.fGroup == kGroupID_None )
				{
					// Group not found... create it!
					m_NewNTEntry.fGroup = pSubTable->AddGroup( m_strCurGroup );
				}
			}
		}

		// Now add the entry
		bool bAdded = m_pNameTableParser->GetNameTable()->AddEntry
		( 
			m_NewNTEntry,
			CNameTable::kResolveOption_AddrMatchSkip |
			CNameTable::kResolveOption_NoMatchAdd |
			CNameTable::kResolveOption_NameMatchAdd  
		);
		if ( bAdded )
		{
			// Let the parser controller know that the entry was added
			m_pNameTableParser->EntryAdded();
		}
		else
		{
			// Let the parser controller know that entry already exists
			m_pNameTableParser->EntrySkipped();
		}
	}
}

void
CProtocolTableParser::ProcessChar( const wchar_t* pwchChars, int cchChars )
{
	// Replace the following with string buffer based manipulation 
	// rather than using CString based
	CStringW strENodeValue( pwchChars, cchChars );
	strENodeValue = strENodeValue.Trim();
	if ( strENodeValue.IsEmpty() )
	{
		return;
	}

	// we have to concat all process chars things together as they come in
	// this is because special characters (&amp; in XML => &) come in seperately
	//TRACE( _T("\t") );
	if ( !m_strCurElemName.Compare(L"Name") )
	{
		//TRACE( _T("Name: %s\n"), strENodeValue );
		int curLen = (int) wcslen( m_NewNTEntry.fName );
		int toCopyLen = min( cchChars, kMaxNameLength - curLen );
		wcsncpy( m_NewNTEntry.fName + curLen, strENodeValue, toCopyLen );
		m_NewNTEntry.fName[curLen + toCopyLen] = _T('\0');
	}
	else if ( !m_strCurElemName.Compare(L"Protocol") )
	{
		//TRACE( _T("Address: %s\n"), strENodeValue );
		m_strCurProtocol += strENodeValue;
	}
	else if ( !m_strCurElemName.Compare(L"Color") )
	{
		// Color is "#xxxxxx", which are hex values for R,G,B.
		wchar_t*    pStart = strENodeValue.GetBuffer();
		wchar_t*	pEnd;
		UInt32	nColor = wcstoul( &pStart[1], &pEnd, 16 );
		strENodeValue.ReleaseBuffer();

		UInt8	nRed = (UInt8)(nColor >> 16);
		UInt8	nGreen = (UInt8)(nColor >> 8);
		UInt8	nBlue = (UInt8) nColor;
		m_NewNTEntry.fColor = RGB( nRed, nGreen, nBlue );
		//TRACE( _T("Color: %s\n"), strENodeValue );
	}
	else if ( !m_strCurElemName.Compare(L"Group") )
	{
		//TRACE( _T("Group: %s\n"), strENodeValue );
		m_strCurGroup += strENodeValue;
	}
	else if ( !m_strCurElemName.Compare(L"Mod") )
	{
		//TRACE( _T("Mod: %s\n"), strENodeValue );
		GetDateFromString( strENodeValue, &m_NewNTEntry.fDateModified );
	}
	else if ( !m_strCurElemName.Compare(L"Used") )
	{
		//TRACE( _T("Used: %s\n"), strENodeValue );
		GetDateFromString( strENodeValue, &m_NewNTEntry.fDateLastUsed );
	}
}