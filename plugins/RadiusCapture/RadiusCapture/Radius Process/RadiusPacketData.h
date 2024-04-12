// ============================================================================
//	RadiusPacketData.h
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "PeekPacket.h"

// ============================================================================
//	CRadiusPacketData
// ============================================================================

class CRadiusPacketData
{

	friend class CRadiusPacketDataMgr;

public:
	CRadiusPacketData() :
		m_pPacketData( NULL ),
		m_nDataLength( 0 ) {
			::ZeroMemory( (PVOID)&m_PeekPacket, sizeof( PeekPacket ) );
		}
	~CRadiusPacketData() {
		Clean();
	}

	CRadiusPacketData( const CRadiusPacketData& inOriginal ) {
		NewRadiusPacketData( &inOriginal.m_PeekPacket, inOriginal.m_pPacketData, inOriginal.m_nDataLength );
	}

	CRadiusPacketData& operator= ( const CRadiusPacketData& inOriginal ) {
		if ( this == &inOriginal ) return *this;
		
		if ( m_pPacketData ) {
			Clean();
		}

		NewRadiusPacketData( &inOriginal.m_PeekPacket, inOriginal.m_pPacketData, inOriginal.m_nDataLength );
		return *this;
	}

	bool GetData( PeekPacket& outPeekPacket, UInt8* * outData ) {
		if ( m_nDataLength == 0 ) {
			*outData = NULL;
			return false;
		}

		ASSERT( outData );
		ASSERT( m_pPacketData );
		outPeekPacket = m_PeekPacket;
		*outData = m_pPacketData;

		return true;
	}

	UInt64 GetTimeStamp() { return m_PeekPacket.fTimeStamp; }

protected:

	void NewRadiusPacketData( const PeekPacket* inPacket, const UInt8* inData, UInt16 inDataLength ) {
		CopyPeekPacket( inPacket );
		NewPacketData( inData, inDataLength );
	}

	void NewPacketData( const UInt8* pData, UInt16 uDataLength ) {
		if ( pData == NULL || uDataLength == 0 ) {
			ASSERT( 0 );
			return;
		}
		m_pPacketData = new UInt8[uDataLength]; 
		if( m_pPacketData == NULL ) {
			ASSERT( 0 );
			m_nDataLength = 0;
			return;
		}
		m_nDataLength = uDataLength;
		memcpy( m_pPacketData, pData, m_nDataLength );
	}

	void CopyPeekPacket( const PeekPacket* fPacket ) {
		memcpy( &m_PeekPacket, fPacket, sizeof( PeekPacket ) );

		if ( m_PeekPacket.fMediaSpecInfoBlock != NULL ) {
			m_PeekPacket.fMediaSpecInfoBlock = NewMediaSpecInfoBlock( (MediaSpecificHeaderAll*)fPacket->fMediaSpecInfoBlock );
		}
	}

	UInt8* NewMediaSpecInfoBlock( MediaSpecificHeaderAll* theHeaderAll );

	void Clean() {
		if ( m_pPacketData ) {
			ASSERT( m_nDataLength > 0 );
			delete m_pPacketData;
			m_pPacketData = NULL;
			m_nDataLength = 0;
		}
	} 

protected:
	UInt8*		m_pPacketData;
	UInt16		m_nDataLength;
	PeekPacket	m_PeekPacket;
};


// ============================================================================
//	CRadiusPacketDataMgr
// ============================================================================

class CRadiusPacketDataMgr
	: public CAtlArray < CRadiusPacketData >
{

public:
	CRadiusPacketDataMgr() : m_uLatestTimestamp( 0 ) {}
	~CRadiusPacketDataMgr() {
		Clean();
	}

	CRadiusPacketDataMgr& CRadiusPacketDataMgr::operator=(
		const CRadiusPacketDataMgr& inOriginal ){
		if ( this != &inOriginal ) {
			Clean();
			Copy( inOriginal );
			m_uLatestTimestamp = inOriginal.m_uLatestTimestamp;
		}
		return *this;
	}

	void Clean() {
		for ( size_t i = 0; i < GetCount(); i++ ) {
			CRadiusPacketData& PacketData = GetAt( i );
			PacketData.Clean();
		}
		RemoveAll(); 
	}

	UInt64 GetLatestTimestamp() { return m_uLatestTimestamp; }

	void AppendDataFromList( CRadiusPacketDataMgr& theOtherPacketDataList );

protected:
	UInt64 m_uLatestTimestamp;
};
