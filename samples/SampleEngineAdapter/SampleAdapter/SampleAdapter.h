// =============================================================================
//	SampleAdapter.h
// =============================================================================
//	Copyright (c) 2008 WildPackets, Inc. All rights reserved.

#pragma once

#include "stdafx.h"
#include "SampleAdapter.h"
#include "SampleParser.h"
#include "OmniPlugin.h"
#include "ObjectWithSiteImpl.h"
#include <captureplugin.h>
#include <map>


// =============================================================================
//		CSampleAdapter
// =============================================================================

class HE_NO_VTABLE CSampleAdapter :
	public CHeObjRoot<CHeMultiThreadModel>,
	public CHeClass<CSampleAdapter>,
	public IAdapter,
//	public IPeekPlugin,
	public IPacketProvider,
	public ICapturePackets,
	public ICaptureList,
	public IPluginCapture,
	public IPluginCaptureOption,
	public CSampleParser,	
	public IHeObjectWithSiteImpl<CSampleAdapter>
{
public:
	HE_DEFINE_STATIC_IID_ACCESSOR(CSampleAdapter_CID);

public:
	HE_INTERFACE_MAP_BEGIN(CSampleAdapter)		
		HE_INTERFACE_ENTRY_IID(IAdapter_IID,IAdapter)
		//HE_INTERFACE_ENTRY_IID(IPeekPlugin_IID,IPeekPlugin)
		HE_INTERFACE_ENTRY_IID(IPacketProvider_IID,IPacketProvider)
		HE_INTERFACE_ENTRY_IID(ICapturePackets_IID,ICapturePackets)
		HE_INTERFACE_ENTRY_IID(ICaptureList_IID,ICaptureList)
		HE_INTERFACE_ENTRY_IID(IPluginCapture_IID,IPluginCapture)
		HE_INTERFACE_ENTRY_IID(IPluginCaptureOption_IID,IPluginCaptureOption)		
		HE_INTERFACE_ENTRY_IID(HE_IHEOBJECTWITHSITE_IID,IHeObjectWithSite)
	HE_INTERFACE_MAP_END()

public:
	static CHePtr<IHeServiceProvider> m_spServices;
	static UInt32			m_dbgPacketNumber;

protected:
	CHePtr<IAdapterInfo>	m_spInfo;
	CHePtr<IProtoSpecs>		m_spPSpecs;
	CHePtr<IPacketBuffer>	m_spPacketBuffer;
	CHePtr<IMediaInfo>		m_spMediaInfo;		

	BOOL					m_bCapturing;
	BOOL					m_bPaused;	
	TMediaType				m_MediaType;
	TMediaSubType			m_MediaSubType;
	UInt64					m_ullLinkSpeed;
	UInt32					m_ulPacketCount;

	CHeBSTR					m_strAdapterName;
	CHeBSTR					m_strAdapterID;

	CHeBSTR					m_strDataPath;

	SOCKET					m_Socket;				
	HANDLE					m_hCaptureThread;
	HANDLE					m_hStopEvent;
	UInt16					m_nPort;

	UInt32					m_nRunningCaptures;			
	
	CHePtr<IPacketHandler>	m_spPacketHandler;
	BOOL					m_bInsertingPacket;
	BOOL					m_bGotPackets;

	std::map<HeID, COmniPlugin*>	m_mapCapIDToPluginObj;
	CHeCriticalSection				m_heCapIdPluginLock;

protected:
	void		CloseSocket();

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

	// IPeekPlugin
public:
//	HE_IMETHOD GetName(HEBSTR* pbstrName);
	HE_IMETHOD GetClassID( HeCID* pCLSID );

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

	// ICaptureList
	HE_IMETHOD AddCapture( IHeUnknown* pUnknown );
	HE_IMETHOD RemoveCapture( IHeUnknown* pUnknown );

	// IPluginCapture
	HE_IMETHOD AddPluginCapture( const Helium::HeID& idCapture, void* pPlugin );
	HE_IMETHOD RemovePluginCapture( const Helium::HeID& idCapture );
	HE_IMETHOD GetPluginCapture( const Helium::HeID& idCapture, void* pPlugin );	

	// IPluginCaptureOption
	HE_IMETHOD AddPluginCaptureOption( const Helium::HeID& idCapture );
	HE_IMETHOD RemovePluginCaptureOption( const Helium::HeID& idCapture );
	HE_IMETHOD GetPluginCaptureOption( const Helium::HeID& idCapture, void* pOption );
	HE_IMETHOD SetPluginCaptureOption( const Helium::HeID& idCapture, void* pOption );	

public:
	HE_IMETHOD SetSite( Helium::IHeUnknown* pUnkSite );

public:
	;			CSampleAdapter();
	virtual		~CSampleAdapter() {}

	HRESULT		FinalConstruct();
	void		FinalRelease();

	void		Run();	
	int			BindSocket();
	CHeBSTR		GetDataFileFullName();
	CHeBSTR		GetDataPath();
};
