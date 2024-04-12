// =============================================================================
//	ItemList.h
// =============================================================================
//	Copyright (c) 2014 WildPackets, Inc. All rights reserved.

#pragma once

#ifdef _WIN32

#include "afxwin.h"
#include <vector>


// =============================================================================
//		CItemList
// =============================================================================

class CItemList
	:	std::vector<UINT>
{
protected:
	CDialog*	m_pDlg;

public:
	static void EnableItem( CDialog* inDlg, UINT inId, bool inEnable ) {
		CWnd*	pItem( inDlg->GetDlgItem( inId ) );
		if ( pItem ) pItem->EnableWindow( inEnable );
	}
	static void ShowItem( CDialog* inDlg, UINT inId, bool inShow ) {
		CWnd*	pItem( inDlg->GetDlgItem( inId ) );
		if ( pItem ) pItem->ShowWindow( (inShow) ? SW_SHOW : SW_HIDE );
	}

	CItemList( CDialog* inDlg )
		: m_pDlg( inDlg )
	{
	}

	CItemList( CDialog* inDlg, UINT* inIds )
		: m_pDlg( inDlg )
	{
		if ( inIds ) {
			const UINT* pNext( inIds );
			while ( *pNext ) {
				push_back( *pNext );
				++pNext;
			}
		}
	}

	void operator=( UINT* inIds ) {
		inIds;
	}

	void EnableItems( bool inEnable ) {
		std::vector<UINT>::const_iterator itr;
		for ( itr = begin(); itr != end(); ++itr ) {
			EnableItem( m_pDlg, *itr, inEnable );
		}
	}

	void ShowItems( bool inShow ) {
		std::vector<UINT>::const_iterator itr;
		for ( itr = begin(); itr != end(); ++itr ) {
			ShowItem( m_pDlg, *itr, inShow );
		}
	}
};

#endif // _WIN32
