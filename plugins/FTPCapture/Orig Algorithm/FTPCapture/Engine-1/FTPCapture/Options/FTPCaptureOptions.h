// ============================================================================
//	FTPCaptureOptions.h:
//		interface for the CFTPCaptureOptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#pragma once

#include "TargetNamePage.h"
#include "PortNumberPage.h"
#include "AlertingPage.h"
#include "Options.h"


// ============================================================================
//		CFTPCaptureOptions
// ============================================================================

class CFTPCaptureOptions
	:	public CPropertySheet
{
protected:
	COptions			m_Options;
	CTargetNamePage		m_TargetNamePage;
	CPortNumberPage		m_PortNumberPage;
	CAlertingPage		m_AlertPage;

	virtual	BOOL	OnInitDialog();

public:
	;				CFTPCaptureOptions( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	;				CFTPCaptureOptions( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual			~CFTPCaptureOptions() {};

//	CString			GetContextPrefs() const { return m_Options.GetContextPrefs(); }
	const COptions&	GetOptions() const { return m_Options; }

//	void			SetContextPrefs( CString inXml ) { m_Options.SetContextPrefs( inXml ); }
	void			SetOptions( const COptions& inOptions ) { m_Options = inOptions; }

	DECLARE_MESSAGE_MAP()
};
