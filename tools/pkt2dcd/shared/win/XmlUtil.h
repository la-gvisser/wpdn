// =============================================================================
//	XmlUtil.h
// =============================================================================
//	Copyright (c) 2003-2008. WildPackets, Inc. All rights reserved.

#ifndef XMLUTIL_H
#define XMLUTIL_H

#include "AGTypes.h"
#include "IPersistXml.h"
#include "VariantUtil.h"
#include <atlcoll.h>
#include <msxml2.h>
#include <string>

namespace XmlUtil
{
	// Check the HRESULT return code from various MSXML interfaces that return
	// S_FALSE instead of an error code.
	// 
	// Notes:
	// - S_FALSE is converted to E_FAIL
	// - throws with AtlThrow on failure
	inline void
	CheckMSXMLResult(
		HRESULT hr )
	{
		if ( S_FALSE == hr ) hr = E_FAIL;		// Change S_FALSE to E_FAIL.
		if ( FAILED(hr) ) AtlThrow( hr );		// Throw on failure.
	}

	// Creates an XML document and adds the necessary processing instructions etc.
	HRESULT	CreateXmlDocument( IXMLDOMDocument** ppXmlDoc, bool bAddPI = true );

	// Load and XML document from a file.
	HRESULT	LoadXmlFile( LPCTSTR pszFile, IXMLDOMDocument** ppXmlDoc );

	// Save an XML document to a file.
	HRESULT	SaveXmlFile( LPCTSTR pszFile, IXMLDOMDocument* pXmlDoc );

	// GetElement - general purpose
	void	GetElement( IXMLDOMElement* pElem, VARTYPE vt, CComVariantEx& varVal );

	// GetElement - wrappers for specific types
	void	GetElement( IXMLDOMElement* pElem, CComBSTR& bstrVal );
	void	GetElement( IXMLDOMElement* pElem, CString& strVal );
	void	GetElement( IXMLDOMElement* pElem, std::string& strVal );
	void	GetElement( IXMLDOMElement* pElem, std::wstring& strVal );
	void	GetElement( IXMLDOMElement* pElem, SInt8& cVal );
	void	GetElement( IXMLDOMElement* pElem, UInt8& uiVal );
	void	GetElement( IXMLDOMElement* pElem, SInt16& iVal );
	void	GetElement( IXMLDOMElement* pElem, UInt16& uiVal );
	void	GetElement( IXMLDOMElement* pElem, SInt32& lVal );
	void	GetElement( IXMLDOMElement* pElem, UInt32& ulVal );
	void	GetElement( IXMLDOMElement* pElem, SInt64& llVal );
	void	GetElement( IXMLDOMElement* pElem, UInt64& ullVal );
	void	GetElement( IXMLDOMElement* pElem, bool& boolVal );
	void	GetElement( IXMLDOMElement* pElem, int& intVal );
	void	GetElement( IXMLDOMElement* pElem, unsigned int& uintVal );
	void	GetElement( IXMLDOMElement* pElem, double& dblVal );
	void	GetElement( IXMLDOMElement* pElem, GUID& guidVal );
	void	GetElement( IXMLDOMElement* pElem, BYTE* pData, size_t& nDataLen );

	// GetElement - get object that supports IPersistXml
	template <typename Q>
	void GetElement( IXMLDOMElement* pElem, Q** ppObj )
	{
		// Sanity checks.
		ThrowIfNull( pElem );

		// Get the class id attribute.
		CLSID	clsid;
		ThrowIf( !GetAttribute( pElem, _T("clsid"), clsid ) );

		// Get the XML doc.
		CComPtr<IXMLDOMDocument>	spDoc;
		ThrowIfFailed( pElem->get_ownerDocument( &spDoc ) );

		// Create the object.
		CComPtr<Q>	spObj;
		ThrowIfFailed( spObj.CoCreateInstance( clsid ) );

		// Load it.
		CComPtr<IPersistXml>	spPersistXml;
		ThrowIfFailed( spObj->QueryInterface( &spPersistXml ) );
		ThrowIfFailed( spPersistXml->Load( spDoc, pElem ) );

		// Return it.
		ThrowIfFailed( spObj.CopyTo( ppObj ) );
	}

	// SetElement - set object that supports IPersistXml
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, IUnknown* pObj,
				bool bWriteClsidAttribute = true, bool bClearDirty = true );

	// SetElement - general purpose
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, const CComVariantEx& varVal );

	// SetElement - wrappers for specific types
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, const CComBSTR& bstrVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, const std::string& strVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, const std::wstring& strVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, LPCOLESTR pszVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, LPCSTR pszVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, SInt8 cVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, UInt8 bVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, SInt16 iVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, UInt16 uiVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, SInt32 lVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, UInt32 ulVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, SInt64 llVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, UInt64 ullVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, bool boolVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, int intVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, unsigned int uintVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, double dblVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, const GUID& guidVal );
	void	SetElement( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, const BYTE* pData, size_t nDataLen );

	// GetChildElement - get the named child element
	// Return false if the element doesn't exist
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, IXMLDOMElement** ppElem );

	// GetChildElement - general purpose
	// Return false if the element doesn't exist
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName,
				VARTYPE vt, CComVariantEx& varVal );

	// GetChildElement - wrappers for specific types
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, CComBSTR& bstrVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, CString& strVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, std::string& strVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, std::wstring& strVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, SInt8& cVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, UInt8& bVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, SInt16& iVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, UInt16& uiVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, SInt32& lVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, UInt32& ulVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, SInt64& llVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, UInt64& ullVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, bool& boolVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, int& intVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, unsigned int& uintVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, double& dblVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, GUID& guidVal );
	bool	GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, BYTE* pData, size_t& nDataLen );

	// GetChildElement - get a child object that supports IPersistXml
	// Return false if the element doesn't exist, otherwise throws on error
	template <typename Q>
	bool GetChildElement( IXMLDOMNode* pNode, LPCTSTR pszName, Q** ppObj )
	{
		// Get the element.
		CComPtr<IXMLDOMElement>	spElem;
		if ( !GetChildElement( pNode, pszName, &spElem ) )
		{
			return false;
		}

		// Get the object.
		GetElement( spElem, ppObj );

		return true;
	}

	// Create an element within a namespace
	void	CreateElementNS( IXMLDOMDocument* pDoc, LPCTSTR pszName,
				LPCTSTR pszNamespaceURI, IXMLDOMElement** ppElem );

	// AddChildElementNS - create a child element within a namespace
	void	AddChildElementNS( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, LPCTSTR pszNamespaceURI, const CComVariantEx& varVal,
				IXMLDOMElement** ppElem = NULL );

	// AddChildElement - create empty child element
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, IXMLDOMElement** ppElem = NULL );

	// AddChildElement - add a child object that supports IPersistXml
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, IUnknown* pObj, IXMLDOMElement** ppElem = NULL,
				bool bWriteClsidAttribute = true, bool bClearDirty = true );

	// AddChildElement - general purpose
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, const CComVariantEx& varVal, IXMLDOMElement** ppElem = NULL );

	// AddChildElement - wrappers for specific types
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, const CComBSTR& bstrVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, const std::string& strVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, const std::wstring& strVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, LPCOLESTR pszVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, LPCSTR pszVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, SInt8 cVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, UInt8 bVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, SInt16 iVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, UInt16 uiVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, SInt32 lVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, UInt32 ulVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, SInt64 llVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, UInt64 ullVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, bool boolVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, int intVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, unsigned int uintVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, double dblVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, const GUID& guidVal, IXMLDOMElement** ppElem = NULL );
	void	AddChildElement( IXMLDOMDocument* pDoc, IXMLDOMNode* pNode,
				LPCTSTR pszName, const BYTE* pData, size_t nDataLen, IXMLDOMElement** ppElem = NULL );

	// GetAttribute - general purpose
	// Return false if the attribute doesn't exist
	bool	GetAttribute( IXMLDOMElement* pElement, LPCTSTR pszName,
				VARTYPE vt, CComVariantEx& varVal );

	// GetAttribute - wrappers for specific types
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, CComBSTR& bstrVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, CString& strVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, std::string& strVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, std::wstring& strVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt8& cVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt8& bVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt16& iVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt16& uiVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt32& lVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt32& ulVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt64& llVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt64& ullVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, bool& boolVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, int& intVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, unsigned int& uintVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, double& dblVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, GUID& guidVal );
	bool	GetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, BYTE* pData, size_t& nDataLen );

	// SetAttributeNS - create an attribute within a namespace
	void	CreateAttributeNS( IXMLDOMDocument* pDoc, LPCTSTR pszName,
				LPCTSTR pszNamespaceURI, IXMLDOMAttribute** ppAttr );

	// SetAttributeNS - set an attribute within a namespace
	void	SetAttributeNS( IXMLDOMElement* pElem, LPCTSTR pszName,
				LPCTSTR pszNamespaceURI, const CComVariantEx& varVal );

	// SetAttribute - general purpose
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, const CComVariantEx& varVal );

	// SetAttribute - wrappers for specific types
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, const CComBSTR& bstrVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, const std::string& strVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, const std::wstring& strVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, LPCOLESTR pszVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, LPCSTR pszVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt8 cVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt8 bVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt16 iVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt16 uiVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt32 lVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt32 ulVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, SInt64 llVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, UInt64 ullVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, bool boolVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, int intVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, unsigned int uintVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, double dblVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, const GUID& guidVal );
	void	SetAttribute( IXMLDOMElement* pElem, LPCTSTR pszName, const BYTE* pData, size_t nDataLen );

	CString	SanitizeWhitespace( const CString& inString );
    CString GetNodeText( IXMLDOMNode* pNode );

	// Utility for adding tabs and returns to an xml document to get 'pretty' formatted doc for save.
	HRESULT	PrettyFormatXml( IXMLDOMDocument* pDoc, LPCTSTR pszIndent = NULL );
	HRESULT	PrettyFormatXml( IXMLDOMNode* pNode, CString& strIndent, int& indentOrigLen );

	
	// =========================================================================
	//	ChildIterator
	// =========================================================================
	//
	// Given a parent node and a tag name, iterate through all child
	// elements with that tag.
	//
	// A C++ wrapper for IXMLDOMNodeList that cuts down on some of the
	// boilerplate you gotta type when iterating through XML children.
	//
	//	Example: iterate through 3 "row" children
	//
	//		<row-list>					<-- spRowListNode
	//			<row>...</row>			
	//			<row>...</row>
	//			<row>...</row>
	//		</row-list>
	//		
	//		// for each <row>
	//		ChildIterator	rowIter( spRowListNode, _T( "row" ) );
	//		while ( rowIter.hasNext() )
	//		{
	//			CComPtr<IXMLDOMElement>	spRowElement;
	//			rowIter.next( &spRowElement );
	//	
	//			... use spRowElement in a sentence
	//		}
	class ChildIterator
	{
	public:
		ChildIterator(
			IXMLDOMNode*	inParentNode,
			LPCTSTR			inChildTagName,
			bool			bAllDescendents = false );

		// probably could add a __ct( IXMLDOMNodeList * ) overload
		// if we ever needed one. 
		
		ChildIterator();
		ChildIterator( const ChildIterator& inOriginal );
		~ChildIterator();
		ChildIterator& operator = ( const ChildIterator& inOriginal );

		long				getChildCount() const { return m_nChildCount; }
		bool				hasNext() const;
		void				next( IXMLDOMElement** ppElem );

	private:
		CComPtr<IXMLDOMNodeList>	m_spChildNodeList;
		long						m_nChildCount;
		long						m_nCurrentIndex;
	};

} /* namespace XmlUtil */

#endif /* XMLUTIL_H */
