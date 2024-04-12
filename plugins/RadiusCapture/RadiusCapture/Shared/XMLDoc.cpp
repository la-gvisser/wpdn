// =============================================================================
//	XMLDoc.cpp
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2005. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#include "stdafx.h"
#include "XMLBuilder.h"
#include "XMLDoc.h"
#include "XMLParser.h"

#ifndef _LINUX
#include "XMLParserFactory.h"
#endif

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

// =============================================================================
//	XXMLAttribute
// =============================================================================
// An attribute of an XML element.

// -----------------------------------------------------------------------------
//	XXMLAttribute												   [constructor]
// -----------------------------------------------------------------------------

XXMLAttribute::XXMLAttribute(
	const XString&	inName,
	const XString&	inValue,
	bool bCDATA /*=false*/)
		: m_strName( inName ),
		  m_strValue( inValue ),
		  m_bValueCDATA( bCDATA )
{
}

XXMLAttribute::XXMLAttribute()
{	
}


// =============================================================================
//	XXMLElement
// =============================================================================

// -----------------------------------------------------------------------------
//	XXMLElement													   [constructor]
// -----------------------------------------------------------------------------

XXMLElement::XXMLElement(
	const XString&	inString,
	bool			inOwnsChildren /* = true */)
		: m_nAttributeCount( 0 ),
		  m_nChildCount( 0 ),
		  m_pAttributes( NULL ),
		  m_pChildren( NULL ),
		  m_bOwnsChildren( inOwnsChildren ),
		  m_bValueCDATA( false )
{
	m_strName = inString;
	m_nAttributeCount = 0;
}

XXMLElement::XXMLElement()
	: m_nAttributeCount( 0 ),
	  m_nChildCount( 0 ),
	  m_pAttributes( NULL ),
	  m_pChildren( NULL ),
	  m_bOwnsChildren( true ),
	  m_bValueCDATA( false )
{
	m_nAttributeCount = 0;
}			

// -----------------------------------------------------------------------------
//	~XXMLElement													[destructor]
// -----------------------------------------------------------------------------

XXMLElement::~XXMLElement()
{
	// Destroy attributes.
	SInt32	nCount = CountAttributes();
	for( SInt32 i = 0; i < nCount; i++ )
	{
		XXMLAttribute*	pDoomedAttribute = m_pAttributes[i];
		delete pDoomedAttribute;
	}

	free(m_pAttributes);	// TODO: may be allocated by new[]!

	if( m_bOwnsChildren )
	{
		// Destroy children.
		nCount = CountChildren();
		for( SInt32 j = 0; j < nCount; j++ )
		{
			XXMLElement* pDoomedElement = m_pChildren[j];
			delete pDoomedElement;
		}
	}

	free(m_pChildren);	// TODO: may be allocated by new[]!
}	


// -----------------------------------------------------------------------------
//	AddValue( const XString& )
// -----------------------------------------------------------------------------

void
XXMLElement::AddValue( 
	const XString& inText,
	bool bCDATA /*=false*/)
{
	m_strValue += inText;
	m_bValueCDATA = bCDATA;
}


// -----------------------------------------------------------------------------
//	AddValue( const TCHAR )
// -----------------------------------------------------------------------------

void
XXMLElement::AddValue(
	const TCHAR	inChar )
{
	m_strValue += inChar;
}


// -----------------------------------------------------------------------------
//	GetChild
// -----------------------------------------------------------------------------

const XXMLElement*
XXMLElement::GetChild( 
	SInt32	inIndex ) const
{
	// Get nth element.
	if( inIndex >= 0 && inIndex < CountChildren() )
	{
		return m_pChildren[inIndex];
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetChild
// -----------------------------------------------------------------------------

XXMLElement*
XXMLElement::GetChild( 
	SInt32	inIndex )
{
	// Get nth element.
	if( inIndex >= 0 && inIndex < CountChildren() )
	{
		return m_pChildren[inIndex];
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetNamedChild
// -----------------------------------------------------------------------------
// Return nth named child.

const XXMLElement*
XXMLElement::GetNamedChild( 
	const TCHAR*	inChildName,
	SInt32			inIndex ) const
{
	ASSERT( inChildName != NULL );
	if( inChildName == NULL ) return NULL;
	
	SInt32	nOccurrence = 0;
	SInt32	nChildren = CountChildren();
	for( SInt32 x = 0; x < nChildren; x++ )
	{
		const XXMLElement*	pChild = GetChild( x );
		if( pChild->GetName() == inChildName )
		{
			// Found one!
			if( nOccurrence == inIndex )
			{
				// This is at the right index.
				return pChild;
			}
			else
			{
				nOccurrence++;
			}
		}
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetNamedChild
// -----------------------------------------------------------------------------
// Return nth named child.
XXMLElement*
XXMLElement::GetNamedChild( 
	const TCHAR*	inChildName,
	SInt32			inIndex )
{
	ASSERT( inChildName != NULL );
	if( inChildName == NULL ) return NULL;
	
	SInt32	nOccurrence = 0;
	SInt32	nChildren = CountChildren();
	for( SInt32 x = 0; x < nChildren; x++ )
	{
		XXMLElement*	pChild = GetChild( x );
		if( pChild->GetName() == inChildName )
		{
			// Found one!
			if( nOccurrence == inIndex )
			{
				// This is at the right index.
				return pChild;
			}
			else
			{
				nOccurrence++;
			}
		}
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetNamedChildCount
// -----------------------------------------------------------------------------
// Returns the # of named children

SInt32 XXMLElement::GetNamedChildCount( 
	const TCHAR* inChildName ) const
{
	ASSERT( inChildName != NULL );
	if( inChildName == NULL ) return 0;
	
	SInt32	nOccurrence = 0;
	SInt32	nChildren = CountChildren();

	for( SInt32 x = 0; x < nChildren; x++ )
	{
		const XXMLElement*	pChild = GetChild( x );
		if( pChild->GetName() == inChildName )
		{
			// Found one!
			nOccurrence++;
		}
	}
	
	return nOccurrence;
}



// -----------------------------------------------------------------------------
//	GetNamedChildValue
// -----------------------------------------------------------------------------
// Return nth named child's value.

bool
XXMLElement::GetNamedChildValue( 
	const TCHAR*	inChildName,
	XString&			outValue,
	SInt32			inIndex ) const
{
	ASSERT( inChildName != NULL );
	if( inChildName == NULL ) return false;
	
	const XXMLElement*	pElement = GetNamedChild( inChildName, inIndex );
	if( pElement != NULL )
	{
		outValue = pElement->GetValue();
		return true;
	}
	
	return false;
}


// -----------------------------------------------------------------------------
//	GetAttribute
// -----------------------------------------------------------------------------

const XXMLAttribute*
XXMLElement::GetAttribute( 
	SInt32	inIndex ) const
{
	// Get nth attribute.
	if( inIndex >= 0 && inIndex < CountAttributes() )
	{
		return m_pAttributes[inIndex];
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetAttribute
// -----------------------------------------------------------------------------

XXMLAttribute*
XXMLElement::GetAttribute( 
	SInt32	inIndex ) 
{
	// Get nth attribute.
	if( inIndex >= 0 && inIndex < CountAttributes() )
	{
		return m_pAttributes[inIndex];
	}
	
	return NULL;
}


// -----------------------------------------------------------------------------
//	GetNamedAttribute
// -----------------------------------------------------------------------------

const XXMLAttribute*
XXMLElement::GetNamedAttribute( 
	const TCHAR* inAttributeName ) const
{
	ASSERT( inAttributeName != NULL );
	if( inAttributeName == NULL ) return NULL;
	
	SInt32	nAttributes = CountAttributes();
	for( SInt32 x = 0; x < nAttributes; x++ )
	{
		const XXMLAttribute*	pAttribute = GetAttribute( x );
		if( pAttribute->GetName() == inAttributeName )
		{
			return pAttribute;
		}
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetNamedAttribute
// -----------------------------------------------------------------------------

XXMLAttribute*
XXMLElement::GetNamedAttribute( 
	const TCHAR* inAttributeName )
{
	ASSERT( inAttributeName != NULL );
	if( inAttributeName == NULL ) return NULL;
	
	SInt32	nAttributes = CountAttributes();
	for( SInt32 x = 0; x < nAttributes; x++ )
	{
		XXMLAttribute*	pAttribute = GetAttribute( x );
		if( pAttribute->GetName() == inAttributeName )
		{
			return pAttribute;
		}
	}
	
	return NULL;
}

// -----------------------------------------------------------------------------
//	GetNamedAttributeValue
// -----------------------------------------------------------------------------

bool
XXMLElement::GetNamedAttributeValue( 
	const TCHAR*	inAttributeName,
	XString&		outValue ) const
{
	ASSERT( inAttributeName != NULL );
	if( inAttributeName == NULL ) return false;
	
	const XXMLAttribute* pAttribute = GetNamedAttribute( inAttributeName );
	if( pAttribute != NULL )
	{
		outValue = pAttribute->GetValue();
		return true;
	}
	
	return false;
}


// -----------------------------------------------------------------------------
//	AddChild( XXMLElement* )
// -----------------------------------------------------------------------------

bool
XXMLElement::AddChild( 
	XXMLElement*	inChild )
{
	ASSERT( inChild != NULL );
	if( inChild == NULL ) return false;

	// TODO: m_pChildren may have been allocated by new[]!
	m_pChildren = (XXMLElement**) realloc(m_pChildren, sizeof(XXMLElement*)*(++m_nChildCount));
	m_pChildren[m_nChildCount - 1] = inChild;
	
	return true;
}


// -----------------------------------------------------------------------------
//	AddChild( const TCHAR*, const TCHAR* )
// -----------------------------------------------------------------------------

bool
XXMLElement::AddChild( 
	const TCHAR*	inElementName,
	const TCHAR*	inElementValue )
{
	ASSERT( inElementName != NULL );
	ASSERT( inElementValue != NULL );
	if( (inElementName == NULL) || (inElementValue == NULL) ) return false;

	XXMLElement*	pNewElement = new XXMLElement( inElementName );
	ASSERT( pNewElement != NULL );
	if( pNewElement == NULL ) return false;
	
	pNewElement->AddValue( inElementValue );
	
	return AddChild( pNewElement );
}

// -----------------------------------------------------------------------------
//	AddChild( const TCHAR*, const TCHAR* )
// -----------------------------------------------------------------------------

bool
XXMLElement::ReplaceChild( 
	XXMLElement* inCurChild, 
	XXMLElement* inNewChild )
{
	// find inCurChild, replace with inNewChild
	// delete inCurChild if we are the owner
	for (SInt32 nChild = 0; nChild < m_nChildCount; nChild++)
	{
		if (inCurChild == m_pChildren[nChild])
		{
			if (m_bOwnsChildren)
				delete inCurChild;
			m_pChildren[nChild] = inNewChild;
			return true;
		}
	}

	return false;
}

// -----------------------------------------------------------------------------
//	AddAttribute( XXMLAttribute* )
// -----------------------------------------------------------------------------

bool
XXMLElement::AddAttribute(
	XXMLAttribute*	inAttribute )
{
	ASSERT( inAttribute != NULL );
	if( inAttribute == NULL ) return false;

	m_pAttributes = (XXMLAttribute**) realloc(m_pAttributes, (++m_nAttributeCount)*sizeof(XXMLAttribute*));
	m_pAttributes[m_nAttributeCount - 1] = inAttribute;

	return true;
}


// -----------------------------------------------------------------------------
//	AddAttribute( const TCHAR*, const TCHAR* )
// -----------------------------------------------------------------------------

bool
XXMLElement::AddAttribute(
	const TCHAR*	inAttributeName,
	const TCHAR*	inAttributeValue,
	bool bIsCDATA /* = false */ )
{
	ASSERT( inAttributeName != NULL );
	ASSERT( inAttributeValue != NULL );
	if( (inAttributeName == NULL) || (inAttributeValue == NULL) ) return false;

	XXMLAttribute* pNewAttribute = new XXMLAttribute( inAttributeName, inAttributeValue, bIsCDATA );

	if( pNewAttribute == NULL ) return false;

	return AddAttribute( pNewAttribute );
}


// -----------------------------------------------------------------------------
//	AddAttribute( const TCHAR*, UInt32 )
// -----------------------------------------------------------------------------

bool		
XXMLElement::AddAttribute( const TCHAR* inAttributeName, UInt32 inAttributeValue )
{
	ASSERT( inAttributeName != NULL );
	if( (inAttributeName == NULL) ) return false;
	XString str;
	str.Format(_T("%u"), inAttributeValue);
	XXMLAttribute* pNewAttribute = new XXMLAttribute( inAttributeName, str );

	if( pNewAttribute == NULL ) return false;

	return AddAttribute( pNewAttribute );
}


// -----------------------------------------------------------------------------
//	AddAttribute( const TCHAR*, SInt32 )
// -----------------------------------------------------------------------------

bool		
XXMLElement::AddAttribute( const TCHAR* inAttributeName, SInt32 inAttributeValue )
{
	ASSERT( inAttributeName != NULL );
	if( (inAttributeName == NULL) ) return false;
	XString str;
	str.Format(_T("%d"), inAttributeValue);
	XXMLAttribute* pNewAttribute = new XXMLAttribute( inAttributeName, str );

	if( pNewAttribute == NULL ) return false;

	return AddAttribute( pNewAttribute );
}

// =============================================================================
//	XXMLDoc
// =============================================================================

// -----------------------------------------------------------------------------
//	XXMLDoc														   [constructor]
// -----------------------------------------------------------------------------

XXMLDoc::XXMLDoc()
	: m_pRootElement( NULL )
{
}


// -----------------------------------------------------------------------------
//	~XXMLDoc														[destructor]
// -----------------------------------------------------------------------------

XXMLDoc::~XXMLDoc()
{
	delete m_pRootElement;
}


// -----------------------------------------------------------------------------
//	BuildStream
// -----------------------------------------------------------------------------
// Build an XMLDoc into a ByteStream.

void
XXMLDoc::BuildStream( 
	CByteStream& inStream )
{
	XXMLBuilder	theBuilder;
	try
	{
		theBuilder.DoBuild( m_pRootElement, &inStream );
	}
	catch( XParseException /*e*/ )
	{
		// +++ Catch error here!
	}	
}


// -----------------------------------------------------------------------------
//	ParseStream
// -----------------------------------------------------------------------------
// Parse a ByteStream into an XMLDoc.

void
XXMLDoc::ParseStream( 
	const CByteStream& inStream )
{
#if TARGET_OS_WIN32
	// make a VARIANT that points the the data in inStream and pass to the parser
	CXMLParser*	theParser = CXMLParser::NewInstance();
	ASSERT( theParser != NULL );
	if (theParser == NULL )
	{
		return;
	}

	// Parse the document
	m_pRootElement = theParser->DoParse( inStream );
#else
	// TODO: Replace the parse with something that's UNICODE compatible
//	ASSERT(0);
	XXMLParser	theParser;
	try
	{
		m_pRootElement = theParser.DoParse( inStream );
	}
	catch( XParseException /*e*/ )
	{
		// +++ Catch error here!
	}
	
	if( m_pRootElement != NULL )
	{
		m_strVersion	= theParser.GetXMLVersion();
		m_strEncoding	= theParser.GetEncoding();
	}
#endif
}


// -----------------------------------------------------------------------------
//	ParseFile
// -----------------------------------------------------------------------------
// Parse a ByteStream into an XMLDoc.

void
XXMLDoc::ParseFile( LPCTSTR szPathname )
{
#if TARGET_OS_WIN32
	CXMLParser*	theParser = CXMLParser::NewInstance();
	ASSERT( theParser != NULL );
	if (theParser == NULL )
	{
		return;
	}

	// Parse the document
	m_pRootElement = theParser->DoParse( szPathname );
#else
	XXMLParser theParser;
	CByteStream theStream;
	theStream.ReadFromFile( szPathname );

	// Parse the document
	m_pRootElement = theParser.DoParse( theStream );
#endif
}

int XXMLElement::Reset()
{   // Destroy attributes.
	SInt32	nCount = CountAttributes();
	// not the first one, we own that
	for( SInt32 i = 0; i < nCount; i++ )
	{
		XXMLAttribute*	pDoomedAttribute = m_pAttributes[i];
		delete pDoomedAttribute;
	}

	// free(m_pAttributes);	// don't free the storage just yet
	
	if( m_bOwnsChildren )
	{
		// Destroy children.
		nCount = CountChildren();
		for( SInt32 j = 0; j < nCount; j++ )
		{
			XXMLElement* pDoomedElement = m_pChildren[j];
			delete pDoomedElement;
		}
	}

	// TODO: may be allocated by new[]!
	free(m_pChildren);

	// m_pAttributes = NULL;
	m_pChildren = NULL;
	m_nChildCount =  0;
	m_nAttributeCount = 0;
	m_strValue.Empty();
	m_strName.Empty();

	return X_OK;
}

bool XXMLElement::RemoveChild( XXMLElement* inChild, bool bForceDelete /* = false */)
{	// find the element
	for (SInt32 n = 0; n < CountChildren(); n++)
		if (m_pChildren[n] == inChild)
		{	// realloc the array
			// TODO: other parts of this code use malloc/realloc/free for m_pChildren!!!
			XXMLElement** ayTemp = new XXMLElement*[CountChildren() - 1];
			memcpy(ayTemp, m_pChildren, n*sizeof(XXMLElement*));
			memcpy(&ayTemp[n], &m_pChildren[n + 1], (CountChildren() - 1 - n)*sizeof(XXMLElement*));
			delete m_pChildren;	// TODO: delete [] or free!?
			m_pChildren = ayTemp;
			m_nChildCount--;
			if (bForceDelete)	// || m_bOwnsChildren)
				delete inChild;
			return true;
		}

	return false;
}

bool XXMLElement::DetachChild( XXMLElement* inChild )
{	// find the element
	for (SInt32 n = 0; n < CountChildren(); n++)
		if (m_pChildren[n] == inChild)
		{	// realloc the array
			// TODO: other parts of this code use malloc/realloc/free for m_pChildren!!!
			XXMLElement** ayTemp = new XXMLElement*[CountChildren() - 1];
			memcpy(ayTemp, m_pChildren, n*sizeof(XXMLElement*));
			memcpy(&ayTemp[n], &m_pChildren[n + 1], (CountChildren() - 1 - n)*sizeof(XXMLElement*));
			delete m_pChildren;	// TODO: delete [] or free!?
			m_pChildren = ayTemp;
			m_nChildCount--;
			return true;
		}

	return false;
}

bool XXMLElement::RemoveAttribute( XXMLAttribute* inAttrib, bool bForceDelete /* = false */)
{	// find the element
	for (SInt32 n = 0; n < CountAttributes(); n++)
		if (m_pAttributes[n] == inAttrib)
		{	// realloc the array
			// TODO: other parts of this code use malloc/realloc/free for m_pAttributes!!!
			XXMLAttribute** ayTemp = new XXMLAttribute*[CountAttributes() - 1];
			memcpy(ayTemp, m_pAttributes, n*sizeof(XXMLAttribute*));
			memcpy(&ayTemp[n], &m_pAttributes[n + 1], (CountAttributes() - 1 - n)*sizeof(XXMLAttribute*));
			delete m_pAttributes;	// TODO: delete [] or free!?
			m_pAttributes = ayTemp;
			m_nAttributeCount--;
			if (bForceDelete) // || m_bOwnsChildren)
				delete inAttrib;
			return true;
		}

	return false;
}
