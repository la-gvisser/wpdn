// =============================================================================
//	Options.cpp
// =============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#include "StdAfx.h"
#include "Options.h"
#include "PeekDataModeler.h"


// =============================================================================
//		COptions
// =============================================================================

CPeekString		COptions::s_strTagForensics( L"Forensic Search" );

COptions::COptions()
{
	Reset();
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
	inOther;
}


// -----------------------------------------------------------------------------
//		IsEqual
// -----------------------------------------------------------------------------

bool
COptions::IsEqual(
	const COptions& inOther )
{
	inOther;

	return true;
}


// -----------------------------------------------------------------------------
//		Model
// -----------------------------------------------------------------------------

bool
COptions::Model(
	CPeekDataModeler& ioPrefs )
{
	ioPrefs;
	return true;
}

bool
COptions::Model(
	CPeekDataElement& ioElement )
{
	ioElement;
	return true;
}


// -----------------------------------------------------------------------------
//		Reset
// -----------------------------------------------------------------------------

void
COptions::Reset()
{
////////////////////////////////////////////////////////////////////////
// Example usage
#ifdef IMPLEMENT_CODEEXAMPLES

#ifdef IMPLEMENT_NOTIFICATIONS
	m_bReceiveNotifies = false;
#endif // IMPLEMENT_NOTIFICATIONS

	m_bEnabledExample = FALSE;
	m_strLabelExample = L"User One";
	m_EthernetExample.Reset();
	m_IpAddressExample.Reset();
	m_nPortExample.Reset();
	m_TimeExample.SetNow();
	m_ayComputers.RemoveAll();
//
////////////////////////////////////////////////////////////////////////
#endif // IMPLEMENT_CODEEXAMPLES
}
