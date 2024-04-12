#include "StdAfx.h"
#include <comdef.h>
#include <msxml2.h>
#include <atlsafe.h>
#include "XMLParserFactory.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// Handy macro to take care of the COM error handling
#define		CHECKHR(x)				{ HRESULT hr = x; if (FAILED(hr)) _com_issue_error(hr);}
#define		PROG_ID_MSXML			L"Microsoft.XMLDOM"
#define		MAX_TAG_LEN				256
#define		MAX_ATTR_NAME_LEN		256

// Initialization of the static sole instance of this class
CXMLParser	CXMLParser::s_SoleInstance;
bool		CXMLParser::s_bInitialized;

CXMLParser::CXMLParser(void)
{
	s_bInitialized = Init();
}

CXMLParser::~CXMLParser(void)
{
}

CXMLParser*
CXMLParser::NewInstance()
{
	if (s_bInitialized)
	{
		return &s_SoleInstance;
	}
	else
	{
		return NULL;
	}
}

XXMLElement*
CXMLParser::DoParse( LPCTSTR  inFileName )
{
	XXMLElement*	theRootElem = NULL;
	try
	{
		// Create the Doc instance
		CComPtr<IXMLDOMDocument2> spXmlDoc;
		CHECKHR(CoCreateInstance(m_clsidMSXML, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&spXmlDoc);)
		
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
		CHECKHR(CoCreateInstance(m_clsidMSXML, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&spXmlDoc);)
		
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

bool
CXMLParser::Init()
{
	bool bRet = false;
	try
	{
		// We can't just use the CLSID_DOMDocument defined in msxml.h because for other systems
		// that results in Class Not Registered COM Error. Thereofre we go by the ProgID and 
		// get CLSID from the ProgID.
		CHECKHR(::CLSIDFromProgID( PROG_ID_MSXML, &m_clsidMSXML ); )
		
		// All is well ...
		bRet = true;
	}
	catch( _com_error& err )
	{
		TRACE( _T("COM Error while looking for MSXML implementation: 0x%8.8X\n"), err.Error() );
		err;
	}

	return bRet;
}

void
CXMLParser::BuildDOM( IXMLDOMNode* inSrcNode, XXMLElement** outDestElem )
{
	try
	{
		CString			strNodeValue;
		// Copy this element and the associated attributes
		DOMNodeType	theNodeType;
		CHECKHR( inSrcNode->get_nodeType( &theNodeType );)
		if 
		( 
			theNodeType == NODE_ELEMENT
		)
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
					VARIANT varValue;
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
		else if 
		(
			theNodeType == NODE_TEXT || 
			theNodeType == NODE_CDATA_SECTION
		)
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
		CString csName( bstrName.m_str );
		outDestElem->SetName( csName );

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
			
			VARIANT					varValue;
			CHECKHR( spNextAttr->get_nodeValue( &varValue ); )
			// TBD Check for NULL value

			// Now set this attributes 
			CString					csName( bstrAttrName.m_str );
			CString					csAttrValue( varValue );
			outDestElem->AddAttribute( csName, csAttrValue );
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

//=============================================================================
//
//						CSAXContentHandlerImpl Class
//
//=============================================================================

// -----------------------------------------------------------------------------
//	CSAXContentHandlerImpl													
// -----------------------------------------------------------------------------
  
CSAXContentHandlerImpl::CSAXContentHandlerImpl()
{
}

// -----------------------------------------------------------------------------
//	~CSAXContentHandlerImpl													
// -----------------------------------------------------------------------------
  
CSAXContentHandlerImpl::~CSAXContentHandlerImpl()
{
}

// -----------------------------------------------------------------------------
//	CSAXContentHandlerImpl													
// -----------------------------------------------------------------------------
  
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::putDocumentLocator( ISAXLocator* /*pLocator*/)
{
    return S_OK;
}
        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::startDocument()
{
    return S_OK;
}
        

        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::endDocument( void)
{
    return S_OK;
}
        
        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::startPrefixMapping
( 
	const wchar_t*	/*pwchPrefix*/,
	int			/*cchPrefix*/,
	const wchar_t*	/*pwchUri*/,
	int			/*cchUri*/)
{
    return S_OK;
}
        
        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::endPrefixMapping
( 
	const wchar_t*	/*pwchPrefix*/,
	int			/*cchPrefix*/)
{
    return S_OK;
}

        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::startElement
( 
	const wchar_t*		/*pwchNamespaceUri*/,
	int				/*cchNamespaceUri*/,
	const wchar_t*		/*pwchLocalName*/,
	int				/*cchLocalName*/,
	const wchar_t*		/*pwchRawName*/,
	int				/*cchRawName*/,
	ISAXAttributes*	/*pAttributes*/)
{
    return S_OK;
}
        
       
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::endElement
( 
	const wchar_t*	/*pwchNamespaceUri*/,
	int			/*cchNamespaceUri*/,
	const wchar_t*	/*pwchLocalName*/,
	int			/*cchLocalName*/,
	const wchar_t*	/*pwchRawName*/,
	int			/*cchRawName*/
)
{
    return S_OK;
}
        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::characters
( 
	const wchar_t*	/*pwchChars*/,
	int			/*cchChars*/
)
{
    return S_OK;
}
        

HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::ignorableWhitespace
( 
	const wchar_t*	/*pwchChars*/,
	int			/*cchChars*/
)
{
    return S_OK;
}
        

HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::processingInstruction
( 
	const wchar_t*	/*pwchTarget*/,
	int			/*cchTarget */,
	const wchar_t*	/*pwchData*/,
	int			/*cchData*/
)
{
    return S_OK;
}
        
        
HRESULT STDMETHODCALLTYPE
CSAXContentHandlerImpl::skippedEntity
( 
	const wchar_t*	/*pwchVal*/,
	int			/*cchVal*/
)
{
    return S_OK;
}


long STDMETHODCALLTYPE CSAXContentHandlerImpl::QueryInterface(const struct _GUID& ,void** )
{
    // hack-hack-hack!
    return 0;
}

unsigned long STDMETHODCALLTYPE CSAXContentHandlerImpl::AddRef()
{
    // hack-hack-hack!
    return 0;
}

unsigned long STDMETHODCALLTYPE CSAXContentHandlerImpl::Release()
{
    // hack-hack-hack!
    return 0;
}
