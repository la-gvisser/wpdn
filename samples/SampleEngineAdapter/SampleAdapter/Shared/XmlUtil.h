// =============================================================================
//	xmlutil.h
// =============================================================================
//	Copyright (c) 2003-2008. WildPackets, Inc. All rights reserved.

#ifndef XMLUTIL_H
#define XMLUTIL_H

#include "ixmldom.h"
#include <string>
#include <vector>

namespace XmlUtil
{

// Creates an XML document and adds the necessary processing instructions etc.
HeResult	CreateXmlDocument( Xml::IXMLDOMDocument** ppXmlDoc, bool bAddPI = true );

// Load and XML document from a file.
HeResult	LoadXmlFile( const wchar_t* pszFile, Xml::IXMLDOMDocument** ppXmlDoc );

// Save an XML document to a file.
HeResult	SaveXmlFile( const wchar_t* pszFile, Xml::IXMLDOMDocument* pXmlDoc );

// Utility for adding tabs and returns to an XML document to get 'pretty' formatted doc for save.
HeResult	PrettyFormatXml( Xml::IXMLDOMDocument* pDoc, const wchar_t* pszIndent = NULL );
HeResult	PrettyFormatXml( Xml::IXMLDOMNode* pNode, std::wstring& strIndent, size_t& indentOrigLen );

// GetAttribute - general purpose
// Return false if the attribute doesn't exist
bool	GetAttribute( Xml::IXMLDOMElement* pElement, const wchar_t* pszName,
			Helium::HEVARTYPE vt, HeLib::CHeVariant& varVal );

// GetAttribute - wrappers for specific types
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, HeLib::CHeBSTR& bstrVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, std::wstring& strVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, std::string& strVal );
#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, CString& strVal );
#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt8& cVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt8& bVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt16& iVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt16& uiVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt32& lVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt32& ulVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt64& llVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt64& ullVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, bool& boolVal );
#ifndef OPT_64BITINTTYPES
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, int& intVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, unsigned int& uintVal );
#endif /* OPT_64BITINTTYPES */
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, double& dblVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, Helium::HeID& guidVal );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt8* pData, size_t& nDataLen );
bool	GetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, std::vector<UInt8>& data );

// SetAttribute - general purpose
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const HeLib::CHeVariant& varVal );

// SetAttribute - wrappers for specific types
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const HeLib::CHeBSTR& bstrVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const std::wstring& strVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const std::string& strVal );
#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const CString& strVal );
#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const wchar_t* pszVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const char* pszVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt8 cVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt8 bVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt16 iVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt16 uiVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt32 lVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt32 ulVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, SInt64 llVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, UInt64 ullVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, bool boolVal );
#ifndef OPT_64BITINTTYPES
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, int intVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, unsigned int uintVal );
#endif /* OPT_64BITINTTYPES */
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, double dblVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const Helium::HeID& guidVal );
void	SetAttribute( Xml::IXMLDOMElement* pElem, const wchar_t* pszName, const UInt8* pData, size_t nDataLen );

// GetElement - general purpose
void	GetElement( Xml::IXMLDOMElement* pElem, Helium::HEVARTYPE vt, HeLib::CHeVariant& varVal );

// GetElement - wrappers for specific types
void	GetElement( Xml::IXMLDOMElement* pElem, HeLib::CHeBSTR& bstrVal );
void	GetElement( Xml::IXMLDOMElement* pElem, std::wstring& strVal );
void	GetElement( Xml::IXMLDOMElement* pElem, std::string& strVal );
#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
void	GetElement( Xml::IXMLDOMElement* pElem, CString& strVal );
#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */
void	GetElement( Xml::IXMLDOMElement* pElem, SInt8& cVal );
void	GetElement( Xml::IXMLDOMElement* pElem, UInt8& uiVal );
void	GetElement( Xml::IXMLDOMElement* pElem, SInt16& iVal );
void	GetElement( Xml::IXMLDOMElement* pElem, UInt16& uiVal );
void	GetElement( Xml::IXMLDOMElement* pElem, SInt32& lVal );
void	GetElement( Xml::IXMLDOMElement* pElem, UInt32& ulVal );
void	GetElement( Xml::IXMLDOMElement* pElem, SInt64& llVal );
void	GetElement( Xml::IXMLDOMElement* pElem, UInt64& ullVal );
void	GetElement( Xml::IXMLDOMElement* pElem, bool& boolVal );
#ifndef OPT_64BITINTTYPES
void	GetElement( Xml::IXMLDOMElement* pElem, int& intVal );
void	GetElement( Xml::IXMLDOMElement* pElem, unsigned int& uintVal );
#endif /* OPT_64BITINTTYPES */
void	GetElement( Xml::IXMLDOMElement* pElem, double& dblVal );
void	GetElement( Xml::IXMLDOMElement* pElem, Helium::HeID& guidVal );
void	GetElement( Xml::IXMLDOMElement* pElem, UInt8* pData, size_t& nDataLen );
void	GetElement( Xml::IXMLDOMElement* pElem, std::vector<UInt8>& data );

// Duplicate the Source XML Node under the Destination Node.
HeResult	DuplicateXmlNode( Xml::IXMLDOMDocument* pSrcDoc, Xml::IXMLDOMNode* pSrcNode,
	Xml::IXMLDOMDocument* pDstDoc, Xml::IXMLDOMNode* pDstNode, Xml::IXMLDOMElement** ppNewElem );

// GetElement - get object that supports IPersistXml
template <typename Q>
void GetElement( Xml::IXMLDOMElement* pElem, Q** ppObj )
{
	// Get the class id attribute.
	HeCID	clsid;
	HeThrowIf( !GetAttribute( pElem, L"clsid", clsid ) );

	// Get the XML doc.
	CHePtr<Xml::IXMLDOMDocument>	spXmlDoc;
	HeThrowIfFailed( pElem->get_ownerDocument( &spXmlDoc.p ) );

	// Create the object.
	CHePtr<Q>	spObj;
	HeThrowIfFailed( spObj.CreateInstance( clsid ) );

	// Load it.
	CHePtr<IHePersistXml>	spPersistXml;
	HeThrowIfFailed( spObj->QueryInterface( &spPersistXml.p ) );
	HeThrowIfFailed( spPersistXml->Load( spXmlDoc, pElem ) );

	// Return it.
	HeThrowIfFailed( spObj.CopyTo( ppObj ) );
}

// SetElement - set object that supports IPersistXml
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, Helium::IHeUnknown* pObj,
			bool bWriteClsidAttribute = true, bool bClearDirty = true );

// SetElement - general purpose
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const HeLib::CHeVariant& varVal );

// SetElement - wrappers for specific types
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const HeLib::CHeBSTR& bstrVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const std::wstring& strVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const std::string& strVal );
#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const CString& strVal );
#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const wchar_t* pszVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const char* pszVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, SInt8 cVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, UInt8 bVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, SInt16 iVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, UInt16 uiVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, SInt32 lVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, UInt32 ulVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, SInt64 llVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, UInt64 ullVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, bool boolVal );
#ifndef OPT_64BITINTTYPES
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, int intVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, unsigned int uintVal );
#endif /* OPT_64BITINTTYPES */
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, double dblVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const Helium::HeID& guidVal );
void	SetElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMElement* pElem, const UInt8* pData, size_t nDataLen );

// GetChildElement - get the named child element
// Return false if the element doesn't exist
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, Xml::IXMLDOMElement** ppElem );

// GetChildElement - general purpose
// Return false if the element doesn't exist
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName,
			Helium::HEVARTYPE vt, HeLib::CHeVariant& varVal );

// GetChildElement - wrappers for specific types
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, HeLib::CHeBSTR& bstrVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, std::wstring& strVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, std::string& strVal );
#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, CString& strVal );
#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, SInt8& cVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, UInt8& bVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, SInt16& iVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, UInt16& uiVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, SInt32& lVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, UInt32& ulVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, SInt64& llVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, UInt64& ullVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, bool& boolVal );
#ifndef OPT_64BITINTTYPES
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, int& intVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, unsigned int& uintVal );
#endif /* OPT_64BITINTTYPES */
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, double& dblVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, Helium::HeID& guidVal );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, UInt8* pData, size_t& nDataLen );
bool	GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, std::vector<UInt8>& data );

// GetChildElement - get a child object that supports IHePersistXml
// Return false if the element doesn't exist, otherwise throws on error
template <typename Q>
bool GetChildElement( Xml::IXMLDOMNode* pNode, const wchar_t* pszName, Q** ppObj )
{
	// Get the element.
	CHePtr<Xml::IXMLDOMElement>	spElem;
	if ( !GetChildElement( pNode, pszName, &spElem.p ) )
	{
		return false;
	}

	// Get the object.
	GetElement( spElem, ppObj );

	return true;
}

// AddChildElement - create empty child element
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, Xml::IXMLDOMElement** ppElem = NULL );

// AddChildElement - add a child object that supports IPersistXml
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, Helium::IHeUnknown* pObj, Xml::IXMLDOMElement** ppElem = NULL,
			bool bWriteClsidAttribute = true, bool bClearDirty = true );

// AddChildElement - general purpose
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const HeLib::CHeVariant& varVal, Xml::IXMLDOMElement** ppElem = NULL );

// AddChildElement - wrappers for specific types
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const HeLib::CHeBSTR& bstrVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const std::wstring& strVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const std::string& strVal, Xml::IXMLDOMElement** ppElem = NULL );
#if defined(__ATLSTR_H__) || defined(__AFXSTR_H__)
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const CString& bstrVal, Xml::IXMLDOMElement** ppElem = NULL );
#endif /* defined(__ATLSTR_H__) || defined(__AFXSTR_H__) */
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const wchar_t* pszVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const char* pszVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, SInt8 cVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, UInt8 bVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, SInt16 iVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, UInt16 uiVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, SInt32 lVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, UInt32 ulVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, SInt64 llVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, UInt64 ullVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, bool boolVal, Xml::IXMLDOMElement** ppElem = NULL );
#ifndef OPT_64BITINTTYPES
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, int intVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, unsigned int uintVal, Xml::IXMLDOMElement** ppElem = NULL );
#endif /* OPT_64BITINTTYPES */
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, double dblVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const Helium::HeID& guidVal, Xml::IXMLDOMElement** ppElem = NULL );
void	AddChildElement( Xml::IXMLDOMDocument* pDoc, Xml::IXMLDOMNode* pNode,
			const wchar_t* pszName, const UInt8* pData, size_t nDataLen, Xml::IXMLDOMElement** ppElem = NULL );

enum XmlResult
{
	kXmlResult_Existed,
	kXmlResult_Created
};

// LoadOrCreateXmlDocument - Loads an XML document if it exists; otherwise
// creates a new, empty, unsaved XML document.
inline XmlResult
LoadOrCreateXmlDocument(
	const wchar_t*			pszFile,
	Xml::IXMLDOMDocument**	ppXmlDoc )
{
	HeResult hr = LoadXmlFile( pszFile, ppXmlDoc );
	if ( HE_SUCCEEDED(hr) ) return kXmlResult_Existed;

	hr = CreateXmlDocument( ppXmlDoc );
	HeThrowIfFailed( hr );
	return kXmlResult_Created;
}

// GetOrAddChildElement - Finds a child element or creates it if not present.
inline XmlResult
GetOrAddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	Xml::IXMLDOMElement**	ppElem )
{
	if ( GetChildElement( pNode, pszName, ppElem ) ) return kXmlResult_Existed;

	AddChildElement( pDoc, pNode, pszName, ppElem );
	return kXmlResult_Created;
}

inline XmlResult
ReplaceOrAddChildElement(
	Xml::IXMLDOMDocument*	pDoc,
	Xml::IXMLDOMNode*		pNode,
	const wchar_t*			pszName,
	Xml::IXMLDOMElement**	ppElem )
{
	XmlResult nXmlResult = kXmlResult_Created;

	{
		CHePtr<Xml::IXMLDOMElement> spOldElement;
		if ( GetChildElement( pNode, pszName, &spOldElement.p ) )
		{
			HeThrowIfNotOK( pNode->removeChild( spOldElement, NULL ) );
			nXmlResult = kXmlResult_Existed;
		}
	}

	AddChildElement( pDoc, pNode, pszName, ppElem );
	return nXmlResult;
}

// =============================================================================
//	ElementIterator
// =============================================================================
// Given a parent node and a tag name, iterate through all child elements
// with that tag.
//
//	Example: iterate through 3 "row" children
//
//		<row-list>          <-- pParentNode
//			<row>...</row>			
//			<row>...</row>
//			<row>...</row>
//		</row-list>
//
//		XmlUtil::ElementIterator	it( pParentNode, L"row" );
//		for ( ;; )
//		{
//			CHePtr<Xml::IXMLDOMElement>	spElem;
//			if ( !it.GetNext( &spElem.p ) ) break;
//			...
//		}
class ElementIterator
{
public:
	ElementIterator( Xml::IXMLDOMNode* pParentNode, const wchar_t* pszChildTagName = L"*" );

	SInt32	GetCount() const;
	bool	GetNext( Xml::IXMLDOMElement** ppNextElem );
private:
	HeLib::CHePtr<Xml::IXMLDOMNodeList>	m_spNodeList;
};

}	/* namespace XmlUtil */

#endif /* XMLUTIL_H */
