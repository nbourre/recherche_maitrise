; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CXmlViewerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "XmlViewer.h"

ClassCount=3
Class1=CXmlViewerApp
Class2=CXmlViewerDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_XMLVIEWER_DIALOG

[CLS:CXmlViewerApp]
Type=0
HeaderFile=XmlViewer.h
ImplementationFile=XmlViewer.cpp
Filter=N

[CLS:CXmlViewerDlg]
Type=0
HeaderFile=XmlViewerDlg.h
ImplementationFile=XmlViewerDlg.cpp
Filter=D
LastObject=CXmlViewerDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=XmlViewerDlg.h
ImplementationFile=XmlViewerDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_XMLVIEWER_DIALOG]
Type=1
Class=CXmlViewerDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_ELEMENT_TREE,SysTreeView32,1350631431
Control3=IDC_ATTR_GRID,MFCGridCtrl,1350631424
Control4=IDC_STATIC_FILE_NAME,static,1342308866
Control5=IDC_READ_BUTTON,button,1342242816
Control6=IDC_EDIT_TAG,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_DATA,edit,1350631620

