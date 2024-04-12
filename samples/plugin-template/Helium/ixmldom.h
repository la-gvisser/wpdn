// =============================================================================
//	ixmldom.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef IXMLDOM_H
#define IXMLDOM_H

#include "heunk.h"
#include "hestr.h"
#include "hevariant.h"

namespace Xml
{

class IXMLDOMImplementation;
class IXMLDOMNode;
class IXMLDOMDocumentFragment;
class IXMLDOMDocument;
class IXMLDOMDocument2;
class IXMLDOMNodeList;
class IXMLDOMNamedNodeMap;
class IXMLDOMCharacterData;
class IXMLDOMAttribute;
class IXMLDOMElement;
class IXMLDOMText;
class IXMLDOMComment;
class IXMLDOMProcessingInstruction;
class IXMLDOMCDATASection;
class IXMLDOMDocumentType;
class IXMLDOMNotation;
class IXMLDOMEntity;
class IXMLDOMEntityReference;
class IXMLDOMParseError;
class IXMLDOMSchemaCollection;

enum DOMNodeType
{
	NODE_INVALID,
	NODE_ELEMENT,
	NODE_ATTRIBUTE,
	NODE_TEXT,
	NODE_CDATA_SECTION,
	NODE_ENTITY_REFERENCE,
	NODE_ENTITY,
	NODE_PROCESSING_INSTRUCTION,
	NODE_COMMENT,
	NODE_DOCUMENT,
	NODE_DOCUMENT_TYPE,
	NODE_DOCUMENT_FRAGMENT,
	NODE_NOTATION
};


#define IXMLDOMImplementation_IID \
{ 0x2933BF8F, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMImplementation : public Helium::IHeDispatch
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMImplementation_IID)

	HE_IMETHOD hasFeature(/*in*/ Helium::HEBSTR bstrFeature,
		/*in*/ Helium::HEBSTR bstrVersion, /*out*/ Helium::HEVARBOOL* bHasFeature) = 0;
};


#define IXMLDOMNode_IID \
{ 0x2933BF80, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMNode : public Helium::IHeDispatch
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMNode_IID);

	HE_IMETHOD get_nodeName(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	HE_IMETHOD get_nodeValue(/*out*/ Helium::HEVARIANT* pValue) = 0;
	HE_IMETHOD put_nodeValue(/*in*/ Helium::HEVARIANT value) = 0;
	HE_IMETHOD get_nodeType(/*out*/ DOMNodeType* pType) = 0;
	HE_IMETHOD get_parentNode(/*out*/ IXMLDOMNode** ppParent) = 0;
	HE_IMETHOD get_childNodes(/*out*/ IXMLDOMNodeList** ppChildList) = 0;
	HE_IMETHOD get_firstChild(/*out*/ IXMLDOMNode** ppFirstChild) = 0;
	HE_IMETHOD get_lastChild(/*out*/ IXMLDOMNode** ppLastChild) = 0;
	HE_IMETHOD get_previousSibling(/*out*/ IXMLDOMNode** ppPreviousSibling) = 0;
	HE_IMETHOD get_nextSibling(/*out*/ IXMLDOMNode** ppNextSibling) = 0;
	HE_IMETHOD get_attributes(/*out*/ IXMLDOMNamedNodeMap** ppAttributeMap) = 0;
	HE_IMETHOD insertBefore(/*in*/ IXMLDOMNode* pNewChild, /*in*/ Helium::HEVARIANT refChild,
		/*out*/ IXMLDOMNode** ppOutNewChild) = 0;
	HE_IMETHOD replaceChild(/*in*/ IXMLDOMNode* pNewChild, /*in*/ IXMLDOMNode* pOldChild,
		/*out*/ IXMLDOMNode** ppOutOldChild) = 0;
	HE_IMETHOD removeChild(/*in*/ IXMLDOMNode* pChildNode,
		/*out*/ IXMLDOMNode** pOldChild) = 0;
	HE_IMETHOD appendChild(/*in*/ IXMLDOMNode* pNewChild,
		/*out*/ IXMLDOMNode** ppOutNewChild) = 0;
	HE_IMETHOD hasChildNodes(/*out*/ Helium::HEVARBOOL* pbHasChild) = 0;
	HE_IMETHOD get_ownerDocument(/*out*/ IXMLDOMDocument** ppDOMDocument) = 0;
	HE_IMETHOD cloneNode(/*in*/ Helium::HEVARBOOL bDeep, /*out*/ IXMLDOMNode** ppCloneRoot) = 0;
	HE_IMETHOD get_nodeTypeString(/**/ Helium::HEBSTR* pbstrNodeType) = 0;
	HE_IMETHOD get_text(/*out*/ Helium::HEBSTR* text) = 0;
	HE_IMETHOD put_text(/*in*/ Helium::HEBSTR text) = 0;
	HE_IMETHOD get_specified(/*out*/ Helium::HEVARBOOL* isSpecified) = 0;
	HE_IMETHOD get_definition(/*out*/ IXMLDOMNode** ppDefinitionNode) = 0;
	HE_IMETHOD get_nodeTypedValue(/*out*/ Helium::HEVARIANT* pTypedValue) = 0;
	HE_IMETHOD put_nodeTypedValue(/*in*/ Helium::HEVARIANT typedValue) = 0;
	HE_IMETHOD get_dataType(/*out*/ Helium::HEVARIANT* pDataTypeName) = 0;
	HE_IMETHOD put_dataType(/*in*/ Helium::HEBSTR dataTypeName) = 0;
	HE_IMETHOD get_xml(/*out*/ Helium::HEBSTR* pbstrXmlString) = 0;
	HE_IMETHOD transformNode(/*in*/ IXMLDOMNode* pStylesheet,
		/*out*/ Helium::HEBSTR* pbstrXxmlString) = 0;
	HE_IMETHOD selectNodes(/*in*/ Helium::HEBSTR queryString,
		/*out*/ IXMLDOMNodeList** ppResultList) = 0;
	HE_IMETHOD selectSingleNode(/*in*/ Helium::HEBSTR queryString,
		/*out*/ IXMLDOMNode** ppResultNode) = 0;
	HE_IMETHOD get_parsed(/*out*/ Helium::HEVARBOOL* isParsed) = 0;
	HE_IMETHOD get_namespaceURI(/*out*/ Helium::HEBSTR* namespaceURI) = 0;
	HE_IMETHOD get_prefix(/*out*/ Helium::HEBSTR* ppPrefixString) = 0;
	HE_IMETHOD get_baseName(/*out*/ Helium::HEBSTR* ppbstrNameString) = 0;
	HE_IMETHOD transformNodeToObject(/*in*/ IXMLDOMNode* stylesheet, /*in*/ Helium::HEVARIANT outputObject) = 0;
};


#define IXMLDOMDocumentFragment_IID \
{ 0x3EFAA413, 0x272F, 0x11D2, {0x83, 0x6F, 0x00, 0x00, 0xF8, 0x7A, 0x77, 0x82} }

class HE_NO_VTABLE IXMLDOMDocumentFragment : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMDocumentFragment_IID)
};


#define IXMLDOMDocument_IID \
{ 0x2933BF81, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMDocument : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMDocument_IID)

	HE_IMETHOD get_doctype(/*out*/ IXMLDOMDocumentType** documentType) = 0;
	HE_IMETHOD get_implementation(/*out*/ IXMLDOMImplementation** impl) = 0;
	HE_IMETHOD get_documentElement(/*out*/ IXMLDOMElement** DOMElement) = 0;
	HE_IMETHOD putref_documentElement(/*in*/ IXMLDOMElement* DOMElement) = 0;
	HE_IMETHOD createElement(/*in*/ Helium::HEBSTR tagName,
		/*out*/ IXMLDOMElement** element) = 0;
	HE_IMETHOD createDocumentFragment(/*out*/ IXMLDOMDocumentFragment** docFrag) = 0;
	HE_IMETHOD createTextNode(/*in*/ Helium::HEBSTR data,
		/*out*/ IXMLDOMText** text) = 0;
	HE_IMETHOD createComment(/*in*/ Helium::HEBSTR data,
		/*out*/ IXMLDOMComment** comment) = 0;
	HE_IMETHOD createCDATASection(/*in*/ Helium::HEBSTR data,
		/*out*/ IXMLDOMCDATASection** cdata) = 0;
	HE_IMETHOD createProcessingInstruction(/*in*/ Helium::HEBSTR target,
		/*in*/ Helium::HEBSTR data, /*out*/ IXMLDOMProcessingInstruction** pi) = 0;
	HE_IMETHOD createAttribute(/*in*/ Helium::HEBSTR name,
		/*out*/ IXMLDOMAttribute** attribute) = 0;
	HE_IMETHOD createEntityReference(/*in*/ Helium::HEBSTR name,
		/*out*/ IXMLDOMEntityReference** entityRef) = 0;
	HE_IMETHOD getElementsByTagName(/*in*/ Helium::HEBSTR tagName,
		/*out*/ IXMLDOMNodeList** resultList) = 0;
	HE_IMETHOD createNode(/*in*/ Helium::HEVARIANT Type,
		/*in*/ Helium::HEBSTR name, /*in*/ Helium::HEBSTR namespaceURI,
		/*out*/ IXMLDOMNode** node) = 0;
	HE_IMETHOD nodeFromID(/*in*/ Helium::HEBSTR idString,
		/*out*/ IXMLDOMNode** node) = 0;
	HE_IMETHOD load(/*in*/ Helium::HEVARIANT xmlSource, /*out*/ Helium::HEVARBOOL* isSuccessful) = 0;
	HE_IMETHOD get_readyState(/*out*/ SInt32* value) = 0;
	HE_IMETHOD get_parseError(/*out*/ IXMLDOMParseError** errorObj) = 0;
	HE_IMETHOD get_url(/*out*/ Helium::HEBSTR* urlString) = 0;
	HE_IMETHOD get_async(/*out*/ Helium::HEVARBOOL* isAsync) = 0;
	HE_IMETHOD put_async(/*in*/ Helium::HEVARBOOL	isAsync) = 0;
	HE_IMETHOD abort() = 0;
	HE_IMETHOD loadXML(/*in*/ Helium::HEBSTR bstrXML, /*out*/ Helium::HEVARBOOL* isSuccessful) = 0;
	HE_IMETHOD save(/*in*/ Helium::HEVARIANT destination) = 0;
	HE_IMETHOD get_validateOnParse(/*out*/ Helium::HEVARBOOL* isValidating) = 0;
	HE_IMETHOD put_validateOnParse(/*in*/ Helium::HEVARBOOL isValidating) = 0;
	HE_IMETHOD get_resolveExternals(/*out*/ Helium::HEVARBOOL* isResolving) = 0;
	HE_IMETHOD put_resolveExternals(/*in*/ Helium::HEVARBOOL isResolving) = 0;
	HE_IMETHOD get_preserveWhiteSpace(/*out*/ Helium::HEVARBOOL* isPreserving) = 0;
	HE_IMETHOD put_preserveWhiteSpace(/*in*/ Helium::HEVARBOOL isPreserving) = 0;
};


#define IXMLDOMDocument2_IID \
{ 0x2933BF95, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMDocument2 : public IXMLDOMDocument
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMDocument2_IID)

	HE_IMETHOD get_namespaces(/*out*/ IXMLDOMSchemaCollection** namespaceCollection) = 0;
	HE_IMETHOD get_schemas(/*out*/ Helium::HEVARIANT* otherCollection) = 0;
	HE_IMETHOD putref_schemas(/*in*/ Helium::HEVARIANT otherCollection) = 0;
	HE_IMETHOD validate(/*out*/ IXMLDOMParseError** errorObj) = 0;
	HE_IMETHOD setProperty(/*in*/ Helium::HEBSTR name, /*in*/ Helium::HEVARIANT value) = 0;
	HE_IMETHOD getProperty(/*in*/ Helium::HEBSTR name, /*out*/ Helium::HEVARIANT* value) = 0;
};


#define IXMLDOMNodeList_IID \
{ 0x2933BF82, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMNodeList : public Helium::IHeDispatch
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMNodeList_IID)

	HE_IMETHOD get_item(/*in*/ SInt32 index, /*out*/ IXMLDOMNode** pNode) = 0;
	HE_IMETHOD get_length(/*out*/ SInt32* pcLength) = 0;
	HE_IMETHOD nextNode(/*out*/ IXMLDOMNode** nextItem) = 0;
	HE_IMETHOD reset() = 0;
	HE_IMETHOD get__newEnum(/*out*/ Helium::IHeUnknown** ppUnk) = 0;
};


#define IXMLDOMNamedNodeMap_IID \
{ 0x2933BF83, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMNamedNodeMap : public	Helium::IHeDispatch
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMNamedNodeMap_IID)

	HE_IMETHOD getNamedItem(/*in*/ Helium::HEBSTR name,
		/*out*/ IXMLDOMNode** namedItem) = 0;
	HE_IMETHOD setNamedItem(/*in*/ IXMLDOMNode* newItem,
		/*out*/ IXMLDOMNode** nameItem) = 0;
	HE_IMETHOD removeNamedItem(/*in*/ Helium::HEBSTR name,
		/*out*/ IXMLDOMNode** namedItem) = 0;
	HE_IMETHOD get_item(/*in*/ SInt32 index,
		/*out*/ IXMLDOMNode** listItem) = 0;
	HE_IMETHOD get_length(/*out*/ SInt32* listLength) = 0;
	HE_IMETHOD getQualifiedItem(/*in*/ Helium::HEBSTR baseName,
		/*in*/ Helium::HEBSTR namespaceURI,
		/*out*/ IXMLDOMNode** qualifiedItem) = 0;
	HE_IMETHOD removeQualifiedItem(/*in*/ Helium::HEBSTR baseName,
		/*in*/ Helium::HEBSTR namespaceURI,
		/*out*/ IXMLDOMNode** qualifiedItem) = 0;
	HE_IMETHOD nextNode(/*out*/ IXMLDOMNode** nextItem) = 0;
	HE_IMETHOD reset() = 0;
	HE_IMETHOD get__newEnum(/*out*/ Helium::IHeUnknown** ppUnk) = 0;
};


#define IXMLDOMCharacterData_IID \
{ 0x2933BF84, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMCharacterData : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMCharacterData_IID)

	HE_IMETHOD get_data(/*out*/ Helium::HEBSTR* data) = 0;
	HE_IMETHOD put_data(/*in*/ Helium::HEBSTR data) = 0;
	HE_IMETHOD get_length(/*out*/ SInt32* dataLength) = 0;
	HE_IMETHOD substringData(/*in*/ SInt32 offset, /*in*/ SInt32 count,
		/*out*/ Helium::HEBSTR *data) = 0;
	HE_IMETHOD appendData(/*in*/ Helium::HEBSTR data) = 0;
	HE_IMETHOD insertData(/*in*/ SInt32 offset, /*in*/ Helium::HEBSTR data) = 0;
	HE_IMETHOD deleteData(/*in*/ SInt32 offset, /*in*/ SInt32 count) = 0;
	HE_IMETHOD replaceData(/*in*/ SInt32 offset, /*in*/ SInt32 count,
		/*in*/ Helium::HEBSTR data) = 0;
};


#define IXMLDOMAttribute_IID \
{ 0x2933BF85, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMAttribute	: public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMAttribute_IID)

	HE_IMETHOD get_name(/*out*/ Helium::HEBSTR* pbstrName) = 0;
	HE_IMETHOD get_value(/*out*/ Helium::HEVARIANT* pValue) = 0;
	HE_IMETHOD put_value(/*in*/ Helium::HEVARIANT value) = 0;
};


#define IXMLDOMElement_IID \
{ 0x2933BF86, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMElement :	public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMElement_IID)

	HE_IMETHOD get_tagName(/*out*/ Helium::HEBSTR* tagName) = 0;
	HE_IMETHOD getAttribute(/*in*/ Helium::HEBSTR name, /*out*/ Helium::HEVARIANT* value) = 0;
	HE_IMETHOD setAttribute(/*in*/ Helium::HEBSTR name, /*in*/ Helium::HEVARIANT value) = 0;
	HE_IMETHOD removeAttribute(/*in*/ Helium::HEBSTR name) = 0;
	HE_IMETHOD getAttributeNode(/*in*/ Helium::HEBSTR name, /*out*/ IXMLDOMAttribute** attributeNode) = 0;
	HE_IMETHOD setAttributeNode(/*in*/ IXMLDOMAttribute* DOMAttribute, /*out*/ IXMLDOMAttribute** attributeNode) = 0;
	HE_IMETHOD removeAttributeNode(/*in*/ IXMLDOMAttribute* DOMAttribute, /*out*/ IXMLDOMAttribute** attributeNode) = 0;
	HE_IMETHOD getElementsByTagName(/*in*/ Helium::HEBSTR tagName, /*out*/ IXMLDOMNodeList** resultList) = 0;
	HE_IMETHOD normalize() = 0;
};


#define IXMLDOMText_IID \
{ 0x2933BF87, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMText : public IXMLDOMCharacterData
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMText_IID)

	HE_IMETHOD splitText(/*in*/ SInt32 offset,
		/*out*/ IXMLDOMText** rightHandTextNode) = 0;
};


#define IXMLDOMComment_IID \
{ 0x2933BF88, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMComment : public IXMLDOMCharacterData
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMComment_IID)
};


#define IXMLDOMProcessingInstruction_IID \
{ 0x2933BF89, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMProcessingInstruction	: public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMProcessingInstruction_IID)

	HE_IMETHOD get_target(/*out*/ Helium::HEBSTR* name) = 0;
	HE_IMETHOD get_data(/*out*/ Helium::HEBSTR* value) = 0;
	HE_IMETHOD put_data(/*in*/ Helium::HEBSTR value) = 0;
};


#define IXMLDOMCDATASection_IID \
{ 0x2933BF8A, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMCDATASection : public IXMLDOMText
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMCDATASection_IID)
};


#define IXMLDOMDocumentType_IID \
{ 0x2933BF8B, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMDocumentType : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMDocumentType_IID)

	HE_IMETHOD get_name(/*out*/ Helium::HEBSTR* pbstrRootName) = 0;
	HE_IMETHOD get_entities(/*out*/ IXMLDOMNamedNodeMap** ppEntityMap) = 0;
	HE_IMETHOD get_notations(/*out*/ IXMLDOMNamedNodeMap** ppNotationMap) = 0;
};


#define IXMLDOMNotation_IID \
{ 0x2933BF8C, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMNotation : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMNotation_IID)

	HE_IMETHOD get_publicId(/*out*/ Helium::HEVARIANT* publicID) = 0;
	HE_IMETHOD get_systemId(/*out*/ Helium::HEVARIANT* systemID) = 0;
};


#define IXMLDOMEntity_IID \
{ 0x2933BF8D, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMEntity : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMEntity_IID)

	HE_IMETHOD get_publicId(/*out*/ Helium::HEVARIANT* publicID) = 0;
	HE_IMETHOD get_systemId(/*out*/ Helium::HEVARIANT* systemID) = 0;
	HE_IMETHOD get_notationName(/*out*/ Helium::HEBSTR* name) = 0;
};


#define IXMLDOMEntityReference_IID \
{ 0x2933BF8E, 0x7B36, 0x11D2, {0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60} }

class HE_NO_VTABLE IXMLDOMEntityReference : public IXMLDOMNode
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMEntityReference_IID)
};


#define IXMLDOMParseError_IID \
{ 0x3efaa426, 0x272f, 0x11D2, {0x83, 0x6f, 0x00, 0x00, 0xf8, 0x7a, 0x77, 0x82} }

class HE_NO_VTABLE IXMLDOMParseError : public Helium::IHeDispatch
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMParseError_IID)

	HE_IMETHOD get_errorCode(/*out*/ SInt32* errorCode) = 0;
	HE_IMETHOD get_url(/*out*/ Helium::HEBSTR* urlString) = 0;
	HE_IMETHOD get_reason(/*out*/ Helium::HEBSTR* reasonString) = 0;
	HE_IMETHOD get_srcText(/*out*/ Helium::HEBSTR* sourceString) = 0;
	HE_IMETHOD get_line(/*out*/ SInt32* lineNumber) = 0;
	HE_IMETHOD get_linepos(/*out*/ SInt32* linePosition) = 0;
	HE_IMETHOD get_filepos(/*out*/ SInt32* filePosition) = 0;
};


#define IXMLDOMSchemaCollection_IID \
{ 0x373984C8, 0xB845, 0x449B, {0x91, 0xE7, 0x45, 0xAC, 0x83, 0x03, 0x6A, 0xDE} }

class HE_NO_VTABLE IXMLDOMSchemaCollection : public Helium::IHeDispatch
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IXMLDOMSchemaCollection_IID)

	HE_IMETHOD add(/*in*/ Helium::HEBSTR namespaceURI, /*in*/ Helium::HEVARIANT var) = 0;
	HE_IMETHOD get(/*in*/ Helium::HEBSTR namespaceURI, /*out*/ IXMLDOMNode** schemaNode) = 0;
	HE_IMETHOD remove(/*in*/ Helium::HEBSTR namespaceURI) = 0;
	HE_IMETHOD get_length(/*out*/ SInt32* length) = 0;
	HE_IMETHOD get_namespaceURI(/*in*/ SInt32 index, /*out*/ Helium::HEBSTR* length) = 0;
	HE_IMETHOD addCollection(/*in*/ IXMLDOMSchemaCollection* otherCollection) = 0;
	HE_IMETHOD get__newEnum(/*out*/ Helium::IHeUnknown** ppUnk) = 0;
};


#define XML_DOCUMENT_CID \
{ 0xF6D90F11, 0x9C73, 0x11D3, {0xB3, 0x2E, 0x00, 0xC0, 0x4F, 0x99, 0x0B, 0xB4} }

} /* namespace	Xml */

#endif /* IXMLDOM_H */
