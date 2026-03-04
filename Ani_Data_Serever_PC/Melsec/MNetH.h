//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for T7-2
// MNetH.h: interface for the MNetH class.
//
//
//		Version		Updated		 Author		 Note
//      -------     -------      ------      ----
//		   1.0      2005/12/20   Grouchy	 Create	
//		   1.1      2006/02/07   cha		 Modify
//		   1.2      2006/02/09   cha		 Modify(TrimSpace추가)
//		   1.3      2006/02/15   cha		 Modify(FillSpace수정)
//		   1.4      2006/02/27   cha		 SetJobOrder_ToLowerEqWord()함수 추가
//		   1.5      2015/11/16   JSLee       E4 Melsec Function 추가.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MNETH_H__50FAA8A4_1B84_45A6_88C5_D84DFE668D13__INCLUDED_)
#define AFX_MNETH_H__50FAA8A4_1B84_45A6_88C5_D84DFE668D13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		DEFAULT_CHANNEL		51
#define		DEFAULT_STATION		255

#define		RV_SUCCESS			0


#include "MNetHData.h"

class MNetHDlg;

class MNetH //: public virtual ICommon
{
public:
	MNetH(CString sIniFile= _T(".\\Melsec\\MNetH.ini"));	//
	virtual ~MNetH();
	
	void DestroyDlg();

public:
	int				m_nChannel;

	int				m_nStation;

	long			m_lPath;
	BOOL			m_fActive;
	CString			m_sIniFile;

	CStringArray	m_asLocalName;
	CStringArray	m_asUnitName;
	
	int				m_nCurLocal;
	int				m_nCurUnit;
	int				m_nPrevLocal;
	int				m_nNextLocal;



	int				m_nToUpperEqQty;
	int				m_nToLowerEqQty;

	//<<130717 kmh
	long			m_lNetwork;
	long			m_lStation;
	long			lBufSize;
	//<<


	bool			m_bSetGlassDataFlag;

	int				m_nT1;
	int				m_nT2;
	int				m_nT3;
	int				m_nDefaultTimeOut;
	DWORD			m_dwTransferStart;

	//	Common Variable for XSECNetDlg
	MNetHDlg		*m_pDlg;		//	Dialog Box Handle
	CWnd			*m_pMainWnd;	//	Main Window Handle
	bool			m_bShow;		//	Show type(true:Visible, false:Invisible)
	bool			m_bUseDialog;
	bool			m_bUseInterface; //151213 JSLee

	CString m_lastReadAddrW;
	CString m_lastWriteAddrW;
	CString m_lastReadAddrB;
	CString m_lastWriteAddrB;

public:
	MNetHDlg**		GetMNetHDlg();

	unsigned short	MelsecOpen(); 
	unsigned short	MelsecClose(); 

private:
	//HANDLE		m_hMutex;
	CCriticalSection m_csMelsec;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Data Handling.
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	CString		MakeString(unsigned short nValue, bool bSwap=true);
	long MakeunsignedshortArray(CString sData, unsigned short *pnArray, unsigned short nLen, unsigned short nIndex, bool bSwap);

public:
	long GetWordData(int nLocalNo, int nType, void* pWordData, int dataSize, int addressOffset = 0);
	long SetWordData(int nLocalNo, int nType, void* pWordData, int dataSize, int addressOffset = 0);

	void SetBitData(int nLocal, int nType, int nItem, BOOL bOn);
	BOOL GetBitData(int nLocal, int nType, int nItem);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize & Basic Function
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	long	Start();
	long	Stop();
	BOOL	IsConnected();
	void	ViewVisible(bool bView);
	void	MonitoringPLCArea();

	//160315 JYLee
	int			ReadConfig();
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Data Conversion Function
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	long	HexToBin(CString sHex, CString& sBin);
	long	BinToHex(CString sBin, CString& sHex);
	long	HexToDec(CString sHex, long& lDec);
	long	HexToDec(CString sHex, CString& sDec);
	long	DecToHex(long lDec, CString& sHex);
	long	DecToHex(CString sDec, CString& sHex);
	// Add Function by cha 2006/02/07 
	void	AscToString(TCHAR *pszOut, unsigned short *pnaBuf, unsigned short nPoints);
	void	AscToString(char *pszOut, unsigned short *pnaBuf, unsigned short nPoints); //151207 JSLee
	void	StringToAsc(TCHAR *pszIn, unsigned short *pnabuf, unsigned short nPoints);
	void	StringToAsc(char *pszIn, unsigned short *pnabuf, unsigned short nPoints); //151207 JSLee
	void	NewStringToAsc(TCHAR *pszIn, short *pnabuf, long lPoints);
	void	FillSpace(TCHAR *pszIn, unsigned short nStr);
	void	FillSpace(char *pszIn, unsigned short nStr); //151207 JSLee
	void	TrimSpace(TCHAR *pszIn);
	void	TrimSpace(char *pszIn); //151207 JSLee

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Read Function
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	long	ReadLB(unsigned short nAddr, unsigned short nPoints, unsigned short *nRBuf, unsigned short nBufSize);
	long	ReadLW(unsigned short nAddr, unsigned short nPoints, unsigned short *nRBuf, unsigned short nBufSize);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ReadEX Function
	////////////////////////////////////////////////////////////////////////////////////////////////////////
//>> 130717 kmh

	//long	ReadLBEx(unsigned short nAddr, unsigned short nPoints, unsigned short *nRBuf, unsigned short nBufSize);
	// bit 영역은 일단 pass.
	//long	ReadLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, short *pnRBuf, long nBufSize);
	long	ReadLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *pnRBuf, long nBufSize); //151117 JSLee short -> unsigned short
	long	ReadZREx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *pnRBuf, long nBufSize); //<< 20160824 kang
	//<<


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write Function 
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	long	WriteLB(unsigned short nAddr, unsigned short nPoints, unsigned short *nRBuf, unsigned short nBufSize);
	long	WriteLW(unsigned short nAddr, unsigned short nPoints, unsigned short *nRBuf, unsigned short nBufSize);
	long	TrayWriteLW(unsigned short nAddr,unsigned short nPoints,unsigned short *nRBuf,unsigned short nBufSize); //130704 kmh
			// traywritelw 필요 없음 삭제 정리 할 예정.
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write Function 
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	long	WriteLBEx(unsigned short nAddr, unsigned short nPoints, unsigned short *nRBuf, unsigned short nBufSize);
	// bit 영역은 일단 pass.
	//130717 kmh
	//long	WriteLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, short *nRBuf, long nBufSize);
	long	WriteLWEx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *nRBuf, long nBufSize); //151117 JSLee short -> unsigned short
	long	WriteZREx(long lAddr, long m_lNetwork, long m_lStation, long lPoints, unsigned short *nRBuf, long nBufSize); //<< 20160824 kang

	void GetPLCAddressBit(int nLocal, int nType, unsigned short *pnAddr); //151116 JSLee
	void GetPLCAddressWord(int nLocal, int nType, long *plAddr); //151116 JSLee

	long GetModelNameData(ModelNameData* pModelNameData);
#if _SYSTEM_AMTAFT_
	BOOL GetCurrentIndexZone(int addr);
#endif
	void SetWordResultOffSet(int type, int addressOffset, void *result);
	void GetWordResultOffSet(int type, int addressOffset, void *result);

	//<< 공통사용  (Vision, Line Scan , Viewing Angle , Align)
	BOOL GetPlcBitData(int type, int addr);
	void SetPlcBitData(int type, int addr, BOOL bOn);

	long GetPlcWordData(int type, void *result);
	void SetPlcWordData(int type, void *result);

	long GetPanelData(int type, PanelData* pPanelData);
	long SetPanelData(int type, PanelData* pPanelData);

	long GetFpcIdData(int type, FpcIDData* pFpcIDData);

	long GetCardReaderIDData(int type, CardReaderID* pCardReaderID);
	long GetCardReaderPassWordData(int type, CardReaderPassWord* pCardReaderPassWord);

	void SetCardReaderUserData(int type, LoginUserData* pLoginUserData);
	long GetCardReaderUserData(int type, LoginUserData* pLoginUserData);

	long GetModuleData(int type, ModuleData* pModuleData);
	long GetJobData(int type, JobData* pJobData);

	void SetAlarmTextData(int type, AlarmTextData* pAlarmTextData);

	void SetAutoFocusData(int type, AutoFocusData* pAutoFocusData);

	void SetFFUData(int type, FFUData* pFFUData);
	void SetEsdData(int type, EsdPlcData* pEsdData);

	long GetDfsData(int type, DfsData* pDfsData);
	
	//>> 공통사용

	long GetDataStatus(int type, void *result);
	long ULDGetDataStatus(int type, void *result);

	void SetAlignResult(int type, AlignResult* pAlignResult);
	void SetTrayCheckResult(int type, TrayCheckResult* TrayCheckResult);
	void SetTrayLowerAlignResult(int type, TrayLowerAlignResult* pTrayLowerAlignResult);
	void SetComViewWordData(int addr, void *result);
	long GetComViewWordData(int addr, void *result);

	void SetComViewBitData(int addr, BOOL bOn);
	long GetComViewBitData(int addr);

	long GetAddrWordData(int addr, void* pPanelData, int nSize);

	long GetAddrWordModelData(int addr, ModelData* pModelNameData);

	long SetDefectRankData(int addr, DefectCodeRank* pDefectCodeRank);
	long SetDefectGradeRankData(int addr, DefectGradeRank* pDefectGradeRank);
	long SetDefectRankDataOffSet(int addr, int addressOffset, DefectCodeRank* pDefectCodeRank);

	long SetPanelOkGreadeData(int addr, PanelOkGrade* pPanelOkGrade);

	
	long GetAxisRecipeIDData(int type, AxisRecipeID* pAxisRecipeID);
	long GetAxisModifyPositionData(int type, AxisModifyPosition* pAxisModifyPosition);
	long GetAxisModifyBeforePositionData(int type, AxisModifyBeforePosition* pAxisModifyBeforePosition);

	long GetAlignAxisData(int type, AlignAxisData* pAlignAxisData);

	long GetOperateTimeData(int type, OperateTime* pOperateTimeData);
};

#endif // !defined(AFX_MNETH_H__50FAA8A4_1B84_45A6_88C5_D84DFE668D13__INCLUDED_)
