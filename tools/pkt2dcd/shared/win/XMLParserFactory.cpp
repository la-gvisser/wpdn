// =============================================================================
//	XMLParserFactory.cpp
// =============================================================================
//	Copyright (c) 2002-2009 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "XMLParserFactory.h"
#include <atlsafe.h>
#include <comdef.h>

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// Handy macro to take care of the COM error handling
#define CHECKHR(x)		{ HRESULT hr = x; if (FAILED(hr)) _com_issue_error(hr);}

CXMLParser::CXMLParser()
{
}

CXMLParser::~CXMLParser()
{
}

CXMLParser*
CXMLParser::NewInstance()
{
	static CXMLParser	s_SoleInstance;	
	return &s_SoleInstance;
}

XXMLElement*
CXMLParser::DoParse( LPCTSTR  inFileName )
{
	XXMLElement*	theRootElem = NULL;
	try
	{
		// Create the Doc instance
		CComPtr<IXMLDOMDocument2> spXmlDoc;
		CHECKHR(spXmlDoc.CoCreateInstance( OLESTR("Msxml2.DOMDocument") ));
		
		// Load the document
		VARIANT_BOOL varResult;
		CHECKHR( spXmlDoc->put_async( VARIANT_FALSE );)
		CHECKHR( spXmlDoc->load( CComVariant(inFileName), &varResult ); )
		if (varResult==VARIANT_FALSE)
		{
			TRACE( _T("Cannot read xml document: %s\n"), inFileName );
			return theRootElem;
		}

		// Now create the Document per WP XML Document spec.
		CComPtr<IXMLDOMElement> spRootElem;
		CHECKHR(spXmlDoc->get_documentElement(&spRootElem));
		
		CComQIPtr<IXMLDOMNode> spRootNode = spRootElem;
		ASSERT( spRootNode != NULL );
		if (spRootNode)
		{
			// Now recursively build the document
			BuildDOM( spRootNode, &theRootElem );
		}
	}
	catch( _com_error& err )
	{
		TRACE( _T("COM Error while parsing XML document: 0x%8.8X\n"), err.Error() );
		err;
	}

	return theRootElem;
}

XXMLElement*
CXMLParser::DoParse( const CByteStream& inStream )
{
	XXMLElement*	theRootElem = NULL;
	try
	{
		// Create the Doc instance
		CComPtr<IXMLDOMDocument2> spXmlDoc;
		CHECKHR(spXmlDoc.CoCreateInstance( OLESTR("Msxml2.DOMDocument") ); )
		
		// turn the stream into a VARIANT
		UInt32 theMarker = inStream.GetMarker();
		UInt32 totalLength = inStream.GetLength() - theMarker;
		CComSafeArray<UInt8>  saStream( totalLength );
		LPSAFEARRAY* pSA = saStream.GetSafeArrayPtr();
		// copy to the safe array
		memcpy((*pSA)->pvData, ((UInt8*) inStream.GetData()) + theMarker, totalLength );
		VARIANT varStream;
		varStream.vt = (VT_ARRAY|VT_UI1);
		varStream.parray = (*pSA);
		// Load the document
		VARIANT_BOOL varResult;
		CHECKHR( spXmlDoc->put_async( VARIANT_FALSE );)
		CHECKHR( spXmlDoc->load( varStream, &varResult ); )
		if (varResult==VARIANT_FALSE)
		{
			TRACE( _T("Cannot read XML stream\n") );
			return theRootElem;
		}

		// Now create the Document per WP XML Document spec.
		CComPtr<IXMLDOMElement> spRootElem;
		CHECKHR(spXmlDoc->get_documentElement(&spRootElem));
		
		CComQIPtr<IXMLDOMNode> spRootNode = spRootElem;
		ASSERT( spRootNode != NULL );
		if (spRootNode)
		{
			// Now recursively build the document
			BuildDOM( spRootNode, &theRootElem );
		}
	}
	catch( _com_error& err )
	{
		TRACE( _T("COM Error while parsing XML document: 0x%8.8X\n"), err.Error() );
		err;
	}

	return theRootElem;
}

void
CXMLParser::BuildDOM( IXMLDOMNode* inSrcNode, XXMLElement** outDestElem )
{
	try
	{
		// Copy this element and the associated attributes
		DOMNodeType	theNodeType;
		CHECKHR( inSrcNode->get_nodeType( &theNodeType );)
		if ( theNodeType == NODE_ELEMENT )
		{
			XXMLElement*	theElement = new XXMLElement();
			CopyElement( inSrcNode, theElement );
			// Add it to the parent node
			*outDestElem = theElement;

			// Get all the children under this element
			CComPtr<IXMLDOMNodeList> spChildNodeList;
			CHECKHR( inSrcNode->get_childNodes( &spChildNodeList );)
			long nLength;
			CHECKHR( spChildNodeList->get_length( &nLength ); )

			// The WP XML Object model treats text/cdata values as the values of
			// an element. So we need to iterate over the W3C DOM picking up
			// all the children that are text/cdata of a given element. We
			// use the following string for that
			CString csElementValue;
			bool	bSawCDATA = false;

			// Iterate over the children list and copy the DOM
			for ( long i = 0; i < nLength; i++ )
			{
				CComPtr<IXMLDOMNode> spNextChild;
				CHECKHR( spChildNodeList->get_item( i, &spNextChild ); )
				
				// Now check if this child is an Element or text/cdata/comment
				DOMNodeType		theChildType;
				CHECKHR( spNextChild->get_nodeType(&theChildType); )
				if ( theChildType == NODE_ELEMENT )
				{
					// Found another child
					XXMLElement* theChild;
					BuildDOM( spNextChild, &theChild );
					theElement->AddChild(theChild);
				}
				else if ( theChildType == NODE_TEXT || theChildType == NODE_CDATA_SECTION )
				{
					CComVariant	varValue;
					CHECKHR( spNextChild->get_nodeValue( &varValue ));
					csElementValue += varValue.bstrVal;
				}

				// remember if we saw a CDATA section
				bSawCDATA |= ( theChildType == NODE_CDATA_SECTION );
			}

			// Now set the value for this element (could be empty too)
			theElement->SetValue( (LPCTSTR) csElementValue );
			theElement->SetCDATA( bSawCDATA );
		}
		else if ( theNodeType == NODE_TEXT || 
			theNodeType == NODE_CDATA_SECTION )
		{
			// Should never get here.
			ASSERT(0);
		}
	}
	catch( _com_error& err )
	{
		TRACE( _T("COM Error while parsing MSXML DOM: 0x%8.8X\n"), err.Error() );
		err;
	}
}

HRESULT
CXMLParser::CopyElement( IXMLDOMNode* inSrcNode, XXMLElement* outDestElem )
{
	HRESULT hrRet = E_FAIL;
	try
	{
		// Get the src element name
		CComBSTR bstrName;
		CHECKHR( inSrcNode->get_nodeName( &bstrName ););

		// Set the element name
		if ( bstrName.m_str != NULL )
		{
			outDestElem->SetName( CW2CT(bstrName.m_str) );
		}

		// Now add the attributes if any
		CComPtr<IXMLDOMNamedNodeMap> spAttrMap;
		CHECKHR( inSrcNode->get_attributes(&spAttrMap); )

		long nAttrNum;
		CHECKHR( spAttrMap->get_length(&nAttrNum); )
		for ( long i=0; i < nAttrNum; i++ )
		{
			CComPtr<IXMLDOMNode>	spNextAttr;
			CHECKHR( spAttrMap->get_item(i, &spNextAttr ); )
			
			CComBSTR				bstrAttrName;
			CHECKHR( spNextAttr->get_nodeName( &bstrAttrName ); )
			
			CComVariant				varValue;
			CHECKHR( spNextAttr->get_nodeValue( &varValue ); )
			if ( varValue.vt != VT_BSTR )
			{
				CHECKHR( varValue.ChangeType( VT_BSTR ) );
			}

			if ( bstrAttrName.m_str != NULL )
			{
				// Now set this attributes 
				LPCWSTR	pszVal = (varValue.bstrVal != NULL) ? varValue.bstrVal : L"";
				outDestElem->AddAttribute( CW2CT(bstrAttrName.m_str), CW2CT(pszVal) );
			}
		}

		hrRet = S_OK;
	}
	catch( _com_error& err )
	{
		TRACE( _T("COM Error while copying MSXML DOM Element: %s\n"), err.ErrorMessage() );
		hrRet = err.Error();
	}

	return hrRet;
}
