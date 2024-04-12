// ============================================================================
// IpFilterDlg.cpp:
//      implementation of the CIpFilterDlg class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2013. All rights reserved.

#include "StdAfx.h"

#ifdef _WIN32

#include "IpFilterDlg.h"
#include "ItemList.h"
#include "resource.h"


// ============================================================================
//		CIpFilterDialog
// ============================================================================

IMPLEMENT_DYNAMIC(CIpFilterDialog, CDialog)

CIpFilterDialog::CIpFilterDialog(
	CWnd*	pParent /*=nullptr*/ )
	:	CDialog( CIpFilterDialog::IDD, pParent )
{
}


// ----------------------------------------------------------------------------
//		DoDataExchange
// ----------------------------------------------------------------------------

void
CIpFilterDialog::DoDataExchange(
	CDataExchange*	pDX )
{
	CDialog::DoDataExchange( pDX );

	DWORD	dwAddress1;
	DWORD	dwAddress2;
	int		nDirection;
	int		nAnyAddress;
	BOOL	bPort;
	UInt32	nPort;

	if ( !pDX->m_bSaveAndValidate ) {
		dwAddress1 = m_IpFilter.GetAddress1();
		dwAddress2 = m_IpFilter.GetAddress2();
		nDirection = m_IpFilter.GetDirection();
		nAnyAddress = (m_IpFilter.IsAnyOtherAddress())
			? 1 /*IDC_ADDRESS2ANY_RADIO*/
			: 0 /*IDC_ADDRESS2ENABLED_RADIO*/;
		bPort = m_IpFilter.IsPort();
		nPort = m_IpFilter.GetPort();
	}

	DDX_IPAddress( pDX, IDC_ADDRESS1_EDIT, dwAddress1 );
	DDX_IPAddress( pDX, IDC_ADDRESS2_EDIT, dwAddress2 );
	DDX_CBIndex( pDX, IDC_DIRECTION_COMBO, nDirection );
	DDX_Radio( pDX, IDC_ADDRESS2ENABLED_RADIO, nAnyAddress );
	DDX_Check( pDX,  IDC_PORT_CHECK, bPort );
	DDX_Text( pDX, IDC_PORT_EDIT, nPort );

	if ( pDX->m_bSaveAndValidate ) {
		m_IpFilter.SetAddress1( dwAddress1 );
		m_IpFilter.SetAddress2( dwAddress2 );
		m_IpFilter.SetDirection( static_cast<CIpFilter::tDirectionType>( nDirection ) );
		m_IpFilter.SetAnyAddress( nAnyAddress == 1 );
		m_IpFilter.SetPort( (bPort != FALSE) );
		if ( bPort ) {
			CPort	port( static_cast<UInt16>( nPort ) );
			m_IpFilter.SetPort( port );
		}
	}
}


// -----------------------------------------------------------------------------
//		OnInitDialog
// -----------------------------------------------------------------------------

BOOL
CIpFilterDialog::OnInitDialog()
{
	// Call inherited.
	CDialog::OnInitDialog();

	OnBnClickedAddress2Radio();
	OnBnClickedPortCheck();

	return TRUE;
}


// ----------------------------------------------------------------------------
//		Message Map
// ----------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CIpFilterDialog, CDialog)
	ON_BN_CLICKED(IDC_ADDRESS2ENABLED_RADIO, OnBnClickedAddress2Radio)
	ON_BN_CLICKED(IDC_ADDRESS2ANY_RADIO, OnBnClickedAddress2Radio)
	ON_BN_CLICKED(IDC_PORT_CHECK, OnBnClickedPortCheck)
END_MESSAGE_MAP()


// ----------------------------------------------------------------------------
//		OnBnClickedAddress2Radio
// ----------------------------------------------------------------------------

void
CIpFilterDialog::OnBnClickedAddress2Radio()
{
	static UINT	ayIds[] = {
		IDC_ADDRESS2_EDIT,
		0
	};
	CItemList	ids( this, ayIds );

	// Get the checked state of the m_VLANCheck button
	bool	bChecked = (IsDlgButtonChecked( IDC_ADDRESS2ENABLED_RADIO ) == BST_CHECKED);
	ids.EnableItems( bChecked );
}


// ----------------------------------------------------------------------------
//		OnBnClickedPortCheck
// ----------------------------------------------------------------------------

void
CIpFilterDialog::OnBnClickedPortCheck()
{
	static UINT	ayIds[] = {
		IDC_PORT_EDIT,
		0
	};
	CItemList	ids( this, ayIds );

	// Get the checked state of the m_VLANCheck button
	bool	bChecked = (IsDlgButtonChecked( IDC_PORT_CHECK ) == BST_CHECKED);
	ids.EnableItems( bChecked );
}

#endif // _WIN32
