
// mywork.h : main header file for the mywork application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CmyworkApp:
// See mywork.cpp for the implementation of this class
//

class CmyworkApp : public CWinApp
{
public:
	CmyworkApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CmyworkApp theApp;
