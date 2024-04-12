// ============================================================================
//	PortOptions.cpp
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#include "StdAfx.h"
#include "PortOptions.h"
#include "GlobalConstants.h"


// =============================================================================
//		CPortOptions
// =============================================================================

// Options modeling labels
const PCWSTR	kUseDefaultPorts( L"UseDefaultPorts" );
const PCWSTR	kPorts( L"Ports" );
const PCWSTR	kPort( L"Port" );

// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CPortOptions::Model(
	CPeekDataElement&	ioElement,
	bool				bIsCase )
{
	const bool bIsLoading = ioElement.IsLoading();
	const bool bIsStoring = ioElement.IsStoring();

	size_t	nCount( 0 );

	if ( bIsCase ) {
		CPeekDataElement	elemUseDefaultPorts( kUseDefaultPorts, ioElement );
			elemUseDefaultPorts.Enabled( m_bUseDefaultPorts );
		elemUseDefaultPorts.End();
	}

	CPeekDataElement	elemPorts( kPorts, ioElement );
		if ( bIsStoring ) {
			nCount = m_PortNumberList.GetCount();
			if ( nCount == 0 ) {
				InitializePortNumberDefaults();
				nCount = m_PortNumberList.GetCount();
			}
		}
		else {
			m_PortNumberList.Clean();

			nCount = elemPorts.GetChildCount();
			if ( nCount == 0 ) {
				InitializePortNumberDefaults();
				elemPorts.End();
				return true;
			}
		}

		for ( UInt32 i = 0; i < nCount; i++ ) {
			UInt16	nPort( 0 );
			bool	bEnabled( false );

			if ( bIsStoring ) {
				nPort = m_PortNumberList[i]->GetPort();
				bEnabled = m_PortNumberList[i]->IsEnabled();
			}

			CPeekDataElement elemPort( kPort, elemPorts, i );
			if ( elemPort ) {
				elemPort.Enabled( bEnabled );
				elemPort.Value( nPort );
			}
			
			if ( bIsLoading ) {
				m_PortNumberList.Add( new CPortNumber( bEnabled, nPort ) );
			}
		}

	elemPorts.End();

	return true;
}
