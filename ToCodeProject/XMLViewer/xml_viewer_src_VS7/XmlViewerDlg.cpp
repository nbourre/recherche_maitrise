// XmlViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XmlViewer.h"
#include "XmlViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXmlViewerDlg dialog

CXmlViewerDlg::CXmlViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXmlViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXmlViewerDlg)
	m_FileName = _T("");
	m_Tag = _T("");//X3D");
	m_Data = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXmlViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXmlViewerDlg)
	DDX_Control(pDX, IDC_ELEMENT_TREE, m_ElementTree);
	DDX_Text(pDX, IDC_STATIC_FILE_NAME, m_FileName);
	DDX_Text(pDX, IDC_EDIT_TAG, m_Tag);
	DDX_Text(pDX, IDC_DATA, m_Data);
	//}}AFX_DATA_MAP
	DDX_GridControl(pDX, IDC_ATTR_GRID, m_AttrGrid);
}

BEGIN_MESSAGE_MAP(CXmlViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CXmlViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_ELEMENT_TREE, OnClickElementTree)
	ON_BN_CLICKED(IDC_READ_BUTTON, OnReadButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXmlViewerDlg message handlers

BOOL CXmlViewerDlg::OnInitDialog()
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
	m_AttrGrid.SetColumnCount(2);
	m_AttrGrid.SetRowCount(10);
	m_AttrGrid.SetFixedRowCount(1);
	m_AttrGrid.SetItemText(0,0,"Attr");
	m_AttrGrid.SetItemText(0,1,"Value");
	CRect GRect;
	m_AttrGrid.GetWindowRect(&GRect);
	m_AttrGrid.SetColumnWidth(0,GRect.Width()/3);
	m_AttrGrid.SetColumnWidth(1,GRect.Width()*2/3-3);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXmlViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CXmlViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXmlViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CXmlViewerDlg::OnClickElementTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;
	DWORD find;
//	find = m_ElementTree.GetItemData(pNMTreeView->itemNew.hItem);
	DWORD dwPos = ::GetMessagePos(); 
	POINTS pntsPos = MAKEPOINTS( dwPos ); 
	POINT p2;
	p2.x=pntsPos.x;
	p2.y=pntsPos.y;
	// Put into Tree Window
	m_ElementTree.ScreenToClient(&p2);
	UINT uFlags;
    HTREEITEM htItem = m_ElementTree.HitTest(p2, &uFlags);
    if (htItem != NULL)
		find = m_ElementTree.GetItemData(htItem);
	else
		return;
	
	// Get the element
	CXML *SelX;
	SelX = m_XmlPtrArray.GetAt(find);
	// Display the Attributes
	long count = SelX->GetAttrCount();
	m_AttrGrid.SetRowCount(count+1);
	long i = 1;
	CAttribute *a;
	a = SelX->GetFirstAttribute();
	while(a!=NULL && i<=count)
	{
		m_AttrGrid.SetItemText(i,0,a->GetAttributeName());
		m_AttrGrid.SetItemText(i,1,a->GetAttributeText());
		i++;
		a = SelX->GetNextAttribute();
	}
	m_AttrGrid.Invalidate();
	m_Data = SelX->GetText();
	UpdateData(FALSE);
	*pResult = 0;
}

void CXmlViewerDlg::OnReadButton() 
{
	UpdateData(TRUE);
	m_ElementTree.DeleteAllItems();
	UpdateData(FALSE);
	m_xmlRoot.ResetContents();
	CFile in;
	CString Decl;
	Decl ="<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
	CMabString defext=".xml";
	CMabString filter="XML Files (*.xml)| *.xml| All Files (*.*)| *.*||";
	CFileDialog myfile(TRUE,NULL,defext,NULL,filter,NULL);
	if(myfile.DoModal()==IDOK)
	{
		in.Open(myfile.GetPathName(),CFile::modeRead);
		m_FileName.Format("File:%s",myfile.GetPathName());
		m_xmlRoot.XMLRead(&in,m_Tag);
		in.Close();
		in.Open("Test1.xml",CFile::modeCreate|CFile::modeWrite);
		in.Write(Decl,Decl.GetLength());
		m_xmlRoot.XMLWrite(&in,-1); // Set -1 to zero for intenting
		in.Close();
//		in.Open("Test1.xml",CFile::modeRead);
//		m_xmlRoot.XMLRead(&in,m_Tag);
//		in.Close();
//		in.Open("Test2.xml",CFile::modeCreate|CFile::modeWrite);
//		in.Write(Decl,Decl.GetLength());
//		m_xmlRoot.XMLWrite(&in,0);
//		in.Close();
		// Populate the Tree
		m_XmlPtrArray.RemoveAll();
		m_XmlPtrArray.FreeExtra();
		m_XmlPtrArray.SetSize(0,100);
		LoadArray(&m_xmlRoot,NULL);
	}
	Invalidate();
//	UpdateData(FALSE);
}

void CXmlViewerDlg::LoadArray(CXML *element, HTREEITEM hParent)
{
	long pos;
	HTREEITEM me;
	if(hParent==NULL)
		me = m_ElementTree.InsertItem(element->GetTagName(),TVI_ROOT);
	else
		me = m_ElementTree.InsertItem(element->GetTagName(),hParent);

	pos = m_XmlPtrArray.GetSize();
	pos = m_XmlPtrArray.GetUpperBound()+1;
	m_XmlPtrArray.SetAtGrow(pos,element);
	m_ElementTree.SetItemData(me,pos);
	CXML *child;
	child=element->GetFirstChild();
	while(child!=NULL)
	{
		LoadArray(child,me);
		child=element->GetNextChild();
	}
}
