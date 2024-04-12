// ============================================================================
// ParentStatic.cpp:
//      implementation of the CParentStatic class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "ParentStatic.h"


// ============================================================================
//		CParentStatic
// ============================================================================

IMPLEMENT_DYNAMIC(CParentStatic, CStatic)

CParentStatic::CParentStatic()
	:	m_pTabWnd( NULL )
{
}

CParentStatic::~CParentStatic()
{
}

// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CParentStatic, CStatic)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
CParentStatic::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CStatic::OnSize( nType, cx, cy );

	if ( m_pTabWnd ) {
		m_pTabWnd->MoveWindow (0, 0, cx, cy );
	}
}
