// ============================================================================
//	CMIFilter.h
//		interface for the CCMIFilterApp class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CMIFilterPlugin.h"


// ============================================================================
//		CCMIFilterApp
// ============================================================================

class CCMIFilterApp
	:	public CWinApp
{
protected:
	DECLARE_MESSAGE_MAP()

public:
	CCMIFilterPlugin	m_Plugin;

	;					CCMIFilterApp();
	CCMIFilterPlugin&	GetPlugin() { return m_Plugin; };
	virtual BOOL		InitInstance();
};
