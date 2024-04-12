// ============================================================================
//	AddressTableParser.cpp
// ============================================================================
//	Copyright (c) 2002-2012 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "AddressTableParser.h"
#include "MediaSpecUtil.h"
#include <comdef.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// Handy macro to take care of the COM error handling
#define CHECKHR(x)		{ HRESULT hr = x; if (FAILED(hr)) _com_issue_error(hr);}

static const SIDNameMapEntry
s_MediaSpecNames[] =
{
	{ kMediaSpecType_IPAddr,				_T("IP") },
	{ kMediaSpecType_IPv6Addr,				_T("IPv6") },
	{ kMediaSpecType_EthernetAddr,			_T("Ethernet") },
	{ kMediaSpecType_WirelessAddr,			_T("Wireless") },
	{ kMediaSpecType_AppleTalkAddr,			_T("AppleTalk") },
	{ kMediaSpecType_IPXAddr,				_T("IPX") },
	{ kMediaSpecType_DECnetAddr,			_T("DECnet") },
	{ kMediaSpecType_TokenRingAddr,			_T("TokenRing") },
	{ kMediaSpecType_WAN_DLCIAddr,			_T("DLCI") }
};
#define kNumMediaSpecNameEntries			((sizeof(s_MediaSpecNames))/sizeof(SIDNameMapEntry))

static const SIDNameMapEntry
s_NodeTypeNames[] =
{
	{ kNameEntryType_Unknown,				_T("Unknown") },
	{ kNameEntryType_Workstation,			_T("Workstation") },
	{ kNameEntryType_Server,				_T("Server") },
	{ kNameEntryType_Router,				_T("Router") },
	{ kNameEntryType_Switch,				_T("Switch") },
	{ kNameEntryType_Repeater,				_T("Repeater") },
	{ kNameEntryType_Printer,				_T("Printer") },
	{ kNameEntryType_AccessPoint,			_T("Access Point") }
};
#define kNumNodeTypeNameEntries				((sizeof(s_NodeTypeNames))/sizeof(SIDNameMapEntry))

static const SIDNameMapEntry
s_ResolveTypeNamesv3[] =
{
	{ kNameEntrySource_ResolveUnknown,		_T("Unknown") },
	{ kNameEntrySource_ResolveActive,		_T("Active") },
	{ kNameEntrySource_ResolvePassive,		_T("Passive") },
	{ kNameEntrySource_ResolveUser,			_T("User") },
	{ kNameEntrySource_ResolvePlugin,		_T("Plugin") },
	{ kNameEntrySource_ResolveWildcard,		_T("Wildcard") },
	{ kNameEntrySource_ResolveNone,			_T("None") }
};
#define kNumResolveTypeEntriesv3			((sizeof(s_ResolveTypeNamesv3))/sizeof(SIDNameMapEntry))

static const SIDNameMapEntry
s_TrustNames[] = 
{
	{ kNameTrust_Unknown,					_T("Unknown") },
	{ kNameTrust_Known,						_T("Known") },
	{ kNameTrust_Trusted,					_T("Trusted") },
};
#define kNumTrustNames						((sizeof(s_TrustNames))/sizeof(SIDNameMapEntry))

CAddressTableParser::CAddressTableParser( CNameTableParser* pMainParser ) :
	m_nCurMediaSpecType(kMediaSpecType_Null ),
	m_pNameTableParser(pMainParser)
{
	memset( &m_NewNTEntry, 0, sizeof(m_NewNTEntry) );
}

CAddressTableParser::~CAddressTableParser()
{
}

void 
CAddressTableParser::Init()
{
	memset( &m_NewNTEntry, 0, sizeof(m_NewNTEntry) );
	m_NewNTEntry.fDateModified	= time( NULL );
	m_NewNTEntry.fDateLastUsed	= m_NewNTEntry.fDateModified;
	m_NewNTEntry.fType			= kNameEntryType_Unknown;
	m_NewNTEntry.fSource 		= kNameEntrySource_TypeUnknown | kNameEntrySource_ResolveUnknown;
	m_NewNTEntry.fFlags 		= 0;
	m_NewNTEntry.fGroup 		= kGroupID_None;
	m_NewNTEntry.fColor			= RGB(0,0,0);		// Default is black.
	m_NewNTEntry.fTrust			= kNameTrust_Unknown;
	
	m_strCurAddress.Empty();
	m_strCurGroup.Empty();
	m_strCurTrust.Empty();
}
void
CAddressTableParser::BeginElement( const wchar_t* pwchName, int cchChars, ISAXAttributes * pAttributes )
{
	wchar_t*	szBuffer = m_strCurElemName.GetBuffer( cchChars+1 );
	wcsncpy( szBuffer, pwchName, cchChars );
	szBuffer[cchChars] = '\0';
	m_strCurElemName.ReleaseBuffer();
	if ( !m_strCurElemName.Compare(L"Address") )
	{
		try
		{
			const wchar_t*	szAddrType;
			const wchar_t*	szNodeType;
			const wchar_t*	szResolveType;
			
			int		nLen = 0;
			HRESULT hr = pAttributes->getValueFromQName( L"Type", 4, &szAddrType, &nLen );
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
					TRACE( _T("Unsupported Media Type: %s\n"), CStringW( szAddrType, nLen ) );
					m_pNameTableParser->EntryParsingAbort();
				}
			}
			else
			{
				// Must define Address Type!! Wrong format... so skip
				TRACE( _T("Media Type not defined!\n") );
				m_pNameTableParser->EntryParsingAbort();
			}

			// Node Type
			nLen = 0;
			hr = pAttributes->getValueFromQName( L"Node", 4, (const wchar_t**)&szNodeType, &nLen );
			if ( hr == E_FAIL )
			{
				_com_issue_error(hr);
			}
			if ( hr == S_OK )
			{	
				UInt32 nNodeType;
				if ( GetIdFromName( (SIDNameMapEntry*) s_NodeTypeNames, kNumNodeTypeNameEntries, szNodeType, nLen, &nNodeType ))
				{
					m_NewNTEntry.fType = (UInt8) nNodeType;
				}
			}
			
			// Resolve Type
			nLen = 0;
			hr = pAttributes->getValueFromQName( L"Resolve", 7, (const wchar_t**)&szResolveType, &nLen );
			if ( hr == E_FAIL )
			{
				_com_issue_error(hr);
			}
			if ( hr == S_OK )
			{	
				UInt32 nResolveType;
				if ( GetIdFromName( (SIDNameMapEntry*) s_ResolveTypeNamesv3, kNumResolveTypeEntriesv3, szResolveType, nLen, &nResolveType ))
				{
					m_NewNTEntry.fSource |= (UInt8) nResolveType;
				}
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
CAddressTableParser::EndElement( const wchar_t* pwchName, int cchChars )
{
	if ( !wcsncmp(pwchName, L"Entry", cchChars ) )
	{
		// This is the last element
		//TRACE( _T("AddrTbl End of Entry\n") );

		// Get the medis spec 
		UMediaSpecUtils::StringToSpec( m_strCurAddress, m_nCurMediaSpecType, m_NewNTEntry.fSpec );

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
		
		// process trust
		if ( !m_strCurTrust.IsEmpty())
		{
			UInt32 nTrust;
			if ( GetIdFromName( (SIDNameMapEntry*) s_TrustNames, kNumTrustNames, m_strCurTrust, m_strCurTrust.GetLength(), &nTrust))
			{
				m_NewNTEntry.fTrust = (UInt8) nTrust;
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
CAddressTableParser::ProcessChar( const wchar_t* pwchChars, int cchChars )
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
		int curLen = (int) wcslen( m_NewNTEntry.fName );
		int toCopyLen = min( cchChars, kMaxNameLength - curLen );
		wcsncpy( m_NewNTEntry.fName + curLen, strENodeValue, toCopyLen );
		m_NewNTEntry.fName[curLen + toCopyLen] = _T('\0');
	}
	else if ( !m_strCurElemName.Compare(L"Address") )
	{
		//TRACE( _T("Address: %s\n"), strENodeValue );
		m_strCurAddress += strENodeValue;
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
	else if ( !m_strCurElemName.Compare(L"Trust") )
	{
		m_strCurTrust	+= strENodeValue;
	}
}
