
// src-tools-mfc.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CsrctoolsmfcApp:
// See src-tools-mfc.cpp for the implementation of this class
//

class CsrctoolsmfcApp : public CWinApp
{
public:
	CsrctoolsmfcApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CsrctoolsmfcApp theApp;
