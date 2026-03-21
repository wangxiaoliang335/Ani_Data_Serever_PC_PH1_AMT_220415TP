// MNetH.cpp: implementation of the MNetH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MNetH.h"
#include "Mdfunc.h"
#include	<malloc.h>    //>>130803.kty

#include "ProfileDataIF.h" //151208 JSLee
#include "Ani_Data_Serever_PC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _MELSEC_SYNC
#define MLS_CS_MELSEC_LOCK				MLS_CS_MELSEC_LOCK
#define MLS_CS_MELSEC_UNLOCK			m_csIsInterlock.Unlock();
#else
#define MLS_CS_MELSEC_LOCK			
#define MLS_CS_MELSEC_UNLOCK		
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MNetH::MNetH(CString sIniFile)
{
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] Constructor Start, IniFile=%s"), sIniFile);

	//	Create Mutex
	//m_hMutex = ::CreateMutex(NULL, FALSE, NULL); //130629 JSPark
	//	Initial Variable for XNet
	m_nChannel = DEFAULT_CHANNEL;
	m_nStation = DEFAULT_STATION;
	//<<130717 kmh
	m_lNetwork = 0;
	m_lStation = DEFAULT_STATION;
	//<<

	m_lPath = 0;
	m_fActive = FALSE;
	m_sIniFile = sIniFile;

	m_nCurLocal = 1;
	m_nCurUnit = 1;
	m_nPrevLocal = m_nCurLocal-1;
	m_nNextLocal = m_nCurLocal+1;
	m_nToUpperEqQty = 1;
	m_nToLowerEqQty = 1;

	m_bSetGlassDataFlag = false;

	m_dwTransferStart = 0;
	//	Common Variable for XSECNetDlg
	m_pDlg = NULL;
	m_pMainWnd = NULL;
	m_bShow = false;
	m_bUseInterface = false; //151213 JSLee
	//	Reading configuration file
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] Calling ReadConfig()"));
	ReadConfig();
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] ReadConfig() Completed, m_nChannel=%d, m_nCurLocal=%d, m_lNetwork=%d"), m_nChannel, m_nCurLocal, m_lNetwork);
	//Prev Dialog Delete
	//if ( m_pDlg ){ m_pDlg->DestroyWindow(); delete m_pDlg; m_pDlg=NULL; }

	//if(m_bUseDialog)
	//{
	//	//Dialog Create
	//	m_pDlg = new MNetHDlg;
	//	m_pDlg->m_pMNetH = this;
	//	m_pDlg->Create(IDD_MNETH);

	//	//Dialog Visibled
	//	if( m_bShow ){ m_pDlg->ShowWindow(SW_SHOW);	}
	//	else { m_pDlg->ShowWindow(SW_HIDE); }
	//}

	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] Calling Start()"));
	Start();
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] Constructor End"));
}

MNetH::~MNetH()
{
	DestroyDlg();
	MelsecClose();
	//::CloseHandle(m_hMutex);
}

MNetHDlg** MNetH::GetMNetHDlg()
{
	return &m_pDlg;
}

void MNetH::DestroyDlg()
{
	/*if(m_pDlg)
	{
		m_pDlg->DestroyWindow();
		delete m_pDlg;
		m_pDlg = NULL;
	}*/
}

/*========================================================================================
	FUNCTION : MNetH::ViewVisible()
	DESCRIPT : Visible On/Off for dialog
	RETURN	 : none
	ARGUMENT :
		NONE
	UPDATE	 : 2004/08/12, KJH; First work!
========================================================================================*/
void MNetH::ViewVisible(bool bView)
{
	/*if(!m_bUseDialog){ return; }

	m_bShow = bView;
	if(bView) 
	{
		if(m_pDlg)
		{
			m_pDlg->ShowWindow(SW_SHOW);
			m_pDlg->SetFocus();
		}
	}
	else
	{
		if(m_pDlg) m_pDlg->ShowWindow(SW_HIDE);
	}*/
}

/*========================================================================================
	FUNCTION : MNetH::DecToHex()
	DESCRIPT : Dec to Hex
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/11/26, James Park; First work!
========================================================================================*/
long MNetH::DecToHex(long lDec, CString& sHex)
{
	long lRet=0;

	sHex = _T("");
	sHex.Format(_T("%X"), lDec);

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::DecToHex()
	DESCRIPT : Dec(String) to Hex
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/11/26, James Park; First work!
========================================================================================*/
long MNetH::DecToHex(CString sDec, CString& sHex)
{
	long lRet=0;
	long lValue=0;
	TCHAR   *ptr;
 
	sHex = _T("");

	lValue = _tcstol(sDec, &ptr, 10);
	sHex.Format(_T("%X"), lValue);

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::HexToDec()
	DESCRIPT : Hex to Dec
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/11/26, James Park; First work!
========================================================================================*/
long MNetH::HexToDec(CString sHex, long& lDec)
{
	long	lRet=0;
	TCHAR   *ptr;
	
	lDec = _tcstol(sHex, &ptr, 16);

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::HexToDec()
	DESCRIPT : Hex to Dec(String)
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/11/26, James Park; First work!
========================================================================================*/
long MNetH::HexToDec(CString sHex, CString& sDec)
{
	long	lRet=0;
	long	lValue;
	TCHAR   *ptr;
	
	lValue = _tcstol(sHex, &ptr, 16);
	sDec.Format(_T("%d"),lValue);

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::HexToBin()
	DESCRIPT : Hex to Binary string
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/11/18, James Park; First work!
========================================================================================*/
long MNetH::HexToBin(CString sHex, CString& sBin)
{
	long lRet=0;
	long	lValue;
	TCHAR *ptr;
	
	lValue = _tcstol(sHex, &ptr, 16);
	_ltot_s(lValue, sBin.GetBuffer(0), sBin.GetLength(), 2);

// 	long n;
// 	char   *str, *ptr;
// 	CString	sTmp;

// 	str = new char[2];
// 	memset(str, NULL, sizeof(char)*2);
// 	for(int i=0; i < sHex.GetLength(); i++)
// 	{
// 		memcpy(str, sHex.Mid(i,1), 1);
// 		n = strtol(str, &ptr, 16);
// 		for(int j=0; j < 4; j++)
// 		{
// 			itoa(n & 0x1, str, 10);
// 			sTmp = sTmp + str;
// 			n = n>>1;
// 		}
// 		sTmp.MakeReverse();
// 		sBin = sBin + sTmp;
// 		sTmp = _T("");
// 	}
// 
// 	delete [] str; str = NULL;

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::BinToHex()
	DESCRIPT : Binary to Hex string
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/11/18, James Park; First work!
========================================================================================*/
long MNetH::BinToHex(CString sBin, CString& sHex)
{
	long lRet=0;
	long lValue;
	TCHAR* ptr;

	lValue = _tcstol(sBin, &ptr, 2);
	sHex.Format(_T("%X"), lValue);

// 	long n;
// 	char   *str, *ptr;
// 	CString	sTmp;
// 
// 	str = new char[5];
// 	memset(str, NULL, sizeof(char)*5);
// 	for(int i=0; i < sBin.GetLength(); i=i+4)
// 	{
// 		memcpy(str, sBin.Mid(i,4), 4);
// 		n = strtol(str, &ptr, 2);
// 		sTmp.Format(_T("%X"), n);
// 		sHex = sHex + sTmp;
// 	}
// 	delete [] str; str = NULL;
	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::Start()
	DESCRIPT : Start for xnet
	RETURN	 : long
	ARGUMENT :
		None
	UPDATE	 : 2004/08/12, KJH; First work!
========================================================================================*/
long MNetH::Start()
{
	long	lRet;

	lRet = MelsecOpen();

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::Stop()
	DESCRIPT : Stop for xnet
	RETURN	 : long
	ARGUMENT :
		NONE
	UPDATE	 : 2004/08/12, KJH; First work!
========================================================================================*/
long MNetH::Stop()
{
	long	lRet;

	lRet = MelsecClose();

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::IsConnected()
	DESCRIPT : Connection State
	RETURN	 : TRUE/FALSE
	ARGUMENT :
		NONE
	UPDATE	 : 2004/08/12, KJH; First work!
========================================================================================*/
BOOL MNetH::IsConnected()
{
	return m_fActive;
}	

/*========================================================================================
	FUNCTION : MNetH::ReadLB()
	DESCRIPT : 
	RETURN	 : 
	ARGUMENT :
	UPDATE	 : 2005/07/04, KJH
========================================================================================*/
long MNetH::ReadLB(unsigned short nAddr, unsigned short nPoints, unsigned short *pnRBuf, unsigned short nBufSize)
{
	if(m_fActive == FALSE)
		return -1;

	unsigned short	nDev[4];
	unsigned short	nRet = 0;
	unsigned short	nCount = 0;
	unsigned short	nIndex = 0;
	unsigned short	nDigit = 0x0001;
	unsigned short	i = 0;

	nCount = nBufSize/sizeof(unsigned short);
	if (nCount < nPoints) { return -10001; }

	unsigned short	*pnBuf = new unsigned short[nCount];
	for (i=0; i<nCount; i++) { pnBuf[i] = 0; }

	nDev[0] = 1;
	nDev[1] = DevB;
	nDev[2] = nAddr;
	nDev[3] = nPoints;

	MLS_CS_MELSEC_LOCK;

#ifdef _USE_MELSEC_
	nRet = mdRandR(m_lPath, m_nStation, nDev, pnBuf, nBufSize);
#endif
	MLS_CS_MELSEC_UNLOCK;

	for(i=0; i<nCount; i++) 
	{
		pnRBuf[i] = (pnBuf[nIndex] & nDigit) ? 1 : 0;

		if(nDigit != 0x8000)
			nDigit <<= 1;
		else 
		{
			nDigit = 0x0001;
			nIndex++;
		}
	}

	delete [] pnBuf;

	return nRet;
}

/*
long MNetH::ReadLBEx(unsigned short nAddr, unsigned short nPoints, unsigned short *pnRBuf, unsigned short nBufSize)
{ //<<130717 kmh
	if(m_fActive == FALSE)
		return -1;
	
	unsigned short	nDev[4];
	unsigned short	nRet = 0;
	unsigned short	nCount = 0;
	unsigned short	nIndex = 0;
	unsigned short	nDigit = 0x0001;
	unsigned short	i = 0;
	
	nCount = nBufSize/sizeof(unsigned short);
	if (nCount < nPoints) { return -10001; }
	
	unsigned short	*pnBuf = new unsigned short[nCount];
	for (i=0; i<nCount; i++) { pnBuf[i] = 0; }
	
	nDev[0] = 1;
	nDev[1] = DevB;
	nDev[2] = nAddr;
	nDev[3] = nPoints;
	
	MLS_CS_MELSEC_LOCK
		
	
	nRet = mdRandREx(m_lPath,m_lNetwork, m_lStation, nDev, pnBuf, nBufSize);
	for(i=0; i<nCount; i++) 
	{
		pnRBuf[i] = (pnBuf[nIndex] & nDigit) ? 1 : 0;
		
		if(nDigit != 0x8000)
			nDigit <<= 1;
		else 
		{
			nDigit = 0x0001;
			nIndex++;
		}
	}
	
	delete [] pnBuf;
	MLS_CS_MELSEC_UNLOCK;
	
	return nRet;
}*/

/*========================================================================================
	FUNCTION : MNetH::ReadLW()
	DESCRIPT : 
	RETURN	 : 
	ARGUMENT :
	UPDATE	 : 2005/07/04, KJH
========================================================================================*/
long MNetH::ReadLW(unsigned short nAddr, unsigned short nPoints, unsigned short *pnRBuf, unsigned short nBufSize)
{
	if (m_fActive == FALSE) { return -1; }

	unsigned short	nDev[4];
	unsigned short	nRet = 0;
	unsigned short	nCount = 0;

	nCount = nBufSize / sizeof(unsigned short);
	if (nCount < nPoints) { return -10001; }

	nDev[0] = 1;
	nDev[1] = DevW;
	nDev[2] = nAddr;
	nDev[3] = nPoints;

	MLS_CS_MELSEC_LOCK
	
#ifdef _USE_MELSEC_
	nRet = mdRandR(m_lPath, m_nStation, nDev, pnRBuf, nBufSize);
#endif
	MLS_CS_MELSEC_UNLOCK;
	return nRet;
}	


//long MNetH::ReadLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, short *pnRBuf, long nBufSize)
long MNetH::ReadLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *pnRBuf, long nBufSize) //151117 JSLee short -> unsigned short
{
	if (m_fActive == FALSE) { return -1; }
	
	long	lDev[4];
	long	lRet = 0;
	long	lCount = 0;
	
	//lCount = nBufSize / sizeof(long);   //>>130718.kty
	lCount = nBufSize / sizeof(unsigned short); //151208 JSLee
	if (lCount < lPoints) { return -10001; }
	
	lDev[0] = 1;
	lDev[1] = DevW;
	lDev[2] = lAddr;
	lDev[3] = lPoints;
	
	MLS_CS_MELSEC_LOCK
		
	/*
	VC++	: ret = mdRandWEx(path,netno,stno,dev,buf,bufsize);
	
	  long	ret;	return value	OUT
	  long	path;	path of channel	IN
	  long	netno;	network number	IN
	  long	stno;	station number	IN
	  long	dev[ ];	random selected device	IN
	  short	buf[ ];	written data(single precision)	IN
	  long	bufsize;	dummy	IN
	*/
//	nRet = mdRandR(m_lPath, m_nStation, nDev, pnRBuf, nBufSize);


#ifdef _USE_MELSEC_
	if (m_lStation == m_nCurLocal) //<< CASE OF OWN Station 
		lRet = mdRandREx(m_lPath, 0, 255, lDev, pnRBuf, nBufSize);
	else
		lRet = mdRandREx(m_lPath, m_lNetwork, m_lStation, lDev, pnRBuf, nBufSize);
#endif

	//>>151207 JSLee
	//lRet = mdRandREx(m_lPath,m_lNetwork,m_lStation,lDev,pnRBuf,nBufSize);
	//lRet = mdRandREx(m_lPath,m_lNetwork,m_lStation,lDev,pnRBuf,NULL);
	//<<
	MLS_CS_MELSEC_UNLOCK
	
	return lRet;
}	

//>> 20160824 kang
long MNetH::ReadZREx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *pnRBuf, long nBufSize) 
{
	if (m_fActive == FALSE) { return -1; }

	long	lDev[4];
	long	lRet = 0;
	long	lCount = 0;

	//lCount = nBufSize / sizeof(long);   //>>130718.kty
	lCount = nBufSize / sizeof(unsigned short); //151208 JSLee
	if (lCount < lPoints) { return -10001; }

	lDev[0] = 1;
	lDev[1] = DevZR;
	lDev[2] = lAddr;
	lDev[3] = lPoints;

	/*
	VC++	: ret = mdRandWEx(path,netno,stno,dev,buf,bufsize);

	long	ret;	return value	OUT
	long	path;	path of channel	IN
	long	netno;	network number	IN
	long	stno;	station number	IN
	long	dev[ ];	random selected device	IN
	short	buf[ ];	written data(single precision)	IN
	long	bufsize;	dummy	IN
	*/
	//	nRet = mdRandR(m_lPath, m_nStation, nDev, pnRBuf, nBufSize);

#ifdef _USE_MELSEC_
	lRet = mdRandREx(m_lPath, m_lNetwork, m_lStation, lDev, pnRBuf, nBufSize);
#endif

	return lRet;
}


/*========================================================================================
	FUNCTION : MNetH::MakeString()
	DESCRIPT : 
	RETURN	 : 
	ARGUMENT :
	UPDATE	 : 2004/08/12, KJH; First work!
========================================================================================*/
CString MNetH::MakeString(unsigned short nValue, bool bSwap)
{
	CString		sRtnStr=_T("");
	TCHAR		szTemp[2] = {0, };
	int			nTemp = 0;

	if(nValue==0){
		if(bSwap)
			return _T("    ");
		else
			return _T("0000");
	}
	szTemp[1] = nValue / 0x100;		nTemp = szTemp[1];
	szTemp[0] = nValue - (nTemp * 0x100);
	if(bSwap==true){
		sRtnStr += szTemp[0]; 
		sRtnStr += szTemp[1];
	}
	else{
		sRtnStr.Format(_T("%02X%02X"), szTemp[1], szTemp[0]);
	}

	return sRtnStr;
}


/*========================================================================================
	FUNCTION : MNetH::MakeunsignedshortArray()
	DESCRIPT : 
	RETURN	 : 
	ARGUMENT :
	UPDATE	 : 2004/12/02, James Park
========================================================================================*/
long MNetH::MakeunsignedshortArray(CString sData, unsigned short *pnArray, unsigned short nLen, unsigned short nIndex, bool bSwap)
{
	TCHAR		*psTemp;
	int			nTemp1=0, nTemp2=0;
	int			nType=0, nQty=0;
	int			nStep=0;
	int			i;
	CString		sStr=_T("");

	// Check Data Length.
	if((nLen*2)<sData.GetLength()){ sStr = sData.Left(nLen*2);}
	else						  { sStr = sData; }

	psTemp = new TCHAR[(nLen*2)+1];
	memset(psTemp, NULL, sizeof(TCHAR)*((nLen*2)+1));
	if(bSwap)
		memset(psTemp, 0x20, sizeof(TCHAR)*((nLen*2)+1));
	_tcscpy_s(psTemp, sStr.GetLength(), sStr);
	if((nLen*2)>sStr.GetLength())
	{
		if(bSwap)
			psTemp[sStr.GetLength()] = 0x20;
	}

	nType=sStr.GetLength()%2; nQty=sStr.GetLength()/2;
	if(nType==0){
		for(i=0; i<nQty; i++){
			nTemp1 = psTemp[nStep]; nTemp2 = psTemp[nStep+1];
			if(bSwap==true){
				pnArray[nIndex++] = nTemp2 * 0x100 + nTemp1;
			}
			else{
				pnArray[nIndex++] = nTemp1 * 0x100 + nTemp2;
			}
			nStep += 2;
		}
	}
	else{
		for(i=0; i<nQty; i++){
			nTemp1 = psTemp[nStep]; nTemp2 = psTemp[nStep+1];
			if(bSwap==true){
				pnArray[nIndex++] = nTemp2 * 0x100 + nTemp1;
			}
			else{
				pnArray[nIndex++] = nTemp1 * 0x100 + nTemp2;
			}
			nStep += 2;
		}
		nTemp1 = psTemp[nStep]; nTemp2 = 0;
		if(bSwap==true){
			pnArray[nIndex++] = nTemp2 * 0x100 + nTemp1;
		}
		else{
			pnArray[nIndex++] = nTemp1 * 0x100 + nTemp2;
		}
	}
	delete [] psTemp; psTemp = NULL;
	return 0;
}

/*========================================================================================
	FUNCTION : MNetH::WriteLB()
	DESCRIPT : 
	RETURN	 : 
	ARGUMENT :
	UPDATE	 : 2005/07/04, KJH
========================================================================================*/
long MNetH::WriteLB(unsigned short nAddr, unsigned short nPoints, unsigned short *pnWBuf, unsigned short nBufSize)
{
	if (m_fActive == FALSE) { return -1; }

	unsigned short	nDev[4];
	unsigned short	nRet = 0;
	unsigned short	nCount = 0;
	unsigned short	nIndex = 0;
	unsigned short	nDigit = 0x0001;
	unsigned short	i = 0;

	nCount = nBufSize / sizeof(unsigned short);
	if (nCount < nPoints) { return -10001; }

	unsigned short	*pnBuf = new unsigned short[nCount];
	for (i = 0; i < nCount; i++) { pnBuf[i] = 0; }

	nDev[0] = 1;
	nDev[1] = DevB;
	nDev[2] = nAddr;
	nDev[3] = nPoints;

	for (i = 0; i < nCount; i++) {
		pnBuf[nIndex] = pnBuf[nIndex] | (pnWBuf[i] * nDigit);

		if (nDigit != 0x8000) { nDigit <<= 1; }
		else {
			nDigit = 0x0001;
			nIndex++;
		}
	}

	MLS_CS_MELSEC_LOCK;		

#ifdef _USE_MELSEC_
	nRet = mdRandW(m_lPath, m_nStation, nDev, pnBuf, nBufSize);
#endif
	MLS_CS_MELSEC_UNLOCK;

	delete [] pnBuf;

	return nRet;
}

/*========================================================================================
	FUNCTION : MNetH::WriteLW()
	DESCRIPT : 
	RETURN	 : 
	ARGUMENT :
	UPDATE	 : 2005/07/04, 
========================================================================================*/
long MNetH::WriteLW(unsigned short nAddr, unsigned short nPoints, unsigned short *pnWBuf, unsigned short nBufSize)
{
	if (m_fActive == FALSE) { return -1; }
	// TODO: Add your dispatch handler code here
	unsigned short	nDev[4];
	unsigned short	nRet = 0;
	unsigned short	nCount = 0;

	nCount = nBufSize / sizeof(unsigned short);
	if (nCount < nPoints) { return -10001; }

	nDev[0] = 1;
	nDev[1] = DevW;
	nDev[2] = nAddr;
	nDev[3] = nPoints;

	MLS_CS_MELSEC_LOCK;

#ifdef _USE_MELSEC_
	nRet = mdRandW(m_lPath, m_nStation, nDev, pnWBuf, nBufSize);
#endif
	MLS_CS_MELSEC_UNLOCK;

	return nRet;
}	

long MNetH::TrayWriteLW(unsigned short nAddr,unsigned short nPoints,unsigned short *pnWBuf, unsigned short nBufSize) //130705 kmh
{
	if (m_fActive == FALSE) { return -1; }
	// TODO: Add your dispatch handler code here
	unsigned short	nDev[4];
	unsigned short	nRet = 0;
	unsigned short	nCount = 0;
	
	nCount = nBufSize / sizeof(unsigned short);
	if (nCount < nPoints) { return -10001; }
	
	nDev[0] = 1;
	nDev[1] = DevW;
	nDev[2] = nAddr;
	nDev[3] = nPoints;
	
	MLS_CS_MELSEC_LOCK;
#ifdef _USE_MELSEC_
	nRet = mdRandW(m_lPath, m_nStation, nDev, pnWBuf, nBufSize);
#endif
	MLS_CS_MELSEC_UNLOCK;
	
	return nRet;
}

//long MNetH::WriteLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, short *nRBuf, long nBufSize)
long MNetH::WriteLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *nRBuf, long nBufSize) //151117 JSLee short -> unsigned short
{
	if (m_fActive == FALSE) { return -1; }
	// TODO: Add your dispatch handler code here
	long	lDev[4];
	long	lRet = 0;
	long	nCount = 0;   //>>130718.kty
	
	//nCount = nBufSize / sizeof(long);     //>>130718.kty
	nCount = nBufSize / sizeof(unsigned short);     //>>130718.kty
	if (nCount < lPoints) { return -10001; }
	
	lDev[0] = 1;
	lDev[1] = DevW;
	lDev[2] = lAddr;
	lDev[3] = lPoints;
	
	MLS_CS_MELSEC_LOCK;

#ifdef _USE_MELSEC_
	if (m_lStation == m_nCurLocal) //<< CASE OF OWN Station 
		lRet = mdRandWEx(m_lPath, 0, 255, lDev, nRBuf, NULL);
	else
		lRet = mdRandWEx(m_lPath, m_lNetwork, m_lStation, lDev, nRBuf, NULL);
#endif
//	lRet = mdRandREx(m_lPath,m_lNetwork,m_lStation,lDev,pnRBuf,nBufSize);
//	lRet = mdRandWEx(m_lPath,m_lNetwork, m_nStation, lDev, nRBuf, nBufSize);

	MLS_CS_MELSEC_UNLOCK;
	
	return lRet;
}

//>> 20160824 kang
long MNetH::WriteZREx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *nRBuf, long nBufSize)
{
	if (m_fActive == FALSE) { return -1; }
	// TODO: Add your dispatch handler code here
	long	lDev[4];
	long	lRet = 0;
	long	nCount = 0;   //>>130718.kty

	//nCount = nBufSize / sizeof(long);     //>>130718.kty
	nCount = nBufSize / sizeof(unsigned short);     //>>130718.kty
	if (nCount < lPoints) { return -10001; }

	lDev[0] = 1;
	lDev[1] = DevZR;
	lDev[2] = lAddr;
	lDev[3] = lPoints;

	MLS_CS_MELSEC_LOCK;		

#ifdef _USE_MELSEC_
	lRet = mdRandWEx(m_lPath, m_lNetwork, m_nStation, lDev, nRBuf, NULL);     //>>130830.kty
#endif
	MLS_CS_MELSEC_UNLOCK;

	return lRet;
}

/*========================================================================================
	FUNCTION : MNetH::MelsecClose()
	DESCRIPT : Close Melsec Comm
	RETURN	 : 
	ARGUMENT :
	FIRST	 : 2004/09/02, KJH
	LAST	 : 2004/09/02, KJH
========================================================================================*/
unsigned short MNetH::MelsecClose() 
{
	unsigned short nRet = 0;

	theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecClose] Start, m_fActive=%s, m_lPath=%d"), m_fActive ? _T("TRUE") : _T("FALSE"), m_lPath);

	if (m_fActive == FALSE) { 
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecClose] Already inactive, return SUCCESS"));
		return RV_SUCCESS; 
	}
#ifdef _USE_MELSEC_
	nRet = mdClose(m_lPath);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecClose] mdClose result=%d"), nRet);
#endif
	if (nRet == RV_SUCCESS) { 
		m_fActive = FALSE;
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecClose] SUCCESS, m_fActive=FALSE"));
	}
	return nRet;
}

/*========================================================================================
	FUNCTION : MNetH::MelsecOpen()
	DESCRIPT : Open Melsec Comm
	RETURN	 : 
	ARGUMENT :
		nChannel : 51 - first melsec board, 52 - second melsec board, ...
		nStation : 255 - own's station
	UPDATE	 : 2002/07/04, Inhyeok
			   2004/05/02, Hubri; Second work! (Change variable attribute from property)
========================================================================================*/
unsigned short MNetH::MelsecOpen() 
{
	unsigned short nRet = 0;

	theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecOpen] Start, Channel=%d, Station=%d"), m_nChannel, m_nStation);

	if (m_fActive != TRUE) 
	{
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecOpen] Not active, calling MelsecClose()"));
		MelsecClose(); 
	}

#ifdef _USE_MELSEC_
	nRet = mdOpen(m_nChannel, -1, &m_lPath);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecOpen] mdOpen result=%d, Path=%d"), nRet, m_lPath);
#endif
	if (nRet == RV_SUCCESS) { 
		m_fActive = TRUE;
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecOpen] Connection SUCCESS, m_fActive=TRUE"));
	}
	else {
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH::MelsecOpen] Connection FAILED, m_fActive=FALSE"));
	}
	return nRet;
}

int MNetH::ReadConfig() //151208 JSLee
{
	CProfileDataIF cDataIf;
	BOOL bflag;
	int	nListQty(0);
	CString str;
	CString strPath;

	//strPath.Format(_T("%s%s"), SYSTEM_DATA_PATH, _T("MNetH.ini"));
	cDataIf.SetFilePath(m_sIniFile);

	CString strNode(_T("")), strKey(_T(""));

	// ????????????????
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH ReadConfig] ConfigFile=%s"), m_sIniFile);

	// Basic information
	strNode.Format(_T("BASIC"));
	// Chanel
	m_nChannel = cDataIf.GetInt(strNode, _T("Chanel"), 151);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] Chanel=%d"), m_nChannel);

	// Local
	m_nCurLocal = cDataIf.GetInt(strNode, _T("Local"), 3);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] Local=%d"), m_nCurLocal);

	// Network
	m_lNetwork = cDataIf.GetInt(strNode, _T("Network"), 0);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] Network=%d"), m_lNetwork);

	// Unit
	m_nCurUnit = cDataIf.GetInt(strNode, _T("Unit"), 1);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] Unit=%d"), m_nCurUnit);

	// Previous Local
	m_nPrevLocal = cDataIf.GetInt(strNode, _T("PrevLocal"), 0);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] PrevLocal=%d"), m_nPrevLocal);

	// Next Local
	m_nNextLocal = cDataIf.GetInt(strNode, _T("NextLocal"), 0);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] NextLocal=%d"), m_nNextLocal);

	// UseDialog
	bflag = cDataIf.GetInt(strNode, _T("UseDialog"), TRUE);
	if (bflag){ m_bUseDialog = true; }
	else { m_bUseDialog = false; }
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] UseDialog=%s"), bflag ? _T("TRUE") : _T("FALSE"));

	// ShowDialog
	bflag = cDataIf.GetInt(strNode, _T("ShowDialog"), TRUE);
	if (bflag){ m_bShow = true; }
	else { m_bShow = false; }
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] ShowDialog=%s"), bflag ? _T("TRUE") : _T("FALSE"));

	//>> 151213 JSLee
	// UseInterface
	bflag = cDataIf.GetInt(strNode, _T("UseInterface"), FALSE);
	if (bflag){ m_bUseInterface = true; }
	else { m_bUseInterface = false; }
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [BASIC] UseInterface=%s"), bflag ? _T("TRUE") : _T("FALSE"));
	//<<

	//////////////////////////////////////////////////////////////////////////////////////////
	// Local Name List
	strNode.Format(_T("LOCAL_TITLE"));

	// Name Count.
	nListQty = cDataIf.GetInt(strNode, _T("Local_Qty"), 1);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [LOCAL_TITLE] Local_Qty=%d"), nListQty);

	for (int i = 0; i < nListQty; i++)
	{
		// Protocol
		str.Format(_T("Local_%02d"), i + 1);
		m_asLocalName.Add(cDataIf.GetString(strNode, str, _T("")));
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [LOCAL_TITLE] %s=%s"), str, m_asLocalName[i]);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Unit Name List
	strNode.Format(_T("UNIT_TITLE"));

	// Name Count.
	nListQty = cDataIf.GetInt(strNode, _T("Unit_Qty"), 1);
	theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [UNIT_TITLE] Unit_Qty=%d"), nListQty);

	for (int i = 0; i < nListQty; i++)
	{
		// Protocol
		str.Format(_T("Unit_%02d"), i + 1);
		m_asUnitName.Add(cDataIf.GetString(strNode, str, _T("")));
		theApp.m_PlcLog->LOG_INFO(_T("[MNetH] [UNIT_TITLE] %s=%s"), str, m_asUnitName[i]);
	}

	theApp.m_PlcLog->LOG_INFO(_T("[MNetH ReadConfig] Completed"));
	return 0;
}


// Add Function by cha 2006/02/07 
/*========================================================================================
	FUNCTION : MNetH::AscToString()
	DESCRIPT : Ascii?? String?? ?????? ???.
	RETURN	 : None
	ARGUMENT : 
	UPDATE	 : 2005/07/07, Hubri; First work!
========================================================================================*/
void MNetH::AscToString(TCHAR *pszOut, unsigned short *pnaBuf, unsigned short nPoints)
{
	unsigned short	i = 0;

	for (i = 0; i < nPoints; i++) {
		pszOut[(i * 2) + 1] = pnaBuf[i] / 0x0100;
		pszOut[(i * 2) + 0] = pnaBuf[i] - ((pnaBuf[i] / 0x0100) * 0x0100);
	}
	pszOut[(i * 2) + 0] = '\0';
}
void MNetH::AscToString(char *pszOut, unsigned short *pnaBuf, unsigned short nPoints) //151207 JSLee
{
	unsigned short	i = 0;

	for (i = 0; i < nPoints; i++) {
		pszOut[(i * 2) + 1] = pnaBuf[i] / 0x0100;
		pszOut[(i * 2) + 0] = pnaBuf[i] - ((pnaBuf[i] / 0x0100) * 0x0100);
	}
	pszOut[(i * 2) + 0] = '\0';
}

// Add Function by cha 2006/02/07 
/*========================================================================================
	FUNCTION: StringToAsc()
	DESCRIPT: String?? Ascii?? ?????? ???.
	RETURN	: void
	ARGUMENT: 
	UPDATE	: 2005/6/22, KyeongWhan; First work!
========================================================================================*/
void MNetH::StringToAsc(TCHAR *pszIn,unsigned short *pnabuf, unsigned short nPoints)
{
#ifdef _UNICODE
	int len = _tcslen(pszIn);
	char* szIn = new char[len];
	size_t num;
	wcstombs_s(&num, szIn, len, pszIn, len);
	for(int i=0; i<nPoints; i++)
		pnabuf[i] = MAKEWORD(szIn[2*i], szIn[(2*i)+1]);

	delete [] szIn;
#else
	for(int i=0; i<nPoints; i++)
		pnabuf[i] = MAKEWORD(pszIn[2*i], pszIn[(2*i)+1]);
#endif

}
void MNetH::StringToAsc(char *pszIn, unsigned short *pnabuf, unsigned short nPoints)
{
	for (int i = 0; i < nPoints; i++)
		pnabuf[i] = MAKEWORD(pszIn[2 * i], pszIn[(2 * i) + 1]);
}

void MNetH::NewStringToAsc(TCHAR *pszIn, short *pnabuf, long lPoints)
{
#ifdef _UNICODE
	int len = _tcslen(pszIn);
	char* szIn = new char[len];
	size_t num;
	wcstombs_s(&num, szIn, len, pszIn, len);
	for(int i=0; i<lPoints; i++)
		pnabuf[i] = MAKEWORD(szIn[2*i], szIn[(2*i)+1]);
	
	delete [] szIn;
#else
	for(int i=0; i<lPoints; i++)
		pnabuf[i] = MAKEWORD(pszIn[2*i], pszIn[(2*i)+1]);
#endif

}

// Add Function by cha 2006/02/07 
/*========================================================================================
	FUNCTION : FillSpace()
	DESCRIPT : Make the space filled ASCII item.
	RETURN	 : None
	ARGUMENT :
		pszIn  	: (i)Input string
		nLen	: (i)Total output length
	UPDATE	 : 2002/09/13, Wontae; First work!
			   2004/07/03, Hubri; 2nd work!
			   2005/01/27, Hubri; 3rd work!
			   2006/02/15, cha;   4th work!
========================================================================================*/
void MNetH::FillSpace(TCHAR *pszIn, unsigned short nStr)
{
 	unsigned short	nLen = _tcslen(pszIn) + 1;

	TCHAR *pszDm = new TCHAR[nLen];

	memset(pszDm, 0, sizeof(TCHAR) * nLen);
 	if (nLen != 0) { _tcsncpy_s(pszDm, nLen, pszIn, nLen); }

	for (int i = 0; i < nStr; i++)
	{
		pszIn[i] = ' ';
	}
// 	memset(pszIn, ' ', sizeof(TCHAR) * nStr);
//	pszIn[nStr + 1] = '\0';
// 	*(pszIn + nStr) = '\0';
	if (nLen == 0) { 
		delete [] pszDm;	pszDm = NULL; 
		return; 
	}
	if (nStr == 0) { 
		delete [] pszDm;	pszDm = NULL;
		return; 
	}
	if (nLen > nStr) { nLen = nStr; }

 	_tcsncpy_s(pszIn, nLen, pszDm, nLen);	

	delete [] pszDm;	pszDm = NULL;
}

void MNetH::FillSpace(char *pszIn, unsigned short nStr) //151207 JSLee
{
	char *pszDm = new char[strlen(pszIn) + 1];
	unsigned short	nLen = strlen(pszIn);

	memset(pszDm, 0, sizeof(char)* (nLen + 1));
	if (nLen != 0) { strncpy_s(pszDm, nLen, pszIn, nLen); }

	memset(pszIn, ' ', nStr);	*(pszIn + nStr) = '\0';
	if (nLen == 0) {
		delete[] pszDm;	pszDm = NULL;
		return;
	}
	if (nStr == 0) {
		delete[] pszDm;	pszDm = NULL;
		return;
	}
	if (nLen > nStr) { nLen = nStr; }

	strncpy_s(pszIn, nLen, pszDm, nLen);
	delete[] pszDm;	pszDm = NULL;
}

/*========================================================================================
	FUNCTION : TrimSpace()
	DESCRIPT : Trim the space ASCII item.
	RETURN	 : None
	ARGUMENT :
		pszIn  	: (i)Input string
	UPDATE	 : 2004/07/03, Hubri; First work!
			   2005/01/27, Hubri; 2nd work!
========================================================================================*/
void MNetH::TrimSpace(TCHAR *pszIn)
{
	unsigned short	nLen = _tcslen(pszIn)+1;
	TCHAR *pszDm = new TCHAR[nLen];

	unsigned short	nStr = 0;
	unsigned short	nEnd = 0;
	unsigned short	nCnt = 0;
	unsigned short	i = 0;

	memset(pszDm, 0, sizeof(TCHAR) * nLen);
	if (nLen == 0) {
		delete [] pszDm;	pszDm = NULL;
		return;
	}

	_tcsncpy_s(pszDm, nLen, pszIn, nLen);

	memset(pszIn, 0, sizeof(TCHAR) * nLen);
	for (i = 0; i < nLen; i++) {	//	Trim-Left
		if (*(pszDm + i) != ' ') {
			nStr = i;
			break;
		}
	}
	for (i = 0; i < nLen; i++) {	//	Trim-Right
		if (*(pszDm + (nLen - (i + 1))) != ' ') {
			nEnd = nLen - i;
			break;
		}
	}
	nCnt = nEnd - nStr;
	if (nCnt == 0) { 
		delete [] pszDm;	pszDm = NULL;
		return; 
	}
	_tcsncpy_s(pszIn, nCnt, pszDm + nStr, nCnt);
	delete [] pszDm;	pszDm = NULL;
}
void MNetH::TrimSpace(char *pszIn) //151207 JSLee
{
	char *pszDm = new char[strlen(pszIn) + 1];
	unsigned short	nLen = strlen(pszIn);
	unsigned short	nStr = 0;
	unsigned short	nEnd = 0;
	unsigned short	nCnt = 0;
	unsigned short	i = 0;

	memset(pszDm, 0, sizeof(char)* (nLen + 1));
	if (nLen == 0) {
		delete[] pszDm;	pszDm = NULL;
		return;
	}

	strncpy_s(pszDm, nLen, pszIn, nLen);

	memset(pszIn, 0, sizeof(char)* (nLen + 1));
	for (i = 0; i < nLen; i++) {	//	Trim-Left
		if (*(pszDm + i) != ' ') {
			nStr = i;
			break;
		}
	}
	for (i = 0; i < nLen; i++) {	//	Trim-Right
		if (*(pszDm + (nLen - (i + 1))) != ' ') {
			nEnd = nLen - i;
			break;
		}
	}
	nCnt = nEnd - nStr;
	if (nCnt == 0) {
		delete[] pszDm;	pszDm = NULL;
		return;
	}
	strncpy_s(pszIn, nCnt, pszDm + nStr, nCnt);
	delete[] pszDm;	pszDm = NULL;
}

/*========================================================================================
FUNCTION : MNetH::GetPLCAddressBit()
DESCRIPT :
RETURN	 :
ARGUMENT :
FIRST	 : 2015/11/16, JSLee
LAST	 : 
========================================================================================*/
void MNetH::GetPLCAddressBit(int nLocal, int nType, unsigned short *pnAddr) //151116 JSLee
{
	unsigned short	nRet = 0;
	
	if(nLocal == -1)
		nLocal = m_nCurLocal;
	nLocal = nLocal - eLocal2;
	if (nLocal < 0)
		nLocal = 0;

	switch (nType)
	{
		//////////////////////////////////////////// PLC > PC Bit ////////////////////////////////////////////
		case eBitType_PlcHearbit:							*pnAddr = LOCAL_BIT_L2M_PLC_HEARTBIT; break;
		case eBitType_PlcStartStatus:						*pnAddr = LOCAL_BIT_L2M_PLC_START_STATUS_ADDR; break;
		case eBitType_ModelStart:							*pnAddr = LOCAL_BIT_L2M_MODEL_START; break;

		case eBitType_AlarmStart:							*pnAddr = LOCAL_BIT_L2M_ALARM_START; break;
		case eBitType_AlarmReset:							*pnAddr = LOCAL_BIT_L2M_ALARM_RESET; break;

		case eBitType_OperateStart:							*pnAddr = LOCAL_BIT_L2M_OPERATION_START; break;
		case eBitType_AllPassModeStart:						*pnAddr = LOCAL_BIT_L2M_ALL_PASS_MODE_START; break;

		case eBitType_AxisStart:							*pnAddr = LOCAL_BIT_L2M_AXIS_START; break;

		case eBitType_Align1TStart1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_1_T_START1; break;
		case eBitType_Align2TStart1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_2_T_START1; break;

		case eBitType_Align1TStart2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_1_T_START2; break;
		case eBitType_Align2TStart2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_2_T_START2; break;

		case eBitType_Align1XyStart1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_1_XY_START1; break;
		case eBitType_Align2XyStart1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_2_XY_START1; break;

		case eBitType_Align1XyStart2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_1_XY_START2; break;
		case eBitType_Align2XyStart2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_2_XY_START2; break;

		case eBitType_Align1LightOn1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_1_LIGHT_ON1; break;
		case eBitType_Align2LightOn1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_2_LIGHT_ON1; break;

		case eBitType_Align1LightOn2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_1_LIGHT_ON2; break;
		case eBitType_Align2LightOn2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_2_LIGHT_ON2; break;

		case eBitType_Align1DataPlcSend1:					*pnAddr = LOCAL_BIT_L2M_ALIGN_1_PLC_SEND1; break;
		case eBitType_Align2DataPlcSend1:					*pnAddr = LOCAL_BIT_L2M_ALIGN_2_PLC_SEND1; break;

		case eBitType_Align1DataPlcSend2:					*pnAddr = LOCAL_BIT_L2M_ALIGN_1_PLC_SEND2; break;
		case eBitType_Align2DataPlcSend2:					*pnAddr = LOCAL_BIT_L2M_ALIGN_2_PLC_SEND2; break;

		case eBitType_Align1DataPlcReceived1:				*pnAddr = LOCAL_BIT_L2M_ALIGN_1_PLC_RECEIVED1; break;
		case eBitType_Align2DataPlcReceived1:				*pnAddr = LOCAL_BIT_L2M_ALIGN_2_PLC_RECEIVED1; break;

		case eBitType_Align1DataPlcReceived2:				*pnAddr = LOCAL_BIT_L2M_ALIGN_1_PLC_RECEIVED2; break;
		case eBitType_Align2DataPlcReceived2:				*pnAddr = LOCAL_BIT_L2M_ALIGN_2_PLC_RECEIVED2; break;

		case eBitType_TrayCheckStart1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_START1; break;
		case eBitType_TrayCheckStart2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_START2; break;
		case eBitType_TrayCheckStart3:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_START3; break;

		case eBitType_TrayCheckLightOn1:					*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_LIGHT_ON1; break;
		case eBitType_TrayCheckLightOn2:					*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_LIGHT_ON2; break;
		case eBitType_TrayCheckLightOn3:					*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_LIGHT_ON3; break;

		case eBitType_TrayLowerAlignStart1:					*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_START1; break;
		case eBitType_TrayLowerAlignStart2:					*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_START2; break;

		case eBitType_TrayLowerAlignLightOn1:				*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_LIGHT_ON1; break;
		case eBitType_TrayLowerAlignLightOn2:				*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_LIGHT_ON2; break;

		case eBitType_TrayLowerAlignDataSend1:				*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_PLC_SEND1; break;
		case eBitType_TrayLowerAlignDataSend2:				*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_PLC_SEND2; break;

		case eBitType_TrayLowerAlignDataReceived1:			*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_PLC_RECEIVED1; break;
		case eBitType_TrayLowerAlignDataReceived2:			*pnAddr = LOCAL_BIT_L2M_STAGE_ALIGN_PLC_RECEIVED2; break;

		case eBitType_TrayAlignStart1:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_START1; break;
		case eBitType_TrayAlignStart2:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_START2; break;
		case eBitType_TrayAlignStart3:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_START3; break;

		case eBitType_TrayAlignLightNo1:					*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_LIGHT_ON1; break;
		case eBitType_TrayAlignLightNo2:					*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_LIGHT_ON2; break;
		case eBitType_TrayAlignLightNo3:					*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_LIGHT_ON3; break;
	
#if _SYSTEM_AMTAFT_
		case eBitType_CurrentIndexZone:						*pnAddr = LOCAL_BIT_L2M_CURRENT_INDEX_ZONE; break;

		case eBitType_DataReportStart1:						*pnAddr = LOCAL_BIT_L2M_CH_1_DATA_REPORT_START; break;
		case eBitType_DataReportStart2:						*pnAddr = LOCAL_BIT_L2M_CH_2_DATA_REPORT_START; break;
		case eBitType_DataReportStart3:						*pnAddr = LOCAL_BIT_L2M_CH_3_DATA_REPORT_START; break;
		case eBitType_DataReportStart4:						*pnAddr = LOCAL_BIT_L2M_CH_4_DATA_REPORT_START; break;

		case eBitType_TrayReportStart1:						*pnAddr = LOCAL_BIT_L2M_CH_1_TRAY_REPORT_START; break;
		case eBitType_TrayReportStart2:						*pnAddr = LOCAL_BIT_L2M_CH_2_TRAY_REPORT_START; break;

		case eBitType_GoodReportStart:						*pnAddr = LOCAL_BIT_L2M_GOOD_REPORT_START; break;
		case eBitType_NgReportStart:						*pnAddr = LOCAL_BIT_L2M_NG_REPORT_START; break;
		case eBitType_SampleReportStart:					*pnAddr = LOCAL_BIT_L2M_SAMPLE_REPORT_START; break;
		case eBitType_BufferReportStart:					*pnAddr = LOCAL_BIT_L2M_BUFFER_REPORT_START; break;

		case eBitType_AZoneContactOnStart:					*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_ON_START; break;
		case eBitType_BZoneContactOnStart:					*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_ON_START; break;
		case eBitType_CZoneContactOnStart:					*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_ON_START; break;
		case eBitType_DZoneContactOnStart:					*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_ON_START; break;

		case eBitType_AZoneContactOffStart:					*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_OFF_START; break;
		case eBitType_BZoneContactOffStart:					*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_OFF_START; break;
		case eBitType_CZoneContactOffStart:					*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_OFF_START; break;
		case eBitType_DZoneContactOffStart:					*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_OFF_START; break;

		case eBitType_AZoneTouchInspectionStart:			*pnAddr = LOCAL_BIT_L2M_AZONE_TOUCH_INSPECTION_START; break;
		case eBitType_BZoneTouchInspectionStart:			*pnAddr = LOCAL_BIT_L2M_BZONE_TOUCH_INSPECTION_START; break;
		case eBitType_CZoneTouchInspectionStart:			*pnAddr = LOCAL_BIT_L2M_CZONE_TOUCH_INSPECTION_START; break;
		case eBitType_DZoneTouchInspectionStart:			*pnAddr = LOCAL_BIT_L2M_DZONE_TOUCH_INSPECTION_START; break;

		case eBitType_VisionPlcSend:						*pnAddr = LOCAL_BIT_L2M_VISION_PLC_SEND; break;
		case eBitType_VisionPlcReceiver:					*pnAddr = LOCAL_BIT_L2M_VISION_PLC_RECEIVER; break;
		case eBitType_VisionStart1:							*pnAddr = LOCAL_BIT_L2M_VISION_START1; break;
		case eBitType_VisionStart2:							*pnAddr = LOCAL_BIT_L2M_VISION_START2; break;
		case eBitType_VisionStart3:							*pnAddr = LOCAL_BIT_L2M_VISION_START3; break;
		case eBitType_VisionStart4:							*pnAddr = LOCAL_BIT_L2M_VISION_START4; break;

		case eBitType_ViewingAnglePlcSend:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_PLC_SEND; break;
		case eBitType_ViewingAnglePlcReceiver:				*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_PLC_RECEIVER; break;
		case eBitType_ViewingAngleStart1:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_START1; break;
		case eBitType_ViewingAngleStart2:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_START2; break;
		case eBitType_ViewingAngleStart3:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_START3; break;
		case eBitType_ViewingAngleStart4:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_START4; break;

		case eBitType_AZoneTouchPlcSend:					*pnAddr = LOCAL_BIT_L2M_AZONE_TOUCH_PLC_SEND; break;
		case eBitType_BZoneTouchPlcSend:					*pnAddr = LOCAL_BIT_L2M_BZONE_TOUCH_PLC_SEND; break;
		case eBitType_CZoneTouchPlcSend:					*pnAddr = LOCAL_BIT_L2M_CZONE_TOUCH_PLC_SEND; break;
		case eBitType_DZoneTouchPlcSend:					*pnAddr = LOCAL_BIT_L2M_DZONE_TOUCH_PLC_SEND; break;

		case eBitType_AZoneTouchPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_AZONE_TOUCH_PLC_RECEIVER; break;
		case eBitType_BZoneTouchPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_BZONE_TOUCH_PLC_RECEIVER; break;
		case eBitType_CZoneTouchPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_CZONE_TOUCH_PLC_RECEIVER; break;
		case eBitType_DZoneTouchPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_DZONE_TOUCH_PLC_RECEIVER; break;

		case eBitType_PreGammaPlcSend:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_PLC_SEND; break;
		case eBitType_PreGammaPlcReceiver:					*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_PLC_RECEIVER; break;

		case eBitType_AZoneContactPlcSend:					*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_PLC_SEND; break;
		case eBitType_BZoneContactPlcSend:					*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_PLC_SEND; break;
		case eBitType_CZoneContactPlcSend:					*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_PLC_SEND; break;
		case eBitType_DZoneContactPlcSend:					*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_PLC_SEND; break;

		case eBitType_AZoneContactPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_PLC_RECEIVER; break;
		case eBitType_BZoneContactPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_PLC_RECEIVER; break;
		case eBitType_CZoneContactPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_PLC_RECEIVER; break;		
		case eBitType_DZoneContactPlcReceiver:				*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_PLC_RECEIVER; break;

		case eBitType_PreGammaStart1:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_START1; break;
		case eBitType_PreGammaStart2:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_START2; break;
		case eBitType_PreGammaStart3:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_START3; break;
		case eBitType_PreGammaStart4:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_START4; break;

		case eBitType_UnloadJobDataStart1:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_JOB_DATA_START1; break;
		case eBitType_UnloadJobDataStart2:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_JOB_DATA_START2; break;

		case eBitType_UnloadBCDataExist1:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_BC_DATA_EXIST1; break;
		case eBitType_UnloadBCDataExist2:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_BC_DATA_EXIST2; break;

		case eBitType_AutoFocusReady1:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_READY1; break;
		case eBitType_AutoFocusReady2:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_READY2; break;

		case eBitType_AutoFocusEnd1:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_END1; break;
		case eBitType_AutoFocusEnd2:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_END2; break;

		case eBitType_MStageAContactOnStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_ON_START; break;
		case eBitType_MStageBContactOnStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_ON_START; break;

		case eBitType_MStageAContactOffStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_OFF_START; break;
		case eBitType_MStageBContactOffStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_OFF_START; break;

		case eBitType_MStageAContactNextStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_NEXT_START; break;
		case eBitType_MStageBContactNextStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_NEXT_START; break;

		case eBitType_MStageAContactBackStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_BACK_START; break;
		case eBitType_MStageBContactBackStart:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_BACK_START; break;

		case eBitType_MStageAContactPlcDataSend:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_PLC_SEND; break;
		case eBitType_MStageBContactPlcDataSend:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_PLC_SEND; break;

		case eBitType_MStageAContactPlcDataReceived:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_PLC_RECEIVED; break;
		case eBitType_MStageBContactPlcDataReceived:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_PLC_RECEIVED; break;

		case eBitType_MStageAPreGammaStart:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_START; break;
		case eBitType_MStageBPreGammaStart:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_START; break;

		case eBitType_MStageAPreGammaPlcDataSend:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_PLC_SEND; break;
		case eBitType_MStageBPreGammaPlcDataSend:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_PLC_SEND; break;

		case eBitType_MStageAPreGammaPlcDataReceived:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_PLC_RECEIVED; break;
		case eBitType_MStageBPreGammaPlcDataReceived:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_PLC_RECEIVED; break;

		case eBitType_MStageATouchStart:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_START; break;
		case eBitType_MStageBTouchStart:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_START; break;

		case eBitType_MStageATouchPlcDataSend:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_PLC_SEND; break;
		case eBitType_MStageBTouchPlcDataSend:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_PLC_SEND; break;

		case eBitType_MStageATouchPlcDataReceived:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_PLC_RECEIVED; break;
		case eBitType_MStageBTouchPlcDataReceived:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_PLC_RECEIVED; break;

		case eBitType_MStageA_OperatorViewStart:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_START; break;
		case eBitType_MStageB_OperatorViewStart:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_START; break;

		case eBitType_MStageA_OperatorViewPlcDataSend:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_PLC_SEND; break;
		case eBitType_MStageB_OperatorViewPlcDataSend:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_PLC_SEND; break;

		case eBitType_MStageA_OperatorViewPlcDataReceived:	*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_PLC_RECEIVED; break;
		case eBitType_MStageB_OperatorViewPlcDataReceived:	*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_PLC_RECEIVED; break;

		case eBitType_MStageA_OperatorViewBufferTrayStart:	*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_BUFFERTRAY_START; break;
		case eBitType_MStageB_OperatorViewBufferTrayStart:	*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_BUFFERTRAY_START; break;

		case eBitType_UnloadOKDFSStart1:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_OK_DFS_START1; break;
		case eBitType_UnloadOKDFSStart2:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_OK_DFS_START2; break;

		case eBitType_UnloadNGDFSStart1:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_NG_DFS_START1; break;
		case eBitType_UnloadNGDFSStart2:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_NG_DFS_START2; break;

		case eBitType_ULD_OK_DefectCodeStart1:				*pnAddr = LOCAL_BIT_L2M_ULD_OK_DEFECT_CODE_START1; break;
		case eBitType_ULD_OK_DefectCodeStart2:				*pnAddr = LOCAL_BIT_L2M_ULD_OK_DEFECT_CODE_START2; break;

		case eBitType_ULD_NG_DefectCodeStart1:				*pnAddr = LOCAL_BIT_L2M_ULD_NG_DEFECT_CODE_START1; break;
		case eBitType_ULD_NG_DefectCodeStart2:				*pnAddr = LOCAL_BIT_L2M_ULD_NG_DEFECT_CODE_START2; break;

#elif _SYSTEM_GAMMA_
		case eBitType_DataReportStart1:						*pnAddr = LOCAL_BIT_L2M_CH_1_DATA_REPORT_START; break;
		case eBitType_DataReportStart2:						*pnAddr = LOCAL_BIT_L2M_CH_2_DATA_REPORT_START; break;

		case eBitType_NgReportStart1:						*pnAddr = LOCAL_BIT_L2M_CH_1_TRAY_REPORT_START; break;
		case eBitType_NgReportStart2:						*pnAddr = LOCAL_BIT_L2M_CH_2_TRAY_REPORT_START; break;

		case eBitType_GammaContactOnStart1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart3:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart4:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart5:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart6:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart7:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart8:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart9:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart10:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart11:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_CONTACT_ON_START; break;
		case eBitType_GammaContactOnStart12:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_CONTACT_ON_START; break;
				
		case eBitType_GammaContactOffStart1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart3:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart4:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart5:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart6:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart7:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart8:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart9:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart10:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart11:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_CONTACT_OFF_START; break;
		case eBitType_GammaContactOffStart12:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_CONTACT_OFF_START; break;
				
		case eBitType_GammaContactNextStart1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_NEXT_START; break;
		case eBitType_GammaContactNextStart2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_NEXT_START; break;
		case eBitType_GammaContactNextStart3:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_NEXT_START; break;
		case eBitType_GammaContactNextStart4:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_NEXT_START; break;
		case eBitType_GammaContactNextStart5:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_NEXT_START; break;
		case eBitType_GammaContactNextStart6:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_NEXT_START; break;
		case eBitType_GammaContactNextStart7:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_NEXT_START; break;
		case eBitType_GammaContactNextStart8:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_NEXT_START; break;
		case eBitType_GammaContactNextStart9:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_NEXT_START; break;
		case eBitType_GammaContactNextStart10:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_NEXT_START; break;
		case eBitType_GammaContactNextStart11:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_NEXT_START; break;
		case eBitType_GammaContactNextStart12:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_NEXT_START; break;
				
		case eBitType_GammaContactBackStart1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_BACK_START; break;
		case eBitType_GammaContactBackStart2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_BACK_START; break;
		case eBitType_GammaContactBackStart3:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_BACK_START; break;
		case eBitType_GammaContactBackStart4:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_BACK_START; break;
		case eBitType_GammaContactBackStart5:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_BACK_START; break;
		case eBitType_GammaContactBackStart6:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_BACK_START; break;
		case eBitType_GammaContactBackStart7:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_BACK_START; break;
		case eBitType_GammaContactBackStart8:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_BACK_START; break;
		case eBitType_GammaContactBackStart9:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_BACK_START; break;
		case eBitType_GammaContactBackStart10:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_BACK_START; break;
		case eBitType_GammaContactBackStart11:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_BACK_START; break;
		case eBitType_GammaContactBackStart12:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_BACK_START; break;
				
		case eBitType_Gamma_1MTPStart1:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_MTP_START1; break;
		case eBitType_Gamma_1MTPStart2:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_MTP_START1; break;
		case eBitType_Gamma_1MTPStart3:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_MTP_START1; break;
		case eBitType_Gamma_1MTPStart4:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_MTP_START1; break;
		case eBitType_Gamma_1MTPStart5:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_MTP_START1; break;
		case eBitType_Gamma_1MTPStart6:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_MTP_START1; break;
		case eBitType_Gamma_1MTPStart7:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_MTP_START1; break;
		case eBitType_Gamma_1MTPStart8:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_MTP_START1; break;
		case eBitType_Gamma_1MTPStart9:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_MTP_START1; break;
		case eBitType_Gamma_1MTPStart10:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_MTP_START1; break;
		case eBitType_Gamma_1MTPStart11:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_MTP_START1; break;
		case eBitType_Gamma_1MTPStart12:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_MTP_START1; break;

		case eBitType_Gamma_2MTPStart1:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_MTP_START2; break;
		case eBitType_Gamma_2MTPStart2:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_MTP_START2; break;
		case eBitType_Gamma_2MTPStart3:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_MTP_START2; break;
		case eBitType_Gamma_2MTPStart4:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_MTP_START2; break;
		case eBitType_Gamma_2MTPStart5:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_MTP_START2; break;
		case eBitType_Gamma_2MTPStart6:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_MTP_START2; break;
		case eBitType_Gamma_2MTPStart7:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_MTP_START2; break;
		case eBitType_Gamma_2MTPStart8:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_MTP_START2; break;
		case eBitType_Gamma_2MTPStart9:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_MTP_START2; break;
		case eBitType_Gamma_2MTPStart10:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_MTP_START2; break;
		case eBitType_Gamma_2MTPStart11:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_MTP_START2; break;
		case eBitType_Gamma_2MTPStart12:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_MTP_START2; break;
				
		case eBitType_GammaStage1PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PLC_RECEIVER; break;
		case eBitType_GammaStage2PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PLC_RECEIVER; break;
		case eBitType_GammaStage3PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PLC_RECEIVER; break;
		case eBitType_GammaStage4PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PLC_RECEIVER; break;
		case eBitType_GammaStage5PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PLC_RECEIVER; break;
		case eBitType_GammaStage6PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PLC_RECEIVER; break;
		case eBitType_GammaStage7PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PLC_RECEIVER; break;
		case eBitType_GammaStage8PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PLC_RECEIVER; break;
		case eBitType_GammaStage9PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PLC_RECEIVER; break;
		case eBitType_GammaStage10PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PLC_RECEIVER; break;
		case eBitType_GammaStage11PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PLC_RECEIVER; break;
		case eBitType_GammaStage12PlcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PLC_RECEIVER; break;
				
		case eBitType_GammaStage1PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PLC_SEND; break;
		case eBitType_GammaStage2PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PLC_SEND; break;
		case eBitType_GammaStage3PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PLC_SEND; break;
		case eBitType_GammaStage4PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PLC_SEND; break;
		case eBitType_GammaStage5PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PLC_SEND; break;
		case eBitType_GammaStage6PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PLC_SEND; break;
		case eBitType_GammaStage7PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PLC_SEND; break;
		case eBitType_GammaStage8PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PLC_SEND; break;
		case eBitType_GammaStage9PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PLC_SEND; break;
		case eBitType_GammaStage10PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PLC_SEND; break;
		case eBitType_GammaStage11PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PLC_SEND; break;
		case eBitType_GammaStage12PlcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PLC_SEND; break;

		case eBitType_GammaStage1PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PANEL_EXIST_1; break;
		case eBitType_GammaStage2PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PANEL_EXIST_1; break;
		case eBitType_GammaStage3PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PANEL_EXIST_1; break;
		case eBitType_GammaStage4PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PANEL_EXIST_1; break;
		case eBitType_GammaStage5PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PANEL_EXIST_1; break;
		case eBitType_GammaStage6PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PANEL_EXIST_1; break;
		case eBitType_GammaStage7PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PANEL_EXIST_1; break;
		case eBitType_GammaStage8PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PANEL_EXIST_1; break;
		case eBitType_GammaStage9PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PANEL_EXIST_1; break;
		case eBitType_GammaStage10PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PANEL_EXIST_1; break;
		case eBitType_GammaStage11PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PANEL_EXIST_1; break;
		case eBitType_GammaStage12PanelExist1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PANEL_EXIST_1; break;

		case eBitType_GammaStage1PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PANEL_EXIST_2; break;
		case eBitType_GammaStage2PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PANEL_EXIST_2; break;
		case eBitType_GammaStage3PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PANEL_EXIST_2; break;
		case eBitType_GammaStage4PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PANEL_EXIST_2; break;
		case eBitType_GammaStage5PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PANEL_EXIST_2; break;
		case eBitType_GammaStage6PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PANEL_EXIST_2; break;
		case eBitType_GammaStage7PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PANEL_EXIST_2; break;
		case eBitType_GammaStage8PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PANEL_EXIST_2; break;
		case eBitType_GammaStage9PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PANEL_EXIST_2; break;
		case eBitType_GammaStage10PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PANEL_EXIST_2; break;
		case eBitType_GammaStage11PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PANEL_EXIST_2; break;
		case eBitType_GammaStage12PanelExist2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PANEL_EXIST_2; break;

		case eBitType_GammaStagePIDCheckStart1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart3:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart4:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart5:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart6:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart7:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart8:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart9:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PDI_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart10:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PID_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart11:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PID_CHECK_START; break;
		case eBitType_GammaStagePIDCheckStart12:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PID_CHECK_START; break;

		case eBitType_GammaStageOperatorModeStart1:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart2:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart3:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart4:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart5:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart6:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart7:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart8:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart9:			*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart10:		*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart11:		*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_OPERATOR_MODE_START; break;
		case eBitType_GammaStageOperatorModeStart12:		*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_OPERATOR_MODE_START; break;

		case eBitType_GammaNGDFSStart1:						*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DFS_START1; break;
		case eBitType_GammaNGDFSStart2:						*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DFS_START2; break;

		case eBitType_GammaNGDefectCodeStart1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DEFECT_CODE_START1; break;
		case eBitType_GammaNGDefectCodeStart2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DEFECT_CODE_START1; break;
#endif

		case eBitType_JobDataStart1:						*pnAddr = LOCAL_BIT_L2M_JOB_DATA_START1; break;
		case eBitType_JobDataStart2:						*pnAddr = LOCAL_BIT_L2M_JOB_DATA_START2; break;

		case eBitType_BCDataExist1:							*pnAddr = LOCAL_BIT_L2M_BC_DATA_EXIST1; break;
		case eBitType_BCDataExist2:							*pnAddr = LOCAL_BIT_L2M_BC_DATA_EXIST2; break;

		case eBitType_IDSerarchSend:						*pnAddr = LOCAL_BIT_L2M_ID_SEARCH_SEND; break;
		case eBitType_IDSerarchStart:						*pnAddr = LOCAL_BIT_L2M_ID_SEARCH_START; break;
		case eBitType_IDSerarchPmMode:						*pnAddr = LOCAL_BIT_L2M_ID_SEARCH_PM_MODE; break;

		case eBitType_PassWordSerarchSend:					*pnAddr = LOCAL_BIT_L2M_PASSOWRD_SEARCH_SEND; break;
		case eBitType_PassWordSerarchStart:					*pnAddr = LOCAL_BIT_L2M_PASSOWRD_SEARCH_START; break;
		case eBitType_PassWordSerarchPmMode:				*pnAddr = LOCAL_BIT_L2M_PASSOWRD_SEARCH_PM_MODE; break;

		case eBitType_FFUStart:								*pnAddr = LOCAL_BIT_L2M_FFU_START; break;

		case eBitType_DFSStart1:							*pnAddr = LOCAL_BIT_L2M_DFS_START1; break;
		case eBitType_DFSStart2:							*pnAddr = LOCAL_BIT_L2M_DFS_START2; break;

		case eBitType_DefectCodeStart1:						*pnAddr = LOCAL_BIT_L2M_DEFECT_CODE_START1; break;
		case eBitType_DefectCodeStart2:						*pnAddr = LOCAL_BIT_L2M_DEFECT_CODE_START2; break;

		//////////////////////////////////////////// PC > PLC Bit ////////////////////////////////////////////

		case eBitType_DataServerPcHeartBit:					*pnAddr = LOCAL_BIT_L2M_DATA_SERVER_PC_HEARTBIT; break;
		case eBitType_AlarmEnd:								*pnAddr = LOCAL_BIT_L2M_ALARM_END; break;

		case eBitType_ModelEnd:								*pnAddr = LOCAL_BIT_L2M_MODEL_END; break;

		case eBitType_OperateEnd:							*pnAddr = LOCAL_BIT_L2M_OPERATION_END; break;
		case eBitType_AxisEnd:								*pnAddr = LOCAL_BIT_L2M_AXIS_END; break;

		case eBitType_Align1Ready1:							*pnAddr = LOCAL_BIT_L2M_ALGIN1_READY_ADDR1; break;
		case eBitType_Align2Ready1:							*pnAddr = LOCAL_BIT_L2M_ALGIN2_READY_ADDR1; break;

		case eBitType_Align1Ready2:							*pnAddr = LOCAL_BIT_L2M_ALGIN1_READY_ADDR2; break;
		case eBitType_Align2Ready2:							*pnAddr = LOCAL_BIT_L2M_ALGIN2_READY_ADDR2; break;

		case eBitType_Align1DataPcSend1:					*pnAddr = LOCAL_BIT_L2M_ALIGN1_PC_SEND1; break;
		case eBitType_Align2DataPcSend1:					*pnAddr = LOCAL_BIT_L2M_ALIGN2_PC_SEND1; break;

		case eBitType_Align1DataPcSend2:					*pnAddr = LOCAL_BIT_L2M_ALIGN1_PC_SEND2; break;
		case eBitType_Align2DataPcSend2:					*pnAddr = LOCAL_BIT_L2M_ALIGN2_PC_SEND2; break;

		case eBitType_Align1DataPcReceived1:				*pnAddr = LOCAL_BIT_L2M_ALIGN1_PC_RECEIVED1; break;
		case eBitType_Align2DataPcReceived1:				*pnAddr = LOCAL_BIT_L2M_ALIGN2_PC_RECEIVED1; break;

		case eBitType_Align1DataPcReceived2:				*pnAddr = LOCAL_BIT_L2M_ALIGN1_PC_RECEIVED2; break;
		case eBitType_Align2DataPcReceived2:				*pnAddr = LOCAL_BIT_L2M_ALIGN2_PC_RECEIVED2; break;

		case eBitType_Align1End1:							*pnAddr = LOCAL_BIT_L2M_ALIGN1_END1; break; // AOI / GAMMA PANEL 1
		case eBitType_Align2End1:							*pnAddr = LOCAL_BIT_L2M_ALIGN2_END1; break; // ULD PANEL 1

		case eBitType_Align1End2:							*pnAddr = LOCAL_BIT_L2M_ALIGN1_END2; break; // AOI / GAMMA PANEL 2
		case eBitType_Align2End2:							*pnAddr = LOCAL_BIT_L2M_ALIGN2_END2; break; // ULD PANEL 2

		case eBitType_TrayCheckReady1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_READY1; break;
		case eBitType_TrayCheckReady2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_READY2; break;
		case eBitType_TrayCheckReady3:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_READY3; break;

		case eBitType_TrayCheckEnd1:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_END1; break;
		case eBitType_TrayCheckEnd2:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_END2; break;
		case eBitType_TrayCheckEnd3:						*pnAddr = LOCAL_BIT_L2M_ALIGN_TRAY_CHECK_END3; break;

		case eBitType_TrayLowerAlignSend1:					*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_PC_SEND1; break;
		case eBitType_TrayLowerAlignSend2:					*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_PC_SEND2; break;

		case eBitType_TrayLowerAlignReceived1:				*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_PC_RECEIVED1; break;
		case eBitType_TrayLowerAlignReceived2:				*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_PC_RECEIVED2; break;

		case eBitType_TrayLowerAlignReady1:					*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_READY1; break;
		case eBitType_TrayLowerAlignReady2:					*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_READY2; break;

		case eBitType_TrayLowerAlignEnd1:					*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_END1; break;
		case eBitType_TrayLowerAlignEnd2:					*pnAddr = LOCAL_BIT_L2M_TRAY_LOWER_ALIGN_END2; break;

		case eBitType_TrayAlignReady1:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_READY1; break;
		case eBitType_TrayAlignReady2:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_READY2; break;
		case eBitType_TrayAlignReady3:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_READY3; break;

		case eBitType_TrayAlignEnd1:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_END1; break;
		case eBitType_TrayAlignEnd2:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_END2; break;
		case eBitType_TrayAlignEnd3:						*pnAddr = LOCAL_BIT_L2M_TRAY_ALIGN_END3; break;

#if _SYSTEM_AMTAFT_
		case eBitType_DataReportEnd1:						*pnAddr = LOCAL_BIT_L2M_CH_1_DATA_REPORT_END; break;
		case eBitType_DataReportEnd2:						*pnAddr = LOCAL_BIT_L2M_CH_2_DATA_REPORT_END; break;
		case eBitType_DataReportEnd3:						*pnAddr = LOCAL_BIT_L2M_CH_3_DATA_REPORT_END; break;
		case eBitType_DataReportEnd4:						*pnAddr = LOCAL_BIT_L2M_CH_4_DATA_REPORT_END; break;

		case eBitType_TrayReportEnd1:						*pnAddr = LOCAL_BIT_L2M_CH_1_TRAY_REPORT_END; break;
		case eBitType_TrayReportEnd2:						*pnAddr = LOCAL_BIT_L2M_CH_2_TRAY_REPORT_END; break;

		case eBitType_GoodReportEnd:						*pnAddr = LOCAL_BIT_L2M_GOOD_REPORT_END; break;
		case eBitType_NgReportEnd:							*pnAddr = LOCAL_BIT_L2M_NG_REPORT_END; break;
		case eBitType_SampleReportEnd:						*pnAddr = LOCAL_BIT_L2M_SAMPLE_REPORT_END; break;
		case eBitType_BufferReportEnd:						*pnAddr = LOCAL_BIT_L2M_BUFFER_REPORT_END; break;

		case eBitType_AZoneContactOnEnd:					*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_ON_END; break;
		case eBitType_BZoneContactOnEnd:					*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_ON_END; break;
		case eBitType_CZoneContactOnEnd:					*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_ON_END; break;
		case eBitType_DZoneContactOnEnd:					*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_ON_END; break;

		case eBitType_AZoneContactOffEnd:					*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_OFF_END; break;
		case eBitType_BZoneContactOffEnd:					*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_OFF_END; break;
		case eBitType_CZoneContactOffEnd:					*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_OFF_END; break;
		case eBitType_DZoneContactOffEnd:					*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_OFF_END; break;

		case eBitType_AZoneTouchInspectionEnd:				*pnAddr = LOCAL_BIT_L2M_AZONE_TOUCH_INSPECTION_END; break;
		case eBitType_BZoneTouchInspectionEnd:				*pnAddr = LOCAL_BIT_L2M_BZONE_TOUCH_INSPECTION_END; break;
		case eBitType_CZoneTouchInspectionEnd:				*pnAddr = LOCAL_BIT_L2M_CZONE_TOUCH_INSPECTION_END; break;
		case eBitType_DZoneTouchInspectionEnd:				*pnAddr = LOCAL_BIT_L2M_DZONE_TOUCH_INSPECTION_END; break;

		case eBitType_LumitopEnd1:							*pnAddr = LOCAL_BIT_L2M_LUMITOP_END1_ADDR; break;
		case eBitType_LumitopEnd2:							*pnAddr = LOCAL_BIT_L2M_LUMITOP_END2_ADDR; break;
		case eBitType_LumitopEnd3:							*pnAddr = LOCAL_BIT_L2M_LUMITOP_END3_ADDR; break;
		case eBitType_LumitopEnd4:							*pnAddr = LOCAL_BIT_L2M_LUMITOP_END4_ADDR; break;

		case eBitType_LumitopGrabEnd1:						*pnAddr = LOCAL_BIT_L2M_LUMITOP_GRAB_END1_ADDR; break;
		case eBitType_LumitopGrabEnd2:						*pnAddr = LOCAL_BIT_L2M_LUMITOP_GRAB_END2_ADDR; break;
		case eBitType_LumitopGrabEnd3:						*pnAddr = LOCAL_BIT_L2M_LUMITOP_GRAB_END3_ADDR; break;
		case eBitType_LumitopGrabEnd4:						*pnAddr = LOCAL_BIT_L2M_LUMITOP_GRAB_END4_ADDR; break;

		case eBitType_VisionEnd1:							*pnAddr = LOCAL_BIT_L2M_VISION_END1_ADDR; break;
		case eBitType_VisionEnd2:							*pnAddr = LOCAL_BIT_L2M_VISION_END2_ADDR; break;
		case eBitType_VisionEnd3:							*pnAddr = LOCAL_BIT_L2M_VISION_END3_ADDR; break;
		case eBitType_VisionEnd4:							*pnAddr = LOCAL_BIT_L2M_VISION_END4_ADDR; break;

		case eBitType_VisionGrabEnd1:						*pnAddr = LOCAL_BIT_L2M_VISION_GRAB_END1_ADDR; break;
		case eBitType_VisionGrabEnd2:						*pnAddr = LOCAL_BIT_L2M_VISION_GRAB_END2_ADDR; break;
		case eBitType_VisionGrabEnd3:						*pnAddr = LOCAL_BIT_L2M_VISION_GRAB_END3_ADDR; break;
		case eBitType_VisionGrabEnd4:						*pnAddr = LOCAL_BIT_L2M_VISION_GRAB_END4_ADDR; break;

		case eBitType_VisionSameDefectAlarmStart:			*pnAddr = LOCAL_BIT_L2M_VISION_SAME_DEFECT_ALARM_ADDR; break;

		case eBitType_ViewingAngleEnd1:						*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_END1_ADDR; break;
		case eBitType_ViewingAngleEnd2:						*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_END2_ADDR; break;
		case eBitType_ViewingAngleEnd3:						*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_END3_ADDR; break;
		case eBitType_ViewingAngleEnd4:						*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_END4_ADDR; break;

		case eBitType_AZoneTouchReady:						*pnAddr = LOCAL_BIT_L2M_INDEX_TOUCH_AZONE_READY_ADDR; break;
		case eBitType_BZoneTouchReady:						*pnAddr = LOCAL_BIT_L2M_INDEX_TOUCH_BZONE_READY_ADDR; break;
		case eBitType_CZoneTouchReady:						*pnAddr = LOCAL_BIT_L2M_INDEX_TOUCH_CZONE_READY_ADDR; break;
		case eBitType_DZoneTouchReady:						*pnAddr = LOCAL_BIT_L2M_INDEX_TOUCH_DZONE_READY_ADDR; break;
		case eBitType_PreGammaReady:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_READY_ADDR; break;
		case eBitType_VisionReady:							*pnAddr = LOCAL_BIT_L2M_VISION_READY_ADDR; break;
		case eBitType_ViewingAngleReady:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_READY_ADDR; break;
		case eBitType_LumitopReady:							*pnAddr = LOCAL_BIT_L2M_LUMITOP_READY_ADDR; break;

		case eBitType_ViewingAnglePcSend:					*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_PC_SEND; break;
		case eBitType_ViewingAnglePcReceiver:				*pnAddr = LOCAL_BIT_L2M_VIEWING_ANGLE_PC_RECEIVER; break;

		case eBitType_VisionPcSend:							*pnAddr = LOCAL_BIT_L2M_VISION_PC_SEND; break;
		case eBitType_VisionPcReceiver:						*pnAddr = LOCAL_BIT_L2M_VISION_PC_RECEIVER; break;

		case eBitType_LumitopPcSend:						*pnAddr = LOCAL_BIT_L2M_LUMITOP_PC_SEND; break;
		case eBitType_LumitopPcReceiver:					*pnAddr = LOCAL_BIT_L2M_LUMITOP_PC_RECEIVER; break;

		case eBitType_AZoneTouchPcSend:						*pnAddr = LOCAL_BIT_L2M_AZONE_TOUCH_PC_SEND; break;
		case eBitType_BZoneTouchPcSend:						*pnAddr = LOCAL_BIT_L2M_BZONE_TOUCH_PC_SEND; break;
		case eBitType_CZoneTouchPcSend:						*pnAddr = LOCAL_BIT_L2M_CZONE_TOUCH_PC_SEND; break;
		case eBitType_DZoneTouchPcSend:						*pnAddr = LOCAL_BIT_L2M_DZONE_TOUCH_PC_SEND; break;

		case eBitType_AZoneTouchPcReceiver:					*pnAddr = LOCAL_BIT_L2M_AZONE_TOUCH_PC_RECEIVER; break;
		case eBitType_BZoneTouchPcReceiver:					*pnAddr = LOCAL_BIT_L2M_BZONE_TOUCH_PC_RECEIVER; break;
		case eBitType_CZoneTouchPcReceiver:					*pnAddr = LOCAL_BIT_L2M_CZONE_TOUCH_PC_RECEIVER; break;
		case eBitType_DZoneTouchPcReceiver:					*pnAddr = LOCAL_BIT_L2M_DZONE_TOUCH_PC_RECEIVER; break;

		case eBitType_PreGammaPcSend:						*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_PC_SEND; break;
		case eBitType_PreGammaPcReceiver:					*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_PC_RECEIVER; break;

		case eBitType_AZoneContactPcSend:					*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_PC_SEND; break;
		case eBitType_BZoneContactPcSend:					*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_PC_SEND; break;
		case eBitType_CZoneContactPcSend:					*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_PC_SEND; break;
		case eBitType_DZoneContactPcSend:					*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_PC_SEND; break;

		case eBitType_AZoneContactPcReceiver:				*pnAddr = LOCAL_BIT_L2M_AZONE_CONTACT_PC_RECEIVER; break;
		case eBitType_BZoneContactPcReceiver:				*pnAddr = LOCAL_BIT_L2M_BZONE_CONTACT_PC_RECEIVER; break;
		case eBitType_CZoneContactPcReceiver:				*pnAddr = LOCAL_BIT_L2M_CZONE_CONTACT_PC_RECEIVER; break;
		case eBitType_DZoneContactPcReceiver:				*pnAddr = LOCAL_BIT_L2M_DZONE_CONTACT_PC_RECEIVER; break;

		case eBitType_PreGammaEnd1:							*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_END1; break;
		case eBitType_PreGammaEnd2:							*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_END2; break;
		case eBitType_PreGammaEnd3:							*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_END3; break;
		case eBitType_PreGammaEnd4:							*pnAddr = LOCAL_BIT_L2M_PRE_GAMMA_END4; break;

		case eBitType_AutoFocusStart1:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_START1; break;
		case eBitType_AutoFocusStart2:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_START2; break;

		case eBitType_AutoFocusSave1:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_SAVE1; break;
		case eBitType_AutoFocusSave2:						*pnAddr = LOCAL_BIT_L2M_AUTO_FOCUS_SAVE2; break;

		case eBitType_MStageAContactOnEnd:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_ON_END; break;
		case eBitType_MStageBContactOnEnd:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_ON_END; break;

		case eBitType_MStageAContactOffEnd:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_OFF_END; break;
		case eBitType_MStageBContactOffEnd:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_OFF_END; break;

		case eBitType_MStageAContactNextEnd:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_NEXT_END; break;
		case eBitType_MStageBContactNextEnd:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_NEXT_END; break;

		case eBitType_MStageAContactBackEnd:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_BACK_END; break;
		case eBitType_MStageBContactBackEnd:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_BACK_END; break;

		case eBitType_MStageAContactPcDataSend:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_PC_SEND; break;
		case eBitType_MStageBContactPcDataSend:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_PC_SEND; break;

		case eBitType_MStageAContactPcDataReceived:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_CONTACT_PC_RECEIVED; break;
		case eBitType_MStageBContactPcDataReceived:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_CONTACT_PC_RECEIVED; break;

		case eBitType_MStageAPreGammaReady:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_READY; break;
		case eBitType_MStageBPreGammaReady:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_READY; break;

		case eBitType_MStageAPreGammaEnd:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_END; break;
		case eBitType_MStageBPreGammaEnd:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_END; break;

		case eBitType_MStageAPreGammaPcDataSend:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_PC_SEND; break;
		case eBitType_MStageBPreGammaPcDataSend:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_PC_SEND; break;

		case eBitType_MStageAPreGammaPcDataReceived:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_PRE_GAMMA_PC_RECEIVED; break;
		case eBitType_MStageBPreGammaPcDataReceived:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_PRE_GAMMA_PC_RECEIVED; break;

		case eBitType_MStageATouchReady:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_READY; break;
		case eBitType_MStageBTouchReady:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_READY; break;

		case eBitType_MStageATouchEnd:						*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_END; break;
		case eBitType_MStageBTouchEnd:						*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_END; break;

		case eBitType_MStageATouchPcDataSend:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_PC_SEND; break;
		case eBitType_MStageBTouchPcDataSend:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_PC_SEND; break;

		case eBitType_MStageATouchPcDataReceived:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_TOUCH_PC_RECEIVED; break;
		case eBitType_MStageBTouchPcDataReceived:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_TOUCH_PC_RECEIVED; break;

		case eBitType_MStageA_OperatorViewReady:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_READY; break;
		case eBitType_MStageB_OperatorViewReady:			*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_READY; break;

		case eBitType_MStageA_OperatorViewEnd:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_END; break;
		case eBitType_MStageB_OperatorViewEnd:				*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_END; break;

		case eBitType_MStageA_OperatorViewPcDataSend:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_PC_SEND; break;
		case eBitType_MStageB_OperatorViewPcDataSend:		*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_PC_SEND; break;

		case eBitType_MStageA_OperatorViewPcDataReceived:	*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_PC_RECEIVED; break;
		case eBitType_MStageB_OperatorViewPcDataReceived:	*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_PC_RECEIVED; break;

		case eBitType_MStageA_OPVLoginOut:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_A_OPV_LOGIN_OUT; break;
		case eBitType_MStageB_OPVLoginOut:					*pnAddr = LOCAL_BIT_L2M_M_STAGE_B_OPV_LOGIN_OUT; break;

		case eBitType_UnloadOKDFSEnd1:						*pnAddr = LOCAL_BIT_L2M_UNLOAD_OK_DFS_END1; break;
		case eBitType_UnloadOKDFSEnd2:						*pnAddr = LOCAL_BIT_L2M_UNLOAD_OK_DFS_END2; break;

		case eBitType_UnloadNGDFSEnd1:						*pnAddr = LOCAL_BIT_L2M_UNLOAD_NG_DFS_END1; break;
		case eBitType_UnloadNGDFSEnd2:						*pnAddr = LOCAL_BIT_L2M_UNLOAD_NG_DFS_END2; break;

		case eBitType_ULD_OK_DefectCodeEnd1:				*pnAddr = LOCAL_BIT_L2M_ULD_OK_DEFECT_CODE_END1; break;
		case eBitType_ULD_OK_DefectCodeEnd2:				*pnAddr = LOCAL_BIT_L2M_ULD_OK_DEFECT_CODE_END2; break;

		case eBitType_ULD_NG_DefectCodeEnd1:				*pnAddr = LOCAL_BIT_L2M_ULD_NG_DEFECT_CODE_END1; break;
		case eBitType_ULD_NG_DefectCodeEnd2:				*pnAddr = LOCAL_BIT_L2M_ULD_NG_DEFECT_CODE_END2; break;

#elif _SYSTEM_GAMMA_
		case eBitType_DataReportEnd1:						*pnAddr = LOCAL_BIT_L2M_CH_1_DATA_REPORT_END; break;
		case eBitType_DataReportEnd2:						*pnAddr = LOCAL_BIT_L2M_CH_2_DATA_REPORT_END; break;

		case eBitType_NgReportEnd1:							*pnAddr = LOCAL_BIT_L2M_CH_1_TRAY_REPORT_END; break;
		case eBitType_NgReportEnd2:							*pnAddr = LOCAL_BIT_L2M_CH_2_TRAY_REPORT_END; break;

		case eBitType_GammaContactOnEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd3:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd4:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd5:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd6:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd7:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd8:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd9:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd10:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd11:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_CONTACT_ON_END; break;
		case eBitType_GammaContactOnEnd12:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_CONTACT_ON_END; break;

		case eBitType_GammaContactOffEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd3:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd4:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd5:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd6:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd7:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd8:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd9:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd10:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd11:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_CONTACT_OFF_END; break;
		case eBitType_GammaContactOffEnd12:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_CONTACT_OFF_END; break;

		case eBitType_GammaContactNextEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_NEXT_END; break;
		case eBitType_GammaContactNextEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_NEXT_END; break;
		case eBitType_GammaContactNextEnd3:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_NEXT_END; break;
		case eBitType_GammaContactNextEnd4:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_NEXT_END; break;
		case eBitType_GammaContactNextEnd5:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_NEXT_END; break;
		case eBitType_GammaContactNextEnd6:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_NEXT_END; break;
		case eBitType_GammaContactNextEnd7:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_NEXT_END; break;
		case eBitType_GammaContactNextEnd8:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_NEXT_END; break;
		case eBitType_GammaContactNextEnd9:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_NEXT_END; break;
		case eBitType_GammaContactNextEnd10:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_NEXT_END; break;
		case eBitType_GammaContactNextEnd11:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_NEXT_END; break;
		case eBitType_GammaContactNextEnd12:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_NEXT_END; break;

		case eBitType_GammaContactBackEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_BACK_END; break;
		case eBitType_GammaContactBackEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_BACK_END; break;
		case eBitType_GammaContactBackEnd3:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_BACK_END; break;
		case eBitType_GammaContactBackEnd4:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_BACK_END; break;
		case eBitType_GammaContactBackEnd5:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_BACK_END; break;
		case eBitType_GammaContactBackEnd6:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_BACK_END; break;
		case eBitType_GammaContactBackEnd7:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_BACK_END; break;
		case eBitType_GammaContactBackEnd8:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_BACK_END; break;
		case eBitType_GammaContactBackEnd9:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_BACK_END; break;
		case eBitType_GammaContactBackEnd10:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_BACK_END; break;
		case eBitType_GammaContactBackEnd11:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_BACK_END; break;
		case eBitType_GammaContactBackEnd12:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_BACK_END; break;

		case eBitType_GammaStage1MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_MTP_END1; break;
		case eBitType_GammaStage2MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_MTP_END1; break;
		case eBitType_GammaStage3MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_MTP_END1; break;
		case eBitType_GammaStage4MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_MTP_END1; break;
		case eBitType_GammaStage5MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_MTP_END1; break;
		case eBitType_GammaStage6MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_MTP_END1; break;
		case eBitType_GammaStage7MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_MTP_END1; break;
		case eBitType_GammaStage8MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_MTP_END1; break;
		case eBitType_GammaStage9MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_MTP_END1; break;
		case eBitType_GammaStage10MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_MTP_END1; break;
		case eBitType_GammaStage11MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_MTP_END1; break;
		case eBitType_GammaStage12MTPEnd1:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_MTP_END1; break;

		case eBitType_GammaStage1MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_MTP_END2; break;
		case eBitType_GammaStage2MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_MTP_END2; break;
		case eBitType_GammaStage3MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_MTP_END2; break;
		case eBitType_GammaStage4MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_MTP_END2; break;
		case eBitType_GammaStage5MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_MTP_END2; break;
		case eBitType_GammaStage6MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_MTP_END2; break;
		case eBitType_GammaStage7MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_MTP_END2; break;
		case eBitType_GammaStage8MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_MTP_END2; break;
		case eBitType_GammaStage9MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_MTP_END2; break;
		case eBitType_GammaStage10MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_MTP_END2; break;
		case eBitType_GammaStage11MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_MTP_END2; break;
		case eBitType_GammaStage12MTPEnd2:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_MTP_END2; break;

		case eBitType_GammaStage1PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PC_RECEIVER_END; break;
		case eBitType_GammaStage2PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PC_RECEIVER_END; break;
		case eBitType_GammaStage3PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PC_RECEIVER_END; break;
		case eBitType_GammaStage4PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PC_RECEIVER_END; break;
		case eBitType_GammaStage5PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PC_RECEIVER_END; break;
		case eBitType_GammaStage6PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PC_RECEIVER_END; break;
		case eBitType_GammaStage7PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PC_RECEIVER_END; break;
		case eBitType_GammaStage8PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PC_RECEIVER_END; break;
		case eBitType_GammaStage9PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PC_RECEIVER_END; break;
		case eBitType_GammaStage10PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PC_RECEIVER_END; break;
		case eBitType_GammaStage11PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PC_RECEIVER_END; break;
		case eBitType_GammaStage12PcReceiver:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PC_RECEIVER_END; break;

		case eBitType_GammaStage1PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PC_SEND_END; break;
		case eBitType_GammaStage2PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PC_SEND_END; break;
		case eBitType_GammaStage3PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PC_SEND_END; break;
		case eBitType_GammaStage4PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PC_SEND_END; break;
		case eBitType_GammaStage5PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PC_SEND_END; break;
		case eBitType_GammaStage6PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PC_SEND_END; break;
		case eBitType_GammaStage7PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PC_SEND_END; break;
		case eBitType_GammaStage8PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PC_SEND_END; break;
		case eBitType_GammaStage9PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PC_SEND_END; break;
		case eBitType_GammaStage10PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PC_SEND_END; break;
		case eBitType_GammaStage11PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PC_SEND_END; break;
		case eBitType_GammaStage12PcSend:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PC_SEND_END; break;

		case eBitType_GammaStage1Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_READY; break;
		case eBitType_GammaStage2Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_READY; break;
		case eBitType_GammaStage3Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_READY; break;
		case eBitType_GammaStage4Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_READY; break;
		case eBitType_GammaStage5Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_READY; break;
		case eBitType_GammaStage6Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_READY; break;
		case eBitType_GammaStage7Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_READY; break;
		case eBitType_GammaStage8Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_READY; break;
		case eBitType_GammaStage9Ready:						*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_READY; break;
		case eBitType_GammaStage10Ready:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_READY; break;
		case eBitType_GammaStage11Ready:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_READY; break;
		case eBitType_GammaStage12Ready:					*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_READY; break;

		case eBitType_GammaStage1PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_1_PID_CHCEK_END; break;
		case eBitType_GammaStage2PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_2_PID_CHCEK_END; break;
		case eBitType_GammaStage3PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_3_PID_CHCEK_END; break;
		case eBitType_GammaStage4PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_4_PID_CHCEK_END; break;
		case eBitType_GammaStage5PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_5_PID_CHCEK_END; break;
		case eBitType_GammaStage6PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_6_PID_CHCEK_END; break;
		case eBitType_GammaStage7PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_7_PID_CHCEK_END; break;
		case eBitType_GammaStage8PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_8_PID_CHCEK_END; break;
		case eBitType_GammaStage9PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_9_PID_CHCEK_END; break;
		case eBitType_GammaStage10PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_10_PID_CHECK_END; break;
		case eBitType_GammaStage11PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_11_PID_CHECK_END; break;
		case eBitType_GammaStage12PIDCheckEnd:				*pnAddr = LOCAL_BIT_L2M_GAMMA_STAGE_12_PID_CHECK_END; break;

		case eBitType_GammaNGDFSEnd1:						*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DFS_END1; break;
		case eBitType_GammaNGDFSEnd2:						*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DFS_END2; break;

		case eBitType_GammaNGDefectCodeEnd1:				*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DEFECT_CODE_END1; break;
		case eBitType_GammaNGDefectCodeEnd2:				*pnAddr = LOCAL_BIT_L2M_GAMMA_NG_DEFECT_CODE_END2; break;
#endif

		case eBitType_IDSerarchReceived:					*pnAddr = LOCAL_BIT_L2M_ID_SEARCH_RECEIVED; break;
		case eBitType_IDSerarchEnd:							*pnAddr = LOCAL_BIT_L2M_ID_SEARCH_END; break;
		case eBitType_PassWordSerarchReceived:				*pnAddr = LOCAL_BIT_L2M_PASSOWRD_SEARCH_RECEIVED; break;
		case eBitType_PassWordSerarchEnd:					*pnAddr = LOCAL_BIT_L2M_PASSOWRD_SEARCH_END; break;

		case eBitType_JobDataEnd1:							*pnAddr = LOCAL_BIT_L2M_JOB_DATA_END1; break;
		case eBitType_JobDataEnd2:							*pnAddr = LOCAL_BIT_L2M_JOB_DATA_END2; break;

		case eBitType_UnloadJobDataEnd1:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_JOB_DATA_END1; break;
		case eBitType_UnloadJobDataEnd2:					*pnAddr = LOCAL_BIT_L2M_UNLOAD_JOB_DATA_END2; break;

		case eBitType_FFUEnd:								*pnAddr = LOCAL_BIT_L2M_FFU_END; break;

		case eBitType_DFSEnd1:								*pnAddr = LOCAL_BIT_L2M_DFS_END1; break;
		case eBitType_DFSEnd2:								*pnAddr = LOCAL_BIT_L2M_DFS_END2; break;

		case eBitType_DefectCodeEnd1:						*pnAddr = LOCAL_BIT_L2M_DEFECT_CODE_END1; break;
		case eBitType_DefectCodeEnd2:						*pnAddr = LOCAL_BIT_L2M_DEFECT_CODE_END2; break;
	}
}

/*========================================================================================
FUNCTION : MNetH::GetPLCAddressWord()
DESCRIPT :
RETURN	 :
ARGUMENT :
FIRST	 : 2015/11/16, JSLee
LAST	 :
========================================================================================*/
void MNetH::GetPLCAddressWord(int nLocal, int nType, long *plAddr) //151116 JSLee
{
	unsigned short	nRet = 0;

	if (nLocal == -1)
		nLocal = m_nCurLocal;

	nLocal = nLocal - eLocal2;
	if (nLocal < 0)
		nLocal = 0;

	switch (nType)
	{
		//////////////////////////////////////////// PC > PLC WORD ////////////////////////////////////////////
		case eWordType_ModelRecipeData:						*plAddr = LOCAL_WORD_L2M_MODEL_RECIPE_DATA; break;

		case eWordType_AlarmCode:							*plAddr = LOCAL_WORD_L2M_ALARM_CODE; break;
		case eWordType_OperateTimeValue:					*plAddr = LOCAL_WORD_L2M_OPERATE_TIME_VALUE; break;

		case eWordType_TactTime:							*plAddr = LOCAL_WORD_L2M_TACT_TIME; break;

		case eWordType_AxisRecipeIdValue:					*plAddr = LOCAL_WORD_L2M_AXIS_RECIPE_ID_VALUE; break;
		case eWordType_AxisPostionBeforeValue:				*plAddr = LOCAL_WORD_L2M_AXIS_POSITION_BEFORE_VALUE; break;
		case eWordType_AxisPostionValue:					*plAddr = LOCAL_WORD_L2M_AXIS_POSITION_VALUE; break;

		case eWordType_AlignPosition1:						*plAddr = LOCAL_WORD_L2M_ALIGN_POSITION1; break;
		case eWordType_AlignPosition2:						*plAddr = LOCAL_WORD_L2M_ALIGN_POSITION2; break;

		case eWordType_AlignCount1:							*plAddr = LOCAL_WORD_L2M_ALIGN_COUNT1; break;
		case eWordType_AlignCount2:							*plAddr = LOCAL_WORD_L2M_ALIGN_COUNT2; break;

		case eWordType_AlignReverse:						*plAddr = LOCAL_WORD_L2M_ALIGN_REVERSE; break;

		case eWordType_AlignAxisValue1:						*plAddr = LOCAL_WORD_L2M_ALIGN_AXIS_VALUE1; break;
		case eWordType_AlignAxisValue2:						*plAddr = LOCAL_WORD_L2M_ALIGN_AXIS_VALUE2; break;

		case eWordType_Align1PanelID1:						*plAddr = LOCAL_WORD_L2M_ALIGN1_PANEL_ID_1; break;
		case eWordType_Align2PanelID1:						*plAddr = LOCAL_WORD_L2M_ALIGN2_PANEL_ID_1; break;

		case eWordType_Align1PanelID2:						*plAddr = LOCAL_WORD_L2M_ALIGN1_PANEL_ID_2; break;
		case eWordType_Align2PanelID2:						*plAddr = LOCAL_WORD_L2M_ALIGN2_PANEL_ID_2; break;

		case eWordType_TrayLowerAlignPosition1:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN_POSITION1; break;
		case eWordType_TrayLowerAlignPosition2:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN_POSITION2; break;

		case eWordType_TrayLowerAlignCount1:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN_COUNT1; break;
		case eWordType_TrayLowerAlignCount2:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN_COUNT2; break;

		case eWordType_TrayLower1AlignPanelID1:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN1_PANEL_ID_1; break;
		case eWordType_TrayLower2AlignPanelID1:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN2_PANEL_ID_1; break;

		case eWordType_TrayLower1AlignPanelID2:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN1_PANEL_ID_2; break;
		case eWordType_TrayLower2AlignPanelID2:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN2_PANEL_ID_2; break;

#if _SYSTEM_AMTAFT_
		case eWordType_DataReportValue1:					*plAddr = LOCAL_WORD_L2M_CH_1_DATA_REPORT_VALUE; break;
		case eWordType_DataReportValue2:					*plAddr = LOCAL_WORD_L2M_CH_2_DATA_REPORT_VALUE; break;
		case eWordType_DataReportValue3:					*plAddr = LOCAL_WORD_L2M_CH_3_DATA_REPORT_VALUE; break;
		case eWordType_DataReportValue4:					*plAddr = LOCAL_WORD_L2M_CH_4_DATA_REPORT_VALUE; break;

		case eWordType_TrayReportValue1:					*plAddr = LOCAL_WORD_L2M_CH_1_TRAY_REPORT_VALUE; break;
		case eWordType_TrayReportValue2:					*plAddr = LOCAL_WORD_L2M_CH_2_TRAY_REPORT_VALUE; break;

		case eWordType_GoodReportValue1:					*plAddr = LOCAL_WORD_L2M_GOOD_REPORT_VALUE1; break;
		case eWordType_GoodReportValue2:					*plAddr = LOCAL_WORD_L2M_GOOD_REPORT_VALUE2; break;

		case eWordType_NgReportValue1:						*plAddr = LOCAL_WORD_L2M_NG_REPORT_VALUE1; break;
		case eWordType_NgReportValue2:						*plAddr = LOCAL_WORD_L2M_NG_REPORT_VALUE2; break;

		case eWordType_SampleReportValue1:					*plAddr = LOCAL_WORD_L2M_SAMPLE_REPORT_VALUE1; break;
		case eWordType_SampleReportValue2:					*plAddr = LOCAL_WORD_L2M_SAMPLE_REPORT_VALUE2; break;

		case eWordType_BufferReportValue1:					*plAddr = LOCAL_WORD_L2M_BUFFER_REPORT_VALUE1; break;
		case eWordType_BufferReportValue2:					*plAddr = LOCAL_WORD_L2M_BUFFER_REPORT_VALUE2; break;

		case eWordType_AZoneFpcID1:							*plAddr = LOCAL_WORD_L2M_AZONE_FPC_ID_1; break;
		case eWordType_AZoneFpcID2:							*plAddr = LOCAL_WORD_L2M_AZONE_FPC_ID_2; break;
		case eWordType_AZoneFpcID3:							*plAddr = LOCAL_WORD_L2M_AZONE_FPC_ID_3; break;
		case eWordType_AZoneFpcID4:							*plAddr = LOCAL_WORD_L2M_AZONE_FPC_ID_4; break;

		case eWordType_BZoneFpcID1:							*plAddr = LOCAL_WORD_L2M_BZONE_FPC_ID_1; break;
		case eWordType_BZoneFpcID2:							*plAddr = LOCAL_WORD_L2M_BZONE_FPC_ID_2; break;
		case eWordType_BZoneFpcID3:							*plAddr = LOCAL_WORD_L2M_BZONE_FPC_ID_3; break;
		case eWordType_BZoneFpcID4:							*plAddr = LOCAL_WORD_L2M_BZONE_FPC_ID_4; break;

		case eWordType_CZoneFpcID1:							*plAddr = LOCAL_WORD_L2M_CZONE_FPC_ID_1; break;
		case eWordType_CZoneFpcID2:							*plAddr = LOCAL_WORD_L2M_CZONE_FPC_ID_2; break;
		case eWordType_CZoneFpcID3:							*plAddr = LOCAL_WORD_L2M_CZONE_FPC_ID_3; break;
		case eWordType_CZoneFpcID4:							*plAddr = LOCAL_WORD_L2M_CZONE_FPC_ID_4; break;

		case eWordType_DZoneFpcID1:							*plAddr = LOCAL_WORD_L2M_DZONE_FPC_ID_1; break;
		case eWordType_DZoneFpcID2:							*plAddr = LOCAL_WORD_L2M_DZONE_FPC_ID_2; break;
		case eWordType_DZoneFpcID3:							*plAddr = LOCAL_WORD_L2M_DZONE_FPC_ID_3; break;
		case eWordType_DZoneFpcID4:							*plAddr = LOCAL_WORD_L2M_DZONE_FPC_ID_4; break;

		case eWordType_AZonePanel1:							*plAddr = LOCAL_WORD_L2M_AZONE_PANEL_1; break;
		case eWordType_AZonePanel2:							*plAddr = LOCAL_WORD_L2M_AZONE_PANEL_2; break;
		case eWordType_AZonePanel3:							*plAddr = LOCAL_WORD_L2M_AZONE_PANEL_3; break;
		case eWordType_AZonePanel4:							*plAddr = LOCAL_WORD_L2M_AZONE_PANEL_4; break;

		case eWordType_BZonePanel1:							*plAddr = LOCAL_WORD_L2M_BZONE_PANEL_1; break;
		case eWordType_BZonePanel2:							*plAddr = LOCAL_WORD_L2M_BZONE_PANEL_2; break;
		case eWordType_BZonePanel3:							*plAddr = LOCAL_WORD_L2M_BZONE_PANEL_3; break;
		case eWordType_BZonePanel4:							*plAddr = LOCAL_WORD_L2M_BZONE_PANEL_4; break;

		case eWordType_CZonePanel1:							*plAddr = LOCAL_WORD_L2M_CZONE_PANEL_1; break;
		case eWordType_CZonePanel2:							*plAddr = LOCAL_WORD_L2M_CZONE_PANEL_2; break;
		case eWordType_CZonePanel3:							*plAddr = LOCAL_WORD_L2M_CZONE_PANEL_3; break;
		case eWordType_CZonePanel4:							*plAddr = LOCAL_WORD_L2M_CZONE_PANEL_4; break;

		case eWordType_DZonePanel1:							*plAddr = LOCAL_WORD_L2M_DZONE_PANEL_1; break;
		case eWordType_DZonePanel2:							*plAddr = LOCAL_WORD_L2M_DZONE_PANEL_2; break;
		case eWordType_DZonePanel3:							*plAddr = LOCAL_WORD_L2M_DZONE_PANEL_3; break;
		case eWordType_DZonePanel4:							*plAddr = LOCAL_WORD_L2M_DZONE_PANEL_4; break;

		case eWordType_PreGammaFpcID1:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_FPC_ID_1; break;
		case eWordType_PreGammaFpcID2:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_FPC_ID_2; break;
		case eWordType_PreGammaFpcID3:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_FPC_ID_3; break;
		case eWordType_PreGammaFpcID4:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_FPC_ID_4; break;

		case eWordType_PreGammaPanel1:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_PANEL_1; break;
		case eWordType_PreGammaPanel2:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_PANEL_2; break;
		case eWordType_PreGammaPanel3:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_PANEL_3; break;
		case eWordType_PreGammaPanel4:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_PANEL_4; break;

		case eWordType_VisionFpcID1:						*plAddr = LOCAL_WORD_L2M_VISION_FPC_ID_1; break;
		case eWordType_VisionFpcID2:						*plAddr = LOCAL_WORD_L2M_VISION_FPC_ID_2; break;
		case eWordType_VisionFpcID3:						*plAddr = LOCAL_WORD_L2M_VISION_FPC_ID_3; break;
		case eWordType_VisionFpcID4:						*plAddr = LOCAL_WORD_L2M_VISION_FPC_ID_4; break;

		case eWordType_VisionPanel1:						*plAddr = LOCAL_WORD_L2M_VISION_PANEL_1; break;
		case eWordType_VisionPanel2:						*plAddr = LOCAL_WORD_L2M_VISION_PANEL_2; break;
		case eWordType_VisionPanel3:						*plAddr = LOCAL_WORD_L2M_VISION_PANEL_3; break;
		case eWordType_VisionPanel4:						*plAddr = LOCAL_WORD_L2M_VISION_PANEL_4; break;

		case eWordType_ViewingAngleFpcID1:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_FPC_ID_1; break;
		case eWordType_ViewingAngleFpcID2:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_FPC_ID_2; break;
		case eWordType_ViewingAngleFpcID3:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_FPC_ID_3; break;
		case eWordType_ViewingAngleFpcID4:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_FPC_ID_4; break;

		case eWordType_ViewingAnglePanel1:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_PANEL_1; break;
		case eWordType_ViewingAnglePanel2:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_PANEL_2; break;
		case eWordType_ViewingAnglePanel3:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_PANEL_3; break;
		case eWordType_ViewingAnglePanel4:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_PANEL_4; break;

		case eWordType_PreGammaIndexNum:					*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_INDEX_NUM; break;

		case eWordType_VisionInspectionPosition:			*plAddr = LOCAL_WORD_L2M_VISION_INSPECTION_POSITION; break;

		case eWordType_AutoFocusResult1:					*plAddr = LOCAL_WORD_L2M_M_STAGE_A_FPC_ID; break;
		case eWordType_AutoFocusResult2:					*plAddr = LOCAL_WORD_L2M_M_STAGE_B_FPC_ID; break;

		case eWordType_MStageAFpcID:						*plAddr = LOCAL_WORD_L2M_M_STAGE_A_FPC_ID; break;
		case eWordType_MStageBFpcID:						*plAddr = LOCAL_WORD_L2M_M_STAGE_B_FPC_ID; break;

		case eWordType_MStageAPanel:						*plAddr = LOCAL_WORD_L2M_M_STAGE_A_PANEL; break;
		case eWordType_MStageBPanel:						*plAddr = LOCAL_WORD_L2M_M_STAGE_B_PANEL; break;

		case eWordType_MStageABufferTrayINFlag:				*plAddr = LOCAL_WORD_L2M_M_STAGE_A_BUFFERTRAY_FLAG; break;
		case eWordType_MStageBBufferTrayINFlag:				*plAddr = LOCAL_WORD_L2M_M_STAGE_B_BUFFERTRAY_FLAG; break;

		case eWordType_UnloadOKDFSValue1:					*plAddr = LOCAL_WORD_L2M_UNLOAD_OK_DFS_VALUE1; break;
		case eWordType_UnloadOKDFSValue2:					*plAddr = LOCAL_WORD_L2M_UNLOAD_OK_DFS_VALUE2; break;

		case eWordType_UnloadNGDFSValue1:					*plAddr = LOCAL_WORD_L2M_UNLOAD_NG_DFS_VALUE1; break;
		case eWordType_UnloadNGDFSValue2:					*plAddr = LOCAL_WORD_L2M_UNLOAD_NG_DFS_VALUE2; break;

		case eWordType_ULD_OK_DefectCodeFpcID1:				*plAddr = LOCAL_WORD_L2M_ULD_OK_DEFECT_CODE_FPC_ID1; break;
		case eWordType_ULD_OK_DefectCodeFpcID2:				*plAddr = LOCAL_WORD_L2M_ULD_OK_DEFECT_CODE_FPC_ID2; break;

		case eWordType_ULD_OK_DefectCodePanelID1:			*plAddr = LOCAL_WORD_L2M_ULD_OK_DEFECT_CODE_PANEL_ID1; break;
		case eWordType_ULD_OK_DefectCodePanelID2:			*plAddr = LOCAL_WORD_L2M_ULD_OK_DEFECT_CODE_PANEL_ID2; break;

		case eWordType_ULD_NG_DefectCodeFpcID1:				*plAddr = LOCAL_WORD_L2M_ULD_NG_DEFECT_CODE_FPC_ID1; break;
		case eWordType_ULD_NG_DefectCodeFpcID2:				*plAddr = LOCAL_WORD_L2M_ULD_NG_DEFECT_CODE_FPC_ID2; break;

		case eWordType_ULD_NG_DefectCodePanelID1:			*plAddr = LOCAL_WORD_L2M_ULD_NG_DEFECT_CODE_PANEL_ID1; break;
		case eWordType_ULD_NG_DefectCodePanelID2:			*plAddr = LOCAL_WORD_L2M_ULD_NG_DEFECT_CODE_PANEL_ID2; break;

#elif _SYSTEM_GAMMA_
		case eWordType_DataReportValue1:					*plAddr = LOCAL_WORD_L2M_CH_1_DATA_REPORT_VALUE; break;
		case eWordType_DataReportValue2:					*plAddr = LOCAL_WORD_L2M_CH_2_DATA_REPORT_VALUE; break;

		case eWordType_NgReportValue1:						*plAddr = LOCAL_WORD_L2M_CH_1_TRAY_REPORT_VALUE; break;
		case eWordType_NgReportValue2:						*plAddr = LOCAL_WORD_L2M_CH_2_TRAY_REPORT_VALUE; break;

		case eWordType_GammaStage1FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_FPC_ID_1; break;
		case eWordType_GammaStage2FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_FPC_ID_1; break;
		case eWordType_GammaStage3FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_FPC_ID_1; break;
		case eWordType_GammaStage4FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_FPC_ID_1; break;
		case eWordType_GammaStage5FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_FPC_ID_1; break;
		case eWordType_GammaStage6FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_FPC_ID_1; break;
		case eWordType_GammaStage7FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_FPC_ID_1; break;
		case eWordType_GammaStage8FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_FPC_ID_1; break;
		case eWordType_GammaStage9FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_FPC_ID_1; break;
		case eWordType_GammaStage10FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_FPC_ID_1; break;
		case eWordType_GammaStage11FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_FPC_ID_1; break;
		case eWordType_GammaStage12FpcID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_FPC_ID_1; break;

		case eWordType_GammaStage1FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_FPC_ID_2; break;
		case eWordType_GammaStage2FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_FPC_ID_2; break;
		case eWordType_GammaStage3FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_FPC_ID_2; break;
		case eWordType_GammaStage4FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_FPC_ID_2; break;
		case eWordType_GammaStage5FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_FPC_ID_2; break;
		case eWordType_GammaStage6FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_FPC_ID_2; break;
		case eWordType_GammaStage7FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_FPC_ID_2; break;
		case eWordType_GammaStage8FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_FPC_ID_2; break;
		case eWordType_GammaStage9FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_FPC_ID_2; break;
		case eWordType_GammaStage10FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_FPC_ID_2; break;
		case eWordType_GammaStage11FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_FPC_ID_2; break;
		case eWordType_GammaStage12FpcID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_FPC_ID_2; break;

		case eWordType_GammaStage1PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_PANEL_ID_1; break;
		case eWordType_GammaStage2PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_PANEL_ID_1; break;
		case eWordType_GammaStage3PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_PANEL_ID_1; break;
		case eWordType_GammaStage4PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_PANEL_ID_1; break;
		case eWordType_GammaStage5PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_PANEL_ID_1; break;
		case eWordType_GammaStage6PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_PANEL_ID_1; break;
		case eWordType_GammaStage7PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_PANEL_ID_1; break;
		case eWordType_GammaStage8PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_PANEL_ID_1; break;
		case eWordType_GammaStage9PanelID1:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_PANEL_ID_1; break;
		case eWordType_GammaStage10PanelID1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_PANEL_ID_1; break;
		case eWordType_GammaStage11PanelID1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_PANEL_ID_1; break;
		case eWordType_GammaStage12PanelID1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_PANEL_ID_1; break;

		case eWordType_GammaStage1PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_PANEL_ID_2; break;
		case eWordType_GammaStage2PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_PANEL_ID_2; break;
		case eWordType_GammaStage3PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_PANEL_ID_2; break;
		case eWordType_GammaStage4PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_PANEL_ID_2; break;
		case eWordType_GammaStage5PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_PANEL_ID_2; break;
		case eWordType_GammaStage6PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_PANEL_ID_2; break;
		case eWordType_GammaStage7PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_PANEL_ID_2; break;
		case eWordType_GammaStage8PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_PANEL_ID_2; break;
		case eWordType_GammaStage9PanelID2:					*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_PANEL_ID_2; break;
		case eWordType_GammaStage10PanelID2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_PANEL_ID_2; break;
		case eWordType_GammaStage11PanelID2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_PANEL_ID_2; break;
		case eWordType_GammaStage12PanelID2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_PANEL_ID_2; break;

		case eWordType_GammaNGDFSValue1:					*plAddr = LOCAL_WORD_L2M_GAMMA_NG_DFS_VALUE1; break;
		case eWordType_GammaNGDFSValue2:					*plAddr = LOCAL_WORD_L2M_GAMMA_NG_DFS_VALUE2; break;

		case eWordType_GammaNGDefectCodeFpcID1:				*plAddr = LOCAL_WORD_L2M_GAMMA_NG_DEFECT_CODE_FPC_ID1; break;
		case eWordType_GammaNGDefectCodeFpcID2:				*plAddr = LOCAL_WORD_L2M_GAMMA_NG_DEFECT_CODE_FPC_ID2; break;

		case eWordType_GammaNGDefectCodePanelID1:			*plAddr = LOCAL_WORD_L2M_GAMMA_NG_DEFECT_CODE_PANEL_ID1; break;
		case eWordType_GammaNGDefectCodePanelID2:			*plAddr = LOCAL_WORD_L2M_GAMMA_NG_DEFECT_CODE_PANEL_ID2; break;

#endif

		case eWordType_SearchID:							*plAddr = LOCAL_WORD_L2M_SEARCH_ID; break;
		case eWordType_PM_Mode_LoginClear:					*plAddr = LOCAL_WORD_L2M_SEARCH_ID_INOUT; break;

		case eWordType_JobData1:							*plAddr = LOCAL_WORD_L2M_JOB_DATA1; break;
		case eWordType_JobData2:							*plAddr = LOCAL_WORD_L2M_JOB_DATA2; break;

		case eWordType_UnloadJobData1:						*plAddr = LOCAL_WORD_L2M_UNLOAD_JOB_DATA1; break;
		case eWordType_UnloadJobData2:						*plAddr = LOCAL_WORD_L2M_UNLOAD_JOB_DATA2; break;

		case eWordType_SearchPassWord:						*plAddr = LOCAL_WORD_L2M_SEARCH_PASSWORD; break;

		case eWordType_FFUSetValue:							*plAddr = LOCAL_WORD_L2M_FFU_SET_VALUE; break;

		case eWordType_DFSValue1:							*plAddr = LOCAL_WORD_L2M_DFS_VALUE1; break;
		case eWordType_DFSValue2:							*plAddr = LOCAL_WORD_L2M_DFS_VALUE2; break;

		case eWordType_DefectCodeFpcID1:					*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_FPC_ID1; break;
		case eWordType_DefectCodeFpcID2:					*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_FPC_ID2; break;

		case eWordType_DefectCodePanelID1:					*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_PANEL_ID1; break;
		case eWordType_DefectCodePanelID2:					*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_PANEL_ID2; break;

		//////////////////////////////////////////// PLC > PC WORD ////////////////////////////////////////////

		case eWordType_ModelNameDataChange:					*plAddr = LOCAL_WORD_L2M_MODEL_NAME_DATA_CHANGE; break;
		case eWordType_AlarmData:							*plAddr = LOCAL_WORD_L2M_ALARM_DATA; break;

		case eWordType_Align1Result1:						*plAddr = LOCAL_WORD_L2M_ALIGN1_RESULT1; break;
		case eWordType_Align2Result1:						*plAddr = LOCAL_WORD_L2M_ALIGN2_RESULT1; break;

		case eWordType_Align1Result2:						*plAddr = LOCAL_WORD_L2M_ALIGN1_RESULT2; break;
		case eWordType_Align2Result2:						*plAddr = LOCAL_WORD_L2M_ALIGN2_RESULT2; break;

		case eWordType_TrayCheckResult1:					*plAddr = LOCAL_WORD_L2M_TRAY_CHECK_RESULT1; break;
		case eWordType_TrayCheckResult2:					*plAddr = LOCAL_WORD_L2M_TRAY_CHECK_RESULT2; break;
		case eWordType_TrayCheckResult3:					*plAddr = LOCAL_WORD_L2M_TRAY_CHECK_RESULT3; break;

		case eWordType_TrayLowerAlignResult1:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN_RESULT1; break;
		case eWordType_TrayLowerAlignResult2:				*plAddr = LOCAL_WORD_L2M_TRAY_LOWER_ALIGN_RESULT2; break;

		case eWordType_TrayAlignResult1:					*plAddr = LOCAL_WORD_L2M_TRAY_LAIGN_RESULT1; break;
		case eWordType_TrayAlignResult2:					*plAddr = LOCAL_WORD_L2M_TRAY_LAIGN_RESULT2; break;
		case eWordType_TrayAlignResult3:					*plAddr = LOCAL_WORD_L2M_TRAY_LAIGN_RESULT3; break;

#if _SYSTEM_AMTAFT_
		case eWordType_AZoneContactOnResult:				*plAddr = LOCAL_WORD_L2M_AZONE_CONTACT_ON_RESULT; break;
		case eWordType_BZoneContactOnResult:				*plAddr = LOCAL_WORD_L2M_BZONE_CONTACT_ON_RESULT; break;
		case eWordType_CZoneContactOnResult:				*plAddr = LOCAL_WORD_L2M_CZONE_CONTACT_ON_RESULT; break;
		case eWordType_DZoneContactOnResult:				*plAddr = LOCAL_WORD_L2M_DZONE_CONTACT_ON_RESULT; break;

		case eWordType_AZoneContactOffResult:				*plAddr = LOCAL_WORD_L2M_AZONE_CONTACT_OFF_RESULT; break;
		case eWordType_BZoneContactOffResult:				*plAddr = LOCAL_WORD_L2M_BZONE_CONTACT_OFF_RESULT; break;
		case eWordType_CZoneContactOffResult:				*plAddr = LOCAL_WORD_L2M_CZONE_CONTACT_OFF_RESULT; break;
		case eWordType_DZoneContactOffResult:				*plAddr = LOCAL_WORD_L2M_DZONE_CONTACT_OFF_RESULT; break;

		case eWordType_AZone1stContactResult:				*plAddr = LOCAL_WORD_L2M_AZONE_1ST_CONTACT_ON_RESULT; break;
		case eWordType_BZone1stContactResult:				*plAddr = LOCAL_WORD_L2M_BZONE_1ST_CONTACT_ON_RESULT; break;
		case eWordType_CZone1stContactResult:				*plAddr = LOCAL_WORD_L2M_CZONE_1ST_CONTACT_ON_RESULT; break;
		case eWordType_DZone1stContactResult:				*plAddr = LOCAL_WORD_L2M_DZONE_1ST_CONTACT_ON_RESULT; break;

		case eWordType_AZoneTouchResult:					*plAddr = LOCAL_WORD_L2M_AZONE_TOUCH_RESULT; break;
		case eWordType_BZoneTouchResult:					*plAddr = LOCAL_WORD_L2M_BZONE_TOUCH_RESULT; break;
		case eWordType_CZoneTouchResult:					*plAddr = LOCAL_WORD_L2M_CZONE_TOUCH_RESULT; break;
		case eWordType_DZoneTouchResult:					*plAddr = LOCAL_WORD_L2M_DZONE_TOUCH_RESULT; break;

		case eWordType_PreGammaResult1:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RESULT1; break;
		case eWordType_PreGammaResult2:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RESULT2; break;
		case eWordType_PreGammaResult3:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RESULT3; break;
		case eWordType_PreGammaResult4:						*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RESULT4; break;

		case eWordType_PreGammaRankCode1:					*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RANK_CODE1; break;
		case eWordType_PreGammaRankCode2:					*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RANK_CODE2; break;
		case eWordType_PreGammaRankCode3:					*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RANK_CODE3; break;
		case eWordType_PreGammaRankCode4:					*plAddr = LOCAL_WORD_L2M_PRE_GAMMA_RANK_CODE4; break;

		case eWordType_VisionResult1:						*plAddr = LOCAL_WORD_L2M_VISION_RESULT1; break;
		case eWordType_VisionResult2:						*plAddr = LOCAL_WORD_L2M_VISION_RESULT2; break;
		case eWordType_VisionResult3:						*plAddr = LOCAL_WORD_L2M_VISION_RESULT3; break;
		case eWordType_VisionResult4:						*plAddr = LOCAL_WORD_L2M_VISION_RESULT4; break;

		case eWordType_ViewingAngleResult1:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_RESULT1; break;
		case eWordType_ViewingAngleResult2:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_RESULT2; break;
		case eWordType_ViewingAngleResult3:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_RESULT3; break;
		case eWordType_ViewingAngleResult4:					*plAddr = LOCAL_WORD_L2M_VIEWING_ANGLE_RESULT4; break;

		case eWordType_AutoFocusMoter1:						*plAddr = LOCAL_WORD_L2M_AUTO_FOCUS_MOTER1; break;
		case eWordType_AutoFocusMoter2:						*plAddr = LOCAL_WORD_L2M_AUTO_FOCUS_MOTER2; break;

		case eWordType_AutoFocusValue1:						*plAddr = LOCAL_WORD_L2M_AUTO_FOCUS_VALUE1; break;
		case eWordType_AutoFocusValue2:						*plAddr = LOCAL_WORD_L2M_AUTO_FOCUS_VALUE2; break;

		case eWordType_MStageAContactOnResult:				*plAddr = LOCAL_WORD_L2M_M_STAGE_A_CONTACT_ON_RESULT; break;
		case eWordType_MStageBContactOnResult:				*plAddr = LOCAL_WORD_L2M_M_STAGE_B_CONTACT_ON_RESULT; break;
		case eWordType_MStageAContactOffResult:				*plAddr = LOCAL_WORD_L2M_M_STAGE_A_CONTACT_OFF_RESULT; break;
		case eWordType_MStageBContactOffResult:				*plAddr = LOCAL_WORD_L2M_M_STAGE_B_CONTACT_OFF_RESULT; break;
		case eWordType_MStageAContactResetResult:			*plAddr = LOCAL_WORD_L2M_M_STAGE_A_CONTACT_RESET_RESULT; break;
		case eWordType_MStageBContactResetResult:			*plAddr = LOCAL_WORD_L2M_M_STAGE_B_CONTACT_RESET_RESULT; break;
		case eWordType_MStageAGammaResult:					*plAddr = LOCAL_WORD_L2M_M_STAGE_A_GAMMA_RESULT; break;
		case eWordType_MStageBGammaResult:					*plAddr = LOCAL_WORD_L2M_M_STAGE_B_GAMMA_RESULT; break;
		case eWordType_MStageATouchResult:					*plAddr = LOCAL_WORD_L2M_M_STAGE_A_TOUCH_RESULT; break;
		case eWordType_MStageBTouchResult:					*plAddr = LOCAL_WORD_L2M_M_STAGE_B_TOUCH_RESULT; break;
		case eWordType_MStageAOperatorViewResult:			*plAddr = LOCAL_WORD_L2M_M_STAGE_A_OPV_RESULT; break;
		case eWordType_MStageBOperatorViewResult:			*plAddr = LOCAL_WORD_L2M_M_STAGE_B_OPV_RESULT; break;

		case eWordType_UnloadOKDefectCodeResult1:			*plAddr = LOCAL_WORD_L2M_UNLOAD_OK_DEFECT_CODE_RESULT1; break;
		case eWordType_UnloadOKDefectCodeResult2:			*plAddr = LOCAL_WORD_L2M_UNLOAD_OK_DEFECT_CODE_RESULT2; break;
		case eWordType_UnloadOKDefectGradeResult1:			*plAddr = LOCAL_WORD_L2M_UNLOAD_OK_DEFECT_GRADE_RESULT1; break;
		case eWordType_UnloadOKDefectGradeResult2:			*plAddr = LOCAL_WORD_L2M_UNLOAD_OK_DEFECT_GRADE_RESULT2; break;
		case eWordType_UnloadNGDefectCodeResult1:			*plAddr = LOCAL_WORD_L2M_UNLOAD_NG_DEFECT_CODE_RESULT1; break;
		case eWordType_UnloadNGDefectCodeResult2:			*plAddr = LOCAL_WORD_L2M_UNLOAD_NG_DEFECT_CODE_RESULT2; break;
		case eWordType_UnloadNGDefectGradeResult1:			*plAddr = LOCAL_WORD_L2M_UNLOAD_NG_DEFECT_GRADE_RESULT1; break;
		case eWordType_UnloadNGDefectGradeResult2:			*plAddr = LOCAL_WORD_L2M_UNLOAD_NG_DEFECT_GRADE_RESULT2; break;

			//>>210422
		case eWordType_PGCodeCh1Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH1_RESULT; break;
		case eWordType_PGCodeCh2Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH2_RESULT; break;
		case eWordType_PGCodeCh3Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH3_RESULT; break;
		case eWordType_PGCodeCh4Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH4_RESULT; break;
		case eWordType_PGCodeCh5Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH5_RESULT; break;
		case eWordType_PGCodeCh6Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH6_RESULT; break;
		case eWordType_PGCodeCh7Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH7_RESULT; break;
		case eWordType_PGCodeCh8Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH8_RESULT; break;
		case eWordType_PGCodeCh9Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH9_RESULT; break;
		case eWordType_PGCodeCh10Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH10_RESULT; break;
		case eWordType_PGCodeCh11Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH11_RESULT; break;
		case eWordType_PGCodeCh12Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH12_RESULT; break;
		case eWordType_PGCodeCh13Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH13_RESULT; break;
		case eWordType_PGCodeCh14Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH14_RESULT; break;
		case eWordType_PGCodeCh15Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH15_RESULT; break;
		case eWordType_PGCodeCh16Result: *plAddr = LOCAL_WORD_L2M_PG_CODE_CH16_RESULT; break;

		case eWordType_TPCodeCh1Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH1_RESULT; break;
		case eWordType_TPCodeCh2Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH2_RESULT; break;
		case eWordType_TPCodeCh3Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH3_RESULT; break;
		case eWordType_TPCodeCh4Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH4_RESULT; break;
		case eWordType_TPCodeCh5Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH5_RESULT; break;
		case eWordType_TPCodeCh6Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH6_RESULT; break;
		case eWordType_TPCodeCh7Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH7_RESULT; break;
		case eWordType_TPCodeCh8Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH8_RESULT; break;
		case eWordType_TPCodeCh9Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH9_RESULT; break;
		case eWordType_TPCodeCh10Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH10_RESULT; break;
		case eWordType_TPCodeCh11Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH11_RESULT; break;
		case eWordType_TPCodeCh12Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH12_RESULT; break;
		case eWordType_TPCodeCh13Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH13_RESULT; break;
		case eWordType_TPCodeCh14Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH14_RESULT; break;
		case eWordType_TPCodeCh15Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH15_RESULT; break;
		case eWordType_TPCodeCh16Result: *plAddr = LOCAL_WORD_L2M_TP_CODE_CH16_RESULT; break;
			//<<

#elif _SYSTEM_GAMMA_

		case eWordType_GammaStage1ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage2ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage3ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage4ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage5ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage6ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage7ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage8ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage9ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage10ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage11ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_CONTACT_ON_RESULT; break;
		case eWordType_GammaStage12ContactOnResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_CONTACT_ON_RESULT; break;

		case eWordType_GammaStage1ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage2ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage3ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage4ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage5ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage6ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage7ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage8ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage9ContactOffResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage10ContactOffResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage11ContactOffResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_CONTACT_OFF_RESULT; break;
		case eWordType_GammaStage12ContactOffResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_CONTACT_OFF_RESULT; break;

		case eWordType_GammaStage1MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_MTP_RESULT1; break;
		case eWordType_GammaStage2MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_MTP_RESULT1; break;
		case eWordType_GammaStage3MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_MTP_RESULT1; break;
		case eWordType_GammaStage4MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_MTP_RESULT1; break;
		case eWordType_GammaStage5MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_MTP_RESULT1; break;
		case eWordType_GammaStage6MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_MTP_RESULT1; break;
		case eWordType_GammaStage7MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_MTP_RESULT1; break;
		case eWordType_GammaStage8MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_MTP_RESULT1; break;
		case eWordType_GammaStage9MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_MTP_RESULT1; break;
		case eWordType_GammaStage10MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_MTP_RESULT1; break;
		case eWordType_GammaStage11MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_MTP_RESULT1; break;
		case eWordType_GammaStage12MTPResult1:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_MTP_RESULT1; break;

		case eWordType_GammaStage1MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_MTP_RESULT2; break;
		case eWordType_GammaStage2MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_MTP_RESULT2; break;
		case eWordType_GammaStage3MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_MTP_RESULT2; break;
		case eWordType_GammaStage4MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_MTP_RESULT2; break;
		case eWordType_GammaStage5MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_MTP_RESULT2; break;
		case eWordType_GammaStage6MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_MTP_RESULT2; break;
		case eWordType_GammaStage7MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_MTP_RESULT2; break;
		case eWordType_GammaStage8MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_MTP_RESULT2; break;
		case eWordType_GammaStage9MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_MTP_RESULT2; break;
		case eWordType_GammaStage10MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_MTP_RESULT2; break;
		case eWordType_GammaStage11MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_MTP_RESULT2; break;
		case eWordType_GammaStage12MTPResult2:				*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_MTP_RESULT2; break;

		case eWordType_GammaStage1ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage2ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage3ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage4ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage5ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage6ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage7ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage8ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage9ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage10ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage11ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_CONTACT_ON_1ST_RESULT; break;
		case eWordType_GammaStage12ContactOn1stResult:		*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_CONTACT_ON_1ST_RESULT; break;

		case eWordType_GammaStage1PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_1_PID_CHECK_RESULT; break;
		case eWordType_GammaStage2PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_2_PID_CHECK_RESULT; break;
		case eWordType_GammaStage3PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_3_PID_CHECK_RESULT; break;
		case eWordType_GammaStage4PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_4_PID_CHECK_RESULT; break;
		case eWordType_GammaStage5PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_5_PID_CHECK_RESULT; break;
		case eWordType_GammaStage6PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_6_PID_CHECK_RESULT; break;
		case eWordType_GammaStage7PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_7_PID_CHECK_RESULT; break;
		case eWordType_GammaStage8PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_8_PID_CHECK_RESULT; break;
		case eWordType_GammaStage9PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_9_PID_CHECK_RESULT; break;
		case eWordType_GammaStage10PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_10_PID_CHECK_RESULT; break;
		case eWordType_GammaStage11PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_11_PID_CHECK_RESULT; break;
		case eWordType_GammaStage12PIDCheckResult:			*plAddr = LOCAL_WORD_L2M_GAMMA_STAGE_12_PID_CHECK_RESULT; break;
#endif

		case eWordType_IdResult:							*plAddr = LOCAL_WORD_L2M_SEARCH_ID_RESULT; break;
		case eWordType_PassWordResult:						*plAddr = LOCAL_WORD_L2M_SEARCH_PASSWORD_RESULT; break;

		case eWordType_FFUResult:							*plAddr = LOCAL_WORD_L2M_FFU_RESULT; break;

		case eWordType_DefectCodeResult1:					*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_RESULT1; break;
		case eWordType_DefectCodeResult2:					*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_RESULT2; break;

		case eWordType_DefectGradeResult1:					*plAddr = LOCAL_WORD_L2M_DEFECT_GRADE_RESULT1; break;
		case eWordType_DefectGradeResult2:					*plAddr = LOCAL_WORD_L2M_DEFECT_GRADE_RESULT2; break;

		case eWordType_DefectCodeResult1_NG:				*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_RESULT1_NG; break;
		case eWordType_DefectCodeResult2_NG:				*plAddr = LOCAL_WORD_L2M_DEFECT_CODE_RESULT2_NG; break;

		case eWordType_DefectGradeResult1_NG:				*plAddr = LOCAL_WORD_L2M_DEFECT_GRADE_RESULT1_NG; break;
		case eWordType_DefectGradeResult2_NG:				*plAddr = LOCAL_WORD_L2M_DEFECT_GRADE_RESULT2_NG; break;

		case eWordType_EsdValue1:							*plAddr = LOCAL_WORD_L2M_ESD_RESULT1; break;
		case eWordType_EsdValue2:							*plAddr = LOCAL_WORD_L2M_ESD_RESULT2; break;
		case eWordType_EsdValue3:							*plAddr = LOCAL_WORD_L2M_ESD_RESULT3; break;
	}
}

void MNetH::SetBitData(int nLocal, int nType, int nItem, BOOL bOn)
{
	long	lRet = 0;
	unsigned short	nAddr = 0;
	unsigned short	nPoints = 0;
	unsigned short	nLen = 0;
	unsigned short	*pnRBuf = NULL;
	int		nIndex = 0;

	GetPLCAddressBit(nLocal, nType, &nAddr);
	nAddr += nItem;

	nPoints = 1;
	pnRBuf = new unsigned short[nPoints];
	memset(pnRBuf, 0, sizeof(unsigned short)*nPoints);

	pnRBuf[0] = (USHORT)bOn;
	lRet = WriteLB(nAddr, nPoints, pnRBuf, sizeof(unsigned short)*nPoints);

	m_lastWriteAddrB.Format(_T("0x%x"), nAddr);

	delete[] pnRBuf; pnRBuf = NULL;
}

BOOL MNetH::GetBitData(int nLocal, int nType, int nItem)
{
	long	lRet = 0;
	unsigned short	nAddr = 0;
	unsigned short	nPoints = 0;
	unsigned short	nLen = 0;
	unsigned short	*pnRBuf = NULL;
	int		nIndex = 0;

	BOOL bIsOn = FALSE;

	GetPLCAddressBit(nLocal, nType, &nAddr);
	nAddr += nItem;

	nPoints = 1;
	pnRBuf = new unsigned short[nPoints];
	memset(pnRBuf, 0, sizeof(unsigned short)*nPoints);

	lRet = ReadLB(nAddr, nPoints, pnRBuf, sizeof(unsigned short)*nPoints);

	m_lastReadAddrB.Format(_T("0x%x"), nAddr);

	if (!lRet) { bIsOn = pnRBuf[0]; }
	delete[] pnRBuf; pnRBuf = NULL;

	return bIsOn;
}

long MNetH::GetWordData(int nLocalNo, int nType, void* pWordData, int dataSize, int addressOffset)
{
	long			lRet = 0;
	long			lAddr = 0;
	long			lPoints = 0;
	unsigned short	*pnRBuf = NULL;

	int iTypeSize = sizeof(unsigned short);
	lPoints = dataSize / iTypeSize;

	GetPLCAddressWord(nLocalNo, nType, &lAddr);

	pnRBuf = new unsigned short[lPoints];

	memset(pnRBuf, 0, iTypeSize *lPoints);
	lRet = ReadLWEx(lAddr + addressOffset, m_lNetwork, m_nCurLocal, lPoints, pnRBuf, iTypeSize*lPoints);

	m_lastReadAddrW.Format(_T("0x%x"), lAddr + addressOffset);

	//Wxxx0~lPoints
	memcpy_s(pWordData, lPoints*iTypeSize, pnRBuf, lPoints*iTypeSize);

	delete[] pnRBuf;
	pnRBuf = NULL;

	return lRet;
}

long MNetH::SetWordData(int nLocalNo, int nType, void* pWordData, int dataSize, int addressOffset)
{
	long			lRet = 0;
	long			lAddr = 0;
	long			lPoints = 0;
	unsigned short	*pnRBuf = NULL;

	int iTypeSize = sizeof(unsigned short);
	lPoints = dataSize / iTypeSize;

	GetPLCAddressWord(nLocalNo, nType, &lAddr);

	pnRBuf = new unsigned short[lPoints];

	memset(pnRBuf, 0, iTypeSize *lPoints);

	lRet = WriteLWEx(lAddr + addressOffset, m_lNetwork, nLocalNo, lPoints, (USHORT*)pWordData, iTypeSize*lPoints);

	m_lastWriteAddrW.Format(_T("0x%x"), lAddr + addressOffset);

	delete[] pnRBuf;
	pnRBuf = NULL;

	return lRet;
}

long MNetH::GetModelNameData(ModelNameData* pModelNameData)
{
	return GetWordData(m_nCurLocal, eWordType_ModelRecipeData, pModelNameData, sizeof(ModelNameData));
}

#if _SYSTEM_AMTAFT_
BOOL MNetH::GetCurrentIndexZone(int addr)
{
	return GetBitData(m_nCurLocal, eBitType_CurrentIndexZone, addr);
}
#endif

void MNetH::SetWordResultOffSet(int type, int addressOffset, void *result)
{
	SetWordData(m_nCurLocal, type, result, sizeof(unsigned short), addressOffset);
}

void MNetH::GetWordResultOffSet(int type, int addressOffset, void *result)
{
	GetWordData(m_nCurLocal, type, result, sizeof(unsigned short), addressOffset);
}

BOOL MNetH::GetPlcBitData(int type, int addr)
{
	return GetBitData(m_nCurLocal, type, addr);
}

void MNetH::SetPlcBitData(int type, int addr, BOOL bOn)
{
	SetBitData(m_nCurLocal, type, addr, bOn);
}

long MNetH::GetPlcWordData(int type, void *result)
{
	return GetWordData(m_nCurLocal, type, result, sizeof(unsigned short));
}

void MNetH::SetPlcWordData(int type, void *result)
{
	SetWordData(m_nCurLocal, type, result, sizeof(unsigned short));
} 
void MNetH::SetTrayLowerAlignResult(int type, TrayLowerAlignResult* pTrayLowerAlignResult)
{
	SetWordData(m_nCurLocal, type, pTrayLowerAlignResult, sizeof(TrayLowerAlignResult));
}

void MNetH::SetTrayCheckResult(int type, TrayCheckResult* pTrayCheckResult)
{
	SetWordData(m_nCurLocal, type, pTrayCheckResult, sizeof(TrayCheckResult));
}

void MNetH::SetAlignResult(int type, AlignResult* pAlignResult)
{
	SetWordData(m_nCurLocal, type, pAlignResult, sizeof(AlignResult));
}

void MNetH::SetEsdData(int type, EsdPlcData* pEsdData)
{
	SetWordData(m_nCurLocal, type, pEsdData, sizeof(EsdPlcData));
}

void MNetH::SetAutoFocusData(int type, AutoFocusData* pAutoFocusData)
{
	SetWordData(m_nCurLocal, type, pAutoFocusData, sizeof(AutoFocusData));
}

long MNetH::GetPanelData(int type, PanelData* pPanelData)
{
	return GetWordData(m_nCurLocal, type, pPanelData, sizeof(PanelData));
}

long MNetH::GetFpcIdData(int type, FpcIDData* pFpcIDData)
{
	return GetWordData(m_nCurLocal, type, pFpcIDData, sizeof(FpcIDData));
}

long MNetH::GetCardReaderIDData(int type, CardReaderID* pCardReaderID)
{
	return GetWordData(m_nCurLocal, type, pCardReaderID, sizeof(CardReaderID));
}

long MNetH::GetCardReaderPassWordData(int type, CardReaderPassWord* pCardReaderPassWord)
{
	return GetWordData(m_nCurLocal, type, pCardReaderPassWord, sizeof(CardReaderPassWord));
}

void MNetH::SetCardReaderUserData(int type, LoginUserData* pLoginUserData)
{
	SetWordData(m_nCurLocal, type, pLoginUserData, sizeof(LoginUserData));
}

long MNetH::GetCardReaderUserData(int type, LoginUserData* pLoginUserData)
{
	return GetWordData(m_nCurLocal, type, pLoginUserData, sizeof(LoginUserData));
}

long MNetH::GetModuleData(int type, ModuleData* pModuleData)
{
	return GetWordData(m_nCurLocal, type, pModuleData, sizeof(ModuleData));
}

long MNetH::GetJobData(int type, JobData* pJobData)
{
	return GetWordData(m_nCurLocal, type, pJobData, sizeof(JobData));
}

void MNetH::SetAlarmTextData(int type, AlarmTextData* pAlarmTextData)
{
	SetWordData(m_nCurLocal, type, pAlarmTextData, sizeof(AlarmTextData));
}

void MNetH::SetFFUData(int type, FFUData* pFFUData)
{
	SetWordData(m_nCurLocal, type, pFFUData, sizeof(FFUData));
}

long MNetH::GetDfsData(int type, DfsData* pDfsData)
{
	return GetWordData(m_nCurLocal, type, pDfsData, sizeof(DfsData));
}

long MNetH::SetPanelData(int type, PanelData* pPanelData)
{
	return SetWordData(m_nCurLocal, type, pPanelData, sizeof(PanelData));
}

long MNetH::GetDataStatus(int type, void *result)
{
	return GetWordData(m_nCurLocal, type, result, sizeof(DataStatus));
}

long MNetH::ULDGetDataStatus(int type, void *result)
{
	return GetWordData(m_nCurLocal, type, result, sizeof(UnloaderDataStatus));
}

long MNetH::SetDefectRankData(int type, DefectCodeRank* pDefectCodeRank)
{
	return SetWordData(m_nCurLocal, type, pDefectCodeRank, sizeof(DefectCodeRank));
}

long MNetH::SetDefectGradeRankData(int type, DefectGradeRank* pDefectGradeRank)
{
	return SetWordData(m_nCurLocal, type, pDefectGradeRank, sizeof(DefectCodeRank));
}

long MNetH::SetPanelOkGreadeData(int type, PanelOkGrade* pPanelOkGrade)
{
	return SetWordData(m_nCurLocal, type, pPanelOkGrade, sizeof(PanelOkGrade));
}

long MNetH::SetDefectRankDataOffSet(int type, int addressOffset, DefectCodeRank* pDefectCodeRank)
{
	return SetWordData(m_nCurLocal, type, pDefectCodeRank, sizeof(DefectCodeRank), addressOffset);
}

long MNetH::GetAxisRecipeIDData(int type, AxisRecipeID* pAxisRecipeID)
{
	return GetWordData(m_nCurLocal, type, pAxisRecipeID, sizeof(AxisRecipeID));
}

long MNetH::GetAxisModifyPositionData(int type, AxisModifyPosition* pAxisModifyPosition)
{
	return GetWordData(m_nCurLocal, type, pAxisModifyPosition, sizeof(AxisModifyPosition));
}

long MNetH::GetAxisModifyBeforePositionData(int type, AxisModifyBeforePosition* pAxisModifyBeforePosition)
{
	return GetWordData(m_nCurLocal, type, pAxisModifyBeforePosition, sizeof(AxisModifyPosition));
}

long MNetH::GetAlignAxisData(int type, AlignAxisData* pAlignAxisData)
{
	return GetWordData(m_nCurLocal, type, pAlignAxisData, sizeof(AlignAxisData));
}

long MNetH::GetOperateTimeData(int type, OperateTime* pOperateTimeData)
{
	return GetWordData(m_nCurLocal, type, pOperateTimeData, sizeof(OperateTime));
}

void MNetH::SetComViewWordData(int addr, void *result)
{
	long			lRet = 0;
	long			lAddr = 0;
	long			lPoints = 0;
	unsigned short	*pnRBuf = NULL;

	int iTypeSize = sizeof(unsigned short);
	lPoints = 1;// / iTypeSize;

	pnRBuf = new unsigned short[lPoints];
	lAddr = addr;

	memset(pnRBuf, 0, iTypeSize *lPoints);

	lRet = WriteLWEx(lAddr, m_lNetwork, m_nCurLocal, lPoints, (USHORT*)result, iTypeSize*lPoints);

	m_lastWriteAddrW.Format(_T("0x%x"), lAddr);

	delete[] pnRBuf;
	pnRBuf = NULL;
}

long MNetH::GetAddrWordData(int addr, void* pPanelData, int nSize)
{
	long			lRet = 0;
	long			lAddr = 0;
	long			lPoints = 0;
	unsigned short	*pnRBuf = NULL;

	int iTypeSize = sizeof(unsigned short);
	lPoints = nSize / iTypeSize;

	pnRBuf = new unsigned short[lPoints];
	lAddr = addr;

	memset(pnRBuf, 0, iTypeSize *lPoints);
	lRet = ReadLWEx(lAddr, m_lNetwork, m_nCurLocal, lPoints, pnRBuf, iTypeSize*lPoints);

	m_lastReadAddrW.Format(_T("0x%x"), lAddr);

	//Wxxx0~lPoints
	memcpy_s(pPanelData, lPoints*iTypeSize, pnRBuf, lPoints*iTypeSize);

	delete[] pnRBuf;
	pnRBuf = NULL;

	return lRet;
}


long MNetH::GetAddrWordModelData(int addr, ModelData* pModelNameData)
{
	long			lRet = 0;
	long			lAddr = 0;
	long			lPoints = 0;
	unsigned short	*pnRBuf = NULL;

	int iTypeSize = sizeof(unsigned short);
	lPoints = sizeof(ModelData) / iTypeSize;

	pnRBuf = new unsigned short[lPoints];
	lAddr = addr;

	memset(pnRBuf, 0, iTypeSize *lPoints);
	lRet = ReadLWEx(lAddr, m_lNetwork, m_nCurLocal, lPoints, pnRBuf, iTypeSize*lPoints);

	m_lastReadAddrW.Format(_T("0x%x"), lAddr);

	//Wxxx0~lPoints
	memcpy_s(pModelNameData, lPoints*iTypeSize, pnRBuf, lPoints*iTypeSize);

	delete[] pnRBuf;
	pnRBuf = NULL;

	return lRet;
}

long MNetH::GetComViewWordData(int addr, void *result)
{
	long			lRet = 0;
	long			lAddr = 0;
	long			lPoints = 0;
	unsigned short	*pnRBuf = NULL;

	int iTypeSize = sizeof(unsigned short);
	lPoints = 2; // iTypeSize;

	pnRBuf = new unsigned short[lPoints];
	lAddr = addr;

	memset(pnRBuf, 0, iTypeSize *lPoints);
	lRet = ReadLWEx(lAddr, m_lNetwork, m_nCurLocal, lPoints, pnRBuf, iTypeSize*lPoints);

	m_lastReadAddrW.Format(_T("0x%x"), lAddr);

	//Wxxx0~lPoints
	memcpy_s(result, lPoints*iTypeSize, pnRBuf, lPoints*iTypeSize);

	delete[] pnRBuf;
	pnRBuf = NULL;

	return lRet;
}

void MNetH::SetComViewBitData(int addr, BOOL bOn)
{
	long	lRet = 0;
	unsigned short	nAddr = 0;
	unsigned short	nPoints = 0;
	unsigned short	nLen = 0;
	unsigned short	*pnRBuf = NULL;
	int		nIndex = 0;

	nAddr = addr;

	nPoints = 1;
	pnRBuf = new unsigned short[nPoints];
	memset(pnRBuf, 0, sizeof(unsigned short)*nPoints);

	pnRBuf[0] = (USHORT)bOn;
	lRet = WriteLB(nAddr, nPoints, pnRBuf, sizeof(unsigned short)*nPoints);

	m_lastWriteAddrB.Format(_T("0x%x"), nAddr);

	delete[] pnRBuf; pnRBuf = NULL;
}

long MNetH::GetComViewBitData(int addr)
{
	long	lRet = 0;
	unsigned short	nAddr = 0;
	unsigned short	nPoints = 0;
	unsigned short	nLen = 0;
	unsigned short	*pnRBuf = NULL;
	int		nIndex = 0;

	BOOL bIsOn = FALSE;

	nAddr = addr;

	nPoints = 1;
	pnRBuf = new unsigned short[nPoints];
	memset(pnRBuf, 0, sizeof(unsigned short)*nPoints);

	lRet = ReadLB(nAddr, nPoints, pnRBuf, sizeof(unsigned short)*nPoints);

	m_lastReadAddrB.Format(_T("0x%x"), nAddr);

	if (!lRet) { bIsOn = pnRBuf[0]; }
	delete[] pnRBuf; pnRBuf = NULL;

	return bIsOn;
}