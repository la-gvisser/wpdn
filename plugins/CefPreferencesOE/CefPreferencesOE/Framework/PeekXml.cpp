// =============================================================================
//	PeekXml.cpp
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "imemorystream.h"
#include "PeekStrings.h"
#include "PeekXml.h"


// =============================================================================
//		CPeekXml
// =============================================================================

static wchar_t	s_szXml[] = L"xml";
static wchar_t	s_szVersion[] = L"version=\"1.0\" encoding=\"utf-8\"";

// -----------------------------------------------------------------------------
//		Create
// -----------------------------------------------------------------------------

void
CPeekXml::Create(
	const wchar_t*	inName,
	bool			inAsync /*= false*/,
	bool			inProcInstr /*= false*/ )
{
	Close();

	Peek::ThrowIfFailed( m_spCreatedDoc.CreateInstance( "WPXML.Document" ) );

	if ( inAsync ) {
		Peek::ThrowIfFailed( m_spCreatedDoc->put_async( HE_VAR_FALSE ) );
	}

	if ( inProcInstr ) {
		// Add the standard XML processing instruction.
		CHePtr<Xml::IXMLDOMProcessingInstruction>	spPI;
		Peek::ThrowIfFailed(
				m_spCreatedDoc->createProcessingInstruction( s_szXml, s_szVersion, &spPI.p ) );
		Peek::ThrowIfFailed( m_spCreatedDoc->appendChild( spPI, nullptr ) );
	}

	// Create the child element.
	Peek::ThrowIfFailed( m_spCreatedDoc->createElement( CHeBSTR( inName ), &m_spElement.p ) );

	// Append the child element.
	Peek::ThrowIfFailed( m_spCreatedDoc->appendChild( m_spElement, nullptr ) );
}


// -----------------------------------------------------------------------------
//		CreateChild
// -----------------------------------------------------------------------------

CPeekXml
CPeekXml::CreateChild(
	const wchar_t*	inName )
{
	CPeekXmlDocument	spDocument;
	Peek::ThrowIfFailed( m_spElement->get_ownerDocument( &spDocument.p ) );

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>		spElement;
	Peek::ThrowIfFailed( spDocument->createElement( CHeBSTR( inName ), &spElement.p ) );

	// Append the child element.
	Peek::ThrowIfFailed( m_spElement->appendChild( spElement, nullptr ) );

	CPeekXml	xml( spDocument, spElement );
	return xml;
}


// -----------------------------------------------------------------------------
//		GetAttribute
// -----------------------------------------------------------------------------

bool
CPeekXml::GetAttribute(
	const wchar_t*		inName,
	HeLib::CHeVariant&	outAttribute,
	Helium::HEVARTYPE	inType /*= HE_VT_BSTR*/ )
{
	_ASSERTE( inName );
	if ( !inName ) return false;

	HeResult	hr = m_spElement->getAttribute( CHeBSTR( inName ), &outAttribute );
	if ( hr == HE_S_FALSE ){
		return false; // The attribute doesn't exist.
	}
	Peek::ThrowIfFailed( hr );
	Peek::ThrowIfFailed( outAttribute.ChangeType( inType ) );
	return true;
}

bool
CPeekXml::GetAttribute(
	const wchar_t*			inName,
	CPeekString&	outAttribute )
{
	HeLib::CHeVariant	varAttribute;
	if ( !GetAttribute( inName, varAttribute ) ) return false;

	outAttribute = CPeekString( varAttribute.bstrVal );
	return true;
}


// -----------------------------------------------------------------------------
//		GetChild
// -----------------------------------------------------------------------------

CPeekXml
CPeekXml::GetChild(
	const wchar_t*	inName,
	size_t			inIndex /*= 0*/ )
{
	SInt32		nIndex = static_cast<SInt32>( inIndex );
	CPeekXml	xml;

	if ( IsOpen() ) {
		CHePtr<Xml::IXMLDOMNode>	spNode;
		if ( nIndex == 0 ) {
			Peek::ThrowIfFailed( m_spElement->selectSingleNode( CHeBSTR( inName ), &spNode.p ) );
		}
		else {
			std::wstring		strName( inName );
			CPeekXmlNodeList	spNodeList = m_NodeMap[strName];
			if ( spNodeList == nullptr ) {
				Peek::ThrowIfFailed( m_spElement->selectNodes( CHeBSTR( inName ), &spNodeList.p ) );
				m_NodeMap[strName] = spNodeList;
			}
			if ( spNodeList != nullptr ) {
				SInt32	nCount = 0;
				Peek::ThrowIfFailed( spNodeList->get_length( &nCount ) );
				if ( nCount > nIndex ) {
					Peek::ThrowIfFailed( spNodeList->get_item( nIndex, &spNode.p ) );
				}
			}
		}
		if ( spNode != nullptr ) {
			spNode->QueryInterface( &xml.m_spElement.p );
		}
	}

	return xml;
}


// -----------------------------------------------------------------------------
//		GetChildCount
// -----------------------------------------------------------------------------

UInt32
CPeekXml::GetChildCount(
	const wchar_t*	inName /*= nullptr*/ ) const
{
	SInt32	nCount = 0;

	if ( IsOpen() ) {
		CPeekXmlNodeList	spNodeList;
		if ( inName == nullptr ) {
			Peek::ThrowIfFailed( m_spElement->get_childNodes( &spNodeList.p ) );
		}
		else {
			std::wstring	strName( inName );
			spNodeList = m_NodeMap[strName];
			if ( spNodeList == nullptr ) {
				Peek::ThrowIfFailed( m_spElement->selectNodes( CHeBSTR( inName ), &spNodeList.p ) );
				m_NodeMap[strName] = spNodeList;
			}
		}
		Peek::ThrowIfFailed( spNodeList->get_length( &nCount ) );
	}

	return static_cast<UInt32>( nCount );
}


// -----------------------------------------------------------------------------
//		GetName
// -----------------------------------------------------------------------------

CPeekString
CPeekXml::GetName()
{
	CHeBSTR	bstrName;
	if ( IsOpen() ) {
		Peek::ThrowIfFailed( m_spElement->get_nodeName( &bstrName.m_str ) );
	}
	return CPeekString( bstrName );
}


// -----------------------------------------------------------------------------
//		GetValue
// -----------------------------------------------------------------------------

bool
CPeekXml::GetValue(
	HeLib::CHeVariant&	outValue,
	Helium::HEVARTYPE	inType /*= HE_VT_BSTR*/ )
{
	if ( !IsOpen() ) return false;

	HeResult					hr( HE_E_FAIL );
	CHePtr<Xml::IXMLDOMNode>	spChildNode;
	hr = m_spElement->get_firstChild( &spChildNode.p );
	while ( HE_SUCCEEDED( hr ) && (spChildNode != nullptr) ) {
		Xml::DOMNodeType	type;
		hr = spChildNode->get_nodeType( &type );
		if ( HE_SUCCEEDED( hr ) && (type == Xml::NODE_TEXT) ) {
			CHeBSTR	bstrValue;
			Peek::ThrowIfFailed( spChildNode->get_text( &bstrValue.m_str ) );
			outValue = bstrValue;
			outValue.ChangeType( inType );
			return true;
		}
		CHePtr<Xml::IXMLDOMNode>	spSibling;
		hr = spChildNode->get_nextSibling( &spSibling.p );
		spChildNode = spSibling;
	}

	return false;
}

bool
CPeekXml::GetValue(
	CPeekString&	outValue )
{
	if ( !IsOpen() ) return false;

	HeResult					hr( HE_E_FAIL );
	CHePtr<Xml::IXMLDOMNode>	spChildNode;

	hr = m_spElement->get_firstChild( &spChildNode.p );
	while ( HE_SUCCEEDED( hr ) && (spChildNode != nullptr) ) {
		Xml::DOMNodeType	type;
		hr = spChildNode->get_nodeType( &type );
		if ( HE_SUCCEEDED( hr ) && (type == Xml::NODE_TEXT) ) {
			CHeBSTR	bstrValue;
			Peek::ThrowIfFailed( spChildNode->get_text( &bstrValue.m_str ) );
			outValue = CPeekString( bstrValue );
			return true;
		}
		CHePtr<Xml::IXMLDOMNode>	spSibling;
		hr = spChildNode->get_nextSibling( &spSibling.p );
		spChildNode = spSibling;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		HasValue
// -----------------------------------------------------------------------------

bool
CPeekXml::HasValue() const
{
	if ( !IsOpen() ) return false;

	HeResult					hr( HE_E_FAIL );
	CHePtr<Xml::IXMLDOMNode>	spChildNode;

	hr = m_spElement->get_firstChild( &spChildNode.p );
	if ( HE_FAILED( hr ) || (spChildNode == nullptr ) ) {
		return false;
	}

	while ( HE_SUCCEEDED( hr ) && (spChildNode != nullptr) ) {
		Xml::DOMNodeType	type;
		hr = spChildNode->get_nodeType( &type );
		if ( HE_SUCCEEDED( hr ) && (type == Xml::NODE_TEXT) ) {
			Helium::HEBSTR	bstrValue;
			return (spChildNode->get_text( &bstrValue ) == HE_S_OK);
		}
		CHePtr<Xml::IXMLDOMNode>	spSibling;
		hr = spChildNode->get_nextSibling( &spSibling.p );
		spChildNode = spSibling;
	}

	return false;
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

void
CPeekXml::Load(
	const CPeekStream&	inStream )
{
	_ASSERTE( m_spCreatedDoc == nullptr );
	_ASSERTE( m_spElement == nullptr );

	Peek::ThrowIfFailed( m_spCreatedDoc.CreateInstance( "WPXML.Document" ) );
	Peek::ThrowIfFailed( m_spCreatedDoc->put_async( HE_VAR_FALSE ) );

	CHePtr<Helium::IHePersistStream>	spPersistStream;
	Peek::ThrowIfFailed( m_spCreatedDoc->QueryInterface( &spPersistStream.p ) );

	Peek::ThrowIfFailed( spPersistStream->Load( inStream.GetIStreamPtr() ) );

	m_spElement = nullptr;
	Peek::ThrowIfFailed( m_spCreatedDoc->get_documentElement( &m_spElement.p ) );
}

void
CPeekXml::Load(
	size_t			inLength,
	const UInt8*	inData )
{
	CPeekStream	psXml;
	size_t		nWritten = psXml.Write( inLength, inData );
	_ASSERTE( nWritten == inLength );
	if ( nWritten != inLength ) {
		Peek::Throw( E_FAIL );
	}
	psXml.Rewind();

	Load( psXml );
}

void
CPeekXml::Load(
	const wchar_t*	inFileName,
	const wchar_t*	inRootName )
{
	Close();

	Peek::ThrowIfFailed( m_spCreatedDoc.CreateInstance( "WPXML.Document" ) );
	Peek::ThrowIfFailed( m_spCreatedDoc->put_async( HE_VAR_FALSE ) );

	CPeekPersistFile	spFile = *this;
	if ( spFile ) {
		spFile->Load( inFileName, FALSE );
		m_spElement = nullptr;
		Peek::ThrowIfFailed( m_spCreatedDoc->get_documentElement( &m_spElement.p ) );

		if ( m_spElement ) {
			CHeBSTR		bstrName;
			m_spElement->get_nodeName( &bstrName.m_str );
			for ( int i = 0; i < 3; i++ ) {
				if ( bstrName == inRootName ) break;

				CHePtr<Xml::IXMLDOMNode>	spNode;
				Peek::ThrowIfFailed( m_spElement->selectSingleNode( CHeBSTR( inRootName ), &spNode.p ) );
				if ( spNode ) {
					spNode->QueryInterface( &m_spElement.p );
					m_spElement->get_nodeName( &bstrName.m_str );
				}
			}
		}
	}
}


// -----------------------------------------------------------------------------
//		Save
// -----------------------------------------------------------------------------

void
CPeekXml::Save(
	Helium::IHeStream*	outStream ) const
{
	CPeekXmlDocument	spDocument;
	Peek::ThrowIfFailed( m_spElement->get_ownerDocument( &spDocument.p ) );

	CHePtr<Helium::IHePersistStream>	spPersistStream;
	Peek::ThrowIfFailed( spDocument->QueryInterface( &spPersistStream.p ) );

	Peek::ThrowIfFailed( spPersistStream->Save( outStream, true ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute
// -----------------------------------------------------------------------------

void
CPeekXml::SetAttribute(
	const wchar_t*				inName,
	const HeLib::CHeVariant&	inAttribute )
{
	_ASSERTE( m_spElement );
	if ( m_spElement == nullptr ) return;
	_ASSERTE( inName );
	if ( inName == nullptr ) return;

	Peek::ThrowIfFailed( m_spElement->setAttribute( CHeBSTR( inName ), inAttribute ) );
}

void
CPeekXml::SetAttribute(
	const wchar_t*			inName,
	const Helium::HEBSTR*	inAttribute )
{
	_ASSERTE( m_spElement );
	if ( m_spElement == nullptr ) return;
	_ASSERTE( inName );
	if ( inName == nullptr ) return;

	Helium::HEVARIANT	varAttribute;
	varAttribute.vt = Helium::HE_VT_BSTR;
	varAttribute.pbstrVal = const_cast<Helium::HEBSTR*>( inAttribute );
	Peek::ThrowIfFailed( m_spElement->setAttribute( CHeBSTR( inName ), varAttribute ) );
}


// -----------------------------------------------------------------------------
//		SetValue
// -----------------------------------------------------------------------------

void
CPeekXml::SetValue(
	const HeLib::CHeVariant&	inValue )
{
	HeLib::CHeVariant	varString( inValue );
	Peek::ThrowIfFailed( varString.ChangeType( Helium::HE_VT_BSTR ) );
	SetValue( varString.bstrVal );
}

void
CPeekXml::SetValue(
	const Helium::HEBSTR	inValue )
{
	// consider deleting the current value.
	if ( HasValue() ) return;

	_ASSERTE( IsValid() );
	if ( IsNotValid() ) return;

	CPeekXmlDocument	spDocument;
	Peek::ThrowIfFailed( m_spElement->get_ownerDocument( &spDocument.p ) );

	CHePtr<Xml::IXMLDOMText>	spTextNode;
	Peek::ThrowIfFailed( spDocument->createTextNode( inValue, &spTextNode.p ) );
	Peek::ThrowIfFailed( m_spElement->appendChild( spTextNode, nullptr ) );
}
