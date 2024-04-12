// =============================================================================
//	report.cpp
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "StdAfx.h"
#include "report.h"
#include "CaptureFile/CaptureFile.h"
#include "CaptureFile/Packet.h"
#include <iostream>						// needed for wcout

#ifdef _DEBUG
#include <filesystem>
#endif

using namespace std;
using namespace CaptureFile;

using std::wcout;
using std::wcerr;


// =============================================================================
//		VerboseEcho
// =============================================================================

void
ReportEcho(
	const path&	inPath )
{
	wcout << L"  " << inPath.wstring() << endl;
}


// =============================================================================
//		Report
// =============================================================================

int
Report(
	const tReportData&	inData )
{
	wcout << L"Report" << endl;

	if ( inData.fSources.size() < 1 ) {
		wcout << L"At least 1 source files (-i) must be specified." << endl;
		wcout << endl;
		return -1;
	}

	if ( inData.IsVerbose() ) {
		wcout << L"Input files:" << endl;
		for_each( inData.fSources.begin(), inData.fSources.end(), ReportEcho );
		wcout << endl;
	}

    bool    bPorts( inData.fFlags & CFilePacket::kFlag_Ports );
    std::vector<UInt16> ayAllPorts;

    UInt32  nErrorCount( 0 );
    UInt32  nResult( 0 );
    for ( const path& pathSource : inData.fSources ) {
        if ( inData.IsVerbose() ) {
            wcout << "Open: " << pathSource.wstring() << endl;

#ifdef _DEBUG
            wcout << "Bytes: " << dec << std::filesystem::file_size( pathSource ) << endl;
#endif
        }
        CCaptureFile	cfReport( pathSource, CF_TYPE_READER );
        if ( !cfReport.IsOpen() ) {
            wcout << "Failed to open: " << pathSource.wstring() << endl;
            wcout << endl;
            nErrorCount++;
            continue;
        }

    	size_t	        nCount( 0 );
        UInt32	        nError( 0 );
        CFilePacket		pktReport;
        std::vector<UInt16> ayPorts;

        wcout << endl << pathSource.filename().wstring() << endl;
        
        wcout <<
            setw(  6 ) << L"Packet" << L"  " <<
            setw( 17 ) << L"Destination" << L"  " <<
            setw( 17 ) << L"Source" << L"  " <<
            setw(  4 ) << L"Port" << L"  " <<
            setw( 15 ) << L"Destination" << L"  " <<
            setw( 15 ) << L"Source" << L"  " <<
            setw(  4 ) << L"Port" << L"  " <<
            setw(  4 ) << L"Length" << L"  " <<
            endl;

        while ( cfReport.ReadFilePacket( pktReport, nError ) ) {
#ifdef _DEBUG
            // CByteVectorStream   bvs( pktReport.GetPacketDataLength(), pktReport.GetPacketData() );
            // wstring             strData( bvs.Format() );
            // wcout << strData << endl;
#endif

            CPacket pkt;
            if ( pktReport.ParsePacketData( pkt ) ) {
                ++nCount;

                const CEthernetHeader&  eth( pkt.GetEthernetHeader() );
                if ( bPorts ) {
                    if ( std::find(ayPorts.begin(), ayPorts.end(), eth.m_nProtocolType) == ayPorts.end() ) {
                        ayPorts.push_back( eth.m_nProtocolType );
                    }
                }

                const CIPHeader*        pIP( pkt.GetIPHeader() );
                if ( pIP ) {
                    wcout << 
                        setw( 6 ) << setfill( L' ' ) << dec << nCount << L"  " <<
                        setw( 17 ) << eth.m_Destination.Format() << L"  " <<
                        setw( 17 ) << eth.m_Source.Format() << L"  " <<
                        setw( 4 ) << setfill( L'0' ) << hex << eth.m_nProtocolType << L"  " <<
                        setw( 15 ) << setfill( L' ' ) << pIP->Destination().Format() << L"  " <<
                        setw( 15 ) << setfill( L' ' ) << pIP->Source().Format() << L"  " <<
                        setw( 4 ) << setfill( L'0' ) << hex << pktReport.GetProtocol() << L"  " <<
                        setw( 4 ) << setfill( L' ' ) << dec << pktReport.GetPacketDataLength() << L"  " <<
                        endl;
                }
                else {
                    wcout << 
                        setw( 6 ) << setfill( L' ' ) << dec << nCount << L"  " <<
                        setw( 17 ) << eth.m_Destination.Format() << L"  " <<
                        setw( 17 ) << eth.m_Source.Format() << L"  " <<
                        setw( 4 ) << setfill( L'0' ) << hex << eth.m_nProtocolType << L"  " <<
                        setw( 15 ) << setfill( L' ' ) << L" " << L"  " <<
                        setw( 15 ) << setfill( L' ' ) << L" " << L"  " <<
                        setw( 4 ) << setfill( L'0' ) << hex << pktReport.GetProtocol() << L"  " <<
                        setw( 4 ) << setfill( L' ' ) << dec << pktReport.GetPacketDataLength() << L"  " <<
                        endl;
                }
            }
            // else {
            //     wcerr << L"Bad Packet or too small - " << pktReport.GetPacketDataLength() <<
            //         L" bytes" << endl;
            // }
        }

        wcout << endl << nCount << " Packets" << endl;

	    if ( bPorts ) {
            std::sort( ayPorts.begin(), ayPorts.end() );
            wcout << endl;
            wcout << L"Unsupported ProtocolTypes in " << pathSource.wstring() << L":" << endl;
            int nColumn( 1 );
            for ( auto port : ayPorts ) {
                wcout << setw( 6 ) << hex << port;
                if ( (nColumn % 8) == 0 ) {
                    wcout << endl;
                    nColumn = 1;
                }
                else {
                    ++nColumn;
                }

                if ( std::find(ayAllPorts.begin(), ayAllPorts.end(), port) == ayAllPorts.end() ) {
                    ayAllPorts.push_back( port );
                }
            }
            wcout << endl;
        }

        cfReport.Close();
        nResult++;
    	wcout << endl;
    }

	wcout << endl;

	if ( bPorts ) {
        wcout << endl;

        std::sort( ayAllPorts.begin(), ayAllPorts.end() );
        wcout << endl;
        wcout << L"All unsupported ProtocolTypes:" << endl;
        int nColumn( 1 );
        for ( auto port : ayAllPorts ) {
            wcout << setw( 6 ) << hex << port;
            if ( (nColumn % 8) == 0 ) {
                wcout << endl;
                nColumn = 1;
            }
            else {
                ++nColumn;
            }
        }
        wcout << endl;
    }

	return nResult;
}
