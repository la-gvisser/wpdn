// ============================================================================
//	PeekXml.cpp
//		implementation of the CPeekXml class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PeekXml.h"

#ifndef CATCH_HR
#ifdef _AFX
#define CATCH_HR(_hr) \
	catch ( ATL::CAtlException& e ) { \
		_hr = e; \
	} catch ( COleException* e ) { \
		_hr = e->m_sc; \
		e->Delete(); \
	} catch ( CMemoryException* e ) { \
		_hr = E_OUTOFMEMORY; \
		e->Delete(); \
	} catch ( CException* e ) { \
		_hr = E_UNEXPECTED; \
		e->Delete(); \
	} catch ( ... ) { \
		_hr = E_UNEXPECTED; \
	}
#else
#define CATCH_HR(_hr) \
	catch ( ATL::CAtlException& e ) { \
		_hr = e; \
	} catch ( ... ) { \
		_hr = E_UNEXPECTED; \
	}
#endif
#endif


// ----------------------------------------------------------------------------
//		FailedMSXMLResult
//
// Check the HRESULT return code from various MSXML interfaces that return
// S_FALSE instead of an error code.
//
// Notes:
// - S_FALSE is converted to E_FAIL
// ----------------------------------------------------------------------------

inline bool
FailedMSXMLResult(
	HRESULT&	hResult )
{
	if ( hResult == S_FALSE ) {
		hResult = E_FAIL;		// Change S_FALSE to E_FAIL.
	}
	return FAILED( hResult );
}


// ============================================================================
//		CPeekXml
// ============================================================================

namespace CPeekXml
{
	const CComBSTR	True( _T( "True" ) );
	const CComBSTR	False( _T( "False" ) );
	const CComBSTR	Enabled( _T( "Enabled" ) );

	bool
	StringCompare(
		CString	strA,
		CString	strB )
	{
		if ( (strA.GetLength() == 1) || (strB.GetLength() == 1) ) {
			return (_tcsnicmp( strA, strB, 1 ) == 0);
		}
		return (_tcsicmp( strA, strB ) == 0);
	}
}


// ============================================================================
//		CPeekXmlElement
// ============================================================================

// ----------------------------------------------------------------------------
//		AddAttribute
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::AddAttribute(
	const CComBSTR&	inName,
	const CComBSTR&	inValue )
{
	if ( m_spElement == NULL ) return false;

	try {
		HRESULT	hResult;

		VARIANT		varValue;
		varValue.vt = VT_BSTR;
		varValue.bstrVal = inValue;

		hResult = m_spElement->setAttribute( inName, varValue );
		if ( FailedMSXMLResult( hResult ) ) return false;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		AddChild
// ----------------------------------------------------------------------------

CPeekXmlElement
CPeekXmlElement::AddChild(
	PCTSTR	inName,
	PCTSTR	inValue /*= NULL*/ )
{
	if ( inName == NULL ) return NULL;

	CComPtr<IXMLDOMElement>	spElement;

	try {
		HRESULT	hResult;

		// Get the parent document.
		CComPtr<IXMLDOMDocument>	spDocument;
		hResult = m_spElement->get_ownerDocument( &spDocument );
		if ( FAILED( hResult ) ) return NULL;

		// Create the child element.
		hResult = spDocument->createElement( CComBSTR( inName ), &spElement );
		if ( FAILED( hResult ) ) return NULL;

		if ( inValue != NULL ) {
			// Set the value.
			hResult = spElement->put_text( CComBSTR( inValue ) );
			if ( FAILED( hResult ) ) return NULL;
		}

		// Append the child element.
		hResult = m_spElement->appendChild( spElement, NULL );
		if ( FAILED( hResult ) ) return NULL;
	}
	catch ( ... ) {
		return NULL;
	}

	return CPeekXmlElement( spElement );
}

CPeekXmlElement
CPeekXmlElement::AddChild(
	PCTSTR	inName,
	UInt32	inValue )
{
	CString	strValue;

	strValue.Format( _T( "%u" ), inValue );
	return AddChild( inName, strValue );
}


// ----------------------------------------------------------------------------
//		AddChildEnable
// ----------------------------------------------------------------------------

CPeekXmlElement
CPeekXmlElement::AddChildEnable(
	PCTSTR	inName,
	bool	inEnabled )
{
	CPeekXmlElement	spElement = AddChild( inName );
	if ( spElement != NULL ) {
		spElement.SetEnabled( inEnabled );
	}

	return CPeekXmlElement( spElement );
}


// ----------------------------------------------------------------------------
//		GetAttributeValue
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::GetAttributeValue(
	PCTSTR		inName,
	CString&	outValue ) const
{
	if ( m_spElement == NULL ) return false;

	try {
		HRESULT	hResult;

		// Get the attribute's value.
		CComVariantEx	varValue;
		hResult = m_spElement->getAttribute( CComBSTR( inName ), &varValue );
		if ( FailedMSXMLResult( hResult ) ) return false;

		// Change the value's type.
		hResult = varValue.ChangeType( VT_BSTR );
		if ( FailedMSXMLResult( hResult ) ) return false;

		outValue = varValue.bstrVal;
	}
	catch ( ... ) {
		return NULL;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		GetChild
// ----------------------------------------------------------------------------

CPeekXmlElement
CPeekXmlElement::GetChild(
	PCTSTR	inName,
	SInt32	inIndex /*= 0*/ ) const
{
	if ( inName == NULL ) return NULL;

	CComPtr<IXMLDOMElement>	spElement;

	try {
		HRESULT	hResult;

		CComPtr<IXMLDOMNode>	spNode;
		if ( inIndex == 0 ) {
			// Get the first matching node.
			hResult = m_spElement->selectSingleNode( CComBSTR( inName ), &spNode );
			if ( FAILED( hResult ) ) return NULL;
		}
		else {
			// Get the matching nodes.
			CComPtr<IXMLDOMNodeList>	spNodeList;
			hResult = m_spElement->selectNodes( CComBSTR( inName ), &spNodeList );
			if ( FAILED( hResult ) ) return NULL;

			// Get the requested node.
			hResult = spNodeList->get_item( inIndex, &spNode );
			if ( FAILED( hResult ) ) return NULL;
		}

		// Convert the node to an element.
		hResult = spNode->QueryInterface( &spElement );
		if ( FAILED( hResult ) ) return NULL;
	}
	catch ( ... ) {
		return NULL;
	}

	return CPeekXmlElement( spElement );
}


// ----------------------------------------------------------------------------
//		GetChildCount
// ----------------------------------------------------------------------------

UInt32
CPeekXmlElement::GetChildCount(
	PCTSTR	inName ) const
{
	if ( inName == NULL ) return NULL;

	long	nCount = 0;

	try {
		HRESULT	hResult;

		CComPtr<IXMLDOMNodeList>	spChildList;
		hResult = m_spElement->get_childNodes( &spChildList );
		if ( FailedMSXMLResult( hResult ) ) return 0;

		hResult = spChildList->get_length( &nCount );
		if ( FailedMSXMLResult( hResult ) ) return 0;
	}
	catch ( ... ) {
		return 0;
	}

	return (UInt32) nCount;
}


// ----------------------------------------------------------------------------
//		GetChildValue
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::GetChildValue(
	PCTSTR	inName,
	UInt32&	outValue,
	SInt32	inIndex /*= 0*/ ) const
{
	if ( inName == NULL ) return NULL;

	try {
		HRESULT	hResult;

		CComPtr<IXMLDOMNode>	spNode;
		if ( inIndex == 0 ) {
			// Get the first matching node.
			hResult = m_spElement->selectSingleNode( CComBSTR( inName ), &spNode );
			if ( FAILED( hResult ) ) return NULL;
		}
		else {
			// Get the matching nodes.
			CComPtr<IXMLDOMNodeList>	spNodeList;
			hResult = m_spElement->selectNodes( CComBSTR( inName ), &spNodeList );
			if ( FAILED( hResult ) ) return NULL;

			// Get the requested node.
			hResult = spNodeList->get_item( inIndex, &spNode );
			if ( FAILED( hResult ) ) return NULL;
		}

		// Convert the node to an element.
		CComPtr<IXMLDOMElement>	spElement;
		hResult = spNode->QueryInterface( &spElement );
		if ( FAILED( hResult ) ) return false;

		// Get the value as a 4-byte unsigned integer.
		CComVariantEx	varValue;
		bool			bResult;
		bResult = GetValue( spElement, VT_UI4, varValue );
		if ( !bResult ) return false;

		outValue = varValue.ulVal;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		GetValue
// ----------------------------------------------------------------------------

CString
CPeekXmlElement::GetValue(
	IXMLDOMElement*	inElement )
{
	CString	strValue;
	try {
		if ( inElement != NULL ) {
			BSTR	bstrValue;
			HRESULT hResult = inElement->get_text( &bstrValue );
			if ( FailedMSXMLResult( hResult ) ) return strValue;

			strValue = bstrValue;
		}
	}
	catch ( ... ) {
		return strValue;
	}

	return strValue;
}

bool
CPeekXmlElement::GetValue(
	IXMLDOMElement*		inElement,
	VARTYPE				inType,
	CComVariantEx&		outValue )
{
	if ( inElement == NULL ) return false;

	HRESULT	hResult = E_FAIL;
	try {
		// Get the value string.
		outValue.Clear();
		outValue.vt = VT_BSTR;
		hResult = inElement->get_text( &outValue.bstrVal );
		if ( FailedMSXMLResult( hResult ) ) return false;

		// Change to requested variant type.
		hResult = outValue.ChangeType( inType );
		if ( FailedMSXMLResult( hResult ) ) return false;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}

bool
CPeekXmlElement::GetValue(
	UInt32&	outValue ) const
{
	CComVariantEx	varValue;
	bool			bResult;
	bResult = GetValue( m_spElement, VT_UI4, varValue );
	if ( !bResult ) return false;

	outValue = varValue.uintVal;

	return true;
}


// ----------------------------------------------------------------------------
//		IsEnabled
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::IsEnabled() const
{
	CString	strValue;

	if ( !GetAttributeValue( CPeekXml::Enabled, strValue ) ) return false;
	return CPeekXml::StringCompare( strValue, CString( CPeekXml::True ) );
}


// ----------------------------------------------------------------------------
//		IsChildEnabled
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::IsChildEnabled(
	PCTSTR	inName,
	bool	/*inDefault*/ /*= false*/,
	SInt32	inIndex /*= 0*/ ) const
{
	CPeekXmlElement	Child = GetChild( inName, inIndex );
	if ( Child == NULL ) return false;

	return Child.IsEnabled();
}


// ----------------------------------------------------------------------------
//		PrettyFormat
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::PrettyFormat(
	IXMLDOMNode*	inNode,
	PCTSTR			inIndent,
	int&			ioIndentLength )
{
	// Sanity checks.
	if ( inNode == NULL ) return false;

	HRESULT	hResult = S_OK;

	try {
		CString	strIndent( inIndent );
		if ( ioIndentLength == 0 ) {
			ioIndentLength = strIndent.GetLength();
		}

		CComPtr<IXMLDOMNode>	spChildNode;
		hResult = inNode->get_firstChild( &spChildNode );
		if ( SUCCEEDED( hResult ) ) {
			CComPtr<IXMLDOMDocument>	spDoc;
			hResult = inNode->get_ownerDocument( &spDoc );
			if ( FailedMSXMLResult( hResult ) ) return false;

			while ( SUCCEEDED( hResult ) && (spChildNode != NULL) ) {
				CString	strNewIndent = strIndent + strIndent.Left( ioIndentLength );
				hResult = PrettyFormat( spChildNode, strNewIndent, ioIndentLength );
				if ( FailedMSXMLResult( hResult ) ) return false;

				DOMNodeType	type;
				hResult = inNode->get_nodeType( &type );
				if ( FailedMSXMLResult( hResult ) ) return false;

				if ( type == NODE_ELEMENT ) {
					CComPtr<IXMLDOMNode>	spNewNode;
					hResult = spDoc->createNode( CComVariant( NODE_TEXT ), NULL, NULL, &spNewNode );
					if ( FailedMSXMLResult( hResult ) ) return false;

					CString	strValue = _T( "\r\n" ) + strIndent;
					hResult = spNewNode->put_nodeValue( CComVariant( strValue ) );
					if ( FailedMSXMLResult( hResult ) ) return false;

					CComPtr<IXMLDOMNode>	spN;
					hResult = inNode->insertBefore( spNewNode, CComVariant( spChildNode ), &spN );
					if ( FailedMSXMLResult( hResult ) ) return false;
				}

				CComPtr<IXMLDOMNode>	spSibling;
				hResult = spChildNode->get_nextSibling( &spSibling );
				spChildNode = spSibling;
			}

			DOMNodeType	type;
			hResult = inNode->get_nodeType( &type );
			if ( FailedMSXMLResult( hResult ) ) return false;

			if ( type == NODE_ELEMENT ) {
				CComPtr<IXMLDOMNode>	spNewNode;
				hResult = spDoc->createNode( CComVariant( NODE_TEXT ), NULL, NULL, &spNewNode );
				if ( FailedMSXMLResult( hResult ) ) return false;

				CString	strValue = _T( "\r\n" ) + strIndent.Left( strIndent.GetLength() - 1 );
				hResult = spNewNode->put_nodeValue( CComVariant( strValue ) );
				if ( FailedMSXMLResult( hResult ) ) return false;

				hResult = inNode->appendChild( spNewNode, NULL );
				if ( FailedMSXMLResult( hResult ) ) return false;
			}
		}
		else {
			hResult = S_OK;
		}
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		SetEnabled
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::SetEnabled(
	bool	inEnabled )
{
	return AddAttribute(
		CPeekXml::Enabled,
		(inEnabled) ? CPeekXml::True : CPeekXml::False );
}


// ----------------------------------------------------------------------------
//		SetValue
// ----------------------------------------------------------------------------

bool
CPeekXmlElement::SetValue(
	PCTSTR	inValue )
{
	if ( m_spElement ) {
		HRESULT	hResult = m_spElement->put_text( CComBSTR( inValue ) );
		return FailedMSXMLResult( hResult );
	}
	return false;
}


// ============================================================================
//		CPeekXmlDocument
// ============================================================================

CPeekXmlDocument::CPeekXmlDocument()
{
	HRESULT	hResult;

	hResult = m_spDocument.CoCreateInstance( __uuidof( DOMDocument ) );
	FailedMSXMLResult( hResult );
}


// ----------------------------------------------------------------------------
//		AddRootElement
// ----------------------------------------------------------------------------

CPeekXmlElement
CPeekXmlDocument::AddRootElement(
	PCTSTR	inName )
{
	if ( inName == NULL ) return NULL;

	HRESULT	hResult;
	CComPtr<IXMLDOMElement>	spRootElement;

	try {
		CComPtr<IXMLDOMNode>	spXmlNode;
		hResult = m_spDocument->QueryInterface( &spXmlNode );
		ASSERT( SUCCEEDED( hResult ) );
		if ( FailedMSXMLResult( hResult ) ) return NULL;

		hResult = m_spDocument->createElement( CComBSTR( inName ), &spRootElement );
		ASSERT( SUCCEEDED( hResult ) );
		if ( FailedMSXMLResult( hResult ) ) return NULL;

		CComPtr<IXMLDOMNode>	spRootNode;
		hResult = spXmlNode->appendChild( spRootElement, &spRootNode );
		ASSERT( SUCCEEDED( hResult ) );
		if ( FailedMSXMLResult( hResult ) ) return NULL;
	}
	catch ( ... ) {
		return NULL;
	}

	return CPeekXmlElement( spRootElement );
}


// ----------------------------------------------------------------------------
//		Format
// ----------------------------------------------------------------------------

CString
CPeekXmlDocument::Format()
{
	CString	strXml;

	try {
		HRESULT	hResult;

		CComPtr<IXMLDOMElement> spRootElement;
		hResult = m_spDocument->get_documentElement( &spRootElement );
		if ( FailedMSXMLResult( hResult ) ) return strXml;

		BSTR	bstrXml;
		hResult = spRootElement->get_xml( &bstrXml );
		if ( FailedMSXMLResult( hResult ) ) return strXml;

		strXml = bstrXml;
	}
	catch ( ... ) {
		return strXml;
	}

	return strXml;
}


// ----------------------------------------------------------------------------
//		GetRootElement
// ----------------------------------------------------------------------------

CPeekXmlElement
CPeekXmlDocument::GetRootElement()
{
	CComPtr<IXMLDOMElement> spRootElement;

	try {
		HRESULT	hResult;
		hResult = m_spDocument->get_documentElement( &spRootElement );
	}
	catch ( ... ) {
		return NULL;
	}

	return CPeekXmlElement( spRootElement );
}


// ----------------------------------------------------------------------------
//		LoadFile
// ----------------------------------------------------------------------------

bool
CPeekXmlDocument::LoadFile(
	PCTSTR	inFileName )
{
	// Sanity checks.
	if ( inFileName == NULL ) return false;
	if ( m_spDocument == NULL ) return false;

	try {
		HRESULT	hResult;

		// Set synchronous.
		hResult = m_spDocument->put_async( VARIANT_FALSE );
		if ( FailedMSXMLResult( hResult ) ) return false;

		// Load the XML file.
		VARIANT_BOOL	bSuccess = VARIANT_FALSE;
		hResult = m_spDocument->load( CComVariant( inFileName ), &bSuccess );
		if ( FailedMSXMLResult( hResult ) ) return false;
		if ( bSuccess != VARIANT_TRUE ) return false;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		Parse
// ----------------------------------------------------------------------------

bool
CPeekXmlDocument::Parse(
	PCTSTR	inXml )
{
	CStringA	strXml( inXml );
	if ( strXml.IsEmpty() ) return false;

	try {
		HRESULT	hResult;

		// Load the XML string.
		SAFEARRAY *psaXml;
		psaXml = SafeArrayCreateVector( VT_UI1, 0, strXml.GetLength() );

		char *pXml;
		hResult = SafeArrayAccessData( psaXml, (void **)&pXml );
		if ( FailedMSXMLResult( hResult ) ) return false;

		memcpy( pXml, strXml.GetBuffer(), strXml.GetLength() );
		hResult = SafeArrayUnaccessData( psaXml );
		if ( FailedMSXMLResult( hResult ) ) return false;

		CComVariant		vtXml( psaXml );
		VARIANT_BOOL	vtResult;
		hResult = m_spDocument->load( vtXml, &vtResult );
		if ( FailedMSXMLResult( hResult ) ) {
#ifdef _DEBUG
			CComPtr<IXMLDOMParseError>	spParseError;
			HRESULT						hr;

			hr = m_spDocument->get_parseError( &spParseError );
			if ( !FailedMSXMLResult( hr ) ) {
				long	nErrCode = 0;
				BSTR	bstrErr;
				spParseError->get_errorCode( &nErrCode );
				spParseError->get_reason( &bstrErr );
			}
#endif
			return false;
		}
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		PrettyFormat
// ----------------------------------------------------------------------------

bool
CPeekXmlDocument::PrettyFormat(
	PCTSTR	inIndent /*= NULL*/ )
{
	try {
		CPeekXmlElement	Root;
		Root = GetRootElement();
		if ( Root == NULL ) return false;

		// Default to a tab indent if one wasn't specified.
		CString strIndent = inIndent;
		if ( strIndent.IsEmpty() ) {
			strIndent = _T( "\t" );
		}
		int nIndentLen = 0;
		bool bResult = Root.PrettyFormat( strIndent, nIndentLen );
		if ( !bResult ) return false;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}


// ----------------------------------------------------------------------------
//		StoreFile
// ----------------------------------------------------------------------------

bool
CPeekXmlDocument::StoreFile(
	PCTSTR	inFileName )
{
	// Sanity checks.
	if ( inFileName == NULL ) return false;
	if ( m_spDocument == NULL ) return false;

	// Save the XML document to a file.
	try {
		HRESULT	hResult;
		hResult = m_spDocument->save( CComVariant( CT2COLE( inFileName ) ) );
		if ( FailedMSXMLResult( hResult ) ) return false;
	}
	catch ( ... ) {
		return false;
	}

	return true;
}
