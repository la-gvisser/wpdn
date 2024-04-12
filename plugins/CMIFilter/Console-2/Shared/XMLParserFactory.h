// ============================================================================
//	XMLParserFactory.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002. All rights reserved.

#pragma once

#include <msxml2.h>
#include "XString.h"
#include "XMLDoc.h"


//-----------------------------------------------------------------------
//	
//	This class provides XML parsing facilities. This is a singleton.
//
//-----------------------------------------------------------------------
class DECLSPEC CXMLParser
{
	public:
		// Returns the sole insance of this class. If the parser is not initialized 
		// properly, then this method returns NULL
		static CXMLParser*	NewInstance();

		// Parsing facilities
		XXMLElement*				DoParse( LPCTSTR  inFileName );
		XXMLElement*				DoParse( const CByteStream& inStream );

		// Utilities
		const XString&				GetXMLVersion() { return m_strVersion; }
		const XString&				GetEncoding()	{ return m_strEncoding; }

	protected:
		// Construction
		CXMLParser(void);
		~CXMLParser(void);

		bool						Init();
		XString						m_strVersion;
		XString						m_strEncoding;
		CLSID						m_clsidMSXML;
		static CXMLParser			s_SoleInstance;	
		static bool					s_bInitialized;	
		
		// Creates a replica of the WP XML Element from W3C DOM Element
		HRESULT						CopyElement( IXMLDOMNode* inSrcNode, XXMLElement* outDestElem );

		// Recurssively builds identical hierarchy of XXMLElement from W3C XMLDOMElement
		void						BuildDOM( IXMLDOMNode* inSrcElem, XXMLElement** outDestElem );
};


//-----------------------------------------------------------------------
//	
//	This class provides a non functioning implementation of the 
//	ISAXContentHandler interface. Classes that are interested in parsing 
//  XML using SAX can override from this class and override the necessary
//	inplementation.
//
//-----------------------------------------------------------------------
class DECLSPEC CSAXContentHandlerImpl : public ISAXContentHandler
{
	public:

		// Construction/destruction
		CSAXContentHandlerImpl();
		~CSAXContentHandlerImpl();

        // This must be correctly implemented, if your handler must be a COM Object 
        long __stdcall QueryInterface(const struct _GUID &,void ** );
        unsigned long __stdcall AddRef(void);
        unsigned long __stdcall Release(void);

        virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
            /* [in] */ ISAXLocator  *pLocator);
        
        virtual HRESULT STDMETHODCALLTYPE startDocument( void);
        
        virtual HRESULT STDMETHODCALLTYPE endDocument( void);
        
        virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
            /* [in] */ const wchar_t  *pwchPrefix,
            /* [in] */ int cchPrefix,
            /* [in] */ const wchar_t  *pwchUri,
            /* [in] */ int cchUri);
        
        virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
            /* [in] */ const wchar_t  *pwchPrefix,
            /* [in] */ int cchPrefix);
        
        virtual HRESULT STDMETHODCALLTYPE startElement( 
            /* [in] */ const wchar_t  *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ const wchar_t  *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ const wchar_t  *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes  *pAttributes);
        
        virtual HRESULT STDMETHODCALLTYPE endElement( 
            /* [in] */ const wchar_t  *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ const wchar_t  *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ const wchar_t  *pwchRawName,
            /* [in] */ int cchRawName);
        
        virtual HRESULT STDMETHODCALLTYPE characters( 
            /* [in] */ const wchar_t  *pwchChars,
            /* [in] */ int cchChars);
        
        virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
            /* [in] */ const wchar_t  *pwchChars,
            /* [in] */ int cchChars);
        
        virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
            /* [in] */ const wchar_t  *pwchTarget,
            /* [in] */ int cchTarget,
            /* [in] */ const wchar_t  *pwchData,
            /* [in] */ int cchData);
        
        virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
            /* [in] */ const wchar_t  *pwchName,
            /* [in] */ int cchName);

};