// ============================================================================
//	NameTableParser.h
// ============================================================================
//	Copyright (c) 2002-2007 WildPackets, Inc. All rights reserved.

#pragma once

#include "SAXContentHandlerImpl.h"
#include "NameTable.h"
//#include "Progress.h"

// Forward declarations
class CNameTable;
class INameSubTableParser;
class CAddressTableParser;
class CProtocolTableParser;
class CPortTableParser;

// Error codes
typedef enum
{
	NT_PARSER_ERR_SUCCESS			= 0,	// Successfully parsed
	NT_PARSER_ERR_WRONG_VERSION		= 10,	// Not capable be parsing older docs
	NT_PARSER_ERR_WRONG_DOC_FORMAT	= 20,	// Missing necessary attribute/element
	NT_PARSER_ERR_USER_ABORT		= 30,	// User pressed cancel 
	NT_PARSER_ERR_UNKNOWN			= 90,
	NT_PARSER_ERR_SYSTEM_ERROR		= 99	// Typically COM error 
} NTParserErrCodeType;

// =============================================================================
//	CNameTableParser
//  
//	This class is a singleton that parses a name table file and builds the 
//  object model.
//	
//	This class uses MSXML SAX parsing for parsing the XML file.
//
// =============================================================================

class CNameTableParser
{
	public:
		// Only way to get an object of this class
		static const CNameTableParser*	NewInstance();

		// Parses the XML file containing the nametable and returns the object model
		// In case of error, the return value is false and output stats contain 0.
		NTParserErrCodeType Parse
		( 
			LPCTSTR			szFilename, 
			CNameTable*		inNameTable,
			UInt32*			pOutValidNames,
			UInt32*			pOutImported,
			UInt32*			pOutSkipped
		);

		// Returns the Name table that is being created
		CNameTable*	GetNameTable() { return m_pNameTable; }

		// Called by individual parsers to denote that 
		// the entry has been added
		void		EntryAdded();

		// Called by individual parsers to denote 
		// parsing aborting due to error
		void		EntryParsingAbort();

		// Called by individual parsers to denote that 
		// the entry has been skipped due to duplicate
		void		EntrySkipped();
	protected:

		// Constructor
		CNameTableParser();

		// Destructor
		~CNameTableParser();

		// Sole instance of this class
		static CNameTableParser		s_SoleInstance;

		// Parsers for the name sub tables
		INameSubTableParser*	m_pCurSubtableParser;
		CAddressTableParser*	m_pAddrTableParser;
		CProtocolTableParser*	m_pProtoTableparser;
		CPortTableParser*		m_pPortTableparser;
		CNameTable*				m_pNameTable;

		friend class CNmTblContentHandler;

		// Keeps track of how many bytes read so far
		int						m_nBytesRead;

		// Keeps track of the statistics
		int						m_nEntriesImported;
		int						m_nEntriesSkipped;
		int						m_nEntriesValid;

		HRESULT					m_hrParserState;

		// Progress Indicator
		//CProgressIndicator*		m_pProgressIndicator;
};


// =============================================================================
//	CNmTblContentHandler
//  
//	Our own content handler that takes care of parsing the XML
//
// =============================================================================

// 
class CNmTblContentHandler : public CSAXContentHandlerImpl
{
	public:
		CNmTblContentHandler( CNameTableParser* pNameTableParser, CNameTable* pNameTable );
		~CNmTblContentHandler();
		virtual HRESULT STDMETHODCALLTYPE startElement( 
			/* [in] */ const wchar_t *pwchNamespaceUri,
			/* [in] */ int cchNamespaceUri,
			/* [in] */ const wchar_t *pwchLocalName,
			/* [in] */ int cchLocalName,
			/* [in] */ const wchar_t *pwchRawName,
			/* [in] */ int cchRawName,
			/* [in] */ ISAXAttributes *pAttributes);
		
		virtual HRESULT STDMETHODCALLTYPE endElement( 
			/* [in] */ const wchar_t *pwchNamespaceUri,
			/* [in] */ int cchNamespaceUri,
			/* [in] */ const wchar_t *pwchLocalName,
			/* [in] */ int cchLocalName,
			/* [in] */ const wchar_t *pwchRawName,
			/* [in] */ int cchRawName);

		virtual HRESULT STDMETHODCALLTYPE startDocument();
		virtual HRESULT STDMETHODCALLTYPE endDocument();

		virtual HRESULT STDMETHODCALLTYPE characters( 
			/* [in] */ const wchar_t *pwchChars,
			/* [in] */ int cchChars);
	private:
		CNameTableParser*	m_pParser;
		CNameTable*			m_pNameTable;
		bool				m_bCorrectVersion;
};

// =============================================================================
//	INameSubTableParser
//  
//	This interface is used for delegating the aprsing of the name table
//	parsing to respective sub table parsers.
//	
//	This class uses MSXML SAX parsing for parsing the XML file.
//
// =============================================================================

class INameSubTableParser
{
	public:
		virtual void Init() = 0;
		virtual void ProcessChar( const wchar_t* pwchChars, int cchChars ) = 0;
		virtual void BeginElement( const wchar_t* pwchName, int cchChars, ISAXAttributes *pAttributes ) = 0;
		virtual void EndElement( const wchar_t* pwchName, int cchChars ) = 0;
};

typedef struct SIDNameMapEntry
{
	UInt32			f_nID;
	const TCHAR*	f_pszName;
} SIDNameMapEntryType;

// =============================================================================
//	CNameSubTableParserBase
//  
//	This base class contains common methods used by all other name table parsers
//
// =============================================================================

class CNameSubTableParserBase 
	: public INameSubTableParser
{
	protected:
		/// This method returns Id from the given name. The inName points to the buffer containing the name and
		/// nSize points to the size of the string in this buffer
		bool GetIdFromName( SIDNameMapEntry* inMap, int inArrayLen, const wchar_t* inName, int nSize, UInt32* OutVal ) const;

		/// Returns date from the given string. The input inStrDate points to the date string and nLen points to the length of this string
		void GetDateFromString( CStringW& inStrDate, time_t* outSeconds );
};