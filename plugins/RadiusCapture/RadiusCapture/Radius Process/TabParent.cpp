// ============================================================================
// TabParent.cpp:
//      implementation of the CTabParent class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "stdafx.h"
#include "TabParent.h"

#if defined(_DEBUG) && !defined(__GNUC__)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// =============================================================================
//		CTabParent
// =============================================================================

IMPLEMENT_DYNAMIC(CTabParent, CStatic)


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CTabParent, CStatic)
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	ON_WM_CHILDACTIVATE()
END_MESSAGE_MAP()	


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
CTabParent::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CStatic::OnSize( nType, cx, cy );

	if ( m_pChild != NULL ) {
		m_pChild->MoveWindow( 0, 0, cx, cy );
	}
}
