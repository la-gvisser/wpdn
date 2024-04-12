// ============================================================================
//	AboutDialog.h
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef _WIN32

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
	;		CAboutDialog( HINSTANCE inInstance, CWnd* inParent = nullptr );
	;		~CAboutDialog();

	BOOL	OnInitDialog();
};

#endif // _WIN32
