// =============================================================================
//	RemoteAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "stdafx.h"
#include "OmniAdapter.h"
#include "SampleParser.h"
#include "OmniPlugin.h"
#include "ObjectWithSiteImpl.h"
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

class HE_NO_VTABLE CRemoteAdapter :
	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>,
	public HeLib::CHeClass<CRemoteAdapter>,
	public IAdapter,
	public IPacketProvider,
	public ICapturePackets,
	public IAdapterStats,
	public INdisAdapter,
	public IHeObjectWithSiteImpl<CRemoteAdapter>
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(CRemoteAdapter_CID);

public:
	HE_INTERFACE_MAP_BEGIN(CRemoteAdapter)
		HE_INTERFACE_ENTRY_IID(IAdapter_IID,IAdapter)
		HE_INTERFACE_ENTRY_IID(IPacketProvider_IID,IPacketProvider)
		HE_INTERFACE_ENTRY_IID(ICapturePackets_IID,ICapturePackets)
		HE_INTERFACE_ENTRY_IID(IAdapterStats_IID,IAdapterStats)
		HE_INTERFACE_ENTRY_IID(INdisAdapter_IID,INdisAdapter)
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

public:
	static CHePtr<Helium::IHeServiceProvider> m_spServices;

protected:
	CPeekEngineProxy		m_EngineProxy;
	COmniAdapter			m_Adapter;

	CHePtr<IProtoSpecs>		m_spPSpecs;

	CPacketHandler			m_PacketHandler;

	std::map<HeID, COmniPlugin*>	m_mapCapIDToPluginObj;

		// IAdapter
public:
	HE_IMETHOD GetAdapterInfo( IAdapterInfo** ppAdapterInfo );
	HE_IMETHOD GetHardwareOptions( IHeUnknown** ppHardwareOptions );
	HE_IMETHOD SetHardwareOptions( IHeUnknown* pHardwareOptions );
	HE_IMETHOD GetAdapterType( enum PeekAdapterType* pAdapterType );
	HE_IMETHOD GetName( BSTR* pbstrName );
	HE_IMETHOD GetMediaType( enum TMediaType* pMediaType );
	HE_IMETHOD GetMediaSubType( enum TMediaSubType* pMediaSubType );
	HE_IMETHOD GetLinkSpeed( ULONGLONG* pullLinkSpeed );
	HE_IMETHOD GetAddress( BYTE pAddress[6] );
	HE_IMETHOD GetNetSupportDuringCapture( BOOL* pbNetSupp );
	HE_IMETHOD Open();
	HE_IMETHOD Close();

public:
	HE_IMETHOD GetClassID( Helium::HeCID* pCLSID );

	// IAdapterStats
public:
	HE_IMETHOD GetStatistics( IPacketHandler* pHandler, void** ppStatistics);

	// INdisAdapter
public:
	HE_IMETHOD GetRingBufferSize( UInt32* pulRingBufferSize ) {
		if ( pulRingBufferSize != NULL ) {
			*pulRingBufferSize = 0;
		}
		return HE_S_OK;
	}
	HE_IMETHOD SetRingBufferSize(UInt32 ulRingBufferSize) {
		(void)ulRingBufferSize;
		return HE_S_OK;
	}
	HE_IMETHOD GetPacketsDropped(UInt64* pullPacketsDropped) {
		if ( pullPacketsDropped != NULL ) {
			*pullPacketsDropped = 0;
		}
		return HE_S_OK;
	}

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
	HE_IMETHOD SetSite( Helium::IHeUnknown* pUnkSite );

public:
	;			CRemoteAdapter() {}
	virtual		~CRemoteAdapter() {}

	HRESULT		FinalConstruct();
	void		FinalRelease();

	COmniAdapter&		GetAdapter() { return m_Adapter; }
	CPeekString			GetDataFileFullName();
	const CPeekString&	GetDataPath() { return m_Adapter.GetDataPath(); }

	HeResult	Run();
};
