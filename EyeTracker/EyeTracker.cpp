// EyeTracker.cpp : Définit les comportements de classe pour l'application.
//

#include "stdafx.h"
#include "EyeTracker.h"
#include "EyeTrackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEyeTrackerApp

BEGIN_MESSAGE_MAP(CEyeTrackerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// construction CEyeTrackerApp

CEyeTrackerApp::CEyeTrackerApp()
{
	// TODO : ajoutez ici du code de construction,
	// Placez toutes les initialisations significatives dans InitInstance
}


// Seul et unique objet CEyeTrackerApp

CEyeTrackerApp theApp;


// initialisation CEyeTrackerApp

BOOL CEyeTrackerApp::InitInstance()
{
	// InitCommonControls() est requis sur Windows XP si le manifeste de l'application
	// spécifie l'utilisation de ComCtl32.dll version 6 ou ultérieure pour activer les
	// styles visuels.  Dans le cas contraire, la création de fenêtres échouera.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Initialisation standard
	// Si vous n'utilisez pas ces fonctionnalités et que vous souhaitez réduire la taille
	// de votre exécutable final, vous devez supprimer ci-dessous
	// les routines d'initialisation spécifiques dont vous n'avez pas besoin.
	// Changez la clé de Registre sous laquelle nos paramètres sont enregistrés.
	// TODO : Modifiez cette chaîne avec des informations appropriées,
	// telles que le nom de votre société ou organisation
	SetRegistryKey(_T("Applications locales générées par AppWizard"));

	CEyeTrackerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO : Placez ici le code définissant le comportement lorsque la boîte de dialogue est
		//  fermée avec OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO : Placez ici le code définissant le comportement lorsque la boîte de dialogue est
		//  fermée avec Annuler
	}

	// Lorsque la boîte de dialogue est fermée, retourner FALSE afin de quitter
	//  l'application, plutôt que de démarrer la pompe de messages de l'application.
	return FALSE;
}
