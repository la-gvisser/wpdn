// =============================================================================
//	tstring.h
// =============================================================================
//	Copyright (c) 2005 WildPackets, Inc. All rights reserved.

#ifndef TSTRING_H
#define TSTRING_H

#include <string>

#if defined(_UNICODE) || defined(UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#endif
