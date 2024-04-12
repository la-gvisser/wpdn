// =============================================================================
//	Protospecs.h
// =============================================================================
//	Copyright (c) 2001-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "MediaTypes.h"

// Product's Media Type
#define	ETHERNET		0x00000100

// pack to 1 byte alignment for compatibility
#include "wppushpack.h"

// This block flips between export (when building the library)
// and import (when using the library in an application.
#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef PROTOSPECS_EXPORTS
		#define PROTOSPECS_API __declspec(dllexport)
	#else
		#define PROTOSPECS_API __declspec(dllimport)
	#endif
#elif defined(__GNUC__) && (__GNUC__ >= 4)
	#ifdef PROTOSPECS_EXPORTS
		#define PROTOSPECS_API __attribute__ ((visibility("default")))
	#else
		#define PROTOSPECS_API
	#endif
#else
	#define PROTOSPECS_API
#endif

//  for location of the PSpecsV1.xml and PSpecsV1.bin files
typedef const wchar_t*	PROTOSPECS_LOCATIONW;
typedef const char*		PROTOSPECS_LOCATIONA;
#ifdef UNICODE
#define PROTOSPECS_LOCATION	PROTOSPECS_LOCATIONW
#else
#define PROTOSPECS_LOCATION	PROTOSPECS_LOCATIONA
#endif

#define PROTOSPEC_INVLID_ID		0		// 0 is not ok
#define PROTOSPEC_INVALID_ID	0		// 0 is not ok. Neither is misspelling "invalid".
#define MAX_PROTOSPEC_DEPTH		32		// optimization used to preallocate arrays, etc.
#define PSID_RESERVED_SWITCH	0xFFFF

typedef UInt32 PROTOSPECS_LIB_VERSION;

typedef UInt32 PROTOSPEC_INSTID;		// instance + id # (the 32 bit number that represents an instance of a particular protospec, e.g. IP over SNAP over 802.3)
typedef UInt16 PROTOSPEC_INSTANCE;		// instance # (i.e. the first instance of IP is 1)
typedef UInt16 PROTOSPEC_ID;			// id # (e.g. IP = 1000)

namespace ProtoSpecs
{
	// GetPSVersion (returns 0xXXYYZZTT)
	// for instance, v1.2.3.4 =>
	//		XX ==0x01
	//		YY ==0x02
	//		ZZ ==0x03
	//		TT ==0x04
	HRESULT PROTOSPECS_API GetPSVersion( PROTOSPECS_LIB_VERSION& version );

	// this one returns the version of the loaded file (if loaded)
	HRESULT PROTOSPECS_API GetPSFileVersionW( wchar_t* pszVersion, int cchVersion );
	HRESULT PROTOSPECS_API GetPSFileVersionA( char* pszVersion, int cchVersion );

#ifdef UNICODE
	#define GetPSFileVersion GetPSFileVersionW
#else
	#define GetPSFileVersion GetPSFileVersionA
#endif

	HRESULT PROTOSPECS_API GetPSProductCode( UInt32& code );	// gets the current product code for converting the XML to PSpecs

	HRESULT PROTOSPECS_API IsProtospecsLoaded();

	HRESULT PROTOSPECS_API InitProtoSpecUtilsFileW( const wchar_t* inXMLLocation,
											 const wchar_t* inBINLocation,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
	HRESULT PROTOSPECS_API InitProtoSpecUtilsFileA( const char* inXMLLocation,
											 const char* inBINLocation,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
#ifdef UNICODE
	#define InitProtoSpecUtilsFile InitProtoSpecUtilsFileW
#else
	#define InitProtoSpecUtilsFile InitProtoSpecUtilsFileA
#endif

	HRESULT PROTOSPECS_API InitProtoSpecUtilsStreamW( const wchar_t* pszXMLStream,
											 int	cchStream,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
	HRESULT PROTOSPECS_API InitProtoSpecUtilsStreamA( const char* pszXMLStream,
											 int	cchStream,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
#ifdef UNICODE
	#define InitProtoSpecUtilsStream InitProtoSpecUtilsStreamW
#else
	#define InitProtoSpecUtilsStream InitProtoSpecUtilsStreamA
#endif

	HRESULT PROTOSPECS_API CleanupProtoSpecUtils();

	HRESULT PROTOSPECS_API IsValidInstance( PROTOSPEC_INSTID inInstID );
	HRESULT PROTOSPECS_API IsValidID( PROTOSPEC_ID inID );
	HRESULT PROTOSPECS_API GetPSInstanceCount( PROTOSPEC_ID inID, UInt16 & outCount );
	HRESULT PROTOSPECS_API GetPSActiveInstances( PROTOSPEC_ID inID, PROTOSPEC_INSTID * payInstIDs, UInt16 & ioCount );

	HRESULT PROTOSPECS_API GetParentID( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID& outParentInstanceID );
	HRESULT PROTOSPECS_API GetNameID( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_ID& outNameID );
	HRESULT PROTOSPECS_API GetPSDescriptionID( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_ID& outDescriptionID );

	HRESULT PROTOSPECS_API GetPSHierarchicalNameW( PROTOSPEC_INSTID inInstanceID, wchar_t* pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSHierarchicalNameA( PROTOSPEC_INSTID inInstanceID, char* pszName, int cchName );
#ifdef UNICODE
	#define GetPSHierarchicalName GetPSHierarchicalNameW
#else
	#define GetPSHierarchicalName GetPSHierarchicalNameA
#endif

	HRESULT PROTOSPECS_API GetPSShortNameW( PROTOSPEC_INSTID inInstanceID, wchar_t* pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSShortNameA( PROTOSPEC_INSTID inInstanceID, char* pszName, int cchName );
#ifdef UNICODE
	#define GetPSShortName GetPSShortNameW
#else
	#define GetPSShortName GetPSShortNameA
#endif

	HRESULT PROTOSPECS_API GetPSLongNameW( PROTOSPEC_INSTID inInstanceID, wchar_t* pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSLongNameA( PROTOSPEC_INSTID inInstanceID, char* pszName, int cchName );
#ifdef UNICODE
	#define GetPSLongName GetPSLongNameW
#else
	#define GetPSLongName GetPSLongNameA
#endif

	HRESULT PROTOSPECS_API GetPSDescriptionW( PROTOSPEC_INSTID inInstanceID, wchar_t* pszDesc, int cchDesc );
	HRESULT PROTOSPECS_API GetPSDescriptionA( PROTOSPEC_INSTID inInstanceID, char* pszDesc, int cchDesc );
#ifdef UNICODE
	#define GetPSDescription GetPSDescriptionW
#else
	#define GetPSDescription GetPSDescriptionA
#endif

	HRESULT PROTOSPECS_API GetColor( PROTOSPEC_INSTID inInstanceID, COLORREF& cr );
	HRESULT PROTOSPECS_API GetPSNamedColorW( const wchar_t* pszName, COLORREF& cr );
	HRESULT PROTOSPECS_API GetPSNamedColorA( const char* pszName, COLORREF& cr );
#ifdef UNICODE
	#define GetPSNamedColor GetPSNamedColorW
#else
	#define GetPSNamedColor GetPSNamedColorA
#endif

	HRESULT PROTOSPECS_API GetPSColorNameW( COLORREF cr, wchar_t* pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSColorNameA( COLORREF cr, char* pszName, int cchName );
#ifdef UNICODE
	#define GetPSColorName GetPSColorNameW
#else
	#define GetPSColorName GetPSColorNameA
#endif

	HRESULT PROTOSPECS_API GetPSColorTableEntryW( UInt32 nIndex, wchar_t* pszName, int cchName, COLORREF& crColor );
	HRESULT PROTOSPECS_API GetPSColorTableEntryA( UInt32 nIndex, char* pszName, int cchName, COLORREF& crColor );
#ifdef UNICODE
	#define GetPSColorTableEntry GetPSColorTableEntryW
#else
	#define GetPSColorTableEntry GetPSColorTableEntryA
#endif

	HRESULT PROTOSPECS_API GetColorTableCount( UInt32 &nCount );

	HRESULT PROTOSPECS_API GetDepth( PROTOSPEC_INSTID inInstanceID, int& nDepth );

	HRESULT PROTOSPECS_API GetPSConditionExpW( PROTOSPEC_INSTID inInstanceID, wchar_t* pszExp, int cchExp );
	HRESULT PROTOSPECS_API GetPSConditionExpA( PROTOSPEC_INSTID inInstanceID, char* pszExp, int cchExp );
#ifdef UNICODE
	#define GetPSConditionExp GetPSConditionExpW
#else
	#define GetPSConditionExp GetPSConditionExpA
#endif

	HRESULT PROTOSPECS_API GetPSNextLayerExpW( PROTOSPEC_INSTID inInstanceID, wchar_t* pszExp, int cchExp );
	HRESULT PROTOSPECS_API GetPSNextLayerExpA( PROTOSPEC_INSTID inInstanceID, char* pszExp, int cchExp );
#ifdef UNICODE
	#define GetPSNextLayerExp GetPSNextLayerExpW
#else
	#define GetPSNextLayerExp GetPSNextLayerExpA
#endif

	HRESULT PROTOSPECS_API GetPSIDCount( UInt16 & outCount );
	HRESULT PROTOSPECS_API GetPSIDArray( PROTOSPEC_ID * payIDs, UInt16 & ioCount );

	HRESULT PROTOSPECS_API GetProtoSpecCount( UInt32& nCount );
	HRESULT PROTOSPECS_API GetProtoSpecArray( PROTOSPEC_INSTID* payArray, UInt32 nCount );

	HRESULT PROTOSPECS_API GetRootNodeCount( SInt32 &nCount );
	HRESULT PROTOSPECS_API GetRootNode( SInt32 nIndex, PROTOSPEC_INSTID& rootInstID );

	HRESULT PROTOSPECS_API GetPacketProtoSpec( TMediaType inMediaType, TMediaSubType inMediaSubType,
							UInt32 inPacketFlags, const UInt8* inPacket, UInt16 inPacketSize,
							PROTOSPEC_INSTID& outID );

	// zz flagless overload for old API that much code calls
	HRESULT PROTOSPECS_API GetPacketProtoSpec(	TMediaType	inMediaType, TMediaSubType inMediaSubType,
											const UInt8* inPacket, UInt16 inPacketSize,
											PROTOSPEC_INSTID& outID );

	HRESULT PROTOSPECS_API IsDescendentOf( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID* outMatchParentInstID,
										const PROTOSPEC_INSTID* inParentIDs, SInt32 inParentCount,
										SInt32* outMatchParentIndex );

	HRESULT PROTOSPECS_API GetSibling( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID& outSibling );
	HRESULT PROTOSPECS_API GetChild( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID& outChild );

	HRESULT PROTOSPECS_API TranslateID( PROTOSPEC_ID inID, PROTOSPEC_ID& outID );

	enum {
		MEM_USAGE_DESCTBL = 0x01,
		MEM_USAGE_NAMETBL = 0x02,
		MEM_USAGE_PSPECS  = 0x04,
		MEM_USAGE_COLORTBL = 0x08,
		MEM_USAGE_EXPRESSIONS = 0x10,
		MEM_USAGE_ALL = 0xFFFFFFFF
	};

	HRESULT  PROTOSPECS_API GetMemoryUsage(UInt32 nFlags, UInt32& memUsage );

	// convert to #defines or just export as functions?

#ifndef GET_PSID
#define GET_PSID(inInstanceID)				((PROTOSPEC_ID) ((inInstanceID) & 0xFFFF))
#endif
#ifndef GET_PSINSTANCE
#define GET_PSINSTANCE(inInstanceID)		((PROTOSPEC_INSTANCE) (( (inInstanceID) & 0xFFFF0000 ) >> 16))
#endif
#ifndef MAKE_PSINSTID
#define MAKE_PSINSTID(inInstance, inPSID)	((PROTOSPEC_INSTID) (( (inInstance) << 16 ) | (inPSID) ))
#endif

	HRESULT PROTOSPECS_API FindProtospecIDW( const wchar_t* pszHierName, PROTOSPEC_ID& outID );
	HRESULT PROTOSPECS_API FindProtospecIDA( const char* pszHierName, PROTOSPEC_ID& outID );
#ifdef UNICODE
	#define FindProtospecID FindProtospecIDW
#else
	#define FindProtospecID FindProtospecIDA
#endif

	// like FindProtospecID above, except you can specify which name to search on
	enum {
		SEARCH_SNAME,
		SEARCH_LNAME,
		SEARCH_HNAME
	};

	HRESULT PROTOSPECS_API FindProtospecW( const wchar_t* pszName, UInt32 nSearchFor, PROTOSPEC_ID& outID );
	HRESULT PROTOSPECS_API FindProtospecA( const char* pszName, UInt32 nSearchFor, PROTOSPEC_ID& outID );
#ifdef UNICODE
	#define FindProtospec FindProtospecW
#else
	#define FindProtospec FindProtospecA
#endif

	HRESULT PROTOSPECS_API FindProtospecInstancesW( const wchar_t* pszHierName, PROTOSPEC_INSTID* payInstances, int cchInstances );
	HRESULT PROTOSPECS_API FindProtospecInstancesA( const char* pszHierName, PROTOSPEC_INSTID* payInstances, int cchInstances );
#ifdef UNICODE
	#define FindProtospecInstances FindProtospecInstancesW
#else
	#define FindProtospecInstances FindProtospecInstancesA
#endif

	HRESULT PROTOSPECS_API FindProtospecInstanceW( const wchar_t* pszPath, PROTOSPEC_INSTID& outID );	// for example, _T("IEEE 802.3//SNAP//IP//TCP//HTTP")
	HRESULT PROTOSPECS_API FindProtospecInstanceA( const char* pszPath, PROTOSPEC_INSTID& outID );	// for example, _T("IEEE 802.3//SNAP//IP//TCP//HTTP")
	HRESULT PROTOSPECS_API FindProtospecInstanceByID( PROTOSPEC_ID* payIDs, int nLength, PROTOSPEC_INSTID& outID );	// for example, payIDs[] = { 1, 9, 1000, 1400, 1418 }
#ifdef UNICODE
	#define FindProtospecInstance FindProtospecInstanceW
#else
	#define FindProtospecInstance FindProtospecInstanceA
#endif

	HRESULT PROTOSPECS_API BuildPSParentPathW( PROTOSPEC_INSTID nInstanceID, wchar_t* pszBuff, int cchBuff );	// for example, _T("IEEE 802.3//SNAP//IP//TCP//HTTP")
	HRESULT PROTOSPECS_API BuildPSParentPathA( PROTOSPEC_INSTID nInstanceID, char* pszBuff, int cchBuff );	// for example, _T("IEEE 802.3//SNAP//IP//TCP//HTTP")
#ifdef UNICODE
	#define BuildPSParentPath BuildPSParentPathW
#else
	#define BuildPSParentPath BuildPSParentPathA
#endif

	// returns a reverse path to the root item in the array
	HRESULT PROTOSPECS_API BuildParentArray( PROTOSPEC_INSTID nInstanceID, PROTOSPEC_INSTID* payInstIDBuff, int &nBuffCount );

	HRESULT PROTOSPECS_API GetDataLayer( TMediaType inMediaType, TMediaSubType inMediaSubType,
							const UInt8* inPacket, UInt16 inPacketSize,
							PROTOSPEC_INSTID inID, PROTOSPEC_INSTID& outSource,
							const UInt8** ppData );
	HRESULT PROTOSPECS_API GetHeaderLayer( TMediaType inMediaType, TMediaSubType inMediaSubType,
							const UInt8* inPacket, UInt16 inPacketSize,
							PROTOSPEC_INSTID inID, PROTOSPEC_INSTID& outSource,
							const UInt8** ppData );

	HRESULT PROTOSPECS_API IsRootPSpec( PROTOSPEC_INSTID inID );

	struct SProtospecInfo {
		PROTOSPEC_INSTID	nInstID;
		const UInt8*		pHeader;
		const UInt8*		pPayload;
	} WP_PACK_STRUCT;
	typedef struct SProtospecInfo	SProtospecInfo;

	// returns a reverse path to the root item in the array, with all header and data pointers
	HRESULT PROTOSPECS_API BuildParentInfo(
							TMediaType inMediaType,
							TMediaSubType inMediaSubType,
							const UInt8* inPacket,
							UInt16 inPacketSize,
							PROTOSPEC_INSTID inID,
							SProtospecInfo* payInfo,
							int &nInfoCount );
	// utility to obtain the desired index from the info array
	HRESULT PROTOSPECS_API GetIndexFromInfo(
							SProtospecInfo* payInfo,
							int inInfoCount,
							PROTOSPEC_INSTID inID,
							int& outIndex );

	enum {
		RENDER_RESET_NAME_TABLE = 0x01,
		RENDER_RESET_DESC_TABLE = 0x02,
		RENDER_RESET_COLOR_TABLE = 0x04,
		RENDER_RESET_PSPEC_TABLE = 0x08,
		RENDER_NAME_TABLE = 0x10,
		RENDER_DESC_TABLE = 0x20,
		RENDER_PSPEC_TABLE = 0x40,
		RENDER_COLOR_TABLE = 0x80,
		RENDER_DRIVER_SUPPORT = RENDER_PSPEC_TABLE |
								RENDER_RESET_NAME_TABLE |
								RENDER_RESET_DESC_TABLE |
								RENDER_RESET_COLOR_TABLE,
		RENDER_ALL = 0xFFFFFFFF
	};

	// render flags aren't really supported (yet)
	HRESULT PROTOSPECS_API GetRenderSize( UInt32 nRenderFlags, UInt32& nRenderSize );
	HRESULT PROTOSPECS_API RenderProtoSpecs( UInt8* pBuffer, UInt32 nBufferSize, UInt32 nRenderFlags );

	typedef void (*GetPacketProtoSpecHook)( TMediaType inMediaType, TMediaSubType inMediaSubType,
		UInt32 inPacketFlags, const UInt8* inPacket, UInt16 inPacketSize, PROTOSPEC_INSTID& ioID );
	void PROTOSPECS_API SetGetPacketProtospecHook( GetPacketProtoSpecHook hook );
	void PROTOSPECS_API GetGetPacketProtospecHook( GetPacketProtoSpecHook* pHook );
}

#include "wppoppack.h"
