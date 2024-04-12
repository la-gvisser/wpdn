// =============================================================================
//	TabHost.h
// =============================================================================
//	Copyright (c) 2010-2017 Savvius, Inc. All rights reserved.

#pragma once

#if defined(_WIN32) && defined(IMPLEMENT_TAB)

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
	;			CTabHost() : m_pChild( nullptr ) {}
	virtual		~CTabHost() { delete m_pChild; m_pChild = nullptr; }

	bool		HasChild() const { return (m_pChild != nullptr); }

	bool		IsValid() const { return (GetSafeHwnd() != nullptr); }

	void		SetChild( CWnd* inChild ) { m_pChild = inChild; }

	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
};

#endif // _WIN32 && IMPLEMENT_TAB
