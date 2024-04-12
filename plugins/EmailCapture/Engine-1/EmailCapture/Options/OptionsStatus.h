// ============================================================================
//	OptionsStatus.h
//		interface for the COptionsStatus class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include "resource.h"
#include "PeekTime.h"
#include "Options.h"


// ============================================================================
//		COptionsStatus
// ============================================================================

class COptionsStatus
	:	public CDialog
{
	DECLARE_DYNAMIC( COptionsStatus )

protected:
	COptions		m_Options;

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	BOOL	OnInitDialog();

public:
	enum { IDD = IDD_OPTIONS_STATUS };

	;				COptionsStatus( const COptions& inOptions );
	;				~COptionsStatus() {}

	const COptions&	GetOptions() const { return m_Options; }

	void			Refresh( const COptions& inOptions );
	void			Reset();

	void			SetCaseList();
	void			SetStatus( size_t inIndex );

	DECLARE_MESSAGE_MAP()
	afx_msg void	OnBnClickedExportAddress();
	afx_msg void	OnLbnSelchangeCaseList();
	afx_msg	void	OnShowWindow( BOOL bShow, UINT nStatus );
};


// ============================================================================
//		COptionsStatusList
// ============================================================================

class COptionsStatusList
	:	public CPeekArray<COptionsStatus*>
{
protected:
	CRect	m_rcSize;

public:
	;		COptionsStatusList() {}
	;		~COptionsStatusList() {
		Reset();
	}

	size_t	Add( COptionsStatus* inOptStat ) {
		if ( inOptStat && inOptStat->GetSafeHwnd() ) {
			inOptStat->MoveWindow( m_rcSize );
		}
		return CPeekArray<COptionsStatus*>::Add( inOptStat );
	}

	void	Hide() {
		const size_t	nCount( GetCount() );
		for ( size_t i( 0 ); i < nCount; i++ ) {
			COptionsStatus*	pOptStat( GetAt( i ) );
			if ( pOptStat && pOptStat->GetSafeHwnd() ) {
				pOptStat->ShowWindow( SW_HIDE );
			}
		}
	}

	void	Reset() {
		const size_t	nCount( GetCount() );
		for ( size_t i( 0 ); i < nCount; i++ ) {
			delete GetAt( i );
		}
		CPeekArray<COptionsStatus*>::RemoveAll();
	}

	void	ResizeAll( const CRect& inSize ) {
		m_rcSize = inSize;

		const size_t	nCount( GetCount() );
		for ( size_t i( 0 ); i < nCount; i++ ) {
			COptionsStatus*	pOptStat( GetAt( i ) );
			if ( pOptStat && pOptStat->GetSafeHwnd() ) {
				pOptStat->MoveWindow( inSize );
			}
		}
	}
};

#endif // HE_WIN32
