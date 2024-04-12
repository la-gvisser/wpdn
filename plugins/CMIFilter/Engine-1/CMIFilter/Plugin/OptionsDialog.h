// =============================================================================
//	OptionsDialog.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#if defined(_WIN32) && defined(PEEK_UI)

#include "resource.h"
#include "Options.h"


// =============================================================================
//		COptionsDialog
// =============================================================================

class COptionsDialog
	:	public CDialog
{
public:
	enum { IDD = IDD_PLUGIN_OPTIONS };

protected:
	COptions	m_Options;
	CString		m_strNotes[COptions::kType_Max];
	bool		m_bInEditValidation;

	void		DoDataExchange( CDataExchange* pDX );
	void		ValidateNumeric( int nDlgNumber, UInt32 nMaxValue, UInt32& theValue, bool bHexValue = false );
	void		UpdateIpFilterList();

public:
	;				COptionsDialog( const COptions& inOptions, CWnd* inParent = nullptr );
	virtual			~COptionsDialog() {}

	const COptions&	GetOptions() { return m_Options; }

	BOOL			OnInitDialog();
	void			OnOK();

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnBnClickedAddIpFilter();
	afx_msg void	OnBnClickedDeleteIpFilter();
	afx_msg void	OnBnClickedEditIpFilter();
	afx_msg void	OnBnClickedInterceptIdCheck();
	afx_msg void	OnBnClickedInterceptSiteCheck();
	afx_msg void	OnBnClickedIpCheck();
	afx_msg void	OnBnClickedMacCheck();
	afx_msg void	OnBnClickedSaveBrowse();
	afx_msg void	OnBnClickedSaveCheck();
	afx_msg void	OnBnClickedVlanCheck();
	afx_msg void	OnCbnSelChangeProtocolType();
	afx_msg void	OnEnChangeHexEdit();
	afx_msg void	OnEnChangeProtocolTypeEdit();
	afx_msg void	OnLbnSelChangeIpFilterList();
	afx_msg void	OnLvnItemChangedIpFilterList( NMHDR *pNMHDR, LRESULT *pResult );
};

#endif // _WIN32 && PEEK_UI
