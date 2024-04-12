// ============================================================================
//	DataTransferPage.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2011. All rights reserved.

#pragma once

#ifdef IMPLEMENT_DATA_CAPTURE
#include "Options.h"

class CDataTransferPage
	:	public CPropertyPage
{
protected:
	CDataTransfer&  m_DataTransfer;
	COptions&		m_Options;

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	void			EnableControls( bool inEnable );

	bool			IsObscuring() { return m_Options.IsObscureNames(); }

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_DATA_TRANSFER };

	;				CDataTransferPage( COptions& inOptions );
	;				~CDataTransferPage() {}

//	virtual BOOL	OnInitDialog();
	virtual BOOL	OnSetActive();

	afx_msg void OnBnClickedDoDatatransfer();
	afx_msg void OnBnClickedCaptureToDirChk();
	afx_msg void OnBnClickedLogToFileChk();
	afx_msg void OnBnClickedLogFtpCmdsChk();
};

#endif // IMPLEMENT_DATA_CAPTURE
