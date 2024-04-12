// =============================================================================
//	RemoteAdapter.h
// =============================================================================
//	Copyright (c) 2008-2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "OmniAdapter.h"
#include "OmniPlugin.h"
#include "objectwithsiteimpl.h"
#include "heid.h"
#include "hecom.h"
#include "hlsync.h"
#include <map>

class CRemotePlugin;

// {BD809200-3453-4e53-975C-82417293550A}
#define CRemoteAdapter_CID \
	{ 0xBD809200, 0x3453, 0x4E53, { 0x97, 0x5C, 0x82, 0x41, 0x72, 0x93, 0x55, 0x0A } }


// =============================================================================
//		CRemoteAdapter
// =============================================================================

class HE_NO_VTABLE CRemoteAdapter
	:	public HeLib::CHeObjRoot<HeLib::CHeMultiThreadModel>
	,	public HeLib::CHeClass<CRemoteAdapter>
	,	public IAdapterInitialize
	,	public IAdapter
	,	public IPacketProvider
	,	public ICapturePackets
	,	public IPluginAdapter
	,	public INdisAdapter
#ifdef IMPLEMENT_SUMMARY
	,	public IUpdateSummaryStats
#endif	// IMPLEMENT_SUMMARY
	,	public Helium::IHePersistXml
	,	public IHeObjectWithSiteImpl<CRemoteAdapter>
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(CRemoteAdapter_CID);

public:
	HE_INTERFACE_MAP_BEGIN(CRemoteAdapter)
		HE_INTERFACE_ENTRY_IID(IAdapterInitialize_IID,IAdapterInitialize)
		HE_INTERFACE_ENTRY_IID(IAdapter_IID,IAdapter)
		HE_INTERFACE_ENTRY_IID(IPacketProvider_IID,IPacketProvider)
		HE_INTERFACE_ENTRY_IID(ICapturePackets_IID,ICapturePackets)
		HE_INTERFACE_ENTRY_IID(IPluginAdapter_IID, IPluginAdapter)
		HE_INTERFACE_ENTRY_IID(INdisAdapter_IID,INdisAdapter)
#ifdef IMPLEMENT_SUMMARY
		HE_INTERFACE_ENTRY_IID(IUpdateSummaryStats_IID,IUpdateSummaryStats)
#endif	// IMPLEMENT_SUMMARY
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSIST_IID,IHePersist)
		HE_INTERFACE_ENTRY_IID(HE_IHEPERSISTXML_IID,IHePersistXml)
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

protected:
	CHePtr<IPluginAdapter>	m_spPluginAdapter;
	CPeekEngineProxy		m_EngineProxy;
	CHePtr<IProtoSpecs>		m_spPSpecs;
	CPeekDataModelerPtr		m_spPrefsModeler;
	COmniAdapter			m_OmniAdapter;
	UInt32					m_nRingBufferSize;

public:
	;			CRemoteAdapter();
	virtual		~CRemoteAdapter();

	HRESULT		FinalConstruct();
	void		FinalRelease();

	COmniAdapter&	GetAdapter() { return m_OmniAdapter; }

	HeResult	Run();

public:
	HE_IMETHOD	GetClassID( Helium::HeCID* pCLSID );

	// IAdapterInitialize
public:
	HE_IMETHOD	Initialize( IAdapterInfo* pAdapterInfo );

	// IAdapter
public:
	HE_IMETHOD	GetAdapterInfo( IAdapterInfo** ppAdapterInfo );
	HE_IMETHOD	GetHardwareOptions( IHeUnknown** ppHardwareOptions );
	HE_IMETHOD	SetHardwareOptions( IHeUnknown* pHardwareOptions );
	HE_IMETHOD	GetAdapterType( enum PeekAdapterType* pAdapterType );
	HE_IMETHOD	GetName( Helium::HEBSTR* pbstrName );
	HE_IMETHOD	GetMediaType( enum TMediaType* pMediaType );
	HE_IMETHOD	GetMediaSubType( enum TMediaSubType* pMediaSubType );
	HE_IMETHOD	GetLinkSpeed( UInt64* pullLinkSpeed );
	HE_IMETHOD	GetAddress( UInt8 pAddress[6] );
	HE_IMETHOD	GetNetSupportDuringCapture( BOOL* pbNetSupp );
	HE_IMETHOD	Open();
	HE_IMETHOD	Close();

	// IPacketProvider
public:
	HE_IMETHOD	AddPacketHandler( IPacketHandler* pHandler );
	HE_IMETHOD	RemovePacketHandler( IPacketHandler* pHandler );

	// ICapturePackets
public:
	HE_IMETHOD	IsCapturing( BOOL *pbCapturing );
	HE_IMETHOD	PauseCapture();
	HE_IMETHOD	ResumeCapture();
	HE_IMETHOD	StartCapture();
	HE_IMETHOD	StopCapture();

	// IPluginAdapter
public:
	HE_IMETHOD DeleteAdapter( const Helium::HeID& adapterID );

	// INdisAdapter
public:
	HE_IMETHOD GetRingBufferSize(UInt32* pulRingBufferSize);
	HE_IMETHOD SetRingBufferSize(UInt32 ulRingBufferSize);
	HE_IMETHOD GetPacketsDropped(UInt64* pullPacketsDropped);

#ifdef IMPLEMENT_SUMMARY
	// IUpdateSummaryStats
public:
	HE_IMETHOD	UpdateSummaryStats( IHeUnknown* pSnapshot );
#endif	// IMPLEMENT_SUMMARY

	// IHePersistXml
public:
	HE_IMETHOD	Load( IHeUnknown* inDocument, IHeUnknown* inNode );
	HE_IMETHOD	Save( IHeUnknown* ioDocument, IHeUnknown* ioNode, int inClearDirty );

	// IHeObjectWithSiteImpl
public:
	HE_IMETHOD	SetSite( Helium::IHeUnknown* pUnkSite );
};
