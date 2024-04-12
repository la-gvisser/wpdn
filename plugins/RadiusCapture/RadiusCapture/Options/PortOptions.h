// ============================================================================
//	PortOptions.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PortNumber.h"
#include "PeekDataModeler.h"

#define kPref_Port				L"Port"
#define kPref_Ports				L"Ports"
#define	kPref_UseDefaultPorts	L"UseDefaultPorts"

// =============================================================================
//		CPortOptions
// =============================================================================

class CPortOptions
{

public:

	CPortOptions() {
		Reset();
	}
	~CPortOptions() {}

	CPortOptions& operator=( const CPortOptions& in ) {
		m_bUseDefaultPorts = in.m_bUseDefaultPorts;
		m_PortNumberList = in.m_PortNumberList;
		return *this;
	}
	bool operator==( const CPortOptions& in ) {
		if ( m_bUseDefaultPorts != in.m_bUseDefaultPorts ) return false;
		if ( m_PortNumberList != in.m_PortNumberList ) return false;
		return true;
	}
	bool operator!=( const CPortOptions& inOptions ) {
		return !operator==(inOptions);
	}

	void			InitializePortNumberDefaults() {
		m_PortNumberList.Initialize();
	}

	CPortNumberList& GetPortNumberList() {
		return m_PortNumberList;
	}
	bool	IsUsingGlobalDefaults() {
		return m_bUseDefaultPorts;
	}

	bool	Model( CPeekDataElement& ioElement, bool bIsCase );

	void	Reset() {
		m_bUseDefaultPorts = true;
		InitializePortNumberDefaults();
	}

	void	SetGlobalDefaultsFlag( bool bUsingGlobalDefaults ) {
		m_bUseDefaultPorts = bUsingGlobalDefaults;
	}
	void	SyncGlobalDefaults( CPortNumberList& inGlobalPortNumberList ) {
		m_bUseDefaultPorts = true;
		m_PortNumberList = inGlobalPortNumberList;
	}
	CPeekString GetFormattedActivePortsString() {
		return m_PortNumberList.GetFormattedActivePortsString();
	}
	bool AreEnabledPorts() {
		return m_PortNumberList.AreEnabledPorts();
	}

protected:
	CPortNumberList	m_PortNumberList;
	bool			m_bUseDefaultPorts;

};
