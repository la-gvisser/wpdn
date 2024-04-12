// ============================================================================
//	CasesOpts.h
//		interface for the CCasesOpts class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef HE_WIN32

#include "resource.h"
#include "PeekTime.h"
#include "Options.h"
#include "CheckboxUtil.h"
#include <afxcmn.h>

#define DT_MULTILINE	0


// ============================================================================
//		CColumnProp
// ============================================================================

class CColumnProp
{
protected:
	int				m_nId;
	CPeekString		m_strLabel;
	CPeekString		m_strConfigTag;
	int				m_nWidth;

public:
	;		CColumnProp() : m_nWidth( 0 ) {}
	;		CColumnProp( int inId, const CPeekString& inLabel,
				const CPeekString& inConfigTag, int inWidth )
		:	m_nId( inId )
		,	m_strLabel( inLabel )
		,	m_strConfigTag( inConfigTag )
		,	m_nWidth( inWidth )
	{}

	int					GetId() const { return m_nId; }
	const CPeekString&	GetLabel() const { return m_strLabel; }
	const CPeekString&	GetConfigTag() const { return m_strConfigTag; }
	int					GetWidth() const { return m_nWidth; }

	void		SetLabel( const CPeekString& inLabel ) { m_strLabel = inLabel; }
	void		SetConfigTag( const CPeekString& inConfigTag ) { m_strConfigTag = inConfigTag; }
	void		SetWidth( int inWidth ) { m_nWidth = inWidth; }
};

class CColumnPropList
	:	public CAtlArray<CColumnProp>
{
public:
	;		CColumnPropList();
};

	
// ============================================================================
//		CCasesEdit
// ============================================================================

class CCasesEdit
	:	public CEdit {
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
};


// ============================================================================
//		CCasesCtrl
// ============================================================================

class CCasesCtrl
	:	public CListCtrl
{
protected:
	class CDrawIterator {
	protected:
		CCasesCtrl*		m_pCtrl;
		CDC&			m_DC;
		CSize			m_sizeCheckBox;
		int				m_nColumn;
		CRect			m_rc;

	public:
		;		CDrawIterator( CCasesCtrl* inCtrl, CDC& inDC, CSize inSize, const CRect& inRC )
					: m_pCtrl( inCtrl ), m_DC( inDC ), m_sizeCheckBox( inSize ), m_nColumn( 0 ), m_rc( inRC )
					{}
		void	Draw( const CString& inText, int inDrawFlags = DT_SINGLELINE );
		void	DrawChecked( bool inChecked, const CString& inText, int inDrawFlags = DT_SINGLELINE );
	};

#if (0)
	COptionsView&				m_View;
#endif
	COptions::COptionsCases&	m_Cases;
	CSize						m_sizeCheckBox;
	CPeekTime					m_OldeTime;

	void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

public:
	;		CCasesCtrl( /*COptionsView& inView, */COptions::COptionsCases& inCases )
#if (0)
		:	m_View( inView )
#endif
		:	m_Cases( inCases )
		,	m_sizeCheckBox( 12, 12 )
		,	m_OldeTime()
		{}

	void	GetCheckboxRect( const CRect& inRect, CRect& outRect ) const;
	void	Init();
	void	Refresh();
	void	UpdateStatus();
	void	Term();

	DECLARE_MESSAGE_MAP()

	afx_msg void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItem );
	afx_msg LRESULT OnSetFont( WPARAM wParam, LPARAM );
};


// ============================================================================
//		CCasesOpts
// ============================================================================

class CCasesOpts
	:	public CDialog
{
	DECLARE_DYNAMIC( CCasesOpts )

protected:
	COptions		m_Options;
	bool			m_bReadOnly;

	size_t			m_nLastIndex;
	CCasesCtrl		m_CaseListCtrl;

	CPoint			m_ptCaseListDelta;
	int				m_nButtonTopDelta;
	int				m_nEditLeft;
	int				m_nAddLeft;
	int				m_nDeleteLeft;
	int				m_nImportLeft;
	int				m_nExportLeft;
	int				m_nSliceStatusLeft;

	virtual void	DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

	void	EditCase();
	size_t	FindLastIndex( const CPeekString& inName );
	void	SetLastIndex();
	void	UpdateStatus();
	BOOL	OnToolTipNotify( UINT nId, NMHDR* pNMHDR, LRESULT* pResult );
	BOOL	OnInitDialog();

public:
	enum { IDD = IDD_CASE_LIST_OPTS };

	;				CCasesOpts( const COptions& inOptions, bool inReadOnly = false );
	;				~CCasesOpts() {}

	void			EnableButtons();

	const COptions&	GetOptions() const { return m_Options; }

	bool			IsReadOnly() const { return m_bReadOnly; }

	void			Refresh( bool bRefresh = true ) {
		SetLastIndex();
		if ( bRefresh ) {
			m_CaseListCtrl.Refresh();
			m_CaseListCtrl.UpdateStatus();
		}
		EnableButtons();
		UpdateStatus();
	}

	void			SetOptions( const COptions& inOptions ) {
		m_Options = inOptions;
		Refresh( true );
	}

	DECLARE_MESSAGE_MAP()
	afx_msg void	OnBnClickedEditCase();
	afx_msg void	OnBnClickedAddCase();
	afx_msg void	OnBnClickedDeleteCase();
	afx_msg void	OnBnClickedImportAddress();
	afx_msg void	OnBnClickedExportAddress();
	afx_msg void	OnNMClickCaseList( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void	OnNotifyDoubleClickedCaseList( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnSize( UINT nType, int cx, int cy );
	afx_msg void	OnTimer( UINT_PTR nIDEvent );
};


// ============================================================================
//		CCasesOptsList
// ============================================================================

class CCasesOptsList
	:	public CPeekArray<CCasesOpts*>
{
protected:
	CRect	m_rcSize;

public:
	;		CCasesOptsList() {}
	;		~CCasesOptsList() {
		Reset();
	}

	size_t	Add( CCasesOpts* inOpts ) {
		if ( inOpts && inOpts->GetSafeHwnd() ) {
			inOpts->MoveWindow( m_rcSize );
		}
		return CPeekArray<CCasesOpts*>::Add( inOpts );
	}

	void	Hide() {
		const size_t	nCount( GetCount() );
		for ( size_t i( 0 ); i < nCount; i++ ) {
			CCasesOpts*	pOpts( GetAt( i ) );
			if ( pOpts && pOpts->GetSafeHwnd() ) {
				pOpts->ShowWindow( SW_HIDE );
			}
		}
	}

	void	Reset() {
		const size_t	nCount( GetCount() );
		for ( size_t i( 0 ); i < nCount; i++ ) {
			delete GetAt( i );
		}
		CPeekArray<CCasesOpts*>::RemoveAll();
	}

	void	ResizeAll( const CRect& inSize ) {
		m_rcSize = inSize;

		const size_t	nCount( GetCount() );
		for ( size_t i( 0 ); i < nCount; i++ ) {
			CCasesOpts*	pOpts( GetAt( i ) );
			if ( pOpts && pOpts->GetSafeHwnd() ) {
				pOpts->MoveWindow( inSize );
			}
		}
	}
};


// ============================================================================
//		COptionsTreeCtrl
// ============================================================================


class COptionsTreeCtrl
	:	public CTreeCtrl
{
public:
	typedef	void	OTCFunc( DWORD_PTR& inItemData, DWORD_PTR inArgument );

protected:
	void	DeepIterate( OTCFunc* inFunc, DWORD_PTR inArgument = NULL );

	static void	DoDestroy( DWORD_PTR& inItemData, DWORD_PTR ) {
		CDialog*	pDialog( reinterpret_cast<CDialog*>( inItemData ) );
		if ( pDialog ) {
			if ( pDialog->GetSafeHwnd() != NULL ) {
				pDialog->DestroyWindow();
			}
		}
	}
	static void	DoMaxSize( DWORD_PTR& inItemData, DWORD_PTR inArgument ) {
		CDialog*	pDialog( reinterpret_cast<CDialog*>( inItemData ) );
		CSize*		pMaxSize( reinterpret_cast<CSize*>( inArgument ) );
		if ( pDialog && pMaxSize &&(pDialog->GetSafeHwnd() != NULL) ) {
			CRect	rc;
			pDialog->GetWindowRect( &rc );
			if ( rc.Height() > pMaxSize->cx ) {
				pMaxSize->cx = rc.Height();
			}
			if ( rc.Width() > pMaxSize->cy ) {
				pMaxSize->cy = rc.Width();
			}
		}
	}
	static void	DoMonitor( DWORD_PTR& inItemData, DWORD_PTR inArgument ) {
		CDialog*	pDialog( reinterpret_cast<CDialog*>( inItemData ) );
		DWORD_PTR	nIDEvent( inArgument );
		if ( pDialog &&  (pDialog->GetSafeHwnd() != NULL) ) {
			pDialog->SendMessage( WM_TIMER, nIDEvent );
		}
	}
	static void	DoMove( DWORD_PTR& inItemData, DWORD_PTR inArgument ) {
		CDialog*	pDialog( reinterpret_cast<CDialog*>( inItemData ) );
		CRect*		pRect( reinterpret_cast<CRect*>( inArgument ) );
		if ( pDialog && pRect && (pDialog->GetSafeHwnd() != NULL) ) {
			pDialog->MoveWindow( pRect );
		}
	}
	static void	DoUpdate( DWORD_PTR& inItemData, DWORD_PTR ) {
		CDialog*	pDialog( reinterpret_cast<CDialog*>( inItemData ) );
		if ( pDialog && (pDialog->GetSafeHwnd() != NULL) ) {
			pDialog->UpdateData( TRUE );
		}
	}

public:
	void	DestroyWindows() { DeepIterate( &DoDestroy ); }
	void	MaxWindowSize( CSize* inSize ) { DeepIterate( &DoMaxSize, reinterpret_cast<DWORD_PTR>( inSize ) ); }
	void	Monitor( UINT_PTR inIDEvent ) { DeepIterate( &DoMonitor, inIDEvent ); }
	void	MoveWindows( const CRect* inRect ) { DeepIterate( &DoMove, reinterpret_cast<DWORD_PTR>( inRect ) ); }
	void	UpdateWindows() { DeepIterate( &DoUpdate ); }
};

#endif // HE_WIN32
