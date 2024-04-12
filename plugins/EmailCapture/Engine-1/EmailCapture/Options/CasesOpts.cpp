// ============================================================================
//	CasesOpts.cpp
//		implementation of the CCasesOpts class.
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"

#ifdef HE_WIN32

#include "CasesOpts.h"
#include "CaseDlg.h"
#include "PeekDataModeler.h"
#include "GlobalTags.h"
#include <afxwin.h>
#include <afxdlgs.h>


// ============================================================================
//		Configuration Tags
// ============================================================================

namespace ConfigTags
{
	// Root Name
	const CPeekString	kRootName( L"EmailCaptureOE" );

	// Case Options
	const CPeekString	kCaseList( L"CaseList" );
	const CPeekString	kCase( L"Case" );
}


// ============================================================================
//		ColumnProps
// ============================================================================

namespace ColumnProps
{
	// Case Options Column Names
	const CPeekString	kCaseId( L"Id" );
	const CPeekString	kName( L"Name" );
	const CPeekString	kAddress( L"Address" );
	const CPeekString	kCount( L"Count" );
	const CPeekString	kProtocol( L"Protocol" );
	const CPeekString	kMode( L"Mode" );
	const CPeekString	kState( L"State" );
	const CPeekString	kStartTime( L"Start Time" );
	const CPeekString	kStopTime( L"Stop Time" );
	const CPeekString	kFolder( L"Output" );

	// Case Options Column Widths
	const int		kCaseIdWidth( 0 );
	const int		kNameWidth( 100 );
	const int		kAddressWidth( 200 );
	const int		kCountWidth( 50 );
	const int		kProtocolWidth( 75 );
	const int		kModeWidth( 55 );
	const int		kStateWidth( 70 );
	const int		kStartTimeWidth( 175 );
	const int		kStopTimeWidth( 175 );
	const int		kOutputWidth( 200 );
}

// Used to initialize CColumnPropList
// Column Labels
#define kTag_Id				L"Id"
#define	kTag_Name			L"Name"
#define	kTag_Address		L"Address"
#define	kTag_Count			L"Count"
#define kTag_Protocol		L"Protocol"
#define	kTag_Mode			L"Mode"
#define	kTag_State			L"State"
#define	kTag_Triggers		L"Triggers"
#define	kTag_Output			L"Output"

// Column Configuration Tags
#define kConfigTag_Id		L"Id"
#define	kConfigTag_Name		L"Name"
#define	kConfigTag_Address	L"Address"
#define	kConfigTag_Count	L"Count"
#define	kConfigTag_Protocol	L"Protocol"
#define kConfigTag_Mode		L"Mode"
#define	kConfigTag_State	L"State"
#define	kConfigTag_Triggers	L"Triggers"
#define	kConfigTag_Output	L"Output"

// Column Widths
#define kDefault_CaseIdWidth	0
#define kDefault_NameWidth		100
#define kDefault_AddressWidth	200
#define kDefault_CountWidth		50
#define kDefault_ProtocolWidth	75
#define kDefault_ModeWidth		75
#define kDefault_StateWidth		55
#define kDefault_TriggersWidth	175
#define kDefault_OutputWidth	200

CColumnPropList	g_ColumnPropList;


// ============================================================================
//		CColumnPropList
// ============================================================================

CColumnPropList::CColumnPropList()
{
	Add( CColumnProp( CCase::kColumnId_Id, kTag_Id, kConfigTag_Id, kDefault_CaseIdWidth ) );
	Add( CColumnProp( CCase::kColumnId_Name, kTag_Name, kConfigTag_Name, kDefault_NameWidth ) );
	Add( CColumnProp( CCase::kColumnId_Address, kTag_Address, kConfigTag_Address, kDefault_AddressWidth ) );
	Add( CColumnProp( CCase::kColumnId_Count, kTag_Count, kConfigTag_Count, kDefault_CountWidth ) );
	Add( CColumnProp( CCase::kColumnId_Protocol, kTag_Protocol, kConfigTag_Protocol, kDefault_ProtocolWidth ) );
	Add( CColumnProp( CCase::kColumnId_Mode, kTag_Mode, kConfigTag_Mode, kDefault_ModeWidth ) );
	Add( CColumnProp( CCase::kColumnId_State, kTag_State, kConfigTag_State, kDefault_StateWidth ) );
	Add( CColumnProp( CCase::kColumnId_Triggers, kTag_Triggers, kConfigTag_Triggers, kDefault_TriggersWidth ) );
	Add( CColumnProp( CCase::kColumnId_Folder, kTag_Output, kConfigTag_Output, kDefault_OutputWidth ) );
}


// ============================================================================
//		CDrawIterator
// ============================================================================

// ----------------------------------------------------------------------------
//		Draw
// ----------------------------------------------------------------------------

void
CCasesCtrl::CDrawIterator::Draw(
	const CString&	inText,
	int				inDrawFlags /* = DT_SINGLELINE */ )
{
	ASSERT( m_pCtrl != NULL );
	if ( m_pCtrl == NULL ) return;

	CRect	rcItem( m_rc );
	rcItem.right = rcItem.left + m_pCtrl->GetColumnWidth( m_nColumn );

	rcItem.DeflateRect( 4, 2 );

	m_DC.DrawText(
		inText,
		&rcItem,
		(DT_LEFT | DT_TOP | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS| DT_HIDEPREFIX | DT_WORDBREAK | inDrawFlags) );

	// increment
	m_rc.left += m_pCtrl->GetColumnWidth( m_nColumn );
	m_nColumn++;
}


// ----------------------------------------------------------------------------
//		DrawChecked
// ----------------------------------------------------------------------------

void
CCasesCtrl::CDrawIterator::DrawChecked(
	bool			inChecked,
	const CString&	inText,
	int				inDrawFlags /* = DT_SINGLELINE */ )
{
	ASSERT( m_pCtrl != NULL );
	if ( m_pCtrl == NULL ) return;

	CRect	rcItem( m_rc );
	rcItem.right = rcItem.left + m_pCtrl->GetColumnWidth( m_nColumn );

	rcItem.DeflateRect( 4, 2 );

	CRect	rcCheckbox;
	m_pCtrl->GetCheckboxRect( rcItem, rcCheckbox );
	int nValue = (inChecked) ? 1 : 0;
	CheckboxUtil::DrawCheckbox( m_DC, rcCheckbox, true, nValue );
	rcItem.left += rcCheckbox.right;

	m_DC.DrawText(
		inText,
		&rcItem,
		(DT_LEFT | DT_TOP | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS| DT_HIDEPREFIX | DT_WORDBREAK | inDrawFlags) );

	// increment
	m_rc.left += m_pCtrl->GetColumnWidth( m_nColumn );
	m_nColumn++;
}


// ============================================================================
//		CCasesCtrl
// ============================================================================

// ----------------------------------------------------------------------------
//		DrawItem
// ----------------------------------------------------------------------------

void
CCasesCtrl::DrawItem(
	LPDRAWITEMSTRUCT	lpDrawItemStruct )
{
	ASSERT( lpDrawItemStruct->CtlType == ODT_LISTVIEW );
	if ( lpDrawItemStruct->itemID >= m_Cases.GetCount() ) return;

	const CCase&	Case( m_Cases.GetAt( lpDrawItemStruct->itemID ) );

	CDC	dc;
	dc.Attach( lpDrawItemStruct->hDC );

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	// If this item is selected, set the background color
	// and the text color to appropriate values. Also, erase
	// rect by filling it with the background color.
	if ( (lpDrawItemStruct->itemAction | ODA_SELECT)  &&
		 (lpDrawItemStruct->itemState & ODS_SELECTED) ) {
		dc.SetTextColor( ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
		dc.SetBkColor( ::GetSysColor( COLOR_HIGHLIGHT ) );
		dc.FillSolidRect( &lpDrawItemStruct->rcItem, ::GetSysColor( COLOR_HIGHLIGHT ) );
	}
	else {
		dc.FillSolidRect( &lpDrawItemStruct->rcItem, crOldBkColor );
	}

	// If this item has the focus, draw a red frame around the
	// item's rect.
	if ( (lpDrawItemStruct->itemAction | ODA_FOCUS)  &&
		 (lpDrawItemStruct->itemState & ODS_FOCUS) ) {
		CBrush br( ::GetSysColor( COLOR_BTNHILIGHT ) );
		dc.FrameRect( &lpDrawItemStruct->rcItem, &br );
	}

	// Draw the text.
	CDrawIterator							DrawIter( this, dc, m_sizeCheckBox, lpDrawItemStruct->rcItem );

	DrawIter.Draw( CString( Case.GetColumnText( g_ColumnPropList[0].GetId() ) ), DT_MULTILINE );
	DrawIter.DrawChecked( Case.IsEnabled(), CString( Case.GetColumnText( g_ColumnPropList[1].GetId() ) ) );
	for ( size_t i = 2; i < g_ColumnPropList.GetCount(); i++ ) {
		DrawIter.Draw( CString( Case.GetColumnText( g_ColumnPropList[i].GetId() ) ), DT_MULTILINE );
	}

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor( crOldTextColor );
	dc.SetBkColor( crOldBkColor );

	dc.Detach();
}


// ----------------------------------------------------------------------------
//		GetCheckboxRect
// ----------------------------------------------------------------------------

void
CCasesCtrl::GetCheckboxRect(
	const CRect&	inRect,
	CRect&			outRect ) const
{
	outRect.left   = 0;
	outRect.top    = 0;
	outRect.right  = m_sizeCheckBox.cx;
	outRect.bottom = m_sizeCheckBox.cy;
	CheckboxUtil::CenterRectInRect( outRect, inRect, CheckboxUtil::kCenterVertical );
}


// ----------------------------------------------------------------------------
//		Init
// ----------------------------------------------------------------------------

void
CCasesCtrl::Init()
{
	m_OldeTime.SetNow();

	CDC*	pDC = GetDC();
	if ( pDC == NULL ) return;

	CheckboxUtil::CTheme	theme;
	theme.OpenThemeData( GetSafeHwnd(), L"button" );

	if ( !theme.IsThemeNull() ) {
		theme.GetThemePartSize( pDC->GetSafeHdc(), BP_CHECKBOX,
			CBS_CHECKEDNORMAL, NULL, TS_TRUE, &m_sizeCheckBox );
	}

	for ( size_t i = 0; i < g_ColumnPropList.GetCount(); i ++ ) {
		int		nIndex = static_cast<int>( i );
		InsertColumn( nIndex, g_ColumnPropList[nIndex].GetLabel() );
		SetColumnWidth( nIndex, g_ColumnPropList[nIndex].GetWidth() );
	}

	Refresh();
}


// ----------------------------------------------------------------------------
//		Refresh
// ----------------------------------------------------------------------------

void
CCasesCtrl::Refresh()
{
	DeleteAllItems();
	for ( size_t i = 0; i < m_Cases.GetCount(); i++ ) {
		InsertItem( static_cast<int>( i ), L"" );
	}
}


// ----------------------------------------------------------------------------
//		UpdateStatus
// ----------------------------------------------------------------------------

void
CCasesCtrl::UpdateStatus()
{
	ASSERT( GetItemCount() == m_Cases.GetCount32() );
	if ( GetItemCount() != static_cast<int>( m_Cases.GetCount32() ) ) {
		return;
	}

	const CCaseList&	aCases( m_Cases.GetList() );
	size_t				dwCount = aCases.GetCount();
	CPeekTime			ptNow( true );
	for ( size_t i = 0; i < dwCount; i++ ) {
		const CCase&			Case( aCases[i] );
		const CCaseSettings&	Settings( Case.GetSettings() );

		if ( Settings.IsActive( ptNow ) != Settings.IsActive( m_OldeTime ) ) {
			InvalidateRect( NULL, FALSE );
			break;
		}
	}
	m_OldeTime = ptNow;
}


// ----------------------------------------------------------------------------
//		Term
// ----------------------------------------------------------------------------

void
CCasesCtrl::Term()
{
	//	DWORD	dwCount( m_Cases.GetCount() );
	//	for ( DWORD i = 0; i < dwCount; i++ ) {
	//		CCase&	Case( const_cast<CCase&>( m_Cases.GetAt( i ) ) );

	//		bool	bChecked( (m_CaseListCtrl.GetCheck( i ) == BST_CHECKED) );
	//		Case.SetEnabled( bChecked );
	//		m_Cases.Update( i, Case );
	//	}

#if (0)
	for ( size_t i = 0; i < g_ColumnPropList.GetCount(); i++ ) {
		int		nIndex = static_cast<int>( i );
		g_ColumnPropList[nIndex].SetWidth( GetColumnWidth( nIndex ) );
	}
#endif
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP( CCasesCtrl, CListCtrl )
	ON_WM_MEASUREITEM_REFLECT()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		MeasureItem
// ----------------------------------------------------------------------------

void
CCasesCtrl::MeasureItem(
	LPMEASUREITEMSTRUCT lpMeasureItem )
{
	if ( lpMeasureItem == NULL ) return;

	TEXTMETRIC	tm;
	HDC			hDC = ::GetDC( NULL );
	CFont*		pFont = GetFont();
	HFONT		hFontOld = static_cast<HFONT>( SelectObject( hDC, pFont->GetSafeHandle() ) );

	GetTextMetrics( hDC, &tm );
	lpMeasureItem->itemHeight = ((tm.tmHeight + tm.tmExternalLeading + 3) * 2);
	SelectObject( hDC, hFontOld );
	::ReleaseDC( NULL, hDC );
}


// ----------------------------------------------------------------------------
//		OnSetFont
// ----------------------------------------------------------------------------

LRESULT
CCasesCtrl::OnSetFont(
	WPARAM ,
	LPARAM )
{
    LRESULT res = Default();

    CRect rc;
    GetWindowRect( &rc );

    WINDOWPOS wp;
    wp.hwnd  = m_hWnd;
    wp.cx    = rc.Width();
    wp.cy    = rc.Height();
    wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
    SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

    return res;
}


// ============================================================================
//		CCasesEdit
// ============================================================================

BEGIN_MESSAGE_MAP(CCasesEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void
CCasesEdit::OnChar(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags )
{
	if ( nChar == '\r' ) {
		GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM( IDC_ADD_CASE, BN_CLICKED ), 0 );
	}
	else {
		CEdit::OnChar( nChar, nRepCnt, nFlags );
	}
}


// ============================================================================
//		CCasesOpts
// ============================================================================

IMPLEMENT_DYNAMIC(CCasesOpts, CDialog)

CCasesOpts::CCasesOpts(
	const COptions&	inOptions,
	bool			inReadOnly /*= false*/ )
	:	CDialog()
	,	m_bReadOnly( inReadOnly )
	,	m_CaseListCtrl( m_Options.GetCases() )
	,	m_nLastIndex( 1 )
	,	m_ptCaseListDelta( 0, 0 )
	,	m_nButtonTopDelta( 0 )
	,	m_nEditLeft( 0 )
	,	m_nAddLeft( 0 )
	,	m_nDeleteLeft( 0 )
	,	m_nImportLeft( 0 )
	,	m_nExportLeft( 0 )
	,	m_nSliceStatusLeft( 0 )
{
	m_Options.Copy( inOptions );
}


// ----------------------------------------------------------------------------
//		EditCase
// ----------------------------------------------------------------------------

void
CCasesOpts::EditCase()
{
	int	nItem( m_CaseListCtrl.GetSelectionMark() );
	if ( nItem < 0 ) return;

	COptions::COptionsCases&	CaseList( m_Options.GetCases() );

	CCase		oldCase( CaseList.GetAt( nItem ) );
	CCaseDlg	CaseDlg( oldCase );
	if ( CaseDlg.DoModal() == IDOK ) {
		CaseList.Update( nItem, CaseDlg.GetCase() );
	}
	Refresh();
}


// ----------------------------------------------------------------------------
//		EnableButtons
// ----------------------------------------------------------------------------

void
CCasesOpts::EnableButtons()
{
	int	nButtonIds[] = {
		IDC_EDIT_CASE,
		IDC_DELETE_CASE,
		IDC_EXPORT_ADDRESS
	};

	int		nItem( m_CaseListCtrl.GetSelectionMark() );
	BOOL	bEnable( nItem >= 0 );
	for ( size_t i = 0; i < COUNTOF( nButtonIds ); i++ ) {
		CButton*	pWnd( static_cast<CButton*>( GetDlgItem( nButtonIds[i] ) ) );
		if ( pWnd && pWnd->GetSafeHwnd() ) {
			pWnd->EnableWindow( bEnable );
			if ( bEnable && (i == 0) ) pWnd->SetFocus();
		}
	}
}


// ----------------------------------------------------------------------------
//		FindLastIndex
// ----------------------------------------------------------------------------

size_t
CCasesOpts::FindLastIndex(
	const CPeekString&	inName )
{
	size_t				nLastIndex( 0 );
	const CCaseList&	aCases( m_Options.GetCases().GetList() );
	size_t				dwCount( aCases.GetCount() );
	for ( size_t i = 0; i < dwCount; i++ ) {
		const CCase&	Case( aCases[i] );

		const CPeekString&	strName( Case.GetName() );
		int					nOffset( strName.Find( inName, 0 ) );
		if ( nOffset >= 0 ) {
			ptrdiff_t	nCount( strName.GetLength() - nOffset - inName.GetLength() );
			if ( nCount > 0 ) {
				size_t	nValue( _ttol( strName.Right( nCount ) ) );
				if ( nValue >= nLastIndex ) {
					nLastIndex = nValue + 1;
				}
			}
		}
	}
	return nLastIndex;
}


// ----------------------------------------------------------------------------
//		SetLastIndex
// ----------------------------------------------------------------------------

void
CCasesOpts::SetLastIndex()
{
	size_t	nLastIndex( FindLastIndex( CCase::GetDefaultName() ) );
	if ( nLastIndex >= m_nLastIndex ) {
		m_nLastIndex = nLastIndex;
	}
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CCasesOpts::DoDataExchange(
	CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	if ( pDX->m_bSaveAndValidate ) {
		m_CaseListCtrl.Term();
	}
}


// ----------------------------------------------------------------------------
//		OnInitDialog
// ----------------------------------------------------------------------------

BOOL
CCasesOpts::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	EnableToolTips( TRUE );

	m_CaseListCtrl.SubclassDlgItem( IDC_CASE_LIST, this );
	m_CaseListCtrl.SetExtendedStyle(
		m_CaseListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES );
	m_CaseListCtrl.ModifyStyle( 0, WS_CLIPCHILDREN );
	m_CaseListCtrl.Init();

	CFont*	pDlgFont( m_CaseListCtrl.GetFont() );
	m_CaseListCtrl.SetFont( pDlgFont, FALSE );

	CDialog::OnInitDialog();

	CRect	rcWindow;
	GetWindowRect( &rcWindow );
	{
		CRect	rc;
		m_CaseListCtrl.GetWindowRect( &rc );
		m_ptCaseListDelta.x = rcWindow.Width() - rc.Width();
		m_ptCaseListDelta.y = rcWindow.Height() - rc.Height();
	}

	{
		CWnd*	pEdit( GetDlgItem( IDC_EDIT_CASE ) );
		if ( pEdit ) {
			if ( IsReadOnly() ) {
				pEdit->ShowWindow( SW_HIDE );
			}
			else {
				CRect	rc;
				pEdit->GetWindowRect( &rc );
				m_nButtonTopDelta = rcWindow.bottom - rc.top + 1;
				ScreenToClient( &rc );
				m_nEditLeft = rc.left;
			}
		}
	}

	{
		CWnd*	pAdd( GetDlgItem( IDC_ADD_CASE ) );
		if ( pAdd ) {
			if ( IsReadOnly() ) {
				pAdd->ShowWindow( SW_HIDE );
			}
			else {
				CRect	rc;
				pAdd->GetWindowRect( &rc );
				ScreenToClient( &rc );
				m_nAddLeft = rc.left;
			}
		}
	}

	{
		CWnd*	pDelete( GetDlgItem( IDC_DELETE_CASE ) );
		if ( pDelete ) {
			if ( IsReadOnly() ) {
				pDelete->ShowWindow( SW_HIDE );
			}
			else {
				CRect	rc;
				pDelete->GetWindowRect( &rc );
				ScreenToClient( &rc );
				m_nDeleteLeft = rc.left;
			}
		}
	}

	{
		CWnd*	pImport( GetDlgItem( IDC_IMPORT_ADDRESS ) );
		if ( pImport ) {
			if ( IsReadOnly() ) {
				pImport->ShowWindow( SW_HIDE );
			}
			else {
				CRect	rc;
				pImport->GetWindowRect( &rc );
				ScreenToClient( &rc );
				m_nImportLeft = rc.left;
			}
		}
	}

	{
		CWnd*	pExport( GetDlgItem( IDC_EXPORT_ADDRESS ) );
		if ( pExport ) {
			CRect	rc;
			pExport->GetWindowRect( &rc );
			ScreenToClient( &rc );
			m_nExportLeft = rc.left;
		}
	}

	{
		CWnd*	pSliceStatus( GetDlgItem( IDC_SLICE_STATUS ) );
		if ( pSliceStatus ) {
			CRect	rc;
			pSliceStatus->GetWindowRect( &rc );
			ScreenToClient( &rc );
			m_nSliceStatusLeft = rc.left;
		}
	}

	Refresh();

	SetTimer( 1, 100, NULL );

#ifdef _DEBUG
	int	nIds[4] = {
		IDC_TIME_NOW,
		IDC_TIME_ONE_DAY,
		IDC_TIME_ONE_WEEK,
		IDC_TIME_ONE_MONTH
	};
	for ( int i = 0; i < 4; i++ ) {
		CWnd*	pWnd( GetDlgItem( nIds[i] ) );
		if ( pWnd ) pWnd->ShowWindow( SW_NORMAL );
	}
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// ----------------------------------------------------------------------------
//		UpdateStatus
// ----------------------------------------------------------------------------

void
CCasesOpts::UpdateStatus()
{
	m_CaseListCtrl.UpdateStatus();

	CWnd*	pSliceStatus( GetDlgItem( IDC_SLICE_STATUS ) );
	if ( pSliceStatus ) {
		CString	strStatus;
		int		nStatusId( (m_Options.IsSlicePackets()) ? IDS_SLICE_ON : IDS_SLICE_OFF );
		strStatus.LoadString( nStatusId );

		CString	strCurrent;
		pSliceStatus->GetWindowText( strCurrent );
		if ( !strCurrent.IsEmpty() && (strStatus != strCurrent) ) {
			pSliceStatus->SetWindowText( strStatus );
		}
	}
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCasesOpts, CDialog)
	ON_BN_CLICKED(IDC_EDIT_CASE, OnBnClickedEditCase)
	ON_BN_CLICKED(IDC_ADD_CASE, OnBnClickedAddCase)
	ON_BN_CLICKED(IDC_DELETE_CASE, OnBnClickedDeleteCase)
	ON_BN_CLICKED(IDC_EXPORT_ADDRESS, OnBnClickedExportAddress)
	ON_BN_CLICKED(IDC_IMPORT_ADDRESS, OnBnClickedImportAddress)
	ON_NOTIFY(NM_CLICK, IDC_CASE_LIST, &CCasesOpts::OnNMClickCaseList)
	ON_NOTIFY(NM_DBLCLK, IDC_CASE_LIST, OnNotifyDoubleClickedCaseList)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedEditCase
// ----------------------------------------------------------------------------

void
CCasesOpts::OnBnClickedEditCase()
{
	EditCase();
}


// ----------------------------------------------------------------------------
//		OnBnClickedAddCase
// ----------------------------------------------------------------------------

void
CCasesOpts::OnBnClickedAddCase()
{
	CCase		NewCase( m_nLastIndex );
	CCaseDlg	AddCaseDlg( NewCase, &m_Options.GetCases() );
	if ( AddCaseDlg.DoModal() == IDOK ) {
		const CCase&	Case( AddCaseDlg.GetCase() );

		m_Options.GetCases().Add( Case );

		int nIndex( m_CaseListCtrl.GetItemCount() );
		m_CaseListCtrl.InsertItem( nIndex, Case.GetName() );

		m_CaseListCtrl.SetCheck( nIndex, Case.IsEnabled() );
	}
	Refresh();
}


// ----------------------------------------------------------------------------
//		OnBnClickedDeleteAddress
// ----------------------------------------------------------------------------

void
CCasesOpts::OnBnClickedDeleteCase()
{
	int	nItem( m_CaseListCtrl.GetSelectionMark() );
	if ( nItem < 0 ) return;

	COptions::COptionsCases&	CaseList( m_Options.GetCases() );
	CaseList.RemoveAt( nItem );

	m_CaseListCtrl.DeleteItem( nItem );
	m_CaseListCtrl.SetSelectionMark( -1 );
	Refresh();
}


// ----------------------------------------------------------------------------
//		OnBnClickedImportAddress
// ----------------------------------------------------------------------------

void
CCasesOpts::OnBnClickedImportAddress()
{
	CFileDialog	FileDialog( TRUE );
	FileDialog.m_ofn.lpstrFilter = L"XML File\0*.xml\0\0";
	if ( FileDialog.DoModal() != IDOK ) {
		return;
	}
	CString	strFileName( FileDialog.GetPathName() );

	CListCtrl*	pList( static_cast<CListCtrl*>( GetDlgItem( IDC_CASE_LIST ) ) );
	if ( pList == NULL ) return;

	try {
		CCaseList			ayCases;
		CPeekDataModeler	dmOptions( strFileName, ConfigTags::kRootName );
		CPeekDataElement	deCases( ConfigTags::kCaseList, dmOptions );
		if ( ayCases.Model( deCases) ) {
			const size_t	nCount( ayCases.GetCount() );
			for ( size_t i( 0 ); i < nCount; i++ ) {
				CCase&			cCase( ayCases[i] );
				CPeekString		strCaseName( cCase.GetName() );
				strCaseName = strCaseName.TrimRight( _T( "0123456789" ) );

				size_t	nLastIndex( FindLastIndex( strCaseName ) );
				if ( nLastIndex > 0 ) {
					CPeekString	strIndex;
					IntegerToString( nLastIndex, strIndex );
					const CPeekString*	ayStrs[] = {
						&strCaseName,
						Tags::kxSpace.RefW(),
						&strIndex,
						NULL
					};
					strCaseName = FastCat( ayStrs );
				}
				cCase.SetName( strCaseName );

				m_Options.GetCases().Add( cCase );

				int nIndex( pList->GetItemCount() );
				pList->InsertItem( nIndex, cCase.GetName() );
				pList->SetCheck( nIndex, cCase.IsEnabled() );

				if ( m_Options.GetCases().GetCount() >= CCaseList::GetMaxSize() ) {
					break;
				}
			}
		}
	}
	catch ( ... ) {
		// Catch and squelch.
	}

	Refresh();
}


// ----------------------------------------------------------------------------
//		OnBnClickedExportAddress
// ----------------------------------------------------------------------------

void
CCasesOpts::OnBnClickedExportAddress()
{
	CListCtrl*	pList( static_cast<CListCtrl*>( GetDlgItem( IDC_CASE_LIST ) ) );
	ASSERT( pList );
	if ( pList == NULL ) return;

	UINT	nCount( pList->GetSelectedCount() );
	if ( nCount == 0 ) return;

	try {
		CCaseList	ayCases;
		CPeekString	strCaseName;
		int			nItem( -1 );
		for ( UINT i = 0; i < nCount; i++ ) {
			nItem = pList->GetNextItem( nItem, LVNI_SELECTED );
			ASSERT( nItem != -1 );

			const CCase&	Case( m_Options.GetCases().GetAt( nItem ) );
			ayCases.Add( Case );
			if ( strCaseName.IsEmpty() ) {
				strCaseName = Case.GetName();
			}
		}

		// Save XML file
		CFileDialog	FileDialog( FALSE );
		CPeekString	strTempName( CFileEx::LegalizeFileName( strCaseName ) );
		wchar_t		szTempName[MAX_PATH + 1];
		wcscpy_s( szTempName, MAX_PATH, strTempName );
		strTempName.reserve( MAX_PATH + 1 );
		FileDialog.GetOFN().nMaxFile = MAX_PATH + 1;
		FileDialog.GetOFN().lpstrFile = szTempName;
		FileDialog.GetOFN().lpstrInitialDir = strCaseName;
		FileDialog.GetOFN().lpstrFilter = L"XML File\0*.xml\0\0";

		if ( FileDialog.DoModal() != IDOK ) {
			return;
		}

		CPeekString	strFileName( FileDialog.GetPathName() );
		CPeekString	strExt( FileDialog.GetFileExt() );
		if ( strExt.IsEmpty() && (strFileName.Right( 1 ) != L".") ) {
			strFileName += L".xml";
		}

		CPeekDataModeler	dmOptions( ConfigTags::kRootName );
		CPeekDataElement	deCases( ConfigTags::kCaseList, dmOptions );
		if ( ayCases.Model( deCases ) ) {
			CPeekPersistFile	spFile = dmOptions.GetXml();
			if ( spFile ) {
				spFile->Save( strFileName, FALSE );
			}
		}
	}
	catch ( ... ) {
	}
}


// ----------------------------------------------------------------------------
//		OnNMClickCaseList
// ----------------------------------------------------------------------------

void
CCasesOpts::OnNMClickCaseList(
	NMHDR*		/*pNMHDR*/,
	LRESULT*	pResult )
{
	*pResult = 0;

	CPoint pt;
	GetCursorPos( &pt );
	m_CaseListCtrl.ScreenToClient( &pt );

	UINT	nFlags( 0 );
	int		nIndex( m_CaseListCtrl.HitTest( pt, &nFlags ) );
	if ( nIndex >= 0 ) {
		CRect	rc;
		m_CaseListCtrl.GetItemRect( nIndex, &rc, LVIR_BOUNDS );

		CRect	rcCheck;
		m_CaseListCtrl.GetCheckboxRect( rc, rcCheck );

		if ( rcCheck.PtInRect( pt ) ) {
			CCase&	Case( const_cast<CCase&>( m_Options.GetCases().GetAt( nIndex ) ) );
			Case.SetEnabled( !Case.IsEnabled() );
			m_CaseListCtrl.RedrawItems( nIndex, nIndex );
		}
	}
	else {
		m_CaseListCtrl.SetSelectionMark( -1 );
	}
	Refresh( false );
}


// ----------------------------------------------------------------------------
//		OnNotifyDoubleClickedCaseList
// ----------------------------------------------------------------------------

void
CCasesOpts::OnNotifyDoubleClickedCaseList(
	NMHDR*		pNMHDR,
	LRESULT*	pResult )
{
	*pResult = 0;

	if ( (pNMHDR == NULL) || (pNMHDR->idFrom != IDC_CASE_LIST) ) return;

	EditCase();
}


// ----------------------------------------------------------------------------
//		OnSize
// ----------------------------------------------------------------------------

void
CCasesOpts::OnSize(
	UINT	nType,
	int		cx,
	int		cy )
{
	CDialog::OnSize( nType, cx, cy );

	if ( m_CaseListCtrl.GetSafeHwnd() != NULL ) {
		CRect	rc;
		m_CaseListCtrl.GetWindowRect( &rc );
		ScreenToClient( &rc );

		rc.right = cx;
		rc.bottom = cy - m_ptCaseListDelta.y + rc.top - 1;

		m_CaseListCtrl.MoveWindow( &rc );
	}

	CSize	sizeButton( 0, 0 );
	{
		CWnd*	pEdit( GetDlgItem( IDC_EDIT_CASE ) );
		if ( pEdit ) {
			CRect	rc;
			pEdit->GetWindowRect( &rc );
			sizeButton = rc.Size();
			rc.MoveToXY( m_nEditLeft, (cy - m_nButtonTopDelta) );
			pEdit->MoveWindow( &rc );
		}
	}

	{
		CWnd*	pAdd( GetDlgItem( IDC_ADD_CASE ) );
		if ( pAdd ) {
			CRect	rc;
			pAdd->GetWindowRect( &rc );
			sizeButton = rc.Size();
			rc.MoveToXY( m_nAddLeft, (cy - m_nButtonTopDelta) );
			pAdd->MoveWindow( &rc );
		}
	}

	{
		CWnd*	pDelete( GetDlgItem( IDC_DELETE_CASE ) );
		if ( pDelete ) {
			CRect	rc;
			pDelete->GetWindowRect( &rc );
			sizeButton = rc.Size();
			rc.MoveToXY( m_nDeleteLeft, (cy - m_nButtonTopDelta) );
			pDelete->MoveWindow( &rc );
		}
	}

	{
		CWnd*	pImport( GetDlgItem( IDC_IMPORT_ADDRESS ) );
		if ( pImport ) {
			CRect	rc;
			pImport->GetWindowRect( &rc );
			sizeButton = rc.Size();
			rc.MoveToXY( m_nImportLeft, (cy - m_nButtonTopDelta) );
			pImport->MoveWindow( &rc );
		}
	}

	{
		CWnd*	pExport( GetDlgItem( IDC_EXPORT_ADDRESS ) );
		if ( pExport ) {
			CRect	rc;
			pExport->GetWindowRect( &rc );
			sizeButton = rc.Size();
			rc.MoveToXY( m_nExportLeft, (cy - m_nButtonTopDelta) );
			pExport->MoveWindow( &rc );
		}
	}

	{
		CWnd*	pSliceStatus( GetDlgItem( IDC_SLICE_STATUS ) );
		if ( pSliceStatus ) {
			CRect	rc;
			pSliceStatus->GetWindowRect( &rc );

			CSize	sizeSliceStatus( rc.Size() );
			int	nTop( (cy - m_nButtonTopDelta) + (sizeButton.cy / 2) - (sizeSliceStatus.cy / 2) );
			
			rc.MoveToXY( m_nSliceStatusLeft, nTop );
			pSliceStatus->MoveWindow( &rc );
		}
	}
}


// ----------------------------------------------------------------------------
//		OnTimer
// ----------------------------------------------------------------------------

void
CCasesOpts::OnTimer(UINT_PTR nIDEvent)
{
	UpdateStatus();
	CDialog::OnTimer( nIDEvent );
}


// ----------------------------------------------------------------------------
//		OnToolTipNotify
// ----------------------------------------------------------------------------

BOOL
CCasesOpts::OnToolTipNotify(
	UINT		/* nId */,
	NMHDR*		pNMHDR,
	LRESULT*	pResult )
{
	//
	// Note: This implementation is taken straight from the CFrameWnd::OnToolTipText()
	// with minor modifications.
	ASSERT( (pNMHDR->code == TTN_NEEDTEXTA) || (pNMHDR->code == TTN_NEEDTEXTW) );

	// Need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA*	pTTTA( reinterpret_cast<TOOLTIPTEXTA*>( pNMHDR ) );
	TOOLTIPTEXTW*	pTTTW( reinterpret_cast<TOOLTIPTEXTW*>( pNMHDR ) );
	CString			strTipText;

	DWORD	dwPos( ::GetMessagePos() );

	// Don't use LOWORD/HIWORD Macros, as per GetMessagePos.
	CPoint	ptCurrent( GET_X_LPARAM( dwPos ), GET_Y_LPARAM( dwPos ) );

	// Clear the sub-item text the user clicked on.
	CPoint	ptTip( ptCurrent );
	m_CaseListCtrl.ScreenToClient( &ptTip );

	LVHITTESTINFO lvHitTestInfo;
	lvHitTestInfo.pt = ptTip;
	int	nItem( m_CaseListCtrl.SubItemHitTest( &lvHitTestInfo ) );
	if ( nItem < 0 ) return FALSE;

	const CCase&				Case( m_Options.GetCases().GetAt( nItem ) );
	//const CCaseSettings&		Settings( Case.GetSettings() );
	//const CTargetAddressList&	AddressList( Case.GetAddressList() );

	strTipText = Case.GetColumnText( lvHitTestInfo.iSubItem );

	if ( pNMHDR->code == TTN_NEEDTEXTA ) {
		_wcstombsz( pTTTA->szText, strTipText, COUNTOF( pTTTA->szText ) );
	}
	else {
		lstrcpyn( pTTTW->szText, strTipText, COUNTOF( pTTTW->szText ) );
	}

	*pResult = 0;

	// Bring the tool-tip window above other popup windows.
	CPoint	ptToolTip( ptCurrent );
	ptToolTip.x += 16;
	::SetWindowPos( pNMHDR->hwndFrom, HWND_TOP, ptToolTip.x, ptToolTip.y, 0, 0,
		(SWP_NOACTIVATE | SWP_NOSIZE | /* SWP_NOMOVE | */ SWP_NOOWNERZORDER) );

	return TRUE;
}


// ============================================================================
//		COptionsTreeCtrl
// ============================================================================

// ----------------------------------------------------------------------------
//		DeepIterate
// ----------------------------------------------------------------------------

void
COptionsTreeCtrl::DeepIterate(
	OTCFunc*	inFunc,
	DWORD_PTR	inArgument )
{
	if ( inFunc == NULL ) return;
	if ( GetSafeHwnd() == NULL ) return;

	HTREEITEM	hTreeItem( GetRootItem() );
	while ( hTreeItem != NULL ) {
		HTREEITEM	hChildItem( GetChildItem( hTreeItem ) );
		while ( hChildItem != NULL ) {
			DWORD_PTR	dwChildItemData( GetItemData( hChildItem ) );
			(*inFunc)( dwChildItemData, inArgument );
			hChildItem = GetNextSiblingItem( hChildItem );
		}

		DWORD_PTR	dwItemData( GetItemData( hTreeItem ) );
		(*inFunc)( dwItemData, inArgument );
		hTreeItem = GetNextSiblingItem( hTreeItem );
	}
}

#endif // HE_WIN32
