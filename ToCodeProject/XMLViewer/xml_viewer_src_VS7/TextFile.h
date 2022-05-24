#if !defined(AFX_TextFile_H__0E251BDB_46E8_11D1_B541_00A024838B6B__INCLUDED_)
#define AFX_TextFile_H__0E251BDB_46E8_11D1_B541_00A024838B6B__INCLUDED_
// (C) 1996-2002 MAB Technologies
// 6521 Wrigley Way
// Fort Worth, TX 76133-5140
//
// CTextFile.h
#include "MabString.h"
//
/////////////////////////////////////////////////////////////////////////////

class CTextFile : public CFile
{
friend class CXML;
public: // create from serialization only
	CTextFile();
	~CTextFile();

protected:
	BOOL m_pEOF;
private:
	char *m_pBuffer;
	long m_bufpos;

public:
	CMabString m_line;
//  added for nastran file
	CMabString m_fldStr[10];
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFile)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL NoBlank;
	void SetNoBlank();
	void Reset();
	char NextChar();
	CString NextLine(char delim=(char)13);
	CString NextNasLine();
	BOOL isEOF();
// added for nastran support
	CMabString fld(int num);
private:
	long m_SizeFile;
// added for nastran support
	void FormatNasLine();
// Generated message map functions
protected:
	//{{AFX_MSG(CTextFile)//ChildFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_TextFile_H__0E251BDB_46E8_11D1_B541_00A024838B6B__INCLUDED_)
