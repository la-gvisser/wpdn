// ProtospecsV2.h -- version 2 of the protospecs stuff
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#ifndef _PROTOSPECS_H_
#define _PROTOSPECS_H_

///////////////////////////////////////////////////////////////////////
//	includes
///////////////////////////////////////////////////////////////////////
#include "WPTypes.h"
#include "MediaTypes.h"

// pack to 1 byte alignment for compatibility
#pragma pack( push, 1 )

#define TIME_EVALS	0

///////////////////////////////////////////////////////////////////////
//  this block flips between export (when building the DLL)
//  and import (when using the DLL in an application
///////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
	#ifdef PROTOSPECS_EXPORTS
		#define PROTOSPECS_API __declspec(dllexport)
	#else
		#define PROTOSPECS_API __declspec(dllimport)
	#endif
#else
	#define PROTOSPECS_API
#endif

///////////////////////////////////////////////////////////////////////
//  typedefs, structures, constants
///////////////////////////////////////////////////////////////////////

//  for location of the PSpecsV1.xml and PSpecsV1.bin files
typedef LPCWSTR 	PROTOSPECS_LOCATIONW;
typedef LPCTSTR 	PROTOSPECS_LOCATIONA;
#ifdef UNICODE
#define PROTOSPECS_LOCATION	PROTOSPECS_LOCATIONW
#else
#define PROTOSPECS_LOCATION	PROTOSPECS_LOCATIONA
#endif

#define PROTOSPEC_INVLID_ID			0	// 0 is not ok
#define PROTOSPEC_INVALID_ID		0	// 0 is not ok. Neither is misspelling "invalid".
#define MAX_PROTOSPEC_DEPTH		  256	// optimization used to preallocate arrays, etc.
#define PSID_RESERVED_SWITCH	0xffff

class XXMLDoc;

typedef UInt32 PROTOSPECS_LIB_VERSION;

typedef UInt32				PROTOSPEC_INSTID;		// instance + id # (the 32 bit number that represents an instance of a particular protospec, e.g. IP over SNAP over 802.3)
typedef UInt16				PROTOSPEC_INSTANCE;		// instance # (i.e. the first instance of IP is 1)
typedef UInt16				PROTOSPEC_ID;			// id # (e.g. IP = 1000)

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
	HRESULT PROTOSPECS_API GetPSFileVersionW( LPWSTR pszVersion, int cchVersion );
	HRESULT PROTOSPECS_API GetPSFileVersionA( LPSTR pszVersion, int cchVersion );

#ifdef UNICODE
	#define GetPSFileVersion GetPSFileVersionW
#else
	#define GetPSFileVersion GetPSFileVersionA
#endif

	HRESULT PROTOSPECS_API GetPSProductCode( UInt32& code );	// gets the current product code for converting the XML to PSpecs

	HRESULT PROTOSPECS_API IsProtospecsLoaded();

#ifdef PROTOSPECS_EXPORTS /* not needed by plugins and ETHERNET is not defined. */
	HRESULT PROTOSPECS_API InitProtoSpecUtilsFileW( LPCWSTR inXMLLocation,
											 LPCWSTR inBINLocation,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
	HRESULT PROTOSPECS_API InitProtoSpecUtilsFileA( LPCSTR inXMLLocation,
											 LPCSTR inBINLocation,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
#ifdef UNICODE
	#define InitProtoSpecUtilsFile InitProtoSpecUtilsFileW
#else
	#define InitProtoSpecUtilsFile InitProtoSpecUtilsFileA
#endif

	HRESULT PROTOSPECS_API InitProtoSpecUtilsStreamW( LPCWSTR pszXMLStream,
											 int	cchStream,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
	HRESULT PROTOSPECS_API InitProtoSpecUtilsStreamA( LPCSTR pszXMLStream,
											 int	cchStream,
											 UInt32 dwProductCode = ETHERNET,
											 bool  inShowHidden = false );
#ifdef UNICODE
	#define InitProtoSpecUtilsStream InitProtoSpecUtilsStreamW
#else
	#define InitProtoSpecUtilsStream InitProtoSpecUtilsStreamA
#endif
#endif /* PROTOSPECS_EXPORTED */

	HRESULT PROTOSPECS_API CleanupProtoSpecUtils();

	HRESULT PROTOSPECS_API IsValidInstance( PROTOSPEC_INSTID inInstID );
	HRESULT PROTOSPECS_API IsValidID( PROTOSPEC_ID inID );
	HRESULT PROTOSPECS_API GetPSInstanceCount( PROTOSPEC_ID inID, UInt16 & outCount );
	HRESULT PROTOSPECS_API GetPSActiveInstances( PROTOSPEC_ID inID, PROTOSPEC_INSTID * payInstIDs, UInt16 & ioCount );

	HRESULT PROTOSPECS_API GetParentID( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID& outParentInstanceID );
	HRESULT PROTOSPECS_API GetNameID( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_ID& outNameID );
	HRESULT PROTOSPECS_API GetPSDescriptionID( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_ID& outDescriptionID );

	HRESULT PROTOSPECS_API GetPSHierarchicalNameW( PROTOSPEC_INSTID inInstanceID, LPWSTR pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSHierarchicalNameA( PROTOSPEC_INSTID inInstanceID, LPSTR pszName, int cchName );
#ifdef UNICODE
	#define GetPSHierarchicalName GetPSHierarchicalNameW
#else
	#define GetPSHierarchicalName GetPSHierarchicalNameA
#endif

	HRESULT PROTOSPECS_API GetPSShortNameW( PROTOSPEC_INSTID inInstanceID, LPWSTR pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSShortNameA( PROTOSPEC_INSTID inInstanceID, LPSTR pszName, int cchName );
#ifdef UNICODE
	#define GetPSShortName GetPSShortNameW
#else
	#define GetPSShortName GetPSShortNameA
#endif

	HRESULT PROTOSPECS_API GetPSLongNameW( PROTOSPEC_INSTID inInstanceID, LPWSTR pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSLongNameA( PROTOSPEC_INSTID inInstanceID, LPSTR pszName, int cchName );
#ifdef UNICODE
	#define GetPSLongName GetPSLongNameW
#else
	#define GetPSLongName GetPSLongNameA
#endif

	HRESULT PROTOSPECS_API GetPSDescriptionW( PROTOSPEC_INSTID inInstanceID, LPWSTR pszDesc, int cchDesc );
	HRESULT PROTOSPECS_API GetPSDescriptionA( PROTOSPEC_INSTID inInstanceID, LPSTR pszDesc, int cchDesc );
#ifdef UNICODE
	#define GetPSDescription GetPSDescriptionW
#else
	#define GetPSDescription GetPSDescriptionA
#endif

	HRESULT PROTOSPECS_API GetColor( PROTOSPEC_INSTID inInstanceID, COLORREF& cr );
	HRESULT PROTOSPECS_API GetPSNamedColorW( LPCWSTR pszName, COLORREF& cr );
	HRESULT PROTOSPECS_API GetPSNamedColorA( LPCSTR pszName, COLORREF& cr );
#ifdef UNICODE
	#define GetPSNamedColor GetPSNamedColorW
#else
	#define GetPSNamedColor GetPSNamedColorA
#endif

	HRESULT PROTOSPECS_API GetPSColorNameW( COLORREF cr, LPWSTR pszName, int cchName );
	HRESULT PROTOSPECS_API GetPSColorNameA( COLORREF cr, LPSTR pszName, int cchName );
#ifdef UNICODE
	#define GetPSColorName GetPSColorNameW
#else
	#define GetPSColorName GetPSColorNameA
#endif

	HRESULT PROTOSPECS_API GetPSColorTableEntryW( UInt32 nIndex, LPWSTR pszName, int cchName, COLORREF& crColor );
	HRESULT PROTOSPECS_API GetPSColorTableEntryA( UInt32 nIndex, LPSTR pszName, int cchName, COLORREF& crColor );
#ifdef UNICODE
	#define GetPSColorTableEntry GetPSColorTableEntryW
#else
	#define GetPSColorTableEntry GetPSColorTableEntryA
#endif

	HRESULT PROTOSPECS_API GetColorTableCount( UInt32 &nCount );

	HRESULT PROTOSPECS_API GetDepth( PROTOSPEC_INSTID inInstanceID, int& nDepth );

	HRESULT PROTOSPECS_API GetPSConditionExpW( PROTOSPEC_INSTID inInstanceID, LPWSTR pszExp, int cchExp );
	HRESULT PROTOSPECS_API GetPSConditionExpA( PROTOSPEC_INSTID inInstanceID, LPSTR pszExp, int cchExp );
#ifdef UNICODE
	#define GetPSConditionExp GetPSConditionExpW
#else
	#define GetPSConditionExp GetPSConditionExpA
#endif

	HRESULT PROTOSPECS_API GetPSNextLayerExpW( PROTOSPEC_INSTID inInstanceID, LPWSTR pszExp, int cchExp );
	HRESULT PROTOSPECS_API GetPSNextLayerExpA( PROTOSPEC_INSTID inInstanceID, LPSTR pszExp, int cchExp );
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

	enum
	{
		MEM_USAGE_DESCTBL = 0x01,
		MEM_USAGE_NAMETBL = 0x02,
		MEM_USAGE_PSPECS  = 0x04,
		MEM_USAGE_COLORTBL = 0x08,
		MEM_USAGE_EXPRESSIONS = 0x10,
		MEM_USAGE_ALL = 0xffffffff
	};

	HRESULT  PROTOSPECS_API GetMemoryUsage(UInt32 nFlags, UInt32& memUsage );

	// convert to #defines or just export as functions?

#ifndef GET_PSID
#define GET_PSID(inInstanceID)				((PROTOSPEC_ID) ( (inInstanceID) & 0xffff ))
#endif
#ifndef GET_PSINSTANCE
#define GET_PSINSTANCE(inInstanceID)		((PROTOSPEC_INSTANCE) (( (inInstanceID) & 0xffff0000 ) >> 16))
#endif
#ifndef MAKE_PSINSTID
#define MAKE_PSINSTID(inInstance, inPSID)	((PROTOSPEC_INSTID) (( (inInstance) << 16 ) | (inPSID) ))
#endif

	HRESULT PROTOSPECS_API FindProtospecIDW(LPCWSTR pszHierName, PROTOSPEC_ID& outID );
	HRESULT PROTOSPECS_API FindProtospecIDA(LPCSTR pszHierName, PROTOSPEC_ID& outID );
#ifdef UNICODE
	#define FindProtospecID FindProtospecIDW
#else
	#define FindProtospecID FindProtospecIDA
#endif

	// like FindProtospecID above, except you can specify which name to search on
	enum
	{
		SEARCH_SNAME = 0,
		SEARCH_LNAME,
		SEARCH_HNAME,
	};

	HRESULT PROTOSPECS_API FindProtospecW(LPCWSTR pszName, UINT nSearchFor, PROTOSPEC_ID& outID );
	HRESULT PROTOSPECS_API FindProtospecA(LPCSTR pszName, UINT nSearchFor, PROTOSPEC_ID& outID );
#ifdef UNICODE
	#define FindProtospec FindProtospecW
#else
	#define FindProtospec FindProtospecA
#endif

	HRESULT PROTOSPECS_API FindProtospecInstancesW(LPCWSTR pszHierName, PROTOSPEC_INSTID* payInstances, int cchInstances );
	HRESULT PROTOSPECS_API FindProtospecInstancesA(LPCSTR pszHierName, PROTOSPEC_INSTID* payInstances, int cchInstances );
#ifdef UNICODE
	#define FindProtospecInstances FindProtospecInstancesW
#else
	#define FindProtospecInstances FindProtospecInstancesA
#endif

	HRESULT PROTOSPECS_API FindProtospecInstanceW(LPCWSTR pszPath, PROTOSPEC_INSTID& outID );	// for example, L"IEEE 802.3//SNAP//IP//TCP//HTTP"
	HRESULT PROTOSPECS_API FindProtospecInstanceA(LPCSTR pszPath, PROTOSPEC_INSTID& outID );	// for example, L"IEEE 802.3//SNAP//IP//TCP//HTTP"
	HRESULT PROTOSPECS_API FindProtospecInstanceByID(PROTOSPEC_ID* payIDs, int nLength, PROTOSPEC_INSTID& outID );	// for example, payIDs[] = { 1, 9, 1000, 1400, 1418 }
#ifdef UNICODE
	#define FindProtospecInstance FindProtospecInstanceW
#else
	#define FindProtospecInstance FindProtospecInstanceA
#endif

	HRESULT PROTOSPECS_API BuildPSParentPathW(PROTOSPEC_INSTID nInstanceID, LPWSTR pszBuff, int cchBuff);	// for example, L"IEEE 802.3//SNAP//IP//TCP//HTTP"
	HRESULT PROTOSPECS_API BuildPSParentPathA(PROTOSPEC_INSTID nInstanceID, LPSTR pszBuff, int cchBuff);	// for example, L"IEEE 802.3//SNAP//IP//TCP//HTTP"
#ifdef UNICODE
	#define BuildPSParentPath BuildPSParentPathW
#else
	#define BuildPSParentPath BuildPSParentPathA
#endif

	// returns a reverse path to the root item in the array
	HRESULT PROTOSPECS_API BuildParentArray(PROTOSPEC_INSTID nInstanceID, PROTOSPEC_INSTID* payInstIDBuff, int &nBuffCount);

	HRESULT PROTOSPECS_API GetDataLayer( TMediaType inMediaType, TMediaSubType inMediaSubType,
							const UInt8* inPacket, UInt16 inPacketSize,
							PROTOSPEC_INSTID inID, PROTOSPEC_INSTID& outSource,
							const UInt8** ppData );
	HRESULT PROTOSPECS_API GetHeaderLayer( TMediaType inMediaType, TMediaSubType inMediaSubType,
							const UInt8* inPacket, UInt16 inPacketSize,
							PROTOSPEC_INSTID inID, PROTOSPEC_INSTID& outSource,
							const UInt8** ppData );

	HRESULT PROTOSPECS_API IsRootPSpec( PROTOSPEC_INSTID inID );

	typedef struct SProtospecInfo
	{
		PROTOSPEC_INSTID	nInstID;
		const UInt8*		pHeader;
		const UInt8*		pPayload;
	} SProtospecInfo;

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

	enum
	{
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
		RENDER_ALL = 0xffffffff
	};

	// render flags aren't really supported (yet)
	HRESULT PROTOSPECS_API GetRenderSize(UInt32 nRenderFlags, UInt32& nRenderSize);
	HRESULT PROTOSPECS_API RenderProtoSpecs(UInt8* pBuffer, UInt32 nBufferSize, UInt32 nRenderFlags);
}

#pragma pack( pop )

#endif
