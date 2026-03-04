#if !defined(__DISPINSPTEST_MIGRATION_HEADER__)
#define __DISPINSPTEST_MIGRATION_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MS_YESNO 0
#define MS_OK 1

#define DLG_OK 1
#define DLG_CANCEL 2
#define MSG_OK 6
#define MSG_CANCEL 7

#define DFS_CHECK_PANEL_SIZE	3

#define DFS_SHARE_PATH	_T("D:\\Data\\Share\\")
#define DFS_SHARE_OPV_PATH	_T("D:\\Data\\Share\\OPV\\")
#define DFS_SHARE_OPV_RESULT_PATH	_T("D:\\Data\\Share\\OPV_Result\\")
#define DFS_VIEWING_ANGLE_SHARE_PATH _T("\\\\192.168.1.104\\Share\\")
#define DFS_SHARE_OPVDFS_PATH  _T("\\\\192.168.1.202\\Data\\Share\\")
#define FS_SHARE_JOBDATA_PATH  _T("D:\\Data\\Share\\JobData\\")

#define USHORT unsigned short
#define UINT unsigned int

static USHORT m_codeReset = 0;
static USHORT m_codeOk = 1;
static USHORT m_codeFail = 2;
static USHORT m_codeTimeOut = 3;
static USHORT m_codeResponseError = 4;
static USHORT m_codePlcSendReceiverError = 5;
static USHORT m_codePlcPanelError = 6;
static USHORT m_codePositionError = 7;
static USHORT m_codeDfsFileError = 8;
static USHORT m_codeAlignCount = 9;
static USHORT m_Y9 = 10;

static USHORT m_WordReset = 0;
static USHORT m_WordEnd = 1;

static USHORT m_UpperInsert = 0;
static USHORT m_TrayInsert = 1;

static USHORT m_ManualStage1 = 1;
static USHORT m_ManualStage2 = 2;

static USHORT m_dfsTpNG = 2;
static USHORT m_dfsPreGammaNG = 2;
static USHORT m_dfsContactNG = 3;

const int MAINVIEW = 0;
const int COMVIEW = 1;
const int ADDRVIEW = 2;
const int UNLOADERVIEW = 3;
const int DEFECTCOUNTVIEW = 4;

const int WM_USER_INIT_SYSTEM = WM_USER + 1;
const int WM_USER_CLOSE = WM_USER + 2;
const int WM_USER_FFU_RECEIVE = WM_USER + 3;
const int TMR_MAIN_INSPECT_INFO = 509;
const int TMR_MAIN_DEFECT_INFO = 510;
const int TMR_MSG_BOX = 511;

#define CREATE_DLG(nSys,ptr, Class, nIDD, pWnd, nShow)	if(nSys == TRUE && ptr == NULL) \
																{ ptr = new Class; \
																ptr->Create(nIDD, pWnd); \
																ptr->ShowWindow(nShow);}

#define CREATE_RECT_DLG(ptr, Class, nIDD, pWnd, rect, nShow)	if(ptr == NULL) \
																	{ ptr = new Class; \
																	ptr->Create(nIDD, pWnd); \
																	ptr->MoveWindow(rect); \
																	ptr->ShowWindow(nShow);}

#define SQUARE(a)		((a) * (a))

#define TWHITE	RGB(255, 255, 255)
#define TRED	RGB(255, 0, 0)
#define TBLUE	RGB(0, 0, 255)
#define TGREEN	RGB(0, 255, 0)
#define TBLACK	RGB(0, 0, 0)
#define TYELLOW	RGB(255, 255, 0)
#define TCYAN	RGB(0, 255, 255)
#define TVIOLET	RGB(255, 0, 255)

#define CVWHITE		CV_RGB(255, 255, 255)
#define CVRED		CV_RGB(255, 0, 0)
#define CVBLUE		CV_RGB(0, 0, 255)
#define CVGREEN		CV_RGB(0, 255, 0)
#define CVBLACK		CV_RGB(0, 0, 0)
#define CVYELLOW	CV_RGB(255, 255, 0)
#define CVCYAN		CV_RGB(0, 255, 255)
#define CVVIOLET	CV_RGB(255, 0, 255)

//>>>>>>>>>>>>> Color Define 210422
const COLORREF BACKGROUND_COLOR = RGB(230, 230, 230);
const COLORREF CLOUDBLUE = RGB(128, 184, 223);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF BLACK = RGB(1, 1, 1);
const COLORREF DKGRAY = RGB(128, 128, 128);
const COLORREF LTGRAY = RGB(192, 192, 192);
const COLORREF LTYELLOW = RGB(255, 255, 128);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF DKYELLOW = RGB(128, 128, 0);
const COLORREF RED = RGB(255, 0, 0);
const COLORREF DKRED = RGB(128, 0, 0);
const COLORREF LTRED = RGB(255, 128, 0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF DKBLUE = RGB(0, 0, 220);
const COLORREF CYAN = RGB(0, 255, 255);
const COLORREF DKCYAN = RGB(0, 128, 128);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF DKGREEN = RGB(0, 128, 0);
const COLORREF MAGENTA = RGB(255, 0, 255);
const COLORREF DKMAGENTA = RGB(128, 0, 128);
const COLORREF BASIC = RGB(212, 208, 200);
const COLORREF LTVIOLET = RGB(157, 157, 255);
const COLORREF SYS_COLOR = RGB(236, 233, 216);
const COLORREF LTSKYBLUE = RGB(128, 255, 255);
const COLORREF DATA_ITEM_CHANGE_BACKGROUND_COLOR = CYAN;
const COLORREF DATA_ITEM_CHANGE_TEXT_COLOR = BLACK;
const COLORREF DATA_ITEM_TEXT_COLOR = BLACK;
const COLORREF DATA_ITEM_BACKGROUND_COLOR = CYAN;
//<<

#define MAX_RECEIVE_LEN							(1024)
#define _MAX_KEY_LEN						(120)
#define GET_KEY_VALUE(scope, key, dvalue)	::GetPrivateProfileString(scope, key, dvalue, (LPWSTR)(LPCWSTR)strKeyValue, _MAX_KEY_LEN, (LPWSTR)(LPCWSTR)sFileName);	\
											strKeyValue.TrimRight(); strKeyValue.TrimLeft();
#define SET_KEY_VALUE(scope, key, value)	::WritePrivateProfileString(scope, key, (LPWSTR)(LPCWSTR)value, (LPWSTR)(LPCWSTR)sFileName);
#define MAX_TACT_NUM 10

enum{
	VS_ARE_YOU_THERE,
	VS_PCTIME_REQUEST,
	VS_STATE,
	VS_STATE_REQUEST,
	VS_MODEL,
	VS_MODEL_REQUEST,
	VS_MODEL_CREATE,
	VS_MODEL_DELETE,
	VS_MODEL_MODIFY,
	VS_MODEL_CHANGE,
	VS_GRAB_START,
	VS_GRAB_END,
	VS_INSPECTION_OK,
	VS_INSPECTION_RESULT,
	VS_INSPECTION_JUDGE,
	VS_INSECTION_OTP_OK,
	VS_INSECTION_OTP_RESULT,
	VS_INSECTION_START_ING,
	VS_ALIGN_AXIS_RESULT_START,
	VS_AUTO_CAM_SET_START,
	VS_Z_MOVE_REQUEST,
	VS_FOCUS_MOVE_REQUEST,
	VS_Z_SAVE_POS_REQUEST,
	VS_FOCUS_SAVE_POS_REQUEST,
	VS_OPV_LOGIN_REQUEST,
	VS_OPV_LOGOUT_REQUEST,
	VS_VISION_TEST,
	VS_VIEWING_TEST,
	VS_LUMITOP_TEST,
	VS_PACKET_MAX_NUM
};
enum {
	_LUMITOP,
	_AOI,
	_PREGAMMA,
	_DOT,
	_CONTACT,
	_MAX
};
enum{
	MC_ARE_YOU_THERE,
	MC_PCTIME,
	MC_STATE,
	MC_STATE_REQUEST,
	MC_MODEL,
	MC_MODEL_REQUEST,
	MC_MODEL_CREATE,
	MC_MODEL_DELETE,
	MC_MODEL_MODIFY,
	MC_MODEL_CHANGE,
	MC_INSPECTION_START,
	MC_GRAB_READY_REQUEST,
	MC_GRAB_END_RECEIVE,
	MC_INSPECTION_REQUEST,
	MC_INSPECTION_END,
	MC_INSPECTION_RESULT_RECEIVE,
	MC_INSPECTION_JUDGE_REQUEST,
	MC_INSPECTION_JUDGE_RECEIVE,
	MC_INSECTION_OTP_START,
	MC_INSECTION_OTP_END,
	MC_INSECTION_OTP_RESULT_RECEIVE,
	MC_ALIGN_LIGHT_CONTROL,
	MC_ALIGN_AXIS_RESULT_END,
	MC_ALIGN_LIGHT_OFF,
	MC_Z_MOVE_DONE,
	MC_FOCUS_MOVE_DONE,
	MC_Z_SAVE_POS_DONE,
	MC_FOCUS_SAVE_POS_DONE,
	MC_OPV_LOGIN_DONE,
	MC_OPV_LOGOUT_DONE,
	MC_NG_PANEL,
	MC_MANUAL_TP_NG,
	MC_PACKET_MAX_NUM
};

//>>210422
enum {
	MesASeq_MES,
	MesASeq_PanelID,
	MesASeq_ProductSpec,
	MesASeq_End,
	MesASeq_OkNg,
	MesASeq_PGCode,
	MesASeq_PGCodeList,
	MesASeq_MAX_NUM
};


//<<
enum
{
	PgCommand_KeyReset,
	PgCommand_KeyEnter,
	PgCommand_KeyBack,
	PgCommand_KeyNext,
	PgCommand_KeyFunc,
	PgCommand_KeyUp,
	PgCommand_KeyAuto,
	PgCommand_KeyDown,
	PgCommand_TurnOn,
	PgCommand_TurnOff,
	PgCommand_Pattern,
	PgCommand_GammaStart,
	PgCommand_Contact,
	PgCommand_TSPStart,
	PgCommand_GammaVerify,
	PgCommand_Max_Num
};

enum {
	MC_STATE_MANUAL,
	MC_STATE_AUTO
};

enum {
	CH_1,
	CH_2,
	ChMaxCount
};

enum {
	PC1 = 0,
	PC2,
	PCMaxCount
};

enum {
	PgServer_1,
	PgServer_2,
#if _SYSTEM_AMTAFT_
	PgServer_3,
#endif
	PgServerMaxCount
};

enum {
	PanelNum1,
	PanelNum2,
	PanelNum3,
	PanelNum4,
	PanelMaxCount
};

enum {
	AZone,
	BZone,
	CZone,
	DZone,
	MaxZone
};

enum
{
	RESET,
	OK,
	FAIL,
	TIMEOUT,
	RESPONSE_ERROR,
	SEND_RECEVIER_ERROR,
	PLC_PANEL_ERROR,
	GAMMACOUNTFULL,
	AMTMTPERROR,
	GAMMAREADERROR,
	Y9,
	CONTACT_NG,
	PLCRESULTMAX
};

enum
{
	_THREAD_ALIGN,
	_THREAD_VIEWING_ANGLE,
	_THREAD_VISION,
	_THREAD_PG,
	_THREAD_TP,
	_THREAD_OPV,
	_THREAD_LUMITOP,
	_THREAD_TOTAL_COUNT
};

enum
{
	_MODEL_CHECK_ALIGN,
	_MODEL_CHECK_VIEWING_ANGLE,
	_MODEL_CHECK_VISION1,
	_MODEL_CHECK_VISION2,
	_MODEL_CHECK_OPERATOR_VIEW,
	_MODEL_CHECK_LUMITOP,
	_MODEL_CHECK_TOTAL_COUNT
};

enum
{
	NUM_NETWORK_VISION_1,
	NUM_NETWORK_VISION_2,
	NUM_NETWORK_ALIGN,
	NUM_NETWORK_PLC,
	NUM_NETWORK_VIEWING_ANGLE_1,
	NUM_NETWORK_VIEWING_ANGLE_2,
	NUM_NETWORK_PG1,
	NUM_NETWORK_PG2,
	NUM_NETWORK_PG3,
	NUM_NETWORK_TP_1,
	NUM_NETWORK_OPV_1,
	NUM_NETWORK_OPV_2,
	NUM_NETWORK_ICON
};

enum
{
#if _SYSTEM_AMTAFT_
	PG_PREGAMMA = 1,
#else
	PG_GAMMA = 1,
#endif
	PG_CONTACT_ON,
	PG_CONTACT_OFF,
	PG_PATTERN_NEXT,
	PG_PATTERN_BACK,
	PG_TOUCH,
	PG_PID_CHECK,
	PG_STATUS_MAX_COUNT
};

enum {
	USER_OPERATOR,
	USER_ENGINEER,
	USER_MAKER,
	USER_ORIGINER
};

enum {
	_MP_Defect_Num,
	_MP_Defect_Code,
	_MP_Defect_Name,
	_MP_ptPos_x,
	_MP_ptPosYy,
	_MP_rectArea_height,
	_MP_rectArea_width,
	_MP_rectArea_x,
	_MP_rectArea_y,
	_MP_DEFECT_LABEL_PARAM_MAX
};

enum 
{
	PLCBIT,
	PLCWORD,
	PCBIT,
	PCWORD,
	PCTOTLACOUNT
};

enum 
{
	KOR,
	ENG,
	CHI
};
enum
{
	AOIdfs,
	VIEWdfs,
	LUMITOPdfs,
	OPVdfs,
	SUMdfs
};

enum 
{
	CAM1,
	CAM2,
	MaxCamCount
};

enum{
	SERIAL_NO,
	PANEL_ID,
	AT_DEFECT_CODE,
	AT_DEFECT_TYPE,
	AT_DEFECT_JUDGE_FLAG,
	AT_DEFECT_TYPE_SEQ_NO,
	REPAIR_DEFECT_CODE,
	REPAIR_DEFECT_TYPE_SEQ_NO,
	GATE_START,
	DATA_START,
	GATE_END,
	DATA_END,
	X_START,
	Y_START,
	X_END,
	Y_END,
	AVG_POSITIVE_DEFECT_VOLTAGE,
	AVG_NEGATIVE_DEFECT_VOLTAGE,
	SITE_MEAN_V_P,
	SITE_MEAN_V_N,
	IMAGEFILE,
	MAXAMTAOTINFO
};

enum{
	_STATE_CONFIRM_PASSWORD,
	_STATE_INPUT_PASSWORD,
	_STATE_CHANGE_PASSWORD
};

enum{
	PG_CH_01,
	PG_CH_02,
	PG_CH_03,
	PG_CH_04,
	PG_CH_05,
	PG_CH_06,
	PG_CH_07,
	PG_CH_08,
	PG_CH_09,
	PG_CH_10,
	PG_CH_11,
	PG_CH_12,
	PG_CH_13,
	PG_CH_14,
	PG_CH_15,
	PG_CH_16,
	PG_CH_17,
	PG_CH_18,
#if _SYSTEM_GAMMA_
	PG_CH_19,
	PG_CH_20,
	PG_CH_21,
	PG_CH_22,
	PG_CH_23,
	PG_CH_24,
#endif
	PG_MAX_CH
};

enum
{
	PGGammaTimer,
	PGContactOnTimer,
	PGContactOffTimer,
	PGNextTimer,
	PGBackTimer,
	PGTPTimer,
	VisionGrabTimer,
	VisionLastGrabTimer,
	ViewingAngleGrabTimer,
	ViewingAngleLastGrabTimer,
	AlignGrabTimer,
	AlignLastGrabTimer,
	LumitopGrabTimer,
	LumitopLastGrabTimer,
	MaxTimerCount
};

enum ShiftIndex
{
	Shift_DY,
	Shift_NT,
	eNumShift
};

enum
{
	Align_Start_T,
	Align_Start_XY,
	Align_Light_On = 1
};

enum
{
	Align_View,
	Contact_View,
	TP_View,
	PreGamma_View
};

enum
{
	Inspection_Start = 1,
	Inspection_No
};

enum
{
	Shift_Start,
	Shift_End,
	Shift_Count
};

enum
{
	VisionAoiNm,
	ViewingAngleNm,
	NameCount
};

enum
{
	HistoryMode,
	RankMode,
	ModeCount
};

enum
{
	InspectTime1,
	InspectTime2,
	InspectTime3,
	InspectTime4,
	InspectTime5,
	InspectTime6,
	InspectTime7,
	InspectTime8,
	InspectTime9,
	InspectTime10,
	InspectTime11,
	InspectTime12,
	InspectTimeTotalCount
};

enum{
	AOI,
	OPV,
	RankListCount
};

enum
{
	ChangeRank,
	AddRank
};

enum
{
	Panel_A_GRADE = 1,
	Panel_B_GRADE,
	Panel_C_GRADE,
};

enum{
	MechineType,
	DfsId,
	EqpNum,
	CompanyLine,
	FileServerID,
	OpvImageWidth,
	OpvImageHeight,
	OkGrade,
	ContactNgGrade,
	ContactNgCode,
	OK_ProcessID,
	NG_ProcessID,
};

enum
{
	IN_LOAD_WAIT,
	IN_LOAD_LOADER,
	IN_LOAD_STAGE,
	ALIGN_LOADER,
	ALIGN_STAGE,
	INDEX_LOADER,
	INDEX_AZONE,
	INDEX_BZONE,
	INDEX_CZONE,
	INDEX_DZONE,
	INDEX_TURN,
	INDEX_UNLOADER,
	UNLOADER_STAGE,
	UNLOADER_STAGE_WAIT,
	DCR_DATA_RECEIVE,
	TRAY_HANDLER_TRAY_IN,
	TRAY_HANDLER_IN_LOAD_STAGE_IN,
	IN_LOAD_STAGE_WAIT,
	Panel_Tact,
	AOI_TACTTIME
};

//enum // sylee
//{
//	UPPER_MACHINE,
//	GOOD_LOADER,
//	GOOD_UNLOADER,
//	SAMPLING_STAGE,
//	ALIGN_STAGE_NORMAL_MODE,
//	ALIGN_STAGE_INSERT_MODE,
//	BUFFER_TRAY_HANDLER_TRAY_IN,
//	BUFFER_TRAY_HANDLER_ALIGN_IN,
//	NG_LOADER,
//	MANUAL_STAGE_1,
//	MANUAL_STAGE_2,
//	NG_UNLOADER,
//	UNLOADER_STAGE,
//	LOWER_MACHINE,
//	NG_STAGE,
//	NG_TRAY_HANDLER,
//	DCR_1_DATA_RECEIVE,
//	DCR_2_DATA_RECEIVE,
//	UNLOADER_TACTTIME
//};

enum
{
	ModelCreate = 1,
	ModelChange
};

enum
{
	TactStart = 1,
	TactEnd,
	TactOther
};

enum
{
	Axis_data1,
	Axis_data2,
	Axis_data3,
	Axis_data4,
	Axis_data5,
	Axis_data6,
	Axis_data7,
	Axis_data8,
	Axis_data9,
	Axis_data10,
	Axis_data11,
	Axis_data12,
	Axis_data13,
	Axis_data14,
	Axis_data15,
	Axis_Count
};

enum
{
	AxisCameraX1,
	AxisCameraX2,
	AxisAllStageY,
	AxisStageX1,
	AxisStageY1,
	AxisStageX2,
	AxisStageY2,
	AlignAxisData_Count
};

enum
{
	ContactPanelCheck,
	TouchPanelCheck,
	PreGammaPanelCheck,
	OperatorViewPanelCheck
};

enum
{
	ManualStageContactOn,
	ManualStageContactOff,
	ManualStageNext,
	ManualStageBack,
	ManualStagePreGamma,
	ManualStageTouch,
	ManualStageOperatorView,
	ManualStageMaxCount
};

enum
{
	Align_Position_Error,
	Align_Count_Error,
	Align_PanelID_Error
};

enum
{
	Data_LowerMachineOut,
	Data_TrayOut,
	AOIMaxDataCount
};

enum
{
	Data_GoodOut,
	Data_NgOut,
	Data_SampleOut,
	Data_BufferTrayOut,
	Data_BufferTrayIn,
	UnloaderMaxDataCount
};

enum
{
	Card_Reader_ID,
	Card_Reader_PassWord
};

enum
{
	DataAdd,
	DataChange
};

enum
{
	UserList,
	UserCurrentList
};

enum
{
	Panel_ID,
	ModuleData_FPC_ID,
	Glass_Type,
	Product_ID,
	Owner_ID,
	Owner_Code,
	Owner_Type,
	Lot_ID,
	Process_ID,
	Recipe_ID,
	SaleOrder,
	PreProcess_ID_1,
	Group_ID,
	Product_Info,
	LOT_Info,
	Product_Group,
	From_Site,
	Current_Site,
	From_Shop,
	Current_Shop,
	Thickness,
	MMGFlag,
	PANELSIZE,
	PGRADE,
	ModuleData_Count
};

enum
{
	Cassette_Sequence_No,
	Job_Sequence_No,
	Group_Index,
	Product_Type,
	CST_Operation_Mode,
	SubStrate_Type,
	CIM_Mode,
	Job_Type,
	Job_Judge,
	Sampling_Slot_Flag,
	First_Run,
	Job_Grade,
	Job_ID,
	INSP_Reservation,
	EQP_Reservation,
	LastGlass_Flag,
	InspJudge_Data,
	Tracking_Data,
	EQP_Flag,
	Chip_Count,
	PP_ID,
	JobData_FPC_ID,
	Cassette_Setting_Code,
	JobData_Count
};

enum
{
	RankAOI,
	RankViewing,
};

enum
{
	TotalNgCount,
	OkCount,
	NgCount,
	OpvResultMax
};

enum
{
	PanelDefectCount,
	TitleDefectCount,
};

enum
{
	DefectTitleName_1,
	DefectTitleName_2,
	DefectTitleName_3,
	DefectTitleName_4,
	DefectTitleName_5,
	DefectTitleMaxCount
};

enum 
{
	GammaStage_1,
	GammaStage_2,
	GammaStage_3,
	GammaStage_4,
	GammaStage_5,
	GammaStage_6,
	GammaStage_7,
	GammaStage_8,
	GammaStage_9,
	GammaStage_10,
	GammaStage_11,
	GammaStage_12,
	MaxGammaStage
};

enum
{
	AlignCount_1,
	AlignCount_2,
	AlignCount_3,
	AlignCount_4,
	AlignCount_5,
	AlignCount_6,
	AlignCount_7,
	AlignCount_8,
	AlignCount_9,
	AlignCount_10,
	MaxAlignCount
};

enum
{
	PatternAlign,
	TrayCheck,
	TrayAlign,
	TrayLowerAlign,
	MaxAlignInspectType
};

enum
{
	GammaStageMtp1,
	GammaStageMtp2,
	GammaStageContactOn,
	GammaStageContactOff,
	GammaStageNext,
	GammaStageBack,
	GammaStagePIDCheck,
	GammaPGStatusCount
};

enum
{
	OKPanel,
	NGPanel,
	DfsMax
};

enum 
{
	Machine_AOI,
	Machine_ULD,
	Machine_GAMMA,
};

enum
{
	SetGAMMA,
	SetAMT,
	SetAFT,
};

enum
{
	PG_MuhanZC,
	PG_WithSystem
};

enum
{
	TP_CheckConnect,
	TP_ProductID,
	TP_SendPanelID,
	TP_InspStart,
	TP_InspResult,
	TP_CodeRequest, //210422
	TP_TotalPacketNum
};

enum
{
	FS_IP_1Line,
	FS_IP_2Line,
	FS_IP_3Line,
	FS_IP_4Line,
	FS_IP_5Line,
	FS_IPAdressCount
};

enum
{
	DefectCode,
	SameDefectMaxCount,
	SmaeDefectAlarmCount
};

enum
{
	SetSystemParm,
	SetVisionParm,
};
//>>210422
enum
{
	SocketPG,
	SocketTP,
	SocketCodeMax
};
//<<
#define MAX_SEND_LEN							(1024)
#define MAX_RECEIVE_LEN							(1024)
#define MAX_RECV_BUFF_COUNT						(10)
#define _TIMER_DISPLAY_TIME						(1008)

#define MAX_MSG_LEN								(4096)

#define _DUMMY 0x00
#define _STX 0x02
#define _ETX 0x03
#define _DEST 0x80
#define _DEVICEID 0xFF

const CString LOG_CFG_PATH = _T("D:\\ANI\\DataServer\\Bin");
const CString LOG_PATH = _T("D:\\ANI\\DataServer\\Log\\");
const CString LOG_PLC_LOG_PATH = LOG_PATH + _T("PLCLog\\");
const CString LOG_PlcHeartBit_LOG_PATH = LOG_PATH + _T("PlcHeartBitLog\\");
const CString LOG_VIEWING_ANGLE_LOG_PATH = LOG_PATH + _T("ViewingAngleLog\\");
const CString LOG_VISION_LOG_PATH = LOG_PATH + _T("VisionLog\\");
const CString LOG_LUMITOP_LOG_PATH = LOG_PATH + _T("LumitopLog\\");
const CString LOG_ALIGN_LOG_PATH = LOG_PATH + _T("AlignLog\\");
const CString LOG_TIME_OUT_LOG_PATH = LOG_PATH + _T("TimeOut\\");
const CString LOG_PG_LOG_PATH = LOG_PATH + _T("PgLog\\");
const CString LOG_PG_SEND_RECEIVER_LOG_PATH = LOG_PATH + _T("PgSendReceiverLog\\");
const CString LOG_FTP_LOG_PATH = LOG_PATH + _T("FtpLog\\");
const CString LOG_VISION_SEND_RECIEVER_LOG_1 = LOG_PATH + _T("VisionSendReceiver1Log\\");
const CString LOG_VISION_SEND_RECIEVER_LOG_2 = LOG_PATH + _T("VisionSendReceiver2Log\\");
const CString LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_1 = LOG_PATH + _T("ViewingAngleSendReceiver1Log\\");
const CString LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_2 = LOG_PATH + _T("ViewingAngleSendReceiver2Log\\");
const CString LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_3 = LOG_PATH + _T("ViewingAngleSendReceiver3Log\\");
const CString LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_4 = LOG_PATH + _T("ViewingAngleSendReceiver4Log\\");
const CString LOG_LUMITOP_SEND_RECIEVER_LOG_1 = LOG_PATH + _T("LumitopSendReceiver1Log\\");
const CString LOG_LUMITOP_SEND_RECIEVER_LOG_2 = LOG_PATH + _T("LumitopSendReceiver2Log\\");
const CString LOG_ALARM_HISTORY_PATH = LOG_PATH + _T("AlarmHistory\\");
const CString LOG_DATA_STATUS_PATH = LOG_PATH + _T("DataStatus\\");
const CString LOG_TACT_TIME_PATH = LOG_PATH + _T("TactTime\\");
const CString LOG_TRACE_PATH = LOG_PATH + _T("TraceLog\\");
const CString LOG_AXIS_PATH = LOG_PATH + _T("AxisLog\\");
const CString LOG_OPERATE_TIME_PATH = LOG_PATH + _T("OperateTimeLog\\");
const CString LOG_TP_PATH = LOG_PATH + _T("TpLog\\");
const CString LOG_TP_SEND_RECIEVER_PATH = LOG_PATH + _T("TpSendReceiverLog\\");
const CString LOG_OPV_PATH = LOG_PATH + _T("OpvLog\\");
const CString LOG_OPV_SEND_RECIEVER1_PATH = LOG_PATH + _T("OpvSendReceiver1Log\\");
const CString LOG_OPV_SEND_RECIEVER2_PATH = LOG_PATH + _T("OpvSendReceiver2Log\\");
const CString LOG_USER_LOGIN_OUT_PATH = LOG_PATH + _T("UserLoginOutLog\\");
const CString LOG_USER_PATH = LOG_PATH + _T("UserLog\\");
const CString LOG_USER_HISTORY_PATH = LOG_PATH + _T("UserHistory\\");
const CString LOG_FFU_SEND_RECIEVER_PATH = LOG_PATH + _T("FFUSendReceiver\\");
const CString LOG_SEND_DEFECT_CODE_PATH = LOG_PATH + _T("SendDefectCodeLog\\");
const CString LOG_ARS_SEND_RECIEVER_PATH = LOG_PATH + _T("ARSSendReceiver\\");

const CString DATA_PATH = _T("D:\\ANI\\DataServer\\Data\\");
const CString DATA_SYSTEM_PATH = DATA_PATH + _T("System\\");
const CString DATA_SYSTEM_DATA_PATH = DATA_SYSTEM_PATH + _T("sysData.ini");
const CString DATA_SYSTEM_PM_MOCDE_USER_DATA_PATH = DATA_SYSTEM_PATH + _T("PmModeUserData.ini");
const CString DATA_SYSTEM_TACT_NAME = DATA_SYSTEM_PATH + _T("TactName.ini");
const CString DATA_SYSTEM_SET_NGRANK_PATH = DATA_SYSTEM_PATH + _T("SetRank.ini");
const CString DATA_SYSTEM_ALARM_PATH = DATA_SYSTEM_PATH + _T("Alarm\\");
const CString DATA_SYSTEM_AXIS_PATH = DATA_SYSTEM_PATH + _T("Axis\\");
const CString DATA_SYSTEM_DATA_SUM_PATH = DATA_SYSTEM_PATH + _T("JobDataSum");
const CString DATA_DEFECT_CODE_PATH = DATA_SYSTEM_PATH + _T("DefectCodeInfo");
const CString DATA_GAMMA_DFS_INFO_PATH = DATA_SYSTEM_PATH + _T("GammaDfsInfo");
const CString DATA_PG_DFS_INFO_PATH = DATA_SYSTEM_PATH + _T("PGDfsInfo");
//const CString DATA_OPV_PATH = _T("D:\\ANI\\OperatorView\\Data\\");
const CString DATA_OPV_PC_1_PATH = _T("\\\\192.168.1.202\\ANI\\OperatorView\\Data\\");
const CString DATA_DEFECTLIST_1_PATH = DATA_OPV_PC_1_PATH + _T("Model\\");
const CString DATA_OPV_SYSTEM_SET_SYSTEM_DATA_1_PATH = DATA_OPV_PC_1_PATH + _T("System\\");

const CString DATA_OPV_PC_2_PATH = _T("\\\\192.168.1.203\\ANI\\OperatorView\\Data\\");
const CString DATA_DEFECTLIST_2_PATH = DATA_OPV_PC_2_PATH + _T("Model\\");
const CString DATA_OPV_SYSTEM_SET_SYSTEM_DATA_2_PATH = DATA_OPV_PC_2_PATH + _T("System\\");

const CString DATA_PREGAMMA_PATH = DATA_PATH + _T("PreGamma\\");
const CString DATA_PREGAMMA_PATHTIME = DATA_PATH + _T("PreGammaTime\\");
const CString DATA_TP_PATH = DATA_PATH + _T("TP\\");
const CString DATA_TP_PATHTIME = DATA_PATH + _T("TPTime\\");
const CString DATA_CONTACT_PATH = DATA_PATH + _T("Contact\\");
const CString DATA_CONTACT_PATHTIME = DATA_PATH + _T("ContactTime\\");

const CString DATA_ALARM_PATH = DATA_PATH + _T("Alarm\\");
const CString DATA_TACT_TIME_PATH = DATA_PATH + _T("TactTime\\");
const CString DATA_TACT_TIME_UNIT_PATH = DATA_PATH + _T("TactTimeUnit\\");
const CString DATA_INSPECT_RESULT_CODE_PATH = DATA_PATH + _T("ResultCode\\");
const CString DATA_ALARM_COUNT_PATH = DATA_PATH + _T("AlarmCount\\");
const CString DATA_INSPECT_LOGIN_OUT_PATH = DATA_PATH + _T("LoginOut\\");
const CString DATA_MTP_PATH = DATA_PATH + _T("Mtp\\");
const CString DATA_MTP_PATHTIME = DATA_PATH + _T("MtpTime\\");
const CString DATA_OPV_DEFECT_LIST_PATH = DATA_PATH + _T("OpvDefectList\\");
const CString DATA_OPV_DEFECT_HISTORY_PATH = DATA_PATH + _T("OpvDefectHistory\\");
const CString DATA_OPV_DEFECT_CODE_PATH = DATA_PATH + _T("OpvDefectCode\\");
const CString DATA_OPV_SUM_DEFECT_CODE_PATH = DATA_PATH + _T("OpvSumDefectCode\\");
const CString DATA_TOTAL_CHECK_PATH = _T("C:\\Users\\USER1\\Desktop\\");

#if _SYSTEM_AMTAFT_
const CString DATA_INSPECT_PATH = _T("D:\\ANI\\DataServer\\Data\\Inspect\\");
const CString DATA_INSPECT_PATHTIME = _T("D:\\ANI\\DataServer\\Data\\InspectTime\\");
const CString DATA_INSPECT_CSV_PATH = _T("D:\\ANI\\DataServer\\Data\\InspectCSV\\");
const CString DATA_ALIGN_PATH = _T("D:\\ANI\\DataServer\\Data\\Align\\");
const CString DATA_ALIGN_PATHTIME = _T("D:\\ANI\\DataServer\\Data\\AlignTime\\");

const CString AOI_DATA_INSPECT_PATH = DATA_INSPECT_PATH + _T("AOI\\");
const CString AOI_DATA_INSPECT_PATHTIME = DATA_INSPECT_PATHTIME + _T("AOI\\");
const CString AOI_DATA_INSPECT_CSV_PATH = DATA_INSPECT_CSV_PATH + _T("AOI\\");
const CString AOI_DATA_ALIGN_PATH = DATA_ALIGN_PATH + _T("AOI\\");
const CString AOI_DATA_ALIGN_PATHTIME = DATA_ALIGN_PATHTIME + _T("AOI\\");

const CString ULD_DATA_INSPECT_PATH = DATA_INSPECT_PATH + _T("UNLOADER\\");
const CString ULD_DATA_INSPECT_PATHTIME = DATA_INSPECT_PATHTIME + _T("UNLOADER\\");
const CString ULD_DATA_INSPECT_CSV_PATH = DATA_INSPECT_CSV_PATH + _T("UNLOADER\\");
const CString ULD_DATA_ALIGN_PATH = DATA_ALIGN_PATH + _T("UNLOADER\\");
const CString ULD_DATA_ALIGN_PATHTIME = DATA_ALIGN_PATHTIME + _T("UNLOADER\\");
#else
const CString DATA_INSPECT_PATH = DATA_PATH + _T("Inspect\\");
const CString DATA_INSPECT_PATHTIME = DATA_PATH + _T("InspectTime\\");
const CString DATA_INSPECT_CSV_PATH = DATA_PATH + _T("InspectCsv\\");
const CString DATA_ALIGN_PATH = DATA_PATH + _T("Align\\");
const CString DATA_ALIGN_PATHTIME = DATA_PATH + _T("AlignTime\\");
#endif

#if _SYSTEM_AMTAFT_
const CString DATA_PC_ADDR_PATH_BIT = DATA_SYSTEM_PATH + _T("AddrAmtPC_DataBit.ini");
const CString DATA_PC_ADDR_PATH_WORD = DATA_SYSTEM_PATH + _T("AddrAmtPC_DataWord.ini");
const CString DATA_PLC_ADDR_PATH_BIT = DATA_SYSTEM_PATH + _T("AddrAmtPLC_DataBit.ini");
const CString DATA_PLC_ADDR_PATH_WORD = DATA_SYSTEM_PATH + _T("AddrAmtPLC_DataWord.ini");
#else
const CString DATA_PC_ADDR_PATH_BIT = DATA_SYSTEM_PATH + _T("AddrGammaPC_DataBit.ini");
const CString DATA_PC_ADDR_PATH_WORD = DATA_SYSTEM_PATH + _T("AddrGammaPC_DataWord.ini");
const CString DATA_PLC_ADDR_PATH_BIT = DATA_SYSTEM_PATH + _T("AddrGammaPLC_DataBit.ini");
const CString DATA_PLC_ADDR_PATH_WORD = DATA_SYSTEM_PATH + _T("AddrGammaPLC_DataWord.ini");
#endif

//Port
#if _SYSTEM_AMTAFT_
const CString VISION_PC1_PORT_NUM = _T("8011");
const CString VISION_PC2_PORT_NUM = _T("8012");
const CString LUMITOP_PC1_PORT_NUM = _T("8013");
const CString LUMITOP_PC2_PORT_NUM = _T("8014");
const CString VIEWING_ANGLE_PANEL1_PORT_NUM = _T("8015");
const CString VIEWING_ANGLE_PANEL2_PORT_NUM = _T("8016");
const CString VIEWING_ANGLE_PANEL3_PORT_NUM = _T("8017");
const CString VIEWING_ANGLE_PANEL4_PORT_NUM = _T("8018");
const CString TP_PC_PORT_NUM = _T("8019");
const CString OPV1_PORT_NUM = _T("8020");
const CString OPV2_PORT_NUM = _T("8021");
#endif

const CString MC_PacketNameTable[MC_PACKET_MAX_NUM] =
{
	_T("MC_ARE_YOU_THERE"),
	_T("MC_PCTIME"),
	_T("MC_STATE"),
	_T("MC_STATE_REQUEST"),
	_T("MC_MODEL"),
	_T("MC_MODEL_REQUEST"),
	_T("MC_MODEL_CREATE"),
	_T("MC_MODEL_DELETE"),
	_T("MC_MODEL_MODIFY"),
	_T("MC_MODEL_CHANGE"),
	_T("MC_INSPECTION_START"),
	_T("MC_GRAB_READY_REQUEST"),
	_T("MC_GRAB_END_RECEIVE"),
	_T("MC_INSPECTION_REQUEST"),
	_T("MC_INSPECTION_END"),
	_T("MC_INSPECTION_RESULT_RECEIVE"),
	_T("MC_INSPECTION_JUDGE_REQUEST"),
	_T("MC_INSPECTION_JUDGE_RECEIVE"),
	_T("MC_INSECTION_OTP_START"),
	_T("MC_INSECTION_OTP_END"),
	_T("MC_INSECTION_OTP_RESULT_RECEIVE"),
	_T("MC_ALIGN_LIGHT_CONTROL"),
	_T("MC_ALIGN_AXIS_RESULT_END"),
	_T("MC_ALIGN_LIGHT_OFF"),
	_T("MC_Z_MOVE_DONE"),
	_T("MC_FOCUS_MOVE_DONE"),
	_T("MC_Z_SAVE_POS_DONE"),
	_T("MC_FOCUS_SAVE_POS_DONE"),
	_T("MC_OPV_LOGIN_DONE"),
	_T("MC_OPV_LOGOUT_DONE"),
	_T("MC_VIEWING_TEST"),
};

const CString VS_PacketNameTable[VS_PACKET_MAX_NUM] =
{
	_T("VS_ARE_YOU_THERE"),
	_T("VS_PCTIME_REQUEST"),
	_T("VS_STATE"),
	_T("VS_STATE_REQUEST"),
	_T("VS_MODEL"),
	_T("VS_MODEL_REQUEST"),
	_T("VS_MODEL_CREATE"),
	_T("VS_MODEL_DELETE"),
	_T("VS_MODEL_MODIFY"),
	_T("VS_MODEL_CHANGE"),
	_T("VS_GRAB_START"),
	_T("VS_GRAB_END"),
	_T("VS_INSPECTION_OK"),
	_T("VS_INSPECTION_RESULT"),
	_T("VS_INSPECTION_JUDGE"),
	_T("VS_INSECTION_OTP_OK"),
	_T("VS_INSECTION_OTP_RESULT"),
	_T("VS_INSECTION_START_ING"),
	_T("VS_ALIGN_AXIS_RESULT_START"),
	_T("VS_AUTO_CAM_SET_START"),
	_T("VS_Z_MOVE_REQUEST"),
	_T("VS_FOCUS_MOVE_REQUEST"),
	_T("VS_Z_SAVE_POS_REQUEST"),
	_T("VS_FOCUS_SAVE_POS_REQUEST"),
	_T("VS_OPV_LOGIN_REQUEST"),
	_T("VS_OPV_LOGOUT_REQUEST"),
	_T("VS_VISION_TEST"),
	_T("VS_VIEWING_TEST"),
};

const CString PG_PacketNameTable[PgCommand_Max_Num] =
{
	_T("CONTACTOFF"), 
	_T("KEY,ENTER"), 
	_T("KEY,BACK"), 
	_T("KEY,NEXT"), 
	_T("KEY,FUNC"), 
	_T("KEY,UP"),
	_T("KEY,AUTO"), 
	_T("KEY,DOWN"), 
	_T("TURNON"),
	_T("TURNOFF"),
	_T("PTRN"),
	_T("GAMMA,START"), 
	_T("CONTACT"),
	_T("TSP,START"),
	_T("PREGAMMA,START")
};

const CString PLC_ResultValue[PLCRESULTMAX] =
{
	_T("CONTACTOFF"),
	_T("OK"),
	_T("Fail"),
	_T("TimeOut"),
	_T("Response Error"),
	_T("Send/Recevier Error"),
	_T("PLC Panel Error"),
	_T("PLC Position Error"),
	_T("Dfs File Error"),
	_T("Align Count Error"),
	_T("Y9"),
	_T("Contact NG")
};
#if _SYSTEM_AMTAFT_
const CString PG_IndexName[MaxZone] =
{
	_T("AZone"),
	_T("BZone"),
	_T("CZone"),
	_T("DZone")
};

const CString PG_UIIndexName[MaxZone] =
{
	_T("UI_AZone"),
	_T("UI_BZone"),
	_T("UI_CZone"),
	_T("UI_DZone")
};

const CString ULD_PG_IndexName[ChMaxCount] =
{
	_T("MStage1"),
	_T("MStage2")
};

const CString ULD_PG_UIIndexName[ChMaxCount] =
{
	_T("UI_MStage1"),
	_T("UI_MStage2")
};
#else
const CString PG_IndexName[MaxGammaStage] =
{
	_T("Stage1"),
	_T("Stage2"),
	_T("Stage3"),
	_T("Stage4"),
	_T("Stage5"),
	_T("Stage6"),
	_T("Stage7"),
	_T("Stage8"),
	_T("Stage9"),
	_T("Stage10"),
	_T("Stage11"),
	_T("Stage12")
};

const CString PG_UIIndexName[MaxGammaStage] =
{
	_T("UI_Stage1"),
	_T("UI_Stage2"),
	_T("UI_Stage3"),
	_T("UI_Stage4"),
	_T("UI_Stage5"),
	_T("UI_Stage6"),
	_T("UI_Stage7"),
	_T("UI_Stage8"),
	_T("UI_Stage9"),
	_T("UI_Stage10"),
	_T("UI_Stage11"),
	_T("UI_Stage12")
};
#endif

const CString ChName[ChMaxCount] =
{
	_T("CH1"),
	_T("CH2")

};

const CString UI_ChName[ChMaxCount] =
{
	_T("UI_CH1"),
	_T("UI_CH2")
};

const CString Defect_label[_MP_DEFECT_LABEL_PARAM_MAX] =
{
	_T("Defect_Num"),
	_T("Defect_Code"),
	_T("Defect_Name"),
	_T("ptPos.x"),
	_T("ptPos.y"),
	_T("rectArea.height"),
	_T("rectArea.width"),
	_T("rectArea.x"),
	_T("rectArea.y")
};

const CString AMTAOTInfo[MAXAMTAOTINFO] =
{
	_T("SERIAL_NO"),
	_T("PANEL_ID"),
	_T("AT_DEFECT_CODE"),
	_T("AT_DEFECT_TYPE"),
	_T("AT_DEFECT_JUDGE_FLAG"),
	_T("AT_DEFECT_TYPE_SEQ_NO"),
	_T("REPAIR_DEFECT_CODE"),
	_T("REPAIR_DEFECT_TYPE_SEQ_NO"),
	_T("GATE_START"),
	_T("DATA_START"),
	_T("GATE_END"),
	_T("DATA_END"),
	_T("X_START"),
	_T("Y_START"),
	_T("X_END"),
	_T("Y_END"),
	_T("AVG_POSITIVE_DEFECT_VOLTAGE"),
	_T("AVG_NEGATIVE_DEFECT_VOLTAGE"),
	_T("SITE_MEAN_V_P"),
	_T("SITE_MEAN_V_N"),
	_T("IMAGEFILE:")
};

const CString PG[PG_STATUS_MAX_COUNT] =
{
	_T("PG_PREGAMMA"),
	_T("PG_CONTACT_ON"),
	_T("PG_CONTACT_OFF"),
	_T("PG_PATTERN_NEXT"),
	_T("PG_PATTERN_BACK"),
	_T("PG_TP"),
};

const CString ShiftDY_NT[eNumShift] =
{
	_T("DY"),
	_T("NT"),
};

const CString InspectName[NameCount] =
{
	_T("AOI"),
	_T("Viewing"),
};

const CString RankIniTital[RankListCount] =
{
	_T("AOI_NG_RANK"),
	_T("OPV_NG_RANK")
};

const CString DataOutName[AOIMaxDataCount] =
{
	_T("Data_LowerMachineOut"),
	_T("Data_TrayOut")
};

const CString UnloaderDataOutName[UnloaderMaxDataCount] =
{
	_T("Data_GoodOut"),
	_T("Data_NgOut"),
	_T("Data_SampleOut"),
	_T("Data_BufferTrayOut")
};

//음... 이걸 고정으로 갈까... 아니면 받아서 사용할까.. 생각좀하고 변경하자
const CString TP_IP_NAME[PG_MAX_CH] =
{
	_T("192.168.1.150"),
	_T("192.168.1.151"),
	_T("192.168.1.152"),
	_T("192.168.1.153"),
	_T("192.168.1.154"),
	_T("192.168.1.155"),
	_T("192.168.1.156"),
	_T("192.168.1.157"),
	_T("192.168.1.158"),
	_T("192.168.1.159"),
	_T("192.168.1.160"),
	_T("192.168.1.161"),
	_T("192.168.1.162"),
	_T("192.168.1.163"),
	_T("192.168.1.164"),
	_T("192.168.1.165")
	_T("192.168.1.166"),
	_T("192.168.1.167"),
};

const CString ALIGN_PORT_NUM[MaxAlignCount] = 
{
	_T("8001"),
	_T("8002"),
	_T("8003"),
	_T("8004"),
	_T("8005"),
	_T("8006"),
	_T("8007"),
	_T("8008"),
	_T("8009"),
	_T("8010")
};

const CString PG_PORT_NUM[PgServerMaxCount] =
{
	_T("55000"),
	_T("55009"),
#if _SYSTEM_AMTAFT_
	_T("55010")
#endif
};

const CString OpvResultName[OpvResultMax] =
{
	_T("TotalNG"),
	_T("OK"),
	_T("NG")
};

const CString AlignTypeName[MaxAlignInspectType]=
{
	_T("PatternAlign"),
	_T("TrayCheck"),
	_T("TrayAlign"),
	_T("TrayLowerAlign")
};
//>> 210422 yjlim
struct PGCoderesult
{
	CString m_cellId;
	//CString m_PGCode;
	int iChNum;
	CTimerCheck time_check;
	CString m_PGCode[10]; // Granda 자,수동, 징처 자,수동

	void Reset()
	{
		m_cellId = _T("");
		for (int i = 0; i < 10; i++)
			m_PGCode[i] = _T("");
		//m_PGCode.clear();
		iChNum = 0;
		time_check.StopTimer();
	}
};


enum
{
	PGCode_OK = 1,
	PGCode_NG,
	PGCode_AckTimeout
};

//<<
struct InspResult
{
	CString m_strZoneName;
	CString m_cellId;
	CString m_FpcID;
	CString m_strcellId[ChMaxCount];
	BOOL m_bInspStart;
	BOOL m_LastCheck;
	BOOL m_bResult;
	BOOL m_bContactOn;
	BOOL m_bGrabEnd;
	USHORT m_iResultValue;
	CTimerCheck time_check;
	int m_iIndexPanelNum;
	int m_iPanelNum;
	int m_iPCNum;
	int m_iStatus;
	int m_iCurIndex;
	int m_iGammaRunChNum;

	void Reset()
	{
		m_iResultValue = m_iIndexPanelNum = m_iPanelNum = m_iPCNum = m_iStatus = m_iCurIndex = m_iGammaRunChNum = 0;
		m_bGrabEnd = m_bContactOn = m_bResult = m_LastCheck = m_bInspStart = FALSE;
		m_FpcID = m_strZoneName = m_cellId = m_strcellId[CH_1] = m_strcellId[CH_2] = _T("");
		time_check.StopTimer();
	}
};

struct Packet
{
	CString m_strCommand;
	CString m_strContents;
};

struct IndexList
{
	int m_indexNum;
	int m_indexProgramNum;
};

struct ModelName
{
	int m_AlignPcCurrentModelNum;
	CString m_AlignPcCurrentModelName;

	int m_AlignPcPreviousModelNum;
	CString m_AlignPcPreviousModelName;

	BOOL m_bModelChange;
	BOOL m_bModelCreate;
	BOOL m_bModelCopy;
};

#if _SYSTEM_AMTAFT_
struct AOIProductionData
{
	int m_InspectionTotal[eNumShift];
	int m_AlignResult[eNumShift];
	int m_GoodResult[eNumShift];
	int m_BadResult[eNumShift];
	int m_UnKnowResult[eNumShift];
	int m_ContactResult[eNumShift];
	int m_AlignShiftGood[eNumShift][PanelMaxCount];
	int m_AlignShiftNg[eNumShift][PanelMaxCount];
	int m_ContactGood[eNumShift][PanelMaxCount];
	int m_ContactNg[eNumShift][PanelMaxCount];
	int m_TpGood[eNumShift][PanelMaxCount];
	int m_TpNg[eNumShift][PanelMaxCount];
	int m_PreGammaGood[eNumShift][PanelMaxCount];
	int m_PreGammaNg[eNumShift][PanelMaxCount];
	int m_VisionResult[eNumShift];
	int m_ViewingResult[eNumShift];
	int m_TpResult[eNumShift];
	int m_GoodAGradeResult[eNumShift];
	int m_GoodBGradeResult[eNumShift];
	int m_GoodCGradeResult[eNumShift];
	int m_UpperInsertstatus[eNumShift];
	int m_TrayInsertstatus[eNumShift];
	int m_LowerDataOut[eNumShift];
	int m_TrayDataOut[eNumShift];
	int m_PreGammaResult[eNumShift];
	int m_FirstContactResult[eNumShift];
	int m_FirstContactNG[eNumShift][PanelMaxCount];

	void Reset(int iNumShift)
	{
		m_GoodAGradeResult[iNumShift] = m_GoodBGradeResult[iNumShift] = m_GoodCGradeResult[iNumShift] = m_TpResult[iNumShift] = m_ViewingResult[iNumShift] = m_VisionResult[iNumShift] = 0;
		m_PreGammaResult[iNumShift] = m_BadResult[iNumShift] = m_GoodResult[iNumShift] = m_ContactResult[iNumShift] = 0;
		m_FirstContactResult[iNumShift] = m_AlignResult[iNumShift] = m_UnKnowResult[iNumShift] = m_InspectionTotal[iNumShift] = 0;
		m_UpperInsertstatus[iNumShift] = m_TrayInsertstatus[iNumShift] = m_LowerDataOut[iNumShift] = m_TrayDataOut[iNumShift] = 0;
		for (int ii = 0; ii < PanelMaxCount; ii++)
		{
			m_AlignShiftGood[iNumShift][ii] = 0;
			m_AlignShiftNg[iNumShift][ii] = 0;
			m_ContactGood[iNumShift][ii] = 0;
			m_ContactNg[iNumShift][ii] = 0;
			m_TpGood[iNumShift][ii] = 0;
			m_TpNg[iNumShift][ii] = 0;
			m_PreGammaGood[iNumShift][ii] = 0;
			m_PreGammaNg[iNumShift][ii] = 0;
			m_FirstContactNG[iNumShift][ii] = 0;
		}
		
	}
};

struct ULDProductionData
{
	int m_InspectionTotal[eNumShift];
	int m_GoodResult[eNumShift];
	int m_BadResult[eNumShift];
	int m_SampleResult[eNumShift];
	int m_BufferTrayResult[eNumShift];
	int m_AlignResult[eNumShift];
	int m_ContactResult[eNumShift];
	int m_ManualContactResult[eNumShift];
	int m_GammaResult[eNumShift];
	int m_TouchResult[eNumShift];
	int m_OpvResult[eNumShift];
	int m_TrayInsertstatus[eNumShift];
	int m_UpperInsertstatus[eNumShift];
	int m_FirstContactResult[eNumShift];

	int m_AlignShiftGood[eNumShift];
	int m_AlignShiftNg[eNumShift];
	int m_TpGood[eNumShift];
	int m_TpNg[eNumShift];

	void Reset(int iNumShift)
	{
		m_UpperInsertstatus[iNumShift] = m_TrayInsertstatus[iNumShift] = m_OpvResult[iNumShift] = m_InspectionTotal[iNumShift] = m_GoodResult[iNumShift] = 0;
		m_ManualContactResult[iNumShift] = m_BadResult[iNumShift] = m_AlignResult[iNumShift] = m_ContactResult[iNumShift] = m_TouchResult[iNumShift] = 0;
		m_FirstContactResult[iNumShift] = m_GammaResult[iNumShift] = m_BufferTrayResult[iNumShift] = m_SampleResult[iNumShift] = 0;
		m_TpGood[iNumShift] = m_TpNg[iNumShift] = m_AlignShiftNg[iNumShift] = m_AlignShiftGood[iNumShift] = 0;
	}
};

struct DefectCountData
{
	CString m_strTime;
	CString m_strPanelID;
	CString m_strOperationID;
	CString m_strOpvResult;
	int	m_iChNum;
	int m_iTotalMatch;
	int m_iTotalOverKill;
	int m_iTotalUnderKill;
	int m_AoiDefectTotalSum[DefectTitleMaxCount];
	int m_OpvDefectTotalSum[DefectTitleMaxCount];
	int m_MatchDefectTotalSum[DefectTitleMaxCount];
	int m_OverKillDefectTotalSum[DefectTitleMaxCount];
	int m_UnderKillDefectTotalSum[DefectTitleMaxCount];
	int m_TotalDefectSum[DefectTitleMaxCount];
	int m_OpvTotalNgSum;
	int m_OpvOkSum;
	int m_OpvNgSum;

	void Reset()
	{
		m_OpvTotalNgSum = 0;
		m_OpvOkSum = 0;
		m_OpvNgSum = 0;
		for (int ii = 0; ii < DefectTitleMaxCount; ii++)
		{
			m_AoiDefectTotalSum[ii] = 0;
			m_OpvDefectTotalSum[ii] = 0;
			m_MatchDefectTotalSum[ii] = 0;
			m_OverKillDefectTotalSum[ii] = 0;
			m_UnderKillDefectTotalSum[ii] = 0;
			m_TotalDefectSum[ii] = 0;
		}
	}
};

struct DefectSumCountData
{
	int m_AoiDefectTotalSum;
	int m_OpvDefectTotalSum;
	int m_MatchDefectTotalSum;
	int m_OverKillDefectTotalSum;
	int m_UnderKillDefectTotalSum;
	int m_TotalDefectSum;
	int m_OpvTotalNgSum;
	int m_OpvOkSum;
	int m_OpvNgSum;

	void Reset()
	{
		m_AoiDefectTotalSum = m_OpvDefectTotalSum = m_MatchDefectTotalSum = m_OverKillDefectTotalSum = m_UnderKillDefectTotalSum = m_TotalDefectSum = m_OpvTotalNgSum = m_OpvOkSum = m_OpvNgSum = 0;
	}
};
#else
struct ProductionData
{
	int m_InspectionTotal[eNumShift];
	int m_GoodResult[eNumShift];
	int m_BadResult[eNumShift];
	int m_ContactResult[eNumShift];
	int m_ManualContactResult[eNumShift];
	int m_MtpResult[eNumShift];
	int m_AlignResult[eNumShift];
	int m_FirstContactResult[eNumShift];
	int m_ContactGood[eNumShift][ChMaxCount];
	int m_ContactNg[eNumShift][ChMaxCount];
	int m_MtpGood[eNumShift][ChMaxCount];
	int m_MtpNg[eNumShift][ChMaxCount];
	int m_AlignShiftGood[eNumShift][ChMaxCount];
	int m_AlignShiftNg[eNumShift][ChMaxCount];
	int m_FirstContactNG[eNumShift][ChMaxCount];

	void Reset(int iNumShift)
	{
		m_InspectionTotal[iNumShift] = m_GoodResult[iNumShift] = m_BadResult[iNumShift] = 
			m_ContactResult[iNumShift] = m_ManualContactResult[iNumShift] = m_FirstContactResult[iNumShift] = m_MtpResult[iNumShift] = m_AlignResult[iNumShift] = 0;

		for(int ii = 0; ii < ChMaxCount; ii++)
		{
			m_ContactGood[iNumShift][ii] = m_ContactNg[iNumShift][ii] = m_MtpGood[iNumShift][ii] = m_MtpNg[iNumShift][ii] = m_AlignShiftGood[iNumShift][ii] = m_AlignShiftNg[iNumShift][ii] = m_FirstContactNG[iNumShift][ii] = 0;
		}
	}
};
#endif

struct EsdData
{
	int m_iAddr;
	CString m_strEsdPole;
	int m_iEsdData1;
	int m_iEsdData2;
	int m_iEsdData3;
	int m_iEsdData4;
	int m_iEsdData5;
	CString m_strEsdResult;
};

struct AlarmDataItem
{
	CString m_strTime;
	CString m_alarmCode;
	CString m_alarmMsg;
	CString m_alarmClearTime;
	CString m_alarmStartTime;
	CString m_alarmEndTime;
	int m_alarmCount;
};

struct DataStatusItem
{
	CString DataDefect;
	CString DataContactStatus;
	CString DataManualContactStatus;
	CString DataVisionStatus;
	CString DataViewingStatus;
	CString DataGammaStatus;
	CString DataTpStatus;
	CString DataOpvStatus;
	CString DataOkGrade;
	CString DataInStatus;
	CString DataOutStatus;
	CString DataOtpStatus;
	CString DataStageNum;
	CString DataFirstContactStatus;
	CString DataGammaTuningTime;
	CString DataInTrayNum;
	CString DataCIEStatus;
	CString DataGammaTuningStatus;
	
	void Reset()
	{
		DataFirstContactStatus = DataOtpStatus = DataManualContactStatus = DataOpvStatus = DataTpStatus = DataOkGrade = DataGammaStatus = DataViewingStatus = DataContactStatus = DataVisionStatus = DataCIEStatus = DataGammaTuningStatus = _T("BYPASS");
		DataOutStatus = DataInStatus = DataDefect = DataGammaTuningTime = DataInTrayNum = _T("");
	}
};

struct PLCSendDefect
{
	int m_iCount;
	CString m_strCode;
	CString m_strGrade;

	void Reset()
	{
		m_iCount = 0;
		m_strCode = m_strGrade = _T("");
	}
};

struct ResultCodeRank
{
	int m_iCh;
	CString m_strZone;
	CString m_strResultCode;
	int m_iResultCodeCount;

	void Reset()
	{
		m_iCh = m_iResultCodeCount = 0;
		m_strZone = m_strResultCode = _T("");
	}
};

struct TimeInspect
{
	int m_iTimeNum;
	int m_iShiftTimeStart;
	int m_iShiftTimeEnd;
};

struct TactTimeName
{
	int m_iTactTimeNum;
	CString m_strTactTimeName;
};

struct SDataPanelInfo{
	CString strTime;
	CString strPanel_ID;
	CString strFpc_ID;
	CString strDefect_Result;
	CString strPanel_Grade;
	CString strPanel_Width;
	CString strPanel_Hegiht;
	CString strPreGammaContactStatus;
	CString strModel_ID;
	CString strIndexNum;
	CString strChNum;
	CString strVisionResult;
	CString strViewingResult;
	CString strTpResult;
	CString strLumitopResult;
};


struct SDataDefectInfo{
	CString strNo;
	CString strInspName;
	CString strDefect_Code;
	CString strDefect_Name;
	CString strDefect_StartX;
	CString strDefect_StartY;
	CString strDefect_EndX;
	CString strDefect_EndY;
	CString strDefect_Pattern;
	CString strDefect_Grade;
};

struct SDataModuleShop
{
	CString Panel_ID;
	CString FPC_ID;
	CString Glass_Type;
	CString Product_ID;
	CString Owner_ID;
	CString Owner_Code;
	CString Owner_Type;
	CString Lot_ID;
	CString Process_ID;
	CString Recipe_ID;
	CString SaleOrder;
	CString PreProcess_ID_1;
	CString Group_ID;
	CString Product_Info;
	CString LOT_Info;
	CString Product_Group;
	CString From_Site;
	CString Current_Site;
	CString From_Shop;
	CString Current_Shop;
	CString Thickness;
	CString MMGFlag;
	CString Panel_Size;
};

struct SJobDataShop
{
	BOOL BCDataFileExist;
	int TypeNum;
	CString Cassette_Sequence_No;
	CString Job_Sequence_No;
	CString Group_Index;
	CString Product_Type;
	CString CST_Operation_Mode;
	CString SubStrate_Type;
	CString CIM_Mode;
	CString Job_Type;
	CString Job_Judge;
	CString Sampling_Slot_Flag;
	CString First_Run;
	CString Job_Grade;
	CString Job_ID;
	CString INSP_Reservation;
	CString EQP_Reservation;
	CString LastGlass_Flag;
	CString InspJudge_Data;
	CString Tracking_Data;
	CString EQP_Flag;
	CString Chip_Count;
	CString PP_ID;
	CString FPC_ID;
	CString Cassette_Setting_Code;

	void Reset()
	{
		BCDataFileExist = FALSE;
		TypeNum = 0;
		CIM_Mode = SubStrate_Type = CST_Operation_Mode = Product_Type = Group_Index = Job_Sequence_No = Cassette_Sequence_No = _T("");
		EQP_Reservation = INSP_Reservation = Job_ID = Job_Grade = First_Run = Sampling_Slot_Flag = Job_Judge = Job_Type = _T("");
		Cassette_Setting_Code= FPC_ID = PP_ID = Chip_Count = EQP_Flag = Tracking_Data = InspJudge_Data = LastGlass_Flag = _T("");
	}
};

struct SBITJobData
{
	CString m_strBitJodata[2];
	char strCST_Operation_Mode[100];
	char strINSP_Reservation[100];
	int iCST_Operation_Mode;
	int iSubStrate_Type;
	int iCIM_Mode;
	int iJob_Type;
	int iJob_Judge;
	int iSampling_Slot_Flag;
	int iFirst_Run;
	int iINSP_Reservation;
	int iEQP_Reservation;
	int iLastGlass_Flag;
};

struct IDCardReader
{
	int m_iNum;
	CString m_strLogintTime;
	CString m_strLevel;
	CString m_strUserID;
	CString m_strUserPassWord;
	CString m_strIDCardNo;
	CString m_strDivision;
	CString m_strUserName;
	CString m_strLoginOut;
	BOOL m_bIdSerarchFlag;
	BOOL m_bLoginFlag;

	void Reset()
	{
		m_strLogintTime = m_strLoginOut = m_strLevel = m_strUserID = m_strUserPassWord = m_strIDCardNo = m_strDivision = m_strUserName = _T("");
		m_iNum = 0;
		m_bIdSerarchFlag = m_bLoginFlag = FALSE;
	}
};

struct DfsDataValue
{
	int m_TypeNum;
	int m_StageNum;
	CString m_FpcID;
	CString m_PanelID;
	CString m_StartTime;
	CString m_EndTime;
	CString m_LoadHandlerTime;
	CString m_UnloadHandlerTime;
	CString m_TpTime;
	CString m_PreGammaTime;
	CString m_TactTime;
	CString m_PreGammaContactStatus;
	CString m_ModelID;
	CString m_IndexNum;
	CString m_ChNum;
	CString m_TpResult;
	CString m_Contact;
	CString m_PreGamma;
	CString m_AOIInpsect;
	CString m_TpResult2;
	CString m_Lumitop;
	CString m_mura;
	CString m_opViewResult;
	CString m_ContactCount;
	CString m_LoadeHandlerNUM;
	CString m_UnLoadeHandlerNUM;

	void Reset()
	{
		m_TypeNum = m_StageNum = 0;
		m_FpcID = m_PreGammaContactStatus = m_TactTime = m_EndTime = m_PreGammaTime = m_TpTime = m_UnloadHandlerTime = m_LoadHandlerTime = m_StartTime =
			m_PanelID = m_ModelID = m_IndexNum = m_ChNum = m_TpResult = _T("");
	}
};

struct OpvPanelCountData
{
	CString strTime;
	CString strPanelID;
	CString strDefectTitle[DefectTitleMaxCount];
	CString strOverKillCount;
	CString strUnderKillCount;

	void Reset()
	{
		strTime = strPanelID = strOverKillCount = strUnderKillCount = _T("");
		for (int ii = 0; ii < DefectTitleMaxCount; ii++)
		{
			strDefectTitle[ii] = _T("");
		}
	}
};

struct DefectList
{
	CString strTime;
	CString strPanelID;
	int strChNum;
	CString strDefectCode;
	CString strDefectGrade;
	CString strDefectDesctiption;
	CString strDefectOpvResult;
	CString strUserID;
};

struct PGDfsList
{
	CString strPanelID;
	CString strFpcID;
	CString m_strVBIT;
	CString m_strVDDI;
	CString m_strVCI;
	CString m_strProgramVersion;
};

struct VisionSameDefect
{
	CString m_strDefectCode;
	CString m_strPanelID;
	CString m_strFpcID;
	CString m_strChNum;
	int m_iSameDefectCount;
};
#endif