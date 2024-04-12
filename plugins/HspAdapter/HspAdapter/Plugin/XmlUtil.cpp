// =============================================================================
//	xmlutil.cpp
// =============================================================================
//	Copyright (c) 2003-2008. WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "XmlUtil.h"
#include "encodehex.h"

using namespace Helium;
using namespace HeLib;
using namespace EncodeHex;

namespace XmlUtil
{


// -----------------------------------------------------------------------------
//		CreateXmlDocument
// -----------------------------------------------------------------------------

HeResult
CreateXmlDocument(
	Xml::IXMLDOMDocument**	ppXmlDoc,
	bool					bAddPI )
{
	if ( ppXmlDoc == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr;

	// Create an XML doc.
	CHePtr<Xml::IXMLDOMDocument>	spXmlDoc;
	hr = spXmlDoc.CreateInstance( "WPXML.Document" );
	if ( HE_FAILED(hr) ) return hr;

	// Set synchronous by default.
	hr = spXmlDoc->put_async( HE_VAR_FALSE );
	if ( HE_FAILED(hr) ) return hr;

	if ( bAddPI )
	{
		// Add the standard XML processing instruction.
		CHePtr<Xml::IXMLDOMProcessingInstruction>	spPI;
		hr = spXmlDoc->createProcessingInstruction( CHeBSTR( L"xml" ),
			CHeBSTR( L"version=\"1.0\" encoding=\"utf-8\"" ), &spPI.p );
		if ( HE_FAILED(hr) ) return hr;
		
		hr = spXmlDoc->appendChild( spPI, nullptr );
		if ( HE_FAILED(hr) ) return hr;
	}

	return spXmlDoc.CopyTo( ppXmlDoc );
}


// -----------------------------------------------------------------------------
//		LoadXmlFile
// -----------------------------------------------------------------------------

HeResult
LoadXmlFile(
	const wchar_t*			pszFile,
	Xml::IXMLDOMDocument**	ppXmlDoc )
{
	// Sanity checks.
	if ( pszFile == nullptr ) return HE_E_NULL_POINTER;
	if ( ppXmlDoc == nullptr ) return HE_E_NULL_POINTER;

	*ppXmlDoc = nullptr;

	HeResult	hr;

	// Create an XML doc.
	CHePtr<Xml::IXMLDOMDocument>	spXmlDoc;
	hr = spXmlDoc.CreateInstance( "WPXML.Document" );
	if ( HE_FAILED(hr) ) return hr;

	// Set synchronous.
	hr = spXmlDoc->put_async( HE_VAR_FALSE );
	if ( HE_FAILED(hr) ) return hr;

	// Load the XML file.
	HEVARBOOL	bSuccess = HE_VAR_FALSE;
	hr = spXmlDoc->load( CHeVariant( pszFile ), &bSuccess );
	if ( HE_FAILED(hr) ) return hr;
	if ( (hr != HE_S_OK) || (bSuccess != HE_VAR_TRUE) ) return HE_E_FAIL;

	return spXmlDoc.CopyTo( ppXmlDoc );
}


// -----------------------------------------------------------------------------
//		SaveXmlFile
// -----------------------------------------------------------------------------

HeResult
SaveXmlFile(
	const wchar_t*			pszFile,
	Xml::IXMLDOMDocument*	pXmlDoc )
{
	// Sanity checks.
	if ( pszFile == nullptr ) return HE_E_NULL_POINTER;
	if ( pXmlDoc == nullptr ) return HE_E_NULL_POINTER;

	// Save the XML document to a file.
	return pXmlDoc->save( CHeVariant( pszFile ) );
}


// -----------------------------------------------------------------------------
//		PrettyFormatXml
// -----------------------------------------------------------------------------

HeResult
PrettyFormatXml(
	Xml::IXMLDOMDocument*	pDoc,
	const wchar_t*			pszIndent )
{
	// Sanity checks.
	if ( pDoc == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try
	{
		// Default to a tab indent if one wasn't specified.
		std::wstring	strIndent;
		if ( pszIndent != nullptr ) strIndent = pszIndent;
		if ( strIndent.empty() )
		{
			strIndent = L"\t";
		}

		size_t	indentOrigLen = 0;
		hr = PrettyFormatXml( pDoc, strIndent, indentOrigLen );
	}
	HE_CATCH(hr)

	return hr;
}


// -----------------------------------------------------------------------------
//		PrettyFormatXml
// -----------------------------------------------------------------------------

HeResult
PrettyFormatXml(
	Xml::IXMLDOMNode*	pNode,
	std::wstring&		strIndent,
	size_t&				indentOrigLen )
{
	// Sanity checks.
	if ( pNode == nullptr ) return HE_E_NULL_POINTER;

	HeResult	hr = HE_S_OK;

	try
	{
		if ( indentOrigLen == 0 )
		{
			indentOrigLen = strIndent.size();
		}

		CHePtr<Xml::IXMLDOMNode>	spChildNode;
		hr = pNode->get_firstChild( &spChildNode.p );
		if ( hr == HE_S_OK )
		{
			CHePtr<Xml::IXMLDOMDocument>	spDoc;
			HeThrowIfFailed( spChildNode->get_ownerDocument( &spDoc.p ) );

			// Get ready for some convoluted logic...

			Xml::DOMNodeType	typeChildNode;
			HeThrowIfFailed( spChildNode->get_nodeType( &typeChildNode ) );
			const bool			bFirstChildTextNode = (typeChildNode == Xml::NODE_TEXT);

			bool	bLastWasTextNode = false;
			bool	bLastWasPI       = false;
			while ( (hr == HE_S_OK) && (spChildNode != nullptr) )
			{
				HeThrowIfFailed( spChildNode->get_nodeType( &typeChildNode ) );

				if ( (typeChildNode == Xml::NODE_ELEMENT) && !bLastWasTextNode )
				{
					CHePtr<Xml::IXMLDOMNode>	spNewNode;
					HeThrowIfFailed( spDoc->createNode( CHeVariant( Xml::NODE_TEXT ), nullptr, nullptr, &spNewNode.p ) );
					std::wstring	strValue = L"\r\n";
					if ( !bLastWasPI ) strValue += strIndent;
					HeThrowIfFailed( spNewNode->put_nodeValue( CHeVariant( strValue ) ) );
					CHePtr<Xml::IXMLDOMNode>	spN;
					HeThrowIfFailed( pNode->insertBefore( spNewNode, CHeVariant( spChildNode ), &spN.p ) );
				}

				std::wstring	strNewIndent = strIndent;
				if ( !bLastWasPI ) strNewIndent += strIndent.substr( 0, indentOrigLen );

				HeThrowIfFailed( PrettyFormatXml( spChildNode, strNewIndent, indentOrigLen ) );

				bLastWasTextNode = (typeChildNode == Xml::NODE_TEXT);
				bLastWasPI       = (typeChildNode == Xml::NODE_PROCESSING_INSTRUCTION);

				CHePtr<Xml::IXMLDOMNode>	spSibling;
				(void) spChildNode->get_nextSibling( &spSibling.p );
				spChildNode = spSibling;
			}

			if ( !bFirstChildTextNode )
			{
				CHePtr<Xml::IXMLDOMNode>	spNewNode;
				HeThrowIfFailed( spDoc->createNode( CHeVariant( Xml::NODE_TEXT ), nullptr, nullptr, &spNewNode.p ) );
				std::wstring				strValue = L"\r\n";
				if ( strIndent.size() > indentOrigLen )
				{
					strValue += strIndent.substr( 0, strIndent.size() - indentOrigLen );
				}
				HeThrowIfFailed( spNewNode->put_nodeValue( CHeVariant( strValue ) ) );
				HeThrowIfFailed( pNode->appendChild( spNewNode, nullptr ) );
			}
		}

		hr = HE_S_OK;
	}
	HE_CATCH(hr)

	return hr;
}


// -----------------------------------------------------------------------------
//		GetAttribute
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	HEVARTYPE				vt,
	CHeVariant&				varVal )
{
	HeThrowIfNull( pElem );
	HeThrowIfNull( pszName );

	// Get the attribute.
	HeResult	hr = pElem->getAttribute( CHeBSTR( pszName ), &varVal );
	if ( hr == HE_S_FALSE )
	{
		// The attribute doesn't exist.
		return false;
	}
	HeThrowIfFailed( hr );

	// Change to requested variant type.
	HeThrowIfFailed( varVal.ChangeType( vt ) );

	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (CHeBSTR)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	CHeBSTR&				bstrVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;
	bstrVal.Attach( varVal.bstrVal );
	varVal.vt = HE_VT_EMPTY;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (std::wstring)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	std::wstring&			strVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HeThrowIfFailed( HeHEBSTRToString( varVal.bstrVal, strVal ) );
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (std::string)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	std::string&			strVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HeThrowIfFailed( HeHEBSTRToString( varVal.bstrVal, strVal ) );
	return true;
}


#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)

// -----------------------------------------------------------------------------
//		GetAttribute (CString)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	CString&				strVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;
	strVal = CString( varVal.bstrVal, static_cast<int>(HeStringLen( varVal.bstrVal )) );
	return true;
}

#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */


// -----------------------------------------------------------------------------
//		GetAttribute (SInt8)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt8&					cVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_I1, varVal ) )
		return false;
	cVal = varVal.cVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt8)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt8&					bVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_UI1, varVal ) )
		return false;
	bVal = varVal.bVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt16)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt16&					iVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_I2, varVal ) )
		return false;
	iVal = varVal.iVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt16)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt16&					uiVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_UI2, varVal ) )
		return false;
	uiVal = varVal.uiVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt32)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt32&					lVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_I4, varVal ) )
		return false;
	lVal = varVal.lVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt32)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt32&					ulVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_UI4, varVal ) )
		return false;
	ulVal = varVal.ulVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt64)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt64&					llVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_I8, varVal ) )
		return false;
	llVal = varVal.llVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt64)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt64&					ullVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_UI8, varVal ) )
		return false;
	ullVal = varVal.ullVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (bool)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	bool&					boolVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_I4, varVal ) )
		return false;
	boolVal = (varVal.lVal != 0);
	return true;
}


#ifndef OPT_64BITINTTYPES

// -----------------------------------------------------------------------------
//		GetAttribute (int)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	int&					intVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_INT, varVal ) )
		return false;
	intVal = varVal.intVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (unsigned int)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	unsigned int&			uintVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_UINT, varVal ) )
		return false;
	uintVal = varVal.uintVal;
	return true;
}

#endif /* OPT_64BITINTTYPES */


// -----------------------------------------------------------------------------
//		GetAttribute (double)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	double&					dblVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_R8, varVal ) )
		return false;
	dblVal = varVal.dblVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (HeID)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	HeID&					guidVal )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;
	if ( !guidVal.Parse( varVal.bstrVal ) )
		HeThrow( HE_E_FAIL );
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt8*,size_t&)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt8*					pData,
	size_t&					nDataLen )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HexDecode( pData, nDataLen, varVal );
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (std::vector<UInt8>&)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	std::vector<UInt8>&		data )
{
	CHeVariant	varVal;
	if ( !GetAttribute( pElem, pszName, HE_VT_BSTR, varVal ) )
		return false;

	size_t	cb = HexDecodeGetRequiredLength( HeStringLen( varVal.bstrVal ) );

	if ( cb > 0 )
	{
		data.resize( cb );
		HexDecode( &data[0], cb, varVal );
	}
	else
	{
		data.clear();
	}

	return true;
}


// -----------------------------------------------------------------------------
//		SetAttribute
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const CHeVariant&		varVal )
{
	HeThrowIfNull( pElem );
	HeThrowIfNull( pszName );

	// Set the attribute.
	HeThrowIfFailed( pElem->setAttribute( CHeBSTR( pszName ), varVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (CHeBSTR)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const CHeBSTR&			bstrVal )
{
	HeThrowIfNull( pElem );
	HeThrowIfNull( pszName );

	// Set the attribute (avoid copying the variant).
	HEVARIANT	varVal;
	varVal.vt      = HE_VT_BSTR;
	varVal.bstrVal = bstrVal;
	HeThrowIfFailed( pElem->setAttribute( CHeBSTR( pszName ), varVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (std::wstring)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const std::wstring&		strVal )
{
	SetAttribute( pElem, pszName, CHeBSTR( strVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (std::string)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const std::string&		strVal )
{
	SetAttribute( pElem, pszName, CHeBSTR( strVal ) );
}


#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)

// -----------------------------------------------------------------------------
//		SetAttribute (CString)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const CString&			strVal )
{
	SetAttribute( pElem, pszName, CHeBSTR( strVal.GetLength(), strVal ) );
}

#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */


// -----------------------------------------------------------------------------
//		SetAttribute (const wchar_t*)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const wchar_t*			pszVal )
{
	SetAttribute( pElem, pszName, CHeBSTR( pszVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (const char*)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const char*				pszVal )
{
	SetAttribute( pElem, pszName, CHeBSTR( pszVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt8)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt8					cVal )
{
	CHeVariant	varVal( cVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt8)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt8					bVal )
{
	CHeVariant	varVal( bVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt16)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt16					iVal )
{
	CHeVariant	varVal( iVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt16)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt16					uiVal )
{
	CHeVariant	varVal( uiVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt32)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt32					lVal )
{
	CHeVariant	varVal( lVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt32)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt32					ulVal )
{
	CHeVariant	varVal( ulVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt64)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	SInt64					llVal )
{
	CHeVariant	varVal( llVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt64)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	UInt64					ullVal )
{
	CHeVariant	varVal( ullVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (bool)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	bool					boolVal )
{
	CHeVariant	varVal( boolVal ? 1 : 0 );
	SetAttribute( pElem, pszName, varVal );
}


#ifndef OPT_64BITINTTYPES

// -----------------------------------------------------------------------------
//		SetAttribute (int)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	int						intVal )
{
	CHeVariant	varVal( intVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (unsigned int)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	unsigned int			uintVal )
{
	CHeVariant	varVal( uintVal );
	SetAttribute( pElem, pszName, varVal );
}

#endif /* OPT_64BITINTTYPES */


// -----------------------------------------------------------------------------
//		SetAttribute (double)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	double					dblVal )
{
	CHeVariant	varVal( dblVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (HeID)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const HeID&				guidVal )
{
	SetAttribute( pElem, pszName, CHeBSTR( guidVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (const UInt8*,size_t)
// -----------------------------------------------------------------------------

void
SetAttribute(
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			pszName,
	const UInt8*			pData,
	size_t					nDataLen )
{
	CHeVariant	varVal;
	HexEncode( pData, nDataLen, varVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		GetElement
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	HEVARTYPE				vt,
	CHeVariant&				varVal )
{
	HeThrowIfNull( pElem );

	// Get the value string.
	CHeBSTR		bstrVal;
	HeResult	hr = pElem->get_text( &bstrVal.m_str );
	if ( hr == HE_S_FALSE ) hr = HE_E_FAIL;
	HeThrowIfFailed( hr );

	// Copy to the variant.
	varVal.Clear();
	varVal.vt      = HE_VT_BSTR;
	varVal.bstrVal = bstrVal.Detach();

	if ( vt != HE_VT_BSTR )
	{
		// Change to requested variant type.
		HeThrowIfFailed( varVal.ChangeType( vt ) );
	}
}


// -----------------------------------------------------------------------------
//		GetElement (CHeBSTR)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	CHeBSTR&				bstrVal )
{
	HeThrowIfNull( pElem );

	// Get the value string.
	bstrVal.Empty();
	HeResult	hr = pElem->get_text( &bstrVal.m_str );
	if ( hr == HE_S_FALSE ) hr = HE_E_FAIL;
	HeThrowIfFailed( hr );
}


// -----------------------------------------------------------------------------
//		GetElement (std::wstring)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	std::wstring&			strVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_BSTR, varVal );
	HeThrowIfFailed( HeHEBSTRToString( varVal.bstrVal, strVal ) );
}


// -----------------------------------------------------------------------------
//		GetElement (std::string)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	std::string&			strVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_BSTR, varVal );
	HeThrowIfFailed( HeHEBSTRToString( varVal.bstrVal, strVal ) );
}


#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)

// -----------------------------------------------------------------------------
//		GetElement (CString)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	CString&				strVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_BSTR, varVal );
	strVal = CString( varVal.bstrVal, static_cast<int>(HeStringLen( varVal.bstrVal )) );
}

#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */


// -----------------------------------------------------------------------------
//		GetElement (SInt8)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	SInt8&					cVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_I1, varVal );
	cVal = varVal.cVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt8)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	UInt8&					bVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_UI1, varVal );
	bVal = varVal.bVal;
}


// -----------------------------------------------------------------------------
//		GetElement (SInt16)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	SInt16&					iVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_I2, varVal );
	iVal = varVal.iVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt16)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	UInt16&					uiVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_UI2, varVal );
	uiVal = varVal.uiVal;
}


// -----------------------------------------------------------------------------
//		GetElement (SInt32)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	SInt32&					lVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_I4, varVal );
	lVal = varVal.lVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt32)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	UInt32&					ulVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_UI4, varVal );
	ulVal = varVal.ulVal;
}


// -----------------------------------------------------------------------------
//		GetElement (SInt64)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	SInt64&					llVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_I8, varVal );
	llVal = varVal.llVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt64)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	UInt64&					ullVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_UI8, varVal );
	ullVal = varVal.ullVal;
}


// -----------------------------------------------------------------------------
//		GetElement (bool)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	bool&					boolVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_I4, varVal );
	boolVal = (varVal.lVal != 0);
}


#ifndef OPT_64BITINTTYPES

// -----------------------------------------------------------------------------
//		GetElement (int)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	int&					intVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_INT, varVal );
	intVal = varVal.intVal;
}


// -----------------------------------------------------------------------------
//		GetElement (unsigned int)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	unsigned int&			uintVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_UINT, varVal );
	uintVal = varVal.uintVal;
}

#endif /* OPT_64BITINTTYPES */


// -----------------------------------------------------------------------------
//		GetElement (double)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	double&					dblVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_R8, varVal );
	dblVal = varVal.dblVal;
}


// -----------------------------------------------------------------------------
//		GetElement (HeID)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	HeID&					guidVal )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_BSTR, varVal );
	HeThrowIf( !guidVal.Parse( varVal.bstrVal ) );
}


// -----------------------------------------------------------------------------
//		GetElement (UInt8*,size_t&)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	UInt8*					pData,
	size_t&					nDataLen )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_BSTR, varVal );
	HexDecode( pData, nDataLen, varVal );
}


// -----------------------------------------------------------------------------
//		GetElement (std::vector<UInt8>&)
// -----------------------------------------------------------------------------

void
GetElement(
	Xml::IXMLDOMElement*	pElem,
	std::vector<UInt8>&		data )
{
	CHeVariant	varVal;
	GetElement( pElem, HE_VT_BSTR, varVal );

	size_t	cb = HexDecodeGetRequiredLength( HeStringLen( varVal.bstrVal ) );

	if ( cb > 0 )
	{
		data.resize( cb );
		HexDecode( &data[0], cb, varVal );
	}
	else
	{
		data.clear();
	}
}


// -----------------------------------------------------------------------------
//		SetElement
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	IHeUnknown*				pObj,
	bool					bWriteClsidAttribute,	// default = true
	bool					bClearDirty )			// default = true
{
	// Sanity checks.
	HeThrowIfNull( pDoc );
	HeThrowIfNull( pElem );
	HeThrowIfNull( pObj );

	// Get the XML persistence interface.
	CHePtr<IHePersistXml>	spPersistXml;
	HeThrowIfFailed( pObj->QueryInterface( &spPersistXml.p ) );

	if ( bWriteClsidAttribute )
	{
		// Get the object class id.
		HeCID	clsid;
		HeThrowIfFailed( spPersistXml->GetClassID( &clsid ) );

		// Set the class id attribute.
		SetAttribute( pElem, L"clsid", clsid );
	}

	// Save to XML.
	HeThrowIfFailed( spPersistXml->Save( pDoc, pElem, bClearDirty ) );
}


// -----------------------------------------------------------------------------
//		SetElement
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const CHeVariant&		varVal )
{
	HeThrowIfNull( pDoc );
	HeThrowIfNull( pElem );

	if ( varVal.vt == HE_VT_BSTR )
	{
		// Create the text node.
		CHePtr<Xml::IXMLDOMText>	spTextNode;
		HeThrowIfFailed( pDoc->createTextNode( varVal.bstrVal, &spTextNode.p ) );
		
		// Append the text node.
		HeThrowIfFailed( pElem->appendChild( spTextNode, nullptr ) );
	}
	else
	{
		// Convert to string.
		CHeVariant	varBstrVal;
		HeThrowIfFailed( varBstrVal.ChangeType( HE_VT_BSTR, &varVal ) );

		// Create the text node.
		CHePtr<Xml::IXMLDOMText>	spTextNode;
		HeThrowIfFailed( pDoc->createTextNode( varBstrVal.bstrVal, &spTextNode.p ) );
		
		// Append the text node.
		HeThrowIfFailed( pElem->appendChild( spTextNode, nullptr ) );
	}
}


// -----------------------------------------------------------------------------
//		SetElement (CHeBSTR)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const CHeBSTR&			bstrVal )
{
	if ( pDoc == nullptr ) HeThrow( HE_E_NULL_POINTER );
	if ( pElem == nullptr ) HeThrow( HE_E_NULL_POINTER );

	// Create the text node.
	CHePtr<Xml::IXMLDOMText>	spTextNode;
	HeThrowIfFailed( pDoc->createTextNode( bstrVal, &spTextNode.p ) );
	
	// Append the text node.
	HeThrowIfFailed( pElem->appendChild( spTextNode, nullptr ) );
}


// -----------------------------------------------------------------------------
//		SetElement (std::wstring)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const std::wstring&		strVal )
{
	SetElement( pDoc, pElem, CHeBSTR( strVal ) );
}


// -----------------------------------------------------------------------------
//		SetElement (std::string)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const std::string&		strVal )
{
	SetElement( pDoc, pElem, CHeBSTR( strVal ) );
}


#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)

// -----------------------------------------------------------------------------
//		SetElement (CString)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const CString&			strVal )
{
	SetElement( pDoc, pElem, CHeBSTR( strVal.GetLength(), strVal ) );
}

#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */


// -----------------------------------------------------------------------------
//		SetElement (const wchar_t*)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const wchar_t*			psz )
{
	SetElement( pDoc, pElem, CHeBSTR( psz ) );
}


// -----------------------------------------------------------------------------
//		SetElement (const char*)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const char*				psz )
{
	SetElement( pDoc, pElem, CHeBSTR( psz ) );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt8)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	SInt8					cVal )
{
	CHeVariant	varVal( cVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt8)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	UInt8					bVal )
{
	CHeVariant	varVal( bVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt16)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	SInt16					iVal )
{
	CHeVariant	varVal( iVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt16)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	UInt16					uiVal )
{
	CHeVariant	varVal( uiVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt32)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	SInt32					lVal )
{
	CHeVariant	varVal( lVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt32)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	UInt32					ulVal )
{
	CHeVariant	varVal( ulVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt64)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	SInt64					llVal )
{
	CHeVariant	varVal( llVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt64)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	UInt64					ullVal )
{
	CHeVariant	varVal( ullVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (bool)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	bool					boolVal )
{
	CHeVariant	varVal( boolVal ? 1 : 0 );
	SetElement( pDoc, pElem, varVal );
}


#ifndef OPT_64BITINTTYPES

// -----------------------------------------------------------------------------
//		SetElement (int)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	int						intVal )
{
	CHeVariant	varVal( intVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (unsigned int)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	unsigned int			uintVal )
{
	CHeVariant	varVal( uintVal );
	SetElement( pDoc, pElem, varVal );
}

#endif /* OPT_64BITINTTYPES */


// -----------------------------------------------------------------------------
//		SetElement (double)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	double					dblVal )
{
	CHeVariant	varVal( dblVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (HeID)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const HeID&				guidVal )
{
	SetElement( pDoc, pElem, CHeBSTR( guidVal ) );
}


// -----------------------------------------------------------------------------
//		SetElement (const UInt8*,size_t)
// -----------------------------------------------------------------------------

void
SetElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMElement*	pElem,
	const UInt8*			pData,
	size_t					nDataLen )
{
	CHeVariant	varVal;
	HexEncode( pData, nDataLen, varVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		GetChildElement
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	Xml::IXMLDOMElement**	ppElem )
{
	// Sanity checks.
	HeThrowIfNull( pNode );
	HeThrowIfNull( pszName );
	HeThrowIfNull( ppElem );

	// Get the child node.
	CHePtr<Xml::IXMLDOMNode>	spChildNode;
	HeResult	hr = pNode->selectSingleNode( CHeBSTR( pszName ), &spChildNode.p );
	if ( hr == HE_S_FALSE )
	{
		// Node doesn't exist.
		return false;
	}
	HeThrowIfFailed( hr );

	// Get the element interface.
	HeThrowIfFailed( spChildNode.QueryInterface( ppElem ) );

	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	HEVARTYPE			vt,
	CHeVariant&			varVal )
{
	// Get the element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	if ( !GetChildElement( pNode, pszName, &spElem.p ) )
	{
		return false;
	}

	// Get the element value.
	GetElement( spElem, vt, varVal );

	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (CHeBSTR)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	CHeBSTR&			bstrVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;
	bstrVal.Attach( varVal.bstrVal );
	varVal.vt = HE_VT_EMPTY;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (std::wstring)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	std::wstring&		strVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HeThrowIfFailed( HeHEBSTRToString( varVal.bstrVal, strVal ) );
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (std::string)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	std::string&		strVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HeThrowIfFailed( HeHEBSTRToString( varVal.bstrVal, strVal ) );
	return true;
}


#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)

// -----------------------------------------------------------------------------
//		GetChildElement (CString)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	CString&	strVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;
	strVal = CString( varVal.bstrVal, static_cast<int>(HeStringLen( varVal.bstrVal )) );
	return true;
}

#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */


// -----------------------------------------------------------------------------
//		GetChildElement (SInt8)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	SInt8&				cVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_I1, varVal ) )
		return false;
	cVal = varVal.cVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt8)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	UInt8&				bVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_UI1, varVal ) )
		return false;
	bVal = varVal.bVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt16)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	SInt16&				iVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_I2, varVal ) )
		return false;
	iVal = varVal.iVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt16)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	UInt16&				uiVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_UI2, varVal ) )
		return false;
	uiVal = varVal.uiVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt32)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	SInt32&				lVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_I4, varVal ) )
		return false;
	lVal = varVal.lVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt32)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	UInt32&				ulVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_UI4, varVal ) )
		return false;
	ulVal = varVal.ulVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt64)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	SInt64&				llVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_I8, varVal ) )
		return false;
	llVal = varVal.llVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt64)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	UInt64&				ullVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_UI8, varVal ) )
		return false;
	ullVal = varVal.ullVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (bool)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	bool&				boolVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_I4, varVal ) )
		return false;
	boolVal = (varVal.lVal != 0);
	return true;
}


#ifndef OPT_64BITINTTYPES

// -----------------------------------------------------------------------------
//		GetChildElement (int)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	int&				intVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_INT, varVal ) )
		return false;
	intVal = varVal.intVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (unsigned int)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	unsigned int&		uintVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_UINT, varVal ) )
		return false;
	uintVal = varVal.uintVal;
	return true;
}

#endif /* OPT_64BITINTTYPES */


// -----------------------------------------------------------------------------
//		GetChildElement (double)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	double&				dblVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_R8, varVal ) )
		return false;
	dblVal = varVal.dblVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (HeID)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	HeID&				guidVal )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HeThrowIf( !guidVal.Parse( varVal.bstrVal ) );
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt8*,size_t&)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	UInt8*				pData,
	size_t&				nDataLen )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;
	HexDecode( pData, nDataLen, varVal );
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (std::vector<UInt8>&)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	Xml::IXMLDOMNode*	pNode,
	const wchar_t*		pszName,
	std::vector<UInt8>&	data )
{
	CHeVariant	varVal;
	if ( !GetChildElement( pNode, pszName, HE_VT_BSTR, varVal ) )
		return false;

	size_t	cb = HexDecodeGetRequiredLength( HeStringLen( varVal.bstrVal ) );

	if ( cb > 0 )
	{
		data.resize( cb );
		HexDecode( &data[0], cb, varVal );
	}
	else
	{
		data.clear();
	}

	return true;
}


// -----------------------------------------------------------------------------
//		AddChildElement
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	Xml::IXMLDOMElement**	ppElem )
{
	HeThrowIfNull( pDoc );
	HeThrowIfNull( pNode );
	HeThrowIfNull( pszName );

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	HeThrowIfFailed( pDoc->createElement( CHeBSTR( pszName ), &spElem.p ) );

	// Append the child element.
	HeThrowIfFailed( pNode->appendChild( spElem, nullptr ) );

	if ( ppElem != nullptr )
	{
		HeThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	IHeUnknown*				pObj,
	Xml::IXMLDOMElement**	ppElem,					// default = nullptr
	bool					bWriteClsidAttribute,	// default = true
	bool					bClearDirty )			// default = true
{
	// Sanity checks.
	HeThrowIfNull( pDoc );
	HeThrowIfNull( pNode );
	HeThrowIfNull( pszName );

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	HeThrowIfFailed( pDoc->createElement( CHeBSTR( pszName ), &spElem.p ) );

	// Set the object.
	SetElement( pDoc, spElem, pObj, bWriteClsidAttribute, bClearDirty );

	// Append the child element.
	HeThrowIfFailed( pNode->appendChild( spElem, nullptr ) );

	if ( ppElem != nullptr )
	{
		HeThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const CHeVariant&		varVal,
	Xml::IXMLDOMElement**	ppElem )
{
	// Sanity checks.
	HeThrowIfNull( pDoc );
	HeThrowIfNull( pNode );
	HeThrowIfNull( pszName );

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	HeThrowIfFailed( pDoc->createElement( CHeBSTR( pszName ), &spElem.p ) );

	// Set the element value.
	SetElement( pDoc, spElem, varVal );

	// Append the child element.
	HeThrowIfFailed( pNode->appendChild( spElem, nullptr ) );

	if ( ppElem != nullptr )
	{
		HeThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement (CHeBSTR)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const CHeBSTR&			bstrVal,
	Xml::IXMLDOMElement**	ppElem )
{
	// Sanity checks.
	HeThrowIfNull( pDoc );
	HeThrowIfNull( pNode );
	HeThrowIfNull( pszName );

	// Create the child element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	HeThrowIfFailed( pDoc->createElement( CHeBSTR( pszName ), &spElem.p ) );

	// Set the element value.
	SetElement( pDoc, spElem, bstrVal );

	// Append the child element.
	HeThrowIfFailed( pNode->appendChild( spElem, nullptr ) );

	if ( ppElem != nullptr )
	{
		HeThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement (std::wstring)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const std::wstring&		strVal,
	Xml::IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, CHeBSTR( strVal ), ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (std::string)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const std::string&		strVal,
	Xml::IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, CHeBSTR( strVal ), ppElem );
}


#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)

// -----------------------------------------------------------------------------
//		AddChildElement (CString)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const CString&			strVal,
	Xml::IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, CHeBSTR( strVal.GetLength(), strVal ), ppElem );
}

#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */


// -----------------------------------------------------------------------------
//		AddChildElement (const wchar_t*)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const wchar_t*			pszVal,
	Xml::IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, CHeBSTR( pszVal ), ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (const char*)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const char*				pszVal,
	Xml::IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, CHeBSTR( pszVal ), ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt8)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	SInt8					cVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( cVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt8)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	UInt8					bVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( bVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt16)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	SInt16					iVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( iVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt16)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	UInt16					uiVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( uiVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt32)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	SInt32					lVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( lVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt32)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	UInt32					ulVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( ulVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt64)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	SInt64					llVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( llVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt64)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	UInt64					ullVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( ullVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (bool)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	bool					boolVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( boolVal ? 1 : 0 );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


#ifndef OPT_64BITINTTYPES

// -----------------------------------------------------------------------------
//		AddChildElement (int)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	int						intVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( intVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (unsigned int)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	unsigned int			uintVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( uintVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}

#endif /* OPT_64BITINTTYPES */


// -----------------------------------------------------------------------------
//		AddChildElement (double)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	double					dblVal,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal( dblVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (HeID)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const HeID&				guidVal,
	Xml::IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, CHeBSTR( guidVal ), ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (const UInt8*,size_t)
// -----------------------------------------------------------------------------

void
AddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	const UInt8*			pData,
	size_t					nDataLen,
	Xml::IXMLDOMElement**	ppElem )
{
	CHeVariant	varVal;
	HexEncode( pData, nDataLen, varVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		DuplicateXmlNode
// -----------------------------------------------------------------------------

HeResult
DuplicateXmlNode(
	Xml::IXMLDOMDocument*	pSrcDoc,
	Xml::IXMLDOMNode*		pSrcNode,
	Xml::IXMLDOMDocument*	pDstDoc,
	Xml::IXMLDOMNode*		pDstNode,
	Xml::IXMLDOMElement**	ppNewElem )
{
	// Based on CXMLParser::BuildDOM
	// 
	if ( (pSrcDoc == nullptr) || (pSrcNode == nullptr) ||
			(pDstDoc == nullptr) || (pDstNode == nullptr) ) return HE_E_NULL_POINTER;

	HeResult						hr = HE_S_OK;
	CHeQIPtr<Xml::IXMLDOMElement>	spNewElem;

	try
	{
		// Copy the Source Element and the associated attributes.
		CHePtr<Xml::IXMLDOMElement>	spSrcElem;
		Xml::DOMNodeType			SrcNodeType;
		HeThrowIfFailed( pSrcNode->get_nodeType( &SrcNodeType ) );
		if ( SrcNodeType == Xml::NODE_DOCUMENT )
		{
			hr = ((Xml::IXMLDOMDocument*)pSrcNode)->get_documentElement( &spSrcElem.p );
		}
		else if ( SrcNodeType == Xml::NODE_ELEMENT )
		{
			hr = pSrcNode->QueryInterface( &spSrcElem.p );
		}
		else
		{
			hr = HE_E_FAIL;
		}
		HeThrowIfFailed( hr );

		CHeBSTR	bstrRootName;
		HeThrowIfFailed( spSrcElem->get_nodeName( &bstrRootName.m_str ) );
		HeThrowIf( bstrRootName.Length() == 0 );
		HeThrowIfFailed( pDstDoc->createElement( bstrRootName, &spNewElem.p ) );

		{
			// Based on CXMLParser::CopyElement
			// Copy all the attributes.
			CHePtr<Xml::IXMLDOMNamedNodeMap>	spAttribMap;
			hr = spSrcElem->get_attributes( &spAttribMap.p );
			if ( HE_SUCCEEDED(hr) && (spAttribMap != nullptr) )
			{
				SInt32	nAttribCount;
				HeThrowIfFailed( spAttribMap->get_length( &nAttribCount ) );
				for ( SInt32 i = 0; i < nAttribCount; i++ )
				{
					CHePtr<Xml::IXMLDOMNode>	spAttrib;
					hr = spAttribMap->get_item( i, &spAttrib.p );
					if ( HE_FAILED(hr) || (spAttrib == nullptr) ) continue;

					CHeBSTR		bstrAttribName;
					(void) spAttrib->get_nodeName( &bstrAttribName.m_str );

					CHeVariant	varAttribValue;
					(void) spAttrib->get_nodeValue( &varAttribValue );

					(void) spNewElem->setAttribute( bstrAttribName, varAttribValue );
				}
			}
		}

		CHeBSTR	bstrChildValue;
		{
			// Get all the children under the source node.
			CHePtr<Xml::IXMLDOMNodeList>	spNodeList;
			HeThrowIfFailed( spSrcElem->get_childNodes( &spNodeList.p ) );

			SInt32	nChildCount;
			spNodeList->get_length( &nChildCount );
			for ( SInt32 i = 0; i < nChildCount; i++ )
			{
				CHePtr<Xml::IXMLDOMNode>	spChildNode;
				hr = spNodeList->get_item( i, &spChildNode.p );
				if ( HE_FAILED(hr) ) continue;

				Xml::DOMNodeType	ChildNodeType;
				HeThrowIfFailed( spChildNode->get_nodeType( &ChildNodeType ) );
				if ( ChildNodeType == Xml::NODE_ELEMENT )
				{
					HeThrowIfFailed( DuplicateXmlNode( pSrcDoc, spChildNode, pDstDoc, spNewElem, nullptr ) );
				}
				else if ( (ChildNodeType == Xml::NODE_TEXT) || (ChildNodeType == Xml::NODE_CDATA_SECTION) )
				{
					CHeVariant	ChildValue;
					hr = spChildNode->get_nodeValue( &ChildValue );
					if ( HE_SUCCEEDED(hr) )
					{
						bstrChildValue += ChildValue.bstrVal;
					}
				}
			}
		}
		if ( bstrChildValue.Length() > 0 )
		{
			CHePtr<Xml::IXMLDOMText>	spNewText;
			HeThrowIfFailed( pDstDoc->createTextNode( bstrChildValue, &spNewText.p ) );
			HeThrowIfFailed( spNewElem->appendChild( spNewText, nullptr ) );
		}

		HeThrowIfFailed( pDstNode->appendChild( spNewElem, nullptr ) );
	}
	HE_CATCH(hr)

	if ( HE_SUCCEEDED(hr) && (ppNewElem != nullptr) )
	{
		hr = spNewElem.CopyTo( ppNewElem );
	}

	return hr;
}


// =============================================================================
//	ElementIterator
// =============================================================================

// -----------------------------------------------------------------------------
//		ElementIterator(Xml::IXMLDOMNode*,const wchar_t*)
// -----------------------------------------------------------------------------

ElementIterator::ElementIterator(
	Xml::IXMLDOMNode*	pParentNode,
	const wchar_t*		pszChildTagName )
{
	CHeQIPtr<Xml::IXMLDOMElement>	spParentElement( pParentNode );
	HeThrowIfNull( spParentElement );

	// getElementsByTagName returns an empty list if no elements.
	HeResult	hr = spParentElement->getElementsByTagName(
					CHeBSTR( pszChildTagName ), &m_spNodeList.p );
	HeThrowIfNotOK( hr );
}


// -----------------------------------------------------------------------------
//		GetCount
// -----------------------------------------------------------------------------

SInt32
ElementIterator::GetCount() const
{
	HeThrowIfNull( m_spNodeList );

	SInt32		cItems;
	HeResult	hr = m_spNodeList->get_length( &cItems );
	HeThrowIfFailed( hr );

	return cItems;
}


// -----------------------------------------------------------------------------
//		GetNext
// -----------------------------------------------------------------------------

bool
ElementIterator::GetNext(
	Xml::IXMLDOMElement**	ppNextElem )
{
	HeThrowIfNull( m_spNodeList );

	CHePtr<Xml::IXMLDOMNode>	spNextNode;
	HeResult	hr = m_spNodeList->nextNode( &spNextNode.p );
	HeThrowIfFailed( hr );
	if ( hr == HE_S_OK )
	{
		hr = spNextNode.QueryInterface( ppNextElem );
		return HE_SUCCEEDED(hr);
	}

	return false;
}


} /*namespace XmlUtil */
