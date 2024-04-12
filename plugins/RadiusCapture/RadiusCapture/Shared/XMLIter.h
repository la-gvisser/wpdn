// ============================================================================
//	XMLIter.h
// ============================================================================
//	Copyright (c) Wildpackets, Inc. 2001-2004. All rights reserved.

#ifndef WILDPACKETS_XMLITER_H
#define WILDPACKETS_XMLITER_H

#include "XMLParser.h"

// here's how you use this:
// if you've got some idea of the structure, this thing is fairly efficient.
// for instance, if you know you've got 3 levels in your file, the root, a 
// bunch of children, and each child has a few sub elements, an efficient
// way to process this would be:
// 
//	XXMLElement			root, level1, level2;
//	XXMLParseIterator	iter;
//
//	VERIFY(iter.SetDataSource(inStream);
//	VERIFY(iter.InitParser());
//
//	VERIFY(iter.NextChild(root, NULL));	// read in root item and attribs
//	
//	// loop through all level1 children
//	while(iter.NextChild(level1, &root))
//	{
//		// now read in children
//		while (iter.NextRoot(level2, &level1))
//		{
//			// lowest level of processing
//			...
//			level2.Reset();
//		}
//
//		// read in the end tag and move on to the next level1 item
//		VERIFY(iter.EndElement(&level1));
//		level1.Reset();
//	}
//
//	VERIFY(iter.EndElement(&root));	
//
// Even if the file had more sublevels of elements, NextRoot would read them 
// in, but it would allocate the things below level 2 as children of level2,
// and would be less efficient
//
class XXMLParseIterator : public XXMLParser
{
public:
	XXMLParseIterator();
	virtual ~XXMLParseIterator();

	virtual bool InitParser();
	virtual bool NextChild(XXMLElement& element, XXMLElement* pParent);
	virtual bool NextRoot(XXMLElement& element, XXMLElement* pParent);
	virtual bool EndElement(XXMLElement* pElement);

	// in case clients want to know how much of the file we've gone through so far
	UInt32 GetPosition() const { return (UInt32)(XParser::m_szSourceCurrent - XParser::m_szSource); }

protected:
	bool	ParseTagBeginning(XXMLElement& element, XXMLElement* pParent);
	bool	ParseElement(XXMLElement& element, XXMLElement* pParent);
	bool	ParseContentETag(XXMLElement& element, XXMLElement* pParent);
	bool	ParseMarkup(XXMLElement& inElement, XXMLElement* pParent);

	bool	m_bEmptyTag;	// used when we need to monitor the parse state
};

#endif // WILDPACKETS_XMLITER_H
