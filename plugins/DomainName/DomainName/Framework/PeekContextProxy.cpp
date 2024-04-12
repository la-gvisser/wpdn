// =============================================================================
//	PeekContextProxy.cpp
// =============================================================================
//	Copyright (c) 2009-2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "PeekContextProxy.h"
#include "PeekProxy.h"


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
	if ( inUnkSite == NULL ) return;

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
