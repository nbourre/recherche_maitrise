// (C) 1996 MAB Technologies
// 6521 Wrigley Way
// Fort Worth, TX 76133-5140
//
// CTextFile.cpp
//
// 24 Jan 2002 - Fixed Memory leak in m_pBuffer
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TextFile.h"

CTextFile::CTextFile()
{
	NoBlank=FALSE;
	m_line="";
	m_bufpos=-1;
	m_pEOF=FALSE;
	m_pBuffer=NULL;
}

CTextFile::~CTextFile()
{
	if(m_pBuffer!=NULL) delete []m_pBuffer;
}

CString CTextFile::NextNasLine()
{
	m_line="";
	if(m_bufpos<0)
	{
		m_SizeFile = (long)GetLength();
		m_pBuffer = new char[m_SizeFile];
		if(m_pBuffer == NULL)
		{
			m_pEOF=TRUE;
			m_bufpos=0;
			return m_line;
		}
		TRY
		{
			Read(m_pBuffer,m_SizeFile);
		}
		CATCH(CFileException, e)
		{
			m_pEOF=TRUE;
			m_bufpos=0;
			return m_line;
		}
		END_CATCH
	}
	while(m_bufpos<m_SizeFile)
	{
		m_bufpos++;
		if(m_pBuffer[m_bufpos]==10)
		{
			m_bufpos++;
			if(m_pBuffer[m_bufpos]==13)
			{
				FormatNasLine();
				return m_line;
			}
			m_bufpos--;
			FormatNasLine();
			return m_line;
		}
		if(m_pBuffer[m_bufpos]==13)
		{
			FormatNasLine();
			return m_line;
		}
		m_line+=m_pBuffer[m_bufpos];
	}
	FormatNasLine();
	m_pEOF=TRUE;
	return m_line;
}

CString CTextFile::NextLine(char delim)
{
Restart:
	m_line="";
	if(m_pEOF) return m_line;
	if(m_bufpos<0)
	{
		m_SizeFile = (long)GetLength();
		m_pBuffer = new char[m_SizeFile];
		if(m_pBuffer == NULL)
		{
			m_pEOF=TRUE;
			m_bufpos=0;
			return m_line;
		}
		TRY
		{
			Read(m_pBuffer,m_SizeFile);
		}
		CATCH(CFileException, e)
		{
			m_pEOF=TRUE;
			m_bufpos=0;
			return m_line;
		}
		END_CATCH
	}
	while(m_bufpos<m_SizeFile)
	{
		m_bufpos++;
		if(m_pBuffer[m_bufpos]==10)
		{
			m_bufpos++;
			if(m_pBuffer[m_bufpos]==13)
			{
				if(NoBlank) m_line.Trim();
				if(m_line.GetLength()<1) goto Restart;
				return m_line;
			}
			m_bufpos--;
			if(NoBlank) m_line.Trim();
			if(m_line.GetLength()<1) goto Restart;
			return m_line;
		}
		if(m_pBuffer[m_bufpos]==13)
		{
			m_bufpos++;
			if(m_pBuffer[m_bufpos]==10)
			{
				if(NoBlank) m_line.Trim();
				if(m_line.GetLength()<1) goto Restart;
				return m_line;
			}
			m_bufpos--;
			if(NoBlank) m_line.Trim();
			if(m_line.GetLength()<1) goto Restart;
			return m_line;
		}
		if(m_pBuffer[m_bufpos]==delim)
		{
			if(NoBlank) m_line.Trim();
			if(m_line.GetLength()<1) goto Restart;
			return m_line;
		}
		m_line+=m_pBuffer[m_bufpos];
	}
	m_pEOF=TRUE;
	if(NoBlank) m_line.Trim();
	return m_line;
}

BOOL CTextFile::isEOF()
{
	return m_pEOF;
}

void CTextFile::FormatNasLine()
{
	while(m_line.GetLength()<80) m_line=m_line+
		"                                        ";
	BOOL free,doub;
	// test for free format
	if(m_line.Find(',')>0) free=TRUE;
	else free=FALSE;
	// test for double field data
	if(m_line.Find('*')>0) doub=TRUE;
	else doub=FALSE;
	if(!free)
	{
		if(doub)
		{
		}
		else
		{
		m_fldStr[0]=m_line.Left(8);
		m_fldStr[1]=m_line.Mid(8,8);
		m_fldStr[2]=m_line.Mid(16,8);
		m_fldStr[3]=m_line.Mid(24,8);
		m_fldStr[4]=m_line.Mid(32,8);
		m_fldStr[5]=m_line.Mid(40,8);
		m_fldStr[6]=m_line.Mid(48,8);
		m_fldStr[7]=m_line.Mid(56,8);
		m_fldStr[8]=m_line.Mid(64,8);
		m_fldStr[9]=m_line.Mid(72,8);
		m_fldStr[9].TrimRight();
		}
	}
	else
		{

		}
}

CMabString CTextFile::fld(int pos=1)
{
	return m_fldStr[pos-1];
}

char CTextFile::NextChar()
{
	char c;
	m_bufpos++;
	if(m_bufpos>=m_SizeFile)
	{
		m_pEOF=TRUE;
		c=(char)0;
		return c;
	}
	return m_pBuffer[m_bufpos];
}

void CTextFile::Reset()
{
	m_pEOF=FALSE;
	if(m_pBuffer!=NULL) delete []m_pBuffer;
	m_pBuffer=NULL;
	m_bufpos=-1;
}

void CTextFile::SetNoBlank()
{
	NoBlank=TRUE;
}
