// =============================================================================
//	PeekProxy.cpp
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekProxy.h"
#include <peeknames.h>

using namespace Plugin;


// =============================================================================
//		CPeekProxy
// =============================================================================

CPeekProxy::CPeekProxy()
	:	m_pvMediaUtils( NULL )
	,	m_pvNotify( NULL )
	,	m_pvNameTable( NULL )
	,	m_pvLookupNameEntry( NULL )
{
}

CPeekProxy::~CPeekProxy()
{
}


// -----------------------------------------------------------------------------
//		FinalConstruct
// -----------------------------------------------------------------------------

SInt32
CPeekProxy::FinalConstruct()
{
	HeResult	hr = HE_S_OK;

	try {
		CHePtr<IMediaUtils>		spMediaUtils;
		HeThrowIfFailed( spMediaUtils.CreateInstance( "PeekCore.MediaUtils" ) );
		SetPtr( kMediaUtils, spMediaUtils.p );

		hr = ProtoSpecs::IsProtospecsLoaded();
		hr = HE_S_OK;

	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		FinalRelease
// -----------------------------------------------------------------------------

void
CPeekProxy::FinalRelease()
{
	ReleasePtr( kAll );
}


// -----------------------------------------------------------------------------
//		ReleasePtr
// -----------------------------------------------------------------------------

void
CPeekProxy::ReleasePtr(
	tServiceType	Type )
{
	if ( Type & kMediaUtils ) {
		IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
		if ( pMediaUtils ) {
			pMediaUtils->Release();
			m_pvMediaUtils = NULL;
		}
	}
}

// -----------------------------------------------------------------------------
//		SetPtr
// -----------------------------------------------------------------------------

void
CPeekProxy::SetPtr(
	tServiceType	Type,
	void*			p )
{
	switch ( Type ) {
	case kMediaUtils:
		{
			IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( p ) );
			if ( pMediaUtils ) {
				m_pvMediaUtils = p;
				pMediaUtils->AddRef();
			}
		}
		break;

	case kNotify:
		{
			INotify*	pNotify( reinterpret_cast<INotify*>( p ) );
			if ( pNotify ) {
				m_pvNotify = pNotify;
				pNotify->AddRef();
			}
		}

	case kNameTable:
		{
			INameTable*	pNameTable( reinterpret_cast<INameTable*>( p ) );
			if ( pNameTable ) {
				m_pvNameTable = pNameTable;
				pNameTable->AddRef();
			}
		}
		break;

	case kLookupNameEntry:
		{
			ILookupNameEntry*	pLookupNameEntry( reinterpret_cast<ILookupNameEntry*>( p ) );
			if ( pLookupNameEntry ) {
				m_pvLookupNameEntry = pLookupNameEntry;
				pLookupNameEntry->AddRef();
			}
		}
		break;
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//		Media Utils
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// -----------------------------------------------------------------------------
//		GetMediaString
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaString(
	TMediaType		Type,
	TMediaSubType	SubType,
	CStringW&		strMedia )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrMedia;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaString( Type, SubType, &bstrMedia.m_str ) );
		strMedia = bstrMedia;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		GetMediaTypeString
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaTypeString(
	TMediaType	Type,
	CStringW&	strMediaType )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrMediaType;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaTypeString( Type, &bstrMediaType.m_str ) );
		strMediaType = bstrMediaType;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		GetMediaSubTypeString
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaSubTypeString(
	TMediaSubType	SubType,
	CStringW&		strType )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrType;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaSubTypeString( SubType, &bstrType.m_str ) );
		strType = bstrType;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecClassString
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaSpecClassString(
	TMediaSpecClass	SpecType,
	CStringW&		strClass )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrClass;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaSpecClassString( SpecType, &bstrClass.m_str ) );
		strClass = bstrClass;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecTypeString
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaSpecTypeString(
	TMediaSpecType	SubType,
	CStringW&		strType )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrType;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaSpecTypeString( SubType, &bstrType.m_str ) );
		strType = bstrType;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecTypePrefix
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaSpecTypePrefix(
	TMediaSpecType	SubType,
	CStringW&		strPrefix )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrPrefix;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaSpecTypePrefix( SubType, &bstrPrefix.m_str ) );
		strPrefix = bstrPrefix;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		GetMediaSpecTypeDefault
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetMediaSpecTypeDefault(
	TMediaSpecType	SubType,
	CStringW&		strType )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrType;
	try {
		ThrowIfFailed( pMediaUtils->GetMediaSpecTypeDefault( SubType, &bstrType.m_str ) );
		strType = bstrType;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		SpecToString
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::SpecToString(
	const TMediaSpec&	Spec,
	CStringW&			strText )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrText;
	try {
		ThrowIfFailed( pMediaUtils->SpecToString( &Spec, &bstrText.m_str ) );
		strText = bstrText;
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		StringToSpec
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::StringToSpec(
	CString			strSpec,
	TMediaSpecType	SpecType,
	TMediaSpec&		Spec )
{
	HeResult		hr = HE_S_OK;
	IMediaUtils*	pMediaUtils( reinterpret_cast<IMediaUtils*>( m_pvMediaUtils ) );
	ThrowIf( !pMediaUtils );

	CHeBSTR	bstrSpec( strSpec.GetBuffer() );
	hr = pMediaUtils->StringToSpec( bstrSpec, SpecType, &Spec );

	return hr;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//		Notify
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// -----------------------------------------------------------------------------
//		Notify
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::Notify(
	GUID&	guidContext,
	GUID&	guidSource,
	UInt32	nSourceKey,
	UInt64	nTimeStamp,
	int		Severity,
	CStringW	strShortMessage,
	CStringW	strLongMessage )
{
	HeResult	hr = HE_E_FAIL;

	try {
		INotify*	pNotify( reinterpret_cast<INotify*>( m_pvNotify ) );
		if ( pNotify ) {
			hr = pNotify->Notify( *(reinterpret_cast<HeID*>( &guidContext )), *(reinterpret_cast<HeID*>( &guidSource )),
				nSourceKey, nTimeStamp, static_cast<PeekSeverity>( Severity ), strShortMessage.GetBuffer(),
				strLongMessage.GetBuffer() );
		}
	}
	HE_CATCH( hr );

	return hr;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//		Name Table
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// -----------------------------------------------------------------------------
//		AddNameEntry
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::AddNameEntry(
	const NameEntry&	Entry,
	UInt32				Options )
{
	HeResult	hr = HE_E_FAIL;

	try {
		INameTable*	pNameTable( reinterpret_cast<INameTable*>( m_pvNameTable ) );
		if ( pNameTable ) {
			hr = pNameTable->Add( reinterpret_cast<const PeekNameEntry*>( &Entry ), Options );
		}
	}
	HE_CATCH( hr );

	return hr;
}


// -----------------------------------------------------------------------------
//		RemoveNameEntry
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::RemoveNameEntry(
	const TMediaSpec&	Spec )
{
	HeResult	hr = HE_E_FAIL;

	try {
		INameTable*	pNameTable( reinterpret_cast<INameTable*>( m_pvNameTable ) );
		if ( pNameTable ) {
			hr = pNameTable->Remove( &Spec );
		}
	}
	HE_CATCH( hr );

	return hr;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//		Lookup Name Entry
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// -----------------------------------------------------------------------------
//		LookupNameEntry
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::LookupNameEntry(
	TMediaSpec&		Spec,
	NameEntry&		Entry,
	bool&			bResult )
{
	HeResult	hr = HE_E_FAIL;

	try {
		ILookupNameEntry*	pLookupNameEntry( reinterpret_cast<ILookupNameEntry*>( m_pvLookupNameEntry ) );
		if ( pLookupNameEntry ) {
			BOOL	b = FALSE;
			hr = pLookupNameEntry->LookupNameEntry( &Spec, reinterpret_cast<PeekNameEntry*>( &Entry ), &b );
			if ( HE_SUCCEEDED( hr ) ) {
				bResult = (b != FALSE);
			}
		}
	}
	HE_CATCH( hr );

	return hr;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//		Protocol Specification (ProtoSpecs)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// -----------------------------------------------------------------------------
//		GetHierarchicalName
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetHierarchicalName(
	PROTOSPEC_INSTID	inInstanceID,
	LPWSTR				pszName,
	int					cchName )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPSHierarchicalName( inInstanceID, pszName, cchName );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetShortName
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetShortName(
	PROTOSPEC_INSTID	inInstanceID,
	LPWSTR				pszName,
	int					cchName )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPSShortName( inInstanceID, pszName, cchName );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetLongName
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetLongName(
	PROTOSPEC_INSTID	inInstanceID,
	LPWSTR				pszName,
	int					cchName )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPSLongName( inInstanceID, pszName, cchName );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetDescription
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetDescription(
	PROTOSPEC_INSTID	inInstanceID,
	LPWSTR				pszDesc,
	int					cchDesc )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPSDescription( inInstanceID, pszDesc, cchDesc );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetColor
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetColor(
	PROTOSPEC_INSTID	inInstanceID,
	COLORREF&			cr )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetColor( inInstanceID, cr );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetNamedColor
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetNamedColor(
	LPCWSTR		pszName,
	COLORREF&	cr )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPSNamedColor( pszName, cr );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetColorName
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetColorName(
	COLORREF	cr,
	LPWSTR		pszName,
	int			cchName )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPSColorName( cr, pszName, cchName );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetPacketProtoSpec
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetPacketProtoSpec(
	TMediaType			inMediaType,
	TMediaSubType		inMediaSubType,
	UInt32				inPacketFlags,
	const UInt8*		inPacket,
	UInt16				inPacketSize,
	PROTOSPEC_INSTID&	outID )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetPacketProtoSpec( inMediaType, inMediaSubType, inPacketFlags, inPacket, inPacketSize, outID );
	return hr;
}


// -----------------------------------------------------------------------------
//		IsDescendentOf
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::IsDescendentOf(
	PROTOSPEC_INSTID		inInstanceID,
	PROTOSPEC_INSTID*		outMatchParentInstID, 
	const PROTOSPEC_INSTID*	inParentIDs,
	SInt32					inParentCount,
	SInt32*					outMatchParentIndex )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::IsDescendentOf( inInstanceID, outMatchParentInstID, inParentIDs, inParentCount, outMatchParentIndex );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetSibling
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetSibling(
	PROTOSPEC_INSTID	inInstanceID,
	PROTOSPEC_INSTID&	outSibling )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetSibling( inInstanceID, outSibling );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetChild
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetChild(
	PROTOSPEC_INSTID	inInstanceID,
	PROTOSPEC_INSTID&	outChild )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetChild( inInstanceID, outChild );
	return hr;
}


// -----------------------------------------------------------------------------
//		BuildParentArray
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::BuildParentArray(
	PROTOSPEC_INSTID nInstanceID,
	PROTOSPEC_INSTID* payInstIDBuff,
	int &nBuffCount )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::BuildParentArray( nInstanceID, payInstIDBuff, nBuffCount );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetDataLayer
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetDataLayer(
	TMediaType			inMediaType,
	TMediaSubType		inMediaSubType,
	const UInt8*		inPacket,
	UInt16				inPacketSize,
	PROTOSPEC_INSTID	inID,
	PROTOSPEC_INSTID&	outSource,
	const UInt8**		ppData )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetDataLayer( inMediaType, inMediaSubType, inPacket, inPacketSize,  inID, outSource, ppData );
	return hr;
}


// -----------------------------------------------------------------------------
//		GetHeaderLayer
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::GetHeaderLayer(
	TMediaType			inMediaType,
	TMediaSubType		inMediaSubType,
	const UInt8*		inPacket,
	UInt16				inPacketSize,
	PROTOSPEC_INSTID	inID,
	PROTOSPEC_INSTID&	outSource,
	const UInt8**		ppData )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::GetHeaderLayer( inMediaType, inMediaSubType, inPacket, inPacketSize, inID, outSource, ppData );
	return hr;
}


// -----------------------------------------------------------------------------
//		IsRootProtoSpec
// -----------------------------------------------------------------------------

UInt32
CPeekProxy::IsRootProtoSpec(
	PROTOSPEC_INSTID	inID )
{
	UInt32	hr = HE_S_OK;
	hr = ProtoSpecs::IsRootPSpec( inID );
	return hr;
}


// -----------------------------------------------------------------------------
//		BuildParentInfo
// -----------------------------------------------------------------------------

//UInt32
//CPeekProxy::BuildParentInfo(
//	TMediaType			inMediaType,
//	TMediaSubType		inMediaSubType,
//	const UInt8*		inPacket, 
//	UInt16				inPacketSize,
//	PROTOSPEC_INSTID	inID,
//	SProtospecInfo*		payInfo,
//	int&				nInfoCount )
//{
//	UInt32	hr = HE_S_OK;
//	hr = ProtoSpecs::BuildParentInfo( inMediaType, inMediaSubType, inPacket, inPacketSize, inID, payInfo, nInfoCount );
//	return hr;
//}
