// =============================================================================
//	XMLBuilder.h
// =============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.

#ifndef XMLBUILDER_H
#define XMLBUILDER_H

#include "XMLDoc.h"

class DECLSPEC XXMLBuilder :
	virtual public XObject
{
public:
				XXMLBuilder();
	virtual		~XXMLBuilder();
			
	// Throw XXMLException?
	bool		DoBuild( XXMLElement* inElement, CByteStream* inStream );
	
protected:
	virtual bool		BuildDocument();
	virtual bool		BuildXMLDecl();	
	virtual bool		BuildString( const char* inString );
	virtual bool		BuildString( const wchar_t* inString );
	virtual bool		BuildAttribString( const XString& inString );
	virtual bool		BuildCDATAString(const XString& inString );
	virtual bool		BuildElement( const XXMLElement* inElement );
	virtual bool		BuildContentETag( const XXMLElement* inElement );
	virtual bool		BuildTagBeginning( const XXMLElement* inElement );
	virtual bool		BuildTagEnd();
	virtual bool		BuildEmptyTagEnd();
	virtual bool		BuildAttributes( const XXMLElement* inElement );
	virtual bool		BuildAttribute( const XXMLAttribute* inAttribute );
	virtual void		BuildEq();
	virtual void		BuildQuote();
	virtual void		BuildSpace();
	virtual void		BuildTab();
	virtual void		BuildEndline();
	virtual void		BuildChar( char inChar );

	CByteStream*	m_Stream;
	XXMLElement*	m_pRootElement;
	static SInt32	s_nDepth;
};

#endif /* XMLBUILDER_H */
