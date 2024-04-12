// ============================================================================
//	AboutDialog.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once
 
#include "resource.h"
#include <atlstr.h>
#include <atlwin.h>


// ============================================================================
//		CAboutDialog
// ============================================================================

class CAboutDialog :
	public CDialogImpl<CAboutDialog>
{
private:
	HFONT	m_hBoldFont;

public:
	enum { IDD = IDD_PLUGIN_ABOUT };

	BEGIN_MSG_MAP(CAboutDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnClose)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
	END_MSG_MAP()

	CAboutDialog()
		:	m_hBoldFont( NULL )
	{
	}

	~CAboutDialog()
	{
		if ( m_hBoldFont != NULL ) {
			::DeleteObject( m_hBoldFont );
		}
	}

	LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		// Get the name caption.
		CWindow	wndName = GetDlgItem( IDC_PLUGIN_ABOUT_NAME );
		if ( wndName.IsWindow() ) {
			// Get the current name caption font.
			HFONT	hFont = wndName.GetFont();
			if ( hFont != NULL ) {
				// Create a bold font based on the current name caption font.
				LOGFONT	lf;
				::GetObject( hFont, sizeof(lf), &lf );
				lf.lfWeight = FW_BOLD;
				m_hBoldFont = ::CreateFontIndirect( &lf );
				if ( m_hBoldFont != NULL ) {
					// Set the name caption font.
					wndName.SetFont( m_hBoldFont );
				}
			}
		}

		CenterWindow();

		return TRUE;
	}

	LRESULT OnClose( UInt16 /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
	{
		::EndDialog( m_hWnd, wID );
		return 0;
	}
};
