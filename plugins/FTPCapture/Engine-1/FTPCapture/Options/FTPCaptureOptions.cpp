// ============================================================================
//	FTPCaptureOptions.cpp:
//		implementation of the CFTPCaptureOptions class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2005. All rights reserved.

#include "stdafx.h"
#include "resource.h"
#include "FTPCaptureOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ============================================================================
//		CFTPCaptureOptions
// ============================================================================

CFTPCaptureOptions::CFTPCaptureOptions(
	UINT	nIDCaption,
	CWnd*	pParentWnd,
	UINT	iSelectPage )
	:	CPropertySheet( nIDCaption, pParentWnd, iSelectPage )
	,	m_TargetNamePage( m_Options.GetTargetNames() )
	,	m_PortNumberPage( m_Options.GetPortList() )
	,	m_AlertPage( m_Options )
  #ifdef IMPLEMENT_DATA_CAPTURE
	,	m_DataTransferPage( m_Options )
  #endif // IMPLEMENT_DATA_CAPTURE
{
	AddPage( &m_TargetNamePage );
	AddPage( &m_PortNumberPage );
	AddPage( &m_AlertPage );
  #ifdef IMPLEMENT_DATA_CAPTURE
	AddPage( &m_DataTransferPage );
  #endif // IMPLEMENT_DATA_CAPTURE
}


CFTPCaptureOptions::CFTPCaptureOptions(
	LPCTSTR	pszCaption,
	CWnd*	pParentWnd,
	UINT	iSelectPage )
	:	CPropertySheet( pszCaption, pParentWnd, iSelectPage )
	,	m_TargetNamePage( m_Options.GetTargetNames() )
	,	m_PortNumberPage( m_Options.GetPortList() )
	,	m_AlertPage( m_Options )
  #ifdef IMPLEMENT_DATA_CAPTURE
	,	m_DataTransferPage( m_Options )
  #endif // IMPLEMENT_DATA_CAPTURE
{
	AddPage( &m_TargetNamePage );
	AddPage( &m_PortNumberPage );
	AddPage( &m_AlertPage );
  #ifdef IMPLEMENT_DATA_CAPTURE
	AddPage( &m_DataTransferPage );
  #endif // IMPLEMENT_DATA_CAPTURE
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CFTPCaptureOptions::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	return bResult;
}


// -----------------------------------------------------------------------------
//		Message Map
// -----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CFTPCaptureOptions, CPropertySheet)
END_MESSAGE_MAP()
