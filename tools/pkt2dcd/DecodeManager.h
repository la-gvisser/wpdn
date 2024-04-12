// =============================================================================
//	DecodeManager.h
// =============================================================================
//  Copyright (c) 2015-2016 Savvius, Inc. All rights reserved
//	Copyright (c) 2000-2015 WildPackets, Inc. All rights reserved.
//	Copyright (c) 1999-2000 AG Group, Inc. All rights reserved.

#pragma once

#include "Dcod.h"
#include "PacketLoader.h"
#include "NameTableReader.h"

#if _WIN64
#define DecorateSymbolName(s, n)   s
#else
#define DecorateSymbolName(s, n)   "_" ## s ## "@" ## n
#endif


// =============================================================================
//		CDecodeManager
// =============================================================================

class CDecodeManager
{
public:
	static DECODE_RESULT	Load( CString inDcodFolder );
	static DECODE_RESULT	Unload();

	static bool	SaveFile( CPacketLoader* pPacketLoader, const CString& inFilePath,
		DECODE_INTERPRET_FORMAT inFormat, CODE_PAGE inCodePage,
		DECODE_OPTIONS inDecodeOptions );

	static void MakeDecodePacket( const CPacketLoader* pPacketLoader, 
		const PeekPacket* pPacketHeader, const UInt8* pPacketData, UInt64 nPacketNumber,
		DECODE_PACKET& outDecodePacket, DECODE_OPTIONS& ioOptions );

	static DECODE_RESULT PacketToTagStream( const DECODE_PACKET* pDecodePacket,
		DECODE_OPTIONS inOptions, CODE_PAGE inCodePage, DECODE_TAG_STREAM* pTagStream );

	static DECODE_RESULT FreeTagStream( DECODE_TAG_STREAM*	pTagStream );

	static DECODE_RESULT InterpretTagStream( const DECODE_PACKET* pDecodePacket,
		DECODE_INTERPRET_FORMAT nFormat, DECODE_OPTIONS inOptions, CODE_PAGE inCodePage,
		const DECODE_TAG_STREAM* pTagStream, DECODE_TEXT_STREAM* pTextStream );

	static DECODE_RESULT FreeTextStream( DECODE_TEXT_STREAM* pTextStream );

	static DECODE_RESULT FormatTextHeader( DECODE_INTERPRET_FORMAT inFormat,
		DECODE_TEXT_STREAM* outTextStream );
	
	static DECODE_RESULT FormatTextFooter( DECODE_INTERPRET_FORMAT inFormat,
		DECODE_TEXT_STREAM* outTextStream );
	
	static bool WINAPI	NameLookup( DECODE_TAG_CLASS inTagClass, DECODE_TAG_TYPE inTagType,
		const UInt8* inAddress, COLORREF* outColor, TCHAR* outName );

	static DECODE_RESULT GetCallbacks( DECODE_LIB_REF inLibRef, DECODE_CALLBACKS* outCallbacks );
	static DECODE_RESULT SetCallbacks( DECODE_LIB_REF inLibRef, DECODE_CALLBACKS* inCallbacks );

	static void DecodeTagToMediaSpecTypeAndClass( DECODE_TAG_CLASS inClass, DECODE_TAG_TYPE inType,
		TMediaSpecClass& outClass, TMediaSpecType& outType );

protected:
	static DECODE_LIB_REF		s_LibRef;
	static HMODULE				s_hLib;
	static CNameTableReader*	s_nameTable;
};
