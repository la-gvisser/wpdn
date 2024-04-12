// =============================================================================
//	PeekContextProxy.h
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#pragma once

#include "WPTypes.h"
#include "peekcore.h"
#include "omnigraphs.h"
#include "Peek.h"
#include "Packet.h"
#include "PeekStrings.h"
#include "ProxySummaryStats.h"
#include "hestdint.h"


class CPersistFile;

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
	;			CStream( Helium::IHeStream* inStream = nullptr ) : m_spStream( nullptr ) { SetPtr( inStream ); }
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
//		CAdapter
// =============================================================================

class CAdapter
{
protected:
	CHePtr<IAdapter>	m_spAdapter;

protected:
	IAdapter*	GetPtr() { return m_spAdapter; }
	void		ReleasePtr() { m_spAdapter.Release(); }
	void		SetPtr( IAdapter* inAdapter ) { m_spAdapter = inAdapter; }

public:
	;		CAdapter( IAdapter* inAdapter = nullptr ) : m_spAdapter( nullptr ) { SetPtr( inAdapter ); }
	;		~CAdapter() {}

	void	Close() {
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->Close() );
		}
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

	bool			IsNetSupportDuringCapture() const {
		BOOL	bSupported = FALSE;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->GetNetSupportDuringCapture( &bSupported ) );
		}
		return (bSupported != FALSE);
	}
	bool			IsNotValid() const { return (m_spAdapter == nullptr); }
	bool			IsValid() const { return (m_spAdapter != nullptr); }

	bool			Open() {
		HeResult	hr = HE_E_FAIL;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spAdapter->Open() );
		}
		return HE_SUCCEEDED( hr );
	}

#if (0)
	// Get the adapter info.
	HeResult GetInfo( /*out*/ IAdapterInfo** ppAdapterInfo ) {
		return m_spAdapter->GetAdapterInfo( ppAdapterInfo );
	}
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
//		CBasePacketBuffer
// =============================================================================

class CBasePacketBuffer
{
public:
	;			CBasePacketBuffer() {}
	;			~CBasePacketBuffer() {}

	void		CreatePacket( CPacket& inPacket ) { inPacket.Create(); }
	IPacket*	GetIPacketPtr( const CPacket& inPacket ) { return inPacket.GetIPacketPtr(); }
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
	void			SetPtr( IPacketBuffer* inPacketBuffer ) { m_spPacketBuffer = inPacketBuffer; }

public:
	;		CPacketBuffer( IPacketBuffer* inPacketBuffer = nullptr )
		:	m_spPacketBuffer( nullptr ) {
		SetPtr( inPacketBuffer );
	}
	;		~CPacketBuffer() {}

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
	CPacket		GetPacket( UInt32 nIndex ) const {
		CPacket thePacket;
		_ASSERTE( IsValid() );
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
		const UInt8*	thePacketData = nullptr;
		if ( IsValid() ) {
			Peek::ThrowIfFailed( m_spPacketBuffer->GetPacketData( inIndex, &thePacketData ) );
		}
		return thePacketData;
	}
	const PeekPacket*	GetPacketHeader( UInt32 inIndex ) const {
		const PeekPacket*	theHeader = nullptr;
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
		const void*	theMetaData = nullptr;
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

	bool		IsNotValid() const { return (m_spPacketBuffer == nullptr); }
	bool		IsValid() const { return (m_spPacketBuffer != nullptr); }
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

#if 0
	// Get the media info.
	bool GetMediaInfo( IMediaInfo*& outMediaInfo ) {
		Peek::ThrowIfFailed( m_spPacketBuffer->GetMediaInfo( &outMediaInfo ) );
		return( S_OK );
	}
	// Set the media info.
	void SetMediaInfo( IMediaInfo* inMediaInfo ) {
		Peek::ThrowIfFailed( m_spPacketBuffer->SetMediaInfo( inMediaInfo ) );
	}
#endif
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
	void					ReleasePtr() { m_spDynamicBuffer = nullptr; }
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
		_ASSERTE( IsValid() );
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
	bool		IsValid32() const { return (m_spDynamicBuffer != nullptr); }
	bool		IsValid64() const { return (m_spDynamicBuffer2 != nullptr); }

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

#if (0)
	// Set the packet buffer events receiver callback interface.
	HeResult	SetEventsReceiver( IPacketBufferEvents* pEventsReceiver ) {
		return m_spDynamicBuffer->SetEventsReceiver( pEventsReceiver );
	}
#endif
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
	;		CPacketHandler( IPacketHandler* inHandler = nullptr )
		: m_spHandler( nullptr )
	{
		SetPtr( inHandler );
	}
	;		~CPacketHandler() {}

	bool	HandlePacket( const CPacket& inPacket ) {
		if ( IsNotValid() ) return false;

		HeResult	hr = m_spHandler->HandlePacket( inPacket.GetIPacketPtr() );
		return HE_SUCCEEDED( hr );
	}

	bool	IsNotValid() const { return (m_spHandler == nullptr); }
	bool	IsValid() const { return (m_spHandler != nullptr); }
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
	;		CPeekCapture( IPeekCapture* inOther = nullptr )
		:	m_nType( kContextType_Unknown ), m_spPeekCapture( inOther ) {}
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
		PeekCaptureStatus	theCaptureStatus = peekCaptureStatusIdle;
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
	CPacketHandler	GetPacketHandler() const {
		CHePtr<IPacketHandler> spPacketHandler;
		Peek::ThrowIfFailed( m_spPeekCapture->QueryInterface( &spPacketHandler.p ) );
		return CPacketHandler( spPacketHandler );
	}
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
	bool			IsNotValid() const { return (m_spPeekCapture == nullptr); }
	bool		IsValid() const { return (m_spPeekCapture != nullptr); }

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

#if (0)
	// Get the packet filter.
	HeResult GetPacketFilter( /*out*/ IFilterPacket** ppPacketFilter ) {
		return m_spCapture->GetPacketFilter( ppPacketFilter );
	}
	// Set the packet filter.
	HeResult SetPacketFilter( /*out*/ IFilterPacket* pPacketFilter ) {
		return m_spCapture->SetPacketFilter( pPacketFilter );
	}
	// Get the capture ID.
	HeResult GetID( /*out*/ Helium::HeID* pID ) {
		return m_spCapture->GetID( pID );
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
	;		CGraphsContext( IGraphsContext* inOther ) : m_spGraphsContext( inOther ) {}
	;		CGraphsContext() : m_spGraphsContext( nullptr ) {}
	;		~CGraphsContext() {}

	bool	IsNotValid() const { return (m_spGraphsContext == nullptr); }
	bool	IsValid() const { return (m_spGraphsContext != nullptr); }

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
	;		CFileContext( IFileView* inFileView = nullptr ) : m_spFileView( inFileView ) {}
	;		~CFileContext() {}

	bool	IsNotValid() const { return (m_spFileView == nullptr); }
	bool	IsValid() const { return (m_spFileView != nullptr); }

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
	CProxySummaryStats		m_ProxySummaryStats;

	void			SetInterfacePointers( IHeUnknown* inUnkSite, bool& outHasGraphs );

public:
	;		CPeekContextProxy() : m_pUnkSite( nullptr ) {}
	virtual	~CPeekContextProxy() {}

	CAdapter		GetAdapter() { return m_Adapter; }
	CPeekCapture	GetCapture() { return m_PeekCapture; }
	const CPeekString&		GetCaptureName() const { return m_strCaptureName; }
	CDynamicPacketBuffer	GetDynamicPacketBuffer() { return m_DynamicPacketBuffer; }
	TMediaSubType	GetMediaSubType() const { return m_nMediaSubType; }
	TMediaType		GetMediaType() const { return m_nMediaType; }
	CPacketBuffer	GetPacketBuffer() { return m_PacketBuffer; }
	CPacketHandler	GetPacketHandler() { return m_PacketHandler; }

	int				SummaryModifyEntry( const CPeekString& inLabel, const CPeekString& inGroup, UInt32 inType, void* inData );

	HE_IMETHOD		UpdateSummaryStats( IHeUnknown* inSnapshot ) {
		return m_ProxySummaryStats.UpdateSummaryStats( inSnapshot );
	}
};


// =============================================================================
//		CPersistFile
// =============================================================================

class CPersistFile
{
protected:
	CHePtr<Helium::IHePersistFile>	m_spPersistFile;

protected:
	Helium::IHePersistFile*	GetPtr() { return m_spPersistFile; }

	void			ReleasePtr() { m_spPersistFile.Release(); }

	void			SetPtr( Helium::IHePersistFile* inPersistFile ) { m_spPersistFile = inPersistFile; }

public:
	;		CPersistFile( Helium::IHePersistFile* inPersistFile = nullptr )
		: m_spPersistFile( nullptr )
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

	CPeekString		GetFileName() const {
		CPeekString strFileName;
		if ( IsValid() ) {
			HeResult	hr = HE_S_OK;
			wchar_t*	pszFilePath = nullptr;
			hr = m_spPersistFile->GetCurFile( &pszFilePath );
			if ( HE_SUCCEEDED( hr ) ) {
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
	bool	IsNotValid() const { return (m_spPersistFile == nullptr); }
	bool	IsValid() const { return (m_spPersistFile != nullptr); }

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
