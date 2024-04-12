// =============================================================================
//	readerlist.cpp
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "StdAfx.h"
#include "readerlist.h"
#include "CaptureFile/CaptureFile.h"

#include <filesystem>

using std::filesystem::path;
using namespace CaptureFile;


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
		size_t	nNextItem = std::numeric_limits<size_t>::max();
		UInt64	nMinDelta = std::numeric_limits<UInt64>::max();
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
		if ( nNextItem == std::numeric_limits<size_t>::max() ) return false;

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
	const std::vector<path>& inFiles )
{
	const tMediaType	unknownType( kMediaType_Max, kMediaSubType_Max );
	m_ayFiles.clear();
	m_nStartTime = 0;
	m_nNextFile = 0;

	for ( path pathName : inFiles ) {
		m_ayFiles.emplace_back( pathName, CF_TYPE_READER );
		CFileItem&	fiItem( m_ayFiles.back() );

		if ( fiItem.IsValid() ) {
			const CCaptureFile*	cf( fiItem );
			if ( m_MediaType == kUnknownMediaType ) {
				m_MediaType = cf->GetMediaType();
			}
			else {
				tMediaType	mtSource( cf->GetMediaType() );
				if ( mtSource != m_MediaType ) {
					continue;
				}
			}
			fiItem.ReadFirst();
		}
		else {
			m_ayFiles.pop_back();
		}
	}

	return m_ayFiles.size();
}
