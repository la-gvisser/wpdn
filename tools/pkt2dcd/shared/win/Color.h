// =============================================================================
//	Color.h
// =============================================================================
//	Copyright (c) 2002-2014 WildPackets, Inc. All rights reserved.

#pragma once

namespace WPColor
{
	const COLORREF BLACK   = RGB( 0x00, 0x00, 0x00 );
	const COLORREF WHITE   = RGB( 0xFF, 0xFF, 0xFF );
	const COLORREF BLUISH  = RGB( 0xC0, 0xC0, 0xFF );
	const COLORREF YELLOW  = RGB( 0xFF, 0xFF, 0x00 );
	const COLORREF LT_BLUE = RGB( 0xF2, 0xF8, 0xFF );
	const COLORREF BLUE    = RGB( 0x4A, 0x83, 0xC6 );

	COLORREF Chart( const int iIndex );
}
