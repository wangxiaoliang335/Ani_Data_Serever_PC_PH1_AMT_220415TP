// MNetHData.cpp: implementation of the MNetHData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MNetHData.h"
#include "StringSupport.h"
#include "ProfileDataIF.h"


PanelData::PanelData()
{
	memset(m_PanelData, 0x20, sizeof(m_PanelData));// Hex 0x20 : BLANK
}

FpcIDData::FpcIDData()
{
	memset(m_FpcIDData, 0x20, sizeof(m_FpcIDData));// Hex 0x20 : BLANK
}

CardReaderID::CardReaderID()
{
	m_CardNo = 0;
	memset(m_UserID, 0x20, sizeof(m_UserID));// Hex 0x20 : BLANK
	m_IDReaderInOut = 0;
}

CardReaderPassWord::CardReaderPassWord()
{
	memset(m_UserPassWord, 0x20, sizeof(m_UserPassWord));// Hex 0x20 : BLANK
	m_IDReaderInOut = 0;
}

ModelNameData::ModelNameData()
{
	m_PlcCurrentModelNum = 0;
	m_PlcModelCopyorChange = 0;
	m_PlcModelCopyNum = 0;
	m_PlcModelArriveName = 0;

	memset(m_Reserve, 0x20, sizeof(m_Reserve));// Hex 0x20 : BLANK
	memset(m_PlcCurrentModelName, 0x20, sizeof(m_PlcCurrentModelName));// Hex 0x20 : BLANK
}

ModelCopyName::ModelCopyName()
{
	m_PlcModelCopyorChange = 0;
	m_PlcModelCopyNum = 0;
	m_PlcModelArriveName = 0;
}

TrayLowerAlignResult::TrayLowerAlignResult()
{
	resultX1 = 0;
	resultY1 = 0;
	resultT1 = 0;
	resultX2 = 0;
	resultY2 = 0;
	resultT2 = 0;
	resultValue = 0;
}

TrayCheckResult::TrayCheckResult()
{
	//resultValue = 0;
	result[0] = 0;
	result[1] = 0;
}

AlignResult::AlignResult()
{
	resultX = 0;
	resultY = 0;
	resultT = 0;
	resultValue = 0;
}

DataStatus::DataStatus()
{
	memset(m_PanelData, 0x20, sizeof(m_PanelData));// Hex 0x20 : BLANK

#if _SYSTEM_AMTAFT_
	m_ContactStatus = 0;
	m_TpStatus = 0;
	m_VisionStatus = 0;
	m_OtpStatus = 0;
	m_ViewingStatus = 0;
	m_IndexNumStatus = 0;
	m_OkGrade = 0;
	m_TryInsertStatus = 0;
#elif _SYSTEM_GAMMA_
	m_ContactStatus = 0;
	m_ManualContactStatus = 0;
	m_MtpStatus = 0;
	m_GammaStageNum = 0;
#endif
	m_FirstContactStatus = 0;
#if _SYSTEM_GAMMA_
	m_GammaTunningTime = 0;
	m_InTrayNum = 0;
#endif
}

UnloaderDataStatus::UnloaderDataStatus()
{
	memset(m_PanelData, 0x20, sizeof(m_PanelData));// Hex 0x20 : BLANK

	memset(m_Reserve, 0x20, sizeof(m_Reserve));// Hex 0x20 : BLANK
	m_ManualStageNum = 0;
	m_ContactStatus = 0;
	m_ManualContactStatus = 0;
	m_PreGammaStatus = 0;
	m_TpStatus = 0;
	m_OpvStatus = 0;
	m_TryInsertStatus = 0;
	m_FirstContactStatus = 0;
}

DefectCodeRank::DefectCodeRank()
{
	memset(m_DefectCode, 0x20, sizeof(m_DefectCode));// Hex 0x20 : BLANK
	for (int ii = 0; ii < 8; ii++)
	{
		m_DefectCode[ii] = _T(' ');
	}
}

DefectGradeRank::DefectGradeRank()
{
	memset(m_DefectGrade, 0x20, sizeof(m_DefectGrade));// Hex 0x20 : BLANK
	for (int ii = 0; ii < 2; ii++)
	{
		m_DefectGrade[ii] = _T(' ');
	}
}

PanelOkGrade::PanelOkGrade()
{
	memset(m_OkGreade, 0x20, sizeof(m_OkGreade));// Hex 0x20 : BLANK
	for (int ii = 0; ii < 4; ii++)
	{
		m_OkGreade[ii] = _T(' ');
	}
}

AxisRecipeID::AxisRecipeID()
{
	memset(m_RecipeID, 0x20, sizeof(m_RecipeID));// Hex 0x20 : BLANK
	USHORT	m_AxisNo = 0;
	USHORT	m_ChangePoint = 0;
}

AxisModifyPosition::AxisModifyPosition()
{
	m_PositionData = 0;
	m_PositionSpeedData = 0;

	m_Reserve = 0;

	m_AccData = 0;
	m_DecData = 0;
	m_SoftData1 = 0;
	m_SoftData2 = 0;
}

AxisModifyBeforePosition::AxisModifyBeforePosition()
{
	m_PositionBeforeData = 0;
	m_PositionBeforeSpeedData = 0;

	m_Reserve = 0;

	m_AccBeforeData = 0;
	m_DecBeforeData = 0;
	m_SoftBeforeData1 = 0;
	m_SoftBeforeData2 = 0;
}

AlignAxisData::AlignAxisData()
{
	for (int ii = 0; ii < AlignAxisData_Count; ii++)
	{
		m_AlignAxisData[ii] = 0;
	}
}

OperateTime::OperateTime()
{
	m_iRateOfOperate = 0;
	m_iOperateTime = 0;
	m_iIdleTime = 0;
	m_iStopTime = 0;
}

ModelData::ModelData()
{
	memset(m_strModlName, 0x20, sizeof(m_strModlName));// Hex 0x20 : BLANK
}

ModuleData::ModuleData()
{
	memset(Panel_ID, 0x20, sizeof(Panel_ID));// Hex 0x20 : BLANK
	memset(FPC_ID, 0x20, sizeof(FPC_ID));// Hex 0x20 : BLANK
	memset(Glass_Type, 0x20, sizeof(Glass_Type));// Hex 0x20 : BLANK
	memset(Product_ID, 0x20, sizeof(Product_ID));// Hex 0x20 : BLANK
	memset(Owner_ID, 0x20, sizeof(Owner_ID));// Hex 0x20 : BLANK
	Owner_Code = 0;
	memset(Owner_Type, 0x20, sizeof(Owner_Type));// Hex 0x20 : BLANK
	memset(Lot_ID, 0x20, sizeof(Lot_ID));// Hex 0x20 : BLANK
	memset(Process_ID, 0x20, sizeof(Process_ID));// Hex 0x20 : BLANK
	memset(Recipe_ID, 0x20, sizeof(Recipe_ID));// Hex 0x20 : BLANK
	memset(SaleOrder, 0x20, sizeof(SaleOrder));// Hex 0x20 : BLANK
	memset(PreProcess_ID_1, 0x20, sizeof(PreProcess_ID_1));// Hex 0x20 : BLANK
	memset(Group_ID, 0x20, sizeof(Group_ID));// Hex 0x20 : BLANK
	memset(Product_Info, 0x20, sizeof(Product_Info));// Hex 0x20 : BLANK
	memset(LOT_Info, 0x20, sizeof(LOT_Info));// Hex 0x20 : BLANK
	memset(Product_Group, 0x20, sizeof(Product_Group));// Hex 0x20 : BLANK
	memset(From_Site, 0x20, sizeof(From_Site));// Hex 0x20 : BLANK
	memset(Current_Site, 0x20, sizeof(Current_Site));// Hex 0x20 : BLANK
	memset(From_Shop, 0x20, sizeof(From_Shop));// Hex 0x20 : BLANK
	memset(Current_Shop, 0x20, sizeof(Current_Shop));// Hex 0x20 : BLANK
	Thickness = 0;
	MMGFlag = 0;
	memset(PANELSIZE, 0x20, sizeof(PANELSIZE));// Hex 0x20 : BLANK
	memset(PGrade, 0x20, sizeof(PGRADE));// Hex 0x20 : BLANK
}

JobData::JobData()
{
	Casssette_Sequence_No = 0;
	Job_Sequence_No = 0;
	Group_Index = 0;
	Product_Type = 0;
	CST_Operation_Mode = 0;
	Job_Grade = 0;
	memset(Job_ID, 0x20, sizeof(Job_ID));// Hex 0x20 : BLANK
	INSP_Reservation = 0;
	memset(InspJudge_Data, 0x20, sizeof(InspJudge_Data));// Hex 0x20 : BLANK
	memset(Tracking_Data, 0x20, sizeof(Tracking_Data));// Hex 0x20 : BLANK
	memset(EQP_Flag, 0x20, sizeof(EQP_Flag));// Hex 0x20 : BLANK
	Chip_Count = 0;
	memset(PP_ID, 0x20, sizeof(PP_ID));// Hex 0x20 : BLANK
	memset(FPC_ID, 0x20, sizeof(FPC_ID));// Hex 0x20 : BLANK
	memset(Cassette_Setting_Code, 0x20, sizeof(Cassette_Setting_Code));// Hex 0x20 : BLANK
}

LoginUserData::LoginUserData()
{
	m_ResultValue = 0;
	m_UserLevel = 0;
	memset(m_UserID, 0x20, sizeof(m_UserID));// Hex 0x20 : BLANK
	memset(m_UserPassWord, 0x20, sizeof(m_UserPassWord));// Hex 0x20 : BLANK
	memset(m_UserIdCardNo, 0x20, sizeof(m_UserIdCardNo));// Hex 0x20 : BLANK
	memset(m_UserDivision, 0x20, sizeof(m_UserDivision));// Hex 0x20 : BLANK
	memset(m_UserName, 0x20, sizeof(m_UserName));// Hex 0x20 : BLANK
};

AutoFocusData::AutoFocusData()
{
	m_MoterValue = 0;
};

FFUData::FFUData()
{
	m_ICU1_ProcessValue = 0;
	m_ICU1_SettingValue = 0;
	m_ICU2_ProcessValue = 0;
	m_ICU2_SettingValue = 0;
	m_ICU3_ProcessValue = 0;
	m_ICU3_SettingValue = 0;
	m_ICU4_ProcessValue = 0;
	m_ICU4_SettingValue = 0;
	m_ICU5_ProcessValue = 0;
	m_ICU5_SettingValue = 0;
	m_ICU6_ProcessValue = 0;
	m_ICU6_SettingValue = 0;
	m_ICU7_ProcessValue = 0;
	m_ICU7_SettingValue = 0;
	m_ICU8_ProcessValue = 0;
	m_ICU8_SettingValue = 0;
	m_ICU9_ProcessValue = 0;
	m_ICU9_SettingValue = 0;
	m_ICU10_ProcessValue = 0;
	m_ICU10_SettingValue = 0;
	m_ICU11_ProcessValue = 0;
	m_ICU11_SettingValue = 0;
	m_ICU12_ProcessValue = 0;
	m_ICU12_SettingValue = 0;
	m_ICU13_ProcessValue = 0;
	m_ICU13_SettingValue = 0;
	m_ICU14_ProcessValue = 0;
	m_ICU14_SettingValue = 0;
	m_ICU15_ProcessValue = 0;
	m_ICU15_SettingValue = 0;
	m_ICU16_ProcessValue = 0;
	m_ICU16_SettingValue = 0;
}

DfsData::DfsData()
{
	m_StartTime1 = 0;
	m_StartTime2 = 0;
	m_StartTime3 = 0;
	m_LoadHandlerTime1 = 0;
	m_LoadHandlerTime2 = 0;
	m_LoadHandlerTime3 = 0;
	m_UnLoadHandlerTime1= 0;
	m_UnLoadHandlerTime2= 0;
	m_UnLoadHandlerTime3= 0;
	m_TPTime = 0;
	m_PreGammaTime = 0;
	memset(m_PanelID, 0x20, sizeof(m_PanelID));// Hex 0x20 : BLANK
	memset(m_FpcID, 0x20, sizeof(m_FpcID));// Hex 0x20 : BLANK
	m_EndTime1 = 0;
	m_EndTime2 = 0;
	m_EndTime3 = 0;
	m_PreGammaContactStatus = 0;
	memset(m_ModelID, 0x20, sizeof(m_ModelID));// Hex 0x20 : BLANK
	m_IndexNum = 0;
	m_ChNum = 0;
}

AlarmTextData::AlarmTextData()
{
	m_AlarmUnit = 0;
	m_AlarmCode = 0;
	m_AlarmLevel = 0;
	m_AlarmUsingFlag = 0;
	memset(m_AlarmText, 0x20, sizeof(m_AlarmText));// Hex 0x20 : BLANK
}

EsdPlcData::EsdPlcData()
{
	m_strEsdResult = 0;
	m_strEsdPole = 0;
	m_iEsdData1 = 0;
	m_iEsdData2 = 0;
	m_iEsdData3 = 0;
	m_iEsdData4 = 0;
	m_iEsdData5 = 0;
}