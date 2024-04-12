// =============================================================================
//	PeekContextProxy.cpp
// =============================================================================
//	Copyright (c) 2009-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekContextProxy.h"
#include "PeekProxy.h"
#include "RemotePlugin.h"
#include "hecom.h"


// =============================================================================
//		CStream
// =============================================================================

// -----------------------------------------------------------------------------
//		Read
// -----------------------------------------------------------------------------

bool
CStream::Read(
	void* outData,
	UInt32 inCount,
	UInt32* outCount )
{
	if ( m_spStream.get() == nullptr ) return false;
	return HE_SUCCEEDED( m_spStream->Read( outData, inCount, outCount ) );
}


// -----------------------------------------------------------------------------
//		Seek
// -----------------------------------------------------------------------------

UInt64
CStream::Seek(
	SInt64 inOffset,
	UInt32 inOrigin )
{
	UInt64	nNewPos( 0 );
	if ( (m_spStream.get() == nullptr) || HE_FAILED( m_spStream->Seek( inOffset, inOrigin, &nNewPos ) ) ) {
		return 0;
	}

	return nNewPos;
}


// -----------------------------------------------------------------------------
//		SetSize
// -----------------------------------------------------------------------------

bool
CStream::SetSize(
	UInt64 inNewSize )
{
	if ( m_spStream.get() == nullptr ) return false;
	return HE_SUCCEEDED( m_spStream->SetSize( inNewSize ) );
}


// -----------------------------------------------------------------------------
//		Write
// -----------------------------------------------------------------------------

bool
CStream::Write(
	const void* inData,
	UInt32 inCount,
	UInt32* outCount )
{
	if ( m_spStream.get() == nullptr ) return false;
	return HE_SUCCEEDED( m_spStream->Write( inData, inCount, outCount ) );;
}


// =============================================================================
//		CAdapter
// =============================================================================

// -----------------------------------------------------------------------------
//		Create
// -----------------------------------------------------------------------------

void
CAdapter::Create(
	CRemotePlugin*		inRemotePlugin,
	const CGlobalId&	inId,
	const CPeekString&	inName )
{
	CHePtr<IAdapter> spAdapter;
	Peek::ThrowIfFailed(
		HeLib::CHeCreator< HeLib::CHeObject<CRemoteAdapter> >::CreateInstance(
			NULL, IAdapter::GetIID(),
			reinterpret_cast<void**>( &spAdapter.p ) ) );

 	HeLib::CHeQIPtr<Helium::IHeObjectWithSite>	spObject( spAdapter );
 	if ( spObject != NULL ) {
		Helium::IHeUnknown*	pSite;
		inRemotePlugin->GetSite( IHeUnknown::GetIID(), (void**)&pSite );
 		spObject->SetSite( pSite );
 	}

	// Set the IAdapter Ptr.
	spAdapter.CopyTo( &m_spAdapter.p );

	Helium::HeCID	idPlugin;
	inRemotePlugin->GetClassID( &idPlugin );

	CPluginAdapterInfo	info( GetPluginAdapterInfo() );
	info.SetDescription( inName );
	//info.SetTitle( inName );
	info.SetPluginId( CGlobalId( idPlugin ) );

	if ( inId.IsNotNull() ) {
		info.SetIdentifier( inId );
	}
}


// =============================================================================
//		CPacketBuffer
// =============================================================================

CPacketBuffer&
CPacketBuffer::operator=(
	CPersistFile& inPersistFile )
{
	CHeQIPtr<IPacketBuffer>	spPacketBuffer( inPersistFile.GetPtr() );
	SetPtr( spPacketBuffer );
	return *this;
}


// =============================================================================
//		CPeekCapture
// =============================================================================

// -----------------------------------------------------------------------------
//		GetPacketHandler
// -----------------------------------------------------------------------------

CPacketHandler
CPeekCapture::GetPacketHandler() const {
	CHePtr<IPacketHandler> spPacketHandler;
	Peek::ThrowIfFailed( m_spPeekCapture->QueryInterface( &spPacketHandler.p ) );
	return CPacketHandler( spPacketHandler );
}


// =============================================================================
//		CPeekContextProxy
// =============================================================================

// -----------------------------------------------------------------------------
//		SetInterfacePointers
// -----------------------------------------------------------------------------

void
CPeekContextProxy::SetInterfacePointers(
	IHeUnknown*	inUnkSite,
	bool&		outHasGraphs )
{
	outHasGraphs = false;
	if ( inUnkSite == nullptr ) return;

	m_pUnkSite = inUnkSite;

	HeResult					hr = HE_S_OK;
	CHePtr<IPeekSetup>			spPeekSetup;
	CHePtr<IPeekCapture>		spPeekCapture;
	CHePtr<IGraphsContext>		spGraphsContext;
	CHePtr<IFileView>			spFileContext;
	CHePtr<Helium::IHeServiceProvider>	spServiceProvider;

	try {
		hr = inUnkSite->QueryInterface( &spPeekCapture.p );
		if ( HE_SUCCEEDED( hr ) ) {
			m_PeekCapture.SetPtr( spPeekCapture );
			m_strCaptureName = m_PeekCapture.GetName();
			m_Adapter = m_PeekCapture.GetAdapter();
			m_nMediaType = m_Adapter.GetMediaType();
			m_nMediaSubType = m_Adapter.GetMediaSubType();
			m_PacketBuffer = m_PeekCapture.GetPacketBuffer();
			m_DynamicPacketBuffer = m_PeekCapture.GetDynamicPacketBuffer();
			m_PacketHandler = m_PeekCapture.GetPacketHandler();
		}

		hr = inUnkSite->QueryInterface( &spServiceProvider.p );
		if ( HE_SUCCEEDED( hr ) ) {
			hr = spServiceProvider->QueryService( IGraphsContext::GetIID(), &spGraphsContext.p );
			if ( HE_SUCCEEDED( hr ) ) {
				m_GraphsContext.SetPtr( spGraphsContext );
				outHasGraphs = true;
			}
		}

		hr = inUnkSite->QueryInterface( &spFileContext.p );
		if ( HE_SUCCEEDED( hr ) ) {
			m_FileContext.SetPtr( spFileContext );
		}
		else {
			spServiceProvider.Release();
			hr = inUnkSite->QueryInterface( &spServiceProvider.p );
			if ( HE_SUCCEEDED( hr ) ) {
				m_FileContext.SetPtrOther( spServiceProvider );

				CHePtr<IHeUnknown>	spUnk;
				spServiceProvider->QueryService( IHeUnknown::GetIID(), &spUnk.p );
			}
		}

		//_ASSERTE( HE_SUCCEEDED( hr ) );
		//if ( HE_FAILED( hr ) ) HeThrow( hr );
	}
	HE_CATCH(hr);
}


#ifdef IMPLEMENT_SUMMARYMODIFYENTRY
// ----------------------------------------------------------------------------
//		DoSummaryModifyEntry
// ----------------------------------------------------------------------------

int
CPeekContextProxy::SummaryModifyEntry(
	const CPeekString&	inLabel,
	const CPeekString&	inGroup,
	UInt32				inType,
	void*				inData )
{
	_ASSERTE( CPeekProxy::IsEngine() );
	static CPeekString	strNone( L"-none-" );

	m_ProxySummaryStats.ModifyEntry(
		inLabel, (inGroup.IsEmpty() ? strNone : inGroup), inType, inData );
	return HE_S_OK;
}
#endif // IMPLEMENT_SUMMARYMODIFYENTRY
