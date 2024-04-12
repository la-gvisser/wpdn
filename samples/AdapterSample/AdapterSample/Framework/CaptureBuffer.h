// =============================================================================
//	CaptureBuffer.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "peekcore.h"
#include "PeekContextProxy.h"
#include "Packet.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Capture Buffer
//
//	A Capture Buffer encapsulates a Packet Buffer, for read access, a Dynamic
//	Packet Buffer for write access, and a Persist File, for persistence to and
//	from (save and load) the file system. The three objects are interfaces into
//	a Base Packet Buffer that manages the packets. A Capture Buffer efficiently
//	manages a large number of packets.
//
//	Use a File Capture Buffer for storing larger numbers of packets on disk.
//
//	A packet buffer contains an organization of memory that efficiently caches
//	packets as they flow from the network. This memory space is optimized for
//	accumulating and retrieving data in a consecutive (or sequential) manner.
//	A packet's data, in fact, exists within the context of the packet buffer 
//	itself, and the packet data resides in the buffer, and not in the packet 
//	object. Thus the buffer plays a critical role in the capture process, and
//	is nearly synonymous with a capture. Buffer methods include buffer size,
//	packet count, a number of packet-related methods and a number of
//	capture-related methods such as start and stop time, media type, and so on.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		CCaptureBuffer
// =============================================================================

class CCaptureBuffer
{
protected:
	CPacketBuffer			m_PacketBuffer;
	CDynamicPacketBuffer	m_DynamicBuffer;
	CPersistFile			m_PersistFile;

public:
	;		CCaptureBuffer::CCaptureBuffer( UInt64 inCapacity = BYTES_PER_MEGABYTE )
	{
		m_PacketBuffer.Create();
		Peek::ThrowIf( !m_PacketBuffer.IsValid() );

		m_DynamicBuffer = m_PacketBuffer;
		m_PersistFile = m_PacketBuffer;

		SetCapacity( inCapacity );
	}
	virtual		CCaptureBuffer::~CCaptureBuffer() {}

	CCaptureBuffer&	operator=( const CCaptureBuffer& inOther ) {
		if ( this != &inOther ) {
			if ( !Copy( inOther ) ) {
				ASSERT( 0 );
			}
		}
		return *this;
	}

	bool	Copy( const CCaptureBuffer& inOther ) {
		const UInt64 nCapacity = inOther.GetCapacity();
		if ( nCapacity == 0 ) return false;
		HE_ASSERT( inOther.m_DynamicBuffer.IsValid() );

		SetCapacity( nCapacity );
		SetLinkSpeed( inOther.GetLinkSpeed() );
#if (0)
		SetMediaType( inOther.GetMediaType() );
		SetMediaSubType( inOther.GetMediaSubType() );
#else
		SetMediaType( inOther.GetMediaType() );
#endif
		SetStartDateTime( inOther.GetStartDateTime() );
		SetStopDateTime( inOther.GetStopDateTime() );

		for ( UInt32 i = inOther.GetFirstPacketIndex(); i < inOther.GetCount(); i++ ) {
			CPacket	thePacket = inOther.m_PacketBuffer.GetPacket( i );
			m_DynamicBuffer.PutPacket( thePacket );
		}
		HE_ASSERT( GetCount() == inOther.GetCount() );
		return true;
	}


	//
	// IPacketBuffer
	//

	//void			AddRefPacket( UInt32 inIndex ) { m_PacketBuffer.AddRefPacket( inIndex ); }

	UInt64			GetBufferSize() const { return m_PacketBuffer.GetSize(); }
	SInt64			GetDuration() const { return m_PacketBuffer.GetDuration(); }
	UInt32			GetFirstPacketIndex() const { return m_PacketBuffer.GetFirstPacketIndex(); }
	UInt64			GetFirstPacketNumber() const { return m_PacketBuffer.GetFirstPacketNumber(); }
	UInt64			GetLinkSpeed() const { return m_PacketBuffer.GetLinkSpeed(); }
	tMediaType		GetMediaType() const { return m_PacketBuffer.GetMediaType(); }
#if (0)
	TMediaType		GetMediaType() const { return m_PacketBuffer.GetMediaType(); }
	TMediaSubType	GetMediaSubType() const { return m_PacketBuffer.GetMediaSubType(); }
#endif
	CPacket			GetPacket( UInt32 inIndex ) const { return m_PacketBuffer.GetPacket( inIndex ); }
	UInt32			GetCount() const { return m_PacketBuffer.GetCount(); }
	const UInt8*		GetPacketData( UInt32 inIndex ) const { return m_PacketBuffer.GetPacketData( inIndex ); }
	const PeekPacket*	GetPacketHeader( UInt32 inIndex ) const { return m_PacketBuffer.GetPacketHeader( inIndex ); }
	UInt32				GetPacketIndex( UInt64 inPacketNumber ) const { return m_PacketBuffer.GetPacketIndex( inPacketNumber ); }
	const void*			GetPacketMetaData( UInt32 inIndex ) const { return m_PacketBuffer.GetPacketMetaData( inIndex ); }
	UInt64			GetPacketNumber( UInt32 inIndex ) const { return m_PacketBuffer.GetPacketNumber( inIndex ); }
	UInt64			GetStartDateTime() const { return m_PacketBuffer.GetStartDateTime(); }
	UInt64			GetStopDateTime() const { return m_PacketBuffer.GetStopDateTime(); }

	bool			IsValidPacketIndex( UInt32 inIndex ) const { return m_PacketBuffer.IsValidPacketIndex( inIndex ); }

	bool			ReadPacket( UInt32 inIndex, CPacket& outPacket ) {
		try {
			outPacket = GetPacket( inIndex );
			outPacket.Detach();
			ASSERT( outPacket.IsDetached() );
		}
		catch ( ... ) {
			outPacket.Reset();
			return false;
		}
		return true;
	}
	//void			ReleasePacket( UInt32 inIndex ) { m_PacketBuffer.ReleasePacket( inIndex ); }
	void			Reset() { m_PacketBuffer.Reset(); }

	void			SaveSelected( const CPeekString& inPath, UInt32 inRangeCount, UInt64* inRanges ) {
		m_PacketBuffer.SaveSelected( inPath, inRangeCount, inRanges );
	}
	void			SetLinkSpeed( UInt64 inSpeed ) { m_PacketBuffer.SetLinkSpeed( inSpeed ); }
	void			SetMediaType( tMediaType inMediaType ) { return m_PacketBuffer.SetMediaType( inMediaType ); }
	void			SetMediaType( TMediaType inType ) { return m_PacketBuffer.SetMediaType( inType ); }
	void			SetMediaSubType( TMediaSubType inSubType ) { return m_PacketBuffer.SetMediaSubType( inSubType ); }
	void			SetStartDateTime( UInt64 inTime ) { return m_PacketBuffer.SetStartDateTime( inTime ); }
	void			SetStopDateTime( UInt64 inTime ) { return m_PacketBuffer.SetStopDateTime( inTime ); }

	//
	// IDynamicPacketBuffer
	//

	UInt64		GetCapacity() const { return m_DynamicBuffer.GetCapacity(); }
	UInt64		GetCapacityAvailable() const { return m_DynamicBuffer.GetCapacityAvailable(); }
	UInt64		GetCapacityUsed() const { return m_DynamicBuffer.GetCapacityUsed(); }

	bool		IsFull() const { return m_DynamicBuffer.IsFull(); }

	void		SetCapacity( UInt64 inCapacity ) { return m_DynamicBuffer.SetCapacity( inCapacity ); }

	void		WritePacket( const CPacket& inPacket ) { m_DynamicBuffer.PutPacket( inPacket ); }

	//
	// IHePersistFile
	//

	CPeekString	GetCurFile() const { return m_PersistFile.GetFileName(); }

	bool		IsDirty() const { return m_PersistFile.IsDirty(); }

	bool		LoadFromFile( const CPeekString& inFilePath ) {
		return m_PersistFile.LoadFromFile( inFilePath );
	}
// No current implementation
//	bool		SaveCompletedToFile( const CPeekString& inFilePath ) const {
//		return m_PersistFile.SaveCompletedToFile( inFilePath );
//	}
	bool		Save( const CPeekString& inFilePath ) const {
		return m_PersistFile.Save( inFilePath );
	}
};
