// ============================================================================
//	CaptureOptionsDlg.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CaptureOptions.h"
#include "Options.h"
#include "resource.h"


////////////////////////////////////////////////////////////////////////////////
//		CCaptureOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CCaptureOptionsDlg 
	: public CDialog
{
		DECLARE_DYNAMIC(CCaptureOptionsDlg)

	public:
		CCaptureOptionsDlg( CCaptureOptions& inCaptureOptions, COptions* inOptions, bool bIsCapturing, bool bIsDockedTab )
			:	m_CaptureOptions( inCaptureOptions )
			,	m_GlobalCaptureOptions( inOptions->GetCaptureOptions() )
			,	m_bIsCapturing( bIsCapturing )
			,	m_bIsDockedTab( bIsDockedTab )
			,	m_bInitializing( false )
		{}

		virtual ~CCaptureOptionsDlg() {}
		virtual BOOL	OnInitDialog();

		enum { IDD = IDD_CASE_CAPTURE_OPTS };

protected:

		DECLARE_MESSAGE_MAP()

		afx_msg void OnBnClickedCaptureToFile();
		afx_msg void OnBnClickedCaptureToScreen();
		afx_msg void OnEnChangeFileAgeCount();
		afx_msg void OnCbnSelchangeFileAgeUnits();
		afx_msg void OnEnChangeFileSizeCount();
		afx_msg void OnCbnSelchangeFileSizeUnits();
		afx_msg void OnBnClickedFileAge();
		afx_msg void OnBnClickedFileSize();
		afx_msg void OnBnClickedChkUseGlobalDefaults();
		afx_msg void OnBnClickedLogMsgsToFile();
		afx_msg void OnBnClickedLogMsgsToScreen();
		afx_msg void OnBnClickedFoldersForCases();

	virtual void	DoDataExchange( CDataExchange* pDX );

	void			SetControlsInitialState();
	void			SetControlsEnabled();
	void			SetAgeValues();
	void			SetSizeValues();

protected:
	CCaptureOptions&	m_CaptureOptions;
	CCaptureOptions&	m_GlobalCaptureOptions;
	bool				m_bIsCapturing;
	bool				m_bIsDockedTab;
	bool				m_bInitializing;
};