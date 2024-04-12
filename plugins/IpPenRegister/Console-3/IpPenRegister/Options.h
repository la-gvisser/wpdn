// ============================================================================
//	Options.h
//		interface for the COptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekPlugin.h"
#include "PeekXml.h"
#include "Utils.h"


// ============================================================================
//		COptions
// ============================================================================

class COptions
{
    friend class CGeneralOpts;

public:
	class COptionsGeneral
	{
	public:
	;			COptionsGeneral()
				{}

		void	CheckDefaults() {};
		void	Read( CPeekXmlElement& inElement );
		void	Write( CPeekXmlElement& inElement );
	};
protected:
	CPeekPlugin*	m_pPlugin;
	//CString			m_strConfigFileName;
	//CString			m_strChangeLogFileName;
	//CString			m_strPrefs;
	COptionsGeneral	m_General;

	void	CheckDefaults() {
				m_General.CheckDefaults();
			}
	void	LoadPrefs( CPeekXmlDocument& inXmlDoc );
	void	StorePrefs( CPeekXmlDocument& inXmlDoc );

public:
	;		COptions() : m_pPlugin( NULL ) {}
	;		~COptions() {}

	void	operator=( const COptions& in ) {
				m_pPlugin = in.m_pPlugin;
				m_General = in.m_General;
			}
	void	Duplicate( COptions& in ) {
				in = *this;
			}

	void	Init( CPeekPlugin* inPlugin );

	void	SetOptions( COptions& in ) {
				*this = in;		// ToDo: Log the changes.
				Write();
			}

	void	Read();
	void	Write();
};
