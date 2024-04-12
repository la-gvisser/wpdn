// =============================================================================
//	MediaSpecUtil.h
// =============================================================================
//	Copyright (c) 1996-2008 WildPackets, Inc. All rights reserved.

#ifndef MEDIASPECUTIL_H
#define MEDIASPECUTIL_H

#include "MediaSpec.h"

class DECLSPEC UMediaSpecUtils
{
public:
	static void		NetworkToHostMediaSpec( TMediaSpec& inSpec );
	static void		HostToNetworkMediaSpec( TMediaSpec& inSpec );

	static bool		SpecToString( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		StringToSpec( LPCTSTR inString, TMediaSpecType inType, CMediaSpec& outSpec );
	static CMediaSpec	StringToSpec( LPCTSTR inString, TMediaSpecType inType );
	
	static bool		MaskToBitfield( const CMediaSpec& inSpec, UInt8* outBitField );

	static LPCTSTR	GetMediaString( TMediaType inType, TMediaSubType inSubType );
	static LPCTSTR	GetMediaString( TMediaType inType );
	static LPCTSTR	GetMediaSubTypeString( TMediaSubType inSubType );
	static LPCTSTR	GetMediaClassString( TMediaSpecClass inClass );
	static LPCTSTR	GetMediaTypeString( TMediaSpecType inType );
	static LPCTSTR	GetMediaTypePrefix( TMediaSpecType inType );
	static LPCTSTR	GetMediaTypeDefault( TMediaSpecType inType );
	
	static TMediaSpecType	GetMediaTypeFromString( LPCTSTR inString );

	// this shouldn't really be here but I need in the CPacket
	static UInt32	GetSubTypeFromProtocol( TMediaType inMediaType, UInt32 inWanProtocol, TMediaSubType* outMediaSubType );

protected:
	static bool		FormatEtherProto( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatLSAPProto( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatSNAPProto( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatPhysicalAddr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatAppleTalkAddr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatIPAddr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatDECnetAddr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatIPv6Addr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatIPXAddr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatIPPort( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatATPort( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatNWPort( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatTCPPortPair( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatWanPPPProto( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatWanFrameRelayProto( const CMediaSpec& inSpec, LPTSTR outString );
	static bool		FormatWanX25Proto( const CMediaSpec& inSpec, LPTSTR outString );
	static bool     FormatWANDLCIAddr( const CMediaSpec& inSpec, LPTSTR outString );
	static bool     FormatApplicationID( const CMediaSpec& inSpec, LPTSTR outString );

	static bool		ParseEtherProto( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseLSAPProto( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseSNAPProto( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParsePhysicalAddr( LPCTSTR inString, TMediaSpecType inType, CMediaSpec& outSpec );
	static bool		ParseAppleTalkAddr( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseIPAddr( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseDECnetAddr( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseIPv6Addr( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseIPXAddr( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseIPPort( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseATPort( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseNWPort( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseWANPPPProto( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseWANFrameRelayProto( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseWANDLCIAddr( LPCTSTR inString, CMediaSpec& outSpec );
	static bool		ParseApplicationID( LPCTSTR inString, CMediaSpec& outSpec );

	static bool		HexStrToByte( LPCTSTR inStr, UInt8& outByte );
	static bool		HexCharToValue( TCHAR inChar, UInt8& outValue );

};

// Macros for converting the fields of a media spec that need
// to be swapped if saving to or loading from disk.
#define _NetworkToHostMediaSpec( theSpec )	UMediaSpecUtils::NetworkToHostMediaSpec( theSpec )
#define _HostToNetworkMediaSpec( theSpec )	UMediaSpecUtils::HostToNetworkMediaSpec( theSpec )

#endif /* MEDIASPECUTIL_H */
