// =============================================================================
//	imemorystream.h
// =============================================================================
//	Copyright (c) 2005-2017 Savvius, Inc. All rights reserved.

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


#define IMEMORYSTREAM2_IID \
{ 0xAEFC685C, 0xEFA6, 0x46EC, {0x85, 0x44, 0xA4, 0x7E, 0x28, 0x80, 0xCE, 0x61} }

class HE_NO_VTABLE IMemoryStream2 : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(IMEMORYSTREAM2_IID)

	HE_IMETHOD Attach(/*in*/ const void* pv, /*in*/ size_t cb) = 0;
	HE_IMETHOD Detach(/*out*/ void** ppv, /*out*/ size_t* pcb) = 0;
	HE_IMETHOD Clear() = 0;
	HE_IMETHOD Reserve(/*in*/ size_t cbCapacity) = 0;
	HE_IMETHOD GetCapacity(/*out*/ size_t* pcbCapacity) = 0;
	HE_IMETHOD ShrinkToFit() = 0;
};

#endif /* IMEMORYSTREAM_H */
