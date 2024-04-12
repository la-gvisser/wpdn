// ============================================================================
//	PortTableParser.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002. All rights reserved.

#pragma once

#include "NameTableParser.h"

struct SIDNameMapEntry;

// =============================================================================
//	CPortTableParser
// =============================================================================

class CPortTableParser : public CNameSubTableParserBase
{
	public:
		CPortTableParser( CNameTableParser* pParserController );
		virtual ~CPortTableParser();
		virtual void Init();
		void ProcessChar( const wchar_t* pwchChars, int cchChars );
		void BeginElement( const wchar_t* pwchName, int cchChars, ISAXAttributes *pAttributes );
		void EndElement( const wchar_t* pwchName, int cchChars );

	protected:
		CStringW			m_strCurElemName;
		CNameTableParser*	m_pNameTableParser;
		SNameTableEntry		m_NewNTEntry;
		CStringW			m_strCurPort;
		TMediaSpecType		m_nCurMediaSpecType;
		CStringW			m_strCurGroup;
};
