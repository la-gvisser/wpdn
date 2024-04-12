// =============================================================================
//	TabHost.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_TAB
#ifdef HE_WIN32

#include "resource.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Tab Host
//
//	A Tab Host is the parent of all Tab objects created by the plugin. Tabs
//	subclass STATIC class windows.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CTabHost
// =============================================================================

class CTabHost
	:	public CStatic
{
protected:
	DECLARE_DYNAMIC(CTabHost)

protected:
	CWnd*		m_pChild;

public:
	;			CTabHost() : m_pChild( NULL ) {}
	virtual		~CTabHost() { delete m_pChild; m_pChild = NULL; }

	bool		HasChild() const { return (m_pChild != NULL); }

	bool		IsValid() const { return (GetSafeHwnd() != NULL); }

	void		SetChild( CWnd* inChild ) { m_pChild = inChild; }

	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
