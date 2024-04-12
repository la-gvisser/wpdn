// =============================================================================
//	XMLDoc.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#pragma once

#include "XString.h"
#include "ArrayEx.h"
#include "ByteStream.h"
#include "XObject.h"

class XXMLDoc;

class DECLSPEC XXMLAttribute :
	virtual public XObject
{
public:
					XXMLAttribute( const XString& inName, const XString& inValue, bool bCDATA = false );
					XXMLAttribute();

					~XXMLAttribute() {}

	XXMLAttribute& operator =(const XXMLAttribute& other)
	{
		m_strName = other.GetName();
		m_strValue = other.GetValue();
		m_bValueCDATA = other.m_bValueCDATA;
		return *this;
	}

	const XString&	GetName() const { return m_strName; }
	const XString&	GetValue() const { return m_strValue; }

	void SetValue(const TCHAR* szVal, bool bCDATA)	{	m_strValue = szVal;	m_bValueCDATA = bCDATA;	}
	void SetName(const TCHAR* szName)	{	m_strName = szName;	}

	bool IsCDATA() const	{	return m_bValueCDATA;	}

protected:
	XString	m_strName;						// Name.
	XString	m_strValue;						// Value.
	bool	m_bValueCDATA;					// Value is a CDATA block
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

	const XString&		GetName() const	 { return m_strName; }
	const XString&		GetValue() const { return m_strValue;}

	virtual SInt32		CountChildren() const	{ return m_nChildCount;		}
	virtual SInt32		CountAttributes() const { return m_nAttributeCount; }

	// Get elements.
	virtual const XXMLElement* GetChild( SInt32 inIndex ) const;
	virtual		  XXMLElement* GetChild( SInt32 inIndex );

	virtual const XXMLElement* GetNamedChild( const TCHAR* inChildName, SInt32 inIndex = 0 ) const;
	virtual		  XXMLElement* GetNamedChild( const TCHAR* inChildName, SInt32 inIndex = 0 );

	virtual bool		GetNamedChildValue( const TCHAR* inChildName,
											XString& outValue, SInt32 inIndex = 0 ) const;
	virtual SInt32		GetNamedChildCount( const TCHAR* inChildName ) const;

	// Get attributes.
	virtual const XXMLAttribute*
						GetAttribute( SInt32 inIndex ) const;
	virtual		  XXMLAttribute*
						GetAttribute( SInt32 inIndex );

	virtual const XXMLAttribute*
						GetNamedAttribute( const TCHAR* inAttributeName ) const;
	virtual		  XXMLAttribute*
						GetNamedAttribute( const TCHAR* inAttributeName );

	virtual bool		GetNamedAttributeValue( const TCHAR* inAttributeName,
							XString& outValue ) const;

	// Add child elements.
	virtual bool		AddChild( XXMLElement* inChild );
	virtual bool		AddChild( const TCHAR* inElementName, const TCHAR* inElementValue );
	virtual bool		ReplaceChild( XXMLElement* inCurChild, XXMLElement* inNewChild );

	// Add attributes.
	virtual bool		AddAttribute( XXMLAttribute* inAttribute );
	virtual	bool		AddAttribute( const TCHAR* inAttributeName, const TCHAR* inAttributeValue, bool bIsCDATA = false );
	virtual	bool		AddAttribute( const TCHAR* inAttributeName, UInt32 inAttributeValue );
	virtual	bool		AddAttribute( const TCHAR* inAttributeName, SInt32 inAttributeValue );

	bool				OwnsChildren() { return m_bOwnsChildren; }

	virtual int			Reset();

	inline void			SetName(LPCTSTR lpszName)	{	m_strName = lpszName;	}
	inline void			SetValue(LPCTSTR lpszValue)	{	m_strValue = lpszValue;	}
	inline void			SetCDATA(bool bValueCDATA)	{	m_bValueCDATA = bValueCDATA;	}

	bool IsCDATA() const	{	return m_bValueCDATA;	}

	// Remove child
	virtual bool		RemoveChild( XXMLElement* inChild, bool bForceDelete = false );
	virtual bool		DetachChild( XXMLElement* inChild );
	virtual bool		RemoveAttribute( XXMLAttribute* inAttrib, bool bForceDelete = false);

protected:
	XString		m_strName;		// Name of this element.
	XString		m_strValue;		// Value of this element.

	SInt32		m_nAttributeCount;
	SInt32		m_nChildCount;

	XXMLAttribute**	m_pAttributes;		// List of attributes.
	XXMLElement**	m_pChildren;		// List of child elements.

	bool		m_bOwnsChildren;	// True if we own our child elements.
	bool		m_bValueCDATA;
};


class DECLSPEC XXMLDoc :
	virtual public XObject
{
public:
					XXMLDoc();
	virtual			~XXMLDoc();

	XXMLElement*		GetRootElement()		{ return m_pRootElement; }
	const XXMLElement*	GetRootElement() const	{ return m_pRootElement; }

	void				SetRootElement( XXMLElement* inElement )
											{m_pRootElement = inElement; }

	// Build XML ByteStream from XMLDoc.
	void				BuildStream( CByteStream& inStream );

	// Build XMLDoc from XML ByteStream.
	void				ParseStream( const CByteStream& inStream );

	// Build XMLDoc from an XML file
	void				ParseFile( LPCTSTR szPathname );

protected:
	XXMLElement*	m_pRootElement;
	XString			m_strEncoding;
	XString			m_strVersion;
};
