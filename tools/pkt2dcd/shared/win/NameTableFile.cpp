// =============================================================================
//	NameTableFile.cpp
// =============================================================================
//	Copyright (c) 1999-2008 WildPackets, Inc. All rights reserved.

#include "stdafx.h"
#include "NameTableFile.h"
#include "ArrayEx.h"
#include "ByteStream.h"
#include "EP3xColors.h"
//#include "LoadResourceString.h"
#include "MediaSpecUtil.h"
#include "MemUtil.h"
#include "NameTable.h"

#include "Resource.h"
#include "XMLDoc.h"
#include "XMLIter.h"
#include <time.h>
#include "hlstrconv.h"


#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

namespace NameTableFile
{

// Container structure for element and attribute names.
struct NameTableXMLKeys
{
	const TCHAR*	f_pszNameTableVersion;
	const TCHAR*	f_pszElementName_Entry;
	const TCHAR*	f_pszElementName_Color;
	const TCHAR*	f_pszElementName_Group;
	const TCHAR*	f_pszElementName_ModDate;
	const TCHAR*	f_pszElementName_UsedDate;
	const TCHAR*	f_pszElementName_Address;
	const TCHAR*	f_pszElementName_Protocol;
	const TCHAR*	f_pszElementName_Port;
	const TCHAR*	f_pszElementName_Name;
	const TCHAR*	f_pszAttributeName_Class;
	const TCHAR*	f_pszAttributeName_Type;
	const TCHAR*	f_pszAttributeName_NodeType;
	const TCHAR*	f_pszAttributeName_ResolveType;
	const TCHAR*	f_pszAttributeName_SourceType;
	const TCHAR*	f_pszElementName_Trust;
};

// Version 1.0 element and attribute names.
static const NameTableXMLKeys s_Version1Keys =
{
	_T("1.0"),
	_T("NameTableEntry"),
	_T("Color"),
	_T("Group"),
	_T("ModDate"),
	_T("UsedDate"),
	_T("Address"),
	_T("Protocol"),
	_T("Port"),
	_T("Name"),
	_T("Class"),
	_T("Type"),
	_T("NodeType"),
	_T("ResolveType"),
	_T("SourceType"),
	_T("Trust")
};

// Version 2.0 element and attribute names.
static const NameTableXMLKeys s_Version2Keys =
{
	_T("2.0"),
	_T("Entry"),
	_T("Color"),
	_T("Group"),
	_T("Mod"),
	_T("Used"),
	_T("Address"),
	_T("Protocol"),
	_T("Port"),
	_T("Name"),
	_T("Class"),
	_T("Type"),
	_T("Node"),
	_T("Resolve"),
	_T("Source"),
	_T("Trust")
};

// Version 3.0 element and attribute names.
static const NameTableXMLKeys s_Version3Keys =
{
	_T("3.0"),
	_T("Entry"),
	_T("Color"),
	_T("Group"),
	_T("Mod"),
	_T("Used"),
	_T("Address"),
	_T("Protocol"),
	_T("Port"),
	_T("Name"),
	_T("Class"),
	_T("Type"),
	_T("Node"),
	_T("Resolve"),
	_T("Source"),
	_T("Trust")
};

// Set current version keys to latest version (currently, 3.0).
static const NameTableXMLKeys*	s_pCurrentVersionKeys = &s_Version3Keys;

// Root element name and version attribute name should NEVER change!
static const TCHAR*	kElementName_Root		= _T("NameTable");
static const TCHAR* kAttributeName_Version	= _T("Version");

struct IDNameMapEntry
{
	UInt32			f_nID;
	const TCHAR*	f_pszName;
};

static const IDNameMapEntry
s_NameTableColumns[] =
{
	{ kNameTableColumn_ID,					_T("ID") },
	{ kNameTableColumn_Name,				_T("Name") },
	{ kNameTableColumn_Type,				_T("Type") },
	{ kNameTableColumn_Entry,				_T("Entry") },
	{ kNameTableColumn_Color,				_T("Color") },
	{ kNameTableColumn_Group,				_T("Group") },
	{ kNameTableColumn_LastModDate,			_T("LastModDate") },
	{ kNameTableColumn_LastUsedDate,		_T("LastUsedDate") },
	{ kNameTableColumn_NodeType,			_T("NodeType") },
	{ kNameTableColumn_ResolveType,			_T("ResolveType") },
	{ kNameTableColumn_ResolveSource,		_T("ResolveSource") },
	{ kNameTableColumn_Trust,				_T("Trust") }
};

static const IDNameMapEntry
s_MediaSpecNames[] =
{
	{ kMediaSpecType_EthernetAddr,			_T("Ethernet") },
	{ kMediaSpecType_TokenRingAddr,			_T("TokenRing") },
	{ kMediaSpecType_AppleTalkAddr,			_T("AppleTalk") },
	{ kMediaSpecType_IPAddr,				_T("IP") },
	{ kMediaSpecType_IPXAddr,				_T("IPX") },
	{ kMediaSpecType_DECnetAddr,			_T("DECnet") },
	{ kMediaSpecType_EthernetProto,			_T("EtherProto") },
	{ kMediaSpecType_LSAP,					_T("LSAP") },
	{ kMediaSpecType_SNAP,					_T("SNAP") },
	{ kMediaSpecType_IPPort,				_T("IPPort") },
	{ kMediaSpecType_ATPort,				_T("ATPort") },
	{ kMediaSpecType_NWPort,				_T("NWPort") },
	{ kMediaSpecType_WAN_PPP_Proto,			_T("WANPPPProto") },
	{ kMediaSpecType_WAN_FrameRelay_Proto,	_T("WANFrameRelayProto") },
	{ kMediaSpecType_WAN_X25_Proto,			_T("WANX25Proto") },
	{ kMediaSpecType_WAN_X25e_Proto,		_T("WANX25eProto") },
	{ kMediaSpecType_WAN_U200_Proto,		_T("WANU200Proto") },
	{ kMediaSpecType_WAN_Ipars_Proto,		_T("WANIparsProto") },
	{ kMediaSpecType_WAN_DLCIAddr,			_T("WANDLCIAddr") },
	{ kMediaSpecType_WAN_Q931_Proto,		_T("WANQ931Proto") },
	{ kMediaSpecType_WirelessAddr,			_T("Wireless") },
	{ kMediaSpecType_IPv6Addr,				_T("IPv6") }
};

static const IDNameMapEntry
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

static const IDNameMapEntry
s_ResolveTypeNamesv2[] =
{
	{ kNameEntrySource_ResolveUnknown,		_T("Unknown") },
	{ kNameEntrySource_ResolveActive,		_T("Active") },
	{ kNameEntrySource_ResolveUser,			_T("Passive") },
	{ kNameEntrySource_ResolvePlugin,		_T("User") },
	{ kNameEntrySource_ResolveWildcard,		_T("Plugin") },
	{ kNameEntrySource_ResolveNone,			_T("Wildcard") }
};

static const IDNameMapEntry
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

static const IDNameMapEntry
s_SourceTypeNames[] =
{
	{ kNameEntrySource_TypeUnknown,			_T("Unknown") },
	{ kNameEntrySource_TypeIP,				_T("IP") },
	{ kNameEntrySource_TypeAT,				_T("AppleTalk") }
};	

static const IDNameMapEntry
s_TrustNames[] = 
{
	{ kNameTrust_Unknown,					_T("Unknown") },
	{ kNameTrust_Known,						_T("Known") },
	{ kNameTrust_Trusted,					_T("Trusted") }
};

// Private functions.
static bool         NameTableExportTextRow( const CNameSubTable* pSubTable,
						SNameTableEntry& theEntry, const TCHAR inSeparator, CString& theRecord,
						const NameTableColumn* pColumns, const int nColumns );
static void         NameTableExportXMLFormat( const CNameSubTable* pSubTable,
						XXMLElement* pNameTable, SNameTableEntry &theEntry );
static const TCHAR*	GetMediaSpecName( TMediaSpecType inType );
static const TCHAR*	GetNodeTypeName( UInt8	inType );
static const TCHAR*	GetResolveTypeName( UInt8 inType );
static const TCHAR* GetSourceTypeName( UInt8 inType );
static const TCHAR* GetTrustTypeName( UInt8 inTrust );
static bool			GetMediaSpecType( const TCHAR* inName, TMediaSpecType& outType );
static bool			GetNodeType( const TCHAR* inName, UInt8& outType );
static bool			GetResolveTypeVersion2( const TCHAR* inName, UInt8& outType );
static bool			GetResolveTypeVersion3( const TCHAR* inName, UInt8& outType );
static bool			GetSourceType( const TCHAR* inName, UInt8& outType );
static bool			GetTrustType( const TCHAR* inName, UInt8& outType );
static void			CalcFieldDelimiter( const CByteStream& inNameTableStream, TCHAR* szOutDelimiter, bool* outIsASCII  );
static void			TrimQuotes( CString& ioString );

// Typedefs for different GetResolveType versions.
typedef bool (*PGETRSOLVETYPEFUNC)( const TCHAR*, UInt8& );


// -----------------------------------------------------------------------------
//		NameTableExportTextRow
// -----------------------------------------------------------------------------

bool
NameTableExportTextRow(
	const CNameSubTable*    pSubTable,
	SNameTableEntry&		theEntry,
	const TCHAR				inSeparator,
	CString&				theRecord,
	const NameTableColumn*	pColumns,
	const int				nColumns )
{
	static const NameTableColumn s_DefaultColumns[] = 
	{
		kNameTableColumn_Name,
		kNameTableColumn_Type,
		kNameTableColumn_Entry,
		kNameTableColumn_Color,
		kNameTableColumn_Group,
		kNameTableColumn_LastModDate,
		kNameTableColumn_LastUsedDate,
		kNameTableColumn_NodeType,
		kNameTableColumn_ResolveType,
		kNameTableColumn_ResolveSource,
		kNameTableColumn_Trust,
	};
	const NameTableColumn*	pExportColumns = (NULL != pColumns) ? pColumns : s_DefaultColumns;
	const int				nExportColumns = (NULL != pColumns) ? nColumns : COUNTOF(s_DefaultColumns);

	for ( int i = 0; i < nExportColumns; ++i )
	{
		switch ( pExportColumns[i] )
		{
			case kNameTableColumn_ID:
			{
			}
			break;

			case kNameTableColumn_Name:
			{
				if ( inSeparator == _T(',') )
				{
					theRecord += _T('"');
				}
				theRecord += theEntry.fName;
				if ( inSeparator == _T(',') )
				{
					theRecord += _T('"');
				}
			}
			break;

			case kNameTableColumn_Type:
			{
				// Type.
				const TCHAR* pTypeName = GetMediaSpecName( theEntry.fSpec.GetType() );
				if ( pTypeName == NULL ) return false;
				theRecord += pTypeName;
			}
			break;

			case kNameTableColumn_Entry:
			{
				// Entry.
				TCHAR	entryData[256];
				if ( !UMediaSpecUtils::SpecToString( theEntry.fSpec, entryData ) )
				{
					return false;
				}
				theRecord += entryData;
			}
			break;

			case kNameTableColumn_Color:
			{
				// Color.
				if ( theEntry.fColor != RGB(0,0,0) )	// Color is not black.
				{
					TCHAR	szColor[256];
					_stprintf( szColor, _T("#%02X%02X%02X"), GetRValue(theEntry.fColor),
						GetGValue(theEntry.fColor), GetBValue(theEntry.fColor) );
					theRecord += szColor;
				}
			}
			break;

			case kNameTableColumn_Group:
			{
				// Group.
				if ( (pSubTable != NULL) && (theEntry.fGroup != kGroupID_None) )
				{
					TCHAR	szGroup[256];
					pSubTable->GetNthGroupName( theEntry.fGroup, szGroup );
					if ( szGroup[0] != 0 )
					{
						theRecord += szGroup;
					}
				}
			}
			break;

			case kNameTableColumn_LastModDate:
			{
				// Modified date.
				if ( theEntry.fDateModified != 0 )
				{
					struct tm*	pTime = gmtime( &theEntry.fDateModified );
					if ( pTime != NULL )
					{
						// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssZ
						// C = century; Y = year; M = month, D = day,
						// h = hour, m = minute, s = second. Z indicates UTC.
						TCHAR	szDate[256];
						_stprintf( szDate, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ"),
							pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
							pTime->tm_hour, pTime->tm_min, pTime->tm_sec );
						theRecord += szDate;
					}
				}
			}
			break;

			case kNameTableColumn_LastUsedDate:
			{
				// Last used date.
				if ( theEntry.fDateLastUsed != 0 )
				{
					struct tm*	pTime = gmtime( &theEntry.fDateLastUsed );
					if ( pTime != NULL )
					{
						TCHAR	szDate[256];
						_stprintf( szDate, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ"),
							pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
							pTime->tm_hour, pTime->tm_min, pTime->tm_sec );
						theRecord += szDate;
					}
				}
			}
			break;

			case kNameTableColumn_NodeType:
			{
				if ( theEntry.fType != kNameEntryType_Unknown )
				{
					LPCTSTR	pszTemp = GetNodeTypeName( theEntry.fType );
					if ( pszTemp != NULL )
					{
						theRecord += pszTemp;
					}
				}
			}
			break;

			case kNameTableColumn_ResolveType:
			{
				const UInt8	nResolveType = (UInt8)(theEntry.fSource & kNameEntrySource_ResolveMask);
				if ( nResolveType != 0 )
				{
					LPCTSTR	pszTemp = GetResolveTypeName( nResolveType );
					if ( pszTemp != NULL )
					{
						theRecord += pszTemp;
					}
				}
			}
			break;

			case kNameTableColumn_ResolveSource:
			{
				const UInt8	nResolveSource = (UInt8)(theEntry.fSource & kNameEntrySource_TypeMask);
				if ( nResolveSource != 0 )
				{
					LPCTSTR	pszTemp = GetSourceTypeName( nResolveSource );
					if ( pszTemp != NULL )
					{
						theRecord += pszTemp;
					}
				}
			}
			break;

			case kNameTableColumn_Trust:
			{
				// Trust.
				if ( theEntry.fTrust != kNameTrust_Unknown )
				{
					theRecord += GetTrustTypeName( theEntry.fTrust );
				}
			}
			break;
		}

		if ( i < (nExportColumns-1) )
		{
			theRecord += inSeparator;
		}
	}

	theRecord += _T("\r\n");

	return true;
}


// -----------------------------------------------------------------------------
//		NameTableExportText
// -----------------------------------------------------------------------------

bool
NameTableExportText(
	const CNameTable*		inNameTable,
	CByteStream&			outNameTableStream,
	const TCHAR				inSeparator,
	const NameTableColumn*	pColumns,
	const int				nColumns )
{
	// Sanity check.
	ASSERT( inNameTable != NULL );
	if ( inNameTable == NULL ) return false;

	const bool		bUsingCSV = (_T(',') == inSeparator) ? true : false;

	// Figure out how many names to export.
	const UInt32	nNamesToExport = inNameTable->GetCount();

	// Set up progress indicator.
	//CProgressIndicator	theProgress( AfxGetMainWnd(), IDD_PROGRESS_CANCEL,
	//						LoadResourceString( IDS_PROGRESS_EXPORTING_NAMES ) );

	//theProgress.SetRange( 1, nNamesToExport );
	//theProgress.SetStep( 1 );

	const SInt32			kSubTableCount = 3;
	const CNameSubTable*	SubTables[kSubTableCount] =
	{
		inNameTable->GetAddressSubTable(),
		inNameTable->GetProtocolSubTable(),
		inNameTable->GetPortSubTable()
	};

	// Write the utf-8 BOM at the beginning of the file
	// For each sub-table. However, don't do this for CSV (MS Excel can't handle it)

	if ( !bUsingCSV )
	{
		unsigned char szBOM[] = { 0xEF, 0xBB, 0xBF };
		outNameTableStream.WriteBytes( szBOM, 3 );
	}

	for ( SInt32 i = 0; i < kSubTableCount; i++ )
	{
		ASSERT( SubTables[i] != NULL );
		if ( SubTables[i] == NULL ) continue;

		// Get each name table entry.
		const SInt32	nNames = SubTables[i]->GetCount();
		for ( SInt32 x = 0; x < nNames; x++ )
		{
			// Update the progress.
			//theProgress.StepIt( true );
			//if ( (x % 100) == 0 )
			//{
			//	if ( theProgress.CheckForCancel() )
			//	{
			//		break;
			//	}
			//}

			SNameTableEntry		theEntry;
			if ( !SubTables[i]->IndexToEntry( x, theEntry ) ) continue;

			// Skip hidden and/or deleted entries.
			if ( (theEntry.fFlags & kNameTableFlag_Deleted) != 0 ) continue;
			if ( (theEntry.fFlags & kNameTableFlag_Hidden) != 0 ) continue;

			// Build the output string.
			CFixedStringT<CString,1024>	theRecord;
			if ( !NameTableExportTextRow( SubTables[i], theEntry, inSeparator,
				theRecord, pColumns, nColumns ) )
			{
				continue;
			}

			if ( !bUsingCSV )
			{
				
				//// Convert the text to UTF-8.
				outNameTableStream.Write( (LPCSTR) HeLib::CW2UTF8((LPCTSTR) theRecord ) );
			}
			else
			{
				// Use ASCII
				outNameTableStream.Write( (LPCSTR) HeLib::CW2A((LPCTSTR) theRecord ) );
			}
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		NameTableExportSelectedText
// -----------------------------------------------------------------------------

bool
NameTableExportSelectedText(
	const CNameSubTable*	inSubTable,
	UInt32*					inEntryArray,
	UInt32					inEntryCount,
	CByteStream&			outNameTableStream,
	TCHAR					inSeparator )
{
	// Sanity check.
	ASSERT( inEntryArray != NULL );
	if ( inEntryArray == NULL ) return false;

	const bool		bUsingCSV = (_T(',') == inSeparator) ? true : false;

	// Set up progress indicator.
	//CProgressIndicator	theProgress( AfxGetMainWnd(), IDD_PROGRESS_CANCEL,
	//						LoadResourceString( IDS_PROGRESS_EXPORTING_NAMES ) );
	//theProgress.SetRange( 1, inEntryCount );
	//theProgress.SetStep( 1 );

	// Write the utf-8 BOM at the beginning of the file.
	// However, don't do this for CSV (MS Excel can't handle it)
	if ( !bUsingCSV )
	{
		unsigned char szBOM[] = { 0xEF, 0xBB, 0xBF };
		outNameTableStream.WriteBytes( szBOM, 3 );
	}

	// Get each name table entry.
	for ( UInt32 x = 0; x < inEntryCount; x++ )
	{
		// Update the progress.
		//theProgress.StepIt( true );
		//if ( theProgress.CheckForCancel() )
		//{
		//	break;
		//}

		SNameTableEntry	theEntry;
		memset( &theEntry, 0, sizeof( theEntry ) );
		bool	bHaveEntry = inSubTable->IndexToEntry( inEntryArray[x], theEntry );

		if ( bHaveEntry )
		{
			// Skip hidden and/or deleted entries.
			if ( (theEntry.fFlags & kNameTableFlag_Deleted) != 0 ) continue;
			if ( (theEntry.fFlags & kNameTableFlag_Hidden) != 0 ) continue;

			// Build the output string.
			CFixedStringT<CString,1024>	theRecord;
			if ( !NameTableExportTextRow( inSubTable, theEntry,
				inSeparator, theRecord, NULL, 0 ) )
			{
				continue;
			}

			if ( !bUsingCSV )
			{
				
				// Convert the text to UTF-8.
				outNameTableStream.Write( (LPCSTR) HeLib::CW2UTF8((LPCTSTR) theRecord ) );
			}
			else
			{
				// Use ASCII
				outNameTableStream.Write( (LPCSTR) HeLib::CW2A((LPCTSTR) theRecord ) );
			}
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		NameTableExportXMLFormat
// -----------------------------------------------------------------------------

static void
NameTableExportXMLFormat(
	const CNameSubTable*    pSubTable,
	XXMLElement*	        pNameTable,
	SNameTableEntry&		theEntry )
{
	// Create NameTableEntry element.
	XXMLElement*	pNewEntry = new XXMLElement( s_pCurrentVersionKeys->f_pszElementName_Entry );
	ASSERT( pNewEntry != NULL );

	// Name.
	pNewEntry->AddChild( s_pCurrentVersionKeys->f_pszElementName_Name, theEntry.fName );

	// Entry.
	TCHAR	entryData[256];
	if ( !UMediaSpecUtils::SpecToString( theEntry.fSpec, entryData ) )
	{
		return;
	}

	switch ( theEntry.fSpec.GetClass() )
	{
		case kMediaSpecClass_Address:
		{
			// Entry Class = Address.
			pNewEntry->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_Class,
				s_pCurrentVersionKeys->f_pszElementName_Address );

			// Create Address element.
			XXMLElement*	pAddr = new XXMLElement( s_pCurrentVersionKeys->f_pszElementName_Address );
			ASSERT( pAddr != NULL );

			// Add value to address element.
			pAddr->AddValue( entryData );

			// Add type attribute to address element.
			const TCHAR*	pTempName = GetMediaSpecName( theEntry.fSpec.GetType() );
			if ( pTempName == NULL ) return;	// Invalid!
			pAddr->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_Type, pTempName );

			// Add node type attribute to address element.
			if ( theEntry.fType != kNameEntryType_Unknown )
			{
				pTempName = GetNodeTypeName( theEntry.fType );
				if ( pTempName != NULL )
				{
					pAddr->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_NodeType, pTempName );
				}
			}

			// Resolve type.
			if ( (theEntry.fSource & kNameEntrySource_ResolveMask) != 0 )
			{
				pTempName = GetResolveTypeName( (UInt8)(theEntry.fSource & kNameEntrySource_ResolveMask) );
				if ( pTempName != NULL )
				{
					pAddr->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_ResolveType, pTempName );
				}
			}

			// Source type.
			if ( (theEntry.fSource & kNameEntrySource_TypeMask) != 0 )
			{
				pTempName = GetSourceTypeName( (UInt8)(theEntry.fSource & kNameEntrySource_TypeMask) );
				if ( pTempName != NULL )
				{
					pAddr->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_SourceType, pTempName );
				}
			}

			// Add address element to NameTableEntry element.
			pNewEntry->AddChild( pAddr );
		}
		break;

		case kMediaSpecClass_Protocol:
		{
			// Entry Class = Protocol.
			pNewEntry->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_Class,
				s_pCurrentVersionKeys->f_pszElementName_Protocol );

			// Create Protocol element.
			XXMLElement*	pProto = new XXMLElement(
				s_pCurrentVersionKeys->f_pszElementName_Protocol );
			ASSERT( pProto != NULL );

			// Add value to Protocol element.
			pProto->AddValue( entryData );

			// Add Type attribute to Protocol element.
			const TCHAR*	pTempName = GetMediaSpecName( theEntry.fSpec.GetType() );
			if ( pTempName == NULL ) return;	// Invalid.
			pProto->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_Type, pTempName );

			// Add Protocol element to NameTableEntry element.
			pNewEntry->AddChild( pProto );
		}
		break;

		case kMediaSpecClass_Port:
		{
			// Entry Class = Port.
			pNewEntry->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_Class,
				s_pCurrentVersionKeys->f_pszElementName_Port );

			// Create Port element.
			XXMLElement*	pPort = new XXMLElement( s_pCurrentVersionKeys->f_pszElementName_Port );
			ASSERT( pPort != NULL );

			// Add value to Port element.
			pPort->AddValue( entryData );

			// Add type attribute to Port element.
			const TCHAR*	pTempName = GetMediaSpecName( theEntry.fSpec.GetType() );
			if ( pTempName == NULL ) return;	// Invalid!
			pPort->AddAttribute( s_pCurrentVersionKeys->f_pszAttributeName_Type, pTempName );

			// Add Port element to NameTableEntry element.
			pNewEntry->AddChild( pPort );
		}
		break;

		default:
		{
			// +++ Error!
		}
		break;
	}

	// Color.
	if ( theEntry.fColor != RGB(0,0,0) )	// Color is not black.
	{
		TCHAR	szColor[256];
		_stprintf( szColor, _T("#%02X%02X%02X"), GetRValue(theEntry.fColor),
			GetGValue(theEntry.fColor), GetBValue(theEntry.fColor) );
		pNewEntry->AddChild( s_pCurrentVersionKeys->f_pszElementName_Color, szColor );
	}

	// Group.
	if ( theEntry.fGroup != kGroupID_None )
	{
		TCHAR	szGroup[256];
		pSubTable->GetNthGroupName( theEntry.fGroup, szGroup );
		if ( _tcslen( szGroup ) )
		{
			pNewEntry->AddChild( s_pCurrentVersionKeys->f_pszElementName_Group, szGroup );
		}
	}

	// trust
	if ( theEntry.fTrust != kNameTrust_Unknown )
	{
		const TCHAR*	pszTrust = GetTrustTypeName( theEntry.fTrust );
		pNewEntry->AddChild( s_pCurrentVersionKeys->f_pszElementName_Trust, pszTrust );
	}

	// Modified date.
	if ( theEntry.fDateModified != 0 )
	{
		struct tm*		pTime = gmtime( &theEntry.fDateModified );
		if ( pTime != NULL )
		{
			// Time is ISO 8601 format: CCYY-MM-DDThh:mm:ss.sssZ
			// C = century; Y = year; M = month, D = day,
			// h = hour, m = minute, s = second. Z indicates UTC.
			TCHAR	szDate[256];
			_stprintf( szDate, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ"),
				pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
				pTime->tm_hour, pTime->tm_min, pTime->tm_sec );

			pNewEntry->AddChild( s_pCurrentVersionKeys->f_pszElementName_ModDate, szDate );
		}
	}

	// Last used date.
	if ( theEntry.fDateLastUsed != 0 )
	{
		struct tm*		pTime = gmtime( &theEntry.fDateLastUsed );
		if ( pTime != NULL )
		{
			TCHAR	szDate[256];
			_stprintf( szDate, _T("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ"),
				pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday,
				pTime->tm_hour, pTime->tm_min, pTime->tm_sec );

			pNewEntry->AddChild( s_pCurrentVersionKeys->f_pszElementName_UsedDate, szDate );
		}
	}

	// Add NameTableEntry element to NameTable element.
	pNameTable->AddChild( pNewEntry );
}


// -----------------------------------------------------------------------------
//		NameTableExportXML
// -----------------------------------------------------------------------------

bool
NameTableExportXML(
	const CNameTable*	inNameTable,
	CByteStream&		outNameTableStream )
{
	// Sanity check.
	ASSERT( inNameTable != NULL );
	if ( inNameTable == NULL ) return false;

	// Figure out how many names to export.
	const UInt32	nNamesToExport = inNameTable->GetCount();

	// Set up progress indicator.
	//CProgressIndicator	theProgress( AfxGetMainWnd(), IDD_PROGRESS_CANCEL,
	//						LoadResourceString( IDS_PROGRESS_EXPORTING_NAMES ) );
	//theProgress.SetRange( 1, nNamesToExport );
	//theProgress.SetStep( 1 );

	// Only way to export is XML.
	XXMLDoc	theDoc;

	try
	{
		// Create root element (NameTable).
		XXMLElement*	pNameTable = new XXMLElement( kElementName_Root );
		ASSERT( pNameTable != NULL );

		// Version.
		pNameTable->AddAttribute( kAttributeName_Version,
			s_pCurrentVersionKeys->f_pszNameTableVersion );

		const SInt32			kSubTableCount = 3;
		const CNameSubTable*	SubTables[kSubTableCount] =
		{
			inNameTable->GetAddressSubTable(),
			inNameTable->GetProtocolSubTable(),
			inNameTable->GetPortSubTable()
		};

		// For each sub-table...
		for ( SInt32 i = 0; i < kSubTableCount; i++ )
		{
			ASSERT( SubTables[i] != NULL );
			if ( SubTables[i] == NULL ) continue;

			SNameTableEntry		theEntry;

			// Get each name table entry.
			SInt32	nNames = SubTables[i]->GetCount();
			for ( SInt32 x = 0; x < nNames; x++ )
			{
				// Update the progress.
				//theProgress.StepIt( true );
				//if ( theProgress.CheckForCancel() )
				//{
				//	break;
				//}

				if ( !SubTables[i]->IndexToEntry( x, theEntry ) ) continue;

				// Skip hidden and/or deleted entries.
				if ( (theEntry.fFlags & kNameTableFlag_Deleted) != 0 ) continue;
				if ( (theEntry.fFlags & kNameTableFlag_Hidden) != 0 ) continue;

				NameTableExportXMLFormat( SubTables[i], pNameTable, theEntry );
			}
		}

		// Set NameTable element as the root element.
		theDoc.SetRootElement( pNameTable );

		// Build doc into ByteStream.
		theDoc.BuildStream( outNameTableStream );
	}
	catch ( ... )
	{
		// +++ Handle error here!
	}

	return true;
}


// -----------------------------------------------------------------------------
//		NameTableExportSelectedXML
// -----------------------------------------------------------------------------

bool
NameTableExportSelectedXML(
	const CNameSubTable*	inSubTable,
	UInt32*					inEntryArray,
	UInt32					inEntryCount,
	CByteStream&			outNameTableStream )
{
	// Sanity check.
	ASSERT( inEntryArray != NULL );
	if ( inEntryArray == NULL ) return false;

	// Set up progress indicator.
	//CProgressIndicator	theProgress( AfxGetMainWnd(), IDD_PROGRESS_CANCEL,
	//						LoadResourceString( IDS_PROGRESS_EXPORTING_NAMES ) );
	//theProgress.SetRange( 1, inEntryCount );
	//theProgress.SetStep( 1 );

	// Only way to export is XML.
	XXMLDoc	theDoc;

	try
	{
		// Create root element (NameTable).
		XXMLElement*	pNameTable = new XXMLElement( kElementName_Root );
		ASSERT( pNameTable != NULL );

		// Version.
		pNameTable->AddAttribute( kAttributeName_Version,
			s_pCurrentVersionKeys->f_pszNameTableVersion );

		for ( UInt32 x = 0; x < inEntryCount; x++ )
		{
			// Update the progress.
			//theProgress.StepIt( true );
			//if ( theProgress.CheckForCancel() )
			//{
			//	break;
			//}

			SNameTableEntry	theEntry;
			memset( &theEntry, 0, sizeof( theEntry ) );
			bool	bHaveEntry = inSubTable->IndexToEntry( inEntryArray[x], theEntry );

			if ( bHaveEntry )
			{
				// Skip hidden and/or deleted entries.
				if ( (theEntry.fFlags & kNameTableFlag_Deleted) != 0 ) continue;
				if ( (theEntry.fFlags & kNameTableFlag_Hidden) != 0 ) continue;

				NameTableExportXMLFormat( inSubTable, pNameTable, theEntry );
			}
		}

		// Set NameTable element as the root element.
		theDoc.SetRootElement( pNameTable );

		// Build doc into ByteStream.
		theDoc.BuildStream( outNameTableStream );
	}
	catch ( ... )
	{
		// +++ Handle error here!
	}

	return true;
}


// -----------------------------------------------------------------------------
//		NameTableImportXML
// -----------------------------------------------------------------------------

#define NTP_SKIP	\
	{		TRACE(_T("Skipping @ line %d\n"), __LINE__);\
			while (iter.NextRoot(eleSub, &eleClass))	\
				eleSub.Reset();							\
			VERIFY(iter.EndElement(&eleClass));			\
			eleClass.Reset();							\
			continue;									\
	}

bool
NameTableImportXML(
	const CByteStream&	inNameTableStream,
	CNameTable*			inNameTable,
	UInt32&				outValidNames,
	UInt32&				outImported,
	UInt32&				outSkipped )
{
	outValidNames = 0;
	outImported   = 0;
	outSkipped    = 0;

	// Sanity check.
	ASSERT ( inNameTable != NULL );
	if ( inNameTable == NULL ) return false;

	// Get timezone bias.
	// We're going to use mktime, which does localtime.
	// Need to adjust for time zone bias.
	const SInt32	nTimezoneDelta = -_timezone + (_daylight * 3600);

	inNameTable->StopBroadcasting();

	// "routine" to skip when we hit an error.  this means
	// go to the next root item
	try
	{
		XXMLParseIterator	iter;
		XXMLElement			eleRoot, eleClass, eleSub;

		VERIFY(iter.SetDataSource(inNameTableStream));

		VERIFY(iter.InitParser());
		VERIFY(iter.NextChild(eleRoot, NULL));

		// parse the version stuff
		const NameTableXMLKeys*	pCurrentKeys = NULL;
		PGETRSOLVETYPEFUNC		pGetResolveType = NULL;

		XString	strVersion;
		if ( eleRoot.GetNamedAttributeValue( kAttributeName_Version, strVersion ) )
		{
			if ( strVersion == s_Version1Keys.f_pszNameTableVersion )
			{
				// Version 1.
				pCurrentKeys = &s_Version1Keys;
				pGetResolveType = &GetResolveTypeVersion2;
			}
			else if ( strVersion == s_Version2Keys.f_pszNameTableVersion )
			{
				// Version 2.
				pCurrentKeys = &s_Version2Keys;
				pGetResolveType = GetResolveTypeVersion2;
			}
			else if ( strVersion == s_Version3Keys.f_pszNameTableVersion )
			{
				// Version 3.
				pCurrentKeys = &s_Version3Keys;
				pGetResolveType = GetResolveTypeVersion3;
			}
		}

		if ( pCurrentKeys == NULL || pGetResolveType == NULL)
		{
			throw -2;	// Unknown version!
		}

		// Count the number of bytes in the file (divide by 8)
		UInt32 nEntries = inNameTableStream.GetLength() / 8;

		// Set up progress indicator.
		//CProgressIndicator	theProgress( AfxGetMainWnd(), IDD_PROGRESS_CANCEL,
		//						LoadResourceString( IDS_PROGRESS_IMPORTING_NAMES ) );
		//theProgress.SetRange( 1, nEntries );

		const time_t	curTime = time( NULL );

		while (iter.NextChild(eleClass, &eleRoot))
		{
			//theProgress.SetPos( iter.GetPosition() / 8 );
			//if ( theProgress.CheckForCancel() )
			//{
			//	break;
			//}

			// Initialize default values.
			SNameTableEntry	newEntry;
			memset( &newEntry, 0, sizeof(newEntry) );
			newEntry.fDateModified	= curTime;
			newEntry.fDateLastUsed	= newEntry.fDateModified;
			newEntry.fType			= kNameEntryType_Unknown;
			newEntry.fSource 		= kNameEntrySource_TypeUnknown | kNameEntrySource_ResolveUnknown;
			newEntry.fFlags 		= 0;
			newEntry.fGroup 		= kGroupID_None;

			bool					bMajorType = false;
			bool					bName = false;
			SInt32					nClassType = -1;
			TMediaSpecType			nType = kMediaSpecType_Null;

			LPCTSTR	strName = eleClass.GetName();
			if (_tcscmp(strName, pCurrentKeys->f_pszElementName_Entry) != 0)
			{	// skip it all
				NTP_SKIP;
			}

			XString strClass;
			if (!eleClass.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_Class, strClass ))
			{	// skip it all
				NTP_SKIP;
			}

			if (strClass == pCurrentKeys->f_pszElementName_Address)
			{
				nClassType = 0;
			}
			else if (strClass == pCurrentKeys->f_pszElementName_Protocol)
			{
				nClassType = 1;
			}
			else if (strClass == pCurrentKeys->f_pszElementName_Port)
			{
				nClassType = 2;
			}
			else
			{	// skip it all
				NTP_SKIP;
			}

// continue includes reseting the element we use to get the next bit of data
#define NTP_CONTINUE	eleSub.Reset();  continue;

			XString strEntry;
			XString strType;
			XString strGroup;
			XString strTrust;

			// parse the entire sub element and all its children
			while (iter.NextRoot(eleSub, &eleClass))
			{
				XString strSubName = eleSub.GetName();

				if (!bMajorType)
				{
					switch (nClassType)
					{
					case 0:	// addr
						if (strSubName == pCurrentKeys->f_pszElementName_Address)
						{	// get attribs for address
							bMajorType = true;

							// Address class.
							strEntry = eleSub.GetValue();
							if ( !eleSub.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_Type, strType ) )
							{
								continue;	// Invalid if no Type attribute.
							}

							if ( !GetMediaSpecType( strType, nType ) )
							{
								continue;	// Must be an invalid Type.
							}

							// Node type.
							XString	strNodeType;
							if ( eleSub.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_NodeType, strNodeType ) )
							{
								if ( !GetNodeType( strNodeType, newEntry.fType ) )
								{
									newEntry.fType = kNameEntryType_Unknown;
								}
							}

							// Resolve type.
							XString strResolveType;
							if ( eleSub.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_ResolveType , strResolveType ) )
							{
								UInt8	nResolve;
								if ( (*pGetResolveType) ( strResolveType, nResolve ) )
								{
									newEntry.fSource |= nResolve;
								}
								else
								{
									newEntry.fSource |= kNameEntrySource_ResolveUnknown;
								}
							}

							// Source type.
							XString	strSourceType;
							if ( eleSub.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_SourceType , strSourceType ) )
							{
								UInt8	nSource;
								if ( GetSourceType( strSourceType, nSource ) )
								{
									newEntry.fSource |= nSource;
								}
								else
								{
									newEntry.fSource |= kNameEntrySource_TypeUnknown;
								}
							}

							NTP_CONTINUE;
						}
						break;
					case 1: // protocol
						if (strSubName == pCurrentKeys->f_pszElementName_Protocol)
						{	// get attribs for protocol
							bMajorType = true;
							strEntry = eleSub.GetValue();
							if ( !eleSub.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_Type, strType ) )
							{
								continue;	// Invalid if no type attribute.
							}
							if ( !GetMediaSpecType( strType, nType ) )
							{
								continue;	// Invalid type.
							}

							NTP_CONTINUE;
						}
						break;
					case 2: // port
						if (strSubName == pCurrentKeys->f_pszElementName_Port)
						{	// get attribs for port
							bMajorType = true;
							strEntry = eleSub.GetValue();
							if ( !eleSub.GetNamedAttributeValue( pCurrentKeys->f_pszAttributeName_Type, strType ) )
							{
								continue;	// Invalid if no type attribute.
							}
							if ( !GetMediaSpecType( strType, nType ) )
							{
								continue;	// Invalid type.
							}
							NTP_CONTINUE;
						}
						break;
					default:
						break;
					}
				}

				if (!bName && strSubName == pCurrentKeys->f_pszElementName_Name)
				{
					bName = true;
					const XString& strName = eleSub.GetValue();
					// Copy name.
					if ( strName.GetLength() > kMaxNameLength )
					{
						// Truncate name if too long.
						_tcsncpy( newEntry.fName, strName, kMaxNameLength );
						newEntry.fName[kMaxNameLength] = 0;
					}
					else
					{
						_tcscpy( newEntry.fName, strName );
					}
					NTP_CONTINUE;
				}

				if (strSubName == pCurrentKeys->f_pszElementName_Color)
				{
					COLORREF	theColor = RGB(0,0,0);		// Default is black.

					LPCTSTR strColor = eleSub.GetValue();
					// Color is "#xxxxxx", which are hex values for R,G,B.
					TCHAR*	pEnd;
					UInt32	nColor = _tcstoul( &strColor[1], &pEnd, 16 );
					UInt8	nRed = (UInt8)(nColor >> 16);
					UInt8	nGreen = (UInt8)(nColor >> 8);
					UInt8	nBlue = (UInt8) nColor;
					theColor = RGB( nRed, nGreen, nBlue );

					newEntry.fColor = theColor;

					NTP_CONTINUE;
				}

				if (strSubName == pCurrentKeys->f_pszElementName_Group)
				{
					strGroup = eleSub.GetValue();
					// do the lookup later when we have the fSpec
					NTP_CONTINUE;
				}

				if (strSubName == pCurrentKeys->f_pszElementName_UsedDate)
				{
					// Parse date: ISO 9601 format.
					struct tm	dateTime;

					const TCHAR*	pszDate = eleSub.GetValue();

					memset( &dateTime, 0, sizeof(dateTime));
					dateTime.tm_year	= _ttoi( pszDate ) - 1900;
					dateTime.tm_mon		= _ttoi( pszDate + 5 ) - 1;
					dateTime.tm_mday	= _ttoi( pszDate + 8 );
					dateTime.tm_hour	= _ttoi( pszDate + 11 );
					dateTime.tm_min		= _ttoi( pszDate + 14 );
					dateTime.tm_sec		= _ttoi( pszDate + 17 );
					dateTime.tm_isdst	= -1;	// -1 is use sysLib to compute DST or not

					time_t	nSeconds;
					nSeconds = mktime( &dateTime );
					nSeconds += nTimezoneDelta;
					newEntry.fDateLastUsed	= nSeconds;
					NTP_CONTINUE;
				}

				if (strSubName == pCurrentKeys->f_pszElementName_ModDate)
				{	// Parse date: ISO 9601 format.
					struct tm	dateTime;

					const TCHAR*	pszDate = eleSub.GetValue();

					memset( &dateTime, 0, sizeof(dateTime) );
					dateTime.tm_year	= _ttoi( pszDate ) - 1900;
					dateTime.tm_mon		= _ttoi( pszDate + 5 ) - 1;
					dateTime.tm_mday	= _ttoi( pszDate + 8 );
					dateTime.tm_hour	= _ttoi( pszDate + 11 );
					dateTime.tm_min		= _ttoi( pszDate + 14 );
					dateTime.tm_sec		= _ttoi( pszDate + 17 );
					dateTime.tm_isdst	= -1;	// -1 is use sysLib to compute DST or not

					time_t	nSeconds;
					nSeconds = mktime( &dateTime );
					nSeconds += nTimezoneDelta;
					newEntry.fDateModified	= nSeconds;
					NTP_CONTINUE;
				}

				if (strSubName == pCurrentKeys->f_pszElementName_Trust)
				{
					strTrust = eleSub.GetValue();
					NTP_CONTINUE;
				}

				// error
				ASSERT(0);
				eleSub.Reset();
			}

			VERIFY(iter.EndElement(&eleClass));
			eleClass.Reset();

			if ( !(bMajorType && bName) )
			{	// error, these are required
				outSkipped++;
				continue;
			}

			// Make MediaSpec.
			if ( !UMediaSpecUtils::StringToSpec( strEntry, nType, newEntry.fSpec ) )
			{
				// Something went wrong.
				outSkipped++;
				continue;
			}

			if ( !strGroup.IsEmpty() )
			{
				// finish processing the group
				CNameSubTable*	pSubTable = inNameTable->ClassToSubTable( newEntry.fSpec.GetClass() );
				if ( pSubTable != NULL )
				{
					TCHAR	szGroup[256];
					_tcscpy( szGroup, strGroup );
					newEntry.fGroup = pSubTable->GroupNameToIndex( szGroup );
					if ( newEntry.fGroup == kGroupID_None )
					{
						// Group not found... create it!
						newEntry.fGroup = pSubTable->AddGroup( szGroup );
					}
				}
			}

			if ( !strTrust.IsEmpty() )
			{
				if ( !GetTrustType( strTrust, newEntry.fTrust ) )
				{
					newEntry.fTrust = kNameTrust_Unknown;
				}
			}
			else
			{
				newEntry.fTrust = kNameTrust_Unknown;
			}

			// At this point, we can assume the entry as valid.
			outValidNames++;

			// Add new name entry to array.
			if ( inNameTable->AddEntry( newEntry,
				CNameTable::kResolveOption_AddrMatchSkip |
				CNameTable::kResolveOption_NoMatchAdd |
				CNameTable::kResolveOption_NameMatchAdd ) )
			{
				outImported++;
			}
			else
			{
				outSkipped++;
			}
		}
	}
	catch ( ... )
	{
		// +++ Handle error!
	}

	inNameTable->StartBroadcasting();
	inNameTable->NamesImported();

	return true;
}


// -----------------------------------------------------------------------------
//		NameTableImportText
// -----------------------------------------------------------------------------

bool
NameTableImportText(
	const CByteStream&	inNameTableStream,
	CNameTable*			inNameTable,
	UInt32&				outValidNames,
	UInt32&				outImported,
	UInt32&				outSkipped )
{
	typedef CFixedStringT<CString,64> String;

	outValidNames = 0;
	outImported   = 0;
	outSkipped    = 0;

	ASSERT( inNameTable != NULL );
	if ( inNameTable == NULL ) return false;

	SInt32	nStreamLen = inNameTableStream.GetLength();
	if ( inNameTableStream.GetData() == NULL || nStreamLen == 0 )
	{
		return false;
	}

	// Get timezone bias.
	// We're going to use mktime, which does localtime.
	// Need to adjust for time zone bias.
	const SInt32	nTimezoneDelta = -_timezone + (_daylight * 3600);

	// To keep name table update messages from going crazy.
	inNameTable->StopBroadcasting();

	// Set up progress indicator.
	//CProgressIndicator	theProgress( AfxGetMainWnd(), IDD_PROGRESS_CANCEL,
	//						LoadResourceString( IDS_PROGRESS_IMPORTING_NAMES ) );

	// Figure out if tab- or comma-delimited.
	TCHAR	cDelimiter;
	bool	bIsASCII;
	CalcFieldDelimiter( inNameTableStream, &cDelimiter, &bIsASCII );
	if ( cDelimiter == 0 )
	{
		return false;
	}

	//theProgress.SetRange( 1, nStreamLen );

	const time_t curTime = time( NULL );

	char*	pszBuff = NULL;

#define IS_EOL(x) ( ((x) == _T('\r')) || ((x) == _T('\n')) || ((x) == 0) )

	try
	{
		// Create a buffer of bytes and null terminate it.
		pszBuff = (char*) malloc( nStreamLen+1 );
		if ( pszBuff == NULL )
		{
			inNameTable->StartBroadcasting();
			return false;
		}
		memcpy( pszBuff, inNameTableStream.GetData(), nStreamLen );
		pszBuff[nStreamLen] = 0;

		// Now do UTF and ASCII conversions.
		// Made this slightly more efficient by building either the ASCII or
		// UTF buffers but not both.
		// The auto_ptr's need to be in the same scope as pData otherwise the data
		// Could be free'd before processing is finished.
		std::auto_ptr< CUTF82T > pUtfData(0);
		std::auto_ptr<CA2T> pAsciiData(0);
		TCHAR*	pData = NULL;

		if ( bIsASCII )
		{
			pAsciiData.reset(new CA2T(pszBuff));
			pData = (TCHAR*) (*(pAsciiData.get()));
		}
		else
		{
			pUtfData.reset(new CUTF82T(pszBuff + 3));
			pData = (TCHAR*) ((*pUtfData.get()));
		}

		free( pszBuff );
		pszBuff = NULL;

		// Adjust the string length
		nStreamLen = static_cast<SInt32>( _tcslen( pData ) );

		TCHAR*	pDataBegin = pData;
		TCHAR*	pDataEnd = pData + nStreamLen;

		while ( pData < pDataEnd )
		{
			// Update progress dialog.
			//if ( (outValidNames % 100) == 0 )
			//{
			//	theProgress.SetPos( static_cast<int>(pData - pDataBegin) );
			//	if ( theProgress.CheckForCancel() )
			//	{
			//		break;
			//	}
			//}

			// Make new name table entry.
			SNameTableEntry	newEntry;
			memset( &newEntry, 0, sizeof(newEntry) );

			// Eat endline characters.
			while ( IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			if ( pData >= pDataEnd ) break;

			// Eat comment lines.
			while ( *pData == _T('#') )
			{
				pData++;	// Skip past #.
				while ( !IS_EOL( *pData ) && (pData < pDataEnd) )
				{
					pData++;
				}
				pData++;	// Skip past cr/lf.
			}
			if ( pData >= pDataEnd ) break;

			// Eat endline characters.
			while ( IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			if ( pData >= pDataEnd ) break;

			// Read name first.
			TCHAR*	psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strName( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strName.Trim( _T('\"') );
			}

			// Now read entry type.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strType( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strType.Trim( _T('\"') );
			}

			// Now read entry data.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strData( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strData.Trim( _T('\"') );
			}

			// Color.
			// Apparently some old Mac name table files may contain an extra
			// delimiter here, tbfy.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) )
			{
				pData++;
			}
			String	strColor( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strColor.Trim( _T('\"') );
			}

			// Group.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strGroup( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strGroup.Trim( _T('\"') );
			}

			// Modified date.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strModDate( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strModDate.Trim( _T('\"') );
			}
			if( strModDate[0] == _T('#') )
			{
				strModDate = _T("");
			}

			// Last used date.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strLastUsedDate( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strLastUsedDate.Trim( _T('\"') );
			}

			// Node type.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strNodeType( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strNodeType.Trim( _T('\"') );
			}

			// Resolve type.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strResolveType( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strResolveType.Trim( _T('\"') );
			}

			// Resolve source.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strResolveSource( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strResolveSource.Trim( _T('\"') );
			}

			// Trust.
			psz = pData;
			while ( (*pData != cDelimiter) && !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}
			String	strTrust( psz, static_cast<int>(pData - psz) );
			if ( *pData == cDelimiter ) pData++;	// Skip delimiter.
			if ( cDelimiter == _T(',') )
			{
				strTrust.Trim( _T('\"') );
			}

			// Skip to end of line.
			while ( !IS_EOL( *pData ) && (pData < pDataEnd) )
			{
				pData++;
			}

			// Make MediaSpec, based on type.
			TMediaSpecType	nType = kMediaSpecType_Null;
			if ( !GetMediaSpecType( strType, nType ) )
			{
				continue;		// Invalid media spec type!
			}

			// Make MediaSpec.
			if ( !UMediaSpecUtils::StringToSpec( strData, nType, newEntry.fSpec ) )
			{
				// Something went wrong.
				continue;
			}

			// Copy name.
			_tcscpy( newEntry.fName, strName );

			// Do color.
			if ( strColor.GetLength() > 0 )
			{
				if ( (strColor.GetLength() == 6) || (strColor.GetLength() == 7) )
				{
					// Color is "#XXXXXX", which are hex values for R,G,B.
					LPCTSTR	pszColor = strColor;
					if ( !_istxdigit( pszColor[0] ) ) pszColor++;
					UInt32	nColor = _tcstoul( pszColor, NULL, 16 );
					UInt8	nRed   = (UInt8)(nColor >> 16);
					UInt8	nGreen = (UInt8)(nColor >> 8);
					UInt8	nBlue  = (UInt8) nColor;
					newEntry.fColor = RGB( nRed, nGreen, nBlue );
				}
				else
				{
					int	nColor = _ttoi( strColor );
					if( nColor == 0 )
					{
						newEntry.fColor = CNameTable::GenerateAutoColor( newEntry.fSpec );
					}
					else
					{
						newEntry.fColor = GetEP3xIndexedColor( (UInt8) nColor );
					}
					
				}
			}

			newEntry.fGroup = kGroupID_None;
			if ( strGroup.GetLength() > 0 )
			{
				CNameSubTable*	pSubTable = inNameTable->ClassToSubTable( newEntry.fSpec.GetClass() );
				if ( pSubTable != NULL )
				{
					newEntry.fGroup = pSubTable->GroupNameToIndex( strGroup );
					if ( newEntry.fGroup == kGroupID_None )
					{
						// Group not found... create it!
						newEntry.fGroup = pSubTable->AddGroup( strGroup );
					}
				}
			}

			newEntry.fDateModified = curTime;
			if ( strModDate.GetLength() > 0 )
			{
				// Parse date: ISO 9601 format.
				const TCHAR*	pszDate = strModDate;
				struct tm		dateTime;
				memset( &dateTime, 0, sizeof(dateTime) );
				dateTime.tm_year  = _ttoi( pszDate ) - 1900;
				dateTime.tm_mon   = _ttoi( pszDate + 5 ) - 1;
				dateTime.tm_mday  = _ttoi( pszDate + 8 );
				dateTime.tm_hour  = _ttoi( pszDate + 11 );
				dateTime.tm_min   = _ttoi( pszDate + 14 );
				dateTime.tm_sec   = _ttoi( pszDate + 17 );
				dateTime.tm_isdst = -1;	// -1 is use sysLib to compute DST or not

				newEntry.fDateModified = mktime( &dateTime ) + nTimezoneDelta;
			}

			newEntry.fDateLastUsed = newEntry.fDateModified;
			if ( strLastUsedDate.GetLength() > 0 )
			{
				// Parse date: ISO 9601 format.
				const TCHAR*	pszDate = strModDate;
				struct tm		dateTime;
				memset( &dateTime, 0, sizeof(dateTime) );
				dateTime.tm_year  = _ttoi( pszDate ) - 1900;
				dateTime.tm_mon   = _ttoi( pszDate + 5 ) - 1;
				dateTime.tm_mday  = _ttoi( pszDate + 8 );
				dateTime.tm_hour  = _ttoi( pszDate + 11 );
				dateTime.tm_min   = _ttoi( pszDate + 14 );
				dateTime.tm_sec   = _ttoi( pszDate + 17 );
				dateTime.tm_isdst = -1;	// -1 is use sysLib to compute DST or not

				newEntry.fDateLastUsed =  mktime( &dateTime ) + nTimezoneDelta;
			}

			if ( strNodeType.GetLength() > 0 )
			{
				if ( !GetNodeType( strNodeType, newEntry.fType ) )
				{
					newEntry.fType = kNameEntryType_Unknown;
				}
			}
			else
			{
				newEntry.fType = kNameEntryType_Unknown;
			}

			newEntry.fSource = 0;

			if ( strResolveType.GetLength() > 0 )
			{
				UInt8	nResolveType;
				if ( GetResolveTypeVersion3( strResolveType, nResolveType ) )
				{
					newEntry.fSource |= nResolveType;
				}
				else
				{
					newEntry.fSource |= kNameEntrySource_TypeUnknown;
				}
			}
			else
			{
				newEntry.fSource |= kNameEntrySource_TypeUnknown;
			}

			if ( strResolveSource.GetLength() > 0 )
			{
				UInt8	nResolveSource;
				if ( GetSourceType( strResolveSource, nResolveSource ) )
				{
					newEntry.fSource |= nResolveSource;
				}
				else
				{
					newEntry.fSource |= kNameEntrySource_ResolveUnknown;
				}
			}
			else
			{
				newEntry.fSource |= kNameEntrySource_ResolveUnknown;
			}

			if ( strTrust.GetLength() > 0 )
			{
				if ( !GetTrustType( strTrust, newEntry.fTrust ) )
				{
					newEntry.fTrust = kNameTrust_Unknown;
				}
			}
			else
			{
				newEntry.fTrust = kNameTrust_Unknown;
			}

			// At this point, we can assume the entry was valid.
			outValidNames++;

			if ( inNameTable->AddEntry( newEntry,
				CNameTable::kResolveOption_AddrMatchReplace |
				CNameTable::kResolveOption_NoMatchAdd |
				CNameTable::kResolveOption_NameMatchAdd ) )
			{
				outImported++;
			}
			else
			{
				outSkipped++;
			}
		}
	}
	catch ( ... )
	{
		// +++ Do something here.
		if(pszBuff != NULL)
			free (pszBuff);
	}

	inNameTable->StartBroadcasting();
	inNameTable->NamesImported();

	return true;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecName
// -----------------------------------------------------------------------------
// Linear search through MediaSpec map for ID, return name if found.

const TCHAR*
GetMediaSpecName(
	TMediaSpecType		inType )
{
	for ( SInt32 x = 0; x < COUNTOF(s_MediaSpecNames); x++ )
	{
		if ( inType == static_cast<TMediaSpecType>(s_MediaSpecNames[x].f_nID) )
		{
			return s_MediaSpecNames[x].f_pszName;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetNodeTypeName
// -----------------------------------------------------------------------------
// Linear search through NodeType map for ID, return name if found.

const TCHAR*
GetNodeTypeName(
	UInt8	inType )
{
	for ( SInt32 x = 0; x < COUNTOF(s_NodeTypeNames); x++ )
	{
		if ( s_NodeTypeNames[x].f_nID == inType )
		{
			return s_NodeTypeNames[x].f_pszName;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetResolveTypeName
// -----------------------------------------------------------------------------
// Linear search through ResolveType map for ID, return name if found.

const TCHAR*
GetResolveTypeName(
	UInt8	inType )
{
	for ( SInt32 x = 0; x < COUNTOF(s_ResolveTypeNamesv3); x++ )
	{
		if ( s_ResolveTypeNamesv3[x].f_nID == inType )
		{
			return s_ResolveTypeNamesv3[x].f_pszName;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetTrustTypeName
// -----------------------------------------------------------------------------
// Linear search through Trust map for ID, return name if found.

const TCHAR*
GetTrustTypeName(
	UInt8	inTrust )
{
	for ( SInt32 x = 0; x < COUNTOF(s_TrustNames); x++ )
	{
		if ( s_TrustNames[x].f_nID == inTrust )
		{
			return s_TrustNames[x].f_pszName;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetSourceTypeName
// -----------------------------------------------------------------------------
// Linear search through SourceType map for ID, return name if found.

const TCHAR*
GetSourceTypeName(
	UInt8	inType )
{
	for ( SInt32 x = 0; x < COUNTOF(s_SourceTypeNames); x++ )
	{
		if ( s_SourceTypeNames[x].f_nID == inType )
		{
			return s_SourceTypeNames[x].f_pszName;
		}
	}

	return NULL;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecType
// -----------------------------------------------------------------------------
// Linear search through MediaSpec map for name, return type if found.

bool
GetMediaSpecType(
	const TCHAR*	inName,
	TMediaSpecType&	outType )
{
	// Sanity check.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	for ( SInt32 x = 0; x < COUNTOF(s_MediaSpecNames); x++ )
	{
		if ( _tcsicmp( s_MediaSpecNames[x].f_pszName, inName ) == 0 )
		{
			outType = static_cast<TMediaSpecType>(s_MediaSpecNames[x].f_nID);
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetNodeType
// -----------------------------------------------------------------------------
// Linear search through NodeType map for name, return type if found.

bool
GetNodeType(
	const TCHAR*	inName,
	UInt8&			outType )
{
	// Sanity checks.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	for ( SInt32 x = 0; x < COUNTOF(s_NodeTypeNames); x++ )
	{
		if ( _tcsicmp( s_NodeTypeNames[x].f_pszName, inName ) == 0 )
		{
			outType = (UInt8) s_NodeTypeNames[x].f_nID;
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetResolveTypeVersion2
// -----------------------------------------------------------------------------
// Linear search through ResolveType map for name, return type if found.

bool
GetResolveTypeVersion2(
	const TCHAR*	inName,
	UInt8&			outType)
{
	// Sanity checks.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	for ( SInt32 x = 0; x < COUNTOF(s_ResolveTypeNamesv2); x++ )
	{
		if ( _tcsicmp( s_ResolveTypeNamesv2[x].f_pszName, inName ) == 0 )
		{
			outType = (UInt8) s_ResolveTypeNamesv2[x].f_nID;
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetResolveTypeVersion3
// -----------------------------------------------------------------------------
// Linear search through ResolveType map for name, return type if found.

bool
GetResolveTypeVersion3(
	const TCHAR*	inName,
	UInt8&			outType)
{
	// Sanity checks.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	for ( SInt32 x = 0; x < COUNTOF(s_ResolveTypeNamesv3); x++ )
	{
		if ( _tcsicmp( s_ResolveTypeNamesv3[x].f_pszName, inName ) == 0 )
		{
			outType = (UInt8) s_ResolveTypeNamesv3[x].f_nID;
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetSourceType
// -----------------------------------------------------------------------------
// Linear search through SourceType map for name, return type if found.

bool
GetSourceType(
	const TCHAR*	inName,
	UInt8&			outType )
{
	// Sanity checks.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	for ( SInt32 x = 0; x < COUNTOF(s_SourceTypeNames); x++ )
	{
		if ( _tcsicmp( s_SourceTypeNames[x].f_pszName, inName ) == 0 )
		{
			outType = (UInt8) s_SourceTypeNames[x].f_nID;
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		GetTrustType
// -----------------------------------------------------------------------------
// Linear search through s_TrustNames map for name, return type if found.

bool
GetTrustType(
	const TCHAR*	inName,
	UInt8&			outType )
{
	// Sanity checks.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	for ( size_t x = 0; x < COUNTOF(s_TrustNames); x++ )
	{
		if ( _tcsicmp( s_TrustNames[x].f_pszName, inName ) == 0 )
		{
			outType = (UInt8) s_TrustNames[x].f_nID;
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		CalcFieldDelimiter
// -----------------------------------------------------------------------------
// Go through first part of name table; try to determine what the delimiter is.
// Currently looks for tab and comma.
// Record delimiter is assumed to be cr, lf, or both.

void
CalcFieldDelimiter(
	const CByteStream&	inNameTableStream,
	TCHAR*				szOutDelimiter,
	bool*				outIsASCII )
{
	// First determine if this is a utf-8 file or ASCII file. If this is an ASCII
	// file then it was saved with "," (i.e., CSV). Otherwise it's utf-8 file.
	*outIsASCII = true;
	*szOutDelimiter = _T(',');

	// Read first 9 characters. First we will try ASCII. Then utf-8 encoded file.
	char buffer[4];
	memcpy( buffer, (char*) inNameTableStream.GetData(), 3 );
	buffer[3] = 0;

	// Try the UTF Encoded file, with BOM marker
	UInt32 *pnValue = (UInt32*) buffer;
	
	// SWAP the Bytes and shift by 8 bits
	*pnValue = MemUtil::Swap32(*pnValue) >> 8;
	
	// BOM for UTF-8 is: EF BB BF
	if ( (*pnValue) == 0x00EFBBBF )
	{
		// This is a utf-8 file. We will force the delimiter to be tab because
		// csv files cannot be UTF-8 encoded by definition.
		*outIsASCII = false;
		*szOutDelimiter = _T('\t');
		return;
	}

	// Not UTF-8. It's ASCII. Now find out whether it is CSV or tab delimited

	char*	pData = (char*) inNameTableStream.GetData();
	char*	pDataBegin = pData;
	SInt32	nStreamLen = inNameTableStream.GetLength();
	if ( pData == NULL || nStreamLen == 0 )
	{
		return;
	}

	while ( (pData - pDataBegin) < nStreamLen && *pData != 0 )
	{
		// Eat endline characters.
		while ( (*pData == '\r') || (*pData == '\n') )
		{
			pData++;
		}
		if ( (pData - pDataBegin) >= nStreamLen ) break;

		// Eat comment lines.
		while ( *pData == '#' )
		{
			pData++;	// Skip past #.
			while ( (*pData != '\r') && (*pData != '\n') )
			{
				pData++;
			}
			pData++;	// Skip past cr/lf.
		}
		if ( (pData - pDataBegin) >= nStreamLen ) break;

		// Eat endline characters.
		while ( (*pData == '\r') || (*pData == '\n') )
		{
			pData++;
		}
		if ( (pData - pDataBegin) >= nStreamLen ) break;

		// Now we have an actual data line.
		// Parse through it, and count how many commas and tabs there are.
		UInt32	nTabs = 0;
		UInt32	nCommas = 0;
		while ( (*pData != '\r') && (*pData != '\n') && (*pData != 0) )
		{
			if( *pData == '\t' )
			{
				nTabs++;
			}
			else if( *pData == ',' )
			{
				nCommas++;
			}
			pData++;
		}

		if ( nCommas == 0 )
		{
			*szOutDelimiter = _T('\t');
		}
	}
}

} /* namespace NameTableFile */
