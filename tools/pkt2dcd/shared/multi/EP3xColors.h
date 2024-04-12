// ============================================================================
//	EP3xColors.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Copyright (c) AG Group, Inc. 1999-2000. All rights reserved.
//	Contains COLORREFs which correspond to EPM 3.x's default color table.

#ifndef EP3XCOLORS_H
#define EP3XCOLORS_H

inline COLORREF
GetEP3xIndexedColor(
	UInt8	inColorIndex )
{
	static const COLORREF	s_ColorTable[] =
	{
#ifdef _WIN32
		RGB( 0x00, 0x40, 0x80 ),
		RGB( 0x00, 0x80, 0xFF ),
		RGB( 0x40, 0x00, 0x40 ),
		RGB( 0x40, 0x40, 0x80 ),
		RGB( 0x80, 0x00, 0x00 ),
		RGB( 0x80, 0x00, 0x80 ),
		RGB( 0xFF, 0x00, 0x00 ),
		RGB( 0x80, 0x80, 0x80 ),
		RGB( 0x40, 0x80, 0x80 ),
		RGB( 0x40, 0x00, 0x80 ),
		RGB( 0x40, 0x40, 0x00 ),
		RGB( 0x00, 0x00, 0x40 ),
		RGB( 0x00, 0x00, 0xFF ),
		RGB( 0x80, 0x00, 0xFF ),
		RGB( 0x80, 0x80, 0x00 ),
		RGB( 0x00, 0x80, 0x80 )
#else
		RGB( 0, 0, 0 ),
		RGB( 85, 0, 0 ),
		RGB( 85, 63, 0 ),
		RGB( 42, 95, 0 ),
		RGB( 0, 127, 0 ),
		RGB( 0, 127, 85 ),
		RGB( 42, 31, 85 ),
		RGB( 42, 0, 85 ),
		RGB( 85, 0, 85 ),
		RGB( 127, 63, 0 ),
		RGB( 127, 159, 0 ),
		RGB( 0, 159, 0 ),
		RGB( 0, 159, 85 ),
		RGB( 0, 128, 128 ),
		RGB( 0, 0, 128 ),
		RGB( 128, 0, 128 ),
		RGB( 127, 0, 85 ),
		RGB( 212, 127, 0 ),
		RGB( 0, 191, 0 ),
		RGB( 0, 191, 170 ),
		RGB( 0, 127, 170 ),
		RGB( 85, 0, 170 ),
		RGB( 212, 0, 170 ),
		RGB( 212, 0, 0 )
#endif
	};
	static const int	s_nColors = sizeof(s_ColorTable) / sizeof(COLORREF);

	return s_ColorTable[inColorIndex % s_nColors]; 
}

#endif
