// =============================================================================
//	PeekProxy.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "Plugin.h"
#include "Protospecs.h"
#include <atlstr.h>

using namespace Plugin;
using namespace ProtoSpecs;

// =============================================================================
//		CPeekProxy
// =============================================================================

class CPeekProxy
{
public:
	enum tServiceType {
		kAll = -1,
		kMediaUtils = 0x01,
		kNotify = 0x02,
		kNameTable = 0x04,
		kLookupNameEntry = 0x08
	};

protected:
	void*		m_pvMediaUtils;
	void*		m_pvNotify;
	void*		m_pvNameTable;
	void*		m_pvLookupNameEntry;

public:
	;			CPeekProxy();
	virtual		~CPeekProxy();

	SInt32		FinalConstruct();
	void		FinalRelease();
	void		SetPtr( tServiceType Type, void* p );
	void		ReleasePtr( tServiceType Type );

	// Media Utils
	UInt32		GetMediaString( TMediaType Type, TMediaSubType SubType, CString& strMedia );
	UInt32		GetMediaTypeString( TMediaType Type, CString& strMediaType );
	UInt32		GetMediaSubTypeString( TMediaSubType SubType, CString& strType );
	UInt32		GetMediaSpecClassString( TMediaSpecClass SpecType, CString& strClass );
	UInt32		GetMediaSpecTypeString( TMediaSpecType SubType, CString& strType );
	UInt32		GetMediaSpecTypePrefix( TMediaSpecType SubType, CString& strPrefix );
	UInt32		GetMediaSpecTypeDefault( TMediaSpecType SubType, CString& strType );
	UInt32		SpecToString( const TMediaSpec& Spec, CStringW& strText );
	UInt32		StringToSpec( CString strSpec, TMediaSpecType SpecType, TMediaSpec& Spec );

	// Notify
	UInt32		Notify( GUID& guidContext, GUID& guidSource, UInt32 nSourceKey, UInt64 nTimeStamp,
		int Severity, CStringW strShortMessage, CStringW strLongMessage );

	// Name Table
	UInt32		AddNameEntry( const NameEntry& Entry, UInt32 Options );
	UInt32		RemoveNameEntry( const TMediaSpec& Spec );

	// LookupNameEntry
	UInt32		LookupNameEntry( TMediaSpec& Spec, NameEntry& Entry, bool& bResult );

	// Protocol Specification (ProtoSpecs)
	UInt32		GetHierarchicalName( PROTOSPEC_INSTID inInstanceID, LPWSTR pszName, int cchName );
	UInt32		GetShortName( PROTOSPEC_INSTID inInstanceID, LPWSTR pszName, int cchName );
	UInt32		GetLongName( PROTOSPEC_INSTID inInstanceID, LPWSTR pszName, int cchName );
	UInt32		GetDescription( PROTOSPEC_INSTID inInstanceID, LPWSTR pszDesc, int cchDesc );
	UInt32		GetColor( PROTOSPEC_INSTID inInstanceID, COLORREF& cr );
	UInt32		GetNamedColor( LPCWSTR pszName, COLORREF& cr );
	UInt32		GetColorName( COLORREF cr, LPWSTR pszName, int cchName );
	UInt32		GetPacketProtoSpec( TMediaType inMediaType, TMediaSubType inMediaSubType, UInt32 inPacketFlags,
					const UInt8* inPacket, UInt16 inPacketSize, PROTOSPEC_INSTID& outID );
	UInt32		IsDescendentOf( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID* outMatchParentInstID, 
					const PROTOSPEC_INSTID* inParentIDs, SInt32 inParentCount, SInt32* outMatchParentIndex );
	UInt32		GetSibling( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID& outSibling );
	UInt32		GetChild( PROTOSPEC_INSTID inInstanceID, PROTOSPEC_INSTID& outChild );
	UInt32		BuildParentArray( PROTOSPEC_INSTID nInstanceID, PROTOSPEC_INSTID* payInstIDBuff, int &nBuffCount );
	UInt32		GetDataLayer( TMediaType inMediaType, TMediaSubType inMediaSubType, const UInt8* inPacket,
					UInt16 inPacketSize,  PROTOSPEC_INSTID inID, PROTOSPEC_INSTID& outSource, const UInt8** ppData );
	UInt32		GetHeaderLayer( TMediaType inMediaType, TMediaSubType inMediaSubType, const UInt8* inPacket,
					UInt16 inPacketSize, PROTOSPEC_INSTID inID, PROTOSPEC_INSTID& outSource, const UInt8** ppData );
	UInt32		IsRootProtoSpec( PROTOSPEC_INSTID inID );
	//UInt32		BuildParentInfo( TMediaType inMediaType, TMediaSubType inMediaSubType, const UInt8* inPacket, 
	//				UInt16 inPacketSize, PROTOSPEC_INSTID inID, SProtospecInfo* payInfo, int& nInfoCount );
};
