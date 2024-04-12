// sql2dcd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "pkt2dcd.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "PeekPacket.h"
#include "DecodeManager.h"
#include "PacketLoader.h"
#include "NameTableReader.h"

DECODE_LIB_REF theDecodeContext;

using namespace std;


 // ----------------------------------------------------------------------------
 //		ProcessFile
 // ----------------------------------------------------------------------------

int 
ProcessFile(
	const CString&	inFile,
	const CString&	outFile,
	CODE_PAGE		inCodePage )
{

	// open the input file
	CPacketLoader loader;
	if ( !loader.Open( inFile ) ) {
		printf( "Could not open the input file\n" );
		return -1;
	}

	DECODE_INTERPRET_FORMAT	theFormat = DECODE_INTERPRET_FORMAT_PLAIN_TEXT;
	DECODE_OPTIONS			theDecodeOptions = DECODE_OPTIONS_SHOW_OFFSETS;

	bool success = CDecodeManager::SaveFile( &loader, outFile, theFormat, inCodePage, theDecodeOptions );

	loader.Close();

	return (success) ? 0 : -1;
 }


// -----------------------------------------------------------------------------
//		Usage
// -----------------------------------------------------------------------------

void
Usage()
{
	//       012345678901234567890123456789012345678901234567890123456789012345678901234567890
	//                 1         2         3         4         5         6         7         8
	printf( "\npkt2dcd v1.5 Copyright Savvius, Inc. All rights reserved.\n" );
	printf( "\nUsage: pkt2dcd infile.pkt <outfile> <options>\n" );
	printf( "Output to console if outfile is not specified.\n" );
#ifdef _WIN64
	printf( "-p path to 64-bit OmniPeek install to use it's Decoder library.\n" );
#else
	printf( "-p path to 32-bit OmniPeek install to use it's Decoder library.\n" );
#endif
	printf( "   Default path is the current folder\n" );
	printf( "-c codepage for multi-byte character decoding\n" );
	printf( "   0     - ascii\n" );
	printf( "   65001 - utf-8\n" );
	printf( "   28596 - Baltic-ISO (iso-8859-6)\n" );
	printf( "   1257  - Baltic-Windows (windows-1257)\n" );
	printf( "   28592 - Central European ISO (iso-8859-2)\n" );
	printf( "   1250  - Central  European Windows (windows-1250)\n" );
	printf( "   1256  - Arabic Windows (windows-1256)\n" );
	printf( "   51936 - EUC-Chinese Simplified\n" );
	printf( "   936	- Chinese Simplified GB2312\n" );
	printf( "   54936 - Chinese Simplified GB18030\n" );
	printf( "   950   - Chinese Traditional Big5\n" );
	printf( "   28595 - Cyrillic ISO (iso-8859-5)\n" );
	printf( "   20866 - Cyrillic KOI8-R\n" );
	printf( "   21866 - Cyrillic KOI8-U\n" );
	printf( "   1251  - Cyrillic Windows\n" );
	printf( "   37    - EBCDIC\n" );
	printf( "   28597 - Greek ISO (iso-8859-7)\n" );
	printf( "   1253  - Greek Windows\n" );
	printf( "   28598 - Hebrew ISO (iso-8859-8)\n" );
	printf( "   1255  - Hebrew Windows\n" );
	printf( "   20932 - EUC-Japanese\n" );
	printf( "   932   - Japanese shift-jis\n" );
	printf( "   51949 - EUC-Korean\n" );
	printf( "   874   - Thai Windows\n" );
	printf( "   28599 - Turkish ISO (iso-8859-9)\n" );
	printf( "   1254  - Turkish Windows\n" );
	printf( "   28591 - Western European ISO (iso-8859-1)\n" );
	printf( "   1252  - Western European Windows\n" );
	printf( "   1258  - Vietnamese Windows\n" );
}


// -----------------------------------------------------------------------------
//		main
// -----------------------------------------------------------------------------

int
main(
	int		argc,
	char*	argv[] )
{
	wchar_t	strWorking[MAX_PATH];
	::GetCurrentDirectoryW( MAX_PATH, strWorking );

	if ( argc < 2 ) {
		Usage();
		return -1;
	}
	
	int		nNextArg = 1;
	CString	inFile;
	inFile = argv[nNextArg++];
	inFile.Remove( '\'' );

	CString		libraryPath = L"";
	CString		outFile;
	unsigned	int codePage = 0;

	bool bGoodArgs = true;

	if ( (argc > 2) && argv[nNextArg][1] != '-' ) {
		outFile = argv[nNextArg++];
	}

	// iterate through arguments
	for ( int i = nNextArg; i < argc; i += 2 ) { 
		if ( (i + 1) != argc ) {
			if ( !strcmp( argv[i], "-o" ) ) {
				outFile = argv[i + 1];
				outFile.Remove( '\'' );
			}
			else if ( !strcmp( argv[i], "-p" ) ) {
				libraryPath = argv[i + 1];
			}
			else if( !strcmp( argv[i], "-c" ) ) {
				codePage = atoi( argv[i + 1] );
			}
			else {
				bGoodArgs = false;
			}
		}
		else {
			bGoodArgs = false;
		}

		if ( !bGoodArgs ) {
			printf( "Not enough or invalid arguments.\n" );
			Usage();
			Sleep( 2000 );
			exit( -1 );
		}
	}

	int nRetCode = 0;
	  
	// initialize MFC and print and error on failure
	if ( !AfxWinInit( ::GetModuleHandle( NULL ), NULL, ::GetCommandLine(), 0 ) ) {
		// TODO: change error code to suit your needs
		printf( "Fatal Error: MFC initialization failed\n" );
		nRetCode = -1;
	}
	
	// set up decoders
	DECODE_RESULT dResult = CDecodeManager::Load( libraryPath );
	if ( dResult != DECODE_RESULT_NOERROR ) return -1;
	// print out packets
	ProcessFile( inFile, outFile, codePage );

	// unload decoders
	dResult = CDecodeManager::Unload();
	if ( dResult != DECODE_RESULT_NOERROR ) return -1;

	return nRetCode;
}
