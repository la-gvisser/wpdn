// =============================================================================
//	AdapterOptions.cpp
// =============================================================================
//	Copyright (c) 2008-2017 Savvius, Inc. All rights reserved.

#include "StdAfx.h"
#include "AdapterOptions.h"
#include "PeekDataModeler.h"
#include <string>

const SInt32	g_nDefaultSpeed( -1 );


// =============================================================================
//		CAdapterOptions
// =============================================================================

CAdapterOptions::CAdapterOptions()
	:	m_nSpeed( g_nDefaultSpeed )
	,	m_bDontCheckFileOpen( true )
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
	m_strMonitor = inOther.m_strMonitor;
	m_strMask = inOther.m_strMask;
	m_strName = inOther.m_strName;
	m_nSpeed = inOther.m_nSpeed;
	m_strSave = inOther.m_strSave;
	m_bDontCheckFileOpen = inOther.m_bDontCheckFileOpen;
}


// -----------------------------------------------------------------------------
//		IsEmpty
// -----------------------------------------------------------------------------

bool
CAdapterOptions::IsEmpty() const
{
	if ( m_Id.IsNotNull() ) return false;
	if ( !m_strMonitor.empty() ) return false;
	if ( !m_strMask.empty() ) return false;
	if ( !m_strName.empty() ) return false;
	if ( !m_strSave.empty() ) return false;

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
	if ( inOther.m_strMonitor != m_strMonitor ) return false;
	if ( inOther.m_strMask != m_strMask ) return false;
	if ( inOther.m_strName != m_strName ) return false;
	if ( inOther.m_nSpeed != m_nSpeed ) return false;
	if ( inOther.m_strSave != m_strSave ) return false;
	if ( inOther.m_bDontCheckFileOpen != m_bDontCheckFileOpen ) return false;

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
	//   <FolderAdapter>
	//     <Id>  </Id>
	//     ...
	//   </FolderAdapter>
	// </Root>
	CPeekDataElement	elemRoot( ioPrefs.GetRootName(), ioPrefs );
	CPeekDataElement	elemAdapter( L"FolderAdapter", ioPrefs );
	if ( elemAdapter ) {
		Model( elemAdapter );
	}

	return true;
}

bool
CAdapterOptions::Model(
	CPeekDataElement& ioElement )
{
	// <FolderAdapter>
	//   <Id>  </Id>
	//   ...
	// </FolderAdapter>
	// ioElement is a FolderAdapter
	if ( ioElement ) {
		ioElement.ChildValue( L"Id", m_Id );
		ioElement.ChildValue( L"Monitor", m_strMonitor );
		ioElement.ChildValue( L"Mask", m_strMask );
		ioElement.ChildValue( L"Name", m_strName );
		ioElement.ChildValue( L"Speed", m_nSpeed );
		ioElement.ChildValue( L"Save", m_strSave );
		if ( ioElement.IsStoring() && !m_bDontCheckFileOpen ) {
			ioElement.ChildValue( L"DontCheckFileOpen", m_bDontCheckFileOpen );
		}
		else {
			ioElement.ChildValue( L"DontCheckFileOpen", m_bDontCheckFileOpen );
		}
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
	m_strMonitor.Empty();
	m_strMask.Empty();
	m_strName.Empty();
	m_nSpeed = g_nDefaultSpeed;
	m_strSave.Empty();
	m_bDontCheckFileOpen = true;
}


// -----------------------------------------------------------------------------
//		ToString
// -----------------------------------------------------------------------------

CPeekString
CAdapterOptions::ToString() const
{
	std::wostringstream	ossXML;
	ossXML << L"<FolderAdapter>";
	ossXML << L"<Id>" << m_Id.Format() << L"</Id>";
	ossXML << L"<Monitor>" << m_strMonitor << L"</Monitor>";
	ossXML << L"<Mask>" << m_strMask << L"</Mask>";
	ossXML << L"<Name>" << m_strName << L"</Name>";
	ossXML << L"<Speed>" << std::to_wstring( m_nSpeed ) << L"</Speed>";
	ossXML << L"<Save>" << m_strSave << L"</>";
	ossXML << L"<DontCheckFileOpen>" <<
		((m_bDontCheckFileOpen) ? L"True" : L"False") << L"</DontCheckFileOpen>";
	ossXML << L"</FolderAdapter>";
	return ossXML.str();
}
