// ============================================================================
//	Version.h
// ============================================================================
//	Copyright (c) 2011 WildPackets, Inc. All rights reserved.

#pragma once

#include "Peek.h"
#include "PeekStrings.h"


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//	Version
//
//	A Version has major and minor versions and revisions.
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#ifndef PRODUCT_UNDEFINED
// Taken from WinNT.h in Visual Studio 2008.
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Product types
// This list grows with each OS release.
//
// There is no ordering of values to ensure callers
// do an equality test i.e. greater-than and less-than
// comparisons are not useful.
//
// NOTE: Values in this list should never be deleted.
//       When a product-type 'X' gets dropped from a
//       OS release onwards, the value of 'X' continues
//       to be used in the mapping table of GetProductInfo.
//

#define PRODUCT_UNDEFINED                       0x00000000

#define PRODUCT_ULTIMATE                        0x00000001
#define PRODUCT_HOME_BASIC                      0x00000002
#define PRODUCT_HOME_PREMIUM                    0x00000003
#define PRODUCT_ENTERPRISE                      0x00000004
#define PRODUCT_HOME_BASIC_N                    0x00000005
#define PRODUCT_BUSINESS                        0x00000006
#define PRODUCT_STANDARD_SERVER                 0x00000007
#define PRODUCT_DATACENTER_SERVER               0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER            0x00000009
#define PRODUCT_ENTERPRISE_SERVER               0x0000000A
#define PRODUCT_STARTER                         0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE          0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE            0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE          0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64          0x0000000F
#define PRODUCT_BUSINESS_N                      0x00000010
#define PRODUCT_WEB_SERVER                      0x00000011
#define PRODUCT_CLUSTER_SERVER                  0x00000012
#define PRODUCT_HOME_SERVER                     0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER          0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER         0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER        0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER       0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS        0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM    0x00000019

#define PRODUCT_UNLICENSED                      0xABCDABCD
#endif


// ============================================================================
//		CVersion
// ============================================================================

class CVersion
	:	public COmniModeledData
{
protected:
	UInt32	m_nMajorVersion;
	UInt32	m_nMinorVersion;
	UInt32	m_nMajorRevision;
	UInt32	m_nMinorRevision;

public:
	;		CVersion()
		:	m_nMajorVersion( 0 )
		,	m_nMinorVersion( 0 )
		,	m_nMajorRevision( 0 )
		,	m_nMinorRevision( 0 )
	{}
	;		CVersion( UInt32 inMajorVerions, UInt32 inMinorVersion,
				UInt32 inMajorRevision, UInt32 inMinorRevision )
		:	m_nMajorVersion( inMajorVerions )
		,	m_nMinorVersion( inMinorVersion )
		,	m_nMajorRevision( inMajorRevision )
		,	m_nMinorRevision( inMinorRevision )
	{}

	bool	operator==( const CVersion& inOther ) {
		return ((m_nMajorVersion == inOther.m_nMajorVersion) &&
			(m_nMinorVersion == inOther.m_nMinorVersion) &&
			(m_nMajorRevision == inOther.m_nMajorRevision)&&
			(m_nMinorRevision == inOther.m_nMinorRevision));
	}

	CPeekString	Format() const;

	UInt32	GetMajorRevision() const { return m_nMajorRevision; }
	UInt32	GetMajorVersion() const { return m_nMajorVersion; }
	UInt32	GetMinorRevision() const { return m_nMinorRevision; }
	UInt32	GetMinorVersion() const { return m_nMinorVersion; }

	bool	IsNull() const {
		return ((m_nMajorVersion == 0) && (m_nMinorVersion == 0) &&
			(m_nMajorRevision == 0) && (m_nMinorRevision == 0));
	}

	bool	Parse( const CPeekString& inVersion );

	void	Reset() {
			m_nMajorVersion = 0;
			m_nMinorVersion = 0;
			m_nMajorRevision = 0;
			m_nMinorRevision = 0;
	}

	void	Set( UInt32 inMajorVerions, UInt32 inMinorVersion,
					UInt32 inMajorRevision, UInt32 inMinorRevision ) {
		m_nMajorVersion = inMajorVerions;
		m_nMinorVersion = inMinorVersion;
		m_nMajorRevision = inMajorRevision;
		m_nMinorRevision = inMinorRevision;
	}
	void	Set( DWORD inMostSignificant, DWORD inLeastSignificant ) {
		m_nMajorVersion = HIWORD( inMostSignificant );
		m_nMinorVersion = LOWORD( inMostSignificant );
		m_nMajorRevision = HIWORD( inLeastSignificant );
		m_nMinorRevision = LOWORD( inLeastSignificant );
	}
	void	SetMajorRevision( UInt32 inMajorRevision ) { m_nMajorRevision = inMajorRevision; }
	void	SetMajorVersion( UInt32 inMajorVerions ) { m_nMajorVersion = inMajorVerions; }
	void	SetMinorRevision( UInt32 inMinorRevision ) { m_nMinorRevision = inMinorRevision; }
	void	SetMinorVersion( UInt32 inMinorVersion ) { m_nMinorVersion = inMinorVersion; }

	// COmniModeledData
	virtual CPeekString	ModeledDataGet() const { return Format(); }
	virtual void		ModeledDataSet( const CPeekString& inValue ) { Parse( inValue ); }
};


namespace Environment
{
#if (0)
CPeekString	GetOSVersion();
#endif

bool		GetModuleVersion( CVersion& outFile, CVersion& outProduct, HMODULE inModule = NULL );
bool		GetFileVersion( PCTSTR inFilePath, CVersion& outFile, CVersion& outProduct );
}	// namespace Environment
