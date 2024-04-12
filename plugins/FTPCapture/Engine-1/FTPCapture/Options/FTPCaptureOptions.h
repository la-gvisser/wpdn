// ============================================================================
//	FTPCaptureOptions.h:
//		interface for the CFTPCaptureOptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#pragma once

#include "TargetNamePage.h"
#include "PortNumberPage.h"
#include "AlertingPage.h"
#ifdef IMPLEMENT_DATA_CAPTURE
#include "DataTransferPage.h"
#endif // IMPLEMENT_DATA_CAPTURE
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
  #ifdef IMPLEMENT_DATA_CAPTURE
	CDataTransferPage	m_DataTransferPage;
  #endif // IMPLEMENT_DATA_CAPTURE

	virtual	BOOL	OnInitDialog();

public:
	;				CFTPCaptureOptions( UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	;				CFTPCaptureOptions( LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0 );
	virtual			~CFTPCaptureOptions() {};

	const COptions&	GetOptions() const { return m_Options; }

	void			SetOptions( const COptions& inOptions ) { m_Options = inOptions; }

	DECLARE_MESSAGE_MAP()
};
