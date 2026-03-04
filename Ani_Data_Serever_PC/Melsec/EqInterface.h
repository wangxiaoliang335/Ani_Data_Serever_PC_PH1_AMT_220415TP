// EqInterface.h: interface for the CEqInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EQINTERFACE_H__A3126498_8CCC_48DA_96B5_9EFDD864B69B__INCLUDED_)
#define AFX_EQINTERFACE_H__A3126498_8CCC_48DA_96B5_9EFDD864B69B__INCLUDED_

#include "MNetH.h"
#include "MNetHData.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEqInterface  
{
public:
	bool CheckEqInterface(); //130702 JSPark

	void CloseEqIf();
	BOOL InitEqIf();

	int m_iCIMLimitTime;

	CWnd* m_pWnd;

	MNetH* m_pMNetH;
	BOOL m_bEqAlive;
	BOOL m_bMasterAlive;
	CString m_curRecipe;

	int m_iJobCount;
	int m_iAliveCheckStartTime;
	BOOL m_bUnitInitFlag; //160303 JSLee
	CString m_strCurRcpId;
	BOOL m_bCall;
	CEqInterface(); //<< 20160814 kang
	virtual ~CEqInterface();

};

#endif // !defined(AFX_EQINTERFACE_H__A3126498_8CCC_48DA_96B5_9EFDD864B69B__INCLUDED_)
