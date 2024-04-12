// =============================================================================
//	hevariant.h
// =============================================================================
//	Copyright (c) 2005-2015 Savvius, Inc. All rights reserved.

#ifndef	HEVARIANT_H
#define	HEVARIANT_H

#include "heid.h"
#include "hestr.h"
#include "heunk.h"

namespace Helium
{

typedef SInt16 HEVARBOOL;
#define HE_VAR_TRUE			((Helium::HEVARBOOL)-1)
#define HE_VAR_FALSE		((Helium::HEVARBOOL)0)

enum HEVARTYPEENUM
{
	HE_VT_EMPTY				= 0,
	HE_VT_NULL				= 1,
	HE_VT_I2				= 2,
	HE_VT_I4				= 3,
	HE_VT_R4				= 4,
	HE_VT_R8				= 5,
	HE_VT_CY				= 6,
	HE_VT_DATE				= 7,
	HE_VT_BSTR				= 8,
	HE_VT_DISPATCH			= 9,
	HE_VT_ERROR				= 10,
	HE_VT_BOOL				= 11,
	HE_VT_VARIANT			= 12,
	HE_VT_UNKNOWN			= 13,
	HE_VT_DECIMAL			= 14,
	HE_VT_I1				= 16,
	HE_VT_UI1				= 17,
	HE_VT_UI2				= 18,
	HE_VT_UI4				= 19,
	HE_VT_I8				= 20,
	HE_VT_UI8				= 21,
	HE_VT_INT				= 22,
	HE_VT_UINT				= 23,
	HE_VT_VOID				= 24,
	HE_VT_HERESULT			= 25,
	HE_VT_PTR				= 26,
	HE_VT_SAFEARRAY			= 27,
	HE_VT_CARRAY			= 28,
	HE_VT_USERDEFINED		= 29,
	HE_VT_LPSTR				= 30,
	HE_VT_LPWSTR			= 31,
	HE_VT_RECORD			= 36,
	HE_VT_INT_PTR			= 37,
	HE_VT_UINT_PTR			= 38,
	HE_VT_FILETIME			= 64,
	HE_VT_BLOB				= 65,
	HE_VT_STREAM			= 66,
	HE_VT_STORAGE			= 67,
	HE_VT_STREAMED_OBJECT	= 68,
	HE_VT_STORED_OBJECT		= 69,
	HE_VT_BLOB_OBJECT		= 70,
	HE_VT_CF				= 71,
	HE_VT_CLSID				= 72,
	HE_VT_VERSIONED_STREAM	= 73,
	HE_VT_BSTR_BLOB			= 0x0fff,
	HE_VT_VECTOR			= 0x1000,
	HE_VT_ARRAY				= 0x2000,
	HE_VT_BYREF				= 0x4000,
	HE_VT_RESERVED			= 0x8000,
	HE_VT_ILLEGAL			= 0xffff,
	HE_VT_ILLEGALMASKED		= 0x0fff,
	HE_VT_TYPEMASK			= 0x0fff
};

typedef UInt16 HEVARTYPE;

/// Just like Win32 VARIANT. Must be 16 bytes.
#include "hepushpack.h"
struct HEVARIANT
{
	HEVARTYPE	vt;
	UInt16		wReserved1;
	UInt16		wReserved2;
	UInt16		wReserved3;
	union 
	{
		SInt16			iVal;			// HE_VT_I2
		SInt32			lVal;			// HE_VT_I4
		float			fltVal;			// HE_VT_R4
		double			dblVal;			// HE_VT_R8
		double			date;			// HE_VT_DATE
		HEBSTR			bstrVal;		// HE_VT_BSTR
		HEVARBOOL		boolVal;		// HE_VT_BOOL
		IHeUnknown*		punkVal;		// HE_VT_UNKNOWN
		char			cVal;			// HE_VT_I1
		UInt8			bVal;			// HE_VT_UI1
		UInt16			uiVal;			// HE_VT_UI2
		UInt32			ulVal;			// HE_VT_UI4
		SInt64			llVal;			// HE_VT_I8
		UInt64			ullVal;			// HE_VT_UI8
		int				intVal;			// HE_VT_INT
		unsigned int	uintVal;		// HE_VT_UINT
		HeResult		hrVal;			// HE_VT_ERROR or HE_VT_HERESULT
		UInt8*			pbVal;			// HE_VT_BYREF|HE_VT_UI1
		SInt16*			piVal;			// HE_VT_BYREF|HE_VT_I2
		SInt32*			plVal;			// HE_VT_BYREF|HE_VT_I4
		SInt64*			pllVal;			// HE_VT_BYREF|HE_VT_I8
		float*			pfltVal;		// HE_VT_BYREF|HE_VT_R4
		double*			pdblVal;		// HE_VT_BYREF|HE_VT_R8
		int*			pboolVal;		// HE_VT_BYREF|HE_VT_BOOL
		HEBSTR*			pbstrVal;		// HE_VT_BYREF|HE_VT_BSTR
		IHeUnknown**	ppunkVal;		// HE_VT_BYREF|HE_VT_UNKNOWN
		HEVARIANT*		pvarVal;		// HE_VT_BYREF|HE_VT_VARIANT
		void*			byref;			// HE_VT_BYREF (Generic ByRef)
		char*			pcVal;			// HE_VT_BYREF|HE_VT_I1
		UInt16*			puiVal;			// HE_VT_BYREF|HE_VT_UI2
		UInt32*			pulVal;			// HE_VT_BYREF|HE_VT_UI4
		UInt64*			pullVal;		// HE_VT_BYREF|HE_VT_UI8
		int*			pintVal;		// HE_VT_BYREF|HE_VT_INT
		unsigned int*	puintVal;		// HE_VT_BYREF|HE_VT_UINT
		char*			pszVal;			// HE_VT_LPSTR
		wchar_t*		pwszVal;		// HE_VT_LPWSTR
		HeID*			puuid;			// HE_VT_CLSID
	} HE_PACK_STRUCT;
} HE_PACK_STRUCT;
#include "hepoppack.h"

} /* namespace Helium	*/

#endif /* HEVARIANT_H */
