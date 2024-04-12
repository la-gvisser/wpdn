// ============================================================================
// IpFilterDlg.h:
//      interface for the CIpFilterDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2013. All rights reserved.

#pragma once

#ifdef _WIN32

#include "resource.h"
#include "Options.h"


// ============================================================================
//		CIpFilterDialog
// ============================================================================
 
class CIpFilterDialog
	:	public CDialog
{
	DECLARE_DYNAMIC(CIpFilterDialog)

protected:
	CIpFilter		m_IpFilter;

protected:
	virtual void	DoDataExchange( CDataExchange* pDX );
	virtual BOOL	OnInitDialog();

public:
	enum { IDD = IDD_IPFILTER };

	;			CIpFilterDialog( CWnd* pParent = nullptr );
	virtual		~CIpFilterDialog() {}

	const CIpFilter&	GetIpFilter() const { return m_IpFilter; }

	void		SetIpFilter( const CIpFilter& inIpFilter ) {
		m_IpFilter = inIpFilter;
	}

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnBnClickedAddress2Radio();
	afx_msg void	OnBnClickedPortCheck();
};

#endif // _WIN32
