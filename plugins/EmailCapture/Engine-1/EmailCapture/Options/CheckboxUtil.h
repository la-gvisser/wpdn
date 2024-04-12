// ============================================================================
//	CheckboxUtil.h
// ============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

// Linker: Additional Dependencies: uxtheme.lib
#include <uxtheme.h>
#include <TmSchema.h>


// ============================================================================
//		CheckboxUtil
// ============================================================================

namespace CheckboxUtil
{
const int	kCenterHorizontal( 1 );
const int	kCenterVertical( 2 );
const int	kCenter( 3 );

__declspec(selectany) DWORD	s_dwVerComCtl32 = 0;
__declspec(selectany) int	s_nIsThemingSupported = -1;


// ----------------------------------------------------------------------------
//		IsThemingSupported
// ----------------------------------------------------------------------------

static bool
IsThemingSupported()
{
	if ( s_nIsThemingSupported == -1 ) {
		HMODULE	hThemeDLL = ::LoadLibrary( _T( "uxtheme.dll" ) );
		s_nIsThemingSupported = (hThemeDLL != NULL) ? 1 : 0;
		if ( hThemeDLL != NULL ) {
			::FreeLibrary( hThemeDLL );
		}
	}

	ASSERT( s_nIsThemingSupported != -1 );
	return (s_nIsThemingSupported == 1);
}


// ============================================================================
//		CTheme
// ============================================================================

class CTheme
{
private:
	HTHEME		m_hTheme;

public:
	CTheme()
		:	m_hTheme( NULL ) {
		IsThemingSupported();
	}

	~CTheme() {
		CloseThemeData();
	}

	HRESULT CloseThemeData() {
		HRESULT hRet = S_FALSE;
		if ( m_hTheme != NULL ) {
			hRet = ::CloseThemeData( m_hTheme );
			if ( SUCCEEDED( hRet ) ) {
				m_hTheme = NULL;
			}
		}
		return hRet;
	}

	HRESULT DrawThemeBackground(
			HDC hDC, int nPartID, int nStateID, LPCRECT pRect, LPCRECT pClipRect = NULL ) {
		ASSERT( m_hTheme != NULL );
		return ::DrawThemeBackground( m_hTheme, hDC, nPartID, nStateID, pRect, pClipRect );
	}

	HRESULT GetThemePartSize(
			HDC hDC, int nPartID, int nStateID, LPRECT pRect,
			enum THEMESIZE eSize, LPSIZE pSize ) const {
		ASSERT( m_hTheme != NULL );
		return ::GetThemePartSize( m_hTheme, hDC, nPartID, nStateID, pRect, eSize, pSize );
	}

	bool IsThemeNull() const { return (m_hTheme == NULL); }

	HTHEME OpenThemeData( HWND hWnd, LPCWSTR pszClassList ) {
		ASSERT( m_hTheme == NULL );

		if ( IsThemingSupported() && ::IsAppThemed() && ::IsThemeActive() ) {
			m_hTheme = ::OpenThemeData( hWnd, pszClassList );
		}

		return m_hTheme;
	}
};


// ----------------------------------------------------------------------------
//		CenterRectInRect
// ----------------------------------------------------------------------------

static void
CenterRectInRect(
	CRect&			outRect,
	const CRect&	inRect,
	int				inFlags = kCenter )
{
	CPoint	theOffset;

	// Calculate the offset between the two centers.
	theOffset.x = (inRect.left + inRect.Width() / 2) -
		(outRect.left + outRect.Width() / 2);
	theOffset.y = (inRect.top + inRect.Height() / 2) -
		(outRect.top + outRect.Height() / 2);

	// Offset the rect.
	if ( inFlags & kCenterHorizontal ) {
		outRect.left += theOffset.x;
		outRect.right += theOffset.x;
	}
	else {
		outRect.left += inRect.left;
		outRect.right += inRect.left;
	}
	if ( inFlags & kCenterVertical ) {
		outRect.top += theOffset.y;
		outRect.bottom += theOffset.y;
	}
	else {
		outRect.top += inRect.top;
		outRect.bottom += inRect.top;
	}
}


// ----------------------------------------------------------------------------
//		DrawCheckbox
// ----------------------------------------------------------------------------

static void
DrawCheckbox(
	CDC&			inDC,
	const CRect&	inRect,
	bool			inEnabled,
	int 			inValue )
{
	int	nDCSave = inDC.SaveDC();

	CTheme	theme;
	theme.OpenThemeData( inDC.GetWindow()->GetSafeHwnd(), L"button" );

	if ( !theme.IsThemeNull() ) {
		int	nState = 0;
		if ( inEnabled ) {
			nState = (inValue > 0) ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;
		}
		else {
			nState = (inValue > 0) ? CBS_CHECKEDDISABLED : CBS_UNCHECKEDDISABLED;
		}
		theme.DrawThemeBackground( inDC.GetSafeHdc(), BP_CHECKBOX, nState, &inRect );
	}
	else {
		// Fill the box background.
		if ( inEnabled ) {
			if ( inValue == 2 ) {
				// Fill with a light gray color for a mixed value checkbox.
				inDC.FillSolidRect( inRect, RGB( 192, 192, 192 ) );
			}
			else {
				// Fill the checkbox interior with white.
				inDC.FillSolidRect( inRect, RGB( 255, 255, 255) );
			}
		}
		else {
			// Fill with a light gray color for a disabled checkbox.
			inDC.FillSolidRect( inRect, RGB( 192, 192, 192 ) );
		}

		// Frame the box.
		CBrush	theBrush( RGB( 128, 128, 128 ) );
		inDC.FrameRect( inRect, &theBrush );

		if ( inValue != 0 ) {
			// Setup the check pen.
			COLORREF	theCheckColor = inEnabled ? RGB( 0, 0, 0 ) : RGB( 128, 128, 128 );
			CPen		theCheckPen( PS_SOLID, 1, theCheckColor );
			CPen*		pOldPen = inDC.SelectObject( &theCheckPen );

			inDC.SetBkMode( OPAQUE );
			inDC.SetBkColor( theCheckColor );
			inDC.SetTextColor( theCheckColor );

			// Draw the check.
			inDC.MoveTo( (inRect.left + 2), (inRect.top + 4) );
			inDC.LineTo( (inRect.left + 4), (inRect.top + 6) );
			inDC.LineTo( (inRect.left + 9), (inRect.top + 1) );
			inDC.MoveTo( (inRect.left + 2), (inRect.top + 5) );
			inDC.LineTo( (inRect.left + 4), (inRect.top + 7) );
			inDC.LineTo( (inRect.left + 9), (inRect.top + 2) );
			inDC.MoveTo( (inRect.left + 2), (inRect.top + 6) );
			inDC.LineTo( (inRect.left + 4), (inRect.top + 8) );
			inDC.LineTo( (inRect.left + 9), (inRect.top + 3) );

			// Restore the pen.
			inDC.SelectObject( pOldPen );
		}
	}

	inDC.RestoreDC( nDCSave );
}

} // namespace CheckboxUtil
