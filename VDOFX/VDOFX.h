// VDOFX.h : fichier d'en-t�te principal pour l'application PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error inclut 'stdafx.h' avant d'inclure ce fichier pour PCH
#endif

#include "resource.h"		// symboles principaux


// CVDOFXApp�:
// Consultez VDOFX.cpp pour l'impl�mentation de cette classe
//

class CVDOFXApp : public CWinApp
{
public:
	CVDOFXApp();

// Substitutions
	public:
	virtual BOOL InitInstance();

// Impl�mentation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CVDOFXApp theApp;
