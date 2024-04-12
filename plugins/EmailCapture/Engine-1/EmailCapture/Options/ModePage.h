// ============================================================================
//	ModePage.h
//		interface for the CModePage class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include "Case.h"
#include "resource.h"

class CCaseDlg;


// ============================================================================
//		CModePage
// ============================================================================

class CModePage
	:	public CDialog
{
	DECLARE_DYNAMIC( CModePage )

protected:
	enum {
		kSave_Minutes,
		kSave_Hours,
		kSave_Days
	} kSaveUnits;

	enum {
		kMax_Kilobytes,
		kMax_Megabytes,
		kMax_Gigabytes
	} kMaxUnits;

	CCaseDlg*			m_pParent;
	CString				m_strFileNamePrefix;
	CString				m_strOutputDirectory;
	CCaseSettings&		m_Settings;
	CReportMode&		m_ReportMode;
	UInt32				m_nTimeCount;
	int					m_nTimeUnits;
	UInt32				m_nSizeCount;
	int					m_nSizeUnits;

	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_MODE_OPTS };

	;				CModePage( CCase& inCase );
	virtual			~CModePage();

	virtual void	OnCancel();
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();

	afx_msg void	OnBnClickedBrowse();
	afx_msg void	OnBnClickedSmtp();
	afx_msg void	OnBnClickedPop3();
	afx_msg void	OnBnClickedMode();
	afx_msg void	OnBnClickedSaveAge();
	afx_msg void	OnBnClickedSaveSize();
};

#endif // HE_WIN32
