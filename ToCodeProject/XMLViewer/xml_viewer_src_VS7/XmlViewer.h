// XmlViewer.h : main header file for the XMLVIEWER application
//

#if !defined(AFX_XMLVIEWER_H__286D3FCD_7CE7_11D4_A09D_00A024838B6B__INCLUDED_)
#define AFX_XMLVIEWER_H__286D3FCD_7CE7_11D4_A09D_00A024838B6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXmlViewerApp:
// See XmlViewer.cpp for the implementation of this class
//

class CXmlViewerApp : public CWinApp
{
public:
	CXmlViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXmlViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXmlViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMLVIEWER_H__286D3FCD_7CE7_11D4_A09D_00A024838B6B__INCLUDED_)
