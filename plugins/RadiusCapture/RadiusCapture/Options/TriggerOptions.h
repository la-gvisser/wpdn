// ============================================================================
//	TriggerOptions.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "CaseOptions.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////
//		CTriggerOptionsDlg
////////////////////////////////////////////////////////////////////////////////

class CTriggerOptionsDlg 
	: public CDialog
{
	DECLARE_DYNAMIC(CTriggerOptionsDlg)

public:
	CTriggerOptionsDlg(CCaseOptions::CTriggerOptions& inOptions, bool m_bIsDockedTab );
	virtual ~CTriggerOptionsDlg() {}

	enum { IDD = IDD_TRIGGER_OPTS };

protected:

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void		 SetControlsInitialState();

protected:
	CCaseOptions::CTriggerOptions&	m_Options;
	bool							m_bIsDockedTab;

};
