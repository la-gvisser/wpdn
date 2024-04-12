// =============================================================================
//	PeekXml.h
// =============================================================================
//	Copyright (c) 2010-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "Peek.h"
#include "ixmldom.h"
#include "PeekStrings.h"
#include "PeekStream.h"
#include <map>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Peek XML
//
//	Peek XML is a wrapper for Omni's core IXMLDOM objects, with additional
//	services. The Peek Data Modeler relies on Peek XML.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


class CPeekXmlList;

typedef CHePtr<Xml::IXMLDOMDocument>	CPeekXmlDocument;
typedef CHePtr<Xml::IXMLDOMElement>		CPeekXmlElement;
typedef	CHePtr<Xml::IXMLDOMNodeList>	CPeekXmlNodeList;
typedef CHePtr<Helium::IHePersistFile>	CPeekPersistFile;

typedef std::map<std::wstring,CPeekXmlNodeList>				CPeekXmlNodeMap;
typedef std::map<std::wstring,CPeekXmlNodeList>::iterator	CPeekXmlNodeMapItr;


// =============================================================================
//		CPeekXml
// =============================================================================

class CPeekXml
{
	friend class CPeekXmlList;

protected:
	CPeekXmlElement			m_spElement;
	CPeekXmlDocument		m_spCreatedDoc;	// The Doc created by this.
	mutable CPeekXmlNodeMap	m_NodeMap;

public:
	static bool	Validate( IHeUnknown* inDocument, IHeUnknown* inElement )
	{
		if ( inDocument && inElement ) {
			CPeekXmlDocument	spDocument;
			inDocument->QueryInterface( &spDocument.p );

			CPeekXmlElement	spElement;
			inElement->QueryInterface( &spElement.p );
			if ( spElement != nullptr ) {
				CPeekXmlDocument	spOwner;
				spElement->get_ownerDocument( &spOwner.p );
				return (spOwner == spDocument);
			}
		}
		return false;
	}

public:
	;			CPeekXml() {}
	;			CPeekXml( IHeUnknown* inDocument, IHeUnknown* inElement )
	{
#if _DEBUG
		_ASSERTE( Validate( inDocument, inElement ) );
#endif
		if ( inElement ) {
			if ( inDocument ) {
				inElement->QueryInterface( &m_spCreatedDoc.p );
			}
			inElement->QueryInterface( &m_spElement.p );
		}
	}
	;			CPeekXml( Xml::IXMLDOMDocument* inDocument, Xml::IXMLDOMElement* inElement )
		:	m_spElement( inElement )
	{
		(void)inDocument;
#if _DEBUG
		_ASSERTE( Validate( inDocument, inElement ) );
#endif
	}
	;			CPeekXml( const CPeekXml& inOther )
					:	m_spElement( inOther.m_spElement )
					,	m_spCreatedDoc( inOther.m_spCreatedDoc )
	{
	}
	virtual		~CPeekXml()
	{
		Close();
	}

	operator CPeekXmlDocument() const { return m_spCreatedDoc; }

	operator CPeekPersistFile() const {
		if ( m_spCreatedDoc == nullptr ) {
			CPeekXmlDocument	spOwner;
			m_spElement->get_ownerDocument( &spOwner.p );
			if ( spOwner ) {
				CHePtr<Helium::IHePersistFile>	spPersistFile;
				spOwner.QueryInterface( &spPersistFile.p );
				return spPersistFile;
			}
		}
		else {
			CHePtr<Helium::IHePersistFile>	spPersistFile;
			m_spCreatedDoc.QueryInterface( &spPersistFile.p );
			return spPersistFile;
		}
		return nullptr;
	}

	virtual void	Close() {
		m_spElement = nullptr;
		m_spCreatedDoc = nullptr;
		m_NodeMap.clear();
	}
	void			Create( const wchar_t* inName, bool inAsync = false, bool inProcInstr = false );
	CPeekXml		CreateChild( const wchar_t* inName, const wchar_t* inValue = nullptr );

	bool			GetAttribute( const wchar_t* inName, CPeekString& outAttribute );
	bool			GetAttribute( const wchar_t* inName, HeLib::CHeVariant& outAttribute,
						Helium::HEVARTYPE inType = Helium::HE_VT_BSTR );
	CPeekXml		GetChild( const wchar_t* inName, size_t inIndex = 0 );
	UInt32			GetChildCount( const wchar_t* inName = nullptr ) const;
	CPeekXmlElement	GetElement() { return m_spElement; }
	bool			GetLoadStore( CPeekXmlDocument& outDocument, CPeekXmlElement& outElement ) const {
		if ( m_spElement ) {
			m_spElement->get_ownerDocument( &outDocument.p );
			outElement = m_spElement;
			return true;
		}
		if ( m_spCreatedDoc ) {
			CHeQIPtr<Xml::IXMLDOMDocument> spDocument( m_spCreatedDoc );
			Xml::IXMLDOMNode*	pNode( nullptr );
			spDocument->get_firstChild( &pNode );
			CHeQIPtr<Xml::IXMLDOMElement> spElement( pNode );
			if ( spDocument && spElement && Validate( spDocument, spElement ) ) {
				outDocument = spDocument;
				outElement = spElement;
				return true;
			}
		}
		return false;
	}
	CPeekString		GetName();
	bool			GetValue( HeLib::CHeVariant& outValue,
						Helium::HEVARTYPE inType = Helium::HE_VT_BSTR );
	bool			GetValue( CPeekString& outValue );

	bool	HasValue() const;

	bool	IsNotValid() const { return (m_spElement == nullptr); }
	bool	IsOpen() const { return (m_spElement != nullptr); }
	bool	IsValid() const { return (m_spElement != nullptr); }

	void	Load( const CPeekStream& inStream );
	void	Load( size_t inLength, const UInt8* inData );
	void	Load( const wchar_t* inFileName, const wchar_t* inRootName );

	void	Save( Helium::IHeStream* outStream ) const;
	void	SetAttribute( const wchar_t* inName, const HeLib::CHeVariant& inAttribute );
	void	SetAttribute( const wchar_t* inName, const Helium::HEBSTR* inAttribute );
	void	SetValue( const HeLib::CHeVariant& inValue );
	void	SetValue( const wchar_t* inValue ) {
		CHeBSTR	bstrValue = inValue;
		SetValue( bstrValue.m_str );
	}
	void	SetValue( const Helium::HEBSTR inValue );
};
