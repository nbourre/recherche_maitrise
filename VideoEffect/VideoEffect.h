// VideoEffect.h : fichier d'en-t�te principal pour l'application PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error inclut 'stdafx.h' avant d'inclure ce fichier pour PCH
#endif

#include "resource.h"		// symboles principaux


// CVideoEffectApp�:
// Consultez VideoEffect.cpp pour l'impl�mentation de cette classe
//

class CVideoEffectApp : public CWinApp
{
public:
	CVideoEffectApp();

// Substitutions
	public:
	virtual BOOL InitInstance();

// Impl�mentation

	DECLARE_MESSAGE_MAP()
};

extern CVideoEffectApp theApp;
