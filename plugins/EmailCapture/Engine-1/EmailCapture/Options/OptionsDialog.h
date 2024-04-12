// ============================================================================
//	OptionsDlg.h
//		interface for the COptionsDialog class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#ifdef PEEK_UI

#include "resource.h"
#include "Options.h"
#include "CasesOpts.h"

class COmniPlugin;


// ============================================================================
//		COptionsDialog
// ============================================================================

class COptionsDialog
	:	public CDialog
{
protected:
	DECLARE_DYNAMIC( COptionsDialog )
	DECLARE_MESSAGE_MAP()

protected:
	enum { IDD = IDD_PLUGIN_OPTIONS };

	CCasesOpts			m_CasesOpts;

protected:
	virtual void	DoDataExchange( CDataExchange* pDX );
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();

public:
	;				COptionsDialog( const COptions& inOptions, CWnd* pParent = NULL );
	virtual			~COptionsDialog();

	const COptions&	GetOptions() const {
		return m_CasesOpts.GetOptions();
	}

	afx_msg void	OnSize( UINT nType, int cx, int cy );
	afx_msg void	OnTimer( UINT_PTR nIDEvent );
};

#endif // PEEK_UI
