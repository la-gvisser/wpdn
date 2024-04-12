// ============================================================================
// WizardInstaller.h :
//		main header file for the Peek Plugin Wizard Installer application
// ============================================================================
//	Copyright (c) Savvius, Inc. 2006-2017. All rights reserved.

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


///////////////////////////////////////////////////////////////////////////////
//		CWizardInstallerApp
//
//	See WizardInstaller.cpp for the implementation of this class
///////////////////////////////////////////////////////////////////////////////

class CWizardInstallerApp
	:	public CWinApp
{
protected:
	DECLARE_MESSAGE_MAP()

public:
	;				CWizardInstallerApp();
	virtual			~CWizardInstallerApp();

	virtual BOOL	InitInstance();
};

extern CWizardInstallerApp theApp;
