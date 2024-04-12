// ============================================================================
// PacketList.h:
//      interface for the CPacketList class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2009. All rights reserved.

#pragma once

#include "Packet.h"
#include "Utils.h"
#include "LockedPtr.h"


// ============================================================================
//		CPacketList
// ============================================================================

class CPacketList
	:	public CAutoPtrArray<CPacket>
{
protected:
	mutable CMutex		m_Mutex;						// used by CSafePtr

public:
	bool	Lock() const {								// used by CSafePtr
		return (m_Mutex.Lock( DEFAULT_TIMEOUT ) != FALSE);
	}
	void	Unlock() const { m_Mutex.Unlock(); }		// used by CSafePtr
};


// ============================================================================
//	CPacketListPtr
// ============================================================================

typedef class TLockedPtr<CPacketList>	CPacketListPtr;


// ============================================================================
//	CSafePacketList
// ============================================================================

class CSafePacketList
{
protected:
	CPacketList	m_PacketList;

public:
	;		CSafePacketList() {}

	void			Clear() {
		CPacketListPtr	pList( Get() );
		if ( pList ) {
			pList->RemoveAll();
		}
	}
	CPacketListPtr	Get() { return CPacketListPtr( &m_PacketList ); }
};


// ============================================================================
//		CIPv4PacketList
// ============================================================================

class CIPv4PacketList
	:	public CAutoPtrArray<CIPv4Packet>
{
protected:
	mutable CMutex	m_Mutex;						// used by CSafePtr
	UInt64			m_nTimeStamp;

public:
	;		CIPv4PacketList() : m_nTimeStamp( 0 ) {}

	CIPv4Packet*	GetLast() { 
		size_t	nCount( GetCount() );
		if ( nCount == 0 ) return NULL;
		size_t	nLast( nCount - 1 );
		return GetAt( nLast );
	}

	size_t	Add( CAutoPtr<CIPv4Packet> inItem ) {
		m_nTimeStamp = ::GetTimeStamp();
		return CAutoPtrArray<CIPv4Packet>::Add( inItem );
	}

	void	GetSortList( CUIntArray& outList );
	UInt64	GetTimeStamp() const { return m_nTimeStamp; }

	bool	IsComplete( bool inAddMac ) const;

	bool	Lock() const {								// used by CSafePtr
		return (m_Mutex.Lock( DEFAULT_TIMEOUT ) != FALSE);
	}

	bool	Reassemble( CByteArray& outData );
	
	void	Unlock() const { m_Mutex.Unlock(); }		// used by CSafePtr
};


// ============================================================================
//	CIPv4PacketListPtr
// ============================================================================

typedef class TLockedPtr<CIPv4PacketList>	CIPv4PacketListPtr;


// ============================================================================
//	CSafeIPv4PacketList
// ============================================================================

class CSafeIPv4PacketList
{
protected:
	CIPv4PacketList	m_PacketList;

public:
	;		CSafeIPv4PacketList() {}

	void			Clear() {
		CIPv4PacketListPtr	pList( Get() );
		if ( pList ) {
			pList->RemoveAll();
		}
	}
	CIPv4PacketListPtr	Get() { return CIPv4PacketListPtr( &m_PacketList ); }
};


// ============================================================================
//		CIPv4PacketListList
// ============================================================================

class CIPv4PacketListList
	:	public	CAtlArray<CIPv4PacketList*>
{
public:
	bool	Remove( CIPv4PacketList* in ) {
		if ( in == NULL ) return false;
		size_t	nCount( GetCount() );
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( in == GetAt( i ) ) {
				RemoveAt( i );
				return true;
			}
		}
		return false;
	}
};


// ============================================================================
//		CUDPPacketList
// ============================================================================

class CUDPPacketList
	:	public CAutoPtrArray<CUDPPacket>
{
protected:
	UInt64	m_nTimeStamp;

public:
	CUDPPacket*	GetLast() { 
		size_t	nCount( GetCount() );
		if ( nCount == 0 ) return NULL;
		size_t	nLast( nCount - 1 );
		return GetAt( nLast );
	}

	size_t	Add( CAutoPtr<CUDPPacket> inItem ) {
		m_nTimeStamp = ::GetTimeStamp();
		return CAutoPtrArray<CUDPPacket>::Add( inItem );
	}
	void	GetSortList( CUIntArray& outList );
	UInt64	GetTimeStamp() { return m_nTimeStamp; }
	bool	Reassemble( CByteArray& outData );
};


// ============================================================================
//		CUDPPacketListList
// ============================================================================

class CUDPPacketListList
	:	public	CAtlArray<CUDPPacketList*>
{
public:
	bool	Remove( CUDPPacketList* in ) {
		if ( in == NULL ) return false;
		size_t	nCount( GetCount() );
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( in == GetAt( i ) ) {
				RemoveAt( i );
				return true;
			}
		}
		return false;
	}
};


// ============================================================================
//		CFragmentMap
// ============================================================================

class CFragmentMap
	:	public 	CMap<UInt64, UInt64&, CIPv4PacketList*, CIPv4PacketList*&>
{
protected:
	mutable CMutex		m_Mutex;						// used by CSafePtr

public:
	void	Clear() {
		POSITION	Pos( GetStartPosition() );
		UInt64		Key;
		CIPv4PacketList*	pItem( NULL );
		while ( Pos ) {
			GetNextAssoc( Pos, Key, pItem );
			delete pItem;
		}
		RemoveAll();
	}
	UInt32	GetDeepCount() const {
		UInt32		nCount( 0 );
		POSITION	Pos( GetStartPosition() );
		UInt64		Key;
		CIPv4PacketList*	pItem( NULL );
		while ( Pos ) {
			GetNextAssoc( Pos, Key, pItem );
			if ( pItem ) {
				nCount += static_cast<UInt32>( pItem->GetCount() );
			}
		}
		return nCount;
	}
	bool	Lock() const {								// used by CSafePtr
		return (m_Mutex.Lock( DEFAULT_TIMEOUT ) != FALSE);
	}
	void	Unlock() const { m_Mutex.Unlock(); }		// used by CSafePtr
};


// ============================================================================
//		CAddressMap
// ============================================================================

class CAddressMap
	:	public CMap<UInt32, UInt32&, CIPv4PacketListList*, CIPv4PacketListList*&>
{
public:
	void	Clear() {
		POSITION	Pos( GetStartPosition() );
		UInt32		Key;
		CIPv4PacketListList*	pItem( NULL );
		while ( Pos ) {
			GetNextAssoc( Pos, Key, pItem );
			delete pItem;
		}
		RemoveAll();
	}
	UInt32	GetDeepCount() const {
		UInt32		nCount( 0 );
		POSITION	Pos( GetStartPosition() );
		UInt32		Key;
		CIPv4PacketListList*	pItem( NULL );
		while ( Pos ) {
			GetNextAssoc( Pos, Key, pItem );
			if ( pItem ) {
				nCount += static_cast<UInt32>( pItem->GetCount() );
			}
		}
		return nCount;
	}
};


// ============================================================================
//	CMapsPtr
// ============================================================================

typedef class TLockedPtr<CFragmentMap>	CFragmentMapPtr;
typedef class CAddressMap*				CAddressMapPtr;


// ============================================================================
//	CSafeMaps
// ============================================================================

class CSafeMaps
{
protected:
	CFragmentMap	m_FragmentMap;
	CAddressMap		m_AddressMap;

public:
	;		CSafeMaps() {}

	void	Clear() {
		CFragmentMapPtr	spFrag;
		CAddressMapPtr	spAddr;
		if ( Get( spFrag, spAddr ) ) {
			spFrag->Clear();
			spAddr->Clear();
		}
	}
	bool	Get( CFragmentMapPtr& spFragMap, CAddressMapPtr& spAddrMap ) {
		spFragMap = CFragmentMapPtr( &m_FragmentMap );
		if ( spFragMap ) {
			spAddrMap = & m_AddressMap;
			return true;
		}
		return false;
	}
};
