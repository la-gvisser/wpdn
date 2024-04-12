// ============================================================================
// TabParent.h:
//      interface for the CTabParent class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once


// =============================================================================
//		CTabParent
// =============================================================================

class CTabParent
	:	public CStatic
{
	DECLARE_DYNAMIC(CTabParent)
	DECLARE_MESSAGE_MAP()

protected:
	CWnd*	m_pChild;

public:
	;			CTabParent() : m_pChild( NULL ) {}
	virtual		~CTabParent() {}

	void		SetChild( CWnd* inChild ) { m_pChild = inChild; }

	afx_msg void OnSize( UINT nType, int cx, int cy );
};
