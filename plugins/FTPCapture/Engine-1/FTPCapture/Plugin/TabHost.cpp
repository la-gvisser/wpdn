// =============================================================================
//	TabHost.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef IMPLEMENT_TAB
#ifdef HE_WIN32

#include "TabHost.h"


// =============================================================================
//		CTabHost
// =============================================================================

IMPLEMENT_DYNAMIC(CTabHost, CStatic)


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CTabHost, CStatic )
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnShowWindow
// ----------------------------------------------------------------------------

void
CTabHost::OnShowWindow(
	BOOL	bShow,
	UINT	nStatus )
{
	CStatic::OnShowWindow( bShow, nStatus );
	if ( m_pChild != NULL ) {
		m_pChild->ShowWindow( (bShow) ? SW_SHOW : SW_HIDE );
	}
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
CTabHost::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CStatic::OnSize( nType, cx, cy );

	if ( m_pChild ) {
		m_pChild->MoveWindow( 0, 0, cx, cy );
	}
}

#endif // HE_WIN32
#endif // IMPLEMENT_TAB
