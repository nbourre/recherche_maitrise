// XML.cpp: implementation of the CXML "XML Element Centric" class.
// Primary parsing provided by MSXML DOM Model
//////////////////////////////////////////////////////////////////////
/*
// History
23 Jan 2002 - Modifed usage of MabString Class calls due to it's modification.
27 Aug 2001 - Added parsing and loading via MSXML loadXML option
21 Apr 2001 - Fixed error in saving long intergers due to string length. Thanks to Martin Schmidt.
05 Apr 2001 - Added support for single quotes in parameter declaration
29 Mar 2001 - Fixed error allowing xml declaration to be read as root element
		Fixed several memory leaks.
25 Aug 2000  - renamed helper class from pre XML notation of Param to XML Attribute convention
25 Aug 2000  - Added special funcitons to support standard names of ID and NAME.
20 July 2000 - Add Serialization to class to support embedding in binary applications.
20 July 2000 - Added copy functions
14 July 2000 - Modified read buffer to support large blocks of text and parameter fields.
*/

#include "stdafx.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <atlbase.h>
//#import "msxml4.dll" named_guids

// Implementation of the list of child Tagged Blocks 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CAttributeList::CAttributeList()
{

}

CAttributeList::~CAttributeList()
{
	CAttribute *d;
	while(this->GetCount()>0)
	{
		d=GetTail();
		RemoveTail();
		delete d;
	}
}

CAttribute::CAttribute()
{

}

CAttribute::~CAttribute()
{

}

CMabString CAttribute::GetAttributeText()
{
	return m_AttributeValue;
}

int CAttribute::GetAttributeInt()
{
	return m_AttributeValue.GetInt();
}

long CAttribute::GetAttributeLong()
{
	return m_AttributeValue.GetLong();
}

float CAttribute::GetAttributeFloat()
{
	return m_AttributeValue.GetFloat();
}

double CAttribute::GetAttributeDouble()
{
	return m_AttributeValue.GetDouble();
}

void CAttribute::SetAttribute(LPCTSTR Value)
{
	m_AttributeValue = Value;
}

void CAttribute::SetAttribute(int Value)
{
	CMabString sVal;
	sVal.PutInt(Value);
	SetAttribute(sVal);
}

void CAttribute::SetAttribute(long Value)
{
	CMabString sVal;
	sVal.PutLong(Value);
	SetAttribute(sVal);
}

void CAttribute::SetAttribute(float Value)
{
	CMabString sVal;
	sVal.PutFloat(Value);
	SetAttribute(sVal);
}

void CAttribute::SetAttribute(double Value)
{
	CMabString sVal;
	sVal.PutDouble(Value);
	SetAttribute(sVal);
}

CMabString CAttribute::GetField(char Delim, int FieldNum)
{
	CMabString sf;
	sf = m_AttributeValue.GetField(Delim,FieldNum);
	return sf;
}

CMabString CAttribute::FirstField(char Delim)
{
	CMabString sf;
	sf = m_AttributeValue.FirstField(Delim);
	return sf;
}

CMabString CAttribute::NextField(char Delim)
{
	CMabString sf;
	sf = m_AttributeValue.NextField(Delim);
	return sf;
}

int CAttribute::GetFieldCount(char Delim)
{
	return m_AttributeValue.GetFieldCount(Delim);
}

CAttribute* CAttribute::Copy()
{
	CAttribute* copy;
	copy = new CAttribute;
	copy->m_AttributeName = m_AttributeName;
	copy->m_AttributeValue = m_AttributeValue;
	return copy;
}

// Added Serialization to support imbedded application data. 20 July 2000
IMPLEMENT_SERIAL(CAttribute, CObject, 0);
void CAttribute::Serialize(CArchive &ar)
{
	if(ar.IsStoring())
	{
		ar << m_AttributeName << m_AttributeValue;
	}
	else
	{
		ar >> m_AttributeName >> m_AttributeValue;
	}
}
//&&&&&&&&&&&&&%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Implementation of the list of child Tagged Blocks 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLList::CXMLList()
{

}

CXMLList::~CXMLList()
{

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXML::CXML()
{
	m_EOF=FALSE;
	m_AttributePos=NULL;
	m_ChildPos=NULL;
	m_TextBlock="";
	m_TagName = "";
	m_TextBlock = "";
	m_FileName = "";
	m_strQtest[0]=(char)34;
	m_strQtest[1]=(char)39;
	m_pParent=NULL;
}

CXML::~CXML()
{
	if(m_pParent!=NULL)
	{
		m_pParent->RemoveChild(this);
	}
	ResetContents();
}

void CXML::AddChild(CXML *Child)
{
	if(Child==NULL) return;
	CMabString Tag = Child->GetTagName();
	if(Tag.GetLength()<1)
		{
		Child->SetTagName("Unknown");//AfxMessageBox("No Valid Tag Name");
		}
	m_ChildList.AddTail(Child);
	Child->SetParent(this);
}

void CXML::AddAttr(CAttribute *Attribute)
{
	POSITION pos;
	CMabString NewName = Attribute->m_AttributeName;
	CAttribute *OldAttribute;
	if(m_AttributeList.GetCount()<1)
	{
		m_AttributeList.AddHead(Attribute);
		return;
	}
	pos = m_AttributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		OldAttribute = m_AttributeList.GetAt(pos);
		if(NewName.CompareNoCase(OldAttribute->m_AttributeName)==0)
		{
			OldAttribute->m_AttributeValue = Attribute->m_AttributeValue;
			delete Attribute;
			return;
		}
		else if(NewName<=OldAttribute->m_AttributeName)
		{
			m_AttributeList.InsertBefore(pos,Attribute);
			return;
		}
		m_AttributeList.GetNext(pos);
	}
	m_AttributeList.AddTail(Attribute);
	return;
}

void CXML::AddToText(CString Text)
{
	if(Text.GetLength()<1) return;
	if(Text.GetLength()>2)
	{
		if((Text[0]==13) | (Text[1]==10))
			Text=Text.Left(Text.GetLength()-2);
		if(Text.GetLength()<1) return;
	}
	else
	{
		if((Text[0]==10) | (Text[0]==13)) return;
	}
	m_TextBlock+=Text;
}

void CXML::AddToTextTrim(CString Text) // 21 Sept 2001 Experiment for Formating support
{
	if(Text.GetLength()<1) return;
	int crpos = Text.ReverseFind(char(10));
	int epos = Text.GetLength();
	int pos;
	for(pos=crpos+1;pos<epos;pos++)
	{
		if(Text[pos]>32) 
		{
			m_TextBlock+=Text;
			return;
		}
	}
	if(crpos>0)AddToText(Text.Left(crpos));
}

// Added Copy ability to support versioning - 20 July 2000
CXML* CXML::Copy()
{
	CXML* copy;
	copy = new CXML;
	copy->m_FileName = m_FileName;
	copy->m_TagName = m_TagName;
	copy->m_TextBlock = m_TextBlock;

	CAttribute *p,*np;
	p = GetFirstAttribute();
	while(p!=NULL)
		{
		np=p->Copy();
		copy->m_AttributeList.AddTail(np);
		p = GetNextAttribute();
		}
	CXML *x,*nx;
	x = GetFirstChild();
	while(x!=NULL)
		{
		nx = x->Copy();
		copy->m_ChildList.AddTail(nx);
		x=GetNextChild();
		}
	return copy;
}

CXML* CXML::GetChildAt(int index)
{
	m_ChildPos = m_ChildList.FindIndex(index);
	if(m_ChildPos==NULL) return NULL;
	return m_ChildList.GetAt(m_ChildPos);
}

long CXML::GetChildCount()
{
	return m_ChildList.GetCount();
}

CXML* CXML::GetCurrentChild()
{
	if(m_ChildPos==NULL) return NULL;
	return m_ChildList.GetAt(m_ChildPos);
}

CAttribute* CXML::GetCurrentAttribute()
{
	if(m_AttributePos==NULL) return NULL;
	return m_AttributeList.GetAt(m_AttributePos);
}

CMabString CXML::GetField(char Delim, int FieldNum)
{
	CMabString sf;
	sf = m_TextBlock.GetField(Delim,FieldNum);
	return sf;
}

int CXML::GetFieldCount(char Delim)
{
	return m_TextBlock.GetFieldCount(Delim);
}

// 14 July 2000 - Modified line buffer to enhance larger blocks of data.
CMabString CXML::GetNextLine(CFile* pFile)
{
	static unsigned long LocalBufSize = sizeof(char)*8192;
//	unsigned long LocalBufSize;
//	LocalBufSize = pFile->GetLength();
	CMabString Line="";
	CStringA ReadLine;
	LPTSTR LineBuffer;
	long NRead,pos1,pos2,pos3,possq,posdq,TestChar;
	DWORD fpos;
	unsigned char lastchar = ' ';
	BOOL isEOF=FALSE;
	BOOL InDQuote=FALSE;
	BOOL InSQuote=FALSE;
	// 
	// We are inside of the XML block
	// we first need to find the next block to handle.
	// If there is text data we need to append it to the text data buffer.
	// for this element
	while(!isEOF && lastchar!='<')
	{
		fpos = (DWORD)(pFile->GetPosition());
		ReadLine.Empty();
		LineBuffer = ReadLine.GetBuffer(LocalBufSize);
		memset(LineBuffer,0,LocalBufSize);// 22 Jan 2002
		NRead = pFile->Read(LineBuffer,LocalBufSize);
		ReadLine.ReleaseBuffer(LocalBufSize);
		if(NRead<1)
		{
			m_EOF=TRUE;
			return Line;
		}
		pos1 = ReadLine.Find('<',0);
		posdq = ReadLine.Find('"',0);//FindQuote(&ReadLine,&MatchQuote);
		pos3 = ReadLine.Find('>',0);
		possq = -1;// ReadLine.Find((char)39,0); A single Quote is only meaningful if inside a block.
		pos2=-1;
		if(InDQuote)
		{
			if(posdq>=0)
			{
				AddToText(ReadLine.Left(pos2+1));
				pFile->Seek(fpos+posdq+1,CFile::begin);
				InDQuote=FALSE;
			}
			AddToText(ReadLine);
		}
		else if(InSQuote)
		{
			if(possq>=0)
			{
				AddToText(ReadLine.Left(pos2+1));
				pFile->Seek(fpos+possq+1,CFile::begin);
				InDQuote=FALSE;
			}
			AddToText(ReadLine);
		}
		else
		{
			if(posdq>0 && possq>0) pos2 = min(posdq,possq);
			else if(posdq>0)
			{
				pos2=posdq;
				possq=1000000;
			}
			else if(possq>0)
			{
				pos2=possq;
				posdq=1000000;
			}
			TestChar = ReadLine.GetAt(0);
			if(pos1==0)
			{
				lastchar='<';
				pFile->Seek(fpos+1,CFile::begin);
			}
			else if(pos1==1 && TestChar<33)
			{
				if(pos3<pos2 && pos3>2)
				{
					pFile->Seek(fpos+pos3+1,CFile::begin);
					Line = ReadLine.Mid(pos1,pos3);
					return Line;
				}
				lastchar='<';
				pFile->Seek(fpos+2,CFile::begin);
			}
			else if(pos2==0)
			{
				if(possq>pos2)
				{
					InDQuote=TRUE;
					m_TextBlock+='"';
				}
				else
				{
					InSQuote = TRUE;
					m_TextBlock+=(char)39;
				}
				m_TextBlock+='"';
				pFile->Seek(fpos+1,CFile::begin);
			}
			else if(pos2>=0 && (pos2<pos1 || pos1<0) )
			{
				AddToText(ReadLine.Left(pos2+1));
				pFile->Seek(fpos+pos2+1,CFile::begin);
				InDQuote=TRUE;
			}
			else if(pos1>=0)
			{
				AddToTextTrim(ReadLine.Left(pos1));
				pFile->Seek(fpos+pos1,CFile::begin);
			}
			else
			{
				if(TestChar==10)
					AddToText(ReadLine.Right(ReadLine.GetLength()-1));
				else
					AddToText(ReadLine);
			}
		}
	}
	// We are now inside of the block to read.
	// find the end.
	Line ="<";
	int InDQuote2=1;
	int InSQuote2=1;
	while(!isEOF)
	{
		fpos = (DWORD)(pFile->GetPosition());
		ReadLine.Empty();
		LineBuffer = ReadLine.GetBuffer(LocalBufSize);
		memset(LineBuffer,0,LocalBufSize);
		NRead = pFile->Read(LineBuffer,LocalBufSize);
		ReadLine.ReleaseBuffer(LocalBufSize);
		if(NRead<1)
		{
			m_EOF=TRUE;
			return Line;
		}
		pos1 = ReadLine.Find('<',0);
		posdq = ReadLine.Find('"',0);
		pos3 = ReadLine.Find('>',0);
		possq = ReadLine.Find((char)39,0);
		pos2=-1;
		if(InDQuote2<0)
		{
			if(posdq<0)
			{
				Line+=ReadLine;
			}
			else
			{
				InDQuote2*=-1;
				Line+=ReadLine.Left(posdq+1);
				pFile->Seek(fpos+posdq+1,CFile::begin);
			}
		}
		else if(InSQuote2<0)
		{
			if(possq<0)
			{
				Line+=ReadLine;
			}
			else
			{
				InSQuote2*=-1;
				Line+=ReadLine.Left(possq);
				Line+='"';
				pFile->Seek(fpos+possq+1,CFile::begin);
			}
		}
		else
		{
			if(posdq>0 && possq>0) pos2 = min(posdq,possq);
			else if(posdq>0)
			{
				pos2=posdq;
				possq=1000000;
			}
			else if(possq>0)
			{
				pos2=possq;
				posdq=1000000;
			}
			if(pos2>-1 && pos3>-1)
			{
				if(pos3<pos2)
				{
					Line+=ReadLine.Left(pos3+1);
					pFile->Seek(fpos+pos3+1,CFile::begin);
					return Line;
				}
				if(posdq<possq) InDQuote2*=-1;
				else InSQuote2*=-1;
				Line+=ReadLine.Left(pos2);
				Line+='"';
				pFile->Seek(fpos+pos2+1,CFile::begin);
			}
			else if(pos3>-1)
			{
					Line+=ReadLine.Left(pos3+1);
					pFile->Seek(fpos+pos3+1,CFile::begin);
					return Line;
			}
			else if(pos2>-1)
			{
				if(posdq<possq) InDQuote2*=-1;
				else InSQuote2*=-1;
				Line+=ReadLine.Left(pos2+1);
				pFile->Seek(fpos+pos2+1,CFile::begin);
			}
			else
			{
				Line+=ReadLine;
			}
		}
	}
	if(isEOF)
		m_EOF=TRUE;
	else
		Line+=">";
	return Line;
}

// Attributeeter functions
CMabString CXML::GetAttrText(LPCTSTR Name)
{
	CMabString TestName = Name;
	POSITION pos;
	CAttribute *Attribute;
	if(m_AttributeList.GetCount()<1) return "";
	pos = m_AttributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		Attribute = m_AttributeList.GetNext(pos);
		if(TestName==Attribute->m_AttributeName)
		{
			TestName = Attribute->m_AttributeValue;
			return TestName;
		}
	}
	TestName="";
	return TestName;
}

CMabString CXML::GetAttrTextUS(LPCTSTR Name)
{
	CMabString TestName = Name;
	POSITION pos;
	CAttribute *Attribute;
	if(m_AttributeList.GetCount()<1) return "";
	pos = m_AttributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		Attribute = m_AttributeList.GetNext(pos);
		if(TestName==Attribute->m_AttributeName)
		{
			TestName = Attribute->m_AttributeValue;
			if(TestName.GetLength()<1) TestName="0.00";
			return TestName;
		}
	}
	TestName="0.00";
	return TestName;
}


int CXML::GetAttrInt(LPCTSTR Name)
{
	CMabString sVal = GetAttrText(Name);
	return sVal.GetInt();
}

long CXML::GetAttrLong(LPCTSTR Name)
{
	CMabString sVal = GetAttrText(Name);
	return sVal.GetLong();
}

float CXML::GetAttrFloat(LPCTSTR Name)
{
	CMabString sVal = GetAttrText(Name);
	return sVal.GetFloat();
}

double CXML::GetAttrDouble(LPCTSTR Name)
{
	CMabString sVal = GetAttrText(Name);
	return sVal.GetDouble();
}

CAttribute* CXML::GetFirstAttribute()
{
	if(m_AttributeList.GetCount()<1) 
	{
		m_AttributePos=NULL;
		return NULL;
	}
	m_AttributePos = m_AttributeList.GetHeadPosition();
	return m_AttributeList.GetAt(m_AttributePos);
}

CAttribute* CXML::GetNextAttribute()
{
	if(m_AttributePos==NULL) return NULL;
	m_AttributeList.GetNext(m_AttributePos);
	if(m_AttributePos==NULL) return NULL;
	return m_AttributeList.GetAt(m_AttributePos);
}

CAttribute* CXML::GetPrevAttribute()
{
	if(m_AttributePos==NULL) return NULL;
	m_AttributeList.GetPrev(m_AttributePos);
	if(m_AttributePos==NULL) return NULL;
	return m_AttributeList.GetAt(m_AttributePos);
}


CAttribute* CXML::GetLastAttribute()
{
	if(m_AttributeList.GetCount()<1) 
	{
		m_AttributePos=NULL;
		return NULL;
	}
	m_AttributePos = m_AttributeList.GetTailPosition();
	return m_AttributeList.GetAt(m_AttributePos);
}


void CXML::SetCurrentAttr(CAttribute *Attribute)
{
	if(m_AttributePos==NULL) return;
	m_AttributeList.SetAt(m_AttributePos,Attribute);
}

BOOL CXML::IsStartBlock(CMabString Line, CMabString TagName)
{
	int endpos;
	if(Line[0]!=60) return FALSE;
	if(TagName.GetLength()<1) return TRUE;
	endpos = Line.Find(' ');
	CMabString LineName;
	if(endpos<0)
	{
		endpos = Line.GetLength();
		if(Line[endpos-1]!='>') return FALSE;
		if(Line[endpos-2]=='/') endpos--;
		LineName = Line.Mid(1,endpos-2);
	}
	else
		LineName = Line.Mid(1,endpos-1);
	if(LineName!=TagName) return FALSE;
	return TRUE;
}

void CXML::ParseBlock(CMabString Line, CFile *pFile)
{
	int LinePos,EndPos;
	ResetContents();
	if(Line.GetLength()<1) Line=" ";//return;// mab 12 May 2000
	if(Line[0]!=60) return;
	LinePos = Line.Find(' ');
	if(LinePos<0)
	{
		LinePos = Line.GetLength()-1;
	}
	m_TagName = Line.Mid(1,LinePos-1);
	if(m_TagName[0]=='/') AfxMessageBox("Warning Element Terminator exists without matching initial Block "+m_TagName);
	EndPos = Line.GetLength();
	if(Line[EndPos-1]!='>') AfxMessageBox("Warning Tag Block is Incomplete");
	Line.SetAt(EndPos-1,' ');
	BOOL Children=TRUE;
	if(Line[EndPos-2]=='/')
	{
		Children=FALSE;
		Line.SetAt(EndPos-2,' ');
	}
	Line = Line.Right(Line.GetLength() - m_TagName.GetLength()-1);
	Line.Trim();
	CMabString pname,pval;
	while(Line.GetLength()>1)
	{
		LinePos = Line.Find('=');
		if(LinePos<1) break;
		pname = Line.Left(LinePos);
		pname.Trim();
		LinePos = Line.Find('"');
		Line = Line.Right(Line.GetLength()-LinePos-1);
		LinePos = Line.Find('"');
		pval = Line.Left(LinePos);
		SetAttr((LPCTSTR)pname,(LPCTSTR)pval);
		Line = Line.Right(Line.GetLength()-LinePos-1);
		Line.TrimLeft();
	}
	if(!Children) return;
	Line.Empty();
	Line = GetNextLine(pFile);
	CMabString LastLine;
	LastLine = "</";
	LastLine += m_TagName;
	LastLine += '>';
	CXML *child;
	while(Line!=LastLine && !m_EOF)
	{
		if(Line[1]!='!' && Line[1]!='?')
		{
		child = new CXML;
		child->ParseBlock(Line,pFile);
		if(child->m_TagName.GetLength()>0) AddChild(child);
		else delete child;
		}
		Line = GetNextLine(pFile);
	}
	// Test TextBlock for extra line feed.
	int TestChar,lenpos;
	lenpos = m_TextBlock.GetLength()-1;
	if(lenpos>=0)
		{
		TestChar = m_TextBlock.GetAt(lenpos);
		if(TestChar==10) m_TextBlock.SetAt(lenpos,(char)32);
		}
}

void CXML::RemoveCurrentAttr()
{
	if(m_AttributePos==NULL) return;
	CAttribute *Attr;
	POSITION pos = m_AttributePos;
	Attr = m_AttributeList.GetAt(m_AttributePos);
	m_AttributeList.GetNext(pos);
	if(pos==NULL)
	{
		m_AttributeList.RemoveAt(m_AttributePos);
		GetLastAttribute();
	}
	else
	{
		m_AttributeList.RemoveAt(m_AttributePos);
		m_AttributePos = pos;
	}
// ?? inconsistent with child handlinge	if(Attr!=NULL) delete Attr; // mab 23 mar 2001
}

void CXML::SetAttr(LPCTSTR Name, LPCTSTR Value)
{
	POSITION pos;
	CMabString NewName = Name;
	CAttribute *Attribute,*OldAttribute;
	Attribute = new CAttribute;
	Attribute->m_AttributeName = Name;
	Attribute->m_AttributeValue = Value;
	long cnt = m_AttributeList.GetCount();
	if(m_AttributeList.GetCount()<1)
	{
		m_AttributeList.AddHead(Attribute);
		return;
	}
	pos = m_AttributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		OldAttribute = m_AttributeList.GetAt(pos);
		if(NewName==OldAttribute->m_AttributeName)
		{
			OldAttribute->m_AttributeValue = Value;
			delete Attribute;
			return;
		}
		else if(NewName<=OldAttribute->m_AttributeName)
		{
			m_AttributeList.InsertBefore(pos,Attribute);
			return;
		}
		m_AttributeList.GetNext(pos);
	}
	m_AttributeList.AddTail(Attribute);
	return;
}

void CXML::SetAttr(LPCTSTR Name, int Value)
{
	CMabString sVal;
	sVal.PutInt(Value,12);
	sVal.Trim();
	SetAttr(Name,sVal);
}

void CXML::SetAttr(LPCTSTR Name, long Value)
{
	CMabString sVal;
	sVal.PutLong(Value,12);
	sVal.Trim();
	SetAttr(Name,sVal);
}

void CXML::SetAttr(LPCTSTR Name, float Value)
{
	CMabString sVal;
	sVal.PutFloat(Value,10);
	SetAttr(Name,sVal);
}

void CXML::SetAttr(LPCTSTR Name, double Value)
{
	CMabString sVal;
	sVal.PutDouble(Value,12);
	SetAttr(Name,sVal);
}

// Child functions
CXML* CXML::GetFirstChild()
{
	if(m_ChildList.GetCount()<1)
	{
		m_ChildPos=NULL;
		return NULL;
	}
	m_ChildPos = m_ChildList.GetHeadPosition();
	if(m_ChildPos==NULL) return NULL;
	return m_ChildList.GetAt(m_ChildPos);
}

CXML* CXML::GetNextChild()
{
	if(m_ChildPos==NULL) return NULL;
	m_ChildList.GetNext(m_ChildPos);
	if(m_ChildPos==NULL) return NULL;
	return m_ChildList.GetAt(m_ChildPos);
}

CXML* CXML::GetPrevChild()
{
	if(m_ChildPos==NULL) return NULL;
	m_ChildList.GetPrev(m_ChildPos);
	if(m_ChildPos==NULL) return NULL;
	return m_ChildList.GetAt(m_ChildPos);
}


CXML* CXML::GetLastChild()
{
	if(m_ChildList.GetCount()<1)
	{
		m_ChildPos=NULL;
		return NULL;
	}
	m_ChildPos = m_ChildList.GetTailPosition();
	return m_ChildList.GetAt(m_ChildPos);
}


void CXML::SetCurrentChild(CXML *Child)
{
	if(m_ChildPos==NULL) return;
	m_ChildList.SetAt(m_ChildPos,Child);
}

void CXML::RemoveChild(CXML *Child)
{
	CXML *child;
	child = GetFirstChild();
	while(child!=NULL)
	{
		if(child==Child)
		{
			m_ChildList.RemoveAt(m_ChildPos);
			child->m_pParent=NULL;
			return;
		}
		child = GetNextChild();
	}
}

void CXML::RemoveCurrentChild()
{
	if(m_ChildPos==NULL) return;
	CXML *x;
	x = m_ChildList.GetAt(m_ChildPos);
	POSITION pos = m_ChildPos;
	m_ChildList.GetNext(pos);
	if(pos==NULL)
	{
		m_ChildList.RemoveAt(m_ChildPos);
		GetLastChild();
	}
	else
	{
		m_ChildList.RemoveAt(m_ChildPos);
		m_ChildPos = pos;
	}
}

void CXML::ResetContents()
{
	CAttribute *p;
	while(m_AttributeList.GetCount()>0)
		{
		p=m_AttributeList.GetTail();
		m_AttributeList.RemoveTail();
		delete p;
		}
	CXML *x;
	while(m_ChildList.GetCount()>0)
		{
		x=m_ChildList.GetTail();
		m_ChildList.RemoveTail();
		delete x;
		}
	m_TextBlock.Empty();
}

long CXML::GetAttrCount()
{
	return m_AttributeList.GetCount();
}

void CXML::XMLWrite(CFile *pFile,int depth)
{
	CMabString Line;
	CMabString Addition;
	if(depth>-1) for(int i=0;i<depth;i++) Line+="  ";//char(9); // 21 Sept 2001 - Format experiment
	Line += "<";
	ASSERT(m_TagName.GetLength()>0);
	Line += m_TagName;
	CAttribute *Attribute;
	Attribute = GetFirstAttribute();
	while(Attribute!=NULL)
	{
		Addition = " ";
		Addition += Attribute->m_AttributeName;
		Addition += "=\"";
		Attribute->m_AttributeValue.Trim();
		Addition += XmlizeString(Attribute->m_AttributeValue);
		Addition += "\"";
		Line += Addition;
		Attribute = GetNextAttribute();
	}
	if((m_ChildList.GetCount()<1) && (m_TextBlock.GetLength()<1))
	{
		Addition = "/>\n";
		Line += Addition;
		pFile->Write(Line,Line.GetLength());
		return;
	}
	if(m_TextBlock.GetLength()>0) Addition = ">";
	else Addition = ">\n";
	Line += Addition;
	pFile->Write(Line,Line.GetLength());
	// Now the children
	int termchar;
	if(m_TextBlock.GetLength()>0)
		{
		CString WriteBlock = XmlizeString(m_TextBlock);
		termchar = WriteBlock.GetAt(WriteBlock.GetLength()-1);
		if(termchar!=10)
		{
			WriteBlock+= char(10);
		}
		pFile->Write(WriteBlock,WriteBlock.GetLength());
		}
	CXML *child;
	child = GetFirstChild();
	while(child!=NULL)
	{
		if(depth>-1) child->XMLWrite(pFile,depth+1);
		else child->XMLWrite(pFile,-1);
		child = GetNextChild();
	}
	// and the terminator
	Line="";
	if(depth>-1) for(int i=0;i<depth;i++) Line+="  ";//char(9);
	Line += "</";
	Line += m_TagName;
	Line += ">\n";
	pFile->Write(Line,Line.GetLength());
	return;
}

BOOL CXML::Open(LPCTSTR InFilter, LPCTSTR TagName)
{
	CFile in;
	CMabString defext;
	CMabString filter = InFilter;
	int pos1 = filter.Find('(');
	int pos2 = filter.Find(')');
	defext = filter.Mid(pos1+1,pos2-pos1-1);
	CFileDialog myfile(TRUE,NULL,defext,NULL,filter,NULL);
	if(myfile.DoModal()==IDOK)
	{
		in.Open(myfile.GetPathName(),CFile::modeRead);
		XMLReadDOM(&in,TagName);
		in.Close();
		m_FileName = myfile.GetPathName();
		return TRUE;
	}
	return FALSE;
}

BOOL CXML::SaveAs(LPCTSTR InFilter)
{
	CFile in;
	CMabString defext;
	CMabString filter = InFilter;
	int pos1 = filter.Find('(');
	int pos2 = filter.Find(')');
	defext = filter.Mid(pos1+1,pos2-pos1-1);
	CFileDialog myfile(FALSE,NULL,defext,NULL,filter,NULL);
	if(myfile.DoModal()==IDOK)
	{
		in.Open(myfile.GetPathName(),CFile::modeCreate|CFile::modeWrite);
		XMLWrite(&in);
		in.Close();
		m_FileName = myfile.GetPathName();
		return TRUE;
	}
	m_FileName = "";
	return FALSE;
}

BOOL CXML::Save(LPCTSTR InFilter)
{
	if(m_FileName.GetLength()<1)
	{
		return SaveAs(InFilter);
	}
	CFile in;
	in.Open(m_FileName,CFile::modeCreate|CFile::modeWrite);
	XMLWrite(&in);
	in.Close();
	return TRUE;
}

void CXML::SetTagName(LPCTSTR NewName)
{
//	CString line;
//	line.Format("Setting tag from %s to %s",(LPCTSTR)m_TagName,NewName);
//	AfxMessageBox(line);
	m_TagName = NewName;
}

LPCTSTR CXML::GetTagName()
{
	return (LPCTSTR)m_TagName;
}

CMabString CAttribute::GetAttributeName()
{
	return m_AttributeName;
}

void CXML::SetText(LPCTSTR NewText)
{
	m_TextBlock = NewText;
}

LPCTSTR CXML::GetText()
{
	return (LPCTSTR)m_TextBlock;
}


void CXML::InsertChild(CXML *Child)
{
	if(m_ChildPos==NULL) 
		m_ChildList.AddTail(Child);
	else
		m_ChildList.InsertBefore(m_ChildPos,Child);
	return;
}

CMabString CXML::GetTag()
{
	return m_TagName;
}

CMabString CXML::GetAttr(LPCTSTR Name)
{
	CMabString TestName = Name;
	POSITION pos;
	CAttribute *Attribute;
	if( (m_AttributeList.GetCount()) < 1) return "";
	pos = m_AttributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		Attribute = m_AttributeList.GetNext(pos);
		if(TestName==Attribute->m_AttributeName) return Attribute->m_AttributeValue;
	}
	return "";
}

void CXML::SetAttrUS(LPCSTR Name, double value)
{
	CString str;
	str.Format("%.2f",value+0.001);
	SetAttr(Name,str);
}

void CXML::RemoveAttr(LPCSTR Name)
{
	CMabString TestName = Name;
	CAttribute *Attribute;
	if( (m_AttributeList.GetCount()) < 1) return;
	Attribute = GetFirstAttribute();
	while(Attribute!=NULL)
	{
		if(TestName==Attribute->m_AttributeName)
		{
			RemoveCurrentAttr();
			return;
		}
		Attribute = GetNextAttribute();
	}
}

BOOL CXML::IsAlpha(char c)
{
	if(c>64 && c<91) return TRUE;
	if(c>96 && c<123) return TRUE;
	return FALSE;
}

CMabString CXML::FirstField(char Delim)
{
	CMabString sf;
	sf = m_TextBlock.FirstField(Delim);
	return sf;
}

CMabString CXML::NextField(char Delim)
{
	CMabString sf;
	sf = m_TextBlock.NextField(Delim);
	return sf;
}

BOOL CXML::XMLReadDOM(CFile *pFile, LPCTSTR TagName)
{
	CString source;
	pFile->Read(source.GetBuffer((int)pFile->GetLength()),(int)pFile->GetLength());
	source.ReleaseBuffer();
	BOOL DOMLoaded = LoadXML(source.AllocSysString(), TagName);
	
	if(!DOMLoaded)
	{
		// Use legacy parser
		pFile->SeekToBegin();
		DOMLoaded = XMLRead(pFile,TagName);
	}	

	m_TextBlock = UnXmlizeString(m_TextBlock);
	
	return DOMLoaded;
}


BOOL CXML::XMLRead(CFile *pFile, LPCTSTR TagName)
{
	m_TagName = TagName;
	CMabString NextLine;
	NextLine = GetNextLine(pFile);
	// Skip header
	if(NextLine.GetLength()>1) while((NextLine[1]==63) || (NextLine[1]==33)) NextLine=GetNextLine(pFile);
	while (!m_EOF && !IsStartBlock(NextLine,TagName))
	{
		NextLine = GetNextLine(pFile);
	}
	m_TextBlock.Empty();
	if(m_EOF) return FALSE;
	else ParseBlock(NextLine,pFile);
	m_TextBlock = UnXmlizeString(m_TextBlock);
	return TRUE;
}


CAttribute* CXML::GetAttributePointer(LPCTSTR Name)
{
	CMabString TestName = Name;
	POSITION pos;
	CAttribute *Attribute;
	if( (m_AttributeList.GetCount()) < 1) return NULL;
	pos = m_AttributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		Attribute = m_AttributeList.GetNext(pos);
		if(TestName==Attribute->m_AttributeName) return Attribute;
	}
	return NULL;
}

IMPLEMENT_SERIAL(CXML, CObject, 0);
void CXML::Serialize(CArchive &ar)
{
	if(ar.IsStoring())
	{
		ar << m_TagName << m_TextBlock;
	}
	else
	{
		ar >> m_TagName >> m_TextBlock;
	}
	m_AttributeList.Serialize(ar);
	m_ChildList.Serialize(ar);
}


CXML* CXML::RemoveAt(int position)
{
	CXML* child;
	child = GetChildAt(position);
	if(child!=NULL) RemoveCurrentChild();
	return child;
}

void CXML::InsertChildBefore(int ZeroBasedPos, CXML *Child)
{
	m_ChildList.InsertBefore(m_ChildList.FindIndex(ZeroBasedPos),Child);
}

void CAttribute::AddToText(CString text)
{
	m_AttributeValue+=text;
}


// Added 25 Aug 2000 to handle the special case of the ID and Name attributes

CXML* CXML::FindByID(long id)
{
	CXML *child,*grand;
	child=GetFirstChild();
	while(child!=NULL)
	{
		if(child->GetAttrLong("ID")==id) return child;
		grand=child->FindByID(id);
		if(grand!=NULL) return grand;
		child=GetNextChild();
	}
//	AfxMessageBox("XML ID NOT FOUND");
	return child;
}

CXML* CXML::FindByName(CString name)
{
	CXML *child,*grand;
	child=GetFirstChild();
	while(child!=NULL)
	{
		if(name.CompareNoCase(child->GetAttrText("NAME"))==0) return child;
		grand=child->FindByName(name);
		if(grand!=NULL) return grand;
		child=GetNextChild();
	}
	return child;
}

void CXML::SortByName()
{
	CXML *c1,*c2;
	CString n1;
restart:
	c1=GetFirstChild();
	c2=GetNextChild();
	while(c2!=NULL)
	{
		n1=c1->GetAttrText("NAME");
		if(n1.CompareNoCase(c2->GetAttrText("NAME"))>0)
		{
			RemoveCurrentChild();
			AddChildByName(c2);
			goto restart;
		};
		c1=c2;
		c2=GetNextChild();
	}
	return;

}

void CXML::SortById()
{
	CXML *c1,*c2;
	long n1;
restart:
	c1=GetFirstChild();
	c2=GetNextChild();
	while(c2!=NULL)
	{
		n1=c1->GetAttrLong("ID");
		if(n1>c2->GetAttrLong("ID"))
		{
			RemoveCurrentChild();
			AddChildById(c2);
			goto restart;
		};
		c1=c2;
		c2=GetNextChild();
	}
	return;

}

void CXML::AddChildByName(CXML *newchild)
{
	CXML *child;
	CString n1;
	child=GetFirstChild();
	while(child!=NULL)
	{
		n1=child->GetAttrText("NAME");
		if(n1.CompareNoCase(newchild->GetAttrText("NAME"))>0)
		{
			InsertChild(newchild);
			newchild->SetParent(this);
			return;
		}
		child=GetNextChild();
	}
	AddChild(newchild);
	newchild->SetParent(this);
	return;
}

void CXML::AddChildById(CXML *newchild)
{
	CXML *child;
	long id;
	id=newchild->GetAttrLong("ID");
	child=GetFirstChild();
	while(child!=NULL)
	{
		if(id<child->GetAttrLong("ID"))
		{
			InsertChild(newchild);
			newchild->SetParent(this);
			return;
		}
		child=GetNextChild();
	}
	AddChild(newchild);
	newchild->SetParent(this);
	return;
}

void CXML::LoadQuickBooks(CMabString DataField)
{
	CMabString f,fname,fval;
	long fields = DataField.GetFieldCount(char(30));
	// Field Zero is blank;
	for(long i=1;i<fields;i++)
	{
		f=DataField.GetField(char(30),i);
		fname = f.GetField(char(28),0);
		fval  = f.GetField(char(28),1);
		if(fname.GetLength()>0) SetAttr(fname,fval);
	}
}

long CXML::FindQuote(CString *str, char *q)
{
	*q = (char)34;
	long pos;
	pos = str->FindOneOf(m_strQtest);
	if(pos>-1) *q = str->GetAt(pos);
	return pos;
}

void CXML::RemoveText()
{
	m_TextBlock="";
	CXML *child;
	child=GetFirstChild();
	while(child!=NULL)
	{
		child->RemoveText();
		child=GetNextChild();
	}
}

BOOL CXML::LoadXML(BSTR bstrXML, LPCSTR TagName)
{
	VARIANT_BOOL varOkay;
	HRESULT hr;

	IXMLDOMDocument *pXML = NULL;
	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
       IID_IXMLDOMDocument2, (void**)&pXML); // Check the return value, hr...
	ASSERT(SUCCEEDED(hr) && pXML!=NULL);

	hr = pXML->loadXML(bstrXML,&varOkay);
	// The MSXML parser is not nearly as robust as the internal one.
	// reocver if needed!
	if(!(SUCCEEDED(hr) && varOkay))
	{
		return FALSE;
	}

	IXMLDOMElement *pRoot;
	IXMLDOMNode *pReqNode;
	hr = pXML->get_documentElement(&pRoot);
	ASSERT(SUCCEEDED(hr));
	ASSERT(pRoot!=NULL);

	ResetContents();
	
	// We have the option of loading a child Node
	if(TagName==NULL)
	{
		varOkay = LoadXML((IXMLDOMNode*)pRoot);
	}
	else
	{
		pReqNode = FindNode(pRoot,TagName);
		if(pReqNode!=NULL) varOkay = LoadXML(pReqNode);
		else varOkay = FALSE;
	}

	return varOkay;
}

IXMLDOMNode* CXML::FindNode(IXMLDOMNode* pRoot,LPCSTR TagName)
{
	CComBSTR bstrName;
	USES_CONVERSION;
	HRESULT hr;
	CString Name=TagName;
	hr = pRoot->get_nodeName(&bstrName);
	if(Name.Compare(W2A(bstrName.m_str))==0)
	{
		return pRoot;
	}
	IXMLDOMNode *child,*find;
	hr = pRoot->get_firstChild(&child);
	while(child!=NULL)
	{
		find = FindNode(child,TagName);
		if(find!=NULL) return find;
		hr = child->get_nextSibling(&child);
	}
	return NULL;
}

 // This uses the MsDom Parser an alternative is the XMLRead functions that useses the legacy reader.
BOOL CXML::LoadXML(CString strFileName)
{
	CComVariant varFileName = (LPCSTR)strFileName;
	VARIANT_BOOL varOkay;
	HRESULT hr;

	IXMLDOMDocument *pXML = NULL;
	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
       IID_IXMLDOMDocument2, (void**)&pXML); // Check the return value, hr...
	ASSERT(SUCCEEDED(hr) && pXML!=NULL);

	hr = pXML->load(varFileName,&varOkay);
//	hr = pXML->loadXML(bstrXML,&varOkay);

	IXMLDOMElement *pRoot;
	hr = pXML->get_documentElement(&pRoot);

	ResetContents();
	varOkay = LoadXML((IXMLDOMNode*)pRoot);

	return varOkay;
}

BOOL CXML::LoadXML(IXMLDOMNode *pNode)
{
	if(pNode==NULL) return FALSE;
	VARIANT vaData;
	VariantInit(&vaData);
	HRESULT hr;
	CComBSTR txt;
	hr = pNode->get_baseName(&txt);
	CString str = txt;
	CString name,data;
	SetTagName(str);
	// Now step through the children and load data;
	IXMLDOMNode *child;
	DOMNodeType type;
	CString lflf,crlf,lf;
	lf.Format("%c",10);
	lflf.Format("%c%c",10,10);
	crlf.Format("%c %c",10,10);
	// Get Text data
/*	hr = pNode->get_firstChild(&child);
	while(child!=NULL)
	{
		hr = child->get_nodeType(&type);
		if(type == NODE_TEXT)
		{
			child->get_baseName(&txt);
			child->get_nodeValue(&vaData);
			name = txt;
			data = vaData.bstrVal;
			if(data.GetLength()>1) AddToText(data);
		}
		else if(type == NODE_ELEMENT)
		{
			CXML *x;
			x = new CXML;
			x->SetTagName("MsxmlAdd");
			AddChild(x);
			x->LoadXML(child);
		}
		else if(type == NODE_ATTRIBUTE)
		{
			child->get_baseName(&txt);
			child->get_nodeValue(&vaData);
			name = txt;
			data = vaData.bstrVal;
			SetAttr(name,data);
		}
		hr = child->get_nextSibling(&child);
	}
*/
	int c;
	hr = pNode->get_text(&txt);
	if(SUCCEEDED(hr))
	{
		data = txt;
		if(data.GetLength()>0)
		{
			c=data.Replace(crlf,lf);
			c=data.Replace(lflf,lf);
			AddToText(UnXmlizeString(data));
		}
	}
	// Get Attributes
	IXMLDOMNamedNodeMap *AtMap;
	hr = pNode->get_attributes(&AtMap);
	long atpos,atlen;
	hr = AtMap->get_length(&atlen);
	IXMLDOMNode *item;
	for(atpos=0;atpos<atlen;atpos++)
	{
		hr = AtMap->get_item(atpos,&item);
		hr = item->get_baseName(&txt);
		name = txt;
		hr = item->get_text(&txt);
		data = txt;
		SetAttr(name,UnXmlizeString(data));
	}
	// Get Child Elements
	hr = pNode->get_firstChild(&child);
	while(SUCCEEDED(hr)&&child!=NULL)
	{
		hr = child->get_nodeType(&type);
		if(type == NODE_ELEMENT)
		{
			CXML *x;
			x = new CXML;
			x->SetTagName("MsxmlAdd");
			AddChild(x);
			x->LoadXML(child);
		}
/*		else if(type == NODE_ATTRIBUTE)
		{
			child->get_baseName(&txt);
			child->get_nodeValue(&vaData);
			name = txt;
			data = vaData.bstrVal;
			SetAttr(name,data);
		}
		else if(type == NODE_TEXT)
		{
			child->get_baseName(&txt);
			child->get_nodeValue(&vaData);
			name = txt;
			data = vaData.bstrVal;
			AddToText(data);
		}
		else
		{
			child->get_baseName(&txt);
			child->get_nodeValue(&vaData);
		}
*/
		hr = child->get_nextSibling(&child);
	}
	return TRUE;
}

BOOL CXML::SetParent(CXML* parent)
{
	// first if we have a parent remove ourselves from it's list
	if(m_pParent!=NULL)
	{
		m_pParent->RemoveChild(this);
	}
	m_pParent=parent;
	return TRUE;
}

CXML* CXML::GetParent()
{
	return m_pParent;
}

BOOL CXML::Load(CString FileName)
{
	CFile file;
	file.Open(FileName,CFile::modeRead);
	BOOL ret = XMLReadDOM(&file);
	file.Close();
	return ret;
}

CString CXML::XmlizeString(CString inStr)
{
	CString RetString = inStr;
	// Search for invalid characters
	RetString.Replace("&","&amp;");
	RetString.Replace("<","&lt;");
	RetString.Replace(">","&gt;");
	RetString.Replace("\'","&apos;");
	RetString.Replace("\"","&quot;");
	return RetString;
}

CString CXML::UnXmlizeString(CString inStr)
{
	CString RetString = inStr;
	RetString.Replace("&amp;","&");
	RetString.Replace("&lt;","<");
	RetString.Replace("&gt;",">");
	RetString.Replace("&apos;","\'");
	RetString.Replace("&quot","\"");
	return RetString;
}

