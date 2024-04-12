// =============================================================================
//	compare.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "compare.h"
#include "CaptureFile\CaptureFile.h"
#include <iostream>						// needed for wcout

using namespace std;
using namespace CaptureFile;


// =============================================================================
//		VerboseEcho
// =============================================================================

void
CompareEcho(
	const wstring&	inString )
{
	wcout << L"  " << inString << endl;
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
	bool	bMultiFile( nSources > 2 );

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
	CCaptureFile	cfLeft( inData.fSources[0], tFileMode( CF_TYPE_READER, true ) );
	if ( !cfLeft.IsOpen() ) {
		wcout << "Failed to open: " << inData.fSources[0] << endl;
		return -1;
	}

	if ( inData.IsVerbose() ) {
		wcout << "Open: " << inData.fSources[1] << endl;
	}
	CCaptureFile	cfRight( inData.fSources[1], tFileMode( CF_TYPE_READER, true ) );
	if ( !cfRight.IsOpen() ) {
		wcout << "Failed to open: " << inData.fSources[1] << endl;
		return -1;
	}
	size_t	nFileIndex = 2;

	size_t	nLeftCount( cfLeft.GetPacketCount() );
	size_t	nRightCount( cfRight.GetPacketCount() );

	// When comparing one fine to another, is the First file (left) larger?
	bool	bLeftGreater( bMultiFile || (nLeftCount > nRightCount) );

	if ( bMultiFile ) {
		wcout << L"Comparing " << nRightCount << " of " << nLeftCount << " packets." << endl;
	}
	else {
		if ( nLeftCount != nRightCount ) {
			wcout << L"The files do not contain the same number of packets: " <<
				nLeftCount << L" vs. " << nRightCount << endl;
		}
		else {
			wcout << L"Comparing " << nLeftCount << " packets." << endl;
		}
	}

	CFilePacket		pktLeft;
	size_t			nLeftIndex( 0 );
	CFilePacket		pktRight;
	size_t			nRightIndex( 0 );
	CFilePacket		pktGreater;
	CFilePacket&	pktLesser( (bLeftGreater) ? pktRight : pktLeft );
	CCaptureFile&	cfGreater( (bLeftGreater) ? cfLeft : cfRight );

	size_t	nCount( (bLeftGreater) ? nLeftCount : nRightCount );
	size_t	nCompare( 0 );
	size_t	nMatch( 0 );
	size_t	nMissMatch( 0 );
	size_t	nProgress( 0 );

	if ( nCount > 100000 ) {
		nProgress = nCount / 100;
	}

	while ( cfLeft.ReadFilePacket( pktLeft ) ) {
		nLeftIndex++;
		if ( !cfRight.ReadFilePacket( pktRight ) ) {
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
			if ( !cfRight.Open( inData.fSources[nFileIndex], tFileMode( CF_TYPE_READER, true ) ) ) {
				wcout << "Failed to open: " << inData.fSources[nFileIndex] << endl;
				break;
			}
			nRightIndex = 0;
			nRightCount = cfRight.GetPacketCount();

			if ( inData.IsVerbose() ) {
				wcout << "Comparing " << nRightCount << " of " << (nCount - nCompare) << " remaining packets." << endl;
			}
			if ( !cfRight.ReadFilePacket( pktRight ) ) {
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
		if ( pktLeft.Compare( pktRight, inData.fFlags, &strError ) ) {
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
