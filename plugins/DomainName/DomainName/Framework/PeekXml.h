// =============================================================================
//	PeekXml.h
// =============================================================================
//	Copyright (c) 2010-2011 WildPackets, Inc. All rights reserved.

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
			if ( spElement != NULL ) {
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
		if ( inDocument ) {
			inElement->QueryInterface( &m_spCreatedDoc.p );
		}
		if ( inElement ) {
			inElement->QueryInterface( &m_spElement.p );
		}
	}
	;			CPeekXml( Xml::IXMLDOMDocument* inDocument, Xml::IXMLDOMElement* inElement )
		:	m_spElement( inElement )
	{
		inDocument;
#if _DEBUG
		_ASSERTE( Validate( inDocument, inElement ) );
#endif
	}
	;			CPeekXml( const CPeekXml& inOther )
		:	m_spCreatedDoc( inOther.m_spCreatedDoc ), m_spElement( inOther.m_spElement )
	{
	}
	virtual		~CPeekXml()
	{
		Close();
	}

	operator CPeekPersistFile() const {
		if ( m_spCreatedDoc == NULL ) {
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
		return NULL;
	}

	virtual void	Close() {
		m_spElement = NULL;
		m_spCreatedDoc = NULL;
		m_NodeMap.clear();
	}
	void			Create( PCWSTR inName, bool inAsync = false, bool inProcInstr = false );
	CPeekXml		CreateChild( PCWSTR inName );

	bool			GetAttribute( PCWSTR inName, CPeekString& outAttribute );
	bool			GetAttribute( PCWSTR inName, HeLib::CHeVariant& outAttribute,
						Helium::HEVARTYPE inType = Helium::HE_VT_BSTR );
	CPeekXml		GetChild( PCWSTR inName, size_t inIndex = 0 );
	UInt32			GetChildCount( PCWSTR inName = NULL ) const;
	CPeekXmlElement	GetElement() { return m_spElement; }
	CPeekString		GetName();
	bool			GetValue( HeLib::CHeVariant& outValue,
						Helium::HEVARTYPE inType = Helium::HE_VT_BSTR );
	bool			GetValue( CPeekString& outValue );

	bool	HasValue() const;

	bool	IsNotValid() const { return (m_spElement == NULL); }
	bool	IsOpen() const { return (m_spElement != NULL); }
	bool	IsValid() const { return (m_spElement != NULL); }

	void	Load( const CPeekStream& inStream );
	void	Load( size_t inLength, const UInt8* inData );
	void	Load( PCWSTR inFileName, PCWSTR inRootName );

	void	Save( Helium::IHeStream* outStream ) const;
	void	SetAttribute( PCWSTR inName, const HeLib::CHeVariant& inAttribute );
	void	SetAttribute( PCWSTR inName, const Helium::HEBSTR* inAttribute );
	void	SetValue( const HeLib::CHeVariant& inValue );
	void	SetValue( PCWSTR inValue ) {
		CHeBSTR	bstrValue = inValue;
		SetValue( bstrValue.m_str );
	}
	void	SetValue( const Helium::HEBSTR inValue );
};
