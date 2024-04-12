// =============================================================================
//	Options.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

class CXmlElement;

// =============================================================================
//		COptions
// =============================================================================

class COptions
{
protected:
	UInt32		m_nIP;

public:
	;			COptions() : m_nIP( INADDR_ANY ) {}
	virtual		~COptions() {}

	UInt32		GetIP() const { return m_nIP; }

	bool		HasIP() const { return (m_nIP != INADDR_ANY); }

	int			Load( CXmlElement& xmlElement );
	int			Load(IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode);

	void		Reset() { m_nIP = INADDR_ANY; }

	void		SetIP( UInt32 nValue ) { m_nIP = nValue; }
	int			Store( CXmlElement& xmlElement );
	int			Store(IHeUnknown* pXmlDoc, IHeUnknown* pXmlNode);
};
