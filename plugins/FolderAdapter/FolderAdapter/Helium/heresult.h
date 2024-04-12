// =============================================================================
//	heresult.h
// =============================================================================
//	Copyright (c) 2005-2014 WildPackets, Inc. All rights reserved.

#ifndef HERESULT_H
#define HERESULT_H

#include <errno.h>
#include "hestdint.h"

/// \typedef HeResult
/// \brief Helium result code. Similar to HRESULT or nsresult.
typedef SInt32 HeResult;

#define HERESULT_CONST(x)				static_cast<HeResult>(SINT32_CONST(x))

#define HE_S_OK							HERESULT_CONST(0)
#define	HE_S_FALSE						HERESULT_CONST(0x00000001)
#define	HE_E_UNEXPECTED					HERESULT_CONST(0x8000ffff)
#define	HE_E_PENDING					HERESULT_CONST(0x8000000A)
#define	HE_E_NOT_IMPLEMENTED			HERESULT_CONST(0x80004001)
#define	HE_E_NO_INTERFACE				HERESULT_CONST(0x80004002)
#define	HE_E_INVALID_POINTER			HERESULT_CONST(0x80004003)
#define	HE_E_NULL_POINTER				HE_E_INVALID_POINTER
#define	HE_E_ABORT						HERESULT_CONST(0x80004004)
#define	HE_E_FAIL						HERESULT_CONST(0x80004005)
#define	HE_E_NOT_AVAILABLE				HERESULT_CONST(0x80040111)
#define	HE_E_FACTORY_NOT_REGISTERED		HERESULT_CONST(0x80040154)
#define	HE_E_FACTORY_NOT_LOADED			HERESULT_CONST(0x800401f8)
#define HE_E_FILE_NOT_FOUND				HERESULT_CONST(0x80070002)
#define HE_E_PATH_NOT_FOUND				HERESULT_CONST(0x80070003)
#define	HE_E_ACCESS_DENIED				HERESULT_CONST(0x80070005)
#define HE_E_INVALID_HANDLE				HERESULT_CONST(0x80070006)
#define	HE_E_OUT_OF_MEMORY				HERESULT_CONST(0x8007000e)
#define	HE_E_WRITE_PROTECT				HERESULT_CONST(0x80070013)
#define	HE_E_EOF						HERESULT_CONST(0x80070026)
#define HE_E_FILE_EXISTS				HERESULT_CONST(0x80070050)
#define	HE_E_ILLEGAL_VALUE				HERESULT_CONST(0x80070057)
#define	HE_E_INVALID_ARG				HE_E_ILLEGAL_VALUE
#define	HE_E_DISK_FULL					HERESULT_CONST(0x80070070)
#define HE_E_INSUFFICIENT_BUFFER		HERESULT_CONST(0x8007007a)
#define HE_E_ILLEGAL_FILE_NAME			HERESULT_CONST(0x8007007b)
#define HE_E_CANCELLED					HERESULT_CONST(0x800704c7)
#define HE_E_TIMEOUT					HERESULT_CONST(0x800705b4)
#define HE_E_TYPE_MISMATCH				HERESULT_CONST(0x80020005)
#define HE_E_BADVARTYPE					HERESULT_CONST(0x80020008)
#define HE_E_OVERFLOW					HERESULT_CONST(0x8002000a)

/// \def HE_SUCCEEDED
/// \brief Corresponds to and compatible with SUCCEEDED().
#define HE_SUCCEEDED(_hr)				(static_cast<HeResult>(_hr) >= 0)

/// \def HE_FAILED
/// \brief Corresponds to and compatible with FAILED().
#define HE_FAILED(_hr)					(static_cast<HeResult>(_hr) < 0)

namespace Helium
{

/// \brief Create an HeResult from a Windows error code.
/// \param x The Windows error code.
inline HeResult HeResultFromWin32( const UInt32 e )
{
	HeResult	hr = static_cast<HeResult>(e);
	if ( hr > 0 )
	{
		switch ( hr )
		{
			// Convert error codes that don't properly map to
			// FACILITY_WIN32.
			case HERESULT_CONST(5):
				hr = HE_E_ACCESS_DENIED;
				break;
			case HERESULT_CONST(8):
				hr = HE_E_OUT_OF_MEMORY;
				break;
			default:
				hr = static_cast<HeResult>((e & 0x0000ffff) | 0x80070000);
				break;
		}
	}
	return hr;
}

/// \brief Create an HeResult from a standard library error code.
/// \param e The error code.
inline HeResult HeResultFromErrorCodeBase( const int e )
{
	return (e <= 0) ?
		static_cast<HeResult>(e) :
		static_cast<HeResult>(((e + 0x0200) & 0x0000ffff) | 0x80040000);
}

/// \brief Create an HeResult from a standard library error code, mapping
/// common codes.
/// \param e The error code.
inline HeResult HeResultFromErrorCode( const int e )
{
	// Map errno codes to well-known HeResults. Only includes values that can
	// successfully round-trip. For example, both EACCES and EPERM could
	// logically map to HE_E_ACCESS_DENIED, but HE_E_ACCESS_DENIED cannot
	// be mapped back to both. In such cases, the closest match is used.
	switch ( e )
	{
		case ENOENT:
			return HE_E_FILE_NOT_FOUND;
		case ENOMEM:
			return HE_E_OUT_OF_MEMORY;
		case EACCES:
			return HE_E_ACCESS_DENIED;
		case ENOTDIR:
			return HE_E_PATH_NOT_FOUND;
		case ENOSPC:
			return HE_E_DISK_FULL;
		default:
			return HeResultFromErrorCodeBase( e );
	}
}

/// \brief Get a standard library error code from an HeResult.
/// \param hr The HeResult code to convert.
inline int ErrorCodeFromHeResult( const HeResult hr )
{
	// Map well-known HeResults back to errno codes. This is the inverse
	// operation of HeResultFromErrorCode().
	switch ( hr )
	{
		case HE_E_FILE_NOT_FOUND:
			return ENOENT;
		case HE_E_PATH_NOT_FOUND:
			return ENOTDIR;
		case HE_E_ACCESS_DENIED:
			return EACCES;
		case HE_E_OUT_OF_MEMORY:
			return ENOMEM;
		case HE_E_DISK_FULL:
			return ENOSPC;
		default:
			return static_cast<int>((hr & 0x0000ffff) - 0x0200);
	}
}

/// \brief Determine if an an HeResult represents a standard library error code.
/// \param hr The HeResult code to convert.
inline bool IsErrorCodeResult( const HeResult hr )
{
	return (((hr & 0xffff0000) == 0x80040000) && ((hr & 0x0000ffff) >= 0x0200));
}

} /* namespace Helium */

#endif /* HERESULT_H */
