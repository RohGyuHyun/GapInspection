#pragma once
#define NEW_ALG
#define NEW_ALG2
#define TWO_CHANNEL_MODE
#define DRY_RUN
//#define MODEL_WINNER
#define TABLET
//#define CAMERA
#ifndef DRY_RUN
#define PLC_USE
#define CAMERA
#else
#define TEST_RUN
#endif


//#define LIGHT_TEST


#ifndef TWO_CHANNEL_MODE
#define MOTION 
#define LIGHT
#endif


#define NEXT_BARCODE_READ

//#define SERIAL_BARCODE
//#define KEY_BARCODE

#define ASCII_READER

#define BARCODE_COM_PORT										3
#define LIGHT_COM_PORT											5

#define METAL_RIV_INSP				0
#define METAL_HOUSING_INSP		1
#define METAL_AM_INSP				2

#define AM_SIZE_X_Y					360
#define AM_OFFSET_X					80
#define AM_OFFSET_Y					80

#define CI_PI_OFFSET					50
#define CI_PI_X_Y						(AM_SIZE_X_Y / 2) - CI_PI_OFFSET
#define CI_PI_SPACE					100


//#define TEST_CAMERA

#define CYLINDER_NOT_USE
#define NEW_HOUSING

#define OPEN_CV_LIB

#ifndef MODEL_WINNER
#define MODEL_BLOOM
#endif

#ifndef CAMERA
#define NO_BARCODE
#endif

#ifdef OPEN_CV_LIB
///////////////////////////////
//	OpenCV
#include "opencv\cv.h"
#include "opencv\cvaux.h"
#include "opencv\highgui.h"
#include "opencv2\opencv.hpp"
///////////////////////////////
#endif

#include "ProfileDataIF.h"
#include "LogFile.h"
#include "TimeCheck.h"
#include "resource.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <GdiPlus.h>

#include "define.h"
#include "GetNumDlg.h"
#include "GetTextDlg.h"
#include "ListBoxST.h"
#include "MyListCtrl.h"
#include "afxcmn.h"
#include "btnenhctrl.h"
#include "StaticMatDisplay.h"
#include "AutoDelete.h"
#include "NghUtil.h"
using namespace std;

#define FREE_SIZE_AUTO_DEL								1
#define AUTO_DEL_DRIVE_SIZE_PER						90.

const CString SYSTEM_VER								    = _T("Ver-21.06.10.01(R)");

const CString MAIN_PATH										= _T("D:\\GapInspData\\");
const CString MODEL_FILE_PATH								= MAIN_PATH + _T("MODEL\\");
const CString MOTION_FILE_PATH							= MAIN_PATH + _T("MOTION\\");
const CString SYSTEM_FILE_PATH							= MAIN_PATH + _T("SYSTEM\\");
const CString LOG_FILE_PATH									= MAIN_PATH + _T("LOG\\");
const CString BACKUP_FILE_PATH							= MAIN_PATH + _T("BACKUP\\");
const CString LOG_RSLT_IMAGE_SAVE_PATH				= LOG_FILE_PATH + _T("InspectingImage\\");
const CString DATA_CAMERA_INFO_FILE					= SYSTEM_FILE_PATH + _T("CameraInfo.txt");
const CString SYSTEM_INFO_FILE							= SYSTEM_FILE_PATH + _T("System.ini");
const CString BARCODE_INFO_FILE							= SYSTEM_FILE_PATH + _T("Barcode.ini");
const CString LOG_COUNT_PATH								= LOG_FILE_PATH + _T("Count_Log\\");
const CString LOG_BARCODE_PATH							= LOG_FILE_PATH + _T("Barcode_Log\\");

const CString RSLT_FILE_PATH								= _T(":\\GapInspRslt\\");
const CString RSLT_REPORT_FILE_PATH					= RSLT_FILE_PATH + _T("Report\\");
const CString RSLT_IMAGE_FILE_PATH						= RSLT_FILE_PATH + _T("Image\\");

const CString DGS_FILE_PATH								= _T("C:\\DGS\\LOGS\\");

//20190125 ngh
/////////////////////////////////////////////////
#ifdef MODEL_BLOOM
const CString RSLT_FILE_LABEL								= _T("Barcode,Port,Model,StartTime,EndTime,TackTime(sec),Result,Y(Y Right - Y Left),Right X(CAM R - USB R),Left X(CAM L - USB L),Spec Min(Y),Spec Max(Y),Spec Min(Right X),Spec Max(Right X),Spec Min(Left X),Spec Max(Left X),1(Y Right),2(X CAM R),3(X USB R),4(Y Left),5(X CAM L),6(X USB L),Spec Min(1:Y Right),Spec Max(1:Y Right),Spec Min(2:X CAM R),Spec Max(2:X CAM R),Spec Min(3:X USB R),Spec Max(3:X USB R),Spec Min(4:Y Left),Spec Max(4:Y Left),Spec Min(5:X CAM L),Spec Max(5:X CAM L),Spec Min(6:X USB L),Spec Max(6:X USB L)\n");
#else
const CString RSLT_FILE_LABEL								= _T("Barcode,Port,Model,StartTime,EndTime,TackTime(sec),Result,Y(CAM C - USB C),CAM X(CAM L - CAM R),USB X(USB L - USB R),Spec Min(Y),Spec Max(Y),Spec Min(CAM X),Spec Max(CAM X),Spec Min(USB X),Spec Max(USB X),1(Y CAM),2(X CAM R),3(X CAM L),4(Y USB),5(X USB R),6(X USB L),Spec Min(1:Y CAM),Spec Max(1:Y CAM),Spec Min(2:X CAM R),Spec Max(2:X CAM R),Spec Min(3:X CAM L),Spec Max(3:X CAM L),Spec Min(4:Y USB),Spec Max(4:Y USB),Spec Min(5:X USB R),Spec Max(5:X USB R),Spec Min(6:X USB L),Spec Max(6:X USB L)\n");
#endif

const CString STR_PASS_WORD									= _T("SAMSUNG");
////////////////////////////////////////////////

const CString IO_STR_PARA_FILENAME							= _T("DIOLabel.ini");
const CString MOT_STR_PARA_FILENAME							= _T("MotPara.ini");
				

const CString DGS_BLOOM_MODEL_NAME							= _T("F-700");
const CString DGS_TOP_CODE									= _T("TOPK45");

#define BARCODE_READ_TIME									50

#define PLC_AUTO_DEFAULT_STATE_TIME				500
#define PLC_AUTO_STATE_TIME								150
#define PLC_RECIVE_WAIT_TIME								30

#define MAX_CHAR											512

#define MAX_VAL												255
#define M_PI													3.14159265

#define CAM_WIDTH											3840
#define CAM_HEIGHT										2748

#define TOP_CAMERA											0
#define BOTTOM_CAMERA										1

#ifdef TWO_CHANNEL_MODE
#define MAX_CAM_CNT											4
#else
#define MAX_CAM_CNT											2
#endif
#define MAX_GRAB_CNT										2
#define MAX_LIGHT_CNT										3

#define MAX_MOTION_CNT										3
#define MAX_INSP_CNT										2
#define MAX_INSP_POINT_CNT									6
#define MAX_RSLT_POINT_CNT									3
#define MAX_INSP_SEQ_CNT									6
#define MAX_RSLT_CNT										5
#define MAX_CENTER_INSP_RECT_CNT							4
#define MAX_LEFT_RIGHT_INSP_RECT_CNT						2
#define MAX_INSP_RECT										MAX_CENTER_INSP_RECT_CNT

#define INSP_SPEC_TYPE_0									0
#define INSP_SPEC_TYPE_1									1


//20190131 by X <-> Y 바꿈
#define Y_MOTION											0
#define X_MOTION											1
#define Z_MOTION											2
//20190207 ngh
#define DIRECTION_UP										0
#define DIRECTION_DW										1

#define X_MOTION_INSP										0
#define Y_MOTION_INSP										1
#define Z_MOTION_INSP_0										2
#define Z_MOTION_INSP_1										3

#define TOP_CAMERA_IDX										0
#define BOTTOM_CAMERA_IDX									1

#define MAX_IMAGE_SAVE_DATE_TIME_LEN						13

#define MAX_CAM_NUM											MAX_CAM_CNT
#define MAX_BUF_NUM											MAX_CAM_NUM

//20190128 ngh
#define INSP_END_TIMECHECK									20000 //ms


//20190207 ngh
#define TOP_CAMERA_LIGHT_START_IDX							3
#define BOT_CAMERA_LIGHT_START_IDX							0

#define RSLT_LIST_SIDE_MAX_COL_COUNT						7

//#define SEQ_LIST_SIDE_MAX_COL_COUNT							7
#define SEQ_LIST_SIDE_MAX_COL_COUNT							3
#define INSP_LIST_SIDE_MAX_COL_COUNT						3
#define INSP_LIST_SIDE_MAX_ROW_COUNT						20

#define VER_HOR_RSLT_SIDE_MAX_COL_COUNT						4
#define MAX_VER_HOR_INSP_CNT								3

#define INSP_DIRECTION_VERTICAL								0//세로
#define INSP_DIRECTION_HORIZONTAL							1//가로

#define	DEFAULT_TIMEOUT										1000
#define GRAB_TIMEOUT										10000

#define MAX_GRAB_RECONNECT_CNT								5

#define VERTICALITY_INSP_ANGLE_SPEC							180.5
#define HORIZONTALITY_INSP_ANGLE_SPEC						180.5

#define INSP_CENTER_TRACKER_IDX_UB_SEARCH					0
#define INSP_CENTER_TRACKER_IDX_BRACKET_VERTICALITY			1
#define INSP_CENTER_TRACKER_IDX_BRACKET_HORIZONTALITY_0		2
#define INSP_CENTER_TRACKER_IDX_BRACKET_HORIZONTALITY_1		3

#define INSP_LEFT_RIGHT_TRACKER_IDX_BRACKET_SEARCH			0
#define INSP_LEFT_RIGHT_TRACKER_IDX_PROJECTION				1


//20190125 ngh
/////////////////////////////////////////////////
#define MAX_VERTICALITY_INSP_CNT							1
#define MAX_HORIZONTALITY_INSP_CNT							2
//////////////////////////////////////////////////

#ifdef MODEL_BLOOM
const CString INSP_CENTER_TRACKER_IDX_LABEL_0					= _T("UB_HOR_SEARCH_RECT");
const CString INSP_CENTER_TRACKER_IDX_LABEL_1					= _T("METAL_HOR_SEARCH_RECT");
const CString INSP_CENTER_TRACKER_IDX_LABEL_2					= _T("METAL_VER_SEARCH_RECT_0");
const CString INSP_CENTER_TRACKER_IDX_LABEL_3					= _T("METAL_VER_SEARCH_RECT_1");

const CString INSP_LEFT_TRACKER_IDX_LABEL_0						= _T("METAL_VER_SEARCH_RECT");
const CString INSP_LEFT_TRACKER_IDX_LABEL_1						= _T("PROJECTION_VER_SEARCH_RECT");
#else
const CString INSP_CENTER_TRACKER_IDX_LABEL_0					= _T("UB_HOR_SEARCH_RECT");
const CString INSP_CENTER_TRACKER_IDX_LABEL_1					= _T("METAL_HOR_SEARCH_RECT");
const CString INSP_CENTER_TRACKER_IDX_LABEL_2					= _T("BRACKET_VER_SEARCH_RECT_0");
const CString INSP_CENTER_TRACKER_IDX_LABEL_3					= _T("BRACKET_VER_SEARCH_RECT_1");

const CString INSP_LEFT_TRACKER_IDX_LABEL_0						= _T("BRACKET_VER_SEARCH_RECT");
const CString INSP_LEFT_TRACKER_IDX_LABEL_1						= _T("PROJECTION_VER_SEARCH_RECT");
#endif

#ifdef  _UNICODE
#define	_tsprintf	wsprintf
#else
#define	_tsprintf	sprintf
#endif

//20190125 ngh
/////////////////////////////////////////////////
#define MAIN_TIMER_WINDOWS_VISIBLE								10
#define MAIN_TIMER_EVENT_MANUAL_INSPECTION_END_CHECK			20
#define MAIN_TIMER_CAMERA_CONNECT_CHECK							30
#define MAIN_TIMER_CURRENT_TIME_SHOW							40
#define MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_0		50
#define MAIN_TIMER_AUTO_SEQ_INSP_END_RSLT_DISPLAY_CHANNEL_1		55
#define MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_0					60
#define MAIN_TIMER_AUTO_SEQ_END_CHECK_CHANNEL_1					65
#define MAIN_TIMER_JOG_CUR_POS									70
#define MAIN_TIMER_BARCODE_INPUT								80
#define MAIN_TIMER_MENU_SELECT									90
#define MAIN_TIMER_MENU_AUTO									100
#define MAIN_TIMER_MAIN_ALL_ORIGIN_CHECK						110
#define MAIN_TIMER_BARCODE_WAIT_INPUT							120
#define MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_0		130
#define MAIN_TIMER_SAUMSUNG_LOG_COPY_WRITE_CHECK_CHANNEL_1		135
#define MAIN_TIMER_SPREAD_ALARM_CHECK							140
#define MAIN_TIMER_SERVER_AUTO_CONNECT							150
#define MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_0			160
#define MAIN_TIMER_INSP_LOG_COPY_WRITE_CHECK_CHANNEL_1			165
#define MAIN_TIMER_AUTO_CONNECT									170
#define MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_0					180
#define MAIN_TIMER_ALARM_MESSAGE_BOX_CHANNEL_1					190
#define MAIN_TIMER_PLC_READ_CHECK_CHANNEL_0						200
#define MAIN_TIMEER_PLC_READ_CHANNEL_0							210
#define MAIN_TIMEER_PLC_READ_CHANNEL_1							220
#define MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_0				230
#define MAIN_TIMER_AUTO_STATE_PLC_SEND_CHANNEL_1				240
#define MAIN_TIMER_DISPLAY_DIALOG								250
#define MAIN_TIMER_BARCODE_READER_CANNEL_0									260
#define MAIN_TIMER_BARCODE_READER_CANNEL_1									270

#define TEACH_TIMER_WINDOWS_VISIBLE								10
#define TEACH_TIMER_SPEC_SHOW_WINDOW							20
#define TEACH_TIMER_SPEC_HIDE_WINDOW							25
#define TEACH_TIMER_SPEC_POINT_SHOW_WINDOW						30
#define TEACH_TIMER_SPEC_POINT_HIDE_WINDOW						35
#define TEACH_TIMER_SYSTEM_HIDE_WINDOW							40
#define TEACH_TIMER_INPUT_SEQ_LIST_VAL							50
#define TEACH_TIMER_INPUT_PARA_LIST_VAL							60

#define ZOOM_VIEW_TIMER_DIALOG_INIT								10
#define ZOOM_VIEW_TIMER_WINDOWS_VISIBLE							20

#define SYSTEM_TIMER_WINDOWS_VISIBLE							10
#define SYSTEM_TIMER_DRIVE_SELECT								20
#define SYSTEM_TIMER_ALGORITHM_SELECT							30


#define BARCODE_TIMER_BARCODE_READER							10

#define USE_MSG_ZOOM_VIEW_TO_MAIN_DIALOG						WM_USER + 15
#define ZOOM_VIEW_TO_MAIN_GRAB_MSG								0
#define ZOOM_VIEW_TO_MAIN_LIVE_START_MSG						1
#define ZOOM_VIEW_TO_MAIN_LIVE_STOP_MSG							2
#define ZOOM_VIEW_TO_MAIN_INSPECTION_MSG						3
#define ZOOM_VIEW_TO_MAIN_ZOOM_VIEW_CAM_CHANGE					4
#define ZOOM_VIEW_TO_MAIN_ZOOM_VIEW_IMAGE_CHANGE				5
#define ZOOM_VIEW_TO_MAIN_IMAGE_SAVE_MSG						6
#define ZOOM_VIEW_TO_MAIN_IMAGE_LOAD_MSG						7
#define ZOOM_VIEW_TO_MAIN_SET_TRACKER_RECT						8
#define ZOOM_VIEW_TO_MAIN_SET_TRACKER_IDX						9

#define USE_MSG_JOG_TO_MAIN_DIALOG								WM_USER + 20
#define JOG_TO_MAIN_SET_CUR_POS_SEQ								0
#define JOG_TO_MAIN_AXIS_X_JOG_MOVE								1
#define JOG_TO_MAIN_AXIS_Y_JOG_MOVE								2
#define JOG_TO_MAIN_AXIS_Z_JOG_MOVE								3

#define USE_MSG_TEACH_TO_MAIN_DIALOG							WM_USER + 25
#define TEACH_TO_MAIN_SELECT_SEQ_POS_MOVE_0						0
#define TEACH_TO_MAIN_SELECT_SEQ_POS_MOVE_1						1
#define TEACH_TO_MAIN_SELECT_SEQ_CHANGE							2
#define TEACH_TO_MAIN_MODEL_CHANGE								3
#define TEACH_TO_MAIN_APPLY_SEQ_PARA							4
#define TEACH_TO_MAIN_LOG_WRITE									5
#define TEACH_TO_MAIN_CYL_ALL_FW_BK								6
#define TEACH_TO_MAIN_DIALOG_CANCEL								7

#define USE_MSG_SYSTEM_TO_MAIN_DIALOG							WM_USER + 35
#define SYSTEM_TO_MAIN_APPLY_SYSTEM_PARA						0
#define SYSTEM_TO_MAIN_MODEL_ADD								1
#define SYSTEM_TO_MAIN_MODEL_DELETE								2
#define SYSTEM_TO_MAIN_MODEL_CHANGE								3

#define USE_MSG_ISNP_RSLT_TO_MAIN_DIALOG						WM_USER + 40
#define INSP_RSLT_TO_MAIN_DB_CLICK								0
//////////////////////////////////////////////////

#define USE_MSG_MOTION_TO_MAIN_DIALOG							WM_USER + 30

#define USE_MSG_MAIN_GRAB_DISPLAY_DIALOG_CHANNEL_0				WM_USER + 45

#define USE_MSG_MAIN_GRAB_DISPLAY_DIALOG_CHANNEL_1				WM_USER + 50

#define USE_MSG_MAIN_INSP_END_DISPLAY_DIALOG					WM_USER + 55


#define WM_USER_MEDIACHANGED									WM_USER + 200


#ifdef MODEL_BLOOM
const CString INSP_STRING_POINT_1								 = _T("Y_RIGHT");
const CString INSP_STRING_POINT_2								 = _T("X_CAM_RIGHT");
const CString INSP_STRING_POINT_3								 = _T("X_USB_RIGHT");
const CString INSP_STRING_POINT_4								 = _T("Y_LEFT");
const CString INSP_STRING_POINT_5								 = _T("X_CAM_LEFT");
const CString INSP_STRING_POINT_6								 = _T("X_USB_LEFT");
#else
const CString INSP_STRING_POINT_1								 = _T("Y_CAM");
const CString INSP_STRING_POINT_2								 = _T("X_CAM_RIGHT");
const CString INSP_STRING_POINT_3								 = _T("X_CAM_LEFT");
const CString INSP_STRING_POINT_4								 = _T("Y_USB");
const CString INSP_STRING_POINT_5								 = _T("X_USB_RIGHT");
const CString INSP_STRING_POINT_6								 = _T("X_USB_LEFT");
#endif

//20190410 ngh
#define PORT_BARCODE_EMPTY				-1
#define PORT_BARCODE_LEFT				1
#define PORT_BARCODE_RIGHT				0

//20190416 ngh
#define MAX_AVG_RSLT_LIST_CNT			9


//20191024 ngh
#define MAX_CHANNEL_CNT					2

#define NO_1_CHANNEL		0
#define NO_2_CHANNEL		1

#define AUTO_CHANNEL_1		0
#define AUTO_CHANNEL_2		1
#define TEACH_CHANNEL_1		2
#define TEACH_CHANNEL_2		3


//20190219 ngh
extern "C" __declspec(dllexport) CString InspPointToString(int nInspPoint);

enum RSLT_DIRECTION
{
	NON_DIRECTION = 0,
	RIGHT_OR_TOP_DIRECTION = 1,
	LEFT_OR_BOT_DIRECTION = 2,
}RSLT_DIRECTION;

enum INSP_POINT
{
	INSP_SKIP = 0,
	INSP_POINT1 = 1,
	INSP_POINT2 = 2,
	INSP_POINT3 = 3,
	INSP_POINT4 = 4,
	INSP_POINT5 = 5,
	INSP_POINT6 = 6
}INSP_POINT;

enum INSP_POINT_NAME
{
	INSP_Y_CAM = 0,
	INSP_X_CAM_R = 1,
	INSP_X_CAM_L = 2,
	INSP_Y_USB = 3,
	INSP_X_USB_R = 4,
	INSP_X_USB_L = 5,
}INSP_POINT_NAME;

enum RSLT_POINT
{
	RSLT_POINT1 = 0,
	RSLT_POINT2 = 1,
	RSLT_POINT3 = 2
}RSLT_POINT;

enum LIGHT_IDX
{
	LIGHT_SIDE_LEFT = 0,
	LIGHT_SIDE_RIGHT = 1,
	LIGHT_SIDE_COAXIAL = 2,
	LIGHT_BACK_LIGHT = 3
}LIGHT_IDX;

enum PORT_IDX
{
	LEFT_PORT = 0,
	RIGHT_PORT = 1,
	EMPTY_PORT = 2
}PORT_IDX;

//20191014
enum INSP_ALGORITHM_MODEL
{
	WINNER_ALGO = 0,
	BLOOM_ALGO = 1
}INSP_ALGORITHM_MODEL;

enum SEQ_STEP
{
	INIT_SEQ = 0,
	GRAB_SEQ = 1,
	END_SEQ = 4,
	READY = 5,
	SEQ_ERROR = 6,
	RE_INIT_SEQ = 7
}SEQ_STEP_INDEX;

enum COL_SEARCH_DIRECTION
{
	TOP_TO_BOTTOM = 0,
	BOTTOM_TO_TOP = 1,
}COL_SEARCH_DIRECTION_INDEX;

enum ROW_SEARCH_DIRECTION
{
	RIGHT_TO_LEFT = 0,
	LEFT_TO_RIGHT = 1,
}ROW_SEARCH_DIRECTION_INDEX;

const CString INSP_ALGORITHM_0								 = _T("WINNER");
const CString INSP_ALGORITHM_1								 = _T("BLOOM");

//20190125 ngh
/////////////////////////////////////////////////
typedef struct typeSpec
{
	double dMinSpec;
	double dMaxSpec;
	double dOffset;
}TypeSpecPara;

typedef struct typeFilter
{
	int nDirection;
	int nGaousKenel;
	double dGaousSigma;
	int nThreshHold;
	CvPoint cMorpSize;
	int nSizeThresh;
	int nSearchDirection;
}TypeFilterPara;

typedef struct typeInsp
{
	BOOL bOneGrabInspection;
	CRect InspRect[MAX_CENTER_INSP_RECT_CNT];
	TypeFilterPara FilterPara[MAX_CENTER_INSP_RECT_CNT];
}TypeInspPara;

typedef struct typeInspection
{
	int nSeqIdx;
	int nInspPoint[MAX_CAM_CNT / 2];
	int nLightVal[MAX_CAM_CNT / 2][MAX_GRAB_CNT][MAX_LIGHT_CNT];
	double dMotionVal[MAX_MOTION_CNT + 1];
	TypeInspPara InspPara[MAX_CAM_CNT / 2];
}TypeInspectionPara;

typedef struct TCP_ConnectPara
{
	CString strIP;
	int nPortNum;
}typeConnectPara;

typedef struct typeSystem
{
	CString strUseModelName;
	BOOL bRsltImageSave;
	BOOL bInspImageSave;
	BOOL bRsltNGImageSave;
	double dPixelSizeWidth;
	double dPixelSizeHeight;
	BOOL isUsePW;
	BOOL bTrace;
	CString strDrive;
	typeDirAutoDelPara DirAutoDelPara;
	int nAvgRsltCnt;
	int nSpreadAlarmCheckCnt;
	typeConnectPara ServerPara;
	int nAlgorithmIdx;
	int nMetalAlgorithmIdx;
	BOOL bVIPMode;
	BOOL bBarcodeUse;
	int nLightOnOffWaitTime;
}TypeSystemPara;

typedef struct typeSeq
{
	char cDGSModelName[128];
	int nMaxSeqIdx;
	TypeSpecPara Spec[MAX_RSLT_POINT_CNT];
	TypeSpecPara SpecPoint[MAX_INSP_POINT_CNT];
	TypeSpecPara Spread[MAX_RSLT_POINT_CNT];
	TypeSpecPara SpreadPoint[MAX_INSP_POINT_CNT];
	vector<TypeInspectionPara> InspectionPara;
	int nSelectAlgorithmIdx;
}TypeSeqPara;

typedef struct typeSeqMotion
{
	double dMotion[MAX_MOTION_CNT + 1];
}TypeSeqMotionPara;

typedef struct typeSeqLight
{
	int nLightVal[MAX_CAM_CNT / 2][MAX_GRAB_CNT][MAX_LIGHT_CNT];
}TypeSeqLightPara;

typedef struct typeInspEachRslt
{
	int nInspPoint;
	double dGapRslt;
	BOOL bGapRslt;
	BOOL bPointRslt;
	IplImage *RsltImage;
}TypeInspEachRslt;

typedef struct typeInspEach
{
	int nInspPoint;
	double dGapRslt;
	BOOL bGapRslt;
	BOOL bPointRslt;
}TypeInspEach;

typedef struct typeInspRslt
{
	BOOL bRslt;
	double dGapRslt[MAX_RSLT_POINT_CNT];
	BOOL bGapRslt[MAX_RSLT_POINT_CNT];
	int nRsltDirection[MAX_RSLT_POINT_CNT];
}TypeInspRslt;

typedef struct typeAvgRslt
{
	double dGapRslt[MAX_RSLT_POINT_CNT];
	double dGapPointRslt[MAX_INSP_POINT_CNT];
}TypeAvgRslt;

typedef struct typeChartData
{
	char strLabel[5];
	double dData;
}TypeChartData;

typedef struct typeSpreadAlarmData
{
	int nDirection[MAX_AVG_RSLT_LIST_CNT];
	BOOL bSpreadAlarm[MAX_AVG_RSLT_LIST_CNT];
}TypeSpreadAlarmData;

typedef struct typeSpreadAlarm
{
	BOOL bAlarm;
	char cAlarmString[256];
}TypeSpreadAlarm;

typedef struct typeSeqStepPara
{
	BOOL bStepStart;
	BOOL bStepEnd;
}TypeSeqStepPara;

#define MAX_DGS_CHAR_LEN				50

typedef struct DGSFile
{
	char cDGSFileName[MAX_DGS_CHAR_LEN];
	int nChannel;
	char cModel[MAX_DGS_CHAR_LEN];
	char cBarcode[MAX_DGS_CHAR_LEN];
	char cDate[MAX_DGS_CHAR_LEN];
	char cStartTime[MAX_DGS_CHAR_LEN];
	char cExeFileName[MAX_DGS_CHAR_LEN];
	char cModelFileName[MAX_DGS_CHAR_LEN];
	char cRsltData[MAX_RSLT_POINT_CNT][MAX_DGS_CHAR_LEN];
	char cPointRsltData[MAX_INSP_POINT_CNT][MAX_DGS_CHAR_LEN];
	double dTackTime;
	char cFailItem[MAX_DGS_CHAR_LEN];
	BOOL isRslt;
}typeDGSFilePara;

typedef struct typeBarcodeString
{
	BYTE byBarcode[MAX_BYTE_CNT];
	int nBarcodeLength;
}TypeStringBarcode;

////////////////////////////////////////////