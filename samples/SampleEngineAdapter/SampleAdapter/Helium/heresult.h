// =============================================================================
//	heresult.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef HERESULT_H
#define HERESULT_H

#include <errno.h>
#include "hestdint.h"

/// \typedef HeResult
/// \brief Helium result code. Similar to HRESULT or nsresult.
typedef SInt32 HeResult;

#define HE_S_OK							((HeResult) 0)
#define	HE_S_FALSE						((HeResult)	0x00000001L)
#define	HE_E_UNEXPECTED					((HeResult)	0x8000ffffL)
#define	HE_E_PENDING					((HeResult)	0x8000000AL)
#define	HE_E_NOT_IMPLEMENTED			((HeResult)	0x80004001L)
#define	HE_E_NO_INTERFACE				((HeResult)	0x80004002L)
#define	HE_E_INVALID_POINTER			((HeResult)	0x80004003L)
#define	HE_E_NULL_POINTER				HE_E_INVALID_POINTER
#define	HE_E_ABORT						((HeResult)	0x80004004L)
#define	HE_E_FAIL						((HeResult)	0x80004005L)
#define	HE_E_NOT_AVAILABLE				((HeResult)	0x80040111L)
#define	HE_E_FACTORY_NOT_REGISTERED		((HeResult)	0x80040154L)
#define	HE_E_FACTORY_NOT_LOADED			((HeResult)	0x800401f8L)
#define HE_E_FILE_NOT_FOUND				((HeResult) 0x80070002L)
#define HE_E_PATH_NOT_FOUND				((HeResult) 0x80070003L)
#define	HE_E_ACCESS_DENIED				((HeResult)	0x80070005L)
#define HE_E_INVALID_HANDLE				((HeResult) 0x80070006L)
#define	HE_E_OUT_OF_MEMORY				((HeResult)	0x8007000eL)
#define	HE_E_EOF						((HeResult)	0x80070026L)
#define HE_E_FILE_EXISTS				((HeResult) 0x80070050L)
#define	HE_E_ILLEGAL_VALUE				((HeResult)	0x80070057L)
#define	HE_E_INVALID_ARG				HE_E_ILLEGAL_VALUE
#define	HE_E_DISK_FULL					((HeResult)	0x80070070L)
#define HE_E_INSUFFICIENT_BUFFER		((HeResult) 0x8007007aL)
#define HE_E_CANCELLED					((HeResult) 0x800704c7L)
#define HE_E_TIMEOUT					((HeResult) 0x800705b4L)
#define HE_E_TYPE_MISMATCH				((HeResult) 0x80020005L)
#define HE_E_BADVARTYPE					((HeResult) 0x80020008L)
#define HE_E_OVERFLOW					((HeResult) 0x8002000aL)

/// \def HE_SUCCEEDED
/// \brief Corresponds to and compatible with SUCCEEDED().
#define HE_SUCCEEDED(_hr)	((HeResult)(_hr) >= 0)

/// \def HE_FAILED
/// \brief Corresponds to and compatible with FAILED().
#define HE_FAILED(_hr)		((HeResult)(_hr) < 0)

namespace Helium
{

/// \brief Create an HeResult from a Windows error code.
/// \param x The Windows error code.
inline HeResult HeResultFromWin32( const SInt32 x )
{
	return (x <= 0) ?
		static_cast<HeResult>(x) :
		static_cast<HeResult>((x & 0x0000ffff) | 0x80070000);
}

/// \brief Create an HeResult from a standard library error code.
/// \param x The error code.
inline HeResult HeResultFromErrorCode( const int x )
{
	// Map errno codes to well-known HeResults. Only includes values that can
	// successfully round-trip. For example, both EACCES and EPERM could
	// logically map to HE_E_ACCESS_DENIED, but HE_E_ACCESS_DENIED cannot
	// be mapped back to both. In such cases, the closest match is used.
	// Keep in sync with ErrorCodeFromHeResult().
	switch ( x )
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
			return (x <= 0) ?
				static_cast<HeResult>(x) :
				static_cast<HeResult>(((x + 0x0200) & 0x0000ffff) | 0x80040000);
	}
}

/// \brief Get a standard library error code from an HeResult.
/// \param hr The HeResult code to convert.
inline int ErrorCodeFromHeResult( const HeResult hr )
{
	// Map well-known HeResults back to errno codes. This is the inverse
	// operation of HeResultFromErrorCode().
	// Keep in sync with HeResultFromErrorCode().
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

} /* namespace Helium */

#endif /* HERESULT_H */
