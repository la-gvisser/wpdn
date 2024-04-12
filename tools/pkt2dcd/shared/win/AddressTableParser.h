// ============================================================================
//	AddressTableParser.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2002. All rights reserved.

#pragma once

#include "NameTableParser.h"

// =============================================================================
//	CAddressTableParser
// =============================================================================

class CAddressTableParser : public CNameSubTableParserBase
{
	public:
		CAddressTableParser( CNameTableParser* pMainParser );
		virtual ~CAddressTableParser();
		virtual void Init();
		void ProcessChar( const wchar_t* pwchChars, int cchChars );
		void BeginElement( const wchar_t* pwchName, int cchChars, ISAXAttributes *pAttributes );
		void EndElement( const wchar_t* pwchName, int cchChars );

	protected:
		CStringW				m_strCurElemName;
		SNameTableEntry			m_NewNTEntry;
		CStringW				m_strCurAddress;
		TMediaSpecType			m_nCurMediaSpecType;
		CStringW				m_strCurGroup;
		CStringW				m_strCurTrust;
		
		CNameTableParser*		m_pNameTableParser;
};
