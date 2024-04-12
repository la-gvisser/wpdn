// ============================================================================
//	GeneralOpts.h
//		interface for the CGeneralOpts class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2008. All rights reserved.

#pragma once

#include "Options.h"

// ============================================================================
//		CGeneralOpts
// ============================================================================

class CGeneralOpts
	:	public CPropertyPage
{
protected:
	COptions::COptionsGeneral&	m_Options;

	virtual void DoDataExchange( CDataExchange* pDX );		// DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_GENERAL_OPTS };

	;				CGeneralOpts( COptions& inOptions )
						: CPropertyPage( CGeneralOpts::IDD )
						, m_Options( inOptions.m_General ) {
					}
	virtual			~CGeneralOpts() {}
	virtual BOOL	OnInitDialog();
};
