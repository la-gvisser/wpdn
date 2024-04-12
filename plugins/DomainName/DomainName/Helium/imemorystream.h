// =============================================================================
//	imemorystream.h
// =============================================================================
//	Copyright (c) 2005 WildPackets, Inc. All rights reserved.

#ifndef	IMEMORYSTREAM_H
#define	IMEMORYSTREAM_H

#include "heunk.h"

#define IMEMORYSTREAM_IID \
{ 0x82F33283, 0xA00A, 0x4c84, {0xB5, 0xD9, 0x0F, 0x50, 0xB0, 0x5F, 0xFF, 0xBA} }

class HE_NO_VTABLE IMemoryStream : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMEMORYSTREAM_IID)

	HE_IMETHOD_(UInt8*) GetData() = 0;
	HE_IMETHOD_(size_t) GetDataSize() const = 0;
};

#endif
