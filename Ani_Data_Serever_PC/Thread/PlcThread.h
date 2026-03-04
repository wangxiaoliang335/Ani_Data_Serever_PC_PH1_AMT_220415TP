#pragma once

#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"
#include "TimeCheck.h"
#include "StringSupport.h"
#include "EZini.h"
#include "TopCtrl.h"

class CPlcThread
{
public:
	CPlcThread();
	virtual ~CPlcThread();
	void ThreadRun();

	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThreadPlc;
	CWinThread *m_pThreadmHeartBit;
	void HeartBitCloseTask();
	BOOL HeartBitCreateTask();
	static UINT HeartBitThreadProc(LPVOID pParam);
	//UINT HeartBitThreadProc(LPVOID pParam);
	static UINT PlcThreadProc(LPVOID pParam);
	void HeartBitThreadRun();
	HANDLE m_hQuit;

	void TactThreadRun();
	BOOL TactCreateTask();
	void TactCloseTask();

	CWinThread *m_pTactThreadPlc;
	static UINT TactPlcThreadProc(LPVOID pParam);
	HANDLE m_hQuitTact;

	void SetModelData(ModelNameData pModelName);
	void SetModelCreate(ModelNameData pModelName);
	void SetModelChangeMethod(CString strModelName);
	void ModelCheckMethod();

	void ModelCreateChange(CString sendMsg, int iCommand);

	void LogWrite(CString strContents);

	BOOL m_plcStart;
	void ProgramStartStopLog();
	void ModelCreateChangeIniSave(CString strTilteName, int iChangeCreate);
	void ModelCreateChangeIniLoad();
	void ModelCreateChangeModify(CString strTitleName, CString strValueName, BOOL bValue);

	void CardReaderIdPasswordCheck(int iCommand);
	
	
	AlignResult m_AlignResultReset;
	TrayCheckResult m_trayCheckResultReset;
	TrayLowerAlignResult m_trayLowerAlignReslutReset;

	vector<AlarmDataItem> m_vecAlarmReset;

	int m_iPriviousIndexNum = 0;
	BOOL m_bFirstCheck;
	BOOL m_bTestCheck;
	BOOL m_bStartFlag;
	BOOL m_bAlarmStart;
	BOOL m_bAlarmReset;
	BOOL m_AxisStart;
	BOOL m_bModelStart;
	BOOL m_bModelFirstCheck;
	BOOL m_OperateTimeStart;
	BOOL m_bBCDataExistCheck;
	BOOL m_bPlcHeartBitFlag;
	long m_AlarmCodeResult;
	long m_TactTimeStart;
	long m_TactTimeStartFlag[50];
	BOOL m_bTest;
	BOOL m_bAlarmCountFlag;
	
	BOOL m_bDataFlag[PanelMaxCount];
	BOOL m_bTrayDataFlag[ChMaxCount];
	BOOL m_bUnloaderDataFlag[UnloaderMaxDataCount][ChMaxCount];

	BOOL m_bCardReaderIDStart;
	BOOL m_bCardReaderPassWordStart;
	long m_lPmModeLoginClear;
	BOOL m_bDfsStart[ChMaxCount];
	BOOL m_bDfsStartOK[ChMaxCount];
	BOOL m_bDfsStartNG[ChMaxCount];
	BOOL m_bDefectCodeStart[ChMaxCount];
	BOOL m_bULD_OK_DefectCodeStart[ChMaxCount];
	BOOL m_bULD_NG_DefectCodeStart[ChMaxCount];
	BOOL m_bJobDataStart[ChMaxCount];

	BOOL m_bPanelOutFlag;
	BOOL m_bBzonePassMode;
	BOOL m_bBZoneComplateFlag;
	long m_lBZoneIndexNum;
	BOOL m_bProductDataSaveFlag[3];

	CCriticalSection m_csData;
	CCriticalSection m_csTact;
	CCriticalSection m_csDFS;
	CCriticalSection m_csSumDFS;
	CCriticalSection m_csDefectCode;
	CCriticalSection m_csSumDefectCode;

	void AlarmDataParser();
	void AlarmResetInfo();
	void AxisDataParser();
	void OperateTimeParser();
	void TactTimeStartEndReset(int TactTimeUnit, int TactTimeStatus);
	void SetAlarmRankCount(AlarmDataItem alarmData, int nShift);
	void DefectRankClear();
	CString GammaTuningTimeParser(CString strTime);

	void CardReaderIDSerarch();
	void CardReaderPassWordSerarch();
	void CardReaderCsvFileSave(IDCardReader cardReader);

	void JobDataStart(int iNum, int iType);
	void SumDefectCodeStart(int iNum, int iType, int iOkNg);
	void SumDFSDataStart(int iNum, int iOkNg, int iType);
	void ProductDataSave(BOOL bFlag);
	CString DFSDataTimeParser(USHORT Time1, USHORT Time2, USHORT Time3);
	CString DFSDataTactTimeParser(USHORT Time1, USHORT Time2, USHORT Time3, USHORT Time4);
#if _SYSTEM_AMTAFT_
	void DFSDataStart(int iNum, int iOkNg, int iType);
	void DefectCodeStart(int iNum);
	void AOIInspectDataParser(int iPanelNum, int iCommand);
	void ULDInspectDataParser(int iPanelNum, int iCommand);
	void SendPlcDefectCode(int iNum, DfsDataValue PanelData, int iType);
#else
	void GAMMAInspectDataParser(int iPanelNum, int iCommand);
#endif
};
