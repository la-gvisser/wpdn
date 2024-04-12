// ============================================================================
// OptionsDlg.h:
//      interface for the COptionsDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "resource.h"
#include "Options.h"


// ============================================================================
//		COptionsDialog
// ============================================================================
 
class COptionsDialog
	:	public CDialog
{
	DECLARE_DYNAMIC(COptionsDialog)

public:
	class CItemList
		:	std::vector<UINT>
	{
	protected:
		CDialog*	m_pDlg;

	public:
		static void EnableItem( CDialog* inDlg, UINT inId, bool inEnable ) {
			CWnd*	pItem( inDlg->GetDlgItem( inId ) );
			if ( pItem ) pItem->EnableWindow( inEnable );
		}
		static void ShowItem( CDialog* inDlg, UINT inId, bool inShow ) {
			CWnd*	pItem( inDlg->GetDlgItem( inId ) );
			if ( pItem ) pItem->ShowWindow( (inShow) ? SW_SHOW : SW_HIDE );
		}

		CItemList( CDialog* inDlg )
			: m_pDlg( inDlg )
		{
		}

		CItemList( CDialog* inDlg, UINT* inIds )
			: m_pDlg( inDlg )
		{
			if ( inIds ) {
				const UINT* pNext( inIds );
				while ( *pNext ) {
					push_back( *pNext );
					++pNext;
				}
			}
		}

		void operator=( UINT* inIds ) {
			inIds;
		}

		void EnableItems( bool inEnable ) {
			std::vector<UINT>::const_iterator itr;
			for ( itr = begin(); itr != end(); ++itr ) {
				EnableItem( m_pDlg, *itr, inEnable );
			}
		}

		void ShowItems( bool inShow ) {
			std::vector<UINT>::const_iterator itr;
			for ( itr = begin(); itr != end(); ++itr ) {
				ShowItem( m_pDlg, *itr, inShow );
			}
		}
	};

protected:
	COptions		m_Options;
	CString			m_strNotes[COptions::kType_Max];

	virtual void	DoDataExchange( CDataExchange* pDX );
	virtual BOOL	OnInitDialog();
	void			UpdateIpFilterList();

public:
	enum { IDD = IDD_OPTIONS };

	;				COptionsDialog( CWnd* pParent = NULL );
	virtual			~COptionsDialog() {}

	const COptions&	GetOptions() const { return m_Options; };
	CStringA		GetPrefs() const;
	void			SetOptions( const COptions& inOptions );
	void			SetPrefs( CStringA inPrefs );

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
	afx_msg void	OnEnChangePrototypeEdit();
	afx_msg void	OnEnChangeHexEdit();
	afx_msg void	OnLbnSelChangeIpFilterList();
	afx_msg void	OnLvnItemChangedIpFilterList( NMHDR *pNMHDR, LRESULT *pResult );
};
