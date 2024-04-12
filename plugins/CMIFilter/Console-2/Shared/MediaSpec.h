// ============================================================================
//	MediaSpec.h
// ============================================================================
//	Coypright (c) WildPackets, Inc. 2000-2004. All rights reserved.
//	Coypright (c) AG Group, Inc. 1996-2000. All rights reserved.

#ifndef MEDIASPEC_H
#define MEDIASPEC_H

#include "AGTypes.h"
#include "MediaTypes.h"
#if defined(_DEBUG)
#include "ByteStream.h"
#endif

class DECLSPEC CMediaSpec : public TMediaSpec
{
public:
	CMediaSpec()
	{
		// Set class and type to invalid values.
		fClass = kMediaSpecClass_Null;
		fType = kMediaSpecType_Null;
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
		UInt32 inMask, const UInt8* inData );

	CMediaSpec::~CMediaSpec()
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
		fType = kMediaSpecType_Null;
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

	bool			operator==( const CMediaSpec& inMediaSpec ) const;
	int				Compare( const CMediaSpec& inMediaSpec ) const;
	bool			CompareWithMask( const CMediaSpec& inMediaSpec ) const;

	UInt16			GetDataLength() const;
	UInt32			GetTypeMask() const;
	
	bool			IsWildcard() const;
	bool			IsMulticast() const;
	bool			IsBroadcast() const;
	bool			IsBroadcastOrMulticast() const;

	UInt32			CalculateHash() const;

#if defined(_DEBUG)
	void			Dump( CByteStream& inStream ) const;
#endif
};

#endif
