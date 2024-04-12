// =============================================================================
//	OptionsDialog.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "OptionsDialog.h"
#include <atlstr.h>

  
#ifdef HE_WIN32
 
// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

LRESULT
COptionsDialog::OnInitDialog(
	UINT	/*uMsg*/,
	WPARAM	/*wParam*/,
	LPARAM	/*lParam*/,
	BOOL&	/*bHandled*/)
{
	CenterWindow();

	CComBSTR	bstrIP;

	if( m_Options.HasIP() ) {
		UInt32 nIPAddress = m_Options.GetIP();
		struct in_addr	ipAddress;
		ipAddress.S_un.S_addr = nIPAddress;
		bstrIP = inet_ntoa( ipAddress );
	}
	SetDlgItemText( IDC_EDIT_IP, bstrIP );

	return TRUE;
}


// -----------------------------------------------------------------------------
//		OnOK
// -----------------------------------------------------------------------------

LRESULT
COptionsDialog::OnOK(
	UInt16	/*wNotifyCode*/,
	UInt16	wID,
	HWND	/*hWndCtl*/,
	BOOL&	/*bHandled*/ )
{
	CString	strIP;
	GetDlgItemText( IDC_EDIT_IP, strIP );
	if ( strIP.IsEmpty() ) {
		m_Options.Reset();
	}
	else {
		m_Options.SetIP( inet_addr( CStringA( strIP ) ) );
	}

	EndDialog( wID );

	return 0;
}


// -----------------------------------------------------------------------------
//		OnCancel
// -----------------------------------------------------------------------------

LRESULT
COptionsDialog::OnCancel(
	UInt16	/*wNotifyCode*/,
	UInt16	wID,
	HWND	/*hWndCtl*/,
	BOOL&	/*bHandled*/ )
{
	EndDialog( wID );

	return 0;
}

#endif /* HE_WIN32 */
