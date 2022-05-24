// Copyright MAB Tech 2002
// Portions copyright as noted.
// See Header File

/*
// Update History
23 Jan 2002 - Port to derive class from CString in MFC7
              Removed redundant code.
17 July 2000 - added stepping cability to field retrieval
prior items documented as comments in code.
*/

#include "StdAfx.h"
#include "MabString.h"
#include "math.h"
#include "float.h"

CString CMabString::m_csNAN("NaN");
CString CMabString::m_csNINF("-Inf");
CString CMabString::m_csPINF("+Inf");

// Base operator fucntions
void CMabString::operator=(double val)
{
	this->PutDouble(val);
}

void CMabString::operator=(float val)
{
	this->PutFloat(val);
}

void CMabString::operator=(int val)
{
	this->PutInt(val);
}

void CMabString::operator=(long val)
{
	this->PutLong(val);
}

// GetField 	- Gets a delimited field
// Returns	- A CMabString object that contains a copy of
//		  the specified field
// delim	- The delimiter string
// fieldnum 	- The field index - zero is the first
// NOTE 	- Returns the whole string for field zero
//		  if delim not found
//		  Returns empty string if # of delim found
//		  is less than fieldnum
CMabString CMabString::GetField( LPCTSTR delim, int fieldnum)
{
	LPTSTR lpsz, lpszRemainder, lpszret, pchData;
	pchData = GetBuffer(1);
	ReleaseBuffer(1);
	lpszRemainder = pchData;
	int retlen, lenDelim = lstrlen( delim );

	while( fieldnum-- >= 0 )
	{
		lpszret = lpszRemainder;
		lpsz = _tcsstr(lpszRemainder, delim);
		if( lpsz )
		{
			// We did find the delim
			retlen = (int)(lpsz - lpszRemainder);
			lpszRemainder = lpsz + lenDelim;
		}
		else
		{
			retlen = lstrlen( lpszRemainder );
			lpszRemainder += retlen;	// change to empty string
		}
	}
	return Mid( (int)(lpszret - pchData), retlen );
}

// GetField 	- Gets a delimited field
// Returns	- A CMabString object that contains a copy of
//		  the specified field
// delim	- The delimiter char
// fieldnum 	- The field index - zero is the first
// NOTE 	- Returns the whole string for field zero
//		  if delim not found
//		  Returns empty string if # of delim found
//		  is less than fieldnum
CMabString CMabString::GetField( TCHAR delim, int fieldnum)
{
	LPTSTR lpsz, lpszRemainder, lpszret, pchData;
	pchData = GetBuffer(1);
	ReleaseBuffer(1);
	lpszRemainder = pchData;
	int retlen;

	while( fieldnum-- >= 0 )
	{
		lpszret = lpszRemainder;
		lpsz = _tcschr(lpszRemainder, (_TUCHAR)delim);
		if( lpsz )
		{
			// We did find the delim
			retlen = (int)(lpsz - lpszRemainder);
			lpszRemainder = lpsz + 1;
		}
		else
		{
			retlen = lstrlen( lpszRemainder );
			lpszRemainder += retlen;	// change to empty string
		}
	}
	return Mid( (int)(lpszret - pchData), retlen );
}


// GetFieldCount	- Get number of fields in a string
// Returns		- The number of fields
//			  Is always greater than zero
// delim		- The delimiter character
int CMabString::GetFieldCount( TCHAR delim )
{
	TCHAR a[2];
	a[0] = delim;
	a[1] = 0;
	return GetFieldCount( a );
}


// GetFieldCount	- Get number of fields in a string
// Returns		- The number of fields
//			  Is always greater than zero
// delim		- The delimiter string
int CMabString::GetFieldCount( LPCTSTR delim )
{
	LPTSTR lpsz, lpszRemainder = GetBuffer(1);
	ReleaseBuffer(1);
	int lenDelim = lstrlen( delim );

	int iCount = 1;
	while( (lpsz = _tcsstr(lpszRemainder, delim)) != NULL )
	{
		lpszRemainder = lpsz + lenDelim;
		iCount++;
	}
	return iCount;
}


// GetDelimitedField	- Finds a field delimited on both ends
// Returns		- A CMabString object that contains a copy of
//			  the specified field
// delimStart		- Delimiter at the start of the field
// delimEnd 		- Delimiter at the end of the field
CMabString CMabString::GetDelimitedField( LPCTSTR delimStart, LPCTSTR delimEnd, int fieldnum /*= 0*/)
{
	LPTSTR lpsz, lpszEnd, lpszRet, lpszRemainder, pchData ;
	pchData = GetBuffer(1);
	ReleaseBuffer(1);
	lpszRemainder = pchData;
	int lenDelimStart = lstrlen( delimStart ), lenDelimEnd = lstrlen( delimEnd );

	while( fieldnum-- >= 0 )
	{
		lpsz = _tcsstr(lpszRemainder, delimStart);
		if( lpsz )
		{
			// We did find the Start delim
			lpszRet = lpszRemainder = lpsz + lenDelimStart;
			lpszEnd = _tcsstr(lpszRemainder, delimEnd);
			if( lpszEnd == NULL ) return"";
			lpszRemainder = lpsz + lenDelimEnd;
		}
		else return "";
	}
	return Mid( (int)(lpszRet - pchData), (int)(lpszEnd - lpszRet) );
}

// IsFLoat - test to see if a '.' exists (assumes you know it is a number)
// return - TRUE/FALSE
BOOL CMabString::IsFloat()
{
	if(!Find(".")) return FALSE;
	return TRUE;
}

// IsFLoat - test to see if a '.' does not exists (assumes you know it is a number)
// return - TRUE/FALSE
BOOL CMabString::IsInt()
{
	if(Find(".")) return FALSE;
	return TRUE;
}

// PutFloat - writes a floating point number into the test field with the specified length
// Returns nothing
// Previous contents are destroyed
void CMabString::PutFloat(float value, int len, char ExpChar)
{
	switch	(_fpclass(value))
	{
		case _FPCLASS_SNAN:
		case _FPCLASS_QNAN:
			Format("%s", m_csNAN); // "NaN"
			return;
		case _FPCLASS_NINF:
			Format("%s", m_csNINF);// "-Inf"
			return;
		case _FPCLASS_PINF:
			Format("%s", m_csPINF);// "+Inf"
			return;
	}
	if(!_finite(value))
	{
			Format("ERR");
			return;
	}
	PutDouble((double)value, len, ExpChar);
	// have we exceeded our acuracy?
	int acc = 8;
	if(len< acc+1) return;
	Accuracy(acc,(int)ExpChar);
}

void CMabString::Accuracy(int acc,int ExpAscii)
{
	int len = GetLength();
	int i=0,i1;
	if(GetAt(i)==35) return;
	while(GetAt(i)==48 || GetAt(i)==45 || GetAt(i)==46)
	{
		if (GetAt(i)==ExpAscii) return;
		i++;
		if (i>=len) break;
	}
	i1=i-1;
	if(acc+i1+1>len) i1=len-acc-1;
	while(i<=acc+i1)
	{
		if(GetAt(i)==ExpAscii) return;
		i++;
	}
	if(Find('.') > i-acc && Find('.') < i) i++;
	for(;i<len;i++)
	{
		if(GetAt(i)==ExpAscii) return;
		if(GetAt(i)!=46) SetAt(i,'0');
	}
}

// PutFloat - writes a floating point number into the test field with 
//       the specified length of significant digits rather than total length
// Returns nothing
// Previous contents are destroyed
void CMabString::PutFloatS(float value, int nsignificands, char ExpChar)
{
	// Code From
	// Copyright SOCABIM 1999
	// Pierre Caussin
	// Jean-Paul Retru
	switch	(_fpclass(value))
	{
		case _FPCLASS_SNAN:
		case _FPCLASS_QNAN:
			Format("%s", m_csNAN); // "NaN"
			return;
		case _FPCLASS_NINF:
			Format("%s", m_csNINF);// "-Inf"
			return;
		case _FPCLASS_PINF:
			Format("%s", m_csPINF);// "+Inf"
			return;
	}
	if(!_finite(value))
	{
			Format("ERR");
			return;
	}
	PutDoubleS((double) value, nsignificands, ExpChar);
	int acc = 8;
	if(nsignificands< acc+1) return;
	Accuracy(acc,(int)ExpChar);
	return;
}

// PutDoubleS - writes a double floating point number into the test field with 
//       the specified length of significant digits rather than total length
// Returns nothing
// Previous contents are destroyed
void CMabString::PutDoubleS(double value, int nsignificands, char ExpChar)
{
	Empty( );
	// Code From
	// Copyright SOCABIM 1999
	// Pierre Caussin
	// Jean-Paul Retru
	CString fmt;
	int nk;
	switch	(_fpclass(value))
		{
		case _FPCLASS_SNAN:
		case _FPCLASS_QNAN:
			Format("%s", m_csNAN); // "NaN"
			return;
		case _FPCLASS_NINF:
			Format("%s", m_csNINF);// "-Inf"
			return;
		case _FPCLASS_PINF:
			Format("%s", m_csPINF);// "+Inf"
			return;
		case _FPCLASS_NN:
		case _FPCLASS_PN:
			nk = (int)log10(fabs(value));
			if	(fabs(value)<1) nk--;
			if	(nk > nsignificands-1 || nk < -2)
				{
				fmt.Format("%%.%df%c%d",nsignificands-1,ExpChar,nk);
				Format(fmt,value/pow(10.,nk));
				}
			else
				{
				if	(nk == nsignificands-1 && nk>1)
					fmt.Format("%%.%df.",0);
				else
					fmt.Format("%%.%df",nsignificands>nk+1?nsignificands-nk-1:0);
				Format(fmt,value);
				}
			return;
		default:
			fmt.Format("%%.%df",nsignificands);
			Format(fmt,0.);
	}
	int acc = 16;
	Accuracy(acc,(int)ExpChar);
	return;
}

// PutDouble - writes a double floating point number into the test field with 
//       the specified total length
// Returns nothing
// Previous contents are destroyed
void CMabString::PutDouble(double value, int len, char ExpChar)
{
	Empty( );
	switch	(_fpclass(value))
	{
		case _FPCLASS_SNAN:
		case _FPCLASS_QNAN:
			Format("%s", m_csNAN); // "NaN"
			return;
		case _FPCLASS_NINF:
			Format("%s", m_csNINF);// "-Inf"
			return;
		case _FPCLASS_PINF:
			Format("%s", m_csPINF);// "+Inf"
			return;
	}
	if(!_finite(value))
	{
			Format("ERR");
			return;
	}
	dtoan(GetBuffer(len),value,len);
	ReleaseBuffer(len);
	// have we exceeded our acuracy?
	// This line replaces the "E" with the users exponent definition.
	if(Find('E')>0 && ExpChar!=69) SetAt(Find('E')-1,ExpChar);
	int acc = 16;
	if(len< acc+1) return;
	Accuracy(acc,(int)ExpChar);
}

// PutLong - writes a long integer number into the test field with 
//       the specified total length
// Returns nothing
// Previous contents are destroyed
void CMabString::PutLong(long i, int len)
{
	Empty( );
	if(i>=0)
	{
		if(ltoan(GetBuffer(len),i,len))
			ReleaseBuffer(len);
		else
		{
			ReleaseBuffer(len);
			Format("Error");
		}
	}
	else
	{
		ltoan(GetBuffer(len),abs(i),len);
		ReleaseBuffer(len);
		i=0;
		while(GetAt(i)==32) i++;
		if(i>0) SetAt(i-1,'-');
		else Format("Error");
	}
}

// PutInt - writes a integer number into the test field with 
//       the specified total length
// Returns nothing
// Previous contents are destroyed
void CMabString::PutInt(int i, int len)
{
	PutLong((long)i,len);
}

// GetFloat - read a floating point number from the field
// Returns float
// Previous contents are unchanged
float CMabString::GetFloat()
{
	TestExp();
	float f;
	f = atofn(GetBuffer(GetLength()),GetLength());
	return f;
}

// GetDouble - read a double floating point number from the field
// Returns double
// Previous contents are unchanged
double CMabString::GetDouble()
{
	if(GetLength()<1) return 0.;
	TestExp();
	double f;
	if(GetAt(0)==36)
		f = atodn(Right(GetLength()-1).GetBuffer(GetLength()),GetLength()-1);
	else
		f = atodn(GetBuffer(GetLength()),GetLength());

	return f;
}

// GetLong - read a long number from the field
// Returns long
// Previous contents are unchanged
long CMabString::GetLong()
{
	long f;
	f = atoln(GetBuffer(GetLength()),GetLength());
	return f;
}

// GetInt - read an integer number from the field
// Returns int
// Previous contents are unchanged
int CMabString::GetInt()
{
	return atoin(GetBuffer(GetLength()),GetLength());
}

// atodn - read a double floating point number from the field
// Returns double
// Previous contents are unchanged
inline double CMabString::atodn(char alpha[],int n)
{
	double val;
	char format[16];
	char string[32];
	sprintf(format,"%c%df",'%',n);
	if(n>32) n=32;
	strncpy(string,alpha,n);
	string[n]='\0';
	val=atof(string);
	return (val);
}

// atofn - read a floating point number from the field
// Returns float
// Previous contents are unchanged
inline float CMabString::atofn(char *alpha,int n)
{
	float val;
	char format[16];
	char string[32];
	sprintf(format,"%c%df",'%',n);
	if(n>32) n=32;
	strncpy(string,alpha,n);
	string[n]='\0';
	val=(float)0.;
	sscanf(string,format,&val);
	return (val);
}

// atoin - read an integer number from the field
// Returns int
// Previous contents are unchanged
inline int CMabString::atoin(char *alpha,int n)
{
	int val;
	char format[32];
	char string[32];
	sprintf(format,"%c%dd",'%',n);
	val=0;
	if(n>32)n=32;
	strncpy(string,alpha,n);
	string[n]='\0';
	sscanf(string,format,&val);
	return (val);
}

// atoln - read a long number from the field
// Returns long
// Previous contents are unchanged
inline long int CMabString::atoln(char alpha[],int n)
{
	long int val;
	char string[32];
	if(n>31) return(0);
	strncpy(string,alpha,n);
	string[n]='\0';
	val=atol(string);
	return (val);
}

// dtoan - write a double into a character field
// Returns nothing
// Previous contents are destroyed
inline void CMabString::dtoan(char *alpha,double val,int len)
{
	if(len<4)
	{
		AfxMessageBox("Minimum String Length is 4 for error handling!");
		return;
	}
	char format[16];
	int exp,i;
	unsigned int lenx;
	char *temp;
	// do we need to use exponential 
	if(val < 0.0) sprintf(format,"%9.2E",val);
	else          sprintf(format,"%9.2E",val);
	exp=(int)strlen(&format[6]);
	exp=abs(atoin(&format[6],exp));
	if( (val>=0 && exp+3 >len) || (val<0 && exp+4 >len) || (exp>2 && fabs(val)<1.0))
	{ // use E format 
		lenx=len+11;
		i=sizeof(char);
		temp=(char near *)calloc(lenx,i);
		if(val < 0.0) sprintf(format,"%c%d.%dE",'%',len-1,len-5);
		else          sprintf(format,"%c%d.%dE",'%',len,len-4);
		sprintf(temp,format,val);
		lenx=(int)strlen(temp);
		i=lenx-3;
		do {
			if(temp[i]=='0') strcpy(&temp[i],&temp[i+1]);
		} while(i<(int)strlen(temp) && temp[i]=='0');
		do	{
			i--;
		} while(i>0 && temp[i]!='E');
		if(temp[i]=='E' && temp[i+1]=='+') strcpy(&temp[i+1],&temp[i+2]);
		while (len < (int) strlen(temp) && i>3)
		{
			i--;
			strcpy(&temp[i],&temp[i+1]);
		}
		if((int)strlen(temp)>len) strcpy(alpha,"ERR");
		else strcpy(alpha,temp);
		free(temp);
	}
	else
	{ // use f format 
		if(val < 0.0f) sprintf(format,"%c%d.%df",'%',len-1,len-3-exp);
		else if (val< 0.) sprintf(format,"%c%d.%df",'%',len,len-3);
		else if (val< 1.) sprintf(format,"%c%d.%df",'%',len,len-2);
		else          sprintf(format,"%c%d.%df",'%',len,len-2-exp);
		sprintf(alpha,format,val);
	}
	return;
}

// itoan - write an integer character field
// Returns nothing
// Previous contents are destroyed
inline void CMabString::itoan(char alpha[],int n,int l)
{
	int val;
	int count;
	char item;
	for (count=1;count<=l;count++)
	{
		val=n-(n/10)*10;
		itoa(val,&item,10);
		alpha[l-count]=item;
		n=n/10;
	}
	count=0;
	while ((int)alpha[count]==48 && count<l-1)
	{
		alpha[count]=' ';
		count++;
	}
	return;
}

// ltoan - write a long into a character field
// Returns bool
// Previous contents are destroyed
inline BOOL CMabString::ltoan(char alpha[],long int n,int l)
{
	long int val;
	int count;
	char item;
	BOOL ret=FALSE;
	for (count=1;count<=l;count++)
	{
		val=n-(n/10)*10;
		ltoa(val,&item,10);
		alpha[l-count]=item;
		n=n/10;
	}
	if(n==0) ret=TRUE;
	count=0;
	while ((int)alpha[count]==48 && count<l-1)
	{
		alpha[count]=' ';
		count++;
	}
	return ret;
}

// TestExp - Cleanup the exponent in the scientific format string
// Returns nothing
// Previous contents are destroyed
void CMabString::TestExp()
{
	int pos;
	pos = max(ReverseFind('-'),ReverseFind('+'));
	if(pos<2) return;
	if(GetAt(pos-1)==32) return;
	if(GetAt(pos-1)==69 || GetAt(pos-1)==101) return;
	Insert(pos-1,'E');
}

// UsCur - formats the string to 2 sig digits.
// Returns nothing
// Previous contents are destroyed
void CMabString::UsCur()
{
	double d;
	d=GetDouble()+0.001;// round off?
	if(d<0.01)
	{
		Empty();
		return;
	}
	Format("%14.2f",d);
	Trim();
}

// Fill - fills the string with specifed number of single chars
// Returns nothing
// Previous contents are destroyed
void CMabString::Fill(int len, char c)
{
	int i;
	int oldlen = GetLength();
	char *buf;
	buf = GetBuffer(len+1);
	for(i=oldlen;i<len;i++) buf[i]=c;
	buf[i] = (char) 0;
	ReleaseBuffer(len+1);
}

// NoDoubleSpace - reomves all double spaces from the string
// Returns nothing
// Previous contents are modified
void CMabString::NoDoubleSpace()
{
	while(Find("  ")>=0)
		{
			Replace("  "," ");
		}
}

// NoControlChar - replaces all control chars with blanks
// Returns nothing
// Previous contents are modififed
void CMabString::NoControlChar()
{
	int i,j;
	j=GetLength();
	for(i=0;i<j;i++)
		{
		if(GetAt(i)<32) SetAt(i,' ');
		}
}

// &&&&&&&&&&&&&&&&&&&&&&&&&&&& 17 July 2000 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// The following two functions assist in reading large strings that are 
// really a collection of fields. The first field is read with the StepField
// functions that initalizes the reading. Subsequent fields are read with the 
// next field functions that uses the previous location to start from. This has
// given me an order of magnitude increase in reading some blocks. (Specifically
// VRML-200X files.
CMabString CMabString::FirstField(TCHAR delim)
{
	int retlen;
	m_pzhData = GetBuffer(1);// 22 Jan 2002
	ReleaseBuffer(1);
	m_lpszRemainder = m_pzhData; // 22 Jan 2002
	m_lpszret = m_lpszRemainder;
	m_lpsz = _tcschr(m_lpszRemainder, (_TUCHAR)delim);
	if( m_lpsz )
	{
		// We did find the delim
		retlen = (int)(m_lpsz - m_lpszRemainder);
		m_lpszRemainder = m_lpsz + 1;
	}
	else
	{
		retlen = lstrlen( m_lpszRemainder );
		m_lpszRemainder += retlen;	// change to empty string
	}
	return Mid((int)( m_lpszret - m_pzhData), retlen );
}

CMabString CMabString::NextField( TCHAR delim)
{
	int retlen;
	m_lpszret = m_lpszRemainder;
	m_lpsz = _tcschr(m_lpszRemainder, (_TUCHAR)delim);
	if( m_lpsz )
	{
		// We did find the delim
		retlen = (int)(m_lpsz - m_lpszRemainder);
		m_lpszRemainder = m_lpsz + 1;
	}
	else
	{
		retlen = lstrlen( m_lpszRemainder );
		m_lpszRemainder += retlen;	// change to empty string
	}
	return Mid( (int)(m_lpszret - m_pzhData), retlen );
}
