// =============================================================================
//	MediaSpec.h
// =============================================================================
//	Copyright (c) 2000-2008 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1996-2000 AG Group, Inc. All rights reserved.

#ifndef MEDIASPEC_H
#define MEDIASPEC_H

#include "WPTypes.h"
#include "MediaTypes.h"
#include <memory.h>
#include <string.h>

class DECLSPEC CMediaSpec : public TMediaSpec
{
public:
	static const UInt32	AUTOCALC_MASK = 0;
	
	CMediaSpec()
	{
		// Set class and type to invalid values.
		fClass = kMediaSpecClass_Null;
		fType  = kMediaSpecType_Null;
		fMask  = 0;
	}

	CMediaSpec( const CMediaSpec& inMediaSpec )
	{
		// Looks dangerous, but isn't.
		memcpy( this, &inMediaSpec, sizeof(CMediaSpec) );
	}

	CMediaSpec( const TMediaSpec& inMediaSpec )
	{
		// Looks dangerous, but isn't.
		memcpy( this, &inMediaSpec, sizeof(CMediaSpec) );
	}

	CMediaSpec( TMediaSpecClass inClass, TMediaSpecType inType,
		UInt32 inMask, const UInt8* inData )
	{
		fClass = static_cast<UInt8>(inClass);
		fType  = static_cast<UInt8>(inType);
		fMask  = inMask;
		if ( fMask == AUTOCALC_MASK )
		{
			fMask = GetTypeMask();
		}
		if ( inData != NULL )
		{
			memmove( fData, inData, GetDataLength() );
		}
	}

	~CMediaSpec()
	{
		// Nothing to do here, we're really just a wrapper around a struct.
	}

public:
	inline void Clear()
	{
		// Looks dangerous, but isn't.
		memset( this, 0, sizeof(CMediaSpec) );
	}

	inline void Invalidate()
	{
		// Set class and type to invalid values.
		fClass = kMediaSpecClass_Null;
		fType  = kMediaSpecType_Null;
	}

	inline bool IsValid() const
	{
		return (fClass != kMediaSpecClass_Null) && (fType != kMediaSpecType_Null);
	}

	inline TMediaSpecClass GetClass() const
	{
		return static_cast<TMediaSpecClass>(fClass);
	}

	inline void SetClass( TMediaSpecClass inClass )
	{
		fClass = static_cast<UInt8>(inClass);
	}

	inline TMediaSpecType GetType() const
	{
		return static_cast<TMediaSpecType>(fType);
	}
	
	inline void SetType( TMediaSpecType inType )
	{
		fType = static_cast<UInt8>(inType);
	}

	inline bool IsWildcard() const
	{
		const UInt32 nTypeMask = this->GetTypeMask();
		return (nTypeMask & fMask) != nTypeMask;
	}

	inline bool operator==( const CMediaSpec& inMediaSpec ) const
	{
		// Compare the class, type, and data.
		return ( (inMediaSpec.fClass == fClass) && (inMediaSpec.fType == fType) &&
			(inMediaSpec.fMask == fMask ) &&
			(memcmp( inMediaSpec.fData, fData, GetDataLength()) == 0) );
	}

	inline bool operator!=( const CMediaSpec& inMediaSpec ) const
	{
		// Compare the class, type, and data.
		return ( (inMediaSpec.fClass != fClass) || (inMediaSpec.fType != fType) ||
			(inMediaSpec.fMask != fMask ) ||
			(memcmp( inMediaSpec.fData, fData, GetDataLength()) != 0) );
	}

	int					Compare( const CMediaSpec& inMediaSpec ) const;
	bool				CompareWithMask( const CMediaSpec& inMediaSpec ) const;

	static UInt16		GetDataLength( TMediaSpecType type );
	inline UInt16		GetDataLength() const { return GetDataLength( GetType() ); }

	static UInt32		GetTypeMask( TMediaSpecType type );
	inline UInt32		GetTypeMask() const { return GetTypeMask( GetType() ); }
	
	bool				IsMulticast() const;
	bool				IsBroadcast() const;
	bool				IsBroadcastOrMulticast() const;

	UInt32				CalculateHash() const;

	// Don't wanna couple to Protospecs.h for PROTOSPEC_INSTID or PROTOSPEC_ID.
	static CMediaSpec	FromProtoSpecInstID( UInt32 inProtoSpecInstID );
	static CMediaSpec	FromProtoSpecID( UInt16 inProtoSpecID );
	static CMediaSpec	FromTCPUDPPort( UInt16 inPortNumber );
	
	// Hash support for THashMap
	class Hashor
	{
	public:
		inline static int hashcode( const CMediaSpec& inAddress )
		{
			return static_cast<int>(inAddress.CalculateHash());
		}
	};
	
#if defined(_DEBUG)
	void				Dump( char* inStream, UInt32 ulCount ) const;
#endif
};

inline bool
operator < ( const CMediaSpec& a, const CMediaSpec& b )
{
	return (a.Compare( b ) < 0);
}

#endif
