// ============================================================================
// ParentStatic.h:
//      interface for the CParentStatic class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once


// ============================================================================
//		CParentStatic
// ============================================================================

class CParentStatic
	:	public CStatic
{
	DECLARE_DYNAMIC(CParentStatic)

protected:
	CWnd	*m_pTabWnd;

	DECLARE_MESSAGE_MAP()

public:

	;			CParentStatic();
	virtual		~CParentStatic();

	void	SetTab( CWnd* inChild ) { m_pTabWnd = inChild; }

	afx_msg void	OnSize( UINT nType, int cx, int cy );
};
