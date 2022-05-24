// trackerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CtrackerDlg dialog
class CtrackerDlg : public CDialog
{
// Construction
public:
	CtrackerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TRACKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedbtnopen();
	afx_msg void OnBnClickedbtnprocess();
	afx_msg void OnClose();
	afx_msg void OnNMCustomdrawSliderTest(NMHDR *pNMHDR, LRESULT *pResult);
	int txtSliderValue;
	afx_msg void OnNMReleasedcaptureSliderTest(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_Slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CString m_SliderValue;
	afx_msg void OnBnClickedbtnlk();
	CString szPoints;
	afx_msg void OnBnClickedbtnrefreshlist();
	afx_msg void OnTimer(UINT nIDEvent);
	// Position de la souris
	CString szMouse;
	afx_msg void OnBnClickedOpencam();
};
