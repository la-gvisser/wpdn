// ============================================================================
//	IpPenRegister.h
//		interface for the CIpPenRegisterApp class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "IpPenRegisterPlugin.h"

// ============================================================================
//		CIpPenRegisterApp
// ============================================================================

class CIpPenRegisterApp
	:	public CWinApp
{
protected:
	DECLARE_MESSAGE_MAP()

public:
	CIpPenRegisterPlugin	m_Plugin;

	;		CIpPenRegisterApp();

	CIpPenRegisterPlugin&		GetPlugin() { return m_Plugin; };

	virtual BOOL	InitInstance();
};
