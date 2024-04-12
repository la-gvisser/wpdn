// =============================================================================
//	XMLParserFactory.h
// =============================================================================
//	Copyright (c) 2002-2007 WildPackets, Inc. All rights reserved.

#pragma once

#include <msxml2.h>
#include "XMLDoc.h"


// -----------------------------------------------------------------------------
//	This class provides XML parsing facilities. This is a singleton.
// -----------------------------------------------------------------------------

class DECLSPEC CXMLParser
{
public:
	// Returns the sole instance of this class. If the parser is not initialized 
	// properly, then this method returns NULL
	static CXMLParser*			NewInstance();

	// Parsing facilities
	XXMLElement*				DoParse( LPCTSTR  inFileName );
	XXMLElement*				DoParse( const CByteStream& inStream );

	// Utilities
	const XString&				GetXMLVersion() { return m_strVersion; }
	const XString&				GetEncoding()	{ return m_strEncoding; }

private:
	// Construction
	CXMLParser();
	~CXMLParser();

	// Creates a replica of the WP XML Element from W3C DOM Element
	HRESULT						CopyElement( IXMLDOMNode* inSrcNode, XXMLElement* outDestElem );

	// Recursively builds identical hierarchy of XXMLElement from W3C XMLDOMElement
	void						BuildDOM( IXMLDOMNode* inSrcElem, XXMLElement** outDestElem );

private:
	XString						m_strVersion;
	XString						m_strEncoding;
	CLSID						m_clsidMSXML;
};
