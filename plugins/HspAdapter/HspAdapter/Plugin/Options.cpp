// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2008-2015 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "AdapterOptions.h"
#include "PeekDataModeler.h"


// =============================================================================
//		COptions
// =============================================================================

COptions::COptions()
{
}

COptions::~COptions()
{
}


// -----------------------------------------------------------------------------
//		Copy
// -----------------------------------------------------------------------------

void
COptions::Copy(
	const COptions& inOther )
{
	(void)inOther;
}


// -----------------------------------------------------------------------------
//		Delete
// -----------------------------------------------------------------------------

bool
COptions::Delete(
	CGlobalId	inId )
{
	for ( auto itr = m_ayAdapters.begin(); itr != m_ayAdapters.end(); ++itr ) {
		if ( itr->GetId() == inId ) {
			m_ayAdapters.Remove( *itr );
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		Find
// -----------------------------------------------------------------------------

bool
COptions::Find(
	CGlobalId			inId,
	CAdapterOptions&	outAdapterOptions ) const
{
	for ( auto itr = m_ayAdapters.begin(); itr != m_ayAdapters.end(); ++itr ) {
		if ( itr->GetId() == inId ) {
			outAdapterOptions = *itr;
			return true;
		}
	}

	return false;
}


// -----------------------------------------------------------------------------
//		IsEmpty
// -----------------------------------------------------------------------------

bool
COptions::IsEmpty() const
{
	return m_ayAdapters.IsEmpty();
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther ) const
{
	if ( inOther.m_ayAdapters.GetCount() != m_ayAdapters.GetCount() ) return false;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	CPeekDataElement	elemRoot( L"HspAdapters", ioPrefs );
	if ( elemRoot ) {
		UInt32	nChildCount = elemRoot.GetChildCount( L"HspAdapter" );
		if ( m_ayAdapters.IsEmpty() && (nChildCount > 0) ) {
			m_ayAdapters.SetCount( nChildCount );
		}
		size_t	nIndex = 0;
		for ( auto itr = m_ayAdapters.begin(); itr != m_ayAdapters.end(); ++itr ) {
			CPeekDataElement	elemAdapter( L"HspAdapter", elemRoot, nIndex );
			itr->Model( elemAdapter );
		}
	}

	return true;
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	CPeekDataElement	elemRoot( L"HspAdapters", ioElement );
	if ( elemRoot ) {
		UInt32	nChildCount = elemRoot.GetChildCount( L"HspAdapter" );
		if ( m_ayAdapters.IsEmpty() && (nChildCount > 0) ) {
			m_ayAdapters.SetCount( nChildCount );
		}
		size_t	nIndex = 0;
		for ( auto itr = m_ayAdapters.begin(); itr != m_ayAdapters.end(); ++itr ) {
			CPeekDataElement	elemAdapter( L"HspAdapter", elemRoot, nIndex );
			itr->Model( elemAdapter );
		}
	}

	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset()
{
	m_ayAdapters.RemoveAll();
}
