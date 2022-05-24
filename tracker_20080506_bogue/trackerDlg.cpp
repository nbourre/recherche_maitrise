// trackerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tracker.h"
#include "trackerDlg.h"
#include ".\trackerdlg.h"
#include "cvApp.h"
#include "CVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CtrackerDlg dialog



CtrackerDlg::CtrackerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtrackerDlg::IDD, pParent)
	//, m_Slider(0)
	//, txtSliderValue(0)
	, m_SliderValue(_T(""))
	, szPoints(_T(""))
	, szMouse(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtrackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Slider(pDX, IDC_SLIDER_TEST, m_Slider);
	DDX_Control(pDX, IDC_SLIDER, m_Slider);
	DDX_Text(pDX, IDC_SLIDER_VALUE, m_SliderValue);
	DDX_Text(pDX, IDC_txtPointList, szPoints);
	DDX_Text(pDX, IDC_EDIT_Mouse, szMouse);
}

BEGIN_MESSAGE_MAP(CtrackerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_btnOpen, OnBnClickedbtnopen)
	ON_BN_CLICKED(IDC_btnProcess, OnBnClickedbtnprocess)
	ON_WM_CLOSE()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_btnLK, OnBnClickedbtnlk)
	ON_BN_CLICKED(IDC_btnRefreshList, OnBnClickedbtnrefreshlist)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_OPENCAM, OnBnClickedOpencam)
END_MESSAGE_MAP()


// CtrackerDlg message handlers

BOOL CtrackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_Slider.SetRangeMin(0, false);
	m_Slider.SetRangeMax(255, false);
	m_SliderValue = "0";
	UpdateData(FALSE);

	// Initialisation des timers
	SetTimer(IDT_Mouse, 100, NULL);
	SetTimer(IDT_Position, 100, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtrackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtrackerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtrackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtrackerDlg::OnBnClickedbtnopen()
{
	  CFileDialog dlg(TRUE, _T("*.bmp"), "", OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		  "AVI files (*.avi) |*.avi|image files (*.bmp; *.jpg) |*.bmp;*.jpg|All Files (*.*)|*.*||",NULL);
	  
	  char title[]= {"Open Image"};
	  dlg.m_ofn.lpstrTitle = title;
	  
	  if (dlg.DoModal() == IDOK) 
	  {
		  CString path= dlg.GetPathName();  // contain the selected filename

		  vdo = new CVideo(path, false);
		  

		  //if (proc != 0)
			 // delete proc;

		  //proc = new ImageProcessor(path);
	  }

}

void CtrackerDlg::OnBnClickedbtnprocess()
{	
	
	vdo->PlayProcessed();
}

void CtrackerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	// Si la vidéo joue, on brise la boucle infinie
	if (vdo != NULL)
		vdo->BreakPlayback = true;

	cvDestroyAllWindows();

	KillTimer(IDT_Mouse);
	KillTimer(IDT_Position);

	CDialog::OnClose();
}

void CtrackerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (nSBCode == SB_THUMBPOSITION)
	{
		if (!vdo == NULL)
		{
			vdo->SliderValue = nPos;
			m_SliderValue.Format("%ld", nPos);
			UpdateData(false);
		}
	}
	else
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CtrackerDlg::OnBnClickedbtnlk()
{
	// TODO: Add your control notification handler code here
	//vdo->LucasKanade();	
}

void CtrackerDlg::OnBnClickedbtnrefreshlist()
{
	// TODO: Add your control notification handler code here
	//szPoints = vdo->GetPointList();
	//UpdateData(false);
}

// Gestion des timer sur le dialogue
void CtrackerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/*if (vdo != NULL)
	{*/
		// Commenter le 2008/05/06
		//if (nIDEvent == IDT_Position)
		//{
			
		//	//if (vdo->GetLKStatus() && vdo->GetStatus())
		//	//{
		//	//	szPoints = vdo->GetPointList();
		//	//	UpdateData(false);
		//	//}
		//}

		//if (nIDEvent == IDT_Mouse)
		//{
		//	CvPoint ptMouse = vdo->GetMousePos();

		//	szMouse.Format("(%d, %d)", ptMouse.x, ptMouse.y);
		//	UpdateData(false);
		//}
	//}	

	CDialog::OnTimer(nIDEvent);
}

void CtrackerDlg::OnBnClickedOpencam()
{
	// TODO: Add your control notification handler code here
	vdo = new CVideo(0);
}
