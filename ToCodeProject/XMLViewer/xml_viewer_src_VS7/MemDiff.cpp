// MemDiff.cpp: implementation of the CMemDiff class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrtDbg.h"
#include "MemDiff.h"

#ifdef _DEBUG
   #undef THIS_FILE
   static char THIS_FILE[]=__FILE__;
   #define new DEBUG_NEW
#endif

#ifdef _DEBUG

#ifndef _INC_CRTDBG

#error Missing include "Supports debugging features of the C runtime library."
/* don't forget add this includes to stdafx.h
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif

  OR

#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC
   #include <stdlib.h>
   #include <crtdbg.h>
#endif
*/

#endif



//////////////////////////////////////////////////////////////////////
// Class CMemDiff
//////////////////////////////////////////////////////////////////////


class CMemDiff : CMemoryState
{
// Construction/Destruction
public:
	CMemDiff();
	~CMemDiff();
};

#pragma warning (disable : 4073) // initializers put in library initialization area
#pragma init_seg(lib)

static CMemDiff gMemDiff;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemDiff::CMemDiff()
{
   TRACE(_T("\n--> Started track memory leaks\n\n"));

   // If you remove comment below you would warned about memory leak without freed.
   // afxMemDF = allocMemDF | delayFreeMemDF | checkAlwaysMemDF;

   AfxEnableMemoryTracking(TRUE);
   Checkpoint();
}

CMemDiff::~CMemDiff()
{
   CMemoryState oMemoryState;
   CMemoryState diffMemoryState;

   oMemoryState.Checkpoint();
   
   if(diffMemoryState.Difference((*this),oMemoryState))
   {
      Beep(1000,500);

      TRACE(_T("\n--> Detected memory leaks!\n"));
      diffMemoryState.DumpStatistics();

      // If you remove comment below you could tune output messages.
      // diffMemoryState.DumpAllObjectsSince();
      // TRACE(_T("--> Dump all objects still in heap\n"));
      // AfxDumpMemoryLeaks();
   }

   TRACE(_T("\n--> Finished track memory leaks\n\n"));
}

#endif
