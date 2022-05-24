// Copyrite MABTECH 2002
// Portions copyrite as noted.
#if !defined(AFX_STRINGDEG_B__3A310B45_A6C3_11D1_B56D_00A024838B6B__INCLUDED_)
#define AFX_STRINGDEG_B__3A310B45_A6C3_11D1_B56D_00A024838B6B__INCLUDED_
// Portions COPYRITE 1995 MAB TECH ; 96 ,97 , 2000-2002
#include <cstringt.h>
// &&&&&&&& The following may be changed to user preference
#define DefStrLen  8	// Default string length to use for number conversion
#define DefExpChr 69	// Default Exponential character in scientific format
#include <cstringt.h>
class CMabString : public CString
{
public:
	void NoDoubleSpace();
	void NoControlChar();
	void Fill(int len, char c=' ');
	void UsCur();
	void operator=(double val);
	void operator=(float val);
	void operator=(int val);
	void operator=(long val);
	void Accuracy(int acc,int ExpAscii);
	// Code From
	// Copyright SOCABIM 1999
	// Pierre Caussin
	// Jean-Paul Retru
	static CString m_csNAN;
	static CString m_csNINF;
	static CString m_csPINF;
	//
	void TestExp();
	inline double atodn(char alpha[],int n);
	inline float atofn(char *alpha,int n);
	inline int atoin(char *alpha,int n);
	inline long int atoln(char alpha[],int n);
	inline void dtoan(char *alpha, double val, int len);
	inline void itoan(char alpha[],int n,int l);
	inline BOOL ltoan(char alpha[],long int n,int l);
	int GetInt();
	long GetLong();
	float GetFloat();
	double GetDouble();
	void PutLong(long i, int len=DefStrLen);
	void PutInt(int i, int len=DefStrLen);
	void PutFloat(float value, int len=DefStrLen, char ExpChar=DefExpChr);
	void PutDouble(double value, int len=DefStrLen, char ExpChar=DefExpChr);
	void PutFloatS(float value, int nsignificands=DefStrLen, char ExpChar=DefExpChr);
	void PutDoubleS(double value, int nsignificands=DefStrLen, char ExpChar=DefExpChr);
	BOOL IsInt();
	BOOL IsFloat();
	CMabString() : CString( ){};
	CMabString( const CString& stringSrc) : CString( stringSrc ){};
	CMabString( const CMabString& stringSrc) : CString( stringSrc ){};
	CMabString( TCHAR ch, int nRepeat = 1 ) : CString( ch, nRepeat ){};
	CMabString( LPCTSTR lpch, int nLength ) : CString( lpch, nLength ){};
	CMabString( const unsigned char* psz ) : CString( psz ){};
	CMabString( LPCWSTR lpsz ) : CString( lpsz ){};
	CMabString( LPCSTR lpsz ) : CString( lpsz ){};

	CMabString GetField( LPCTSTR delim, int fieldnum);
	CMabString GetField( TCHAR delim, int fieldnum);
	int GetFieldCount( LPCTSTR delim );
	int GetFieldCount( TCHAR delim );
	CMabString FirstField( TCHAR delim);
	CMabString NextField( TCHAR delim);

	CMabString GetDelimitedField( LPCTSTR delimStart, LPCTSTR delimEnd,
				int fieldnum = 0);
private:
	// for stepping through the fields
	LPTSTR m_pzhData;
    LPTSTR m_lpsz;
	LPTSTR m_lpszRemainder;
	LPTSTR m_lpszret;
};

#endif // !defined(AFX_STRINGDEG_B__3A310B45_A6C3_11D1_B56D_00A024838B6B__INCLUDED_)
