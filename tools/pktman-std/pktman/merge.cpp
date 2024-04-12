// =============================================================================
//	merge.cpp
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "StdAfx.h"
#include "merge.h"
#include "readerlist.h"
#include "CaptureFile/CaptureFile.h"
#include <cassert>
#include <iostream>						// needed for wcout

using namespace std;
using namespace CaptureFile;

// Use (void) to silence unused warnings.
#define assertm(exp, msg)	assert(((void)msg, exp))


// =============================================================================
//		MergeEcho
// =============================================================================

void
MergeEcho(
	const path&	inPath )
{
	wcout << L"  " << inPath.wstring() << endl;
}


// =============================================================================
//		MergeFiles
// =============================================================================

int
MergeFiles(
	const tMergeData&	inData )
{
	size_t	nCount = inData.fSources.size();
	if ( nCount == 0 ) {
		wcout << L"At least one source file (-i) must be specified." << endl;
		return -1;
	}
	if ( inData.fDestination.empty() ) {
		wcout << L"At least one destination file (-o) must be specified." << endl;
		return -1;
	}

	if ( inData.IsVerbose() ) {
		wcout << L"Input files:" << endl;
		for_each( inData.fSources.begin(), inData.fSources.end(), MergeEcho );

		wcout << L"Output file:" << endl;
		wcout << L"  " << inData.fDestination << endl;

		if ( inData.IsThinOut() ) {
			wcout << L"Thin every " << inData.fThinEvery << " packet(s)." << endl;
		}
		if ( inData.IsInterleave() ) {
			wcout << L"Interleave files" << endl;
		}
		wcout << endl;
	}

	CReaderList	ayInFiles( inData.IsInterleave() );
	nCount = ayInFiles.Load( inData.fSources );
	assertm( (nCount > 0), "Number of files must be greater than 0." );

	tFileMode		fmMode( CF_TYPE_WRITER, CF_FORMAT_UNKNOWN, CF_ACCESS_SERIAL,
						false, ayInFiles.GetMediaType() );
	CCaptureFile	cwOut( inData.fDestination, fmMode );

	CFilePacket	pkt;
	if ( ayInFiles.GetNextPacket( pkt ) ) {
		UInt64	nTimeStamp = pkt.GetTimeStamp();
		UInt64	nPrevTime = pkt.GetTimeStamp();
		int		i = 0;
		do {
			i++;
			if ( inData.IsThinOut() && ((i % inData.fThinEvery) != 0) ) continue;
			if ( inData.IsDeltaTime() ) {
				UInt64	nRelativeTime = pkt.GetTimeStamp() - nPrevTime;
				nPrevTime = pkt.GetTimeStamp();
				nTimeStamp += inData.GetDuration();
				nTimeStamp += (nRelativeTime) ? (nRelativeTime % 1000000) : 0;
				pkt.SetTimeStamp( nTimeStamp );
			}
			cwOut.WriteFilePacket( pkt );
		} while ( ayInFiles.GetNextPacket( pkt ) );
	}

	ayInFiles.Close();
	cwOut.Close();

	return true;
}
