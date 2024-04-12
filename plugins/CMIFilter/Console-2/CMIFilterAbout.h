// ============================================================================
//	CMIFilterAbout.h:
//		interface for the CCMIFilterAbout class.
// ============================================================================
//	Copyright (c) WildPackets, Inc. 2004. All rights reserved.

#pragma once


// ============================================================================
//		CCMIFilterAbout
// ============================================================================

class CCMIFilterAbout
	:	public CDialog
{
protected:
	enum { IDD = IDD_ABOUT };

	DECLARE_MESSAGE_MAP()

	virtual void	DoDataExchange( CDataExchange* pDX );
	BOOL			OnInitDialog();

public:
	;		CCMIFilterAbout( CWnd* pParent = NULL );
};
