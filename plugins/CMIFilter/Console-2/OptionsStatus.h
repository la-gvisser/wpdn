// ============================================================================
// OptionsStatus.h:
//      interface for the COptionsStatus class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "resource.h"
#include "Options.h"


// ============================================================================
//		COptionsStatus
// ============================================================================
 
class COptionsStatus
	:	public CDialog
	,	public CBroadcaster
	,	public CListener
{
	DECLARE_DYNAMIC(COptionsStatus)

protected:
	COptions		m_Options;
	CDialog*		m_pParentDialog;
	CString			m_strNotes[COptions::kType_Max];

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_STATUS };

	;			COptionsStatus();
	virtual		~COptionsStatus(){}

	void		SetParentDialog( CDialog* pDialog ) { 
		m_pParentDialog = pDialog;
	}

	const COptions&	GetOptions() const { return m_Options; };
	CStringA		GetPrefs() const;
	void			SetOptions( const COptions& inOptions );
	void			SetPrefs( CStringA inPrefs );

	// CListener
	virtual void	ListenToMessage( CBLMessage& inMessage );

	afx_msg void	OnBnClickedModify();
};
