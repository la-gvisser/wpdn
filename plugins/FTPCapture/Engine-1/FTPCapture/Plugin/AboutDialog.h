// ============================================================================
//	AboutDialog.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include <afxwin.h>
#include "resource.h"


// ============================================================================
//		CAboutDialog
// ============================================================================

class CAboutDialog
	:	public CDialog
{
public:
	enum { IDD = IDD_PLUGIN_ABOUT };

protected:
	HINSTANCE	m_hInstance;

	DECLARE_MESSAGE_MAP()

	void	DoDataExchange( CDataExchange* pDX );

public:
	;		CAboutDialog( HINSTANCE inInstance, CWnd* inParent = NULL );
	;		~CAboutDialog();

	BOOL	OnInitDialog();
};

#endif // HE_WIN32
