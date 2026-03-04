#pragma once

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"
#include "EZini.h"
#include "StringSupport.h"


struct ULD_SDFSDefectDataBegin2 {
	CString strPanel_ID;
	CString strFpc_ID;
	CString strDefect_Grade;
	CString strTP_Function;
	CString strDefect_code;
	CString strDefect_Ptn;
	CString strProcessID; // DFS항목아니고 OPV ProcessID 확인용
};

class COpvManager:  public CSocketComm
{
public:
	COpvManager();
	virtual ~COpvManager();
	bool SocketServerOpen(CString strServerPort);
	BOOL getConectCheck();

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	void SendOpvMessage(CString strMsg, int iPanelNum, int iCommand);
	void OpvLogMessage(CString strContents);
	BOOL GetTitleCheck(CStdioFile& sFile, int iSize);
	int GetItemCount(CString strInfo);
	CString GetExtractionMsg(CString& strMsg);
	CString GetLastExtractionMsg(CString& strMsg);
	BOOL LoadDefectListInfo(CString strPanel, int iCount);
	void OpvInspectionResult(int Num, CString strContents);
	void GetOPID(int Num, CString strContents);
	void OpvModelRequest(int Num, CString strModelID);
	void OpvPcTimeRequest(int Num);

	void OpvDefectCountSave();

	vector <ULD_SDFSDefectDataBegin2> m_ULD_DefectDataList2;

	std::vector<CString> m_lastContent;
	std::vector<CString> m_lastResult;
	std::vector<CString> m_lastCommand;
	std::vector<CString> m_lastRequest;

	CString GetLastContents(int iChNum) { return m_lastContent[iChNum]; }
	CString GetLastResult(int iChNum) { return m_lastResult[iChNum]; }
	CString GetLastCommand(int iChNum) { return m_lastCommand[iChNum]; }
	CString GetLastRequest(int iChNum) { return m_lastRequest[iChNum]; }
	CCriticalSection m_csSocketSend;
	CCriticalSection m_csData;

	int m_OpvCheckCount;
	int m_iOpvNum;
	CString m_strOPID;
};
#endif