// ============================================================================
//	TriggerPage.h
//		interface for the CTriggerPage class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include "resource.h"
#include "Options.h"
#include "PeekTime.h"
#include "afxdtctl.h"


// ============================================================================
// CTriggerPage
// ============================================================================

class CTriggerPage
	:	public CDialog
{
	DECLARE_DYNAMIC( CTriggerPage )

protected:
	typedef enum {
		kOne_Day,
		kOne_Week,
		kOne_Month,
	} tOneUnit;

	CCaseSettings&	m_Settings;

	CPeekTime		GetStartTime();
	void			GetStartTime( SYSTEMTIME& outDateTime );
	CPeekTime		GetStopTime();

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	void			OnOneUnit( tOneUnit inUnit );

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_TRIGGER_OPTS };

	;				CTriggerPage( CCase& inCase );
	virtual			~CTriggerPage() {}

	virtual void	OnCancel();
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	void			EndDialog( int nEndID );

	afx_msg void	OnBnClickedTimeNow();
	afx_msg void	OnBnClickedTimeOneDay();
	afx_msg void	OnBnClickedTimeOneWeek();
	afx_msg void	OnBnClickedTimeOneMonth();
};

#endif // HE_WIN32
