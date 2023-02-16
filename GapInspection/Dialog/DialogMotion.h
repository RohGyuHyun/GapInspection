#pragma once
#include "resource.h"
#include "afxcmn.h"

#ifndef TWO_CHANNEL_MODE
#define MAX_AXIS_NUM		3
#define MAX_INPUT_NUM		12
#define MAX_OUTPUT_NUM		12

#define MAX_CHAR_LENG		512

#define ORIGIN_OK			0
#define ORIGIN_SEARCHING	1

#define TIMER_ORIGIN_DLG	100
#define TIMER_IO_READ		200

#define JOG_MODE			0
#define STEP_MODE			1

// CDialogMotion 대화 상자입니다.
class CDialogMotion : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogMotion)

public:
	CDialogMotion(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogMotion();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MANUAL };
protected:
	wchar_t m_sDILabel[MAX_INPUT_NUM][MAX_CHAR_LENG];
	wchar_t m_sDOLabel[MAX_OUTPUT_NUM][MAX_CHAR_LENG];
	
	BOOL m_bInputState[MAX_INPUT_NUM];
	BOOL m_bOutputState[MAX_OUTPUT_NUM];
	BOOL m_bDebugBypass;

	DWORD m_dwIndexInput[MAX_INPUT_NUM];
	DWORD m_dwIndexOutput[MAX_OUTPUT_NUM];
	
	CBtnenhctrl m_CBtnAxisName[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisPulse[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisSensorP[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisSensorN[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxisServo[MAX_AXIS_NUM];
	CBtnenhctrl m_CBtnAxis_Velocity[MAX_AXIS_NUM];
	
	CBtnenhctrl m_CBtnJog;
	CBtnenhctrl m_ctrl_progress;
	
	int m_nRadioAxisName;
	int m_nRadioMoveType;			// 0 : Jog 1: Step
	int m_nProcessMoveIdx;
	double m_edit_dStepVal;
	long m_lAxisVelocity[MAX_AXIS_NUM];

	//20190730 ngh
	BOOL m_bHomming;
public:
	CListCtrl m_listctrl_IO_Input;
	CListCtrl m_listctrl_IO_Output;

	BOOL LoadIOLabelPara();
	void DisplayReadIO();
	void DispCurPos();
	void DispAxisState();

	void SetListIO();
	void AxisMove();

	void LoadPara();
	void SavePara();

	long GetAxisVelocity(BYTE nSlaveNum)	{ return m_lAxisVelocity[nSlaveNum]; }
	BOOL GetSeqByPass();

	//20190215 ngh
	void SetAllHome();

	//20190730 ngh
	BOOL GetHomming(){return m_bHomming;};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMClickListIoOutput(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlButtonHome();
	void ClickBtnenhctrlLabelAxisNameX();
	void ClickBtnenhctrlLabelAxisNameY();
	void ClickBtnenhctrlLabelAxisNameZ();
	void ClickBtnenhctrlButtonServoOn();
	void ClickBtnenhctrlButtonServoOff();
	void ClickBtnenhctrlButtonEmoStop();
	void ClickBtnenhctrlButtonMoveXMinus();
	void ClickBtnenhctrlButtonMoveXPlus();
	void ClickBtnenhctrlButtonMoveYMinus();
	void ClickBtnenhctrlButtonMoveYPlus();
	void ClickBtnenhctrlButtonMoveZMinus();
	void ClickBtnenhctrlButtonMoveZPlus();
	void ClickBtnenhctrlRadioJog();
	void ClickBtnenhctrlRadioStep();
	void ClickBtnenhctrlButtonMotionParameter();
	void DblClickBtnenhctrlLabelVelocityX();
	void DblClickBtnenhctrlLabelVelocityY();
	void DblClickBtnenhctrlLabelVelocityZ();
	afx_msg void OnBnClickedButtonIoTest();
	afx_msg void OnBnClickedButtonIoTest2();
	afx_msg void OnBnClickedButtonIoTest3();
	afx_msg void OnBnClickedButtonIoTest4();
	void ClickBtnenhctrlLabelVelocityX();
	void ClickBtnenhctrlLabelVelocityY();
	void ClickBtnenhctrlLabelVelocityZ();
	BOOL m_chk_AllAxisHome;
	void ClickBtnenhctrlButtonReConnect();
	void ClickBtnenhctrlButtonAlarmReset();
};
#endif