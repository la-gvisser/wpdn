// =============================================================================
//	XmlUtil.cpp
// =============================================================================
//	Copyright (c) 2003-2008. WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "XmlUtil.h"
#include "CatchHR.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

using namespace ATL;

namespace XmlUtil
{

// -----------------------------------------------------------------------------
//		CreateXmlDocument
// -----------------------------------------------------------------------------

HRESULT
CreateXmlDocument(
	IXMLDOMDocument**	ppXmlDoc,
	bool				bAddPI )
{
	if ( ppXmlDoc == NULL ) return E_POINTER;

	HRESULT	hr;

	// Create an XML doc.
	CComPtr<IXMLDOMDocument>	spXmlDoc;
#if 1
	hr = spXmlDoc.CoCreateInstance( __uuidof(DOMDocument) );
	if ( FAILED(hr) ) return hr;
#else
	hr = spXmlDoc.CoCreateInstance( __uuidof(DOMDocument40) );
	if ( FAILED(hr) )
	{
		ATLTRACE( _T("Could not create MSXML 4.0 DOM document\n") );
		hr = spXmlDoc.CoCreateInstance( __uuidof(DOMDocument) );
		if ( FAILED(hr) ) return hr;
	}
#endif

	// Set synchronous by default.
	hr = spXmlDoc->put_async( VARIANT_FALSE );
	if ( FAILED(hr) ) return hr;

	if ( bAddPI )
	{
		// Add the standard XML processing instruction.
		CComPtr<IXMLDOMProcessingInstruction>	spPI;
		hr = spXmlDoc->createProcessingInstruction( CComBSTR( OLESTR("xml") ),
			CComBSTR( OLESTR("version=\"1.0\" encoding=\"utf-8\"") ), &spPI );
		if ( FAILED(hr) ) return hr;
	
		hr = spXmlDoc->appendChild( spPI, NULL );
		if ( FAILED(hr) ) return hr;
	}

	return spXmlDoc.CopyTo( ppXmlDoc );
}


// -----------------------------------------------------------------------------
//		LoadXmlFile
// -----------------------------------------------------------------------------

HRESULT
LoadXmlFile(
	LPCTSTR				pszFile,
	IXMLDOMDocument**	ppXmlDoc )
{
	// Sanity checks.
	if ( pszFile == NULL ) return E_POINTER;
	if ( ppXmlDoc == NULL ) return E_POINTER;

	HRESULT	hr;

	// Create an XML doc.
	CComPtr<IXMLDOMDocument>	spXmlDoc;
#if 1
	hr = spXmlDoc.CoCreateInstance( __uuidof(DOMDocument) );
	if ( FAILED(hr) ) return hr;
#else
	hr = spXmlDoc.CoCreateInstance( __uuidof(DOMDocument40) );
	if ( FAILED(hr) )
	{
		ATLTRACE( _T("Could not create MSXML 4.0 DOM document\n") );
		hr = spXmlDoc.CoCreateInstance( __uuidof(DOMDocument) );
		if ( FAILED(hr) ) return hr;
	}
#endif

	// Set synchronous.
	hr = spXmlDoc->put_async( VARIANT_FALSE );
	if ( FAILED(hr) ) return hr;

	// Load the XML file.
	VARIANT_BOOL	bSuccess = VARIANT_FALSE;
	hr = spXmlDoc->load( CComVariantEx( pszFile ), &bSuccess );
	if ( hr != S_OK ) return E_FAIL;
	if ( bSuccess != VARIANT_TRUE ) return E_FAIL;

	return spXmlDoc.CopyTo( ppXmlDoc );
}


// -----------------------------------------------------------------------------
//		SaveXmlFile
// -----------------------------------------------------------------------------

HRESULT
SaveXmlFile(
	LPCTSTR				pszFile,
	IXMLDOMDocument*	pXmlDoc )
{
	// Sanity checks.
	if ( pszFile == NULL ) return E_POINTER;
	if ( pXmlDoc == NULL ) return E_POINTER;

	// Save the XML document to a file.
	return pXmlDoc->save( CComVariantEx( CT2COLE( pszFile ) ) );
}


// -----------------------------------------------------------------------------
//		GetElement
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*		pElem,
	VARTYPE				vt,
	CComVariantEx&		varVal )
{
	ThrowIfNull( pElem );

	// Get the value string.
	varVal.Clear();
	varVal.vt = VT_BSTR;
	HRESULT	hr = pElem->get_text( &varVal.bstrVal );
	if ( hr == S_FALSE ) hr = E_FAIL;
	ThrowIfFailed( hr );

	// Change to requested variant type.
	ThrowIfFailed( varVal.ChangeType( vt ) );
}


// -----------------------------------------------------------------------------
//		GetElement (CComBSTR)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	CComBSTR&		bstrVal )
{
	ThrowIfNull( pElem );

	CComVariantEx	varVal;
	GetElement( pElem, VT_BSTR, varVal );
	bstrVal = varVal.bstrVal;
}


// -----------------------------------------------------------------------------
//		GetElement (CString)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	CString&		strVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_BSTR, varVal );
	strVal = varVal.bstrVal;
}


// -----------------------------------------------------------------------------
//		GetElement (std::string)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	std::string&	strVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_BSTR, varVal );
	if ( varVal.bstrVal != NULL )
	{
		strVal = CW2A( varVal.bstrVal );
	}
	else
	{
		strVal.clear();
	}
}


// -----------------------------------------------------------------------------
//		GetElement (std::wstring)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	std::wstring&	strVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_BSTR, varVal );
	if ( varVal.bstrVal != NULL )
	{
		strVal = varVal.bstrVal;
	}
	else
	{
		strVal.clear();
	}
}


// -----------------------------------------------------------------------------
//		GetElement (SInt8)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	SInt8&			cVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_I1, varVal );
	cVal = varVal.cVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt8)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	UInt8&			bVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_UI1, varVal );
	bVal = varVal.bVal;
}


// -----------------------------------------------------------------------------
//		GetElement (SInt16)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	SInt16&			iVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_I2, varVal );
	iVal = varVal.iVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt16)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	UInt16&			uiVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_UI2, varVal );
	uiVal = varVal.uiVal;
}


// -----------------------------------------------------------------------------
//		GetElement (SInt32)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	SInt32&			lVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_I4, varVal );
	lVal = varVal.lVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt32)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	UInt32&			ulVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_UI4, varVal );
	ulVal = varVal.ulVal;
}


// -----------------------------------------------------------------------------
//		GetElement (SInt64)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	SInt64&			llVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_I8, varVal );
	llVal = varVal.llVal;
}


// -----------------------------------------------------------------------------
//		GetElement (UInt64)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	UInt64&			ullVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_UI8, varVal );
	ullVal = varVal.ullVal;
}


// -----------------------------------------------------------------------------
//		GetElement (bool)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	bool&			boolVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_I4, varVal );
	boolVal = (V_I4(&varVal) != 0);
}


// -----------------------------------------------------------------------------
//		GetElement (int)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	int&			intVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_INT, varVal );
	intVal = varVal.intVal;
}


// -----------------------------------------------------------------------------
//		GetElement (unsigned int)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	unsigned int&	uintVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_UINT, varVal );
	uintVal = varVal.uintVal;
}


// -----------------------------------------------------------------------------
//		GetElement (double)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	double&			dblVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_R8, varVal );
	dblVal = varVal.dblVal;
}


// -----------------------------------------------------------------------------
//		GetElement (GUID)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	GUID&			guidVal )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_BSTR, varVal );
	ThrowIfFailed( ::CLSIDFromString( varVal.bstrVal, &guidVal ) );
}


// -----------------------------------------------------------------------------
//		GetElement (BYTE*,size_t&)
// -----------------------------------------------------------------------------

void
GetElement(
	IXMLDOMElement*	pElem,
	BYTE*			pData,
	size_t&			nDataLen )
{
	CComVariantEx	varVal;
	GetElement( pElem, VT_BSTR, varVal );
	varVal.HexDecode( pData, nDataLen );
}


// ------------------------------------------------------------------------------
//		SetElement
// ------------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	IUnknown*			pObj,
	bool				bWriteClsidAttribute,	// default = true
	bool				bClearDirty )			// default = true
{
	// Sanity checks.
	ThrowIfNull( pDoc );
	ThrowIfNull( pElem );
	ThrowIfNull( pObj );

	// Get the XML persistence interface.
	CComPtr<IPersistXml>	spPersistXml;
	ThrowIfFailed( pObj->QueryInterface( &spPersistXml ) );

	if ( bWriteClsidAttribute )
	{
		// Get the object class id.
		CLSID	clsid;
		ThrowIfFailed( spPersistXml->GetClassID( &clsid ) );

		// Set the class id attribute.
		SetAttribute( pElem, _T("clsid"), clsid );
	}

	// Save to XML.
	ThrowIfFailed( spPersistXml->Save( pDoc, pElem, bClearDirty ) );
}


// -----------------------------------------------------------------------------
//		SetElement
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*		/*pDoc*/,
	IXMLDOMElement*			pElem,
	const CComVariantEx&	varVal )
{
	ThrowIfNull( pElem );

	if ( varVal.vt == VT_BSTR )
	{
		ThrowIfFailed( pElem->put_text( varVal.bstrVal ) );
	}
	else
	{
		CComVariantEx	varBstrVal;
		ThrowIfFailed( varBstrVal.ChangeType( VT_BSTR, &varVal ) );
		ThrowIfFailed( pElem->put_text( varBstrVal.bstrVal ) );
	}
}


// -----------------------------------------------------------------------------
//		SetElement (CComBSTR)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	/*pDoc*/,
	IXMLDOMElement*		pElem,
	const CComBSTR&		bstrVal )
{
	ThrowIfNull( pElem );
	ThrowIfFailed( pElem->put_text( bstrVal ) );
}


// -----------------------------------------------------------------------------
//		SetElement (std::string)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	const std::string&	strVal )
{
	SetElement( pDoc, pElem, strVal.c_str() );
}


// -----------------------------------------------------------------------------
//		SetElement (std::wstring)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	const std::wstring&	strVal )
{
	SetElement( pDoc, pElem, strVal.c_str() );
}


// -----------------------------------------------------------------------------
//		SetElement (LPCOLESTR)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	LPCOLESTR			psz )
{
	CComVariantEx	varVal( psz );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (LPCSTR)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	LPCSTR				psz )
{
	CComVariantEx	varVal( psz );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt8)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	SInt8				cVal )
{
	CComVariantEx	varVal( cVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt8)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	UInt8				bVal )
{
	CComVariantEx	varVal( bVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt16)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	SInt16				iVal )
{
	CComVariantEx	varVal( iVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt16)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	UInt16				uiVal )
{
	CComVariantEx	varVal( uiVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt32)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	SInt32				lVal )
{
	CComVariantEx	varVal( lVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt32)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	UInt32				ulVal )
{
	CComVariantEx	varVal( ulVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (SInt64)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	SInt64				llVal )
{
	CComVariantEx	varVal( llVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (UInt64)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	UInt64				ullVal )
{
	CComVariantEx	varVal( ullVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (bool)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	bool				boolVal )
{
	CComVariantEx	varVal( boolVal ? 1 : 0 );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (int)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	int					intVal )
{
	CComVariantEx	varVal( intVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (unsigned int)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	unsigned int		uintVal )
{
	CComVariantEx	varVal( uintVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (double)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	double				dblVal )
{
	CComVariantEx	varVal( dblVal );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		SetElement (GUID)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	const GUID&			guidVal )
{
	CComBSTR	bstrVal( guidVal );
	bstrVal.ToUpper();
	SetElement( pDoc, pElem, bstrVal );
}


// -----------------------------------------------------------------------------
//		SetElement (const BYTE*,size_t)
// -----------------------------------------------------------------------------

void
SetElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMElement*		pElem,
	const BYTE*			pData,
	size_t				nDataLen )
{
	CComVariantEx	varVal( pData, nDataLen );
	SetElement( pDoc, pElem, varVal );
}


// -----------------------------------------------------------------------------
//		GetChildElement
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	IXMLDOMElement**	ppElem )
{
	// Sanity checks.
	ThrowIfNull( pNode );
	ThrowIfNull( pszName );
	ThrowIfNull( ppElem );

	// Get the child node.
	CComPtr<IXMLDOMNode>	spChildNode;
	HRESULT	hr = pNode->selectSingleNode( CComBSTR( CT2COLE( pszName ) ), &spChildNode );
	if ( hr == S_FALSE )
	{
		// Node doesn't exist.
		return false;
	}
	ThrowIfFailed( hr );

	// Get the element interface.
	ThrowIfFailed( spChildNode->QueryInterface( ppElem ) );

	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	VARTYPE				vt,
	CComVariantEx&		varVal )
{
	// Get the element.
	CComPtr<IXMLDOMElement>	spElem;
	if ( !GetChildElement( pNode, pszName, &spElem ) )
	{
		return false;
	}

	// Get the element value.
	GetElement( spElem, vt, varVal );

	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (CComBSTR)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	CComBSTR&		bstrVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_BSTR, varVal ) )
		return false;
	bstrVal = varVal.bstrVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (CString)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	CString&		strVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_BSTR, varVal ) )
		return false;
	strVal = varVal.bstrVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (std::string)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	std::string&	strVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_BSTR, varVal ) )
		return false;
	if ( varVal.bstrVal != NULL )
	{
		strVal = CW2A( varVal.bstrVal );
	}
	else
	{
		strVal.clear();
	}
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (std::wstring)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	std::wstring&	strVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_BSTR, varVal ) )
		return false;
	if ( varVal.bstrVal != NULL )
	{
		strVal = varVal.bstrVal;
	}
	else
	{
		strVal.clear();
	}
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt8)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	SInt8&			cVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_I1, varVal ) )
		return false;
	cVal = varVal.cVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt8)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	UInt8&			bVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_UI1, varVal ) )
		return false;
	bVal = varVal.bVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt16)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	SInt16&			iVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_I2, varVal ) )
		return false;
	iVal = varVal.iVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt16)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	UInt16&			uiVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_UI2, varVal ) )
		return false;
	uiVal = varVal.uiVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt32)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	SInt32&			lVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_I4, varVal ) )
		return false;
	lVal = varVal.lVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt32)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	UInt32&			ulVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_UI4, varVal ) )
		return false;
	ulVal = varVal.ulVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (SInt64)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	SInt64&			llVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_I8, varVal ) )
		return false;
	llVal = varVal.llVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (UInt64)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	UInt64&			ullVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_UI8, varVal ) )
		return false;
	ullVal = varVal.ullVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (bool)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	bool&			boolVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_I4, varVal ) )
		return false;
	boolVal = (V_I4(&varVal) != 0);
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (int)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	int&			intVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_INT, varVal ) )
		return false;
	intVal = varVal.intVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (unsigned int)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	unsigned int&	uintVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_UINT, varVal ) )
		return false;
	uintVal = varVal.uintVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (double)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	double&			dblVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_R8, varVal ) )
		return false;
	dblVal = varVal.dblVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (GUID)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	GUID&			guidVal )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_BSTR, varVal ) )
		return false;
	ThrowIfFailed( ::CLSIDFromString( varVal.bstrVal, &guidVal ) );
	return true;
}


// -----------------------------------------------------------------------------
//		GetChildElement (BYTE*,size_t&)
// -----------------------------------------------------------------------------

bool
GetChildElement(
	IXMLDOMNode*	pNode,
	LPCTSTR			pszName,
	BYTE*			pData,
	size_t&			nDataLen )
{
	CComVariantEx	varVal;
	if ( !GetChildElement( pNode, pszName, VT_BSTR, varVal ) )
		return false;
	varVal.HexDecode( pData, nDataLen );
	return true;
}


// -----------------------------------------------------------------------------
//		CreateElementNS
// -----------------------------------------------------------------------------

void
CreateElementNS(
	IXMLDOMDocument*	pDoc,
	LPCTSTR				pszName,
	LPCTSTR				pszNamespaceURI,
	IXMLDOMElement**	ppElem )
{
	// Sanity checks.
	ThrowIfNull( pDoc );

	CComPtr<IXMLDOMNode>	spNode;
	ThrowIfFailed( pDoc->createNode( CComVariantEx( NODE_ELEMENT ),
		CComBSTR( CT2COLE( pszName ) ), CComBSTR( CT2COLE( pszNamespaceURI ) ),
		&spNode ) );
	ThrowIfFailed( spNode->QueryInterface( ppElem ) );
}


// -----------------------------------------------------------------------------
//		AddChildElementNS
// -----------------------------------------------------------------------------

void
AddChildElementNS(
	IXMLDOMDocument*		pDoc,
	IXMLDOMNode*			pNode,
	LPCTSTR					pszName,
	LPCTSTR					pszNamespaceURI,
	const CComVariantEx&	varVal,
	IXMLDOMElement**		ppElem )
{
	if ( ppElem != NULL )
	{
		*ppElem = NULL;
	}

	// Sanity checks.
	ThrowIfNull( pDoc );
	ThrowIfNull( pNode );
	ThrowIfNull( pszName );

	CComPtr<IXMLDOMElement>	spElem;
	CreateElementNS( pDoc, pszName, pszNamespaceURI, &spElem );

	if ( varVal.vt == VT_BSTR )
	{
		ThrowIfFailed( spElem->put_text( varVal.bstrVal ) );
	}
	else if ( varVal.vt != VT_EMPTY )
	{
		CComVariantEx	varBstrVal;
		ThrowIfFailed( varBstrVal.ChangeType( VT_BSTR, &varVal ) );
		ThrowIfFailed( spElem->put_text( varBstrVal.bstrVal ) );
	}

	ThrowIfFailed( pNode->appendChild( spElem, NULL ) );

	if ( ppElem != NULL )
	{
		ThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	IXMLDOMElement**	ppElem )
{
	if ( ppElem != NULL )
	{
		*ppElem = NULL;
	}

	// Sanity checks.
	ThrowIfNull( pDoc );
	ThrowIfNull( pNode );
	ThrowIfNull( pszName );

	// Create the child element.
	CComPtr<IXMLDOMElement>	spElem;
	ThrowIfFailed( pDoc->createElement( CComBSTR( CT2COLE( pszName ) ), &spElem ) );

	// Append the child element.
	ThrowIfFailed( pNode->appendChild( spElem, NULL ) );

	if ( ppElem != NULL )
	{
		ThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// ------------------------------------------------------------------------------
//		AddChildElement
// ------------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	IUnknown*			pObj,
	IXMLDOMElement**	ppElem,					// default = NULL
	bool				bWriteClsidAttribute,	// default = true
	bool				bClearDirty )			// default = true
{
	if ( ppElem != NULL )
	{
		*ppElem = NULL;
	}

	// Sanity checks.
	ThrowIfNull( pDoc );
	ThrowIfNull( pNode );
	ThrowIfNull( pszName );

	// Create the child element.
	CComPtr<IXMLDOMElement>	spElem;
	ThrowIfFailed( pDoc->createElement( CComBSTR( CT2COLE( pszName ) ), &spElem ) );

	// Set the object.
	SetElement( pDoc, spElem, pObj, bWriteClsidAttribute, bClearDirty );

	// Append the child element.
	ThrowIfFailed( pNode->appendChild( spElem, NULL ) );

	if ( ppElem != NULL )
	{
		ThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*		pDoc,
	IXMLDOMNode*			pNode,
	LPCTSTR					pszName,
	const CComVariantEx&	varVal,
	IXMLDOMElement**		ppElem )
{
	if ( ppElem != NULL )
	{
		*ppElem = NULL;
	}

	ThrowIfNull( pDoc );
	ThrowIfNull( pNode );
	ThrowIfNull( pszName );

	// Create the child element.
	CComPtr<IXMLDOMElement>	spElem;
	ThrowIfFailed( pDoc->createElement( CComBSTR( CT2COLE( pszName ) ), &spElem ) );

	if ( varVal.vt != VT_EMPTY )
	{
		// Set the element value.
		SetElement( pDoc, spElem, varVal );
	}

	// Append the child element.
	ThrowIfFailed( pNode->appendChild( spElem, NULL ) );

	if ( ppElem != NULL )
	{
		ThrowIfFailed( spElem.CopyTo( ppElem ) );
	}
}


// -----------------------------------------------------------------------------
//		AddChildElement (CComBSTR)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	const CComBSTR&		bstrVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( bstrVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (std::string)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	const std::string&	strVal,
	IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, strVal.c_str(), ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (std::wstring)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	const std::wstring&	strVal,
	IXMLDOMElement**	ppElem )
{
	AddChildElement( pDoc, pNode, pszName, strVal.c_str(), ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (LPCOLESTR)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	LPCOLESTR			pszVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( pszVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (LPCSTR)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	LPCSTR				pszVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( pszVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt8)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	SInt8				cVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( cVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt8)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	UInt8				bVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( bVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt16)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	SInt16				iVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( iVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt16)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	UInt16				uiVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( uiVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt32)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	SInt32				lVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( lVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt32)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	UInt32				ulVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( ulVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (SInt64)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	SInt64				llVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( llVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (UInt64)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	UInt64				ullVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( ullVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (bool)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	bool				boolVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( boolVal ? 1 : 0 );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (int)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	int					intVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( intVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (unsigned int)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	unsigned int		uintVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( uintVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (double)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	double				dblVal,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( dblVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (GUID)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	const GUID&			guidVal,
	IXMLDOMElement**	ppElem )
{
	CComBSTR		bstrVal( guidVal );
	bstrVal.ToUpper();
	CComVariantEx	varVal( bstrVal );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		AddChildElement (const BYTE*,size_t)
// -----------------------------------------------------------------------------

void
AddChildElement(
	IXMLDOMDocument*	pDoc,
	IXMLDOMNode*		pNode,
	LPCTSTR				pszName,
	const BYTE*			pData,
	size_t				nDataLen,
	IXMLDOMElement**	ppElem )
{
	CComVariantEx	varVal( pData, nDataLen );
	AddChildElement( pDoc, pNode, pszName, varVal, ppElem );
}


// -----------------------------------------------------------------------------
//		GetAttribute
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*		pElem,
	LPCTSTR				pszName,
	VARTYPE				vt,
	CComVariantEx&		varVal )
{
	ThrowIfNull( pElem );
	ThrowIfNull( pszName );

	HRESULT	hr;

	// Get the attribute.
	hr = pElem->getAttribute( CComBSTR( CT2COLE( pszName ) ), &varVal );
	if ( hr == S_FALSE )
	{
		// The attribute doesn't exist.
		return false;
	}
	ThrowIfFailed( hr );

	// Change to requested variant type.
	ThrowIfFailed( varVal.ChangeType( vt ) );

	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (CComBSTR)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	CComBSTR&		bstrVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_BSTR, varVal ) )
		return false;
	bstrVal = varVal.bstrVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (CString)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	CString&		strVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_BSTR, varVal ) )
		return false;
	strVal = varVal.bstrVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (std::string)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	std::string&	strVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_BSTR, varVal ) )
		return false;
	if ( varVal.bstrVal != NULL )
	{
		strVal = CW2A( varVal.bstrVal );
	}
	else
	{
		strVal.clear();
	}
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (std::wstring)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	std::wstring&	strVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_BSTR, varVal ) )
		return false;
	if ( varVal.bstrVal != NULL )
	{
		strVal = varVal.bstrVal;
	}
	else
	{
		strVal.clear();
	}
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt8)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt8&			cVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_I1, varVal ) )
		return false;
	cVal = varVal.cVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt8)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt8&			bVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_UI1, varVal ) )
		return false;
	bVal = varVal.bVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt16)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt16&			iVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_I2, varVal ) )
		return false;
	iVal = varVal.iVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt16)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt16&			uiVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_UI2, varVal ) )
		return false;
	uiVal = varVal.uiVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt32)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt32&			lVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_I4, varVal ) )
		return false;
	lVal = varVal.lVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt32)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt32&			ulVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_UI4, varVal ) )
		return false;
	ulVal = varVal.ulVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (SInt64)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt64&			llVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_I8, varVal ) )
		return false;
	llVal = varVal.llVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (UInt64)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt64&			ullVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_UI8, varVal ) )
		return false;
	ullVal = varVal.ullVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (bool)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	bool&			boolVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_I4, varVal ) )
		return false;
	boolVal = (V_I4(&varVal) != 0);
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (int)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	int&			intVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_INT, varVal ) )
		return false;
	intVal = varVal.intVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (unsigned int)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	unsigned int&	uintVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_UINT, varVal ) )
		return false;
	uintVal = varVal.uintVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (double)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	double&			dblVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_R8, varVal ) )
		return false;
	dblVal = varVal.dblVal;
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (GUID)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	GUID&			guidVal )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_BSTR, varVal ) )
		return false;
	ThrowIfFailed( ::CLSIDFromString( varVal.bstrVal, &guidVal ) );
	return true;
}


// -----------------------------------------------------------------------------
//		GetAttribute (BYTE*,size_t&)
// -----------------------------------------------------------------------------

bool
GetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	BYTE*			pData,
	size_t&			nDataLen )
{
	CComVariantEx	varVal;
	if ( !GetAttribute( pElem, pszName, VT_BSTR, varVal ) )
		return false;
	varVal.HexDecode( pData, nDataLen );
	return true;
}


// -----------------------------------------------------------------------------
//		CreateAttributeNS
// -----------------------------------------------------------------------------

void
CreateAttributeNS(
	IXMLDOMDocument*		pDoc,
	LPCTSTR					pszName,
	LPCTSTR					pszNamespaceURI,
	IXMLDOMAttribute**		ppAttr )
{
	// Sanity checks.
	ThrowIfNull( pDoc );

	CComPtr<IXMLDOMNode>	spNode;
	ThrowIfFailed( pDoc->createNode( CComVariant( NODE_ATTRIBUTE ),
		CComBSTR( CT2COLE( pszName ) ), CComBSTR( CT2COLE( pszNamespaceURI ) ),
		&spNode ) );
	ThrowIfFailed( spNode->QueryInterface( ppAttr ) );
}


// -----------------------------------------------------------------------------
//		SetAttributeNS
// -----------------------------------------------------------------------------

void
SetAttributeNS(
	IXMLDOMElement*			pElem,
	LPCTSTR					pszName,
	LPCTSTR					pszNamespaceURI,
	const CComVariantEx&	varVal )
{
	// Sanity checks.
	ThrowIfNull( pElem );

	// Create the attribute.
	CComPtr<IXMLDOMDocument>	spDoc;
	ThrowIfFailed( pElem->get_ownerDocument( &spDoc ) );
	CComPtr<IXMLDOMAttribute>	spAttr;
	CreateAttributeNS( spDoc, pszName, pszNamespaceURI, &spAttr );

	// Set the attribute value.
	if ( varVal.vt == VT_BSTR )
	{
		ThrowIfFailed( spAttr->put_text( varVal.bstrVal ) );
	}
	else if ( varVal.vt != VT_EMPTY )
	{
		CComVariantEx	varBstrVal;
		ThrowIfFailed( varBstrVal.ChangeType( VT_BSTR, &varVal ) );
		ThrowIfFailed( spAttr->put_text( varBstrVal.bstrVal ) );
	}

	// Set the attribute on the element.
	ThrowIfFailed( pElem->setAttributeNode( spAttr, NULL ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*			pElem,
	LPCTSTR					pszName,
	const CComVariantEx&	varVal )
{
	ThrowIfNull( pElem );
	ThrowIfNull( pszName );

	const CComBSTR	bstrName = CT2COLE( pszName );

	if ( varVal.vt == VT_BSTR )
	{
		// Set the attribute.
		ThrowIfFailed( pElem->setAttribute( bstrName, varVal ) );
	}
	else
	{
		// Change to BSTR here to get special conversion from CComVariantEx.
		CComVariantEx	varBstrVal;
		ThrowIfFailed( varBstrVal.ChangeType( VT_BSTR, &varVal ) );

		// Set the attribute.
		ThrowIfFailed( pElem->setAttribute( bstrName, varBstrVal ) );
	}
}


// -----------------------------------------------------------------------------
//		SetAttribute (CComBSTR)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	const CComBSTR&	bstrVal )
{
	ThrowIfNull( pElem );
	ThrowIfNull( pszName );

	// Set the attribute (avoid copying the variant).
	VARIANT		varVal;
	varVal.vt      = VT_BSTR;
	varVal.bstrVal = bstrVal;
	ThrowIfFailed( pElem->setAttribute( CComBSTR( CT2COLE( pszName ) ), varVal ) );
}


// -----------------------------------------------------------------------------
//		SetAttribute (std::string)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*		pElem,
	LPCTSTR				pszName,
	const std::string&	strVal )
{
	SetAttribute( pElem, pszName, strVal.c_str() );
}


// -----------------------------------------------------------------------------
//		SetAttribute (std::wstring)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*		pElem,
	LPCTSTR				pszName,
	const std::wstring&	strVal )
{
	SetAttribute( pElem, pszName, strVal.c_str() );
}


// -----------------------------------------------------------------------------
//		SetAttribute (LPCOLESTR)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	LPCOLESTR		pszVal )
{
	CComVariantEx	varVal( pszVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (LPCSTR)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	LPCSTR			pszVal )
{
	CComVariantEx	varVal( pszVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt8)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt8			cVal )
{
	CComVariantEx	varVal( cVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt8)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt8			bVal )
{
	CComVariantEx	varVal( bVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt16)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt16			iVal )
{
	CComVariantEx	varVal( iVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt16)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt16			uiVal )
{
	CComVariantEx	varVal( uiVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt32)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt32			lVal )
{
	CComVariantEx	varVal( lVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt32)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt32			ulVal )
{
	CComVariantEx	varVal( ulVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (SInt64)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	SInt64			llVal )
{
	CComVariantEx	varVal( llVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (UInt64)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	UInt64			ullVal )
{
	CComVariantEx	varVal( ullVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (bool)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	bool			boolVal )
{
	CComVariantEx	varVal( boolVal ? 1 : 0 );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (int)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	int				intVal )
{
	CComVariantEx	varVal( intVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (unsigned int)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	unsigned int	uintVal )
{
	CComVariantEx	varVal( uintVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (double)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	double			dblVal )
{
	CComVariantEx	varVal( dblVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (GUID)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	const GUID&		guidVal )
{
	CComBSTR		bstrVal( guidVal );
	bstrVal.ToUpper();
	CComVariantEx	varVal( bstrVal );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SetAttribute (const BYTE*,size_t)
// -----------------------------------------------------------------------------

void
SetAttribute(
	IXMLDOMElement*	pElem,
	LPCTSTR			pszName,
	const BYTE*		pData,
	size_t			nDataLen )
{
	CComVariantEx	varVal( pData, nDataLen );
	SetAttribute( pElem, pszName, varVal );
}


// -----------------------------------------------------------------------------
//		SanitizeWhitespace
// -----------------------------------------------------------------------------
/// Convert tab and return to space characters.
/// Convert multiple whitespace characters into a single space.

CString
SanitizeWhitespace(
	const CString&	inString )
{
	const int	nCharCount = inString.GetLength();
	if ( 0 == nCharCount ) return inString;
	
	CString	strResult;
	TCHAR *				pDest			= strResult.GetBuffer( 1 + nCharCount );
	const TCHAR *		pSource			= inString;
	static const TCHAR	CR				= 0x13;
	static const TCHAR	LF				= 0x10;
	static const TCHAR	SPACE			= ' ';
	int					bufferCharCount	= 0;
	TCHAR				prevChar		= SPACE;	// initial space prevents leading whitespace
	for ( int i = 0; i < nCharCount; ++i, ++pSource )
	{
		TCHAR	c = *pSource;
		
		// convert whitespace to space
		switch ( c )
		{
			case CR   :
			case LF   :
			case '\t' :
			case '\r' :	// yeah, I know CR is \r or \n, but
			case '\n' :	// I can never remember which one...
			case '\a' :	// that goes for LF, too
				c = SPACE;
				break;
		}
		
		// if space and prev char was space, skip it
		if (( SPACE == c ) && ( SPACE == prevChar ))
		{
			continue;
		}
		
		// copy the char over to output string
		*pDest	= c;
		++pDest;
		++bufferCharCount;
		prevChar = c;
	}
	
	// prevent trailing whitespace
	if (( SPACE == prevChar ) && ( 0 < bufferCharCount ))
	{
		--bufferCharCount;
		--pDest;
	}

	// NULL terminate.
	*pDest = 0;

	strResult.ReleaseBuffer();
	
	return strResult;
}


// -----------------------------------------------------------------------------
//		GetNodeText
// -----------------------------------------------------------------------------

CString
GetNodeText(
	IXMLDOMNode*	pNode )
{
	HRESULT hr = S_OK;

	try
	{
		CComBSTR	bstrNodeText;
		ThrowIfNotOK( pNode->get_text( &bstrNodeText ) );
		if ( bstrNodeText == NULL )
		{
			return CString();
		}	

		return SanitizeWhitespace( CString( bstrNodeText ) );
	}
	CATCH_HR(hr)

	return CString();
}


// -----------------------------------------------------------------------------
//		PrettyFormatXml
// -----------------------------------------------------------------------------

HRESULT
PrettyFormatXml(
	IXMLDOMDocument*	pDoc,
	LPCTSTR				pszIndent )	// default = NULL
{
	// Sanity checks.
	ATLASSERT( NULL != pDoc );
	if ( NULL == pDoc ) return E_POINTER;

	HRESULT hr = S_OK;

	try
	{
		CComPtr<IXMLDOMElement> spDocElem;
		ThrowIfNotOK( pDoc->get_documentElement( &spDocElem ) );

		// Default to a tab indent if one wasn't specified.
		CString strIndent = pszIndent;
		if ( strIndent.IsEmpty() )
		{
			strIndent = _T("\t");
		}
		int indentOrigLen = 0;
		hr = PrettyFormatXml( spDocElem, strIndent, indentOrigLen );
	}
	CATCH_HR(hr)

	return hr;
}


// -----------------------------------------------------------------------------
//		PrettyFormatXml
// -----------------------------------------------------------------------------

HRESULT
PrettyFormatXml(
	IXMLDOMNode*	pNode,
	CString&		strIndent,
	int&			indentOrigLen )
{
	// Sanity checks.
	ATLASSERT( NULL != pNode );
	if ( NULL == pNode ) return E_POINTER;

	HRESULT	hr = S_OK;

	try
	{
		if ( indentOrigLen == 0 )
		{
			indentOrigLen = strIndent.GetLength();
		}

		CComPtr<IXMLDOMNode>	spChildNode;
		hr = pNode->get_firstChild( &spChildNode );
		if ( hr == S_OK )
		{
			CComPtr<IXMLDOMDocument>	spDoc;
			ThrowIfFailed( pNode->get_ownerDocument( &spDoc ) );

			// Get ready for some convoluted logic...

			DOMNodeType	typeChildNode;
			ThrowIfFailed( spChildNode->get_nodeType( &typeChildNode ) );
			const bool	bFirstChildTextNode = (typeChildNode == NODE_TEXT);

			bool	bLastWasTextNode = false;
			while ( (hr == S_OK) && (spChildNode != NULL) )
			{
				ThrowIfFailed( spChildNode->get_nodeType( &typeChildNode ) );

				if ( (typeChildNode == NODE_ELEMENT) && !bLastWasTextNode )
				{
					CComPtr<IXMLDOMNode>	spNewNode;
					ThrowIfFailed( spDoc->createNode( CComVariantEx( NODE_TEXT ), NULL, NULL, &spNewNode ) );
					CString	strValue = _T("\r\n") + strIndent;
					ThrowIfFailed( spNewNode->put_nodeValue( CComVariantEx( strValue ) ) );
					CComPtr<IXMLDOMNode>	spN;
					ThrowIfFailed( pNode->insertBefore( spNewNode, CComVariantEx( spChildNode ), &spN ) );
				}

				CString	strNewIndent = strIndent + strIndent.Left( indentOrigLen );
				ThrowIfFailed( PrettyFormatXml( spChildNode, strNewIndent, indentOrigLen ) );

				bLastWasTextNode = (typeChildNode == NODE_TEXT);

				CComPtr<IXMLDOMNode>	spSibling;
				(void) spChildNode->get_nextSibling( &spSibling );
				spChildNode = spSibling;
			}

			if ( !bFirstChildTextNode )
			{
				CComPtr<IXMLDOMNode>	spNewNode;
				ThrowIfFailed( spDoc->createNode( CComVariantEx( NODE_TEXT ), NULL, NULL, &spNewNode ) );
				CString	strValue = _T("\r\n") + strIndent.Left( strIndent.GetLength() - 1 );
				ThrowIfFailed( spNewNode->put_nodeValue( CComVariantEx( strValue ) ) );
				ThrowIfFailed( pNode->appendChild( spNewNode, NULL ) );
			}
		}

		hr = S_OK;
	}
	CATCH_HR(hr)

	return hr;
}


// =============================================================================
//	ChildIterator
// =============================================================================

// -----------------------------------------------------------------------------
//		ChildIterator(IXMLDOMNode*,LPCTSTR)
// -----------------------------------------------------------------------------

ChildIterator::ChildIterator(
	IXMLDOMNode*	inParentNode,
	LPCTSTR			inChildTagName,
	bool			bAllDescendents ) :
		m_nChildCount( 0 ),
		m_nCurrentIndex( -1 )
{
	if ( bAllDescendents )
	{
		// Parent could be an element or the document.
		CComQIPtr<IXMLDOMElement>	spParentElement( inParentNode );
		if ( spParentElement != NULL )
		{
			ThrowIfNotOK( spParentElement->getElementsByTagName( CComBSTR( inChildTagName ), &m_spChildNodeList ) );
		}
		else
		{
			CComQIPtr<IXMLDOMDocument>	spParentDoc( inParentNode );
			ThrowIfNull( spParentDoc, E_INVALIDARG );
			ThrowIfNotOK( spParentDoc->getElementsByTagName( CComBSTR( inChildTagName ), &m_spChildNodeList ) );
		}
	}
	else
	{
		ThrowIfNull( inParentNode );
		ThrowIfNotOK( inParentNode->selectNodes( CComBSTR( inChildTagName ), &m_spChildNodeList ) );
	}

	ThrowIfFailed( m_spChildNodeList->get_length( &m_nChildCount ) );
}


// -----------------------------------------------------------------------------
//		ChildIterator()
// -----------------------------------------------------------------------------

ChildIterator::ChildIterator() :
	m_nChildCount( 0 ),
	m_nCurrentIndex( -1 )
{
}


// -----------------------------------------------------------------------------
//		ChildIterator(const ChildIterator&)
// -----------------------------------------------------------------------------

ChildIterator::ChildIterator(
	const ChildIterator&	inOriginal ) :
		m_spChildNodeList( inOriginal.m_spChildNodeList ),
		m_nChildCount(     inOriginal.m_nChildCount		),
		m_nCurrentIndex(   inOriginal.m_nCurrentIndex   )
{
}


// -----------------------------------------------------------------------------
//		~ChildIterator
// -----------------------------------------------------------------------------

ChildIterator::~ChildIterator()
{
}


// -----------------------------------------------------------------------------
//		operator =
// -----------------------------------------------------------------------------

ChildIterator& 
ChildIterator::operator = (
	const ChildIterator&	inOriginal )
{
	m_spChildNodeList = inOriginal.m_spChildNodeList;
	m_nChildCount     = inOriginal.m_nChildCount;
	m_nCurrentIndex   = inOriginal.m_nCurrentIndex;
	return *this;
}


// -----------------------------------------------------------------------------
//		hasNext
// -----------------------------------------------------------------------------

bool				
ChildIterator::hasNext() const
{
	return ( ( 1 + m_nCurrentIndex ) < m_nChildCount ) && ( 0 < m_nChildCount ); 
}


// -----------------------------------------------------------------------------
//		next
// -----------------------------------------------------------------------------
// Return the next element. Can't just return IXMLDOMElement* without weird
// ref counting semantics.

void
ChildIterator::next(
	IXMLDOMElement**	ppElem )
{
	++m_nCurrentIndex;

	CComPtr<IXMLDOMNode>	spChildNode;
	ThrowIfFailed( m_spChildNodeList->get_item( m_nCurrentIndex, &spChildNode ) );
	ThrowIfFailed( spChildNode->QueryInterface( ppElem ) );
}


} /*namespace XmlUtil */
