// =============================================================================
//	compare.cpp
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "StdAfx.h"
#include "compare.h"
#include "CaptureFile/CaptureFile.h"
#include <iostream>						// needed for wcout

using namespace std;
using namespace CaptureFile;


// =============================================================================
//		VerboseEcho
// =============================================================================

void
CompareEcho(
	const path&	inPath )
{
	wcout << L"  " << inPath.wstring() << endl;
}


// =============================================================================
//		CompareFiles
// =============================================================================

int
CompareFiles(
	const tCompareData&	inData )
{
	wcout << L"Comparing Files" << endl;

	size_t	nSources = inData.fSources.size();
	if ( nSources < 2 ) {
		wcout << L"At least 2 source files (-i) must be specified." << endl;
		wcout << endl;
		return -1;
	}

	if ( inData.IsVerbose() ) {
		wcout << L"Input files:" << endl;
		for_each( inData.fSources.begin(), inData.fSources.end(), CompareEcho );
		wcout << endl;
	}

	if ( inData.IsVerbose() ) {
		wcout << "Open: " << inData.fSources[0] << endl;
	}
	CCaptureFile	cfLeft( inData.fSources[0], CF_TYPE_READER );
	if ( !cfLeft.IsOpen() ) {
		wcout << "Failed to open: " << inData.fSources[0] << endl;
		return -1;
	}

	if ( inData.IsVerbose() ) {
		wcout << "Open: " << inData.fSources[1] << endl;
	}
	CCaptureFile	cfRight( inData.fSources[1], CF_TYPE_READER );
	if ( !cfRight.IsOpen() ) {
		wcout << "Failed to open: " << inData.fSources[1] << endl;
		return -1;
	}
	size_t	nFileIndex = 2;

	CFilePacket		pktLeft;
	size_t			nLeftIndex( 0 );
	CFilePacket		pktRight;
	size_t			nRightIndex( 0 );
	CFilePacket		pktGreater;
	// CFilePacket&	pktLesser( (bLeftGreater) ? pktRight : pktLeft );
	// CCaptureFile&	cfGreater( (bLeftGreater) ? cfLeft : cfRight );

	size_t	nCount( 0 );
	size_t	nCompare( 0 );
	size_t	nMatch( 0 );
	size_t	nMissMatch( 0 );
	size_t	nProgress( 0 );

	UInt32	nError( 0 );
	while ( cfLeft.ReadFilePacket( pktLeft, nError ) ) {
		nLeftIndex++;
		if ( !cfRight.ReadFilePacket( pktRight, nError ) ) {
			if ( inData.IsVerbose() ) {
				wcout << "No more packets in " << inData.fSources[nFileIndex - 1] << endl;
			}
			if ( nFileIndex >= nSources ) {
				wcout << "No more files." << endl;
				break;
			}
			cfRight.Close();
			if ( inData.IsVerbose() ) {
				wcout << "Closed: " << inData.fSources[nFileIndex - 1] << endl;
				wcout << "Open:   " << inData.fSources[nFileIndex] << endl;
			}
			if ( !cfRight.Open( inData.fSources[nFileIndex], CF_TYPE_READER ) ) {
				wcout << "Failed to open: " << inData.fSources[nFileIndex] << endl;
				break;
			}

			nRightIndex = 0;
			if ( !cfRight.ReadFilePacket( pktRight, nError ) ) {
				wcout << "Failed to read packet: " << inData.fSources[nFileIndex] << endl;
				break;
			}
			nRightIndex++;
			nFileIndex++;
		}

		nCompare++;
		if ( nProgress && ((nCompare % nProgress) == 0) && inData.IsVerbose() ) {
			wcout << "Compared " << nCompare << " of " << nCount << " packets." << endl;
		}

		// Compare the packets.
		wstring	strError;
		nCount++;
		if ( pktLeft.Compare( pktRight, inData.fFlags ) ) {
			nMatch++;
		}
		else {
			nMissMatch++;
			if ( inData.IsVerbose() ) {
				wcout << L"Packet " << nCompare << L" " << strError << L"." << endl;
			}

#if (0)
			// Check the rest of the larger file for a matching packet.
			size_t	nSubMatch( 0 );
			size_t	nSubMissMatch( 0 );
			while ( cfGreater.ReadFilePacket( pktGreater ) ) {
				if ( ((nCompare % 1000) == 0) && inData.IsVerbose() ) {
					wcout << "Compared " << nCompare << " of " << nCount << " packets." << endl;
				}

				if ( pktLesser.Compare( pktGreater, inData.fFlags ) ) {
					nSubMatch++;
					break;
				}
				else {
					nSubMissMatch++;
				}
			}

			if ( nSubMatch > 0 ) {
				nMatch++;

				// Re-sync cfLeft and cfRight and continue.
				CFilePacket		pkt;
				cfLeft.ReadFilePacket( pkt, (nLeftIndex - 1) );
				cfRight.ReadFilePacket( pkt, (nRightIndex - 1) );
			}
#endif
		}
	}

	wcout << endl;

	int	nResult( 0 );

	if ( nMissMatch == 0 ) {
		nResult = 0;
		wcout << "Result: " << nMatch << L" packets of " << nCount << L" packets match." << endl;
		wcout << L"All packets match." << endl;
	}
	else if ( nMatch == 0 ) {
		nResult = 2;
		wcout << L"No packets match." << endl;
	}
	else {
		nResult = 1;
		wcout << "Result: " << nMatch << L" of " << nCount << L" packets match." << endl;
	}

	cfLeft.Close();
	cfRight.Close();

	return nResult;
}
