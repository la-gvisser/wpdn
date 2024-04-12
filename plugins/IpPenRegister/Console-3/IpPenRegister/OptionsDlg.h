// ============================================================================
//	OptionsDlg.h:
//		interface for the COptionsDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "GeneralOpts.h"
#include "Options.h"


// ============================================================================
//		COptionsDlg
// ============================================================================

class COptionsDlg
	:	public CPropertySheet
{
protected:
	virtual BOOL	OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	COptions		m_Options;
	CGeneralOpts	m_GeneralPage;

	;				COptionsDlg( UINT nIDCaption, CWnd* pParentWnd = NULL,
						UINT iSelectPage = 0 );
	;				COptionsDlg( LPCTSTR pszCaption, CWnd* pParentWnd = NULL,
						UINT iSelectPage = 0 );
	virtual			~COptionsDlg() {};
};
