// =============================================================================
//	PeekContextProxy.h
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "peekcore.h"
#include "omnigraphs.h"
#include "Peek.h"
#include "ProxySummaryStats.h"
#include "Packet.h"
#include "PeekStrings.h"
#include "GlobalId.h"

class CPersistFile;
class CPacketHandler;


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Packet Buffer
//
//	A Packet Buffer provides read access to Base Packet Buffer. The OmniEngine
//	supplies a Packet Buffer, and therefor read-only access, with Filter and
//	Process Packet methods. Create a Capture Buffer or an array of packets for
//	read and write access to in memory packet storage.
//
//	Dynamic Packet Buffer
//
//	A Dynamic Packet Buffer provides write access to a Base Packet Buffer. Use
//	a Capture Buffer or an array of packets for read and write access to an in
//	memory packet storage.
//
//	Use a Capture Buffer to efficiently manage a large number packets in memory.
//	Use an array of Packets to manage a small number of packets.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// =============================================================================
//		Site Services
// =============================================================================
//
//	PeekSetup - Engine Context
//		FilterCollection			: IFilterCollection
//		MessageLog					: ILogMessage
//		NameTable					: INameTable
//		HardwareOptionsCollection	: IHardwareOptionsCollection
//		ExpertPrefs					: IExpertPrefs
//		NotifyService				: INotify
//		Engine						: IEngine
//		SimplePropBag				: Helium::IHePropertyBag
//		ActivationInfo				: IActivationInfo
//		FileLog						: IFileLog
//		Engine						: IEngine
//		AlarmCollection				: IAlarmCollection
//		EnginePluginCollection		: IEnginePluginCollection
//		AdapterManager				: IAdapterManager
//		CaptureTemplateCollection	: ICaptureTemplateCollection
//		EngineManagement			: IEngineManagement
//		AppConfig					: IAppConfig
//		GraphTemplateMgr			: IGraphTemplateMgr
//
//	PeekCapture - EngineContext
//		StatsContext
//		PluginManager
//		StatsServer
//		: PeekSetup
//
//	GraphsContext - Engine Context
//		StatsContext
//		GraphsContext
//		PluginManager
//		StatsServer
//		: PeekSetup
//		- PeekSetup:NotifyService is blocked.
//
//	FileContext
//		IObjectWithProperties
//		IObjectWithProperties2
//		IHeServiceProvider
//		IServiceContainer
//		IEnumerable
//		IHeObjectWithSite


// =============================================================================
//		Data Structures and Constants
// =============================================================================

enum {
	kContextType_Unknown,
	kContextType_Null,		// Created at initialization, it has no context id.
	kContextType_Capture,
	kContextType_Graphs,
	kContextType_File
};


// =============================================================================
//		CStream
// =============================================================================

class CStream
{
private:
	CHePtr<Helium::IHeStream>	m_spStream;

protected:
	Helium::IHeStream*	GetPtr() { return m_spStream; }

	void		ReleasePtr() { m_spStream.Release(); }

	void		SetPtr( Helium::IHeStream* inStream ) { m_spStream = inStream; }

public:
	;			CStream( Helium::IHeStream* inStream = NULL )
		: m_spStream( NULL )
	{
		SetPtr( inStream );
	}
	;			~CStream() {}

	bool		Read( void* outData, UInt32 inCount, UInt32* outCount );

	UInt64		Seek( SInt64 inOffset, UInt32 inOrigin );
	bool		SetSize( UInt64 inNewSize );

	bool		Write( const void* inData, UInt32 inCount, UInt32* outCount );

	//bool		Clone(IHeStream** ppStream) = 0;
	//bool		CopyTo( IHeStream* pStream, UInt64 cb, UInt64* pcbRead, UInt64* pcbWritten );
	//bool		LockRegion(UInt64 offset, UInt64 cb, UInt32 lockType) = 0;
	//bool		Revert();
	//bool		Stat(HE_STATSTG* pStat, UInt32 grfStatFlag) = 0;
	//bool		UnlockRegion(UInt64 offset, UInt64 cb, UInt32 lockType) = 0;
};


// =============================================================================
//		CMediaInfo
// =============================================================================

typedef std::vector<UInt32>				tChannelList;
typedef std::vector<WirelessChannel>	tWirelessChannelList;
typedef std::vector<UInt32>				tRateList;

class CMediaInfo
{
	friend class CPacketBuffer;
	friend class CPluginAdapterInfo;

protected:
	CHePtr<IMediaInfo>	m_spMediaInfo;

protected:
	IMediaInfo*	GetPtr() { return m_spMediaInfo; }
	void		ReleasePtr() { m_spMediaInfo.Release(); }
	void		SetPtr( IMediaInfo* inMediaInfo ) { m_spMediaInfo = inMediaInfo; }

public:
	;		CMediaInfo( IMediaInfo* inMediaInfo = NULL )
		: m_spMediaInfo( NULL )
	{
		SetPtr( inMediaInfo );
	}
	;		~CMediaInfo() {}

	tChannelList	GetChannels() const {
		tChannelList	ayChannels;
		if ( IsValid() ) {
			UInt32	nCount( 0 );
			UInt32*	pChannels( NULL );
			Peek::ThrowIfFailed( m_spMediaInfo->GetChannels( &pChannels, &nCount ) );
			ayChannels.assign( pChannels, (pChannels + nCount) );
		}
		return ayChannels;
	}
	UInt64		GetLinkSpeed() const {
		UInt64	nLinkSpeed( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->GetLinkSpeed( &nLinkSpeed ) );
		}
		return nLinkSpeed;
	}
	TMediaDomain	GetMediaDomain() const {
		TMediaDomain	md( kMediaDomain_Null );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->GetMediaDomain( &md ) );
		}
		return md;
	}
	tMediaType	GetMediaType() const {
		tMediaType	mt;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->GetMediaType( &mt.fType ) );
			Peek::ThrowIfFailed( m_spMediaInfo->GetMediaSubType( &mt.fSubType ) );
		}
		return mt;
	}
	tWirelessChannelList	GetWirelessChannels() const {
		tWirelessChannelList	ayChannels;
		if ( IsValid() ) {
			UInt32				nCount( 0 );
			WirelessChannel*	pChannels( NULL );
			Peek::ThrowIfFailed( m_spMediaInfo->GetWirelessChannels( &pChannels, &nCount ) );
			ayChannels.assign( pChannels, (pChannels + nCount) );
		}
		return ayChannels;
	}
	tRateList		GetWirelessDataRates() const {
		tRateList	ayRates;
		if ( IsValid() ) {
			UInt32	nCount( 0 );
			UInt32*	pRates( NULL );
			Peek::ThrowIfFailed( m_spMediaInfo->GetWirelessDataRates( &pRates, &nCount ) );
			ayRates.assign( pRates, (pRates + nCount) );
		}
		return ayRates;
	}

	bool	IsNotValid() const { return (m_spMediaInfo == NULL); }
	bool	IsValid() const { return (m_spMediaInfo != NULL); }

	void	SetChannels( const tChannelList& inChannels ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->SetChannels( &inChannels[0], (UInt32)inChannels.size() ) );
		}
	}
	void	SetLinkSpeed( UInt64 inLinkSpeek ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->SetLinkSpeed( inLinkSpeek ) );
		}
	}
	void	SetMediaDomain( TMediaDomain inMediaDomain ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->SetMediaDomain( inMediaDomain ) );
		}
	}
	void	SetMediaType( tMediaType inMediaType ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->SetMediaType( inMediaType.fType ) );
			Peek::ThrowIfFailed( m_spMediaInfo->SetMediaSubType( inMediaType.fSubType ) );
		}
	}
	void	SetWirelessChannels( const tWirelessChannelList& inChannels ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->SetWirelessChannels( &inChannels[0], (UInt32)inChannels.size() ) );
		}
	}
	void	SetWirelessDataRates( const tRateList& inRates ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spMediaInfo->SetWirelessDataRates( &inRates[0], (UInt32)inRates.size() ) );
		}
	}
};


// =============================================================================
//		CAdapterInfo
// =============================================================================

class CAdapterInfo
{
	friend class CAdapterManager;

protected:
	CHePtr<IAdapterInfo>	m_spAdapterInfo;

protected:
	IAdapterInfo*	GetPtr() { return m_spAdapterInfo; }
	void			ReleasePtr() { m_spAdapterInfo.Release(); }
	void			SetPtr( IAdapterInfo* inAdapterInfo ) {
		m_spAdapterInfo= inAdapterInfo;
	}

public:
	;		CAdapterInfo( IAdapterInfo* inAdapterInfo = NULL )
		: m_spAdapterInfo( NULL )
	{
		SetPtr( inAdapterInfo );
	}
	;		~CAdapterInfo() {}

	bool	CompareAddress( const tEthernetAddress& inAddress ) {
		BOOL	bResult( FALSE );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->CompareAddress( inAddress.bytes, &bResult ) );
		}
		return (bResult != FALSE);
	}
	void	Copy( IAdapterInfo** outAdapterInfor ) {
		m_spAdapterInfo.CopyTo( outAdapterInfor );
	}
	void	Create() {
		m_spAdapterInfo.CreateInstance( "PeekCore.PluginAdapterInfo" );
	}

	PeekAdapterType	GetAdapterType() {
		PeekAdapterType	enAdapterType( peekNullAdapter );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->GetAdapterType( &enAdapterType ) );
		}
		return enAdapterType;
	}
	CEthernetAddress	GetAddress() const {
		tEthernetAddress	theAddress;
		memset( &theAddress, 0, sizeof( theAddress ) );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->GetAddress( theAddress.bytes ) );
		}
		return theAddress;
	}
	bool		GetDeletable() const {
		BOOL	bDeletable( FALSE );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->GetDeletable( &bDeletable ) );
		}
		return (bDeletable != FALSE);
	}
	CPeekString	GetDescription() const {
		CPeekString	strDescription;
		if ( IsValid() ) {
			Helium::HEBSTR	bstrDescription;
			Peek::ThrowIfFailed( m_spAdapterInfo->GetDescription( &bstrDescription ) );
			strDescription = CPeekString( bstrDescription );
			HeLib::HeFreeString( bstrDescription );
		}
		return strDescription;
	}
	CPeekString	GetId() const {
		CPeekString	strId;
		if ( IsValid() ) {
			Helium::HEBSTR	bstrId;
			Peek::ThrowIfFailed( m_spAdapterInfo->GetIdentifier( &bstrId ) );
			strId = CPeekString( bstrId );
			HeLib::HeFreeString( bstrId );
		}
		return strId;
	}
	UInt64		GetLinkSpeed() const {
		UInt64	nLinkSpeed( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->GetLinkSpeed( &nLinkSpeed ) );
		}
		return nLinkSpeed;
	}
	tMediaType	GetMediaType() const {
		tMediaType	mt;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->GetMediaType( &mt.fType ) );
			Peek::ThrowIfFailed( m_spAdapterInfo->GetMediaSubType( &mt.fSubType ) );
		}
		return mt;
	}
	bool		GetPersistent() const {
		BOOL	bPersisitent( FALSE );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterInfo->GetPersistent( &bPersisitent ) );
		}
		return (bPersisitent != FALSE);
	}

	bool	IsDeletable() const { return GetDeletable(); }
	bool	IsNotValid() const { return (m_spAdapterInfo == NULL); }
	bool	IsPersisitent() const { return GetPersistent(); }
	bool	IsValid() const { return (m_spAdapterInfo != NULL); }

	void	SetDescription( const CPeekString& inDescription ) {
		if ( IsValid() ) {
			CHeBSTR	bstrDescription( inDescription );
			Peek::ThrowIfFailed( m_spAdapterInfo->SetDescription( bstrDescription ) );
		}
	}

#if (0)
	/// Get the channels that the adapter supports.
	HE_IMETHOD GetChannelManager(IChannelManager** ppChannelManager ) = 0;
	/// Get the media info.
	HE_IMETHOD GetMediaInfo(IMediaInfo** ppMediaInfo) = 0;
	/// Compare an address to the adapter physical address.
	HE_IMETHOD CreateAdapter(/*out*/ IAdapter** ppAdapter) = 0;
#endif
};


// =============================================================================
//		CPluginAdapterInfo
// =============================================================================

class CPluginAdapterInfo
	:	public CAdapterInfo
{
protected:
	CHePtr<IPluginAdapterInfo>	m_spPluginAdapterInfo;

protected:
	IPluginAdapterInfo*	GetPtr() { return m_spPluginAdapterInfo; }
	void				ReleasePtr() {
		m_spPluginAdapterInfo.Release();
		CAdapterInfo::ReleasePtr();
	}
	void				SetPtr( IPluginAdapterInfo* inPluginAdapterInfoformation ) {
		HeLib::CHeQIPtr<IAdapterInfo>	spAdapterInfo = inPluginAdapterInfoformation;
		CAdapterInfo::SetPtr( spAdapterInfo );
		if ( CAdapterInfo::IsValid() ) {
			m_spPluginAdapterInfo = inPluginAdapterInfoformation;
		}
	}

public:
	;		CPluginAdapterInfo( IPluginAdapterInfo* inPluginAdapterInfo = NULL )
		:	m_spPluginAdapterInfo( NULL )
	{
		SetPtr( inPluginAdapterInfo );
	}
	;		~CPluginAdapterInfo() {}

	void	Create() {
		CAdapterInfo::Create();
		if ( CAdapterInfo::IsValid() ) {
			m_spAdapterInfo.QueryInterface( &m_spPluginAdapterInfo.p );
		}
	}

	PeekPluginAdapterType GetPluginAdapterType() const {
		PeekPluginAdapterType	nType( peekNullPluginAdapter );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->GetPluginAdapterType( &nType ) );
		}
		return nType;
	}
	CPeekString			GetPluginName() const {
		CPeekString	strName;
		if ( IsValid() ) {
			Helium::HEBSTR	bstrName;
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->GetPluginName( &bstrName ) );
			strName = CPeekString( bstrName );
			HeLib::HeFreeString( bstrName );
		}
		return strName;

	}
	CPeekString			GetTitle() const {
		CPeekString	strTitle;
		if ( IsValid() ) {
			Helium::HEBSTR	bstrTitle;
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->GetTitle( &bstrTitle ) );
			strTitle = CPeekString( bstrTitle );
			HeLib::HeFreeString( bstrTitle );
		}
		return strTitle;

	}

	bool	IsNotValid() const { return (m_spPluginAdapterInfo == NULL); }
	bool	IsValid() const { return (m_spPluginAdapterInfo != NULL); }

	void	SetAddress( tEthernetAddress& inAddress ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetAddress( inAddress.bytes ) );
		}
	}
	void	SetChannelManager( IChannelManager* inChannelManager ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetChannelManager( inChannelManager ) );
		}
	}
	void	SetDeletable( bool inDeleteable ) {
		if ( IsValid() ) {
			BOOL	bDeletable( inDeleteable );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetPersistent( bDeletable ) );
		}
	}
	void	SetDescription( const CPeekString& inDescription ) {
		if ( IsValid() ) {
			CHeBSTR	bstrDescription( inDescription );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetDescription( bstrDescription.m_str ) );
		}
	}
	void	SetIdentifier( CGlobalId inId ) {
		if ( IsValid() ) {
			CHeBSTR	bstrId( inId.Format() );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetIdentifier( bstrId.m_str ) );
		}
	}
	void	SetLinkSpeed( UInt64 inLinkSpeed ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetLinkSpeed( inLinkSpeed ) );
		}
	}
	void	SetMediaInfo( CMediaInfo inMediaInfo ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetMediaInfo( inMediaInfo.GetPtr() ) );
		}
	}
	void	SetMediaType( const tMediaType& inMediaType ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetMediaType( inMediaType.fType ) );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetMediaSubType( inMediaType.fSubType ) );
		}
	}
	void	SetPersistent( bool inPersistent ) {
		if ( IsValid() ) {
			BOOL	bPersistent( inPersistent );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetPersistent( bPersistent ) );
		}
	}
	void	SetPluginName( const CPeekString& inName ) {
		if ( IsValid() ) {
			CHeBSTR	bstrName( inName );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetPluginName( bstrName.m_str ) );
		}
	}
	void	SetTitle( const CPeekString& inTitle ) {
		if ( IsValid() ) {
			CHeBSTR	bstrTitle( inTitle );
			Peek::ThrowIfFailed( m_spPluginAdapterInfo->SetTitle( bstrTitle.m_str ) );
		}
	}
};


// =============================================================================
//		CAdapterConfig
// =============================================================================

class CAdapterConfig
{
	friend class CAdapterManager;

protected:
	CHePtr<IAdapterConfig>	m_spAdapterConfig;

protected:
	IAdapterConfig*	GetPtr() { return m_spAdapterConfig; }
	void			ReleasePtr() { m_spAdapterConfig.Release(); }
	void			SetPtr( IAdapterConfig* inAdapterConfig ) {
		m_spAdapterConfig = inAdapterConfig;
	}

public:
	;		CAdapterConfig( IAdapterConfig* inAdapterConfig = NULL )
		: m_spAdapterConfig( NULL )
	{
		SetPtr( inAdapterConfig );
	}
	;		~CAdapterConfig() {}

	void	Add( CGlobalId inId ) {
		if ( IsValid() ) {
			Helium::HeID	id( inId );
			Peek::ThrowIfFailed( m_spAdapterConfig->Add( id ) );
		}
	}

	void	Create() {
		ReleasePtr();
		Peek::ThrowIfFailed( m_spAdapterConfig.CreateInstance( "PeekCore.AdapterConfig" ) );
	}

	UInt32	GetCount() const {
		UInt32	nCount( 0 );
		if ( IsValid() ) {
			SInt32	nSigned( 0 );
			Peek::ThrowIfFailed( m_spAdapterConfig->GetCount( &nSigned ) );
			if ( nSigned > 0 ) {
				nCount = nSigned;
			}
		}
		return nCount;
	}
	UInt64	GetDefaultLinkSpeed() const {
		UInt64	nLinkSpeed( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->GetDefaultLinkSpeed( &nLinkSpeed ) );
		}
		return nLinkSpeed;
	}
	CGlobalId	GetItem( SInt32 inIndex ) const {
		CGlobalId	theId;
		if ( IsValid() ) {
			Helium::HeID	id;
			Peek::ThrowIfFailed( m_spAdapterConfig->GetItem( inIndex, &id ) );
			theId = id;
		}
		return theId;
	}
	UInt64	GetLinkSpeed() const {
		UInt64	nLinkSpeed( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->GetLinkSpeed( &nLinkSpeed ) );
		}
		return nLinkSpeed;
	}
	UInt32	GetRingBufferSize() const {
		UInt32	nBufferSize( 0 );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->GetRingBufferSize( &nBufferSize ) );
		}
		return nBufferSize;
	}

	bool	IsNotValid() const { return (m_spAdapterConfig == NULL); }
	bool	IsValid() const { return (m_spAdapterConfig != NULL); }

	void	Remove( CGlobalId inId ) {
		if ( IsValid() ) {
			Helium::HeID	id( inId );
			Peek::ThrowIfFailed( m_spAdapterConfig->Remove( id ) );
		}
	}
	void	RemoveAll() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->RemoveAll() );
		}
	}

	void	SetDefaultLinkSpeed( UInt64 inLinkSpeed ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->SetDefaultLinkSpeed( inLinkSpeed ) );
		}
	}
	void	SetLinkSpeed( UInt64 inLinkSpeed ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->SetLinkSpeed( inLinkSpeed ) );
		}
	}
	void	SetRingBufferSize( UInt32 inBufferSize ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterConfig->SetRingBufferSize( inBufferSize ) );
		}
	}
};


// =============================================================================
//		CAdapter
// =============================================================================

class CAdapter
{
	friend class CAdapterManager;

protected:
	CHePtr<IAdapter>	m_spAdapter;

protected:
	IAdapter*	GetPtr() { return m_spAdapter; }
	void		ReleasePtr() { m_spAdapter.Release(); }
	void		SetPtr( IAdapter* inAdapter ) { m_spAdapter = inAdapter; }

public:
	;		CAdapter( IAdapter* inAdapter = NULL )
		: m_spAdapter( NULL )
	{
		SetPtr( inAdapter );
	}
	;		~CAdapter() {}

	void	Close() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->Close() );
		}
	}

	CAdapterInfo		GetAdapterInfo() {
		IAdapterInfo* pAdapterInfo( NULL );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetAdapterInfo( &pAdapterInfo ) );
		}
		return CAdapterInfo( pAdapterInfo );
	}
	CEthernetAddress	GetAddress() const {
		tEthernetAddress	theAddress;
		memset( &theAddress, 0, sizeof( theAddress ) );
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetAddress( theAddress.bytes ) );
		}
		return theAddress;
	}
	UInt64			GetLinkSpeed() const {
		UInt64	theLinkSpeed = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetLinkSpeed( &theLinkSpeed ) );
		}
		return theLinkSpeed;
	}
	TMediaType		GetMediaType() const {
		TMediaType	theType = kMediaType_Max;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetMediaType( &theType ) );
		}
		return theType;
	}
	TMediaSubType	GetMediaSubType() const {
		TMediaSubType	theSubType = kMediaSubType_Max;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetMediaSubType( &theSubType ) );
		}
		return theSubType;
	}
	CPeekString		GetName() const {
		CPeekString	strName;
		if ( IsValid() ) {
			Helium::HEBSTR	bstrName;
			Peek::ThrowIfFailed( m_spAdapter->GetName( &bstrName ) );
			strName = CPeekString( bstrName );
			HeLib::HeFreeString( bstrName );
		}
		return strName;
	}
	PeekAdapterType	GetType() const {
		PeekAdapterType	theType = peekNullAdapter;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetAdapterType( &theType ) );
		}
		return theType;
	}

	bool	IsNetSupportDuringCapture() const {
		BOOL	bSupported = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetNetSupportDuringCapture( &bSupported ) );
		}
		return (bSupported != FALSE);
	}
	bool	IsNotValid() const { return (m_spAdapter == NULL); }
	bool	IsValid() const { return (m_spAdapter != NULL); }

	bool	Open() {
		HeResult	hr = HE_E_FAIL;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->Open() );
		}
		return HE_SUCCEEDED( hr );
	}

#if 0
	// Get the adapter hardware options.
	HeResult GetHardwareOptions( /*out*/ Helium::IHeUnknown** ppHardwareOptions ) {
		return m_spAdapter->GetHardwareOptions( ppHardwareOptions );
	}
	// Set the adapter hardware options.
	HeResult SetHardwareOptions( /*in*/ Helium::IHeUnknown* pHardwareOptions ) {
		return m_spAdapter->SetHardwareOptions( pHardwareOptions );
	}
#endif
};


// =============================================================================
//		CAdapterManager
// =============================================================================

class CAdapterManager
{
	friend class CPeekEngineProxy;

protected:
	CHePtr<IAdapterManager>	m_spAdapterManager;

protected:
	IAdapterManager*	GetPtr() { return m_spAdapterManager; }
	void				ReleasePtr() { m_spAdapterManager.Release(); }
	void				SetPtr( IAdapterManager* inAdapterManager ) {
		m_spAdapterManager = inAdapterManager;
	}

public:
	;		CAdapterManager( IAdapterManager* inAdapterManager = NULL )
		: m_spAdapterManager( NULL )
	{
		SetPtr( inAdapterManager );
	}
	;		~CAdapterManager() {}

	void	AddAdapter( CAdapter inAdapter, CAdapterInfo inAdapterInfo,
				CAdapterConfig inAdapterConfig ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapterManager->AddAdapter( inAdapter.GetPtr(),
				inAdapterInfo.GetPtr(), inAdapterConfig.GetPtr() ) );
		}
	}

	CAdapter	GetAdapter( PeekAdapterType inType, CGlobalId inId,
					const CPeekString& inDescription ) {
		CAdapter	theAdapter;
		if ( IsValid() ) {
			CHeBSTR		bstrId( inId.Format() );
			CHeBSTR		bstrDescription( inDescription );
			IAdapter*	pAdapter( NULL );
			Peek::ThrowIfFailed( m_spAdapterManager->GetAdapter(
				inType, bstrId.m_str, bstrDescription.m_str, &pAdapter ) );
			theAdapter = CAdapter( pAdapter );
		}
		return theAdapter;
	}
	UInt32	GetAdapterCount() const {
		UInt32	nCount( 0 );
		if ( IsValid() ) {
			SInt32	nSigned( 0 );
			Peek::ThrowIfFailed( m_spAdapterManager->GetAdapterCount( &nSigned ) );
			if ( nSigned > 0 ) {
				nCount = nSigned;
			}
		}
		return nCount;
	}

	bool	IsNotValid() const { return (m_spAdapterManager == NULL); }
	bool	IsValid() const { return (m_spAdapterManager != NULL); }

#if (0)
	/// Get the adapter info list.
	HE_IMETHOD GetAdapterInfoList(/*out*/ IAdapterInfoList** ppAdapterInfoList) = 0;
	/// Get an adapter.
	/// Get adapter configuration.
	HE_IMETHOD GetAdapterConfig(/*in*/ PeekAdapterType type,
		/*in*/ Helium::HEBSTR bstrIdentifier,
		/*out*/ IAdapterConfig** ppAdapterConfig ) = 0;
	/// Set adapter configuration.
	HE_IMETHOD SetAdapterConfig(/*in*/ PeekAdapterType type,
		/*in*/ Helium::HEBSTR bstrIdentifier,
		/*in*/ IAdapterConfig* pAdapterConfig ) = 0;
	// Add an adapter.
	HE_IMETHOD AddAdapter(/*in*/ IAdapter* pAdapter, /*in*/ IAdapterInfo* pAdapterInfo,
		/*in*/ IAdapterConfig* pAdapterConfig) = 0;
	// Remove an adapter.
	HE_IMETHOD RemoveAdapter(/*in*/ IAdapter* pAdapter) = 0;
	// Refresh the adapter info list.
	HE_IMETHOD RefreshAdapterInfo() = 0;
#endif
};


// =============================================================================
//		CBasePacketBuffer
// =============================================================================

class CBasePacketBuffer
{
public:
	;			CBasePacketBuffer() {}
	;			~CBasePacketBuffer() {}

	void		CreatePacket( CPacket& inPacket ) { inPacket.Create(); }
	IPacket*	GetIPacketPtr( const CPacket& inPacket ) {
		return inPacket.GetIPacketPtr();
	}
};


// =============================================================================
//		CPacketBuffer
// =============================================================================

class CPacketBuffer
	:	public CBasePacketBuffer
{
	friend class CPeekContextProxy;
	friend class CPacket;
	friend class CDynamicPacketBuffer;
	friend class CPersistFile;

protected:
	CHePtr<IPacketBuffer>	m_spPacketBuffer;

protected:
	IPacketBuffer*	GetPtr() const { return m_spPacketBuffer; }
	void			ReleasePtr() { m_spPacketBuffer.Release(); }
	void			SetPtr( IPacketBuffer* inPacketBuffer ) {
		m_spPacketBuffer = inPacketBuffer;
	}

public:
	;		CPacketBuffer( IPacketBuffer* inPacketBuffer = NULL )
		:	m_spPacketBuffer( NULL ) {
		SetPtr( inPacketBuffer );
	}
	;		~CPacketBuffer() {}

	CPacketBuffer&	operator=( const CPersistFile& inPersistFile );

	void	AddRefPacket( UInt32 inIndex ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->AddRefPacket( inIndex ) );
		}
	}
	void	AddPacketConsumer() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->AddPacketConsumer() );
		}
	}

	void	Create() {
		ReleasePtr();
		CHePtr<IPacketBuffer>	spPacketBuffer;
		Peek::ThrowIfFailed( spPacketBuffer.CreateInstance( "PeekCore.MemPacketBuffer" ) );
		SetPtr( spPacketBuffer );
	}

	UInt32		GetCount() const {
		UInt32		thePacketCount = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacketCount( &thePacketCount ) );
		}
		return thePacketCount;
	}
	SInt64		GetDuration() const {
		SInt64	theDuration = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetDuration( &theDuration ) );
		}
		return theDuration;
	}
	UInt32		GetFirstPacketIndex() const {
		UInt32	theFirstPacketIndex = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetFirstPacketIndex( &theFirstPacketIndex ) );
		}
		return theFirstPacketIndex;
	}
	UInt64		GetFirstPacketNumber() const {
		UInt64	theFirstPacketNumber = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetFirstPacketNumber( &theFirstPacketNumber ) );
		}
		return theFirstPacketNumber;
	}
	UInt64		GetLinkSpeed() const {
		UInt64	theLinkSpeed = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetLinkSpeed( &theLinkSpeed ) );
		}
		return theLinkSpeed;
	}
	CMediaInfo	GetMediaInfo() const {
		CHePtr<IMediaInfo>	spMediaInfo;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetMediaInfo( &spMediaInfo.p ) );
		}
		return CMediaInfo( spMediaInfo );
	}
	tMediaType	GetMediaType() const {
		tMediaType	theType;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetMediaType( &theType.fType ) );
			Peek::ThrowIfFailed( m_spPacketBuffer->GetMediaSubType( &theType.fSubType ) );
		}
		return theType;
	}
#if (0)
	TMediaType	GetMediaType() const {
		TMediaType	theType = kMediaType_Max;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetMediaType( &theType ) );
		}
		return theType;
	}
	TMediaSubType	GetMediaSubType() const {
		TMediaSubType	theSubType = kMediaSubType_Max;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetMediaSubType( &theSubType ) );
		}
		return theSubType;
	}
#endif
	CPacket		GetPacket( UInt32 nIndex ) const {
		CPacket thePacket;
		ASSERT( IsValid() );
		if ( IsValid() ) {
			thePacket.Create();
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacket( thePacket.GetIPacketPtr(), nIndex ) );
		}
		return( thePacket );
	}
	UInt32		GetPacketConsumerCount() const {
		UInt32	theConsumers = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacketConsumerCount( &theConsumers ) );
		}
		return theConsumers;
	}
	const UInt8*	GetPacketData( UInt32 inIndex ) const {
		const UInt8*	thePacketData = NULL;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacketData( inIndex, &thePacketData ) );
		}
		return thePacketData;
	}
	const PeekPacket*	GetPacketHeader( UInt32 inIndex ) const {
		const PeekPacket*	theHeader = NULL;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacketHeader( inIndex, &theHeader ) );
		}
		return ( theHeader );
	}
	UInt32		GetPacketIndex( UInt64 inPacketNumber ) const {
		UInt32	theIndex = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->PacketNumberToPacketIndex( inPacketNumber, &theIndex ) );
		}
		return theIndex;
	}
	const void*	GetPacketMetaData( UInt32 inIndex ) const {
		const void*	theMetaData = NULL;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacketMetaData( inIndex, &theMetaData ) );
		}
		return theMetaData;
	}
	UInt64		GetPacketNumber( UInt32 inIndex ) const {
		UInt64	theNumber = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->PacketIndexToPacketNumber( inIndex, &theNumber ) );
		}
		return theNumber;
	}
	UInt64		GetSize() const {
		UInt64	theSizeEx = 0;
		if ( IsValid() ) {
			UInt32		theSize = 0;
			HeResult	hr = m_spPacketBuffer->GetSize( &theSize );
			if ( hr == HE_E_OVERFLOW ) {
				Peek::ThrowIfFailed( m_spPacketBuffer->GetSizeEx( &theSizeEx ) );
			}
			else {
				Peek::ThrowIfFailed( hr );
				theSizeEx = theSize;
			}
		}
		return theSizeEx;
	}
	UInt64		GetStartDateTime() const {
		UInt64	theStartDateTime = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetStartDateTime( &theStartDateTime ) );
		}
		return theStartDateTime;
	}
	UInt64		GetStopDateTime() const {
		UInt64		theStopDateTime = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetStopDateTime( &theStopDateTime ) );
		}
		return theStopDateTime;
	}

	bool		IsNotValid() const { return (m_spPacketBuffer == NULL); }
	bool		IsValid() const { return (m_spPacketBuffer != NULL); }
	bool		IsValidPacketIndex( UInt32 inIndex ) const {
		BOOL	bResult = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->IsValidPacketIndex( inIndex, &bResult ) );
		}
		return (bResult != FALSE);
	}

	void		ReleasePacket( UInt32 inIndex ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->ReleasePacket( inIndex ) );
		}
	}
	void		RemovePacketConsumer() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->RemovePacketConsumer() );
		}
	}
	void		Reset() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->Reset() );
		}
	}

	void		SaveSelected( const CPeekString& inPath, UInt32 inRangeCount, UInt64* inRanges ) const {
		if ( IsValid() ) {
			CHeBSTR	bstrPath = inPath;
			Peek::ThrowIfFailed( m_spPacketBuffer->SaveSelected( bstrPath, inRangeCount, inRanges ) );
		}
	}
	void		SetLinkSpeed( UInt64 nLinkSpeed ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetLinkSpeed( nLinkSpeed ) );
		}
	}
	void		SetMediaInfo( CMediaInfo inMediaInfo ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetMediaInfo( inMediaInfo.GetPtr() ) );
		}
	}
	void		SetMediaType( tMediaType inType ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetMediaType( inType.fType ) );
			Peek::ThrowIfFailed( m_spPacketBuffer->SetMediaSubType( inType.fSubType ) );
		}
	}
	void		SetMediaType( TMediaType inType ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetMediaType( inType ) );
		}
	}
	void		SetMediaSubType( TMediaSubType inSubType ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetMediaSubType( inSubType ) );
		}
	}
	void		SetStartDateTime( UInt64 inStartDateTime ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetStartDateTime( inStartDateTime ) );
		}
	}
	void		SetStopDateTime( UInt64 inStopDateTime ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->SetStopDateTime( inStopDateTime ) );
		}
	}
};


// =============================================================================
//		CDynamicPacketBuffer
// =============================================================================

class CDynamicPacketBuffer
	:	public CBasePacketBuffer
{
protected:
	CHePtr<IDynamicPacketBuffer>	m_spDynamicBuffer;
	CHePtr<IDynamicPacketBuffer2>	m_spDynamicBuffer2;

protected:
	IPacketBuffer*			GetPacketBuffer() const {
		CHePtr<IPacketBuffer>	spPacketBuffer;
		Peek::ThrowIfFailed( m_spDynamicBuffer->QueryInterface( &spPacketBuffer.p ) );
		return spPacketBuffer;
	}
	IDynamicPacketBuffer*	GetPtr() { return m_spDynamicBuffer; }
	void					ReleasePtr() { m_spDynamicBuffer = NULL; }
	void					SetPtr( IDynamicPacketBuffer* inDynamicBuffer ) {
		m_spDynamicBuffer2.Release();
		m_spDynamicBuffer = inDynamicBuffer;
	}
	void					SetPtr( IDynamicPacketBuffer2* inDynamicBuffer ) {
		m_spDynamicBuffer.Release();
		m_spDynamicBuffer2 = inDynamicBuffer;
	}

public:
	;		CDynamicPacketBuffer() {}
	;		~CDynamicPacketBuffer() {}

	CDynamicPacketBuffer&	operator=( const CPacketBuffer& inOther ) {
		CHePtr<IPacketBuffer>	spPacketBuffer = inOther.GetPtr();

		CHePtr<IDynamicPacketBuffer2>	spDynamicBuffer2;
		spPacketBuffer.QueryInterface( &spDynamicBuffer2.p );
		if ( spDynamicBuffer2 ) {
			SetPtr( spDynamicBuffer2 );
		}
		else {
			CHePtr<IDynamicPacketBuffer>	spDynamicBuffer;
			spPacketBuffer.QueryInterface( &spDynamicBuffer.p );
			if ( spDynamicBuffer ) {
				SetPtr( spDynamicBuffer );
			}
		}
		ASSERT( IsValid() );
		return *this;
	}

	UInt64		GetCapacity() const {
		if ( IsValid64() ) {
			UInt64	theCapacity = 0;
			Peek::ThrowIfFailed( m_spDynamicBuffer2->GetCapacity( &theCapacity ) );
			return theCapacity;
		}
		if ( IsValid32() ) {
			UInt32	theCapacity = 0;
			Peek::ThrowIfFailed( m_spDynamicBuffer->GetCapacity( &theCapacity ) );
			return theCapacity;
		}
		return 0;
	}
	UInt64		GetCapacityAvailable() const {
		if ( IsValid64() ) {
			UInt64	theCapacity = 0;
			Peek::ThrowIfFailed( m_spDynamicBuffer2->GetCapacityAvailable( &theCapacity ) );
			return theCapacity;
		}
		if ( IsValid32() ) {
			UInt32	theCapacity = 0;
			Peek::ThrowIfFailed( m_spDynamicBuffer->GetCapacityAvailable( &theCapacity ) );
			return theCapacity;
		}
		return 0;
	}
	UInt64		GetCapacityUsed() const {
		if ( IsValid64() ) {
			UInt64	theCapacity = 0;
			Peek::ThrowIfFailed( m_spDynamicBuffer2->GetCapacityUsed( &theCapacity ) );
			return theCapacity;
		}
		if ( IsValid32() ) {
			UInt32	theCapacity = 0;
			Peek::ThrowIfFailed( m_spDynamicBuffer->GetCapacityUsed( &theCapacity ) );
			return theCapacity;
		}
		return 0;
	}

	bool		IsFull() const {
		BOOL	bResult = FALSE;
		if ( IsValid64() ) {
			Peek::ThrowIfFailed( m_spDynamicBuffer2->IsFull( &bResult ) );
		}
		else if ( IsValid32() ) {
			Peek::ThrowIfFailed( m_spDynamicBuffer->IsFull( &bResult ) );
		}
		return (bResult != FALSE);
	}
	bool		IsNotValid() const { return (!IsValid64() && !IsValid32()); }
	bool		IsValid() const { return (IsValid64() || IsValid32()); }
	bool		IsValid32() const { return (m_spDynamicBuffer != NULL); }
	bool		IsValid64() const { return (m_spDynamicBuffer2 != NULL); }

	void		PutPacket( const CPacket& inPacket ) {
		if ( IsValid64() ) {
			UInt16	nDataSize = inPacket.GetPacketDataLength();
			Peek::ThrowIfFailed( m_spDynamicBuffer2->PutPacket( GetIPacketPtr( inPacket ), nDataSize ) );
		}
		else if ( IsValid32() ) {
			UInt16	nDataSize = inPacket.GetPacketDataLength();
			Peek::ThrowIfFailed( m_spDynamicBuffer->PutPacket( GetIPacketPtr( inPacket ), nDataSize ) );
		}
	}

	void		SetCapacity( UInt64 inCapacity ) {
		if ( IsValid64() ) {
			Peek::ThrowIfFailed( m_spDynamicBuffer2->SetCapacity( inCapacity ) );
		}
		else if ( IsValid32() ) {
			if ( inCapacity > 0x100000000 ) throw -1;
			UInt32	nCapacity = static_cast<UInt32>( inCapacity );
			Peek::ThrowIfFailed( m_spDynamicBuffer->SetCapacity( nCapacity ) );
		}
	}

#if 0
	// Set the packet buffer events receiver callback interface.
	HeResult	SetEventsReceiver( IPacketBufferEvents* pEventsReceiver ) {
		return m_spDynamicBuffer->SetEventsReceiver( pEventsReceiver );
	}
#endif
};


// =============================================================================
//		CPeekCapture
// =============================================================================

class CPeekCapture
{
	friend class CPeekSetup;
	friend class CPeekCaptures;
	friend class CPeekContextProxy;

protected:
	int						m_nType;
	CHePtr<IPeekCapture>	m_spPeekCapture;

protected:
	IPeekCapture*	GetPtr() { return m_spPeekCapture; }
	void			ReleasePtr() { m_spPeekCapture.Release(); }
	void			SetPtr( IPeekCapture* inPeekCapture ) {
		m_spPeekCapture = inPeekCapture;
	}

public:
	;		CPeekCapture( IPeekCapture* inOther = NULL )
		: m_nType( kContextType_Unknown )
		, m_spPeekCapture( inOther ) {}
	;		~CPeekCapture() {}

	void			Clear() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->Clear() );
		}
	}
	CAdapter		GetAdapter() const {
		CHePtr<IAdapter>	spAdapter;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetAdapter( &spAdapter.p ) );
		}
		return CAdapter( spAdapter );
	}
	PeekCaptureStatus	GetCaptureStatus() const {
		PeekCaptureStatus	theCaptureStatus;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetCaptureStatus( &theCaptureStatus ) );
		}
		return theCaptureStatus;
	}
	SInt64			GetDuration() const {
		SInt64	theDuration = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetDuration( &theDuration ) );
		}
		return theDuration;
	}
	CDynamicPacketBuffer	GetDynamicPacketBuffer() const {
		CDynamicPacketBuffer	theDynamicPacketBuffer;
		theDynamicPacketBuffer = GetPacketBuffer();
		return theDynamicPacketBuffer;
	}
	PeekFilterMode	GetFilterMode() const {
		PeekFilterMode	theFilterMode = peekFilterModeAcceptAll;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetFilterMode( &theFilterMode ) );
		}
		return theFilterMode;
	}
	CGlobalId		GetId() const {
		CGlobalId	theId;
		if ( IsValid() ) {
			Helium::HeID	heId;
			Peek::ThrowIfFailed( m_spPeekCapture->GetID( &heId ) );
			theId = heId;
		}
		return theId;
	}
	CPeekString		GetName() const {
		CPeekString	theName;
		if ( IsValid() ) {
			Helium::HEBSTR	bstrName;
			Peek::ThrowIfFailed( m_spPeekCapture->GetName( &bstrName ) );
			theName = CPeekString( bstrName );
			HeLib::HeFreeString( bstrName );
		}
		return theName;
	}
	UInt32			GetOptionsResetCount() const {
		UInt32	theResetCount = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetOptionsResetCount( &theResetCount ) );
		}
		return theResetCount;
	}
	CPacketBuffer	GetPacketBuffer() const {
		CHePtr<IPacketBuffer>	spPacketBuffer;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetPacketBuffer( &spPacketBuffer.p ) );
		}
		return CPacketBuffer( spPacketBuffer );
	}
	CPacketHandler	GetPacketHandler() const;
	UInt64			GetPacketsDropped() const {
		UInt64	thePacketsDropped = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetPacketsDropped( &thePacketsDropped ) );
		}
		return thePacketsDropped;
	}
	UInt64			GetPacketsFiltered() const {
		UInt64	thePacketsFiltered = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetPacketsFiltered( &thePacketsFiltered ) );
		}
		return thePacketsFiltered;
	}
	UInt64			GetPacketsReceived() const {
		UInt64	thePacketsReceived = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetPacketsReceived( &thePacketsReceived ) );
		}
		return thePacketsReceived;
	}
	UInt32			GetResetCount() const {
		UInt32	theResetCount = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetResetCount( &theResetCount ) );
		}
		return theResetCount;
	}
	UInt64			GetStartDateTime() const {
		UInt64	theStartDateTime = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetStartDateTime( &theStartDateTime ) );
		}
		return theStartDateTime;
	}
	UInt64			GetStopDateTime() const {
		UInt64	theStopDateTime = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetStopDateTime( &theStopDateTime ) );
		}
		return theStopDateTime;
	}
	UInt32			GetTriggerCount() const {
		UInt32	theTriggerCount = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetTriggerCount( &theTriggerCount ) );
		}
		return theTriggerCount;
	}
	SInt64			GetTriggerDuration() const {
		SInt64	theTriggerDuration = 0;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->GetTriggerDuration( &theTriggerDuration ) );
		}
		return theTriggerDuration;
	}

	bool			IsCapturing() const {
		BOOL	bCapturing = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->IsCapturing( &bCapturing ) );
		}
		return (bCapturing != FALSE);
	}
	bool			IsNotValid() const { return (m_spPeekCapture == NULL); }
	bool		IsValid() const { return (m_spPeekCapture != NULL); }

	void		StartCapture( BOOL bFullReset ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->StartCapture( bFullReset ) );
		}
	}
	void			SetFilterMode( PeekFilterMode inMode ) {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->SetFilterMode( inMode ) );
		}
	}
	void			SetName( const CPeekString& inName ) {
		if ( IsValid() ) {
			CHeBSTR	bstrName = inName;
			Peek::ThrowIfFailed( m_spPeekCapture->SetName( bstrName ) );
		}
	}
	void			StopCapture() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPeekCapture->StopCapture() );
		}
	}

#if 0
	// Get the packet filter.
	HeResult GetPacketFilter( /*out*/ IFilterPacket** ppPacketFilter ) {
		return m_spCapture->GetPacketFilter( ppPacketFilter );
	}
	// Set the packet filter.
	HeResult SetPacketFilter( /*out*/ IFilterPacket* pPacketFilter ) {
		return m_spCapture->SetPacketFilter( pPacketFilter );
	}
	// Get the packet processor.
	HeResult GetPacketProcessor( /*out*/ IPacketProcessor** ppPacketProcessor ) {
		return m_spCapture->GetPacketProcessor( ppPacketProcessor );
	}
	// Get the start trigger.
	HeResult GetStartTrigger( /*out*/ ITrigger** ppTrigger ) {
		return m_spCapture->GetStartTrigger( ppTrigger );
	}
	// Set the start trigger.
	HeResult SetStartTrigger( /*out*/ ITrigger* pTrigger ) {
		return m_spCapture->SetStartTrigger( pTrigger );
	}
	// Get the stop trigger.
	HeResult GetStopTrigger( /*out*/ ITrigger** ppTrigger ) {
		return m_spCapture->GetStopTrigger( ppTrigger );
	}
	// Set the stop trigger.
	HeResult SetStopTrigger( /*out*/ ITrigger* pTrigger ) {
		return m_spCapture->SetStopTrigger( pTrigger );
	}
#endif
};


// =============================================================================
//		CGraphsContext
// =============================================================================

class CGraphsContext
{
	friend class CPeekContextProxy;

protected:
	CHePtr<IGraphsContext>	m_spGraphsContext;

protected:
	IGraphsContext*	GetPtr() { return m_spGraphsContext; }
	void			ReleasePtr() { m_spGraphsContext.Release(); }
	void			SetPtr( IGraphsContext* inGraphsContext ) {
		m_spGraphsContext = inGraphsContext;
	}

public:
	;		CGraphsContext( IGraphsContext* inContext = NULL )
		: m_spGraphsContext( inContext )
	{
		SetPtr( inContext );
	}
	;		~CGraphsContext() {}

	bool	IsNotValid() const { return (m_spGraphsContext == NULL); }
	bool	IsValid() const { return (m_spGraphsContext != NULL); }

	//HE_IMETHOD GetGraphs(/*out*/ IGraphCollection** pVal) = 0;
	//HE_IMETHOD AddGraph(/*in*/ ISimpleGraph *spGraph, /*out*/ Helium::HEVARBOOL* pResult ) = 0;
	//HE_IMETHOD DeleteGraphByID(/*in*/ Helium::HeID grphId, /*out*/ Helium::HEVARBOOL* pResult) = 0;
	//HE_IMETHOD GetStatsContext(/*in*/ UInt64* StartTime, /*in*/ UInt64* EndTime,
	//	/*out*/ IPeekStatsContextCollection** StatsCollection) = 0;
};


// =============================================================================
//		CFileContext
// =============================================================================

class CFileContext
{
	friend class CPeekContextProxy;

protected:
	CHePtr<IFileView>					m_spFileView;
	CHePtr<Helium::IHeServiceProvider>	m_spServiceProvider;

protected:
	IFileView*	GetPtr() { return m_spFileView; }
	void		ReleasePtr() { m_spFileView.Release(); }
	void		SetPtr( IFileView* inFileView ) {
		m_spFileView = inFileView;
	}
	void		SetPtrOther( Helium::IHeServiceProvider* inP ) {
		m_spServiceProvider = inP;
	}

public:
	;		CFileContext( IFileView* inFileView = NULL )
		: m_spFileView( NULL )
	{
		SetPtr( inFileView );
	}
	;		~CFileContext() {}

	bool	IsNotValid() const { return (m_spFileView == NULL); }
	bool	IsValid() const { return (m_spFileView != NULL); }

	///// Get the ID.
	//HE_IMETHOD GetID(/*out*/ Helium::HeID* pID) = 0;
	///// Get the packet buffer.
	//HE_IMETHOD GetPacketBuffer(/*out*/ IPacketBuffer** ppPacketBuffer) = 0;
	///// Open the file view.
	///// \return PEEK_E_FILEVIEW_NO_DATA No data was found
	//HE_IMETHOD Open(/*in*/ Helium::IHeUnknown* pQuery) = 0;
	///// Close the file view.
	//HE_IMETHOD Close() = 0;
	///// Stop loading files into the view.
	//HE_IMETHOD StopLoad() = 0;
};


// =============================================================================
//		CPacketHandler
// =============================================================================

class CPacketHandler
{
protected:
	CHePtr<IPacketHandler>	m_spHandler;

protected:
	IPacketHandler*	GetPtr() { return m_spHandler; }
	void			ReleasePtr() { m_spHandler.Release(); }
	void			SetPtr( IPacketHandler* inHandler ) { m_spHandler = inHandler; }

public:
	;		CPacketHandler( IPacketHandler* inHandler = NULL )
		: m_spHandler( NULL )
	{
		SetPtr( inHandler );
	}
	;		~CPacketHandler() {}

	bool	operator==( const CPacketHandler& inOther ) {
		return (inOther.m_spHandler.p == m_spHandler.p);
	}

	CPeekCapture	GetPeekCapture() const {
		CHePtr<IPeekCapture> spPeekCapture;
		Peek::ThrowIfFailed( m_spHandler->QueryInterface( &spPeekCapture.p ) );
		return CPeekCapture( spPeekCapture );
	}

	bool	HandlePacket( const CPacket& inPacket ) const {
		if ( IsNotValid() ) return false;

		HeResult	hr = m_spHandler->HandlePacket( inPacket.GetIPacketPtr() );
		return HE_SUCCEEDED( hr );
	}

	bool	IsNotValid() const { return (m_spHandler == NULL); }
	bool	IsValid() const { return (m_spHandler != NULL); }
};

// =============================================================================
//		CPacketHandlerIndex
// =============================================================================

typedef std::vector<CPacketHandler>::const_iterator	CPacketHandlerIndex;


// =============================================================================
//		CPacketHandlerArray
// =============================================================================

class CPacketHandlerArray
	:	public std::vector< CPacketHandler >
{
public:
	;		CPacketHandlerArray() {}
	;		~CPacketHandlerArray() {}

	void	Add( CPacketHandler inItem ) { push_back( inItem ); }
	void	AddUnique( CPacketHandler inItem ) {
		CPacketHandlerIndex	item;
		if ( !Find( inItem, item ) ) {
			push_back( inItem );
		}
	}

	bool	Find( CPacketHandler inItem, CPacketHandlerIndex& outIndex ) {
		CPacketHandlerIndex	i = find( begin(), end(), inItem );
		if ( i == end() ) return false;
		outIndex = i;
		return true;
	}

	bool	Remove( CPacketHandler inItem ) {
		CPacketHandlerIndex	i = find( begin(), end(), inItem );
		if ( i == end() ) return false;
		erase( i );
		return true;
	}
};


// =============================================================================
//		CPeekContextProxy
// =============================================================================

class CPeekContextProxy
{
	friend class CPeekContext;

protected:
	IHeUnknown*				m_pUnkSite;
	TMediaType				m_nMediaType;
	TMediaSubType			m_nMediaSubType;
	CPeekString				m_strCaptureName;
	CPeekCapture			m_PeekCapture;
	CFileContext			m_FileContext;
	CGraphsContext			m_GraphsContext;
	CAdapter				m_Adapter;
	CPacketBuffer			m_PacketBuffer;
	CDynamicPacketBuffer	m_DynamicPacketBuffer;
	CPacketHandler			m_PacketHandler;
#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
	CProxySummaryStats		m_ProxySummaryStats;
#endif

	void			SetInterfacePointers( IHeUnknown* inUnkSite, bool& outHasGraphs );

public:
	;		CPeekContextProxy() : m_pUnkSite( NULL ) {}
	virtual	~CPeekContextProxy() {}

	CAdapter		GetAdapter() { return m_Adapter; }
	CPeekCapture	GetCapture() { return m_PeekCapture; }
	const CPeekString&		GetCaptureName() const { return m_strCaptureName; }
	CDynamicPacketBuffer	GetDynamicPacketBuffer() { return m_DynamicPacketBuffer; }
	TMediaSubType	GetMediaSubType() const { return m_nMediaSubType; }
	TMediaType		GetMediaType() const { return m_nMediaType; }
	CPacketBuffer	GetPacketBuffer() { return m_PacketBuffer; }
	CPacketHandler	GetPacketHandler() { return m_PacketHandler; }

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
	int				SummaryModifyEntry( const CPeekString& inLabel,
						const CPeekString& inGroup, UInt32 inType, void* inData );
#endif

#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
	HE_IMETHOD		UpdateSummaryStats( IHeUnknown* inSnapshot ) {
		return m_ProxySummaryStats.UpdateSummaryStats( inSnapshot );
	}
#endif
};


// =============================================================================
//		CPersistFile
// =============================================================================

class CPersistFile
{
	friend class CPacketBuffer;

protected:
	CHePtr<Helium::IHePersistFile>	m_spPersistFile;

protected:
	Helium::IHePersistFile*	GetPtr() const { return m_spPersistFile; }

	void			ReleasePtr() { m_spPersistFile.Release(); }

	void			SetPtr( Helium::IHePersistFile* inPersistFile ) {
		m_spPersistFile = inPersistFile;
	}

public:
	;		CPersistFile( Helium::IHePersistFile* inPersistFile = NULL )
		: m_spPersistFile( NULL )
	{
		SetPtr( inPersistFile );
	}
	;		~CPersistFile() {}

	CPersistFile&	operator=( const CPacketBuffer& inPacketBuffer ) {
		CHePtr<IPacketBuffer>	spPacketBuffer = inPacketBuffer.GetPtr();
		CHePtr<Helium::IHePersistFile>	spPersistFile;
		Peek::ThrowIfFailed( spPacketBuffer.QueryInterface( &spPersistFile.p ) );
		SetPtr( spPersistFile );
		return *this;
	}

	void			Create() {
		m_spPersistFile.CreateInstance( "PeekCore.FilePacketBuffer" );
	}

	CPeekString		GetFileName() const {
		CPeekString strFileName;
		if ( IsValid() ) {
			HeResult	hr = HE_S_OK;
			wchar_t*	pszFilePath = NULL;
			hr = m_spPersistFile->GetCurFile( &pszFilePath );
			if ( SUCCEEDED( hr ) ) {
				strFileName = CPeekString( pszFilePath );
				HeLib::HeTaskMemFree( pszFilePath );
			}
		}
		return strFileName;
	}

	bool	IsDirty() const {
		HeResult	hr = HE_S_OK;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( (hr = m_spPersistFile->IsDirty()) );
		}
		return HE_SUCCEEDED( hr );
	}
	bool	IsNotValid() const { return (m_spPersistFile == NULL); }
	bool	IsValid() const { return (m_spPersistFile != NULL); }

	// Load a file into the buffer
	bool	LoadFromFile( const CPeekString& inFilePath ) {
		HeResult		hr = HE_E_FAIL;
		const UInt32	nMode = 0;  // Currently mode has no effect
		if ( IsValid() ) {
			hr = m_spPersistFile->Load( inFilePath, nMode );
		}
		return HE_SUCCEEDED( hr );
	}

	bool	Save( const CPeekString& inFilePath ) const {
		HeResult hr = HE_E_FAIL;
		if ( IsValid() ) {
			hr = m_spPersistFile->Save( inFilePath, TRUE );
		}
		return HE_SUCCEEDED( hr );
	}
	// No current implementation
	//	bool	SaveCompletedToFile( const CPeekString& inFilePath ) const {
	//		HeResult hr = HE_E_FAIL;
	//		if ( IsValid() ) {
	//			hr = m_spPersistFile->SaveCompleted( inFilePath );
	//		}
	//		return HE_SUCCEEDED( hr );
	//	}
};
