// =============================================================================
//	OptionsDialog.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "Options.h"
#include "resource.h"
#include <atlbase.h>
#include <atlwin.h>


// =============================================================================
//		COptionsDialog
// =============================================================================

class COptionsDialog :
	public CDialogImpl<COptionsDialog>
{
public:
	enum { IDD = IDD_PLUGIN_OPTIONS };

protected:
	bool	IsButtonChecked( UINT nId ) {
		return (IsDlgButtonChecked( nId ) == BST_CHECKED);
	}

protected:
	COptions	m_Options;

public:
	;		COptionsDialog( const COptions& inOptions ) : m_Options( inOptions ) {}

	const COptions&	GetOptions() const { return m_Options; }

	BEGIN_MSG_MAP(COptionsDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnOK( UInt16 wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
	LRESULT OnCancel( UInt16 wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
};
