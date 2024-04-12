// =============================================================================
//	main
// =============================================================================
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 2015-2017 Savvius, Inc. All rights reserved.
//	Copyright (c) 2017-2023 LiveAction, Inc. All rights reserved.

#include "StdAfx.h"
#include "readerlist.h"
#include "merge.h"
#include "compare.h"
#include "report.h"
#include "CaptureFile/CaptureFile.h"

#include <filesystem>
#include <iostream>
#include <regex>
#include <vector>

#ifdef _LINUX
#include <dirent.h>
#include <unistd.h>
#endif

using namespace std;
using std::filesystem::path;
using namespace CaptureFile;


// =============================================================================
//		Data Structures
// =============================================================================

class CPacketRange
{
private:
	UInt64	m_Begin;
	UInt64	m_End;

public:
	;		CPacketRange( UInt64 begin = 0, UInt64 end = 0 )
	{
		if ( begin <= end ) {
			m_Begin = begin;
			m_End = end;
		}
		else {
			m_Begin = end;
			m_End = begin;
		}
	}

	UInt64	begin() const { return m_Begin; }
	UInt64	end() const { return m_End; }
	
	bool operator < ( const CPacketRange& other ) const {
		return (begin() < other.begin());
	}

	bool	IsIn( UInt64 index ) const {
		return (index >= m_Begin) && (index <= m_End);
	}
};

typedef vector<CPacketRange>	CRangeList;

struct tAppendData {
	CPathList		fSources;
	path			fDestination;
	CRangeList*		pRanges;
	bool			fVerbose;

	bool	IsVerbose() const { return fVerbose; }
};

struct tTestData {
	CPathList	fSources;
	path		fDestination;
	bool		fVerbose;

	bool	IsVerbose() const { return fVerbose; }
};


struct csv_reader: std::ctype<char> {
    csv_reader(): std::ctype<char>(get_table()) {}
    static std::ctype_base::mask const* get_table() {
        static std::vector<std::ctype_base::mask> rc(table_size, std::ctype_base::mask());

        rc[','] = std::ctype_base::space;
        rc['\n'] = std::ctype_base::space;
        rc[' '] = std::ctype_base::space;

        return &rc[0];
    }
};


vector<string>&
split(
	const string	&s,
	char			delim,
	vector<string>	&elems)
{
    stringstream ss(s);
    string item;
    while ( std::getline( ss, item, delim ) ) {
        elems.push_back( item );
    }
    return elems;
}


vector<string>
split(
	const string&	s,
	char			delim )
{
    vector<string> elems;
    split( s, delim, elems );
    return elems;
}


// =============================================================================
//		ExpandFiles
// =============================================================================

CPathList
ExpandFiles(
	CPathList	inSources )
{
	CPathList		aySources;
	for ( path pathSource : inSources ) {
		if ( pathSource.empty() ) continue;
		if ( pathSource.is_relative() ) {
			pathSource = std::filesystem::absolute( pathSource );
		}

		if ( pathSource.has_filename() && std::filesystem::exists( pathSource ) ) {
			aySources.push_back( pathSource.wstring() );
		}
		else {
			string strName( pathSource.filename().string() );
			string strExt( pathSource.extension().string() );
			path pathDirectory( pathSource.parent_path() );
			for ( auto const& dirEntry : std::filesystem::directory_iterator( pathDirectory ) ) {
				if ( dirEntry.is_regular_file() ) {
					if ( dirEntry.path().has_extension() ) {
						string	strFileExt( dirEntry.path().extension().string() );
						if ( strFileExt == strExt) {
							aySources.push_back( dirEntry.path() );
						}
					}
					else {
						aySources.push_back( dirEntry.path() );
					}
				}
			}
		}
	}

	return aySources;
}


// =============================================================================
//		IsNumeric
// =============================================================================

bool
IsNumeric(
	const string& inValue )
{
	size_t	nCount = inValue.size();
	if ( nCount == 0 ) return false;

	for ( size_t i = 0; i < nCount; i++ ) {
		if ( !((inValue[i] >= '0') && (inValue[i] <= '9')) ) {
			return false;
		}
	}
	return true;
}


// =============================================================================
//		DisplayOptions
// =============================================================================

void
DisplayOptions()
{
	wcout << endl;
	wcout << L"pktman - Packet Manager v2.1" << endl;
	wcout << L"Copyright (C) 2011-2023, LiveAction, Inc." << endl;
	wcout << endl;
	wcout << L"Packet Manager performs operations on capture/trace file." << endl;
	wcout << endl;
	wcout << L"usage: pktman [-(m[i]|c[celt])][-t <n>][-d <time>][-v] -i <file> [-i <file>]* -o <file> [-s <indexes>]*" << endl;
	wcout << L"  -a    Append:         Creates one file from all the source files." << endl;
	wcout << L"  -c[c][e(d|s)][l][t]:  Compare first two input files." << endl;
	wcout << L"                        c: don't compare CRCs." << endl;
	wcout << L"                        e(d|s): don't compare Ethernet (MAC) addresses." << endl;
	wcout << L"                          Source (ed), destination (es), or both (eds)." << endl;
	wcout << L"                        l: Match sliced packet data with first part of full packet data." << endl;
	wcout << L"                        t: don't compare time stamps." << endl;
	wcout << L"  -d    Delta time:     Set the delta time, n in nanoseconds, between packets." << endl;
	wcout << L"  -i    Input:          Source file name. May include wildcards (*?)." << endl;
	wcout << L"  -o    Output:         Output file name." << endl;
	wcout << L"  -m[i] Merge:          Merge source/input files." << endl;
	wcout << L"                        i: Interleave files." << endl;
	wcout << L"  -s    Select packets: A list of packet numbers and packet ranges." << endl;
	wcout << L"  -r[p] Report:         Generates a report on each input file." << endl;
	wcout << L"                        p: Track and display unsupported protocols." << endl;
	wcout << L"  -t    Trim:           Keeps every nth packet." << endl;
	wcout << L"  -v    Verbosity:      Verbose messages." << endl;
	wcout << L"  -?, --help Help:      Display this help text." << endl;
	wcout << endl;
	wcout << L"All options are case sensitive." << endl;
	wcout << endl;
	wcout << L"Append will append all the input files into the first output file." << endl;
	wcout << L"If packets are selected then only the packets selected are written to the output file." << endl;
	wcout << endl;
	wcout << L"Compare: Compare the contents of at least two source files. If more than two" << endl;
	wcout << L"files, then the first is compared to the rest in the order specified." << endl;
	wcout << L"Choose to ignore: CRC, Ethernet addresses, and time stamps." << endl;
	wcout << L"Choose to compare sliced packets to full packets." << endl;
	wcout << endl;
	wcout << L"Merge: Merge multiple source files into a single file." << endl;
	wcout << L"The packets are interleaved based on the relative timestamp of" << endl;
	wcout << L"each packet within its file. Only files of the same media type" << endl;
	wcout << L"(wired/wireless) as the first source file are merged." << endl;
	wcout << L"The -i option interleaves file one from each file." << endl;
	wcout << endl;
	wcout << L"Supported file formats:" << endl;
	wcout << L"  .pkt        WildPackets Packet files." << endl;
	wcout << L"  .pcap .dmp  Libpcap file." << endl;
	wcout << L"  .pcapng     Libpcap file." << endl;
	wcout << endl;
	wcout << L"Examples:" << endl;
	wcout << L"Save packets 1, 5 through 10, and 20 into a file: pktman -a -i file1.pkt -o three.pkt -s 1,5-10,20" << endl;
	wcout << L"pktman -m -i file1.pkt -i file2.pcap -o merge.pkt" << endl;
	wcout << L"pktman -m -o allfiles.pkt -i *.pkt" << endl;
	wcout << L"Keep every other packet: pktman -m -i file1.pkt -o thin.pkt -t" << endl;
	wcout << L"Keep every third packet: pktman -m -i file1.pkt -o thin.pkt -t 3" << endl;
	wcout << L"pktman -cceds -i file1.pkt -i file2.pkt" << endl;
	wcout << endl;
}


// =============================================================================
//		AppendEcho
// =============================================================================

void
AppendEcho(
	const path&	inPath )
{
	wcout << L"  " << inPath.wstring() << endl;
}


// =============================================================================
//		Append
// =============================================================================

int
Append(
	tAppendData&	inData )
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
		for_each( inData.fSources.begin(), inData.fSources.end(), AppendEcho );

		wcout << L"Output file:" << endl;
		wcout << L"  " << inData.fDestination << endl;

		wcout << endl;
	}

	CReaderList	ayInFiles;
	nCount = ayInFiles.Load( inData.fSources );

	tFileMode		fmMode( CF_TYPE_WRITER, CF_FORMAT_UNKNOWN, CF_ACCESS_SERIAL, false, ayInFiles.GetMediaType() );
	CCaptureFile	cwOut( inData.fDestination, fmMode );

	CFilePacket	pkt;
	if ( inData.pRanges ) {
		UInt64				nIndex = 0;
		for ( auto range = inData.pRanges->begin(); range != inData.pRanges->end(); ++range ) {
			while ( ayInFiles.GetNextPacket( pkt ) ) {
				++nIndex;
				if ( range->IsIn( nIndex ) ) {
					cwOut.WriteFilePacket( pkt );
					if ( nIndex == range->end() ) break;
				}
			}
		}
	}
	else {
		UInt32	nPktCount( 0 );
		if ( ayInFiles.GetNextPacket( pkt ) ) {
			cwOut.WriteFilePacket( pkt );
			while ( ayInFiles.GetNextPacket( pkt ) ) {
				cwOut.WriteFilePacket( pkt );
				++nPktCount;
			}
		}

		if ( inData.fVerbose ) {
			wcout << L"Packets: " << nPktCount << endl;
		}
	}

	ayInFiles.Close();
	cwOut.Close();

	return 0;
}


// =============================================================================
//		Test
// =============================================================================

int
Test(
	tTestData&	inData )
{
	wcout.imbue( std::locale( "" ) );	// 1,000,000

	if ( inData.fVerbose ) {
		wcout << L"Start Test" << endl;
	}

	CFileTime	ftStart( CFileTime::Now() );
	wcout << L"Start Time: " << ftStart.Format( CFileTime::kFormat_Seconds ) << endl;

	CCaptureFile	cfFirst( inData.fSources[0], CF_TYPE_READER );
	if ( cfFirst.IsOpen() ) {
		cfFirst.Close();
	}

	CFileTime	ftEnd( CFileTime::Now() );
	wcout << L"End Time: " << ftEnd.Format( CFileTime::kFormat_Seconds ) << endl;
	wcout << L"Duration: " << (ftEnd - ftStart) << endl;

	return 0;
}


// =============================================================================
//		main
// =============================================================================

int
main(
	int		inArgc,
	char*	argv[] )
{
	bool			bArgumentError = false;
	int				nAction = kAction_None;
	bool			bDeltaTime = false;
	UInt64			nDeltaDuration = 1000;		// 1 millisecond.
	bool			bThinOut = false;
	size_t			nThinEvery = 0;
	bool			bNoCRC = false;
	bool			bNoEthernetSource = false;
	bool			bNoEthernetDestination = false;
	bool			bInterleave = false;
	bool			bMatchSlice = false;
	bool			bNoTimeStamp = false;
	bool			bPorts = false;
	bool			bVerbose = false;
	CPathList		aySources;
	path			pathDestination;
	bool			bSelectPackets = false;
	CRangeList		ayRanges;

	for ( int i = 1; i < inArgc; i++ ) {
		if ( *argv[i] == '-' ) {
			switch ( argv[i][1] ) {
			case 'a':
				nAction = kAction_Append;
				break;

			case 'c':
				{
					nAction = kAction_Compare;
					int	n = 2;
					while ( argv[i][n] != 0 ) {
						switch ( argv[i][n] ) {
						case 'c':
							bNoCRC = true;
							n++;
							break;

						case 'e':
							{
								n++;
								if ( argv[i][n] == 'd' ) {
									bNoEthernetDestination = true;
									n++;
									if ( argv[i][n] == 's' ) {
										bNoEthernetSource = true;
										n++;
									}
								}
								if ( argv[i][n] == 's' ) {
									bNoEthernetSource = true;
									n++;
									if ( argv[i][n] == 'd' ) {
										bNoEthernetDestination = true;
										n++;
									}
								}
							}
							break;

						case 'l':
							bMatchSlice = true;
							n++;
							break;

						case 't':
							bNoTimeStamp= true;
							n++;
							break;

						default:
							bArgumentError = true;
							n++;
							break;
						}
					}
				}
				break;

			case 'd':
				{
					bDeltaTime = true;
					if ( (i + 1) < inArgc ) {
						string strDuration( argv[i+1] );
						if ( IsNumeric( strDuration ) ) {
							istringstream	ssDuration( strDuration );
							size_t	nDuration;
							ssDuration >> nDuration;
							if ( nDuration > 1000 ) {
								nDeltaDuration = nDuration;
							}
							i++;
						}
					}
				}
				break;

			case 'i':
				i++;
				aySources.push_back( argv[i] );
				break;

			case 'm':
				{
					nAction = kAction_Merge;
					int	n = 2;
					while ( argv[i][n] != 0 ) {
						switch ( argv[i][n] ) {
						case 'i':
							bInterleave = true;
							n++;
							break;

						default:
							bArgumentError = true;
							n++;
							break;
						}
					}
				}
				break;

			case 'o':
				i++;
				pathDestination = argv[i];
				break;

			case 'r':
				{
					nAction = kAction_Report;
					int	n = 2;
					while ( argv[i][n] != 0 ) {
						switch ( argv[i][n] ) {
						case 'p':
							bPorts = true;
							break;

						default:
							bArgumentError = true;
							break;
						}
						n++;
					}
				}
				break;

			case 's':
				{
					bSelectPackets = true;

					if ( (i+1) < inArgc ) {
						string			str( argv[i+1] );
						str.erase( std::remove( str.begin(), str.end(), ' ' ), str.end() );
						vector<string> ranges = split(str, ',' );
						for ( auto r = ranges.begin(); r != ranges.end(); ++r ) {
							size_t offset = r->find( '-' );
							if ( offset == string::npos ) {
								UInt64 begin = stoul( r->c_str(), 0, 10 );
								ayRanges.push_back( CPacketRange( begin, begin ) );
							}
							else {
								size_t index = 0;
								UInt64 begin = stoul( r->c_str(), &index, 10 );
								++index;
								UInt64 end = stoul( r->c_str() + index, &index, 10 );
								ayRanges.push_back( CPacketRange( begin, end ) );
							}
						}
						i++;
					}
				}
				break;

			case 't':
				{
					bThinOut = true;
					nThinEvery = 2;
					if ( (i+1) < inArgc ) {
						string strEvery( argv[i+1] );
						if ( IsNumeric( strEvery ) ) {
							istringstream	ssEvery( strEvery );
							size_t	nEvery;
							ssEvery >> nEvery;
							if ( nEvery > 2 ) {
								nThinEvery = nEvery;
							}
							i++;
						}
					}
				}
				break;

			case 'v':
				bVerbose = true;
				break;

			case 'x':
				break;

			case '-':
				{
					string arg( argv[i] );
					string cmd( arg.substr( 2 ) );
					if ( cmd == "help" ) {
						DisplayOptions();
					}
				}
				break;

			case '?':
			default:
				DisplayOptions();
				return 0;
			}
		}
	}

	if ( bArgumentError ) {
		DisplayOptions();
		return 0;
	}

	path	pathCurrent( std::filesystem::current_path() );

#ifdef _DEBUG
	std::filesystem::current_path( pathCurrent.parent_path() );
	pathCurrent = std::filesystem::current_path();
#endif

	if ( bVerbose ) {
		wcout << L"pktman v2.1" << endl;
		wcout << pathCurrent.wstring() << endl;
	}

	std::sort( ayRanges.begin(), ayRanges.end() );

	int	nResult( 0 );
	switch ( nAction ) {
		case kAction_Merge:
			{
				tMergeData	dataMerge;
				dataMerge.fSources = ExpandFiles( aySources );
				dataMerge.fDestination = pathDestination;
				dataMerge.fDeltaTime = bDeltaTime;
				dataMerge.fDeltaDuration = nDeltaDuration;
				dataMerge.fInterleave = bInterleave;
				dataMerge.fThinOut = bThinOut;
				dataMerge.fThinEvery = nThinEvery;
				dataMerge.fVerbose = bVerbose;
				nResult = MergeFiles( dataMerge );
			}
			break;

		case kAction_Append:
			{
				tAppendData	dataAppend;
				dataAppend.fSources = ExpandFiles( aySources );
				dataAppend.fDestination = std::filesystem::absolute( pathDestination );
				dataAppend.pRanges = (bSelectPackets) ? &ayRanges: nullptr;
				dataAppend.fVerbose = bVerbose;
				nResult = Append( dataAppend );
			}
			break;

		case kAction_Compare:
			{
				tCompareData	dataCompare;
				dataCompare.fSources = ExpandFiles( aySources );
				dataCompare.fDestination = pathDestination;
				dataCompare.fFlags = CFilePacket::kFlag_None;
				if ( bNoTimeStamp ) {
					dataCompare.fFlags |= CFilePacket::kFlag_NoTimeStamp;
				}
				if ( bNoCRC ) {
					dataCompare.fFlags |= CFilePacket::kFlag_NoCRC;
				}
				if ( bNoEthernetDestination ) {
					dataCompare.fFlags |= CFilePacket::kFlag_NoEthernetDestination;
				}
				if ( bNoEthernetSource ) {
					dataCompare.fFlags |= CFilePacket::kFlag_NoEthernetSource;
				}
				if ( bMatchSlice ) {
					dataCompare.fFlags |= CFilePacket::kFlag_MatchSlice;
				}
				dataCompare.fVerbose = bVerbose;

				nResult = CompareFiles( dataCompare );
			}
			break;

		case kAction_Report:
			{
				tReportData	dataReport;
				dataReport.fSources = ExpandFiles( aySources );
				dataReport.fDestination = "cout";
				dataReport.fFlags = 0x00;
				if ( bPorts ) {
					dataReport.fFlags |= CFilePacket::kFlag_Ports;
				}
				dataReport.fVerbose = bVerbose;
				nResult = Report( dataReport );
			}
			break;

		case kAction_Test:
			{
				tTestData	dataTest;
				dataTest.fSources = ExpandFiles( aySources );
				dataTest.fDestination = std::filesystem::absolute( pathDestination );
				dataTest.fVerbose = bVerbose;
				nResult = Test( dataTest );
			}
			break;

		case kAction_None:
		default:
			DisplayOptions();
			nResult = 0;
			break;
	}

	return nResult;
}
