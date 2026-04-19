#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE : DataModels.h
// 数据模型定义 - 对应数据库表结构
// 基于: 点灯数据库表说明.xlsx
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATA_MODELS_H_
#define _DATA_MODELS_H_

#include <vector>
#include <afxstr.h>

///////////////////////////////////////////////////////////////////////////////
// 检测结果模型 (对应 IVS_LCD_InspectionResult 表)
///////////////////////////////////////////////////////////////////////////////
class CInspectionResult
{
public:
    // 主键
    int SysID;
    CString GUID;
    
    // 屏基本信息
    CString ScreenID;           // 屏二维码
    CString DeviceID;           // AOI检测设备号
    int PlatformID;             // 位置号 (0,1,2,3...)
    CString LocalIP;            // AOI设备IP地址 (IP-Port目录名)
    CString ModelName;          // 检测模板名
    CString UniqueID;           // ICW唯一标识符
    CString MarkID;             // 屏号
    CString MainAoiFixID;       // 主检治具号
    
    // 检测时间
    COleDateTime StartTime;     // 检测开始时间
    COleDateTime StopTime;      // 检测结束时间
    
    // 检测结果
    CString Status;             // 检测状态
    CString AOIResult;          // 主检结果 (OK/NG/BrightDot/...)
    CString ReviewResult_Worker;    // 人工复判结果
    CString ReviewResult_Machine;   // 自动复检结果
    
    // 定位信息
    double LocateShiftX;
    double LocateShiftY;
    double LocateAngle;
    
    // 图像尺寸
    int RawImageXLen;
    int RawImageYLen;
    int GridImageXLen;
    int GridImageYLen;
    
    // 物理尺寸 (mm)
    double PanelPhysicalXLen;
    double PanelPhysicalYLen;
    
    // 缺陷分类
    CString Code_AOI;
    CString Grade_AOI;          // 产品等级 (S,A,B等)
    CString Level_AOI;
    CString DefClass_AOI;       // 缺陷大类
    CString DefName_AOI;        // 缺陷名称
    
    // 自动复检结果
    CString Code_AutoReview;
    CString Grade_AutoReview;
    CString Level_AutoReview;
    CString DefClass_AutoReview;
    CString DefName_AutoReview;
    
    // 人工复判结果
    CString Code_ManualReview;
    CString Grade_ManualReview;
    CString Level_ManualReview;
    
    // 操作员
    CString OperatorID;
    CString Operator_ManualReview;
    
    // XML扩展信息
    CString XMLInfo;
    
    // 重置所有字段
    void Reset()
    {
        SysID = 0;
        GUID = _T("");
        ScreenID = _T("");
        DeviceID = _T("");
        PlatformID = 0;
        LocalIP = _T("");
        ModelName = _T("");
        UniqueID = _T("");
        MarkID = _T("");
        MainAoiFixID = _T("");
        Status = _T("");
        AOIResult = _T("");
        ReviewResult_Worker = _T("");
        ReviewResult_Machine = _T("");
        Code_AOI = _T("");
        Grade_AOI = _T("");
        Level_AOI = _T("");
        DefClass_AOI = _T("");
        DefName_AOI = _T("");
        Code_AutoReview = _T("");
        XMLInfo = _T("");
        LocateShiftX = 0.0;
        LocateShiftY = 0.0;
        LocateAngle = 0.0;
        RawImageXLen = 0;
        RawImageYLen = 0;
        GridImageXLen = 0;
        GridImageYLen = 0;
        PanelPhysicalXLen = 0.0;
        PanelPhysicalYLen = 0.0;
    }

    // 生成新GUID
    void GenerateGUID();
    
    // 是否OK
    BOOL IsOK() const { return AOIResult.CompareNoCase(_T("OK")) == 0; }
    BOOL IsNG() const { return AOIResult.CompareNoCase(_T("NG")) == 0; }
};

///////////////////////////////////////////////////////////////////////////////
// 缺陷信息模型 (对应 IVS_LCD_AOIResult 表)
///////////////////////////////////////////////////////////////////////////////
class CDefectInfo
{
public:
    // 主键
    int SysID;
    CString GUID_Parent;        // 关联屏表GUID
    
    // 缺陷基本信息
    int DefectIndex;            // 缺陷编号
    CString Type;               // 缺陷类型 (BrightDot/BlackDot/Line/Mura/Block/BM/Other)
    int PatternID;              // 画面号
    CString PatternName;        // 画面名
    CString InspType;           // 检出工位 (MainAOI/AllView)
    
    // 缺陷位置 (像素)
    int Pos_x;
    int Pos_y;
    int Pos_width;
    int Pos_height;
    
    // 缺陷物理尺寸 (mm)
    double TrueSize;            // (长+短)/2
    double TrueDiameter;        // 长轴长
    double TrueLongSize;        // 长尺寸
    double TrueShortSize;       // 短尺寸

    // 原始图特征(像素) - 点灯数据库表说明.xlsx: 需要新增
    int OriArea;                // 原始图面积(像素个数)
    int OriLongSize;            // 原始图长轴长度(像素)
    int OriShortSize;           // 原始图短轴长度(像素)
    
    // 灰度特征
    int GrayScale;              // 缺陷灰度
    int GrayScale_BK;           // 背景灰度
    int GrayScaleDiff;           // 灰度差
    int GrayscaleMean;          // 平均灰度
    double GrayscaleMin;        // 最小灰度
    double GrayscaleMax;        // 最大灰度
    
    // 几何特征
    int Area;                   // 面积 (像素个数)
    double Roundness;           // 圆度
    double MajorAxisAngle;      // 主轴角度
    double JND;                 // JND值
    
    // 层级
    CString Layer;              // 缺陷层 (TP/OCA/Cell等)
    
    // 复判结果
    CString ReviewResult_Worker;
    CString ReviewResult_Machine;
    CString MachineReviewDefectName;
    
    // 缺陷分类
    CString Code_AOI;
    CString Grade_AOI;
    CString Level_AOI;
    CString DefClass_AOI;       // 主检缺陷分类大类(新增)
    CString DefName_AOI;        // 主检缺陷分类名字(新增)
    
    CString Code_AutoReview;
    CString Grade_AutoReview;
    CString Level_AutoReview;
    
    CString Code_ManualReview;
    CString Grade_ManualReview;
    CString Level_ManualReview;
    
    // 图像路径
    CString ImagePath;
    
    // XML扩展
    CString XMLInfo;

    // 算法/特征信息(主检) - 点灯数据库表说明.xlsx: 需要新增
    CString AlgName;            // 算法名
    int AlgID;                  // 算法ID
    CString ReasonCode;         // ReasonCode
    CString FeatureName;        // 规则特征名
    CString FeatureMin;         // 特征规格下限
    CString FeatureMax;         // 特征规格上限
    CString FeatureUnit;        // 特征单位
    CString FeatureValue;       // 特征值
    
    // 自动复检字段
    CString DefColor;           // 颜色 (White/Red/Green/Blue)
    double DefColorValue;       // 颜色亮度
    CString DefClass_AutoReview;
    CString DefName_AutoReview;
    CString PointType;          // 点缺陷类型
    
    // 构造函数
    CDefectInfo()
        : SysID(0)
        , DefectIndex(0)
        , PatternID(0)
        , Pos_x(0)
        , Pos_y(0)
        , Pos_width(0)
        , Pos_height(0)
        , TrueSize(0.0)
        , TrueDiameter(0.0)
        , TrueLongSize(0.0)
        , TrueShortSize(0.0)
        , OriArea(0)
        , OriLongSize(0)
        , OriShortSize(0)
        , GrayScale(0)
        , GrayScale_BK(0)
        , GrayScaleDiff(0.0)
        , GrayscaleMean(0)
        , GrayscaleMin(0)
        , GrayscaleMax(0)
        , Area(0)
        , Roundness(0.0)
        , MajorAxisAngle(0.0)
        , JND(0.0)
        , DefColorValue(0.0)
        , AlgID(0)
    {
    }
    
    // 是否为点缺陷
    BOOL IsDot() const { return Type.Find(_T("Dot")) >= 0; }
    // 是否为线缺陷
    BOOL IsLine() const { return Type.CompareNoCase(_T("Line")) == 0; }
    // 是否为色斑
    BOOL IsMura() const { return Type.CompareNoCase(_T("Mura")) == 0; }
};

///////////////////////////////////////////////////////////////////////////////
// 检测结果列表类型定义
///////////////////////////////////////////////////////////////////////////////
typedef std::vector<CInspectionResult> CInspectionResultList;
typedef std::vector<CDefectInfo> CDefectInfoList;

///////////////////////////////////////////////////////////////////////////////
// ID映射模型 (对应 IVS_LCD_IDMap 表)
///////////////////////////////////////////////////////////////////////////////
class CIDMapInfo
{
public:
    // 主键
    int SysID;
    
    // 关联信息
    CString UniqueID;           // 唯一标识符 (由数据服务器生成)
    CString ScreenID;           // 屏二维码/产品条码
    CString MarkID;             // 治具号 (01,02,03,04)
    int PosID;                  // 位置号 (0,1,2,3)
    CString MainAoiFixID;       // 主检治具号
    
    // 状态
    CString Status;             // 状态 (Using/Used/Finish)
    COleDateTime CreateTime;   // 创建时间
    COleDateTime UpdateTime;   // 更新时间
    
    // 构造函数
    CIDMapInfo()
        : SysID(0)
        , PosID(0)
    {
    }
};

#endif // _DATA_MODELS_H_

