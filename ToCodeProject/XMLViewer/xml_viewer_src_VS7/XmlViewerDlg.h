// XmlViewerDlg.h : header file
//

#if !defined(AFX_XMLVIEWERDLG_H__286D3FCF_7CE7_11D4_A09D_00A024838B6B__INCLUDED_)
#define AFX_XMLVIEWERDLG_H__286D3FCF_7CE7_11D4_A09D_00A024838B6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCtrl_src/GridCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CXmlViewerDlg dialog

class CXmlViewerDlg : public CDialog
{
// Construction
public:
	void LoadArray(CXML *element,  HTREEITEM hParent);
	CXmlViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXmlViewerDlg)
	enum { IDD = IDD_XMLVIEWER_DIALOG };
	CTreeCtrl	m_ElementTree;
	CString	m_FileName;
	CString	m_Tag;
	CString	m_Data;
	//}}AFX_DATA
	CGridCtrl m_AttrGrid;
	CXML m_xmlRoot;
	CTypedPtrArray<CPtrArray,CXML*> m_XmlPtrArray;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXmlViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXmlViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClickElementTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReadButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMLVIEWERDLG_H__286D3FCF_7CE7_11D4_A09D_00A024838B6B__INCLUDED_)
