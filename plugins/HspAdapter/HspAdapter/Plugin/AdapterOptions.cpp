// =============================================================================
//	AdapterOptions.cpp
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "AdapterOptions.h"
#include "PeekDataModeler.h"

const SInt32	g_nDefaultSpeed( -1 );


// =============================================================================
//		CAdapterOptions
// =============================================================================

CAdapterOptions::CAdapterOptions()
{
}

CAdapterOptions::~CAdapterOptions()
{
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
CAdapterOptions::Copy(
	const CAdapterOptions& inOther )
{
	m_Id = inOther.m_Id;
	m_AdapterName = inOther.m_AdapterName;
}


// -----------------------------------------------------------------------------
//		IsEmpty
// -----------------------------------------------------------------------------

bool
CAdapterOptions::IsEmpty() const
{
	if ( m_Id.IsNotNull() ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
CAdapterOptions::IsEqual(
	const CAdapterOptions& inOther ) const
{
	if ( inOther.m_Id != m_Id ) return false;
	if ( inOther.m_AdapterName != m_AdapterName ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
CAdapterOptions::Model(
	CPeekDataModeler& ioPrefs )
{
	// <Root>
	//   <HspAdapter>
	//     <Id>  </Id>
	//	   <Name>  </Name>
	//     ...
	//   </HspAdapter>
	// </Root>
	CPeekDataElement	elemRoot( ioPrefs.GetRootName(), ioPrefs );
	CPeekDataElement	elemAdapter( L"HspAdapter", ioPrefs );
	if ( elemAdapter ) {
		Model( elemAdapter );
	}

	return true;
}

bool
CAdapterOptions::Model(
	CPeekDataElement& ioElement )
{
	// <HspAdapter>
	//   <Id>  </Id>
	//   ...
	// </HspAdapter>
	// ioElement is a HspAdapter
	if ( ioElement ) {
		ioElement.ChildValue( L"Id", m_Id );
		ioElement.ChildValue( L"Name", m_AdapterName );
		ioElement.End();
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
CAdapterOptions::Reset()
{
	m_Id.Reset();
	m_AdapterName.Empty();
}
