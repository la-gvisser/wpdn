// =============================================================================
//	CaseViewDlg.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef IMPLEMENT_TAB

#include "resource.h"
#include "CaseOptionsList.h"
#include "Options.h"

class CCaseViewDlg
	:	public CDialog
{

	DECLARE_DYNAMIC(CCaseViewDlg)

public:

	enum { IDD = IDD_VIEW_CASE };

protected:
	CCaseOptions*		m_pCaseOptions;
	CCaseOptionsList&	m_CaseOptionsList;
	CPeekString			m_strGlobalOutputDirectory;
	int					m_CaseOptionsListIdx;
	bool				m_bIsCapturing;
	CPeekString			m_strDefaultKeyName;
	bool				m_bInitializing;
	COptions*			m_pOptions;

	void				PortListToControl();

	void				UserListToControl();

public:
	CCaseViewDlg( CCaseOptions* CaseOptions, COptions* inOptions, bool bIsCapturing, int Idx = -1, CWnd* pParent = NULL );
	virtual ~CCaseViewDlg() {}

	void		DoDataExchange( CDataExchange* pDX );

	BOOL		OnInitDialog();

protected:

private:
	CFont m_Font;

};

#endif // IMPLEMENT_TAB
