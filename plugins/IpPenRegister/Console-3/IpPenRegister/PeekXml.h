// ============================================================================
//	CPeekXml.h
//		interface for CPeekXML class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once


// ============================================================================
//		CComVariantEx - Variant Utility class
// ============================================================================

class CComVariantEx
	:	public tagVARIANT
{
public:
	;		CComVariantEx() { ::VariantInit( this ); }
	;		CComVariantEx( long inSrc ) { vt = VT_I4; lVal = inSrc; }
	;		CComVariantEx( LPCSTR inSrc ) {
				USES_CONVERSION_EX;
				vt = VT_BSTR;
				bstrVal = NULL;
				if ( inSrc ) {
					bstrVal = ::SysAllocString(
						A2COLE_EX( inSrc, _ATL_SAFE_ALLOCA_DEF_THRESHOLD ) );
					if ( bstrVal == NULL ) {
						vt = VT_ERROR;
						scode = E_OUTOFMEMORY;
					}
				}
			}
	;		CComVariantEx( LPCWSTR inSrc ) {
				vt = VT_BSTR;
				bstrVal = NULL;
				if ( inSrc ) {
					bstrVal = ::SysAllocString( inSrc );
					if ( bstrVal == NULL ) {
						vt = VT_ERROR;
						scode = E_OUTOFMEMORY;
					}
				}
			}
	;		~CComVariantEx() throw() { Clear(); }

	HRESULT	ChangeType( VARTYPE vtNew ) {
				if ( this->vt == vtNew ) return S_OK;
				return ::VariantChangeType( this, this, 0, vtNew );
			}
	void	Clear() { ::VariantClear( this ); }
};


// ============================================================================
//		CPeekXmlElement
// ============================================================================

class CPeekXmlElement
{
protected:
	CComPtr<IXMLDOMElement>		m_spElement;

	bool			AddAttribute( const CComBSTR& inName, const CComBSTR& inValue );
	static CString	GetValue( IXMLDOMElement* inElement );
	static bool		GetValue( IXMLDOMElement* inElement,
						VARTYPE inType, CComVariantEx& inValue );
	static bool		PrettyFormat( IXMLDOMNode* inNode, PCTSTR inIndent,
						int& ioIndentLength );

public:
	;		CPeekXmlElement() {}
	;		CPeekXmlElement( IXMLDOMElement* inElement )
			: m_spElement( inElement )
			{}

	operator const INT_PTR() const {
		return (m_spElement == NULL) ? NULL : (INT_PTR)-1;
	}
	CPeekXmlElement	operator =( IXMLDOMElement* inElement ) {
		m_spElement = inElement;
		return *this;
	}

	bool			AddAttribute( PCTSTR inName, PCTSTR inValue ) {
						if ( (inName == NULL) || (inValue == NULL) ) return false;
						return AddAttribute( CComBSTR( inName ), CComBSTR( inValue ) );
					}
	CPeekXmlElement	AddChild( PCTSTR inName, PCTSTR inValue = NULL );
	CPeekXmlElement	AddChild( PCTSTR inName, UInt32 inValue );
	CPeekXmlElement	AddChildEnable( PCTSTR inName, bool inEnabled );
	CPeekXmlElement	AddChildEnable( PCTSTR inName, BOOL inEnabled ) {
						return AddChildEnable( inName, (inEnabled != FALSE) );
					}

	bool	GetAttributeValue( PCTSTR inName, CString& outValue ) const;

	CPeekXmlElement	GetChild( PCTSTR inName, SInt32 inIndex = 0 ) const;
	UInt32	GetChildCount( PCTSTR inName ) const;
	bool	GetChildValue( PCTSTR inName, UInt32& outValue, SInt32 inIndex = 0 ) const;
	CString	GetValue() const { return GetValue( m_spElement ); }
	bool	GetValue( UInt32& outValue ) const;
	bool	IsEnabled() const;
	bool	IsChildEnabled( PCTSTR inName, bool inDefault = false, SInt32 inIndex = 0 ) const;
	bool	PrettyFormat( PCTSTR inIndent, int& ioIndentLength ) {
				return PrettyFormat( m_spElement, inIndent, ioIndentLength );
			}
	bool	SetEnabled( bool inEnabled );
	bool	SetValue( PCTSTR inValue );
};


// ============================================================================
//		CPeekXmlDocument
// ============================================================================

class CPeekXmlDocument
{
protected:
	CComPtr<IXMLDOMDocument>	m_spDocument;

public:
	;		CPeekXmlDocument();
	virtual	~CPeekXmlDocument() {}

	operator const INT_PTR() {
		return (m_spDocument == NULL) ? NULL : (INT_PTR)-1;
	}

	CPeekXmlElement		AddRootElement( PCTSTR inName );
	bool				Create();
	CString				Format();
	CPeekXmlElement		GetRootElement();
	bool				LoadFile( PCTSTR inFileName );
	bool				Parse( PCTSTR inXml );
	bool				PrettyFormat( PCTSTR inIndent = NULL );
	bool				StoreFile( PCTSTR inFileName );
};
