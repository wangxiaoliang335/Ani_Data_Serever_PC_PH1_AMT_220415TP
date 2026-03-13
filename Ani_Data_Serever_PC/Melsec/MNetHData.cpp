// MNetHData.cpp: implementation of the MNetHData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MNetHData.h"
#include "StringSupport.h"
#include "ProfileDataIF.h"

/*
======================================================================
PLC / DFS / MES 数据流向简要说明（给非软件工程人员看的说明）
----------------------------------------------------------------------
1. 模块作用
   - 本文件中的各个结构体（如 `ModuleData`、`JobData`、`AlignResult`、`DfsData` 等）
     主要用于描述“PLC 寄存器里的数据布局”和“DFS/MES 文件中的字段布局”。
   - `MNetHData` 类（定义在头文件中）使用这些结构，在 PLC 和 Data Server 之间搬运数据：
     - 读 PLC：读取当前面板信息、作业信息、状态信息
     - 写 PLC：写入检测结果、对位结果、TP/PG/Gamma 结果等

2. PLC ↔ Data Server 的典型流程
   1）整机 PLC 在面板到位/流程切换时：
      - 把 Panel ID、FPC ID、作业信息等写入固定的数据区（对应 `ModuleData`、`JobData` 等）
   2）Data Server 周期性或按事件读取 PLC：
      - 使用 MNetH 协议把这些字/位读出，填入相应结构体
      - 上层逻辑（VS/PG/TP/OPV 管理器）根据这些数据决定是否触发检测
   3）检测完成后：
      - 上层逻辑根据返回结果构造 `AlignResult` / `TrayCheckResult` / `DataStatus` 等结构
      - 通过 `SetAlignResult` / `SetPlcBitData` / `SetPlcWordData` 等接口写回 PLC
      - PLC 根据这些结果继续执行机械动作或报警。

3. Data Server ↔ DFS 文件的典型流程
   - 目的：把每片面板的检测过程和结果固化为文本/CSV 文件，供 MES/OPV/追溯系统使用。
   1）检测开始/结束时：
      - 使用 `DfsData` / `DataStatus` 等结构，记录开始时间、结束时间、TP/Pre-Gamma/Contact 状态等
   2）缺陷级别信息：
      - 通过 `DefectCodeRank` / `DefectGradeRank` / `PanelOkGrade` 等，记录缺陷代码、等级、面板最终等级
   3）这些结构最终会在其他模块中被序列化为 DFS 文件（文件路径在 `Migration.h` 中有统一定义）。

4. Data Server ↔ MES 的数据关系
   - MES 侧关注的是“模块/作业维度”的信息：
     - 模块信息 → 对应 `ModuleData` / `SDataModuleShop`
     - 作业信息 → 对应 `JobData` / `SJobDataShop`
   - 一般流程为：
     1）MES 通过文件/Socket 下发模块与作业信息
     2）Data Server 将这些信息映射到 PLC 所需的字段（或只在内部使用）
     3）检测完成后，Data Server 再把结果写入 DFS/MES 指定格式文件，供上层系统采集。

5. 现场查看/修改 PLC 映射的注意事项（给非软件人员参考）
   - 每个结构体数组/成员的长度和顺序都直接对应 PLC 的字节布局，随意改动会导致：
     - 读取到的数据错位（Panel ID、FPC ID 乱掉）
     - 写回的结果写到错误地址（PLC 逻辑判断错误）
   - 若需要新增字段，建议：
     1）先由 PLC 工程师在 PLC 中预留好地址，并输出一份地址表
     2）再在本文件中增加对应成员，同时调整读写函数中的偏移量
======================================================================
*/

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

/**
 * @brief ModuleData结构体构造函数
 * 
 * ModuleData用于存储从PLC读取的模块/产品信息
 * 这些数据通过三菱PLC的MNetH协议从PLC内存区域读取
 * 
 * 数据字段说明：
 * - Panel_ID: 面板ID，唯一标识符，最大20字符
 * - FPC_ID: FPC (柔性印刷电路板) ID，最大30字符
 * - Glass_Type: 玻璃类型，标识玻璃基板类型，最大14字符
 * - Product_ID: 产品ID，标识产品型号，最大14字符
 * - Owner_ID: 所有者ID，标识产品所有者，最大14字符
 * - Owner_Code: 所有者代码，USHORT类型
 * - Owner_Type: 所有者类型，最大14字符
 * - Lot_ID: 批次ID，标识生产批次，最大12字符
 * - Process_ID: 工艺ID，标识当前工艺步骤，最大4字符
 * - Recipe_ID: 配方ID，标识使用的工艺配方，最大40字符
 * - SaleOrder: 销售订单号，最大10字符
 * - PreProcess_ID_1: 前道工艺ID，最大4字符
 * - Group_ID: 组ID，用于产品分组，最大16字符
 * - Product_Info: 产品信息，产品相关的附加信息，最大50字符
 * - LOT_Info: 批次信息，批次相关的附加信息，最大50字符
 * - Product_Group: 产品组，产品所属的组别，最大20字符
 * - From_Site: 来源站点，产品来源的站点代码，最大6字符
 * - Current_Site: 当前站点，产品当前所在的站点代码，最大6字符
 * - From_Shop: 来源车间，产品来源的车间代码，最大6字符
 * - Current_Shop: 当前车间，产品当前所在的车间代码，最大6字符
 * - Thickness: 厚度，面板厚度（单位：um），USHORT类型
 * - MMGFlag: MMG标志，标识是否为MMG产品，USHORT类型
 * - PANELSIZE: 面板尺寸，格式：Width x Height，最大200字符
 * - PGrade: 面板等级，最大200字符
 * 
 * 初始化说明：
 * - 所有字符串字段初始化为0x20（空格字符，ASCII码32）
 * - 数值字段初始化为0
 */
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

/**
 * @brief JobData结构体构造函数
 * 
 * JobData用于存储从PLC读取的作业（Job）信息
 * 这些数据通过三菱PLC的MNetH协议从PLC内存区域读取
 * 
 * 数据字段说明：
 * - Casssette_Sequence_No: 料盒序列号，标识料盒的顺序，USHORT类型
 * - Job_Sequence_No: 作业序列号，标识作业的顺序，USHORT类型
 * - Group_Index: 组索引，用于作业分组，USHORT类型
 * - Product_Type: 产品类型，标识产品的类型，USHORT类型
 * - CST_Operation_Mode: 料盒操作模式，0=自动，1=手动等，USHORT类型
 * - Job_Grade: 作业等级，标识作业的等级，USHORT类型
 * - Job_ID: 作业ID，唯一标识符，最大20字符
 * - INSP_Reservation: 检测预约标志，0=未预约，1=已预约，USHORT类型
 * - InspJudge_Data: 检测判定数据，存储检测判定相关信息，USHORT数组[2]
 * - Tracking_Data: 追溯数据，用于产品追溯，USHORT数组[2]
 * - EQP_Flag: 设备标志，标识设备相关状态，USHORT数组[2]
 * - Chip_Count: 芯片数量，标识芯片的数量，USHORT类型
 * - PP_ID: PP (Process Plan) ID，工艺计划ID，最大26字符
 * - FPC_ID: FPC (柔性印刷电路板) ID，最大40字符
 * - Cassette_Setting_Code: 料盒设置代码，标识料盒的设置，最大4字符
 * 
 * 初始化说明：
 * - 所有字符串字段初始化为0x20（空格字符）
 * - 所有数值字段初始化为0
 */
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

/**
 * @brief DfsData结构体构造函数
 * 
 * DfsData用于存储DFS (Data File System) 文件中的数据
 * DFS文件用于记录检测过程中的时间戳、面板信息、检测结果等
 * 
 * 数据字段说明：
 * - m_StartTime1/2/3: 开始时间1/2/3，记录不同阶段的开始时间（时间戳，秒）
 * - m_LoadHandlerTime1/2/3: 上料处理时间1/2/3，记录上料阶段的时间（时间戳，秒）
 * - m_UnLoadHandlerTime1/2/3: 下料处理时间1/2/3，记录下料阶段的时间（时间戳，秒）
 * - m_TPTime: TP (Test Pattern) 测试时间，记录TP测试的时间（时间戳，秒）
 * - m_PreGammaTime: Pre-Gamma测试时间，记录Pre-Gamma测试的时间（时间戳，秒）
 * - m_PanelID: 面板ID，唯一标识符
 * - m_FpcID: FPC ID，柔性印刷电路板ID
 * - m_EndTime1/2/3: 结束时间1/2/3，记录不同阶段的结束时间（时间戳，秒）
 * - m_PreGammaContactStatus: Pre-Gamma接触状态，0=未接触，1=已接触
 * - m_ModelID: 模型ID，标识检测使用的模型
 * - m_IndexNum: 索引编号，用于标识检测位置
 * - m_ChNum: 通道编号，用于标识检测通道
 * 
 * 初始化说明：
 * - 所有时间戳字段初始化为0
 * - 所有字符串字段初始化为0x20（空格字符）
 * - 所有数值字段初始化为0
 */
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