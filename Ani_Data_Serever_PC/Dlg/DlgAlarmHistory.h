#pragma once
#include "afxcmn.h"
#include "ListCtrlEx.h"

// CDlgAlarmHistory dialog

class CDlgAlarmHistory : public CDialog//, IStringChangeListener
{
	DECLARE_DYNAMIC(CDlgAlarmHistory)

public:
	int m_iCurrentPos;					 // 0~999±îÁöÀÓ
	int m_iOrder;
	int m_iModeNum;
	int m_iRankShiftType;
	CDlgAlarmHistory(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAlarmHistory();

	// Dialog Data
	enum { IDD = DLG_ALARM_HISTORY };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrlEx m_alarmListCtrl;
	CListCtrlEx m_alarmRankCtrl;

	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	struct
	{
		BOOL operator()(AlarmDataItem &AlarmCountVec1, AlarmDataItem &AlarmCountVec2)
		{
			return AlarmCountVec1.m_alarmCount > AlarmCountVec2.m_alarmCount; 
		}
	}AlarmComp;

	void SetAlarmData(int index, AlarmDataItem pAlarmDataItem);
	void SetItemText(int iRow, int iCol, CString strVal);
	
	void ClickHistoryMode();
	void ModeSelect(int iModeNum);
	DECLARE_EVENTSINK_MAP()
	void ClickRankType();
	void RankTypeSelect(int iRankType);

	void AlarmDataCheck();
	BOOL m_bRankClickFlag;
};
