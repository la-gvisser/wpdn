// =============================================================================
//	XMLDoc.h
// =============================================================================
//	Copyright (c) 1999-2013 WildPackets, Inc. All rights reserved.

#ifndef WILDPACKETS_XMLDOC_H
#define WILDPACKETS_XMLDOC_H

#include "ByteStream.h"
#include "XObject.h"
#include "XString.h"

class XXMLDoc;

class DECLSPEC XXMLAttribute :
	virtual public XObject
{
public:
	XXMLAttribute( const XString& inName, const XString& inValue, bool bCDATA = false );
	XXMLAttribute();

	virtual ~XXMLAttribute() {}

	XXMLAttribute& operator =(const XXMLAttribute& other)
	{
		m_strName = other.GetName();
		m_strValue = other.GetValue();
		m_bValueCDATA = other.m_bValueCDATA;
		return *this;
	}
	
	inline const XString&	GetName() const { return m_strName; }
	inline const XString&	GetValue() const { return m_strValue; }

	inline void	SetValue( const TCHAR* szVal, bool bCDATA ) { m_strValue = szVal; m_bValueCDATA = bCDATA; }
	inline void	SetName( const TCHAR* szName ) { m_strName = szName; }

	inline bool	IsCDATA() const { return m_bValueCDATA; }

protected:
	XString		m_strName;			// Name.
	XString		m_strValue;			// Value.
	bool		m_bValueCDATA;		// Value is a CDATA block
};


class DECLSPEC XXMLElement :
	virtual public XObject
{
public:
						XXMLElement( const XString& inName, bool inOwnsChildren = true );
						XXMLElement();

	virtual				~XXMLElement();
	
	void				AddValue( const XString& inText, bool bCDATA = false);
	void				AddValue( const TCHAR inChar );
	
	const XString&		GetName() const { return m_strName; }
	const XString&		GetValue() const { return m_strValue;}
		
	inline SInt32		CountChildren() const { return m_nChildCount; }
	inline SInt32		CountAttributes() const { return m_nAttributeCount; }
	
	// Get elements.
	const XXMLElement*	GetChild( SInt32 inIndex ) const;
	XXMLElement*		GetChild( SInt32 inIndex );

	const XXMLElement*	GetNamedChild( const TCHAR* inChildName ) const;
	XXMLElement*		GetNamedChild( const TCHAR* inChildName );

	const XXMLElement*	GetNamedChild( const TCHAR* inChildName, SInt32 inIndex ) const;
	XXMLElement*		GetNamedChild( const TCHAR* inChildName, SInt32 inIndex );

	bool				GetNamedChildValue( const TCHAR* inChildName, 
							XString& outValue, SInt32 inIndex = 0 ) const;
	SInt32				GetNamedChildCount( const TCHAR* inChildName ) const;
	
	// Get attributes.
	const XXMLAttribute*	GetAttribute( SInt32 inIndex ) const;
	XXMLAttribute*			GetAttribute( SInt32 inIndex );

	const XXMLAttribute*	GetNamedAttribute( const TCHAR* inAttributeName ) const;
	XXMLAttribute*			GetNamedAttribute( const TCHAR* inAttributeName );

	bool				GetNamedAttributeValue( const TCHAR* inAttributeName, 
							XString& outValue ) const;
	
	// Add child elements.
	bool				AddChild( XXMLElement* inChild );
	bool				AddChild( const TCHAR* inElementName, const TCHAR* inElementValue );
	bool				ReplaceChild( XXMLElement* inCurChild, XXMLElement* inNewChild );	

	// Add attributes.
	bool				AddAttribute( XXMLAttribute* inAttribute );
	bool				AddAttribute( const TCHAR* inAttributeName, const TCHAR* inAttributeValue, bool bIsCDATA = false );
	bool				AddAttribute( const TCHAR* inAttributeName, UInt32 inAttributeValue );
	bool				AddAttribute( const TCHAR* inAttributeName, SInt32 inAttributeValue );

	bool				OwnsChildren() { return m_bOwnsChildren; }

	X_RESULT			Reset();

	void				SetName( LPCTSTR lpszName ) { m_strName = lpszName; }
	void				SetValue( LPCTSTR lpszValue ) {	m_strValue = lpszValue; }
	void				SetCDATA( bool bValueCDATA ) { m_bValueCDATA = bValueCDATA; }

	bool				IsCDATA() const { return m_bValueCDATA; }

	// Remove child
	bool				RemoveChild( XXMLElement* inChild, bool bForceDelete = false );
	bool				DetachChild( XXMLElement* inChild );
	bool				RemoveAttribute( XXMLAttribute* inAttrib, bool bForceDelete = false);

protected:
	XString				m_strName;			// Name of this element.
	XString				m_strValue;			// Value of this element.
	
	SInt32				m_nAttributeCount;
	SInt32				m_nChildCount;

	XXMLAttribute**		m_pAttributes;		// List of attributes.
	XXMLElement**		m_pChildren;		// List of child elements.

	bool				m_bOwnsChildren;	// True if we own our child elements.
	bool				m_bValueCDATA;
};


class DECLSPEC XXMLDoc : 
	virtual public XObject
{
public:
						XXMLDoc();
	virtual				~XXMLDoc();
		
	XXMLElement*		GetRootElement()		{ return m_pRootElement; }
	const XXMLElement*	GetRootElement() const	{ return m_pRootElement; }
	
	void				SetRootElement( XXMLElement* inElement )
							{ m_pRootElement = inElement; }
	
	// Build XML ByteStream from XMLDoc.
	virtual void		BuildStream( CByteStream& inStream );
	
	// Build XMLDoc from XML ByteStream.
	void				ParseStream( const CByteStream& inStream );

	// Build XMLDoc from an XML file
	void				ParseFile( LPCTSTR szPathname );

protected:
	XXMLElement*	m_pRootElement;
	XString			m_strEncoding;
	XString			m_strVersion;
};

#endif /* WILDPACKETS_XMLDOC_H */

