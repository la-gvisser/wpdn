// =============================================================================
//	FileCaptureBuffer.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "FileCaptureBuffer.h"


// =============================================================================
//		CFileCaptureBuffer
// =============================================================================

CFileCaptureBuffer::CFileCaptureBuffer(
	const CPeekString&	inFilePath,
	const CPeekString&	inFileBaseName,
	UInt32				inRefreshFileType,
	bool				inAppendTimestring )
		:	CRefreshFile( inFilePath, inFileBaseName, inRefreshFileType, inAppendTimestring )
{
	SetCapacity( kExtraBufferSize );
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

bool
CFileCaptureBuffer::Copy(
	const CFileCaptureBuffer&	inFileCaptureBuffer )
{
	if ( inFileCaptureBuffer.GetCount() == 0 ) return false;
	Deactivate();
	if ( CCaptureBuffer::Copy( inFileCaptureBuffer ) != true ) return false;
	if ( CRefreshFile::Copy( inFileCaptureBuffer ) != true ) return false;
	m_nRefreshSize = inFileCaptureBuffer.m_nRefreshSize;
	m_nRefreshTriggerSize = inFileCaptureBuffer.m_nRefreshTriggerSize;

	return true;
}


// -----------------------------------------------------------------------------
//		Refresh
// -----------------------------------------------------------------------------

bool
CFileCaptureBuffer::Refresh(
	bool	bWriteIfEmpty )
{
	if ( (GetCount() == 0) && !bWriteIfEmpty ) return true;

	if ( !Save( bWriteIfEmpty ) ) return false;

	CCaptureBuffer::Reset();
	ResetTimeInterval();

	return true;
}


// -----------------------------------------------------------------------------
//		Save
// -----------------------------------------------------------------------------

bool
CFileCaptureBuffer::Save(
	bool	bWriteIfEmpty )
{
	bool bReturn = false;

	if ( (GetCount() == 0) && !bWriteIfEmpty ) return true;

	if ( m_nRefreshFileType != kRefreshFileTypeNone ) {
		_ASSERTE( !m_strFilePath.IsEmpty() && DirectoryExists( m_strFilePath ) );
		if ( m_strFilePath.IsEmpty() ) return false;
		if ( !DirectoryExists( m_strFilePath ) ) {
			if ( !MakePath( m_strFilePath ) || !DirectoryExists( m_strFilePath ) ) return false;
		}

		CPeekString strNextSaveName = CreateNextSaveName();
		if ( !strNextSaveName.IsEmpty() ) {
			bReturn = CCaptureBuffer::Save( strNextSaveName );
		}
	}

	return bReturn;
}


// -----------------------------------------------------------------------------
//		SaveSelected
// -----------------------------------------------------------------------------

bool
CFileCaptureBuffer::SaveSelected(
	UInt32	inRangeCount,
	UInt64*	inRanges )
{
	if ( (GetCount() == 0) ) return false;
	if ( m_nRefreshFileType == kRefreshFileTypeNone ) return false;

	_ASSERTE( !m_strFilePath.IsEmpty() && DirectoryExists( m_strFilePath ) );
	if ( m_strFilePath.IsEmpty() ) return false;
	if ( !DirectoryExists( m_strFilePath ) ) {
		if ( !MakePath( m_strFilePath ) || !DirectoryExists( m_strFilePath ) ) return false;
	}

	CPeekString strNextSaveName = CreateNextSaveName();
	if ( !strNextSaveName.IsEmpty() ) {
		CCaptureBuffer::SaveSelected( strNextSaveName, inRangeCount, inRanges );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		SetRefreshSize
// -----------------------------------------------------------------------------

bool
CFileCaptureBuffer::SetRefreshSize(
	const UInt32	inRefreshSize,
	const UInt32	inUnits )
{

	UInt32 nRefreshSize = 0;
	switch ( inUnits ) {
		case kBytes:
			nRefreshSize = inRefreshSize;
			break;
		case kKilobytes:
			nRefreshSize = inRefreshSize * kBytesInKilobyte;
			break;
		case kMegabytes:
			nRefreshSize = inRefreshSize * kBytesInMegabyte;
			break;
		case kGigabytes:
			nRefreshSize = inRefreshSize * kBytesInGigabyte;
			break;
		default:
			return false;
	}

	const UInt32 nWorkMaxRefreshSize = kMaxRefreshSize - kExtraBufferSize;

	_ASSERTE( (nRefreshSize >= kMinRefreshSize) && (nRefreshSize <= nWorkMaxRefreshSize) );
	if ( (nRefreshSize < kMinRefreshSize) || (nRefreshSize > nWorkMaxRefreshSize ) ) {
		return false;
	}

	if ( m_nRefreshTriggerSize > GetCapacityAvailable() ) {
		Refresh();
	}

	m_nRefreshSize = nRefreshSize;
	m_nRefreshTriggerSize = (m_nRefreshSize > kRefreshSizeAdjust) ? (m_nRefreshSize - kRefreshSizeAdjust) : m_nRefreshSize;
	m_nRefreshTriggerSize = (m_nRefreshTriggerSize > kBytesInKilobyte) ? m_nRefreshTriggerSize : kBytesInKilobyte;

	SetCapacity( m_nRefreshSize + kExtraBufferSize );

	if ( m_nRefreshTriggerSize > GetCapacityAvailable() ) {
		Refresh();
	}

	return true;
}
