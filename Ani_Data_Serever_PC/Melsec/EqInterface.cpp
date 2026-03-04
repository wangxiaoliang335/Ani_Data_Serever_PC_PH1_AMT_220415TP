// EqInterface.cpp: implementation of the CEqInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EqInterface.h"
#include "TimeCheck.h"
#include "StringSupport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Check 시간 1초로 설정
CTimerCheck Alive_Check(1000); //130702 JSPark

CEqInterface::CEqInterface() //<< 20160814 kang
{
	m_pMNetH = NULL;
	m_bMasterAlive = FALSE; //151208 JSLee
	m_bUnitInitFlag = FALSE; //160303 JSLee
	m_iJobCount = 0;
	m_bEqAlive = FALSE;
	m_iCIMLimitTime = 4000;
	InitEqIf();
	m_bCall = FALSE;
	m_curRecipe = _T("");
}

CEqInterface::~CEqInterface()
{
	CloseEqIf();
}

BOOL CEqInterface::InitEqIf() //130630 JSPark
{
	m_pMNetH = new MNetH;

	ASSERT(m_pMNetH);

	return TRUE;
}

void CEqInterface::CloseEqIf() //151120 JSLee
{
	if(m_pMNetH != NULL)
	{
		delete m_pMNetH;
		m_pMNetH = NULL;
	}

}

