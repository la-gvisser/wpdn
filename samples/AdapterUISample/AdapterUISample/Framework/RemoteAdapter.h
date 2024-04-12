// =============================================================================
//	RemoteAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "StdAfx.h"
#include "OmniAdapter.h"
#include "OmniPlugin.h"
#include "objectwithsiteimpl.h"
#include "AdapterStats.h"
#include "hecom.h"
#include "hlsync.h"
//#include <captureplugin.h>
#include <map>

// {BD809200-3453-4e53-975C-82417293550A}
#define CRemoteAdapter_CID \
	{ 0xBD809200, 0x3453, 0x4E53, { 0x97, 0x5C, 0x82, 0x41, 0x72, 0x93, 0x55, 0x0A } }


// =============================================================================
//		CRemoteAdapter
// =============================================================================

class HE_NO_VTABLE CRemoteAdapter
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
	,	public HeLib::CHeClass<CRemoteAdapter>
	,	public IAdapter
	,	public IPacketProvider
	,	public ICapturePackets
	,	public IAdapterStats
	,	public IHeObjectWithSiteImpl<CRemoteAdapter>
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(CRemoteAdapter_CID);

public:
	HE_INTERFACE_MAP_BEGIN(CRemoteAdapter)
		HE_INTERFACE_ENTRY_IID(IAdapter_IID,IAdapter)
		HE_INTERFACE_ENTRY_IID(IPacketProvider_IID,IPacketProvider)
		HE_INTERFACE_ENTRY_IID(ICapturePackets_IID,ICapturePackets)
		HE_INTERFACE_ENTRY_IID(IAdapterStats_IID,IAdapterStats)
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

protected:
	COmniAdapter	m_OmniAdapter;

	// IAdapter
public:
	HE_IMETHOD GetAdapterInfo( IAdapterInfo** ppAdapterInfo );
	HE_IMETHOD GetHardwareOptions( IHeUnknown** ppHardwareOptions );
	HE_IMETHOD SetHardwareOptions( IHeUnknown* pHardwareOptions );
	HE_IMETHOD GetAdapterType( enum PeekAdapterType* pAdapterType );
	HE_IMETHOD GetName( Helium::HEBSTR* pbstrName );
	HE_IMETHOD GetMediaType( enum TMediaType* pMediaType );
	HE_IMETHOD GetMediaSubType( enum TMediaSubType* pMediaSubType );
	HE_IMETHOD GetLinkSpeed( UInt64* pullLinkSpeed );
	HE_IMETHOD GetAddress( UInt8* pAddress );
	HE_IMETHOD GetNetSupportDuringCapture( BOOL* pbNetSupp );
	HE_IMETHOD Open();
	HE_IMETHOD Close();

public:
	HE_IMETHOD GetClassID( Helium::HeCID* pCLSID );

	// IAdapterStats
public:
	HE_IMETHOD GetStatistics( IPacketHandler* pHandler, void** ppStatistics);

	// ICapturePackets
public:
	HE_IMETHOD IsCapturing( BOOL *pbCapturing );
	HE_IMETHOD PauseCapture();
	HE_IMETHOD ResumeCapture();
	HE_IMETHOD StartCapture();
	HE_IMETHOD StopCapture();

	// IPacketProvider
public:
	HE_IMETHOD AddPacketHandler( IPacketHandler* pHandler );
	HE_IMETHOD RemovePacketHandler( IPacketHandler* pHandler );

	// IHeObjectWithSiteImpl override
public:
	HE_IMETHOD	SetSite( Helium::IHeUnknown* pUnkSite );

public:
	;			CRemoteAdapter();
	virtual		~CRemoteAdapter() {}

	HRESULT		FinalConstruct();
	void		FinalRelease();

	HeResult	Run();
};
