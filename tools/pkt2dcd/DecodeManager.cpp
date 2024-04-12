// =============================================================================
//	DecodeManager.cpp
// =============================================================================
//  Copyright (c) 2015-2016 Savvius, Inc. All rights reserved
//	Copyright (c) 1999-2015 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "DecodeManager.h"
#include "filepath.h"
#include "Resource.h"

#if defined(_DEBUG) && defined(_AFX)
#define new DEBUG_NEW
#endif

typedef DECODE_RESULT(WINAPI *DcodLoadDecoders_)( DECODE_LOCATION pDecoderDirectory, DECODE_LIB_REF* outLibRef );
typedef DECODE_RESULT(WINAPI *DcodUnloadDecoders_)( DECODE_LIB_REF inLibRef );
typedef DECODE_RESULT(WINAPI *DcodPacketToTagStream_)( DECODE_LIB_REF inLibRef, const DECODE_PACKET* inPacket,
	DECODE_OPTIONS inOptions, CODE_PAGE inCodePage, DECODE_TAG_STREAM* outTagStream, XDebugger* pDebugger );
typedef DECODE_RESULT(WINAPI *DcodFreeTagStream_)( DECODE_TAG_STREAM* inTagStream );
typedef DECODE_RESULT(WINAPI *DcodInterpretTagStream_)( DECODE_LIB_REF inLibRef, DECODE_INTERPRET_FORMAT inFormat,
	const DECODE_PACKET* inPacket, DECODE_OPTIONS inOptions, CODE_PAGE inCodePage,
	const DECODE_TAG_STREAM* inTagStream, DECODE_TEXT_STREAM* outTextStream );
typedef DECODE_RESULT(WINAPI *DcodFreeTextStream_)( DECODE_TEXT_STREAM* inTextStream) ;
typedef DECODE_RESULT(WINAPI *DcodFormatTextHeader_)( DECODE_INTERPRET_FORMAT inFormat, DECODE_TEXT_STREAM* outTextStream );
typedef DECODE_RESULT(WINAPI *DcodFormatTextFooter_)( DECODE_INTERPRET_FORMAT inFormat, DECODE_TEXT_STREAM* outTextStream );
typedef DECODE_RESULT(WINAPI *DcodGetCallbacks_)( DECODE_LIB_REF inLibRef, DECODE_CALLBACKS* outCallbacks) ;
typedef DECODE_RESULT(WINAPI *DcodSetCallbacks_)( DECODE_LIB_REF inLibRef, DECODE_CALLBACKS* inCallbacks );


// =============================================================================
//		CDecodeManager
// =============================================================================

DECODE_LIB_REF		CDecodeManager::s_LibRef = 0;
CNameTableReader*	CDecodeManager::s_nameTable = NULL;
HMODULE				CDecodeManager::s_hLib = NULL;


// -----------------------------------------------------------------------------
//		Load [static]
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::Load(
	CString inDcodFolder )
{
	DECODE_RESULT	nResult = DECODE_RESULT_NOERROR;

	if ( s_LibRef == 0 ) {
		std::wstring	strDcodFolder( inDcodFolder );

		FilePath::PathW	pathDecodeLib( strDcodFolder );
		pathDecodeLib.Append( L"Dcodu.dll" );
		s_hLib = ::LoadLibraryW( pathDecodeLib.get().c_str() );
		if ( s_hLib == NULL ) return DECODE_RESULT_INIT_FAILED;

		DcodLoadDecoders_	f = (DcodLoadDecoders_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodLoadDecoders", "8" ) );
		if ( f == NULL ) {
			::FreeLibrary( s_hLib );
			s_hLib = NULL;
			return DECODE_RESULT_INIT_FAILED;
		}

		FilePath::PathW	pathDecodes( strDcodFolder );
		if ( !inDcodFolder.IsEmpty() ) {
			pathDecodes.Append ( L"Decodes" );
		}
		nResult = (*f)( pathDecodes.get().c_str(), &s_LibRef );
		if ( nResult != DECODE_RESULT_NOERROR ) {
			::FreeLibrary( s_hLib );
			s_hLib = NULL;
			s_LibRef = 0;
			return nResult;
		}

		// Setup our callback procedures.
		DECODE_CALLBACKS	theCallbacks;
		memset( &theCallbacks, 0, sizeof( theCallbacks ) );
		nResult = GetCallbacks( s_LibRef, &theCallbacks );
		theCallbacks.fNameLookupProc = CDecodeManager::NameLookup;
		SetCallbacks( s_LibRef, &theCallbacks );
	}

	if ( s_nameTable == NULL ) {
		s_nameTable = new CNameTableReader();
		if ( s_nameTable != NULL ) {
			// Use the environment variable APPDATA.
			CString	strAppData;
			strAppData.GetEnvironmentVariable( L"APPDATA" );

			CString strTablePath = strAppData + L"\\Savvius\\OmniPeek\\Names.nam";
			s_nameTable->LoadFromFile( strTablePath.GetString() );
		}
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		Unload [static]
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::Unload()
{
	DECODE_RESULT	nResult = DECODE_RESULT_NOERROR;

	if ( s_LibRef != 0 ) {
		DcodUnloadDecoders_	fUnloadDecoders = (DcodUnloadDecoders_) ::GetProcAddress( s_hLib, DecorateSymbolName("DcodUnloadDecoders", "4"));
		if ( fUnloadDecoders != NULL ) {
			nResult = (*fUnloadDecoders)( s_LibRef );
		}
		s_LibRef = 0;

		::FreeLibrary( s_hLib );
		s_hLib = NULL;
	}

	if ( s_nameTable != NULL ) {
		delete s_nameTable;
		s_nameTable = NULL;
	}

	return nResult;
}


// -----------------------------------------------------------------------------
//		SaveFile [static]
// -----------------------------------------------------------------------------

bool
CDecodeManager::SaveFile (
	CPacketLoader*			pPacketLoader,
	const CString&			inFilePath,
	DECODE_INTERPRET_FORMAT inFormat,
	CODE_PAGE				inCodePage,
	DECODE_OPTIONS			inDecodeOptions )
{
	// Create and open the file.
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	if ( inFilePath.IsEmpty() ) {
		hFile = ::GetStdHandle( STD_OUTPUT_HANDLE );
	}
	else {
		hFile = ::CreateFile( inFilePath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	}
	if ( hFile == INVALID_HANDLE_VALUE ) {
		const DWORD	dwResult = ::GetLastError();
		return false;
	}

	// Setup the decode options and specifically clear
	// the Query Unknown Types option.
	inDecodeOptions &= ~DECODE_OPTIONS_QUERY_UNKNOWN_TYPES;

	// Write the file header.
	DECODE_TEXT_STREAM	theTextStream;
	if ( FormatTextHeader( inFormat, &theTextStream ) == DECODE_RESULT_NOERROR ) {
		// Get the length of the text in the stream.
		size_t nLength = strlen( (char*)theTextStream.pTextStream );
		if ( nLength > 0 ) {
			// Write the text.
			DWORD	nBytesWritten = 0;
			if ( !::WriteFile( hFile, theTextStream.pTextStream,
					static_cast<DWORD>( nLength ), &nBytesWritten, NULL ) ) {
				DWORD	nResult = ::GetLastError();
				if ( !inFilePath.IsEmpty() ) {
					::CloseHandle( hFile );
				}
				::DeleteFile( inFilePath );
				return false;
			}
		}

		// Free the text stream.
		FreeTextStream( &theTextStream );
	}

	const CStringA strPktNum( "Packet #" );

	UInt8	ayData[USHRT_MAX];
	UInt16	nLength = 0;
	UInt64	nPacketNumber = 0;
	bool	bAddSpacing = false;

	while ( pPacketLoader->GetNextPacket( ayData, nLength ) ) {
		CHAR	szTemp[256];
		DWORD	nBytesWritten = 0;

		//add spacing before printing a new packet if there is additional text above
		if( bAddSpacing ) {
			strcpy( szTemp, "\r\n\r\n" );

			if( !::WriteFile( hFile, szTemp, static_cast<DWORD>( strlen( szTemp ) ),
					&nBytesWritten, NULL ) ) {
				const DWORD	dwResult = ::GetLastError();
				if ( !inFilePath.IsEmpty() ) {
					::CloseHandle( hFile );
				}
				::DeleteFile( inFilePath );
				return false;
			}
		}

		bAddSpacing = true;

		PeekPacket thePacket = *(pPacketLoader->GetPacketHeader());

		thePacket.fMediaSpecInfoBlock = (UInt8 *)pPacketLoader->GetMediaSpecificHeader();

		DECODE_OPTIONS  theDecodeOptions = DECODE_OPTIONS_SHOW_OFFSETS;
		DECODE_PACKET	theDecodePacket;
		CDecodeManager::MakeDecodePacket( pPacketLoader, &thePacket, ayData,
			++nPacketNumber, theDecodePacket, theDecodeOptions );

		// Write packet header.
		sprintf( szTemp, "%s%I64u\r\n", strPktNum.GetString(), nPacketNumber );
		
		if ( !::WriteFile(hFile, szTemp, static_cast<DWORD>( strlen( szTemp ) ),
				&nBytesWritten, NULL ) ) {
			DWORD	nResult = ::GetLastError();
			if ( !inFilePath.IsEmpty() ) {
				::CloseHandle( hFile );
				::DeleteFile( inFilePath );
			}
			return false;
		}

		// Generate the tag stream.
		DECODE_TAG_STREAM	theTagStream;
		DECODE_RESULT		nResult;
		nResult = PacketToTagStream( &theDecodePacket, inDecodeOptions, inCodePage, &theTagStream );
		if ( nResult != DECODE_RESULT_NOERROR ) {
			//+++ Count errors or give a message and bail.
			break;
		}

		// Interpret the tag stream.
		nResult = InterpretTagStream( &theDecodePacket, inFormat, inDecodeOptions, inCodePage,
			&theTagStream, &theTextStream );

		// Free the tag stream.
		FreeTagStream( &theTagStream );

		if ( nResult != DECODE_RESULT_NOERROR ) {
			//#pragma TODO(Count errors or give a message and bail.)
			break;
		}

		// Write out the decode data.
		// Stream length includes NULL terminator.
		DWORD	nDataBytes = strlen( (char*)theTextStream.pTextStream );
		if ( !::WriteFile( hFile, theTextStream.pTextStream,
				nDataBytes, &nBytesWritten, NULL ) ) {
			const DWORD	dwResult = ::GetLastError();
			if ( !inFilePath.IsEmpty() ) {
				::CloseHandle( hFile );
				::DeleteFile(inFilePath);
			}
			return false;
		}

		// Free the text stream.
		FreeTextStream( &theTextStream );
	}

	// Write the file footer.
	if ( FormatTextFooter( inFormat, &theTextStream ) == DECODE_RESULT_NOERROR ) {
		// Get the length of the text in the stream.
		size_t 	nLength = strlen( (char*)theTextStream.pTextStream );
		if ( nLength > 0 ) {
			DWORD	nBytesWritten = 0;
			if ( !::WriteFile(hFile, theTextStream.pTextStream,
					static_cast<DWORD>( nLength ), &nBytesWritten, NULL ) ) {
				DWORD	nResult = ::GetLastError();
				::CloseHandle( hFile );
				::DeleteFile( inFilePath );
				return false;
			}
		}

		// Free the text stream.
		FreeTextStream(&theTextStream);
	}

	// Close the file handle.
	if ( !inFilePath.IsEmpty() ) {
		::CloseHandle( hFile );
	}

	return true;
}


// -----------------------------------------------------------------------------
//		MakeDecodePacket [static]
//
// Converts a Packet into a DECODE_PACKET that the Decoder can understand.
// -----------------------------------------------------------------------------

void
CDecodeManager::MakeDecodePacket(
	const CPacketLoader* pPacketLoader,
	const PeekPacket*	 pPacketHeader,
	const UInt8*         pPacketData,
	UInt64			     nPacketNumber,
	DECODE_PACKET&	     outDecodePacket,
	DECODE_OPTIONS&      outOptions)
{
	// get local time boolean
	//bool tbTimeFormatLocalTime = UnitPrefs::GetLocalTime();

	// check packet's media type
	if ( pPacketLoader->IsWireless() ) {
		outDecodePacket.nMedia = DECODE_MEDIA_802_11;
	}
	else if ( pPacketLoader->IsEthernet() ) {
		outDecodePacket.nMedia = DECODE_MEDIA_ETHERNET;
	}
	else if ( pPacketLoader->IsTokenRing() ) {
		outDecodePacket.nMedia = DECODE_MEDIA_TOKENRING;
	}
	else if ( pPacketLoader->IsWan() ) {
		outDecodePacket.nMedia = DECODE_MEDIA_WAN;
	}

	// format packet
	outDecodePacket.nTimestampFormat = DECODE_TIMESTAMP_FORMAT_PEEK_NANOSECONDS;
	//if (tbTimeFormatLocalTime)
	outDecodePacket.nTimestampFormat |= DECODE_TIMESTAMP_FORMAT_LOCAL_TIME; // local time flag
	outDecodePacket.nFlags = pPacketHeader->fFlags;
	outDecodePacket.nStatus = pPacketHeader->fStatus;
	outDecodePacket.nPacketLength = pPacketHeader->fPacketLength;
	UInt16	actualLength = pPacketHeader->fSliceLength;
	if ( actualLength == 0 ) {
		actualLength = pPacketHeader->fPacketLength;
	}
	outDecodePacket.nActualLength = actualLength;
	outDecodePacket.pMediaSpecInfo = pPacketHeader->fMediaSpecInfoBlock;
	outDecodePacket.pPacketData = pPacketData;
	outDecodePacket.nProtoSpec = pPacketHeader->fProtoSpec;

	// split time stamp from 64-bits to 2 32-bits
	UInt64	nTimeStamp = pPacketHeader->fTimeStamp;
	outDecodePacket.nTimeStampHigh = (UInt32)( (nTimeStamp >> 32) & 0xFFFFFFFF );
	outDecodePacket.nTimeStampLow = (UInt32)( nTimeStamp & 0xFFFFFFFF );

	outDecodePacket.nPacketNumber = nPacketNumber;

	// use pointers to labels instead of copying the labels
	// into the decode stream
	outOptions |= DECODE_OPTIONS_USE_OBJECTS;
}


// -----------------------------------------------------------------------------
//		PacketToTagStream
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::PacketToTagStream(
	const DECODE_PACKET*	pDecodePacket,
	DECODE_OPTIONS			inOptions,
	CODE_PAGE				inCodePage,
	DECODE_TAG_STREAM*		pTagStream )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodPacketToTagStream_	f = ( DcodPacketToTagStream_ ) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodPacketToTagStream", "24" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( s_LibRef, pDecodePacket, inOptions, inCodePage, pTagStream, NULL );
}


// -----------------------------------------------------------------------------
//		FreeTagStream
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::FreeTagStream(
	DECODE_TAG_STREAM*	pTagStream )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodFreeTagStream_	f = (DcodFreeTagStream_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodFreeTagStream", "4" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( pTagStream );
}


// -----------------------------------------------------------------------------
//		InterpretTagStream
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::InterpretTagStream(
	const DECODE_PACKET*		pDecodePacket,
	DECODE_INTERPRET_FORMAT		nFormat,
	DECODE_OPTIONS				inOptions,
	CODE_PAGE					inCodePage,
	const DECODE_TAG_STREAM*	pTagStream,
	DECODE_TEXT_STREAM*			pTextStream )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodInterpretTagStream_	f = (DcodInterpretTagStream_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodInterpretTagStream", "28" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( s_LibRef, nFormat, pDecodePacket, inOptions, inCodePage, pTagStream, pTextStream );
}


// -----------------------------------------------------------------------------
//		FreeTextStream
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::FreeTextStream(
	DECODE_TEXT_STREAM*	pTextStream )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodFreeTextStream_	f = (DcodFreeTextStream_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodFreeTextStream", "4" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( pTextStream );
}


// -----------------------------------------------------------------------------
//		FormatTextHeader
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::FormatTextHeader(
	DECODE_INTERPRET_FORMAT	inFormat,
	DECODE_TEXT_STREAM*		outTextStream )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodFormatTextHeader_ f = (DcodFormatTextHeader_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodFormatTextHeader", "8" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( inFormat, outTextStream );
}


// -----------------------------------------------------------------------------
//		FormatTextFooter
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::FormatTextFooter(
	DECODE_INTERPRET_FORMAT	inFormat,
	DECODE_TEXT_STREAM*		outTextStream )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodFormatTextFooter_ f = (DcodFormatTextFooter_) ::GetProcAddress( s_hLib, DecorateSymbolName( "FormatTextFooter", "8" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( inFormat, outTextStream );
}


// -----------------------------------------------------------------------------
//		GetCallbacks
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::GetCallbacks(
	DECODE_LIB_REF		inLibRef,
	DECODE_CALLBACKS*	outCallbacks )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodGetCallbacks_ f = (DcodGetCallbacks_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodGetCallbacks", "8" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( inLibRef, outCallbacks );
}


// -----------------------------------------------------------------------------
//		SetCallbacks
// -----------------------------------------------------------------------------

DECODE_RESULT
CDecodeManager::SetCallbacks(
	DECODE_LIB_REF		inLibRef,
	DECODE_CALLBACKS*	inCallbacks )
{
	if ( (s_LibRef == 0) || (s_hLib == NULL) ) return DECODE_RESULT_INIT_FAILED;

	DcodSetCallbacks_ f = (DcodSetCallbacks_) ::GetProcAddress( s_hLib, DecorateSymbolName( "DcodSetCallbacks", "8" ) );
	if ( f == NULL ) return DECODE_RESULT_INIT_FAILED;

	return (*f)( inLibRef, inCallbacks );
}


// -----------------------------------------------------------------------------
//		NameLookup [static]
// -----------------------------------------------------------------------------
// Callback for name table lookup.

bool WINAPI
CDecodeManager::NameLookup(
	DECODE_TAG_CLASS	inTagClass,
	DECODE_TAG_TYPE 	inTagType,
	const UInt8*		inAddress,
	COLORREF*			outColor,
	TCHAR*				outName )
{
	// Sanity check.
	//ASSERT(s_nameTable != NULL);
	if ( s_nameTable == NULL ) return false;
	ASSERT( inAddress != NULL );
	if ( inAddress == NULL ) return false;

	bool	bResult = false;

	// Default values for name & color.
	if ( outName != NULL ) {
		*outName = 0;
	}

	if ( outColor != NULL ) {
		*outColor = RGB( 0, 0, 0 );	// Black.
	}

	// Determine the media class and type.
	TMediaSpecClass	theMediaSpecClass = kMediaSpecClass_Null;
	TMediaSpecType	theMediaSpecType = kMediaSpecType_Null;
	DecodeTagToMediaSpecTypeAndClass( inTagClass, inTagType, theMediaSpecClass, theMediaSpecType );
	if ( (theMediaSpecClass != kMediaSpecClass_Null) &&
		 (theMediaSpecType != kMediaSpecType_Null) ) {
		CMediaSpec	theSpec( theMediaSpecClass, theMediaSpecType, 0, inAddress );

		bResult = s_nameTable->SpecToName( theSpec, outName, true, outColor );
	}

	return bResult;
}


// -----------------------------------------------------------------------------
//		DecodeTagToMediaSpecTypeAndClass [static]
// -----------------------------------------------------------------------------

void
CDecodeManager::DecodeTagToMediaSpecTypeAndClass(
	DECODE_TAG_CLASS	inClass,
	DECODE_TAG_TYPE		inType,
	TMediaSpecClass&	outClass,
	TMediaSpecType&		outType )
{
	// Determine the media class and type.
	TMediaSpecClass	theMediaSpecClass = kMediaSpecClass_Null;
	TMediaSpecType	theMediaSpecType = kMediaSpecType_Null;
	switch ( inClass ) {
		case DECODE_TAG_CLASS_ADDRESS:
		{
			theMediaSpecClass = kMediaSpecClass_Address;
			switch ( inType ) {
				case DECODE_TAG_TYPE_ETHERNET_ADDRESS:
					theMediaSpecType = kMediaSpecType_EthernetAddr;
					break;

				case DECODE_TAG_TYPE_TOKENRING_ADDRESS:
					theMediaSpecType = kMediaSpecType_TokenRingAddr;
					break;

				case DECODE_TAG_TYPE_IPV4_ADDRESS:
					theMediaSpecType = kMediaSpecType_IPAddr;
					break;

				case DECODE_TAG_TYPE_IPV6_ADDRESS:
					theMediaSpecType = kMediaSpecType_IPv6Addr;
					break;

				case DECODE_TAG_TYPE_APPLETALK_LONG_ADDRESS:
					theMediaSpecType = kMediaSpecType_AppleTalkAddr;
					break;

#if 0
				case DECODE_TAG_TYPE_DLCI_ADDRESS:
					theMediaSpecType = kMediaSpecType_WAN_DLCIAddr;
					break;
#endif
			}
		}
		break;

		case DECODE_TAG_CLASS_PROTOCOL:
		{
			theMediaSpecClass = kMediaSpecClass_Protocol;
			switch ( inType ) {
				case DECODE_TAG_TYPE_ETHERNET_PROTOCOL:
					theMediaSpecType = kMediaSpecType_EthernetProto;
					break;

				case DECODE_TAG_TYPE_LSAP_PROTOCOL:
					theMediaSpecType = kMediaSpecType_LSAP;
					break;

				case DECODE_TAG_TYPE_SNAP_PROTOCOL:
					theMediaSpecType = kMediaSpecType_SNAP;
					break;
			}
		}
		break;

		case DECODE_TAG_CLASS_PORT:
		{
			theMediaSpecClass = kMediaSpecClass_Port;
			switch ( inType ) {
				case DECODE_TAG_TYPE_IP_PORT:
					theMediaSpecType = kMediaSpecType_IPPort;
					break;

				case DECODE_TAG_TYPE_NW_PORT:
					theMediaSpecType = kMediaSpecType_NWPort;
					break;
			}
		}
		break;
	}

	outClass = theMediaSpecClass;
	outType = theMediaSpecType;
}
