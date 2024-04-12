// =============================================================================
//	readerlist.cpp
// =============================================================================
//	Copyright (c) 2013 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "readerlist.h"
#include ".\CaptureFile\CaptureFile.h"

using namespace CaptureFile;


// =============================================================================
//		Now
// =============================================================================

UInt64
Now()
{
	union {
		UInt64		i;
		FILETIME	ft;
	} utime;

	::GetSystemTimeAsFileTime( &utime.ft );
	utime.i *= kFileTimeInAPeekTime;
	return utime.i;
}


// =============================================================================
//		CReaderList
// =============================================================================

// -----------------------------------------------------------------------------
//		GetNextPacket
// -----------------------------------------------------------------------------

bool
CReaderList::GetNextPacket(
	CFilePacket&	outPacket )
{
	if ( m_nStartTime == 0 ) {
		outPacket = m_ayFiles[0].GetPacket();
		m_ayFiles[0].ReadNext();
		m_nStartTime = Now();

		outPacket.SetTimeStamp( m_nStartTime );
		if ( m_ayFiles.size() > 1 ) ++m_nNextFile;
		return true;
	}

	if ( IsInterleave() ) {
		if ( m_nNextFile >= m_ayFiles.size() ) {
			m_nNextFile = 0;
		}

		auto next = m_nNextFile;
		while ( m_ayFiles[next].IsEOF() ) {
			++next;
			if ( next >= m_ayFiles.size() ) {
				next = 0;
			}
			if ( next == m_nNextFile ) {
				return false;
			}
		}
		m_nNextFile = next;

		m_nStartTime += m_ayFiles[m_nNextFile].GetDeltaTime();
		outPacket = m_ayFiles[m_nNextFile].GetPacket();
		outPacket.SetTimeStamp( m_nStartTime );

		m_ayFiles[m_nNextFile].ReadNext();

		++m_nNextFile;
		if ( m_nNextFile >= m_ayFiles.size() ) {
			m_nNextFile = 0;
		}
	}
	else {
		size_t	nNextItem = kMaxSizeT;
		UInt64	nMinDelta = kMaxUInt64;
		size_t	nCount = m_ayFiles.size();
		for ( size_t i = 0; i < nCount; i++ ) {
			if ( m_ayFiles[i].IsEOF() ) continue;

			UInt64	nDeltaTime = m_ayFiles[i].GetDeltaTime();
			if ( nDeltaTime == 0 ) {	// Insert first packets in m_ayFiles order.
				nMinDelta = i * 1000;	// 1000 nanoseconds.
				nNextItem = i;
				break;
			}
			if ( nDeltaTime < nMinDelta ) {
				nMinDelta = nDeltaTime;
				nNextItem = i;
			}
		}
		if ( nNextItem == kMaxSizeT ) return false;

		outPacket = m_ayFiles[nNextItem].GetPacket();
		outPacket.SetTimeStamp( m_nStartTime + nMinDelta );

		m_ayFiles[nNextItem].ReadNext();
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Load
// -----------------------------------------------------------------------------

size_t
CReaderList::Load(
	const CStringList& inFiles )
{
	m_ayFiles.clear();
	m_nStartTime = 0;
	m_nNextFile = 0;

	size_t	nCount = inFiles.size();
	for ( size_t i = 0; i < nCount; i++ ) {
		m_ayFiles.emplace_back(inFiles[i], tFileMode(CF_TYPE_READER));

		const CFileItem&	fiItem( m_ayFiles.back() );
		_ASSERTE( fiItem.IsValid() );

		if ( fiItem.IsValid() ) {
			const CCaptureFile*	cf( fiItem );
			if ( i == 0 ) {
				m_MediaType = cf->GetMediaType();
			}
			else {
				tMediaType	mtSource( cf->GetMediaType() );
				if ( mtSource != m_MediaType ) {
					continue;
				}
			}
			m_ayFiles.back().ReadFirst();
		}
		else {
			m_ayFiles.pop_back();
		}
	}

	return m_ayFiles.size();
}
