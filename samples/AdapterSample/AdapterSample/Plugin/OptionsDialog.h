// =============================================================================
//	OptionsDialog.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

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
	COptions	m_Options;

	void		DoDataExchange( CDataExchange* pDX );

public:
	;				COptionsDialog( const COptions& inOptions, CWnd* inParent = NULL );
	virtual			~COptionsDialog() {}

	const COptions&	GetOptions() { return m_Options; }

	BOOL			OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnBnClickedEnableExample();
};

#endif // HE_WIN32