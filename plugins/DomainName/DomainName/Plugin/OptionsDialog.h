// =============================================================================
//	OptionsDialog.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef PEEK_UI

#include "resource.h"
#include "Options.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Options Dialog
//
//	An Options Dialog is the dialog box that displays Options. The Framework
//	example code uses an Options View to display the individual members of the
//	Options object.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		COptionsDialog
// =============================================================================

class COptionsDialog
	:	public CDialog
{
public:
	enum { IDD = IDD_PLUGIN_OPTIONS };

protected:
	COptions		m_Options;
	bool			m_bGlobal;
	HINSTANCE		m_hInstance;

	void			DoDataExchange( CDataExchange* pDX );

	void			ControlToList();
	void			ListToControl();
	virtual BOOL	OnInitDialog();

public:
	;				COptionsDialog( const COptions& inOptions, HINSTANCE inInstance, CWnd* inParent = NULL );
	virtual			~COptionsDialog() {}

	const COptions&	GetOptions() { return m_Options; }

	DECLARE_MESSAGE_MAP()

	void			SetOptions( COptions& inOptions ) { m_Options = inOptions; }

	afx_msg void	OnBnClickedAddName();
	afx_msg void	OnBnClickedDeleteName();
	afx_msg void	OnLbnSelchangeNameList();

};

#endif // HE_WIN32
