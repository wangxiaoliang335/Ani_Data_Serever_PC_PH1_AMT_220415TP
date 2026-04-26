
// Ani_Data_Serever_PCApp.h : CAni_Data_Serever_PCApp
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH stdafx.h"
#endif

#include "resource.h"       //
#include "stdafx.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "Logger.h"
#include "MsgBox.h"
#include "AlignThread.h"
#include "AlignManager.h"
#include "EqInterface.h"
#include "PlcThread.h"
#include "PgManager.h"
#include "TpManager.h"
#include "OpvManager.h"
#include "LightingEventHandler.h"
#include "LightingSocketClient.h"

#include "Util/DataModels.h"
#include "Util/DBInterface.h"
#include "DFS/DFSInfo.h"  // 包含完整的 DFSInfo 定义，包括 SDFSDefectDataBegin 和 LUMITOP_SDFSDefectDataBegin
#include "DFSClient.h"
#include "FTPClient.h"
#include "TLSConnection.h"
#include "Migration.h"  // 包含 m_codeOk, m_codeNg 等常量定义
#include "AllPassModeThread.h"
#include "TpThread.h"
#include "SerialCom.h"
#include "SerialRS485.h"
#include "Tact.h"
#if _SYSTEM_AMTAFT_
#include "PgIndex.h"
#include "ViewingAngleThread.h"
#include "VisionThread.h"
#include "LumitopThread.h"
#include "RankThread.h"
#include "ManualThread.h"
#include "ComView.h"
#else
#include "GammaThread.h"
#endif

// CAni_Data_Serever_PCApp:
//

class CManualThread;

// Lighting 检测结果缓存（从 MySQL 查询后存储）
struct LightingInspectionResult {
	CString m_strGUID;
	CString m_strScreenID;
	CString m_strUniqueID;
	CString m_strAOIResult;      // OK/NG/BrightDot/BlackDot/Line/Mura/Block/BM
	CString m_strCodeAOI;        // 缺陷代码
	CString m_strGradeAOI;       // 缺陷等级
	CString m_strStartTime;
	CString m_strStopTime;
	CString m_strLocalIP;       // AOI设备IP地址（策略3拼图路径用）
	BOOL m_bValid;
	LightingInspectionResult() : m_bValid(FALSE) {}
};

class CAni_Data_Serever_PCApp : public CWinApp, public ILightingEventHandler
{
public:
	CAni_Data_Serever_PCApp();

	LightingInspectionResult QueryInspectionResult(CString uniqueID);

	// Lighting 线程专用的数据库操作（使用线程局部连接）
	//LightingInspectionResult QueryInspectionResultThreadSafe(CString uniqueID, SQLHDBC pConn);
	//BOOL QueryIdMapByFixtureNoThreadSafe(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID, SQLHDBC pConn);

	CMultiDocTemplate* m_pDocOperator;
	HANDLE m_hApp;
	CCriticalSection m_csLightingFlow;
	SQLHDBC m_pLightingConn;
	SQLHDBC m_pDfsLightingConn;  // DFS 模块专用的数据库连接（避免多线程共享连接问题）
	BOOL m_bDfsLightingDBConnected;
	SQLHENV m_odbcEnv;

	CLogger* m_PlcLog;
	CLogger* m_PlcHeartBitLog;
	CLogger* m_ViewingAngleLog;
	CLogger* m_VisionLog;
	CLogger* m_LumitopLog;
	CLogger* m_AlignLog;
	CLogger* m_TimeOutLog;
	CLogger* m_PgLog;
	CLogger* m_pFTPLog;
	CLogger* m_pDataStatusLog;
	CLogger* m_pTactTimeLog;
	CLogger* m_pTraceLog;
	CLogger* m_pAxisLog;
	CLogger* m_PgSendReceiverLog;
	CLogger* m_pSendDefectCodeLog;
	CLogger* m_pUserLoginOutLog;
	CLogger* m_pUserLog;
	CLogger* m_pFFUSendReceiverLog;
	CLogger* m_pARSSendReceiverLog;
	CLogger* m_pTpLog;
	CLogger* m_pTpSendReceiverLog;

	CLogger* m_pAlignSendReceiverLog[10];
	CLogger* m_pVisionSendReceiver1Log;
	CLogger* m_pVisionSendReceiver2Log;
	CLogger* m_pViewingAngleSendReceiver1Log;
	CLogger* m_pViewingAngleSendReceiver2Log;
	CLogger* m_pViewingAngleSendReceiver3Log;
	CLogger* m_pViewingAngleSendReceiver4Log;
	CLogger* m_pLumitopSendReceiver1Log;
	CLogger* m_pLumitopSendReceiver2Log;
	CLogger* m_pOperateTimeLog;

	CLogger* m_pOpvLog;
	CLogger* m_pOpvSendReceiver1Log;
	CLogger* m_pOpvSendReceiver2Log;

	CLogger* m_pGammaSendReceiverLog;
	CLogger* m_pLightingLog;
	CLogger* m_pLightingSendReceiverLog;
	CLogger* m_pTestLog;

	//>> Code Log for MachineManager (MesAdapter)
	CLogger* m_pCodeLog[10];

	ModelName m_CurrentModel;
	BOOL m_CreateModelAlign;
	BOOL m_CreateModelVision1;
	BOOL m_CreateModelVision2;
	BOOL m_CreateModelViewingAngle1;
	BOOL m_CreateModelViewingAngle2;
	BOOL m_CreateModelViewingAngle3;
	BOOL m_CreateModelViewingAngle4;
	BOOL m_CreateModelLumitop1;
	BOOL m_CreateModelLumitop2;

	BOOL m_ChangeModelAlign;
	BOOL m_ChangeModelVision1;
	BOOL m_ChangeModelVision2;
	BOOL m_ChangeModelViewingAngle1;
	BOOL m_ChangeModelViewingAngle2;
	BOOL m_ChangeModelViewingAngle3;
	BOOL m_ChangeModelViewingAngle4;
	BOOL m_ChangeModelLumitop1;
	BOOL m_ChangeModelLumitop2;

	BOOL m_VisionThreadOpenFlag[2];
	BOOL m_ViewingAngleThreadOpenFlag[4];
	BOOL m_AlignThreadOpenFlag[MaxAlignCount];
	BOOL m_LumitopThreadOpenFlag[2];
	
	BOOL m_VisionConectStatus[2];
	BOOL m_ViewingAngleConectStatus[4];
	BOOL m_LumitopConectStatus[2];
	BOOL m_AlignConectStatus[MaxAlignCount];
	BOOL m_PlcConectStatus;
	BOOL m_FFUConectStatus;
	BOOL m_ARSConnectStatus;
#if _SYSTEM_AMTAFT_
	BOOL m_PgConectStatus[PgServerMaxCount];
	BOOL m_TpConectStatus;
	BOOL m_OpvConectStatus[ChMaxCount];
	BOOL m_LightingConectStatus;

	BOOL m_PgThreadOpenFlag[PgServerMaxCount];
	BOOL m_TpThreadOpenFlag;
	BOOL m_OpvThreadOpenFlag[ChMaxCount];
	BOOL m_LightingThreadOpenFlag;
#else
	BOOL m_PgConectStatus[PgServerMaxCount];
	BOOL m_PgThreadOpenFlag[PgServerMaxCount];
#endif
	CMsgBox *m_pMsgBox;
	CMsgBox *m_pMsgBoxAlarm; // 201030 yjlim
	vector<CAlignThread*>  m_AlignThread;
	vector<CAlignManager*> m_AlignSocketManager;
	vector<BOOL> m_AlignPCStatus;

	BOOL m_VisionPCStatus[2];
	BOOL m_ViewingAnglePCStatus[4];
	BOOL m_LumitopPCStatus[2];
	BOOL m_PLCStatus;
	
	BOOL m_bViewingAngleDeleteFlag;
	BOOL m_bVisionDeleteFlag;
	BOOL m_bLumitopDeleteFlag;
	BOOL m_bOtpDeleteFlag;

	BOOL m_PanelTestStart;
	BOOL m_PgPassMode;
	BOOL m_TpPassMode;
	BOOL m_AnglePassMode;
	BOOL m_AOIPassMode;
	BOOL m_OpvPassMode;
	BOOL m_bAllPassMode; //>>Dry Run Mode
	BOOL m_bBCTestMode;
	BOOL m_bDFSTestMode;
	BOOL m_LumitopPassMode;

	BOOL m_bExitFlag;
	int  m_iLanguageSelect;
	int m_iDFSPartSelect;
	int m_iSysPartSelect;
	int m_nUnloaderBool;
	int m_iNumberSendToPlc;

	CPlcThread* m_PlcThread;
	CDFSClient* m_pFTP;
	CFTPClient* m_pFS;
	CAllPassModeThread* m_AllPassModeThread;
	CSerialCom* m_FFUSerialCom;
	CSerialRS485* m_ARSSerialRS485;

	CPgIndex* m_PgInexThread[MaxZone];
	CViewingAngleThread* m_ViewingAngleThread;
	CVisionThread* m_VisionThread;
	CVisionThread m_VisionSocketManager[2];
	CViewingAngleThread m_ViewingAngleSocketManager[4];
	CLumitopThread* m_LumitopThread;
	CLumitopThread m_LumitopSocketManager[2];
	CRankThread* m_pRankTread;
	CTpThread* m_TpThread;
	CManualThread* m_ManualThread;
	CPgManager m_PgSocketManager[PgServerMaxCount];
	CTpManager m_TpSocketManager;
	COpvManager m_OpvSocketManager[ChMaxCount];
	CLightingSocketClient m_LightingSocketManager;
	CComView* m_pComView;
	BOOL m_bLightingCycleInProgress;
	BOOL m_bLightingActiveSlot[4]; // slot 1..4 -> index 0..3
	DWORD m_dwLightingStartTick;
	DWORD m_dwLightingTimeoutMs;
	//void LightingFlowTimeoutCheck();
	
	virtual void OnLightingRunning() override;
	//virtual void OnLightingSnapFN() override;
	//virtual void OnLightingResult(const int resultCode[4]) override;

	LightingInspectionResult m_LightingInspResult[4];
	BOOL m_bLightingDBConnected;
	DWORD m_dwLightingConnectTick;      // 连接成功时的时间戳
	BOOL m_bLightingAutoStartPending;  // 是否等待自动发送 Start
	static const DWORD LIGHTING_AUTO_START_DELAY_MS = 10000; // 10秒后自动发送
	CString m_strLightingDBServer;
	CString m_strLightingDBName;
	CString m_strLightingDBUser;
	CString m_strLightingDBPassword;

	BOOL InitLightingDatabase();
	BOOL ConnectLightingDatabase();
	void CloseLightingDatabase();

	// DFS 模块专用的数据库连接
	BOOL ConnectDfsLightingDatabase();
	void CloseDfsLightingDatabase();
	SQLHDBC GetDfsLightingConnection();

	LightingInspectionResult GetLightingResultByUniqueID(CString uniqueID);
	//void GetLightingResultByBarcode(CString strBarcode, CString& strAOIResult, CString& strCodeAOI, CString& strGradeAOI, BOOL& bValid);
	BOOL QueryLightingDefectList(CString strUniqueID, std::vector<LUMITOP_SDFSDefectDataBegin>& vecDefects);
	BOOL QueryAOIDefectList(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn = SQL_NULL_HANDLE);
	//BOOL QueryAOIDefectListThreadSafe(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn);
	//BOOL QueryAOIDefectListThreadSafe(CString strUniqueID, CDefectInfoList& vecDefects, SQLHDBC pConn);
	CString GetLightingUniqueIDByBarcode(CString strBarcode);

	// 生成AOI CSV文件（在FN$回调中调用）
	//BOOL GenerateAOICsvFile(CString strPanelID, CString strUniqueID, SQLHDBC pConn);

#if _SYSTEM_AMTAFT_
	AOIProductionData m_UiShiftProduction[MaxZone];
	AOIProductionData m_shiftProduction[MaxZone];
	ULDProductionData m_ULDUiShiftProduction[ChMaxCount];
	ULDProductionData m_ULDshiftProduction[ChMaxCount];

	AOIProductionData m_UiShift_TimeProduction[InspectTimeTotalCount];
	AOIProductionData m_shift_TimeProduction[InspectTimeTotalCount];
	ULDProductionData m_ULDUiShift_TimeProduction[InspectTimeTotalCount];
	ULDProductionData m_ULDshift_TimeProduction[InspectTimeTotalCount];

	DefectCountData m_SumDefectCountData[eNumShift];
	std::map<CString, map<CString, int>>m_mapOpvDefectList[eNumShift];
	std::map<CString, vector<DefectList>>m_mapOpvDefectHistory[eNumShift];
	std::vector<DefectCountData> m_VecDefectHistory[eNumShift];
	std::vector<DefectList> m_VecDefectList[DefectTitleMaxCount];
#else
	ProductionData m_UiShiftProduction[MaxGammaStage];
	ProductionData m_shiftProduction[MaxGammaStage];
	ProductionData m_UiShift_TimeProduction[InspectTimeTotalCount];
	ProductionData m_shift_TimeProduction[InspectTimeTotalCount];
#endif	
	CTact m_pTactTimeList[50];

	//<Melsec
	CEqInterface* m_pEqIf;
	std::vector<CString> m_VecInspDefectData[eNumShift];
	std::vector<IndexList>  m_indexList;

	std::vector<AlarmDataItem> m_AlarmDataList;
	map<CString, AlarmDataItem> m_AlarmRankCount[2];

	std::vector<TactTimeName> m_vecTactName;

	std::vector<InspResult> m_lastAlignVec[MaxAlignCount];

	std::vector<InspResult> m_lastInspResultVec;
	std::vector<InspResult> m_lastViewingAngleResultVec;
	std::vector<InspResult> m_lastLumitopResultVec;
	std::vector<InspResult> m_lastIndexPgVec[MaxZone];
	std::vector<InspResult> m_lastOpvResultVec;
	std::vector<InspResult> m_lastGammaVec[MaxGammaStage];

	std::map<CString, vector<pair<CString, ResultCodeRank>>> m_mapRankTotalList[eNumShift];
	std::map<CString, map<CString, int>> m_mapRankCodeCount[eNumShift];

	map<CString, CString> m_Send_Result_Code_Map;
	
	//setrank
	std::vector<pair<CString, CString>> m_strVecRank[RankListCount];

	//>>210422 yjlim
	std::vector<PGCoderesult> m_VecPGCode_PG;
	std::vector<PGCoderesult> m_VecPGCode_Mes;

	//TP Code
	std::vector<PGCoderesult> m_VecTPCode_TP;
	std::vector<PGCoderesult> m_VecTPCode_Mes;

	CCriticalSection m_csPGCodes;
	CString m_strMesAdapterPort;
	BOOL m_bPGCodeUsable;

	//<<

	std::vector<InspResult> m_VecManualStage[ManualStageMaxCount];
	std::vector<IDCardReader> m_VecIDCardReader;

	int AlarmMaxCount;				//MAx Count 1000;
	int UnloadAlarmMaxCount;		//MAx Count 1000;
	int m_iInspAoiMaxCount;
	int m_iInspViewingMaxCount;
	int m_iTimer[MaxTimerCount];
	int m_iShiftTime[Shift_Count];
	TimeInspect m_stuTimeInspect[eNumShift][InspectTimeTotalCount];
	int m_iTimeInspectNum;
	int m_iDataResetTime[eNumShift][Shift_Count];

	int m_CurrentIndexZone;
	int m_iUserClass;
	int m_lastShiftIndex;
	CString m_strCurrentToday;
	CString m_strEqpId;
	CString m_strEqpNum;
	CString m_strFileServerID; 
	CString m_strMachineType;
	CString m_strOpvImageWidth;
	CString m_strOpvImageHeight;
	CString m_strOkGrade;
	CString m_strContactNgGrade;
	CString m_strContactNgCode;
	CString m_strOKProcessID;
	CString m_strNGProcessID;
	CString m_strAlignCount;
	CString m_strCompanyLine;

	CString m_strFFUPortNum;
	CString m_strARSPortNum;
	CString m_strFFUEndPoint;
	CString m_strPGName;
	CString m_strLightingIP;
	CString m_strLightingPort;
	CString m_strLightingAutoTest;

	CString m_strDefectTitleName[DefectTitleMaxCount];
	BOOL m_bContact[PG_MAX_CH];
	CString m_strContactPanelID[PG_MAX_CH];

	int m_iAlignInspectType[MaxAlignCount];
	int m_iMachineType;
	int m_iTotalCompareCount;

	IDCardReader m_CurrentLoginUser;

	IDCardReader m_PmModeLoginUser[6];
	std::vector<IDCardReader> m_LoginOutData;

	BOOL m_bSameDefectChCheckMode;
	BOOL m_bSameDefectMode;
	CString m_strSameDefectCode;
	CString m_strSameDefectMaxCount;
	CString m_strSameDefectAlarmMaxCount;
	CCriticalSection		m_csIndexCheck;
	//
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void getMsgBox(int style, CString strKorMsg, CString strEngMsg, CString strChoMsg);
	void getMsgBox2(int style, CString strKorMsg, CString strEngMsg, CString strChoMsg);
	int YesNoMsgBox(int style, CString strKorMsg, CString strEngMsg, CString strChoMsg);
	void MakeDefaultDir();
	void LanguageChange();
	void ModelCheck(BOOL ModelCreateChangeFlag, CBtnEnh *ModelParm);
	void SaveSetTimer();
	void LoadSetTimer();
	void ThreadCreateDelete(BOOL bdelete, int OldAlignCnt);

	int GetShift(int nTime);
	void GetShiftTime(int nTime, int nShiftTime);

	void ExcelFileSave(CString strFileName, CString strTitle, CString strValue);
	void InspDataShiftReset(int iShiftTime);

#if _SYSTEM_AMTAFT_
	void AOIInspctionDataSum(AOIProductionData productionData[MaxZone], int nShift, AOIProductionData &SumProductionData);
	void ULDInspctionDataSum(ULDProductionData productionData[ChMaxCount], int nShift, ULDProductionData &SumProductionData);

	void AOIInspctionTimeDataSum(AOIProductionData productionData[InspectTimeTotalCount], int nShift, AOIProductionData &SumProductionData);
	void ULDInspctionTimeDataSum(ULDProductionData productionData[InspectTimeTotalCount], int nShift, ULDProductionData &SumProductionData);

	void AOIInspectionDataSave(int nShift);
	void AOIInspectionDataLoad(int nShift);
	void AOIInspectionTimeDataSave(int nShift);
	void AOIInspectionTimeDataLoad(int nShift);

	void ULDInspectionDataSave(int nShift);
	void ULDInspectionDataLoad(int nShift);
	void ULDInspectionTimeDataSave(int nShift);
	void ULDInspectionTimeDataLoad(int nShift);
#else
	void InspctionDataSum(ProductionData productionData[MaxGammaStage], int nShift, ProductionData &SumProductionData);
	void InspctionTimeDataSum(ProductionData productionData[InspectTimeTotalCount], int nShift, ProductionData &SumProductionData);

	void InspectionDataSave(int nShift);
	void InspectionDataLoad(int nShift);
	void InspectionTimeDataSave(int nShift);
	void InspectionTimeDataLoad(int nShift);
#endif

	void AlarmDataSave(vector<AlarmDataItem> alarmData, BOOL bFlag);
	void AlarmDataLoad();

	void TactTimeDataSave(int TactTimeUnit);
	void TactTimeTotalDataSave(int TactTimeUnit, DWORD dwTime, BOOL bTotalFlag);
	void TotalTactTimeLoad();

	void IDCardReaderLoad();
	void IDCardReaderUserHistory();
	void CurrenrUserSave();

	void PmModeIDCardReaderLoad();
	void PmModeIDCardReaderSave();

	CCriticalSection m_csFileSave;
	CCriticalSection m_csDefectCountLoad;
	
#if _SYSTEM_AMTAFT_
	void IndexCheck();
	void TpDataLoad(int nShift);
	void TpDataSave(int nShift);
	void ContactDataLoad(int nShift);
	void ContactDataSave(int nShift);
	void AlignDataLoad(int nShift);
	void AlignDataSave(int nShift);
	void ULDAlignDataLoad(int nShift);
	void ULDAlignDataSave(int nShift);
	void PreGammaDataLoad(int nShift);
	void PreGammaDataSave(int nShift);
	void SetSaveHistoryCode(CString strPanelID, CString strInspName, ResultCodeRank code, BOOL bFlag);
	void SetLoadHistoryCode(int nShift);
	void SetSaveRankCode(CString strInspName, CString strKey, int iValue, BOOL bFlag);
	void InspctionDefectDataSum(DefectCountData productionData, int nShift, DefectSumCountData &SumProductionData);
	void OpvDefectHistoryLosd();
	void OpvDefectPanelHistoryLosd();
	void OpvDefectSumCount();
	CString SetLoadOpvResultCode(CString strPanelID);
	void SetLoadRankCode(int nShift);
	void LoadRank();
	void OpvLoadTitleName();
	void DefectCodeListLoad();
	CString ParsingDefectDesctiption(CString strCode);
	void PGDfsInfoSave(PGDfsList PgList);
#else
	void ContactDataLoad(int nShift);
	void ContactDataSave(int nShift);
	void AlignDataLoad(int nShift);
	void AlignDataSave(int nShift);
	void MtpDataLoad(int nShift);
	void MtpDataSave(int nShift);
	void GammaDefectInfoSave(CString strPanelID, CString strFpcID, CString strCode, CString strGrade);
	CString GammaDefectInfoLoad(CString strPanelID, CString strFpcID);
	void GammaDfsInfoSave(PGDfsList PgList);
#endif
	void GetSystemData();
	void GetTactParameter();
	void LoginCheckMethod();
	void GetAlarmCount();
	CString SetTotalLoadResultCode(CString strPanelID, CString strFpcID, int iTypeNum);
	void SetLoadResultCode(CString strPanelID, CString strFpcID);
	//void SetLoadResultCodeFromDB(CString strPanelID, CString strFpcID);
	void SetSaveResultCode(CString strPanelID, CString strFpcID, CString strTypeName, PLCSendDefect Code, int iType);
	CString GetProcessID(CString strPanel);
	CString GetProjectID(CString strPanelID);
	CString AlarmTimeParsing(CString strEndTime, CString strStartTime);

	struct
	{
		BOOL operator()(pair<CString, int> &pair1, pair<CString, int> &pair2)
		{
			return pair1.second > pair2.second;
		}
	}comp;

	
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAni_Data_Serever_PCApp theApp;
