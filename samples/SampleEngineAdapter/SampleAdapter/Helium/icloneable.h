// =============================================================================
//	icloneable.h
// =============================================================================
//	Copyright (c) 2005-2008 WildPackets, Inc. All rights reserved.

#ifndef	ICLONEABLE_H
#define	ICLONEABLE_H

#include "heunk.h"

#define ICLONEABLE_IID \
{ 0x28DEFF5E, 0xA113, 0x403D, {0xB0, 0x2F, 0x12, 0x34, 0xDE, 0xDE, 0x8B, 0x8D} }

/// \interface ICloneable
/// \brief Clone can be implemented either as a deep copy or a shallow copy.
/// In a deep copy, all objects are duplicated; whereas, in a shallow copy,
/// only the top-level objects are duplicated and the lower levels contain
/// references. The resulting clone must be of the same type as or a compatible
/// type to the original instance.
class HE_NO_VTABLE ICloneable : public Helium::IHeUnknown
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(ICLONEABLE_IID);

	HE_IMETHOD Clone(/*out*/ Helium::IHeUnknown** ppClone) = 0;
};

#endif /* ICLONEABLE_H */
