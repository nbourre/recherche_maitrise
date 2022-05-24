// MABXML.h: interface for the CMABXML class.
// Parsing provided by MSXML DOM Model
//////////////////////////////////////////////////////////////////////
// See source file for comments and revision history.

#if !defined(AFX_MABXML_H__D30A8A49_4E68_11D3_B9EC_00A024838B6B__INCLUDED_)
#define AFX_MABXML_H__D30A8A49_4E68_11D3_B9EC_00A024838B6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>		// MFC Templates
#include "MabString.h"
#include "TextFile.h"
#include <msxml2.h>


class CAttribute : public CObject
{
public:
	void AddToText(CString text);
	DECLARE_SERIAL(CAttribute)
	void Serialize(CArchive& ar);
	CAttribute* Copy();
	CMabString GetAttributeName();
	CAttribute();
	virtual ~CAttribute();
	CMabString m_AttributeName;
	CMabString m_AttributeValue;
	CMabString GetAttributeText();
	int GetAttributeInt();
	long GetAttributeLong();
	float GetAttributeFloat();
	double GetAttributeDouble();
	void SetAttribute(LPCTSTR Value);
	void SetAttribute(int Value);
	void SetAttribute(long Value);
	void SetAttribute(float Value);
	void SetAttribute(double Value);
	CMabString GetField(char Delim, int FieldNum);
	CMabString FirstField(char Delim);
	CMabString NextField(char Delim);
	int GetFieldCount(char Delim);
};

class CXML; // Forward Declaration

class CAttributeList : public CTypedPtrList<CObList, CAttribute*>  
{
public:
	CAttributeList();
	virtual ~CAttributeList();
};

class CXMLList : public CTypedPtrList<CObList, CXML*>  
{
public:
	CXMLList();
	virtual ~CXMLList();

};

class CXML : public CObject
{
public:
	// Helper Functions // 17 Sept 2001
	CString XmlizeString(CString inStr);
	CString UnXmlizeString(CString inStr);
	// added 25 Aug 2000 to handle special attributes of ID and NAME
	void AddChildById(CXML *newchild);
	void AddChildByName(CXML *newchild);
	void SortById();
	void SortByName();
	IXMLDOMNode* FindNode(IXMLDOMNode* pRoot,LPCSTR TagName);
	CXML* FindByName(CString name);
	CXML* FindByID(long id);
	// end 25 Aug 2000
	void InsertChild(CXML *Child);
	LPCTSTR GetTagName();
	void SetTagName(LPCTSTR NewName);
	LPCTSTR GetText();
	void SetText(LPCTSTR TextBlock);
	CXML();
	virtual ~CXML();
	// Block handling
	BOOL Open(LPCTSTR InFilter="XML Files (*.xml)| *.xml| All Files (*.*)| *.*||", LPCTSTR TagName="XML");
	BOOL Save(LPCTSTR InFilter="XML Files (*.xml)| *.xml| All Files (*.*)| *.*||");
	BOOL SaveAs(LPCTSTR InFilter="XML Files (*.xml)| *.xml| All Files (*.*)| *.*||");
	void XMLWrite(CFile *pFile,int depth=-1);
	BOOL XMLReadDOM(CFile *pFile, LPCTSTR TagName=NULL);
	BOOL XMLRead(CFile *pFile, LPCTSTR TagName="XML");
	void ResetContents();
	// Attributeeter access functions
	long GetAttrCount();
	void SetAttr(LPCTSTR Name, LPCTSTR Value);
	void SetAttr(LPCTSTR Name, int Value);
	void SetAttr(LPCTSTR Name, long Value);
	void SetAttr(LPCTSTR Name, float Value);
	void SetAttr(LPCTSTR Name, double Value);
	void SetCurrentAttr(CAttribute *Attribute);
	CAttribute* GetCurrentAttribute();
	CAttribute* GetLastAttribute();
	CAttribute* GetPrevAttribute();
	CAttribute* GetNextAttribute();
	CAttribute* GetFirstAttribute();
	CAttribute* GetAttributePointer(LPCTSTR Name);
	CMabString GetAttrText(LPCTSTR Name);
	CMabString GetAttrTextUS(LPCTSTR Name);
	CMabString GetTag();
	CMabString GetAttr(LPCTSTR Name);
	int GetAttrInt(LPCTSTR Name);
	long GetAttrLong(LPCTSTR Name);
	float GetAttrFloat(LPCTSTR Name);
	double GetAttrDouble(LPCTSTR Name);
	void RemoveCurrentAttr();
	// Child Access Functions
	long GetChildCount();
	void AddChild(CXML *Child);
	void SetCurrentChild(CXML *Child);
	void RemoveCurrentChild();
	CXML* GetCurrentChild();
	CXML* GetLastChild();
	CXML* GetPrevChild();
	CXML* GetNextChild();
	CXML* GetFirstChild();
	BOOL SetParent(CXML* parent);
	CXML* GetParent();

public:
	BOOL Load(CString FileName);
	BOOL LoadXML(BSTR bstrXML, LPCSTR TagName);
	BOOL LoadXML(CString strFileName);
	BOOL LoadXML(IXMLDOMNode *pNode);
	void RemoveText();
	long FindQuote(CString *str, char *q);
	void LoadQuickBooks(CMabString DataField);
	void InsertChildBefore(int ZeroBasedPos, CXML *Child);
	CXML* RemoveAt(int position);
	DECLARE_SERIAL(CXML)
	void Serialize(CArchive& ar);
	CXML* Copy();
	void AddToText(CString Text);
	void AddToTextTrim(CString Text);
	int GetFieldCount(char Delim);
	CMabString GetField(char Delim, int FieldNum);
	CMabString FirstField(char Delim);
	CMabString NextField(char Delim);
	BOOL IsAlpha(char c);
	void AddAttr(CAttribute *Attribute);
	void RemoveAttr(LPCSTR Name);
	void RemoveChild(CXML *Child);
	void SetAttrUS(LPCSTR Name, double value);
	CXML* GetChildAt(int index);
	CXMLList m_ChildList;
	CMabString m_TagName;
private:
	CXML* m_pParent;
	CMabString m_TextBlock;
	CMabString m_FileName;
	CMabString GetNextLine(CFile *pFile);
	void ParseBlock(CMabString Line, CFile *pFile);
	BOOL IsStartBlock(CMabString Line, CMabString TagName);
	BOOL m_EOF;
	CAttributeList m_AttributeList;
	POSITION m_AttributePos,m_ChildPos;
	char m_strQtest[2];
};


#endif // !defined(AFX_MABXML_H__D30A8A49_4E68_11D3_B9EC_00A024838B6B__INCLUDED_)
