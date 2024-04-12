// ============================================================================
//	NameTableParser.cpp
// ============================================================================
//	Copyright (c) 2002-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#include "NameTableParser.h"
#include "NameTable.h"
#include "Resource.h"
#include "MemUtil.h"

#include "AddressTableParser.h"
#include "ProtocolTableParser.h"
#include "PortTableParser.h"

#include <comdef.h>
#include <msxml2.h>
#include <shlwapi.h>
#include <wininet.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// Handy macro to take care of the COM error handling
#define		CHECKHR(x)				{ HRESULT hr = x; if (FAILED(hr)) _com_issue_error(hr);}

#define		CURRENT_NT_VERSION		L"3.0"

#define		XML_ROOT_NAME			L"NameTable"
#define		XML_ELEM_NAME_ENTRY		L"Entry"
#define		XML_ATTR_ID_CLASS		L"Class"
#define		XML_ATTR_ID_VERSION		L"Version"
#define		XATTR_CLASS_ADDRESS		L"Address"
#define		XATTR_CLASS_PROTOCOL	L"Protocol"
#define		XATTR_CLASS_PORT		L"Port"

//-----------------------------------------------------------------------------
//	Static initializers
//-----------------------------------------------------------------------------

CNameTableParser	CNameTableParser::s_SoleInstance;

//-----------------------------------------------------------------------------
//	NewInstance														[static]
//-----------------------------------------------------------------------------

const CNameTableParser*
CNameTableParser::NewInstance()
{
	return &s_SoleInstance;
}

//-----------------------------------------------------------------------------
//	Parse														
//-----------------------------------------------------------------------------

NTParserErrCodeType
CNameTableParser::Parse
(
	LPCTSTR			szFilename, 
	CNameTable*		inNameTable,
	UInt32*			pOutValidNames,
	UInt32*			pOutImported,
	UInt32*			pOutSkipped
)
{
	NTParserErrCodeType bParsingResult = NT_PARSER_ERR_UNKNOWN;

	// Store the nametable 
	m_pNameTable = inNameTable;

	// Stop broadcasting
	m_pNameTable->StopBroadcasting();

	// Reset the statistics
	m_nEntriesImported = 0;
	m_nEntriesSkipped  = 0;
	m_nEntriesValid	   = 0;

	// Parset State
	m_hrParserState = S_OK;

	// Determine the size of the file
	CFile theNTFile(szFilename, CFile::modeRead | CFile::shareDenyNone );
	ULONGLONG nSize = theNTFile.GetLength();
	theNTFile.Close();
	
	// Set up progress indicator.
	//m_pProgressIndicator =  new CProgressIndicator( AfxGetMainWnd(), IDD_PROGRESS_CANCEL, LoadResourceString( IDS_PROGRESS_IMPORTING_NAMES ) );
	//m_pProgressIndicator->SetRange( 1, (int) nSize );

	try
 	{
		// Instantiate the SAX2 Parser
		CComPtr<ISAXXMLReader> spReader;
		if ( FAILED(spReader.CoCreateInstance( L"Msxml2.SAXXMLReader.6.0" )) )
		{
			if ( FAILED(spReader.CoCreateInstance( L"Msxml2.SAXXMLReader.4.0" )) )
			{
				CHECKHR( spReader.CoCreateInstance( L"Msxml2.SAXXMLReader.3.0" ) );
			}
		}

		// Setup the SAX2 parser
		CNmTblContentHandler theContentHandler( this, inNameTable);
		CHECKHR( spReader->putContentHandler( &theContentHandler ););
		
		// Create the URL form the given path
		TCHAR szUrl[INTERNET_MAX_URL_LENGTH];
		DWORD nBufferLen = sizeof(szUrl);
		CHECKHR( ::UrlCreateFromPath( szFilename, szUrl, &nBufferLen, NULL); );

		// Begin parsing
		CHECKHR( spReader->parseURL(CComBSTR(szUrl)); );
		bParsingResult = NT_PARSER_ERR_SUCCESS;
	}
	catch ( _com_error& err )
	{
		HRESULT	hr = err.Error();

		switch( hr )
		{
			case E_ABORT:
				// User canceled
				bParsingResult = NT_PARSER_ERR_USER_ABORT;
				TRACE( _T("User canceled the import operation\n") );
				break;
			case E_NOTIMPL:
				// We don't support this version
				bParsingResult = NT_PARSER_ERR_WRONG_VERSION;
				TRACE( _T("Wrong name table version\n") );
				break;
			case E_FAIL:
				// Other type of error -- typically COM error
				bParsingResult = NT_PARSER_ERR_SYSTEM_ERROR;
				TRACE( _T("Critical error\n") );
				break;
			case E_INVALIDARG:
				// Wrong doc format i.e., missing attribute or element
				bParsingResult = NT_PARSER_ERR_WRONG_DOC_FORMAT; 
				TRACE( _T("Wrong XML document structure\n") );
				break;
			default:
				bParsingResult = NT_PARSER_ERR_UNKNOWN;
				TRACE( _T("Error: %s\n"), err.ErrorMessage() );
				break;
		}
		*pOutValidNames = 0;
		*pOutImported	= 0;
		*pOutSkipped	= 0;
	}

	// Clean up the progress indicator
	//m_pProgressIndicator->SetPos( (int) nSize );
	//m_pProgressIndicator->Stop();

	//delete m_pProgressIndicator;
	//m_pProgressIndicator = NULL;

	// Resume broadcasting
	m_pNameTable->StartBroadcasting();
	m_pNameTable->NamesImported();

	if ( bParsingResult == NT_PARSER_ERR_SUCCESS )
	{
		// Return proper stats
		*pOutValidNames = m_nEntriesValid;
		*pOutImported	= m_nEntriesImported;
		*pOutSkipped	= m_nEntriesSkipped;
	}

	return bParsingResult;
}

//-----------------------------------------------------------------------------
//	EntryAdded
//-----------------------------------------------------------------------------

void
CNameTableParser::EntryAdded()
{
	// Set the current parser to NULL so that we can move to next entry and 
	// pick up the new parser
	m_pCurSubtableParser = NULL;

	// Update the progress indicator
	//m_pProgressIndicator->SetPos(m_nBytesRead);

	// Check to see if the user has canceled the operation
	//if ( m_pProgressIndicator->CheckForCancel() )
	//{
	//	m_hrParserState = E_ABORT;
	//}

	// Update stats
	m_nEntriesValid++;
	m_nEntriesImported++;

	//TRACE( _T("Bytes read so far: %d\n"), m_nBytesRead );
}

//-----------------------------------------------------------------------------
//	EntrySkipped
//-----------------------------------------------------------------------------

void
CNameTableParser::EntrySkipped()
{
	// Set the current parser to NULL so that we can move to next entry and 
	// pick up the new parser
	m_pCurSubtableParser = NULL;

	// Update the progress indicator
	//m_pProgressIndicator->SetPos(m_nBytesRead);

	// Check to see if the user has canceled the operation
	//if ( m_pProgressIndicator->CheckForCancel() )
	//{
	//	m_hrParserState = E_ABORT;
	//}

	// Update stats
	m_nEntriesValid++;
	m_nEntriesSkipped++;

	//TRACE( _T("Bytes read so far: %d\n"), m_nBytesRead );
}

//-----------------------------------------------------------------------------
//	EntryParsingAbort
//-----------------------------------------------------------------------------

void
CNameTableParser::EntryParsingAbort()
{
	// Set the current parser to NULL so that we can move to next entry and 
	// pick up the new parser
	m_pCurSubtableParser = NULL;

	// Update the progress indicator
	//m_pProgressIndicator->SetPos(m_nBytesRead);

	// Check to see if the user has canceled the operation
	//if ( m_pProgressIndicator->CheckForCancel() )
	//{
	//	m_hrParserState = E_ABORT;
	//}
	//TRACE( _T("Bytes read so far: %d\n"), m_nBytesRead );

	m_nEntriesSkipped++;
}

//-----------------------------------------------------------------------------
//	CNameTableParser												[protected]
//-----------------------------------------------------------------------------

CNameTableParser::CNameTableParser() 
	: m_pCurSubtableParser( NULL )
	, m_pAddrTableParser( NULL )
	, m_pProtoTableparser( NULL )
	, m_pPortTableparser( NULL )
	, m_pNameTable( NULL )
	, m_nBytesRead( 0 )
	, m_nEntriesImported( 0 )
	, m_nEntriesSkipped( 0 )
	, m_nEntriesValid( 0 )
	, m_hrParserState( S_OK )
	//, m_pProgressIndicator( NULL )
{
	m_pAddrTableParser  = new CAddressTableParser( this );
	m_pProtoTableparser = new CProtocolTableParser( this );
	m_pPortTableparser  = new CPortTableParser( this );
}

//-----------------------------------------------------------------------------
//	~CNameTableParser												[protected]
//-----------------------------------------------------------------------------

CNameTableParser::~CNameTableParser()
{
	delete m_pAddrTableParser;
	delete m_pProtoTableparser;
	delete m_pPortTableparser;
}

//-----------------------------------------------------------------------------
//	CNmTblContentHandler()
//-----------------------------------------------------------------------------

CNmTblContentHandler::CNmTblContentHandler( CNameTableParser* pParser, CNameTable* pNameTable )
	: m_pParser( pParser )
	, m_pNameTable( pNameTable )
	, m_bCorrectVersion(false)
{
}

//-----------------------------------------------------------------------------
//	~CNmTblContentHandler()
//-----------------------------------------------------------------------------

CNmTblContentHandler::~CNmTblContentHandler()
{
}

//-----------------------------------------------------------------------------
//	CNmTblContentHandler::startDocument()
//-----------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE
CNmTblContentHandler::startDocument()
{
	TRACE( _T("Nametable Parsing begins...\n") );

	// Reset the total bytes read 
	m_pParser->m_nBytesRead = 0;

	return S_OK;
}

//-----------------------------------------------------------------------------
//	CNmTblContentHandler::startDocument()
//-----------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE
CNmTblContentHandler::endDocument()
{
	TRACE( _T("Nametable Parsing ends...\n") );
	TRACE( _T("Total Bytes Read: %d\n"), m_pParser->m_nBytesRead );
	return S_OK;
}

//-----------------------------------------------------------------------------
//	CNmTblContentHandler::startElement()
//-----------------------------------------------------------------------------

HRESULT STDMETHODCALLTYPE
CNmTblContentHandler::startElement
( 
	const wchar_t*	/*pwchNamespaceUri*/,
	int				/*cchNamespaceUri*/,
	const wchar_t*	pwchLocalName,
	int				cchLocalName,
	const wchar_t*	/*pwchRawName*/,
	int				/*cchRawName*/,
	ISAXAttributes*	pAttributes
)
{
	// Sanity checking
	if ( m_pParser->m_hrParserState != S_OK )
	{
		// This will take care of aborting if the user wants that
		return m_pParser->m_hrParserState;
	}

	bool	bResult = true;
	CString	strErrMsg;
	if ( m_pParser->m_pCurSubtableParser == NULL )
	{
		if ( !wcsncmp( pwchLocalName, XML_ELEM_NAME_ENTRY, cchLocalName ) )
		{
			// If this is not the correct version then something is wrong because parsing should have stopped
			if ( !m_bCorrectVersion )
			{
				// Something wrong! 
				return E_FAIL;
			}

			// Get the attribute and switch
			wchar_t *	wchAttrValue;
			int		nSize = 5; // Length of "Class"
			int		nIndex = 0;
			HRESULT hr = pAttributes->getIndexFromName( L"", 0, XML_ATTR_ID_CLASS, nSize, &nIndex );
			if ( hr == S_OK )
			{
				hr = pAttributes->getValue( nIndex, (const wchar_t**) &wchAttrValue, &nSize);
				if ( hr == S_OK )
				{
					if ( !wcsncmp( wchAttrValue, XATTR_CLASS_ADDRESS, nSize ) )
					{
						m_pParser->m_pCurSubtableParser = m_pParser->m_pAddrTableParser;

					}
					else if ( !wcsncmp( wchAttrValue, XATTR_CLASS_PROTOCOL, nSize ) )
					{
						m_pParser->m_pCurSubtableParser = m_pParser->m_pProtoTableparser;
					}
					else if ( !wcsncmp( wchAttrValue, XATTR_CLASS_PORT, nSize ) )
					{
						m_pParser->m_pCurSubtableParser = m_pParser->m_pPortTableparser;
					}
					else
					{	
						CString strAttrClass( wchAttrValue, nSize );
						strErrMsg.Format( L"Parsing Error: Unrecognized Class: %s", strAttrClass );
						bResult = false;
					}
					m_pParser->m_pCurSubtableParser->Init();
					m_pParser->m_pCurSubtableParser->BeginElement( pwchLocalName, cchLocalName, pAttributes );
				}
				else
				{
					// Does not have the Class attribute
					strErrMsg = L"Parsing Error: Attr \'Class\' not defined";
					bResult = false;
					return E_INVALIDARG;
				}				
			}
		}
		else if ( !wcsncmp( pwchLocalName, XML_ROOT_NAME, cchLocalName ) )
		{
			// Check the version CURRENT_NT_VERSION
			wchar_t *	wchAttrValue;
			int		nSize = 7; // Length of "Version"
			int		nIndex = 0;
			HRESULT hr = pAttributes->getIndexFromName( L"", 0, XML_ATTR_ID_VERSION, nSize, &nIndex );
			if ( hr == S_OK )
			{
				hr = pAttributes->getValue( nIndex, (const wchar_t**) &wchAttrValue, &nSize);
				if ( hr == S_OK )
				{
					if ( !wcsncmp( wchAttrValue, CURRENT_NT_VERSION, nSize ) )
					{
						// Correct version
						m_bCorrectVersion = true;
					}
					else
					{
						//  Wrong version
						return E_NOTIMPL;
					}
				}
				else
				{
					// Version missing or other COM error
					return hr;
				}
			}
			else
			{
				return E_INVALIDARG;
			}
		}
		else
		{
			CString strName( pwchLocalName, cchLocalName );
			strErrMsg.Format( _T("Don't know what to do with this element: \'%s\'"), strName );
			TRACE( _T("%s\n"), strErrMsg );
			bResult = false;
		}
	}
	else
	{
		m_pParser->m_pCurSubtableParser->BeginElement( pwchLocalName, cchLocalName, pAttributes );
	}

	// Increment total bytes
	m_pParser->m_nBytesRead += cchLocalName * sizeof(wchar_t);

    return S_OK;
}
        

//-----------------------------------------------------------------------------
//	CNmTblContentHandler::endElement()
//-----------------------------------------------------------------------------
      
HRESULT 
CNmTblContentHandler::endElement
( 
	const wchar_t*	/*pwchNamespaceUri*/,
	int				/*cchNamespaceUri*/,
	const wchar_t*	pwchLocalName,
	int				cchLocalName,
	const wchar_t*	/*pwchRawName*/,
	int				/*cchRawName*/
)
{
	// The m_pCurSubtableParser will become NULL at the end of the parsing
	if ( m_pParser->m_pCurSubtableParser != NULL )
	{
		m_pParser->m_pCurSubtableParser->EndElement( pwchLocalName, cchLocalName );
	}

	// Increment total bytes
	m_pParser->m_nBytesRead += cchLocalName * sizeof(wchar_t);

	return S_OK;
}

//-----------------------------------------------------------------------------
//	CNmTblContentHandler::characters()
//-----------------------------------------------------------------------------

HRESULT
CNmTblContentHandler::characters
( 
	const wchar_t*	pwchChars,
	int				cchChars
)
{
	if ( m_pParser->m_pCurSubtableParser != NULL )
	{
		m_pParser->m_pCurSubtableParser->ProcessChar( pwchChars, cchChars );
	}
    
	// Increment total bytes
	m_pParser->m_nBytesRead += cchChars * sizeof(wchar_t);

	return S_OK;
}


bool
CNameSubTableParserBase::GetIdFromName( 
	SIDNameMapEntry*	inMap, 
	int					inArrayLen, 
	const wchar_t*		inName, 
	int					nSize,
	UInt32*				OutVal ) const
{
	// Sanity check.
	ASSERT( inName != NULL );
	if ( inName == NULL ) return false;

	// The inName can be IP, IPX, IPPort etc. Therefore we have use wcsncmp (cannot use
	// wcsncmp which will return true for all of the above). 
	// Now the problem is that inName is not NULL terminated. So, we need to 
	// create a NULL terminated string and proceed

	const CStringW strName( inName, nSize );

	for ( int x = 0; x < inArrayLen; x++ )
	{
		if ( wcscmp( inMap[x].f_pszName, strName ) == 0 )
		{
			*OutVal = inMap[x].f_nID;
			return true;
		}
	}

	return false;
}

void
CNameSubTableParserBase::GetDateFromString( CStringW& inStrDate, time_t* outSeconds )
{
	// Parse date: ISO 9601 format.
	struct tm	dateTime;

	// Use the buffer again
	wchar_t* pBuf = inStrDate.GetBuffer();

	memset( &dateTime, 0, sizeof(dateTime));

	// Format for the Date string is: 2003-03-14T18:04:06.000Z
	dateTime.tm_year	= _wtoi( pBuf ) - 1900;
	dateTime.tm_mon		= _wtoi( pBuf + 5 ) - 1;
	dateTime.tm_mday	= _wtoi( pBuf + 8 );
	dateTime.tm_hour	= _wtoi( pBuf + 11 );
	dateTime.tm_min		= _wtoi( pBuf + 14 );
	dateTime.tm_sec		= _wtoi( pBuf + 17 );
	dateTime.tm_isdst	= -1;	// -1 is use sysLib to compute DST or not

	inStrDate.ReleaseBuffer();

	*outSeconds = mktime( &dateTime );

	// We're going to use mktime, which does localtime.
	// Need to adjust for time zone bias.
	SInt32 nTimezoneDelta = -_timezone + (_daylight * 3600);
	
	*outSeconds += nTimezoneDelta;
}
