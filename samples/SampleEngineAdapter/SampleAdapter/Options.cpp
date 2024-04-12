// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PluginXml.h"
#include "xmlutil.h"


// =============================================================================
//		Constants
// =============================================================================

#define kElementIP		L"IP"
#define	kAttrAddress	L"Address"


// =============================================================================
//		COptions
// =============================================================================

// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

int
COptions::Load(
	CXmlElement&	xmlElement )
{
	// Sanity Checks.
	if ( xmlElement.IsNull() ) return PEEK_PLUGIN_FAILURE;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		Xml::IXMLDOMDocument*	pDocument = NULL;
		CXmlDocument*			pXmlDocument = NULL;
		if ( PEEK_PLUGIN_SUCCESS != xmlElement.GetDocument( &pXmlDocument ) ) return PEEK_PLUGIN_FAILURE;
		if ( !pXmlDocument ) return PEEK_PLUGIN_FAILURE;

		pDocument = reinterpret_cast<Xml::IXMLDOMDocument*>( pXmlDocument->GetDocument() );

		CHePtr<Xml::IXMLDOMNode>	spIpNode;
		HeThrowIfFailed( pDocument->selectSingleNode( CHeBSTR( kElementIP ), &spIpNode.p ) );

		CHeQIPtr<Xml::IXMLDOMElement>	spElem( spIpNode );
		if ( spElem == NULL ) HeThrow( HE_E_FAIL );

		CHeBSTR	hsIP;
		XmlUtil::GetAttribute( spElem, kAttrAddress, hsIP );
		SetIP( inet_addr( CStringA( hsIP ) ) );
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}

int
COptions::Load(
	IHeUnknown* pXmlDoc,
	IHeUnknown*	pXmlNode )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		CHeQIPtr<Xml::IXMLDOMDocument>	sPluginConfigDoc( pXmlDoc );
		CHeQIPtr<Xml::IXMLDOMNode>		spPluginNode( pXmlNode );	
		CHePtr<Xml::IXMLDOMElement>		spIpNode = NULL;

		XmlUtil::GetChildElement( spPluginNode, kElementIP, &spIpNode.p );
		if ( spIpNode ) {
			CHeBSTR hsIP;
			XmlUtil::GetElement( spIpNode, hsIP );
			SetIP( inet_addr( CStringA( hsIP ) ) );
		}					
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		Store
// -----------------------------------------------------------------------------

int
COptions::Store(
	CXmlElement&	xmlElement )
{
	// Sanity Checks.
	if ( xmlElement.IsNull() ) return PEEK_PLUGIN_FAILURE;

	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		Xml::IXMLDOMDocument*	pDocument = NULL;
		CXmlDocument*			pXmlDocument = NULL;
		if ( PEEK_PLUGIN_SUCCESS != xmlElement.GetDocument( &pXmlDocument ) ) return PEEK_PLUGIN_FAILURE;
		if ( !pXmlDocument ) return PEEK_PLUGIN_FAILURE;

		pDocument = reinterpret_cast<Xml::IXMLDOMDocument*>( pXmlDocument->GetDocument() );
					
		CHePtr<Xml::IXMLDOMElement>	spIPElem;
		HeThrowIfFailed( pDocument->createElement( CHeBSTR( kElementIP ), &spIPElem.p ) );
		HeThrowIfFailed( pDocument->appendChild( spIPElem, NULL ) );

		if ( HasIP() ) {
			struct in_addr	ipAddress;
			ipAddress.S_un.S_addr = GetIP();

			CString	csIP( inet_ntoa( ipAddress ) );
			HeThrowIfFailed( spIPElem->setAttribute( CHeBSTR( kAttrAddress ), CHeVariant( csIP ) ) );
		}
	}
	catch ( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}

int
COptions::Store(
	IHeUnknown* pXmlDoc,
	IHeUnknown* pXmlNode )
{
	int	nResult = PEEK_PLUGIN_SUCCESS;

	try {
		CHeQIPtr<Xml::IXMLDOMDocument>	sPluginConfigDoc( pXmlDoc );
		CHeQIPtr<Xml::IXMLDOMNode>		spPluginNode( pXmlNode );	
		CHePtr<Xml::IXMLDOMElement>		spIpNode;

		if ( HasIP() ) {
			struct in_addr ipAddress;
			ipAddress.S_un.S_addr = GetIP();

			CString	csIP( inet_ntoa( ipAddress ) );
			XmlUtil::AddChildElement( sPluginConfigDoc, spPluginNode, kElementIP, CHeBSTR( csIP ), &spIpNode.p );
		}
	}
	catch( ... ) {
		nResult = PEEK_PLUGIN_FAILURE;
	}

	return nResult;
}
