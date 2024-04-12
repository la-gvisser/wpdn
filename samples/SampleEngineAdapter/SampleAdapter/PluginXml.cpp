// =============================================================================
//	PluginXml.cpp
// =============================================================================
//	Copyright (c) 1997-2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PluginXML.h"
#include "ixmldom.h"
#include "xmlutil.h"

#define	PLUGIN_HE(hr) \
	((HE_SUCCEEDED(hr)) ? PEEK_PLUGIN_SUCCESS : PEEK_PLUGIN_FAILURE)


// =============================================================================
//		Tags
// =============================================================================

namespace Tags {
	const CStringW	kTrue( L"True" );
	const CStringW	kFalse( L"False" );
	const CStringW	kEnabled( L"Enabled" );
};


// =============================================================================
//		CXmlElement
// =============================================================================

CXmlElement::CXmlElement(
	CXmlDocument*	pDocument /*= NULL*/,
	void*			p /*= NULL*/ )
	:	m_pDocument( pDocument )
	,	m_pvElement( NULL )
{
	SetPtr( p );
}

CXmlElement::~CXmlElement()
{
	ReleasePtr();
}


// -----------------------------------------------------------------------------
//		AddChild
// -----------------------------------------------------------------------------

int
CXmlElement::AddChild(
	CString			strLabel,
	CXmlElement&	xmlChild )
{
	HeResult				hr;
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pDocument->m_pvDocument ) );
	if ( (pElement == NULL) || (pDocument == NULL) ) return PEEK_PLUGIN_FAILURE;

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	hr = pDocument->createElement( CHeBSTR( strLabel ), &spElem.p );
	if ( HE_SUCCEEDED( hr ) ) {
		// Append the child element.
		hr = pElement->appendChild( spElem, NULL );
		if ( HE_SUCCEEDED( hr ) ) {
			xmlChild.SetPtr( spElem );
			xmlChild.SetParent( m_pDocument );
		}
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		AddChild
// -----------------------------------------------------------------------------

int
CXmlElement::AddChild(
	CString		strLabel,
	CString		strValue )
{
	HeResult				hr;
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pDocument->m_pvDocument ) );
	if ( (pElement == NULL) || (pDocument == NULL) ) return PEEK_PLUGIN_FAILURE;

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	hr = pDocument->createElement( CHeBSTR( strLabel ), &spElem.p );
	if ( HE_SUCCEEDED( hr ) ) {
		CXmlElement	Elem( m_pDocument, spElem.p );

		// Set the element value.
		Elem.Set( strValue );

		// Append the child element.
		hr = pElement->appendChild( spElem, NULL );
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		AddChildEnabled
// -----------------------------------------------------------------------------

int
CXmlElement::AddChildEnabled(
	CString		strLabel,
	bool		bEnabled )
{
	HeResult				hr;
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pDocument->m_pvDocument ) );
	if ( (pElement == NULL) || (pDocument == NULL) ) return PEEK_PLUGIN_FAILURE;

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	hr = pDocument->createElement( CHeBSTR( strLabel ), &spElem.p );
	if ( HE_SUCCEEDED( hr ) ) {
		// Add the Enabled attribute.
		CHeBSTR		bstrAttribLabel( Tags::kEnabled );
		CHeBSTR		bstrTrue( Tags::kTrue );
		CHeBSTR		bstrFalse( Tags::kFalse );
		HEVARIANT	varValue;
		varValue.vt = HE_VT_BSTR;
		varValue.bstrVal = (bEnabled) ? bstrTrue : bstrFalse;
		spElem->setAttribute( bstrAttribLabel, varValue );

		// Append the child element.
		hr = pElement->appendChild( spElem, NULL );
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		GetChild
// -----------------------------------------------------------------------------

int
CXmlElement::GetChild(
	CString			strLabel,
	CXmlElement&	xmlChild )
{
	HeResult				hr;
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	if ( pElement == NULL ) return PEEK_PLUGIN_FAILURE;

	// Get the child element.
	CHePtr<Xml::IXMLDOMNode>	spNode;
	hr = pElement->selectSingleNode( CHeBSTR( strLabel ), &spNode.p );
	if ( hr == HE_S_FALSE ) {
		return PEEK_PLUGIN_SUCCESS;
	}
	if ( HE_SUCCEEDED( hr ) ) {
		// Get the element interface.
		CHePtr<Xml::IXMLDOMElement>	spElement;
		hr = spNode.QueryInterface( &spElement.p );
		if ( HE_SUCCEEDED( hr ) ) {
			xmlChild.SetPtr( spElement.p );
			xmlChild.SetParent( m_pDocument );
		}
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		GetChildValue
// -----------------------------------------------------------------------------

int
CXmlElement::GetChildValue(
	CString		strLabel,
	UInt32		nValue )
{
	HeResult				hr;
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	if ( pElement == NULL ) return PEEK_PLUGIN_FAILURE;

	// Get the child element.
	CHePtr<Xml::IXMLDOMNode>	spNode;
	hr = pElement->selectSingleNode( CHeBSTR( strLabel ), &spNode.p );
	if ( hr == HE_S_FALSE ) {
		return PEEK_PLUGIN_SUCCESS;
	}
	if ( HE_SUCCEEDED( hr ) ) {
		// Get the element interface.
		CHePtr<Xml::IXMLDOMElement>	spElement;
		hr = spNode.QueryInterface( &spElement.p );
		if ( HE_SUCCEEDED( hr ) ) {
			// Get the value string.
			CHeBSTR		bstrValue;
			hr = spElement->get_text( &bstrValue.m_str );
			if ( HE_SUCCEEDED( hr ) ) {
				// Place the value in a variant for conversion.
				CHeVariant	varValue;
				varValue.vt = HE_VT_BSTR;
				varValue.bstrVal = bstrValue.Detach();

				// Change to requested variant type.
				hr = varValue.ChangeType( HE_VT_UI4 );
				if ( HE_SUCCEEDED( hr ) ) {
					// Set the return value.
					nValue = varValue.ulVal;
				}
			}
		}
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		IsChildEnabled
// -----------------------------------------------------------------------------

int
CXmlElement::IsChildEnabled(
	CString		strLabel,
	bool&		bEnabled )
{
	HeResult				hr;
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	if ( pElement == NULL ) return PEEK_PLUGIN_FAILURE;

	// Get the child element.
	CHePtr<Xml::IXMLDOMNode>	spNode;
	hr = pElement->selectSingleNode( CHeBSTR( strLabel ), &spNode.p );
	if ( hr == HE_S_FALSE ) {
		return PEEK_PLUGIN_SUCCESS;
	}
	if ( HE_SUCCEEDED( hr ) ) {
		// Get the element interface.
		CHePtr<Xml::IXMLDOMElement>	spElement;
		hr = spNode.QueryInterface( &spElement.p );
		if ( HE_SUCCEEDED( hr ) ) {
			// Get the value string.
			CHeVariant	varValue;
			hr = spElement->getAttribute( CHeBSTR( Tags::kEnabled ), &varValue );
			if ( HE_SUCCEEDED( hr ) ) {
				hr = varValue.ChangeType( HE_VT_BSTR );
				if ( HE_SUCCEEDED( hr ) ) {
					CStringW	strValue( varValue.bstrVal );
					// if not true or false, then (atol() != 0);
					if ( toupper( strValue[0] ) == L'T' ) {
						bEnabled = true;
					}
					else if ( toupper( strValue[0] ) == L'F' ) {
						bEnabled = false;
					}
					else {
						UInt32	nValue = _wtol( strValue.GetBuffer() );
						bEnabled = (nValue != 0);
					}
				}
			}
		}
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		ReleasePtr
// -----------------------------------------------------------------------------

void
CXmlElement::ReleasePtr()
{
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	if ( pElement ) {
		pElement->Release();
		m_pvElement = NULL;
	}
}


// -----------------------------------------------------------------------------
//		Set
// -----------------------------------------------------------------------------

int
CXmlElement::Set(
	CString		strValue )
{
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pDocument->m_pvDocument ) );
	if ( pElement == NULL ) return PEEK_PLUGIN_FAILURE;

	// Create the text node.
	HeResult					hr;
	CHePtr<Xml::IXMLDOMText>	spTextNode;
	hr = pDocument->createTextNode( CHeBSTR( strValue ), &spTextNode.p );
	if ( HE_SUCCEEDED( hr ) ) {
		// Append the text node.
		hr = pElement->appendChild( spTextNode, NULL );
	}
	return PLUGIN_HE(hr);
}

int
CXmlElement::Set(
	UInt32	nValue )
{
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( m_pvElement ) );
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pDocument->m_pvDocument ) );
	if ( pElement == NULL ) return PEEK_PLUGIN_FAILURE;

	// Convert the value to text.
	CString	strValue;
	strValue.Format( L"%u", nValue );

	// Create the text node.
	HeResult	hr;
	CHePtr<Xml::IXMLDOMText>	spTextNode;
	hr = pDocument->createTextNode( CHeBSTR( strValue ), &spTextNode.p );
	if ( HE_SUCCEEDED( hr ) ) {
		// Append the text node.
		hr = pElement->appendChild( spTextNode, NULL );
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		SetPtr
// -----------------------------------------------------------------------------

void
CXmlElement::SetPtr(
	void* p )
{
	Xml::IXMLDOMElement*	pElement( reinterpret_cast<Xml::IXMLDOMElement*>( p ) );
	if ( pElement ) {
		m_pvElement = p;
		pElement->AddRef();
	}
}


// =============================================================================
//		CXmlDocument
// =============================================================================

CXmlDocument::CXmlDocument(
	void*	p /*= NULL*/ )
	:	m_pvDocument( NULL )
{
	SetPtr( p );
}

CXmlDocument::~CXmlDocument()
{
	ReleasePtr();
}


// -----------------------------------------------------------------------------
//		CreateElement
// -----------------------------------------------------------------------------

int
CXmlDocument::CreateElement(
	CString			strLabel,
	CXmlElement&	xmlElement )
{
	HeResult				hr;
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pvDocument ) );
	if ( pDocument == NULL ) return PEEK_PLUGIN_FAILURE;

	CHeBSTR	bstrLabel( strLabel );

	CHePtr<Xml::IXMLDOMElement>	spElement;
	hr = pDocument->createElement( bstrLabel, &spElement.p );
	if ( HE_SUCCEEDED( hr ) ) {
		xmlElement.SetParent( this );
		xmlElement.SetPtr( spElement.p );
	}
	return PLUGIN_HE(hr);
}


// -----------------------------------------------------------------------------
//		ReleasePtr
// -----------------------------------------------------------------------------

void
CXmlDocument::ReleasePtr()
{
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pvDocument ) );
	if ( pDocument ) {
		pDocument->Release();
		m_pvDocument = NULL;
	}
}


// -----------------------------------------------------------------------------
//		SetPtr
// -----------------------------------------------------------------------------

void
CXmlDocument::SetPtr(
	void* p )
{
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( p ) );
	if ( pDocument ) {
		m_pvDocument = p;
		pDocument->AddRef();
	}
}

int
CXmlDocument::Save(
	  const wchar_t*	pszFile)
{
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pvDocument ) );
	if ( pDocument == NULL ) return PEEK_PLUGIN_FAILURE;

	HeResult hr;
	
	hr = XmlUtil::SaveXmlFile(pszFile, pDocument);

	return PLUGIN_HE(hr);

}

int			
CXmlDocument::Load(
		const wchar_t*	pszFile)
{
	Xml::IXMLDOMDocument*	pDocument( reinterpret_cast<Xml::IXMLDOMDocument*>( m_pvDocument ) );
	if ( pDocument == NULL ) return PEEK_PLUGIN_FAILURE;

	HeResult hr;
	
	hr = XmlUtil::LoadXmlFile(pszFile, &pDocument);

	return PLUGIN_HE(hr);

}
